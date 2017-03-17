/*******************************************************************************
 * FILE: serial.h
 *
 * DESCRIPTION:
 *
 * Prototypes for card level Serial IO configuration,
 * UART configuration control, and individual UART control.
 *
 * API:  WriteSerialIoControl
 *       ReadSerialIoControl
 *       WriteUartConfig
 *       ReadUartConfig
 *       Write232
 *       Read232
 *       Write485
 *       Read485
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 ******************************************************************************/

#ifndef __SERIAL_H__
#define __SERIAL_H__

/* Individual UART registers */
#define MIO_RHR         0x0
#define MIO_THR         0x0
#define MIO_DLL         0x0
#define MIO_DLM         0x1
#define MIO_IER         0x1
#define MIO_ISR         0x2
#define MIO_FCR         0x2
#define MIO_LCR         0x3
#define MIO_MCR         0x4
#define MIO_LSR         0x5
#define MIO_MSR         0x6
#define MIO_485TADELAY  0x6
#define MIO_SPR         0x7
#define MIO_FCTR        0x8
#define MIO_EFR         0x9
#define MIO_TXCNT       0xA
#define MIO_TXTRG       0xA
#define MIO_RXCNT       0xB
#define MIO_RXTRG       0xB
#define MIO_XOFF1       0xC
#define MIO_XCHAR       0xC
#define MIO_XOFF2       0xD
#define MIO_XON1        0xE
#define MIO_XON2        0xF

/* Quad UART device control registers */
#define MIO_INT0        0x80
#define MIO_INT1        0x81
#define MIO_INT2        0x82
#define MIO_INT3        0x83
#define MIO_TIMERCTL    0x84
#define MIO_TIMER       0x85
#define MIO_TIMERLSB    0x86
#define MIO_TIMERMSB    0x87
#define MIO_8XMODE      0x88
#define MIO_REG1        0x89
#define MIO_RESET       0x8A
#define MIO_SLEEP       0x8B
#define MIO_DREV        0x8C
#define MIO_DVID        0x8D
#define MIO_REG2        0x8E

/* Writes 32 bit value to card's Serial IO Control register */
S16BIT DDCAPI WriteSerialIoControl(S16BIT Card, U32BIT Data);

/* Reads 32 bit value from card's Serial IO Control register */
S16BIT DDCAPI ReadSerialIoControl(S16BIT Card, U32BIT *Data);

/* Writes 8 bit value to UART's Serial Device Configuration register */
S16BIT DDCAPI WriteUartConfig(S16BIT Card, U8BIT Reg, U8BIT Data);

/* Reads 8 bit value from UART's Serial Device Configuration register */
S16BIT DDCAPI ReadUartConfig(S16BIT Card, U8BIT Reg, U8BIT *Data);

/* Writes 8 bit value to individual RS232 UART's Channel Configuration registers */
S16BIT DDCAPI Write232(S16BIT Card, U8BIT Chan, U8BIT Reg, U8BIT Data);

/* Reads 8 bit value from individual RS232 UART's Channel Configuration registers */
S16BIT DDCAPI Read232(S16BIT Card, U8BIT Chan, U8BIT Reg, U8BIT *Data);

/* Writes 8 bit value to individual 422/485 UART's Channel Configuration registers */
S16BIT DDCAPI Write485(S16BIT Card, U8BIT Chan, U8BIT Reg, U8BIT Data);

/* Reads 8 bit value from individual 422/485 UART's Channel Configuration registers */
S16BIT DDCAPI Read485(S16BIT Card, U8BIT Chan, U8BIT Reg, U8BIT *Data);

#endif /* __SERIAL_H__ */

