/*****************************************************************
 *
 * Name:        transportPipe.cpp
 * Purpose:     part of Display Interface Manager (DIM) Simulator and MCDU Simulator
 * Author:      Ian MacRae
 * Created:     July 2011
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportPipe.cpp,v 1.29 2012/05/31 19:02:59 macrae.i Exp $
 */

#ifdef __WXMSW__    // part of MCDU Simulator

#define WANT_DEBUG_STARTUP                  0   // 0=normal, 1=debug pipe creation and connections
#define WANT_DEBUG_SHUTDOWN                 0   // 0=normal, 1=show debugging at shutdown (BG threads, etc)
#define WANT_DEBUG_SIGNALS                  0   // 0=normal, 1=show all wait and signal events
#define WANT_DEBUG_VERBOSE                  0   // 0=normal, 1=show everything (don't need WANT_DEBUG_WRITER or WANT_DEBUG_READER too)
#define WANT_DEBUG_READER                   0   // 0=normal, 1=debug BG reader thread
#define WANT_DEBUG_WRITER                   0   // 0=normal, 1=debug BG writer thread
#define WANT_CIRC_BUFFER_TRACE              0   // 0=normal, 1=debug using circular buffer
#define WANT_SHOW_NOT_YET                   0   // 0=normal, 1=show sendWord not yet (when debugging startup)

#define WANT_SHOW_ALL_TX_DATA               0   // 0=normal, 1=show all write data
#define WANT_SHOW_TX_WORD                   0   // 0=normal, 1=show Tx hex words
#define WANT_SHOW_INTERESTING_TX            1   // 0=normal, 1=show some Tx hex words
#define WANT_SHOW_ALL_RX_DATA               0   // 0=normal, 1=show all read data
#define WANT_SHOW_RX_WORD                   0   // 0=normal, 1=show Rx hex words
#define WANT_SHOW_INTERESTING_RX            1   // 0=normal, 1=show some Rx hex words
#define WANT_TRACE_HEARTBEAT                0   // 0-normal, 1=show heartbeat & related state transitions

#include "conditionals.h"

#define WANT_SHOW_INTERESTING_NON_DATA      1   // TransportPipe: show all words except printable data
#define WANT_SHOW_INTERESTING_ENQ           0   // TransportPipe: show ENQ word
#define WANT_SHOW_INTERESTING_ACK           0   // TransportPipe: show ACK word
#define WANT_SHOW_INTERESTING_CLR           0   // TransportPipe: show CLR word
#define WANT_SHOW_INTERESTING_RTS           0   // TransportPipe: show RTS word
#define WANT_SHOW_INTERESTING_CTS           0   // TransportPipe: show CTS word
#define WANT_SHOW_INTERESTING_DC1           0   // TransportPipe: show DC1 word (MCDU key/button and LOGOFF)

#else

#include "DebugFlags.h" // DIM Sim Debug flags

#define WANT_DEBUG_STARTUP             WANT_DEBUGFLAG(DEBUG_STARTUP          )
#define WANT_DEBUG_SHUTDOWN            WANT_DEBUGFLAG(DEBUG_SHUTDOWN         )
#define WANT_DEBUG_SIGNALS             WANT_DEBUGFLAG(DEBUG_SIGNALS          )
#define WANT_DEBUG_VERBOSE             WANT_DEBUGFLAG(DEBUG_TRANSPORT_VERBOSE) //not just DEBUG_VERBOSE
#define WANT_DEBUG_READER              WANT_DEBUGFLAG(DEBUG_READER           )
#define WANT_DEBUG_WRITER              WANT_DEBUGFLAG(DEBUG_WRITER           )
#define WANT_CIRC_BUFFER_TRACE         WANT_DEBUGFLAG(CIRC_BUFFER_TRACE      )
#define WANT_SHOW_NOT_YET              WANT_DEBUGFLAG(SHOW_NOT_YET           )
#define WANT_SHOW_ALL_TX_DATA          WANT_DEBUGFLAG(SHOW_ALL_TX_DATA       )
#define WANT_SHOW_TX_WORD              WANT_DEBUGFLAG(SHOW_TX_WORD           )
#define WANT_SHOW_ALL_RX_DATA          WANT_DEBUGFLAG(SHOW_ALL_RX_DATA       )
#define WANT_SHOW_RX_WORD              WANT_DEBUGFLAG(SHOW_RX_WORD           )
#define WANT_TRACE_HEARTBEAT           WANT_DEBUGFLAG(TRACE_HEARTBEAT        )
#define WANT_DEBUG_TRANSPORT_STATUS    WANT_DEBUGFLAG(DEBUG_TRANSPORT_STATUS )

