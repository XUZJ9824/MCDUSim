/*****************************************************************
 *
 * Name:        pythonInt.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: pythonInt.cpp,v 1.13 2012/05/31 19:02:59 macrae.i Exp $
 */

#define WANT_TRACE_KEYBOARD         0   // 0=normal, 1=debugging

#include "winsock2_wrapper.h" // do this before wx/wx.h so <winsock2.h> and <windows.h> get included in the right order

#include "defs.h"
#include "log.h"
//#include "wx/socket.h"
#ifndef __WXMSW__
 //(im)//#include "in.h"
 //(im)//#include <sys/socket.h>
 //(im)//#include <netinet/in.h>
 //(im)//#include <arpa/inet.h>
#endif
#include "pythonInt.h"
#include "termScreen.h" // for SCREEN_N_ROWS
#include "skin.h"       // for mcduGeometry_st
extern mcduGeometry_st * which_mcdu_geom;   /* defined in mcdusim.cpp */

#define     ACK_TIMEOUT  8      // 2 seconds

#define     XML_BUFFER_SIZE    100000
#define     XML_CHAR_NODE_SIZE 200
#define     TOTAL_COLOURS      8
#define     TOTAL_SIZES        2
#define     MAX_COLOUR_LENGTH  10
#define     MAX_SIZE_LENGTH    10

#define UNLOCK_RX() \
    if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); } \
    do { if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); }} while(0)

BEGIN_EVENT_TABLE(CPythonInt, wxEvtHandler) /*wxFrame)*/
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_PYTHON_RX, CPythonInt::OnPythonRx)
END_EVENT_TABLE()

CPythonInt::CPythonInt() : wxThread(wxTHREAD_JOINABLE), m_Sem(1),m_screenPtr(NULL)
{

}

