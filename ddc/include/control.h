/*******************************************************************************
 * FILE: control.h
 *
 * DESCRIPTION:
 *
 * Prototypes for various card feature control routines, including
 * Tx/Rx group reset, loopback control, card level ARINC 429 bit
 * formatting, and discrete line control.
 *
 * API:  ResetGroup
 *       GetBitFormat
 *       SetBitFormat
 *       GetLoopBack
 *       SetLoopBack
 *       SetDiscOut
 *       GetDiscOut
 *       SetDiscDir
 *       GetDiscDir
 *       GetDiscIn
 *       GetDiscAll
 *       SetDiscAll
 *       SetAvionOut
 *       GetAvionOut
 *       SetAvionOutEnable
 *       GetAvionOutEnable
 *       GetAvionIn
 *       GetAvionAll
 *       SetAvionAll
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __CONTROL_H__
#define __CONTROL_H__

enum { DD429_LOW_SPEED=0, DD429_HIGH_SPEED, DD429_ARINC575 };
enum { DD429_NO_PARITY=0, DD429_ODD_PARITY, DD429_EVEN_PARITY };
enum { DD429_DISABLE=0, DD429_ENABLE, DD429_ENABLE_ALT };
enum { DD429_BITFORMAT_ORIG=0, DD429_BITFORMAT_ALT };

/* This functions resets a Tx/Rx channel group. */
S16BIT DDCAPI ResetGroup(S16BIT Card, S16BIT Group);

/* This function resets a receiver channel. */
S16BIT DDCAPI ResetRxChannel(S16BIT Card, S16BIT Receiver);

/* This function resets a transmitter channel. */
S16BIT DDCAPI ResetTxChannel(S16BIT Card, S16BIT Transmitter);

/* This function returns the current card level transmit and receive bit format. */
S16BIT DDCAPI GetBitFormat(S16BIT Card);

/* This function sets the card level bit format. */
S16BIT DDCAPI SetBitFormat(S16BIT Card, S16BIT BitFormat);

/* This function returns the status of the internal loopback of a Tx/Rx channel group. */
S16BIT DDCAPI GetLoopBack(S16BIT Card, S16BIT Group);

/* This function enables or disables the internal loopback of a Tx/Rx channel group. */
S16BIT DDCAPI SetLoopBack(S16BIT Card, S16BIT Group, S16BIT LoopBack);

/* This function returns the status o the internal loopback of a Rx channel. */
S16BIT DDCAPI GetChannelLoopBack(S16BIT Card, S16BIT Receiver);

/* This function enables or disables the internal loopback of a Rx channel. */
S16BIT DDCAPI SetChannelLoopBack(S16BIT Card, S16BIT Receiver, S16BIT LoopBack);

/* This function gets which transmitter a receiver is to be internally connected to
   when loopback is enabled. */
S16BIT DDCAPI GetLoopBackMapping(S16BIT Card, S16BIT Receiver, S16BIT *TxMapping);

/* This function programs a receiver to be internally connected to the desired
   transmitter when loopback is enabled. */
S16BIT DDCAPI SetLoopBackMapping(S16BIT Card, S16BIT Receiver, S16BIT TxMapping);

/* This function initializes the IO functions. */
S16BIT DDCAPI IOInitialize(S16BIT Card, U16BIT Options);

/* This function Frees the IO functions. */
S16BIT DDCAPI IOFree(S16BIT Card);

/* This function sets the state of a discrete output. */
S16BIT DDCAPI SetDiscOut(S16BIT Card, S16BIT Discrete, S16BIT Level);

/* This function returns the current state of a discrete ouput. */
S16BIT DDCAPI GetDiscOut(S16BIT Card, S16BIT Discrete);

/* This function sets the direction of a discrete line. */
S16BIT DDCAPI SetDiscDir(S16BIT Card, S16BIT Discrete, S16BIT Direction);

/* This function returns the direction a discrete line. */
S16BIT DDCAPI GetDiscDir(S16BIT Card, S16BIT Discrete);

/* This function returns the current level of a discrete input. */
S16BIT DDCAPI GetDiscIn(S16BIT Card, S16BIT Discrete);

/* Get all discrete line directions and levels. */
S16BIT DDCAPI GetDiscAll(S16BIT Card, U16BIT *Directions, U16BIT *Levels);

/* Set all discrete line directions and levels. */
S16BIT DDCAPI SetDiscAll(S16BIT Card, U16BIT  Directions, U16BIT  Levels);

/* This function sets the level of an avionic output. */
S16BIT DDCAPI SetAvionOut(S16BIT Card, S16BIT Avionic, S16BIT Level);

/* This function returns the current level of an avionic output. */
S16BIT DDCAPI GetAvionOut(S16BIT Card, S16BIT Avionic);

/* This function sets the output enable state of an avionic line. */
S16BIT DDCAPI SetAvionOutEnable(S16BIT Card, S16BIT Avionic, S16BIT State);

/* This function returns the output enable state an avionic line. */
S16BIT DDCAPI GetAvionOutEnable(S16BIT Card, S16BIT Avionic);

/* This function returns the current level of an avionic input. */
S16BIT DDCAPI GetAvionIn(S16BIT Card, S16BIT Avionic);

/* Get all avionic line output enables and levels. */
S16BIT DDCAPI GetAvionAll(S16BIT Card, U16BIT *OutEnables, U16BIT *Levels);

/* Set all avionic line output enables and levels. */
S16BIT DDCAPI SetAvionAll(S16BIT Card, U16BIT  OutEnables, U16BIT  Levels);

#endif /* __CONTROL_H__ */

