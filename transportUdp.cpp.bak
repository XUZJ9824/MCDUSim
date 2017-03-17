/*****************************************************************
 *
 * Name:        transportPipe.cpp
 * Purpose:     part of Display Interface Manager (DIM) Simulator and MCDU Simulator
 * Author:      Ian MacRae
 * Created:     July 2011
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportPipe.cpp,v 1.29 2012/05/31 19:02:59 macrae.i Exp $
 */

#ifdef __WXMSW__    // part of MCDU Simulator

#define WANT_DEBUG_STARTUP                  0   // 0=normal, 1=debug pipe creation and connections
#define WANT_DEBUG_SHUTDOWN                 0   // 0=normal, 1=show debugging at shutdown (BG threads, etc)
#define WANT_DEBUG_SIGNALS                  0   // 0=normal, 1=show all wait and signal events
#define WANT_DEBUG_VERBOSE                  0   // 0=normal, 1=show everything (don't need WANT_DEBUG_WRITER or WANT_DEBUG_READER too)
#define WANT_DEBUG_READER                   0   // 0=normal, 1=debug BG reader thread
#define WANT_DEBUG_WRITER                   0   // 0=normal, 1=debug BG writer thread
#define WANT_CIRC_BUFFER_TRACE              0   // 0=normal, 1=debug using circular buffer
#define WANT_SHOW_NOT_YET                   0   // 0=normal, 1=show sendWord not yet (when debugging startup)

#define WANT_SHOW_ALL_TX_DATA               0   // 0=normal, 1=show all write data
#define WANT_SHOW_TX_WORD                   0   // 0=normal, 1=show Tx hex words
#define WANT_SHOW_INTERESTING_TX            1   // 0=normal, 1=show some Tx hex words
#define WANT_SHOW_ALL_RX_DATA               0   // 0=normal, 1=show all read data
#define WANT_SHOW_RX_WORD                   0   // 0=normal, 1=show Rx hex words
#define WANT_SHOW_INTERESTING_RX            1   // 0=normal, 1=show some Rx hex words
#define WANT_TRACE_HEARTBEAT                0   // 0-normal, 1=show heartbeat & related state transitions

#include "conditionals.h"

#define WANT_SHOW_INTERESTING_NON_DATA      1   // TransportPipe: show all words except printable data
#define WANT_SHOW_INTERESTING_ENQ           0   // TransportPipe: show ENQ word
#define WANT_SHOW_INTERESTING_ACK           0   // TransportPipe: show ACK word
#define WANT_SHOW_INTERESTING_CLR           0   // TransportPipe: show CLR word
#define WANT_SHOW_INTERESTING_RTS           0   // TransportPipe: show RTS word
#define WANT_SHOW_INTERESTING_CTS           0   // TransportPipe: show CTS word
#define WANT_SHOW_INTERESTING_DC1           0   // TransportPipe: show DC1 word (MCDU key/button and LOGOFF)

#else

#include "DebugFlags.h" // DIM Sim Debug flags

#define WANT_DEBUG_STARTUP             WANT_DEBUGFLAG(DEBUG_STARTUP          )
#define WANT_DEBUG_SHUTDOWN            WANT_DEBUGFLAG(DEBUG_SHUTDOWN         )
#define WANT_DEBUG_SIGNALS             WANT_DEBUGFLAG(DEBUG_SIGNALS          )
#define WANT_DEBUG_VERBOSE             WANT_DEBUGFLAG(DEBUG_TRANSPORT_VERBOSE) //not just DEBUG_VERBOSE
#define WANT_DEBUG_READER              WANT_DEBUGFLAG(DEBUG_READER           )
#define WANT_DEBUG_WRITER              WANT_DEBUGFLAG(DEBUG_WRITER           )
#define WANT_CIRC_BUFFER_TRACE         WANT_DEBUGFLAG(CIRC_BUFFER_TRACE      )
#define WANT_SHOW_NOT_YET              WANT_DEBUGFLAG(SHOW_NOT_YET           )
#define WANT_SHOW_ALL_TX_DATA          WANT_DEBUGFLAG(SHOW_ALL_TX_DATA       )
#define WANT_SHOW_TX_WORD              WANT_DEBUGFLAG(SHOW_TX_WORD           )
#define WANT_SHOW_ALL_RX_DATA          WANT_DEBUGFLAG(SHOW_ALL_RX_DATA       )
#define WANT_SHOW_RX_WORD              WANT_DEBUGFLAG(SHOW_RX_WORD           )
#define WANT_TRACE_HEARTBEAT           WANT_DEBUGFLAG(TRACE_HEARTBEAT        )
#define WANT_DEBUG_TRANSPORT_STATUS    WANT_DEBUGFLAG(DEBUG_TRANSPORT_STATUS )

#define WANT_SHOW_INTERESTING_NON_DATA WANT_DEBUGFLAG(SHOW_INTERESTING_NON_DATA) /* TransportPipe: show all words except printable data */
#define WANT_SHOW_INTERESTING_ENQ      WANT_DEBUGFLAG(SHOW_INTERESTING_ENQ     ) /* TransportPipe: show ENQ word */
#define WANT_SHOW_INTERESTING_ACK      WANT_DEBUGFLAG(SHOW_INTERESTING_ACK     ) /* TransportPipe: show ACK word */
#define WANT_SHOW_INTERESTING_CLR      WANT_DEBUGFLAG(SHOW_INTERESTING_CLR     ) /* TransportPipe: show CLR word */
#define WANT_SHOW_INTERESTING_RTS      WANT_DEBUGFLAG(SHOW_INTERESTING_RTS     ) /* TransportPipe: show RTS word */
#define WANT_SHOW_INTERESTING_CTS      WANT_DEBUGFLAG(SHOW_INTERESTING_CTS     ) /* TransportPipe: show CTS word */
#define WANT_SHOW_INTERESTING_DC1      WANT_DEBUGFLAG(SHOW_INTERESTING_DC1     ) /* TransportPipe: show DC1 word (MCDU key/button and LOGOFF) */

#endif

#include <windows.h>    // for Sleep()
#include <stdio.h>      // for sprintf_s

#include <string.h>     // for strcmp, memset
#include <tchar.h>
#include <process.h>    // for _beginthreadex
#include <sddl.h>       // for ConvertStringSecurityDescriptorToSecurityDescriptor, SDDL_REVISION_1

#include "MSecTime.h"
#include "transportPipe.h"

#define MCDU_SIM___CUSTOM_EVENT_RX 1002  // from defs.h

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName); /* in mcduSim.cpp */

/////////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__    // part of MCDU Simulator
#include "log.h"
char * our_ident    = "MCDU";
char * other_ident  = "DISP";
char * our_indent   = "          ";
char * other_indent = "";
#else               // part of Display Interface Manager (DIM) Simulator
#define log_printf sc_printf
extern "C" {
extern int sc_printf(char * fmt, ...); // defined in engine.c
}; /* extern "C" */
char * our_ident    = "DISP";
char * other_ident  = "MCDU";
char * our_indent   = "";
char * other_indent = "          ";
#endif //__WXMSW__

#define WANT_DEBUG_MCDU(n) (1<<n)

int Debug_which_MCDU = (~0); //WANT_DEBUG_MCDU(1) | WANT_DEBUG_MCDU(0);

static bool DebugThisMCDU(int mcdu_index)
{
    if (Debug_which_MCDU & (1<<mcdu_index)) return true;
    return false;
}

#define DEBUG_MCDU(x)     DebugThisMCDU(x->MCDU())
#define DEBUG_THIS_MCDU() DebugThisMCDU(   MCDU())

#define MCDU_ID(x)        (x->MCDU()+1)
#define THIS_MCDU_ID()    (   MCDU()+1)

extern int Ignore_This_Rx_Word(long r);

/////////////////////////////////////////////////////////////////////////////////

#define PIPE_NAME_BASE      "DI2MCDU"   // DI to MCDU pipeline

#define BUFFER_SIZE         512         // max message size (bytes) for a single pipe message

typedef unsigned long long  ULONGLONG;

////////////////////////////////////////////////////////////

// helper functions

#if WINVER >= 0x0500
#define printf_err_and_log_APPEND(x) strcat_s(msg, sizeof_msg, x)
#else
#define printf_err_and_log_APPEND(x) strcat  (msg,             x)
#endif

void append_interpreted_error(char * msg, size_t sizeof_msg, DWORD dwErr, char * sPipeName)
{
    char msg_with_pipename[150];
    if (!sPipeName) sPipeName = "{NULL}";
    // interpret known error code to help when debugging
    switch (dwErr) {
    case ERROR_BAD_NET_NAME:
        sprintf_s(msg_with_pipename, sizeof(msg_with_pipename), " (BAD_NET_NAME %s)",
                                                    sPipeName);
        printf_err_and_log_APPEND(msg_with_pipename);
        break;
    case ERROR_INVALID_NAME:
        sprintf_s(msg_with_pipename, sizeof(msg_with_pipename), " (INVALID_NAME %s)",
                                                    sPipeName);
        printf_err_and_log_APPEND(msg_with_pipename);
        break;
    case ERROR_FILE_NOT_FOUND:
        printf_err_and_log_APPEND(" (FILE_NOT_FOUND)");
        break;
    case ERROR_INVALID_HANDLE:
        printf_err_and_log_APPEND(" (INVALID_HANDLE)");
        break;
    case ERROR_SEM_TIMEOUT:
        printf_err_and_log_APPEND(" (SEM_TIMEOUT)");
        break;
    #if WINVER >= 0x0500
    case ERROR_DOWNGRADE_DETECTED:
        printf_err_and_log_APPEND(" (DOWNGRADE_DETECTED)");
        break;
    #endif
    case ERROR_NO_DATA:
        printf_err_and_log_APPEND(" (NO_DATA)");
        break;
    case ERROR_PIPE_CONNECTED:
        printf_err_and_log_APPEND(" (PIPE_CONNECTED)");
        break;
    case ERROR_PIPE_NOT_CONNECTED:
        printf_err_and_log_APPEND(" (PIPE_NOT_CONNECTED)");
        break;
    case ERROR_BROKEN_PIPE:
        printf_err_and_log_APPEND(" (BROKEN_PIPE)");
        break;
    case ERROR_ALREADY_EXISTS:
        printf_err_and_log_APPEND(" (ALREADY_EXISTS)");
        break;
    default:
        break;
    } // switch
}