CPythonInt::~CPythonInt()
{
    //close(m_sockfd);

    // Kill the thread created by Create() as it passes through TestDestroy()
    wxThread::ExitCode retCode;
    
    /* (im) sometimes at exit, we get deadlock because two threads are trying
     *      to free memory at the same time, both using the same lock.
     *      This thread is about to call
     *              mcduSim.exe!wxThread::Delete(void * * pRc=0x0012f600)  Line 1127
     *              mcduSim.exe!wxThreadInternal::WaitForTerminate(wxCriticalSection & cs={...}, void * * pRc=0x0012f600, wxThread * threadToDelete=0x028a39f8)  Line 822 + 0x15 bytes
     *      and is interrupted by an event (in this case a mouse-over which tries to load "comctl32.dll" to track it)
     *              mcduSim.exe!wxGUIAppTraits::DoMessageFromThreadWait()  Line 231 + 0xf bytes
     *              mcduSim.exe!wxGUIEventLoop::Dispatch()  Line 333 + 0x13 bytes
     *              mcduSim.exe!wxGUIEventLoop::ProcessMessage(tagMSG * msg=0x0012f2e0)  Line 273 + 0xc bytes
     *              user32.dll!75aecc70()   
     *              user32.dll!75aecc19()   
     *              user32.dll!75aec590()   
     *              user32.dll!75aec5e7()   
     *              user32.dll!75aec4e7()   
     *              mcduSim.exe!wxWndProc(HWND__ * hWnd=0x00650b20, unsigned int message=0x00000200, unsigned int wParam=0x00000000, long lParam=0x00020173)  Line 2744 + 0x1e bytes
     *              mcduSim.exe!wxFrame::MSWWindowProc(unsigned int message=0x00000200, unsigned int wParam=0x00000000, long lParam=0x00020173)  Line 1031 + 0x14 bytes
     *              mcduSim.exe!wxTopLevelWindowMSW::MSWWindowProc(unsigned int message=0x00000200, unsigned int wParam=0x00000000, long lParam=0x00020173)  Line 362 + 0x14 bytes
     *              mcduSim.exe!wxWindow::MSWWindowProc(unsigned int message=0x00000200, unsigned int wParam=0x00000000, long lParam=0x00020173)  Line 2909 + 0x23 bytes
     *              mcduSim.exe!wxWindow::HandleMouseMove(int x=0x00000173, int y=0x00000002, unsigned int flags=0x00000000)  Line 5448 + 0x26 bytes
     *              mcduSim.exe!wxLoadedDLL::wxLoadedDLL(const wxString & dllname={...})  Line 399
     *              mcduSim.exe!wxDynamicLibrary::wxDynamicLibrary(const wxString & libname={...}, int flags=0x00000068)  Line 242
     *              mcduSim.exe!wxDynamicLibrary::Load(const wxString & libnameOrig={...}, int flags=0x00000068)  Line 97 + 0xd bytes
     *              mcduSim.exe!wxDynamicLibrary::RawLoad(const wxString & libname={...}, int flags=0x00000068)  Line 243 + 0x19 bytes
     *              0012e960()  
     *              mcduSim.exe!wxString::wx_str()  Line 1641 + 0x35 bytes
     *              msvcp80d.dll!std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> >::c_str()  Line 1633
     *              cccccccc()  
     *              msvcr80d.dll!operator new(unsigned int size=0xcccccccc)  Line 59 + 0x9 bytes
     *              msvcr80d.dll!malloc(unsigned int nSize=0x0012ead8)  Line 154 + 0x15 bytes
     *              msvcr80d.dll!_nh_malloc_dbg(unsigned int nSize=0x00000020, int nhFlag=0x00000000, int nBlockUse=0x00000001, const char * szFileName=0x00000000, int nLine=0x00000000)  Line 268 + 0x15 bytes
     *              msvcr80d.dll!_heap_alloc_dbg(unsigned int nSize=0x00000020, int nBlockUse=0x00000001, const char * szFileName=0x00000000, int nLine=0x00000000)  Line 474 + 0xc bytes
     *              msvcr80d.dll!_heap_alloc_dbg(unsigned int nSize=0x00000020, int nBlockUse=0x00000001, const char * szFileName=0x00000000, int nLine=0x00000000)  Line 477 + 0x7 bytes
     *              msvcr80d.dll!_unlock(int locknum=0x00000004)  Line 376  C
     */
                         SetEvtHandlerEnabled(false);
    m_parentEventObjPtr->SetEvtHandlerEnabled(false);

    /*wxThreadError err =*/ Delete(&retCode);
}

#define BUFLEN 10
bool CPythonInt::init ( wxEvtHandler * eventObjPtr, CProt * protocolObjPtr, int port )
{
    m_parentEventObjPtr = eventObjPtr;
    m_protocolObjPtr = protocolObjPtr;
    m_port = port;

#ifdef __WXMSW__
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = 0x0101; //MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        log_printf("python socket, WSAStartup() = %d\n", err);
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        CATFAIL("WSAStartup failed");
        return false;
    }
#endif

    m_pendingAckTimer = 0;
    //LOG("Call create python rx thread");
    Create();
    Run();

    return true; // (im) make compiler happy
}


