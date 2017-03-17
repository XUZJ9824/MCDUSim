/*****************************************************************
 *
 * Name:        transportPipe.h
 * Purpose:     part of Display Interface Manager (DIM) Simulator and MCDU Simulator
 * Author:      Ian MacRae
 * Created:     July 2011
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transportPipe.h,v 1.17 2012/02/17 19:00:29 macrae.i Exp $
 */

#ifndef __TRANSPORTPIPE_H__
#define __TRANSPORTPIPE_H__

//#include <windows.h>    // for HANDLE
    #ifdef __WXMSW__
#include "transport.h"
    #endif //__WXMSW__
//#include "useful.h"

#ifndef ONRX_HELPER_FUNCTION
typedef void (* ONRX_HELPER_FUNCTION        )(void * pThis, long wordRx, int which_mcdu);
typedef void (* ONREADERFAIL_HELPER_FUNCTION)(void * pThis, int which_mcdu);
typedef void (* ONWRITERFAIL_HELPER_FUNCTION)(void * pThis, int which_mcdu);
#endif

#define WANT_PIPE_STATISTICS        1   // 0=normal, 1=debugging

#include "MsecTime.h"

    #ifdef __WXMSW__    // part of MCDU Simulator
    #else               // part of Display Interface Manager (DIM) Simulator
class CTransportMCDU;
    #endif
class CTransportPipe;

class CNamedPipe {
public:
    CNamedPipe::CNamedPipe(int which_mcdu, char * s_DI_Host, CTransportPipe * parent);
    CNamedPipe::~CNamedPipe();

    HANDLE  CreateReaderEndOfPipe(char * host_name, char * end_name);   // used by BG Reader thread
    bool    SetupReaderEndOfPipe(HANDLE hPipeReader);                   // used by BG Reader thread
    DWORD   SetupWriterEndOfPipe(char * host_name, char * end_name);    // used by BG Writer thread
    HANDLE  OpenWriterEndOfPipe(void);                                  // used by BG Writer thread

private:
    CTransportPipe *        m_parent;       // the transpoart layer that is using this named pipe
public:
    CTransportPipe *        Transport() { return m_parent; }

private:
    unsigned int            m_PipeStatus;   // 0=not initialized yet, 1=init OK
    int                     m_which_mcdu;   // 0,1,2,3
    char                    m_DI_Host[100]; // host name where Display Interface is running (eg ".")

    char                    m_sPipeName_Server[120];
    char                    m_sPipeName_Client[120];
    wchar_t                 m_sPipeName_Server_Wide[120];
    wchar_t                 m_sPipeName_Client_Wide[120];

public:                     // for use in BG threads for error messages
    char *                  PipeName_Server(void) { return m_sPipeName_Server; }
    char *                  PipeName_Client(void) { return m_sPipeName_Client; }

private:
    BOOL                    m_bQuittingTime;
    HANDLE                  m_hWriterThread;
    HANDLE                  m_hReaderThread;
    unsigned int            m_nWriterThreadId;
    unsigned int            m_nReaderThreadId;

    bool                    m_bAllow_Write;
    bool                    m_bAllow_Read;
    HANDLE                  m_WriterEvent;
    HANDLE                  m_SendWordEvent;

public:

    int             MCDU(void)                  { return m_which_mcdu; }
    char *          DI_Host(void)               { return m_DI_Host; }

    unsigned int    PipeStatus(void)            { return m_PipeStatus; }

    BOOL            QuittingTime(void)          { return m_bQuittingTime; }
    void            SetQuittingTime(void)       {
        m_bQuittingTime = true;
        // poke the BG threads so they will notice it is time to go
        SetEvent(m_WriterEvent);      // poke BG Writer thread 
        SetEvent(m_SendWordEvent);    // poke BG Reader thread
    }

public:
    HANDLE          WakeupWriterEvent(void)     { return m_WriterEvent; }
    HANDLE          WakeupSendWordEvent(void)   { return m_SendWordEvent; }

    HANDLE          WriterThread(void)          { return m_hWriterThread; }
    HANDLE          ReaderThread(void)          { return m_hReaderThread; }

