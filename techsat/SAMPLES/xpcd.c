/*
 * @COMPONENT	xpc\iflib\common
 * @FILE	$Source: /CPSoftware/Utilities/mcdusim/techsat/SAMPLES/xpcd.c,v $
 * @DATE	$Date: 2009/11/20 13:39:02 $
 * @REVISION	$Revision: 1.1 $
 * @AUTHOR	TechS.A.T: Dieter Hotz
 * @COPYRIGHT	-----------------------------------------------------
 *              Copyright (c) 1996 -2006 TechS.A.T GmbH
 *		All rights reserved.
 *		-----------------------------------------------------
 *		Technische Systeme fuer Avionik und Test
 *		Gruber Strasse 46c
 *		D-85586 Poing
 *		Tel. +49-8121-703-0
 *		Fax  +49-8121-703-177
 *		-----------------------------------------------------
 * @CONTENT	demonstration program for all XPC products
 *
 *
 */
/*
 * Platform information
 *
 * Software and hardware platforms are selected via defines
 *
 * Software defines
 *
 * DOS		MSDOS6.22 (obsolete)
 * WIN16	Windows/WFW 3.11 (obsolete) 
 * WIN95	Windows 95
 * CAS95	Windows 95/98/NT/2K/XP platform for PCC
 * WIN9x	Windows 95 and Windows 98
 * WIN32	Windows 95/98 and NT/2K/XP
 * LINUX	RedHat LINUX
 *
 * Hardware defines
 *
 * PCC		TechSAT PCMCIA card
 * USB		TechSAT USB card
 * MPC		TechSAT obsolete ISAbus card (No longer supported in this program)
 * EPC		TechSAT ISAbus 4-slot IP carrier with TechSAT IP429
 * PCI2G	TechSAT PCI 2- or 4-slot IP carrier with TechSAT IP429
 * HPVISA	HP 4-slot VxI IP carrier with TechSAT IP429 (NT-only)
 * PC104	Greenspring Flex/104 PC104 2-slot IP carrier with TechSAT IP429
 * IPC01	Microsys 4-slot VME IP carrier with TechSAT IP429
 * PCI40	Greenspring PCI40 carrier with TechSAT IP429
 * PCI60	Greenspring PCI60 carrier with TechSAT IP429
 * IP429	Any carrier using the TechSAT IP429
 */
#if defined(WIN32) || defined (WIN16) || defined (WIN95)
#   if !defined (WIN9x)
#	include <windows.h>
#   endif
#endif

#if ! defined(LINUX)
#include <stddef.h>
#endif
#include <stdio.h>
#if defined(LINUX)
#include <curses.h>
#endif
#include <stdlib.h>
#include <string.h>
#if ! defined(LINUX)
#include <conio.h>
#endif
#include <ctype.h>
#if defined (EPC)
#include "common/a429epc.h"
#elif defined (PC104)
#include "common/pc104.h"
#elif defined (IPC01)
#include "common/ipc01.h"
#elif defined (PCC)
#include "common/a429pcc.h"
#elif defined (USB)
#include "common/a429usb.h"
#elif defined (HPVISA)
#include "common/hpvisa.h"
#elif defined (PCI40) || defined (PCI60)
#include "common/a429pci.h"
#elif defined (PCI2G)
#include "common/a429pci2g.h"
#endif

#if ! defined(LINUX)
#include "genlib/genlib.h"
#endif

static HANDLE	hmpc = (HANDLE)0;
#if defined (WIN32) && !defined (WIN9x)
static short lbn = 0;
#elif defined (PCI40) || defined (PCI60) || defined (PCI2G)
static short lbn = 0;
#elif defined (DOS) || defined (WIN16) || defined (WIN95) || defined (WIN9x)
#   if defined (EPC)
	static short lbn = 0x380;
#   elif defined (PC104)
	static short lbn = 0x320;
#   elif defined (IPC01)
	static short lbn = 0x1000;
#   else
	static short lbn = 0xd000; /* Why was this 0xc000 ??? */
#   endif
#endif

#if defined(LINUX)
/* Replacements for the GENLIB stuff */
static WINDOW *scr;
void video_initialize() {

        scr=initscr();
        box(scr,'*','-');
}

void Locate(short y, short x) {
	        wmove(scr, y-1, x-1);
}

void ShowText(short y, short x, const char *msg) {
	Locate(y, x);
	waddstr(scr, msg);
	wrefresh(scr);
}
#define scanf scanw
#endif


int
frag_taste(short x, short y, const char *lookingfor)
{
    int   ch;

    Locate(y,x);

    for(;;)
    {
#if defined (LINUX)
	ch = getchar();
#else
	ch = _getch();
#endif
	ch = toupper(ch);
	if (strchr(lookingfor,ch))
	    break;
    }
    return(ch);
}

short
ReadShort(short x, short y)
{
    int i;

    Locate(y,x);
    scanf("%d",&i);
    return ((short)i);
}

short
ReadShortMsg(short x, short y, const char *msg)
{
    int i;

    ShowText(y,x,msg);

    Locate(y,(short)(x+strlen(msg)));

    scanf("%d",&i);
    return ((short)i);
}

void
ReadStringMsg(short x, short y, const char *msg, char *s)
{
    ShowText(y,x,msg);

    Locate(y,(short)(x+strlen(msg)));

    scanf("%s", s);
}

unsigned long
ReadULongMsg(short x, short y, const char *msg)
{
    unsigned long i;

    ShowText(y,x,msg);

    Locate(y,(short)(x+strlen(msg)));

    scanf("%lx",&i);
    return (i);
}

unsigned short
ReadUShortMsg(short x, short y, const char *msg)
{
    unsigned int i;

    ShowText(y,x,msg);

    Locate(y,(short)(x+strlen(msg)));

    scanf("%x",&i);
    return ((unsigned short)i);
}
void
WriteErrMsg(const char *msg)
{
    ShowText(24,1,msg);
    ShowText(24,(short)(strlen(msg)+3), "Press a key to continue...");
#if defined (LINUX)
	getchar();
#else
	_getch();
#endif
}

static char rx_number;
static char rx_character;
void
enter_receiver(short col, short row, const char *msg)
{
    char mbuf[80];

    for(;;)
    {
#if defined (EPC) || defined (IPC01) || defined (PCI40) || defined (PCI2G)
	sprintf(mbuf,msg,0xf);
	rx_number = (char)ReadShortMsg(col,row,mbuf);
	if (rx_number < 0 || rx_number > 0xf)
	    continue;
	if (rx_number < 0xa)
	    rx_character = rx_number + '0';
	else
	    rx_character = rx_number + 'W';		/* makes 'a'..'f' */
#elif defined (PC104)
	sprintf(mbuf,msg,7);
	rx_number = (char)ReadShortMsg(col,row,mbuf);
	if (rx_number < 0 || rx_number > 7)
	    continue;
	rx_character = rx_number + '0';
#elif defined (PCI60)
	sprintf(mbuf,msg,23);
	rx_number = (char)ReadShortMsg(col,row,mbuf);
	if (rx_number < 0 || rx_number > 23)
	    continue;
	if (rx_number < 0xa)
	    rx_character = rx_number + '0';
	else
	    rx_character = rx_number + 'W';		/* makes 'a'..'n' */
#else
	sprintf(mbuf,msg,3);
	rx_number = (char)ReadShortMsg(col,row,mbuf);
	if (rx_number < 0 || rx_number > 3)
	    continue;
	rx_character = rx_number + '0';
#endif
	break;
    }
}

static char tr_number;
static char tr_character;
void
enter_transmitter(short col, short row, const char *msg)
{
    char mbuf[80];

    for(;;)
    {
#if defined (EPC) || defined (IPC01) || defined (PCI40) || defined (PCI2G)
	sprintf(mbuf,msg,7);
	tr_number = (char)ReadShortMsg(col,row,mbuf);
	if (tr_number < 0 || tr_number > 7)
	    continue;
    tr_character = tr_number + '0';
#elif defined (PCI60)
	sprintf(mbuf,msg,11);
	tr_number = (char)ReadShortMsg(col,row,mbuf);
	if (tr_number < 0 || tr_number > 11)
	    continue;
	if (tr_number < 0xa)
	    tr_character = tr_number + '0';
	else
	    tr_character = tr_number + 'W';		/* makes 'a'..'b' */
#elif defined (PC104)
	sprintf(mbuf,msg,3);
	tr_number = (char)ReadShortMsg(col,row,mbuf);
	if (tr_number < 0 || tr_number > 3)
	    continue;
	tr_character = tr_number + '0';
#else
	sprintf(mbuf,msg,1);
	tr_number = (char)ReadShortMsg(col,row,mbuf);
	if (tr_number < 0 || tr_number > 1)
	    continue;
	tr_character = tr_number + '0';
#endif
	break;
    }
}

short enter_ipnumber(short col, short row, const char *msg)
{
#if defined (PCC) || defined (HPVISA) || defined (USB)
	return 0;
#else
    char mbuf[80];
    short ipnum;

    for(;;)
    {
#if defined (EPC) || defined (IPC01) || defined (PCI40) || defined (PCI2G)
	sprintf(mbuf,msg,3);
	ipnum = (char)ReadShortMsg(col,row,mbuf);
	if (ipnum < 0 || ipnum > 3)
	    continue;
    return ipnum;
#elif defined (PC104)
	sprintf(mbuf,msg,1);
	ipnum = (char)ReadShortMsg(col,row,mbuf);
	if (ipnum < 0 || ipnum > 1)
	    continue;
    return ipnum;
#elif defined (PCI60)
	sprintf(mbuf,msg,5);
	ipnum = (char)ReadShortMsg(col,row,mbuf);
	if (ipnum < 0 || ipnum > 5)
	    continue;
    return ipnum;
#endif
	break;
    }
#endif
}

void
show_rc(int rc)
{
    char msg[255];
    sprintf(msg,"Function returned %d [0x%x] as result",rc,rc);
    WriteErrMsg((const char*)msg);
}

void
errexit(unsigned long err, char *msg)
{
    char m[255];
    sprintf(m,"ERROR (%d - 0x%x) occured in %s\n",err,err,msg);
    WriteErrMsg((const char *)m);
    if (hmpc)
	_close_xpc(hmpc);
#if defined(LINUX)
	endwin();
#endif
    exit(1);
}


void
print_rx_mode(short y, char rx_num,  unsigned short rstat)
{
    char msg[255];

    if (rstat & 1)
	sprintf(msg,"Receiver %d operates in trigger mode", rx_num);
    else
	sprintf(msg,"Receiver %d operates in non-trigger mode", rx_num);

    ShowText(y,1,(const char*)msg);

    if (rstat & 2)
	sprintf(msg,"Receiver %d operates in storage mode", rx_num);
    else
	sprintf(msg,"Receiver %d operates in non-storage mode", rx_num);

    ShowText((short)(y+1),1,(const char*)msg);

    if (rstat & 4)
	sprintf(msg,"Receiver %d operates in masked mode", rx_num);
    else
	sprintf(msg,"Receiver %d operates in non-masked mode", rx_num);

    ShowText((short)(y+2),1,(const char*)msg);
}

