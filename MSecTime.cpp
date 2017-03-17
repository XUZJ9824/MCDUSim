// MSecTime.cpp: implementation of the MSecTime class.

// 26-Jan-2009 (im) created
// 15-Apr-2009 (im) tidy up, reconcile version info
//v0.15
////////////////////////////////////////////////////////////
// @(#) $Id: MSecTime.cpp,v 1.6 2012/01/13 18:28:29 macrae.i Exp $

#include <time.h>   // for _tzset, _timezone, _daylight, _dstbias, struct tm, mktime()
#include <stdio.h>  // for sprintf

#include "MSecTime.h"

#define SHOW_LONG_DATETIME  0   //0=HH:MM:SS.TTT; 1=DOW DD-MMM-YYYY HH:MM:SS.TTT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// helper functions

DWORD MillisecondsBetweenMSec(DWORD dwStart, DWORD dwNow)
{
    if (dwNow < dwStart)    // we started just before midnight, now it is the next day (or a subsequent day)
        dwNow += MSEC_PER_24HRS;

    return dwNow - dwStart;
}

DWORD MillisecondsBetween(SYSTEMTIME * pStart, SYSTEMTIME * pNow)
{
    DWORD dwStart = SYSTEMTIME_TO_MSEC(*pStart);
    DWORD dwNow = SYSTEMTIME_TO_MSEC(*pNow);
    return MillisecondsBetweenMSec(dwStart, dwNow);
}

void InterpretDHMS(char * szWork, DWORD secs)
{
    unsigned int nDays, nHours, nMinutes, nSeconds;

    nSeconds = (int)(secs % 60L);
    secs /= 60L;
    nMinutes = (int)(secs % 60L);
    secs /= 60L;
    nHours = (int)(secs % 24L);
    secs /= 24L;
    nDays = (int)secs;

    szWork[0] = '\0';
    if (nDays)
        sprintf(szWork+strlen(szWork), " %u day%s", nDays, nDays==1?"":"s");
    if (nHours)
        sprintf(szWork+strlen(szWork), " %u hour%s", nHours, nHours==1?"":"s");
    if (nHours || nMinutes)
        sprintf(szWork+strlen(szWork), " %u min%s", nMinutes, nMinutes==1?"":"s");
    if (nHours || nMinutes || nSeconds)
        sprintf(szWork+strlen(szWork), " %u sec%s", nSeconds, nSeconds==1?"":"s");
}

void FormatAsHMS(char * szWork, DWORD msec)
{
    DWORD nSec = msec/1000L;    // full seconds
    DWORD nMin = nSec/60L;      // full minutes
    nSec %= 60L;                // 0..59 seconds

    if (msec<2000L) {
        sprintf(szWork, "%lu.%02lu sec", nSec, (msec%1000L)/10L);
    }
    else
    if (msec<10000L) {
        sprintf(szWork, "%lu.%01lu sec", nSec, (msec%1000L)/100L);
    }
    else
    if (nMin<1L)
        sprintf(szWork, "%lu sec", nSec);
    else
    if (nMin<10L) {
        sprintf(szWork, "%lu min", nMin);
        if (nSec)
            sprintf(szWork+strlen(szWork), " %lu sec", nSec);
    }
    else {  // 10 minutes or more
        //if ((nSec%60L)>=30L) // bump to account for partial minutes
        //  ++ nMin;
        if (nMin<60L)
            sprintf(szWork, "%lu min", nMin);
        else {
            sprintf(szWork, "%lu hour", nMin/60L);
            nMin %= 60L;        // 0..59 minutes
            if (nMin)
                sprintf(szWork+strlen(szWork), " %lu min", nMin);
        }
        if (nSec)
            sprintf(szWork+strlen(szWork), " %lu sec", nSec);
    }
}

