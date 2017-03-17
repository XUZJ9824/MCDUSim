/*****************************************************************
 *
 * Name:        transportTcp.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportTcp.cpp,v 1.14 2012/01/26 16:21:48 macrae.i Exp $
 */
#include "winsock2_wrapper.h" // do this before wx/wx.h so <winsock2.h> and <windows.h> get included in the right order
#ifndef SIO_UDP_CONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12) // should be in MSTcpIP.h
#endif//SIO_UDP_CONNRESET

#define WANT_DEBUG_TCP_RX                   0   // 0=normal, 1=debug incoming TCP messages
#define WANT_DEBUG_TCP_SHUTDOWN             0   // 0=normal, 1=debug shutdown

#include "transportTcp.h"
#include "defs.h"
#include "log.h"
//#include "wx/socket.h"
#ifndef __WXMSW__
 #include "in.h"
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
#endif


CTransportTcp::CTransportTcp(u_short peerPort, bool ignoreSocketErrors) : m_Sem(1)
{
    m_peerPort = peerPort;
    m_ignoreSocketErrors = ignoreSocketErrors;

    m_EntryShould_Bail = false;
    m_dwEntry_ThreadId = 0;
    m_socketID = 0;
}

CTransportTcp::~CTransportTcp()
{
    m_EntryShould_Bail = true;

#if WANT_DEBUG_TCP_SHUTDOWN
    log_printf("~CTransportTcp: terminate thread %u, close socket %x\n", m_dwEntry_ThreadId, m_socketID);
#endif

    if (m_dwEntry_ThreadId) {
        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_dwEntry_ThreadId);
        if (hThread) {
            TerminateThread(hThread, 999);
            Sleep (10);
            DWORD dwExitCode = 0L;
            GetExitCodeThread(hThread, &dwExitCode);
            //if (dwExitCode!=999) {
            //    do {} while (0);    // whoops!
            //}
        //} else {
        //    DWORD dwErr = GetLastError();
        //    do {} while (0);
        }
    }

    //while (m_socketID) {
    //    m_EntryShould_Bail = true;
    //    Sleep(10);
    //}

    closesocket(m_socketID);
    m_socketID = 0;
}

#define BUFLEN 10
bool CTransportTcp::init ( wxEvtHandler * eventObjPtr, int port )
{
    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

#ifdef __WXMSW__
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = 0x0101; //MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        CATFAIL("init TCP: WSAStartup failed");
        return false;
    }
