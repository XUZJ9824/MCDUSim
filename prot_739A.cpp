/*****************************************************************
 *
 * Name:        prot_739A.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: prot_739A.cpp,v 1.19 2012/05/31 19:02:59 macrae.i Exp $
 */

#define WANT_SAL_TIMEOUT_DISABLE    0   // 0=normal, 1=debug
#define WANT_SHOW_DATA_TO_MCDU      1   // 0=normal, 1=debug
#define WANT_TRACE_PROCESS_STATE    1   // 0=normal, 1=debug
#define WANT_SHOW_NOT_OUR_MCDU      0   // 0=normal, 1=debug (WARNING: 1 may lose data with 3 MCDUs(
#define WANT_TRACE_KEYBOARD         1   // 0=normal, 1=debug
#define WANT_TRACE_MCDU_MENU_QUEUE  1   // 0=normal, 1=debug
#define WANT_TRACE_STATE_CHANGES    1   // 0=normal, 1=debug
#define WANT_VERBOSE_TRACE          0   // 0=normal, 1=debug (verbose when other debugging turned on)
#define WANT_DEBUG_SCRATCHPAD       0   // 0=normal, 1=debug
#define WANT_TRACE_WMBURG_PROTO     1   // 0=normal, 1=debug, show all Rx words
#define WANT_SHOW_NON_DATA_RX       1   // 0=normal, 1=debug, show all Rx hex words except for Williamsburg data
#define WANT_TO_SEE_IGNORED_RX      0   // 0=normal, 1=debug
#define WANT_DEBUG_STARTUP          1   // 0=normal, 1=debug startup/shutdown protocol (heartbeat and SAL/MAL exchange)
#define WANT_DEBUG_TRANSPORT_STATUS 0   // 0=normal, 1=debug

#include "prot_739A.h"
#include "log.h"
#include "transportDDC.h"
#include "transportPipe.h"
#include "transportUdp.h"
#include "transpTechsat.h"
#ifdef __WXMSW__
 #include "transpOldDDC.h"      /* just Windows has support for old PCIs */
#endif

#define PROC_STATE_SOURCE_RX_WORD   0       //
#define PROC_STATE_SOURCE_TIMER     1

#define UDP_PORT_BASE   5001        // for mcdu 0

#define HSD_SAL_TIMEOUT  30 // expect another SAL in 30/4 = 7.5 seconds (unless WANT_SAL_TIMEOUT_DISABLE)
#define SEND_MCDU_ID_TIME 4 // send MCDU_ID(0xE4FF) every 4/4 = 1 second (when state == 0), see OnTimer250ms

// see ARINC 739A-1 section 3.7.2.1.2
#define ENQ_RTS_TIMEOUT   4 // wait 4/4 = 1 second after sending ENQ to see if RTS arrived
#define ENQ_RTS_RETRIES   3 // and retry up to 3 times before givng up

#define NORMAL_TEXT_REQ          0
#define MENU_TEXT_REQUEST        1

extern long cfgArincCard;

enum enu_state
{
    STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION = 0,
    STATE__WAITING_TO_SEND_ENQ,
    STATE__WAITING_RTS,
    STATE__WAITING_STX,
    STATE__WAITING_DATA_OR_CONTROL,
    STATE__WAITING_DATA,
    STATE__WAITING_ETX_OR_EOT

};

const int m_mcduID[5] = { 0220LU, 0221LU, 0222LU, 0230LU, 0356LU}; //Append 5th Label for BITE
//const int m_mcduID[4] = {0356LU, 0356LU, 0356LU, 0356LU};

void OnRx_Helper(void * pThis, long wordRx, int which_mcdu)
{
	//
	//TBD Removed. wordRx = 0x0000E37A; 

    ((CProt *)pThis)->processState (PROC_STATE_SOURCE_RX_WORD, wordRx);
}

void OnReaderFail_helper(void * pThis, int which_mcdu)
{
    ((CProt *)pThis)->OnReaderWriterFail(which_mcdu, "Reader");
}

void OnWriterFail_helper(void * pThis, int which_mcdu)
{
    ((CProt *)pThis)->OnReaderWriterFail(which_mcdu, "Writer");
}

void CProt::CProt_Init(void) // handy place to initialize or re-initialize everything
{
    m_mcduIDTimer = 1;
    m_enq_rts_timer = 0;
    m_enq_rts_retries = 0;

    m_hsd_sal_timer = 0;

    m_simul_hsd_sal = 0;
    m_state = 0;
}

CProt::CProt(u_short peerPort, bool ignoreSocketErrors) :
        m_peerPort(peerPort), m_ignoreSocketErrors(ignoreSocketErrors),
        m_screenPtr(NULL), m_menuScreenPtr(NULL),
        m_mcdu(0), m_transport(NULL)
{
    CProt_Init();

    //CTransportTcp * temp1 = new CTransportTcp();
    //m_transport = temp1;
    m_transport = new CTransportTcp(peerPort, ignoreSocketErrors);

    m_transport->init(this, UDP_PORT_BASE);
}

CProt::CProt( int mcdu, wxEvtHandler * eventObjPtr, u_short peerPort, bool ignoreSocketErrors) :
        m_parentEventObjPtr(eventObjPtr),
        m_peerPort(peerPort), m_ignoreSocketErrors(ignoreSocketErrors),
        m_screenPtr(NULL), m_menuScreenPtr(NULL),
        m_mcdu(mcdu), m_transport(NULL)
{
    // create a new transport object
    UpdateDriver(mcdu);
}