void
drawscreen()
{
    char  str[255];

#if defined(LINUX)
    clear();
#else
    ClearScreen();
#endif
    sprintf(str, 
	  "xPCD V%s Demo for ARINC 429 boards",
	  VERSION_STRING);
    ShowText(1,1,str);
#if defined (HPVISA)
    sprintf(str,"Operating with board at VXI addr: %x(hex)",lbn);
#elif defined (PCI40) || defined (PCI60) || defined (PCI2G)
    sprintf(str,"Operating with board number: %d",lbn);
#elif defined (WIN32) && !defined (WIN9x)
    sprintf(str,"Operating with board number: %d",lbn);
#else
    sprintf(str,"Operating with board at base address: 0x%04x",hmpc);
#endif
    ShowText(2,1,(const char*)str);
    ShowText(25,1,"Copyright (c) Tech S.A.T GmbH 1996-2000");
}

void
initialize()
{
    int rc;
#if defined (WIN95) && defined (IPC01)
    unsigned long b16;
    unsigned long b32;
    long ips;

    drawscreen();
    b16 = ReadULongMsg(1,4,"Enter 16-bit Base address.......");
    b32 = ReadULongMsg(1,5,"Enter 32-bit Base address.......");
    ips = (long)ReadULongMsg(1,6,"Enter IP memory window..........");
    lbn = (USHORT)b16;
    hmpc = _open_xpc(b16,b32,ips,0,0);
#elif defined (HPVISA)
    unsigned short vxibus;
    unsigned short vxiaddr;

    drawscreen();
    vxibus =  ReadUShortMsg(1,4,"Enter VXI Bus Number.(hex).");
    vxiaddr = ReadUShortMsg(1,5,"Enter VXI Address....(hex).");
    hmpc = _open_xpc(0, vxibus, vxiaddr);
    lbn = vxiaddr;
#elif defined (CAS95)
    drawscreen();
    ShowText(5,2,"Board 0 = automatic slot search");
    ShowText(6,2,"Board n > 0 = slot n");
    lbn =  ReadUShortMsg(1,4,"Enter board number (0..4):");
    hmpc = _open_xpc(lbn);
    if (hmpc == INVALID_HANDLE_VALUE)
    {
	printf("Unable to open device (error %d) - abort demo", _xpc_get_lasterror() );
	exit(2);
    }
#else
    hmpc = _open_xpc(lbn);
#endif
#if defined (WIN32) && !defined (WIN9x)
    if (hmpc == INVALID_HANDLE_VALUE)
    {
	printf("Unable to open device - abort demo");
	exit(2);
    }
#endif
#if defined (LINUX)
    if (hmpc == INVALID_HANDLE_VALUE)
    {
	printf("Unable to open device - abort demo");
	endwin();
	exit(2);
    }
#endif
    if ( ( rc = _verify_iflib_version( VERSION_NUMBER) ) != XPC_SUCCESS )
    {
	printf("Verify iflib version failed - abort demo");
#if defined (LINUX)
	endwin();
#endif
	exit(3);
    }
}

void
ClrEOL(short x, short y)
{
    char c[80];
    memset(c,0,80);
    memset(c,' ',80-x);
    ShowText(y,x,c);
}


int
runmenu(const char *title, const char *items[], const char *expected)
{
    int i;

    drawscreen();
    ShowText(1,(short)(80 - strlen(title) - 1),title);
    for(i = 0;  items[i]; i++)
	ShowText((short)(i + 4),19,items[i]);
    ShowText((short)(i + 4),19,"Select function...");
    return(frag_taste(37,(short)(i + 4),expected));
}

void
switch_board()
{
#if defined (WIN95) && defined (IPC01)
    unsigned long b16;
    unsigned long b32;
    long ips;

    drawscreen();
    b16 = ReadULongMsg(1,4,"Enter 16-bit Base address.......");
    b32 = ReadULongMsg(1,5,"Enter 32-bit Base address.......");
    ips = (long)ReadULongMsg(1,6,"Enter IP memory window..........");
    lbn = (USHORT)b16;
    if (hmpc)
    {
	_close_xpc(hmpc);
	hmpc = (HANDLE)0;
    }
    hmpc = _open_xpc(b16,b32,ips,0,0);
#elif defined (HPVISA)
    unsigned short vxibus;
    unsigned short vxiaddr;

    drawscreen();
    vxibus =  ReadUShortMsg(1,4,"Enter VXI Bus Number.(hex).");
    vxiaddr = ReadUShortMsg(1,5,"Enter VXI Address....(hex).");
    hmpc = _open_xpc(0, vxibus, vxiaddr);
    lbn = vxiaddr;
#elif defined (CAS95)
    if (hmpc)
    {
	_close_xpc(hmpc);
	hmpc = (HANDLE)0;
    }
    drawscreen();
    ShowText(5,2,"Board 0 = automatic slot search");
    ShowText(6,2,"Board n > 0 = slot n");
    lbn =  ReadUShortMsg(1,4,"Enter board number (0..4):");
    hmpc = _open_xpc(lbn);
    if (hmpc == INVALID_HANDLE_VALUE)
    {
	printf("Unable to open device (error %d) - abort demo", _xpc_get_lasterror() );
	exit(2);
    }
#else
#   if defined (WIN32) && !defined (WIN9x)
	const char *msg = {"Enter board number (0..9): "};
#   else
	const char *msg = {"Enter base address of board: "};
#   endif
	if (hmpc)
	{
	    _close_xpc(hmpc);
	    hmpc = (HANDLE)0;
	}
    drawscreen();
#   if defined (WIN32) && !defined (WIN9x)
	lbn = ReadShortMsg(1,4,msg);
#   else
	lbn = ReadUShortMsg(1,4,msg);
#   endif

    hmpc = _open_xpc(lbn);
#endif
#if defined (WIN32) && !defined (WIN9x)
	/* Now check if open worked */
	if (hmpc == INVALID_HANDLE_VALUE)
	{
	    WriteErrMsg("Unable to open that device");
	    hmpc = (HANDLE)0;
	}
#endif
}

void
WriteResult(short x, short y, const char *str, unsigned short res)
{
    char msg[255];

    sprintf(msg,"%s%s",str,(res) ? "*** failed ***" : "o.k.");
    ShowText(y,x,(const char*)msg);
}

void
WriteHexRes(short x, short y, const char *str, unsigned short res)
{
    char msg[255];

    sprintf(msg,"%s0x%04x",str,res);
    ShowText(y,x,(const char*)msg);
}

void
reset_st()
{
    int rc;
    short ipn;
    unsigned short selftest_result, dummy;

    drawscreen();
    ShowText(2,62,"Reset and Selftest");
    ipn = enter_ipnumber(1,3,"Enter IP module number [0..%d]: ");
    if ((rc = _reset_and_selftest(hmpc,ipn,&selftest_result,&dummy)) == 0)
    {
	ShowText(4,1,"Selftest-Result:");
	WriteResult(1,6,"DPRAM check............................",
		(unsigned short)(selftest_result & 0x0002));
	WriteResult(1,7,"SRAM check.............................",
		(unsigned short)(selftest_result & 0x0004));
	WriteResult(1,8,"Transceiver A (Tx0,Rx0,Rx1)............",
		(unsigned short)(selftest_result & 0x0008));
	WriteResult(1,9,"Transceiver A (Tx1,Rx2,Rx3)............",
		(unsigned short)(selftest_result & 0x0010));
	WriteResult(1,10,"External loop test.....................",
		(unsigned short)(selftest_result & 0x0020));
#if defined (PCC)
	WriteResult(1,11,"Adapter box connection.................",
		(unsigned short)(selftest_result & 0x0040));
#endif
	WriteResult(1,12,"1. external loop analysis T0-R0.......",
		(unsigned short)(selftest_result & 0x0100));
	WriteResult(1,13,"2. external loop analysis T1-R1.......",
		(unsigned short)(selftest_result & 0x0200));
	WriteResult(1,14,"3. external loop analysis T0-R2.......",
		(unsigned short)(selftest_result & 0x0400));
	WriteResult(1,15,"4. external loop analysis T1-R3.......",
		(unsigned short)(selftest_result & 0x0800));
    }
    show_rc(rc);
}

void
act_tra()
{
    short act_cnt;
    int i;
    tx_activation_t at[256];

    drawscreen();
    ShowText(1,59,"Activate Transmission");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .....");
    act_cnt= ReadShortMsg(1,5,"Enter number of activations [1..256].");
    for(i = 0; i < act_cnt; i++)
    {
	char msg[255];
	sprintf(msg,"%d. of %d activation(s)",i+1,act_cnt);
	ShowText(7,1,(const char*)msg);
	ClrEOL(1,8);
	at[i].transmission_ID = (unsigned char)
		ReadShortMsg(1,8,"Enter transmission ID [0..255].............");
	ClrEOL(1,9);
	at[i].trigger_ID = (unsigned char)
		ReadShortMsg(1,9,"Enter trigger ID      [0..255].............");
    }
    i = _activate_trans(hmpc,tr_character,act_cnt, &at[0]);
    show_rc(i);
}

void
def_tra()
{
    short cnt;
    unsigned short result, timeslice;
    int i;
    tx_definition_t dt[256];

    drawscreen();
    ShowText(1,60,"Define transmissions");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .....");
    cnt    = ReadShortMsg(1,5,"Enter number of definitions [1..256].");
    for(i = 0; i < cnt; i++)
    {
	char msg[255];
	sprintf(msg,"%d. of %d definition(s)",i+1,cnt);
	ShowText(7,1,(const char*)msg);
	ClrEOL(1,8);
	dt[i].transmission_ID = 
		ReadShortMsg(1,8,"Enter transmission ID [0..255].............");
	ClrEOL(1,9);
	dt[i].data_low = 
	       ReadUShortMsg(1,9,"Enter ARINC-429 low-word [4 hex digits]....");
	ClrEOL(1,10);
	dt[i].data_high = 
	      ReadUShortMsg(1,10,"Enter ARINC-429 high-word [4 hex digits]...");
	ClrEOL(1,11);
	dt[i].transmit_interval = 
	       ReadShortMsg(1,11,"Enter transmit interval [in ms]............");
    }
    i = _define_trans(hmpc,tr_character,cnt, &dt[0], &result, &timeslice);
    if (i == 0)
    {
	switch(result)
	{
	    case 0:
		ShowText(14,1,"timeslice fits all transmit definitions");
		break;
	    case 1:
		ShowText(14,1,
      "definition accepted, but too many definitions for calculated timeslice");
		break;
	    case 2:
		ShowText(14,1,"resulting definition count is zero");
		break;
	    default:
	    {
		char msg[255];
		sprintf(msg,"Result = %d",result);
		ShowText(14,1,(const char*)msg);
		break;
	    }
	}
	{
	    char msg[255];
	    sprintf(msg,"The timeslice for transmitter %d is %d milliseconds",
		    tr_number, timeslice);
	    ShowText(15,1,(const char*)msg);
	}
    }
    show_rc(i);
}
void 
del_tra()
{
    short cnt;
    unsigned short result, timeslice;
    int i;
    tx_delete_t dt[256];

    drawscreen();
    ShowText(1,60,"Delete transmissions");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] ...");
    cnt    =       ReadShortMsg(1,5,"Enter number of deletions [1..256].");

    for(i = 0; i < cnt; i++)
    {
	char msg[255];
	sprintf(msg,"%d. of %d deletion(s)",i+1,cnt);
	ShowText(7,1,(const char*)msg);
	ClrEOL(1,8);
	dt[i].transmission_ID = 
		ReadShortMsg(1,8,"Enter transmission ID [0..255].............");
    }
    i = _delete_trans(hmpc,tr_character,cnt, &dt[0], &result, &timeslice);
    if (i == 0)
    {
	switch(result)
	{
	    case 0:
		ShowText(14,1,"timeslice fits all transmit definitions");
		break;
	    case 1:
		ShowText(14,1,
      "definition accepted, but too many definitions for calculated timeslice");
		break;
	    case 2:
		ShowText(14,1,"resulting definition count is zero");
		break;
	    default:
	    {
		char msg[255];
		sprintf(msg,"Result = %d",result);
		ShowText(14,1,(const char*)msg);
		break;
	    }
	}
	{
	    char msg[255];
	    sprintf(msg,"The timeslice for transmitter %d is %d milliseconds",
		    tr_number, timeslice);
	    ShowText(15,1,(const char*)msg);
	}
    }
    show_rc(i);
}

