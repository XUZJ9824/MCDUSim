/*******************************************************************************
 * FILE: receive.h
 *
 * DESCRIPTION:
 *
 * Prototypes for ARINC 429 receive configuration
 * and FIFO and mailbox reception routines.
 *
 * API:  EnableRx
 *       GetRxMode
 *       GetRxChannelMode
 *       SetRxMode
 *       SetRxChannelMode
 *       GetRxParity
 *       GetRxChannelParity
 *       SetRxParity
 *       SetRxChannelParity
 *       GetRxSpeed
 *       GetRxChannelSpeed
 *       SetRxSpeed
 *       SetRxChannelSpeed
 *       GetRxStatus
 *       EnableTimeStamp
 *       ConfigTimeStamp
 *       GetTimeStampStatus
 *       ResetTimeStamp
 *       GetTimeStamp
 *       ClearMailbox
 *       GetMailbox
 *       GetMailboxStatus
 *       ReadMailboxIrig
 *       ClearRxQueue
 *       GetRxQueueStatus
 *       ReadRxQueueIrigOne
 *       ReadRxQueueIrigMore
 *       EnableFilter
 *       ConfigFilter
 *       AddFilter
 *       ClearFilter
 *       DelFilter
 *       GetFilter
 *       GetAllFilter
 *       GetFilterStatus
 *       GetNumOfFilter
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 ******************************************************************************/

#ifndef __RECEIVE_H__
#define __RECEIVE_H__

enum { MAILBOX_MODE=0, FIFO_MODE };

enum
{
   TT48 = 0,
   IRIG_A,
   IRIG_B,
   IRIG_C,
   IRIG_D,
   MAX_TTFORMAT
};

enum
{
   TTRO_16 = 0,
   TTRO_17,
   TTRO_18,
   TTRO_19,
   TTRO_20,
   TTRO_21,
   TTRO_22,
   TTRO_48,
   MAX_TTRO
};

enum
{
   TTRES_64 = 0,
   TTRES_32,
   TTRES_16,
   TTRES_8,
   TTRES_4,
   TTRES_2,
   TTRES_1,
   TTRES_500,
   TTRES_100,
   MAX_TTRES
};

enum
{
   FILTER_LABEL_SDI = 0,
   FILTER_PARITY_ERR,
   FILTER_STALE_MSG,
   MAX_FILTER
};

/* This function enables or disables a receiver. */
S16BIT DDCAPI EnableRx(S16BIT Card, S16BIT Receiver, S16BIT Enable);

/* This function returns a receiver group's mode of operation. */
S16BIT DDCAPI GetRxMode(S16BIT Card, S16BIT Group);

/* This function returns the mode of a receiver. */
S16BIT DDCAPI GetRxChannelMode(S16BIT Card, S16BIT Receiver);

/* This function sets a receiver group's mode of operation (Mailbox / FIFO) */
S16BIT DDCAPI SetRxMode(S16BIT Card, S16BIT Group, S16BIT Mode);

/* This function sets the mode for a receiver. */
S16BIT DDCAPI SetRxChannelMode(S16BIT Card, S16BIT Receiver, S16BIT Mode);

/* This function returns a receiver group's parity. */
S16BIT DDCAPI GetRxParity(S16BIT Card, S16BIT Group);

/* This function returns the parity setting of a receiver. */
S16BIT DDCAPI GetRxChannelParity(S16BIT Card, S16BIT Receiver);

/* This function sets a receiver group's parity. */
S16BIT DDCAPI SetRxParity(S16BIT Card, S16BIT Group, S16BIT Parity);

/* This function sets the parity setting of a receiver. */
S16BIT DDCAPI SetRxChannelParity(S16BIT Card, S16BIT Receiver, S16BIT Parity);

/* This function returns a receiver group's speed. */
S16BIT DDCAPI GetRxSpeed(S16BIT Card, S16BIT Group);

/* This function returns the speed of a receiver. */
S16BIT DDCAPI GetRxChannelSpeed(S16BIT Card, S16BIT Receiver);

/* This function sets a receiver groups's speed. */
S16BIT DDCAPI SetRxSpeed(S16BIT Card, S16BIT Group, S16BIT Speed);

