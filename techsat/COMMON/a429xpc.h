
/*
 *
 * $Header: /CPSoftware/Utilities/mcdusim/techsat/COMMON/a429xpc.h,v 1.1 2009/11/20 13:39:01 ems-tcanada\varanda.m Exp $
 *
 */

/*
 * @COMPONENT   xpc
 * @FILE        $Source: /CPSoftware/Utilities/mcdusim/techsat/COMMON/a429xpc.h,v $
 * @DATE        $Date: 2009/11/20 13:39:01 $
 * @REVISION    $Revision: 1.1 $
 * @AUTHOR      TechS.A.T: Dieter Hotz
 * @COPYRIGHT   -----------------------------------------------------
 *              Copyright (c) 2002 TechSAT GmbH
 *		All rights reserved.
 *		-----------------------------------------------------
 *		Technische Systeme fuer Avionik und Test
 *		Gruber Strasse 46c
 *              D-85586 Poing
 *              Tel. +49-8121-703-0
 *              Fax  +49-8121-703-177
 *              -----------------------------------------------------
 * @CONTENT     Defintions common to all board types
 *      
 *
 *
 */

#ifndef _A429XPC_H
#define _A429XPC_H

#ifdef __cplusplus
extern "C" {    
#endif

/*
 * Platform information
 *
 * Software and hardware platforms are selected via defines
 *
 * Software defines
 *
 * WIN95	Windows 95
 * CAS95	Windows 95/98/NT platform for PCC
 * WIN9x	Windows 95 and Windows 98
 * WIN32	Windows 95/98/NT/W2K/WXP
 * LINUX	RedHat LINUX
 * 
 * Compiler define
 * PLATFORM_X86_MINGW32
 *		Compiled using MINGW32, otherwise MSVC6
 *
 * Hardware defines
 *
 * PCC		TechSAT PCMCIA card
 * USB		TechSAT USB card
 * EPC		TechSAT ISAbus 4-slot IP carrier with TechSAT IP429
 * HPVISA	HP 4-slot VxI IP carrier with TechSAT IP429 (NT-only)
 * PC104	Greenspring Flex/104 PC104 2-slot IP carrier with TechSAT IP429
 * IPC01	Microsys 4-slot VME IP carrier with TechSAT IP429
 * PCI40	Greenspring PCI40 carrier with TechSAT IP429
 * PCI60	Greenspring PCI60 carrier with TechSAT IP429
 * PCI2G	2- or 4-slot TechSAT PCI2G carrier with TechSAT IP429
 * IP429	Any IP carrier using the TechSAT IP429
 */


#if defined (WIN9x)
#   if defined (PCI40) || defined (PCI60) || defined (PCI2G)
#	define HANDLE unsigned long
#	define INVALID_HANDLE_VALUE   0
#   else
#	define HANDLE unsigned short
#   endif
#    define UCHAR  unsigned char
#    define ULONG  unsigned long
#    define USHORT unsigned short
#    define PVOID  void *
#    define BOOL   int
/*
 * Following just allows the W9X version to compile, it's not really correct
 */
#    define LONG64   int
#    if !defined (WIN32)
#		define INVALID_HANDLE_VALUE   99
#    endif
#    if defined (WIN9x)
#       if defined (XPASCAL)
#	    define FEXTHANDLE  extern HANDLE pascal
#	    define FEXTINT     extern int pascal
#	    define FHANDLE     HANDLE pascal
#	    define FINT        int pascal
#       else
#           define FEXTHANDLE  extern HANDLE
#           define FEXTINT     extern int
#           define FHANDLE     HANDLE
#           define FINT        int
#       endif
#    else
#        define FEXTHANDLE  extern HANDLE _far _pascal _export
#        define FEXTINT     extern int _far _pascal _export
#        define FHANDLE     HANDLE _far _pascal _export
#        define FINT        int _far _pascal _export
#    endif
#endif
#if defined (WIN32) && !defined (WIN9x)
#   if defined (XPASCAL)
#	define FEXTHANDLE  extern HANDLE pascal
#	define FEXTINT     extern int pascal
#	define FHANDLE     HANDLE pascal
#	define FINT        int pascal
#   elif defined (VBASIC)
#	define FEXTHANDLE  extern HANDLE __stdcall
#	define FEXTINT     extern int __stdcall
#	define FHANDLE     HANDLE __stdcall
#	define FINT        int __stdcall
#   else
#	define FEXTHANDLE  extern HANDLE
#	define FEXTINT     extern int
#	define FHANDLE     HANDLE
#	define FINT        int
#   endif
#   if defined (PLATFORM_X86_MINGW32)
#	define LONG64	   long long
#   else
#	define LONG64	   _int64
#   endif
#endif
#if defined (LINUX)
#   define HANDLE	void *
#   define UCHAR	unsigned char
#   define ULONG	unsigned long
#   define USHORT	unsigned short
#   define PVOID	void *
#   define BOOL 	int
#   define FEXTHANDLE	extern HANDLE
#   define FEXTINT	extern int
#   define FHANDLE	HANDLE
#   define FINT 	int
#   define LONG64	long long

#   define INVALID_HANDLE_VALUE 0
#endif

/*
 * iflib/dll version
 */
#include "xpc_vers.h"

/*
 * function results
 */

#define XPC_SUCCESS             0
#define XPC_BAD_EPROM_TYPE      0xf0
#define XPC_BAD_EPROM_SECTOR    0xf1
#define XPC_BAD_BOOT_MODE       0xf2
#define XPC_EPROM_VERIFY_ERROR  0xf3
#define XPC_EPROM_PROG_ERROR    0xf4
#define XPC_EPROM_ERASE_ODD     0xf5
#define XPC_EPROM_ERASE_EVEN    0xf6
#define XPC_EPROM_ERASE_BOTH    0xf7
#define XPC_PCC_INIT		0xf8
#define XPC_CANPC_RESET		0xf9
#define XPC_INIPC_INITBOARD	0xfa
#define XPC_BUFFER_TOO_SMALL    0xfb
#define XPC_UNDEFINED_ERROR     0xfc
#define XPC_IPM_TIMEDOUT        0xfd
#define XPC_NOT_IMPLEMENTED     0xfe
#define XPC_FAILED              0xff

/*
 * Error codes
 */

#define XPC_ERROR_HANDLE        1
#define XPC_ERROR_RX_NUMBER     2
#define XPC_ERROR_ACTIONCODE    3

#define XPC_ERROR_TX_NUMBER     2
#define XPC_ERROR_ACTCOUNT      3

#define XPC_ERROR_SPEEDCODE     3

#define XPC_ERROR_SPEEDSEL      3
#define XPC_ERROR_BITRATE       4
#define XPC_ERROR_PARITY        5
#define XPC_ERROR_SLOPE         6

#define XPC_ERROR_DEFCOUNT      3

#define XPC_ERROR_DELCOUNT      3

#define XPC_ERROR_TRIGCOUNT     3

#define XPC_ERROR_LOOPCODE      2

#define XPC_ERROR_TX_ID         3

#define XPC_ERROR_TRIG_ID       2
#define XPC_ERROR_TRIG_FLAG     3
#define XPC_ERROR_SIGNAL_EVENT  3
#define XPC_ERROR_TRIG_ACTCODE  4
#define XPC_ERROR_TIMER_ACTCODE 5

#define XPC_ERROR_ACTCODE       3

#define XPC_ERROR_IPNUM         2
#define XPC_ERROR_CHIPNUM       2
#define XPC_ERROR_SLOTEMPTY     3
#define XPC_ERROR_BYTECOUNT     4
#define XPC_ERROR_WORDCOUNT     4

#define XPC_NO_ID_DEFINED       5

#define XPC_ERROR_SYNCHRO       2
#define XPC_ERROR_STATE         2
#define XPC_ERROR_OUTPCC_STATE  4

#define XPC_ERROR_FILE          3

#define XPC_ERROR_TSRES0        3
#define XPC_ERROR_TSRES1        4
#define XPC_ERROR_TSRES2        5
#define XPC_ERROR_TSRES3        6

#define XPC_ERROR_MCOUNT        5
#define XPC_ERROR_DISCARD       6


/*
 * serial communications definitions
 *
 * OR these flags together to get
 * what you want
 *
 * The reload values for baudrate
 * generation given herein are only
 * the values for standard baudrates
 * to use different baudrates you've
 * to calculate the appropriate values.
 * This also applies if your'e using
 * synchronous mode of operation.
 */

#define SER_8BIT_SYNC           0x0000
#define SER_8BIT_ASYNC          0x0001
#define SER_7BIT_PAR_ASYNC      0x0003
#define SER_9BIT_ASYNC          0x0004
#define SER_8BIT_WAKEUP_ASYNC   0x0005
#define SER_8BIT_PAR_ASYNC      0x0007

#define SER_ONE_STOPBIT         0x0000
#define SER_TWO_STOPBITS        0x0008
#define SER_RX_DISABLED         0x0000
#define SER_RX_ENABLED          0x0010
#define SER_IGNORE_PARITY       0x0000
#define SER_CHECK_PARITY        0x0020
#define SER_IGNORE_FRAMING      0x0000
#define SER_CHECK_FRAMING       0x0040
#define SER_IGNORE_OVERRUN      0x0000
#define SER_CHECK_OVERRUN       0x0080
#define SER_EVEN_PARITY         0x0000
#define SER_ODD_PARITY          0x1000
#define SER_STD_RXTX            0x0000
#define SER_LOOPBACK_RXTX       0x4000

#define SER_625_KBAUD           0x0000
#define SER_19200_BAUD          0x001f
#define SER_9600_BAUD           0x0040
#define SER_4800_BAUD           0x0081
#define SER_2400_BAUD           0x0103
#define SER_1200_BAUD           0x0207
#define SER_600_BAUD            0x0410
#define SER_75_BAUD             0x1fff


/*
 * Type definitions
 */


typedef struct {
   unsigned char  transmission_ID;
   unsigned char  trigger_ID; 
   unsigned short data_low;
   unsigned short data_high;
   unsigned short transmit_interval_millis;
   unsigned short transmit_interval_timesl;
   unsigned short ticks_til_transmission;
   unsigned short transmission_flags;
   unsigned short spare_word;
} read_tx_definition_t;



typedef struct {
   unsigned short bitrate_par_slope_set;
   unsigned short amplitude_set;
} tx_set_t;


typedef struct {
   unsigned short transmission_ID;
   unsigned short data_low;
   unsigned short data_high;
   unsigned short transmit_interval;
} tx_definition_t;


typedef struct {
   unsigned char transmission_ID;
   unsigned char trigger_ID;
} tx_activation_t;


typedef struct {
   unsigned short transmission_ID;
} tx_delete_t;


typedef struct {
   unsigned short transmission_ID;
   unsigned short data_low;
   unsigned short data_high;
   unsigned short dontcare;
} tx_update_t;

		     
typedef struct {
   unsigned short mask_low;
   unsigned short mask_high;
   unsigned short match_low;
   unsigned short match_high;
} rx_mode_t;


typedef struct {
   unsigned long  receive_time;
   unsigned short arinc_low;
   unsigned short arinc_high;
} rx_data_t;

	       
typedef struct {
   unsigned char trigger_ID;
   unsigned char act_code;
   unsigned short trigger_mask_low;
   unsigned short trigger_mask_high;
   unsigned short trigger_match_low;
   unsigned short trigger_match_high;
} trig_definition_t;

#pragma pack(2)
/*
 * The structures between this and the following pragma are those
 * that have
 * 1. mixture of char and/or short and long elements
 * 2. a total length that is a mutiple of 2 but not a multiple of 4
 */
typedef struct {
   unsigned short transmission_ID;
   unsigned short transmit_interval;
   unsigned long* label_ptr;
   unsigned short num_of_labels;
   unsigned short update_interval;      /*transmit function update interval*/
   unsigned short function_flag;        /*hold, wrap or reverse*/
} tx_function_t;

#define _HOLD 		0
#define _WRAP 		1
#define _REVERSE 	2

typedef struct {
   unsigned long factor;   /* Not used for OP_NOT and OP_DELETE */
   unsigned short label_sdi;
   unsigned short label_sdi_mask;
   unsigned short sublabel;
   char sublabel_msb; /* Zero if no sublabel */
   char sublabel_lsb;
   char param_msb;   /* Parameter MSB */
   char param_lsb;   /* Parameter LSB */
   char param_type;  /* One of the DATA_ defines */
   char operat;      /* One of the OP_ defines */
   char sb_bnr;      /* Sign bit pos BNR */
   unsigned char rep_label;   /* Replacement label */
   union lsr
   {
       struct bcd_data 
       {
	   char ldv;    /* Low digit value BCD */
	   char hdv;    /* High digit value BCD */
	   short ls_res; /* LSD res for BCD */
       } bcd;
       float bnr_ls_res; /* LSD res for BNR */
    } res_data;
   /* Total 22 bytes */
} manipulation_t;
#pragma pack()

/* Data replay structure */
typedef struct {
   unsigned long arinc_word;
   unsigned long timestamp;
} replay_t;

#define DR_BUFFSIZE     16384           /* size of replay buffer */

typedef struct {
   unsigned short mode;                 /* mode of operation see below */
   unsigned short error;                /* error code see below */
   unsigned long  lostct;               /* count of untransmitted data */
   unsigned long  lostdata;             /* first ARINC dataword not xmitted */
   unsigned long  immediatect;          /* count of immediately xmitted data */
   unsigned long  deferedct;            /* count of defered transmissions */
} tx_status_t;

/* mode values in tx_status_t */
#define TXMDIDLE        0       /* transmitter is idle */
#define TXMDTRANS       1       /* transmitter is transmitting normally */
#define TXMDMANIP       2       /* transmitter is performing data manipulation*/
#define TXMDREPLAY      3       /* transmitter is replaying data */
/* error values in tx_status_t */
#define TXEOK           0       /* no error */
#define TXEDATALOST     1       /* xmitter cannot transmit data at req. rate */
#define TXENODRDATA     2       /* xmitter ran out of data during data replay*/

typedef struct {
    unsigned char data_frame;
    unsigned char stop_bits;
    unsigned char enable_receiver;
    unsigned char parity_check_enable;
    unsigned char framing_check_enable;
    unsigned char overrun_check_enable;
    unsigned char parity;
    unsigned char loopback;
    unsigned short baudrate_reload_value; /* used to be, incorrectly, char */
} serial_cfg_t;

typedef struct {
   unsigned short serial_data;
   unsigned short err_status;
   unsigned long receive_time;
} serial_data_t;
 
/* 
 * Data structure returned by firmware function GET_CARD_IDENT
 * This structure is the same for TechSAT A429 cards of type PCMCIA and EPC-IP. 
 * The design concepts and the meaning of some of the fields are long lost 
 * in the mists of antiquity, but we still use this structure for backward 
 * compatibility. 
 * Note that this structure reflects what is returned by firmware function 
 * GET_CARD_IDENT, NOT the layout in the EEPROM. 
 */ 
typedef struct
{ 
    unsigned short	Magic; 
    unsigned short	BuildGroup; 
    unsigned long	WriteCycle; 
    unsigned long	SerialNumber; 
    unsigned char	HwRevMajor; 
    unsigned char	HwRevMinor; 
    unsigned char	IdentData[18];		/* NOTE: not a C-string ! */ 
#define _UserLengthOffset 32
#define _UserLengthOffsetROM 74
    unsigned short	UserLength; 
#define _CompanyOffset  34
    unsigned char	CompanyName[20];	/* NOTE: not a C-string ! */ 
    unsigned char	CompanySerialNumber[9];	/* NOTE: not a C-string ! */ 
    unsigned char	CompareByte; 
    /* Now the Hardware config */ 
    unsigned char	TxConfig[4];		/* One BIT per available TX */ 
    unsigned char	RxConfig[4];		/* One BIT per available RX */ 
    unsigned char	RS422Config; 
    unsigned char	RS232Config; 
    unsigned char	I2CConfig; 
    unsigned char	DiscInConfig; 
    unsigned char	DiscOutConfig; 
    unsigned char	AdapBoxConfig; 
    unsigned char	RfuConfig[5]; 
    unsigned char	ChecksumConfig;		/* Checksum of the hardware config */ 
} CardIdent; 
/* Definition for HW configuration data */
typedef struct {
  unsigned char           ATx_00_07;
  unsigned char           ATx_08_15;
  unsigned char           ATx_16_23;
  unsigned char           ATx_24_31;
  unsigned char           ARx_00_07;
  unsigned char           ARx_08_15;
  unsigned char           ARx_16_23;
  unsigned char           ARx_24_31;
  unsigned char           RS422_00_07;
  unsigned char           RS232_00_07;
  unsigned char           I2C_00_07;
  unsigned char           DigIN_00_07;
  unsigned char           DigOUT_00_07;
  unsigned char           AdapBox_00_07;
  unsigned char           spare2;
  unsigned char           spare3;
  unsigned char           spare4;
  unsigned char           spare5;
  unsigned char           spare6;
  unsigned char           spare7;
} HWconfig;
/*
 * Data structures for the Selftest diagnostic info
 */
typedef struct _selftest_data
{
	unsigned short		error_bits;
	unsigned short		word_number;
	unsigned long		expected_data;
	unsigned long		actual_data;
} ST_DATA;
typedef struct _all_st_data
{
	ST_DATA				dpram;		/* DPRAM buffer */
	ST_DATA				txbuff;		/* Transfer buffer */
	ST_DATA				transA;		/* ARINC Transceiver A */
	ST_DATA				transB;		/* ARINC Transceiver B */
	ST_DATA				loopA0;		/* Loop TxA -> Rx0 */
	ST_DATA				loopB1;		/* Loop TxB -> Rx1 */
	ST_DATA				loopA2;		/* Loop TxA -> Rx2 */
	ST_DATA				loopB3;		/* Loop TxB -> Rx3 */
} SelfTestData;
#define ST_TEST_RUN		0x0001
#define ST_HOLT_RESET	0x0002
#define ST_RXNODATA		0x0004
#define ST_RXBADDATA	0x0008
#define ST_RXIBADDATA	0x0010

/*
 * Definitions for the protocol extensions
 */

typedef struct {
		unsigned short	ipnum; /* Not used for PCC */
		unsigned short	txnum;
		unsigned short	rxnum;
		unsigned short	rxlnum;
		unsigned short	jobid;
		} protocol_job_t;
typedef protocol_job_t *PROJOB;

typedef unsigned long ARINC_WORD;

typedef unsigned long ARINC_TIMESTAMP;

typedef struct {
		ARINC_WORD		arinc_word;
		ARINC_TIMESTAMP	timestamp;
		} response_t;

typedef struct {
		unsigned long	response_mask;
		unsigned long	response_match;
			/* The response word is the first word RXd
			   that satisfies the condition
			   ARINC word AND response_mask = response_match
			*/
		unsigned long	response_timeout;
			/* 0 = don't wait for response,
			   n = response timestamp [msecs] */
		} response_parameters_t;

#define PRO_BLOCK_MAX 2048
typedef struct {
		unsigned short	block_length;
			/* Length in ARINC words of transmit block
			   (Maximum 2048) */
		unsigned short	word_gap;
			/* gap-time [msec] between block word transmits
			   (0 = minimum = 4 bit-times) */
		ARINC_WORD		*block;
			/* Pointer to the transmit block ARINC words */
		unsigned short	block_control;
			/* 0 = stop cyclic transmissions,
			   1 = mix with cyclic transmissions */
		} tx_block_t;

typedef struct {
		unsigned char	type;
			/* 0 = end determined by length alone
			   1 = end determined by end_mask & match alone
			   2 = combination of above, which ever comes first
			*/
		unsigned char	label;
			/* ARINC 8-bit label used for block data */
		unsigned short	maximum_length;
			/* Length in ARINC words of receive block
			   (Maximum 2048) */
		unsigned long	start_mask;
		unsigned long	start_match;
		unsigned long	end_mask;
		unsigned long	end_match;
		unsigned long	timeout;
			/* 0 = don't wait for block,
			   n = block timeout [msecs] */
		} rx_block_t;
typedef enum _prostat {
		PRO_IDLE,		/* Nothing going on */
		PRO_CMD,		/* Sending a command */
		PRO_CMD_RESP,	/* Sending a command,
						   with an expected response */
		PRO_CMD_BLOCK,	/* Sending a command
						   with an expected RX block response */
		PRO_RX_RESP,	/* Receiving response */
		PRO_TX_BLOCK,	/* Sending block */
		PRO_TX_BLOCK_RESP,/* Sending block with
						   an expected response */
		PRO_RX_BLOCK_ST,/* Waiting for RX block start */
		PRO_RX_BLOCK 	/* Receiving block */
		} PROSTATUS;

/* Data available flags */
#define PRO_DATA_CMD_TS	0x0001
#define PRO_DATA_TX_BLK 0x0002
#define PRO_DATA_RESP  	0x0004
#define PRO_DATA_RX_BLK 0x0008
#define PRO_TIMEOUT	0x0010

/*
 * Function prototypes
 */

#if defined (IPC01) && defined (WIN95)
FEXTHANDLE _open_xpc(unsigned long base16,
		     unsigned long base32,
		     long ipmemsize,
		     int ivec,
		     int ilevel);
#elif defined (HPVISA) && defined (WIN32)
FEXTHANDLE _open_xpc(HANDLE ohdl,
		     unsigned short vxibus,
		     unsigned short vxiaddr);
#else
FEXTHANDLE _open_xpc(short lbn);
#endif
FEXTINT    _close_xpc(HANDLE hdl);

FEXTINT	   _verify_iflib_version( unsigned short vnum );

FEXTINT    _config_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short speed,
			 unsigned short bitrate,
			 unsigned short parity);