void
get_tra()
{
    read_tx_definition_t td[256];
    unsigned short res, ndef;
    int rc;


    drawscreen();
    ShowText(1,53,"Get transmission definition");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] ...");

    if ((rc = _get_tx_definitions(hmpc,tr_character,&td[0],&res,&ndef))
				   == 0)
    {
	if (res == 1)
	{
	    char msg[255];
	    sprintf(msg,"no definitions made for transmitter %d",tr_number);
	    ShowText(14,1,(const char *)msg);
	}
	else
	{
	    short j = 7;
	    unsigned short i;
	    char msg[255];

	    sprintf(msg,"Transmit data definitions for transmitter %d", tr_number);
	    ShowText(6,1,(const char *)msg);
	    for (i = 0; i < ndef; i++, j++)
	    {
		if (j > (80 - 13))
		{
		    WriteErrMsg(" ");
		    for (j = 7; j < (80 - 13); j++)
			ClrEOL(1,j);
		    j = 7;
		}
		sprintf(msg,
"%3d. of %3d>Tx:%3d Tr:%3d Data:0x%04x%04x Upd:%4d ms/%3d ts TxTF:%3d Fl:0x%4x",
			i+1,
			ndef,
			td[i].transmission_ID,
			td[i].trigger_ID,
			td[i].data_high,
			td[i].data_low,
			td[i].transmit_interval_millis,
			td[i].transmit_interval_timesl,
			td[i].ticks_til_transmission,
			td[i].transmission_flags);
		ShowText(j,1,(const char *)msg);
	    }
	}
    }
    show_rc(rc);
}

void
set_tra()
{
    char tr_mod;
    int rc;

    drawscreen();
    ShowText(1,59,"Set transmission mode");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] ............................");
    tr_mod = (char)ReadShortMsg(1,5,
	    "Enter transmit mode [normal burst mode=0 / time optimized=1.");

    if ((rc = _set_tx_mode(hmpc,tr_character,tr_mod)) == 0)
    {
	char msg[255];
	sprintf(msg, "Transmitter %d set in %s mode", tr_number, 
		(tr_mod == 0) ? "normal burst" : "optimized transmit");
	ShowText(6,1,(const char*)msg);
    }
    show_rc(rc);
}
void
get_s_tra()
{
    unsigned short tr_id;
    read_tx_definition_t td[1];
    unsigned short res;
    int rc;

    drawscreen();
    ShowText(1,46,"Get single transmission definition");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .....");
    tr_id  =       ReadShortMsg(1,5,"Enter transmission ID [0..255].......");

    if ((rc = 
	 _get_single_tx_definition(hmpc,tr_character,tr_id,&td[0],&res))
	 == 0)
    {
	if (res == 1)
	{
	    char msg[255];
	    sprintf(msg,
		"no definitions made for transmitter %d on transmission ID %d",
		tr_number, tr_id);
	    ShowText(14,1,(const char *)msg);
	}
	else
	{
	    char msg[255];

	    sprintf(msg,"Transmit data definitions for transmitter %d", tr_number);
	    ShowText(6,1,(const char *)msg);
	    sprintf(msg,
"->TxID:%3d TrID:%3d Data:0x%04x%04x Upd:%4d ms/%3d ts TxTF:%3d Fl:0x%4x",
		    td[0].transmission_ID,
		    td[0].trigger_ID,
		    td[0].data_high,
		    td[0].data_low,
		    td[0].transmit_interval_millis,
		    td[0].transmit_interval_timesl,
		    td[0].ticks_til_transmission,
		    td[0].transmission_flags);
	    ShowText(7,1,(const char *)msg);
	}
    }
  show_rc(rc);
}

void
get_ts()
{
    unsigned short timeslice;
    int rc;

    drawscreen();
    ShowText(1,67,"Get timeslice");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .....");

    if ((rc = _get_tx_timeslice(hmpc,tr_character,&timeslice)) == 0)
    {
	char msg[255];

	sprintf(msg,"The timeslice for transmitter %d is %d milliseconds",
		tr_number, timeslice);
	ShowText(14,1,(const char *)msg);
    }
    show_rc(rc);
}

void
upd_tra()
{
    unsigned short nupd;
    tx_update_t ut[256];
    unsigned short i;

    drawscreen();
    ShowText(1,60,"Update transmission");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .");
    nupd   =       ReadShortMsg(1,5,"Enter number of updates [1..256].");

    for (i = 0; i < nupd; i++)
    {
	char msg[255];

	sprintf(msg,"%d. of %d update(s)",i+1,nupd);
	ShowText(7,1,(const char*)msg);
	ClrEOL(1,8);
	ut[i].transmission_ID = 
		ReadShortMsg(1,8,"Enter transmission ID [0..255].............");
	ClrEOL(1,9);
	ut[i].data_low = 
	       ReadUShortMsg(1,9,"Enter ARINC-429 low-word [4 hex digits]....");
	ClrEOL(1,10);
	ut[i].data_high = 
	      ReadUShortMsg(1,10,"Enter ARINC-429 high-word [4 hex digits]...");
    }
    i = _update_trans(hmpc,tr_character,nupd,&ut[0]);
    show_rc(i);
}

void
get_tra_stat()
{
    tx_status_t txstat;
    int rc;

    drawscreen();
    ShowText(1,61,"Get transmit status");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .");

    if ((rc = _get_tx_status(hmpc,tr_character,&txstat)) == 0)
    {
	char msg[255];

	sprintf(msg,"Transmitter mode...................: 0x%X", txstat.mode);
	ShowText( 6,1,(const char *)msg);
	sprintf(msg,"Transmitter error..................: 0x%X", txstat.error);
	ShowText( 7,1,(const char *)msg);
	sprintf(msg,"Lost data count....................: %lu", txstat.lostct);
	ShowText( 8,1,(const char *)msg);
	sprintf(msg,"First ARINC word lost..............: 0x%08lX", txstat.lostdata);
	ShowText( 9,1,(const char *)msg);
	sprintf(msg,"Words immediately transmitted count: %lX", txstat.immediatect);
	ShowText(10,1,(const char *)msg);
	sprintf(msg,"Defered transmissions count........: %lX", txstat.deferedct);
	ShowText(11,1,(const char *)msg);
    }
    show_rc(rc);
}

void
act_rec()
{
    unsigned short acode;
    unsigned short rstat;
    rx_mode_t rxm;
    int rc;

    drawscreen();
    ShowText(1,60,"Activate receiver");
    enter_receiver(1,4,"Enter receiver number [0..%d].....");
    ShowText(5,1,"Select the receiver mode:");
    ShowText(6,1,"- stop receiving.............[0]");
    ShowText(7,1,"- start unmasked receiving...[1]");
    ShowText(8,1,"- start masked receiving.....[2]");
    acode = ReadShortMsg(1,9,"- continue receiving.........[3] ");

    if (acode == 2)
    {
	rxm.mask_low = 
	    ReadUShortMsg(1,11,"lower 16-Bit of \"mask\" [4 hex-digits]...");
	rxm.mask_high = 
	    ReadUShortMsg(1,12,"upper 16-Bit of \"mask\" [4 hex-digits]...");
	rxm.match_low = 
	    ReadUShortMsg(1,13,"lower 16-Bit of \"match\" [4 hex-digits]..");
	rxm.match_high = 
	    ReadUShortMsg(1,14,"upper 16-Bit of \"match\" [4 hex-digits]..");
    }

    if ((rc = _activate_rec(hmpc,rx_character,acode, &rxm,&rstat)) == 0)
	print_rx_mode(16,rx_number, rstat);
    show_rc(rc);
}

void
grec_dr()
{
    int rc;
    char msg[80];

    drawscreen();
    ShowText(1,57,"Get receiver data ready");


    enter_receiver(1,4,"Enter receiver number [0..%d]...: ");
    rc = _get_data_ready(hmpc,rx_character);
    if (rc >= 0)
    {
		if (rc == 1)
		    sprintf(msg,"Rx %d: Data ready flag (buffer full) for Rx is set",
	    	     rx_number);
		else
	    	sprintf(msg,
	    	     "Rx %d: Data ready flag (buffer full) for Rx is not set",
	    	     rx_number);
		ShowText(4,1,msg);
    }
    show_rc(rc);
}


void
grec_de()
{
    int rc;

    drawscreen();
    ShowText(1,56,"Get receiver data exist");

    {
    char msg[80];

    enter_receiver(1,4,"Enter receiver number [0..%d]...: ");
    rc = _get_receiver_data_exist(hmpc,rx_character);
    if (rc >= 0)
    {
	if (rc == 1)
	    sprintf(msg,"Rx %d: Data exist flag for Rx is set",
	    	     rx_number);
	else
	    sprintf(msg, "Rx %d: Data exist flag for Rx is not set",
	    	     rx_number);
	ShowText(4,1,msg);
    }
    show_rc(rc);
    }
}
void
grec_dl()
{
    int rc;
    char msg[80];

    drawscreen();
    ShowText(1,57,"Get receiver data lost");


    enter_receiver(1,4,"Enter receiver number [0..%d]...: ");
    rc = _get_receiver_data_lost(hmpc,rx_character);
    if (rc >= 0)
    {
	if (rc == 1)
	    sprintf(msg,"Rx %d: Data lost flag (buffer overrun) for Rx is set",
	    	     rx_number);
	else
	    sprintf(msg, "Rx %d: Data lost flag (buffer overrun) for Rx is not set",
	    	     rx_number);
	ShowText(4,1,msg);
    }
    show_rc(rc);
}

void
rstrec_dl()
{
    int rc;

    drawscreen();
    ShowText(1,57,"Reset receiver data lost");

    enter_receiver(1,4,"Enter receiver number [0..%d]...: ");
    rc = _reset_data_lost(hmpc,rx_character);
    show_rc(rc);
}

void
grec_mo()
{
    unsigned short rstat;
    int rc;

    drawscreen();
    ShowText(1,61,"Get receiver mode");
    enter_receiver(1,4,"Enter receiver number [0..%d].....");

    if ((rc = _get_receiver_mode(hmpc,rx_character,&rstat)) == 0)
	print_rx_mode(5,rx_number,rstat);
    show_rc(rc);
}