#define WANT_SHOW_INTERESTING_NON_DATA WANT_DEBUGFLAG(SHOW_INTERESTING_NON_DATA) /* TransportPipe: show all words except printable data */
#define WANT_SHOW_INTERESTING_ENQ      WANT_DEBUGFLAG(SHOW_INTERESTING_ENQ     ) /* TransportPipe: show ENQ word */
#define WANT_SHOW_INTERESTING_ACK      WANT_DEBUGFLAG(SHOW_INTERESTING_ACK     ) /* TransportPipe: show ACK word */
#define WANT_SHOW_INTERESTING_CLR      WANT_DEBUGFLAG(SHOW_INTERESTING_CLR     ) /* TransportPipe: show CLR word */
#define WANT_SHOW_INTERESTING_RTS      WANT_DEBUGFLAG(SHOW_INTERESTING_RTS     ) /* TransportPipe: show RTS word */
#define WANT_SHOW_INTERESTING_CTS      WANT_DEBUGFLAG(SHOW_INTERESTING_CTS     ) /* TransportPipe: show CTS word */
#define WANT_SHOW_INTERESTING_DC1      WANT_DEBUGFLAG(SHOW_INTERESTING_DC1     ) /* TransportPipe: show DC1 word (MCDU key/button and LOGOFF) */

#endif

#include <windows.h>    // for Sleep()
#include <stdio.h>      // for sprintf_s

#include <string.h>     // for strcmp, memset
#include <tchar.h>
#include <process.h>    // for _beginthreadex
#include <sddl.h>       // for ConvertStringSecurityDescriptorToSecurityDescriptor, SDDL_REVISION_1

#include "MSecTime.h"
#include "transportUdp.h"

#define MCDU_SIM___CUSTOM_EVENT_RX 1002  // from defs.h

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName); /* in mcduSim.cpp */

const int MCDU_SIM = 1;
const int DIM_SIM  = 2;
const int MCDU_SIM_BASE_PORT = 7800;
const int DIM_SIM_BASE_PORT  = 7900;

/////////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__    // part of MCDU Simulator
#include "log.h"
extern char * our_ident;
extern char * other_ident;
extern char * our_indent;
extern char * other_indent;
int simulator_type = MCDU_SIM;
#else               // part of Display Interface Manager (DIM) Simulator
#define log_printf sc_printf
extern "C" {
extern int sc_printf(char * fmt, ...); // defined in engine.c
}; /* extern "C" */
extern char * our_ident;
extern char * other_ident;
extern char * our_indent;
extern char * other_indent;
int simulator_type = DIM_SIM;
#endif //__WXMSW__

#define WANT_DEBUG_MCDU(n) (1<<n)

int Debug_which_MCDU_UDP = (~0); //WANT_DEBUG_MCDU(1) | WANT_DEBUG_MCDU(0);

static bool DebugThisMCDU(int mcdu_index)
{
    if (Debug_which_MCDU_UDP & (1<<mcdu_index)) return true;
    return false;
}

#define DEBUG_MCDU(x)     DebugThisMCDU(x->MCDU())
#define DEBUG_THIS_MCDU() DebugThisMCDU(   MCDU())

#define MCDU_ID(x)        (x->MCDU()+1)
#define THIS_MCDU_ID()    (   MCDU()+1)

extern int Ignore_This_Rx_Word(long r);

/////////////////////////////////////////////////////////////////////////////////

#define PIPE_NAME_BASE      "DI2MCDU"   // DI to MCDU pipeline

#define BUFFER_SIZE         512         // max message size (bytes) for a single pipe message

typedef unsigned long long  ULONGLONG;

////////////////////////////////////////////////////////////

// helper functions

#if WINVER >= 0x0500
#define printf_err_and_log_APPEND(x) strcat_s(msg, sizeof_msg, x)
#else
#define printf_err_and_log_APPEND(x) strcat  (msg,             x)
#endif

extern void append_interpreted_error(char * msg, size_t sizeof_msg, DWORD dwErr, char * sPipeName);

extern void append_dwErr_as_needed_and_LOG(char * sPipeName, char * msg, size_t sizeof_msg, DWORD dwErr);

extern void printf_err_and_LOG(char * sPipeName, char * fmt, int which_mcdu, DWORD dwErr, char * suffix);
extern BOOL CreatePipeSecurity(PSECURITY_ATTRIBUTES *ppSa);

// Free the SECURITY_ATTRIBUTES structure created by CreatePipeSecurity
extern void FreePipeSecurity(PSECURITY_ATTRIBUTES pSA);

//#if WANT_SHOW_INTERESTING_TX || WANT_SHOW_INTERESTING_RX
extern bool WordIsInteresting(unsigned long value);
//#endif //WANT_SHOW_INTERESTING_TX || WANT_SHOW_INTERESTING_RX

////////////////////////////////////////////////////////////

// A wrapper for WaitForSingleObject
// Uses MsgWaitForMultipleObjects instead and handles the system events internally

extern DWORD MyWait(HANDLE hHandle, DWORD dwMilliseconds);