FEXTINT    _get_transmitter_set(HANDLE  hdl, 
				char    tx_number, 
				tx_set_t* buffer);

FEXTINT    _lv_get_transmitter_set(HANDLE hdl, char tx_number, 
				   unsigned short *bitrate_par_slope_set,
				   unsigned short *amplitude_set);

FEXTINT    _define_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short def_count,
			 tx_definition_t* buffer, 
			 unsigned short* result,
			 unsigned short* timeslice);

FEXTINT    _lv_define_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short def_count,
			 unsigned short *tr_id,
			 unsigned short *dlow,
			 unsigned short *dhigh,
			 unsigned short *ti,
			 unsigned short* result,
			 unsigned short* timeslice);

FEXTINT    _get_single_tx_definition(HANDLE hdl, 
				     char tx_number, 
				     unsigned short tx_id, 
				     read_tx_definition_t* buffer,
				     unsigned short* result); 

FEXTINT    _lv_get_single_tx_definition(HANDLE hdl, 
			 char tx_number, 
			 unsigned short tr_id,
			 unsigned char *tg_ID,
			 unsigned short *dlow,
			 unsigned short *dhigh,
			 unsigned short *tims,
			 unsigned short *tits,
			 unsigned short *ticks,
			 unsigned short *tflags,
			 unsigned short *result);