void CProt::UpdateDriver(int mcdu) // used to re-init driver layer after startup
{
    int init_arg_value = mcdu;
    wxString connectionType = wxT("unknown");
    bool want_transport_pipe = false;

    CProt_Init();

    // if there was a transport layer, get rid of the old one. hope it goes away quietly
    if (m_transport) {
        delete m_transport;
        m_transport = NULL;
    }

    // start a new transport layer
    switch (gConfig.cfgConnectionType) {

    case CONNECTION_TYPE_UDP:
    {
        wxString connection_error_msg;

        connectionType = wxT("UDP software");
        LOG("Connection Type: UDP");
        init_arg_value = UDP_PORT_BASE + mcdu;
        m_transport = new CTransportUdp(mcdu, gConfig.cfgDimIpAddress, gConfig.cfgDimBasePort, gConfig.cfgLocalBasePort);
        if (m_transport->init(this, mcdu) == false)
        {
            connection_error_msg.Printf(
                wxT("Connection Error: %s not found!\r\n\r\nSimulated MCDU connection (Transport Pipe) failed too!"),
                                                connectionType );
            (void)wxMessageBox(connection_error_msg, _T("MCDU Simulator"), wxOK | wxICON_ERROR);
            return;
        }

        ((CTransportUdp *)m_transport)->SetRxCallback(this, OnRx_Helper);

        // let the Transport Pipe know what to do when it detects a pipe failure
        //((CTransportUdp *)m_transport)->Set_ReaderFail_Callback(this, OnReaderFail_helper);
        //((CTransportUdp *)m_transport)->Set_WriterFail_Callback(this, OnWriterFail_helper);

        break;
    }    
    case CONNECTION_TYPE_DDC:
        connectionType = wxT("ARINC DDC hardware");
        LOG("Connection Type: ARINC DDC");
        m_transport = new CTransportDDC();
        break;
    
    case CONNECTION_TYPE_TECHSAT:
        connectionType = wxT("ARINC TechSat hardware");
        LOG("Connection Type: ARINC TechSat");
        m_transport = new CTranspTechsat();
        break;

    case CONNECTION_TYPE_PIPE:
        //connectionType = wxT("Simulated MCDI to DIM software");
        //LOG("Connection Type: Simulated (pipe)");
        //m_transport = new CTransportPipe(mcdu);
        m_transport = NULL;
        want_transport_pipe = true;
        break;

    default:
        break;
    } /*switch*/

    // try to init using chosen driver (unless we didn't recognize the choice above)
    if (m_transport==NULL || m_transport->init(this, init_arg_value) == false) {
        // did not work
        wxString connection_error_msg;
        delete m_transport;

        //(im) I wonder why we create temp3, then use temp2 ???
        //temp3 = new CTranspOldDDC();
        //m_transport = temp2;
        //m_transport = new CTransportDDC();

        //(im) as of July 2011, use Transport Pipe to talk to the Display Interface when no hardware available
        m_transport = (CTransport *)new CTransportPipe(mcdu, /*s_DI_Host*/".");
        if (m_transport->init(this, mcdu) == false)
        {
            connection_error_msg.Printf(
                wxT("Connection Error: %s not found!\r\n\r\nSimulated MCDU connection (Transport Pipe) failed too!"),
                                                connectionType );
            (void)wxMessageBox(connection_error_msg, _T("MCDU Simulator"), wxOK | wxICON_ERROR);
            return;
        }

        if (!want_transport_pipe) {
            connection_error_msg.Printf(
                wxT("Connection Error: %s not found!\r\n\r\nUsing Simulated MCDU connection (Transport Pipe)\r\nfor use with Simulated Display Interface Manager"),
                                                    connectionType );
            (void)wxMessageBox(connection_error_msg, _T("MCDU Simulator"), wxOK | wxICON_INFORMATION);
        } // else doing what was requested, no need for error

        // let the Transport Pipe know what to do with incoming data
        ((CTransportPipe *)m_transport)->SetRxCallback(this, OnRx_Helper);

        // let the Transport Pipe know what to do when it detects a pipe failure
        ((CTransportPipe *)m_transport)->Set_ReaderFail_Callback(this, OnReaderFail_helper);
        ((CTransportPipe *)m_transport)->Set_WriterFail_Callback(this, OnWriterFail_helper);
    } /* else chosen transport driver started up OK */
}

void CProt::setScreen(CTermScreen * screenPtr, CTermScreen * menuScreenPtr)
{
    m_screenPtr = screenPtr;
    m_menuScreenPtr = menuScreenPtr;
    //m_screenPtr->printText(0,0,(char *) "<<<<<<< CProt >>>>>>>",0,C_GREEN,0);
}

CProt::~CProt()
{
    delete m_transport;
    m_transport = NULL;
}

BEGIN_EVENT_TABLE(CProt, wxEvtHandler) /*wxFrame)*/
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_RX, CProt::OnRx)
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_CONNECTION, CProt::OnConnect)
    EVT_MENU(MCDU_SIM___CUSTOM_EVENT_DISCONNECTION, CProt::OnDisconnect)
END_EVENT_TABLE()

void CProt::OnRx(wxCommandEvent& event)
{
    if (m_transport && m_transport->IsFinishedInit()) {
        long wordRx;

        //LOG("CProt::OnRx called");
        if ((m_transport->getRxWord(0, &wordRx)) == false)
        {
            log_printf("!!!!!! RX error 0x%08X\n", wordRx);
        }
        else {
            processState (PROC_STATE_SOURCE_RX_WORD, wordRx);
        }
    }
}

void CProt::OnConnect(wxCommandEvent& event)
{
    LOG("CProt::OnConnect called");
}

void CProt::OnDisconnect(wxCommandEvent& event)
{
    LOG("CProt::OnDisconnect called");
}


void CProt::send(unsigned long data, unsigned long label)
{
    if (m_transport && m_transport->IsFinishedInit()) {
        /*(im) is this function ever used? */
        ::DebugBreak();

        unsigned long wordTx = shuffle_arinc_data(data, label);
        m_transport->sendWord( (long) wordTx, m_mcdu );
    }
}

unsigned long CProt::unshuffle_arinc_data(unsigned long arinc_word,
  unsigned long *label)
{
    unsigned long lval;
    unsigned long temp;

    lval = 0L;

    /*** extract the data ***/

    /* bits 0 thru 15 correspond to bits 5 thru 20 of the data */
    lval = (arinc_word & 0xffffL) << 5;

    /* bits 25 thru 26 correspond to bits 21 thru 22 of the data */
    temp = (arinc_word & 0x6000000L) >> 4;
    lval |= temp;

    /* bits 27 thru 31 correspond to bits 0 thru 4 of the data */
    temp = (arinc_word & 0xf8000000L) >> 27;
    lval |= temp;

    /* bit 24 is the parity bit and is not used by the software */

    /*** build the label ***/

    /* bits 16 thru 23 are the label */
    *label = (arinc_word & 0x00ff0000L) >> 16;

    return lval ;
}

unsigned long CProt::shuffle_arinc_data(unsigned long data,unsigned int label)

/* this function builds a 23 bit data value (parity bit set by */
/* the ARINC chipset) with fields and bits in expected positions, and then */
/* reformats the data with the label into positions expected by the */
/* chipset. */

