#ifndef __TRANSPORTDDC_H__
#define __TRANSPORTDDC_H__
/*****************************************************************
 *
 * Name:        transportDDC.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportDDC.h,v 1.6 2012/01/26 16:21:48 macrae.i Exp $
 */
#include "transport.h"

#ifndef __WXMSW__
 #include "in.h"
#endif

#include "std429.h"
#include "config.h"

#define BUF_RX_LEN_TRANSPORTDDC  2048

class CTransportDDC : public CTransport, wxThread
{
public:
    CTransportDDC();
    ~CTransportDDC();
    virtual bool init ( wxEvtHandler * eventObjPtr, int port );
    virtual bool sendWord ( long txWord, int mcdu );
    virtual bool getRxWord ( int connectionID, long * rxWord);

    virtual void * Entry();

private:
    struct sockaddr_in      m_client_socket_addr;
    char                    m_bufrx[BUF_RX_LEN_TRANSPORTDDC];
    int                     m_bufrxLen;
    int                     m_bufrxIdxOut;
    U32BIT                  m_dwRxData;

    wxSemaphore             m_Sem;
};

/*------------------------------------------------------------------*/
#endif

