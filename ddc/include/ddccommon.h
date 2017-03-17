/******************************************************************************
 *		    AceXtreme 'C' Run Time Software Library
 *
 * FILE:          ddccommon.h
 *
 * DESCRIPTION:   Common definitions and types used in the 69092 and 42992
 *                libraries.
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2008 by Data Device Corporation
 * All Rights Reserved.
 ******************************************************************************/
#ifndef __DDC_COMMON_H__
#define __DDC_COMMON_H__

/*============================================================================*/
/*                               D E F I N E S                                */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                               W I N D O W S                                */
/*----------------------------------------------------------------------------*/
#ifdef WIN32

typedef HANDLE				  DDC_HANDLE;
typedef unsigned char         U8BIT;
typedef unsigned short        U16BIT;
typedef unsigned long         U32BIT;
typedef ULONG64               U64BIT;

typedef char                  S8BIT;
typedef short                 S16BIT;
typedef long                  S32BIT;
typedef LONG64                S64BIT;

#define  U64 U64BIT
#define  S64 S64BIT
#define  U32 U32BIT
#define  S32 S32BIT
#define  U16 U16BIT
#define  S16 S16BIT
#define  U8  U8BIT
#define  S8  S8BIT

/* Mutex Definitions */
#define DDC_MUTEX		           DDC_HANDLE
#define DDC_MUTEX_DESTROY(x)       CloseHandle(x)
#define DDC_MUTEX_CREATE(x,y,z)    CreateMutex(x,y,z)
#define DDC_MUTEX_WAIT_FOREVER(x)  WaitForSingleObject(x,INFINITE)
#define DDC_MUTEX_RELEASE(x)       ReleaseMutex(x)

#endif /* WIN32 */

/*----------------------------------------------------------------------------*/
/*                                 L I N U X                                  */
/*----------------------------------------------------------------------------*/
#ifdef LINUX

typedef int                   DDC_HANDLE;

#define HANDLE                DDC_HANDLE

typedef unsigned char         U8BIT;
typedef unsigned short        U16BIT;
typedef unsigned int          U32BIT;
typedef unsigned long long    U64BIT;

typedef char                  S8BIT;
typedef short                 S16BIT;
typedef int                   S32BIT;
typedef long long             S64BIT;

typedef unsigned short        USHORT;
typedef unsigned short        WORD;
typedef unsigned long         ULONG;

typedef unsigned char         UCHAR;
typedef short                 SHORT;
typedef long                  LONG;
typedef unsigned long         DWORD;

#define LPVOID                void*
#define PVOID                 void*
#define VOID                  void

typedef unsigned char         BOOL;
typedef unsigned char         BOOLEAN;

#define  U64 U64BIT
#define  S64 S64BIT
#define  U32 U32BIT
#define  S32 S32BIT
#define  U16 U16BIT
#define  S16 S16BIT
#define  U8  U8BIT
#define  S8  S8BIT

/* Mutex Definitions */
#define DDC_MUTEX                   pthread_mutex_t *
#define DDC_MUTEX_DESTROY(x)        {pthread_mutex_destroy(x);free(x);x=NULL;}
#define DDC_MUTEX_CREATE(x,y,z)     _dvxMutexCreate(x)
#define DDC_MUTEX_WAIT_FOREVER(x)   pthread_mutex_lock(x)
#define DDC_MUTEX_RELEASE(x)        pthread_mutex_unlock(x)

#endif /* linux */

/*----------------------------------------------------------------------------*/
/*                               V X W O R K S                                */
/*----------------------------------------------------------------------------*/
#ifdef VX_WORKS

#include <vxworks.h>

#define HANDLE                void *
#define DDC_HANDLE            void *

typedef UINT8                 U8BIT;
typedef UINT16                U16BIT;
typedef UINT32                U32BIT;
typedef UINT64                U64BIT;

typedef INT8                  S8BIT;
typedef INT16                 S16BIT;
typedef INT32                 S32BIT;
typedef INT64                 S64BIT;

typedef UINT32                DWORD;

#define LPVOID                void*
#define PVOID                 void*
#define VOID                  void

