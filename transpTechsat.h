#ifndef __TRANSPTECHSAT_H__
#define __TRANSPTECHSAT_H__
/*****************************************************************
 *
 * Name:        transpTechsat.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transpTechsat.h,v 1.5 2012/01/13 18:28:30 macrae.i Exp $
 */
#include "transport.h"

#ifndef __WXMSW__
 #include "in.h"
#endif

#include "a429usb.h"
#include "config.h"


#define BUF_RX_LEN_TRANSPTECHSAT  128
//#define         EXTERN_SIM__FRAME_HEADER    0x01
//#define         EXTERN_SIM__FRAME_LEN       7
//#define         TERATERM_PORT               6000
//#define         TERATERM_IP                 "127.0.0.1"

class CTranspTechsat : public CTransport, wxThread
{
public:
    CTranspTechsat();
    ~CTranspTechsat();
    virtual bool init ( wxEvtHandler * eventObjPtr, int port );
    virtual bool sendWord ( long txWord, int mcdu );
    virtual bool getRxWord ( int connectionID, long * rxWord);
    unsigned long invertLabel( unsigned long word);


    virtual void * Entry();

private:
    wxSemaphore             m_Sem;

    HANDLE                  hmpc;
    tx_definition_t         m_tx;
    rx_data_t               m_rx[BUF_RX_LEN_TRANSPTECHSAT];
    int                     m_rxLen;
    int                     m_rxIdxOut;
    tx_activation_t         m_at;
};

/*------------------------------------------------------------------*/
#endif