void * CPythonInt::Entry()       // this is the RX thread
{
    log_printf("Python thread ID %lu starting\n", GetCurrentThreadId());
    //LOG("Rx thread started");
    char * ipClient;

    //(unused)//int lenRx, len;

    LOG("Python Comm thread started");

    Sleep(500);

    LOG("Creating Python Socket");

    memset( m_bufrx, 0, BUF_PYTHON_RX_LEN);
    m_bufrxLen = m_bufrxIdxOut = 0;

    if ((m_sockfd =socket(AF_INET, SOCK_STREAM, 0))==-1) {
        int err = WSAGetLastError();
        log_printf("python socket, socket() err = %d\n", err);
        CATFAIL(wxT("socket could not be created"));
    }

    //#ifdef __WXMSW__ /*(im) we always want the socket to be non-blocking */
    {
    unsigned long var = 1; /*(im) need var=0 for blocking, var=1 for non-blocking */
    ioctlsocket(m_sockfd, FIONBIO, &var);
    }
    //#endif

    memset( (char *) &m_serv_addr, 0, sizeof(m_serv_addr) );
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons( m_port );

    m_clientLen = sizeof( m_cliAddr );

    if (bind(m_sockfd, (struct sockaddr *) &m_serv_addr, sizeof(m_serv_addr)) < 0)
    {
        int err = WSAGetLastError();
        log_printf("python socket, bind() err = %d\n", err);
        CATFAIL(wxT("could not bind socket"));
    }

    if (listen(m_sockfd,SOMAXCONN) != 0) {
        int err = WSAGetLastError();
        log_printf("python socket, listen() err = %d\n", err);
        CATFAIL(wxT("python socket fail to listen"));
    }

    for(;;) {
        /*(im) bail if we are shutting down */
        if (TestDestroy()) {
            log_printf("Python thread ID %lu shutting down (outer loop)\n", GetCurrentThreadId());
            break;
        }

        #ifdef __WXMSW__
        m_clientSocket = accept(m_sockfd, (struct sockaddr *) &m_cliAddr, &m_clientLen);
        #else
        m_clientSocket = accept(m_sockfd, (struct sockaddr *) &m_cliAddr, (socklen_t*)&m_clientLen);
        #endif
        if (m_clientSocket == INVALID_SOCKET) {
            /*(im) handle a non-blocking socket when there is no incoming connection */
            int err = WSAGetLastError();
            if (err==WSAEWOULDBLOCK) {
                Sleep(100);
                continue;
            }
            /*(im) else some other error, really should handle it */
            log_printf("python socket, accept() err = %d\n", err);
            CATFAIL(wxT("python socket fail to accept"));
        } //INVALID_SOCKET // == 0xFFFFFFFF, but SOCKET is unsigned, can't use <0

        // only accept local clients
        ipClient = inet_ntoa(m_cliAddr.sin_addr);
        if (strcmp(ipClient, "127.0.0.1")!=0) { //not localhost
            log_printf("python socket, accept() from \"%s\" port %d, ignored\n", ipClient, ntohs(m_cliAddr.sin_port));
            goto discard_client_socket;
        }

        log_printf("python socket, accept() from \"%s\" port %d\n", ipClient, ntohs(m_cliAddr.sin_port));

        for(;;) {
            /*(im) bail if we are shutting down */
            if (TestDestroy()) {
                log_printf("Python thread ID %lu shutting down (inner loop)\n", GetCurrentThreadId());
                break;
            }
            /************** rx loop *****************/
            memset(m_bufrx, 0, BUF_PYTHON_RX_LEN );
            m_bufrxLen  = recv(m_clientSocket, (char*)m_bufrx, BUF_PYTHON_RX_LEN, 0 );

            // has the other end disconnected cleanly?
            if (m_bufrxLen==0) {    // yes
                log_printf("python socket, recv() from \"%s\" port %d, socket closed\n", ipClient, ntohs(m_cliAddr.sin_port));
                break; /*goto discard_client_socket;*/
            }

            if (m_bufrxLen == SOCKET_ERROR) {
                /*(im) handle a non-blocking socket when there is no incoming connection */
                int err = WSAGetLastError();
                if (err==WSAEWOULDBLOCK) {
                    Sleep(100);
                    continue;
                }
                do {} while (0);
                log_printf("python socket, recv() err = %d\n", err);
                if (err==WSAECONNABORTED || err==WSAECONNRESET) {
                    /*(im) other end went away, we're done with this client socket */
                    break; /*goto discard_client_socket;*/
                }
                /*(im) else some other error, really should handle it */
                Sleep(100);
                continue;
            }

            // notify App that there is a incoming request
            postEventToItself(MCDU_SIM___CUSTOM_EVENT_PYTHON_RX);

            // lock RX in a mutex
            if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }
            if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }
        } // forever, or until recev error indicates client has disconnected

        // done with this client socket, cleanup
