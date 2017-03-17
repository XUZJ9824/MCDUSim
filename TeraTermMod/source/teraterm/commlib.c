/* mTerm
 Copyright(C) 1994-1998 T. Teranishi
 All rights reserved. */

#pragma warning(disable : 4996) /* don't want warning: 'strcpy' may be unsafe */
                                /* don't want warning: 'strcat' may be unsafe */
                                /* don't want warning: The POSIX name for 'stricmp' is deprecated */
                                /* don't want warning: 'sscanf' may be unsafe */

/* TERATERM.EXE, Communication routines */

#include "teraterm.h"
#include "tttypes.h"
#ifdef TERATERM32
#include "tt_res.h"
#include <process.h>
#else
#include "tt_res16.h"
#include <stdlib.h>
#include <string.h>
#endif

#include "ttcommon.h"
#include "ttwsk.h"
#include "ttlib.h"
#include "ttplug.h" /* TTPLUG */

#include "commlib.h"

#define ErrorCaption "mTerm: Error"
#define ErrorCantConn "Cannot connect the host"
#define CommInQueSize 8192
#define CommOutQueSize 2048
#define CommXonLim 2048
#define CommXoffLim 2048

#ifdef TERATERM32
#define READENDNAME "ReadEnd"
#define WRITENAME "Write"
#define READNAME "Read"
#define PRNWRITENAME "PrnWrite"

static HANDLE ReadEnd;
static OVERLAPPED wol, rol;
#endif

// Winsock async operation handle
static HANDLE HAsync=0;

BOOL TCPIPClosed = TRUE;

/* Printer port handle for
   direct pass-thru printing */
#ifdef TERATERM32
static HANDLE PrnID = INVALID_HANDLE_VALUE;
#else
static int PrnID = -1;
#endif
static BOOL LPTFlag;

#ifdef MV_CHANGE
#define         EXTERN_SIM__FRAME_HEADER    0x01
#define         EXTERN_SIM__FRAME_HEADER2   0x01 //0x30
#define         EXTERN_SIM__FRAME_LEN       7
#define         LISTEN_PORT_FOR_GETTING_DATA_FROM_MCDUS     6000
#define         MCDU1_PORT                  5000
#define         MCDU2_PORT                  5001
#define         MCDU3_PORT                  5002
#define         MCDU4_PORT                  5003
#define         BUF_RX_LEN 128
#define         ERRORMSG(x) MessageBox(NULL,x,"Error",MB_TASKMODAL | MB_ICONEXCLAMATION)
#define         BUF_TCP_SERVER_RX_LEN  1024
#define         TCP_SERVER_PORT            9000

    static void UdpThread(void *arg);
    static struct sockaddr_in      m_mcdu_socket_addr;
    static struct sockaddr_in      m_client_socket_addr;
    static int                     m_socketID;
    static char                    m_bufrx[BUF_RX_LEN];
    static int                     m_bufrxLen;
    static int                     m_bufrxIdxOut;
    static PComVar                 m_cv = NULL;
    static struct sockaddr_in      mcduDummy1_Addr;
    static struct sockaddr_in      mcdu1_Addr;
    static struct sockaddr_in      mcdu2_Addr;
    static struct sockaddr_in      mcdu3_Addr;
    static struct sockaddr_in      mcdu4_Addr;
    static char                    m_mcdusIP[128] = {"127.0.0.1"};

    static SOCKET                   m_clientSocket = -1;
    static int                      m_clientLen = 0;
    static struct sockaddr_in       m_cliAddr;
    static SOCKET                   m_sockfd = -1;
    static struct sockaddr_in       m_serv_addr;
    static void                     TcpServerThread(void *arg);
    static char                     m_tcpBufRx[BUF_TCP_SERVER_RX_LEN];
    static int                      m_tcpBufRxLen;
    static int                      m_tcpBufRxIdxOut;
    static int                      m_tcpServerPort;

#define BinToPrintableHex(x) ((x)<=9)?('0'+(x)):('A'+(x)-10)

    HANDLE                  m_udpmutex;
    CRITICAL_SECTION       CriticalSection;
/* definitions for paasing arinc messages coming from com channel
   to relevan udp port which is msgig(byte with index 1 from incoming message)
   minus ARINC_SIM_ID_0 plus ARINC_SIM_PORT_0 vgorine 19.03.2010 */
/* first message id used for passing arinc messages over com channel */
#define ARINC_SIM_ID_0                10
/* last message id used for passing arinc messages over com channel */
#define ARINC_SIM_ID_LAST             100
/* udp port used for passing arinc data with message id ARINC_SIM_ID_0 */
#define ARINC_SIM_PORT_0              5010
    static struct sockaddr_in         arincSim_Addr;
    static char                       m_arincSimIP[128] = {"127.0.0.1"};
#endif
int processRxFilter( BYTE * charPtr, PComVar cv );


// Initialize ComVar.
// This routine is called only once
// by the initialization procedure of mTerm.
void CommInit(PComVar cv)
{
#ifdef MV_CHANGE
    m_cv = cv;
    cv->pendingHDRxBytes = 0;
    cv->validHsdFlag = 0;
#endif

  cv->Open = FALSE;
  cv->Ready = FALSE;

// log-buffer variables
  cv->HLogBuf = 0;
  cv->HBinBuf = 0;
  cv->LogBuf = NULL;
  cv->BinBuf = NULL;
  cv->LogPtr = 0;
  cv->LStart = 0;
  cv->LCount = 0;
  cv->BinPtr = 0;
  cv->BStart = 0;
  cv->BCount = 0;
  cv->DStart = 0;
  cv->DCount = 0;
  cv->BinSkip = 0;
  cv->FilePause = 0;
  cv->ProtoFlag = FALSE;
/* message flag */
  cv->NoMsg = 0;


}

