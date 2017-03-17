/*****************************************************************
 *
 * Name:        transportDDC.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportDDC.cpp,v 1.13 2012/05/31 19:02:59 macrae.i Exp $
 */
#include "transportDDC.h"
#include "defs.h"
#include "log.h"
//#include "wx/socket.h"
#ifndef __WXMSW__
 #include "in.h"
#endif

#define WANT_FILTERS                1   // 0=no filters, 1=normal, rejects other MCDU words
extern long gNoFilters;                 // in mcduSim.cpp, non-zero means don't use filters

// MCDU Address Label (or MAL): see ARINC 739A, Attachment 1 or Attachment 2
static const int m_mcduID[5] = { 0220LU, 0221LU, 0222LU, 0230LU, 0356LU}; //Append 5th Label for BITE

int Ignore_This_Rx_Word(long r) // return 1 to ignore it, 0 to accept it
{
    if (!gNoFilters) {          // command-line option -f means don't filter anything, default is to filter
        int label = r & 0xFF;
        int our_label = m_mcduID[gConfig.cfgMcdu - 1];
        int i;
        for (i=0; i<4; i++) {
            if (label == m_mcduID[i]) {
                if (label == our_label)
                    return 0;   // this is for us, accept it
                //return 0;     // TESTING!!! accept it anyway (see if the filter rejected it)
                return 1;       // this is for another MCDU, ignore it (if filter rejects properly, we'd never see it)
            }
        }
        // not sent specifically to an MCDU: better accept it
    } // not using filters, accept it
    return 0;
}

char * TransportDDC_ErrorCodeHelper(short error_code,
                                    char buffer[], size_t buffer_len,
                                    char * prefix_text)
{
    static char ErrorMessage[200];
    ErrorMessage[0] = '\0';
    GetErrorMsg( (S16BIT)error_code, ErrorMessage);

    if (!ErrorMessage[0] || !strcmp(ErrorMessage, "Unknown.")) {
        sprintf_s(ErrorMessage, sizeof(ErrorMessage), "Unknown Error (%d)", error_code);
    }
    sprintf_s(buffer, buffer_len, "DDC %s: %s", prefix_text, ErrorMessage);
    return buffer;
}

void TransportDDC_Log_Error(short error_code, char * prefix_text)
{
    char err_msg[200];
    TransportDDC_ErrorCodeHelper(error_code, err_msg, sizeof(err_msg), prefix_text);
    LOG(err_msg);
}

CTransportDDC::CTransportDDC() : m_Sem(1)
{
}

CTransportDDC::~CTransportDDC()
{
    EnableTx(gConfig.cfgArincCard, gConfig.cfgArincTxChannel, DD429_DISABLE);
    EnableRx(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, DD429_DISABLE);
    FreeCard(gConfig.cfgArincCard);
}


#define BUFLEN 10
bool CTransportDDC::init ( wxEvtHandler * eventObjPtr, int port )
{
    CHANCOUNT_t strChanCount;

    S16BIT wStatus;
#if WANT_FILTERS
    S16BIT wLabelSdi;
    char ignore_msg[200];
#endif
    U16BIT wLoopMode = DD429_ENABLE;
    U16BIT wSpeed = DD429_HIGH_SPEED;
    U16BIT wVerMajor, wVerMinora, wVerMinorb;

    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

    memset( m_bufrx, 0, sizeof(m_bufrx));
    m_bufrxLen = m_bufrxIdxOut = 0;

    int slen=sizeof(m_client_socket_addr);

//---------------------------------------------------
    GetLibVersionEx(&wVerMajor, &wVerMinora, &wVerMinorb);
    log_printf("DDC init (Card %d) RTL version: %d.%d.%d\n", gConfig.cfgArincCard, wVerMajor,wVerMinora,wVerMinorb);

    /* Initialize device */
    if((wStatus = InitCard(gConfig.cfgArincCard)) )
        goto init_fail;

    /* Determine device channel count */
    if ((wStatus = GetChannelCount(gConfig.cfgArincCard, &strChanCount)))
        goto init_fail;
    log_printf("DDC info: #Tx=%d #Rx=%d #Grp=%d #Disc=%d Model=%d\n", 
        strChanCount.bTx, strChanCount.bRx, strChanCount.bGroup, strChanCount.bDiscrete, strChanCount.bBoardModel);

    if (gConfig.cfgArincGroup > strChanCount.bGroup)  { LOGN("init DDC Error...max group = ",strChanCount.bGroup); return 0;}
    if (gConfig.cfgArincTxChannel > strChanCount.bTx) { LOGN("init DDC Error...max TX channel = ",strChanCount.bTx); return 0;}
    if (gConfig.cfgArincRxChannel > strChanCount.bRx) { LOGN("init DDC Error...max RX channel = ",strChanCount.bRx); return 0;}

    if((wStatus = EnableTx(gConfig.cfgArincCard, gConfig.cfgArincTxChannel, DD429_ENABLE)))
        goto init_fail;
    if((wStatus = SetTxSpeed(gConfig.cfgArincCard, gConfig.cfgArincTxChannel, gConfig.cfgArincTxSpeed)))
        goto init_fail;
    if((wStatus = SetTxParity(gConfig.cfgArincCard, gConfig.cfgArincTxChannel, gConfig.cfgArincParity)))
        goto init_fail;

    if((wStatus = EnableRx(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, DD429_ENABLE)))
        goto init_fail;
    if((wStatus = EnableTimeStamp(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, DD429_ENABLE)))
        goto init_fail;
    if((wStatus = SetRxSpeed(gConfig.cfgArincCard, gConfig.cfgArincGroup, gConfig.cfgArincRxSpeed)))
        goto init_fail;
    if((wStatus = SetRxParity(gConfig.cfgArincCard, gConfig.cfgArincGroup, gConfig.cfgArincParity)))
        goto init_fail;
    if((wStatus = SetRxMode(gConfig.cfgArincCard, gConfig.cfgArincGroup, FIFO_MODE)))
        goto init_fail;

#if WANT_FILTERS
    if (gNoFilters)         // command-line option -f means don't filter anything
        goto no_filters;

    ignore_msg[0] = '\0';
    if((wStatus = EnableFilter(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, DD429_ENABLE)))
        goto init_fail;

    for (wLabelSdi=1; wLabelSdi<1024; wLabelSdi++) {
        int label = wLabelSdi & 0xFF;
        int our_label = m_mcduID[gConfig.cfgMcdu - 1];
        int i;
        bool bIgnore = false;
        for (i=0; i<4; i++) {
            if (label == m_mcduID[i]) {
                if (label == our_label) {
                    break;      // this is for us, accept it
                }
                bIgnore = true; // this is for another MCDU, ignore it
                break;
            }
        }
        if (wLabelSdi<0x100/*don't show labels for all SDIs*/ && bIgnore) {
            size_t len_of_ignore_msg = strlen(ignore_msg);
            char * end_of_ignore_msg = ignore_msg + len_of_ignore_msg;
            sprintf_s(end_of_ignore_msg, sizeof(ignore_msg)-len_of_ignore_msg,
                                    " 0%03o($%02X)", wLabelSdi, wLabelSdi);
            continue;
        }

        S16BIT wfStatus = AddFilter(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, wLabelSdi);
        if (wfStatus!=1) {
            log_printf("DDC init: AddFilter(%d,%d,$%03X) failed, status %d\n", gConfig.cfgArincCard, gConfig.cfgArincRxChannel, wLabelSdi, wfStatus);
            wStatus = wfStatus;
            goto init_fail;
        }
    }
    
    if (ignore_msg[0]) {
        log_printf("DDC init: Ignore label%s\n", ignore_msg);
    }

no_filters:

#endif

    LOG("DDC init: create thread");
    Create();
    Run();

    SetFinishedInit();
    return 1;

init_fail:
    TransportDDC_Log_Error(wStatus, "init DDC card");
    return 0;
}

