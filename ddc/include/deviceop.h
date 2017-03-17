/*
 * Data Device Corp.  Copyright 2007.  All rights reserved.
 *
 * Prototypes for operating system specific card initialization,
 * shutdown, and generalized system calls.
 *
 *  1.0  10/29/06    Added BU-65590 Multi-IO support for VxWorks.
 *  1.1   5/07/07    Removed unused defines
 *  1.2   9/07/07	 Added 64 bit data type.
 *
 */

#ifndef __DEVICEOP429_H__
#define __DEVICEOP429_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 /* Windows 2000 or later */
#endif //_WIN32_WINNT

#include <stdio.h>
#include <windows.h>
#include "ddccommon.h"

#define MAJOR_REV  3
#define MINOR_REV  1
#define MINOR_REVB 3
#define REVISION   0

#define DDCAPI WINAPI

/* These should be defined in the common OS/driver/RTL module. */
#define MAX_NUM_DEVS        8
#define VENDID_DDC      0x4DDC
#define DEVID_BU65578   0x1600    /* Needed? */
#define DEVID_BU65590FM 0x1300
#define DEVID_BU65590C  0x1700
#define DEVID_BU65590U  0x5590
#define DEVID_BU67102U  0x7102
#define DEVID_BU67103U  0x7103
#define DEVID_BU67107FM 0x2000
#define DEVID_BU67108C	0x1F00

/* Minimum card FPGA version supported. */
#define FPGA_REV_MIN_65590FM        10
#define FPGA_REV_MIN_65590C         9
#define FPGA_REV_MIN_65590U			25
#define FPGA_REV_MIN_67102_3U       0
#define FPGA_REV_MIN_UNKNOWN_DEV    0xFFFF
#define FPGA_REV_MIN_67107FM		0
#define FPGA_REV_MIN_67108C			0

/* Interrupt thread array element indicies. */
#define INT_DISPATCH    0
#define INT_WORK        1

/* Single Processor Affinity Mask */
#define SINGLE_PROC_AFFINTIY_MASK	0x1L

/* 64 bit time stamp mask. */
#define	TIME_STAMP_MASK_MSB	0xFFFFFFFF00000000
#define TIME_STAMP_MASK_LSB 0x00000000FFFFFFFF

typedef struct _OSCONTEXT
{
   HANDLE hDriverApp;
   HANDLE hDriverIrq;
   HANDLE hBlockEvent;
   HANDLE hExitEvent;
   HANDLE hThread[2];
   DWORD dwThreadID[2];
} OSCONTEXT;

/* DDC Device Command Control Structure */
typedef struct _IOCTL_CMD
{
	ULONG	Channel;
	ULONG	Param1;
	ULONG	Param2;
	ULONG	Param3;
	ULONG	Param4;
} IOCTL_CMD, *PIOCTL_CMD;

/* Card reset and interrupt enable masks. */
#define CARD_RESET        0x00000001
#define ARINC_INT_ENABLE  0x00000002
#define SER_INT_ENABLE    0x80000000

enum { ISA=0, PCI, USB };

S16BIT osInitCard(S16BIT Card);
S16BIT osFreeCard(S16BIT Card);
S16BIT osInitIO(S16BIT Card, U16BIT Options);
S16BIT osFreeIO(S16BIT Card);
void DDCAPI msleep(U32BIT dwMilliSeconds);

typedef struct _IOINFO
{
   U16BIT bInitialized;                     /* Initialization status.            */
   OSCONTEXT *pContext;                     /* OS specific information.          */
}IOINFO_t, *IOINFO_p;


/* This function determines if the card meets the minimum FPGA revision */
S16BIT checkMinFpgaRev(U16BIT Card);

S16BIT _dv429GetHwVersionInfo(S16BIT Card, PHWVERSIONINFO pHwVersionInfo);
S16BIT _dvReadRxQueueIrigMore( S16BIT Card, S16BIT Receiver, S16BIT N, 
                               U32BIT *Data, U32BIT *StampHi, U32BIT *StampLo );
S16BIT _dvSetLegacyLoopBack(S16BIT Card);

S16BIT _dvLoadTxQueueMore(S16BIT Card, S16BIT Transmitter, S16BIT N,U32BIT *Data);

S16BIT _dvGetPgrmLoopBack(S16BIT Card, S16BIT Receiver, S16BIT *Transmitter);
S16BIT _dvSetPgrmLoopBack(S16BIT Card, S16BIT Receiver, S16BIT Transmitter);

S16BIT  _dvGetTimeStampConfig(S16BIT Card, U8BIT *Format, U8BIT *Rollover,
                              U8BIT *Resolution);
S16BIT  _dvSetTimeStampConfig(S16BIT Card, U8BIT Format, U8BIT Rollover,
                              U8BIT Resolution);

S16BIT _dvLoadTxQueueOne(S16BIT Card, S16BIT Transmitter, U32BIT Data);

#endif /* __DEVICEOP429_H__ */