void
grec_bu()
{
    unsigned short nbuf;
    unsigned short bsize;
    int rc;

    drawscreen();
    ShowText(1,61,"Get receiver buffer count");
    enter_receiver(1,4,"Enter receiver number [0..%d].....");
    if ((rc = _get_rec_buf_count(hmpc,rx_character,&nbuf,&bsize)) == 0)
    {
	char msg[80];
	sprintf(msg,"Rx %d has %d buffers of %d items each",
		rx_number,nbuf,bsize);
	ShowText(5,1,msg);
    }
    show_rc(rc);
}

void
rea_rxb()
{
    unsigned short nentries;
    unsigned short res;
    rx_data_t rd[512];
    int rc;
    
    drawscreen();
    ShowText(1,59,"Read receiver buffer");
    enter_receiver(1,4,"Enter receiver number [0..%d].....");
    
    if ((rc = _read_receiver_buffer(hmpc,rx_character,&rd[0],&res,
				    &nentries)) == 0)
    {
	char msg[255];

	if (res == 1)
	{
	    sprintf(msg,"Receiver %d has no data available", rx_number);
	    ShowText(6,1,(const char*)msg);
	}
	else
	{
	    short j = 7;
	    unsigned short i;

	    for (i = 0; i < nentries; i++, j++)
	    {
		if (j > (25 - 3))
		{
		    WriteErrMsg(" ");
		    for (j = 7; j <= (25 - 3); j++)
			ClrEOL(1,j);
		    j = 7;
		}
		sprintf(msg,
	    "%3d. entry of %3d on Rx%d data: 0x%04x%04x received at %11lu [ms]",
			i+1, nentries, rx_number, rd[i].arinc_high,
			rd[i].arinc_low,rd[i].receive_time);
		ShowText(j,1,(const char*)msg);
	    }
	}
    }
    show_rc(rc);
}

void
set_rb()
{
    unsigned short bsize, rxstat, res;
    int rc;
    
    drawscreen();
    ShowText(1,56,"Set receiver buffer size");
    enter_receiver(1, 9,"Enter receiver number [0..%d].....");
    bsize  =       ReadShortMsg(1,10,"Enter buffer size [0..252].......");
    if ((rc = _set_rec_buf_size(hmpc,rx_character,bsize,&res,&rxstat)) 
	 == 0)
    {
	char msg[255];

	if (res == 0)
	    sprintf(msg,
		    "Receiver buffer size for receiver %d is set to %d items",
		    rx_number,bsize);
	else
	    sprintf(msg,
      "Selected buffer size outside limits, threshold value remains unchanged");

	ShowText(12,1,(const char*)msg);
	print_rx_mode(13,rx_number,rxstat);
    }
    show_rc(rc);
}

void
set_tsres()
{
    short 	r0,r1,r2,r3;
    short	slot;
    int 	rc;
    
    drawscreen();
    ShowText(1,59,"Set timestamp resolution");

    slot = enter_ipnumber(1,4,"Enter IP module number .......[0..%d] ");
    r0   = ReadShortMsg(1,5,"Enter mode for RX 0 ..........[0..1] ");
    r1   = ReadShortMsg(1,6,"Enter mode for RX 1 ..........[0..1] ");
    r2   = ReadShortMsg(1,7,"Enter mode for RX 2 ..........[0..1] ");
    r3   = ReadShortMsg(1,8,"Enter mode for RX 3 ..........[0..1] ");

    rc = _set_timestamp_res(hmpc,slot,(unsigned char)r0, (unsigned char)r1,
				      (unsigned char)r2, (unsigned char)r3);

    show_rc(rc);
}

void
get_tsres()
{
    unsigned char 	r0,r1,r2,r3;
    short		slot;
    int 		rc;
    
    drawscreen();
    ShowText(1,59,"Get timestamp resolution");

    slot = enter_ipnumber(1,4,"Enter IP module number .......[0..%d] ");

    rc = _get_timestamp_res(hmpc,slot, &r0, &r1, &r2, &r3);

    if (rc == XPC_SUCCESS)
    {
	ShowText(6,1,"Timestamp resolution for RX 0 is ");
	ShowText(6,34,(r0) ? "100 usec" : "1 msec");
	ShowText(7,1,"Timestamp resolution for RX 1 is ");
	ShowText(7,34,(r1) ? "100 usec" : "1 msec");
	ShowText(8,1,"Timestamp resolution for RX 2 is ");
	ShowText(8,34,(r2) ? "100 usec" : "1 msec");
	ShowText(9,1,"Timestamp resolution for RX 3 is ");
	ShowText(9,34,(r3) ? "100 usec" : "1 msec");
    }
    show_rc(rc);
}

void
get_boa()
{
    unsigned short btype;
    int rc;
    short ipn;

    drawscreen();
    ShowText(1,66, "Get board type");
    ipn = enter_ipnumber(1,3,"Enter IP module number [0..%d]: ");
    if ((rc = _get_arinc_board_type(hmpc,ipn,&btype)) == 0)
    {
	char msg[255];
#if defined (WIN32) && !defined (WIN9x)
	sprintf(msg,"The logical board #%d is of type %d",lbn,btype);
#elif defined (HPVISA)
	sprintf(msg,"The board is of type %d",btype);
#else
	sprintf(msg,"The board with base address 0x%04x is of type %d",lbn,btype);
#endif
	ShowText(10,1,(const char *)msg);
    }
    show_rc(rc);
}

#if defined(PCC) || defined (EPC) || defined (PCI2G)
void
est_lop()
{
    unsigned short lcode = 0;
#if defined (PCC)
    unsigned short u;
#endif
    int rc;
    drawscreen();
    ShowText(1,66,"Establish loop");
#if defined (PCC)
    ShowText(4, 1,"Select the desired loop connections by entering a '1'");
    ShowText(5, 1,"below the transmitter/receiver numbers, otherwise enter");
    ShowText(6, 1,"a zero '0' - e.g. 0100 to establish a loop between");
    ShowText(7, 1,"transmitter 0 and receiver 1.");
    ShowText(11,1,"Transmitter : 0011");
    ShowText(12,1,"Receiver    : 0213");
    u = ReadUShortMsg(1,13,"Connect(0/1): ");
    if (u & 0x1000)
	lcode |= 0x0001;
    if (u & 0x0100)
	lcode |= 0x0002;
    if (u & 0x0010)
	lcode |= 0x0004;
    if (u & 0x0001)
	lcode |= 0x0008;
#elif defined (EPC) || defined (PCI2G)
    ShowText(4,1, "Select the desired loop connections by entering the");
    ShowText(5,1, "hexadecimal value for the loop code");
    lcode = ReadUShortMsg(1,7,"LoopCode: ");
#endif
    rc = _establish_loop(hmpc,lcode);
    show_rc(rc);
}
#endif

void
get_fw()
{
    unsigned short vers_no;
    int rc;
    short ipn;

    drawscreen();
    ShowText(1,60,"Get firmware version");
    ipn = enter_ipnumber(1,3,"Enter IP module number ......[0..%d] ");
    if ((rc = _get_fw_version(hmpc,ipn,&vers_no)) == 0)
    {
	char msg[255];

	sprintf(msg,"The firmware version number is %d.%d",
		((vers_no >> 4) & 0xf),(vers_no & 0xf));
	ShowText(10,1,(const char *)msg);
    }
    show_rc(rc);
}

#if defined (ISNOCHNICH)
void
dl_fw()
{
    unsigned short vers_no;
    int rc;
    short ipn;
    char p[128];

    drawscreen();
    ShowText(1,60,"Download firmware");
    ipn = ReadShortMsg(1,3,"Enter IP module number ......[0..1] ");
    ReadStringMsg(1,4,"Enter path to sbn file ............ ", p);
    if ((rc = _download_firmware(hmpc,ipn,p)) == 0)
    {
	char msg[255];
        sprintf(msg,"Firmware downloaded from %s", p);
	ShowText(10,1,(const char *)msg);
    }
    show_rc(rc);
}
#endif


void
get_xtime()
{
    unsigned long ms, ms100;
    int rc;
    short ipn;

    drawscreen();
    ShowText(1,60,"Get elapsed time");
    ipn = enter_ipnumber(1,3,"Enter IP module number ......[0..%d] ");
    if ((rc = _get_ext_ticks(hmpc,ipn,&ms,&ms100)) == 0)
    {
	char msg[255];

	sprintf(msg,"Elapsed time since last reset %lu milliseconds",ms);
	ShowText(10,1,(const char *)msg);
	sprintf(msg,"Elapsed time since last reset %lu milliseconds/10",ms100);
	ShowText(11,1,(const char *)msg);
    }
    show_rc(rc);
}

void
get_time()
{
    unsigned long ms;
    short         ipn;
    int rc;

    drawscreen();
    ShowText(1,60,"Get elapsed time");
    ipn = enter_ipnumber(1,3,"Enter IP module number ......[0..%d] ");
    if ((rc = _get_ticks(hmpc,ipn,&ms)) == 0)
    {
		char msg[255];

		sprintf(msg,"Elapsed time since last reset %lu milliseconds",ms);
		ShowText(10,1,(const char *)msg);
    }
    show_rc(rc);
}

void
set_synchro_mode()
{
    short         ipn;
    unsigned short mode,status;
    int rc;
    drawscreen();
    ShowText(1,60,"Set timestamp synchro mode");
    ipn = enter_ipnumber(1,3,"Enter IP module number ......[0..%d] ");
    mode = ReadUShortMsg(1,4,"Enter mode (0=indep,1=slave,2=master):");
	rc = _set_timestamp_synchro_mode( hmpc, ipn, mode, &status );
    show_rc(rc);
}


void
def_trig()
{
    unsigned short tcount;
    unsigned short rstat;
    trig_definition_t td[8];
    int i, rc;
    
    drawscreen();
    ShowText(1,59,"Define trigger events");
    enter_receiver(1,4,"receiver number [0..%d]...................");
    tcount = ReadShortMsg(1,5,"how many trigger event definitions[0..8].");
    
    if (tcount >0 && tcount < 9)
    {
	for (i = 0; i < (int)tcount; i++)
	{
	    char msg[255];

	    sprintf(msg,"%d of %d definitions:",i,tcount);
	    ShowText(6,1,(const char*) msg);
	    ClrEOL(1,8);
	    td[i].trigger_ID = (unsigned char)ReadShortMsg(1,8,
			"affected trigger ID [1..254]..............");
	    ClrEOL(1,9);
	    ShowText(10,1," - stop on match ......[0]");
	    ShowText(11,1," - start on match .....[1]");
	    ShowText(12,1," - stop on mismatch ...[2]");
	    ShowText(13,1," - start on mismatch ..[3]");
	    td[i].act_code = (unsigned char)ReadShortMsg(1,9,
			"trigger action code [0..3]................");
	    td[i].trigger_mask_low = ReadUShortMsg(1,14,
			"lower 16-Bit of mask-word [4 hex-digits]..");
	    td[i].trigger_mask_high = ReadUShortMsg(1,15,
			"upper 16-Bit of mask-word [4 hex-digits]..");
	    td[i].trigger_match_low = ReadUShortMsg(1,16,
			"lower 16-Bit of match-word [4 hex-digits]..");
	    td[i].trigger_match_high = ReadUShortMsg(1,17,
			"upper 16-Bit of match-word [4 hex-digits]..");
	}
    }
    else 
	tcount = 0;
    if ((rc = _define_trigger(hmpc,rx_character,tcount,&td[0],&rstat)) == 0)
    {
	print_rx_mode(18,rx_number,rstat);
    }
    show_rc(rc);
}