discard_client_socket:
        #ifdef __WXMSW__
        closesocket( m_clientSocket );
        #else
        close( m_clientSocket );
        #endif
        m_clientSocket = -1;

        // then wait for another connection
        continue;
    } // forever

    LOG("End python thread\n");
#ifdef __WXMSW__
    closesocket( m_sockfd );
#else
    close( m_sockfd );
#endif

    return (void *)NULL; // wxThread::Entry insists on returning something.
    // (im) if you need this thread's exit value, it is returned from Delete(), see CPythonInt::~CPythonInt().
}

int CPythonInt::SendTxt ( char * txt, int len )
{
    return send( m_clientSocket, txt, len, 0);
}

void CPythonInt::postEventToParent( int eventID, int genericInt )
{
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventID);
    event.SetInt(genericInt);
    m_parentEventObjPtr->AddPendingEvent(event);
}

void CPythonInt::postEventToItself( int eventID )
{
    wxEvtHandler * eventObjPtr = (wxEvtHandler *) this;
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventID);
    eventObjPtr->AddPendingEvent(event);
}


int CPythonInt::GetRx( char * buf, int maxlen )
{
    int len = m_bufrxLen;
    if (m_bufrxLen <= 0) return 0;

    if (maxlen < m_bufrxLen) len = maxlen;

    memcpy(buf, m_bufrx, len);

    return len;

}

void CPythonInt::UnlockRx (void)
{
    UNLOCK_RX();
}

void CPythonInt::OnPythonRx(wxCommandEvent& event)
{
    char cmd[128];
    termChar_st screenChar;

    int i, key, line, col, nchars;
    //(unused)//int idx;

    //(im) sometime we get a binary frame
    if (m_bufrx[0]==EXTERN_SIM__FRAME_HEADER &&
        m_bufrxLen==EXTERN_SIM__FRAME_LEN) {
        //(im) what are we supposed to do with this? show as debugging, then discard
        if (gdisplayLogScreen) {
            char * append_dots="";
            char msg[100];
            msg[0] = '\0';
            if (m_bufrxLen>25) {
                append_dots=" ...";
                m_bufrxLen = 25; // prevent msg[] from overflowing
            }
            for (i=0; i<m_bufrxLen; i++) {
                if (i) strcat(msg," ");
                sprintf(msg+strlen(msg),"%02X", m_bufrx[i]);
            }
            log_printf("OnPythonRx [%s%s]\n", msg, append_dots);
        }
        UNLOCK_RX();
        return;
    }

    /* strip trailing newline characters from command */
    while (m_bufrxLen>0) {
        char ch = m_bufrx[m_bufrxLen - 1];
        if (ch=='\r' || ch=='\n') {
            -- m_bufrxLen;
            m_bufrx[m_bufrxLen] = '\0';
        } else
            break;
    }
    /* if nothing left to scan for a command, bail */
    if (m_bufrxLen <=0) {
        UNLOCK_RX();
        return;
    }

    cmd[0] = 0;
    sscanf((char*)m_bufrx, "%s", cmd);
    //LOG1S("OnPythonRx:", m_bufrx);

    if (strcmp(cmd, "keycode") == 0)
    {
        int nw = sscanf((char*)m_bufrx, "%s %d", cmd, &key);
        if (nw!=2) {
            /* wasn't parsed correctly */
            char * s = "badParameter (keycode)\r\n";
            SendTxt( s, strlen(s));

            UNLOCK_RX();
            return;
        }
        key &= 0x000000ff;
        extern char * LookupKey(int key); // in mcdusim.c
        #if WANT_TRACE_KEYBOARD
        log_printf("OnPythonRx: keycode = 0x%02X {%s}\n", key, LookupKey(key));
        #endif
        postEventToParent(MCDU_SIM___KEY_FROM_PYTHON, key);
        //m_protocolObjPtr->sendKey(key);
        m_pendingAckTimer = ACK_TIMEOUT;
        return;
    }

    if (strcmp(cmd, "screentxt") == 0)
    {
        int nw = sscanf((char*)m_bufrx, "%s %d %d %d", cmd, &line, &col, &nchars);
        if (nw!=4) {
            /* wasn't parsed correctly */
            char * s = "badParameter (screentxt)\r\n";
            SendTxt( s, strlen(s));

            UNLOCK_RX();
            return;
        }
        log_printf("OnPythonRx: screentxt line=%2d col=%2d len=%2d", line, col, nchars );
        if (nchars > SCREEN_N_COLS) {
            nchars = SCREEN_N_COLS;
        }
        line--; col--;
        for (i=0; i<nchars; i++)
        {
            // At this point line is 0,1,2,...
            //(im) if the python script asks for a line 13 and the MCDU appearance is TCP,
            //     that shouldn't be a fatal error: just return blanks and carry on
            //     At least until the python script can ask how many lines there are
            //(im) if the python scripts asks for an impossible line#, that is really bad.
            if (line>=SCREEN_N_ROWS/*m_screenPtr->screen_n_rows*/ && line < MAX_SCREEN_ROWS) { // 10,11,12,13
                break; // not valid for THIS appearance, but not an unreasonable number
            }

            screenChar.charcode = 0;
            m_screenPtr->getCharDesc (&screenChar, line, col++);
            if (screenChar.charcode == 0)
            {
                log_printf(" = bad Parameter!\n");
                char * s = "badParameter\r\n";
                SendTxt( s, strlen(s));

                UNLOCK_RX();
                return;
//screenChar.charcode = '#';
            }
            cmd[i] = screenChar.charcode;
        }
        cmd[i] = '\0';
        log_printf(" = \"%s\"\n", cmd);
        if (i==0) cmd[i++] = '?'; //apparently the retuned text must have at least one character
        cmd[i++] = '\r';
        cmd[i++] = '\n';
        cmd[i++] = 0;
        SendTxt(cmd, strlen(cmd));

        UNLOCK_RX();
        return;
    }

    if (strcmp(cmd, "screenxml") == 0)
    {
        SendScreenXml();
        return;
    }

    if (strcmp(cmd, "makescreenshot") == 0)
    {
        log_printf("OnPythonRx: makescreenshot\n");
        postEventToParent(MCDU_SIM___CUSTOM_EVENT_EXPORT_SCREEN, 0);

        char * s = "OK\r\n";
        SendTxt( s, strlen(s));

        UNLOCK_RX();
        return;
    }

    log_printf("OnPythonRx: unknown cmd {%s}\n", m_bufrx);
    char * s = "unknownCmd\r\n";
    SendTxt( s, strlen(s));
    UNLOCK_RX();
    return;
}