FEXTINT    _get_tx_definitions(HANDLE hdl, 
			       char tx_number, 
			       read_tx_definition_t* buffer,
			       unsigned short* result,
			       unsigned short* num_def); 

FEXTINT    _lv_get_tx_definitions(HANDLE hdl, 
			 char tx_number, 
			 unsigned char *tr_ID,
			 unsigned char *tg_ID,
			 unsigned short *dlow,
			 unsigned short *dhigh,
			 unsigned short *tims,
			 unsigned short *tits,
			 unsigned short *ticks,
			 unsigned short *tflags,
			 unsigned short* result,
			 unsigned short* num_Def);

FEXTINT    _delete_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short del_count, 
			 tx_delete_t* buffer, 
			 unsigned short* result, 
			 unsigned short* calc_time);

FEXTINT    _lv_delete_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short del_count, 
			 unsigned short* tr_id, 
			 unsigned short* result, 
			 unsigned short* calc_time);

FEXTINT    _activate_trans(HANDLE hdl, 
			   char tx_number, 
			   unsigned short act_count, 
			   tx_activation_t* buffer);

FEXTINT    _lv_activate_trans(HANDLE hdl, 
			   char tx_number, 
			   unsigned short act_count, 
			   unsigned char *tr_id, 
			   unsigned char *tg_id);