////////////////////////////////////////////////////////////

//#if WANT_CIRC_BUFFER_TRACE

typedef struct {
    int             mcdu_index;
    char *          hint;   // must be static string
    unsigned long   data;
    MSecTime        t1000;  // current time in msecs since 01-Jan-1970
} Circular_Trace_t;

#define                     Circular_TRACE_SIZE 1000
static Circular_Trace_t     gCUdpHandler_Trace[Circular_TRACE_SIZE];
static unsigned int         gCUdpHandler_Trace_Next_Write = 0;
static unsigned int         gCUdpHandler_Trace_Count      = 0;

extern void Circular_Trace_Dump(void);

extern void Circular_Trace(int mcdu_index, unsigned long data, char * hint);

////////////////////////////////////////////////////////////

//unsigned int WINAPI UdpWriterThread(LPVOID lpvParam);
unsigned int WINAPI UdpReaderThread(LPVOID lpvParam);

CUdpHandler::CUdpHandler(int which_mcdu/*0,1,2,3*/, char * remote_ip_address, long remote_base_port, long local_base_port, CTransportUdp * parent) :
        m_RxCallback_function(NULL), m_RxCallback_class(NULL),
        m_Pipe_Queue_Next_Write(0), m_Pipe_Queue_Next_Read(0)
        //#if WANT_PIPE_STATISTICS
        , m_Pipe_Queue_High_Water_Mark(0)
        //#endif //WANT_PIPE_STATISTICS
        , m_bAcceptingRx(false)
        , m_parent(parent)
        , m_dest_ip(remote_ip_address)
{
    m_which_mcdu = which_mcdu;

    m_PipeStatus = 0;
    m_bQuittingTime = FALSE;
    m_hWriterThread = NULL;
    m_hReaderThread = NULL;
    m_nWriterThreadId = 0;
    m_nReaderThreadId = 0;
    m_WriterEvent = NULL;
    m_SendWordEvent = NULL;
    m_bAllow_Write = false;
    m_bAllow_Read = false;

    // Create an auto-reset event object.
    //SetLastError(0);
    //m_WriterEvent = CreateEvent(NULL,    // default security attributes
    //                           FALSE,   // auto-reset event
    //                           FALSE,   // initial state is nonsignaled
    //                           NULL);   // no name
    //DWORD dwErr = GetLastError();

    //if (!m_WriterEvent) {
    //    if (DEBUG_THIS_MCDU()) {
    //        log_printf("-> #%d: CreateEvent failed, error %lu\n",
    //                which_mcdu+1, dwErr);
    //    }
    //    return;
    //}

    //// Create an auto-reset event object.
    //SetLastError(0);
    //m_SendWordEvent = CreateEvent(NULL,  // default security attributes
    //                             FALSE, // auto-reset event
    //                             TRUE,  // initial state is signaled
    //                             NULL); // no name
    //dwErr = GetLastError();

    //if (!m_SendWordEvent) {
    //    if (DEBUG_THIS_MCDU()) {
    //        log_printf("-> #%d: CreateEvent failed, error %lu\n",
    //                which_mcdu+1, dwErr);
    //    }
    //    return;
    //}

    ///////////////////////////////////////////////////////////////////////////////
    m_udpReadPort = local_base_port + which_mcdu;       
    m_udpWritePort = remote_base_port + which_mcdu;

    SOCKADDR_IN serveraddr;
    int portno = m_udpReadPort;
    memset(&serveraddr,0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(portno);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
 
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        log_printf("WSAStartup failed with error %d\n", iResult);
        return;
    }
    m_serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_serversocket == -1)
    {
        log_printf("Socket Initialiation Error on %s(%d)\n", our_ident, which_mcdu+1);
    } else {
         if(bind(m_serversocket, (SOCKADDR*)&serveraddr,sizeof(SOCKADDR_IN)) < 0) {
            log_printf("ERROR binding in the server socket on %s(%d)\n", our_ident, which_mcdu+1);
        } else {
            m_hReaderThread = (HANDLE) _beginthreadex(NULL, 0, UdpReaderThread, this, 0, &m_nReaderThreadId);
            //m_hWriterThread = (HANDLE) _beginthreadex(NULL, 0, UdpWriterThread, this, 0, &m_nWriterThreadId);

            m_dest.sin_family = AF_INET;
            m_dest.sin_port = htons(m_udpWritePort);
            m_dest.sin_addr.s_addr = inet_addr(m_dest_ip.data());

            Allow_Write();
         }
    }

    ///////////////////////////////////////////////////////////////////////////////

    //Set_ReaderFail_Callback(NULL, NULL);
    //Set_WriterFail_Callback(NULL, NULL);

    // give BG threads time to get started
    //Sleep(10);

    // success
    m_PipeStatus = 1;
    return;
}

 SOCKET CUdpHandler::getUdpSocket() {
     return m_serversocket;
}