/* reset a serial port which is already open */
void CommResetSerial(PTTSet ts, PComVar cv)
{
  DCB dcb;
#ifdef TERATERM32
  DWORD DErr;
  COMMTIMEOUTS ctmo;
#else
  COMSTAT Stat;
  BYTE b;
#endif

  if (! cv->Open ||
      (cv->PortType != IdSerial)) return;

#ifdef TERATERM32
  ClearCommError(cv->ComID,&DErr,NULL);
  SetupComm(cv->ComID,CommInQueSize,CommOutQueSize);
  /* flush input and output buffers */
  PurgeComm(cv->ComID, PURGE_TXABORT | PURGE_RXABORT |
    PURGE_TXCLEAR | PURGE_RXCLEAR);

  memset(&ctmo,0,sizeof(ctmo));
  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.WriteTotalTimeoutConstant = 500;
  SetCommTimeouts(cv->ComID,&ctmo);
#else
  while (GetCommError(cv->ComID, &Stat)!=0) {};
  /* flush input and output buffers */
  FlushComm(cv->ComID,0);
  FlushComm(cv->ComID,1);
#endif
  cv->InBuffCount = 0;
  cv->InPtr = 0;
  cv->OutBuffCount = 0;
  cv->OutPtr = 0;

  cv->DelayPerChar = ts->DelayPerChar;
  cv->DelayPerLine = ts->DelayPerLine;

#ifdef TERATERM32
  memset(&dcb,0,sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
#else
  GetCommState(cv->ComID,&dcb);
  b = dcb.Id;
  memset(&dcb,0,sizeof(DCB));
  dcb.Id = b;
#endif
  switch (ts->Baud) {
    case IdBaud110: dcb.BaudRate = 110; break;
    case IdBaud300: dcb.BaudRate = 300; break;
    case IdBaud600: dcb.BaudRate = 600; break;
    case IdBaud1200: dcb.BaudRate = 1200; break;
    case IdBaud2400: dcb.BaudRate = 2400; break;
    case IdBaud4800: dcb.BaudRate = 4800; break;
    case IdBaud9600: dcb.BaudRate = 9600; break;
    case IdBaud14400: dcb.BaudRate = 14400; break;
    case IdBaud19200: dcb.BaudRate = 19200; break;
    case IdBaud38400: dcb.BaudRate = 38400; break;
    case IdBaud57600: dcb.BaudRate = 57600; break;
#ifdef TERATERM32
    case IdBaud115200: dcb.BaudRate = 115200; break;
#endif
  }
  dcb.fBinary = TRUE;
  switch (ts->Parity) {
    case IdParityEven:
      dcb.fParity = TRUE;
      dcb.Parity = EVENPARITY;
      break;
    case IdParityOdd:
      dcb.fParity = TRUE;
      dcb.Parity = ODDPARITY;
      break;
    case IdParityNone:
      dcb.Parity = NOPARITY;
      break;
  }

#ifdef TERATERM32
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
#endif
  switch (ts->Flow) {
    case IdFlowX:
      dcb.fOutX = TRUE;
      dcb.fInX = TRUE;
      dcb.XonLim = CommXonLim;
      dcb.XoffLim = CommXoffLim;
      dcb.XonChar = XON;
      dcb.XoffChar = XOFF;
      break;
    case IdFlowHard:
      dcb.fOutxCtsFlow = TRUE;
#ifdef TERATERM32
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
#else
      dcb.CtsTimeout = 30;
      dcb.fRtsflow = TRUE;
#endif
      break;
  }

  switch (ts->DataBit) {
    case IdDataBit7: dcb.ByteSize = 7; break;
    case IdDataBit8: dcb.ByteSize = 8; break;
  }
  switch (ts->StopBit) {
    case IdStopBit1: dcb.StopBits = ONESTOPBIT; break;
    case IdStopBit2: dcb.StopBits = TWOSTOPBITS; break;
  }

#ifdef TERATERM32
  SetCommState(cv->ComID, &dcb);

  /* enable receive request */
  SetCommMask(cv->ComID,0);
  SetCommMask(cv->ComID,EV_RXCHAR);
#else
  SetCommState(&dcb);

  /* enable receive request */
  SetCommEventMask(cv->ComID,0);
  SetCommEventMask(cv->ComID,EV_RXCHAR);
#endif
}

void CommOpen(HWND HW, PTTSet ts, PComVar cv)
{
  WORD COMFlag;
  int Err;
  char ErrMsg[21];
  char P[50];

  MSG Msg;
  char HEntBuff[MAXGETHOSTSTRUCT];
  u_long addr;
  SOCKADDR_IN saddr;

  BOOL InvalidHost;
  BOOL BBuf;

  /* initialize ComVar */
  cv->InBuffCount = 0;
  cv->InPtr = 0;
  cv->OutBuffCount = 0;
  cv->OutPtr = 0;
  cv->HWin = HW;
  cv->Ready = FALSE;
  cv->Open = FALSE;
  cv->PortType = ts->PortType;
  cv->ComPort = 0;
  cv->RetryCount = 0;
  cv->s = INVALID_SOCKET;
#ifdef TERATERM32
  cv->ComID = INVALID_HANDLE_VALUE;
#else
  cv->ComID = -1;
#endif
  cv->CanSend = TRUE;
  cv->RRQ = FALSE;
  cv->SendKanjiFlag = FALSE;
  cv->SendCode = IdASCII;
  cv->EchoKanjiFlag = FALSE;
  cv->EchoCode = IdASCII;
  cv->Language = ts->Language;
  cv->CRSend = ts->CRSend;
  cv->KanjiCodeEcho = ts->KanjiCode;
  cv->JIS7KatakanaEcho = ts->JIS7Katakana;
  cv->KanjiCodeSend = ts->KanjiCodeSend;
  cv->JIS7KatakanaSend = ts->JIS7KatakanaSend;
  cv->KanjiIn = ts->KanjiIn;
  cv->KanjiOut = ts->KanjiOut;
  cv->RussHost = ts->RussHost;
  cv->RussClient = ts->RussClient;
  cv->DelayFlag = TRUE;
  cv->DelayPerChar = ts->DelayPerChar;
  cv->DelayPerLine = ts->DelayPerLine;
  cv->TelBinRecv = FALSE;
  cv->TelBinSend = FALSE;
  cv->TelFlag = FALSE;
  cv->TelMode = FALSE;
  cv->IACFlag = FALSE;
  cv->TelCRFlag = FALSE;
  cv->TelCRSend = FALSE;
  cv->TelCRSendEcho = FALSE;
  cv->TelAutoDetect = TRUE; /* TTPLUG */

  if ((ts->PortType!=IdSerial) && (strlen(ts->HostName)==0))
  {
    PostMessage(cv->HWin, WM_USER_COMMNOTIFY, 0, FD_CLOSE);
    return;
  }

  switch (ts->PortType) {
    case IdTCPIP:
      cv->TelFlag = (ts->Telnet > 0);
      if (! LoadWinsock())
      {
	if (cv->NoMsg==0)
	  MessageBox(cv->HWin,"Cannot use winsock",ErrorCaption,
	    MB_TASKMODAL | MB_ICONEXCLAMATION);
	InvalidHost = TRUE;
      }
      else {
        TTXOpenTCP(); /* TTPLUG */
	cv->Open = TRUE;
	if ((ts->HostName[0] >= 0x30) && (ts->HostName[0] <= 0x39))
	{
	  addr = Pinet_addr(ts->HostName);
	  InvalidHost = (addr == 0xffffffff);
	}
	else {
	  HAsync = PWSAAsyncGetHostByName(HW,WM_USER_GETHOST,
	  ts->HostName,HEntBuff,sizeof(HEntBuff));
	  if (HAsync == 0)
	    InvalidHost = TRUE;
	  else {
	    cv->ComPort = 1; // set "getting host" flag
			     //  (see CVTWindow::OnSysCommand())
	    do {
	      if (GetMessage(&Msg,0,0,0))
	      {
		if ((Msg.hwnd==HW) &&
		    ((Msg.message == WM_SYSCOMMAND) &&
		     ((Msg.wParam & 0xfff0) == SC_CLOSE) ||
		     (Msg.message == WM_COMMAND) &&
		     (LOWORD(Msg.wParam) == ID_FILE_EXIT) ||
		     (Msg.message == WM_CLOSE)))
		{ /* Exit when the user closes mTerm */
		  PWSACancelAsyncRequest(HAsync);
		  HAsync = 0;
		  cv->ComPort = 0; // clear "getting host" flag
		  PostMessage(HW,Msg.message,Msg.wParam,Msg.lParam);
		  return;
		}
		if (Msg.message != WM_USER_GETHOST)
		{ /* Prosess messages */
		  TranslateMessage(&Msg);
		  DispatchMessage(&Msg);
		}
	      }
	      else {
		return;
	      }
	    } while (Msg.message!=WM_USER_GETHOST);
	    cv->ComPort = 0; // clear "getting host" flag
	    HAsync = 0;
	    InvalidHost = WSAGETASYNCERROR(Msg.lParam) != 0;
	    if (! InvalidHost)
	    {
	      if (((PHOSTENT)HEntBuff)->h_addr_list != NULL)
		memcpy(&addr,
		  ((PHOSTENT)HEntBuff)->h_addr_list[0],sizeof(addr));
	      else
		InvalidHost = TRUE;
	    }
	  }

	}

	if (InvalidHost)
	{
	  if (cv->NoMsg==0)
	    MessageBox(cv->HWin,"Invalid host",ErrorCaption,
	      MB_TASKMODAL | MB_ICONEXCLAMATION);
	}
	else {
	  cv->s= Psocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	  if (cv->s==INVALID_SOCKET)
	  {
	    InvalidHost = TRUE;
	    if (cv->NoMsg==0)
	      MessageBox(cv->HWin,ErrorCantConn,ErrorCaption,
		MB_TASKMODAL | MB_ICONEXCLAMATION);
	  }
	  else {
	    BBuf = TRUE;
	    Psetsockopt(cv->s,(int)SOL_SOCKET,SO_OOBINLINE,(char FAR *)&BBuf,sizeof(BBuf));

	    PWSAAsyncSelect(cv->s,cv->HWin,WM_USER_COMMOPEN, FD_CONNECT);
	    saddr.sin_family = AF_INET;
	    saddr.sin_port = Phtons(ts->TCPPort);
	    saddr.sin_addr.s_addr = addr;
	    memset(saddr.sin_zero,0,8);

	    Err = Pconnect(cv->s,(LPSOCKADDR)&saddr,sizeof(saddr));
	    if (Err!=0 ) Err = PWSAGetLastError();
	    if (Err==WSAEWOULDBLOCK )
	    {
	      /* Do nothing */
	    }
	    else if (Err!=0 )
	      PostMessage(cv->HWin, WM_USER_COMMOPEN,0,
			  MAKELONG(FD_CONNECT,Err));
	  }
	}
      }
      break;

    case IdSerial:
      strcpy(P,"COM");
      uint2str(ts->ComPort,&P[3],2);
#ifdef TERATERM32
      strcpy(ErrMsg,P);
      strcpy(P,"\\\\.\\");
      strcat(P,ErrMsg);
      cv->ComID =
	CreateFile(P,GENERIC_READ | GENERIC_WRITE,
		   0,NULL,OPEN_EXISTING,
		   FILE_FLAG_OVERLAPPED,NULL);
      if (cv->ComID == INVALID_HANDLE_VALUE )
      {
#else
      cv->ComID = OpenComm(P,CommInQueSize,CommOutQueSize);
      if (cv->ComID <0)
      {
#endif
	strcpy(ErrMsg,"Cannot open ");
#ifdef TERATERM32
	strcat(ErrMsg,&P[4]);
#else
	strcat(ErrMsg,P);
#endif
	if (cv->NoMsg==0)
	  MessageBox(cv->HWin,ErrMsg,ErrorCaption,
	    MB_TASKMODAL | MB_ICONEXCLAMATION);
	InvalidHost = TRUE;
      }
      else {
	cv->Open = TRUE;
	cv->ComPort = ts->ComPort;
	CommResetSerial(ts,cv);

	/* notify to VT window that Comm Port is open */
	PostMessage(cv->HWin, WM_USER_COMMOPEN, 0, 0);
#ifndef TERATERM32
	// disable comm notification
	EnableCommNotification(cv->ComID,0,-1,-1);
#endif
	InvalidHost = FALSE;

	COMFlag = GetCOMFlag();
	COMFlag = COMFlag | (1 << (ts->ComPort-1));
	SetCOMFlag(COMFlag);
      }
      break; /* end of "case IdSerial:" */

    case IdFile:
#ifdef TERATERM32
      cv->ComID = CreateFile(ts->HostName,GENERIC_READ,0,NULL,
	OPEN_EXISTING,0,NULL);
      InvalidHost = (cv->ComID == INVALID_HANDLE_VALUE);
#else
      cv->ComID = _lopen(ts->HostName,0);
      InvalidHost = (cv->ComID<=0);
#endif
      if (InvalidHost)
      {
	if (cv->NoMsg==0)
	  MessageBox(cv->HWin,"Cannot open file",ErrorCaption,
	    MB_TASKMODAL | MB_ICONEXCLAMATION);
      }
      else {
	cv->Open = TRUE;
	PostMessage(cv->HWin, WM_USER_COMMOPEN, 0, 0);
      }
      break;
  } /* end of "switch" */

  if (InvalidHost)
  {
    PostMessage(cv->HWin, WM_USER_COMMNOTIFY, 0, FD_CLOSE);
    if ( (ts->PortType==IdTCPIP) && cv->Open )
    {
      if ( cv->s!=INVALID_SOCKET ) Pclosesocket(cv->s);
      FreeWinsock();
    }
    return;
  }
}

#ifdef TERATERM32
void CommThread(void *arg)
{
  DWORD Evt;
  PComVar cv = (PComVar)arg;
  DWORD DErr;
  HANDLE REnd;
  char Temp[20];

  strcpy(Temp,READENDNAME);
  uint2str(cv->ComPort,&Temp[strlen(Temp)],2);	
  REnd = OpenEvent(EVENT_ALL_ACCESS,FALSE, Temp);
  while (TRUE) {
    if (WaitCommEvent(cv->ComID,&Evt,NULL))
    {
      if (! cv->Ready) _endthread();
      if (! cv->RRQ)
	PostMessage(cv->HWin, WM_USER_COMMNOTIFY, 0, FD_READ);
      WaitForSingleObject(REnd,INFINITE);
    }
    else {
      if (! cv->Ready) _endthread();
      ClearCommError(cv->ComID,&DErr,NULL);
    }
  }
}
#endif

void CommStart(PComVar cv, LONG lParam)
{
  char ErrMsg[31];
#ifdef TERATERM32
  char Temp[20];
  char Temp2[3];
#else
  COMSTAT Stat;
#endif

  if (! cv->Open ) return;
  if ( cv->Ready ) return;
  switch (cv->PortType) {
    case IdTCPIP:
      ErrMsg[0] = 0;
      switch (HIWORD(lParam)) {
	case WSAECONNREFUSED:
	  strcpy(ErrMsg,"Connection refused"); break;
	case WSAENETUNREACH:
	  strcpy(ErrMsg,"Network cannot be reached"); break;
	case WSAETIMEDOUT:
	  strcpy(ErrMsg,"Connection timed out"); break;
	default:
	  strcpy(ErrMsg,ErrorCantConn);
      }
      if (HIWORD(lParam)>0)
      {
	if (cv->NoMsg==0)
	  MessageBox(cv->HWin,ErrMsg,ErrorCaption,
	    MB_TASKMODAL | MB_ICONEXCLAMATION);
	PostMessage(cv->HWin, WM_USER_COMMNOTIFY, 0, FD_CLOSE);
	return;
      }
      PWSAAsyncSelect(cv->s,cv->HWin,WM_USER_COMMNOTIFY, FD_READ | FD_OOB | FD_CLOSE);
      TCPIPClosed = FALSE;
      break;

    case IdSerial:
#ifdef TERATERM32
      uint2str(cv->ComPort,Temp2,2);
      strcpy(Temp,READENDNAME);
      strcat(Temp,Temp2);
      ReadEnd = CreateEvent(NULL,FALSE,FALSE,Temp);
      strcpy(Temp,WRITENAME);
      strcat(Temp,Temp2);
      memset(&wol,0,sizeof(OVERLAPPED));
      wol.hEvent = CreateEvent(NULL,TRUE,TRUE,Temp);
      strcpy(Temp,READNAME);
      strcat(Temp,Temp2);
      memset(&rol,0,sizeof(OVERLAPPED));
      rol.hEvent = CreateEvent(NULL,TRUE,FALSE,Temp);

      /* create the receiver thread */
#ifdef __WATCOMC__
      if (_beginthread(CommThread,8192,cv) == -1)
#else
      if (_beginthread(CommThread,0,cv) == -1)
#endif
	MessageBox(cv->HWin,"Can't create thread",ErrorCaption,
		   MB_TASKMODAL | MB_ICONEXCLAMATION);
#else
      // flush input que
      while (GetCommError(cv->ComID, &Stat)!=0) {};
      FlushComm(cv->ComID,1);
      // enable receive request
      SetCommEventMask(cv->ComID,EV_RXCHAR);
      EnableCommNotification(cv->ComID,cv->HWin,-1,-1);
      GetCommEventMask(cv->ComID,EV_RXCHAR);
#endif
      break;
    case IdFile: cv->RRQ = TRUE; break;
  }
  cv->Ready = TRUE;
}

BOOL CommCanClose(PComVar cv)
{ // check if data remains in buffer
  if (! cv->Open) return TRUE;
  if (cv->InBuffCount>0) return FALSE;
  if ((cv->HLogBuf!=NULL) &&
      ((cv->LCount>0) ||
       (cv->DCount>0))) return FALSE;
  if ((cv->HBinBuf!=NULL) &&
      (cv->BCount>0)) return FALSE;
  return TRUE;
}

void CommClose(PComVar cv)
{
  WORD COMFlag;

  if ( ! cv->Open ) return;
  cv->Open = FALSE;

  /* disable event message posting & flush buffer */
  cv->RRQ = FALSE;
  cv->Ready = FALSE;
  cv->InPtr = 0;
  cv->InBuffCount = 0;
  cv->OutPtr = 0;
  cv->OutBuffCount = 0;

  /* close port & release resources */
  switch (cv->PortType) {
    case IdTCPIP:
      if (HAsync!=0)
	PWSACancelAsyncRequest(HAsync);
      HAsync = 0;
      if ( cv->s!=INVALID_SOCKET )
	Pclosesocket(cv->s);
      cv->s = INVALID_SOCKET;
      TTXCloseTCP(); /* TTPLUG */
      FreeWinsock();
      break;
    case IdSerial:
#ifdef TERATERM32
      if ( cv->ComID != INVALID_HANDLE_VALUE )
      {
	CloseHandle(ReadEnd);
	CloseHandle(wol.hEvent);
	CloseHandle(rol.hEvent);
	PurgeComm(cv->ComID,
		  PURGE_TXABORT | PURGE_RXABORT |
		  PURGE_TXCLEAR | PURGE_RXCLEAR);
	EscapeCommFunction(cv->ComID,CLRDTR);
	SetCommMask(cv->ComID,0);
	CloseHandle(cv->ComID);
#else
      if (cv->ComID >= 0)
      {
	FlushComm(cv->ComID,0);
	FlushComm(cv->ComID,1);
	EscapeCommFunction(cv->ComID,CLRDTR);
	EnableCommNotification(cv->ComID,0,-1,-1) ;
	CloseComm(cv->ComID);
#endif
	COMFlag = GetCOMFlag();
	COMFlag = COMFlag & ~(1 << (cv->ComPort-1));
	SetCOMFlag(COMFlag); 
      }
      break;
    case IdFile:
#ifdef TERATERM32
      if (cv->ComID != INVALID_HANDLE_VALUE)
	CloseHandle(cv->ComID);
      break;
  }
  cv->ComID = INVALID_HANDLE_VALUE;
#else
      if (cv->ComID >= 0)
	_lclose(cv->ComID);
      break;
  }
  cv->ComID = -1;
#endif
  cv->PortType = 0;
}

void CommProcRRQ(PComVar cv)
{
#ifndef TERATERM32
  COMSTAT Stat;
#endif

  if ( ! cv->Ready ) return;
  /* disable receive request */
  switch (cv->PortType) {
    case IdTCPIP:
      if (! TCPIPClosed)
	PWSAAsyncSelect(cv->s,cv->HWin,WM_USER_COMMNOTIFY, FD_OOB | FD_CLOSE);
      break;
    case IdSerial:
#ifndef TERATERM32
      EnableCommNotification(cv->ComID,0,-1,-1);
      while (GetCommError(cv->ComID, &Stat)!=0) {};
#endif
      break;
  }
  cv->RRQ = TRUE;
  CommReceive(cv);
}


void CommReceive(PComVar cv)
{
#ifdef TERATERM32
  DWORD C;
  DWORD DErr;
#else
  int C;
  COMSTAT Stat;
#endif

#ifdef MV_CHANGE
  //static     BYTE InBuffTmp[InBuffSize];
  int       rxCharFlag = 0;
  //int  InBuffTmpIdx = 0;
#endif


#ifndef MV_CHANGE
  if (! cv->Ready || ! cv->RRQ ||
      (cv->InBuffCount>=InBuffSize)) return;
#else
  if (! cv->Ready || ! cv->RRQ ) return;
#endif

  /* Compact buffer */
  if ((cv->InBuffCount>0) && (cv->InPtr>0))
  {
    memmove(cv->InBuff,&(cv->InBuff[cv->InPtr]),cv->InBuffCount);
    cv->InPtr = 0;
  }

  if (cv->InBuffCount<InBuffSize)
  {
    switch (cv->PortType) {
      case IdTCPIP:
        C = Precv(cv->s, &(cv->InBuff[cv->InBuffCount]),
                  InBuffSize-cv->InBuffCount, 0);
        if (C == SOCKET_ERROR)
        {
            C = 0;
            PWSAGetLastError();
        }
        else if (C == 0)
        {
            /* nothing for C == 0 */
        }
        else
        {
            /* process data received from tcp, filter it and send it to python  socket 
               vgorine 19.04.2010 */
            int i;
            int ret;
#if 1
			char msg[100], *msg_p;
			strcpy(msg, "mTerm send ["); msg_p = msg+strlen(msg);
            for (i = 0; i < (int)C; i++) {
				BYTE bb = cv->InBuff[i];
				if (i) *msg_p++ = ' ';
				*msg_p++ = BinToPrintableHex((bb>>4)&0xF);
				*msg_p++ = BinToPrintableHex((bb   )&0xF);
				*msg_p   = '\0';
            }
			strcat(msg, "] to TCP client\r\n");
			OutputDebugStringA(msg);
#endif
            for (i = 0; i < (int)C; i++) {
                /* ignore 0 byte */
                if (cv->InBuff[cv->InBuffCount] != 0) {
                    ret = processRxFilter(&(cv->InBuff[cv->InBuffCount]), cv);
                    // If there is a valid Python client socket we send the incoming character to it
                    if (ret == 1) 
                    {
                        if (m_clientSocket != -1)
                        {
                            send (m_clientSocket, &(cv->InBuff[cv->InBuffCount]), 1, 0);
                        }
                    }
                }
                cv->InBuffCount++;
            }
        }
        break;
      case IdSerial:
#ifdef TERATERM32
 #ifdef MV_CHANGE
    do
    {
        ClearCommError(cv->ComID,&DErr,NULL);
        //if (! ReadFile(cv->ComID,&(cv->InBuff[cv->InBuffCount]), InBuffSize-cv->InBuffCount,&C,&rol))
        if (! ReadFile(cv->ComID,&(cv->InBuff[cv->InBuffCount]), 1,&C,&rol))
	    {
	        if (GetLastError() == ERROR_IO_PENDING)
	        {
	            if (WaitForSingleObject(rol.hEvent, 1000) != WAIT_OBJECT_0) C = 0;
	            else GetOverlappedResult(cv->ComID,&rol,&C,FALSE);
	        }
	        else C = 0;
        }
        rxCharFlag = C;

        if (C == 1) C = processRxFilter(&(cv->InBuff[cv->InBuffCount]), cv);

        // If there is a valid Python client socket we send the incoming character to it
        if (C == 1) 
        {
            int ret;
            if (m_clientSocket != -1)
            {
                ret = send (m_clientSocket, &(cv->InBuff[cv->InBuffCount]), 1, 0);
                if (ret != 1)
                {
                }
            }
        }

        cv->InBuffCount = cv->InBuffCount + C;

    } while ((rxCharFlag!=0) && (cv->InBuffCount<InBuffSize));
 
#if 1
	if (cv->InBuffCount > 0)
	{
			char msg[100], *msg_p;
			int i;
			strcpy(msg, "mTerm send ["); msg_p = msg+strlen(msg);
            for (i = 0; i < cv->InBuffCount; i++) {
				BYTE bb = cv->InBuff[i];
				if (i) *msg_p++ = ' ';
				*msg_p++ = BinToPrintableHex((bb>>4)&0xF);
				*msg_p++ = BinToPrintableHex((bb   )&0xF);
				*msg_p   = '\0';
            }
			strcat(msg, "] to serial client\r\n");
			OutputDebugStringA(msg);
	}
#endif

#else
	do {
        ClearCommError(cv->ComID,&DErr,NULL);
	  if (! ReadFile(cv->ComID,&(cv->InBuff[cv->InBuffCount]),
	    InBuffSize-cv->InBuffCount,&C,&rol))
	  {
	    if (GetLastError() == ERROR_IO_PENDING)
	    {
	      if (WaitForSingleObject(rol.hEvent, 1000) != WAIT_OBJECT_0) C = 0;
	      else GetOverlappedResult(cv->ComID,&rol,&C,FALSE);
	    }
	    else C = 0;
	  }
	  cv->InBuffCount = cv->InBuffCount + C;
	} while ((C!=0) && (cv->InBuffCount<InBuffSize));
#endif //#ifdef MV_CHANGE
	ClearCommError(cv->ComID,&DErr,NULL);
#else
	do {
	  C = ReadComm(cv->ComID, &cv->InBuff[cv->InBuffCount], InBuffSize-cv->InBuffCount);
	  C = abs(C);
	  do {} while (GetCommError(cv->ComID, &Stat)!=0);
	  cv->InBuffCount = cv->InBuffCount + C;
	} while ((C!=0) && (cv->InBuffCount<InBuffSize));
#endif
	break;
      case IdFile:
#ifdef TERATERM32
	ReadFile(cv->ComID,&(cv->InBuff[cv->InBuffCount]),
		 InBuffSize-cv->InBuffCount,&C,NULL);
#else
	C = _lread(cv->ComID, &cv->InBuff[cv->InBuffCount],
		   InBuffSize-cv->InBuffCount);
#endif
	cv->InBuffCount = cv->InBuffCount + C;
	break;
    }
  }

  if (cv->InBuffCount==0)
  {
    switch (cv->PortType) {
      case IdTCPIP:
	if (! TCPIPClosed)
	  PWSAAsyncSelect(cv->s,cv->HWin,
	    WM_USER_COMMNOTIFY, FD_READ | FD_OOB | FD_CLOSE);
	break;
      case IdSerial:
#ifdef TERATERM32
	cv->RRQ = FALSE;
	SetEvent(ReadEnd);
	return;
#else
	while (GetCommError(cv->ComID, &Stat)!=0) {};
	EnableCommNotification(cv->ComID,cv->HWin,-1,-1);
	GetCommEventMask(cv->ComID,EV_RXCHAR);
	break;
#endif
      case IdFile:
	PostMessage(cv->HWin, WM_USER_COMMNOTIFY, 0, FD_CLOSE);
	break;
    }
    cv->RRQ = FALSE;

  }
}
#ifdef MV_CHANGE // CommSend is protected against re-entry
void CommSend(PComVar cv)
{
  int delay;
  COMSTAT Stat;
  BYTE LineEnd;
  int C, D, Max;
#ifdef TERATERM32
  DWORD DErr;
#endif

  WaitForSingleObject(m_udpmutex, INFINITE);

  if ((! cv->Open) || (! cv->Ready))
  {
    cv->OutBuffCount = 0;
    ReleaseMutex(m_udpmutex);
    return;
  }

  if ( (cv->OutBuffCount == 0)  || (! cv->CanSend))
  {
      ReleaseMutex(m_udpmutex);
      return;
  }
  /* Max num of bytes to be written */
  switch (cv->PortType) {
    case IdTCPIP:
      if (TCPIPClosed) cv->OutBuffCount = 0;
      Max = cv->OutBuffCount;
      break;
    case IdSerial:
#ifdef TERATERM32
      ClearCommError(cv->ComID,&DErr,&Stat);
#else
      GetCommError(cv->ComID,&Stat);
#endif
      Max = OutBuffSize - Stat.cbOutQue;
      break;
    case IdFile: Max = cv->OutBuffCount;
      break;
  }

  if ( Max<=0 )
  {
      ReleaseMutex(m_udpmutex);
      return;
  }

  if ( Max > cv->OutBuffCount ) Max = cv->OutBuffCount;

  C = Max;
  delay = 0;

  if ( cv->DelayFlag && (cv->PortType==IdSerial) )
  {
    if ( cv->DelayPerLine > 0 )
    {
      if ( cv->CRSend==IdCR ) LineEnd = 0x0d;
			 else LineEnd = 0x0a;
      C = 1;
      if ( cv->DelayPerChar==0 )
	while ((C<Max) && (cv->OutBuff[cv->OutPtr+C-1]!=LineEnd))
	  C++;
      if ( cv->OutBuff[cv->OutPtr+C-1]==LineEnd )
	delay = cv->DelayPerLine;
      else delay = cv->DelayPerChar;
    }
    else if ( cv->DelayPerChar > 0 )
    {
      C = 1;
      delay = cv->DelayPerChar;
    }
  }

  /* Write to comm driver/Winsock */
#if 1
  {
	char msg[100], *msg_p;
	int i;
	strcpy(msg, "mTerm send ["); msg_p = msg+strlen(msg);
	for (i = 0; i < (int)C; i++) {
		BYTE bb = cv->OutBuff[i];
		if (i) *msg_p++ = ' ';
		*msg_p++ = BinToPrintableHex((bb>>4)&0xF);
		*msg_p++ = BinToPrintableHex((bb   )&0xF);
		*msg_p   = '\0';
	}
	strcat(msg, "] to ");
	switch (cv->PortType) {
	case IdTCPIP:	strcat(msg, "TCP"); break;
    case IdSerial:	strcat(msg, "serial"); break;
	default:		strcat(msg, "UNKNOWN"); break;
	}
	strcat(msg, " port\r\n");
	OutputDebugStringA(msg);
  }
#endif
  switch (cv->PortType) {
    case IdTCPIP:
      D = Psend(cv->s, &(cv->OutBuff[cv->OutPtr]), C, 0);
      if ( D==SOCKET_ERROR ) /* if error occurs */
      {
	PWSAGetLastError(); /* Clear error */
	D = 0;
      }
      break;

    case IdSerial:
#ifdef TERATERM32
      if (! WriteFile(cv->ComID,&(cv->OutBuff[cv->OutPtr]),C,(LPDWORD)&D,&wol))
      {
	if (GetLastError() == ERROR_IO_PENDING)
	{
	  if (WaitForSingleObject(wol.hEvent,1000) !=
	      WAIT_OBJECT_0)
	    D = C;	/* Time out, ignore data */
	  else
	    GetOverlappedResult(cv->ComID,&wol,(LPDWORD)&D,FALSE);
	}
	else /* I/O error */
	  D = C; /* ignore error */
      }
      ClearCommError(cv->ComID,&DErr,&Stat);
#else
      D = WriteComm(cv->ComID, &cv->OutBuff[cv->OutPtr], C);
      D = abs(D);
      while (GetCommError(cv->ComID, &Stat)!=0) {};
#endif
      break;
    case IdFile: D = C; break;
  }

  cv->OutBuffCount = cv->OutBuffCount - D;
  if ( cv->OutBuffCount==0 )
    cv->OutPtr = 0;
  else
    cv->OutPtr = cv->OutPtr + D;

  if ( (C==D) && (delay>0) )
  {
    cv->CanSend = FALSE;
    SetTimer(cv->HWin, IdDelayTimer, delay, NULL);
  }
  ReleaseMutex(m_udpmutex);
}
#else // #ifdef MV_CHANGE
void CommSend(PComVar cv)
{
  int delay;
  COMSTAT Stat;
  BYTE LineEnd;
  int C, D, Max;
#ifdef TERATERM32
  DWORD DErr;
#endif

  if ((! cv->Open) || (! cv->Ready))
  {
    cv->OutBuffCount = 0;  
    return;
  }

  if ((cv->OutBuffCount == 0) || (! cv->CanSend)) return;
  /* Max num of bytes to be written */
  switch (cv->PortType) {
    case IdTCPIP:
      if (TCPIPClosed) cv->OutBuffCount = 0;
      Max = cv->OutBuffCount;
      break;
    case IdSerial:
#ifdef TERATERM32
      ClearCommError(cv->ComID,&DErr,&Stat);
#else
      GetCommError(cv->ComID,&Stat);
#endif
      Max = OutBuffSize - Stat.cbOutQue;
      break;
    case IdFile: Max = cv->OutBuffCount;
      break;
  }

  if ( Max<=0 ) return;
  if ( Max > cv->OutBuffCount ) Max = cv->OutBuffCount;

  C = Max;
  delay = 0;

  if ( cv->DelayFlag && (cv->PortType==IdSerial) )
  {
    if ( cv->DelayPerLine > 0 )
    {
      if ( cv->CRSend==IdCR ) LineEnd = 0x0d;
			 else LineEnd = 0x0a;
      C = 1;
      if ( cv->DelayPerChar==0 )
	while ((C<Max) && (cv->OutBuff[cv->OutPtr+C-1]!=LineEnd))
	  C++;
      if ( cv->OutBuff[cv->OutPtr+C-1]==LineEnd )
	delay = cv->DelayPerLine;
      else delay = cv->DelayPerChar;
    }
    else if ( cv->DelayPerChar > 0 )
    {
      C = 1;
      delay = cv->DelayPerChar;
    }
  }

  /* Write to comm driver/Winsock */
  switch (cv->PortType) {
    case IdTCPIP:
      D = Psend(cv->s, &(cv->OutBuff[cv->OutPtr]), C, 0);
      if ( D==SOCKET_ERROR ) /* if error occurs */
      {
	PWSAGetLastError(); /* Clear error */
	D = 0;
      }
      break;

    case IdSerial:
#ifdef TERATERM32
      if (! WriteFile(cv->ComID,&(cv->OutBuff[cv->OutPtr]),C,(LPDWORD)&D,&wol))
      {
	if (GetLastError() == ERROR_IO_PENDING)
	{
	  if (WaitForSingleObject(wol.hEvent,1000) !=
	      WAIT_OBJECT_0)
	    D = C;	/* Time out, ignore data */
	  else
	    GetOverlappedResult(cv->ComID,&wol,(LPDWORD)&D,FALSE);
	}
	else /* I/O error */
	  D = C; /* ignore error */
      }
      ClearCommError(cv->ComID,&DErr,&Stat);
#else
      D = WriteComm(cv->ComID, &cv->OutBuff[cv->OutPtr], C);
      D = abs(D);
      while (GetCommError(cv->ComID, &Stat)!=0) {};
#endif
      break;
    case IdFile: D = C; break;
  }

  cv->OutBuffCount = cv->OutBuffCount - D;
  if ( cv->OutBuffCount==0 )
    cv->OutPtr = 0;
  else
    cv->OutPtr = cv->OutPtr + D;

  if ( (C==D) && (delay>0) )
  {
    cv->CanSend = FALSE;
    SetTimer(cv->HWin, IdDelayTimer, delay, NULL);
  }
}
#endif // MV_CHANGE

void CommSendBreak(PComVar cv)
/* for only serial ports */
{
  MSG DummyMsg;

  if ( ! cv->Ready ) return;

  switch (cv->PortType) {
    case IdSerial:
      /* Set com port into a break state */
      SetCommBreak(cv->ComID);

      /* pause for 1 sec */
      if (SetTimer(cv->HWin, IdBreakTimer, 1000, NULL) != 0)
	GetMessage(&DummyMsg,cv->HWin,WM_TIMER,WM_TIMER);

      /* Set com port into the nonbreak state */
      ClearCommBreak(cv->ComID);
      break;
  }
}

void CommLock(PTTSet ts, PComVar cv, BOOL Lock)
{
  BYTE b;
  DWORD Func;

  if (! cv->Ready) return;
  if ((cv->PortType==IdTCPIP) ||
      (cv->PortType==IdSerial) &&
      (ts->Flow!=IdFlowHard))
  {
    if (Lock)
      b = XOFF;
    else
      b = XON;
    CommBinaryOut(cv,&b,1);
  }
  else if ((cv->PortType==IdSerial) &&
	   (ts->Flow==IdFlowHard))
  {
    if (Lock)
      Func = CLRRTS;
    else
      Func = SETRTS;
    EscapeCommFunction(cv->ComID,Func);
  }
}

BOOL PrnOpen(PCHAR DevName)
{
  char Temp[MAXPATHLEN];
  DCB dcb;
#ifdef TERATERM32
  DWORD DErr;
  COMMTIMEOUTS ctmo;
#else
  COMSTAT Stat;
#endif

  strcpy(Temp,DevName);
  Temp[4] = 0; // COMn or LPTn
  LPTFlag = (Temp[0]=='L') ||
	    (Temp[0]=='l');
#ifdef TERATERM32
  PrnID =
    CreateFile(Temp,GENERIC_WRITE,
	       0,NULL,OPEN_EXISTING,
	       0,NULL);
  if (PrnID == INVALID_HANDLE_VALUE) return FALSE;

  if (GetCommState(PrnID,&dcb))
  {
    BuildCommDCB(DevName,&dcb);
    SetCommState(PrnID,&dcb);
  }
  ClearCommError(PrnID,&DErr,NULL);
  if (! LPTFlag)
    SetupComm(PrnID,0,CommOutQueSize);
  /* flush output buffer */
  PurgeComm(PrnID, PURGE_TXABORT |
    PURGE_TXCLEAR);
  memset(&ctmo,0,sizeof(ctmo));
  ctmo.WriteTotalTimeoutConstant = 1000;
  SetCommTimeouts(PrnID,&ctmo);
#else
  PrnID = OpenComm(Temp,0,CommOutQueSize);
  if (PrnID<0) return FALSE;
  if (GetCommState(PrnID,&dcb)==0)
  {
    BuildCommDCB(DevName,&dcb);
    SetCommState(&dcb);
  }
  GetCommError(PrnID, &Stat);
  /* flush output buffer */
  FlushComm(PrnID,0);
#endif
  if (! LPTFlag)
    EscapeCommFunction(PrnID,SETDTR);
  return TRUE;
}

int PrnWrite(PCHAR b, int c)
{
  int d;
#ifdef TERATERM32
  DWORD DErr;
#endif
  COMSTAT Stat;

#ifdef TERATERM32
  if (PrnID == INVALID_HANDLE_VALUE )
#else
  if (PrnID < 0)
#endif
    return c;

#ifdef TERATERM32
  ClearCommError(PrnID,&DErr,&Stat);
  if (! LPTFlag &&
      (OutBuffSize - (int)Stat.cbOutQue < c))
    c = OutBuffSize - Stat.cbOutQue;
  if (c<=0) return 0;
  if (! WriteFile(PrnID,b,c,(LPDWORD)&d,NULL))
    d = 0;
  ClearCommError(PrnID,&DErr,NULL);
#else
  GetCommError(PrnID,&Stat);
  if (OutBuffSize - Stat.cbOutQue < c)
    c = OutBuffSize - Stat.cbOutQue;
  if (c<=0) return 0;
  d = WriteComm(PrnID, b, c);
  d = abs(d);
  GetCommError(PrnID, &Stat);
#endif
  return d;
}

void PrnCancel()
{
#ifdef TERATERM32
  PurgeComm(PrnID,
    PURGE_TXABORT | PURGE_TXCLEAR);
#else
  FlushComm(PrnID,0);
#endif
  PrnClose();
}

void PrnClose()
{
#ifdef TERATERM32
  if (PrnID != INVALID_HANDLE_VALUE)
  {
    if (!LPTFlag)
      EscapeCommFunction(PrnID,CLRDTR);
    CloseHandle(PrnID);
  }
  PrnID = INVALID_HANDLE_VALUE;
#else
  if (PrnID >= 0)
  {
    if (!LPTFlag)
      EscapeCommFunction(PrnID,CLRDTR);
    CloseComm(PrnID);
  }
  PrnID = -1;
#endif
}

#ifdef MV_CHANGE

void UdpThread(void *arg)
{
    int len;
    int slen=sizeof(m_client_socket_addr);
    char parity;
    int i;


    while(1)
    {
        m_bufrxLen = m_bufrxIdxOut = 0;
#if 0
        len = recvfrom( m_socketID,
                        m_bufrx,
                        sizeof(m_bufrx),
                        0,
                        (struct sockaddr *) &m_client_socket_addr,
                        &slen);
#else
        len = recv(     m_socketID,
                        m_bufrx,
                        sizeof(m_bufrx),
                        0);
#endif
        if (len<0)
        {
            i = WSAGetLastError();

            if (i != WSAECONNRESET)  // tolerate some errors
            {
                ERRORMSG("recvfrom error in UDP thread");
            }
#if 0
            else
            {
              if (bind(m_socketID, (struct sockaddr *) &m_mcdu_socket_addr, sizeof(m_mcdu_socket_addr))==-1)
              {
                    /*printf("could not bind socket after RX error");*/
                    ERRORMSG("Could not create UDP socket");
              }
            }
#endif
            continue;
        }
        /*printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(m_client_socket_addr.sin_addr), ntohs(m_client_socket_addr.sin_port), m_bufrx);*/

        m_bufrxLen = len;
        m_bufrxIdxOut = 0;

        if (len != EXTERN_SIM__FRAME_LEN) {ERRORMSG("bad len"); continue;}
        if ( (m_bufrx[0] != EXTERN_SIM__FRAME_HEADER) && (m_bufrx[0] != EXTERN_SIM__FRAME_HEADER2)) {ERRORMSG("bad header"); continue;}
        parity = 0;
        for (i=0; i<EXTERN_SIM__FRAME_LEN-1; i++) parity ^= m_bufrx[i];
        //if (parity) {ERRORMSG("parity error"); continue;}
		//m_bufrx[EXTERN_SIM__FRAME_LEN-1] = parity; //(im) fixup parity

        //ERRORMSG("Got a frame");
#if 1
OutputDebugStringA("Received frame\r\n");
#endif

        // send frame to serial port
        // Todo: we may need to add a mutex to access CommRawOut and CommSend as they are not thread safe.
        EnterCriticalSection(&CriticalSection);
        CommRawOut(m_cv,m_bufrx,EXTERN_SIM__FRAME_LEN);
        LeaveCriticalSection(&CriticalSection);

        CommSend(m_cv);
    }

}

void CommInitUdp(void)
{

  int slen;
      WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    m_udpmutex = CreateMutex(0, FALSE, 0);
/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = 0x0101; //MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        ERRORMSG("WSAStartup failed");
    }
    else
    {
        memset( m_bufrx, 0, BUF_RX_LEN);
        m_bufrxLen = 0;
	    m_bufrxIdxOut = 0;

	    slen = sizeof(m_client_socket_addr);

        if ((m_socketID=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
            ERRORMSG("socket could not be created");

        memset((char *) &m_mcdu_socket_addr, 0, sizeof(m_mcdu_socket_addr));
        m_mcdu_socket_addr.sin_family = AF_INET;
        m_mcdu_socket_addr.sin_port = htons(LISTEN_PORT_FOR_GETTING_DATA_FROM_MCDUS);
        m_mcdu_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(m_socketID, (struct sockaddr *) &m_mcdu_socket_addr, sizeof(m_mcdu_socket_addr))==-1)
        {
            /*printf("could not bind socket");*/
            ERRORMSG("Could not create UDP socket");
        }
        else
        {
            // lets start the Udp Thread
            if (_beginthread(UdpThread,0,0) == -1)
	            ERRORMSG("Can't create UDP thread");
            else
            {
                InitializeCriticalSection(  &CriticalSection );

            }
        }
    }

    memset((char *) &mcdu1_Addr, 0, sizeof(mcdu1_Addr));
    mcdu1_Addr.sin_family = AF_INET;
    mcdu1_Addr.sin_port = htons(MCDU1_PORT);
    mcdu1_Addr.sin_addr.s_addr = inet_addr( (const char*) m_mcdusIP );

    memset((char *) &mcdu2_Addr, 0, sizeof(mcdu2_Addr));
    mcdu2_Addr.sin_family = AF_INET;
    mcdu2_Addr.sin_port = htons(MCDU2_PORT);
    mcdu2_Addr.sin_addr.s_addr = inet_addr( (const char*) m_mcdusIP );

    memset((char *) &mcdu3_Addr, 0, sizeof(mcdu3_Addr));
    mcdu3_Addr.sin_family = AF_INET;
    mcdu3_Addr.sin_port = htons(MCDU3_PORT);
    mcdu3_Addr.sin_addr.s_addr = inet_addr( (const char*) m_mcdusIP );

    memset((char *) &mcdu4_Addr, 0, sizeof(mcdu4_Addr));
    mcdu4_Addr.sin_family = AF_INET;
    mcdu4_Addr.sin_port = htons(MCDU4_PORT);
    mcdu4_Addr.sin_addr.s_addr = inet_addr( (const char*) m_mcdusIP );

    /* initial clean up for added variables vgorine 19.03.2010 */
    memset((char *) &arincSim_Addr, 0, sizeof(arincSim_Addr));
    arincSim_Addr.sin_family = AF_INET;

    arincSim_Addr.sin_addr.s_addr = inet_addr( (const char*) m_arincSimIP );

    /*printf("Call create thread");*/

}

void CommSetMcdusIP( char * ip )
{
    strncpy(m_mcdusIP, ip, sizeof(m_mcdusIP) );
}


int processRxFilter( BYTE * charPtr, PComVar cv )
{
    static int pendingBytes = 0;
    static int parity = 0;
    static int arincWord = 0;
    static int mcduId = 0;
    static BYTE frameRx[EXTERN_SIM__FRAME_HEADER];
    static int frameRxIdx = 0;

    int rlen;


    // Note: looks stupit to get arincWord and mcduId. However, we are testing this func
    // for embedded target as well.

    char val = *charPtr;
    int retval = 1;


        if (pendingBytes > 0)
        {
            parity ^= val;
            pendingBytes--;
            cv->pendingHDRxBytes = pendingBytes;
            frameRx[frameRxIdx++] = val;

            if (pendingBytes == 5)  // if got a MCDU number
            {
                mcduId = val;
#if 1
OutputDebugStringA("mTerm  Got MCDU#\r\n");
#endif
			}
            else
            {

                if (pendingBytes > 0) /* it is data byte*/
                {
                    arincWord = arincWord << 8;
                    arincWord |= val & 0x000000ff;
#if 1
OutputDebugStringA("mTerm  Got Data byte\r\n");
#endif

                }
            }

            if (pendingBytes == 0)     /* we got parity */
            {
#if 1
OutputDebugStringA("mTerm  Got Parity\r\n");
#endif
#if 0
                ERRORMSG("Got a frame from Serial");
                if (parity & 0x000000ff)
                {
                    ERRORMSG("data rx from mcdu sim with parity error");
                }
                else
#endif
                {
                    // send frame to MCDUs
                    //ERRORMSG("Got a frame from Serial");

 /*                   rlen =  sendto( m_socketID,
                                    (const char*) frameRx,
                                    EXTERN_SIM__FRAME_LEN,
                                    0,
                                    (const struct sockaddr*) &mcdu1_Addr,
                                    sizeof (mcdu1_Addr)); */
                    m_cv->validHsdFlag = 1;

                    /* pass to mcdu messages with message ids not assigned
                       for passing aring messages as it was done before
                       vgorine 19.03.2010 */
                    if ((frameRx[1] < ARINC_SIM_ID_0) ||
                        (frameRx[1] > ARINC_SIM_ID_LAST)) {
#if 1
{
			char msg[100], *msg_p;
			int i;
			strcpy(msg, "mTerm  Send ["); msg_p = msg+strlen(msg);
            for (i = 0; i < EXTERN_SIM__FRAME_LEN; i++) {
				BYTE bb = frameRx[i];
				if (i) *msg_p++ = ' ';
				*msg_p++ = BinToPrintableHex((bb>>4)&0xF);
				*msg_p++ = BinToPrintableHex((bb   )&0xF);
				*msg_p   = '\0';
            }
			strcat(msg, "] to UDP ports 500{1,2,3},501{0,1,2}\r\n");
			OutputDebugStringA(msg);
}
#endif
                        rlen =  sendto( m_socketID,
                                        (const char*) frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*) &mcdu2_Addr,
                                        sizeof (mcdu2_Addr));

                        rlen =  sendto( m_socketID,
                                        (const char*) frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*) &mcdu3_Addr,
                                        sizeof (mcdu3_Addr));

                        rlen =  sendto( m_socketID,
                                        (const char*) frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*) &mcdu4_Addr,
                                        sizeof (mcdu4_Addr));
                        /* pass mcdu messages to "ARINC" ports that they may be received by
                           ARINC interface vgorine 19.05.2010 */
                        arincSim_Addr.sin_port = htons(ARINC_SIM_PORT_0 + 0);
                        rlen =  sendto( m_socketID,
                                        (const char*)frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*)&arincSim_Addr,
                                        sizeof(mcdu4_Addr));
                        arincSim_Addr.sin_port = htons(ARINC_SIM_PORT_0 + 1);
                        rlen =  sendto( m_socketID,
                                        (const char*)frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*)&arincSim_Addr,
                                        sizeof(mcdu4_Addr));
                        arincSim_Addr.sin_port = htons(ARINC_SIM_PORT_0 + 2);
                        rlen =  sendto( m_socketID,
                                        (const char*)frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*)&arincSim_Addr,
                                        sizeof(mcdu4_Addr));
                    } else {
                        /* identify an udp port to send message to and send it
                           vgorine 19.03.2010 */
                        arincSim_Addr.sin_port = htons(ARINC_SIM_PORT_0 +
                                                       (frameRx[1] - ARINC_SIM_ID_0));
                        rlen =  sendto( m_socketID,
                                        (const char*)frameRx,
                                        EXTERN_SIM__FRAME_LEN,
                                        0,
                                        (const struct sockaddr*)&arincSim_Addr,
                                        sizeof(mcdu4_Addr));
						#if 1
						{ /*context*/
						extern sprintf(char * buffer, char * fmt, ...);
						char msg[100], *msg_p;
						int i;
						strcpy(msg, "mTerm  Send ["); msg_p = msg+strlen(msg);
						for (i = 0; i < EXTERN_SIM__FRAME_LEN; i++) {
							BYTE bb = frameRx[i];
							if (i) *msg_p++ = ' ';
							*msg_p++ = BinToPrintableHex((bb>>4)&0xF);
							*msg_p++ = BinToPrintableHex((bb   )&0xF);
							*msg_p   = '\0';
						}
						sprintf(msg+strlen(msg), "] to UDP port 500%u, ", (1+(frameRx[1] - ARINC_SIM_ID_0))/*1,2,3,4*/ );
						if (rlen==SOCKET_ERROR) {
							int Err = PWSAGetLastError();
							sprintf(msg+strlen(msg), "SOCKET_ERROR %d\r\n", Err);
						} else {
							sprintf(msg+strlen(msg), "%d bytes\r\n", rlen);
						}
						OutputDebugStringA(msg);
						} /*context*/
						#endif
					}
                }
            }

            retval = 0;

        }
        else /* we are waiting a header character */
        {
            if ((val == EXTERN_SIM__FRAME_HEADER) || (val == EXTERN_SIM__FRAME_HEADER2))
            {
#if 1
OutputDebugStringA("mTerm  Got frame header\r\n");
#endif
                parity = EXTERN_SIM__FRAME_HEADER;
                pendingBytes = 6;
                cv->pendingHDRxBytes = pendingBytes;
                arincWord = 0;
                frameRxIdx = 1;
                frameRx[0] = EXTERN_SIM__FRAME_HEADER;
                retval = 0;     /* we are eating the characters... so will return empty */
            }
#if 1
else
OutputDebugStringA("mTerm  Got bad frame header, ignored\r\n");
#endif
        }
    return retval;
}

