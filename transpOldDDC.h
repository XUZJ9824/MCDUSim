#ifndef __TRANSPOLDDDC_H__
#define __TRANSPOLDDDC_H__
/*****************************************************************
 *
 * Name:        transpOldDDC.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transpOldDDC.h,v 1.5 2012/01/20 20:53:44 macrae.i Exp $
 */
#include "transport.h"

#ifndef __WXMSW__
 #include "in.h"
#endif

#include "Types429.h"
#include "config.h"

#define BUF_RX_LEN_TRANSPOLDDDC  2048

class CTranspOldDDC : public CTransport, wxThread
{
public:
    CTranspOldDDC();
    ~CTranspOldDDC();
    virtual bool init ( wxEvtHandler * eventObjPtr, int port );
    virtual bool sendWord ( long txWord, int mcdu );
    virtual bool getRxWord ( int connectionID, long * rxWord);

    virtual void * Entry();

private:
    struct sockaddr_in      m_client_socket_addr;
    long                    m_bufrx[BUF_RX_LEN_TRANSPOLDDDC];
    int                     m_bufrxLen;
    int                     m_bufrxIdxOut;

    wxSemaphore             m_Sem;
};

/*------------------------------------------------------------------*/
#endif

