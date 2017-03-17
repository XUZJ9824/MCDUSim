#ifndef __CONFIG_H__
#define __CONFIG_H__
/*****************************************************************
 *
 * Name:        config.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: config.h,v 1.5 2012/01/13 18:28:29 macrae.i Exp $
 */

typedef enum {
    STD_MCDU_APPEARANCE_TYPE    = 1,    // standard 14-line MCDU
    TCP_MCDU_APPEARANCE_TYPE    = 2,    // TCP 10-line MCDU
    LAST_MCDU_APPEARANCE_TYPE   = 2,    // sanity: must be >0 and <=LAST_MCDU_APPEARANCE_TYPE
} cfgAppearanceType_t;
// choose a default appearance for the MCDU
#ifndef DEFAULT_MCDU_APPEARANCE
#define DEFAULT_MCDU_APPEARANCE STD_MCDU_APPEARANCE_TYPE//TCP_MCDU_APPEARANCE_TYPE
#endif

typedef enum {
    CONNECTION_TYPE_UDP     = 0,        // must match DIM simulator HW driver choices
    CONNECTION_TYPE_DDC     = 1,        // must match DIM simulator HW driver choices
    CONNECTION_TYPE_TECHSAT = 2,        // must match DIM simulator HW driver choices
    LAST_HARDWARE_CONNECTION_TYPE = 2,  // maximum legal hardware driver choice
    CONNECTION_TYPE_PIPE    = 91,       // must match DIM simulator
    CONNECTION_TYPE_OLD_DDC = 92,       // maybe obsolete
    CONNECTION_TYPE_TCP     = 93,       // maybe obsolete
} cfgConnectionType_t;

typedef struct {
    long cfgMcdu;
    long cfgMcduAppearance;
    long cfgConnectionType;
    long cfgArincCard;
    long cfgArincGroup;
    long cfgArincParity;
    long cfgArincTxChannel;
    long cfgArincTxSpeed;
    long cfgArincRxChannel;
    long cfgArincRxSpeed;
    long cfgAlreadyConfig;
    char cfgDisplayInterfaceHost[100];
    char cfgDimIpAddress[100];
    long cfgDimBasePort;
    long cfgLocalBasePort;
	long cfgDimInputLabel;
	long cfgDimOutputLabel;
} config_t;

extern __declspec(align(32)) config_t gConfig;

 //---------------------------------------------------------------
 #endif