void
get_trig()
{
    unsigned short tid;
    unsigned short tflag;
    int rc;
    short ipm;

    drawscreen();
    ShowText(1,63,"Get trigger state");
    ipm = enter_ipnumber(1,10,"Enter the IP module# [0..%d]................");
    tid = ReadShortMsg(1,10,"The state of which trigger ID [0..255].....");

#if defined(EPC) || defined(PC104) || defined (IPC01) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
    if ((rc = _get_trigger_state(hmpc,ipm,tid,&tflag)) == 0)
#else
    if ((rc = _get_trigger_state(hmpc,tid,&tflag)) == 0)
#endif
    {
	char msg[255];

	if (tflag == 1)
	    sprintf(msg,
	    "Trigger ID %d is set - referring transmission IDs are transmitted",
	    tid);
	else
	    sprintf(msg,
    "Trigger ID %d is cleared - referring transmission IDs are not transmitted",
	    tid);
	ShowText(12,1,(const char*) msg);
    }
    show_rc(rc);
}

void
set_trig()
{
    unsigned short tid;
    unsigned short tflag;
#if  defined(EPC) || defined(PC104) || defined (IPC01) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
    short ipm;
#elif defined(PCC) || defined(USB)
    short oflag;
#endif
    int rc;

    drawscreen();
    ShowText(1,63,"Set trigger state");
#if defined(EPC) || defined (IPC01) || defined (PCI40) || defined (PCI2G)
    ipm   = ReadShortMsg(1,10,"Enter the IP module# [0..3]................");
    tid   = ReadShortMsg(1,11,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,12,"set state ON=[1] or OFF=[0]................");
    rc = _set_trigger_state(hmpc,ipm,tid,tflag);
#elif defined(PCI60)
    ipm   = ReadShortMsg(1,10,"Enter the IP module# [0..5]................");
    tid   = ReadShortMsg(1,11,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,12,"set state ON=[1] or OFF=[0]................");
    rc = _set_trigger_state(hmpc,ipm,tid,tflag);
#elif defined(PC104)
    ipm   = ReadShortMsg(1,10,"Enter the IP module# [0/1].................");
    tid   = ReadShortMsg(1,11,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,12,"set state ON=[1] or OFF=[0]................");
    rc = _set_trigger_state(hmpc,ipm,tid,tflag);
#elif defined (PCC)
    tid   = ReadShortMsg(1,10,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,11,"set state ON=[1] or OFF=[0]................");
    oflag = ReadShortMsg(1,12,"set OUTPCC ON=[1] or OFF=[0]...............");
    rc = _set_trigger_state(hmpc,tid,tflag,oflag);
#elif defined (USB)
    tid   = ReadShortMsg(1,10,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,11,"set state ON=[1] or OFF=[0]................");
    oflag = ReadShortMsg(1,12,"set OUTUSB ON=[1] or OFF=[0]...............");
    rc = _set_trigger_state(hmpc,tid,tflag,oflag);
#else
    tid   = ReadShortMsg(1,10,"The state of which trigger ID [1..254].....");
    tflag = ReadShortMsg(1,11,"set state ON=[1] or OFF=[0]................");
    rc = _set_trigger_state(hmpc,tid,tflag);
#endif
    show_rc(rc);
}

void
get_trset()
{
    int rc;
    tx_set_t txs;

    
    drawscreen();
    ShowText(1,61,"Get transmitter set");
    enter_transmitter(1,4,"Enter transmitter number [0..%d] .....");
    if ((rc = _get_transmitter_set(hmpc,tr_character,&txs)) == 0)
    {
	char msg[255];

	sprintf(msg,"Transmitter %d transmits in", tr_number);
	ShowText(6,1,(const char*) msg);
	if ((txs.bitrate_par_slope_set & 0x0010) == 0) 
	{
	    const char *br[] = { "89.4", "100.0", "98.3", "106.7",
				 "109.2","114.3", "122.9","123.1" };
	    ShowText(7,1,"  o  high-speed");
	    sprintf(msg,"  o  with %s kbps", br[txs.bitrate_par_slope_set & 7]);
	    ShowText(8,1,(const char*)msg);
	}
	if ((txs.bitrate_par_slope_set & 0x0010) == 0x10) 
	{
	    const char *br[] = { "11.18", "12.50", "12.29", "13.34",
				 "13.65", "14.29", "15.36", "15.39" };
	    ShowText(7,1,"  o  low-speed");
	    sprintf(msg,"  o  with %s kbps", br[txs.bitrate_par_slope_set & 7]);
	    ShowText(8,1,(const char*)msg);
	}

	sprintf(msg,"  o  with %s parity", (txs.bitrate_par_slope_set & 8) 
					   ? "even" : "odd");
	ShowText(9, 1,(const char*) msg);
	if ((txs.bitrate_par_slope_set & 0x60) == 0)
	    ShowText(10,1,"  o  with 6.6 us slope");
	if ((txs.bitrate_par_slope_set & 0x60) == 0x20)
	    ShowText(10,1,"  o  with 5.0 us slope");
	if ((txs.bitrate_par_slope_set & 0x60) == 0x40)
	    ShowText(10,1,"  o  with 1.0 us slope");
	if ((txs.bitrate_par_slope_set & 0x60) == 0x60)
	    ShowText(10,1,"  o  with 0.6 us slope");

	sprintf(msg,"  o  %s transmit mode", (txs.bitrate_par_slope_set & 0x80) 
					   ? "optimized" : "non-optimized");
	ShowText(11,1,(const char*) msg);

	sprintf(msg,"  o  +/- %f V Output-Voltage swing",
		(float)(txs.amplitude_set / 10000 * 2));
	ShowText(12,1,(const char *)msg);

    }
    show_rc(rc);
}

void
conf_rec()
{
    char rxpair,rxpch;
    unsigned short rxspeed;
    int rc;

    drawscreen();
    ShowText(1,62,"Configure receiver");
#if defined (PCC) || defined (HPVISA) || defined (USB)
    rxpair = (char)ReadShortMsg(1,4,
				"receiver pair number 0/1=[0] 2/3=[1]......");
    rxpch = rxpair + '0';
#elif defined (EPC) || defined (IPC01) || defined (PCI40) || defined (PCI2G)
    rxpair = (char)ReadShortMsg(1,4,
				"receiver pair number 0..7 ................");
    rxpch = rxpair + '0';
#elif defined (PC104)
    rxpair = (char)ReadShortMsg(1,4,
				"receiver pair number 0..3 ................");
    rxpch = rxpair + '0';
#elif defined (PCI60)
    rxpair = (char)ReadShortMsg(1,4,
				"receiver pair number 0..11................");
    if (rxpair < 0xa)
	rxpch = rxpair + '0';
    else
	rxpch = rxpair + 'W';		/* makes 'a'..'b' */
#endif
    rxspeed=       ReadShortMsg(1,5,
				"reception speed HS=[0], LS=[1]............");
    rc = _config_rec(hmpc,rxpch,rxspeed);
    show_rc(rc);
}

void
conf_tra()
{
    const char *hsp[] = { "89.4", "100.0", "98.3", "106.7",
			  "109.2","114.3", "122.9","123.1" };
    const char *lsp[] = { "11.18", "12.50", "12.29", "13.34",
		 	  "13.65", "14.29", "15.36", "15.39" };
    unsigned short tspeed;
    unsigned short trate;
    unsigned short tparity;
    int            rc;
    
    drawscreen();
    ShowText(1,59,"Configure transmitter");
    enter_transmitter(1,4,"transmitter number [0..%d]............");
    tspeed =       ReadShortMsg(1,5,"transmission speed HS=[0], LS=[1]...");

    if (tspeed == 0 || tspeed == 1)
    {
		char msg[255];
		sprintf(msg,"%-5s kbps.....[%d]",(tspeed == 0) ? hsp[1] : lsp[1],1);
		ShowText((short)(1+6),1,(const char*) msg);
		trate = 1;
    }
    else 
    {
		ShowText(7,1,"Wrong speed selected, set to Low/12.5 kbps");
		tspeed = 1;
		trate  = 1;
    }
    tparity = ReadShortMsg(1,15,
		    "which parity shall be generated ODD=[0], EVEN=[1]....");
    rc = _config_trans(hmpc,tr_character,tspeed,trate,tparity);
    show_rc(rc);
}

void
set_tr_wave()
{
    unsigned short tslope;
    int            rc;
    
    drawscreen();
    ShowText(1,49,"Configure transmitter wave form");
    enter_transmitter(1,4,"transmitter number [0..%d]............");
    tslope =       ReadShortMsg(1,5,"transmission slope 5V/usec=[1], 1V/usec=[2]...");

    rc = _set_trans_wave(hmpc,tr_character,tslope);
    show_rc(rc);
}

#if defined (PCC)
void
set_tr_parity()
{
    char           tr_num;
    unsigned short tparity;
    int            rc;
    
    drawscreen();
    ShowText(1,49,"Configure transmitter parity generation");
    tr_num = (char)ReadShortMsg(1,4,"transmitter number [0/1]......................");
    tparity =      ReadShortMsg(1,5,"transmission parity enable=[0], disable=[1]...");

    rc = _set_parity_mode(hmpc,(char)(tr_num + '0'),tparity);
    show_rc(rc);
}
#endif

void
conf_in()
{
     unsigned short trig_ID,signal_event,trigger_action,timer_action;
     int rc;

    short	slot;

    drawscreen();
    ShowText(1,49,"Configure trigger input options");

    slot = enter_ipnumber(1,4,"Enter IP module number .......[0..%d] ");
    trig_ID = ReadShortMsg(1,5,"Trigger ID [1-254]? ");

    ShowText(6,1,"INPCC signal event?");
    ShowText(7,3,"Disable any action on any signal event   =[0]");
    ShowText(8,3,"Action(s) on falling edge of signal INPCC=[1]");
    signal_event = ReadShortMsg(3,9,"Action(s) on rising edge of signal INPCC =[2] --> ");

    ShowText(11,1,"Trigger action?");
    ShowText(12,3,"No trigger action             = 0");
    ShowText(13,3,"Clear the assigned trigger ID = 1");
    trigger_action = ReadShortMsg(3,14,"Set the assigned trigger ID   = 2 --> ");

    ShowText(16,1,"Timer action?");
    ShowText(17,3,"No timer action             = 0");
    timer_action = ReadShortMsg(3,18,"Reset A429PCC system clock  = 1 --> ");

    rc = _config_input(hmpc,
#if ! defined (PCC) && ! defined(USB)
    			slot,
#endif
			trig_ID,signal_event,
                         trigger_action,timer_action);
    show_rc(rc);
}

void
configure()
{
    const char *items[] = {
	"G - Get transmitter set",
	"R - configure Receiver",
	"T - configure Transmitter",
    "W - set transmitter Wave form",
#if defined (PCC) || defined (USB)
	"P - Set transmitter Parity mode",
#endif
    "I - configure discrete Input",
	"X - return to Main menu",
	0 };

    for (;;)
    {
#if defined (PCC) || defined (USB)
		switch(runmenu("Configure-Menu", items, "GRTWPIX"))
#else
        switch(runmenu("Configure-Menu", items, "GRTWIX"))
#endif
		{
		case 'G':
			get_trset();
			break;
		case 'R':
			conf_rec();
			break;
		case 'T':
			conf_tra();
			break;
		case 'W':
			set_tr_wave();
			break;
#if defined (PCC)
		case 'P':
			set_tr_parity();
			break;
#endif
		case 'I':
			conf_in();
			break;
		case 'X':
			return;
			break;
		}
    }
}