void CPythonInt::SendScreenXml(void)
{
    // 144 characters per screen but a lot of text for each character.
    char        fullScreenXml[XML_BUFFER_SIZE] = "\0";
    char        xmlMcduStart[] = "<mcdu> ";
    char        xmlMcduEnd[] = "</mcdu>\r\n";
    char        xmlRowStart[] = "<row>";
    char        xmlRowEnd[] = "</row> ";
    int         rowIndex = 0;
    int         colIndex = 0;
    /* buffer to hold an XML node describing one character */
    char        charXmlNode[XML_CHAR_NODE_SIZE];
    /* matching the lists in mcdusim.cpp */
    char        colours[TOTAL_COLOURS][MAX_COLOUR_LENGTH] = {
        "black", "cyan", "red", "yellow", "green", "magenta", "amber", "white" 
    };
    char        sizes[TOTAL_SIZES][MAX_SIZE_LENGTH] = {"small", "large"};
    termChar_st charDesc;
    char        charXmlEnd[] = "</char>";
    int         bytes_sent = 0;

    if (gdisplayLogScreen) {
        log_printf("ScreenXml\n");
    }

    strncat(fullScreenXml, xmlMcduStart, XML_BUFFER_SIZE);
    for (rowIndex = 0; rowIndex < SCREEN_N_ROWS; rowIndex++)
    {
        strncat(fullScreenXml, xmlRowStart, XML_BUFFER_SIZE);
        for (colIndex = 0; colIndex < SCREEN_N_COLS; colIndex++)
        {
            m_screenPtr->getCharDesc(&charDesc, rowIndex, colIndex);
            assert(charDesc.color < TOTAL_COLOURS && charDesc.color >= 0);
            assert(charDesc.size < TOTAL_SIZES && charDesc.size >= 0);

            sprintf(charXmlNode, "<char colour=\"%s\" font_size=\"%s\">",
                    colours[charDesc.color], sizes[charDesc.size]);

            //(im) function call and sprintf() and strncat() to stuff 1 character -- yuck!!
            //EscapeXml(charDesc.charcode, charXmlNode);

            if (charDesc.charcode == '<') {
                strncat(charXmlNode, "&lt;", XML_CHAR_NODE_SIZE);
            } else
            if (charDesc.charcode == '>') {
                strncat(charXmlNode, "&gt;", XML_CHAR_NODE_SIZE);
            } else {    // append non-escaped character to end of line
                char * charXmlNodeEnd = &charXmlNode[strlen(charXmlNode)];
                if (charXmlNodeEnd < &charXmlNode[XML_CHAR_NODE_SIZE-2]) {
                    *charXmlNodeEnd++ = charDesc.charcode;
                    *charXmlNodeEnd   = '\0';
                }
            }
            strncat(charXmlNode, charXmlEnd, XML_CHAR_NODE_SIZE);

            strncat(fullScreenXml, charXmlNode, XML_BUFFER_SIZE);
        }
        strncat(fullScreenXml, xmlRowEnd, XML_BUFFER_SIZE);
    }
    strncat(fullScreenXml, xmlMcduEnd, XML_BUFFER_SIZE);

    bytes_sent = SendTxt(fullScreenXml, strlen(fullScreenXml));
    if (gdisplayLogScreen) {
        log_printf("ScreenXml returned %d bytes\n", bytes_sent);
    }
    assert(bytes_sent == (int)strlen(fullScreenXml));

    UNLOCK_RX();
}

