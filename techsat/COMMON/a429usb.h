/*
 *
 * $Header: /CPSoftware/Utilities/mcdusim/techsat/COMMON/a429usb.h,v 1.1 2009/11/20 13:39:01 ems-tcanada\varanda.m Exp $
 *
 */

/*
 * @COMPONENT	pcc
 * @FILE	$Source: /CPSoftware/Utilities/mcdusim/techsat/COMMON/a429usb.h,v $
 * @DATE	$Date: 2009/11/20 13:39:01 $
 * @REVISION	$Revision: 1.1 $
 * @AUTHOR	TechS.A.T Brian Stuart
 * @COPYRIGHT	-----------------------------------------------------
 *              Copyright (c) 2006 TechS.A.T GmbH
 *		All rights reserved.
 *		-----------------------------------------------------
 *		Technische Systeme fuer Avionik und Test
 *		Gruber Strasse 46c
 *		D-85586 Poing
 *		Tel. +49-8121-703-0
 *		Fax  +49-8121-703-177
 *		-----------------------------------------------------
 * @CONTENT	
 *	
 *
 *
 */

#ifndef _A429USB_H
#define _A429USB_H

#include "a429xpc.h"		// common to all board types

#ifdef __cplusplus
extern "C" {    
#endif
/*
 * USB-specific defines
 */

/*
 * Function prototypes
 */

FEXTINT    _reset_selftest(HANDLE hdl, unsigned short* selftest_result,
		    unsigned short *errbits);

FEXTINT    _get_trigger_state(HANDLE hdl, 
			      unsigned short trig_ID,
                              unsigned short* trig_flag);

FEXTINT    _get_firmware_version(HANDLE hdl,
				 unsigned short* version_number);

FEXTINT    _get_ticks_mpcboard(HANDLE hdl, 
			       unsigned long* elapsed_ms);

FEXTINT    _usb_get_eeprom_hw_config(HANDLE hdl, HWconfig *pbuff);

FEXTINT    _set_parity_mode(HANDLE hdl, char tr, unsigned int parity);

FEXTINT _usb_set_discretes(HANDLE hdl, unsigned short mask, unsigned short data);

FEXTINT _usb_get_discretes(HANDLE hdl, unsigned short *data);

FEXTINT _xpc_get_lasterror();


#ifdef __cplusplus
}
#endif

#endif 	/* _A429USB_H */
