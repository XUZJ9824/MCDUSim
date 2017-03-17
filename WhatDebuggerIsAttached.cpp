// WhatDebuggerIsAttached.cpp
// @(#) $Id: WhatDebuggerIsAttached.cpp,v 1.7 2012/01/26 16:21:48 macrae.i Exp $

#define WANT_DEBUG_WHAT_DEBUGGER        0   // 0=normal, 1=debug

#define _X86_ //need before <windef.h>
#include <windef.h>
// need to include after <windef.h>
#include <winbase.h>
#include <Tlhelp32.h>
#define PSAPI_VERSION   1   // for PSAPI under versions of Windows before Windows 7
#include <Psapi.h>  //for CreateToolhelp32Snapshot,Process32First,Process32Next
#pragma comment(lib,"Psapi.lib")
#include <Winternl.h>   // for NtQueryInformationProcess

#include "WhatDebuggerIsAttached.h"

////////////////////////////////////////////////////////////////////////////////////

typedef BOOL (* GETPROCESSINFO_CALLBACK_CHECK_FUNC) (PROCESSENTRY32 * pProcEntry, UINT_PTR arg);

static BOOL DoesPidMatch(PROCESSENTRY32 * pProcEntry, UINT_PTR arg)
{
    if (pProcEntry->th32ProcessID == (DWORD)arg) {
        return TRUE;
    } /*if*/
    return FALSE;
}

static BOOL DoesNameMatch(PROCESSENTRY32 * pProcEntry, UINT_PTR arg)
{
    if (!strcmp((char*)pProcEntry->szExeFile, (char*)arg)) {
        return TRUE;
    } /*if*/
    return FALSE;
}

BOOL WINAPI GetProcessInfo(PROCESSENTRY32 * pProcEntry, GETPROCESSINFO_CALLBACK_CHECK_FUNC func, UINT_PTR arg)
{
    BOOL            bResult = FALSE;
    OSVERSIONINFO   osver;
    HINSTANCE       hInstLib;
    HANDLE          hSnapShot;
    BOOL            bContinue;

    // ToolHelp Function Pointers.
    HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD);
    BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32);
    BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32);

    // Check to see if were running under Windows 95 or NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if (!GetVersionEx(&osver))
        goto bail;      // Sorry, that's way too ancient
    if (osver.dwPlatformId != VER_PLATFORM_WIN32_NT)
        goto bail;      // Win32S or Windows 98!

    hInstLib = LoadLibraryA("Kernel32.DLL");
    if (!hInstLib)
        goto bail;      // no Kernel32 !

    // Get procedure addresses. Do this explicitly so this code
    // will still run under an ancient version of Windows which
    // does not have the Toolhelp32 functions the Kernel32.
    lpfCreateToolhelp32Snapshot = (HANDLE(WINAPI *)(DWORD,DWORD))
                        GetProcAddress(hInstLib, "CreateToolhelp32Snapshot");
    lpfProcess32First = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
                        GetProcAddress(hInstLib, "Process32First");
    lpfProcess32Next = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
                        GetProcAddress(hInstLib, "Process32Next");
    if (!lpfProcess32Next || !lpfProcess32First || !lpfCreateToolhelp32Snapshot)
        goto bail;      // couldn't find functions in Kernel32

    // Get a handle to a Toolhelp snapshot of the systems processes.
    hSnapShot = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE)
        goto bail;      // couldn't create snapshot -- maybe no permission?

    // Get first process info
    memset((LPVOID)pProcEntry,0,sizeof(PROCESSENTRY32));
    pProcEntry->dwSize = sizeof(PROCESSENTRY32) ;
    bContinue = lpfProcess32First(hSnapShot, pProcEntry);

    // Keep looping until the desired process is found.
    while (bContinue) {
        if ( (*func)(pProcEntry, arg)) {
            bResult = TRUE;
            break;
        } /*if*/

        pProcEntry->dwSize = sizeof(PROCESSENTRY32);
        bContinue = lpfProcess32Next(hSnapShot, pProcEntry);
    } /*while*/

    // Done
bail:
    if (hInstLib)
        FreeLibrary(hInstLib);

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////////

static bool bgCheckForIsDebuggerAttached = true;
static int  nWhatDebuggerIsAttached = WHAT_DEBUGGER__NONE;