#endif

    memset( m_bufrx, 0, sizeof(m_bufrx));
    m_bufrxLen = m_bufrxIdxOut = 0;

    int slen=sizeof(m_client_socket_addr);

    if ((m_socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        CATFAIL(wxT("init TCP: socket could not be created"));

    // from http://support.microsoft.com/kb/263823 as of February 21, 2007 - Revision: 4.3
    // In Windows 2000, using UDP may generate a WSAECONNRESET response.
    // If sending a datagram using the sendto function results in an
    // "ICMP port unreachable" response and the select function is set
    // for readfds, the program returns 1 and the subsequent call to the
    // recvfrom function does not work with a WSAECONNRESET (10054) error
    // response. In Microsoft Windows NT 4.0, this situation causes the
    // select function to block or time out.
    // A new sockets IOCTL "SIO_UDP_CONNRESET" was introduced in Windows 2000 (SP2).
    {
        DWORD dwBytesReturned = 0;
        BOOL bNewBehavior = FALSE;
        DWORD status;

        //see http://msdn.microsoft.com/en-us/library/windows/desktop/ms741621(v=vs.85).aspx
        // Controls whether UDP PORT_UNREACHABLE messages are reported.
        // Set to TRUE to enable reporting. Set to FALSE to disable reporting.
        status = WSAIoctl(m_socketID, SIO_UDP_CONNRESET,
                            &bNewBehavior, sizeof(bNewBehavior),
                            NULL, 0, &dwBytesReturned,
                            NULL, NULL);
    }

    memset((char *) &m_mcdu_socket_addr, 0, sizeof(m_mcdu_socket_addr));
    m_mcdu_socket_addr.sin_family = AF_INET;
    m_mcdu_socket_addr.sin_port = htons(port);
    m_mcdu_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    log_printf("port = %d\n", port);
    if (bind(m_socketID, (struct sockaddr *) &m_mcdu_socket_addr, sizeof(m_mcdu_socket_addr))==-1)
        CATFAIL(wxT("init TCP: could not bind MCDU own socket"));
#if WANT_DEBUG_TCP_RX
    log_printf("init TCP: Bind socket %x to port %u OK\n", m_socketID, port);
#endif

    // set address for teraterm app
    memset((char *) &m_teraterm_socket_addr, 0, sizeof(m_teraterm_socket_addr));
    m_teraterm_socket_addr.sin_family = AF_INET;
    m_teraterm_socket_addr.sin_port = htons(m_peerPort/*TERATERM_PORT*/);
    m_teraterm_socket_addr.sin_addr.s_addr = inet_addr( (const char*) TERATERM_IP  );

    LOG("init TCP: create thread");
    Create();
    Run();

    SetFinishedInit();
    return 1;
}

bool CTransportTcp::sendWord ( long txWord, int mcdu )
{
    // if the hardware isn't ready, quietly bail
    if (IsFinishedInit() && Has_Transport_Started()) {
        //postEvent(MCDU_SIM___CUSTOM_EVENT_CONNECTION);

        unsigned char txframe[EXTERN_SIM__FRAME_LEN];
        char parity;
        int len,i;

        /* Feb 11 2009

           we send the 32 bits txWord inside a small frame to serial interface
           Frame byte format is: |  H  |  D  | b0 | b1 | b2 | b3 |  P  |
           where: H = EXTERN_SIM__FRAME_HEADER (control A character)
                  D = mcdu destination (0,1,2 and 0xff broadcast)
                  d0~d3 -> txWord bytes
                  P = parity -> xor between all previous 5 bytes

        */
        txframe[0] = EXTERN_SIM__FRAME_HEADER;
        parity = EXTERN_SIM__FRAME_HEADER;            /* parity is being used here to calculate byte parity */
        txframe[1] = mcdu;                              /* TODO: set proper MCDU ID */
        parity = parity ^ txframe[1];                   /* update parity */
        txframe[2] = txWord & 0x000000ff;             /* get d0 */
        parity = parity ^ txframe[2];                   /* update parity */
        txframe[3] = (txWord >> 8) & 0x000000ff;      /* get d1 */
        parity = parity ^ txframe[3];                   /* update parity */
        txframe[4] = (txWord >> 16) & 0x000000ff;     /* get d2 */
        parity = parity ^ txframe[4];                   /* update parity */
        txframe[5] = (txWord >> 24) & 0x000000ff;     /* get d3 */
        txframe[6] = (parity ^ txframe[5]) & 0x000000ff;  /* update parity */


        #ifdef __WXMSW__
        //len = sendto(m_socketID, (char*)txframe, EXTERN_SIM__FRAME_LEN, 0, (struct sockaddr *) (const struct sockaddr*) &m_client_socket_addr, sizeof(m_client_socket_addr) );
        len = sendto(m_socketID, (char*)txframe, EXTERN_SIM__FRAME_LEN, 0, (struct sockaddr *) (const struct sockaddr*) &m_teraterm_socket_addr, sizeof(m_teraterm_socket_addr) );
        #else
        len = sendto(m_socketID, (char*)txframe, EXTERN_SIM__FRAME_LEN, 0, (struct sockaddr *) (const struct sockaddr*) &m_client_socket_addr, sizeof(m_client_socket_addr) );
        #endif
        if (len != EXTERN_SIM__FRAME_LEN) {
            LOG("FRAME sent error");
        } else {
            if (gdisplayLogScreen) {
                char msg[100];
                msg[0] = '\0';
                for (i=0; i<len; i++) { //len is == EXTERN_SIM__FRAME_LEN
                    if (i) strcat(msg," ");
                    sprintf(msg+strlen(msg),"%02X", txframe[i]);
                }
                log_printf("FRAME sent OK: %s\n", msg);
            }
            return true;
        }
    } // else hardware isn't available
    return false;
}

#define UNLOCK_RX() \
    if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); } \
    do { if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); }} while(0)