{
    unsigned long lval;
    unsigned long temp;

    lval = 0L;

    /* ARINC bits 0 thru 15 correspond to bits 5 thru 20 of our data */
    lval = (data & 0x001fffe0L) >> 5;

    /* ARINC bits 25 thru 26 correspond to bits 21 thru 22 of our data */
    temp = (data & 0x00600000L) << 4;
    lval |= temp;

    /* ARINC bits 27 thru 31 correspond to bits 0 thru 4 of our data */
    temp = (data & 0x000001fL) << 27;
    lval |= temp;

    /* bit 24 is the parity bit, and is set by the tx h/w to generate odd */
    /* parity */

    /* ARINC bits 16 thru 23 are the label */
    lval |= ((unsigned long) label) << 16;

    return(lval);
}

unsigned long CProt::invert_bits(unsigned long data,int num_bits)

/* inverts a bitfield containing 'num_bits' */
{
    unsigned long rval;

    rval = 0L;

    while(num_bits--) {
        rval <<= 1;
        rval |= data & 0x1;
        data >>= 1;
    }
    return(rval);
}

unsigned long CProt::add_parity_and_ssm(unsigned long arinc_word,
  float percent_ssm_errors,int ssm)
{
#if 0
    int ival;

    if(percent_ssm_errors != 0.) {
        /* we may want to toggle one of the ssm bits */
        ival = get_random_non_negative_integer(100);
        if(ival < percent_ssm_errors) {
            /* at least one bit will toggle */
            ival = get_random_non_negative_integer(300);
            if(ival > 200) ssm ^= 1;
            else if(ival > 100) ssm ^= 2;
            else ssm ^= 3;
        }
    }
#endif

    /* add ssm bits 30 and 31 (referenced to 1) */
    arinc_word |= ((unsigned long) ssm) << 29;

    /* add the parity bit */
    arinc_word = add_parity_bit(arinc_word);

    return(arinc_word);
}

unsigned long CProt::add_parity_bit(unsigned long arinc_word)

/* parity of arinc words must be odd */

{
    int count_bits;
    unsigned long test;

    test = arinc_word;

    count_bits = 0;
    while(test) {
        if(test & 1) count_bits++;
        test >>= 1;
    }
    if((count_bits & 1) == 0) {
        /* we had an even number of bits in the word */
        return(arinc_word | 0x80000000L);
    }
    return(arinc_word);
}

char *interpret_state(/*enum enu_state*/int state)
{
    switch (state) {
    case STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION  : return "WAITING_FIRST_ID";
    case STATE__WAITING_TO_SEND_ENQ                     : return "WAITING_SEND_ENQ";
    case STATE__WAITING_RTS                             : return "WAITING_RTS";
    case STATE__WAITING_STX                             : return "WAITING_STX";
    case STATE__WAITING_DATA_OR_CONTROL                 : return "WAITING_DATA_OR_CONTROL";
    case STATE__WAITING_DATA                            : return "WAITING_DATA";
    case STATE__WAITING_ETX_OR_EOT                      : return "WAITING_ETX_OR_EOT";
    default                                             : break;
    }
    return "UNKNOWN";
}
        