void CUdpHandler::closeUdpSocket() {
    closesocket(m_serversocket);
    m_serversocket = 0;
    WSACleanup();
    return ;
}

size_t CUdpHandler::writeToUdp(const void* data, size_t len) {

    return sendto(m_serversocket, reinterpret_cast<const char*>(data), len, 0,(SOCKADDR*)&m_dest, sizeof(m_dest));
}

void CUdpHandler::SetRxCallback(void * pThis, ONRX_HELPER_FUNCTION function) {
    m_RxCallback_class = pThis;
    m_RxCallback_function = function;
}

void CUdpHandler::Call_OnRx(long wordRx) {
    if (m_RxCallback_function) {
        (*m_RxCallback_function)(m_RxCallback_class, wordRx, MCDU());
    }
}

//void CUdpHandler::Set_ReaderFail_Callback(void * pThis, ONREADERFAIL_HELPER_FUNCTION function) {
//    m_ReaderFail_Callback_class = pThis;
//    m_ReaderFail_Callback_function = function;
//}

//void CUdpHandler::Call_OnReaderFail() {
//    if (m_ReaderFail_Callback_function) {
//        (*m_ReaderFail_Callback_function)(m_ReaderFail_Callback_class, MCDU());
//    }
//}

//void CUdpHandler::Set_WriterFail_Callback(void * pThis, ONWRITERFAIL_HELPER_FUNCTION function) {
//    m_WriterFail_Callback_class = pThis;
//    m_WriterFail_Callback_function = function;
//}

//void CUdpHandler::Call_OnWriterFail() {
//    if (m_WriterFail_Callback_function) {
//        (*m_WriterFail_Callback_function)(m_WriterFail_Callback_class, MCDU());
//    }
//}

CUdpHandler::~CUdpHandler()
{
    if (WANT_DEBUG_SHUTDOWN) {
        char szWork[100];
        sprintf_s(szWork, sizeof(szWork), "~CUdpHandler %p for MCDU#%d\n", this, THIS_MCDU_ID());
        log_printf(szWork);
    } //WANT_DEBUG_SHUTDOWN

    // if either thread exists, tell them to quit
    if (m_hReaderThread) {
        MSecTime t1000start;            // current time in msecs since 01-Jan-1970

        if (WANT_DEBUG_SHUTDOWN) {
        // tell both the reader and writer threads to shut down
        DWORD dw0_Writer = 0;//::WaitForSingleObject(m_hWriterThread, 0/*msec*/);
        DWORD dw0_Reader = ::WaitForSingleObject(m_hReaderThread, 0/*msec*/);
        // status can be be WAIT_TIMEOUT(0x102=258.) if they are still running...
        char szWork[100];
        sprintf_s(szWork, sizeof(szWork), "~CUdpHandler %p): BG r %d(%d) w %d(%d)\n",
            this, m_nReaderThreadId, dw0_Reader, m_nWriterThreadId, dw0_Writer);
        log_printf(szWork);
        } //WANT_DEBUG_SHUTDOWN

        // yet BG threads know it is time to quit (if they are awake)
        SetQuittingTime();

        // looks extreme, but fairly safe: the BG threads don't own any resources
        if (m_hReaderThread) {
            TerminateThread(m_hReaderThread, 999);
        }
        //if (m_hWriterThread) {
        //    TerminateThread(m_hWriterThread, 999);
        //}

        // Sleep to allow them both a chance to wake up and notice they are being killed
        //Sleep(10);  // allow BG reader and writer threads to be killed

        // Wait around for a short while to make sure both threads have stopped
        for (int nLoops=0; nLoops<(500/20); nLoops++) { /* 1/2 sec max */

            if (!m_hReaderThread && !m_hWriterThread)
                break;  /* OK, the threads are gone */

            HANDLE  h[2];
            h[0] = m_hReaderThread;
            h[1] = m_hWriterThread;

            DWORD dwReaderErrorCode = 0;
            DWORD dwReaderExitCode = 0;
            DWORD dwWriterErrorCode = 0;
            DWORD dwWriterExitCode = 0;

            DWORD dwStatus = WaitForMultipleObjects(2, h, /*fWaitAll*/true, 0/*don't wait*/);
            //log_printf("-> #%d: Wait on Threads, Status %lu\n", which_mcdu+1, dwStatus);
            if (dwStatus == WAIT_FAILED)
                break;  /* OK, both threads are gone (or at least one of them is) */

            // make sure both BG threads have finished

            if (!GetExitCodeThread(m_hReaderThread, &dwReaderExitCode))
                dwReaderErrorCode = GetLastError();
            //if (!GetExitCodeThread(m_hWriterThread, &dwWriterExitCode))
            //    dwWriterErrorCode = GetLastError();

            if (dwReaderExitCode!=STILL_ACTIVE && dwWriterExitCode!=STILL_ACTIVE)
                break;  /* OK, both threads are gone (or something strange happened) */
            /* otherwise at least one of the threads is still active */

            if (WANT_DEBUG_SHUTDOWN) {
            char szWork[100];
            sprintf_s(szWork, sizeof(szWork), "~CUdpHandler %p): BG r %lu(err=%lu), w %lu(err=%lu), wait\n",
                            this, dwReaderExitCode, dwReaderErrorCode, dwWriterExitCode, dwWriterErrorCode);
            log_printf(szWork);
            } //WANT_DEBUG_SHUTDOWN

            //Sleep(20);  // wait for background reader and writer thread(s) to wake up and notice it is time to quit
            continue;
        } /*for*/

        if (WANT_DEBUG_SHUTDOWN) {
        MSecTime t1000now;
        unsigned long ms_diff = t1000now.Interval(t1000start);
        if (ms_diff>50L) {
            log_printf("MCDU#%d) Kill BG threads took %lu ms\n", THIS_MCDU_ID(), ms_diff);
        }
        } //WANT_DEBUG_SHUTDOWN

    } // else both threads already long gone

    m_hReaderThread = NULL;

    if (WANT_DEBUG_SHUTDOWN) {
        char szWork[100];
        sprintf_s(szWork, sizeof(szWork), "~CUdpHandler %p for MCDU#%d done.\n", this, THIS_MCDU_ID());
        log_printf(szWork);
    } //WANT_DEBUG_SHUTDOWN
}


