/*****************************************************************
 *
 * Name:        mcdusim.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: mcdusim.h,v 1.17 2012/05/25 13:06:54 macrae.i Exp $
 */
#include "skin.h"
#include "termScreen.h"
#include "transportTcp.h"
#include "prot_739A.h"
#include "pythonInt.h"

#include "defs.h"


#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

//#include "wx/mediactrl.h"
//#include "remoteinspector.h"

//extern "C" {
//#include "remoteFirmware.h"
//}

// forward reference
class CMcduSimFrame;

// Define a new application
class CMcduSimApp: public wxApp
{
public:
    CMcduSimApp(void);
    ~CMcduSimApp();

    bool OnInit(void);
    
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
    wxString        m_t;
    CMcduSimFrame * m_frame;

private:
    wxMenuItem *m_PcKeyboardMenu;
    bool m_bEnablePcKeyboard;
public:
    void EnablePcKeyboard(bool bEnable);
    bool IsPcKeyboardEnabled(void) { return m_bEnablePcKeyboard; }

private:
    wxMenuItem *m_SmallFontAsLower;
    bool m_bEnableSmallFontAsLower;
public:
    void EnableSmallFontAsLower(bool bEnable);
    bool IsSmallFontAsLowerEnabled(void) { return m_bEnableSmallFontAsLower; }

public:
    void OnKeyboard(wxKeyEvent&);
    void OnLocalBasePort( wxCommandEvent& event );
    DECLARE_EVENT_TABLE()
};

// Define a new frame

class CMcduSimFrame: public wxFrame
{
  public:
    CMcduSimFrame(wxFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~CMcduSimFrame();

    void OnActivate(bool) {}
    //void OnLoadFile(wxCommandEvent& event);
    //void OnSaveFile(wxCommandEvent& event);
    //void OnShowDlg(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackGround(wxEraseEvent& event);
    void OnMouseMoveEvent(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnClose(wxCloseEvent& event);

    void OnTimer100ms(wxTimerEvent& event);
    void OnTimer250ms(wxTimerEvent& event);
    void OnMouseDoubleClickEvent(wxMouseEvent& event);
    void OnUpdateToggleCradleSensorMenu (wxUpdateUIEvent & event);
    void OnExport(wxCommandEvent& event);
    void OnExportBmp(wxCommandEvent& event);
    void OnDumpScreenToDebugLog(wxCommandEvent& event);
    void OnSendMcduId(wxCommandEvent& event);
    void OnCmdLineHelp(wxCommandEvent& event);
    void OnRxAck(wxCommandEvent& event);
    void OnRequestToExportBmp(wxCommandEvent& event);
    void OnKeyboard(wxKeyEvent& event);
    void OnEnablePcKeyboardMcduId(wxCommandEvent& event);
    void OnEnableSmallFontAsLower(wxCommandEvent& event);
    void OnConfigureMCDU(wxCommandEvent& event);
    void OnSduDisconnection(wxCommandEvent& event);
    void OnKeyFromPython(wxCommandEvent& event);
    void OnTxtToClipboard (wxCommandEvent& event);
    void OnExportTxt(wxCommandEvent& event);
    void OnGoToMenuScreen( wxCommandEvent& event );


 /*   void OnRx(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);*/

    // Media event handlers
    //void OnMediaLoaded(wxMediaEvent& event);

    //void changeSendingLED (bool onOff);
    //void changePressOkLED (bool onOff);
    //void changeBacklightLED (bool onOff);

    void processChangesFromApp ( void );
    void printAppLogs( void );
    void printSimLogs( uint8_t event,  uint8_t eventParam );
//    void playSound ( sound_t sound );

    int findControl(skinControls_st * control);
    void GoToMenuScreen(void);
    void onKey( int key, int repeatFlag);

    void setTheActiveScreen(CTermScreen * screenPtr);
    void setActiveScreenAndUpdate( CTermScreen * screenPtr );
    void printMenuTitle( void );
    void CheckMenuScreen( void );
    //void adjustSkin( void );

private:
    bool       m_changed_appearance;
    wxBitmap  *m_BaseSkinBitmap;//Note: points to m_NormalBaseSkinBitmap
    wxBitmap  *m_NormalBaseSkinBitmap;
    wxBitmap  *m_BacklightrdBaseSkinBitmap;
    wxBitmap  *m_CursorSkinBitmap;
    wxBitmap  *m_PressedSkinBitmap;
public:
    void        Load_Appearance_Bitmaps(void);

private:
    wxMenu *    m_app_control_menu;
public:
    void        Set_App_Control_Menu(wxMenu * menu) { m_app_control_menu = menu; };

private:
    int     m_status_counter;   // counts # of 250ms intervals since status message was displayed
public:
    void    SetStatusTextSafely(const wxString& text, int number=0);

private:
    bool    m_OK_keyIsDown;
    bool    m_RangeTest_keyIsDown;
    bool    m_Panic_keyIsDown;
    bool    m_Cancel_keyIsDown;

    bool    m_OK_CursorOver;
    bool    m_RangeTest_CursorOver;
    bool    m_Panic_CursorOver;
    bool    m_Cancel_CursorOver;
    int     m_holdKeyTimer;

    bool    m_motionFlag;

    CTermScreen     * m_sduScreen;
    CTermScreen     * m_menuScreen;
    CTermScreen     * m_activeScreen;//Note: points to either m_sduScreen or m_menuScreen !
public:
    void        InvalidateActiveAcreen() { m_activeScreen->InvalidateScreen(); }

private:
    bool        m_clip;
    wxString    m_export_filename;

    skinControls_st     m_currentButtonDown;
    skinControls_st     m_currentCursorOver;
    skinControls_st     m_lastCursorOver;
    int                 m_keyDown;
    int                 m_keyRepeatTimer;

   wxTimer m_timer100ms;
   wxTimer m_timer250ms;

   // wxMediaCtrl * m_player;
	unsigned long int m_loop_counter;


    bool    m_lacklightOn;

    //CTransport * m_transport;
    CProt * m_prot;

    //Phyton
    CPythonInt * m_python;

DECLARE_EVENT_TABLE()
};

#define MCDU_SIM___QUIT                     100
#define MCDU_SIM___ABOUT                    101
#define MCDU_SIM___LOAD_FILE                102
#define MCDU_SIM___SAVE_FILE                103
#define MCDU_SIM___PCKEYBOARD_MCDU_ID       104
#define MCDU_SIM___CONFIGURE                105
#define MCDU_SIM___EXPORT                   107
#define MCDU_SIM___EXPORT_BMP               108
#define MCDU_SIM___COMMAND_LINE_HELP        109
#define MCDU_SIM___SMALL_FONT_AS_LOWER      110
#define MCDU_SIM___DUMP_SCREEN_TO_DEBUG_LOG	111