void CProt::processState(int source, long par)
{
    unsigned long label;
    unsigned long data, arinc_word;

    unsigned long function;
    static unsigned long record_count;
    static long num_records_remaining;
    static int remaining_words_in_record;

    bool processed_this_word = false;

#if 1 /* just for test */
    data = (par >> 8) & 0x00ffffff;
    label = par & 0x000000ff;
#else
    data = unshuffle_arinc_data( (unsigned long) par, &label);
#endif
    bool bShowTrace = false;
    #if WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE
    bShowTrace = true;
    #endif
    #if WANT_SHOW_NON_DATA_RX
    if (m_state != STATE__WAITING_DATA_OR_CONTROL && m_state != STATE__WAITING_DATA)
        bShowTrace = true;
    #endif
    if (bShowTrace) {
        log_printf("processState() => 0x%06X 0%03o; state %s\n", data, label, interpret_state(m_state));
    }

    /* watch out for scratchpad message */
    /* these contain a single character which is always written to */
    /* line scratchpad_row (usually 14 for standard, 10 for TCP)*/

    if (label == 0172)
    {
        unsigned long rec_simul_hsd_sal = invert_bits(data,8);
        /* (im) Special case, the MCDU received the heartbeat/SAL from the SDU (with a label of 0172),
         *      so both ends are are still communicating ... at least for another few seconds.
         *      If that stops, the MCDU declares a SAL timout and automatically logs off.
         */
        Setup_SAL_Timeout();

        #if WANT_DEBUG_TRANSPORT_STATUS
        log_printf("Transport status = start %X, fail %X (got SAL)\n", m_transport->Has_Transport_Started(), m_transport->Has_Transport_Failed());
        #endif //WANT_DEBUG_TRANSPORT_STATUS

        // If the DI has shut down and/or restarted,
        // the MCDU should restart this side of the conversation too.
        if (m_transport->Has_Transport_Started()) {

            if  ( getSduSAL() == rec_simul_hsd_sal ) {
                #if WANT_TO_SEE_IGNORED_RX
                log_printf("processState: got SAL 0%03o, already had SAL 0%03o -- ignore it; state %s\n",
                    rec_simul_hsd_sal, getSduSAL(), interpret_state(m_state));
                #endif
                return;
            }
            /* else got a new SAL, is that even allowed? */

        } /* else not started yet, or haven't recovered from failure yet */

        /* keep track of the SAL, use in outgoing messages to the SDU */
        setSduSAL (rec_simul_hsd_sal);

        m_state = STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION;
        #if WANT_TRACE_PROCESS_STATE || WANT_DEBUG_STARTUP
        log_printf("processState: got new SAL 0%03o\n", getSduSAL());
        #endif
        #if WANT_TRACE_STATE_CHANGES || WANT_DEBUG_STARTUP || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
        LOG1S("... new state ", interpret_state(m_state));
        #endif

        processed_this_word = true; /* received SAL from SDU */
    }
    else
    if  ( label != (unsigned long ) m_mcduID[m_mcdu])
    {
        #if WANT_TRACE_PROCESS_STATE
        #if WANT_SHOW_NOT_OUR_MCDU
        LOG("processState: discard, not our MCDU");
        #endif
        #endif
        return;
    }

    /*(im) Whenever the MCDU receives anything from the SDU (with our MAL in the label),
     *     both ends are are still communicating ... at least for another few seconds.
     *     If that stops happening, MCDU declares a SAL timout and automatically logs off.
     *     But until that timeout happens (if ever), everything is good...
     */
    Setup_SAL_Timeout();

    if(((data >> 16) & 0x7f) == 0x11) {
        char c[2];
        c[0] = (data >> 8) & 0x7f;
        c[1] = 0;

        int nRow    = (scratchpad_row-1);   /* scratchpad row# */
        int nCol    = (data & 0x1f) - 1;
        int nSize   = (data >> 15) & 1;
        int nColor  = (data >> 5) & 7;
        int nAttr   = 0;                   /* no attributes for scratchpad characters */

        #if WANT_TRACE_PROCESS_STATE
        extern char * LookupKey(int key); // in mcdusim.c
        log_printf("processState: got Scratch row=%d col=%d size=%d color=%d char=%02X {%s}\n",
                                nRow, nCol, nSize, nColor, c[0], LookupKey(c[0]));
        #endif

        /* send an ACK to the HSD */
        arinc_word = getSduSAL();
        arinc_word |= 0x6LU << 24;      /* ACK */
        #if WANT_TRACE_WMBURG_PROTO
        LOG("send ACK for scratch");
        #endif
        m_transport->sendWord( (long) arinc_word, m_mcdu );

        #if WANT_TRACE_MCDU_MENU_QUEUE || WANT_DEBUG_SCRATCHPAD
        log_printf(" >>> %3d %3d cr %d at %X sz %d 0x%02X scratchpad\n", 
                            nRow, nCol, nColor, nAttr, nSize, c[0]);
        #endif

        m_screenPtr->printText( nRow,   //int row,
                                nCol,   //int col,
                                c,      //char * text,
                                nSize,  //int size,
                                nColor, //int color,
                                0);     //int attrib);

        processed_this_word = true; /* single character to scratchpad */
    }

    /* watch out for Discrete Word */
    /* these contain a single character which is always written to */
    /* line 14 for standard, 10 for TCP */

    if(((data >> 16) & 0x7f) == 0x14) {

        #if WANT_TRACE_PROCESS_STATE
        LOG1H("processState: got Discrete word", data);
        #endif

        if ( ( data & 2 ) ||        // if Blank screen req or
             ( data & 4 ) )         // if Clear display buffer
             m_screenPtr->clear();

        /* send an ACK to the HSD */
        arinc_word = getSduSAL();
        arinc_word |= 0x6LU << 24;      /* ACK */
        #if WANT_TRACE_WMBURG_PROTO
        LOG("send ACK for Discrete Word");
        #endif
        m_transport->sendWord( (long) arinc_word, m_mcdu );

        processed_this_word = true; /* discrete word: special ACK from HSD */
    }

    if(((data >> 16) & 0x7f) == 0x06) {
        #if WANT_TRACE_PROCESS_STATE
        LOG("processState: got Python AckRx request");
        #endif
        postEvent(MCDU_SIM___CUSTOM_EVENT_ACK_RX);

        processed_this_word = true; /* Python ACK due to single key/button press from MCDU to SDU */
    }

    switch (m_state)
    {
        case STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION:
            if(label == 0172LU)
            {

                #if 0 /* (im) we already did this up above in the special case */
                /* HSD is sending its SAL */
                Setup_SAL_Timeout();
                setSduSAL( invert_bits(data,8) );
                log_printf("got SAL     0%03o\n", getSduSAL());
                #endif

                #if 0
                // send MCDU identifier TODO: process from a timer
                sendMcduID();
                #endif

                // send ENQ
                data = (0x5L << 16);                /* ENQ */
                data |= (MENU_TEXT_REQUEST << 8);
                data |= invert_bits(m_mcduID[m_mcdu],8);
                /* the function is 0 -- normal request */
                /* we only send this when we want the SATCOM to be the master */
                arinc_word = getSduSAL();
                arinc_word |= data << 8;
                //arinc_word = add_parity_and_ssm(arinc_word,0,0);
                #if WANT_TRACE_WMBURG_PROTO
                LOG("send ENQ");
                #endif
                if (m_transport->sendWord( (long) arinc_word, m_mcdu ))
                { /* sendWord() = true = success */
                    Setup_ENQ_RTS_Timeout(); /* sent ENQ, start waiting for incoming RTS */

                    //send(arinc_word, );
                    m_state = STATE__WAITING_RTS;
                    #if WANT_TRACE_STATE_CHANGES
                    LOG1S("... new state ", interpret_state(m_state));
                    #else
                    #if WANT_TRACE_WMBURG_PROTO
                    #if WANT_VERBOSE_TRACE
                    LOG1S("... new state ", interpret_state(m_state));
                    #else //!WANT_VERBOSE_TRACE
                    LOG("... waiting RTS");
                    #endif //!WANT_VERBOSE_TRACE
                    #endif
                    #endif
                } else { /* sendWord() = false = failure */
                    /*(im) failed to send ENQ, but we did get a SAL: keep trying to send the ENQ */
                    m_state = STATE__WAITING_TO_SEND_ENQ;
                    // failed to send ENQ: stay in STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION
                    LOG("... send ENQ failed, no transport");
                    #if WANT_TRACE_STATE_CHANGES
                    LOG1S("... new state ", interpret_state(m_state));
                    #else
                    #if WANT_TRACE_WMBURG_PROTO
                    #if WANT_VERBOSE_TRACE
                    LOG1S("... new state ", interpret_state(m_state));
                    #else //!WANT_VERBOSE_TRACE
                    LOG("... waiting SEND ENQ");
                    #endif //!WANT_VERBOSE_TRACE
                    #endif
                    #endif
                }

            }
            else {
                log_printf("expecting SAL, got 0x%06X\n", data);
            }
            break;

        case STATE__WAITING_TO_SEND_ENQ:
            // SAL should still be good
            //  log_printf("    SAL     0%03o\n", getSduSAL());

                // send ENQ
                data = (0x5L << 16);                /* ENQ */
                data |= (MENU_TEXT_REQUEST << 8);
                data |= invert_bits(m_mcduID[m_mcdu],8);
                /* the function is 0 -- normal request */
                /* we only send this when we want the SATCOM to be the master */
                arinc_word = getSduSAL();
                arinc_word |= data << 8;
                //arinc_word = add_parity_and_ssm(arinc_word,0,0);
                #if WANT_TRACE_WMBURG_PROTO
                LOG("send ENQ");
                #endif
                if (m_transport->sendWord( (long) arinc_word, m_mcdu ))
                { /* sendWord() = true = success */
                    Setup_ENQ_RTS_Timeout(); /* sent ENQ, start waiting for incoming RTS */

                    //send(arinc_word, );
                    m_state = STATE__WAITING_RTS;
                    #if WANT_TRACE_STATE_CHANGES
                    LOG1S("... new state ", interpret_state(m_state));
                    #else
                    #if WANT_TRACE_WMBURG_PROTO
                    #if WANT_VERBOSE_TRACE
                    LOG1S("... new state ", interpret_state(m_state));
                    #else //!WANT_VERBOSE_TRACE
                    LOG("... waiting RTS");
                    #endif //!WANT_VERBOSE_TRACE
                    #endif
                    #endif
                } else { /* sendWord() = false = failure */
                    m_state = STATE__WAITING_TO_SEND_ENQ;
                    // failed to send ENQ: stay in STATE__WAITING_FIRST_SUBSYSTEM_IDENTIFICATION
                    LOG("... send ENQ failed, no transport");
                    #if WANT_TRACE_STATE_CHANGES
                    LOG1S("... new state ", interpret_state(m_state));
                    #else
                    #if WANT_TRACE_WMBURG_PROTO
                    #if WANT_VERBOSE_TRACE
                    LOG1S("... new state ", interpret_state(m_state));
                    #else //!WANT_VERBOSE_TRACE
                    LOG("... waiting SEND ENQ");
                    #endif //!WANT_VERBOSE_TRACE
                    #endif
                    #endif
                }
            break;

        case STATE__WAITING_RTS:
            if(((data >> 16) & 0xff) == 0x12L) {
                /* RTS word */
                m_rst_function = (data >> 8) & 0x0f;
                #if WANT_TRACE_WMBURG_PROTO
                LOG1N("*********** got RTS ************* Function:", m_rst_function);
                #endif
                Cancel_ENQ_RTS_Timeout(); /* got RTS, everybody can relax now */

                record_count = data & 0x7f;
                num_records_remaining = record_count;
                function = (data >> 8) & 0xf;
                arinc_word = getSduSAL();
                arinc_word |= 0x13LU << 24;     /* DC3 (=CTS) */
                arinc_word |= record_count << 16;
                #if WANT_TRACE_WMBURG_PROTO
                log_printf("send CTS, %3d\n", record_count);
                #endif
                m_transport->sendWord( (long) arinc_word, m_mcdu );

                if(record_count == 0)
                {
                    LOG("fix me");
                    arinc_word = getSduSAL();
                    arinc_word |= data << 8;
                    //arinc_word = add_parity_and_ssm(arinc_word,0,0);
                    #if WANT_TRACE_WMBURG_PROTO
                    LOG("send garbage!");
                    #endif
                    m_transport->sendWord( (long) arinc_word, m_mcdu );
                }

                else {
                    m_state = STATE__WAITING_STX;
                    #if WANT_TRACE_STATE_CHANGES || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
                    LOG1S("... new state ", interpret_state(m_state));
                    #endif
                }
            }
            else {
                if (!processed_this_word) {
                    log_printf("expecting RTS, got 0x%06X\n", data);
                }
            }
            break;

        case STATE__WAITING_STX:
            if(((data >> 16) & 0xff) == 0x2L) {             /* STX */
                #if WANT_TRACE_WMBURG_PROTO
                log_printf("got STX     %3d %3d\n", ((data>>8) & 0xff), ((data & 0xff)-1));
                #endif
                remaining_words_in_record = data & 0xff;
                remaining_words_in_record--;    /* count includes STX */
                m_state = STATE__WAITING_DATA_OR_CONTROL;   /* expecting CTRL or DATA */
                #if WANT_TRACE_STATE_CHANGES || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
                LOG1S("... new state ", interpret_state(m_state));
                #endif
            }
            else
            {
                if (!processed_this_word) {
                    log_printf("expecting STX, got 0x%06X\n", data);
                }
#if 1
                if(((data >> 16) & 0xff) == 0x12L) {
                    /* RTS word */
                    m_rst_function = (data >> 8) & 0x0f;
                    #if WANT_TRACE_WMBURG_PROTO
                    LOG1N("*********** got RTS while waiting STX ??? Function:", m_rst_function);
                    #endif
                    Cancel_ENQ_RTS_Timeout(); /* got RTS, even though we weren't expecting it */

                    record_count = data & 0x7f;
                    num_records_remaining = record_count;
                    function = (data >> 8) & 0xf;
                    arinc_word = getSduSAL();
                    arinc_word |= 0x13LU << 24;     /* DC3 (=CTS) */
                    arinc_word |= record_count << 16;
                    #if WANT_TRACE_WMBURG_PROTO
                    LOG("send CTS");
                    #endif
                    m_transport->sendWord( (long) arinc_word, m_mcdu );

                    if(record_count == 0)
                    {
                        LOG("fix me");
                        arinc_word = getSduSAL();
                        arinc_word |= data << 8;
                        //arinc_word = add_parity_and_ssm(arinc_word,0,0);
                        #if WANT_TRACE_WMBURG_PROTO
                        LOG("send garbage!");
                        #endif
                        m_transport->sendWord( (long) arinc_word, m_mcdu );
                    }

                    else {
                        m_state = STATE__WAITING_STX;
                        #if WANT_TRACE_STATE_CHANGES || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
                        LOG1S("... new state ", interpret_state(m_state));
                        #endif
                    }
                }
#endif
            }
            break;

        case STATE__WAITING_DATA_OR_CONTROL:
            if(((data >> 16) & 0xff) == 0x1L) {             /* control word */
                m_col       =  data        & 0x1F;  /* bottom 5 bits */
                m_attribute = (data >>  5) &  0x7;  /*   next 3 bits */
                m_line      = (data >>  8) &  0xF;  /*   next 4 bits */
                m_color     = (data >> 12) &  0x7;  /*   next 3 bits */
                m_size      = (data >> 15) &  0x1;  /*   last bit */
                #if WANT_TRACE_WMBURG_PROTO && (WANT_TRACE_MCDU_MENU_QUEUE || WANT_DEBUG_SCRATCHPAD)
                    #if !WANT_TRACE_MCDU_MENU_QUEUE && WANT_DEBUG_SCRATCHPAD
                    if (m_line>=(scratchpad_row-1))
                    #endif //!WANT_TRACE_MCDU_MENU_QUEUE && WANT_DEBUG_SCRATCHPAD
                    log_printf("got Control %3d %3d cr %d at %X sz %d; rwir %d\n", 
                                    m_line+1, m_col+1, m_color, m_attribute, m_size,
                                    remaining_words_in_record);
                #endif
                //m_size = m_size ^ 0x00000001;
                m_dataFromHsdIdx = 0;
                remaining_words_in_record--;

                if(remaining_words_in_record <= 0) {
                    m_state = STATE__WAITING_ETX_OR_EOT; //STATE__WAITING_RTS;
                }
                else if(remaining_words_in_record == 1) {
                    m_state = STATE__WAITING_ETX_OR_EOT;
                }
                else m_state = STATE__WAITING_DATA;
                #if WANT_TRACE_STATE_CHANGES || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
                LOG1S("... new state ", interpret_state(m_state));
                #endif
            }
            else {
                if (!processed_this_word) {
                    log_printf("expecting Control, got 0x%06X\n", data);
                }
            }
            break;

        case STATE__WAITING_DATA:

            #if WANT_SHOW_DATA_TO_MCDU
            log_printf("data to MCDU:");
            #endif
            while(data & 0x7f) {
                m_dataFromHsd[m_dataFromHsdIdx]  = data & 0x7f;
                #if WANT_SHOW_DATA_TO_MCDU
                log_printf(" 0x%X", m_dataFromHsd[m_dataFromHsdIdx]);
                #endif
                m_dataFromHsdIdx++;
                data >>= 8;
                if (m_dataFromHsdIdx >= (int) sizeof(m_dataFromHsd))
                {
                    #if WANT_SHOW_DATA_TO_MCDU
                    log_printf(" -- ");
                    #endif
                    log_printf("m_dataFromHsd > %d bytes!", sizeof(m_dataFromHsd));
                    #if !WANT_SHOW_DATA_TO_MCDU
                    log_printf("\n");
                    #endif
                    m_dataFromHsdIdx = 0;
                }
            }
            #if WANT_SHOW_DATA_TO_MCDU
            log_printf("\n");
            #endif

            remaining_words_in_record--;
            if(remaining_words_in_record <= 0) {
                m_state = STATE__WAITING_ETX_OR_EOT; //STATE__WAITING_RTS;
                #if WANT_TRACE_STATE_CHANGES || (WANT_SHOW_DATA_TO_MCDU && WANT_VERBOSE_TRACE)
                LOG1S("... new state ", interpret_state(m_state));
                LOG("remaining_words_in_record <= 0");
                #endif

                // lets print data on screen
                if (m_dataFromHsdIdx > 0)
                {
                    m_dataFromHsd[m_dataFromHsdIdx++] = 0;
                    //m_screenPtr->printText(0,0,(char *) "<<<<<<< CProt >>>>>>>",0,C_GREEN,0);

                    if (m_line > scratchpad_row) {
                        #if WANT_DEBUG_SCRATCHPAD || WANT_TRACE_MCDU_MENU_QUEUE
                        log_printf("     %3d %3d cr %d at %X sz %d \"%s\"%signored\n", 
                                            m_line-1, m_col-1, m_color, m_attribute, m_size,
                                            m_dataFromHsd,
                                            (m_rst_function == NORMAL_TEXT_REQ)?"":" (menu)");
                        #endif
                    } else {
                        #if WANT_TRACE_MCDU_MENU_QUEUE
                        log_printf(" >>> %3d %3d cr %d at %X sz %d \"%s\"%s\n", 
                                            m_line-1, m_col-1, m_color, m_attribute, m_size,
                                            m_dataFromHsd,
                                            (m_rst_function == NORMAL_TEXT_REQ)?"":" (menu)");
                        #endif
                        if (m_rst_function == NORMAL_TEXT_REQ) {
                            m_screenPtr->printText( m_line-1,        //int row,
                                                    m_col-1,         //int col,
                                                    m_dataFromHsd,   //char * text,
                                                    m_size,          //int size,
                                                    m_color,         //int color,
                                                    m_attribute);    //int attrib);
                        } else {
                            m_menuScreenPtr->printMenuText(m_line-1,    //int row,
                                                    m_col-1,            //int col,
                                                    m_dataFromHsd,      //char * text,
                                                    m_size,             //int size,
                                                    m_color,            //int color,
                                                    m_attribute);       //int attrib);
                        }
                    }
                }
                // else empty string. probably lost characters -- who would send an empty string?

            } // else remaining_words_in_record > 0
            else if(remaining_words_in_record == 1) {
                m_state = STATE__WAITING_ETX_OR_EOT;
                #if WANT_TRACE_STATE_CHANGES || (WANT_SHOW_DATA_TO_MCDU && WANT_VERBOSE_TRACE)
                LOG1S("... new state ", interpret_state(m_state));
                #endif
                // lets print data on screen
                if (m_dataFromHsdIdx > 0)
                {
                    m_dataFromHsd[m_dataFromHsdIdx++] = 0;
                    //m_screenPtr->printText(0,0,(char *) "<<<<<<< CProt >>>>>>>",0,C_GREEN,0);

                    if (m_line > scratchpad_row) {
                        #if WANT_DEBUG_SCRATCHPAD || WANT_TRACE_MCDU_MENU_QUEUE
                        log_printf("     %3d %3d cr %d at %X sz %d \"%s\"%s ignored\n", 
                                            m_line-1, m_col-1, m_color, m_attribute, m_size,
                                            (m_rst_function == NORMAL_TEXT_REQ)?"":" (menu)",
                                            m_dataFromHsd);
                        #endif
                    } else {
                    
                        #if WANT_TRACE_MCDU_MENU_QUEUE
                        log_printf(" >>> %3d %3d cr %d at %X sz %d \"%s\"%s\n", 
                                            m_line-1, m_col-1, m_color, m_attribute, m_size,
                                            m_dataFromHsd,
                                            (m_rst_function == NORMAL_TEXT_REQ)?"":" (menu)");
                        #endif
                        if (m_rst_function == NORMAL_TEXT_REQ) {

                            m_screenPtr->printText( m_line-1,       //int row,
                                                    m_col-1,        //int col,
                                                    m_dataFromHsd,  //char * text,
                                                    m_size,         //int size,
                                                    m_color,        //int color,
                                                    m_attribute);   //int attrib);
                        } else {
                            m_menuScreenPtr->printMenuText( m_line-1,       //int row,
                                                    m_col-1,        //int col,
                                                    m_dataFromHsd,  //char * text,
                                                    m_size,         //int size,
                                                    m_color,        //int color,
                                                    m_attribute);   //int attrib);
                        }
                    }
                } // else empty string. probably lost characters -- who would send an empty string?

            } // if (remaining_words_in_record == 1)
            break;

        case STATE__WAITING_ETX_OR_EOT:
            /* expecting ETX or EOT */
            if(((data >> 16) & 0xff) == 0x4L) {             /* EOT */
                /* send an ACK */
                #if WANT_TRACE_WMBURG_PROTO
                log_printf("got EOT     %3d %3d\n", ((data>>8) & 0xff), ((data & 0xff)-1));
                #endif
                arinc_word = getSduSAL();
                arinc_word |= 0x6LU << 24;      /* ACK */
                arinc_word |= record_count << 16;

                /* we don't waste time adding parity and ssm */
                /* the parity bit is ignored -- the ssm is always 0 */
                #if WANT_TRACE_WMBURG_PROTO
                log_printf("send ACK, %3d\n", record_count);
                #endif
                m_transport->sendWord( (long) arinc_word, m_mcdu );
                m_state = STATE__WAITING_RTS;
                #if WANT_TRACE_STATE_CHANGES
                LOG1S("... new state ", interpret_state(m_state));
                #else
                #if WANT_TRACE_WMBURG_PROTO
                #if WANT_VERBOSE_TRACE
                LOG1S("... new state ", interpret_state(m_state));
                #else //!WANT_VERBOSE_TRACE
                LOG("... waiting RTS");
                #endif //!WANT_VERBOSE_TRACE
                #endif
                #endif
                break;          /* part of a recrod */
            }

            if(((data >> 16) & 0xff) == 0x3L) {             /* ETX */
                /* no ack required -- expect an STX for the next record */
                #if WANT_TRACE_WMBURG_PROTO
                log_printf("got ETX     %3d %3d\n", ((data>>8) & 0xff), ((data & 0xff)-1));
                #endif
                m_state = STATE__WAITING_STX;
                #if WANT_TRACE_STATE_CHANGES || (WANT_TRACE_WMBURG_PROTO && WANT_VERBOSE_TRACE)
                LOG1S("... new state ", interpret_state(m_state));
                #endif
                num_records_remaining--;
                break;
            }

            if (!processed_this_word) {
                log_printf("expecting EOT or ETX, got 0x%06X\n", data);
            }
            break;


        default:
            LOG1S("************* unexpected m_state ************* ", interpret_state(m_state));
            break;
    } /* switch */
}

