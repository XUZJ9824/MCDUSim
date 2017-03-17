/*********************************************************************/
/*
* Definitions for Programming
*
* This is an Windows 95 specific file
*
* Description: Include file
*
*
* Modification History:
*
* 05/22/91  RFC
* 05/05/95  DFL - changed types to be compatible with the LabWindows
*                 32-bit interface.  This is conditionally compiled
*                 with the _NI_i386_ when LabWindows is compiling.
* 04/13/00  KWH	- Enhanced Lab Windows support.
*
*/
/*********************************************************************/

#ifndef _STDDEFS_
#define _STDDEFS_
#ifndef STATIC
#define STATIC          static
#endif

#ifndef SHORTINT
  #define SHORTINT  short
  #define USHORTINT unsigned short
#endif

#ifdef _NI_i386_
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef double          DOUBLE;
#else /* not _NI_386_ */

#ifndef interrupt
  #define interrupt void
#endif

#ifndef far
  #define far
#endif

#define _export

#ifndef _INC_WINDOWS		// from win32 windows.h
  typedef char            CHAR;
  typedef unsigned char   UCHAR;
  typedef short           SHORT;
  typedef unsigned short  USHORT;
  typedef int             INT;
  typedef unsigned int    UINT;
  #undef LONG
  typedef long            LONG;
  typedef unsigned long   ULONG;
  typedef double          DOUBLE;
  typedef char            *LPSTR;
//  typedef short           WORD;
  typedef int             BOOL;

  #ifndef VOID
    #define VOID void
  #endif

  #define CDECL
  #define CALLBACK
//  typedef void (CALLBACK* FARPROC)(void);
#endif /* _INC_WINDOWS */

#endif /* _NI_i386_ */

/***************************************************************************/
/*                                                                         */
/* Function Name: INP, OUTP                                                */
/*                                                                         */
/* Abstract:      wrappers for PCI I/O accesses                            */
/*                                                                         */
/***************************************************************************/
extern UCHAR PCI_inp (ULONG addr);
extern void PCI_outp (ULONG addr, UCHAR value);

#endif /* _STDDEFS_ */
/*********************** End of file *********************************/