void CommInitTcpServer(void)
{
    // lets start the Udp Thread
    if (_beginthread(TcpServerThread,0,0) == -1)
    {
        ERRORMSG("Can't create TCP Server thread");
        return;
    }
}

static void TcpServerThread(void *arg)
{
    //ERRORMSG("data rx from mcdu sim with parity error");
    //LOG("Rx thread started");
    char * ipClient;
    unsigned long var = 0;

    //int lenRx, len;

    memset( m_tcpBufRx, 0, BUF_TCP_SERVER_RX_LEN);
    m_tcpBufRxLen = m_tcpBufRxIdxOut = 0;

    if ((m_sockfd =socket(AF_INET, SOCK_STREAM, 0))==-1)
        ERRORMSG("tcp socket server could not be created");

#if 1
    ioctlsocket(m_sockfd, FIONBIO, &var);
#endif



    memset( (char *) &m_serv_addr, 0, sizeof(m_serv_addr) );
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons( TCP_SERVER_PORT );

    m_clientLen = sizeof( m_cliAddr );

    if (bind(m_sockfd, (struct sockaddr *) &m_serv_addr,
              sizeof(m_serv_addr)) < 0)
    {
        ERRORMSG("could not bind tcp socket server");
    }

    if (listen(m_sockfd,SOMAXCONN) != 0) {
        ERRORMSG("tcp socket server fail to listen");
    }

    while(1)
    {
#if 1
        m_clientSocket = accept(  m_sockfd,
                                (struct sockaddr *) &m_cliAddr,
                                &m_clientLen);
#else
        m_clientSocket = accept(  m_sockfd,
                                (struct sockaddr *) &m_cliAddr,
                                (socklen_t*) &m_clientLen);
#endif

        // only accept local clients
        if (m_clientSocket > 0)
        {
            ipClient = inet_ntoa(m_cliAddr.sin_addr);

            while(1)
            {
                /************** rx loop *****************/
                memset(m_tcpBufRx, 0, BUF_TCP_SERVER_RX_LEN );
                m_tcpBufRxLen  = recv( m_clientSocket, m_tcpBufRx, BUF_TCP_SERVER_RX_LEN, 0 );
                if (m_tcpBufRxLen < 1)
                {
                    //LOG("python thread read error");
                    m_clientSocket = -1;

                    //CATFAIL(wxT("Trap on rx len < 1"));
                    break;
                }

                //EnterCriticalSection(&CriticalSection);
                if (m_tcpBufRxLen > 0) 
                {
                    CommRawOut(m_cv,m_tcpBufRx,m_tcpBufRxLen);
                    CommSend(m_cv);
                }
                //LeaveCriticalSection(&CriticalSection);



            }

        }
        else
        {
            //LOG("Bad client connection (socket < 0)");
        }


    }

    //LOG("End python thread\n");
/*(im) 14-Nov-2011 always closesocket() after socket() */
//#ifdef __WXMSW__
    closesocket( m_sockfd );
//#else
//    close( m_sockfd );
//#endif
}

#endif