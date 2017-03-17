/*****************************************************************
 *
 * Name:        transpTechsat.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transpTechsat.cpp,v 1.13 2012/05/31 19:02:59 macrae.i Exp $
 */

#include "transpTechsat.h"
#include "defs.h"
#include "log.h"
//#include "wx/socket.h"
#ifndef __WXMSW__
 #include "in.h"
#endif

extern int Ignore_This_Rx_Word(long r);

CTranspTechsat::CTranspTechsat() : m_Sem(1)
{
}

CTranspTechsat::~CTranspTechsat()
{
}


#define BUFLEN 10
bool CTranspTechsat::init ( wxEvtHandler * eventObjPtr, int port )
{
    int parity, rc, txspeed, rxspeed, trate;
    hmpc = 0;
    unsigned short rxstat;
    unsigned short res;

    char rx_character = gConfig.cfgArincRxChannel - 1 + '0';
    char tx_character = gConfig.cfgArincTxChannel - 1 + '0';

    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

    m_rxLen =0;
    m_rxIdxOut = 0;


   /* Determine device channel count */

    if (gConfig.cfgArincGroup > 1)     { LOG("init TechSat: Error...max group = 1"); return 0;}
    if (gConfig.cfgArincTxChannel > 2) { LOG("init TechSat: Error...max TX channel = 2"); return 0;}
    if (gConfig.cfgArincRxChannel > 4) { LOG("init TechSat: Error...max RX channel = 4"); return 0;}

    hmpc = _open_xpc( gConfig.cfgArincCard ); // base 1
    if (hmpc == INVALID_HANDLE_VALUE)
    {
        LOG("init TechSat: Unable to open device");
        return 0;

    }
    if ( ( rc = _verify_iflib_version( VERSION_NUMBER) ) != XPC_SUCCESS )
    {
        LOG("init TechSat: Verify TechSat iflib version failed");
        return 0;

    }

    // print Firmware version
    {

        unsigned short vers_no;
        short ipn;

        ipn = 0;
        if ((rc = _get_fw_version(hmpc,ipn,&vers_no)) == 0)
        {
            char msg[255];

            sprintf(msg,"init TechSat: firmware version is %d.%d",
                ((vers_no >> 4) & 0xf),(vers_no & 0xf));
            LOG(msg);
        }
        else
            LOG("init TechSat: Could not get Firmware version");

    }

    switch(gConfig.cfgArincParity)
    {
        case 0: parity = 0; break;
        case 1: parity = 0; break;
        default: parity = 1; break;
    }

    if (gConfig.cfgArincTxSpeed == 0) txspeed = 1;
    else txspeed = 0;

    if (gConfig.cfgArincRxSpeed == 0) rxspeed = 1;
    else rxspeed = 0;


    trate = 1; // keep always 1

    rc = _config_trans( hmpc,
                        tx_character,
                        txspeed,
                        trate,
                        parity);

    if (rc != XPC_SUCCESS)
    {
        LOG("init TechSat: Could not config TX");
        return 0;
    }

    m_tx.transmission_ID = 0;
    m_tx.transmit_interval = 0;

    // ---------------- config Rx -------------------

    rc = _config_rec(hmpc,rx_character,rxspeed);

    if (rc != XPC_SUCCESS)
    {
        LOG("init TechSat: Could not config RX");
        return 0;
    }

    // set rx buffer
    rc = _set_rec_buf_size(hmpc,rx_character,sizeof(m_rx),&res,&rxstat);
    if (rc != XPC_SUCCESS)
    {
        LOG("init TechSat: Could not config RX Buffer");
        return 0;
    }

    LOG("init TechSat: create thread");
    Create();
    Run();

    SetFinishedInit();
    return 1;
}