void append_dwErr_as_needed_and_LOG(char * sPipeName, char * msg, size_t sizeof_msg, DWORD dwErr)
{
    if (dwErr) {
        char err_msg[50];
        sprintf_s(err_msg, sizeof(err_msg), ", error %lu", dwErr);
        printf_err_and_log_APPEND(err_msg);
        append_interpreted_error(msg, sizeof_msg, dwErr, sPipeName);
    }
    printf_err_and_log_APPEND("\n");
    log_printf(msg);
}

void printf_err_and_LOG(char * sPipeName, char * fmt, int which_mcdu, DWORD dwErr, char * suffix)
{
    char msg[200];
    size_t sizeof_msg = sizeof(msg);
    // fmt should contain "... %d ..., error %lu"
    sprintf_s(msg, sizeof_msg, fmt, which_mcdu, dwErr);
    size_t nb_in_msg = strlen(msg);
    char * msg_end = msg + nb_in_msg;

    append_interpreted_error(msg_end, sizeof(msg)-nb_in_msg, dwErr, sPipeName);

    if (suffix) { printf_err_and_log_APPEND(suffix); }
    log_printf(msg);
}

// for C++ named-pipe server for IPC (CppNamedPipeServer)
// see http://code.msdn.microsoft.com/CppNamedPipeServer-d1778534
// for C++ named-pipe client for IPC (CppNamedPipeClient)
// see http://code.msdn.microsoft.com/CppNamedPipeClient-a88eb958
// for ConvertStringSecurityDescriptorToSecurityDescriptor
// see http://msdn.microsoft.com/en-us/library/aa376401(v=vs.85).aspx

// Create and initialize a new SECURITY_ATTRIBUTES structure to allow
// Authenticated Users read and write access to a pipe, and to allow
// the Administrators group full access to the pipe.
// Remember to call FreePipeSecurity to get rid of it when done!
BOOL CreatePipeSecurity(PSECURITY_ATTRIBUTES *ppSa)
{
    BOOL fSucceeded = TRUE;
    DWORD dwErr = ERROR_SUCCESS;

    PSECURITY_DESCRIPTOR pSd = NULL;
    PSECURITY_ATTRIBUTES pSA = NULL;

    // Define the SDDL for the security descriptor.
    PCWSTR szSDDL = L"D:"       // Discretionary ACL
      //L"(A;OICI;GRGW;;;AU)"   // Allow read/write to authenticated users
      //L"(A;OICI;GA;;;BA)"     // Allow full control to administrators
        L"(A;;GA;;;AU)"         // Allow full control to authenticated users
        L"(A;;GA;;;BA)"         // Allow full control to administrators
        ;

    if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSDDL,
        SDDL_REVISION_1, &pSd, NULL)) {

        fSucceeded = FALSE;
        dwErr = GetLastError();
        goto Cleanup;
    }

    // Allocate the memory of SECURITY_ATTRIBUTES.
    pSA = (PSECURITY_ATTRIBUTES)LocalAlloc(LPTR, sizeof(*pSA));
    if (!pSA) {
        fSucceeded = FALSE;
        dwErr = GetLastError();
        goto Cleanup;
    }

    pSA->nLength = sizeof(*pSA);
    pSA->lpSecurityDescriptor = pSd;
    pSA->bInheritHandle = FALSE;

    *ppSa = pSA;

Cleanup:
    // Clean up the allocated resources if something is wrong.
    if (!fSucceeded) {
        if (pSd) {
            LocalFree(pSd);
            pSd = NULL;
        }
        if (pSA) {
            LocalFree(pSA);
            pSA = NULL;
        }

        SetLastError(dwErr); // so caller of CreatePipeSecurity can get error using GetLastError()
    }

    return fSucceeded;
}

// Free the SECURITY_ATTRIBUTES structure created by CreatePipeSecurity
void FreePipeSecurity(PSECURITY_ATTRIBUTES pSA)
{
    if (pSA) {
        if (pSA->lpSecurityDescriptor) {
            LocalFree(pSA->lpSecurityDescriptor);
        }
        LocalFree(pSA);
        pSA = NULL;
    }
}

//#if WANT_SHOW_INTERESTING_TX || WANT_SHOW_INTERESTING_RX
bool WordIsInteresting(unsigned long value)
{
    value &= 0x7FFFFFFF; /* discard parity bit */

    if (WANT_TRACE_HEARTBEAT) {
    BYTE label = (BYTE)(value&0xFF);
    if (label==0172/*Subsystem Address Label*/ || label==0377/*MCDU Identifier*/) {
        return true;
    }
    } //WANT_TRACE_HEARTBEAT

    BYTE top_byte = (BYTE) ((value>>24)&0xFF);
    if (WANT_SHOW_INTERESTING_NON_DATA) {
    if (top_byte<0x20 /* not printable character -- not data */) {
      return true;
    }
    } //WANT_SHOW_INTERESTING_NON_DATA

    if (WANT_SHOW_INTERESTING_DC1) {
    if ((value&0xFF7F0000) == 0x11180000/*log off (MCDU to DI)*/) {
        return true;
    }
    } //WANT_SHOW_INTERESTING_DC1

    if (WANT_SHOW_INTERESTING_ENQ && top_byte==0x05/*ENQ*/           ) return true;
    if (WANT_SHOW_INTERESTING_ACK && top_byte==0x06/*ACK*/           ) return true;
    if (WANT_SHOW_INTERESTING_RTS && top_byte==0x12/*RTS*/           ) return true;
    if (WANT_SHOW_INTERESTING_CTS && top_byte==0x13/*CTS*/           ) return true;
    if (WANT_SHOW_INTERESTING_CLR && top_byte==0x14/*CLR*/           ) return true;
    if (WANT_SHOW_INTERESTING_DC1 && top_byte==0x11/*DC1=key/button*/) return true;

    return false;
}
//#endif //WANT_SHOW_INTERESTING_TX || WANT_SHOW_INTERESTING_RX

////////////////////////////////////////////////////////////

// A wrapper for WaitForSingleObject
// Uses MsgWaitForMultipleObjects instead and handles the system events internally

DWORD MyWait(HANDLE hHandle, DWORD dwMilliseconds)
{
    MSecTime t1000start;            // current time in msecs since 01-Jan-1970

    if (!hHandle) {
        return 0;
    }

again:
    SetLastError(0L);
    //DWORD dwWait = ::WaitForSingleObject(hHandle, dwMilliseconds);
    DWORD dwWait = ::MsgWaitForMultipleObjects(1, &hHandle, /*bWaitAll*/FALSE, dwMilliseconds, QS_ALLINPUT);

    switch (dwWait) {

    case WAIT_OBJECT_0+1:           // system event requires attention
        // standard message pump (don't worry about accelerators)
        MSG             msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        break;

    case WAIT_OBJECT_0:             // the object was signalled
    case WAIT_TIMEOUT:              // timeout expired
    case WAIT_ABANDONED:            // abandoned: app is shutting down and the object handle is invalid
        return dwWait;

    default:                        // something else: something strange
        {
        char szWork[100];
        sprintf(szWork, "MyWait, MsgWaitForMultipleObjects returned %d", dwWait);
        //if (dwWait>=WAIT_ABANDONED_0) {                   // 0x80
        //  DWORD nWhichHandle = dwWait-WAIT_ABANDONED_0;   // which handle was abandoned
        //  do {} while (0);                                // better not be anything except 0!
        //}

        ::OutputDebugStringA(szWork);
        ::MessageBoxA(NULL, szWork, "(Debug) unexpected result in MyWait", MB_ICONINFORMATION);
        ::DebugBreak();

        }
        return dwWait;
    }

    if (dwMilliseconds==INFINITE) { /* waiting forever */
        goto again;
    }

    {   /* see if it is time to go */
        MSecTime t1000now;
        unsigned long ms_diff = t1000now.Interval(t1000start);
        if (ms_diff>=dwMilliseconds+20L) {  /* +20 to allow for extra processing time */
            /* the handle wasn't signalled, timeout */
            return WAIT_TIMEOUT;
        }
        /* we already slept ms_diff, how much is left? */
        if (ms_diff<=dwMilliseconds) {
            dwMilliseconds -= ms_diff;
        } // else ms_diff>dwMilliseconds and ms_diff<dwMilliseconds+20L -- wait a bit more to be safe
    }
    goto again;

    return dwWait;
}

////////////////////////////////////////////////////////////

//#if WANT_CIRC_BUFFER_TRACE

typedef struct {
    int             mcdu_index;
    char *          hint;   // must be static string
    unsigned long   data;
    MSecTime        t1000;  // current time in msecs since 01-Jan-1970
} Circular_Trace_t;

#define                     Circular_TRACE_SIZE 1000
static Circular_Trace_t     gCNamedPipe_Trace[Circular_TRACE_SIZE];
static unsigned int         gCNamedPipe_Trace_Next_Write = 0;
static unsigned int         gCNamedPipe_Trace_Count      = 0;

void Circular_Trace_Dump(void)
{
    int index, count;
    if (gCNamedPipe_Trace_Count<Circular_TRACE_SIZE) {
        index = (Circular_TRACE_SIZE + gCNamedPipe_Trace_Next_Write - gCNamedPipe_Trace_Count) %
                                                                            Circular_TRACE_SIZE;
        count = gCNamedPipe_Trace_Count;
    } else {
        index = gCNamedPipe_Trace_Next_Write;
        count = Circular_TRACE_SIZE;
    }

    if (count) {
        if (count>1) {
            log_printf("-> CNamedPipe Trace (latest %d)\n", count);
        }

        while (count) {

            char buffer[200];
            sprintf_s(buffer, sizeof(buffer), "Circ: %12s #%d %08X at ",
                gCNamedPipe_Trace[index].hint,
                gCNamedPipe_Trace[index].mcdu_index+1,
                gCNamedPipe_Trace[index].data);
            MSecTime::DateTimeStampFromTimeT1000(
                gCNamedPipe_Trace[index].t1000.ToTimeT1000(), buffer+strlen(buffer));
            #if WINVER >= 0x0500
            strcat_s(buffer, sizeof(buffer), "\n");
            #else
            strcat  (buffer,                 "\n");
            #endif
            log_printf(buffer);

            index = (index + 1) % Circular_TRACE_SIZE;
            -- count;
        }

        gCNamedPipe_Trace_Count = 0;
    }
}

