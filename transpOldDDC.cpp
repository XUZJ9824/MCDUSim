/*****************************************************************
 *
 * Name:        transpOldTcp.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transpOldDDC.cpp,v 1.14 2012/05/31 19:02:59 macrae.i Exp $
 */
#include "transpOldDDC.h"
#include "defs.h"
#include "log.h"
//#include "wx/socket.h"
#ifndef __WXMSW__
 #include "in.h"
#endif

extern int Ignore_This_Rx_Word(long r);

CTranspOldDDC::CTranspOldDDC() : m_Sem(1)
{
}

CTranspOldDDC::~CTranspOldDDC()
{
    CloseChannelAll();
    Shutdown();
}


#define BUFLEN 10
bool CTranspOldDDC::init ( wxEvtHandler * eventObjPtr, int port )
{
//    CHANCOUNT_t strChanCount;

    //U8BIT  bChan, bGroup, bNumTx, bNumRx, bNumGroup, bCount, bLoop;
    STATUS Code = PC_SUCCESS;
    int parity, speed = LOW_SPEED_ALL;


    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

    memset( m_bufrx, 0, sizeof(m_bufrx));
    m_bufrxLen = m_bufrxIdxOut = 0;

    int slen=sizeof(m_client_socket_addr);

//---------------------------------------------------
    LOG("trying Old DDC PCI Card");

    if ( SetCurrentCard(gConfig.cfgArincCard) != PC_SUCCESS)
    {
      LOG("Fail to Select DDC Card");
      return 0;
    }

   /* Initialize device */
    if ((Code = Initialize("pciexec.hex", NULL,
                (FARPROC) PC_NULLF, (FARPROC) PC_NULLF,
                NO_CHECK, TWO_MILLI_SEC,
                TWO_TX_FOUR_RX_PCI_CARD))    != PC_SUCCESS )
    {
        char Buf[40];
        StatusToAscii(Code, Buf);
        LOG("Initialize FAILED returning:");
        LOG(Buf);
        return(0);
    }


   /* Determine device channel count */

    if (gConfig.cfgArincGroup > 1) { LOG("Error...max group = 1"); return 0;}
    if (gConfig.cfgArincTxChannel > 2) { LOG("Error...max TX channel = 2"); return 0;}
    if (gConfig.cfgArincRxChannel > 4) { LOG("Error...max RX channel = 4"); return 0;}

    switch(gConfig.cfgArincParity)
    {
        case 0: parity = PCI__FORCE_TO_2_BYTES7; break;
        case 1: parity = PCI__ODD_PARITY; break;
        default: parity = PCI__EVEN_PARITY; break;
    }

    // open RX channel
    Code = OpenChannel( (ChannelType) (gConfig.cfgArincRxChannel - 1),
                        PC_FALSE,       // no Filter
                        PC_FALSE,       // no timestamp
                        PC_TRUE,        // FIFO mode
                        (ParityType) parity); /* plain FIFO */

    if (Code != PC_SUCCESS)
    {
        char Buf[40];
        StatusToAscii(Code, Buf);
        LOG("Initialize FAILED returning:");
        LOG(Buf);
        return(0);
    }


    // open TX channel
    Code = OpenChannel( (ChannelType) (T0 + gConfig.cfgArincTxChannel - 1),
                        PC_FALSE,       // no Filter
                        PC_FALSE,       // no timestamp
                        PC_TRUE,        // FIFO mode
                        (ParityType) parity); /* plain FIFO */

    if (Code != PC_SUCCESS)
    {
        char Buf[40];
        StatusToAscii(Code, Buf);
        LOG("Initialize FAILED returning:");
        LOG(Buf);
        return(0);
    }

    if (gConfig.cfgArincTxSpeed) speed = HI_SPEED_ALL;
    Code = SetChipSpeed( CHIP_A, (SpeedType) speed);
    if (Code != PC_SUCCESS)
    {
        char Buf[40];
        StatusToAscii(Code, Buf);
        LOG("Initialize FAILED returning:");
        LOG(Buf);
        return(0);
    }

    Code = SetChipSpeed( CHIP_B, (SpeedType) speed);


    LOG("Call create thread");
    Create();
    Run();

    SetFinishedInit();
    return 1;
}