// This BG Reader thread listens to the udp, reads whatever arrives, and stuffs data into a queue for later.
unsigned int WINAPI UdpReaderThread(LPVOID lpvParam)
{
    CUdpHandler * udp = (CUdpHandler *)lpvParam;
    // RC:
    sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);
    char msg[200];
    //////////////////////////////////////////

    DWORD dwIncomingMessageCount = 0L;
    DWORD dwIncomingMessageBytes = 0L;

    // incoming messages can be this big
    char buffer[BUFFER_SIZE];
    DWORD cbBytesRead = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipeReader = INVALID_HANDLE_VALUE;

    char thread_name[10];
    sprintf_s(thread_name, sizeof(thread_name), "PipeR#%d", MCDU_ID(udp));
    SetThreadName(GetCurrentThreadId(),thread_name);

    if (WANT_DEBUG_READER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN) {
    if (DEBUG_MCDU(udp)) {
        log_printf("-> #%d Reader: starting\n", MCDU_ID(udp));
    }
    } //WANT_DEBUG_READER || WANT_DEBUG_VERBOSE || WANT_DEBUG_SIGNALS || WANT_DEBUG_STARTUP || WANT_DEBUG_SHUTDOWN

    udp->Allow_Read();
    udp->SetAcceptingRx(true);
    udp->Transport()->Set_Transport_Started(); 

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (;;) {
        // are we shutting down?
        if (udp->QuittingTime())
            break;

        fd_set fds;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;

        FD_ZERO(&fds);
        FD_SET(udp->getUdpSocket(), &fds);

        int rc = select(sizeof(fds)*8, &fds, NULL, NULL, &timeout);

        // Read incoming data from the UDP.
        if (WANT_DEBUG_READER && WANT_DEBUG_VERBOSE) {
            if (DEBUG_MCDU(udp)) {
                char msg[200];
                sprintf_s(msg, sizeof(msg), "-> #%d Reader: About to Read ...\n", MCDU_ID(udp));
                log_printf(msg);
            }
        } //WANT_DEBUG_READER && WANT_DEBUG_VERBOSE

        if(rc > 0) {

            int cbBytesRead = recvfrom(udp->getUdpSocket(), buffer, BUFFER_SIZE, 0, (struct sockaddr *)&SenderAddr, &SenderAddrSize);
            if (cbBytesRead < 0)
            {
                sprintf_s(msg, sizeof(msg), "-> #%d Could not receive datagram.\n", MCDU_ID(udp));
                log_printf(msg);
                //udp->closeUdpSocket();
            } else if (cbBytesRead > 0) {
                // Process the incoming message.

                ++ dwIncomingMessageCount;
                dwIncomingMessageBytes += cbBytesRead;

                if (WANT_DEBUG_READER || WANT_SHOW_ALL_RX_DATA) {
                if (DEBUG_MCDU(udp)) {
                    char msg[100];

                #if WINVER >= 0x0500
                    if (WANT_SHOW_ALL_RX_DATA) {
                    if (cbBytesRead==sizeof(unsigned long)) {
                        sprintf_s(msg, sizeof(msg), "-> Read #%d: 0x%08X",
                                MCDU_ID(udp), *(unsigned long *)buffer);
                    } else {
                        sprintf_s(msg, sizeof(msg), "-> Read #%d: 0x%08X -- %d bytes!",
                                MCDU_ID(udp), *(unsigned long *)buffer, cbBytesRead);
                    }
                    } else { //!WANT_SHOW_ALL_RX_DATA
                    sprintf_s(msg, sizeof(msg), "-> #%d Reader: Read udp, %4u bytes:",
                                MCDU_ID(udp), cbBytesRead);
                    for (int j=0; j<cbBytesRead; j++) {
                        size_t nb_in_msg = strlen(msg);
                        char * msg_end = msg + nb_in_msg;
                        sprintf_s(msg_end, sizeof(msg)-nb_in_msg, " %02X", buffer[j]);
                        if (j>=10 && j<cbBytesRead) {
                            strcat_s(msg, sizeof(msg), " ...");
                        }
                    }
                    } //!WANT_SHOW_ALL_RX_DATA
                    strcat_s(msg, sizeof(msg), "\n");
                #else
                    if (WANT_SHOW_ALL_RX_DATA) {
                    if (cbBytesRead==sizeof(unsigned long)) {
                        sprintf  (msg,              "-> Read #%d: 0x%08X",
                                MCDU_ID(udp), *(unsigned long *)buffer);
                    } else {
                        sprintf  (msg,              "-> Read #%d: 0x%08X -- %d bytes!",
                            MCDU_ID(udp), *(unsigned long *)buffer, cbBytesRead);
                    }
                    } else { //!WANT_SHOW_ALL_RX_DATA
                    sprintf  (msg,              "-> #%d Reader: Read udp, %4u bytes:",
                                MCDU_ID(udp), cbBytesRead);
                    for (unsigned int j=0; j<cbBytesRead; j++) {
                        size_t nb_in_msg = strlen(msg);
                        char * msg_end = msg + nb_in_msg;
                        sprintf  (msg_end,                        " %02X", buffer[j]);
                        if (j>=10 && j<cbBytesRead) {
                            strcat  (msg,              " ...");
                        }
                    }
                    } //WANT_SHOW_ALL_RX_DATA
                    strcat  (msg,              "\n");
                #endif
                    log_printf(msg);
                }
                } //WANT_DEBUG_READER || WANT_SHOW_ALL_RX_DATA

                { /*context*/
                unsigned long rxWord = *(unsigned long *)buffer;
                bool bTraceRxWord;
                if (!WANT_SHOW_ALL_RX_DATA && WANT_SHOW_RX_WORD) { // only WANT_SHOW_RX_WORD
                    bTraceRxWord = true;
                } else { //WANT_SHOW_ALL_RX_DATA || !WANT_SHOW_RX_WORD
                    bTraceRxWord = false;
                } //WANT_SHOW_ALL_RX_DATA || !WANT_SHOW_RX_WORD

                //#if WANT_SHOW_INTERESTING_RX
                if (WordIsInteresting(rxWord))
                    bTraceRxWord = true;
                //#endif //WANT_SHOW_INTERESTING_RX

                if (bTraceRxWord && DEBUG_MCDU(udp)) {
                    log_printf ("%sRx #%d: 0x%08X%s\n", other_indent, MCDU_ID(udp), *(unsigned long *)buffer, !udp->IsAcceptingRx()?" -- discard":"");
                }

                } /*context*/

                if (WANT_CIRC_BUFFER_TRACE) {
                    Circular_Trace(udp->MCDU(), *(unsigned long *)buffer, "Rx");
                } //WANT_CIRC_BUFFER_TRACE

                if (udp->IsAcceptingRx()) {
                    // process the data immediately, let the callback function handle it
                    udp->Call_OnRx( *(unsigned long *)buffer );
                }

            } else {
                // no data
            }
        }

    } // forever, exits at quitting time or due to an error

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Disconnect the udp, and close the handle to this udp instance.
    if (WANT_DEBUG_READER) {
        if (DEBUG_MCDU(udp)) {
            log_printf("-> #%d Reader: Disconnect\n", MCDU_ID(udp));
        }
    } //WANT_DEBUG_READER

    if (0 != udp->getUdpSocket()) {
        udp->closeUdpSocket();
    }

    if (WANT_DEBUG_READER || WANT_DEBUG_SHUTDOWN) {
        if (DEBUG_MCDU(udp)) {
            log_printf("-> #%d Reader: Exit\n", MCDU_ID(udp));
        }
    } //WANT_DEBUG_READER || WANT_DEBUG_SHUTDOWN
    return 0;
}