/* This function sets the speed of a receiver. */
S16BIT DDCAPI SetRxChannelSpeed(S16BIT Card, S16BIT Receiver, S16BIT Speed);

/* This function returns a receiver's enabled status. */
S16BIT DDCAPI GetRxStatus(S16BIT Card, S16BIT Receiver);

/* This function enables or disables a receiver's time stamp facility. */
S16BIT DDCAPI EnableTimeStamp(S16BIT Card, S16BIT Receiver, S16BIT Enable);

/* This function configures a card's time stamp feature. */
S16BIT DDCAPI ConfigTimeStamp(S16BIT Card, U8BIT Format, U8BIT Rollover,
                              U8BIT Resolution);

/* This function returns the card's time stamp configuration. */
S16BIT DDCAPI GetTimeStampConfig(S16BIT Card, U8BIT *Format, U8BIT *Rollover,
                                 U8BIT *Resolution);

/* This function returns the receiver's time stamp status. */
S16BIT DDCAPI GetTimeStampStatus(S16BIT Card, S16BIT Receiver);

/* This function returns the 48 bit Time Tag value. */
S16BIT DDCAPI GetTimeStamp(S16BIT Card,U64BIT *ullTTValue);

/* This function resets the timer used for time stamping. */
S16BIT DDCAPI ResetTimeStamp(S16BIT Card);

/* This function clears all slots in a receiver's mailbox. */
S16BIT DDCAPI ClearMailbox(S16BIT Card, S16BIT Receiver);

/* This function returns an array of a receiver's mailbox slots containing new messages. */
S16BIT DDCAPI GetMailbox(S16BIT Card, S16BIT Receiver, S16BIT N, U16BIT *LabelSdi);

/* This function indicates whether a new word has been registered in a receiver's mailbox slot. */
S16BIT DDCAPI GetMailboxStatus(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi);

/* This function reads the latest word received of a specified Label/SDI with 48BIT/IRIG time stamp. */
S16BIT DDCAPI ReadMailboxIrig(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi,
                              U32BIT *Data, U32BIT *StampHi, U32BIT *StampLo);

/* This function clears a receiver's queue. */
S16BIT DDCAPI ClearRxQueue(S16BIT Card, S16BIT Receiver);

/* This function returns a receiver queue's status. */
S16BIT DDCAPI GetRxQueueStatus(S16BIT Card, S16BIT Receiver);

/* This function reads one 32-bit word from the receiver's queue with 48BIT/IRIG time stamp. */
S16BIT DDCAPI ReadRxQueueIrigOne(S16BIT Card, S16BIT Receiver, U32BIT *Data,
                                 U32BIT *StampHi, U32BIT *StampLo);

/* This function reads an array of ARINC words from a receiver's queue with 48BIT/IRIG time stamp. */
S16BIT DDCAPI ReadRxQueueIrigMore(S16BIT Card, S16BIT Receiver, S16BIT N,
                                  U32BIT* Data, U32BIT *StampHi, U32BIT *StampLo);

/* This function enables or disables a channels filtering capibility. */
S16BIT DDCAPI EnableFilter(S16BIT Card, S16BIT Receiver, S16BIT Enable);

/* This function configures the receiver filter mode of operation */
S16BIT DDCAPI ConfigFilter(S16BIT Card, S16BIT Receiver, S16BIT Mode);

/* This function adds a Label/SDI filter to the specified receiver. */
S16BIT DDCAPI AddFilter(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi);

/* This function deletes all filters for the specified receiver. */
S16BIT DDCAPI ClearFilter(S16BIT Card, S16BIT Receiver);

/* This function deletes one filter for the specified receiver. */
S16BIT DDCAPI DelFilter(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi);

/* This function returns whether a particular filter exists. */
S16BIT DDCAPI GetFilter(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi);

/* This function returns an array of enabled filters for the specified receiver. */
S16BIT DDCAPI GetAllFilter(S16BIT Card, S16BIT Receiver, S16BIT *LabelSdi);

/* This function returns the status of the existing filters in Rx queue. */
S16BIT DDCAPI GetFilterStatus(S16BIT Card, S16BIT Receiver);

/* This function returns the number of existing filters for the specified receiver. */
S16BIT DDCAPI GetNumOfFilter(S16BIT Card, S16BIT Receiver);

#endif /* __RECEIVE_H__ */