void CProt::sendKey(int key)
{
    unsigned long arinc_word;
    static int seq = 0;

    arinc_word = getSduSAL();
    arinc_word |= 0x11LU << 24;     /* DC1 (Button word) */
    arinc_word |= (key & 0x000000ff) << 16;     /* key */
    arinc_word |= (seq & 0x0000007f) << 8;
    seq++;

    #if WANT_TRACE_KEYBOARD
    char msg[100];
    extern char * LookupKey(int key); // in mcdusim.c
    int the_key = key & 0x7F;
    int repeat  = key & 0x80;
    sprintf_s(msg, sizeof(msg), "Send key %02X {%s}%s", key, LookupKey(the_key), repeat?",repeat":"");
    LOG(msg);
    #endif
    
    if (m_transport && m_transport->IsFinishedInit()) {
        m_transport->sendWord( (long) arinc_word, m_mcdu );
    }
}

void CProt::sendMcduID(void)
{
    if (m_transport && m_transport->IsFinishedInit()) {
        unsigned long arinc_word;
        unsigned long data = equipment_id << 2;
        arinc_word = 0377LU;
        arinc_word |= data << 8;
        //arinc_word = add_parity_and_ssm(arinc_word,0,0);

        #if WANT_DEBUG_STARTUP || WANT_DEBUG_TRANSPORT_STATUS || WANT_TRACE_STATE_CHANGES || WANT_TRACE_PROCESS_STATE
        //if (m_transport->LoggingThisTx()) {
        LOG1S("send MCDU ID; state", interpret_state(m_state));
        //}
        #endif

        m_transport->sendWord( (long) arinc_word, m_mcdu );
    }
}

