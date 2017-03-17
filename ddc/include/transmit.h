/*******************************************************************************
 * FILE: transmit.h
 *
 * DESCRIPTION:
 *
 * Prototypes for ARINC 429 transmit configuration
 * and FIFO and scheduled message transmission.
 *
 * API:  EnableTx
 *       GetTxParity
 *       SetTxParity
 *       GetTxSpeed
 *       SetTxSpeed
 *       GetTxStatus
 *       GetTxQueueStatus
 *       LoadTxQueueOne
 *       LoadTxQueueMore
 *       AddRepeated
 *       ClearRepeated
 *       DelRepeated
 *       GetNumOfRepeated
 *       GetRepeated
 *       GetAllRepeated
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 ******************************************************************************/

#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

/* This function enables a Transmitter for operation. */
S16BIT DDCAPI EnableTx(S16BIT Card, S16BIT Transmitter, S16BIT Enable);

/* This function returns a Transmitter's parity setting. */
S16BIT DDCAPI GetTxParity(S16BIT Card, S16BIT Transmitter);

/* This function sets a Transmitter's parity setting. */
S16BIT DDCAPI SetTxParity(S16BIT Card, S16BIT Transmitter, S16BIT Parity);

/* This function returns a Transmitter's speed. */
S16BIT DDCAPI GetTxSpeed(S16BIT Card, S16BIT Transmitter);

/* This function sets a Transmitter's speed. */
S16BIT DDCAPI SetTxSpeed(S16BIT Card, S16BIT Transmitter, S16BIT Speed);

/* This function returns a Transmitter's enabled status. */
S16BIT DDCAPI GetTxStatus(S16BIT Card, S16BIT Transmitter);

/* This function determines a transmitter's queue status. */
S16BIT DDCAPI GetTxQueueStatus(S16BIT Card, S16BIT Transmitter);

/* This function loads a 32-bit word into a transitter's queue. */
S16BIT DDCAPI LoadTxQueueOne(S16BIT Card, S16BIT Transmitter, U32BIT Data);

/* This function loads multiple 32-bit words into a transmitter's queue. */
S16BIT DDCAPI LoadTxQueueMore(S16BIT Card, S16BIT Transmitter, S16BIT N,
                              U32BIT *Data);

/* This function schedules a repeated transmission of a word. */
S16BIT DDCAPI AddRepeated(S16BIT Card, S16BIT Transmitter, U32BIT Data,
                          S16BIT Frequency, S16BIT Offset);

/* This function clears a transmitter's schedule table. */
S16BIT DDCAPI ClearRepeated(S16BIT Card, S16BIT Transmitter);

/* This function deletes a label/SDI from a transmitter's schedule table. */
S16BIT DDCAPI DelRepeated(S16BIT Card, S16BIT Transmitter, S16BIT LabelSdi);

/* This function obtains the total number of words in a transmitter's table. */
S16BIT DDCAPI GetNumOfRepeated(S16BIT Card, S16BIT Transmitter);

/* This function tells whether a specific label/SDI is in the schedule table. */
S16BIT DDCAPI GetRepeated(S16BIT Card, S16BIT Transmitter, S16BIT LabelSdi,
                          U32BIT *Data, S16BIT *Frequency, S16BIT *Offset);

/* This function returns all labels/SDI's in the schedule table. */
S16BIT DDCAPI GetAllRepeated(S16BIT Card, S16BIT Transmitter, S16BIT *LabelSdi);

#endif /* __TRANSMIT_H__ */