char * NOW() // show current time of day as a string (for debugging)
{
    static char szTimeStamp[50];
    MSecTime now;
    now.DateTimeStamp(szTimeStamp);
    return szTimeStamp;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SYSTEMTIME MSecTime::stEpoch = {1970,1,0,1, 0,0,0,0};

MSecTime::MSecTime()
{
    MarkNow();
}

MSecTime::~MSecTime()
{
}

void MSecTime::MarkNow(void)
{
    ::GetSystemTime(&m_BaseSystemTime);
}

unsigned long MSecTime::Interval(const class MSecTime &tBase)
{
    FILETIME ftBase, ftThis;

    ::SystemTimeToFileTime(&tBase.m_BaseSystemTime, &ftBase);
    ::SystemTimeToFileTime(&      m_BaseSystemTime, &ftThis);

    unsigned __int64 nn100nsIntervals = *(unsigned __int64 *)&ftThis - *(unsigned __int64 *)&ftBase;
    return (unsigned long) (nn100nsIntervals/(10L*1000L));
}

unsigned long MSecTime::SinceEpoch(void)
{
    FILETIME ftBase, ftThis;

    ::SystemTimeToFileTime(&stEpoch, &ftBase);
    ::SystemTimeToFileTime(&m_BaseSystemTime, &ftThis);

    unsigned __int64 nn100nsIntervals = *(unsigned __int64 *)&ftThis - *(unsigned __int64 *)&ftBase;
    return (unsigned long) (nn100nsIntervals/(10L*1000L));
}

unsigned __int64 MSecTime::ToTimeT1000(void) //msecs since 01-Jan-1970
{
    _tzset();   /* get the current local timezone info */
    #if 0 // from <time.h>
    _CRTIMP extern int _daylight;       /* non-zero if daylight savings time is used */
    _CRTIMP extern long _dstbias;       /* offset for Daylight Saving Time */
    _CRTIMP extern long _timezone;      /* difference in seconds between GMT and local time */
    _CRTIMP extern char * _tzname[2];   /* standard/daylight savings time zone names */
    #endif

    int nTimeZoneDiffSecs = _timezone;
    if (_daylight) {
        nTimeZoneDiffSecs += _dstbias;
    }
    int nTimeZoneHoursDiff = nTimeZoneDiffSecs / 3600;
    int nTimeZoneMinsDiff = nTimeZoneDiffSecs - (nTimeZoneHoursDiff * 3600);

    struct tm time_parts;
    time_parts.tm_year = m_BaseSystemTime.wYear   - 1900;               /* abs year to year since 1900 */
    time_parts.tm_mon  = m_BaseSystemTime.wMonth  - 1;                  /* months in range 0 to 11 */
    time_parts.tm_mday = m_BaseSystemTime.wDay;
    time_parts.tm_hour = m_BaseSystemTime.wHour   - nTimeZoneHoursDiff; /* adjust hours */
    time_parts.tm_min  = m_BaseSystemTime.wMinute - nTimeZoneMinsDiff;  /* adjust minutes */
    time_parts.tm_sec  = m_BaseSystemTime.wSecond;
    time_parts.tm_isdst = 1;

    time_t t = mktime(&time_parts);
    return (t*(unsigned __int64)1000L)+m_BaseSystemTime.wMilliseconds;
}

char * MSecTime::DateTimeStampFromTimeT1000(unsigned __int64 t1000, char * s, bool bIsDiff)
{
    time_t calendarTime = (time_t)     (t1000 / (unsigned __int64)1000L);
    unsigned int msecs = (unsigned int)(t1000 % (unsigned __int64)1000L);

    struct tm * lt = localtime(&calendarTime);
    lt->tm_year += 1900;

    #if SHOW_LONG_DATETIME
    static char * aszDayName[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat", "???"};
    static char * aszMonthName[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","???"};
    #endif
    static char szWork[100];

    unsigned int nDayOfWeek = lt->tm_wday;
    if (nDayOfWeek<0 || nDayOfWeek>=7) nDayOfWeek = 7;
    unsigned int nMonth = lt->tm_mon;
    if (nMonth<0 || nMonth>=12) nMonth = 12;

    #if SHOW_LONG_DATETIME
    if (bIsDiff)
        sprintf(szWork, "              +      :%02u.%03u",
            lt->tm_sec, msecs);
    else
        sprintf(szWork, "%3s %02u-%3s-%04u %02u:%02u:%02u.%03u",
            aszDayName[nDayOfWeek],
            lt->tm_mday, aszMonthName[nMonth], lt->tm_year,
            lt->tm_hour, lt->tm_min, lt->tm_sec, msecs);
    #else
    if (bIsDiff)
        sprintf(szWork, "  +  :%02u.%03u",
            lt->tm_sec, msecs);
    else
        sprintf(szWork, "%02u:%02u:%02u.%03u",
            lt->tm_hour, lt->tm_min, lt->tm_sec, msecs);
    #endif

    if (s) {
        strcpy(s, szWork);
        return s;
    }
    return szWork;
}

char * MSecTime::DateTimeStamp(char * s)
{
    unsigned __int64 t1000 = ToTimeT1000();
    return MSecTime::DateTimeStampFromTimeT1000(t1000, s);
}

unsigned __int64 MSecTime::T1000Plus(unsigned __int64 ms)
{
    unsigned __int64 t1000 = ToTimeT1000();
    return t1000+ms;
}

char * MSecTime::T1000PlusFormatted(char * s, unsigned __int64 ms)
{
    unsigned __int64 t1000 = T1000Plus(ms);
    return MSecTime::DateTimeStampFromTimeT1000(t1000, s);
}

// end of MSecTime.cpp