    unsigned int    WriterThreadId(void)        { return m_nWriterThreadId; }
    unsigned int    ReaderThreadId(void)        { return m_nReaderThreadId; }

public:
    bool            Is_Write_Allowed(void)      { return m_bAllow_Write; }
    bool            Is_Read_Allowed(void)       { return m_bAllow_Read; }
    void            Allow_Write(bool b=true)    { m_bAllow_Write = b; }
    void            Allow_Read(bool b=true)     { m_bAllow_Read = b; }

private:
    void *                  m_RxCallback_class;
    ONRX_HELPER_FUNCTION    m_RxCallback_function;
public:
    void            SetRxCallback(void * pThis, ONRX_HELPER_FUNCTION function);
    void            Call_OnRx(long wordRx);

private:
    void *                          m_ReaderFail_Callback_class;
    ONREADERFAIL_HELPER_FUNCTION    m_ReaderFail_Callback_function;
public:
    void            Set_ReaderFail_Callback(void * pThis, ONREADERFAIL_HELPER_FUNCTION function);
    void            Call_OnReaderFail(void);

private:
    void *                          m_WriterFail_Callback_class;
    ONWRITERFAIL_HELPER_FUNCTION    m_WriterFail_Callback_function;
public:
    void            Set_WriterFail_Callback(void * pThis, ONWRITERFAIL_HELPER_FUNCTION function);
    void            Call_OnWriterFail(void);

private:
    #define         CNamedPipe_QUEUE_SIZE   300  // room for 299 elements
    unsigned long   m_Pipe_Queue[CNamedPipe_QUEUE_SIZE];
    unsigned int    m_Pipe_Queue_Next_Write;    // index to write to queue:  0..CNamedPipe_QUEUE_SIZE-1
    unsigned int    m_Pipe_Queue_Next_Read;     // index to read from queue: 0..CNamedPipe_QUEUE_SIZE-1
    //#if WANT_PIPE_STATISTICS
    unsigned int    m_Pipe_Queue_High_Water_Mark;   // statistics
    //#endif //WANT_PIPE_STATISTICS

public:
    unsigned int Pipe_Queue_Count(void) {       // number of elements in the queue = 0..CNamedPipe_QUEUE_SIZE-1
        return (CNamedPipe_QUEUE_SIZE + m_Pipe_Queue_Next_Write - m_Pipe_Queue_Next_Read) % CNamedPipe_QUEUE_SIZE;
    }

    //#if WANT_PIPE_STATISTICS
    unsigned int Pipe_Queue_High_Water_Mark(void) {
        return m_Pipe_Queue_High_Water_Mark;
    }
    //#endif //WANT_PIPE_STATISTICS

    void Zap_Transport_Queue(void) {
        m_Pipe_Queue_Next_Write = m_Pipe_Queue_Next_Read = 0;
    }

    bool Pipe_Queue_Empty(void) {
        return m_Pipe_Queue_Next_Write == m_Pipe_Queue_Next_Read; //or Pipe_Queue_Count()==0;
    }

    bool Pipe_Queue_Full(void) {
        unsigned int write_next = (m_Pipe_Queue_Next_Write + 1) % CNamedPipe_QUEUE_SIZE;
        return (write_next == m_Pipe_Queue_Next_Read); //or Pipe_Queue_Count()>=(CNamedPipe_QUEUE_SIZE-1);
    }

    bool Write_Pipe_Queue(unsigned long data) {
        unsigned int write_here = m_Pipe_Queue_Next_Write;
        if (!Pipe_Queue_Full()) {
            m_Pipe_Queue_Next_Write = (m_Pipe_Queue_Next_Write + 1) % CNamedPipe_QUEUE_SIZE;
            m_Pipe_Queue[write_here] = data;

            #if WANT_PIPE_STATISTICS
            unsigned long pipe_count = Pipe_Queue_Count();
            if (pipe_count > m_Pipe_Queue_High_Water_Mark) {
                m_Pipe_Queue_High_Water_Mark = pipe_count;
            }
            #endif //WANT_PIPE_STATISTICS

            return true;
        } /* else OOPS! full */
        return false;
    }

