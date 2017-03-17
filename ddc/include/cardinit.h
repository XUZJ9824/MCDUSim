/******************************************************************************
 * FILE: cardinit.h
 *
 * DESCRIPTION:
 *
 * Prototypes for Card initialization, shutdown, version
 * information, and interrupt facility configuration.
 *
 * API:  InitCard
 *       FreeCard
 *       InstallHandler
 *       UninstallHandler
 *       SetIntCondition
 *       GetIntStatus
 *       GetLibVersion
 *       GetLibVersionEx
 *       GetCardType
 *       GetSwVersionInfo
 *       GetHwVersionInfo
 *       GetChannelCount
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __CARDINIT_H__
#define __CARDINIT_H__

#include "hardware.h"   /* MAX_NUM_RX */

/* Channel type. */
enum { CHAN_TYPE_429=0, CHAN_TYPE_232, CHAN_TYPE_485, CHAN_TYPE_TT, MAX_CHAN_TYPE };

/* 429 RX interrupt enable/status masks. */
#define INT_COND_429_PROTO       0x00001000
#define INT_COND_429_FAIL        0x00000800
#define INT_COND_429_FUNCTEST    0x00000400
#define INT_COND_429_SOLO        0x00000200
#define INT_COND_429_TXSTART     0x00000100
#define INT_COND_429_NODATA      0x00000080
#define INT_COND_429_TXEND       0x00000040
#define INT_COND_429_NORMAL      0x00000020
#define INT_COND_429_DATAMATCH   0x00000010
#define INT_COND_429_WORDTYPE    0x00000008
#define INT_COND_429_RXFULL      0x00000004
#define INT_COND_429_RXPARITY    0x00000002
#define INT_COND_429_RXOVRFLO    0x00000001
#define INT_COND_429_NONE        0x00000000

/* UART interrupt enable masks. */
#define INT_COND_SER_NONE        0x00
#define INT_COND_SER_RXENA       0x01
#define INT_COND_SER_TXENA       0x02
#define INT_COND_SER_LSTATENA    0x04
#define INT_COND_SER_MSTATENA    0x08

/* UART interrupt status masks. */
#define INT_STAT_SER_LSR         0x06
#define INT_STAT_SER_RXRDY       0x04
#define INT_STAT_SER_RXTO        0x0C
#define INT_STAT_SER_TXRDY       0x02
#define INT_STAT_SER_MSR         0x00
#define INT_STAT_SER_XONOFF      0x10
#define INT_STAT_SER_STATE       0x20

typedef struct
{
   /* Flash Information */
   U32BIT StartAddress;
   U32BIT SectorCount;
   U32BIT MinSectorSize;
} FLASH_INFO_TYPE;

typedef struct _RX_STATE_TYPE
{
    S16BIT rxParity;
    S16BIT rxSpeed;
    S16BIT rxMode;
    S16BIT rxState;
    S16BIT rxFilter;
    S16BIT rxTimeStamp;
    S16BIT rxLoopBack;
} RX_STATE_TYPE;

typedef struct _TX_STATE_TYPE
{
    S16BIT txOffset[1024];
    S16BIT txParity;
    S16BIT txSpeed;
    S16BIT txState;
} TX_STATE_TYPE;

/* Card structure. */
typedef struct _CARDINFO
{
   OSCONTEXT *pContext;                     /* OS specific information.          */
   U32BIT dwMemBaseAddr;                    /* Memory region.                    */
   U32BIT dwRegBaseAddr;                    /* Register region.                  */
   U32BIT dwCfgBaseAddr;                    /* PLX Register region               */
   S16BIT wIrqInstalled;                    /* non-zero if handler is installed  */
   U32BIT adwStatus[MIO_429_MAX_NUM_RX+1];  /* Interrupt status array.           */
   U8BIT  bBusNum;                          /* Device physical location.         */
   U8BIT  bSlotNum;                         /* PCI Slot Number                   */
   U8BIT  bFuncNum;                         /* PCI Function Number               */
   S16BIT wCardType;                        /* Device ID.                        */
   S16BIT wInterfaceType;                   /* PCI, USB.                         */
   U16BIT bInitialized;                     /* Initialization status.            */
   S16BIT wBitFormat;                       /* ARINC bit format.                 */
   CHANCOUNT_t strChanCount;                /* Number of various channels.       */

   /* Card access function pointers. */
   U32BIT (*ReadMem32)(S16BIT Card, U32BIT dwAddr);
   void (*WriteMem32)(S16BIT Card, U32BIT dwAddr, U32BIT dwData);
   U32BIT (*ReadReg32)(S16BIT Card, U32BIT dwAddr);
   void (*WriteReg32)(S16BIT Card, U32BIT dwAddr, U32BIT dwData);

   /* User handler callbacks. */
   S16BIT (*pfHandler429)(S16BIT Card);
   S16BIT (*pfHandler232)(S16BIT Card);
   S16BIT (*pfHandler485)(S16BIT Card);
   S16BIT (*pfHandlerTT)(S16BIT Card);

   FLASH_INFO_TYPE flashInfo;
  /* IO info */
  IOINFO_t	strIOInfo;
  
  RX_STATE_TYPE strRxState[MIO_429_MAX_NUM_RX];
  TX_STATE_TYPE strTxState[MIO_429_MAX_NUM_TX];

} CARDINFO_t, *CARDINFO_p;


/* Card initialization.  */
S16BIT DDCAPI InitCard(S16BIT Card);

/* Card shutdown.  */
S16BIT DDCAPI FreeCard(S16BIT Card);

/* Returns the RTL version.  */
U16BIT DDCAPI GetLibVersion(void);
S16BIT DDCAPI GetLibVersionEx(U16BIT *nMajor, U16BIT *nMinora, U16BIT *nMinorb);

/* Returns unique device identifier.  */
S16BIT DDCAPI GetCardType(S16BIT Card);

/* Advanced functions to return device/driver information. */
S16BIT DDCAPI GetSwVersionInfo(PSWVERSIONINFO pSwVersionInfo);
S16BIT DDCAPI GetHwVersionInfo(S16BIT Card, PHWVERSIONINFO pHwVersionInfo);

/* Returns device 429 Rx/Tx, Serial, Discrete, etc. channel count.  */
S16BIT DDCAPI GetChannelCount(S16BIT Card, CHANCOUNT_p pChanCount);

/* Associates a user defined interrupt handler for a particular
   channel type (ARINC Rx/RS232/RS485/TimeTag) to the device.    */
S16BIT DDCAPI InstallHandler(S16BIT Card,
                             U8BIT Type,
                             S16BIT (*Handler)(S16BIT),
                             S16BIT Param);

/* Removes a user interrupt handler.  */
S16BIT DDCAPI UninstallHandler(S16BIT Card, U8BIT Type);

/* Configures a device channel of a particular type (ARINC Rx/RS232/RS485/TimeTag)
   to issue interrupts based upon the passed in conditions.  */
S16BIT DDCAPI SetIntCondition(S16BIT Card, U8BIT Type, U8BIT Channel,
                              U32BIT Condition);

/* Returns the interrupt status of a device channel of a particular type.  */
S16BIT DDCAPI GetIntStatus(S16BIT Card, U8BIT Type, U8BIT Channel,
                           U32BIT *Status);

#endif /* __CARDINIT_H__ */

