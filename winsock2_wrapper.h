/* Winsock2 wrapper: use instead of winsock2.h anywhere winsock2.h is included */
/* @(#) $Id: winsock2_wrapper.h,v 1.3 2012/02/17 19:00:29 macrae.i Exp $ */

#ifndef _WINSOCK_WRAPPER_H_
#define _WINSOCK_WRAPPER_H_
 
#undef _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#include <winsock2.h>
 
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
 
#pragma comment(lib, "ws2_32.lib")
 
#endif
