/******************************************************************************
 * FILE: errors.h
 *
 * DESCRIPTION:
 *
 * Prototypes for the error display routine and associated constants.
 *
 * API:  GetErrorMsg
 *       PrintErrorMsg
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __ERRORS_H__
#define __ERRORS_H__

#define ERR_SUCCESS             0
#define ERR_IRQ                 -6
#define ERR_NOCRD               -20
#define ERR_INUSE               -29
#define ERR_TIMEOUT             -31
#define ERR_NULL                -95
#define ERR_CRDSERV             -96
#define ERR_INITFAIL            -100
#define ERR_UNKNOWN             -101
#define ERR_CRDINIT             -102
#define ERR_CRDINITFAIL         -103
#define ERR_NORES               -105
#define ERR_FLASH               -106
#define ERR_TXQUEUESZ           -111
#define ERR_RXQUEUESZ           -113
#define ERR_CHNLGROUP           -120
#define ERR_LOOPBACK            -121
#define ERR_BITFORMAT           -122
#define ERR_TX                  -130
#define ERR_PARITY              -131
#define ERR_SPEED               -132
#define ERR_ENABLE              -133
#define ERR_MODE                -134
#define ERR_TIMETAG             -135
#define ERR_TTFORMAT            -136
#define ERR_TTRO                -137
#define ERR_TTRES               -138
#define ERR_FREQ                -140
#define ERR_OFFSET              -141
#define ERR_RX                  -150
#define ERR_RXGROUP             -151
#define ERR_LABELSDI            -160
#define ERR_FILTER              -161
#define ERR_AVIONIC             -169
#define ERR_DISCRETE            -170
#define ERR_DLEVEL              -171
#define ERR_SERIAL              -175
#define ERR_SERCHNL             -176
#define ERR_SERREG              -177
#define ERR_INT_COND            -180
#define ERR_INT_HANDLER         -181
#define ERR_CHAN_TYPE           -185
#define ERR_OVERFLOW            -200
#define ERR_FPGA_REV            -210
#define ERR_FUNCTION            -500

/* Returns an error string associated with a particular error value. */
S16BIT DDCAPI GetErrorMsg(S16BIT ErrorNumber, char ErrorMessage[]);

/* Prints out the error message with the following format:  *** ERROR: <error message> ***  */
void DDCAPI PrintErrorMsg(S16BIT ErrorNumber);

#endif /* __ERRORS_H__ */