#if defined (EPC)
void
reset_epc()
{
    show_rc(_epc_reset(hmpc));
}
#endif

#if defined (PC104)
void
reset_epc()
{
    show_rc(_pc104_reset(hmpc));
}
#endif

#if defined (IPC01) 
void
reset_epc()
{
    show_rc(_ipc01_reset(hmpc));
}
#endif
#if defined (PCI2G)
void
reset_epc()
{
    show_rc(_pci2g_reset(hmpc));
}
#endif

#if defined (EPC) || defined (IPC01) || defined (PC104) || defined (HPVISA) || defined(PCI40) || defined (PCI60) || defined (PCI2G)
/* Start of Basic IP access routines */
void
read_ipid()
{
    short 	slot;
    int 	rc;
    short 	i,l,c;
    short	bcount;
    unsigned char idstring[256];

    drawscreen();
    ShowText(1,59,"Read IP ID");

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
#if defined (HPVISA)
    rc = _ipm_ipreadid(hmpc,&bcount,idstring);
#else
    rc = _ipm_ipreadid(hmpc,slot,&bcount,idstring);
#endif
    ShowText(6,1,"IP ID:");
    if (bcount == 0)
		ShowText(7,1,"No ID string found");
    else
    {
	for (i = 0, l = 7, c = 1; i < bcount; i++)
	{
	    char msg[255];

	    sprintf(msg, "%c", (isprint(idstring[i])) ? idstring[i] : '.');
	    ShowText(l,c,msg);
	    c += strlen(msg);
	    if (c > 75)
	    {
		c = 1;
		l++;
	    }
	}
	for (i = 0, l++, c = 1; i < bcount; i++)
	{
	    char msg[255];

	    sprintf(msg, "0x%02x ", idstring[i]);
	    ShowText(l,c,msg);
	    c += strlen(msg);
	    if (c > 75)
	    {
		c = 1;
		l++;
	    }
	}
    }
    show_rc(rc);
}

void
read_reg(int how)	
{
    short 	slot;
    short 	reg;
    int 	rc;
    char	msg[255];
    unsigned char bdata = 0;
    unsigned short wdata = 0;

    drawscreen();
    sprintf(msg,"Read IP Register %s", (how == 1) ? "byte" : "word");
    ShowText(1,59,msg);

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    reg  = (short)ReadUShortMsg(1,5,"Enter register number [hex] ...");
#if defined (HPVISA)
	rc = _ipm_ipreadword(hmpc,reg,&wdata);
#   elif defined (IPC01) || defined (PC104)
	rc = _ipm_ipreadword(hmpc,slot,reg,&wdata);
#else
    if (how == 1)
	rc = _ipm_ipreadbyte(hmpc,slot,reg,&bdata);
    else
	rc = _ipm_ipreadword(hmpc,slot,reg,&wdata);
#endif    

    sprintf(msg, "Data returned: 0x%x", (how == 1) ? bdata : wdata);
    ShowText(7,1,msg);
    show_rc(rc);
}
void
write_reg(int how)
{
    short 	slot;
    short 	reg;
    int 	rc;
    char	msg[255];
    unsigned short wdata;

    drawscreen();
    sprintf(msg,"Write IP Register %s", (how == 1) ? "byte" : "word");
    ShowText(1,59,msg);

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    reg  = (short)ReadUShortMsg(1,5,"Enter register number [hex]....");
    wdata = ReadUShortMsg(1,6,"Enter register data [hex]......");
#if defined (HPVISA)
	rc = _ipm_ipwriteword(hmpc,reg,wdata);
#   elif defined (IPC01) || defined (PC104)
	rc = _ipm_ipwriteword(hmpc,slot,reg,wdata);
#else
    if (how == 1)
	rc = _ipm_ipwritebyte(hmpc,slot,reg,(char)wdata);
    else
	rc = _ipm_ipwriteword(hmpc,slot,reg,wdata);
#endif

    show_rc(rc);
}
void
read_mem(int how)
{
    short 	slot;
    short	len;
    unsigned long off;
    int 	rc;
    short	i,l,c;
    char	msg[255];
    unsigned char data[4096];
    unsigned char *pb;
    unsigned short *pw;

    drawscreen();
    sprintf(msg,"Read IP memory %s", (how == 1) ? "byte" : "word");
    ShowText(1,59,msg);

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    len  = ReadShortMsg(1,5,"How many items ................");
    off  = ReadULongMsg(1,6,"Offset in hex .................");
#if defined (HPVISA)
	rc = _ipm_ipreadwordmem(hmpc,off,len, (unsigned short *)data);
#   elif defined (IPC01) || defined (PC104)
	rc = _ipm_ipreadwordmem(hmpc,slot,off,len, (unsigned short *)data);
#else
    if (how == 1)
	rc = _ipm_ipreadbytemem(hmpc,slot,off,len, data);
    else
	rc = _ipm_ipreadwordmem(hmpc,slot,off,len, (unsigned short *)data);
#endif
    ShowText(8,1,"Data read:");
    pb = &data[0];
    pw = (unsigned short*)&data[0];
    for (i = 0, l = 9, c = 1; i < len; i++)
    {
	if (how == 1)
	    sprintf(msg, "0x%02x ", *pb++);
	else
	    sprintf(msg, "0x%04x ", *pw++);

	ShowText(l,c,msg);
	c += strlen(msg);
	if (c > 75)
	{
	    c = 1;
	    l++;
	}
    }
    show_rc(rc);
}
void
write_mem(int how)
{
    short 	slot;
    short	len;
    unsigned long off;
    int 	rc;
    short	i,l;
    char	msg[255];
    unsigned char data[4096];
    unsigned char *pb;
    unsigned short *pw;

    drawscreen();
    sprintf(msg,"Write IP memory %s", (how == 1) ? "byte" : "word");
    ShowText(1,59,msg);

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    len  = ReadShortMsg(1,5,"How many items ................");
    off  = ReadULongMsg(1,6,"Offset in hex .................");
    pb = &data[0];
    pw = (unsigned short*)&data[0];
    for (i = 0, l = 7; i < len; i++)
    {
	if (how == 1)
	{
	    sprintf(msg, "Enter byte %4d (hex)..........", i);
	    *pb++  = (char)ReadUShortMsg(1,l,msg);
	}
	else
	{
	    sprintf(msg, "Enter word %4d (hex)..........", i);
	    *pw++  = ReadUShortMsg(1,l,msg);
	}
	l++;
    }
#if defined (HPVISA)
	rc = _ipm_ipwritewordmem(hmpc,off,len, (unsigned short *)data);
#   elif defined (IPC01) || defined (PC104)
	rc = _ipm_ipwritewordmem(hmpc,slot,off,len, (unsigned short *)data);
#else
    if (how == 1)
	rc = _ipm_ipwritebytemem(hmpc,slot,off,len, data);
    else
	rc = _ipm_ipwritewordmem(hmpc,slot,off,len, (unsigned short *)data);
#endif
    show_rc(rc);
}
/* End of Carrier-specific IP stuff */
#endif

#if defined (EPC) || defined (PCI2G)
void
epc_fw_version()
{
    int 	rc;
    char	msg[255];
    unsigned char vstring[9];

    drawscreen();
#if defined (EPC) 
    ShowText(1,59,"EPC firmware version");

    rc = _epc_firmware_version(hmpc,vstring);
#else
    ShowText(1,59,"PCI2G firmware version");

    rc = _pci2g_firmware_version(hmpc,vstring);
#endif
    vstring[8] = 0;
    sprintf(msg,"Version String: %s", vstring);
    ShowText(5,1,msg);
    show_rc(rc);
}
#endif

#if defined (EPC) || defined (IPC01) || defined (PC104) || defined (HPVISA) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
void
epc_scan()
{
    int 	rc,ns;
    char	msg[255];
    char	s[6];
    short	i;

    drawscreen();
#if defined (EPC)
    ShowText(1,59,"EPC scan IPM");
    ns = 4;
#elif defined (PC104)
    ShowText(1,59,"PC104 scan IPM");
    ns = 4;
#elif defined (IPC01)
    ShowText(1,59,"IPC01 scan IPM");
    ns = 4;
#elif defined (PCI40)
    ShowText(1,59,"PCI40 scan IPM");
    ns = 4;
#elif defined (PCI2G)
    ShowText(1,59,"PCI2G scan IPM");
    ns = 4;
#elif defined (PCI60)
    ShowText(1,59,"PCI60 scan IPM");
    ns = 6;
#elif defined (HPVISA)
    ShowText(1,59,"HPVISA scan IPM");
    ns = 1;
#endif

    rc = _ipm_scan_ipm(hmpc,s);

    for(i = 0; i < ns; i++)
    {
	short c = i + 5;
	sprintf(msg,"Slot %d: ", i);
	ShowText(c,1,msg);
	switch(s[i])
	{
	    case 0:
		ShowText(c,20,"is empty");
		break;
	    case 1:
		ShowText(c,20,"virgin IP429");
		break;
	    case 2:
		ShowText(c,20,"TechS.A.T IP429");
		break;
	    default:
		ShowText(c,20,"Unknown type");
		break;
	}
    }
    show_rc(rc);
}
#endif

void
set_htsres()
{
    short 	slot;
    short	res;
    int 	rc;

    drawscreen();
    ShowText(1,59,"Set timestamp resolution");

    slot = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    res  = ReadShortMsg(1,5,"Resolution [10,100 microsecs]..");
    
    rc = _set_high_resolution(hmpc,slot,res);
    show_rc(rc);
}

void
set_disc_ipm()
{
    short 	slot;
    short	state;
    int 	rc;

    drawscreen();
    ShowText(1,45,"Set state of discrete signal");

    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    state = ReadShortMsg(1,5,"Enter state [0..1] ............");

#if defined (HPVISA) || defined (PCC) || defined (USB)
    rc = _set_out_state(hmpc,state);
#else
    rc = _set_out_state(hmpc,slot,state);
#endif
    show_rc(rc);
}

void
get_disc_ipm()
{
    short 	slot;
    int		state;

    drawscreen();
    ShowText(1,45,"Get state of discrete signal");

    slot  = enter_ipnumber(1,4,"Enter slot number [0.. %d]........");
#if defined (HPVISA) || defined (PCC) || defined (USB)
    state = _get_in_state(hmpc);
#else
    state = _get_in_state(hmpc,slot);
#endif
    if (state == 0)
	ShowText(5,1,"State of discrete line is LOW");
    else if (state == 1)
	ShowText(5,1,"State of discrete line is HIGH");
    show_rc(state);
}

#if defined (EPC)
void
set_disc_carrier()
{
    short	state;
    int 	rc;

    drawscreen();
    ShowText(1,45,"EPC: Set state of discrete signal");

    state = ReadShortMsg(1,4,"Enter state [0..1] ............");

    rc = _epc_set_out_state(hmpc,state);
    show_rc(rc);
}