///* appends the escaped version of inputChar to inputOutputString
//   (assumed to already hold some text) */
//void CPythonInt::EscapeXml(char inputChar, char *inputOutputString)
//{
//    char lessThanHtml[] = "&lt;";
//    char greaterThanHtml[] = "&gt;";
//    char localBuffer[XML_CHAR_NODE_SIZE];
//
//    assert(inputOutputString != NULL);
//    if (inputChar == '<')
//    {
//        strncat(inputOutputString, lessThanHtml, XML_CHAR_NODE_SIZE);
//        return;
//    }
//    if (inputChar == '>')
//    {
//        strncat(inputOutputString, greaterThanHtml, XML_CHAR_NODE_SIZE);
//        return;
//    }
//    sprintf(localBuffer, "%c", inputChar);
//    strncat(inputOutputString, localBuffer, XML_CHAR_NODE_SIZE);
//}

void CPythonInt::ProcessAck(void)
{
    if (m_pendingAckTimer)
    {
        LOG("send OK/Ack to python Client");

        char * s = "OK\r\n";
        SendTxt( s, strlen(s));

        m_pendingAckTimer = 0;
        UNLOCK_RX();

    }
}

void CPythonInt::OnTimer(void)
{
    if (m_pendingAckTimer)
    {
        m_pendingAckTimer--;
        if (m_pendingAckTimer == 0)
        {
            LOG("send Ack error to python Client");

            char * s = "noSduAck\r\n";
            SendTxt( s, strlen(s));

            UNLOCK_RX();
        }
    }
}

void CPythonInt::setScreen(CTermScreen * screenPtr)
{
    if (m_screenPtr != screenPtr) {
        if (m_screenPtr && screenPtr) {
            log_printf("Python switching screen %s to %s\n",
                m_screenPtr->ScreenIdent(), screenPtr->ScreenIdent());
        }

        m_screenPtr = screenPtr;
    } // else not changed
}
