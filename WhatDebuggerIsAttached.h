/* WhatDebuggerIsAttached.h */
/* @(#) $Id: WhatDebuggerIsAttached.h,v 1.4 2012/01/13 18:28:29 macrae.i Exp $ */

#define WHAT_DEBUGGER__NONE         0   // no debugger found
#define WHAT_DEBUGGER__DEVSTUDIO    1   // running inside Developer Studio
#define WHAT_DEBUGGER__DBGVIEW      2   // Dbgview is running
#define WHAT_DEBUGGER__REMOTE       3   // a debugger is attached: CheckRemoteDebuggerPresent
#define WHAT_DEBUGGER__PRESENT      4   // a debugger is attached: IsDebuggerPresent
#define WHAT_DEBUGGER__IN_PEB       5   // a debugger is attached: PEB.BeingDebugged

int WhatDebuggerIsAttached(char * app_name/*debug only*/); // returns one of WHAT_DEBUGGER__xxx

/* end of WhatDebuggerIsAttached.h */