void
get_disc_carrier()
{
    int		state;

    drawscreen();
    ShowText(1,45,"EPC: Get state of discrete signal");

    state = _epc_get_in_state(hmpc);
    if (state == 0)
	ShowText(5,1,"State of discrete line is LOW");
    else if (state == 1)
	ShowText(5,1,"State of discrete line is HIGH");
    show_rc(state);
}
#endif
#if defined (PCI2G)
void
set_disc_carrier()
{
    USHORT	state;
    int 	rc;

    drawscreen();
    ShowText(1,45,"PCI2G: Set state of discrete outputs");

    state = ReadUShortMsg(1,4,"Enter state [0..F] ............");

    rc = _pci2g_set_out_state(hmpc,state);
    show_rc(rc);
}

void
get_disc_carrier()
{
    USHORT	state;
    int 	rc;
    char msg[255];

    drawscreen();
    ShowText(1,45,"PCI2G: Get state of discrete inputs");

    rc = _pci2g_get_in_state(hmpc,&state);
    sprintf(msg,"28V Discrete-in states        : %02x (1=28V)", state & 0xff );
    ShowText(5,1,msg);
    sprintf(msg,"Open/Ground Discrete-in states: %02x (1=Ground)", state >> 8 );
    ShowText(6,1,msg);
    show_rc(rc);
}
#endif

#if defined (EPC) || \
    defined (PC104) || \
    defined (IPC01) || \
    defined (PCI40) || \
    defined (PCI60) || \
    defined (PCI2G) || \
    defined (HPVISA)
void
reset_ip429()
{
    short 	slot;
    int		rc;

    drawscreen();
#if defined (PC104)
    ShowText(1,45,"PC104: Reset IP429-Module");
#elif defined (EPC)
    ShowText(1,45,"EPC: Reset IP429-Module");
#elif defined (IPC01)
    ShowText(1,45,"IPC01: Reset IP429-Module");
#elif defined (PCI40)
    ShowText(1,45,"PCI40: Reset IP429-Module");
#elif defined (PCI60)
    ShowText(1,45,"PCI60: Reset IP429-Module");
#elif defined (PCI2G)
    ShowText(1,45,"PCI2G: Reset IP429-Module");
#elif defined (HPVISA)
    ShowText(1,45,"HPVISA: Reset IP429-Module");
#endif
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
#if defined (HPVISA)
    rc = _ip429_reset(hmpc);
#else
    rc = _ip429_reset(hmpc,slot);
#endif
    show_rc(rc);
}
#endif

#if defined (EPC) || defined (PCI2G)
void
reset_ipm()
{
    short 	slot;
    int		rc;

    drawscreen();
#if defined (EPC)
    ShowText(1,45,"EPC: Reset IP-Module");
#else
    ShowText(1,45,"PCI2G: Reset IP-Module");
#endif
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d].......");
    rc = _ipm_ipreset(hmpc,slot);
    show_rc(rc);
}
#endif


#if defined (EPC)
void
write_routing()
{
    int		rc;
    short 	slot;
    short	pmod;

    drawscreen();
    ShowText(1,59,"EPC: Write Routing");

    slot  = ReadShortMsg(1,4,"Enter slot number [0..3].......");
    ShowText(5,1,"Choose mode of programming:");
    ShowText(6,3,"Erase programming ...............................[0]");
    ShowText(7,3,"Program for timing slave, master on same board ..[1]");
    ShowText(8,3,"Program for timing slave, master external .......[2]");
    ShowText(9,3,"Program for timing master........................[3]");
    pmod  = ReadShortMsg(1,10,"Enter programming mode [0..3].......");
    rc = _epc_write_routing(hmpc,slot,pmod);

    show_rc(rc);
}
#endif

void getcardid()
{
    CardIdent ci;
    int rc;
    char msg[255];
    short 	slot;

    drawscreen();
#if defined (PCC)
    ShowText(1,65,"Get PCC-Card ID");
#elif defined (PC104)
    ShowText(1,45,"PC104: Get card ID");
#elif defined (EPC)
    ShowText(1,45,"EPC: Get card ID");
#elif defined (IPC01)
    ShowText(1,45,"IPC01: Get card ID");
#elif defined (PCI40)
    ShowText(1,45,"PCI40: Get card ID");
#elif defined (PCI60)
    ShowText(1,45,"PCI60: Get card ID");
#elif defined (PCI2G)
    ShowText(1,45,"PCI2G: Get card ID");
#elif defined (HPVISA)
    ShowText(1,45,"HPVISA: Get card ID");
#endif
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d].......");

    rc = _get_card_ident(hmpc,
#if !defined (PCC) && ! defined (USB)
    	slot,
#endif    
	&ci );
    if (rc == 0)
    {     
	sprintf(msg,"Hardware revision..: %d.%d",ci.HwRevMajor,ci.HwRevMinor);
        ShowText(5,3,msg);
	sprintf(msg,"Owner..............: %s",ci.CompanyName);
        ShowText(6,3,msg);
	sprintf(msg,"Serial number......: %s",ci.CompanySerialNumber);
        ShowText(7,3,msg);
    }
    show_rc(rc);
}

void geteehwconf()
{
    int rc;
    char msg[255];
    CardIdent ci;
    short	slot;

    drawscreen();
    ShowText(1,46,"Get EEPROM hardware configuration");
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d]........");

    rc = _get_card_ident(hmpc,
#if defined (EPC) || \
    defined (PC104) || \
    defined (IPC01) || \
    defined (PCI40) || \
    defined (PCI60) || \
    defined (PCI2G) || \
    defined (HPVISA)
	    slot, 
#endif
	    &ci);
    if (rc == 0)
    {
	sprintf(msg,"ATx_00_07     = 0x%02X", ci.TxConfig[0]);
        ShowText(5,3,msg);
	sprintf(msg,"ATx_08_15     = 0x%02X", ci.TxConfig[1]);
        ShowText(6,3,msg);
	sprintf(msg,"ATx_16_23     = 0x%02X", ci.TxConfig[2]);
        ShowText(7,3,msg);
	sprintf(msg,"ATx_24_31     = 0x%02X", ci.TxConfig[3]);
        ShowText(8,3,msg);
	sprintf(msg,"ARx_00_07     = 0x%02X", ci.RxConfig[0]);
        ShowText(9,3,msg);
	sprintf(msg,"ARx_08_15     = 0x%02X", ci.RxConfig[1]);
        ShowText(10,3,msg);
	sprintf(msg,"ARx_16_23     = 0x%02X", ci.RxConfig[2]);
        ShowText(11,3,msg);
	sprintf(msg,"ARx_24_31     = 0x%02X", ci.RxConfig[3]);
        ShowText(12,3,msg);
	sprintf(msg,"RS422_00_07   = 0x%02X", ci.RS422Config);
        ShowText(13,3,msg);
	sprintf(msg,"RS232_00_07   = 0x%02X", ci.RS232Config);
        ShowText(14,3,msg);
	sprintf(msg,"I2C_00_07     = 0x%02X", ci.I2CConfig);
        ShowText(15,3,msg);
	sprintf(msg,"DigIN_00_07   = 0x%02X", ci.DiscInConfig);
        ShowText(16,3,msg);
	sprintf(msg,"DigOUT_00_07  = 0x%02X", ci.DiscOutConfig);
        ShowText(17,3,msg);
	sprintf(msg,"AdapBox_00_07 = 0x%02X", ci.AdapBoxConfig);
        ShowText(18,3,msg);
    }
    show_rc(rc);
}

void getstatus()
{
    unsigned long status,bitw;
    int rc;
    char msg[255];
    int i;
    short slot;

    drawscreen();
    ShowText(1,64,"Get Status Word");
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d]........");

    rc = _get_status_word(hmpc, slot, &status);
    if (rc == 0)
    {
	sprintf(msg,"Value of the status word: 0x%08lX",status);
        ShowText(5,3,msg);
        ShowText( 7,14,"Controller 0:ready; 1:busy");
        ShowText( 8,14,"1:Interrupt pending");
        ShowText( 9,14,"1:Rx0 data ready");
        ShowText(10,14,"1:Rx1 data ready");
        ShowText(11,14,"1:Rx2 data ready");
        ShowText(12,14,"1:Rx3 data ready");
#if defined (PCC)
        ShowText(13,14,"1:| A429PCC Type");
        ShowText(14,14,"0:|");
#elif defined (USB)
        ShowText(13,14,"1:| A429USB Type");
        ShowText(14,14,"1:|");
#else
        ShowText(13,14,"0:| A429IPM Type");
        ShowText(14,14,"1:|");
#endif
        ShowText(15,14,"reserved");
#if defined (PCC)
        ShowText(16,14,"1:Power save mode");
#else
        ShowText(16,14,"reserved");
#endif
        ShowText(17,14,"1:Tx0 off");
        ShowText(18,14,"1:Tx1 off");
        ShowText(19,14,"State of internal output 0");
        ShowText(20,14,"State of internal output 1");
        ShowText(21,14,"State of internal input 1");
        ShowText(22,14,"State of internal input 0");
        ShowText( 7,54,"1:Rx0 data exist");
        ShowText( 8,54,"1:Rx1 data exist");
        ShowText( 9,54,"1:Rx2 data exist");
        ShowText(10,54,"1:Rx3 data exist");
        ShowText(11,54,"1:Rx0 data lost");
        ShowText(12,54,"1:Rx1 data lost");
        ShowText(13,54,"1:Rx2 data lost");
        ShowText(14,54,"1:Rx3 data lost");
        ShowText(15,54,"1:Rx0 high-res. timestamps");
        ShowText(16,54,"1:Rx1 high-res. timestamps");
        ShowText(17,54,"1:Rx2 high-res. timestamps");
        ShowText(18,54,"1:Rx3 high-res. timestamps");
        ShowText(19,54,"1:Tx0 data request");
        ShowText(20,54,"1:Tx1 data request");
        ShowText(21,54,"1:Tx0 error");
        ShowText(22,54,"1:Tx1 error");
        bitw = 1;
        for(i=0;i<=15;i++)
        {
            sprintf(msg,"bit%2d=%c:",i,(status & bitw)?'1':'0');
            ShowText((short)(7+i   ), 5, msg);
            bitw <<= 1;
        }
        for(i=16;i<=31;i++)
        {
            sprintf(msg,"bit%2d=%c:",i,(status & bitw)?'1':'0');
            ShowText((short)(7+i-16),45, msg);
            bitw <<= 1;
        }
    }
    show_rc(rc);
}

void resetclock()
{
    int rc;
    short slot;

    drawscreen();
    ShowText(1,68,"Reset Clock");
    slot  = enter_ipnumber(1,4,"Enter slot number [0..%d]........");

    rc = _reset_clock(hmpc,slot);
    show_rc(rc);
}

