// log_printf.cpp
// replacement for printf()/OutputDebugString()
// @(#) $Id: log_printf.cpp,v 1.9 2012/05/31 19:02:59 macrae.i Exp $

#define WANT_ALL_DEBUG_TO_DEBUGGER      1   // 0=printf, 1=OutputDebugString
#define WANT_ALL_DEBUG_TO_PRINTF_TOO    1   // 0=debugger only, 1=printf to console/log file too
#define WANT_DATE_AND_TIME_IN_STAMP     0   // 0=show time only, 1=show date and time

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>    // for SYSTEMTIME,FILETIME,etc

#include "log.h"
#include "WhatDebuggerIsAttached.h"

extern long gdisplayLogScreen;      /* in mcduSim.cpp, non-zero means logging requested via -l */
extern char * ExportDir_Helper();   /* in mcduSim.cpp */

static int MCDU_ID = 0;

void Identify_the_MCDU(int mcdu_index)
{
    MCDU_ID = mcdu_index + 1;   // want 1,2,3,4
}

////////////////////////////////////////////////////////////////////////////////////

/* get current data and time, store into buffer using current user locale settings */

//#if WANT_ALL_DEBUG_TO_DEBUGGER
//// System debugger puts in a date/time stamp
//#else //!WANT_ALL_DEBUG_TO_DEBUGGER
int FormatCurrentDateAndTime(char * sBuffer, size_t sBufferLen)
{
    int         nCharsStoredInBuffer = 0;
    SYSTEMTIME  st;
    FILETIME    ft;
#if WANT_DATE_AND_TIME_IN_STAMP
    wchar_t     szDate[50];
#endif
    wchar_t     szTime[30];

    GetSystemTime(&st);                                         /* st is zulu time  */
    SystemTimeToTzSpecificLocalTime(/*tz=local*/NULL, &st, &st);/* st is local time */
    SystemTimeToFileTime(&st, &ft);         /* ft is local time (since 01-Jan-1601) */

if (!MCDU_ID) {
    do {} while (0);
    MCDU_ID = 0;
}
#if WANT_DATE_AND_TIME_IN_STAMP
    if (!GetDateFormat(GetSystemDefaultLCID(), 0,
            &st, L"ddd dd-MMM-yyyy", szDate, sizeof(szDate)-1)) {
        szDate[0]='\0';
    }
    if (!GetTimeFormat(GetSystemDefaultLCID(), 0,
            &st, L"HH:mm:ss", szTime, sizeof(szTime)-1)) {
        szTime[0]='\0';
    }

    nCharsStoredInBuffer = sprintf_s(sBuffer, sBufferLen, 
                "%S %S.%03u [#%d] ", szDate, szTime, st.wMilliseconds, MCDU_ID);
#else
    if (!GetTimeFormat(GetSystemDefaultLCID(), 0,
            &st, L"HH:mm:ss", szTime, sizeof(szTime)-1)) {
        szTime[0]='\0';
    }

    nCharsStoredInBuffer = sprintf_s(sBuffer, sBufferLen, 
                "%S.%03u [#%d] ", szTime, st.wMilliseconds, MCDU_ID);
#endif
    return nCharsStoredInBuffer;
}
//#endif //!WANT_ALL_DEBUG_TO_DEBUGGER

////////////////////////////////////////////////////////////////////////////////////

FILE * debug_log_file = NULL;

void CloseDebugLogFile()
{
    if (debug_log_file) {
        fclose(debug_log_file);
        debug_log_file = NULL;
    }
}

void OpenDebugLogFile()
{
    if (debug_log_file)
        return;

    if (gdisplayLogScreen) {
        SYSTEMTIME  st;
        FILETIME    ft;
        char debug_filename[100],szDate[20],szTime[20];

        GetSystemTime(&st);                                         /* st is zulu time  */
        SystemTimeToTzSpecificLocalTime(/*tz=local*/NULL, &st, &st);/* st is local time */
        SystemTimeToFileTime(&st, &ft);         /* ft is local time (since 01-Jan-1601) */

        if (!GetDateFormatA(GetSystemDefaultLCID(), 0,
                &st, "yyyy_MM_dd", szDate, sizeof(szDate)-1)) {
            szDate[0]='\0';
        }
        if (!GetTimeFormatA(GetSystemDefaultLCID(), 0,
                &st, "HH_mm_ss", szTime, sizeof(szTime)-1)) {
            szTime[0]='\0';
        }

        sprintf_s(debug_filename, sizeof(debug_filename), "%s%s__%s__M#%d.log", ExportDir_Helper(), szDate, szTime, MCDU_ID);
        do {} while (0);

        if (fopen_s(&debug_log_file, debug_filename, "w")!=0) {
            char msg[500], err[100];
            strerror_s(err, sizeof(err), errno);
            sprintf_s(msg, sizeof(msg), "Unable to open log file\n%s\n\n%s", debug_filename, err);
            MessageBoxA(NULL, msg, "MCDU Simulator", MB_ICONSTOP);
        } else
        if (debug_log_file) {
            //char msg[500];
            //sprintf_s(msg, sizeof(msg), "Log file\n%s", debug_filename);
            //MessageBoxA(NULL, msg, "MCDU Simulator", MB_ICONINFORMATION);
            atexit(CloseDebugLogFile);
        }
    }
    //INIT_WX_LOGS();
}

