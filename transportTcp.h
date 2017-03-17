#ifndef __TRANSPORTTCP_H__
#define __TRANSPORTTCP_H__
/*****************************************************************
 *
 * Name:        transportTcp.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportTcp.h,v 1.7 2012/01/26 16:21:48 macrae.i Exp $
 */
#include "transport.h"

#ifndef __WXMSW__
 #include "in.h"
#endif

#define BUF_RX_LEN_TRANSPORTTCP  2048
#define         EXTERN_SIM__FRAME_HEADER    0x01
#define         EXTERN_SIM__FRAME_LEN       7

#ifdef __WXMSW__
 #define         TERATERM_IP                 "127.0.0.1"
#else
//#define        TERATERM_IP                 "192.168.86.36"
#endif

class CTransportTcp : public CTransport, wxThread
{
public:
    CTransportTcp(u_short peerPort, bool ignoreSocketErrors);
    ~CTransportTcp();
    virtual bool init ( wxEvtHandler * eventObjPtr, int port );
    virtual bool sendWord ( long txWord, int mcdu );
    virtual bool getRxWord ( int connectionID, long * rxWord);

    virtual void * Entry();

private:
    struct sockaddr_in      m_mcdu_socket_addr;
    struct sockaddr_in      m_client_socket_addr;
    struct sockaddr_in      m_teraterm_socket_addr;

    int                     m_socketID;
    char                    m_bufrx[BUF_RX_LEN_TRANSPORTTCP];
    int                     m_bufrxLen;
    int                     m_bufrxIdxOut;

    DWORD                   m_dwEntry_ThreadId;
    wxSemaphore             m_Sem;

    u_short                 m_peerPort;
    bool                    m_ignoreSocketErrors;
    bool                    m_EntryShould_Bail;
};

/*------------------------------------------------------------------*/
#endif