void Circular_Trace(int mcdu_index, unsigned long data, char * hint)
{
    int write_here = gCNamedPipe_Trace_Next_Write;
    gCNamedPipe_Trace_Next_Write = (gCNamedPipe_Trace_Next_Write + 1) % Circular_TRACE_SIZE;
    ++ gCNamedPipe_Trace_Count;

    gCNamedPipe_Trace[write_here].mcdu_index = mcdu_index;
    gCNamedPipe_Trace[write_here].data       = data;
    gCNamedPipe_Trace[write_here].hint       = hint;    // points to static string
    gCNamedPipe_Trace[write_here].t1000      .MarkNow();
}

//#endif //WANT_CIRC_BUFFER_TRACE

////////////////////////////////////////////////////////////

unsigned int WINAPI PipeWriterThread(LPVOID lpvParam);
unsigned int WINAPI PipeReaderThread(LPVOID lpvParam);

CNamedPipe::CNamedPipe(int which_mcdu/*0,1,2,3*/, char * s_DI_Host, CTransportPipe * parent) :
        m_RxCallback_function(NULL), m_RxCallback_class(NULL),
        m_Pipe_Queue_Next_Write(0), m_Pipe_Queue_Next_Read(0)
        //#if WANT_PIPE_STATISTICS
        , m_Pipe_Queue_High_Water_Mark(0)
        //#endif //WANT_PIPE_STATISTICS
        , m_bAcceptingRx(false)
        , m_parent(parent)
{
    m_which_mcdu = which_mcdu;
    if (!s_DI_Host || !*s_DI_Host) s_DI_Host = ".";
    #if WINVER >= 0x0500
    strncpy_s(m_DI_Host, sizeof(m_DI_Host), s_DI_Host, _TRUNCATE);
    #else
    strncpy  (m_DI_Host,                    s_DI_Host           );
    #endif

    m_PipeStatus = 0;
    m_bQuittingTime = FALSE;
    m_hWriterThread = NULL;
    m_hReaderThread = NULL;
    m_nWriterThreadId = 0;
    m_nReaderThreadId = 0;
    m_WriterEvent = NULL;
    m_SendWordEvent = NULL;
    m_bAllow_Write = false;
    m_bAllow_Read = false;

    Set_ReaderFail_Callback(NULL, NULL);
    Set_WriterFail_Callback(NULL, NULL);

    // Create an auto-reset event object.
    SetLastError(0);
    m_WriterEvent = CreateEvent(NULL,    // default security attributes
                               FALSE,   // auto-reset event
                               FALSE,   // initial state is nonsignaled
                               NULL);   // no name
    DWORD dwErr = GetLastError();

    if (!m_WriterEvent) {
        if (DEBUG_THIS_MCDU()) {
            log_printf("-> #%d: CreateEvent failed, error %lu\n",
                    which_mcdu+1, dwErr);
        }
        goto bail;
    }

    // Create an auto-reset event object.
    SetLastError(0);
    m_SendWordEvent = CreateEvent(NULL,  // default security attributes
                                 FALSE, // auto-reset event
                                 TRUE,  // initial state is signaled
                                 NULL); // no name
    dwErr = GetLastError();

    if (!m_SendWordEvent) {
        if (DEBUG_THIS_MCDU()) {
            log_printf("-> #%d: CreateEvent failed, error %lu\n",
                    which_mcdu+1, dwErr);
        }
        goto bail;
    }

    /* BG reader thread: handles incoming data */

    SetLastError(0);
    m_hReaderThread = (HANDLE) _beginthreadex(NULL, 0, PipeReaderThread, this, 0, &m_nReaderThreadId);
    dwErr = GetLastError();

    if (WANT_DEBUG_VERBOSE) {
    if (DEBUG_THIS_MCDU()) {
        char msg[200];
        sprintf_s(msg, sizeof(msg), "-> #%d: _beginthreadex(Reader) => h=0x%llX tid=%u",
                        which_mcdu+1, (ULONGLONG)m_hReaderThread, m_nReaderThreadId);
        append_dwErr_as_needed_and_LOG(NULL, msg, sizeof(msg), dwErr);
    }
    } //WANT_DEBUG_VERBOSE

    if (!m_hReaderThread) {
        if (!WANT_DEBUG_VERBOSE) {
        if (DEBUG_THIS_MCDU()) {
            log_printf("-> #%d: CreateThread (Reader) failed, error %lu\n",
                        which_mcdu+1, dwErr);
        }
        } //!WANT_DEBUG_VERBOSE
        goto bail;
    }

    /* BG writer thread: handles outgoing data */

    SetLastError(0);
    m_hWriterThread = (HANDLE) _beginthreadex(NULL, 0, PipeWriterThread, this, 0, &m_nWriterThreadId);
    dwErr = GetLastError();

    if (WANT_DEBUG_VERBOSE) {
    if (DEBUG_THIS_MCDU()) {
        char msg[200];
        sprintf_s(msg, sizeof(msg), "-> #%d: _beginthreadex(Writer) => h=0x%llX tid=%u",
                        which_mcdu+1, (ULONGLONG)m_hWriterThread, m_nWriterThreadId);
        append_dwErr_as_needed_and_LOG(NULL, msg, sizeof(msg), dwErr);
    }
    } //WANT_DEBUG_VERBOSE

    if (!m_hWriterThread) {
        if (!WANT_DEBUG_VERBOSE) {
        if (DEBUG_THIS_MCDU()) {
            log_printf("-> #%d: CreateThread (Writer) failed, error %lu\n",
                        which_mcdu+1, dwErr);
        }
        } //!WANT_DEBUG_VERBOSE
        goto bail;
    }

    // give BG threads time to get started
    Sleep(10);

    // success
    m_PipeStatus = 1;
    return;

    // failure
bail:
    return;
}

void CNamedPipe::SetRxCallback(void * pThis, ONRX_HELPER_FUNCTION function) {
    m_RxCallback_class = pThis;
    m_RxCallback_function = function;
}

void CNamedPipe::Call_OnRx(long wordRx) {
    if (m_RxCallback_function) {
        (*m_RxCallback_function)(m_RxCallback_class, wordRx, MCDU());
    }
}

void CNamedPipe::Set_ReaderFail_Callback(void * pThis, ONREADERFAIL_HELPER_FUNCTION function) {
    m_ReaderFail_Callback_class = pThis;
    m_ReaderFail_Callback_function = function;
}

void CNamedPipe::Call_OnReaderFail() {
    if (m_ReaderFail_Callback_function) {
        (*m_ReaderFail_Callback_function)(m_ReaderFail_Callback_class, MCDU());
    }
}

void CNamedPipe::Set_WriterFail_Callback(void * pThis, ONWRITERFAIL_HELPER_FUNCTION function) {
    m_WriterFail_Callback_class = pThis;
    m_WriterFail_Callback_function = function;
}

void CNamedPipe::Call_OnWriterFail() {
    if (m_WriterFail_Callback_function) {
        (*m_WriterFail_Callback_function)(m_WriterFail_Callback_class, MCDU());
    }
}

CNamedPipe::~CNamedPipe()
{
    if (WANT_DEBUG_SHUTDOWN) {
    char szWork[100];
    sprintf_s(szWork, sizeof(szWork), "~CNamedPipe %p for MCDU#%d\n", this, THIS_MCDU_ID());
    log_printf(szWork);
    } //WANT_DEBUG_SHUTDOWN

    // if either thread exists, tell them to quit
    if (m_hWriterThread || m_hReaderThread) {
        MSecTime t1000start;            // current time in msecs since 01-Jan-1970

        if (WANT_DEBUG_SHUTDOWN) {
        // tell both the reader and writer threads to shut down
        DWORD dw0_Writer = ::WaitForSingleObject(m_hWriterThread, 0/*msec*/);
        DWORD dw0_Reader = ::WaitForSingleObject(m_hReaderThread, 0/*msec*/);
        // status can be be WAIT_TIMEOUT(0x102=258.) if they are still running...
        char szWork[100];
        sprintf_s(szWork, sizeof(szWork), "~CNamedPipe %p): BG r %d(%d) w %d(%d)\n",
            this, m_nReaderThreadId, dw0_Reader, m_nWriterThreadId, dw0_Writer);
        log_printf(szWork);
        } //WANT_DEBUG_SHUTDOWN

        // yet BG threads know it is time to quit (if they are awake)
        SetQuittingTime();

        // looks extreme, but fairly safe: the BG threads don't own any resources
        if (m_hReaderThread) {
            TerminateThread(m_hReaderThread, 999);
        }
        if (m_hWriterThread) {
            TerminateThread(m_hWriterThread, 999);
        }

        // Sleep to allow them both a chance to wake up and notice they are being killed
        Sleep(10);  // allow BG reader and writer threads to be killed

        // Wait around for a short while to make sure both threads have stopped
        for (int nLoops=0; nLoops<(500/20); nLoops++) { /* 1/2 sec max */

            if (!m_hReaderThread && !m_hWriterThread)
                break;  /* OK, the threads are gone */

            HANDLE  h[2];
            h[0] = m_hReaderThread;
            h[1] = m_hWriterThread;

            DWORD dwReaderErrorCode = 0;
            DWORD dwReaderExitCode = 0;
            DWORD dwWriterErrorCode = 0;
            DWORD dwWriterExitCode = 0;

            DWORD dwStatus = WaitForMultipleObjects(2, h, /*fWaitAll*/true, 0/*don't wait*/);
            //log_printf("-> #%d: Wait on Threads, Status %lu\n", which_mcdu+1, dwStatus);
            if (dwStatus == WAIT_FAILED)
                break;  /* OK, both threads are gone (or at least one of them is) */

            // make sure both BG threads have finished

            if (!GetExitCodeThread(m_hReaderThread, &dwReaderExitCode))
                dwReaderErrorCode = GetLastError();
            if (!GetExitCodeThread(m_hWriterThread, &dwWriterExitCode))
                dwWriterErrorCode = GetLastError();

            if (dwReaderExitCode!=STILL_ACTIVE && dwWriterExitCode!=STILL_ACTIVE)
                break;  /* OK, both threads are gone (or something strange happened) */
            /* otherwise at least one of the threads is still active */

            if (WANT_DEBUG_SHUTDOWN) {
            char szWork[100];
            sprintf_s(szWork, sizeof(szWork), "~CNamedPipe %p): BG r %lu(err=%lu), w %lu(err=%lu), wait\n",
                            this, dwReaderExitCode, dwReaderErrorCode, dwWriterExitCode, dwWriterErrorCode);
            log_printf(szWork);
            } //WANT_DEBUG_SHUTDOWN

            Sleep(20);  // wait for background reader and writer thread(s) to wake up and notice it is time to quit
            continue;
        } /*for*/

        if (WANT_DEBUG_SHUTDOWN) {
        MSecTime t1000now;
        unsigned long ms_diff = t1000now.Interval(t1000start);
        if (ms_diff>50L) {
            log_printf("MCDU#%d) Kill BG threads took %lu ms\n", THIS_MCDU_ID(), ms_diff);
        }
        } //WANT_DEBUG_SHUTDOWN

    } // else both threads already long gone

    m_hWriterThread = NULL;
    m_hReaderThread = NULL;

    // clean up
    if (m_WriterEvent) {
        CloseHandle(m_WriterEvent); m_WriterEvent = NULL;
    }
    if (m_SendWordEvent) {
        CloseHandle(m_SendWordEvent); m_SendWordEvent = NULL;
    }

    if (WANT_DEBUG_SHUTDOWN) {
    char szWork[100];
    sprintf_s(szWork, sizeof(szWork), "~CNamedPipe %p for MCDU#%d done.\n", this, THIS_MCDU_ID());
    log_printf(szWork);
    } //WANT_DEBUG_SHUTDOWN
}