////////////////////////////////////////////////////////////

    #ifdef __WXMSW__    // part of MCDU Simulator
CTransportUdp::CTransportUdp(int which_mcdu/*0,1,2,3*/, char * remote_ip_address, long remote_base_port, long local_base_port)
    #else               // part of Display Interface Manager (DIM) Simulator
CTransportUdp::CTransportUdp(int which_mcdu/*0,1,2,3*/, char * s_DI_Host, CTransportMCDU * parent)
    #endif
{
    m_udp_haldler = NULL;

    m_transport_start = false;
    m_transport_fail  = false;
    Set_Transport_Init();       // hasn't started or failed yet

    m_udp_haldler = new CUdpHandler(which_mcdu, remote_ip_address, remote_base_port, local_base_port, this);
}

CTransportUdp::~CTransportUdp()
{
    if (m_udp_haldler) {
        int MCDU_ID = m_udp_haldler->MCDU()+1;

        // tell this udp we are shutting down
        m_udp_haldler->SetQuittingTime();

        // Wait around for a short while to give udp BG threads time to wake up
        Sleep(10);

        // Delete the udp
        if (WANT_DEBUG_SHUTDOWN) {
        log_printf("MCDU#%d) Delete udp\n", MCDU_ID);
        } //WANT_DEBUG_SHUTDOWN
        delete m_udp_haldler;
        m_udp_haldler = NULL;

    } /*if m_udp_haldler*/
}