bool CTransportDDC::sendWord ( long txWord, int mcdu )
{
    // if the hardware isn't ready, quietly bail
    if (IsFinishedInit() && Has_Transport_Started()) {

        log_printf("Tx 0x%08X\n",txWord);
        short wStatus = LoadTxQueueMore(gConfig.cfgArincCard, gConfig.cfgArincTxChannel, 1, (U32BIT *) &txWord);
        if (wStatus == 1 ) return true;

        static short last_failure_code = (~0);
        if (last_failure_code != wStatus /* new error */ || LoggingThisTx() /* force it out */) {
            last_failure_code =  wStatus;
            TransportDDC_Log_Error(wStatus, "LoadTxQueueMore");
        } // otherwise probably same error as last time, say nothing

    } // else hardware isn't available
    return false;

}
#define UNLOCK_RX() \
    if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); } \
    do { if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); }} while(0)

bool CTransportDDC::getRxWord ( int connectionID, long * rxWord)
{
    //(unused)//int i;
    int idx = 0;
    //(unused)//long r = 0L;
    //(unused)//int dest, parity;

//    log_printf("rx a frame with len=%d\n", m_bufrxLen);

    if (m_bufrxLen != 1)
    {
        log_printf("!!! getRxWord called with no data available !!!\n", m_bufrxLen);
        //UNLOCK_RX();
        return false;
    }

    if (Ignore_This_Rx_Word(m_dwRxData)) {
        UNLOCK_RX();
        return false;
    }

    log_printf("raw rx data: 0x%08X\n", m_dwRxData);

    *rxWord = m_dwRxData;
    // unlock RX thread
    m_bufrxLen = 0;
    UNLOCK_RX();
    return true;
}

#define SLEEP_TIME          100         // 100 ms
#define SLEEP_THRESHOLD     100

void * CTransportDDC::Entry()       // this is the RX thread
{
    //LOG("Rx thread started");
    short len;
    int slen=sizeof(m_client_socket_addr);
    int sleepCounter=0;

    Set_Transport_Started();    /* the DDC hardware is ready for use */

    while(1)
    {
        m_bufrxLen = m_bufrxIdxOut = 0;

        len = GetRxQueueStatus(gConfig.cfgArincCard, gConfig.cfgArincRxChannel);

        if (len < 0)
        {
            TransportDDC_Log_Error(len, "GetRxQueueStatus");
            break;      // end thread
        }

        if (len == 0)
        {
            if ( sleepCounter >= SLEEP_THRESHOLD) Sleep(SLEEP_TIME);
            else sleepCounter++;
            continue;
        }

        if (len>0)
        {
            len = ReadRxQueueIrigOne(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, &m_dwRxData, NULL, NULL);

            if (len != 1)
            {
                TransportDDC_Log_Error(len, "ReadRxQueueIrigOne");
                break;      // end thread
            }

            // we have a valid word
            m_bufrxLen = 1;
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
