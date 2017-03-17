// Stubs_for_DDC.cpp

#include <windows.h>    // for HANDLE, HMODULE, USHORT..., NULL, LoadLibrary(), FreeLibrary()

#include "Types429.h"   // pic429
//#include "proto429.h" // pci429

typedef STATUS (WINAPI * INITIALIZE_FUNCTION         )(LPSTR HexFile, LPSTR Unused, FARPROC Unused1, FARPROC Unused2, DiagnosticType Diagnostic, ResolutionType TimerResolution, CardType Card);
typedef STATUS (WINAPI * SHUTDOWN_FUNCTION           )(VOID);
typedef STATUS (WINAPI * CLOSECHANNELALL_FUNCTION    )(VOID);
typedef STATUS (WINAPI * SETCHIPSPEED_FUNCTION       )(ChipType ChipId, SpeedType Speed);
typedef STATUS (WINAPI * OPENCHANNEL_FUNCTION        )(ChannelType Channel, PC_BOOLEAN Filter, PC_BOOLEAN KeepTimeInfo, PC_BOOLEAN FIFO, ParityType Parity);
typedef STATUS (WINAPI * STATUSTOASCII_FUNCTION      )(STATUS Code, LPSTR Str);
typedef STATUS (WINAPI * SETCURRENTCARD_FUNCTION     )(UCHAR CardNum);
typedef STATUS (WINAPI * TRANSMIT_FIFO_FUNCTION      )(ChannelType Transmitter, ArincRawWordType FAR* ArincWord);
typedef STATUS (WINAPI * RECEIVE_FIFO_FUNCTION       )(ChannelType Receiver, ArincWordType FAR* ArincLabel);

#include "std429.h"     // dd42992

typedef S16BIT (DDCAPI * GETLIBVERSIONEX_FUNCTION    )(U16BIT *nMajor, U16BIT *nMinora, U16BIT *nMinorb);
typedef S16BIT (DDCAPI * INITCARD_FUNCTION           )(S16BIT Card);
typedef S16BIT (DDCAPI * GETERRORMSG_FUNCTION        )(S16BIT ErrorNumber, char ErrorMessage[]);
typedef S16BIT (DDCAPI * FREECARD_FUNCTION           )(S16BIT Card);
typedef S16BIT (DDCAPI * ENABLERX_FUNCTION           )(S16BIT Card, S16BIT Receiver, S16BIT Enable);
typedef S16BIT (DDCAPI * ENABLETX_FUNCTION           )(S16BIT Card, S16BIT Transmitter, S16BIT Enable);
typedef S16BIT (DDCAPI * SETRXMODE_FUNCTION          )(S16BIT Card, S16BIT Group, S16BIT Mode);
typedef S16BIT (DDCAPI * SETRXPARITY_FUNCTION        )(S16BIT Card, S16BIT Group, S16BIT Parity);
typedef S16BIT (DDCAPI * SETRXSPEED_FUNCTION         )(S16BIT Card, S16BIT Group, S16BIT Speed);
typedef S16BIT (DDCAPI * ENABLETIMESTAMP_FUNCTION    )(S16BIT Card, S16BIT Receiver, S16BIT Enable);
typedef S16BIT (DDCAPI * SETTXPARITY_FUNCTION        )(S16BIT Card, S16BIT Transmitter, S16BIT Parity);
typedef S16BIT (DDCAPI * SETTXSPEED_FUNCTION         )(S16BIT Card, S16BIT Transmitter, S16BIT Speed);
typedef S16BIT (DDCAPI * GETCHANNELCOUNT_FUNCTION    )(S16BIT Card, CHANCOUNT_p pChanCount);
typedef S16BIT (DDCAPI * LOADTXQUEUEMORE_FUNCTION    )(S16BIT Card, S16BIT Transmitter, S16BIT N, U32BIT *Data);
typedef S16BIT (DDCAPI * READRXQUEUEIRIGONE_FUNCTION )(S16BIT Card, S16BIT Receiver, U32BIT *Data, U32BIT *StampHi, U32BIT *StampLo);
typedef S16BIT (DDCAPI * GETRXQUEUESTATUS_FUNCTION   )(S16BIT Card, S16BIT Receiver);
typedef S16BIT (DDCAPI * ENABLEFILTER_FUNCTION       )(S16BIT Card, S16BIT Receiver, S16BIT Enable);
typedef S16BIT (DDCAPI * ADDFILTER_FUNCTION          )(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi);

/////////////////////////////////////////////////////////////////////////////////