#ifdef __WXMSW__
bool CTransportUdp::init ( wxEvtHandler * eventObjPtr, int port )
#else
bool CTransportUdp::init ( void * eventObjPtr, int port )
#endif
{
    #ifdef __WXMSW__
    CTransport::init(eventObjPtr, port);
    #endif

    memset( m_bufrx, 0, sizeof(m_bufrx));
    m_bufrxLen = 0;

    if (!m_udp_haldler)
        return 0;

    SetFinishedInit();
    return m_udp_haldler->PipeStatus()!=0; // 0=failed to initialize, 1=init OK, anything else=OK too
}

// return true if debug message should be printed: writing allowed, returns WANT_SHOW_NOT_YET definition
bool CTransportUdp::Tx_Debug_helper(bool * bNotYet, bool * bQueueFull)
{
    *bNotYet = false;
    *bQueueFull = false;

    // got a udp yet?
    if (!m_udp_haldler)
        return false;    // oops!

    // is the queue full?
    *bQueueFull = m_udp_haldler->Pipe_Queue_Full();

    // allowed to write yet?
    if (!m_udp_haldler->Is_Write_Allowed()) {
        if (WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP) {
        if (WANT_SHOW_NOT_YET) {
        *bNotYet = true;    // not allowed yet, but still want to see it because WANT_SHOW_NOT_YET defined
        } //WANT_SHOW_NOT_YET
        } //WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP
        // not allowed yet, but don't want to see "not yet"
        return false;
    }

    // either the TX will succeed, or it will fail and show "not yet"
    return true;
}

bool CTransportUdp::sendWord (long txWord, int mcdu ) /* true = success, false = failure */
{
    // got a udp yet?
    if (!m_udp_haldler)
        return false;   // oops!

    // if the hardware isn't ready, quietly bail
    if (!IsFinishedInit() || !Has_Transport_Started()) {
        return false;   // oops!
    }

    // allowed to write yet?
    if (!m_udp_haldler->Is_Write_Allowed()) {
        if (WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP) {
        if (WANT_SHOW_NOT_YET) {
        if (DEBUG_MCDU(m_udp_haldler)) {
            log_printf("-> #%d sendWord: not yet\n", m_udp_haldler->MCDU()+1);
        }
        } //WANT_SHOW_NOT_YET
        } //WANT_DEBUG_SIGNALS || WANT_DEBUG_VERBOSE || WANT_DEBUG_WRITER || WANT_DEBUG_STARTUP
        return false;   // not yet
    }

    if (WANT_CIRC_BUFFER_TRACE) {
        Circular_Trace(m_udp_haldler->MCDU(), (unsigned long)txWord, "Tx");
    } //WANT_CIRC_BUFFER_TRACE

    { /*context*/
    bool bTraceTxWord;
    if (!WANT_SHOW_ALL_TX_DATA && WANT_SHOW_TX_WORD) { // only WANT_SHOW_TX_WORD
        bTraceTxWord = true;
    } else { //WANT_SHOW_ALL_TX_DATA || !WANT_SHOW_TX_WORD
        bTraceTxWord = false;
    } //WANT_SHOW_ALL_TX_DATA || !WANT_SHOW_TX_WORD

    //#if WANT_SHOW_INTERESTING_TX
    if (WordIsInteresting(txWord))
        bTraceTxWord = true;
    //#endif //WANT_SHOW_INTERESTING_TX

    if (bTraceTxWord && DEBUG_MCDU(m_udp_haldler)) {
        log_printf ("%sTx #%d: 0x%08X\n", our_indent, m_udp_haldler->MCDU()+1, txWord);
    }

    } /*context*/

    if (WANT_CIRC_BUFFER_TRACE) {
        Circular_Trace_Dump();
    } //WANT_CIRC_BUFFER_TRACE

    // stuff word into queue
    //m_udp_haldler->Write_Pipe_Queue(txWord);
    m_udp_haldler->writeToUdp(&txWord, sizeof(txWord));

    if (WANT_DEBUG_SIGNALS) {
    if (DEBUG_MCDU(m_udp_haldler)) {
        log_printf("-> #%d sendWord: signal WriterEv\n", m_udp_haldler->MCDU()+1);
    }
    } //WANT_DEBUG_SIGNALS

    return true;        // success
}