void CreatePipeName_Helper(char * buffer, size_t buffer_size, char * host_name, char * end_name, int which_mcdu)
{
    sprintf_s(buffer, buffer_size, "\\\\%s\\pipe\\%s_%s#%d", host_name, PIPE_NAME_BASE, end_name, which_mcdu+1);
}

HANDLE CNamedPipe::CreateReaderEndOfPipe(char * host_name, char * end_name) // used by BG Reader thread
{
    HANDLE hPipe = (HANDLE)INVALID_HANDLE_VALUE;

    PSECURITY_ATTRIBUTES pSA = NULL;
    if (!CreatePipeSecurity(&pSA)) {
        DWORD dwErr = GetLastError();
        if (DEBUG_THIS_MCDU()) {
            log_printf("-> #%d: CreatePipeSecurity, failed error %lu\n", THIS_MCDU_ID(), dwErr);
        }
        return (HANDLE)INVALID_HANDLE_VALUE;
    }

    // construct (server) name of pipe for the BG Reader thread
    CreatePipeName_Helper(m_sPipeName_Server, sizeof(m_sPipeName_Server), host_name, end_name, MCDU());
    {
    // convert text to wide
    size_t nbMoved;
    mbstowcs_s(&nbMoved, m_sPipeName_Server_Wide, sizeof(m_sPipeName_Server_Wide)/sizeof(wchar_t),
                         m_sPipeName_Server, _TRUNCATE);
    #define server_pipe_name_in_reader m_sPipeName_Server_Wide
    }

    SetLastError(0L);
    hPipe = CreateNamedPipe(server_pipe_name_in_reader /*m_sPipeName_Server or m_sPipeName_Server_Wide*/,
            /*dwOpenMode*/  PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
            /*dwPipeMode*/  PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // or PIPE_NOWAIT
            /*nMaxInstances*/   PIPE_UNLIMITED_INSTANCES,
            /*nOutBufferSize*/  200,
            /*nInBufferSize*/   200,
            /*nDefaultTimeOut*/ 1000 /*msec*/, // or NMPWAIT_USE_DEFAULT_WAIT
            /*lpSecurityAttributes*/ pSA);
    DWORD dwErr = GetLastError();

    if(WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_THIS_MCDU()) {
        char msg[200];
        sprintf_s(msg, sizeof(msg), "-> #%d Reader: CreateNP(\"%s\") => h=0x%llX",
                        THIS_MCDU_ID(), m_sPipeName_Server, (ULONGLONG)hPipe);
        append_dwErr_as_needed_and_LOG(m_sPipeName_Server, msg, sizeof(msg), dwErr);
    }
    } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA

    FreePipeSecurity(pSA);

    if (hPipe==INVALID_HANDLE_VALUE) {
        if (!(WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN)) {
        if (DEBUG_THIS_MCDU()) {
            printf_err_and_LOG(m_sPipeName_Server, "-> #%d Reader: CreateNP failed, error %lu",
                        THIS_MCDU_ID(), dwErr, "\n");
        }
        } //!(WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN)
    }

    return hPipe;
}

bool CNamedPipe::SetupReaderEndOfPipe(HANDLE hPipeReader) // used by BG Reader thread
{
    if (WANT_DEBUG_READER || WANT_DEBUG_STARTUP) {
    if (DEBUG_THIS_MCDU()) {
        log_printf("-> #%d Reader: Waiting for connect ...\n",
                        THIS_MCDU_ID());
    }
    } //WANT_DEBUG_READER || WANT_DEBUG_STARTUP

    SetLastError(0L);
    BOOL bConnectedOK = ConnectNamedPipe(hPipeReader, NULL);
    DWORD dwErr = GetLastError();

    if (!bConnectedOK) {
        if (dwErr!=ERROR_PIPE_CONNECTED /* && dwErr!=ERROR_PIPE_LISTENING*/) {
            if (WANT_DEBUG_STARTUP) {
            if (DEBUG_THIS_MCDU()) {
                log_printf("-> #%d Reader: ConnectNP() failed, error %lu\n",
                        THIS_MCDU_ID(), dwErr);
            }
            } //WANT_DEBUG_STARTUP
            return false;
        }
        // else the writer thread was fast: connected after the Create and before the Connect
    } // else connected OK

    if (WANT_DEBUG_READER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_THIS_MCDU()) {
        char msg[200];
        sprintf_s(msg, sizeof(msg), "-> #%d Reader: ConnectNP(h=0x%llX)",
                        THIS_MCDU_ID(), (ULONGLONG)hPipeReader);
        append_dwErr_as_needed_and_LOG(m_sPipeName_Server, msg, sizeof(msg), dwErr);
    }
    } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA

    // OK, pipe is connected, BG Reader thread can now accept incoming data
    Allow_Read();
    SetAcceptingRx(true);
    if ((WANT_DEBUG_READER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE) {
    if (DEBUG_THIS_MCDU()) {
        log_printf("-> #%d Allow_Read\n", THIS_MCDU_ID());
    }
    } //(WANT_DEBUG_READER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE

    return true;
}

DWORD CNamedPipe::SetupWriterEndOfPipe(char * host_name, char * end_name) // used by BG Writer thread
{
    MSecTime t1000start;            // current time in msecs since 01-Jan-1970

    // construct (client) name of pipe for the BG Writer thread
    CreatePipeName_Helper(m_sPipeName_Client, sizeof(m_sPipeName_Client), host_name, end_name, MCDU());
    {
    // convert text to wide
    size_t nbMoved;
    mbstowcs_s(&nbMoved, m_sPipeName_Client_Wide, sizeof(m_sPipeName_Client_Wide)/sizeof(wchar_t),
                         m_sPipeName_Client, _TRUNCATE);
    #define client_pipe_name_in_writer m_sPipeName_Client_Wide
    }

    DWORD dwErr = 0L;

    // try to connect to server end of pipe (created by the other app BG Reader thread)
    for(;;) {

        SetLastError(0L);
        // wait until server starts up and creates the pipe
        BOOL bWaitOK = WaitNamedPipe(client_pipe_name_in_writer /*m_sPipeName_Client or m_sPipeName_Client*/,
                            NMPWAIT_NOWAIT); // or NMPWAIT_USE_DEFAULT_WAIT or NMPWAIT_WAIT_FOREVER
        dwErr = GetLastError();

        if (!bWaitOK) {
            if (WANT_DEBUG_STARTUP) {
            static DWORD old_error = 0L;
            static int time_for_a_complaint = 0;
            if (old_error != dwErr || ++time_for_a_complaint > 50 /* every 5 seconds */) {
                old_error = dwErr;

                if (DEBUG_THIS_MCDU()) {
                    char msg[200];
                    sprintf_s(msg, sizeof(msg), "-> #%d Writer: WaitNP(\"%s\")",
                                    THIS_MCDU_ID(), m_sPipeName_Client);
                    append_dwErr_as_needed_and_LOG(m_sPipeName_Client, msg, sizeof(msg), dwErr);
                }
                time_for_a_complaint = 0;
            }
            } //WANT_DEBUG_STARTUP

            Sleep(100L);
            continue;
        } // if !WaitNamedPipe
        // else WaitNamedPipe() succeeded
        break;

    } // forever, or until found the server end of the pipe

    MSecTime t1000now;
    unsigned long ms_diff = t1000now.Interval(t1000start);

    if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_THIS_MCDU()) {
        char msg[200];
        sprintf_s(msg, sizeof(msg), "-> #%d Writer: WaitNP(\"%s\"), OK after %lu ms",
                    THIS_MCDU_ID(), m_sPipeName_Client, ms_diff);
        append_dwErr_as_needed_and_LOG(m_sPipeName_Client, msg, sizeof(msg), dwErr);
    }
    } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA

    return (DWORD)ms_diff;  // hint to caller how long it took
}