FEXTINT    _update_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short upd_count, 
			 tx_update_t* buffer);

FEXTINT    _lv_update_trans(HANDLE hdl, 
			 char tx_number, 
			 unsigned short upd_count, 
			 unsigned short *tr_id,
			 unsigned short *dlow,
			 unsigned short *dhigh);

FEXTINT    _set_tx_mode(HANDLE hdl, 
			char tx_number, 
			unsigned short mode);

FEXTINT    _config_rec(HANDLE hdl, 
		       char rx_number, 
		       unsigned short speed);

FEXTINT    _set_rec_buf_size(HANDLE hdl, 
			     char rx_number, 
			     unsigned short size,
			     unsigned short* result, 
			     unsigned short* status);

FEXTINT    _activate_rec(HANDLE hdl, 
			 char rx_number, 
			 unsigned short act_code,
			 rx_mode_t* buffer, 
			 unsigned short* status);

FEXTINT    _lv_activate_rec(HANDLE hdl, 
			 char rx_number, 
			 unsigned short act_code,
			 unsigned short mask_low,
			 unsigned short mask_high,
			 unsigned short match_low,
			 unsigned short match_high,
			 unsigned short* status);

FEXTINT    _get_receiver_mode(HANDLE hdl, 
			      char rx_number, 
			      unsigned short* rec_mode);