void CProt::postEvent( int eventID )
{
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventID);
    m_parentEventObjPtr->AddPendingEvent(event);
    //log_printf("Post event %d\n", eventID);
}

void CProt::OnTimer250ms(void)
{
    // If the DI has shut down and/or restarted,
    // the MCDU should restart this side of the conversation too.

    #if WANT_DEBUG_TRANSPORT_STATUS
    { /* context */
    static bool b_prev_Has_Transport_Started = (bool)2;
    static bool b_prev_Has_Transport_Failed  = (bool)2;
           bool b_Has_Transport_Started      = m_transport->Has_Transport_Started();
           bool b_Has_Transport_Failed       = m_transport->Has_Transport_Failed();
    if (b_prev_Has_Transport_Started != b_Has_Transport_Started ||
        b_prev_Has_Transport_Failed  != b_Has_Transport_Failed) {
        log_printf("Transport status = start %X, fail %X\n", b_Has_Transport_Started, b_Has_Transport_Failed);
        b_prev_Has_Transport_Started = b_Has_Transport_Started;
        b_prev_Has_Transport_Failed  = b_Has_Transport_Failed;
    }
    } /* context */
    #endif //WANT_DEBUG_TRANSPORT_STATUS

    // (im) No need to poll, get called back directly at CProt::OnReaderWriterFail()
    //if (m_transport->Has_Transport_Started()) {
    //    if (m_transport->Has_Transport_Failed()) {
    //
    //        log_printf("#%d *********** Transport failed! state %s\n", 
    //                            gConfig.cfgMcdu, interpret_state(m_state));
    //
    //        // re-initialize everything we were keeping track of
    //        CProt_Init();
    //
    //        // let transport layer know that we are attempting to recover
    //        m_transport->Set_Transport_Init();
    //
    //    } // else transport hasn't failed, nothing to do
    //} // else transport has not come up yet, nothing to do until the other end starts talking

    if (m_mcduIDTimer > 0) { /* initialized = 1, so always */
        -- m_mcduIDTimer;
        if (m_mcduIDTimer <= 0) {
            //if (m_state==0) { /* comms not established yet */
                //if (m_transport) m_transport->DontLogThisTx(); // not very interesting
                sendMcduID();
            //}
            ///* else comms established, don't interfere with protocol */

            /* keep sending out our ID until someone talks to us (state > 0) */
            m_mcduIDTimer = SEND_MCDU_ID_TIME;
        }
    }

    // process the timeout if we are waiting for RTS after sending ENQ
    if (m_enq_rts_timer) {
        -- m_enq_rts_timer;
        if (!m_enq_rts_timer) {
            /* the timeout period expired. should we send another ENQ? */
            if (m_enq_rts_retries) {
                log_printf("ENQ_RTS retry count %d -> %d\n", m_enq_rts_retries, m_enq_rts_retries-1);
                -- m_enq_rts_retries;
                if (m_enq_rts_retries) {
                    /* send another ENQ, hope for the RTS */
                    sendEnquire(1/*0=text,1=menu*/, 1/*0=L1/Menu,1=from timer*/);
                } else {
                    /* exhausted all the retry attempts. */
                    /* assume SDU is faulty or missing -- give up until it talks to us again */
                    setSduSAL(0);   // this SAL is no longer valid
                    LOG("*********** missing RTS timeout");
                    postEvent(MCDU_SIM___SAL_TIMEOUT);
                }
            }
            /* else ran out of retries */
            //else log_printf("ENQ_RTS %d %d\n", m_enq_rts_timer, m_enq_rts_retries);
        }
        /* else waiting for the timeout period to expire */
        //else log_printf("ENQ_RTS %d %d\n", m_enq_rts_timer, m_enq_rts_retries);
    }
    /* else timer is idle */

    #if !WANT_SAL_TIMEOUT_DISABLE
    // process HSD_SAL timer
    if (m_hsd_sal_timer > 0) {
        m_hsd_sal_timer--;
        if (m_hsd_sal_timer <= 0) {
            setSduSAL(0);   // this SAL is no longer valid
            LOG("*********** missing SAL timeout");
            postEvent(MCDU_SIM___SAL_TIMEOUT);
        }
    }
    #endif //!WANT_SAL_TIMEOUT_DISABLE

}