HANDLE CNamedPipe::OpenWriterEndOfPipe(void) // used by BG Writer thread
{
    HANDLE hPipe;
    MSecTime t1000start;            // current time in msecs since 01-Jan-1970
    unsigned long ms_diff = 0L;

    // open the connection to the server end of pipe (in the other app)
    for (;;) {

        SetLastError(0L);
        hPipe = CreateFile(client_pipe_name_in_writer /*m_sPipeName_Client or m_sPipeName_Client_Wide*/,
                    /*dwDesiredAccess*/  (GENERIC_READ | GENERIC_WRITE),
                    /*dwShareMode*/ (FILE_SHARE_READ | FILE_SHARE_WRITE),
                    /*lpSecurityAttributes*/ NULL,
                    /*dwCreationDisposition*/OPEN_EXISTING,
                    /*dwFlagsAndAttributes*/ FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                    /*hTemplateFile*/ NULL);
        DWORD dwErr = GetLastError();

        if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA || WANT_DEBUG_SHUTDOWN) {
        if (DEBUG_THIS_MCDU()) {
            char msg[200];
            sprintf_s(msg, sizeof(msg), "-> #%d Writer: CreateF(\"%s\") => h=0x%llX",
                        THIS_MCDU_ID(), m_sPipeName_Client, (ULONGLONG)hPipe);
            append_dwErr_as_needed_and_LOG(m_sPipeName_Client, msg, sizeof(msg), dwErr);
        }
        } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA

        // this should have worked, since we already did WaitNamedPipe on this pipe
        if (hPipe==INVALID_HANDLE_VALUE) {
            if (dwErr==ERROR_FILE_NOT_FOUND) {
                DWORD sleep_for       ;
                DWORD sleep_for_limit ;
                if (WANT_DEBUG_STARTUP) {
                    sleep_for       = 1000L;
                    sleep_for_limit = 5000L;
                } else { // not debugging, try more often
                    sleep_for       =  100L;
                    sleep_for_limit = 3000L;
                } //WANT_DEBUG_STARTUP

                Sleep(sleep_for);

                MSecTime t1000now;
                ms_diff = t1000now.Interval(t1000start);
                if (ms_diff < sleep_for_limit) {
                    continue;
                }
                // else it is taking too long, complain
            }
            // else some other error besides FILE_NOT_FOUND

            if (WANT_DEBUG_STARTUP) {
            if (DEBUG_THIS_MCDU()) {
                switch (dwErr) {
                case ERROR_FILE_NOT_FOUND:
                    log_printf("-> #%d Writer: No pipe -- is BG reader running?\n", THIS_MCDU_ID());
                    break;
                case ERROR_PIPE_BUSY:
                    log_printf("-> #%d Writer: No pipe -- BG reader is busy!\n", THIS_MCDU_ID());
                    break;
                default:
                    printf_err_and_LOG(m_sPipeName_Client, "-> #%d Writer: Create failed, error %lu", THIS_MCDU_ID(), dwErr, "\n");
                    break;
                } //switch
            }
            } //WANT_DEBUG_STARTUP

            return (HANDLE)NULL;
        }
        break; // have a good handle, use it
    } // forever, or until created the client end of the pipe

    // OK, pipe is connected, BG Writer thread can now send outgoing data
    Allow_Write();
    if ((WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE) {
    if (DEBUG_THIS_MCDU()) {
        log_printf("-> #%d Allow_Write\n", THIS_MCDU_ID());
    }
    } //(WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE

    return hPipe;
}

// This BG Writer thread wakes up whenevr something needs to be written, then writes it out and goes back to sleep.
unsigned int WINAPI PipeWriterThread(LPVOID lpvParam)
{
    CNamedPipe * pipe = (CNamedPipe *)lpvParam;
    HANDLE  hWriterEndOfPipe = INVALID_HANDLE_VALUE;  // created by CreateFile(), different from hPipe_Server
    DWORD dwOutgoingMessageCount = 0L;
    DWORD dwOutgoingMessageBytes = 0L;
    // outgoing messages can be this big
    DWORD cbBytesToWrite = 0L, cbBytesWritten = 0L;
    BOOL fSuccess = FALSE;
    // handle write errors, followed by recovery
    BOOL bWriteWordPending = FALSE; // TRUE means pulled a word from the queue, write it first
    long txWordPending;             // the value of the pending word to write
    // normally sleep for a long time, shorten when just finished emptying queue
    DWORD sleep_for = 10000L;

    char thread_name[10];
    sprintf_s(thread_name, sizeof(thread_name), "PipeW#%d", MCDU_ID(pipe));
    SetThreadName(GetCurrentThreadId(),thread_name);

start_again:

    if (WANT_DEBUG_WRITER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Writer: starting\n", MCDU_ID(pipe));
    }
    } //WANT_DEBUG_WRITER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN

    // create client end of the pipe, may be on another machine from where the server end is
    for(;;) {

        // are we shutting down?
        if (pipe->QuittingTime())
            goto writer_bail;

        // wait for a server to create the pipe
        DWORD waiting_to_connect_msec = pipe->SetupWriterEndOfPipe(pipe->DI_Host(),our_ident);

        // OK, there is a server pipe available
        hWriterEndOfPipe = pipe->OpenWriterEndOfPipe();
        if (!hWriterEndOfPipe)
            continue;   // OOPS! the other end wasn't available as expected: try again

        // client end of pipe has been created
        if (WANT_DEBUG_WRITER || WANT_SHOW_ALL_TX_DATA || WANT_DEBUG_STARTUP) {
        if (waiting_to_connect_msec>0) {
            if (DEBUG_MCDU(pipe)) {
                log_printf("-> #%d Writer: Opened pipe\n", MCDU_ID(pipe));
            }
        }
        } //WANT_DEBUG_WRITER || WANT_SHOW_ALL_TX_DATA || WANT_DEBUG_STARTUP

        break;
    } // forever

    if (pipe->Is_Read_Allowed() /*&& pipe->Is_Write_Allowed()*/) {
        pipe->Transport()->Set_Transport_Started(); /* the named pipe is ready for use in both directions */
    }

    if ((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Writer: main loop\n", MCDU_ID(pipe));
    }
    } //(WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE

    // Loop until quitting time, or an error occurs (usually when other end of pipe is no longer connected)
    for (;;) {

        // are we shutting down?
        if (pipe->QuittingTime())
            break;

        if (WANT_CIRC_BUFFER_TRACE) {
        Circular_Trace_Dump();
        } //WANT_CIRC_BUFFER_TRACE

        // last chance before sleeping -- anything new to do?
        if (pipe->Pipe_Queue_Empty() && !bWriteWordPending) {

            // queue is empty (and no word is pending), go to sleep
            if (WANT_DEBUG_SIGNALS) {
            if (DEBUG_MCDU(pipe)) {
                log_printf("-> #%d Writer: queue empty, block on WriterEv\n", MCDU_ID(pipe));
            }
            } //WANT_DEBUG_SIGNALS

            MSecTime t1000start;            // current time in msecs since 01-Jan-1970
            SetLastError(0L);
            DWORD dwStatus = MyWait(pipe->WakeupWriterEvent(), sleep_for);
            DWORD dwErr = GetLastError();

            if (WANT_DEBUG_SIGNALS) {
            if (DEBUG_MCDU(pipe)) {
                MSecTime t1000now;
                unsigned long ms_diff = t1000now.Interval(t1000start);
                log_printf("-> #%d Writer: awake (WriterEv) after %lu ms (max sleep %lu)\n", MCDU_ID(pipe), ms_diff, sleep_for);
            }
            } //WANT_DEBUG_SIGNALS
            // normally sleep for a long time, shorten when just finished emptying queue
            sleep_for = 10000L;

            if (WANT_DEBUG_VERBOSE) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                char * interpreted_wait_status = "unknown";
                switch(dwStatus) {
                case WAIT_OBJECT_0:     // the object was signalled
                    interpreted_wait_status = "OBJECT_0"; break;
                case WAIT_TIMEOUT:      // timeout expired, go back to sleep
                    interpreted_wait_status = "TIMEOUT"; break;
                case WAIT_ABANDONED:    // abandoned: app shutting down, object handle invalid
                    interpreted_wait_status = "ABANDONED"; break;
                default:
                    break;
                }
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: Wait() => %lu(%s)",
                            MCDU_ID(pipe), dwStatus, interpreted_wait_status);
                append_dwErr_as_needed_and_LOG(pipe->PipeName_Client(), msg, sizeof(msg), dwErr);
            }
            } //WANT_DEBUG_VERBOSE

            switch (dwStatus) {
            case WAIT_OBJECT_0:     // the object was signalled
                break;
            case WAIT_TIMEOUT:      // timeout expired, go back to sleep
                continue;
            case WAIT_ABANDONED:    // abandoned: app is shutting down and the object handle is invalid
                pipe->SetQuittingTime();
                continue;
            default:
                continue;
            }
        } // else queue wasn't empty

        // woke up because someone (the sendWord function) has stuffed at least one word into the queue,
        // and wants someone (the BG Writer queue) to wake up and actually write it to the pipe

        //#if WANT_PIPE_STATISTICS
        static unsigned int old_pipe_count[4] = {0,0,0,0};
               unsigned int new_pipe_count = pipe->Pipe_Queue_High_Water_Mark();
        if (old_pipe_count[pipe->MCDU()] != new_pipe_count) {
            //if (DEBUG_MCDU(pipe)) {
            //    char msg[200];
            //    sprintf_s(msg, sizeof(msg), "-> #%d Writer: high water %3d now %3d.\n",
            //            MCDU_ID(pipe), old_pipe_count[pipe->MCDU()], new_pipe_count);
            //    log_printf(msg);
            //}
            old_pipe_count[pipe->MCDU()] = new_pipe_count;
        }
        //#endif //WANT_PIPE_STATISTICS

        // make sure we have something to write
        if (pipe->Pipe_Queue_Empty() && !bWriteWordPending) {
            // nothing in queue and no pending word
            if (WANT_DEBUG_SIGNALS) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: queue empty!\n",
                        MCDU_ID(pipe));
                log_printf(msg);
            }
            } //WANT_DEBUG_SIGNALS
            continue;                   // no, woke up for no good reason, go back to sleep
        }

        // flush the queue as quickly as possible by staying awake until the queue is empty
        for(;;) {
            DWORD dwErr;

            long txWord;
            if (bWriteWordPending) {        // already have a word pending?
                txWord = txWordPending;     // yes, send that one first
            } else {                        // nothing pending, get next word from queue
                txWord = pipe->Read_Pipe_Queue();  // want to write this word

                // if SendWord has blocked because the queue is full,
                // wake it up: there is room for one more now
                SetEvent(pipe->WakeupSendWordEvent());
            }

            cbBytesToWrite = sizeof(txWord);

            if ((WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE) || WANT_SHOW_ALL_TX_DATA) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                if (WANT_SHOW_ALL_TX_DATA) {
                sprintf_s(msg, sizeof(msg), "-> Write #%d: 0x%08X%s\n",
                    MCDU_ID(pipe), txWord, bWriteWordPending?" RETRY":"");
                } else { //!WANT_SHOW_ALL_TX_DATA
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: About to Write 0x%X (%d bytes)%s\n",
                            MCDU_ID(pipe), txWord, cbBytesToWrite, bWriteWordPending?" RETRY":"");
                } //!WANT_SHOW_ALL_TX_DATA
                log_printf(msg);
            }
            } //(WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE) || WANT_SHOW_ALL_TX_DATA

            // Write outgoing data to the pipe.

            if (WANT_CIRC_BUFFER_TRACE) {
            Circular_Trace(pipe->MCDU(), (unsigned long)txWord, bWriteWordPending?"Write RETRY":"Write");
            } //WANT_CIRC_BUFFER_TRACE

            bWriteWordPending = FALSE;
            SetLastError(0L);
            fSuccess = WriteFile(hWriterEndOfPipe,  // handle to pipe
                                 &txWord,           // buffer to write from
                                 cbBytesToWrite,    // number of bytes to write
                                 &cbBytesWritten,   // number of bytes actually written
                                 NULL);             // not overlapped I/O
            dwErr = GetLastError();

            if (WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: WriteFile() => %d (%d bytes)",
                                MCDU_ID(pipe), fSuccess, cbBytesWritten);
                append_dwErr_as_needed_and_LOG(pipe->PipeName_Client(), msg, sizeof(msg), dwErr);
            }
            } //WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE

            // are we shutting down?
            if (pipe->QuittingTime()) {
                if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
                if (DEBUG_MCDU(pipe)) {
                    char msg[200];
                    sprintf_s(msg, sizeof(msg), "-> #%d Writer: Quitting",
                                    MCDU_ID(pipe), (ULONGLONG)hWriterEndOfPipe);
                }
                } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA
                break;
            }

            if (!fSuccess || cbBytesToWrite != cbBytesWritten) {
                if (!(WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE)) {
                if (DEBUG_MCDU(pipe)) {
                    printf_err_and_LOG(pipe->PipeName_Client(), "-> #%d Writer: Write failed, error %lu",
                            MCDU_ID(pipe), dwErr, "\n");
                }
                } //!(WANT_DEBUG_WRITER && WANT_DEBUG_VERBOSE)

                // is this a fatal error?
                switch (dwErr) {
                //case ERROR_BAD_NET_NAME:
                //case ERROR_INVALID_NAME:
                //case ERROR_FILE_NOT_FOUND:
                //case ERROR_INVALID_HANDLE:
                //case ERROR_SEM_TIMEOUT:
                //case ERROR_PIPE_CONNECTED:
                case ERROR_PIPE_NOT_CONNECTED:
                //case ERROR_ALREADY_EXISTS:
                case ERROR_NO_DATA:     // MCDU went away, pipe disconnected
                case ERROR_BROKEN_PIPE:
                    if (((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) || WANT_DEBUG_SHUTDOWN) {
                    if (DEBUG_MCDU(pipe)) {
                        log_printf("-> #%d Writer: other end gone\n", MCDU_ID(pipe));
                    }
                    } //((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) || WANT_DEBUG_SHUTDOWN

                    // done with this pipe (but the other end might come back)
                    pipe->Allow_Write(false);

                    // is this a new failure?
                    if (!pipe->Transport()->Has_Transport_Failed()) {
                        pipe->Transport()->Set_Transport_Failed(); /* the writer end of this pipe is no longer useable */
                        if ((WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE) {
                        if (DEBUG_MCDU(pipe)) {
                            log_printf("-> #%d Allow_Write now FALSE\n", MCDU_ID(pipe));
                        }
                        } //(WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE
                    
                        // let the callback function (if any) know that the Writer end of the pipe is down
                        pipe->Call_OnWriterFail();
                    }
                    // else had already failed, no need to callback

                    // clean up
                    SetLastError(0L);
                    CloseHandle(hWriterEndOfPipe);
                    dwErr = GetLastError();

                    if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA || WANT_DEBUG_SHUTDOWN) {
                    if (DEBUG_MCDU(pipe)) {
                        char msg[200];
                        sprintf_s(msg, sizeof(msg), "-> #%d Writer: CloseH(h=0x%llX)",
                                        MCDU_ID(pipe), (ULONGLONG)hWriterEndOfPipe);
                        append_dwErr_as_needed_and_LOG(pipe->PipeName_Client(), msg, sizeof(msg), dwErr);
                    }
                    } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA
                    hWriterEndOfPipe = INVALID_HANDLE_VALUE;

                    // but maybe another will start up
                    bWriteWordPending = TRUE;   // start with the word we just tried to write
                    txWordPending = txWord;
                    goto start_again;

                default:
                    break;
                } // switch

                // some other error we don't know how to handle
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: Unhandled error", MCDU_ID(pipe));
                append_dwErr_as_needed_and_LOG(pipe->PipeName_Client(), msg, sizeof(msg), dwErr);
            }      // end if write problem
            else { // write succeeded
                // keep statistics
                ++ dwOutgoingMessageCount;
                dwOutgoingMessageBytes += cbBytesWritten;
            }

            // if there is no more data waiting in the pipe, flush what we just sent
            if (pipe->Pipe_Queue_Empty())
                break;

            // otherwise there is more data already waiting in the queue, send it now
            if (WANT_DEBUG_SIGNALS) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Writer: wrote 0x%08X, queue still has %d\n",
                        MCDU_ID(pipe), txWord, pipe->Pipe_Queue_Count());
                log_printf(msg);
            }
            } //WANT_DEBUG_SIGNALS

        } // forever (until queue is empty), inner loop

        if (WANT_DEBUG_SIGNALS) {
        if (DEBUG_MCDU(pipe)) {
            log_printf("-> #%d Writer: flush pipe\n", MCDU_ID(pipe));
        }
        } //WANT_DEBUG_SIGNALS

        // Make sure outgoing data is sent through the pipe.
        FlushFileBuffers(hWriterEndOfPipe);

        // normally sleep for a long time, shorten when just finished emptying queue.

        // SendWord may have already queued another word and signalled WakeupWriterEvent,
        // but we're already committed to go to sleep
        sleep_for = 100L;

    } // forever, exits at quitting time or due to an error

