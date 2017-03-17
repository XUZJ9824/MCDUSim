/*****************************************************************
 *
 * Name:        mcdusim.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: mcdusim.cpp,v 1.34 2012/05/31 19:02:59 macrae.i Exp $
 */
#define WANT_DEBUG_ONMOUSE          0   // 0=normal, 1=log all calls to OnMouseXXX()
#define WANT_LOG_SCREEN             1   // 0=normal, 1=use log (terminal window) for debugging
#define MCDU_IN_STATUSBAR           1   // 0=normal, 1=show "#n" so we know which version we're running
#define WANT_FAKE_SAL               0   // 0=normal, 1=initialize the CProt to have a valid SAL at startup
#define WANT_TRACE_KEYBOARD         0   // 0=normal, 1=debugging
#define WANT_CLOSE_CONFIRM_ON_EXIT  0   // 0=normal, 1=must click close/Alt-F4/etc twice to exit
#define WANT_DEBUG_STARTUP          1   // 0=normal, 1=debug startup/shutdown protocol (heartbeat and SAL/MAL exchange)
#define WANT_REFRESH_COUNTER        0   // 0=normal, 1=show refesh counter above screen

#define APP_VERSION                 "2.1.02-ADL_4B"
#define APP_PART_NUMBER             "1252-SW-3801"
#define APP_RELEASE_DATE            "31-May-2012"

// show source and compiler version in status bar when MCDU Sim starts up, and in About box
#if _MSC_VER >= 1600
#define APP_AND_COMPILER_VERSION    APP_RELEASE_DATE ## " (VS2010)"
#elif _MSC_VER >= 1400
#define APP_AND_COMPILER_VERSION    APP_RELEASE_DATE ## " (VS2005)"
#else
#define APP_AND_COMPILER_VERSION    APP_RELEASE_DATE
#endif

#define _X86_
#include <windef.h>
//#include <winbase.h>
#include <Tlhelp32.h>
//#include <Psapi.h>  //for CreateToolhelp32Snapshot,Process32First,Process32Next
//#pragma comment(lib,"Psapi.lib")

#include "winsock2_wrapper.h" // do this before wx/wx.h so <winsock2.h> and <windows.h> get included in the right order

#define QUOTEME_(x) #x
#define QUOTEME_VALUE_(x) QUOTEME_(x)
#define SHOWVALUEOF_(x) "Note: " #x " = " QUOTEME_VALUE_(x) ". "
//#define SHOWEMPTYOF_(x) "Note: " #x " is empty. "
//#pragma message(SHOWVALUEOF_(__WIN32__))
#pragma message(SHOWVALUEOF_(_MSC_VER))
#pragma message(SHOWVALUEOF_(_MSC_FULL_VER))
//#pragma message(SHOWVALUEOF_(_MSC_BUILD))
//#pragma message(SHOWVALUEOF_(WX_MSC_FULL_VER))
//#pragma message(SHOWVALUEOF_(wxUSE_NO_MANIFEST))
//#pragma message(SHOWVALUEOF_(wxMANIFEST_ID))

#define DELETE_AND_NULL(x) \
    if (x) { \
        delete x; \
        x = NULL; \
    }

#include "wx/setup.h"
#define _WX_SETUP_H_
#include "msvc/wx/setup.h"      // see if we can slurp in the libraries mentioned in progma comment
// Note: this includes the "real" setup.h in ...\mcduSim\lib\vc_lib\mswud\wx\setup.h
//                                 copied from C:\wxWidgets-2.9.1\lib\vc_lib\mswud\wx

/////////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".

#include "wx/wxprec.h"
#include <wx/cmdline.h>
#include <wx/clipbrd.h>

#include "wx/xrc/xmlres.h"
#include "wx/sound.h"
#include "wx/filedlg.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/image.h"
#include "wx/msgout.h"      // to redirect logs to con
#include "wx/log.h"      // to redirect logs to con
#include "log.h"

//#include "winsock2_wrapper.h" // do this before wx/wx.h so <winsock2.h> and <windows.h> get included in the right order
#include "wx/wx.h"

#include "mcdusim.h"
#include "config.h"

#include <wx/config.h>
//#include <wx/confbase.h> (base config class)
//#include <wx/fileconf.h> (wxFileConfig class)
//#include <wx/msw/regconf.h> (wxRegConfig class)

#include "CConfDlg.h"

#include "WhatDebuggerIsAttached.h" // check whether running under DevStudio or standalone
#include "resource.h"

#define NUM_MAXSOUNDS 6


//#define Y_OFFSET_FOR_NEW_SKIN 20

#define  DEFAULT_EXPORTED_FILE_NAME "exportedMcduScreen.jpg"
#define TXT_FILE_NAME   wxT("C:\\mcdu_txt_")

#define MAX_SCREEN_N_COLS   50  /* for static buffers, must be wide enough for any screen */

#define POST_EVENT(p,a) do { event_postEvent(p,a); printSimLogs(p,a); processRemoteApp(); processChangesFromApp(); } while(event_getNumEvent () )

#define TIMER_100ms_ID              1
#define TIMER_250ms_ID              2
#define REPEAT_KEY_TIME             4 /*(im) key repeats once per second when held down = 4 x 250ms */

#define wxID_MEDIACTRL 1
#define wxEVT_MEDIA_LOADED 2

#define     MENU_TXT__OUT_CRADLE "&Take Remote away from cradle"
#define     MENU_TXT__IN_CRADLE "&Put Remote in the cradle"

#define EXPORT_XMLBASE_FNAME          "Skins/export.dat"
#define EXPORT_CELL_TXT "    <Cell ss:StyleID=\"%s\"><Data ss:Type=\"String\">%c</Data></Cell>\n"
#define EXPORT_CELL_NUM "    <Cell ss:StyleID=\"%s\"><Data ss:Type=\"Number\">%c</Data></Cell>\n"
const char * charStyle[2][8] = { \
{ "s_black", "s_cyan", "s_red", "s_yellow", "s_green", "s_magenta", "s_amber", "s_white" },
{ "b_black", "b_cyan", "b_red", "b_yellow", "b_green", "b_magenta", "b_amber", "b_white" } };

bool g_LED_sending  = false;
bool g_LED_pressOK = false;
bool g_LED_backlight = false;

#define HELP_TXT "Command line options:\n\n\
 -h or  --help         : displays this message box\n\
 -l or --showlog       : show log screen (option not saved)\n\
 -m or --mcdu          : set mcdu number 1~4\n\
 -x or --connection    : connection (0-UDP, 1-DDC, 2-TechSat)\n\
 -a or --arinccard     : ARINC Card number 1~9\n\
 -s or --arincspeed    : ARINC speed s0->low, s1->high\n\
 -g or --arincgroup    : ARINC group 1~4\n\
 -p or --arincparity   : ARINC parity p0->NON, p1->ODD, p2->EVEN\n\
 -t or --arinctxchannel: ARINC tx channel 1~9\n\
 -r or --arincrxchannel: ARINC rx channel 1~9\n\
 -d or --default       : set default parameters (DDC MCDU=1)\n\
 -c or --configdialog  : show config dialog (option not saved)\n\
 -e or --exportDir          : directory consisting exported screenshot files\n\
 -u or --udppeerport        : udp port for communication with peer (teraterm)\n\
 -i or --ignoresocketerrors : when set then mcdu does not fail if peer does not exist\n\
 -f or --nofilters     : accept all incoming arinc data (option not saved)\n\
 --nopython            : Disable Python server (option not saved)\n"