bool CTranspOldDDC::sendWord ( long txWord, int mcdu )
{
    // if the hardware isn't ready, quietly bail
    if (IsFinishedInit() && Has_Transport_Started()) {

        ArincRawWordType AW;
        STATUS Code = PC_SUCCESS;
        bool bShowTxAndBadStatus = LoggingThisTx();

        AW.i32 = txWord;

        if (gdisplayLogScreen) {
            if (bShowTxAndBadStatus) log_printf ("Tx 0x%X\n",txWord);
            bLog_nextTx = true; // next one might be interesting
        }

        Transmit_FIFO ( (ChannelType) (T0 + gConfig.cfgArincTxChannel - 1), &AW);

        if (Code!=PC_SUCCESS && bShowTxAndBadStatus) {
            switch (Code) {
            case QUEUE_IS_FULL: log_printf("Tx queue is full\n"); break;
            case BAD_ARGUMENT:  log_printf("Transmit_FIFO with Invalid parameter\n"); break;
            default:            log_printf("Transmit_FIFO with unknown error %d\n", Code); break;
            }
        }
    } // else hardware isn't available

    return false;
}

#define UNLOCK_RX() \
    if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); } \
    do { if (m_Sem.Post() != wxSEMA_NO_ERROR) { CATFAIL(wxT("Fail to unlock RX thread")); }} while(0)

bool CTranspOldDDC::getRxWord ( int connectionID, long * rxWord)
{
    int idx = 0;
//  long r = 0L;

//  log_printf("rx a frame with len=%d\n", m_bufrxLen);

    if (m_bufrxLen != 1)
    {
        log_printf("!!! getRxWord called with no data available !!!\n", m_bufrxLen);
        //UNLOCK_RX();
        return false;
    }

    if (Ignore_This_Rx_Word(m_bufrx[0])) {
        UNLOCK_RX();
        return false;
    }

    if (gdisplayLogScreen) {
        log_printf("raw rx data: 0x%08X\n", m_bufrx[0]);
    }

    *rxWord = m_bufrx[0];
    // unlock RX thread
    m_bufrxLen = 0;
    UNLOCK_RX();
    return true;
}

#define SLEEP_TIME          100         // 100 ms
#define SLEEP_THRESHOLD     100

void * CTranspOldDDC::Entry()       // this is the RX thread
{
    //LOG("Rx thread started");
    short len;
    int slen=sizeof(m_client_socket_addr);

    ArincWordType SAW;
    STATUS Code = PC_SUCCESS;
    int sleepCounter = 0;

    Set_Transport_Started();    /* the DDC hardware is ready for use */

    while(1)
    {
        len = m_bufrxLen = m_bufrxIdxOut = 0;

        Code = Receive_FIFO( (ChannelType) (gConfig.cfgArincRxChannel - 1), &SAW);

        switch(Code)
        {
            case PC_SUCCESS: len = 1; /*LOG("Got an ARINC word");*/ break;
            case DATA_WAS_LOST: LOG("Rx Queue overrun"); break;
            case QUEUE_IS_EMPTY:
                {
                    if ( sleepCounter >= SLEEP_THRESHOLD) Sleep(SLEEP_TIME);
                    else sleepCounter++;
                    continue;
                }
            case BAD_ARGUMENT: LOG("Receive_FIFO called with Invalid argument"); break;
            default: LOG("unknown Receive_FIFO error"); break;
        }


        if (len>0)
        {

            // we have a valid word
            m_bufrxLen = 1;
            m_bufrx[0] = SAW.R.i32;
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