#if defined (EPC) || defined (HPVISA) || defined (PC104) || defined (IPC01) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
void
epc_features()
{
    const char *items[] = {
#if defined (EPC)
	"R - Reset EPC",
#elif defined (PCI2G)
	"R - Reset PCI2G",
#elif defined (PC104)
	"R - Reset PC104",
#elif defined (IPC01)
	"R - Reset IPC01",
#endif
	"M - Reset IP429-Module",
	"I - Read IP ID",
#if defined (EPC) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
	"A - Write IP byte register",
	"B - Read IP byte register",
#endif
	"C - Write IP word register",
	"D - Read IP word register",
#if defined (EPC) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
	"E - Write IP byte memory",
	"F - Read IP byte memory",
#endif
	"G - Write IP word memory",
	"H - Read IP word memory",
	"S - Scan IP module types",
#if defined (EPC) || defined (PCI2G)
	"T - Toggle IP reset",
	"V - Get Carrier Firmware version",
#endif
#if defined (EPC) 
	"W - Write routing information",
#endif
	"X - return to main menu",
	0 };

    for(;;)
    {
#if defined (EPC)
	switch(runmenu("EPC Features-Menu",items,"RMIABCDEFGHSTVWX"))
#elif defined (PC104)
	switch(runmenu("PC104 Features-Menu",items,"RMICDGHSX"))
#elif defined (IPC01)
	switch(runmenu("IPC01 Features-Menu",items,"RMICDGHSX"))
#elif defined (PCI40)
	switch(runmenu("PCI40 Features-Menu",items,"MIABCDEFGHSX"))
#elif defined (PCI60)
	switch(runmenu("PCI60 Features-Menu",items,"MIABCDEFGHSX"))
#elif defined (PCI2G)
	switch(runmenu("PCI2G Features-Menu",items,"RMIABCDEFGHSXTV"))
#elif defined (HPVISA)
	switch(runmenu("HPVISA Features-Menu",items,"MIABCDEFGHSX"))
#endif
	{
#if defined (EPC) || defined (IPC01) || defined (PC104) || defined (PCI2G)
	    case 'R':
		reset_epc();
		break;
#endif
	    case 'M':
		reset_ip429();
		break;
	    case 'I':
		read_ipid();
		break;
#if defined (EPC) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
	    case 'A':
		write_reg(1);
		break;
	    case 'B':
		read_reg(1);
		break;
#endif
	    case 'C':
		write_reg(2);
		break;
	    case 'D':
		read_reg(2);
		break;
#if defined (EPC) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
	    case 'E':
		write_mem(1);
		break;
	    case 'F':
		read_mem(1);
		break;
#endif
	    case 'G':
		write_mem(2);
		break;
	    case 'H':
		read_mem(2);
		break;
	    case 'S':
		epc_scan();
		break;
#if defined (EPC) || defined (PCI2G)
	    case 'T':
		reset_ipm();
		break;
	    case 'V':
		epc_fw_version();
		break;
#endif
#if defined (EPC)
	    case 'W':
		write_routing();
		break;
#endif
	    case 'X':
		return;
		break;
	}
    }
}
#endif

void
miscellaneous()
{
    const char *items[] = {
	"B - get Board type",
#if defined (PCC) || defined (EPC) || defined (PCI2G)
		"E - Establish loop",
#endif
 	"F - get Firmware version",
#if defined (ISNOCHNICH)
        "D - Download firmware",
#endif
    "I - get pc/ip-card Id",
    "H - get eeprom Hardware configuration",
    "S - get Status word",
    "C - reset Clock",
    "T - get elapsed Time (extended)",
    "G - get elapsed time",
	"R - Reset selftest",
	"M - set timestamp sychro Mode",
	"X - return to main menu",
	0 };

    for(;;)
    {
#if defined (PCC) || defined (EPC) || defined (PCI2G)
	switch(runmenu("Miscellaneous-Menu",items,"BEFIHSCRTGMX"))
#else
	switch(runmenu("Miscellaneous-Menu",items,"BFIHSCRTGMX"))
#endif
	{
	    case 'B':
		get_boa();
		break;
#if defined (PCC) || defined (EPC) || defined (PCI2G)
	    case 'E':
		est_lop();
		break;
#endif
	    case 'F':
		get_fw();
		break;
#if defined (ISNOCHNICH)
            case 'D':
		dl_fw();
		break;
#endif	    
        case 'I':
		getcardid();
		break;
            case 'H':
		geteehwconf();
		break;
        case 'S':
		getstatus();
		break;
        case 'C':
		resetclock();
		break;
	    case 'T':
		get_xtime();
		break;
	    case 'R':
		reset_st();
		break;
	    case 'G':
		get_time();
		break;
	    case 'M':
		set_synchro_mode();
		break;
	    case 'X':
		return;
		break;
	}
    }
}

void
receive()
{
    const char *items[] = {
	"A - Activate receiver",
	"D - get receiver Data ready",
	"G - Get receiver mode",
	"R - Read receiver buffer",
	"S - Set receiver buffer size",
	"B - get receiver Buffer count",
	"E - get receiver data Exist",
	"L - get receiver data Lost",
	"T - reseT receiver data lost",
	"O - set timestamp resOlution",
	"H - set Highres timestamp resolution",
	"I - get timestamp resolutIon",
	"X - return to main menu",
	0 };

    for(;;)
    {
	switch(runmenu("Receive-Menu", items, "ADGRSBELTOHIX"))
	{
	    case 'A':
		act_rec();
		break;
	    case 'D':
		grec_dr();
		break;
	    case 'G':
		grec_mo();
		break;
	    case 'R':
		rea_rxb();
		break;
	    case 'S':
		set_rb();
		break;
	    case 'B':
		grec_bu();
		break;
        case 'E':
           grec_de();
           break;
	    case 'L':
		grec_dl();
		break;
            case 'T':
		rstrec_dl();
		break;
	    case 'O':
		set_tsres();
		break;
	    case 'H':
		set_htsres();
		break;
	    case 'I':
		get_tsres();
		break;
	    case 'X':
		return;
		break;
	}
    }
}

void
transmit()
{
    const char *items[] = {
	"A - Activate transmissions",
	"D - Define transmissions",
	"E - dElete transmissions",
	"G - Get transmission definition",
	"M - set transmission Mode",
	"S - get Single transmission definition",
	"T - get Timeslice",
	"U - Update transmission",
    "R - get tRansmitter status",
    "X - return to main menu",
	0 };
    for(;;)
    {
	switch(runmenu("Transmit-Menu",items,"ADEGMSTURX"))
	{
	    case 'A':
		act_tra();
		break;
	    case 'D':
		def_tra();
		break;
	    case 'E':
		del_tra();
		break;
	    case 'G':
		get_tra();
		break;
	    case 'M':
		set_tra();
		break;
	    case 'S':
		get_s_tra();
		break;
	    case 'T':
		get_ts();
		break;
	    case 'U':
		upd_tra();
		break;
	    case 'R':
		get_tra_stat();
		break;
        case 'X':
		return;
		break;
	}
    }
}

void
trigger()
{
    const char *items[] = {
	"D - Define trigger events",
	"G - Get state of trigger ID",
	"S - Set state of trigger ID",
	"X - return to Main menu",
	0 };

    for(;;)
    {
	switch(runmenu("Trigger-Menu", items, "DGSX"))
	{
	    case 'D':
		def_trig();
		break;
	    case 'G':
		get_trig();
		break;
	    case 'S':
		set_trig();
		break;
	    case 'X':
		return;
		break;
	}
    }
}
#if defined (USB)
void
set_usb_discr()
{
    int            rc;
	unsigned short mask,data;

    drawscreen();
    ShowText(1,49,"Set USB discretes");
    mask = ReadUShortMsg(1,4,"data mask .....");
    data = ReadUShortMsg(1,5,"data value.....");

	rc = _usb_set_discretes( hmpc, mask, data );
    show_rc(rc);
}
void
get_usb_discr()
{
    int            rc;
	unsigned short data;
	char msg[256];

    drawscreen();
    ShowText(1,49,"Get USB discretes");

	rc = _usb_get_discretes( hmpc, &data );
    sprintf(msg, "Data returned: 0x%x", data);
    ShowText(7,1,msg);
    show_rc(rc);
}
#endif
void
discretes()
{
    const char *items[] = {
#if defined (PCC)
        "S - Set state of discrete on PCC",
		"G - Get state of discrete on PCC",
#elif defined (USB)
        "S - Set state of discretes on USB",
		"G - Get state of discretes on USB",
#else
        "S - Set state of discrete on IP-Module",
		"G - Get state of discrete on IP-Module",
#endif
#if defined (EPC)
		"O - Set state of discrete on Carrier",
		"I - Get state of discrete on Carrier",
#endif
#if defined (PCI2G)
		"O - Set state of discretes on Carrier",
		"I - Get state of discretes on Carrier",
#endif
		"X - return to Main menu",
	0 };

    for(;;)
    {
#if defined (EPC) || defined (PCI2G)
	switch(runmenu("Discretes-Menu", items, "SGOIX"))
#else
	switch(runmenu("Discretes-Menu", items, "SGX"))
#endif
		{
		case 'S':
#if defined (USB)
			set_usb_discr();
#else
			set_disc_ipm();
#endif
			break;
		case 'G':
#if defined (USB)
			get_usb_discr();
#else
			get_disc_ipm();
#endif
			break;
#if defined (EPC) || defined (PCI2G)
		case 'O':
			set_disc_carrier();
			break;
		case 'I':
			get_disc_carrier();
			break;
#endif
		case 'X':
			return;
			break;
		}
    }
}

void main(int argc, char **argv)
{
    const char *items[] = {
	"B - select Board",
	"C - Configure",
	"I - trIgger",
	"M - Miscellaneous",
	"R - Receive",
	"T - Transmit",
#if defined (EPC)
	"E - EPC features",
#endif
#if defined (PC104)
	"E - PC104 features",
#endif
#if defined (IPC01)
	"E - IPC01 features",
#endif
#if defined (PCI40)
	"E - PCI40 features",
#endif
#if defined (PCI60)
	"E - PCI60 features",
#endif
#if defined (PCI2G)
	"E - PCI2G features",
#endif
#if defined (HPVISA)
	"E - HPVISA features",
#endif
#if defined (EPC) || defined (PCC) || defined (USB) || defined (PC104) || defined (IPC01) || defined (PCI40) || defined (PCI60) || defined (PCI2G)
	"D - Discrete signals",
#endif
	"X - eXit demo",
	0 };

    video_initialize();
    initialize();
#if ! defined(LINUX)
    SetColor(COLOR_WHITE,COLOR_BLACK);
#endif

    for (;;)
    {
#if defined (EPC) || \
    defined (PC104) || \
    defined (IPC01) || \
    defined (PCI40) || \
    defined (PCI60) || \
    defined (PCI2G) || \
    defined (HPVISA)
	switch(runmenu("Main-Menu", items, "BCIMRTEDX"))
#endif
#if defined (PCC) || defined (USB)
	switch(runmenu("Main-Menu", items, "BCIMRTDX"))
#endif
	{
	    case 'B':
		switch_board();
		break;
            case 'C':
		configure();
		break;
	    case 'I':
		trigger();
		break;
	    case 'M':
		miscellaneous();
		break;
	    case 'R':
		receive();
		break;
	    case 'T':
		transmit();
		break;
#if defined (EPC) || \
    defined (PC104) || \
    defined (IPC01) || \
    defined (PCI40) || \
    defined (PCI60) || \
    defined (PCI2G) || \
    defined (HPVISA)
	    case 'E':
		epc_features();
		break;
#endif
	    case 'D':
		discretes();
		break;
	    case 'X':
#if defined(LINUX)
		clear();
#else
		ClearScreen();
#endif
		if (hmpc)
		    _close_xpc(hmpc);
#if defined(LINUX)
		endwin();
#endif
		exit(0);
		break;
	}
    }
#if defined(LINUX)
	endwin();
#endif
}