bool CTranspTechsat::sendWord ( long pTxWord, int mcdu )
{
    // if the hardware isn't ready, quietly bail
    if (IsFinishedInit() && Has_Transport_Started()) {

        unsigned short result, timeslice;
        int rc;
        char tx_character = gConfig.cfgArincTxChannel - 1 + '0';
        long txWord = (long) invertLabel(pTxWord);

        m_tx.data_low = txWord & 0x0000ffff;
        m_tx.data_high = (txWord >> 16) & 0x0000ffff;

        rc = _define_trans(hmpc,tx_character,1, &m_tx, &result, &timeslice);
        if (rc != XPC_SUCCESS)
        {
            LOG("Could not Define TX");
            return 0;
        }

        m_at.transmission_ID = 0;
        m_at.trigger_ID = 255;

        rc = _activate_trans(hmpc,tx_character,1, &m_at);
        if (rc != XPC_SUCCESS)
        {
            LOG("Could not Activate TX");
            return 0;
        }

    } // else hardware isn't available

    return false;

}

#define UNLOCK_RX() \
    if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); } \
    do { if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); }} while(0)

bool CTranspTechsat::getRxWord ( int connectionID, long * rxWord)
{
    int idx = 0;
    long r = 0L;

//  log_printf("rx a frame with len=%d\n", m_bufrxLen);

    if (m_rxLen <= 0 )
    {
        log_printf("!!! getRxWord called with no data available !!!\n", m_rxLen);
        //UNLOCK_RX();
        return false;
    }

    r = m_rx[m_rxIdxOut].arinc_high << 16;
    r |= m_rx[m_rxIdxOut].arinc_low;

    r = invertLabel(r);

    if (Ignore_This_Rx_Word(r)) {
        UNLOCK_RX();
        return false;
    }
    
    *rxWord = r;

    if (gdisplayLogScreen) {
        log_printf("raw RX data: 0x%08X\n", r);
    }

    m_rxIdxOut++;
    m_rxLen--;

    if (m_rxLen > 0)
        postEvent(MCDU_SIM___CUSTOM_EVENT_RX);
    else
    {
        UNLOCK_RX();
    }

    return true;
}

#define SLEEP_TIME          100         // 100 ms
#define SLEEP_THRESHOLD     100

void * CTranspTechsat::Entry()       // this is the RX thread
{
    //LOG("Rx thread started");
    unsigned short len;
    unsigned short res;

    int sleepCounter = 0;
    char rx_character = gConfig.cfgArincRxChannel - 1 + '0';
    unsigned short rstat;
    rx_mode_t rxm;
    int rc;
    //(unused)//int i;

    if ((rc = _activate_rec(hmpc,rx_character,1, &rxm,&rstat)) != 0)
    {
        LOG("Could Not activate RX");
    }

    Set_Transport_Started();    /* the TechSat hardware is ready for use */

    while(1)
    {
        len = m_rxLen = m_rxIdxOut = 0;

        //Code = Receive_FIFO( (ChannelType) (gConfig.cfgArincRxChannel - 1), &SAW);
//        rc = _get_data_ready(hmpc,rx_character);
//        if (rc == 1)
        {
            rc = _read_receiver_buffer(hmpc,rx_character,&m_rx[0],&res, &len);
            if (rc != 0)
            {
                LOG("RX Error");
                len = 0;
            }

        }

        if (len == 0)
        {
            if ( sleepCounter >= SLEEP_THRESHOLD) Sleep(SLEEP_TIME);
            else sleepCounter++;
            continue;
        }


        if (len>0)
        {

            // we have a valid word
            m_rxLen = len;
            m_rxIdxOut = 0;
            postEvent(MCDU_SIM___CUSTOM_EVENT_RX);

            // lock RX in a mutex
            if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }
            if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to lock RX thread")); }
            sleepCounter = 0;

        }

    } // while(1)

    //(im) If this function exits, the wxThread is deleted, which also deletes the
    //     transport driver. Unfortunately, other threads are still using it.
    //     So just sleep until the app exits.
    for(;;) Sleep(1000);

    return (void *)NULL; // never actually returns, but wxThread::Entry insists on returning a void *
}

unsigned long CTranspTechsat::invertLabel( unsigned long word)
{
    unsigned long label, mask = 1;
    unsigned long r = 0;
    int i;

    label = (word & 0x000000ff);

    for (i=0; i<8; i++) {
        r = r << 1;
        if (label & 1) r |= 1;
        label = label >> 1;
    }

    r |= (word & 0xffffff00);

    return r;
}