//#ifdef UNICODE
//#pragma message("UNICODE is defined")
//#endif
//#ifdef _UNICODE
//#pragma message("_UNICODE is defined")
//#endif
//#ifdef wxUSE_UNICODE
//#pragma message("wxUSE_UNICODE is defined")
//#endif
//#ifdef wxUSE_UNICODE_UTF8
//#pragma message("wxUSE_UNICODE_UTF8 is defined")
//#endif
//#ifdef wxUSE_UNICODE_WCHAR
//#pragma message("wxUSE_UNICODE_WCHAR is defined")
//#endif

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
     { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT(": displays this message box"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },

     { wxCMD_LINE_SWITCH, wxT("l"), wxT("showlog"), wxT(": show log screen (option not saved)"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_OPTION, wxT("m"), wxT("mcdu"), wxT(": set mcdu number 1~4"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("x"), wxT("connection"), wxT(": connection (0-UDP, 1-DDC, 2-TechSat)"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("a"), wxT("arinccard"), wxT(": ARINC Card number 1~9"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("s"), wxT("arincspeed"), wxT(": ARINC speed s0->low, s1->high"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("g"), wxT("arincgroup"), wxT(": ARINC group 1~4"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("p"), wxT("arincparity"), wxT(": ARINC parity p0->NON, p1->ODD, p2->EVEN"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("t"), wxT("arinctxchannel"), wxT(": ARINC tx channel 1~9"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_OPTION, wxT("r"), wxT("arincrxchannel"), wxT(": ARINC rx channel 1~9"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_SWITCH, wxT("d"), wxT("default"), wxT(": set default parameters (UDP MCDU=1)"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_SWITCH, wxT("c"), wxT("configdialog"), wxT(": show config dialog (option not saved)"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_SWITCH, 0, wxT("nopython"), wxT(": Disable Python server (option not saved)"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_OPTION, wxT("e"), wxT("exportDir"), wxT(": directory consisting exported screenshot files (not saved)"),
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_OPTION, wxT("u"), wxT("udppeerport"), wxT(": set udp port for communication with peer (teraterm)"),
          wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL  },

     { wxCMD_LINE_SWITCH, wxT("i"), wxT("ignoresocketerrors"), wxT(": ignore receive error with communication to peer"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_SWITCH, wxT("f"), wxT("nofilters"), wxT(": accept all incoming arinc data"),
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },

     { wxCMD_LINE_NONE }
};

//(im) Release build got a warning
//      warning C4742: 'struct config_t gConfig' has different alignment in
//          '...\mcduSim\transpOldDDC.cpp'
//          and
//          '...\mcduSim\mcdusim.cpp': 2 and 4
// align it here and where is it declared extern ...
__declspec(align(32)) config_t gConfig;

long gdisplayLogScreen = 1;
long gdisplayConfigDialog = 0;
long gDisablePython = 0;
wxString gExportDir = "";
long gudpPeerPort = 6000;
long gignoreSocketErrors = 0;
long gNoFilters = 0;

extern void log_putstring(char *buffer);

/* helper */

static HANDLE h_MCDU_Mutex = (HANDLE)NULL;

bool AnotherMCDU_Running(int which_mcdu/*1,2,3,4*/)
{
    wxString msg;
    msg.Printf( wxT("MCDU#%d for EMS SATCOM"), which_mcdu);

    // get rid of previous mutex if it exists
    if (h_MCDU_Mutex) {
        CloseHandle(h_MCDU_Mutex);
    }

    SetLastError(0L);
    h_MCDU_Mutex = CreateMutex(NULL, TRUE, msg.wc_str());
    // keep this mutex active to prevent other MCDUs from using the same number

    if (GetLastError()==ERROR_ALREADY_EXISTS) {
        return true;
    }
    return false;
}

/* helper to convert gExportDir from wide characters to C string */
char * ExportDir_Helper()
{
    static char export_dir[500];
    //GetCurrentDirectoryA(sizeof(export_dir), export_dir);
    const wchar_t * wide = gExportDir.wc_str();
    char *          exp = export_dir;
    for (;;) {
        wchar_t ch = *wide++;
        if (!ch)
            break;
        *exp++ = (char)ch;
    }
    *exp++ = '\0';
    return export_dir;
}

/////////////////////////////////////////////////////////////////////////////

/* helper */
void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
    struct {
        DWORD dwType; // must be 0x1000
        LPCSTR szName; // pointer to name (in user addr space)
        DWORD dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags; // reserved for future use, must be zero
    } /*THREADNAME_INFO*/ info;

    info.dwType = 0x1000;
    info.szName = szThreadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    log_printf("New Thread %4u %s\n", dwThreadID, szThreadName);

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}

/* helper */
unsigned int EnumerateMyThreads(char * caller_name)
{
    unsigned int nThreads = 0;
    char msg[100];
    msg[0] = '\0';

    OSVERSIONINFO   osver;
    HINSTANCE       hInstLib;
    // ToolHelp Function Pointers.
    HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD);
    BOOL (WINAPI *lpfThread32First)(HANDLE,LPTHREADENTRY32);
    BOOL (WINAPI *lpfThread32Next)(HANDLE,LPTHREADENTRY32);

    // Check to see if were running under Windows 95 or NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if (!GetVersionEx(&osver))
        goto bail;      // Sorry, that's way too ancient
    if (osver.dwPlatformId != VER_PLATFORM_WIN32_NT)
        goto bail;      // Win32S or Windows 98!

    hInstLib = LoadLibraryA("Kernel32.DLL");
    if (!hInstLib)
        goto bail;      // no Kernel32 !

    // Get procedure addresses. Do this explicitly so this code
    // will still run under an ancient version of Windows which
    // does not have the Toolhelp32 functions the Kernel32.
    lpfCreateToolhelp32Snapshot = (HANDLE(WINAPI *)(DWORD,DWORD))
                        GetProcAddress(hInstLib, "CreateToolhelp32Snapshot");
    lpfThread32First = (BOOL(WINAPI *)(HANDLE,LPTHREADENTRY32))
                        GetProcAddress(hInstLib, "Thread32First");
    lpfThread32Next = (BOOL(WINAPI *)(HANDLE,LPTHREADENTRY32))
                        GetProcAddress(hInstLib, "Thread32Next");
    if (!lpfThread32Next || !lpfThread32First || !lpfCreateToolhelp32Snapshot)
        goto bail;      // couldn't find functions in Kernel32

    // only interested in threads that belong to this process
    DWORD my_pid = GetCurrentProcessId();

    // create a snapshot of all running threads
    HANDLE h = lpfCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    // scan for all the threads belonging to this process
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (lpfThread32First(h, &te)) {
            do {
                if (te.th32OwnerProcessID == my_pid) {
                    // make sure thread is really there
                    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, /*bInheritHandle*/FALSE, te.th32ThreadID);
                    if (hThread != INVALID_HANDLE_VALUE) {
                        DWORD wait_failed = WAIT_FAILED;
                        DWORD still_active = STILL_ACTIVE;
                        DWORD status_timeout = STATUS_TIMEOUT;
                        
                        DWORD dwExitStatus = 0;
                        DWORD dwStatus = ::WaitForSingleObject(hThread, 0/*don't wait*/);
                        if (dwStatus == STATUS_TIMEOUT) { /* OK, thread is still running */
                            ++ nThreads;
                            sprintf_s(msg+strlen(msg), sizeof(msg)-strlen(msg), " %lu", te.th32ThreadID);
                        }
                        CloseHandle(hThread);
                    }
                }
            } while (lpfThread32Next(h, &te));
        }
        CloseHandle(h);
    }

bail:
    if (hInstLib)
        FreeLibrary(hInstLib);

    if (nThreads) {
        log_printf("EnumerateMyThreads found %u:%s in %s\n", nThreads, msg, caller_name);
    }
    return nThreads;
}

/////////////////////////////////////////////////////////////////////////////

mcduGeometry_st mcdu_sim_geom = {
    /*bmp_width*/           372,
    /*bmp_height*/          590,
    /*res_normal*/          IDB_STD_IDLE,
    /*bmp_normal*/          wxT("Skins/MCDU.bmp"),
    /*res_backlit*/         IDB_STD_IDLE,
    /*bmp_backlit*/         wxT("Skins/MCDU.bmp"),
    /*res_mouseOver*/       IDB_STD_MOUSE_OVER,
    /*bmp_mouseOver*/       wxT("Skins/MCDU_cursorOver.bmp"),
    /*res_pressed*/         IDB_STD_PRESSED,
    /*bmp_pressed*/         wxT("Skins/MCDU_pressed.bmp"),
    // (im) must call wxImage::AddHandler(new wx...Handler) in CMcduSimApp::OnInit() for specialized formats
    /*bmp_type*/            wxBITMAP_TYPE_BMP, //wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG

    /*screen_x*/            44,  /* left side of grid within bitmap */
    /*screen_y*/            62,  /*       top of grid within bitmap */
    /*screen_n_cols*/       24,  /* # of columns in display */
    /*screen_char_area_w*/  12,  /* # of horizontal pixels in each grid cell */
    /*screen_n_rows*/       14,  /* # of rows in display */
    /*screen_char_area_h*/  14,  /* # of vertical pixels in each grid cell */

    /*screen_family*/       wxFONTFAMILY_TELETYPE,
    /*screen_normal_size*/  11,
    /*screen_small_size*/    8,
    /*screen_small_offset*/  0, /* # of pixels lower, to account for smaller font */
    /*screen_style*/        wxFONTSTYLE_NORMAL,
    /*screen_weight*/       wxFONTWEIGHT_NORMAL,

    /*equipment_id*/        0x39, /* standard MCDU Equipment ID */
};

mcduGeometry_st tcp_sim_geom = {
    /*bmp_width*/           462,
    /*bmp_height*/          510,
    /*res_normal*/          IDB_TCP_IDLE,
    /*bmp_normal*/          wxT("Skins/TCP-normal.bmp"),
    /*res_backlit*/         IDB_TCP_IDLE,
    /*bmp_backlit*/         wxT("Skins/TCP-normal.bmp"),
    /*res_mouseOver*/       IDB_TCP_MOUSE_OVER,
    /*bmp_mouseOver*/       wxT("Skins/TCP-mouse.bmp"),
    /*res_pressed*/         IDB_TCP_PRESSED,
    /*bmp_pressed*/         wxT("Skins/TCP-pressed.bmp"),
    // (im) must call wxImage::AddHandler(new wx...Handler) in CMcduSimApp::OnInit() for specialized formats
    /*bmp_type*/            wxBITMAP_TYPE_BMP, //wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG

    /*screen_x*/            75,  /* left side of grid within bitmap */
    /*screen_y*/            77,  /*       top of grid within bitmap */
    /*screen_n_cols*/       24,  /* # of columns in display */
    /*screen_char_area_w*/  13,  /* # of horizontal pixels in each grid cell */
    /*screen_n_rows*/       10,  /* # of rows in display */
    /*screen_char_area_h*/  24,  /* # of vertical pixels in each grid cell */
                            
    /*screen_family*/       wxFONTFAMILY_TELETYPE,
    /*screen_normal_size*/  14,
    /*screen_small_size*/   10,
    /*screen_small_offset*/  6, /* # of pixels lower, to account for smaller font */
    /*screen_style*/        wxFONTSTYLE_NORMAL,
    /*screen_weight*/       wxFONTWEIGHT_BOLD, //wxFONTWEIGHT_NORMAL

    /*equipment_id*/        0x170, /* non-standard MCDU Equipment ID */
};

#define     CONTROL_TYPE__BUTTON            0
#define     CONTROL_TYPE__LED               1

//#define     SKIN_CNTRL__SENDING             0
//#define     SKIN_CNTRL__PRESS_OK            1
//#define     SKIN_CNTRL__BUTTON_OK           2
//#define     SKIN_CNTRL__BUTTON_RANGE_TEST   3
//#define     SKIN_CNTRL__BUTTON_PANIC        4
//#define     SKIN_CNTRL__BUTTON_CANCEL       5
//#define     NUM_SKIN_CNTRLS                 6
//skinControls_st skinControls[ NUM_SKIN_CNTRLS ] = {
//    {   0, CONTROL_TYPE__BUTTON,  16,  40, 46,  60 }, /* Sending  LED     */
//    {   0, CONTROL_TYPE__BUTTON,  16,  68, 46,  87 }, /* Press OK LED     */
//    {   0, CONTROL_TYPE__BUTTON,  22,  94, 122, 198 }, /* OK button        */
//    {   0, CONTROL_TYPE__BUTTON, 136,  94, 236, 198 }, /* Range test button*/
//    {   0, CONTROL_TYPE__BUTTON,  16, 218, 130, 332 }, /* Panic button     */
//    {   0, CONTROL_TYPE__BUTTON, 136, 218, 236, 332 }  /* Cancel buttom    */
//};

#include "MCDU_key.h"

skinControls_st skinControls[ /*NUM_SKIN_CNTRLS*/ ] = {
    {   KEY__L1, CONTROL_TYPE__BUTTON, 13,89,34,106     },
    {   KEY__L2, CONTROL_TYPE__BUTTON, 13,117,34,134     },
    {   KEY__L3, CONTROL_TYPE__BUTTON, 13,145,34,162     },
    {   KEY__L4, CONTROL_TYPE__BUTTON, 13,173,34,190     },
    {   KEY__L5, CONTROL_TYPE__BUTTON, 13,201,34,218     },
    {   KEY__L6, CONTROL_TYPE__BUTTON, 13,229,34,246     },

    {   KEY__R1, CONTROL_TYPE__BUTTON,    338,89,361,106       },
    {   KEY__R2, CONTROL_TYPE__BUTTON,    338,117,361,134      },
    {   KEY__R3, CONTROL_TYPE__BUTTON,    338,145,361,162     },
    {   KEY__R4, CONTROL_TYPE__BUTTON,    338,173,361,190     },
    {   KEY__R5, CONTROL_TYPE__BUTTON,    338,201,361,218     },
    {   KEY__R6, CONTROL_TYPE__BUTTON,    338,229,361,246     },

    {   LED__BRT, CONTROL_TYPE__LED,    335,255,364,269     },

    {   KEY__INIT_REF, CONTROL_TYPE__BUTTON,     50,288,87,316 },
    {   KEY__RTE, CONTROL_TYPE__BUTTON,    95,288,133,316      },
    {   KEY__CLB, CONTROL_TYPE__BUTTON,    140,288,179,316     },
    {   KEY__CRZ, CONTROL_TYPE__BUTTON,    186,288,224,316     },
    {   KEY__DES, CONTROL_TYPE__BUTTON,    233,288,271,316     },

    {   KEY__MENU, CONTROL_TYPE__BUTTON,       47,322,86,351       },
    {   KEY__LEGS, CONTROL_TYPE__BUTTON,       95,322,133,351      },
    {   KEY__DEP_ARR, CONTROL_TYPE__BUTTON,    140,322,179,351     },
    {   KEY__HOLD, CONTROL_TYPE__BUTTON ,      186,322,224,351     },
    {   KEY__PROG, CONTROL_TYPE__BUTTON,       233,322,271,351     },

    {   KEY__EXEC, CONTROL_TYPE__BUTTON,     287,322,326,351     },
    {   LED__EXEC, CONTROL_TYPE__LED,        295,337,316,346      },

    {   KEY__N1_LIMIT, CONTROL_TYPE__BUTTON,    47,357,86,386     },
    {   KEY__FIX, CONTROL_TYPE__BUTTON,    95,357,133,386         },

    {   KEY__PREV_PAGE, CONTROL_TYPE__BUTTON,    47,392,86,421      },
    {   KEY__NEXT_PAGE, CONTROL_TYPE__BUTTON,    95,392,133,421     },

    {   KEY__1, CONTROL_TYPE__BUTTON,      50,432,79,457     },
    {   KEY__4, CONTROL_TYPE__BUTTON,      50,467,79,492     },
    {   KEY__7, CONTROL_TYPE__BUTTON,      50,502,79,528     },
    {   KEY__DOT, CONTROL_TYPE__BUTTON,    50,538,79,564     },

    {   KEY__2, CONTROL_TYPE__BUTTON,      86,432,114,457     },
    {   KEY__5, CONTROL_TYPE__BUTTON,      86,467,114,492     },
    {   KEY__8, CONTROL_TYPE__BUTTON,      86,502,114,528     },
    {   KEY__0, CONTROL_TYPE__BUTTON,      86,538,114,564     },

    {   KEY__3, CONTROL_TYPE__BUTTON,      120,432,149,457     },
    {   KEY__6, CONTROL_TYPE__BUTTON,      120,467,149,492     },
    {   KEY__9, CONTROL_TYPE__BUTTON,      120,502,149,528     },
    {   KEY__PLUS_MINUS, CONTROL_TYPE__BUTTON,       120,538,149,564     },

    {   KEY__A, CONTROL_TYPE__BUTTON,      159,362,188,389     },
    {   KEY__F, CONTROL_TYPE__BUTTON,      159,397,188,424     },
    {   KEY__K, CONTROL_TYPE__BUTTON,      159,432,188,457     },
    {   KEY__P, CONTROL_TYPE__BUTTON,      159,467,188,492     },
    {   KEY__U, CONTROL_TYPE__BUTTON,      159,502,188,528     },
    {   KEY__Z, CONTROL_TYPE__BUTTON,      159,538,188,564     },

    {   KEY__B, CONTROL_TYPE__BUTTON,      195,362,223,389     },
    {   KEY__G, CONTROL_TYPE__BUTTON,      195,397,223,424     },
    {   KEY__L, CONTROL_TYPE__BUTTON,      195,432,223,457     },
    {   KEY__Q, CONTROL_TYPE__BUTTON,      195,467,223,492     },
    {   KEY__V, CONTROL_TYPE__BUTTON,      195,502,223,528     },
    {   KEY__SPACE, CONTROL_TYPE__BUTTON,  195,538,223,564 },

    {   KEY__C, CONTROL_TYPE__BUTTON,      231,362,258,389     },
    {   KEY__H, CONTROL_TYPE__BUTTON,      231,397,258,424     },
    {   KEY__M, CONTROL_TYPE__BUTTON,      231,432,258,457     },
    {   KEY__R, CONTROL_TYPE__BUTTON,      231,467,258,492     },
    {   KEY__W, CONTROL_TYPE__BUTTON,      231,502,258,528     },
    {   KEY__DEL, CONTROL_TYPE__BUTTON,      231,538,258,564   },

    {   KEY__D, CONTROL_TYPE__BUTTON,      265,362,293,389     },
    {   KEY__I, CONTROL_TYPE__BUTTON,      265,397,293,424     },
    {   KEY__N, CONTROL_TYPE__BUTTON,      265,432,293,457     },
    {   KEY__S, CONTROL_TYPE__BUTTON,      265,467,293,492     },
    {   KEY__X, CONTROL_TYPE__BUTTON,      265,502,293,528     },
    {   KEY__DASH, CONTROL_TYPE__BUTTON,      265,538,293,564  },

    {   KEY__E, CONTROL_TYPE__BUTTON,      300,362,328,389     },
    {   KEY__J, CONTROL_TYPE__BUTTON,      300,397,328,424     },
    {   KEY__O, CONTROL_TYPE__BUTTON,      300,432,328,457     },
    {   KEY__T, CONTROL_TYPE__BUTTON,      300,467,328,492     },
    {   KEY__Y, CONTROL_TYPE__BUTTON,      300,502,328,528     },
    {   KEY__CLR, CONTROL_TYPE__BUTTON,      300,538,328,564   },

    {   LED__DSPY, CONTROL_TYPE__LED,    17,405,33,448     },
    {   LED__FAIL, CONTROL_TYPE__LED,    17,450,33,496     },
    {   LED__MSG, CONTROL_TYPE__LED,     340,405,356,448   },
    {   LED__OFST, CONTROL_TYPE__LED,    340,450,356,496   },
    {   0,0,    0,0,0,0   }
};

skinControls_st skinControls_TCP[ ] = {
    {   KEY__L1, CONTROL_TYPE__BUTTON,    7, 128,   7+28, 128+16 },
    {   KEY__L2, CONTROL_TYPE__BUTTON,    7, 176,   7+28, 176+16 },
    {   KEY__L3, CONTROL_TYPE__BUTTON,    7, 224,   7+28, 224+16 },
    {   KEY__L4, CONTROL_TYPE__BUTTON,    7, 272,   7+28, 272+16 },

    {   KEY__R1, CONTROL_TYPE__BUTTON,  426, 128, 426+28, 128+16 },
    {   KEY__R2, CONTROL_TYPE__BUTTON,  426, 176, 426+28, 176+16 },
    {   KEY__R3, CONTROL_TYPE__BUTTON,  426, 224, 426+28, 224+16 },
    {   KEY__R4, CONTROL_TYPE__BUTTON,  426, 272, 426+28, 272+16 },

    {   KEY__1, CONTROL_TYPE__BUTTON,    47, 350,  47+27, 350+28 },
    {   KEY__2, CONTROL_TYPE__BUTTON,    93, 350,  93+27, 350+28 },
    {   KEY__3, CONTROL_TYPE__BUTTON,   139, 350, 139+27, 350+28 },
                                                        
    {   KEY__4, CONTROL_TYPE__BUTTON,    47, 390,  47+27, 390+28 },
    {   KEY__5, CONTROL_TYPE__BUTTON,    93, 390,  93+27, 390+28 },
    {   KEY__6, CONTROL_TYPE__BUTTON,   139, 390, 139+27, 390+28 },
                                                        
    {   KEY__7, CONTROL_TYPE__BUTTON,    47, 430,  47+27, 430+28 },
    {   KEY__8, CONTROL_TYPE__BUTTON,    93, 430,  93+27, 430+28 },
    {   KEY__9, CONTROL_TYPE__BUTTON,   139, 430, 139+27, 430+28 },
                                                        
    {   KEY__DOT, CONTROL_TYPE__BUTTON,  47, 470,  47+27, 470+28 },
    {   KEY__0, CONTROL_TYPE__BUTTON,    93, 470,  93+27, 470+28 },
    {   KEY__CLR, CONTROL_TYPE__BUTTON, 139, 470, 139+27, 470+28 },
                                                            
    {   KEY__PREV_PAGE, CONTROL_TYPE__BUTTON, 208, 465, 208+48, 465+30 },
    {   KEY__NEXT_PAGE, CONTROL_TYPE__BUTTON, 271, 465, 271+48, 465+30 },

    {   KEY__MENU, CONTROL_TYPE__BUTTON,      397, 397, 397+45, 397+31 },

    {   0,0,    0,0,0,0   }
};

//----------------------------------------------------------------------------

#if DEFAULT_MCDU_APPEARANCE == TCP_MCDU_APPEARANCE_TYPE
mcduGeometry_st * which_mcdu_geom = &tcp_sim_geom;
skinControls_st * which_mcdu_skin = &skinControls_TCP[0];
#else // must be standard 14-line MCDU
mcduGeometry_st * which_mcdu_geom = &mcdu_sim_geom;
skinControls_st * which_mcdu_skin = &skinControls[0];
#endif

/* helper for CMcduSimFrame when created or appearance changed */
wxSize New_CMcduSimFrame_Size()
{
    // //The height(CY) and width(CX) of a window border, in pixels./
    // int yBorder = ::GetSystemMetrics(SM_CYBORDER);
    // int xBorder = ::GetSystemMetrics(SM_CXBORDER);
    // //The height(CY) and width(CX) of a 3-D window border, in pixels.
    // int yEdge   = ::GetSystemMetrics(SM_CYEDGE);
    // int xEdge   = ::GetSystemMetrics(SM_CXEDGE);
    // //The thickness of the sizing border around the perimeter of a window that can be resized, in pixels.
    // int yRFrame = ::GetSystemMetrics(SM_CYFRAME);
    // int xRFrame = ::GetSystemMetrics(SM_CXFRAME);
    // //The thickness of the frame around the perimeter of a window that has a caption but is not sizable, in pixels.
    // int yFFrame = ::GetSystemMetrics(SM_CYFIXEDFRAME);
    // int xFFrame = ::GetSystemMetrics(SM_CXFIXEDFRAME);

    // fetch non-client area parameters so we can size the window around the bitmap
    int yFFrame = ::GetSystemMetrics(SM_CYFIXEDFRAME);  //Fixed frame thickness (e.g. with caption but not sizable)
    int xFFrame = ::GetSystemMetrics(SM_CXFIXEDFRAME);  //Fixed frame thickness (e.g. with caption but not sizable)
    int yMenu   = ::GetSystemMetrics(SM_CYMENU);        //Single-line menu bar height
    int yTitle  = ::GetSystemMetrics(SM_CYCAPTION);     //Title bar height
    int yStatus = 21;                                   //Status-bar height (compromise: Classic has 20, Win7+Aero has 23)
    // Using 25 in Win7 + Aero is OK                        and 2 pixel gap when TCP (create or after switch STD->TCP)
    // Using 25 in Classic(XP) shows a 3 pixel gap when STD and 4 pixel gap when TCP (appearance)
    // Using 23 in Classic(XP) shows a 1 pixel gap when STD and 2 pixel gap when TCP
    // Using 22 in Classic(XP) is OK                        and 1 pixel gap when TCP 
    // Using 21 in Classic(XP) is OK                        and is OK
    // Using 20 in Win7 + Aero chops a row or two of pixels from the bitmap in both STD and TCP (1 extra in STD)
    // Using 20 in Classic(XP) chops a row or two of pixels from the bitmap in both STD and TCP (1 extra in STD)

    //(im) Haven't found a way to calculate this accurately before the window is created.
    //     It is based on font metrics currently selected into status bar DC,
    //     which in turn depends on the font used for tooltips.

#if 0 // (im) Attempt to determine what the Status bar height will be.
    // determine the font being used for the status area
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    // make up a font using the appropriate font info.
    HFONT hFont = CreateFontIndirect(&ncm.lfStatusFont);
    TEXTMETRIC tm;
    // get the font characteristics. for that, we need a DC temporarily.
    HDC hDC = GetDC(NULL);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
    GetTextMetrics(hDC, &tm);
    SelectObject(hDC, hOldFont);
    ReleaseDC(NULL,hDC);
    // done with the font. examine the font info.
    DeleteObject((HGDIOBJ)(HFONT)hFont); // a.k.a. DeleteFont(hFont);

    yStatus = ncm.iCaptionHeight + 2; // -ncm.lfStatusFont.lfHeight * 72 / 1440;
#endif

    return wxSize(which_mcdu_geom->bmp_width  + (2*xFFrame), 
                  which_mcdu_geom->bmp_height + yTitle+yMenu+yStatus+(2*yFFrame) );
}

/* helper */
wxBitmap * Load_Bitmap_Helper(const wxString& bmp_name, wxBitmapType bmp_type, UINT bmp_res_id, char * desc)
{
    wxBitmap * wxBmp = new wxBitmap(bmp_name, bmp_type);
    BITMAP bm;
    if (wxBmp->Ok()) { /* file exists, use it */
        return wxBmp;
    }
    /* file not found, use embedded bitmap resource */

    // (im) Both wxBitmap::wxBitmap and wxBitmap::LoadFile expect a wxString, not
    //      an LPTSTR and unfortunately, both fail to load the resource properly,
    //      they don't convert the wxString back into what ::LoadBitmap needs.

    //      This code copied from wxBMPResourceHandler::LoadFile() in gdiimage.cpp

    wxBmp->SetHBITMAP((WXHBITMAP)::LoadBitmap(wxGetInstance(), MAKEINTRESOURCE(bmp_res_id)));
    if ( !wxBmp->Ok() ) {
        wxLogError(wxT("Can't load \"%s\" bitmap (id %u) from resources, check .rc file!"), desc, bmp_res_id);
        return wxBmp;
    }

    if ( !::GetObject(GetHbitmapOf(*wxBmp), sizeof(BITMAP), (LPSTR) &bm) ) {
        wxLogLastError(wxT("GetObject(HBITMAP)"));
        return wxBmp;
    }

    wxBmp->SetWidth(bm.bmWidth);
    wxBmp->SetHeight(bm.bmHeight);
    wxBmp->SetDepth(bm.bmBitsPixel);

    // use 0xC0C0C0 as transparent colour by default
    wxBmp->SetMask(new wxMask(*wxBmp, *wxLIGHT_GREY));

    return wxBmp;
}

/* helper */
static void DumpLine_Helper(FILE * out, char * buffer)
{
    if (!out) {
        log_putstring(buffer);
    } else {
        fprintf(out, buffer);
    }
}

/* helper */
void DumpScreen_Helper(CTermScreen * screen, FILE * out)
{
    int row = 0;
    int col = 0;
    termChar_st charDesc;

    DumpLine_Helper( out, "  123456789012345678901234 \r\n" );
    DumpLine_Helper( out, " +------------------------+\r\n" );

    for (row=0; row<SCREEN_N_ROWS; row++)
    {
        char lineBuffer[2+MAX_SCREEN_COLS+5];
        sprintf(lineBuffer, "%d|", (row+1)%10);

        for (col=0; col<SCREEN_N_COLS; col++)
        {
            screen->getCharDesc( &charDesc, row, col );
            if (charDesc.charcode == 0) charDesc.charcode = ' ';
            lineBuffer[col+2] = charDesc.charcode;
        }
        strcpy(&lineBuffer[2+SCREEN_N_COLS], "|\r\n");
        DumpLine_Helper( out, lineBuffer );
    }

    DumpLine_Helper( out, " +------------------------+\r\n" );
}

/////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_APP(CMcduSimApp)

CMcduSimApp * theApp = (CMcduSimApp *)NULL;

BEGIN_EVENT_TABLE(CMcduSimApp, wxApp)
    EVT_KEY_DOWN(CMcduSimApp::OnKeyboard)
END_EVENT_TABLE()

CMcduSimApp::CMcduSimApp()
{
    theApp = this;

    m_PcKeyboardMenu = NULL;
    m_SmallFontAsLower = NULL;
    m_bEnablePcKeyboard = true;         // default allows keyboard input
    m_bEnableSmallFontAsLower = false;  // default show as smaller size but all upper case

    m_frame = (CMcduSimFrame *) NULL;
    m_t.Printf( wxT("MCDU Sim #%d"), gConfig.cfgMcdu);
}

CMcduSimApp::~CMcduSimApp()
{
    log_printf("------------ MCDU#%d exit\n", gConfig.cfgMcdu);
    //(im) these get deleted automatically when the CMcduSimFrame is deleted...
    //DELETE_AND_NULL(m_PcKeyboardMenu);
    //DELETE_AND_NULL(m_SmallFontAsLower);
}

void CMcduSimApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxString app = GetAppName();
    wxString vend = GetVendorName();

    SetVendorName(_T("EMS Satcom"));
    SetAppName(_T("MCDU Simulator")); // not needed, it's the default value
    wxConfigBase *pConfig = wxConfigBase::Get();
    pConfig->SetRecordDefaults();

    gConfig.cfgMcdu             = pConfig->Read(_T("mcdu"),           1L);
    gConfig.cfgMcduAppearance   = pConfig->Read(_T("Appearance"),     DEFAULT_MCDU_APPEARANCE);
    gConfig.cfgConnectionType   = pConfig->Read(_T("ConnectionType"), CONNECTION_TYPE_DDC); // default is DDC
    gConfig.cfgArincCard        = pConfig->Read(_T("ArincCard"),      1L);
    gConfig.cfgArincGroup       = pConfig->Read(_T("ArincGroup"),     1L);
    gConfig.cfgArincParity      = pConfig->Read(_T("ArincParity"),    1L);

    gConfig.cfgArincTxChannel   = pConfig->Read(_T("ArincTxChannel"), 1L);
    gConfig.cfgArincTxSpeed     = pConfig->Read(_T("ArincTxSpeed"),   1L);
    gConfig.cfgArincRxChannel   = pConfig->Read(_T("ArincRxChannel"), 1L);
    gConfig.cfgArincRxSpeed     = pConfig->Read(_T("ArincRxSpeed"),   1L);

    wxString s = pConfig->Read(_T("DisplayInterfaceHost"),            "(none)");
    strcpy_s(gConfig.cfgDisplayInterfaceHost, sizeof(gConfig.cfgDisplayInterfaceHost), (const char *)s);

    wxString dimIpAddress = pConfig->Read(_T("DimIpAddress"),            "127.0.0.1");
    strcpy_s(gConfig.cfgDimIpAddress, sizeof(gConfig.cfgDimIpAddress), (const char *)dimIpAddress);

    gConfig.cfgDimBasePort = pConfig->Read(_T("DimBasePort"),            7900L);

    gConfig.cfgLocalBasePort = pConfig->Read(_T("LocalBasePort"),        7800L);

	gConfig.cfgDimInputLabel = pConfig->Read(_T("DimInputLabel"),            227L);
	gConfig.cfgDimOutputLabel = pConfig->Read(_T("DimOutputLabel"),            356L);

    gConfig.cfgAlreadyConfig    = pConfig->Read(_T("alreadyConfig"),  0L);

    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-/"));
    parser.EnableLongOptions(true); // default is true, but it can't hurt
    parser.SetLogo(wxT("MCDU Simulator - EMS Satcom\nCommand line options:\n"));
}

void CMcduSimApp::OnKeyboard(wxKeyEvent& event)
{
    if (m_frame) {
        m_frame->OnKeyboard(event);
    } else {
        // without this, the event will stop here and never reach to the next level.
        event.Skip();
    }
}

bool CMcduSimApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    static int defaultOptioAlreadyProcessedFlag = 0;
    if (parser.Found(wxT("d")) && (defaultOptioAlreadyProcessedFlag == 0) )
    {
        gConfig.cfgMcdu             = 1;
        gConfig.cfgMcduAppearance   = DEFAULT_MCDU_APPEARANCE;
        gConfig.cfgConnectionType   = CONNECTION_TYPE_DDC; // default is DDC
        gConfig.cfgArincCard        = 1;
        gConfig.cfgArincGroup       = 1;
        gConfig.cfgArincParity      = 1;

        gConfig.cfgArincTxChannel   = 1;
        gConfig.cfgArincTxSpeed     = 1;
        gConfig.cfgArincRxChannel   = 1;
        gConfig.cfgArincRxSpeed     = 1;

        defaultOptioAlreadyProcessedFlag = 1;
    }

    // and other command line parameters
    if (parser.Found(wxT("m"), &gConfig.cfgMcdu))
    {
        if ( gConfig.cfgMcdu<1 || gConfig.cfgMcdu>4 ) return false;
        //gConfig.cfgMcdu--;
    }
    // then do what you need with them.

    #if WANT_LOG_SCREEN
    gdisplayLogScreen = 1;
    #else //!WANT_LOG_SCREEN
    gdisplayLogScreen = parser.Found(wxT("l"));
    #endif //WANT_LOG_SCREEN

    //(im) long options are not working properly
    gDisablePython = 0; //(im)//gDisablePython = parser.Found(wxT("nopython"));

    // processing of "-e" command line parameter,
    // as a path value it will always have path separtor
    // symbol at the end - vgorine 07.12.2009
    if (parser.Found(wxT("e"), &gExportDir))
    {
        if ( (gExportDir.Len() > 0) && (!wxIsPathSeparator(gExportDir.Last())))
        {
            gExportDir.append(wxFILE_SEP_PATH);
        }
    }
    // processing of "-u" command line parameter, 29.06.2010
    parser.Found(wxT("u"), &gudpPeerPort);
    // processing of "-i" command line parameter
    gignoreSocketErrors = parser.Found(wxT("i"));

    // processing of "-f" command line parameter
    gNoFilters = parser.Found(wxT("f"));

    gdisplayConfigDialog = parser.Found(wxT("c"));

    // ARINC
    if (parser.Found(wxT("a"), &gConfig.cfgArincCard))
    {
        if ( gConfig.cfgArincCard<1 || gConfig.cfgArincCard>9 ) { return false; }
    }

    if (parser.Found(wxT("x"), &gConfig.cfgConnectionType))
    {
        if ( gConfig.cfgConnectionType<0 || gConfig.cfgConnectionType>LAST_HARDWARE_CONNECTION_TYPE ) { return false; }
    }

    if (parser.Found(wxT("s"), &gConfig.cfgArincTxSpeed))
    {
        if ( gConfig.cfgArincTxSpeed<0 || gConfig.cfgArincTxSpeed>1 ) { parser.Usage(); return false; }
    }

    if (parser.Found(wxT("g"), &gConfig.cfgArincGroup))
    {
        if ( gConfig.cfgArincGroup<1 || gConfig.cfgArincGroup>4 ) { parser.Usage(); return false; }
    }

    if (parser.Found(wxT("p"), &gConfig.cfgArincParity))
    {
        if ( gConfig.cfgArincParity<0 || gConfig.cfgArincParity>2 ) { parser.Usage(); return false; }
    }

    if (parser.Found(wxT("t"), &gConfig.cfgArincTxChannel))
    {
        if ( gConfig.cfgArincTxChannel<1 || gConfig.cfgArincTxChannel>9 ) { parser.Usage(); return false; }
    }

    if (parser.Found(wxT("r"), &gConfig.cfgArincRxChannel))
    {
        if ( gConfig.cfgArincRxChannel<1 || gConfig.cfgArincRxChannel>9 ) { parser.Usage(); return false; }
    }

    return true;
}

bool CMcduSimApp::OnInit(void)
{
    //INIT_WX_LOGS();

    // call default behaviour (mandatory)
    if (!wxApp::OnInit())
        return false;

    // support for additional image formats: Windows only does BMP by default
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);

    //(im) Note: the values at this point are defaults, and
    //     the actual MCDU# is unknown at this point, so
    //     debugging could be very confusing with multiple MCDUs

    //LOGN("mcdu = ",                  gConfig.cfgMcdu);
    //LOGN("Appearance Type = ",       gConfig.cfgMcduAppearance);
    //LOGN("Connection Type = ",       gConfig.cfgConnectionType);
    //LOGN("ArincCard = ",             gConfig.cfgArincCard);
    //LOGN("ArincGroup = ",            gConfig.cfgArincGroup);
    //LOGN("ArincParity = ",           gConfig.cfgArincParity);
    //                           
    //LOGN("ArincTxChannel = ",        gConfig.cfgArincTxChannel);
    //LOGN("ArincTxSpeed = ",          gConfig.cfgArincTxSpeed);
    //LOGN("ArincRxChannel = ",        gConfig.cfgArincRxChannel);
    //LOGN("ArincRxSpeed = ",          gConfig.cfgArincRxSpeed);
    //                           
    //LOGN("gDisablePython = ",        gDisablePython);
    //LOG1S("DisplayInterfaceHost = ", gConfig.cfgDisplayInterfaceHost);

    wxXmlResource::Get()->InitAllHandlers();

    /* (im) Instead of loading the file mcduDlgs.xrc at run time, now
     *      include the compiled version by calling InitXmlResource(),
     *      which is found in mcduDlgRes.cpp; create a new version by
     *      editing the mcduDlgs.xrc file.
     *      When building the project, it should run MakeNewMcduDlgRes.bat
     *      (which runs wxrc) to create a new version of mdcuDlgRes.cpp
     */
    extern void InitXmlResource();  /* in mcduDlgRes.cpp */
    InitXmlResource();

    if (gdisplayConfigDialog || gConfig.cfgAlreadyConfig == 0)
    {
        CConfDlg dlg;
        wxXmlResource::Get()->LoadDialog( (wxDialog *) &dlg, NULL, wxT("ID_CONFDLG"));
        if (!dlg.CreateControls())
            return false;

        dlg.ShowModal();

        //LOG("New values:");
    }

    void Identify_the_MCDU(int mcdu_index); /* in log_printf.cpp */
    Identify_the_MCDU(gConfig.cfgMcdu-1);   // from now on the log file will show the currect MCDU #

    if (gdisplayLogScreen) {
        OpenDebugLogFile(); //INIT_WX_LOGS();
    }

    // Make sure there is only one of us
    if (AnotherMCDU_Running(gConfig.cfgMcdu)) {
        wxString msg;
        msg.Printf( wxT("Sorry, another MCDU #%d Simulator application seems to be running already"), gConfig.cfgMcdu);
        MessageBox(NULL, msg.wc_str(), _T("MCDU Simulator--"), MB_ICONSTOP);
        return FALSE;
    }

    log_printf("------------ MCDU#%d start -- built %s\n", gConfig.cfgMcdu, APP_AND_COMPILER_VERSION);

    LOGN("mcdu = ",                  gConfig.cfgMcdu);
    LOGN("Appearance Type = ",       gConfig.cfgMcduAppearance);
    LOGN("Connection Type = ",       gConfig.cfgConnectionType);
    LOGN("ArincCard = ",             gConfig.cfgArincCard);
    LOGN("ArincGroup = ",            gConfig.cfgArincGroup);
    LOGN("ArincParity = ",           gConfig.cfgArincParity);
                               
    LOGN("ArincTxChannel = ",        gConfig.cfgArincTxChannel);
    LOGN("ArincTxSpeed = ",          gConfig.cfgArincTxSpeed);
    LOGN("ArincRxChannel = ",        gConfig.cfgArincRxChannel);
    LOGN("ArincRxSpeed = ",          gConfig.cfgArincRxSpeed);
                               
    LOGN("gDisablePython = ",        gDisablePython);
    LOG1S("DisplayInterfaceHost = ", gConfig.cfgDisplayInterfaceHost);

    if(gConfig.cfgConnectionType == CONNECTION_TYPE_UDP)
        m_t.Printf( wxT("MCDU%d Sim v%s - EMS SATCOM"), gConfig.cfgMcdu, wxT(APP_VERSION) );
    else
        m_t.Printf( wxT("MCDU%dA Sim v%s - EMS SATCOM"), gConfig.cfgMcdu, wxT(APP_VERSION) );
    LOG( (const char*) m_t.mb_str(wxConvUTF8) );

    // setup MCDU Appearance: either 10-line TCP or Standard 14-line
    if (gConfig.cfgMcduAppearance == TCP_MCDU_APPEARANCE_TYPE) {
        which_mcdu_geom = &tcp_sim_geom;
        which_mcdu_skin = &skinControls_TCP[0];
    } else { // must be standard 14-line MCDU
        which_mcdu_geom = &mcdu_sim_geom;
        which_mcdu_skin = &skinControls[0];
    }

    // Create the main frame window
    m_frame = new CMcduSimFrame(/*parent*/(wxFrame *) NULL,
                                m_t, //_T("MCDU Simulator v0.02 - EMS Satcom"),
                                wxDefaultPosition, /*wxPoint(600, 200),*/
                                New_CMcduSimFrame_Size()
                    );

#if wxUSE_STATUSBAR
    wxStatusBar * wxBar = 
    // Give it a status line with 2 fields
    m_frame->CreateStatusBar(2);

// (im) debug height of status bar after creation
//      See comments about yStatus in New_CMcduSimFrame_Size()
//      Compare value of (sh) and (ptDiff.y) against original (yStatus)
//#if 1
//    {
//    // actual height of status bar after it has been created.
//    int sw, sh;
//    wxBar->GetSize(&sw, &sh);
//    // status bar client size, doesn't include internal borders
//    wxRect wxStatusFieldRect;
//    wxBar->GetFieldRect(1, wxStatusFieldRect);
//
//    int sch = wxBar->GetCharHeight();
//    int sby = wxBar->GetBorderY();
//    // GetHeight() == GetCharHeight() + 2*GetBorderY()
//
//    HWND hWnd = m_frame->GetHWND();//m_hWnd;
//    RECT rcClient, rcWind;
//    POINT ptDiff;
//    GetClientRect(hWnd, &rcClient);
//    GetWindowRect(hWnd, &rcWind);
//    // difference between window size and client size is the non-client area size
//    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;   //        left border +  right border
//    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;  // title + top border + bottom border
//    // since the border sizes are almost always the same, what is left is the title height
//    int yTitle = ptDiff.y - ptDiff.x;
//    // so it looks like the status bar is part of the client area!
//    do {} while (0);
//    }
//#endif
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *control_menu = new wxMenu;
    wxMenu *help_menu = new wxMenu;

    m_frame->Set_App_Control_Menu(control_menu);

    //file_menu->Append(MCDU_SIM___LOAD_FILE, _T("&Load file"),                _T("Load file"));
    //file_menu->Append(MCDU_SIM___SAVE_FILE, _T("&Save file"),                _T("Save file"));
    file_menu->Append(MCDU_SIM___EXPORT, _T("&Export to Excel") _T("\t") _T("Ctrl-E"), _T("Export screen to an XML Excel file."));
    file_menu->Append(MCDU_SIM___EXPORT_BMP, _T("Export to &Bitmap") _T("\t") _T("Ctrl-B"), _T("Export screen to a bitmap file."));
    file_menu->Append(MCDU_SIM___EXPORT_TXT, _T ("Export to &Text") _T("\t") _T("Ctrl-T"), _T("Quick Export screen to a plain text file (goes to C:\\)."));
    file_menu->Append(MCDU_SIM___EXPORT_CLIP, _T("Export to Clipboard") _T("\t") _T("Ctrl-C"), _T("Copy screen to Clipboard."));
    file_menu->AppendSeparator();
    file_menu->Append(MCDU_SIM___QUIT, _T("E&xit"), _T("Quit program"));

    m_PcKeyboardMenu = control_menu->Append(MCDU_SIM___PCKEYBOARD_MCDU_ID, 
                                            _T("&Enable PC Keyboard"), 
                                            _T("Help: Enable/Disable PC Keyboard"),
                                            wxITEM_CHECK);
    EnablePcKeyboard(true);  // default allows keyboard input
    m_SmallFontAsLower = control_menu->Append(MCDU_SIM___SMALL_FONT_AS_LOWER,
                                            _T("&Show Small Font as Lower-case (a..z)") _T("\t") _T("Ctrl-L"),
                                            _T("Help: Show Small Font as Lower Case letters"),
                                            wxITEM_CHECK);
    EnableSmallFontAsLower(false);  // default show small fonts as smaller size but all upper case
    control_menu->Append(MCDU_SIM___CONFIGURE,
                                            _T("&Configure MCDU Settings ..."),
                                            _T("Help: Configure MCDU Settings"));

    help_menu->Append(MCDU_SIM___ABOUT, _T("&About"), _T("About MCDU Simulator"));
    help_menu->Append(MCDU_SIM___COMMAND_LINE_HELP, _T("&Command Line Help"), _T("show all cmd line options"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(control_menu,_T("&Options") );
    menu_bar->Append(help_menu, _T("&Help"));

    // Associate the menu bar with the frame
    m_frame->SetMenuBar(menu_bar);

    m_frame->Show(true);

    #if wxUSE_STATUSBAR
#define READY_APP_AND_COMPILER_VERSION /* "Ready, built " ##*/ APP_AND_COMPILER_VERSION
    m_frame->SetStatusTextSafely(READY_APP_AND_COMPILER_VERSION);
    #if MCDU_IN_STATUSBAR
    {
        int widths[] = {which_mcdu_geom->bmp_width-90, 90};
        m_frame->SetStatusWidths(2, widths);
        wxString im_msg;
        im_msg.Printf(wxT("MCDU#%d"), gConfig.cfgMcdu);
        m_frame->SetStatusTextSafely(im_msg,1); /* show in second cell of status bar */
    }
    #endif //MCDU_IN_STATUSBAR
    #endif // wxUSE_STATUSBAR

    // load bitmaps
    m_frame->Load_Appearance_Bitmaps();

 //   wxXmlResource::Get()->InitAllHandlers();
 //   wxXmlResource::Get()->Load( wxT("Sim.xrc") );

    return true;
}

void CMcduSimApp::EnablePcKeyboard(bool bEnable)
{
    m_bEnablePcKeyboard = bEnable;
    m_PcKeyboardMenu->Check(bEnable);
}

void CMcduSimApp::EnableSmallFontAsLower(bool bEnable)
{
    if (bEnable!=m_bEnableSmallFontAsLower) {
        m_bEnableSmallFontAsLower = bEnable;
        m_SmallFontAsLower->Check(bEnable);
        // make sure the screen gets redrawn with the new font
        m_frame->InvalidateActiveAcreen();
    }
}

/////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CMcduSimFrame, wxFrame)
    EVT_CLOSE(CMcduSimFrame::OnClose)
    EVT_MENU(MCDU_SIM___QUIT,      CMcduSimFrame::OnQuit)
    EVT_MENU(MCDU_SIM___EXPORT,      CMcduSimFrame::OnExport)
    EVT_MENU(MCDU_SIM___EXPORT_BMP,  CMcduSimFrame::OnExportBmp)
    EVT_MENU(MCDU_SIM___DUMP_SCREEN_TO_DEBUG_LOG,  CMcduSimFrame::OnDumpScreenToDebugLog)
    EVT_MENU(MCDU_SIM___ABOUT,     CMcduSimFrame::OnAbout)
    EVT_MENU(MCDU_SIM___COMMAND_LINE_HELP, CMcduSimFrame::OnCmdLineHelp)
    EVT_MENU(MCDU_SIM___PCKEYBOARD_MCDU_ID,     CMcduSimFrame::OnEnablePcKeyboardMcduId)
    EVT_MENU(MCDU_SIM___SMALL_FONT_AS_LOWER,    CMcduSimFrame::OnEnableSmallFontAsLower)
    EVT_MENU(MCDU_SIM___CONFIGURE,              CMcduSimFrame::OnConfigureMCDU)
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_ACK_RX, CMcduSimFrame::OnRxAck)
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_EXPORT_SCREEN, CMcduSimFrame::OnRequestToExportBmp)
    EVT_MENU(MCDU_SIM___SAL_TIMEOUT, CMcduSimFrame::OnSduDisconnection)
    EVT_MENU(MCDU_SIM___KEY_FROM_PYTHON, CMcduSimFrame::OnKeyFromPython)
    EVT_MENU(MCDU_SIM___EXPORT_TXT,  CMcduSimFrame::OnExportTxt)
    EVT_MENU(MCDU_SIM___EXPORT_CLIP,  CMcduSimFrame::OnTxtToClipboard)
    EVT_MENU(MCDU_SIM___GO_BACK_TO_MENU_SCREEN, CMcduSimFrame::OnGoToMenuScreen)
    //EVT_MENU(MCDU_SIM___CUSTOM_EVENT_RX, CMcduSimFrame::OnRx)
    //EVT_MENU(MCDU_SIM___CUSTOM_EVENT_CONNECTION, CMcduSimFrame::OnConnect)
    //EVT_MENU(MCDU_SIM___CUSTOM_EVENT_DISCONNECTION, CMcduSimFrame::OnDisconnect)
    //EVT_MENU(MCDU_SIM___LOAD_FILE, CMcduSimFrame::OnLoadFile)
    //EVT_MENU(MCDU_SIM___SAVE_FILE, CMcduSimFrame::OnSaveFile)
    EVT_PAINT(CMcduSimFrame::OnPaint)
    EVT_ERASE_BACKGROUND(CMcduSimFrame::OnEraseBackGround)
    EVT_MOTION(CMcduSimFrame::OnMouseMoveEvent)
    EVT_LEFT_DOWN(CMcduSimFrame::OnMouseLeftDown)
    EVT_LEFT_UP(CMcduSimFrame::OnMouseLeftUp)
    EVT_TIMER(TIMER_100ms_ID, CMcduSimFrame::OnTimer100ms)
    EVT_TIMER(TIMER_250ms_ID, CMcduSimFrame::OnTimer250ms)
    EVT_LEFT_DCLICK(CMcduSimFrame::OnMouseDoubleClickEvent)
END_EVENT_TABLE()

#define MY_FRAME_STYLE ( \
                        wxSYSTEM_MENU | \
                     /* wxRESIZE_BORDER | */ \
                        wxCAPTION | \
                        wxMINIMIZE_BOX | \
                     /* wxMAXIMIZE_BOX | */ \
                        wxCLOSE_BOX | \
                     /* wxFRAME_FLOAT_ON_PARENT | */ \
                     /* wxFRAME_NO_TASKBAR | */ \
                     /* wxFRAME_TOOL_WINDOW | */ \
                     /* wxSTAY_ON_TOP | */ \
                        wxCLIP_CHILDREN)


typedef struct {
    int value;
    char * desc;
} value_desc_pair;

static value_desc_pair avdpKeyNames[] = {
    {KEY__CLB        , "CLB"},        {KEY__CLR        , "CLR"},
    {KEY__CRZ        , "CRZ"},        {KEY__DASH       , "DASH"/*really "SLASH*/},
    {KEY__DEL        , "DEL"},        {KEY__DEP_ARR    , "DEP_ARR"},
    {KEY__DES        , "DES"},        {KEY__DOT        , "DOT"},
    {KEY__EXEC       , "EXEC"},       {KEY__FIX        , "FIX"},
    {KEY__HOLD       , "HOLD"},       {KEY__INIT_REF   , "INIT_REF"},
    {KEY__LEGS       , "LEGS"},       {KEY__MENU       , "MENU"},
    {KEY__N1_LIMIT   , "N1_LIMIT"},   {KEY__NEXT_PAGE  , "NEXT_PAGE"},
    {KEY__PLUS_MINUS , "PLUS_MINUS"}, {KEY__PREV_PAGE  , "PREV_PAGE"},
    {KEY__PROG       , "PROG"},       {KEY__RTE        , "RTE"},

    {KEY__L1, "L1"},       {KEY__L2, "L2"},       {KEY__L3, "L3"},
    {KEY__L4, "L4"},       {KEY__L5, "L5"},       {KEY__L6, "L6"},
    {KEY__R1, "R1"},       {KEY__R2, "R2"},       {KEY__R3, "R3"},
    {KEY__R4, "R4"},       {KEY__R5, "R5"},       {KEY__R6, "R6"},

    {0x1E, "DOWN_ARROW"},   {0x5E, "UP_ARROW"},     // in ARINC 739 spec

    {0x18, "LOGOFF"}                                // not a button, but it gets sent by MCDU
};

#define ASIZE(x) (sizeof(x)/sizeof(*x))

char * LookupKey(int key)
{
    static char msg[20];
    if (key==' ') {
        strcpy_s(msg, sizeof(msg), "SPACE");
        return msg;
    }
    if (key>='A' && key<='Z') {
        sprintf_s(msg, sizeof(msg), "Letter \"%c\"", key);
        return msg;
    }
    if (key>='0' && key<='9') {
        sprintf_s(msg, sizeof(msg), "Digit \"%c\"", key);
        return msg;
    }
    value_desc_pair * p = avdpKeyNames;
    for (int i=0; i<ASIZE(avdpKeyNames); i++) {
        if (p->value == key) return p->desc;
        ++p;
    }
    return "Unknown!";
}

void CMcduSimFrame::SetStatusTextSafely(const wxString& text, int number)
{
    if (m_frameStatusBar != NULL) {
        SetStatusText(text, number);
        m_status_counter = 0;
    } //otherwise haven't created the status bar yet
}

// Define frame constructor
CMcduSimFrame::CMcduSimFrame(/*parent*/wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
        wxFrame(/*parent*/frame, wxID_ANY, title, pos, size, MY_FRAME_STYLE/*wxDEFAULT_FRAME_STYLE*/ /*, name */),
        m_timer100ms(this, TIMER_100ms_ID), m_timer250ms(this, TIMER_250ms_ID),
        m_activeScreen (NULL), m_python(0), m_prot(0), m_status_counter(0), m_changed_appearance(false),
        m_loop_counter(0L)
{
    //#ifdef __WXMSW__
    //SetIcon(wxIcon(wxT("Skins/mcdusim.ico"), wxBITMAP_TYPE_ICO));
    //#endif
    extern void SetAppIcon(UINT nIconID, HWND hWnd);    // in SetAppIcon.cpp
    SetAppIcon(mcdusim, GetHWND());                     // "Skins/mcdusim.ico"

    m_BaseSkinBitmap = (wxBitmap *)  NULL;
    m_NormalBaseSkinBitmap = (wxBitmap *)  NULL;
    m_BacklightrdBaseSkinBitmap = (wxBitmap *)  NULL;
    m_CursorSkinBitmap = (wxBitmap *)  NULL;
    m_PressedSkinBitmap = (wxBitmap *)  NULL;

    m_OK_keyIsDown = false;
    m_RangeTest_keyIsDown = false;
    m_Panic_keyIsDown = false;
    m_Cancel_keyIsDown = false;

    m_OK_CursorOver = false;
    m_RangeTest_CursorOver = false;
    m_Panic_CursorOver = false;
    m_Cancel_CursorOver = false;
    // no mouse motion detected yet
    m_lastCursorOver.controlID = 0;
    m_currentCursorOver.controlID = 0;
    // no buttons pressed yet
    m_currentButtonDown.controlID = 0;

    m_timer100ms.Start(100);    // 100ms interval
    m_timer250ms.Start(250);    // 1/4 second interval

    m_motionFlag = false;

    wxXmlResource::Get()->InitAllHandlers();
//    wxXmlResource::Get()->Load( wxT("Sim.xrc") );

//    event_init();
//    reset();
//    while( event_getNumEvent () ) processRemoteApp();
//    processChangesFromApp();

    m_sduScreen  = new CTermScreen( (wxFrame *)this, "SDU");
    m_menuScreen = new CTermScreen( (wxFrame *)this, "Menu");
    printMenuTitle();
    setTheActiveScreen(m_menuScreen);

    // lets create a protocol object
    //TODO: provide option for other transport
    m_prot = new CProt(gConfig.cfgMcdu-1, (wxEvtHandler *)this,
                       gudpPeerPort, (gignoreSocketErrors != 0));
    m_prot->setScreen(m_sduScreen, m_menuScreen);
    /* let CProt know where the scratchpad is */
    m_prot->setEquipmentInfo(which_mcdu_geom->equipment_id, SCREEN_N_ROWS);
    #if WANT_FAKE_SAL
    m_prot->setSduSAL(0x90);
    #endif

    //CTransportTcp * temp = new CTransportTcp();  // for now use only TCP. But
    //m_transport = temp;
    //m_transport->init((wxEvtHandler *)this, 5555);
    m_clip = false;

    if (gDisablePython == 0) {
        m_python = new CPythonInt();
        log_printf("Python port = %d\n", PYTHON_BASE_PORT + gConfig.cfgMcdu - 1);
        //log_printf("Python port = %d (m_python 0x%p)\n", PYTHON_BASE_PORT + gConfig.cfgMcdu - 1, m_python);
        m_python->init((wxEvtHandler *)this, m_prot, PYTHON_BASE_PORT + gConfig.cfgMcdu - 1);
        m_python->setScreen(m_activeScreen);
    }
    else m_python = NULL;

    m_keyDown = 0;

    // adjustSkin();
}

// frame destructor
CMcduSimFrame::~CMcduSimFrame()
{
    //Note: don't delete (m_BaseSkinBitmap), it just points to one of the other bitmaps
    DELETE_AND_NULL(m_NormalBaseSkinBitmap);
    DELETE_AND_NULL(m_BacklightrdBaseSkinBitmap);
    DELETE_AND_NULL(m_CursorSkinBitmap);
    DELETE_AND_NULL(m_PressedSkinBitmap);

    DELETE_AND_NULL(m_sduScreen);
    DELETE_AND_NULL(m_menuScreen);

    DELETE_AND_NULL(m_prot);
    //log_printf("~CMcduSimFrame delete m_python 0x%p\n", m_python);
    DELETE_AND_NULL(m_python);
    //log_printf("~CMcduSimFrame delete m_python OK\n");

    //delete m_transport;

    wxConfigBase *pConfig = wxConfigBase::Get();
    if ( pConfig != NULL )
    {
        // save the control's values to the config
        pConfig->Write(_T("mcdu"),                 gConfig.cfgMcdu);
        pConfig->Write(_T("Appearance"),           gConfig.cfgMcduAppearance);
        pConfig->Write(_T("ConnectionType"),       gConfig.cfgConnectionType);
        pConfig->Write(_T("ArincCard"),            gConfig.cfgArincCard);
        pConfig->Write(_T("ArincGroup"),           gConfig.cfgArincGroup);
        pConfig->Write(_T("ArincParity"),          gConfig.cfgArincParity);
                                               
        pConfig->Write(_T("ArincTxChannel"),       gConfig.cfgArincTxChannel);
        pConfig->Write(_T("ArincTxSpeed"),         gConfig.cfgArincTxSpeed);
        pConfig->Write(_T("ArincRxChannel"),       gConfig.cfgArincRxChannel);
        pConfig->Write(_T("ArincRxSpeed"),         gConfig.cfgArincRxSpeed);

        pConfig->Write(_T("DisplayInterfaceHost"), gConfig.cfgDisplayInterfaceHost);

        pConfig->Write(_T("DimIpAddress"),         gConfig.cfgDimIpAddress);
        pConfig->Write(_T("DimBasePort"),          gConfig.cfgDimBasePort);
        pConfig->Write(_T("LocalBasePort"),        gConfig.cfgLocalBasePort);

		pConfig->Write(_T("DimInputLabel"),          gConfig.cfgDimInputLabel);
		pConfig->Write(_T("DimOutputLabel"),          gConfig.cfgDimOutputLabel);

        pConfig->Write(_T("alreadyConfig"),        1L);

        delete wxConfigBase::Set((wxConfigBase *) NULL);
    }
}

void CMcduSimFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void CMcduSimFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString s;
    s.Printf(wxT("MCDU Simulator version %s\nPN: %s\n\n") \
             wxT("NOTICE:  CONFIDENTIAL PROPERTY OF EMS TECHNOLOGIES.\n") \
             wxT("USE AND DISTRIBUTION LIMITED SOLELY TO AUTHORIZED PERSONNEL.\n") \
             wxT("The use, disclosure, reproduction, modification,\n") \
             wxT("transfer or transmittal of this work for any purpose,\n") \
             wxT("in any form, or by any means without the written\n") \
             wxT("permission of EMS Technologies is strictly prohibited.\n\n") \
             wxT("?Copyright 2004-2012 EMS Technologies. All Rights Reserved.\n\nVersion: %s"),
                wxT(APP_VERSION), wxT(APP_PART_NUMBER), APP_AND_COMPILER_VERSION );
    (void)wxMessageBox(s, _T("About MCDU Simulator"), wxOK);
}

void CMcduSimFrame::OnCmdLineHelp(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(_T(HELP_TXT), _T("Command Line Help"), wxOK);
}

void CMcduSimFrame::OnMouseMoveEvent(wxMouseEvent& event) //WXUNUSED(event))
{
    //wxEVT_MOTION
    static skinControls_st oldCursorOverControlID = { 0,0,0,0,0,0};

    wxPoint cursorPos(event.GetPosition() );

    #if WANT_DEBUG_ONMOUSE //wxUSE_STATUSBAR
    wxString s = wxString::Format(_T("Mouse at X=%d, Y=%d"), cursorPos.x, cursorPos.y );
    SetStatusTextSafely(s);
    #endif // wxUSE_STATUSBAR

    skinControls_st controlAreaToUpdate = {0,0};
    controlAreaToUpdate.left = cursorPos.x;
    controlAreaToUpdate.right = cursorPos.x;
    controlAreaToUpdate.top = cursorPos.y;
    controlAreaToUpdate.bottom = cursorPos.y;

    if (findControl(&controlAreaToUpdate)) // if found a match
    {
        if (controlAreaToUpdate.controlID != oldCursorOverControlID.controlID)
        {
            if (oldCursorOverControlID.controlID)
            {
                //LOG("invalidate rectangle to paint old control as background");
                m_lastCursorOver = oldCursorOverControlID;
                RefreshRect (wxRect( oldCursorOverControlID.left,
                             oldCursorOverControlID.top,
                             oldCursorOverControlID.right - oldCursorOverControlID.left,
                             oldCursorOverControlID.bottom - oldCursorOverControlID.top ),
                             false);
            }
            oldCursorOverControlID = controlAreaToUpdate;
        }
        //LOG("invalidate rectangle to paint new keyOver");
        m_currentCursorOver = controlAreaToUpdate;
        RefreshRect (wxRect( controlAreaToUpdate.left,
                             controlAreaToUpdate.top,
                             controlAreaToUpdate.right - controlAreaToUpdate.left,
                             controlAreaToUpdate.bottom - controlAreaToUpdate.top ),
                             false);
    }
    else
    {
        //LOG("not found control match");
        // if there was an old then erase it
        if (m_currentCursorOver.controlID)
        {
            //LOG("invalidate rectangle to paint old control as background");
            m_lastCursorOver = m_currentCursorOver;
            m_currentCursorOver.controlID = 0;
            RefreshRect (wxRect( oldCursorOverControlID.left,
                         oldCursorOverControlID.top,
                         oldCursorOverControlID.right - oldCursorOverControlID.left,
                         oldCursorOverControlID.bottom - oldCursorOverControlID.top ),
                         false);
            oldCursorOverControlID.controlID = 0;
        }
    }
}

void CMcduSimFrame::OnMouseLeftDown(wxMouseEvent& event)
{
    wxPoint cursorPos(event.GetPosition() );

    #if 0 //WANT_DEBUG_ONMOUSE //wxUSE_STATUSBAR
    wxString s = wxString::Format(_T("Mouse Down at X=%d, Y=%d"), cursorPos.x, cursorPos.y );
    SetStatusTextSafely(s);
    #endif // wxUSE_STATUSBAR

    skinControls_st controlAreaToUpdate = {0,0};
    controlAreaToUpdate.left = cursorPos.x;
    controlAreaToUpdate.right = cursorPos.x;
    controlAreaToUpdate.top = cursorPos.y;
    controlAreaToUpdate.bottom = cursorPos.y;

    if (findControl(&controlAreaToUpdate)) // if found a match
    {
        if (controlAreaToUpdate.controlType == CONTROL_TYPE__BUTTON)
        {
            #if WANT_DEBUG_ONMOUSE //wxUSE_STATUSBAR
            wxString s = wxString::Format(_T("Mouse Down in Button"), cursorPos.x, cursorPos.y );
            SetStatusTextSafely(s);
            #endif // wxUSE_STATUSBAR

            m_keyDown = 1;
            m_keyRepeatTimer = REPEAT_KEY_TIME;
            m_currentButtonDown = controlAreaToUpdate;
            //LOG("invalidate rectangle to paint keyDown control");
            RefreshRect (wxRect( m_currentButtonDown.left,
                             m_currentButtonDown.top,
                             m_currentButtonDown.right - m_currentButtonDown.left,
                             m_currentButtonDown.bottom - m_currentButtonDown.top ),
                             false);

            // todo: dispatch key
            onKey(m_currentButtonDown.controlID, 0);

            #if wxUSE_STATUSBAR
            wxString key_name; key_name.Printf(wxT("%s clicked"), LookupKey(m_currentButtonDown.controlID));
            SetStatusTextSafely(key_name);
            #endif // wxUSE_STATUSBAR
        }
    }
}

void CMcduSimFrame::OnMouseLeftUp(wxMouseEvent& event)
{
    // don't care where mouse it released, undo previous control (if any)
    #if WANT_DEBUG_ONMOUSE //wxUSE_STATUSBAR
    wxPoint cursorPos(event.GetPosition() );
    wxString s = wxString::Format(_T("Mouse Up at X=%d, Y=%d"), cursorPos.x, cursorPos.y );
    SetStatusTextSafely(s);
    #endif // wxUSE_STATUSBAR

    if (m_currentButtonDown.controlID)
    {
        //LOG("invalidate rectangle to paint keyDown control");
        m_keyDown = 0;
        m_keyRepeatTimer = 0;
        RefreshRect (wxRect( m_currentButtonDown.left,
                             m_currentButtonDown.top,
                             m_currentButtonDown.right - m_currentButtonDown.left,
                             m_currentButtonDown.bottom - m_currentButtonDown.top ),
                             false);
        // todo: dispatch key
    }
}

void CMcduSimFrame::OnClose(wxCloseEvent& event)
{
    #if WANT_CLOSE_CONFIRM_ON_EXIT
    if (event.CanVeto()) {
        CTermScreen * screenPtr = (CTermScreen *)NULL;
        CTermScreen * menuPtr   = (CTermScreen *)NULL;
        CTermScreen * activePtr = m_activeScreen;

        if (m_prot) {
            screenPtr = m_prot->m_screenPtr;
            menuPtr   = m_prot->m_menuScreenPtr;
        }

        // what screen is showing on the MCDU?
        if (activePtr) {
            //if (activePtr == screenPtr) {
                static bool bAlreadyTriedToExit = false;
                if (!bAlreadyTriedToExit) {
                    bAlreadyTriedToExit = true;
                    activePtr->printText(/*row*/SCREEN_N_ROWS-1,
                                         /*col*/0,
                                         /*text*/"CLICK CLOSE AGAIN TO END",
                                         /*size*/CHAR_SIZE_NORMAL,
                                         /*color*/C_AMBER,
                                         /*attrib*/ATT_NORMAL);
                    activePtr->printText(/*row*/SCREEN_N_ROWS-1,
                                         /*col*/6,
                                         /*text*/"CLOSE",
                                         /*size*/CHAR_SIZE_NORMAL,
                                         /*color*/C_RED,
                                         /*attrib*/ATT_FLASHING);
                    event.Veto();
                    return;
                }
                // else already tried to exit once, let it happen
            //}
            // else on menu screen, OK to exit
        }
        // else we don't know, better bail
    }
    #endif //WANT_CLOSE_CONFIRM_ON_EXIT

    // stop timers before shutting down, or wxWidgets may issue a warning
    m_timer100ms.Stop();    // 100ms interval
    m_timer250ms.Stop();    // 1/4 second interval

    // destroy the main window and exit
    Destroy(); // next stop: CMcduSimFrame::~CMcduSimFrame()
}

void CMcduSimFrame::OnEraseBackGround(wxEraseEvent& event)
{
}

//
int CMcduSimFrame::findControl(skinControls_st * control)
{
    int ret = 0;
    skinControls_st * controlArrayPtr = which_mcdu_skin;

    while (controlArrayPtr->controlID)
    {
 //       wxString s = wxString::Format(_T("input: L=%d, R=%d, T=%d B=%d\ncontrolIn: L=%d, R=%d, T=%d B=%d"),
 //           control->left, control->right, control->top, control->left,
 //           controlArrayPtr->left, controlArrayPtr->right, controlArrayPtr->top, controlArrayPtr-> );

        if ((control->left >= controlArrayPtr->left ) &&
            (control->right <= controlArrayPtr->right) &&
            (control->top >= controlArrayPtr->top) &&
            (control->bottom <= controlArrayPtr->bottom) )
        {
            *control = *controlArrayPtr;
            ret = control->controlID;
            break;
        }
        controlArrayPtr++;
    }

    return ret;
}

void CMcduSimFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if (m_changed_appearance) {
        m_changed_appearance = false;
        saved_termscreen_st sduScreen_contents;
        saved_termscreen_st menuScreen_contents;

        // --- Delete old appearance ---

        //Note: don't delete (m_BaseSkinBitmap), it just points to one of the other bitmaps
        DELETE_AND_NULL(m_NormalBaseSkinBitmap);
        DELETE_AND_NULL(m_BacklightrdBaseSkinBitmap);
        DELETE_AND_NULL(m_CursorSkinBitmap);
        DELETE_AND_NULL(m_PressedSkinBitmap);

        m_sduScreen->PreserveContents(&sduScreen_contents);
        m_menuScreen->PreserveContents(&menuScreen_contents);
        bool bActiveScreenIsMenuScreen = false;
        if (m_activeScreen==m_menuScreen) bActiveScreenIsMenuScreen = true;
        DELETE_AND_NULL(m_sduScreen);
        DELETE_AND_NULL(m_menuScreen);
        if (gDisablePython == 0) {
            m_python->setScreen(NULL); /* set it NULL so it doesn't try to use the old one */
        }

        // --- Create new appearance ---

        // fixup status bar
        #if wxUSE_STATUSBAR
        #if MCDU_IN_STATUSBAR
        {
            int widths[] = {which_mcdu_geom->bmp_width-90, 90};
            SetStatusWidths(2, widths);
            wxString im_msg; im_msg.Printf(wxT("MCDU#%d"), gConfig.cfgMcdu);
            SetStatusTextSafely(im_msg,1); /* show in second cell of status bar */
        }
        #endif //MCDU_IN_STATUSBAR
        #endif // wxUSE_STATUSBAR

        // load bitmaps
        Load_Appearance_Bitmaps();

        m_sduScreen  = new CTermScreen( (wxFrame *)this, "SDU");
        m_menuScreen = new CTermScreen( (wxFrame *)this, "Menu");
        m_sduScreen->RestoreContents(&sduScreen_contents);
        m_menuScreen->RestoreContents(&menuScreen_contents);
        m_activeScreen = m_sduScreen;
        if (bActiveScreenIsMenuScreen) m_activeScreen = m_menuScreen;
        // let Pythin know about the changes
        if (gDisablePython == 0) {
            m_python->setScreen(m_activeScreen);
        }
        // let CProt know about the changes
        m_prot->setScreen(m_sduScreen, m_menuScreen);
        /* let CProt know where the scratchpad is */
        m_prot->setEquipmentInfo(which_mcdu_geom->equipment_id, SCREEN_N_ROWS);

        // resize to new window dimensions
        wxSize wxWindowSize = New_CMcduSimFrame_Size();
        SetWindowPos(m_hWnd, NULL, 0, 0, 
            wxWindowSize.GetWidth(), wxWindowSize.GetHeight(),
                                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    } /* if changed appearance */

    wxRegion region = GetUpdateRegion();
    wxRect rect = region.GetBox();
    skinControls_st controlAreaToUpdate = {0,0};
    controlAreaToUpdate.left = rect.x;
    controlAreaToUpdate.right = rect.x + rect.width;
    controlAreaToUpdate.top = rect.y;
    controlAreaToUpdate.bottom = rect.y + rect.height;

    wxPaintDC dc(this);
    dc.SetPen(* wxRED_PEN);

    if (findControl(&controlAreaToUpdate) == 0) // if there is no match
    {
        // lets repaint the entire base
        if ( m_BaseSkinBitmap && m_BaseSkinBitmap->Ok() )
        {
            wxMemoryDC memDC;
            if ( m_BaseSkinBitmap->GetPalette() )
            {
                memDC.SetPalette(* m_BaseSkinBitmap->GetPalette());
                dc.SetPalette(* m_BaseSkinBitmap->GetPalette());
            }

            memDC.SelectObject(* m_BaseSkinBitmap);
            // Normal, non-transparent blitting
            dc.Blit(0, 0, m_BaseSkinBitmap->GetWidth(), m_BaseSkinBitmap->GetHeight(), & memDC, 0, 0, wxCOPY, false);
            memDC.SelectObject(wxNullBitmap);
        }

        m_lastCursorOver.controlID = 0; // we do not need to erase old cursor over as everything was erased
    }

    // if there is an old cursor over to be erased the do so
    if (m_lastCursorOver.controlID)
    {
        //LOG("onPaint:there is an old cursor over to be erased");
        // old cursor area matches with area to update ?
        //if (m_lastCursorOver.controlID == controlAreaToUpdate.controlID)
        {
            // so lets copy the area
            wxMemoryDC memDC;
            if ( m_BaseSkinBitmap->GetPalette() )
            {
                memDC.SetPalette(* m_BaseSkinBitmap->GetPalette());
                dc.SetPalette(* m_BaseSkinBitmap->GetPalette());
            }

            memDC.SelectObject(* m_BaseSkinBitmap);
            dc.Blit(    m_lastCursorOver.left, m_lastCursorOver.top,
                        m_lastCursorOver.right - m_lastCursorOver.left,
                        m_lastCursorOver.bottom - m_lastCursorOver.top,
                        & memDC,
                        m_lastCursorOver.left, m_lastCursorOver.top,
                        wxCOPY, false);
            memDC.SelectObject(wxNullBitmap);

            m_lastCursorOver.controlID = 0;
        }
    }

    // if there is a current cursor over to painted then do so
    if (m_currentCursorOver.controlID)
    {
        // cursor over area matches with area to update ?
        if (m_currentCursorOver.controlID == controlAreaToUpdate.controlID)
        {
            // so lets copy the area
            wxMemoryDC memDC;
            if ( m_CursorSkinBitmap->GetPalette() )
            {
                memDC.SetPalette(* m_CursorSkinBitmap->GetPalette());
                dc.SetPalette(* m_CursorSkinBitmap->GetPalette());
            }

            memDC.SelectObject(* m_CursorSkinBitmap);
            dc.Blit(    controlAreaToUpdate.left, controlAreaToUpdate.top,
                        controlAreaToUpdate.right - controlAreaToUpdate.left,
                        controlAreaToUpdate.bottom - controlAreaToUpdate.top,
                        & memDC,
                        controlAreaToUpdate.left, controlAreaToUpdate.top,
                        wxCOPY, false);
            memDC.SelectObject(wxNullBitmap);
        }
    }

    if (m_currentButtonDown.controlID)
    {
        if (m_keyDown)
        {
            // so lets copy the area
            wxMemoryDC memDC;
            if ( m_PressedSkinBitmap->GetPalette() )
            {
                memDC.SetPalette(* m_PressedSkinBitmap->GetPalette());
                dc.SetPalette(* m_PressedSkinBitmap->GetPalette());
            }

            memDC.SelectObject(* m_PressedSkinBitmap);
            dc.Blit(    m_currentButtonDown.left, m_currentButtonDown.top,
                        m_currentButtonDown.right - m_currentButtonDown.left,
                        m_currentButtonDown.bottom - m_currentButtonDown.top,
                        & memDC,
                        m_currentButtonDown.left, m_currentButtonDown.top,
                        wxCOPY, false);
            memDC.SelectObject(wxNullBitmap);
        }
        else
        {
            // so lets copy the area
            wxMemoryDC memDC;
            if ( m_BaseSkinBitmap->GetPalette() )
            {
                memDC.SetPalette(* m_BaseSkinBitmap->GetPalette());
                dc.SetPalette(* m_BaseSkinBitmap->GetPalette());
            }

            memDC.SelectObject(* m_BaseSkinBitmap);
            dc.Blit(    m_currentButtonDown.left, m_currentButtonDown.top,
                        m_currentButtonDown.right - m_currentButtonDown.left,
                        m_currentButtonDown.bottom - m_currentButtonDown.top,
                        & memDC,
                        m_currentButtonDown.left, m_currentButtonDown.top,
                        wxCOPY, false);
            memDC.SelectObject(wxNullBitmap);

            m_currentButtonDown.controlID = 0;
        }
    }

    m_activeScreen->paintScreen(rect);

#if WANT_REFRESH_COUNTER
    wxCoord eX, eY, eDesc, eLead;
    wxCoord prevY=(~0);
    wxCoord xMsg, yMsg;
    
    wxString msg; msg.Printf("MCDU#%d %9u", gConfig.cfgMcdu, m_loop_counter);

    for(;;) {
        dc.GetTextExtent(msg, &eX, &eY, &eDesc, &eLead, NULL);

        if (which_mcdu_geom == &mcdu_sim_geom) {
            xMsg = (which_mcdu_geom->bmp_width-120)-eX;
            yMsg = (SCREEN_Y+1)-(2*SCREEN_CHAR_AREA_H)-(2*eY);
        } else {
            xMsg = (which_mcdu_geom->bmp_width-140)-eX;
            yMsg = (SCREEN_Y-1)-(2*eY);
        }
        if (prevY==yMsg+eY) break;

        msg.Printf("(%d,%d)..(%d,%d) MCDU#%d %6u", xMsg, yMsg, xMsg+eX, yMsg+eY, gConfig.cfgMcdu, m_loop_counter);
        prevY = yMsg+eY;
    }

    dc.SetTextForeground(RGB(0,255,0));
    dc.DrawText(msg, xMsg, yMsg);
#endif

    // here we implement the save bmp to a file or to the debug log
    if (m_clip) {

        wxBitmap bitmap(SCREEN_W, SCREEN_H);
        wxMemoryDC copy_dc;

        copy_dc.SelectObject(bitmap);
        copy_dc.Blit(   0, 0, SCREEN_W, SCREEN_H,
                        & dc, SCREEN_X-1, SCREEN_Y+1,
                        wxCOPY, false);
        copy_dc.SelectObject(wxNullBitmap);

        bitmap.SaveFile(m_export_filename, wxBITMAP_TYPE_BMP);
        m_clip = false;
    }
}

//void CMcduSimFrame::changeSendingLED (bool onOff)
//{
//    g_LED_sending = onOff;
//    RefreshRect (wxRect( which_mcdu_skin[SKIN_CNTRL__SENDING].left,
//                         which_mcdu_skin[SKIN_CNTRL__SENDING].top,
//                         which_mcdu_skin[SKIN_CNTRL__SENDING].right - which_mcdu_skin[SKIN_CNTRL__SENDING].left,
//                         which_mcdu_skin[SKIN_CNTRL__SENDING].bottom - which_mcdu_skin[SKIN_CNTRL__SENDING].top ),
//                         false);
//
//}

//void CMcduSimFrame::changeBacklightLED (bool onOff)
//{
//    if (m_BacklightrdBaseSkinBitmap == NULL) return;
//
//    g_LED_backlight = onOff;
//    if (onOff)
//        m_BaseSkinBitmap = m_BacklightrdBaseSkinBitmap;
//    else
//        m_BaseSkinBitmap = m_NormalBaseSkinBitmap;
//    Refresh();
//
//}

//void CMcduSimFrame::changePressOkLED (bool onOff)
//{
//    g_LED_pressOK = onOff;
//    RefreshRect (wxRect( which_mcdu_skin[SKIN_CNTRL__PRESS_OK].left,
//                         which_mcdu_skin[SKIN_CNTRL__PRESS_OK].top,
//                         which_mcdu_skin[SKIN_CNTRL__PRESS_OK].right - which_mcdu_skin[SKIN_CNTRL__PRESS_OK].left,
//                         which_mcdu_skin[SKIN_CNTRL__PRESS_OK].bottom - which_mcdu_skin[SKIN_CNTRL__PRESS_OK].top ),
//                         false);
//
//}

void CMcduSimFrame::OnTimer100ms(wxTimerEvent& event)
{
#if 0
    // ************* process hold key **************
    if (m_holdKeyTimer)
    {
        m_holdKeyTimer--;
        if (m_holdKeyTimer == 0)
        {
            if (m_OK_keyIsDown) POST_EVENT(EVT_LONG_KEY__OK, 0);
            else if (m_RangeTest_keyIsDown) POST_EVENT(EVT_LONG_KEY__RANGE_TEST, 0);
            else if (m_Panic_keyIsDown) POST_EVENT(EVT_LONG_KEY__PANIC, 0);
            else if (m_Cancel_keyIsDown) POST_EVENT(EVT_LONG_KEY__CANCEL, 0);
        }
    }

    decAppFastTimers();
    while( event_getNumEvent () )
    {
        processRemoteApp();
        processChangesFromApp();
    }
#endif

#if WANT_REFRESH_COUNTER
    m_loop_counter += 1;
    int xRight, yTop, yBot;
    if (which_mcdu_geom == &mcdu_sim_geom) {
        xRight  = 252;
        yTop    = 5;
        yBot    = 20;
    } else {
        xRight  = 322;
        yTop    = 46;
        yBot    = 61;
    }
    RefreshRect (wxRect(0, yTop, xRight+1, yBot+1), false);
#endif
}

void CMcduSimFrame::OnTimer250ms(wxTimerEvent& event)
{
    //(unused)//int i;
    static int cycle = 0;

    m_activeScreen->setFlashCycle(cycle);
    cycle++;
    cycle &= 1;

    // process Key repetition
    if (m_keyDown)
    {
        if (m_keyRepeatTimer)
        {
            m_keyRepeatTimer--;
            if (m_keyRepeatTimer == 0)
            {
                m_keyRepeatTimer = REPEAT_KEY_TIME;
                onKey(m_currentButtonDown.controlID, 1);
            }
        }
    }
    if (m_python) m_python->OnTimer();

    if (m_prot) m_prot->OnTimer250ms();

    CheckMenuScreen();


#if 0
    decAppTimers();
    printAppLogs();

    while( event_getNumEvent () ) processRemoteApp();
    processChangesFromApp();

    // process motion
    if (m_motionFlag)
    {
        m_motionFlag = false;
//        POST_EVENT(EVT_MOTION,0);
    }
#endif

}

void CMcduSimFrame::printAppLogs ( void )
{
}

void CMcduSimFrame::printSimLogs( uint8_t event,  uint8_t eventParam )
{

}

void CMcduSimFrame::processChangesFromApp ( void )
{

}

void CMcduSimFrame::OnMouseDoubleClickEvent(wxMouseEvent& event)
{
 //   POST_EVENT(EVT_TAP, 0);      // post a tap event
}

#if 0
void CMcduSimFrame::playSound ( sound_t sound )
{
    const wxChar * fileNamePtr;
    switch(sound) {
    case SOUND_KEY: fileNamePtr = _T("Sounds/SOUND_KEY.wav"); break;
    case SOUND_KEY_HOLD: fileNamePtr = _T("Sounds/SOUND_KEY_HOLD.wav"); break;
    case SOUND_OK: fileNamePtr = _T("Sounds/SOUND_OK.wav"); break;
    case SOUND_OK_WEAK: fileNamePtr = _T("Sounds/SOUND_OK_WEAK.wav"); break;
    case SOUND_PANIC: fileNamePtr = _T("Sounds/SOUND_PANIC.wav"); break;
    case SOUND_NOT_OK: fileNamePtr = _T("Sounds/SOUND_NOT_OK.wav"); break;
    case SOUND_LINK_ALERT: fileNamePtr = _T("Sounds/SOUND_LINK_ALERT.wav"); break;
    case SOUND_ERROR: fileNamePtr = _T("Sounds/SOUND_ERROR.wav"); break;
    case SOUND_RANGE_TEST1: fileNamePtr = _T("Sounds/SOUND_RANGE_TEST1.wav"); break;
    case SOUND_RANGE_TEST2: fileNamePtr = _T("Sounds/SOUND_RANGE_TEST2.wav"); break;
    case SOUND_RANGE_TEST3: fileNamePtr = _T("Sounds/SOUND_RANGE_TEST3.wav"); break;
    case SOUND_RANGE_TEST4: fileNamePtr = _T("Sounds/SOUND_RANGE_TEST4.wav"); break;
    case SOUND_RANGE_TEST5: fileNamePtr = _T("Sounds/SOUND_RANGE_TEST5.wav"); break;
    default: break;
    }
    wxSound(fileNamePtr).Play( wxSOUND_ASYNC );
}
#endif

void CMcduSimFrame::OnKeyboard(wxKeyEvent& e)
{
    int key = e.GetKeyCode();
    int ok = 0;

    #if WANT_TRACE_KEYBOARD
    LOG1H("Modifier = ", e.GetModifiers() );
    LOG1H("KeyCode = ", key );
    #endif

    if (e.GetModifiers() == wxMOD_CONTROL/*2*/) // if control pressed
    {
        wxCommandEvent event;
        switch (key) {
        case 'B':   OnExportBmp             (event); break;
        case 'C':   OnTxtToClipboard        (event); break;
        case 'E':   OnExport                (event); break;
        case 'T':   OnExportTxt             (event); break;
        case ' ':   OnDumpScreenToDebugLog  (event); break;
        case 'L':   OnEnableSmallFontAsLower(event); break;
        default: break;
        }
        return;
    }

    if (!theApp->IsPcKeyboardEnabled())
    {
        LOG("Ignore key: PC Keyboard is disabled");
        return;
    }

    // map keyboard key to known keys and buttons

    switch (key) {

    case WXK_UP:                // Up arrow (in arrow keys)
    case WXK_LEFT:              // Left arrow (in arrow keys)
    case WXK_PAGEUP:            // Page Up (in Ins/Home/PageUp/Delete/End/PageDown)
    case WXK_NUMPAD_PAGEUP:     // numpad 'PgUp'
    case WXK_NUMPAD9:           // numpad '9' (Numlock on)
    case WXK_NUMPAD_LEFT:       // numpad '4' (NumLock off)
    case WXK_NUMPAD_UP:         // numpad '8' (NumLock off)
    case WXK_NUMPAD4:           // numpad '4' (NumLock on)
    case WXK_NUMPAD8:           // numpad '8' (NumLock on)
    case WXK_WINDOWS_LEFT:      // 0x189
        key = KEY__PREV_PAGE; break;

    case WXK_RIGHT:             // Right arrow (in arrow keys)
    case WXK_DOWN:              // Down arrow (in arrow keys)
    case WXK_PAGEDOWN:          // Page Down (in Ins/Home/PageUp/Delete/End/PageDown)
    case WXK_NUMPAD_PAGEDOWN:   // numpad 'PgDn'
    case WXK_NUMPAD3:           // numpad '3' (Numlock on)
    case WXK_NUMPAD_RIGHT:      // numpad '6' (NumLock off)
    case WXK_NUMPAD_DOWN:       // numpad '2' (NumLock off)
    case WXK_NUMPAD6:           // numpad '6' (NumLock on)
    case WXK_NUMPAD2:           // numpad '2' (NumLock on)
    case WXK_WINDOWS_RIGHT:     // 0x18A
        key = KEY__NEXT_PAGE; break;

    case WXK_NUMPAD_ADD:        // numpad '+'
    case WXK_ADD:               // 0x14F
        key = '+'; break;

    case WXK_NUMPAD_SUBTRACT:   // numpad '-'
    case WXK_SUBTRACT:          // 0x151
        key = '-'; break;

    case WXK_NUMPAD_DIVIDE:     // numpad '/'
    case WXK_DIVIDE:            // 0x153
        key = '/'; break;

    case WXK_NUMPAD_DELETE:     // numpad '.'
    case WXK_NUMPAD_DECIMAL:    // numpad '.' (NumLock on)
    case WXK_DECIMAL:           // 0x152
        key = '.'; break;

    case WXK_NUMPAD_EQUAL:      // 0x182
        key = '='; break;

    case WXK_NUMPAD_MULTIPLY:   // numpad '*'
        key = '*'; break;

    case WXK_BACK:              // Backspace
    case WXK_DELETE:            // Delete
        key = KEY__CLR; break;

    case WXK_F1:                // F1 -> L1
    case WXK_F2:                // F2 -> L2
    case WXK_F3:                // F3 -> L3
    case WXK_F4:                // F4 -> L4
    case WXK_F5:                // F5 -> L5
    case WXK_F6:                // F6 -> L6
        key = KEY__L1 + (key-WXK_F1); break;

    case WXK_F7:                // F7 -> R1
    case WXK_F8:                // F8 -> R2
    case WXK_F9:                // F9 -> R3
    case WXK_F10:               // F10-> R4
    case WXK_F11:               // F11-> R5
    case WXK_F12:               // F12-> R6
        key = KEY__R1 + (key-WXK_F7); break;

    default:
        // lower-case a..z to upper-case
        // can't use islower, key can be > 0xFF
        if (key>='a' && key<='z' /*islower(key)*/) {
            key = toupper(key);
        }
    }

    // is this a key/button that the MCDU can handle?
    if ( (key >= '0') && (key <= '9') ) ok = 1;
    else
    if (key >= KEY__L1 && key <= KEY__R6)  ok = 1;  // KEY__L1 through KEY__R6, includes KEY__PREV_PAGE,KEY__NEXT_PAGE
    else
    if ( (key >= 'A') && (key <= 'Z') ) ok = 1;
    else
    switch (key) {
    case KEY__CLR:          /* backspace or 0x08 or '\b' */
    case KEY__SPACE:        /* ' ' */
    case KEY__PLUS_MINUS:   /* '-' */
    case KEY__DOT:          /* '.' */
    case KEY__DASH:         /* '/' */
        ok = 1;
    default:
        break;
    }

    //  Some PC keys are not mapped:0x1E and 0x5E.

    if (ok == 1)
    {
        //m_prot->sendKey(key);
        onKey(key, 0);

        #if wxUSE_STATUSBAR
        wxString key_name; key_name.Printf(wxT("%s pressed"), LookupKey(key));
        SetStatusTextSafely(key_name);
        #endif // wxUSE_STATUSBAR
    }

}

void trim_trailing_blanks(char * buffer)
{
    char * p = &buffer[strlen(buffer)];
    while (--p >= buffer) {
        if (*p!=' ')
            break;
        *p = '\0';
    }
}

void trim_leading_blanks(char * buffer)
{
    char * p = buffer;
    while (*p==' ') ++p;
    /* now buffer is pointing at the start, and p points at the first non-blank
     *         [ _ _ _ _ t e x t \0 ]
     *           ^       ^
     *         buffer    p
     */
    /* now copy from p back over buffer, including the trailing null char */
    for(;;) {
        *buffer++ = *p;
        if (!*p)
            break;
        ++p;
    }
}

void CMcduSimFrame::GoToMenuScreen(void)
{
    if (m_activeScreen != m_menuScreen) {
        setActiveScreenAndUpdate(m_menuScreen);

        // we may need to send LOGOFF here
        if (m_prot) m_prot->sendKey(0x18);

        if (m_prot) m_prot->sendEnquire(1/*0=text,1=menu*/, 0/*0=L1/Menu,1=from timer*/);
    }
}

void CMcduSimFrame::OnGoToMenuScreen( wxCommandEvent& event )
{
    GoToMenuScreen();
}

void CMcduSimFrame::onKey(int key, int repeatFlag)
{
    int processKey = 0;
    int repeat = 0;

    if (repeatFlag) repeat = 0x00000080;

    if(key == 0x08) processKey = 1;                      // CLR
    if(key == 0x1e) processKey = 1;                      // arrow down
    if(key == ' ') processKey = 1;                      // space

    if( (key>= '-') && (key <= '9')) processKey = 1;  // -  . / 0~9
    if( (key>= 'A') && (key <= 'Z')) processKey = 1;  // A~Z
    if(key == 0x5e) processKey = 1;                     // arrow up
    if( (key>= 0x70) && (key <= 0x7d)) processKey = 1;  // SEL1 to SELR6, PREV PAGE, NEXT PAGE
    if(key == 0x7f) processKey = 1;                     // CLR/DEL

    if ((key >= KEY__L1 && key <= KEY__L6) ||
        (key >= KEY__R1 && key <= KEY__R6)) {

        int which_key = (key-KEY__L1)+1; // 1,2,3,4,5,6
        char buffer[MAX_SCREEN_N_COLS+1];

        m_activeScreen->getScreenStringForKey(buffer, key);
        log_printf("------------ MCDU#%d press %s \"%s\"%s\n", 
                                    gConfig.cfgMcdu, LookupKey(key), buffer, repeatFlag?" repeat":"");
    }
    else
        log_printf("------------ MCDU#%d press %s%s%s\n", 
                                    gConfig.cfgMcdu, LookupKey(key), repeatFlag?" repeat":"", 
                                                                     processKey?"":" -- ignored");

    if (/*m_activeScreen == m_sduScreen && */ key == KEY__MENU)
    {
        GoToMenuScreen();
        return;
    }

    if (m_activeScreen == m_menuScreen && key == KEY__L1 && m_prot->getSduSAL( ) > 0) {
        m_sduScreen->clear();
        setActiveScreenAndUpdate(m_sduScreen);

        if (m_prot) m_prot->sendEnquire(0/*0=text,1=menu*/, 0/*0=L1/Menu,1=from timer*/);
        return;
    }

    // send code 0xc on RTE button press - vgorine 30.11.2009
    if (key == KEY__RTE)
    {
        m_prot->sendKey(0x0c | repeat);

        return;
    }

    // do not send keys to MCDU is in Menu screen
    if (m_activeScreen == m_menuScreen) return;

    if (processKey == 1)
    {
        if (m_prot) m_prot->sendKey(key | repeat);
    }
}

void CMcduSimFrame::setTheActiveScreen( CTermScreen * screenPtr )
{
    if (m_activeScreen != screenPtr) {
        if (m_activeScreen) {
            log_printf(" Frame switching screen %s to %s\n",
                m_activeScreen->ScreenIdent(), screenPtr->ScreenIdent());
        }

        m_activeScreen = screenPtr;
    } // else not changed
}

void CMcduSimFrame::setActiveScreenAndUpdate( CTermScreen * screenPtr )
{
    setTheActiveScreen (screenPtr);

    m_activeScreen->setUpdateRowsCols (0, SCREEN_N_COLS-1, 0, SCREEN_N_ROWS-1  );
    m_activeScreen->commitUpdates();

    if (gDisablePython == 0) {
        m_python->setScreen(m_activeScreen);
    }
}

void CMcduSimFrame::OnExport(wxCommandEvent& event)
{
    FILE * base = NULL;
    FILE * out = NULL;
    int row = 0;
    int col = 0;
    char c;
    int ret;
    termChar_st charDesc;
    #include "export_string.h"
    char * export_string_p = (char *)NULL;

    wxFileDialog fd( this, //wxWindow* parent,
                                    wxT("Choose a file"),
                                    wxT(""),     // const wxString& defaultDir = "",
                                    wxT("*.xml"), //const wxString& defaultFile = "*.xml",
                                    wxT("XML files (*.xml)|*.xml"), // const wxString& wildcard = "*.*",
                                    /* in 2.6 was wxOVERWRITE_PROMPT*/ wxFD_OVERWRITE_PROMPT |
                                    /* in 2.6 was wxSAVE*/ wxFD_SAVE); //long style = 0,


    ret = fd.ShowModal();

    if (ret != wxID_OK)
    {
        LOG("Not exported");
        return;
    }
    LOG("Exporting...");

    base = fopen( EXPORT_XMLBASE_FNAME, "rb");
    if (base == NULL)
    {
        export_string_p = export_string;
        //LOG("bad base filename");
        //return;
    }

    out = fopen( (const char *) fd.GetPath().c_str() , "wb" );
    if (out == NULL)
    {
        LOG("bad filename");
        return;
    }

    while (1) //row < SCREEN_N_ROWS)
    {
        if (base) {
            ret = fread(&c, 1, 1, base);
        } else {
            c = *export_string_p++;
            ret = 1;
            if (!c) {
                ret = EOF;
            }
        }
        if (ret == EOF || (base && feof(base)))
        {
            if (base) fclose(base);
            fclose(out);
            if (row < SCREEN_N_ROWS) LOG("incomplete export ???");
            return;
        }
        if (ret != 1)
        {
            if (base) fclose(base);
            fclose(out);
            LOG("Read error");
            return;
        }
        if (c == '$')
        {
            if (row >= SCREEN_N_ROWS)
            {
                //LOG("Row overflow");
                //if (base) fclose(base);
                //fclose(out);
                //return;
                for (col=0; col<SCREEN_N_COLS; col++)
                {
                    fprintf( out, "<Cell ss:StyleID=\"s21\"/>");
                }
            }
            else
            {
                for (col=0; col<SCREEN_N_COLS; col++)
                {
                    m_activeScreen->getCharDesc( &charDesc, row, col );
                    if (charDesc.charcode == 0) charDesc.charcode = ' ';
                    if (charDesc.charcode >= '0' && charDesc.charcode <= '9')
                        fprintf( out, EXPORT_CELL_NUM, charStyle[charDesc.size & 1][charDesc.color], charDesc.charcode);
                    else
                        fprintf( out, EXPORT_CELL_TXT, charStyle[charDesc.size & 1][charDesc.color], charDesc.charcode);
                }
            }
            row++;
        }
        else
        {
            ret = fwrite(&c,1,1,out);
            if (ret != 1)
            {
                if (base) fclose(base);
                fclose(out);
                LOG("Read error");
                return;
            }
        }
    }
}

void CMcduSimFrame::OnExportBmp(wxCommandEvent& event)
{
    int ret;



    wxFileDialog fd( this, //wxWindow* parent,
                                    wxT("Choose a file"),
                                    wxT(""),     // const wxString& defaultDir = "",
                                    wxT("*.bmp"), //const wxString& defaultFile = "*.xml",
                                    wxT("BMP files (*.bmp)|*.bmp"), // const wxString& wildcard = "*.*",
                                    /* in 2.6 was wxOVERWRITE_PROMPT*/ wxFD_OVERWRITE_PROMPT |
                                    /* in 2.6 was wxSAVE*/ wxFD_SAVE); //long style = 0,



    ret = fd.ShowModal();

    if (ret != wxID_OK)
    {
        LOG("BMP Not exported");
        return;
    }
    LOG("Exporting BMP...");
    m_export_filename = fd.GetPath();

    m_clip = true;
    Refresh();
}

void CMcduSimFrame::OnDumpScreenToDebugLog(wxCommandEvent& event)
{
    DumpScreen_Helper (m_activeScreen, NULL/*to debug log*/);
}

void CMcduSimFrame::OnRequestToExportBmp(wxCommandEvent& event)
{
    // save screenshots in the working directory
    // which may be redefined in command line -w parameter vgorine 07.12.2009
    //(im) //m_export_filename = wxT(DEFAULT_EXPORTED_FILE_NAME);
    m_export_filename = gExportDir + DEFAULT_EXPORTED_FILE_NAME;
    
    m_clip = true;
    Refresh();
}

void CMcduSimFrame::OnSendMcduId(wxCommandEvent& event)
{
    if (m_prot) m_prot->sendMcduID();
}

void CMcduSimFrame::OnRxAck(wxCommandEvent& event)
{
    if (m_python) m_python->ProcessAck();
}

void CMcduSimFrame::OnEnablePcKeyboardMcduId(wxCommandEvent& event)
{
    // if it was enabled, disable it. if it was disabled, enable it.
    theApp->EnablePcKeyboard(!theApp->IsPcKeyboardEnabled());
}

void CMcduSimFrame::OnEnableSmallFontAsLower(wxCommandEvent& event)
{
    // if it was enabled, disable it. if it was disabled, enable it.
    theApp->EnableSmallFontAsLower(!theApp->IsSmallFontAsLowerEnabled());
}

void CMcduSimFrame::OnConfigureMCDU(wxCommandEvent& event)
{
    // make a copy so we can tell if anything changed
    config_t old_config = gConfig;

    #if wxUSE_STATUSBAR
    SetStatusTextSafely(_T("Configure MCDU Settings ..."));
    #endif // wxUSE_STATUSBAR

    // pop up the configuration dialog, allow saving to registry
    CConfDlg dlg;
    wxXmlResource::Get()->LoadDialog( (wxDialog *) &dlg, NULL, wxT("ID_CONFDLG"));
    if (dlg.CreateControls()) {
        dlg.ShowModal();
    }

    // see if anything changed
    if (memcmp(&old_config, &gConfig, sizeof(gConfig))!=0) {
        // did the MCDU# change?
        if (old_config.cfgMcdu != gConfig.cfgMcdu) {
            // if so, we need to check that there isn't already
            // an MCDU with the same number already running
            if (AnotherMCDU_Running(gConfig.cfgMcdu)) {
                wxString msg;
                msg.Printf( wxT("Sorry, another MCDU #%d Simulator application seems ")
                            wxT("to be running already -- reverting this MCDU to #%d"),
                                        gConfig.cfgMcdu, old_config.cfgMcdu);
                // reclaim our old number
                gConfig.cfgMcdu = old_config.cfgMcdu;
                AnotherMCDU_Running(gConfig.cfgMcdu);
                // tell user what happened
                MessageBox(NULL, msg.wc_str(), _T("MCDU Simulator"), MB_ICONSTOP);
            } else {
                log_printf("Config: MCDU# changed from %d to %d\n", old_config.cfgMcdu, gConfig.cfgMcdu);
            }
        }

        // did the MCDU# or any ARINC parameters change?
        if (old_config.cfgMcdu != gConfig.cfgMcdu ||
            old_config.cfgConnectionType != gConfig.cfgConnectionType ||
            old_config.cfgArincCard != gConfig.cfgArincCard ||
            old_config.cfgArincGroup != gConfig.cfgArincGroup ||
            old_config.cfgArincParity != gConfig.cfgArincParity ||
            old_config.cfgArincTxChannel != gConfig.cfgArincTxChannel ||
            old_config.cfgArincTxSpeed != gConfig.cfgArincTxSpeed ||
            old_config.cfgArincRxChannel != gConfig.cfgArincRxChannel ||
            old_config.cfgArincRxSpeed != gConfig.cfgArincRxSpeed) {

            log_printf("Config: Driver parameters changed\n");

            // tell CProt to update its driver
            m_prot->UpdateDriver(gConfig.cfgMcdu/*1,2,3,4*/ - 1);
        }

        // did the appearance of the MCDU change?
        if (old_config.cfgMcduAppearance != gConfig.cfgMcduAppearance) {

            // setup MCDU Appearance: either 10-line TCP or Standard 14-line
            if (gConfig.cfgMcduAppearance == TCP_MCDU_APPEARANCE_TYPE) {
                which_mcdu_geom = &tcp_sim_geom;
                which_mcdu_skin = &skinControls_TCP[0];
            } else { // must be standard 14-line MCDU
                which_mcdu_geom = &mcdu_sim_geom;
                which_mcdu_skin = &skinControls[0];
            }
            
            log_printf("Config: Appearance changed to %s (%d rows x %d cols, eqpt id $%03X)\n",
                        (which_mcdu_geom==&tcp_sim_geom)?"TCP":"Std",
                        which_mcdu_geom->screen_n_rows, which_mcdu_geom->screen_n_cols,
                        which_mcdu_geom->equipment_id);

            m_changed_appearance = true;
            // repaint the entire window
            Refresh(true, (const wxRect *) NULL);
        }

        // show new MCDU# if it changed
        #if wxUSE_STATUSBAR
        SetStatusTextSafely(_T("Configuration updated"));
        #if MCDU_IN_STATUSBAR
        {
            wxString im_msg; im_msg.Printf(wxT("MCDU#%d"), gConfig.cfgMcdu);
            SetStatusTextSafely(im_msg,1); /* show in second cell of status bar */
        }
        #endif //MCDU_IN_STATUSBAR
        #endif // wxUSE_STATUSBAR
    } else {
        // nothing changed
        #if wxUSE_STATUSBAR
        SetStatusTextSafely(_T("Configuration not changed"));
        #endif // wxUSE_STATUSBAR
    }

    return;
}

void CMcduSimFrame::OnSduDisconnection(wxCommandEvent& event)
{
    #if WANT_DEBUG_STARTUP
    if (m_prot) {
        char *interpret_state(/*enum enu_state*/int state); /* in prot_739A.cpp */
        log_printf("OnSduDisconnection; state %s ... new state %s\n", interpret_state(m_prot->State()), interpret_state(0));
    }
    #endif

    /* (im) clear the screen so people know something bad happened */
    m_sduScreen->clear();
    m_menuScreen->clear();
    printMenuTitle();
    setActiveScreenAndUpdate(m_menuScreen);
}

void CMcduSimFrame::printMenuTitle( void )
{
    char title_line[25];
    sprintf(title_line, "EMS MCDU SIMULATOR");
    #if !MCDU_IN_STATUSBAR
    //sprintf(title_line, "EMS MCDU SIM #d", gConfig.cfgMcdu);
    #endif
    m_menuScreen->printText(/*row*/0, /*col*/3,
                            /*text*/title_line,
                            /*size*/0, /*color*/C_YELLOW, /*attrib*/0);
    //(im)//playing around
    //int iColor;
    //for (iColor=C_BLACK; iColor<=C_WHITE; iColor++) {
    //    int iAttrib = ATT_NORMAL;
    //    if (iColor==C_BLACK) iAttrib = ATT_REVERSE;

    //    m_menuScreen->printText(
    //            SCREEN_N_ROWS-1-iColor, 0,
    //            "Testing", 
    //            CHAR_SIZE_NORMAL, iColor, iAttrib);
    //}
    //(im)//end playing around
}

void CMcduSimFrame::CheckMenuScreen( void )
{
    // if active screen is the Menu and we have SDU-SAL
    // we should have an text on row 2 col 0.
    // If this is not the case then send a ENQ
    static int counter = 0;
    termChar_st screenChar;

    // erase any old status messages after three seconds
    #if wxUSE_STATUSBAR
    static int status_counter = 0;
    ++ m_status_counter;
    if (m_status_counter>(3*4)) {   // 3 seconds is 12 x 250ms
        SetStatusTextSafely(_T(""));
    }
    #endif // wxUSE_STATUSBAR

    /* (im) We just happen to be called from OnTimer250ms(),
     *      but we want to check the screen only every 2 seconds
     */
    counter++;
    if (counter<8) return;
    counter = 0;

    /* (im) Don't check if we're on the normal screen, or
     *      haven't got transport or a SAL yet.
     */
    if (m_activeScreen != m_menuScreen) return;
    if ( !m_prot || m_prot->getSduSAL() == 0) return;

    /* Have a peek */
    m_menuScreen->getCharDesc (&screenChar, 2, 0);

    static char old_char = '?';
    if (old_char != screenChar.charcode) {
        LOG1H("CheckMenuScreen (line 2 col 0) Main Menu entry: ", screenChar.charcode);
        old_char = screenChar.charcode;
    }

    if (screenChar.charcode == 0 || screenChar.charcode == 0x20)
    {
        if (!m_prot->Waiting_on_ENQ_RTS_timeout()) {
            log_printf("CheckMenuScreen, sendEnquire\n");
            m_prot->sendEnquire(1/*0=text,1=menu*/, 1/*0=L1/Menu,1=from timer*/);
        } /* else already sent one ENQ, let's wait a few seconds to see how it goes */
    }
}

void CMcduSimFrame::OnKeyFromPython(wxCommandEvent& event)
{
    int key = event.GetInt();

    // if active screen is the main menu then fake ack from SDU
    if (m_activeScreen == m_menuScreen) {
        if (m_python) {
            LOG("MCDU on Menu screen...Fake SDU ack");
            m_python->ProcessAck();
        }
    }

    onKey(key,0);
}

void CMcduSimFrame::OnExportTxt(wxCommandEvent& event)
{
    FILE * out;

    wxDateTime now = wxDateTime::Now();
    static int count = 0;
    wxString name = TXT_FILE_NAME + now.FormatISODate() + '_';
    wxString timeSt = now.FormatISOTime();
    timeSt.Replace(":", "_");
    name += timeSt + '_';

    wxString name2 = wxString::Format(wxT("%s%d.txt"), name.c_str(), (int)count++);

    LOG1S("name:", name2.c_str() );

    out = fopen( name2.c_str() , "wb" );
    if (out == NULL)
    {
        LOG("bad filename");
        return;
    }

    DumpScreen_Helper (m_activeScreen, out);
    fclose(out);
}

void CMcduSimFrame::OnTxtToClipboard (wxCommandEvent& event)
{
    int row = 0;
    int col = 0;
    //(unused)//char c;
    //(unused)//int ret;
    termChar_st charDesc;

    wxString    txt, t;

    txt =  wxT(" |123456789012345678901234|\r\n" );
    txt += wxT("-|------------------------|\r\n" );
    for (row=0; row<SCREEN_N_ROWS; row++)
    {
        if (row<9)
            t = wxString::Format(wxT("%d|"), row+1 );
        else
            t = wxString::Format(wxT("%d|"), row-9 );
        txt += t;

        for (col=0; col<SCREEN_N_COLS; col++)
        {
            m_activeScreen->getCharDesc( &charDesc, row, col );
            if (charDesc.charcode == 0) charDesc.charcode = ' ';
            txt += charDesc.charcode;
        }
        txt += wxT("|\r\n" );
    }

    txt += wxT(" |------------------------|\r\n" );

    // Write text to the clipboard
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData( new wxTextDataObject(txt) );
        wxTheClipboard->Close();
    }

}

//void CMcduSimFrame::adjustSkin( void )
//{
//    int i = 0;
//    skinControls_st * controlArrayPtr = which_mcdu_skin;
//
//    while (controlArrayPtr->controlID)
//    {
//        controlArrayPtr->top += Y_OFFSET_FOR_NEW_SKIN;
//        controlArrayPtr->bottom += Y_OFFSET_FOR_NEW_SKIN;
//        controlArrayPtr++;
//        i++;
//    }
//}

void CMcduSimFrame::Load_Appearance_Bitmaps(void)
{
    m_NormalBaseSkinBitmap      = Load_Bitmap_Helper(which_mcdu_geom->bmp_normal, which_mcdu_geom->bmp_type,
                                                     which_mcdu_geom->res_normal, "Normal");

    m_BacklightrdBaseSkinBitmap = Load_Bitmap_Helper(which_mcdu_geom->bmp_backlit, which_mcdu_geom->bmp_type,
                                                     which_mcdu_geom->res_backlit, "BackLit");

    m_CursorSkinBitmap          = Load_Bitmap_Helper(which_mcdu_geom->bmp_mouseOver, which_mcdu_geom->bmp_type,
                                                     which_mcdu_geom->res_mouseOver, "MouseOver");

    m_PressedSkinBitmap         = Load_Bitmap_Helper(which_mcdu_geom->bmp_pressed, which_mcdu_geom->bmp_type,
                                                     which_mcdu_geom->res_pressed, "MouseDown");

    m_BaseSkinBitmap = m_NormalBaseSkinBitmap;
}

/////////////////////////////////////////////////////////////////////////////////

/* end of mcduSim.cpp */