bool CProt::Waiting_on_ENQ_RTS_timeout(void)
{
    if (m_enq_rts_timer || m_enq_rts_retries) return true;
    return false;
}

void CProt::Cancel_ENQ_RTS_Timeout(void)
{
    //log_printf("ENQ_RTS timeout cancelled\n");
    m_enq_rts_timer = 0;
    m_enq_rts_retries = 0;
}

void CProt::Setup_ENQ_RTS_Timeout(void)
{
    if (!m_enq_rts_retries) {
        m_enq_rts_retries = ENQ_RTS_RETRIES;    /* first time, setup retries too */
        /* (im) Because sendEnquire() can be called at any time, the 1/4-second timer
         *      could actually fire any time between 0 and 250 ms later. In order to
         *      ensure the full timeout occurs, give a free 1/4-second the first time.
         */
        m_enq_rts_timer = ENQ_RTS_TIMEOUT+1;    /* extra 1/4 second */
    } else {
        m_enq_rts_timer = ENQ_RTS_TIMEOUT;
    }

    //log_printf("ENQ_RTS timeout: set %.1f sec with %d retries\n", (float)m_enq_rts_timer/4.0, m_enq_rts_retries);
}

//(im) Never want to cancel the timeout, just keep winding it up with Setup_SAL_Timeout
//void CProt::Cancel_SAL_Timeout(void)
//{
//    m_hsd_sal_timer = 0;
//    log_printf("SAL timeout cancelled\n");
//}