writer_bail:

    if (hWriterEndOfPipe!=INVALID_HANDLE_VALUE) {
        CloseHandle(hWriterEndOfPipe);
        hWriterEndOfPipe = INVALID_HANDLE_VALUE;
    }

    if (WANT_DEBUG_WRITER || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Writer: Exit\n", MCDU_ID(pipe));
    }
    } //WANT_DEBUG_WRITER || WANT_DEBUG_SHUTDOWN
    return 0;
}

// This BG Reader thread listens to the pipe, reads whatever arrives, and stuffs data into a queue for later.
unsigned int WINAPI PipeReaderThread(LPVOID lpvParam)
{
    CNamedPipe * pipe = (CNamedPipe *)lpvParam;

    DWORD dwIncomingMessageCount = 0L;
    DWORD dwIncomingMessageBytes = 0L;

    // incoming messages can be this big
    unsigned char buffer[BUFFER_SIZE];
    DWORD cbBytesRead = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipeReader = INVALID_HANDLE_VALUE;

    char thread_name[10];
    sprintf_s(thread_name, sizeof(thread_name), "PipeR#%d", MCDU_ID(pipe));
    SetThreadName(GetCurrentThreadId(),thread_name);

start_again:

    if (WANT_DEBUG_READER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Reader: starting\n", MCDU_ID(pipe));
    }
    } //WANT_DEBUG_READER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN

    // the end of the pipe that does the CreateNamedPipe is always local
    hPipeReader = pipe->CreateReaderEndOfPipe(".", other_ident);
    if (hPipeReader==INVALID_HANDLE_VALUE)
        goto reader_bail;

    // setup a connection so the other end can talk to us
    if (!pipe->SetupReaderEndOfPipe(hPipeReader))
        goto reader_cleanup_and_bail;

    if (/*pipe->Is_Read_Allowed() && */ pipe->Is_Write_Allowed()) {
        pipe->Transport()->Set_Transport_Started(); /* the named pipe is ready for use in both directions */
    }

    if ((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Reader: main loop\n", MCDU_ID(pipe));
    }
    } //(WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE

    // Loop until quitting time, or an error occurs (usually when other end of pipe is no longer connected)
    for (;;) {
        DWORD dwErr;

        // are we shutting down?
        if (pipe->QuittingTime())
            break;

        // let mainline run
        Sleep(0);

        // Read incoming data from the pipe.
        if (WANT_DEBUG_READER && WANT_DEBUG_VERBOSE) {
        if (DEBUG_MCDU(pipe)) {
            char msg[200];
            sprintf_s(msg, sizeof(msg), "-> #%d Reader: About to Read ...\n", MCDU_ID(pipe));
            log_printf(msg);
        }
        } //WANT_DEBUG_READER && WANT_DEBUG_VERBOSE

        SetLastError(0L);
        fSuccess = ReadFile(hPipeReader,    // handle to pipe
                            buffer,         // buffer to receive data
                            BUFFER_SIZE,    // size of buffer
                            &cbBytesRead,   // number of bytes actually read
                            NULL);          // not overlapped I/O
        dwErr = GetLastError();

        // are we shutting down?
        if (pipe->QuittingTime()) {
            if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
            if (DEBUG_MCDU(pipe)) {
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Reader: Quitting",
                                MCDU_ID(pipe), (ULONGLONG)hPipeReader);
            }
            } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA
            break;
        }

        if (WANT_DEBUG_READER && WANT_DEBUG_VERBOSE) {
        if (DEBUG_MCDU(pipe)) {
            char msg[200];
            sprintf_s(msg, sizeof(msg), "-> #%d Reader: ReadFile() => %d (%d bytes)",
                        MCDU_ID(pipe), fSuccess, cbBytesRead);
            append_dwErr_as_needed_and_LOG(pipe->PipeName_Server(), msg, sizeof(msg), dwErr);
        }
        } //WANT_DEBUG_READER && WANT_DEBUG_VERBOSE

        if (!fSuccess || !cbBytesRead) {
            if (!(WANT_DEBUG_READER && WANT_DEBUG_VERBOSE)) {
            if (DEBUG_MCDU(pipe)) {
                printf_err_and_LOG(pipe->PipeName_Server(), "-> #%d Reader: Read failed, error %lu",
                        MCDU_ID(pipe), dwErr, "\n");
            }
            } //!(WANT_DEBUG_READER && WANT_DEBUG_VERBOSE)

            // is this a fatal error?
            switch (dwErr) {
            //case ERROR_BAD_NET_NAME:
            //case ERROR_INVALID_NAME:
            //case ERROR_FILE_NOT_FOUND:
            //case ERROR_INVALID_HANDLE:
            //case ERROR_SEM_TIMEOUT:
            //case ERROR_PIPE_CONNECTED:
            case ERROR_PIPE_NOT_CONNECTED:
            //case ERROR_ALREADY_EXISTS:
            case ERROR_NO_DATA:
            case ERROR_BROKEN_PIPE: // MCDU went away, pipe disconnected
                if (((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) || WANT_DEBUG_SHUTDOWN) {
                if (DEBUG_MCDU(pipe)) {
                    log_printf("-> #%d Reader: other end gone\n", MCDU_ID(pipe));
                }
                } //((WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP) && WANT_DEBUG_VERBOSE) || WANT_DEBUG_SHUTDOWN

                // done with this MCDU
                pipe->Allow_Read(false);
                pipe->SetAcceptingRx(false);

                // is this a new failure?
                if (!pipe->Transport()->Has_Transport_Failed()) {
                    pipe->Transport()->Set_Transport_Failed(); /* the reader end of this pipe is no longer useable */
                    if ((WANT_DEBUG_READER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE) {
                    if (DEBUG_MCDU(pipe)) {
                        log_printf("-> #%d Allow_Read now FALSE\n", MCDU_ID(pipe));
                    }
                    } //(WANT_DEBUG_READER || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) && WANT_DEBUG_VERBOSE

                    // let the callback function (if any) know that the Reader end of the pipe is down
                    pipe->Call_OnReaderFail();
                }
                // else had already failed, no need to callback

                // force client off (in case error was due to something besides client bailing)
                SetLastError(0);
                DisconnectNamedPipe(hPipeReader);
                dwErr = GetLastError();

                if (WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA || WANT_DEBUG_SHUTDOWN) {
                if (DEBUG_MCDU(pipe)) {
                    char msg[200];
                    sprintf_s(msg, sizeof(msg), "-> #%d Reader: DisconnNP(h=0x%llX)",
                                    MCDU_ID(pipe), (ULONGLONG)hPipeReader);
                    append_dwErr_as_needed_and_LOG(pipe->PipeName_Server(), msg, sizeof(msg), dwErr);
                }
                } //WANT_DEBUG_VERBOSE || WANT_DEBUG_STARTUP || WANT_SHOW_ALL_RX_DATA

                // clean up
                CloseHandle(hPipeReader);
                hPipeReader = INVALID_HANDLE_VALUE;

                // but maybe another will start up
                goto start_again;

            default:
                break;
            } // switch

            // some other error we don't know how to handle
            char msg[200];
            sprintf_s(msg, sizeof(msg), "-> #%d Reader: Unhandled error", MCDU_ID(pipe));
            append_dwErr_as_needed_and_LOG(pipe->PipeName_Server(), msg, sizeof(msg), dwErr);
        }      // end if read problem
        else { // read succeeded

            if (cbBytesRead==sizeof(unsigned long)) {
                if (Ignore_This_Rx_Word(*(unsigned long *)buffer)) {
                    continue; // discard this word, go wait for another
                } // else don't want to ignore this word
            } // else strange incoming data, expecting 4 bytes

            // Process the incoming message.
            ++ dwIncomingMessageCount;
            dwIncomingMessageBytes += cbBytesRead;

            if (WANT_DEBUG_READER || WANT_SHOW_ALL_RX_DATA) {
            if (DEBUG_MCDU(pipe)) {
                char msg[100];

            #if WINVER >= 0x0500
                if (WANT_SHOW_ALL_RX_DATA) {
                if (cbBytesRead==sizeof(unsigned long)) {
                    sprintf_s(msg, sizeof(msg), "-> Read #%d: 0x%08X",
                            MCDU_ID(pipe), *(unsigned long *)buffer);
                } else {
                    sprintf_s(msg, sizeof(msg), "-> Read #%d: 0x%08X -- %d bytes!",
                            MCDU_ID(pipe), *(unsigned long *)buffer, cbBytesRead);
                }
                } else { //!WANT_SHOW_ALL_RX_DATA
                sprintf_s(msg, sizeof(msg), "-> #%d Reader: Read pipe, %4u bytes:",
                            MCDU_ID(pipe), cbBytesRead);
                for (unsigned int j=0; j<cbBytesRead; j++) {
                    size_t nb_in_msg = strlen(msg);
                    char * msg_end = msg + nb_in_msg;
                    sprintf_s(msg_end, sizeof(msg)-nb_in_msg, " %02X", buffer[j]);
                    if (j>=10 && j<cbBytesRead) {
                        strcat_s(msg, sizeof(msg), " ...");
                    }
                }
                } //!WANT_SHOW_ALL_RX_DATA
                strcat_s(msg, sizeof(msg), "\n");
            #else
                if (WANT_SHOW_ALL_RX_DATA) {
                if (cbBytesRead==sizeof(unsigned long)) {
                    sprintf  (msg,              "-> Read #%d: 0x%08X",
                            MCDU_ID(pipe), *(unsigned long *)buffer);
                } else {
                    sprintf  (msg,              "-> Read #%d: 0x%08X -- %d bytes!",
                        MCDU_ID(pipe), *(unsigned long *)buffer, cbBytesRead);
                }
                } else { //!WANT_SHOW_ALL_RX_DATA
                sprintf  (msg,              "-> #%d Reader: Read pipe, %4u bytes:",
                            MCDU_ID(pipe), cbBytesRead);
                for (unsigned int j=0; j<cbBytesRead; j++) {
                    size_t nb_in_msg = strlen(msg);
                    char * msg_end = msg + nb_in_msg;
                    sprintf  (msg_end,                        " %02X", buffer[j]);
                    if (j>=10 && j<cbBytesRead) {
                        strcat  (msg,              " ...");
                    }
                }
                } //WANT_SHOW_ALL_RX_DATA
                strcat  (msg,              "\n");
            #endif
                log_printf(msg);
            }
            } //WANT_DEBUG_READER || WANT_SHOW_ALL_RX_DATA

            { /*context*/
            unsigned long rxWord = *(unsigned long *)buffer;
            bool bTraceRxWord;
            if (!WANT_SHOW_ALL_RX_DATA && WANT_SHOW_RX_WORD) { // only WANT_SHOW_RX_WORD
                bTraceRxWord = true;
            } else { //WANT_SHOW_ALL_RX_DATA || !WANT_SHOW_RX_WORD
                bTraceRxWord = false;
            } //WANT_SHOW_ALL_RX_DATA || !WANT_SHOW_RX_WORD

            //#if WANT_SHOW_INTERESTING_RX
            if (WordIsInteresting(rxWord))
                bTraceRxWord = true;
            //#endif //WANT_SHOW_INTERESTING_RX

            if (bTraceRxWord && DEBUG_MCDU(pipe)) {
                log_printf ("%sRx #%d: 0x%08X%s\n", other_indent, MCDU_ID(pipe), *(unsigned long *)buffer, !pipe->IsAcceptingRx()?" -- discard":"");
            }

            } /*context*/

            if (WANT_CIRC_BUFFER_TRACE) {
            Circular_Trace(pipe->MCDU(), *(unsigned long *)buffer, "Rx");
            } //WANT_CIRC_BUFFER_TRACE

            if (pipe->IsAcceptingRx()) {
                // process the data immediately, let the callback function handle it
                pipe->Call_OnRx( *(unsigned long *)buffer );
            } // else discarding, pipe is not connected yet

        } // end if read succeeded

    } // forever, exits at quitting time or due to an error

    // Disconnect the pipe, and close the handle to this pipe instance.
    if (WANT_DEBUG_READER) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Reader: Disconnect\n", MCDU_ID(pipe));
    }
    } //WANT_DEBUG_READER

    if (hPipeReader!=INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(hPipeReader);   // force client off, this end is bailing
    }