    unsigned long Read_Pipe_Queue(void) {
        unsigned int read_here = m_Pipe_Queue_Next_Read;
        unsigned long data = (~0L);
        if (!Pipe_Queue_Empty()) {
            m_Pipe_Queue_Next_Read = (m_Pipe_Queue_Next_Read + 1) % CNamedPipe_QUEUE_SIZE;
            data = m_Pipe_Queue[read_here];
        } /* else OOPS! empty */
        return data;
    }

public:
    bool    IsAcceptingRx(void)             { return m_bAcceptingRx; }
    void    SetAcceptingRx(bool b=true)     { m_bAcceptingRx = b; }

protected:
    bool    m_bAcceptingRx;

public:
};

#define BUF_RX_LEN_TRANSPORT_PIPE 100

class CTransportPipe
    #ifdef __WXMSW__
                    : public CTransport
    #endif //__WXMSW__
{
public:
    #ifdef __WXMSW__    // part of MCDU Simulator
    CTransportPipe(int which_mcdu/*0,1,2,3*/, char * s_DI_Host  );
    #else               // part of Display Interface Manager (DIM) Simulator
    CTransportPipe(int which_mcdu/*0,1,2,3*/, char * s_DI_Host, CTransportMCDU * parent);
    #endif
    virtual ~CTransportPipe();

    #ifdef __WXMSW__
    virtual bool init ( wxEvtHandler * eventObjPtr, int port );
    #else
    virtual bool init ( void * eventObjPtr, int port );
    #endif
    virtual bool Tx_Debug_helper(bool * bNotYet, bool * bQueueFull); /* helper for debug and flow control */
    virtual bool sendWord ( long txWord, int mcdu ); /* true = success, false = failure */
    virtual bool getRxWord ( int connectionID, long * rxWord);

    virtual void * Entry();

    void UNLOCK_RX(void);

private:
    CNamedPipe *    m_pipe;

    long            m_bufrx[BUF_RX_LEN_TRANSPORT_PIPE];
    int             m_bufrxLen;
    DWORD           m_dwRxData;

private:
    //wxSemaphore             m_Sem;

    #ifdef __WXMSW__    // part of MCDU Simulator
    // these are included in the parent class CTransport
    #else               // part of Display Interface Manager (DIM) Simulator
protected:
    bool        bFinished_init;
public:
    void        SetFinishedInit(void)   { bFinished_init = true; }
    bool        IsFinishedInit(void)    { return bFinished_init; }
    #endif

public:
    void SetRxCallback(void * pThis, ONRX_HELPER_FUNCTION function) {
        m_pipe->SetRxCallback(pThis, function);
    }

    void Set_ReaderFail_Callback(void * pThis, ONREADERFAIL_HELPER_FUNCTION function) {
        m_pipe->Set_ReaderFail_Callback(pThis, function);
    }

    void Set_WriterFail_Callback(void * pThis, ONWRITERFAIL_HELPER_FUNCTION function) {
        m_pipe->Set_WriterFail_Callback(pThis, function);
    }

    void Zap_Transport_Queue(void) { m_pipe->Zap_Transport_Queue(); }

public:
    void            SetQuittingTime(void)   { m_pipe->SetQuittingTime(); }
    unsigned int    PipeStatus(void)        { return m_pipe->PipeStatus(); }
    bool            Pipe_Queue_Full(void)   { return m_pipe->Pipe_Queue_Full(); }
    bool            Is_Write_Allowed(void)  { return m_pipe->Is_Write_Allowed(); }
    int             MCDU(void)              { return m_pipe->MCDU(); }

    #ifdef __WXMSW__    // part of MCDU Simulator
    // these are included in the parent class CTransport
    #else               // part of Display Interface Manager (DIM) Simulator
protected:
    bool        m_transport_start;
    bool        m_transport_fail;
public:
    bool        Has_Transport_Started(void);
    bool        Has_Transport_Failed(void);

    void        Set_Transport_Init(void);
    void        Set_Transport_Started(void);
    void        Set_Transport_Failed(void);
    #endif
};

#endif /* __TRANSPORTPIPE_H__ */
// end of transportPipe.h