int WhatDebuggerIsAttached(char * app_name/*debug only*/) // returns one of WHAT_DEBUGGER__xxx
{
    if (!bgCheckForIsDebuggerAttached)      // already checked?
        goto bail;                          // return what we found
    bgCheckForIsDebuggerAttached = false;   // nope, check now
    #if WANT_DEBUG_WHAT_DEBUGGER
    ::MessageBoxA(NULL, "Debugger: pause for attach ...", app_name, MB_ICONSTOP);
    #endif

#if 0 // (im) Added for completeness, but it doesn't tell us which type of debugger as attached
    if (!nWhatDebuggerIsAttached) {
        HINSTANCE hInstLib = LoadLibraryA("NTDLL.DLL");
        if (hInstLib) {
            NTSTATUS (WINAPI *lpfNtQueryInformationProcess)(HANDLE,PROCESSINFOCLASS,PVOID,ULONG,PULONG);
            lpfNtQueryInformationProcess = (NTSTATUS (WINAPI *)(HANDLE,PROCESSINFOCLASS,PVOID,ULONG,PULONG))
                        GetProcAddress(hInstLib, "NtQueryInformationProcess");
            if (lpfNtQueryInformationProcess) {
                DWORD DebugPort = 0;
                DWORD rv = lpfNtQueryInformationProcess(GetCurrentProcess(), (PROCESSINFOCLASS)7/*ProcessDebugPort*/, &DebugPort, sizeof(DebugPort), 0);
                if (rv >= 0 && DebugPort!=0) {
                    // running under a debugger
                    nWhatDebuggerIsAttached = WHAT_DEBUGGER__DBGVIEW; // (im) no idea which debugger is really attached
                    #if WANT_DEBUG_WHAT_DEBUGGER
                    ::MessageBoxA(NULL, "Debugger: NtQueryInformationProcess", app_name, MB_ICONINFORMATION);
                    #endif
                    goto bail;
                }
            } // else NtQueryInformationProcess not found
            FreeLibrary(hInstLib);
        } // else no NTDLL !
    }
#endif

    if (!nWhatDebuggerIsAttached) {
        // see if the parent process is Dev Studio
        DWORD my_pid = GetCurrentProcessId();
        PROCESSENTRY32 process_entry;

        if (GetProcessInfo(&process_entry, DoesPidMatch, my_pid)) {
            DWORD ppid = process_entry.th32ParentProcessID;
            if (GetProcessInfo(&process_entry, DoesPidMatch, ppid)) {
                char * sExeName = (char *)process_entry.szExeFile;
                if (!strcmp(sExeName, "devenv.exe")) {
                    nWhatDebuggerIsAttached = WHAT_DEBUGGER__DEVSTUDIO;
                    #if WANT_DEBUG_WHAT_DEBUGGER
                    ::MessageBoxA(NULL, "Debugger: ppid is devenv", app_name, MB_ICONINFORMATION);
                    #endif
                    goto bail;
                }
            }
        }
    }

    if (!nWhatDebuggerIsAttached) {
        // see if DbgView is running
        PROCESSENTRY32 process_entry;
        if (GetProcessInfo(&process_entry, DoesNameMatch, (UINT_PTR)"Dbgview.exe")) {
            nWhatDebuggerIsAttached = WHAT_DEBUGGER__DBGVIEW;
            #if WANT_DEBUG_WHAT_DEBUGGER
            ::MessageBoxA(NULL, "Debugger: DbgView running", app_name, MB_ICONINFORMATION);
            #endif
            goto bail;
        }
    }

    #if _WIN32_WINNT >= 0x0501  // <0x501 means 95, 98, Me, NT4, Win2K; >=0x501 means XP sp1, Srv 2K3, Vista, Win7
    if (!nWhatDebuggerIsAttached) {
        BOOL dwIsDebuggerPresent = FALSE;
        CheckRemoteDebuggerPresent(GetCurrentProcess(), &dwIsDebuggerPresent);
        if (dwIsDebuggerPresent) {
            nWhatDebuggerIsAttached = WHAT_DEBUGGER__REMOTE;
            #if WANT_DEBUG_WHAT_DEBUGGER
            ::MessageBoxA(NULL, "Debugger: CheckRemoteDebuggerPresent()", app_name, MB_ICONINFORMATION);
            #endif
            goto bail;
        }
    }
    #endif // else CheckRemoteDebuggerPresent not supported

    HINSTANCE hKernel = LoadLibrary((LPCTSTR)"KERNEL32.DLL"); // in Kernel32.dll
    typedef bool (/*WINBASEAPI*/ WINAPI * IsDebuggerPresent_Function) (VOID);
    if (hKernel) {
        IsDebuggerPresent_Function IsDebuggerPresent = 
                (IsDebuggerPresent_Function)GetProcAddress(hKernel, "IsDebuggerPresent");
        if (IsDebuggerPresent) {
            BOOL bIsDebuggerPresent = IsDebuggerPresent();
            FreeLibrary(hKernel);
            if (bIsDebuggerPresent) {
                nWhatDebuggerIsAttached = WHAT_DEBUGGER__PRESENT;
                #if WANT_DEBUG_WHAT_DEBUGGER
                ::MessageBoxA(NULL, "Debugger: IsDebuggerPresent()", app_name, MB_ICONINFORMATION);
                #endif
                goto bail;
            }
        }
    }

    if (!nWhatDebuggerIsAttached) {
        // failed! try to do it ourselves (may be incompatible with different version of Windows)
        
        // for TEB or TIB:
        // see http://www.microsoft.com/msj/archive/s2ce.aspx (Microsoft Systems Journal, May 1996)
        // see http://en.wikipedia.org/wiki/Win32_Thread_Information_Block

        // for PEB:
        // see http://msdn.microsoft.com/en-us/library/aa813706(v=VS.85).aspx
        // see http://www.scribd.com/doc/36616844/20/Thread-Environment-Block-PEB
        // see http://en.wikipedia.org/wiki/Process_Environment_Block
        // also <winternl.h>
        
        typedef struct {
            BYTE Reserved1[2];
            BYTE BeingDebugged;
            BYTE Reserved2[1];
            // rest of the PEB omitted
        } PARTIAL_PEB;
        PARTIAL_PEB * peb;

        __asm
        {
            mov eax, fs:[0x18]              // Get the TIB's linear address
            mov eax, dword ptr [eax + 0x30] // Get the PEB's linear address
            mov peb, eax                    // Save it
        }
        if (peb->BeingDebugged & 0x01) {
            nWhatDebuggerIsAttached = WHAT_DEBUGGER__IN_PEB;
            #if WANT_DEBUG_WHAT_DEBUGGER
            ::MessageBoxA(NULL, "Debugger: PEB shows BeingDebugged", app_name, MB_ICONINFORMATION);
            #endif
        }
    }

    #if WANT_DEBUG_WHAT_DEBUGGER
    if (!nWhatDebuggerIsAttached) {
        ::MessageBoxA(NULL, "Debugger: None", app_name, MB_ICONINFORMATION);
    }
    #endif

bail:
    return nWhatDebuggerIsAttached;
}

// end of WhatDebuggerIsAttached.cpp