reader_cleanup_and_bail:

    CloseHandle(hPipeReader);

reader_bail:

    if (WANT_DEBUG_READER || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_MCDU(pipe)) {
        log_printf("-> #%d Reader: Exit\n", MCDU_ID(pipe));
    }
    } //WANT_DEBUG_READER || WANT_DEBUG_SHUTDOWN
    return 0;
}

////////////////////////////////////////////////////////////

    #ifdef __WXMSW__    // part of MCDU Simulator
CTransportPipe::CTransportPipe(int which_mcdu/*0,1,2,3*/, char * s_DI_Host)
    #else               // part of Display Interface Manager (DIM) Simulator
CTransportPipe::CTransportPipe(int which_mcdu/*0,1,2,3*/, char * s_DI_Host, CTransportMCDU * parent)
    #endif
{
    m_pipe = NULL;

    m_transport_start = false;
    m_transport_fail  = false;
    Set_Transport_Init();       // hasn't started or failed yet

    m_pipe = new CNamedPipe(which_mcdu, s_DI_Host, this);

    //(im) don't init() here, caller will do it shortly...
    //  if (!init(NULL, which_mcdu)) {
    //      log_printf("Create pipe FAILED for MCDU#%d\n", which_mcdu+1);
    //  //} else {
    //  //    log_printf("Create pipe for MCDU#%d\n", m_pipe->MCDU()+1);
    //  }
}

CTransportPipe::~CTransportPipe()
{
    if (m_pipe) {
        int MCDU_ID = m_pipe->MCDU()+1;

        // tell this pipe we are shutting down
        m_pipe->SetQuittingTime();

        // Wait around for a short while to give pipe BG threads time to wake up
        Sleep(10);

        // Delete the pipe
        if (WANT_DEBUG_SHUTDOWN) {
        log_printf("MCDU#%d) Delete pipe\n", MCDU_ID);
        } //WANT_DEBUG_SHUTDOWN
        delete m_pipe;
        m_pipe = NULL;

    } /*if m_pipe*/
}

#ifdef __WXMSW__
bool CTransportPipe::init ( wxEvtHandler * eventObjPtr, int port )
#else
bool CTransportPipe::init ( void * eventObjPtr, int port )
#endif
{
    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

    memset( m_bufrx, 0, sizeof(m_bufrx));
    m_bufrxLen = 0;

    if (!m_pipe)
        return 0;

    SetFinishedInit();
    return m_pipe->PipeStatus()!=0; // 0=failed to initialize, 1=init OK, anything else=OK too
}

// return true if debug message should be printed: writing allowed, returns WANT_SHOW_NOT_YET definition
bool CTransportPipe::Tx_Debug_helper(bool * bNotYet, bool * bQueueFull)
{
    *bNotYet = false;
    *bQueueFull = false;

    // got a pipe yet?
    if (!m_pipe)
        return false;    // oops!

    // is the queue full?
    *bQueueFull = m_pipe->Pipe_Queue_Full();

    // allowed to write yet?
    if (!m_pipe->Is_Write_Allowed()) {
        if (WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP) {
        if (WANT_SHOW_NOT_YET) {
        *bNotYet = true;    // not allowed yet, but still want to see it because WANT_SHOW_NOT_YET defined
        } //WANT_SHOW_NOT_YET
        } //WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP
        // not allowed yet, but don't want to see "not yet"
        return false;
    }

    // either the TX will succeed, or it will fail and show "not yet"
    return true;
}