FEXTINT    _read_receiver_buffer(HANDLE hdl, 
				 char rx_number, 
				 rx_data_t* buffer, 
				 unsigned short* result,
				 unsigned short* num_entry);

FEXTINT    _lv_read_receiver_buffer(HANDLE hdl, 
				 char rx_number, 
				 unsigned long *rtime,
				 unsigned short *alow,
				 unsigned short *ahigh,
				 unsigned short* result,
				 unsigned short* num_entry);

FEXTINT    _define_trigger(HANDLE hdl, 
			   char rx_number, 
			   unsigned short trig_count, 
			   trig_definition_t* buffer, 
			   unsigned short* status);

FEXTINT    _lv_define_trigger(HANDLE hdl, 
			   char rx_number, 
			   unsigned short trig_count, 
			   unsigned char *tg_id,
			   unsigned char *acode,
			   unsigned short *mask_low,
			   unsigned short *mask_high,
			   unsigned short *match_low,
			   unsigned short *match_high,
			   unsigned short* status);

FEXTINT    _establish_loop(HANDLE hdl, unsigned short loop_code);

FEXTINT    _get_tx_timeslice(HANDLE hdl,
			     char tx_number, 
			     unsigned short* timeslice);

FEXTINT    _get_arinc_board_type(HANDLE hdl, unsigned short ipnum,
		unsigned short* board_type);
				    
