/* mTerm
 Copyright(C) 1994-1998 T. Teranishi
 All rights reserved. */

/* TERATERM.EXE, main */

/*  
changes by Marcelo Varanda when MV_CHANGE is defined:
    
    binary frames received via Serial is routed to UDP ports 5000, 5001 and 5002 (broadcasted)
    frames received via UDP port 6000 is sent to serial

    Frame format:

       |  H  |  D  | b0 | b1 | b2 | b3 |  P  |
       where: H = EXTERN_SIM__FRAME_HEADER (control A character) defined in commlib.c
              D = MCDU numbers (ex: 0,1 and 2) or 0xFF for all MCDUs
              d0~d3 -> lval bytes
              P = parity -> xor between all previous 5 bytes

    target host default is "localhost". It can be changed with /M= parameter, exe:
            ttermpro /M=192.168.10.10
  

*/


#include "stdafx.h"
#include "teraterm.h"
#include "tttypes.h"
#include "commlib.h"
#include "ttwinman.h"
#include "buffer.h"
#include "vtterm.h"
#include "vtwin.h"
#include "clipboar.h"
#include "ttftypes.h"
#include "filesys.h"
#include "telnet.h"
#include "tektypes.h"
#include "tekwin.h"
#include "ttdde.h"
#ifndef TERATERM32
  #include "ttctl3d.h"
#endif

#include "teraapp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTeraApp, CWinApp)
	//{{AFX_MSG_MAP(CTeraApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTeraApp::CTeraApp()
{
}

// CTeraApp instance
CTeraApp theApp;
// CTeraApp initialization
BOOL CTeraApp::InitInstance()
{
  hInst = m_hInstance;
#ifndef TERATERM32
  LoadCtl3d(m_hInstance);
#endif

  m_pMainWnd = new CVTWindow();
  CommInitUdp();
  CommInitTcpServer();

  pVTWin = m_pMainWnd;
  return TRUE;
}

int CTeraApp::ExitInstance()
{
#ifdef TERATERM32
  return CWinApp::ExitInstance();
#else
  FreeCtl3d();
  return 0;
#endif
}

// mTerm main engine
BOOL CTeraApp::OnIdle(LONG lCount)
{
  static int Busy = 2;
  int Change, nx, ny;
  BOOL Size;

  if (lCount==0) Busy = 2;

  if (cv.Ready)
  {
    /* Sender */
    CommSend(&cv);

    /* Parser */
    if ((cv.HLogBuf!=NULL) && (cv.LogBuf==NULL))
      cv.LogBuf = (PCHAR)GlobalLock(cv.HLogBuf);

    if ((cv.HBinBuf!=NULL) && (cv.BinBuf==NULL))
      cv.BinBuf = (PCHAR)GlobalLock(cv.HBinBuf);

    if ((TelStatus==TelIdle) && cv.TelMode)
      TelStatus = TelIAC;

    if (TelStatus != TelIdle)
    {
      ParseTel(&Size,&nx,&ny);
      if (Size) {
	LockBuffer();
	ChangeTerminalSize(nx,ny);
	UnlockBuffer();
      }
    }
    else {
      if (cv.ProtoFlag) Change = ProtoDlgParse();
      else {
	switch (ActiveWin) {
	  case IdVT: Change =  ((CVTWindow*)pVTWin)->Parse(); break;
	  case IdTEK:
	    if (pTEKWin != NULL)
	      Change = ((CTEKWindow*)pTEKWin)->Parse();
	    else
	      Change = IdVT;
	    break;
	  default:
	    Change = 0;
	}

	switch (Change) {
	  case IdVT: VTActivate(); break;
	  case IdTEK: ((CVTWindow*)pVTWin)->OpenTEK(); break;
	}
      }
    }

    if (cv.LogBuf!=NULL)
    {
      if (FileLog) LogToFile();
      if (DDELog && AdvFlag) DDEAdv();
      GlobalUnlock(cv.HLogBuf);
      cv.LogBuf = NULL;
    }

    if (cv.BinBuf!=NULL)
    {
      if (BinLog) LogToFile();
      GlobalUnlock(cv.HBinBuf);
      cv.BinBuf = NULL;
    }

    /* Talker */
    switch (TalkStatus) {
      case IdTalkCB: CBSend(); break; /* clip board */
      case IdTalkFile: FileSend(); break; /* file */
    }

    /* Receiver */
    CommReceive(&cv);

  }

#ifdef MV_CHANGE
        if (cv.Ready &&
      (cv.RRQ || (cv.OutBuffCount>0) || (cv.InBuffCount>0) ||
       (cv.LCount>0) || (cv.BCount>0) || (cv.DCount>0)) || (cv.pendingHDRxBytes>0) )
#else
        if (cv.Ready &&
      (cv.RRQ || (cv.OutBuffCount>0) || (cv.InBuffCount>0) ||
       (cv.LCount>0) || (cv.BCount>0) || (cv.DCount>0)) )
#endif
    Busy = 2;
  else
    Busy--;


  return (Busy>0);
}

BOOL CTeraApp::PreTranslateMessage(MSG* pMsg)
{
  if (ts.MetaKey>0)
    return FALSE; /* ignore accelerator keys */
  else
    return CWinApp::PreTranslateMessage(pMsg);
}