////////////////////////////////////////////////////////////////////////////////////

static char Debug_Output[500] = "\0";
static char * Debug_Output_pos = Debug_Output;

static CRITICAL_SECTION FLC_CS;
static bool FLC_CS_needs_init = true;

void Feed_the_Log(char * buffer)
{
    if (buffer) {

        // Special deal for important milestones: don't add anything in front of "---------- xxxxx"
        bool bSpecialPrefix = true;
        {
            char * pIn  = buffer;
            unsigned int nBytesToCheck = 12;
            while (nBytesToCheck--) {
                if (*pIn++ != '-') { // that includes NULL
                    bSpecialPrefix = false;
                    break;
                }
            }
        }

        int nAddedDateAndTimeStamp = 0;

        for(;;) {
            char ch = *buffer++;
            if (!ch)
                break;

            // silent sanity check: really long line? more likely we got lost...
            if (Debug_Output_pos >= &Debug_Output[ sizeof(Debug_Output)-2 ]) {
                do {} while (0);
                ch = '\n';  // force out that long line now, hopefully recover
            }

            if (ch=='\r' || ch=='\n') { // newline
                if (Debug_Output[0]) {

                    // output accumulated text
                    #if WANT_ALL_DEBUG_TO_DEBUGGER
                    #if WANT_ALL_DEBUG_TO_PRINTF_TOO
                    if (gdisplayLogScreen) {
                        OpenDebugLogFile();
                        if (debug_log_file)
                            fprintf(debug_log_file, "%s\n", &Debug_Output[nAddedDateAndTimeStamp]);
                        else
                             printf(                "%s\n", &Debug_Output[nAddedDateAndTimeStamp]);
                    }
                    #endif
                    strcat_s(Debug_Output, sizeof(Debug_Output), "\r\n");
                    OutputDebugStringA(Debug_Output);
                    #else
                    printf("%s\n", Debug_Output);
                    #endif

                    // start new blank line
                    Debug_Output_pos = Debug_Output;
                    *Debug_Output_pos = '\0';

                }
                // else ignore blank lines, or multiple consecutive newlines

            } else { // normal character

                if (!Debug_Output[0]) { // first char in line, need stamp

                    bool bWantDateAndTimeStamp = true;
                    //#if WANT_ALL_DEBUG_TO_DEBUGGER
                    //bWantDateAndTimeStamp = false;
                    //#endif
                    if (WhatDebuggerIsAttached("DIM Simulator (Debug)")==WHAT_DEBUGGER__DBGVIEW) {
                        bWantDateAndTimeStamp = false;
                    }

                    nAddedDateAndTimeStamp = 0;
                    if (bSpecialPrefix) {
                        /* don't add anything */
                    } else
                    if (bWantDateAndTimeStamp) {
                        // Prepend the date and time stamp
                        int nCharsStoredInBuffer = 
                            FormatCurrentDateAndTime(Debug_Output, 
                                            sizeof(Debug_Output)/sizeof(Debug_Output[0]));
                        if (nCharsStoredInBuffer<=0) {  // WHOOPS!
                            nCharsStoredInBuffer = 0;   // quietly recover
                        }
                        Debug_Output_pos = &Debug_Output[nCharsStoredInBuffer];
                        *Debug_Output_pos = '\0';
                        nAddedDateAndTimeStamp = 0; //nCharsStoredInBuffer;
                    } else {
                        // System debugger puts in a date/time stamp, but still want to identify as "[#n]"
                        int nCharsStoredInBuffer = 
                            sprintf_s(Debug_Output, sizeof(Debug_Output), "[#%d] ", MCDU_ID);
                        if (nCharsStoredInBuffer<=0) {  // WHOOPS!
                            nCharsStoredInBuffer = 0;   // quietly recover
                        }
                        Debug_Output_pos = &Debug_Output[nCharsStoredInBuffer];
                        *Debug_Output_pos = '\0';
                        nAddedDateAndTimeStamp = nCharsStoredInBuffer;
                    }

                } /*if*/

                // append next char
                *Debug_Output_pos++ = ch;
                *Debug_Output_pos   = '\0';
            } /*endif normal character*/

        } /* forever */

    } /*if*/
}

// safe version -- uses caller's buffer and sizeof(buffer)
int log_sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
    va_list marker;
    va_start(marker, format);
    int narg = vsprintf_s(buffer, sizeOfBuffer, format, marker);
    va_end(marker);

    Feed_the_Log(buffer);

    return narg;
}

// safe version -- uses caller's buffer (which must be a null-terminated string!)
void log_putstring(char *buffer)
{
    Feed_the_Log(buffer);
}

// warning: may suffer from buffer overflow
int log_printf(const char *format, ...)
{
    static char buffer[500];

    // protect access to that static buffer
    if (FLC_CS_needs_init) {
        InitializeCriticalSection(&FLC_CS);
        FLC_CS_needs_init = false;
    }
    EnterCriticalSection(&FLC_CS);
    // OK, safe to use the buffer

    va_list marker;
    va_start(marker, format);
    int narg = vsprintf_s(buffer, sizeof(buffer), format, marker);
    va_end(marker);

    Feed_the_Log(buffer);

    // done with the buffer
    LeaveCriticalSection(&FLC_CS);
    return narg;
}

////////////////////////////////////////////////////////////////////////////////////

// end of log_printf.cpp
