#ifndef __LOG_H__
#define __LOG_H__
/*****************************************************************
 *
 * Name:        defs.h
 * Purpose:     part of MCDU Simulator
 * Author:      Marcelo Varanda
 * Created:     Feb 2009
 * Copyright:   (c) EMS Satcom
 *
 *****************************************************************
 * @(#) $Id: log.h,v 1.4 2012/05/19 03:31:24 macrae.i Exp $
 */

static int dummy_x = 0;
extern long gdisplayLogScreen;

#define FORCE_LOG

#if defined(__WXDEBUG__) || defined(FORCE_LOG)
int log_printf(const char *format, ...); /* in log_printf.cpp */
 #define LOG(x)                  log_printf("%s\n", x)
 #define LOGN(x,n1)              log_printf("%s %d\n", x, n1)
 #define LOG1H(x,n1)             log_printf("%s 0x%X\n", x, n1)
 #define LOG1N(x,n1)             log_printf("%s %d\n", x, n1)
 #define LOG2N(x,n1,n2)          log_printf("%s %d %d\n", x, n1, n2)
 #define LOG3N(x,n1,n2,n3)       log_printf("%s %d %d %d\n", x, n1,n2,n3)
 #define LOG4N(x,n1,n2,n3,n4)    log_printf("%s %d %d %d %d\n", x, n1,n2,n3,n4)
 #define LOG5N(x,n1,n2,n3,n4,n5) log_printf("%s %d %d %d %d %d\n", x, n1,n2,n3,n4,n5)
 #define LOG1S(x,n1)             log_printf("%s %s\n", x, n1)
#else
 //static int dummy_x = 0;
 #define LOG(x) dummy_x++
 #define LOGN(x,n1) dummy_x++
 #define LOG1H(x,n1) dummy_x++
 #define LOG1N(x,n1) dummy_x++
 #define LOG2N(x,n1,n2) dummy_x++
 #define LOG3N(x,n1,n2,n3) dummy_x++
 #define LOG4N(x,n1,n2,n3,n4) dummy_x++
 #define LOG5N(x,n1,n2,n3,n4,n5) dummy_x++
 #define LOG1S(x,n1) dummy_x++
#endif

#define CATFAIL(x) wxLogFatalError(x)
//#define LOG(x) wxLogMessage(x)
//#define LOG(x) wxMessageOutputStderr::Printf(x)


 #ifdef __WXMSW__
  #if defined(__WXDEBUG__) || defined(FORCE_LOG)
   #define INIT_WX_LOGS()   ::AllocConsole(); \
                            ::freopen ("CONOUT$", "w", stdout )
  #else
   #define INIT_WX_LOGS() dummy_x++
  #endif
 #else
  #define INIT_WX_LOGS() dummy_x++
 #endif

void OpenDebugLogFile();

//----------------------------------------------------------
#endif