bool CTransportTcp::getRxWord ( int connectionID, long * rxWord)
{
    int i, idx = 0;
    long r = 0L;
    int dest, parity;

//  log_printf("rx frame with len=%d\n", m_bufrxLen);

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
            sprintf(msg+strlen(msg),"%02X", m_bufrx[i] & 0xFF);
        }
        log_printf("frame RX data: [%s%s]\n", msg, append_dots);
    }

    if (m_bufrxLen != EXTERN_SIM__FRAME_LEN)
    {
        log_printf("!!! bad frame len %d !!!\n", m_bufrxLen);
        UNLOCK_RX();
        return false;
    }

    if (m_bufrx[idx++] != EXTERN_SIM__FRAME_HEADER )
    {
        LOG("!!! bad header !!!");
        UNLOCK_RX();
        return false;
    }
    parity = EXTERN_SIM__FRAME_HEADER;

    dest = m_bufrx[idx++];
    parity = parity ^  dest;

    for (i=0; i<4; i++)
    {
#if 1
        r = (r >> 8) & 0x00ffffff;
        parity = parity ^  m_bufrx[idx];
        r |= (m_bufrx[idx++] << 24) & 0xff000000;
#else
        r = r << 8;
        parity = parity ^  m_bufrx[idx];
        r |= (m_bufrx[idx++] & 0x000000ff);
#endif
    }

    parity = parity ^  m_bufrx[idx];
    parity &= 0x000000ff;

    if ( parity != 0 )
    {
        log_printf("bad parity = 0x%X\n", parity);
        UNLOCK_RX();
        return false;
    }

    *rxWord = r;
    // unlock RX thread
    UNLOCK_RX();
    return true;
}

void * CTransportTcp::Entry()       // this is the RX thread
{
    int len;
    char * ipClient;
    int slen=sizeof(m_client_socket_addr);

    m_dwEntry_ThreadId = GetCurrentThreadId(); /* so we can be killed */
#if WANT_DEBUG_TCP_RX
    log_printf("Tcp: Rx thread %u started, socket %x\n", m_dwEntry_ThreadId, m_socketID);
#endif
    Set_Transport_Started();    /* the TCP/IP hardware is ready for use */

    while(!m_EntryShould_Bail)
    {
        m_bufrxLen = m_bufrxIdxOut = 0;
        ipClient = NULL;

#ifdef __WXMSW__
        len = recvfrom(m_socketID, m_bufrx, sizeof(m_bufrx), 0, (struct sockaddr *) &m_client_socket_addr, &slen);
#else
        len = recvfrom(m_socketID, m_bufrx, sizeof(m_bufrx), 0, (struct sockaddr *) &m_client_socket_addr, (socklen_t *) &slen);
#endif

#if WANT_DEBUG_TCP_RX
        log_printf("Tcp: Rx thread, recvfrom() = %d\n", len);
#endif

        // has the other end disconnected cleanly?
        if (len==0) { // yes
            if (1/*gdisplayLogScreen*/) {
                if (ipClient) {
                    log_printf("python socket, recvfrom() from \"%s\" port %d, socket closed\n", ipClient, ntohs(m_client_socket_addr.sin_port));
                } else {
                    log_printf("python socket, recvfrom() socket closed\n");
                }
            }
            continue;
        }

        if (len==SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err==WSAECONNRESET)
                continue;

            log_printf("python socket, recvfrom() err = %d\n", err);

            if (!m_ignoreSocketErrors)
                CATFAIL(wxT("catfail in recvfrom() from teraterm"));

            do {} while (0);
            continue;
        }

        // drop packet if not from localhost
        ipClient = inet_ntoa(m_client_socket_addr.sin_addr);
        if (strcmp(ipClient, "127.0.0.1")!=0) { //not localhost
            log_printf("CTransportTcp, recvfrom() from \"%s\" port %d, ignored\n", ipClient, ntohs(m_client_socket_addr.sin_port));
            continue;
        }

        // send this buffer up to CProt::OnRx()
        m_bufrxLen = len;
        m_bufrxIdxOut = 0;
        postEvent(MCDU_SIM___CUSTOM_EVENT_RX);

        // lock RX in a mutex
        if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }
        if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }

    } //while !m_EntryShould_Bail

    closesocket(m_socketID);
    m_socketID = 0;

    //(im) If this function exits, the wxThread is deleted, which also deletes the
    //     transport driver. Unfortunately, other threads are still using it.
    //     So just sleep until the app exits.
    for(;;) Sleep(1000);

    return (void *)NULL; // never actually returns, but wxThread::Entry insists on returning a void *
}