FEXTINT    _get_rec_buf_count(HANDLE hdl,
			      char rx,
			      unsigned short *nbuf,
			      unsigned short *bsize);

FEXTINT    _set_timestamp_synchro_mode(HANDLE hdl,
			      short ipm,
			      unsigned short mode,
			      unsigned short *status);

FEXTINT    _set_timestamp_res(HANDLE hdl,
			      short ipm,
			      unsigned char r0, unsigned char r1,
			      unsigned char r2, unsigned char r3);
FEXTINT    _get_timestamp_res(HANDLE hdl,
			      short ipm,
			      unsigned char *r0, unsigned char *r1,
			      unsigned char *r2, unsigned char *r3);
#if defined (HPVISA) || defined (PCC) || defined (USB)
FEXTINT    _set_out_state(HANDLE hdl, unsigned short state);
FEXTINT    _get_in_state(HANDLE hdl);
#else
FEXTINT    _set_out_state(HANDLE hdl, short ipnum, unsigned short state);
FEXTINT    _get_in_state(HANDLE hdl, short ipnum);
#endif

FEXTINT    _get_fw_version(HANDLE hdl, short ipnum, unsigned short* vnum);

FEXTINT	   _get_data_ready(HANDLE hdl, char rx );

FEXTINT    _get_status_word(HANDLE hdl, short ipnum, unsigned long* stw);