void CProt::Setup_SAL_Timeout(void)
{
    //if ( (float)m_hsd_sal_timer/4.0 > ((float)HSD_SAL_TIMEOUT/4.0-0.5)) {
        if (!m_hsd_sal_timer) {
            log_printf("SAL timeout: set %.1f sec\n", (float)HSD_SAL_TIMEOUT/4.0);
        }
        m_hsd_sal_timer = HSD_SAL_TIMEOUT;
    //}
}

void CProt::sendEnquire(int func/*0=text,1=menu*/, int check_which_state/*0=L1/Menu,1=from timer*/)
{
    //if (check_which_state && m_state>=STATE__WAITING_RTS) { /*(im) don't mess with the protocol */
    //    LOG1S("send Manual ENQ not sent, Already", interpret_state(m_state));
    //    return;
    //}

    if (!m_transport || !m_transport->IsFinishedInit()) {
        LOG("send Manual ENQ not sent: No transport available");
        return;
    }
    if (getSduSAL() == 0) {
        LOG("send Manual ENQ not sent: No valid SDU-SAL");
        return;
    }

    // send ENQ
    long arinc_word;
    long data = (0x5L << 16);               /* ENQ */

    data |= (func << 8);

    data |= invert_bits(m_mcduID[m_mcdu],8);
    arinc_word = getSduSAL();
    arinc_word |= data << 8;
    //arinc_word = add_parity_and_ssm(arinc_word,0,0);
    #if WANT_SHOW_DATA_TO_MCDU || WANT_TRACE_STATE_CHANGES
    LOG1S("send Manual ENQ: state", interpret_state(m_state));
    #endif
    if (m_transport->sendWord( (long) arinc_word, m_mcdu )) {
        /* sendWord() = true = success */
        Setup_ENQ_RTS_Timeout(); /* sent ENQ, start waiting for incoming RTS */
    } /* else sendWord() = false = failure */
}

void CProt::OnReaderWriterFail(int which_mcdu, char * description)
{
    // Note: we're not running in the main thread, so don't touch the GUI!

    log_printf("#%d >>> Transport %s failed! state %s\n", 
                        gConfig.cfgMcdu, description, interpret_state(m_state));

    // re-initialize everything we were keeping track of
    int save_hsd_sal_timer = m_hsd_sal_timer;
    CProt_Init();

    // let transport layer know that we are attempting to recover
    if (m_transport && m_transport->IsFinishedInit()) {
        m_transport->Set_Transport_Init();
    }

    // tell the frame window that it should go back to the menu screen
    m_hsd_sal_timer = save_hsd_sal_timer;
    postEvent(MCDU_SIM___GO_BACK_TO_MENU_SCREEN);
}

/* end of prot_739A.cpp */
