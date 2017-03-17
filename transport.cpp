/*****************************************************************
 *
 * Name:        transport.cpp
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: transport.cpp,v 1.10 2012/01/13 18:28:30 macrae.i Exp $
 */

#define WANT_DEBUG_TRANSPORT_STATUS 0   // 0=normal, 1=debug

#include "transport.h"
#include "defs.h"

CTransport::CTransport()
{
    m_parentEventObjPtr = NULL;

    m_transport_start   = false;
    m_transport_fail    = false;
    bFinished_init      = false;
    Set_Transport_Init();       // hasn't started or failed yet

    bLog_everyTx = false;
    bLog_nextTx  = true;        // first one might be interesting
}

CTransport::~CTransport()
{
    do {} while (0);
}

bool CTransport::init ( wxEvtHandler * eventObjPtr, int port )
{
    /*(im) this is a pure virtual function, so this code will never be executed
     *     unless somebody calls CTransport::init explicitly. Everyone does ...
     */
    m_parentEventObjPtr = eventObjPtr;
    //(im) Never pretend the hardware is ready! Otherwise, it looks like the connection is up, so
    //     code could start using hardware that may not be there, or hasn't been initialized yet!
    //Set_Transport_Started();    /* pretend the hardware is ready for use */
    return true; // (im) make compiler happy
}

void CTransport::postEvent( int eventID )
{
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventID);
    m_parentEventObjPtr->AddPendingEvent(event);
    //log_printf("Post event %d", eventID);
}

////////////////////////////////////////////////////////////

int log_printf(const char *format, ...); /* in log_printf.cpp */

bool CTransport::Has_Transport_Started(void)
{
    return m_transport_start;
}

bool CTransport::Has_Transport_Failed(void)
{
    return m_transport_fail;
}

void CTransport::Set_Transport_Init(void)
{
    #if WANT_DEBUG_TRANSPORT_STATUS
    log_printf("Transport status = %X %X; _Init now %X %X\n", m_transport_start, m_transport_fail, false, false);
    #endif
    m_transport_start = false;
    m_transport_fail = false;
}

void CTransport::Set_Transport_Started(void)
{
    #if WANT_DEBUG_TRANSPORT_STATUS
    log_printf("Transport status = %X %X; _Started now %X %X\n", m_transport_start, m_transport_fail, true, false);
    #endif
    m_transport_start = true;
    m_transport_fail = false;
}

void CTransport::Set_Transport_Failed(void)
{
    #if WANT_DEBUG_TRANSPORT_STATUS
    log_printf("Transport status = %X %X; _Failed now %X %X\n", m_transport_start, m_transport_fail, m_transport_start, true);
    #endif
    m_transport_fail = true;
}

////////////////////////////////////////////////////////////

//--------------------------------------------------
