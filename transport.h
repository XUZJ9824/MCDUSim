#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__
/*****************************************************************
 *
 * Name:        transport.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transport.h,v 1.8 2012/01/13 18:28:30 macrae.i Exp $
 */
#include "wx/wx.h"

class CTransport
{
public:
    CTransport();
    virtual ~CTransport();
    virtual bool init ( wxEvtHandler * eventObjPtr, int port ) = 0;
    virtual bool sendWord ( long txWord, int mcdu ) = 0;
    virtual bool getRxWord ( int connectionID, long * rxWord) = 0;

    void postEvent( int eventID );

private:
    wxEvtHandler * m_parentEventObjPtr;

protected:
    bool        bFinished_init;
    bool        bLog_everyTx;
    bool        bLog_nextTx;
public:
    void        SetFinishedInit(void)   { bFinished_init = true; }
    bool        IsFinishedInit(void)    { return bFinished_init; }

    bool        LoggingThisTx(void) { return (bLog_everyTx || bLog_nextTx); }
    void        DontLogThisTx(void) { bLog_nextTx = false; }

protected:
    bool        m_transport_start;
    bool        m_transport_fail;
public:
    bool        Has_Transport_Started(void);
    bool        Has_Transport_Failed(void);

    void        Set_Transport_Init(void);
    void        Set_Transport_Started(void);
    void        Set_Transport_Failed(void);
};

/*------------------------------------------------------------------*/
#endif
