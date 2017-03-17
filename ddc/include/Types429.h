/*********************************************************************/
/*
*
* This is an Windows 95 specific file
*
* Description: PCI429 DLL - Dynamic-link library interface from
*                           Windows 95 to 429 PCI CARD Setup.
*
* Modification History:
*
*  07/21/93    RFC
*  20/Mar/95   MGC - Added the type StampedDiscreteType
*  05/05/95    DFL - Changed to be usable by LabWindows. Enumerations
*                    forced to 16 bits in LW.
*  9/23/99     KWH - Adapted library to DD-42924I5 PCI card.
*  10/11/00    KWH - Added variable frequency data type 'BitRateType'
*  02/06/02    KWH - Added '#pragma pack(2)' directive.
*/
/*********************************************************************/

/*********************************************************************/
/*
 Abstract: ALL global type declarations for the ARINC 429 ToolBox

	      07/21/93 - Bob Chrisman
		    Converted to DLL
*/
/*********************************************************************/
#ifndef _PC_TYPES429
#define _PC_TYPES429
#include "stdwndef.h"
#ifdef __cplusplus
extern "C"
   {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/*********************************************************************/
/********************* CONSTANT DEFINITIONS **************************/
/*********************************************************************/
/* the 429 Toolbox version -- allows conditional compilation         */
/* not used in the Toolbox code - but useful to programmer's working */
/* with multiple versions of the 429 Toolbox                         */
#define _PC_429_VER          1403 /* for 1.4.00 (1.4.00)             */

typedef SHORTINT ResolutionType;

#define TWO_MILLI_SEC         2
#define TEN_MILLI_SEC         10
#define TWENTY_MILLI_SEC      20   /* Max permisible resolution */
#define PC_NULLF ((void (*)()) 0)

			   /* Bit Mask for EnableGlobalCardToPcInt() */
#define R0_INT_ENABLED          0x8001  /* ARINC word received on R0 */
#define R1_INT_ENABLED          0x8002  /* ARINC word received on R1 */
#define R2_INT_ENABLED          0x8004  /* ARINC word received on R2 */
#define R3_INT_ENABLED          0x8008  /* ARINC word received on R3 */

/*********************************************************************/
/******************** ENUMERATION DEFINITIONS ************************/
/*********************************************************************/
// Description:
//  This type enumerates the values used for specifying the sdi for a
//  label.
//
typedef enum {
  SDI_00,
  SDI_01,
  SDI_10,
  SDI_11,
  SDI_NONE
} SDI_Type;

// Description:
//  This type is used to change the mode of the mailbox for allocating
//  slots for mailbox slots.
//
typedef enum {
  MBOX_NORMAL,	/* label is used to determine mailbox slot          */
  MBOX_EXTENDED	/* sdi are and label used to determine mailbox slot */
} Mailbox_Mode_Type;

typedef enum {
  TWO_TX_FOUR_RX_CARD,
  FOUR_TX_FOUR_RX_CARD,
  EIGHT_TX_FOUR_RX_CARD,
  ONE_TX_TWO_RX_CARD,
  TWO_TX_FOUR_RX_HALF_CARD,
  ONE_TX_TWO_RX_HALF_CARD,
  TWO_TX_FOUR_RX_PCI_CARD,
  INVALID_CARD,                  /* invalid CARD  */
  FORCE_TO_2_BYTES3=32767 /* reqd to make it LabWindows compatible. */
} CardType;

typedef enum {
  PC_FALSE,
  PC_TRUE,
  FORCE_TO_2_BYTES4=32767 /* reqd to make it LabWindows compatible. */
} PC_BOOLEAN;

typedef enum { PC_OFF, PC_ON,
  FORCE_TO_2_BYTES5=32767 /* reqd to make it LabWindows compatible. */
} DiscreteValue;

typedef enum {
BITRATE_100_12,
BITRATE_64_8,
BITRATE_80_10,
BITRATE_111_14,
FORCE_TO_2_BYTES13=32767 /* reqd to make it LabWindows compatible. */
} BitRateType;

typedef enum {
  SELF_TEST_LOW_SPEED,
  SELF_TEST_HIGH_SPEED,
  LOW_SPEED_ALL,
  HI_SPEED_ALL,
  LOW_SPEED_R_HI_SPEED_T, /* low speed rec., high speed trans.      */
  HI_SPEED_R_LOW_SPEED_T,  /* high speed rec., low speed trans.     */
  INVALID_SPEED,            /* invalid speed  */
  FORCE_TO_2_BYTES6=32767 /* reqd to make it LabWindows compatible. */
} SpeedType;

typedef enum {
  PCI__EVEN_PARITY,
  PCI__ODD_PARITY,
  PCI__FORCE_TO_2_BYTES7=32767 /* reqd to make it LabWindows compatible. */
} ParityType;

typedef enum {
  R0,    /* receiver  0 */
  R1,    /* receiver  1 */
  R2,    /* receiver  2 */
  R3,    /* receiver  3 */
  T0,    /* transmitter 0 */
  T1,    /* transmitter 1 */
  T2,    /* transmitter 2 */
  T3,    /* transmitter 3 */
  T4,    /* transmitter 4 */
  T5,    /* transmitter 5 */
  T6,    /* transmitter 6 */
  T7,    /* transmitter 7 */
  INVALID_CHANNEL,
  FORCE_TO_2_BYTES8=32767 /* reqd to make it LabWindows compatible. */
} ChannelType;

typedef enum {
  DI0,    /* discrete input 0 */
  DI1,    /* discrete input 1 */
  DI2,    /* discrete input 2 */
  DI3,    /* discrete input 3 */
  DI4,    /* discrete input 4 */
  DO0,    /* discrete ouput 0 */
  DO1,    /* discrete ouput 1 */
  DO2,    /* discrete ouput 2 */
  DO3,    /* discrete ouput 3 */
  DO4,    /* discrete ouput 4 */
  INVALID_DISCRETE,
  FORCE_TO_2_BYTES9=32767 /* reqd to make it LabWindows compatible. */
} DiscreteType;

typedef enum {
  PC_SUCCESS,
  QUEUE_IS_FULL,
  QUEUE_IS_EMPTY,
  SCHEDULED_IS_FULL, /* same as QUEUE_IS_FULL, but applies only to      */
                     /* transmitters whose fifos' flag are set to false */
                     /* (transmitters which are configured as scheduled)*/
  BIN_IS_EMPTY,      /* same as QUEUE_IS_EMPTY, but applies only to     */
                     /* receivers whose FIFOs' flag are set to false    */
                     /* (receivers in which only the most recent data   */
                     /* are kept).                                      */
  BAD_ARGUMENT,      /* function parameter out of range.                */
  INTERNAL_ERROR,
  TIME_OUT,          /* happens iff there are some boards' problems.          */
                     /* Error Handling routine would be called automatically. */
  DATA_WAS_LOST,     /* happens when a receiver's FIFO in DPRAM overflows and */
                     /* data which hasn't been dequeued (by the PC) is over-  */
                     /* written (by the card).                                */
  PC_FAILURE,
  UNSUPPORTED_FEATURE, /* happens when attempting to change bitrate scale on  */
                       /* an older card that does not have this capability.   */
  FORCE_TO_2_BYTES10=32767 /* reqd to make it LabWindows compatible. */
} STATUS;

typedef enum { CHIP_A, CHIP_B,
  FORCE_TO_2_BYTES11=32767 /* reqd to make it LabWindows compatible. */
} ChipType;

typedef enum {
  NO_CHECK,                /* do not run diagnostics                         */
  ROM_FIFO_CHECK,          /* test ROM and FIFOs                             */
  MEMORY_CHECK,            /* checks the dual port and program RAM.          */
                           /* checks for stuck at faults and coupling faults */
  LOOPBACK_CHECK,          /* checks transceiver internally                  */
  SELF_TEST,               /* run ROM_FIFO test and LOOPBACK                 */
  FORCE_TO_2_BYTES12=32767 /* reqd to make it LabWindows compatible.         */
} DiagnosticType;

/* And their result.
 NOTE:
 This type origionally consisted of 'TEST_PASSED', 'TEST_FAILED'
 and 'FORCE_TO_2_BYTES14' only.  Users recompiling existing
 applications containing any 'if(!TEST_FAILED)' type statements,
 in response to a call to the 'RunDiagnostic()' function, against
 this new version of the library, must reevaluate such statements
 and adjust the application(s) accordingly.  'RunDiagnostic()' now
 returns any of the below listed values.  No longer just '0' or '1'.
 'SELF_TEST' returns pass or fail status only.
 For some of the tests, a structure of 'TestResultStructType'
 is updated with an address or sequence number of the first failing
 memory location or sequence, the expected data and the actual data.
 This structure is accessible by the application programmer by processing
 as follows:
    // Declare a pointer to the structure.
    TestResultStructType	*Results;

    // Assign the internal structure address to the pointer.
    Results = GetTestResultStructAddr();

    // Access the Address of the first failing location / sequence.
    ULONG    Add = Results->Address;

    // Access the Expect data of the first failing location / sequence.
    UINT    Exp = Results->Expect;

    // Access the Actual data of the first failing location / sequence.
    UINT    Act = Results->Actual;

 * = Updates the 'TestResultStruct' structure.*/
typedef enum {
  TEST_PASSED,                                // This is the origional configuration
  TEST_FAILED,                                // along with the LabWindows enumerator below.
  READ_CONFIG_FAILED,                         // Error reading card information from the registry.
  STUCK_AT_FAULT_RAM_TEST_FAILED,             // * RAM test failure.
  COUPLING_FAULT_RAM_TEST_FAILED,             // * RAM test failure.
  INITIALIZE_FAILED,                          // Call to 'Initialize()' failed.
  BOOT_CODE_VERSION_READ_FAILED,              // '186 failed to boot.  Couldn't read the bootcode
                                              // version information from the TO-PC FIFO after reset.
  I186_WRITE_PCI_READ_SHARED_RAM_TEST_FAILED, // * Preliminary shared RAM test failed.
  I186_WRITE_PCI_READ_PROGRAM_RAM_TEST_FAILED,// * Preliminary program RAM test failed.
  I186_RAM_TEST_FAILED,	                      // * Bootcode RAM test failed.
  BOOT_CODE_DISCRETES_TEST_FAILED,            // * Bootcode walking '1's discretes loopback test failed.
  STATUS_REGISTER_TEST_FAILED,                // * Status register / FIFO tests failed.
  LOADER_FAILED,   // Failed to load exec onto card.
  R0_OPEN_FAILED,  // 'OpenChannel(Receiver0)' call in internal loopback test failed.
  R1_OPEN_FAILED,  // 'OpenChannel(Receiver1)' call in internal loopback test failed.
  T0_OPEN_FAILED,  // 'OpenChannel(Transmitter0)' call in internal loopback test failed.
  R2_OPEN_FAILED,  // 'OpenChannel(Receiver2)' call in internal loopback test failed.
  R3_OPEN_FAILED,  // 'OpenChannel(Receiver3)' call in internal loopback test failed.
  T1_OPEN_FAILED,  // 'OpenChannel(Transmitter0)' call in internal loopback test failed.
  CHIP_A_LOW_SPEED_INTERNAL_LOOPBACK_FAILED,  // Loopback test failed for chip A @ low speed.
  CHIP_A_HIGH_SPEED_INTERNAL_LOOPBACK_FAILED, // Loopback test failed for chip A @ high speed.
  CHIP_B_LOW_SPEED_INTERNAL_LOOPBACK_FAILED,  // Loopback test failed for chip B @ low speed.
  CHIP_B_HIGH_SPEED_INTERNAL_LOOPBACK_FAILED, // Loopback test failed for chip B @ high speed.
  FORCE_TO_2_BYTES14=32767 /* reqd to make it LabWindows compatible. */
} TEST_RESULT;

// This is a new structure type as of V1.1, and is for detailed analysis
// of the selftests and is typically for internal use only.  End users
// must NOT alter its contents at any time.
typedef struct TestResultStructType {
  ULONG   Address;  // Address of first failing location of RAM tests,
                    // or sequence number of ROM_FIFO test.
  USHORT  Expect;   // Expected data at address or sequence.
  USHORT  Actual;   // Actual data at address or sequence.
} TestResultStructType;

// Used to obtain the reference to message queues.  For internal use only.
typedef struct {
	VOID * pQueueElements;
	VOID * pQueueControl;
} QueueInformation;

/*********************************************************************/
/************************ TYPE DEFINITIONS ***************************/
/*********************************************************************/
typedef UCHAR LabelType;

/*  Each receiver FIFO can hold any of the following 2 types defined below: */
/*                                                                                                                                                      */
/*  ArincRawWordType:                                                                                                           */
/*        a raw packet                                                                                                          */
/*                                                                                                                                                      */
/*  ArincStampedRawWordType:                                                                                            */
/*        a raw Stamped packet                                                                                          */
/*
  Turbo C 2.0 and indeed Borland C/C++ 3.0 support bitfields, but up to a
  a maximum of 16 bits.  This is why we need a separate data structure for
  Borland's C compilers.  - l 04-28-93
*/

#pragma pack(2)		/* Word align structure members.  (Bit-Fields!) */

typedef union {
  UCHAR i8[4];
  USHORT i16[2];
  ULONG i32;
#ifdef _MSC_VER
/*
 *  Microsoft supports 32 bit bitfields, so we have the following for MSC.
*/
  struct {
    ULONG Label :8;
    ULONG Data  :24;
  } fld;
#else
/*
 *  Turbo C 2.0 and Borland C/C++ 3.0 support bitfields, but only up to a
 *  a maximum of 16 bits.  This is why we need the following data structure'
 *  for Borland's C/C++ compilers.
*/
  struct {
    UCHAR Label;
    UCHAR Data[3];
  } fld;
#endif
} ArincRawWordType;

typedef union {
  UCHAR i8[8];
  USHORT i16[4];
  ULONG i32[2];
#ifdef _MSC_VER
/*
 *  Microsoft supports 32 bit bitfields, so we have the following for MSC.
*/
  struct {
    ULONG Label :8;
    ULONG Data  :24;
    USHORT SoftwareStamp;
    ULONG HardwareStamp;
  } fld;
#else
/*
 *  Turbo C 2.0 and Borland C/C++ 3.0 support bitfields, but only up to a
 *  a maximum of 16 bits.  This is why we need the following data structure'
 *  for Borland's C/C++ compilers.
*/
  struct {
    UCHAR Label;
    UCHAR Data[3];
    USHORT SoftwareStamp;
    ULONG HardwareStamp;
  } fld;
#endif
   } ArincStampedRawWordType;

typedef union {
  ArincRawWordType        R;
  ArincStampedRawWordType SR;
} ArincWordType;

/* added the following type 20-Mar-95 */
/* Description:
//  This type defines a time-stamped snapshot of the discrete inputs.
//  The discrete values 5..1 are stored in bits 4..0 of the object
//  PackedDiscreteState.  These values are recorded on a change of
//  state.
*/

typedef struct {
  USHORT PackedDiscreteState;
  USHORT SoftwareStamp;
  ULONG HardwareStamp;
} StampedDiscreteType;

/*****************************************************************************/
#include "proto429.h"
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif   /* #ifndef TYPES429 */
/************************** End of file ***********************************/