FEXTINT    _get_ticks(HANDLE hdl, short ipnum, unsigned long* t);

FEXTINT    _download_data_replay(HANDLE hdl, char tr, replay_t* pbuff, 
	      unsigned short rcount, unsigned short* result);

FEXTINT    _activate_data_replay(HANDLE hdl, char tr, unsigned short act_code,
		unsigned short *status);

FEXTINT    _define_data_replay(HANDLE hdl, char tr, unsigned short *result);

FEXTINT    _define_trans_func(HANDLE hdl, char tr,
                  unsigned short dcount, tx_function_t *tfunc,
		  unsigned short* result, unsigned short *tslice);

FEXTINT    _define_manipulation(HANDLE hdl, char tr,
                  unsigned short rx_flag, manipulation_t *manip,
                  unsigned short mcount, unsigned short dflag,
		  unsigned short* result);

FEXTINT    _activate_manipulation(HANDLE hdl, char tr, unsigned short act_code,
		unsigned short *status);

FEXTINT    _get_manipulation(HANDLE hdl, char tr, manipulation_t* pbuff, 
		  unsigned short *mcount);

FEXTINT    _get_ext_ticks(HANDLE hdl, short ipnum, 
		unsigned long *elapsed_ms, unsigned long *elapsed_100ms);

FEXTINT    _reset_clock(HANDLE hdl, short ipnum );

FEXTINT    _set_trans_wave(HANDLE hdl, char tr, unsigned int slope);

#if defined(PCC) || defined (USB)
FEXTINT _set_trigger_state(HANDLE hdl, unsigned short trig,
	       unsigned short trig_flag, unsigned short out_flag);
#elif defined(EPC)
FEXTINT _set_trigger_state(HANDLE hdl, short ipm, unsigned short trig,
	       unsigned short flag);
#endif

FEXTINT    _set_high_resolution(HANDLE hdl, short ipnum, short resolution);

FEXTINT    _activate_tx_trigger_mode(HANDLE hdl, char tr,
		unsigned short act_code );

FEXTINT    _activate_rx_trigger_mode(HANDLE hdl, char rx,
		unsigned short act_code );

FEXTINT    _get_receiver_data_exist(HANDLE hdl, char rx);

FEXTINT    _get_receiver_data_lost(HANDLE hdl, char rx);

FEXTINT    _reset_data_lost(HANDLE hdl, char rx);

#if defined (PCC) || defined (USB)
FEXTINT     _get_receiver_data_ready(HANDLE hdl, unsigned short* rx0,
		unsigned short* rx1, unsigned short* rx2, unsigned short* rx3);
#else
FEXTINT _get_receiver_data_ready(HANDLE hdl, char rx);
#endif

FEXTINT    _get_tx_status(HANDLE hdl, char tr, tx_status_t* pbuff);

FEXTINT _get_card_ident(HANDLE hdl,
#if ! defined (PCC) && ! defined (USB)
		unsigned short ipnum,
#endif
CardIdent * pbuff);

FEXTINT    _config_input(HANDLE hdl,
#if ! defined (PCC) && ! defined (USB)
		unsigned short ipnum,
#endif
		unsigned short trig_ID, unsigned short signal_event,
		unsigned short trigger_action, unsigned short timer_action);