static HMODULE                     hDLL_429   = NULL;
static HMODULE                     hDLL_42992 = NULL;

static INITIALIZE_FUNCTION         fn_Initialize = NULL;
static SHUTDOWN_FUNCTION           fn_Shutdown = NULL;
static CLOSECHANNELALL_FUNCTION    fn_CloseChannelAll = NULL;
static SETCHIPSPEED_FUNCTION       fn_SetChipSpeed = NULL;
static OPENCHANNEL_FUNCTION        fn_OpenChannel = NULL;
static STATUSTOASCII_FUNCTION      fn_StatusToAscii = NULL;
static SETCURRENTCARD_FUNCTION     fn_SetCurrentCard = NULL;
static TRANSMIT_FIFO_FUNCTION      fn_Transmit_FIFO = NULL;
static RECEIVE_FIFO_FUNCTION       fn_Receive_FIFO = NULL;

static GETLIBVERSIONEX_FUNCTION    fn_GetLibVersionEx = NULL;
static INITCARD_FUNCTION           fn_InitCard = NULL;
static GETERRORMSG_FUNCTION        fn_GetErrorMsg = NULL;
static FREECARD_FUNCTION           fn_FreeCard = NULL;
static ENABLERX_FUNCTION           fn_EnableRx = NULL;
static ENABLETX_FUNCTION           fn_EnableTx = NULL;
static SETRXMODE_FUNCTION          fn_SetRxMode = NULL;
static SETRXPARITY_FUNCTION        fn_SetRxParity = NULL;
static SETRXSPEED_FUNCTION         fn_SetRxSpeed = NULL;
static ENABLETIMESTAMP_FUNCTION    fn_EnableTimeStamp = NULL;
static SETTXPARITY_FUNCTION        fn_SetTxParity = NULL;
static SETTXSPEED_FUNCTION         fn_SetTxSpeed = NULL;
static GETCHANNELCOUNT_FUNCTION    fn_GetChannelCount = NULL;
static LOADTXQUEUEMORE_FUNCTION    fn_LoadTxQueueMore = NULL;
static READRXQUEUEIRIGONE_FUNCTION fn_ReadRxQueueIrigOne = NULL;
static GETRXQUEUESTATUS_FUNCTION   fn_GetRxQueueStatus = NULL;
static ENABLEFILTER_FUNCTION       fn_EnableFilter = NULL;
static ADDFILTER_FUNCTION          fn_AddFilter = NULL;

/////////////////////////////////////////////////////////////////////////////////

static void Unload_Function_Helper(void)
{
    if (hDLL_429) {
        FreeLibrary(hDLL_429);
        hDLL_429 = NULL;

        fn_Initialize = NULL;
        fn_Shutdown = NULL;
        fn_CloseChannelAll = NULL;
        fn_SetChipSpeed = NULL;
        fn_OpenChannel = NULL;
        fn_StatusToAscii = NULL;
        fn_SetCurrentCard = NULL;
        fn_Transmit_FIFO = NULL;
        fn_Receive_FIFO = NULL;
    }

    if (hDLL_42992) {
        FreeLibrary(hDLL_42992);
        hDLL_42992 = NULL;

        fn_GetLibVersionEx = NULL;
        fn_InitCard = NULL;
        fn_GetErrorMsg = NULL;
        fn_FreeCard = NULL;
        fn_EnableRx = NULL;
        fn_EnableTx = NULL;
        fn_SetRxMode = NULL;
        fn_SetRxParity = NULL;
        fn_SetRxSpeed = NULL;
        fn_EnableTimeStamp = NULL;
        fn_SetTxParity = NULL;
        fn_SetTxSpeed = NULL;
        fn_GetChannelCount = NULL;
        fn_LoadTxQueueMore = NULL;
        fn_ReadRxQueueIrigOne = NULL;
        fn_GetRxQueueStatus = NULL;
    }
}

static void * Load_429_Helper(char * function_name)
{
    if (!hDLL_429) {
        hDLL_429 = LoadLibraryA("pci429.dll");
        if (!hDLL_429) return NULL; // no DLL !
        atexit(Unload_Function_Helper);
    }

    return (void *)GetProcAddress(hDLL_429, function_name);
}

static void * Load_42992_Helper(char * function_name)
{
    if (!hDLL_42992) {
        hDLL_42992 = LoadLibraryA("dd42992.dll");
        if (!hDLL_42992) return NULL; // no DLL !
        atexit(Unload_Function_Helper);
    }

    return (void *)GetProcAddress(hDLL_42992, function_name);
}

