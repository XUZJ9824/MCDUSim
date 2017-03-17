// Stubs_for_TechSAT.c

#include <windows.h>    // for HANDLE, HMODULE, USHORT..., NULL, LoadLibrary(), FreeLibrary()
#include "a429usb.h"

typedef int    (* SET_REC_BUF_SIZE_FUNCTION     )(HANDLE hdl, char rx_number, unsigned short size, unsigned short* result, unsigned short* status);
typedef int    (* CONFIG_REC_FUNCTION           )(HANDLE hdl, char rx_number, unsigned short speed);
typedef int    (* CONFIG_TRANS_FUNCTION         )(HANDLE hdl, char tx_number, unsigned short speed, unsigned short bitrate, unsigned short parity);
typedef int    (* GET_FW_VERSION_FUNCTION       )(HANDLE hdl, short ipnum, unsigned short* vnum);
typedef int    (* VERIFY_IFLIB_VERSION_FUNCTION )(unsigned short vnum);
typedef HANDLE (* OPEN_XPC_FUNCTION             )(short lbn);
typedef int    (* ACTIVATE_TRANS_FUNCTION       )(HANDLE hdl, char tx_number, unsigned short act_count, tx_activation_t* buffer);
typedef int    (* DEFINE_TRANS_FUNCTION         )(HANDLE hdl, char tx_number, unsigned short def_count, tx_definition_t* buffer, unsigned short* result, unsigned short* timeslice);
typedef int    (* READ_RECEIVER_BUFFER_FUNCTION )(HANDLE hdl, char rx_number, rx_data_t* buffer, unsigned short* result, unsigned short* num_entry);
typedef int    (* ACTIVATE_REC_FUNCTION         )(HANDLE hdl, char rx_number, unsigned short act_code, rx_mode_t* buffer, unsigned short* status);

static HMODULE                       hDLL = NULL;
static SET_REC_BUF_SIZE_FUNCTION     fn_set_rec_buf_size = NULL;
static CONFIG_REC_FUNCTION           fn_config_rec = NULL;
static CONFIG_TRANS_FUNCTION         fn_config_trans = NULL;
static GET_FW_VERSION_FUNCTION       fn_get_fw_version = NULL;
static VERIFY_IFLIB_VERSION_FUNCTION fn_verify_iflib_version = NULL;
static OPEN_XPC_FUNCTION             fn_open_xpc = NULL;
static ACTIVATE_TRANS_FUNCTION       fn_activate_trans = NULL;
static DEFINE_TRANS_FUNCTION         fn_define_trans = NULL;
static READ_RECEIVER_BUFFER_FUNCTION fn_read_receiver_buffer = NULL;
static ACTIVATE_REC_FUNCTION         fn_activate_rec = NULL;

static void Unload_Function_Helper(void)
{
    if (hDLL) {
        FreeLibrary(hDLL);
        hDLL = NULL;

        fn_set_rec_buf_size = NULL;
        fn_config_rec = NULL;
        fn_config_trans = NULL;
        fn_get_fw_version = NULL;
        fn_verify_iflib_version = NULL;
        fn_open_xpc = NULL;
        fn_activate_trans = NULL;
        fn_define_trans = NULL;
        fn_read_receiver_buffer = NULL;
        fn_activate_rec = NULL;
    }
}

static void * Load_Function_Helper(char * function_name)
{
    if (!hDLL) {
        hDLL = LoadLibraryA("usb32nh.dll");
        if (!hDLL) return NULL; // no DLL !
        atexit(Unload_Function_Helper);
    }

    return (void *)GetProcAddress(hDLL, function_name);
}

#define LOADER_HELPER(x,t,r) \
    if (!fn##x) { \
        fn##x = (t)Load_Function_Helper(#x); \
    } \
    if (!fn##x) { \
        return r; \
    }

int _set_rec_buf_size(HANDLE hdl, char rx_number, unsigned short size, unsigned short* result, unsigned short* status)
{
    LOADER_HELPER(_set_rec_buf_size, SET_REC_BUF_SIZE_FUNCTION, 0);
    try {
        return (*fn_set_rec_buf_size)(hdl, rx_number, size, result, status);
    } catch (...) {
        ;
    }
    return 0;
}

int _config_rec(HANDLE hdl, char rx_number, unsigned short speed)
{
    LOADER_HELPER(_config_rec, CONFIG_REC_FUNCTION, 0);
    try {
        return (*fn_config_rec)(hdl, rx_number, speed);
    } catch (...) {
        ;
    }
    return 0;
}

int _config_trans(HANDLE hdl, char tx_number, unsigned short speed, unsigned short bitrate, unsigned short parity)
{
    LOADER_HELPER(_config_trans, CONFIG_TRANS_FUNCTION, 0);
    try {
        return (*fn_config_trans)(hdl, tx_number, speed, bitrate, parity);
    } catch (...) {
        ;
    }
    return 0;
}

int _get_fw_version(HANDLE hdl, short ipnum, unsigned short* vnum)
{
    LOADER_HELPER(_get_fw_version, GET_FW_VERSION_FUNCTION, 0);
    try {
        return (*fn_get_fw_version)(hdl, ipnum, vnum);
    } catch (...) {
        do {} while (0);
        ;
    }
    return 0;
}

int  _verify_iflib_version(unsigned short vnum)
{
    LOADER_HELPER(_verify_iflib_version, VERIFY_IFLIB_VERSION_FUNCTION, 0);
    try {
        return (*fn_verify_iflib_version)(vnum);
    } catch (...) {
        ;
    }
    return 0;
}

HANDLE _open_xpc(short lbn)
{
    LOADER_HELPER(_open_xpc, OPEN_XPC_FUNCTION, (HANDLE)INVALID_HANDLE_VALUE);
    try {
        return (*fn_open_xpc)(lbn);
    } catch (...) {
        ;
    }
    return 0;
}

int _activate_trans(HANDLE hdl, char tx_number, unsigned short act_count, tx_activation_t* buffer)
{
    LOADER_HELPER(_activate_trans, ACTIVATE_TRANS_FUNCTION, 0);
    try {
        return (*fn_activate_trans)(hdl, tx_number, act_count, buffer);
    } catch (...) {
        ;
    }
    return 0;
}

int _define_trans(HANDLE hdl, char tx_number, unsigned short def_count, tx_definition_t* buffer, unsigned short* result, unsigned short* timeslice)
{
    LOADER_HELPER(_define_trans, DEFINE_TRANS_FUNCTION, 0);
    try {
        return (*fn_define_trans)(hdl, tx_number, def_count, buffer, result, timeslice);
    } catch (...) {
        ;
    }
    return 0;
}

int _read_receiver_buffer(HANDLE hdl, char rx_number, rx_data_t* buffer, unsigned short* result, unsigned short* num_entry)
{
    LOADER_HELPER(_read_receiver_buffer, READ_RECEIVER_BUFFER_FUNCTION, 0);
    try {
        return (*fn_read_receiver_buffer)(hdl, rx_number, buffer, result, num_entry);
    } catch (...) {
        ;
    }
    return 0;
}

int _activate_rec(HANDLE hdl, char rx_number, unsigned short act_code, rx_mode_t* buffer, unsigned short* status)
{
    LOADER_HELPER(_activate_rec, ACTIVATE_REC_FUNCTION, 0);
    try {
        return (*fn_activate_rec)(hdl, rx_number, act_code, buffer, status);
    } catch (...) {
        ;
    }
    return 0;
}

// end of Stubs_for_TechSAT.c