FEXTINT _get_memory(HANDLE hdl, unsigned short ipnum, unsigned long offset,
	unsigned long *value );
	
FEXTINT _reset_and_selftest(HANDLE hdl, short ipnum, unsigned short *result, 
		    unsigned short *errbits);

FEXTINT _get_selftest_diagnostics(HANDLE hdl, short ipnum, SelfTestData *std );

/* Extensions for RX indexed mode */
FEXTINT _activate_rx_indexed_mode(HANDLE hdl, char rx, unsigned short act_code,
		unsigned short *status);
FEXTINT _read_indexed_data(HANDLE hdl, char rx, unsigned short label,
	unsigned short sdi, unsigned long *arinc_data, unsigned long *timestamp);
FEXTINT _read_all_indexed_data(HANDLE hdl, char rx,
	unsigned long *arinc_data, unsigned long *timestamp);
/*
 * Prototypes for the protocol extensions
 */

FEXTINT _pro_start_job( HANDLE hdl, char tx_number, char rx_number,
			char rx_loop, USHORT tsres, PROJOB *job );

FEXTINT _pro_end_job( HANDLE hdl, PROJOB job );

FEXTINT _pro_get_job_status( HANDLE hdl, PROJOB job, PROSTATUS *status,
			USHORT *data_flags );

FEXTINT _pro_send_command( HANDLE hdl, PROJOB job, ARINC_WORD command );

FEXTINT _pro_expect_response( HANDLE hdl, PROJOB job,
			response_parameters_t *response_params );

FEXTINT _pro_send_command_expect_response( HANDLE hdl, PROJOB job,
			ARINC_WORD command,
			response_parameters_t *response_params );

FEXTINT _pro_send_block( HANDLE hdl, PROJOB job,
			tx_block_t *tx_block_params );

FEXTINT _pro_send_block_expect_response( HANDLE hdl, PROJOB job,
			tx_block_t *tx_block_params,
			response_parameters_t *response_params );

FEXTINT _pro_send_command_expect_block( HANDLE hdl, PROJOB job,
			ARINC_WORD command,
			rx_block_t *rx_block_params );

FEXTINT _pro_expect_block( HANDLE hdl, PROJOB job,
			rx_block_t *rx_block_params );

FEXTINT _pro_get_command_timestamp( HANDLE hdl, PROJOB job,
			ARINC_TIMESTAMP *command_timestamp );

FEXTINT _pro_get_response_data( HANDLE hdl, PROJOB job,
			response_t *response );

FEXTINT _pro_get_tx_block_timestamps( HANDLE hdl, PROJOB job,
			ARINC_TIMESTAMP *block_timestamps, USHORT *count );

FEXTINT _pro_get_rx_block_data( HANDLE hdl, PROJOB job,
			response_t *rx_block_data, USHORT *count );

#ifdef COMPATIBILITY_MODE
/* this section defines the 'old' codes.
 * for new applications, please use
 * only the new definitions above
 */

#define PARA_1_OUT_OF_RANGE 0x01
#define PARA_2_OUT_OF_RANGE 0x02
#define PARA_3_OUT_OF_RANGE 0x03
#define PARA_4_OUT_OF_RANGE 0x04
#define PARA_5_OUT_OF_RANGE 0x05
#define PARA_6_OUT_OF_RANGE 0x06
#define PARA_7_OUT_OF_RANGE 0x07
       
       
#define OK 0x00   
#define NOT_OK 0xFF


#define GET_DEF_TRANS_EB_LAYOUT read_tx_definition_t
#define TRANS_SET               tx_set_t
#define DEF_TRANS_EB_LAYOUT     tx_definition_t
#define ACTIV_TRANS_EB_LAYOUT   tx_activation_t
#define DEL_TRANS_EB_LAYOUT     tx_delete_t
#define UPD_TRANS_EB_LAYOUT     tx_update_t
#define REC_MODE_EB_LAYOUT      rx_mode_t
#define REC_DATA_EB_LAYOUT      rx_data_t
#define DEF_TRIG_EB_LAYOUT      trig_definition_t
#define DEF_TRANS_FUNC_EB_LAYOUT tx_function_t
#define DEF_MANIPULATION_EB_LAYOUT manipulation_t
#define DEF_TX_STATUS           tx_status_t
#define DEF_REPLAY_EB_LAYOUT    replay_t
#define SERIAL_ASYNC_CFG        async_cfg_t

#endif  /* COMPATIBILITY_MODE */


#ifdef __cplusplus
}
#endif


#endif  /*  _A429XPC_H */
