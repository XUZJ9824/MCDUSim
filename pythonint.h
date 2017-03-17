#ifndef __PYTHONINT_H__
#define __PYTHONINT_H__
/*****************************************************************
 *
 * Name:        pythonInt.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************

 */
 #include "wx/wx.h"

#include "termScreen.h"
#include "prot_739A.h"


#if (defined WIN32) || (defined WINCE)
    // already included "winsock2_wrapper.h", don't need these here now
 //#include <stdio.h>
 //#include "Winsock2.h"
 typedef SOCKET socket_type;
#else
 #include <pthread.h>
 #include <stdio.h>
 #include <string.h>
 #include <netdb.h>
 #include <sys/socket.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
  typedef int socket_type;
 typedef unsigned int UINT;
 typedef void * LPVOID;
 #ifdef __CYGWIN__
  #include <cygwin/in.h>
 #endif
#endif


#ifndef __WXMSW__
 #include "in.h"
#endif

#define         BUF_PYTHON_RX_LEN  1024
#define         PYTHON_BASE_PORT            8000

class CPythonInt : public wxThread, wxEvtHandler
{
public:
    CPythonInt();
    ~CPythonInt();
    //CPythonInt( int port );
    void ProcessAck( void );
    void OnTimer(void);
    void setScreen(CTermScreen * screenPtr);


    int SendTxt ( char * txt, int len );
    bool init ( wxEvtHandler * eventObjPtr, CProt * protocolObjPtr, int port );
    virtual void * Entry();

private:
    unsigned char   m_bufrx[BUF_PYTHON_RX_LEN];
    int             m_bufrxLen;
    int             m_bufrxIdxOut;
    int             m_port;
    int             m_pendingAckTimer;

    socket_type         m_clientSocket;
    int                 m_clientLen;
    struct sockaddr_in  m_cliAddr;

    socket_type         m_sockfd;
    struct sockaddr_in  m_serv_addr;

    void postEventToItself( int eventID );
    void postEventToParent( int eventID, int genericInt );
    wxEvtHandler * m_parentEventObjPtr;

    void processAck(void);
    void OnPythonRx(wxCommandEvent& event);
    void SendScreenXml(void);
    //void EscapeXml(char inputChar, char *outputString);

    int GetRx( char * buf, int maxlen );
    void UnlockRx (void);

    CProt *     m_protocolObjPtr;
    CTermScreen * m_screenPtr;


public:
    CProt *     PythonProt() { return m_protocolObjPtr; }
    CTermScreen * PythonScreen() { return m_screenPtr; }

private:


    wxSemaphore             m_Sem;

DECLARE_EVENT_TABLE()

};

/*------------------------------------------------------------------*/
#endif