#define LOADER_HELPER(l,x,t,r) \
    if (!fn_##x) { \
    fn_##x = (t)Load_##l##_Helper(#x); \
    } \
    if (!fn_##x) { \
        return r; \
    }

#define LOAD_PCI429_HELPER(x,t,r) LOADER_HELPER(429,x,t,r)

STATUS WINAPI Initialize(LPSTR HexFile, LPSTR Unused, FARPROC Unused1, FARPROC Unused2, DiagnosticType Diagnostic, ResolutionType TimerResolution, CardType Card)
{
    LOAD_PCI429_HELPER(Initialize, INITIALIZE_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_Initialize)(HexFile, Unused, Unused1, Unused2, Diagnostic, TimerResolution, Card);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI Shutdown(VOID)
{
    LOAD_PCI429_HELPER(Shutdown, SHUTDOWN_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_Shutdown)();
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI CloseChannelAll(VOID)
{
    LOAD_PCI429_HELPER(CloseChannelAll, CLOSECHANNELALL_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_CloseChannelAll)();
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI SetChipSpeed(ChipType ChipId, SpeedType Speed)
{
    LOAD_PCI429_HELPER(SetChipSpeed, SETCHIPSPEED_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_SetChipSpeed)(ChipId, Speed);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI OpenChannel(ChannelType Channel, PC_BOOLEAN Filter, PC_BOOLEAN KeepTimeInfo, PC_BOOLEAN FIFO, ParityType Parity)
{
    LOAD_PCI429_HELPER(OpenChannel, OPENCHANNEL_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_OpenChannel)(Channel, Filter, KeepTimeInfo, FIFO, Parity);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI StatusToAscii(STATUS Code, LPSTR Str)
{
    LOAD_PCI429_HELPER(StatusToAscii, STATUSTOASCII_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_StatusToAscii)(Code, Str);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI SetCurrentCard(UCHAR CardNum)
{
    LOAD_PCI429_HELPER(SetCurrentCard, SETCURRENTCARD_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_SetCurrentCard)(CardNum);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI Transmit_FIFO(ChannelType Transmitter, ArincRawWordType FAR* ArincWord)
{
    LOAD_PCI429_HELPER(Transmit_FIFO, TRANSMIT_FIFO_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_Transmit_FIFO)(Transmitter, ArincWord);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

STATUS WINAPI Receive_FIFO(ChannelType Receiver, ArincWordType FAR* ArincLabel)
{
    LOAD_PCI429_HELPER(Receive_FIFO, RECEIVE_FIFO_FUNCTION, (STATUS)INTERNAL_ERROR);
    try {
        return (*fn_Receive_FIFO)(Receiver, ArincLabel);
    } catch (...) {
        ;
    }
    return (STATUS)INTERNAL_ERROR;
}

#define LOAD_DD42992_HELPER(x,t,r) LOADER_HELPER(42992,x,t,r)

S16BIT DDCAPI GetLibVersionEx(U16BIT *nMajor, U16BIT *nMinora, U16BIT *nMinorb)
{
    LOAD_DD42992_HELPER(GetLibVersionEx, GETLIBVERSIONEX_FUNCTION, (S16BIT)1);
    try {
        return (*fn_GetLibVersionEx)(nMajor, nMinora, nMinorb);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI InitCard(S16BIT Card)
{
    LOAD_DD42992_HELPER(InitCard, INITCARD_FUNCTION, (S16BIT)1);
    try {
        return (*fn_InitCard)(Card);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI GetErrorMsg(S16BIT ErrorNumber, char ErrorMessage[])
{
    LOAD_DD42992_HELPER(GetErrorMsg, GETERRORMSG_FUNCTION, (S16BIT)1);
    try {
        return (*fn_GetErrorMsg)(ErrorNumber, ErrorMessage);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI FreeCard(S16BIT Card)
{
    LOAD_DD42992_HELPER(FreeCard, FREECARD_FUNCTION, (S16BIT)1);
    try {
        return (*fn_FreeCard)(Card);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI EnableRx(S16BIT Card, S16BIT Receiver, S16BIT Enable)
{
    LOAD_DD42992_HELPER(EnableRx, ENABLERX_FUNCTION, (S16BIT)1);
    try {
        return (*fn_EnableRx)(Card, Receiver, Enable);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI EnableTx(S16BIT Card, S16BIT Transmitter, S16BIT Enable)
{
    LOAD_DD42992_HELPER(EnableTx, ENABLETX_FUNCTION, (S16BIT)1);
    try {
        return (*fn_EnableTx)(Card, Transmitter, Enable);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI SetRxMode(S16BIT Card, S16BIT Group, S16BIT Mode)
{
    LOAD_DD42992_HELPER(SetRxMode, SETRXMODE_FUNCTION, (S16BIT)1);
    try {
        return (*fn_SetRxMode)(Card, Group, Mode);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI SetRxParity(S16BIT Card, S16BIT Group, S16BIT Parity)
{
    LOAD_DD42992_HELPER(SetRxParity, SETRXPARITY_FUNCTION, (S16BIT)1);
    try {
        return (*fn_SetRxParity)(Card, Group, Parity);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI SetRxSpeed(S16BIT Card, S16BIT Group, S16BIT Speed)
{
    LOAD_DD42992_HELPER(SetRxSpeed, SETRXSPEED_FUNCTION, (S16BIT)1);
    try {
        return (*fn_SetRxSpeed)(Card, Group, Speed);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI EnableTimeStamp(S16BIT Card, S16BIT Receiver, S16BIT Enable)
{
    LOAD_DD42992_HELPER(EnableTimeStamp, ENABLETIMESTAMP_FUNCTION, (S16BIT)1);
    try {
        return (*fn_EnableTimeStamp)(Card, Receiver, Enable);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI SetTxParity(S16BIT Card, S16BIT Transmitter, S16BIT Parity)
{
    LOAD_DD42992_HELPER(SetTxParity, SETTXPARITY_FUNCTION, (S16BIT)1);
    try {
        return (*fn_SetTxParity)(Card, Transmitter, Parity);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI SetTxSpeed(S16BIT Card, S16BIT Transmitter, S16BIT Speed)
{
    LOAD_DD42992_HELPER(SetTxSpeed, SETTXSPEED_FUNCTION, (S16BIT)1);
    try {
        return (*fn_SetTxSpeed)(Card, Transmitter, Speed);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI GetChannelCount(S16BIT Card, CHANCOUNT_p pChanCount)
{
    LOAD_DD42992_HELPER(GetChannelCount, GETCHANNELCOUNT_FUNCTION, (S16BIT)1);
    try {
        return (*fn_GetChannelCount)(Card, pChanCount);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI LoadTxQueueMore(S16BIT Card, S16BIT Transmitter, S16BIT N, U32BIT *Data)
{
    LOAD_DD42992_HELPER(LoadTxQueueMore, LOADTXQUEUEMORE_FUNCTION, (S16BIT)1);
    try {
        return (*fn_LoadTxQueueMore)(Card, Transmitter, N, Data);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI ReadRxQueueIrigOne(S16BIT Card, S16BIT Receiver, U32BIT *Data, U32BIT *StampHi, U32BIT *StampLo)
{
    LOAD_DD42992_HELPER(ReadRxQueueIrigOne, READRXQUEUEIRIGONE_FUNCTION, (S16BIT)1);
    try {
        return (*fn_ReadRxQueueIrigOne)(Card, Receiver, Data, StampHi, StampLo);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI GetRxQueueStatus(S16BIT Card, S16BIT Receiver)
{
    LOAD_DD42992_HELPER(GetRxQueueStatus, GETRXQUEUESTATUS_FUNCTION, (S16BIT)1);
    try {
        return (*fn_GetRxQueueStatus)(Card, Receiver);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI EnableFilter(S16BIT Card, S16BIT Receiver, S16BIT Enable)
{
    LOAD_DD42992_HELPER(EnableFilter, ENABLEFILTER_FUNCTION, (S16BIT)1);
    try {
        return (*fn_EnableFilter)(Card, Receiver, Enable);
    } catch (...) {
        ;
    }
    return (S16BIT)1;
}

S16BIT DDCAPI AddFilter(S16BIT Card, S16BIT Receiver, S16BIT LabelSdi)
{
    //if (!fn_AddFilter) {
    //  fn_AddFilter = (ADDFILTER_FUNCTION)Load_42992_Helper("AddFilter");
    //  
    //  char msg[100];
    //  sprintf_s(msg, sizeof(msg), "Found AddFilter at $%p\n", (void*)fn_AddFilter);
    //  MessageBoxA(NULL, msg, "MCDU Sim (debug)", MB_OK);
    //}
    LOAD_DD42992_HELPER(AddFilter, ADDFILTER_FUNCTION, (S16BIT)0);
    try {
        return (*fn_AddFilter)(Card, Receiver, LabelSdi);
    } catch (...) {
        ;
    }
    return (S16BIT)0;
}

// end of Stubs_for_DDC.cpp
