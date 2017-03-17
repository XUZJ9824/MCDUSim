// MSecTime.h: interface for the MSecTime class.

// 26-Jan-2009 (im) created
// 15-Apr-2009 (im) tidy up, reconcile version info
//v0.15
// 01-Jun-2011 (im) disable warning 4996
// 01-Jun-2011 (im) fixup for VS2005 (remove operator = )
//v0.50
////////////////////////////////////////////////////////////
// @(#) $Id: MSecTime.h,v 1.7 2012/02/17 19:00:28 macrae.i Exp $

#pragma warning(disable : 4996) /* don't want warning: these are declared deprecated:
                                 * C4996: 'sprintf' was declared deprecated
                                 * C4996: 'strcat' was declared deprecated
                                 * C4996: 'strcpy' was declared deprecated
                                 */

#include <windows.h>    // for SYSTEMTIME

//////////////////////////////// Implementation ///////////////////////////////

#if !defined(MSECTIME_H)
#define MSECTIME_H

// helper functions

#define MSEC_PER_24HRS          (24*60*60*1000L)    // 86,400,000L
#define SYSTEMTIME_TO_MSEC(st)  (1000*(60*(60*(st).wHour + (st).wMinute) + (st).wSecond) + (st).wMilliseconds)

DWORD MillisecondsBetweenMSec(DWORD dwStart, DWORD dwNow);
DWORD MillisecondsBetween(SYSTEMTIME * pStart, SYSTEMTIME * pNow);
void InterpretDHMS(char * szWork, DWORD secs);

void FormatAsHMS(char * szWork, DWORD msec);
char * NOW(); // show current time of day as a string (for debugging)

//////////////////////////////// Implementation ///////////////////////////////

class MSecTime
{
public:
    MSecTime();
    virtual ~MSecTime();

    void                MarkNow(void);
    unsigned long       Interval(const class MSecTime &tBase);
    unsigned long       SinceEpoch(void);
    unsigned __int64    ToTimeT1000(void);
    char *              DateTimeStamp(char * s=NULL);

public:
    static char *       DateTimeStampFromTimeT1000(unsigned __int64 t1000, char * s=NULL, bool bIsDiff=false);

    unsigned __int64    T1000Plus(unsigned __int64 ms);
    char *              T1000PlusFormatted(char * s, unsigned __int64 ms);

protected:
    static SYSTEMTIME stEpoch;

private:
    SYSTEMTIME  m_BaseSystemTime;

public:
//  operator = (const class MSecTime rValue) { m_BaseSystemTime = rValue.m_BaseSystemTime; }

private:
    //unsigned long int FENCE2;
};

#endif // !defined(MSECTIME_H)