typedef unsigned char         BOOLEAN;

typedef unsigned short        WORD;

typedef short                 SHORT;
typedef long                  LONG;
typedef unsigned long *       PULONG;
typedef unsigned short *      PUSHORT;

#define  U64 U64BIT
#define  S64 S64BIT
#define  U32 U32BIT
#define  S32 S32BIT
#define  U16 U16BIT
#define  S16 S16BIT
#define  U8  U8BIT
#define  S8  S8BIT

/* Mutex Definitions */
#define DDC_MUTEX                   SEM_ID
#define DDC_MUTEX_DESTROY(x)        {semDelete(x); x=NULL;}
#define DDC_MUTEX_CREATE(x,y,z)     semBCreate(SEM_Q_FIFO,SEM_FULL)
#define DDC_MUTEX_WAIT_FOREVER(x)   semTake(x, WAIT_FOREVER)
#define DDC_MUTEX_RELEASE(x)        semGive(x)

#define INVALID_FAMILY              0xFF

#endif /* VX_WORKS */

/*----------------------------------------------------------------------------*/
/*                             I N T E G R I T Y                              */
/*----------------------------------------------------------------------------*/
#ifdef INTEGRITY

typedef unsigned char         U8BIT;
typedef unsigned short        U16BIT;
typedef unsigned long         U32BIT;
typedef unsigned long long    U64BIT;

typedef char                  S8BIT;
typedef short                 S16BIT;
typedef long                  S32BIT;
typedef signed long long      S64BIT;

typedef unsigned short        WORD;

typedef unsigned long         DWORD;

typedef unsigned char         BOOL;
typedef unsigned char         BOOLEAN;
typedef void		          VOID;

/* Mutex Definitions */
#define DDC_MUTEX
#define DDC_MUTEX_DESTROY(x)
#define DDC_MUTEX_CREATE(x,y,z)
#define DDC_MUTEX_WAIT_FOREVER(x)
#define DDC_MUTEX_RELEASE(x)

#endif /* INTEGRITY */

/*----------------------------------------------------------------------------*/
/*                              A L L   O S ' S                               */
/*----------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE                 0
#endif

#ifndef TRUE
#define TRUE                  1
#endif


/* Device Family Name Enumeration */
typedef enum _FAMILY
{
   EMACE = 0,
   E2MACE,
   ACEX
} FAMILY;


typedef enum
{
   DISC_INPUT  = 0,
   DISC_OUTPUT = 1
} DISC_DIR;

/*============================================================================*/
/*                               S T R U C T S                                */
/*============================================================================*/
/* Software Version Information struct */
typedef struct _SWVERSIONINFO
{
   U32BIT dwRtlVersion;
   U32BIT dwCoreVersion;
   U8BIT  szRtlVersion[32];
   U32BIT dwReserved1;
   U32BIT dwReserved2;
   U32BIT dwReserved3;
   U32BIT dwReserved4;
} SWVERSIONINFO, *PSWVERSIONINFO;

/* Hardware Version Information struct */
typedef struct _HWVERSIONINFO
{
   U32BIT dwFwVersion;
   U32BIT dwHdlVersion;
   U32BIT dwDriverVersion;
   U32BIT dwSerialNumber;
   FAMILY dwFamilyNumber;
   U32BIT dwModelNumber;
   U8BIT  szModelName[32];
   U8BIT  szDriverVersion[16];
   U32BIT dwReserved1;
   U32BIT dwReserved2;
   U32BIT dwReserved3;
   U32BIT dwReserved4;
   U8BIT  szReserved[32];
} HWVERSIONINFO, *PHWVERSIONINFO;


/* Channel count struct */
typedef struct _CHANCOUNT
{
   U8BIT bTx;
   U8BIT bRx;
   U8BIT bGroup;
   U8BIT bDiscrete;
   U8BIT bAvionic;
   U8BIT bRs232;
   U8BIT bRs485;
   U8BIT bBoardModel;
   U8BIT b1553;
} CHANCOUNT_t, *CHANCOUNT_p;

#endif /* __DDC_COMMON_H__ */
