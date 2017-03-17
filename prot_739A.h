#ifndef __PROT_739A_H__
#define __PROT_739A_H__
/*****************************************************************
 *
 * Name:        prot_739A.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: prot_739A.h,v 1.11 2012/01/13 18:28:29 macrae.i Exp $
 */
#include "defs.h"
#include "wx/wx.h"
#include "wx/event.h"
#include "transportTcp.h"
#include "termScreen.h"


class CProt : public wxEvtHandler
{
public:
    CProt(u_short peerPort, bool ignoreSocketErrors);
    CProt( int mcdu, wxEvtHandler * eventObjPtr, u_short peerPort, bool ignoreSocketErrors);
    ~CProt();
    void CProt_Init(void); // handy place to initialize or re-initialize everything
    void UpdateDriver(int mcdu); // used to re-init driver layer after startup

    void OnRx(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);

    void setScreen(CTermScreen * screenPtr, CTermScreen * menuScreenPtr);
    void processState(int source, long par);
    void sendKey(int key);
    void sendMcduID(void);

    void OnTimer250ms(void);

    bool Waiting_on_ENQ_RTS_timeout(void);
    void Cancel_ENQ_RTS_Timeout(void);
    void Setup_ENQ_RTS_Timeout(void);
    void sendEnquire(int func/*0=text,1=menu*/, int check_which_state/*0=L1/Menu,1=from timer*/);

private:
    u_short                 m_peerPort;
    bool                    m_ignoreSocketErrors;
    int equipment_id;
    int scratchpad_row;
public:
    void setEquipmentInfo(int eqpt_id, int row) { equipment_id = eqpt_id; scratchpad_row = row; }

private:
    unsigned long m_simul_hsd_sal;
public:
    int  getSduSAL(void)              { return m_simul_hsd_sal; }
    void setSduSAL(unsigned long sal=0) { m_simul_hsd_sal = sal; }

protected:
    int m_hsd_sal_timer;
public:
    //void Cancel_SAL_Timeout(void);
    void Setup_SAL_Timeout(void);

private:
    CTransport * m_transport;
    int m_state;
public:
    int State(void) { return m_state; }

public:
    unsigned long unshuffle_arinc_data(unsigned long arinc_word, unsigned long *label);
    unsigned long shuffle_arinc_data(unsigned long data,unsigned int label);
    unsigned long invert_bits(unsigned long data,int num_bits);
    void send(unsigned long data, unsigned long label);
    unsigned long add_parity_and_ssm(unsigned long arinc_word, float percent_ssm_errors,int ssm);
    unsigned long add_parity_bit(unsigned long arinc_word);
    char m_dataFromHsd[128];
    int m_dataFromHsdIdx ;
    int m_line;
    int m_col;
    int m_size;
    int m_attribute;
    int m_color;
    CTermScreen * m_screenPtr;
    CTermScreen * m_menuScreenPtr;
    int m_mcdu;

protected:
    int m_mcduIDTimer;
    int m_enq_rts_timer;
    int m_enq_rts_retries;
    int m_rst_function;

public:
    void postEvent( int eventID );
    wxEvtHandler * m_parentEventObjPtr;

public:
    void OnReaderWriterFail(int which_mcdu, char * description);

DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
#endif