bool CTransportUdp::getRxWord ( int connectionID, long * rxWord)
{
    if (m_bufrxLen != 1)
    {
        if (DEBUG_MCDU(m_udp_haldler)) {
            log_printf("!!! getRxWord called with no data available !!!\n", m_bufrxLen);
        }
        return false;
    }

    if (Ignore_This_Rx_Word(m_dwRxData)) {
        UNLOCK_RX();
        return false;
    }

    if (DEBUG_MCDU(m_udp_haldler)) {
        log_printf("raw rx data: 0x%08X\n", m_dwRxData);
    }

    *rxWord = m_dwRxData;

    m_bufrxLen = 0;
    UNLOCK_RX();
    return true;
}

void CTransportUdp::UNLOCK_RX(void)
{
}

#define SLEEP_TIME          100         // 100 ms
#define SLEEP_THRESHOLD     100

void * CTransportUdp::Entry()       // this is the RX thread
{
    //if (DEBUG_MCDU(m_udp_haldler)) {
    //  log_printf("Rx thread started\n");
    //}
    short len;
    //int slen=sizeof(m_client_socket_addr);
    int sleepCounter = 0;

    while(1)
    {
        m_bufrxLen = 0;

        // TODO: check the queue
        len = 0; //GetRxQueueStatus(gConfig.cfgArincCard, gConfig.cfgArincRxChannel);

        if (len < 0)
        {
            if (DEBUG_MCDU(m_udp_haldler)) {
                log_printf("GetRxQueueStatus: Error %d\n", len);
            }
            break;      // end thread
        }

        if (len == 0)
        {
            if (sleepCounter >= SLEEP_THRESHOLD)
                Sleep(SLEEP_TIME);
            else
                sleepCounter++;
            continue;
        }

        if (len>0)
        {
            /* Read one 32-bit word from the receiver's queue with 48BIT/IRIG time stamp. */
            len = 1; //ReadRxQueueIrigOne(gConfig.cfgArincCard, gConfig.cfgArincRxChannel, &m_dwRxData, NULL, NULL);

            if (len != 1)
            {
                if (DEBUG_MCDU(m_udp_haldler)) {
                    log_printf("ReadRxQueueIrigOne: Error %d\n", len);
                }
                break;      // end thread
            }

            // we have a valid word
            m_bufrxLen = 1;
            #ifdef __WXMSW__
            postEvent(MCDU_SIM___CUSTOM_EVENT_RX);
            #endif

            // lock RX in a mutex
            // Wait indefinitely until the semaphore count becomes strictly positive and then decrement it and return.

            // TODO: Block this BG thread

            //if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL("Fail to lock RX thread"); }
            //if (m_Sem.Wait() != wxSEMA_NO_ERROR) { CATFAIL("Fail to lock RX thread"); }
            sleepCounter = 0;

        }

    } // while(1)

    //(im) If this function exits, the wxThread is deleted, which also deletes the
    //     transport driver. Unfortunately, other threads are still using it.
    //     So just sleep until the app exits.
    for(;;) Sleep(1000);

    return (void *)NULL; // never actually returns, but wxThread::Entry insists on returning a void *
}

////////////////////////////////////////////////////////////

#ifdef __WXMSW__    // part of MCDU Simulator
// these are included in the parent class CTransport
#else               // part of Display Interface Manager (DIM) Simulator

bool CTransportUdp::Has_Transport_Started(void)
{
    return m_transport_start;
}

bool CTransportUdp::Has_Transport_Failed(void)
{
    return m_transport_fail;
}

void CTransportUdp::Set_Transport_Init(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_udp_haldler) {
    int MCDU_ID = m_udp_haldler->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Init  %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, false, false);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_start = false;
    m_transport_fail = false;
}

void CTransportUdp::Set_Transport_Started(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_udp_haldler) {
    int MCDU_ID = m_udp_haldler->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Start %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, true, false);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_start = true;
    m_transport_fail = false;
}

void CTransportUdp::Set_Transport_Failed(void)
{
    if (WANT_DEBUG_TRANSPORT_STATUS) {
    if (m_udp_haldler) {
    int MCDU_ID = m_udp_haldler->MCDU()+1;
    log_printf("#%d Transport status = %X %X; _Fail  %X %X\n", MCDU_ID, m_transport_start, m_transport_fail, m_transport_start, true);
    }
    } //WANT_DEBUG_TRANSPORT_STATUS
    m_transport_fail = true;
}

#endif

////////////////////////////////////////////////////////////

// end of transportPipe.cpp