bool CTransportPipe::sendWord (long txWord, int mcdu ) /* true = success, false = failure */
{
    // got a pipe yet?
    if (!m_pipe)
        return false;   // oops!

    // if the hardware isn't ready, quietly bail
    if (!IsFinishedInit() || !Has_Transport_Started()) {
        return false;   // oops!
    }

    // allowed to write yet?
    if (!m_pipe->Is_Write_Allowed()) {
        if (WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP) {
        if (WANT_SHOW_NOT_YET) {
        if (DEBUG_MCDU(m_pipe)) {
            log_printf("-> #%d sendWord: not yet\n", m_pipe->MCDU()+1);
        }
        } //WANT_SHOW_NOT_YET
        } //WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP
        return false;   // not yet
    }

    if (WANT_CIRC_BUFFER_TRACE) {
    Circular_Trace(m_pipe->MCDU(), (unsigned long)txWord, "Tx");
    } //WANT_CIRC_BUFFER_TRACE

    { /*context*/
    bool bTraceTxWord;
    if (!WANT_SHOW_ALL_TX_DATA && WANT_SHOW_TX_WORD) { // only WANT_SHOW_TX_WORD
        bTraceTxWord = true;
    } else { //WANT_SHOW_ALL_TX_DATA || !WANT_SHOW_TX_WORD
        bTraceTxWord = false;
    } //WANT_SHOW_ALL_TX_DATA || !WANT_SHOW_TX_WORD

    //#if WANT_SHOW_INTERESTING_TX
    if (WordIsInteresting(txWord))
        bTraceTxWord = true;
    //#endif //WANT_SHOW_INTERESTING_TX

    if (bTraceTxWord && DEBUG_MCDU(m_pipe)) {
        log_printf ("%sTx #%d: 0x%08X\n", our_indent, m_pipe->MCDU()+1, txWord);
    }

    } /*context*/

    if (WANT_CIRC_BUFFER_TRACE) {
    Circular_Trace_Dump();
    } //WANT_CIRC_BUFFER_TRACE

    // make sure there is room in the queue to stuff another word
    // (if not, the BG Writer thread is getting behind)

    while (m_pipe->Pipe_Queue_Full()) {

        if (WANT_DEBUG_SIGNALS) {
        if (DEBUG_MCDU(m_pipe)) {
            char msg[200];
            sprintf_s(msg, sizeof(msg), "-> #%d sendWord: queue full!\n",
                    m_pipe->MCDU()+1);
            log_printf(msg);
        }
        } //WANT_DEBUG_SIGNALS

        // Wait until the BG Writer thread sends out some words from the queue
        DWORD sleep_for = 100L;

        if (WANT_DEBUG_SIGNALS) {
        if (DEBUG_MCDU(m_pipe)) {
            log_printf("-> #%d sendWord: sleep for %lu msec\n", m_pipe->MCDU()+1, sleep_for);
        }
        } //WANT_DEBUG_SIGNALS

        MSecTime t1000start;            // current time in msecs since 01-Jan-1970
        SetLastError(0L);
        DWORD dwStatus = MyWait(m_pipe->WakeupSendWordEvent(), sleep_for);
        DWORD dwErr = GetLastError();

        if (WANT_DEBUG_SIGNALS) {
        if (DEBUG_MCDU(m_pipe)) {
            MSecTime t1000now;
            unsigned long ms_diff = t1000now.Interval(t1000start);
            log_printf("-> #%d sendWord: awake after %lu ms\n", m_pipe->MCDU()+1, ms_diff);
        }
        } //WANT_DEBUG_SIGNALS

        if (WANT_DEBUG_VERBOSE) {
        if (DEBUG_MCDU(m_pipe)) {
            char msg[200];
            char * interpreted_wait_status = "unknown";
            switch(dwStatus) {
            case WAIT_OBJECT_0:     // the object was signalled
                interpreted_wait_status = "OBJECT_0"; break;
            case WAIT_TIMEOUT:      // timeout expired, go back to sleep
                interpreted_wait_status = "TIMEOUT"; break;
            case WAIT_ABANDONED:    // abandoned: app is shutting down and the object handle is invalid
                interpreted_wait_status = "ABANDONED"; break;
            default:
                break;
            }
            sprintf_s(msg, sizeof(msg), "-> #%d sendWord: Wait() => %lu(%s)",
                    m_pipe->MCDU()+1, dwStatus, interpreted_wait_status);
            append_dwErr_as_needed_and_LOG(NULL, msg, sizeof(msg), dwErr);
        }
        } //WANT_DEBUG_VERBOSE

        // wake up because someone (the BG Writer thread) just wrote a word,
        // so there should be room for at least one more word in the queue

        //#if WANT_PIPE_STATISTICS
        static unsigned int old_pipe_count[4] = {0,0,0,0};
               unsigned int new_pipe_count = m_pipe->Pipe_Queue_High_Water_Mark();
        if (old_pipe_count[m_pipe->MCDU()] != new_pipe_count) {
            //if (DEBUG_MCDU(m_pipe)) {
            //    char msg[200];
            //    sprintf_s(msg, sizeof(msg), "-> #%d sendWord: high water %3d now %3d.\n",
            //            m_pipe->MCDU()+1, old_pipe_count[m_pipe->MCDU()], new_pipe_count);
            //    log_printf(msg);
            //}
            old_pipe_count[m_pipe->MCDU()] = new_pipe_count;
        }
        //#endif //WANT_PIPE_STATISTICS

        switch (dwStatus) {
        case WAIT_OBJECT_0:     // the object was signalled -- BG Writer thread removed a word from the queue
            break;
        case WAIT_TIMEOUT:      // timeout expired, go back to sleep
            continue;
        case WAIT_ABANDONED:    // abandoned: app is shutting down and the object handle is invalid
            m_pipe->SetQuittingTime();
            return false;       // failure, app is shutting down
        default:
            continue;
        }
        // wake up because hopefully there is room in the queue now
    } // while queue is full

    // stuff word into queue
    m_pipe->Write_Pipe_Queue(txWord);

    if (WANT_DEBUG_SIGNALS) {
    if (DEBUG_MCDU(m_pipe)) {
        log_printf("-> #%d sendWord: signal WriterEv\n", m_pipe->MCDU()+1);
    }
    } //WANT_DEBUG_SIGNALS

    // tell BG Writer thread to wakeup and send it
    SetEvent(m_pipe->WakeupWriterEvent());

    return true;        // success
}

bool CTransportPipe::getRxWord ( int connectionID, long * rxWord)
{
    if (m_bufrxLen != 1)
    {
        if (DEBUG_MCDU(m_pipe)) {
            log_printf("!!! getRxWord called with no data available !!!\n", m_bufrxLen);
        }
        return false;
    }

    if (Ignore_This_Rx_Word(m_dwRxData)) {
        UNLOCK_RX();
        return false;
    }

    if (DEBUG_MCDU(m_pipe)) {
        log_printf("raw rx data: 0x%08X\n", m_dwRxData);
    }

    *rxWord = m_dwRxData;

    m_bufrxLen = 0;
    UNLOCK_RX();
    return true;
}

void CTransportPipe::UNLOCK_RX(void)
{
}

#define SLEEP_TIME          100         // 100 ms
#define SLEEP_THRESHOLD     100

void * CTransportPipe::Entry()       // this is the RX thread
{
    //if (DEBUG_MCDU(m_pipe)) {
    //  log_printf("Rx thread started\n");
    //}
    short len;
    //int slen=sizeof(m_client_socket_addr);
    int sleepCounter = 0;

    while(1)
    {
        m_bufrxLen = 0;

        // TODO: check the queue
        len = 0; //GetRxQueueStatus(gConfig.cfgArincCard, gConfig.cfgArincRxChannel);

        if (len < 0)
        {
            if (DEBUG_MCDU(m_pipe)) {
                log_printf("GetRxQueueStatus: Error %d\n", len);
            }
            break;      // end thread
        }

        if (len == 0)
        {
            if (sleepCounter >= SLEEP_THRESHOLD)
                Sleep(SLEEP_TIME);
            else
                sleepCounter++;
            continue;
        }

        if (len>0)
        {
            /* Read one 32-bit word from the receiver's queue with 48BIT/IRIG time stamp. */
            len = 1; //ReadRxQueueIrigOne(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, &m_dwRxData, NULL, NULL);

            if (len != 1)
            {
                if (DEBUG_MCDU(m_pipe)) {
                    log_printf("ReadRxQueueIrigOne: Error %d\n", len);
                }
                break;      // end thread
            }

            // we have a valid word
            m_bufrxLen = 1;
            #ifdef __WXMSW__
            postEvent(MCDU_SIM___CUSTOM_EVENT_RX);
            #endif

            // lock RX in a mutex
            // Wait indefinitely until the semaphore count becomes strictly positive and then decrement it and return.

            // TODO: Block this BG thread

            //if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL("Fail to lock RX thread"); }
            //if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL("Fail to lock RX thread"); }
            sleepCounter = 0;

        }

    } // while(1)

    //(im) If this function exits, the wxThread is deleted, which also deletes the
    //     transport driver. Unfortunately, other threads are still using it.
    //     So just sleep until the app exits.
    for(;;) Sleep(1000);

    return (void *)NULL; // never actually returns, but wxThread::Entry insists on returning a void *
}

////////////////////////////////////////////////////////////

#ifdef __WXMSW__    // part of MCDU Simulator
// these are included in the parent class CTransport
#else               // part of Display Interface Manager (DIM) Simulator

bool CTransportPipe::Has_Transport_Started(void)
{
    return m_transport_start;
}

bool CTransportPipe::Has_Transport_Failed(void)
{
    return m_transport_fail;
}

void CTransportPipe::Set_Transport_Init(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_pipe) {
    int MCDU_ID = m_pipe->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Init  %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, false, false);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_start = false;
    m_transport_fail = false;
}

void CTransportPipe::Set_Transport_Started(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_pipe) {
    int MCDU_ID = m_pipe->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Start %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, true, false);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_start = true;
    m_transport_fail = false;
}

void CTransportPipe::Set_Transport_Failed(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_pipe) {
    int MCDU_ID = m_pipe->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Fail  %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, m_transport_start, true);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_fail = true;
}

#endif

////////////////////////////////////////////////////////////

// end of transportPipe.cpp
