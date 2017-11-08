/*
 *********************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: gpios.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __GPIOS_H__
#define __GPIOS_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define GPIOS_PRINT_HEADER						"GPIOS: "
#define GPIOS_RESTART_AFTER_CONFIG_SAVE			1
#define GPIOS_AP_MODE_BLINKIN_INTERVAL			2000	/* ms */
#define GPIOS_WPS_TRIGGER_TIME					1000	/* ms */
#define GPIOS_SC_TRIGGER_TIME					3000	/* ms */
#define GPIOS_LOAD_GCONFIG_DEFAULT_TRIGGER_TIME	15000	/* ms */

#define MBIDefault 1
#define Cmd23Default 2
extern u8 SwitchMemoryState; //20170801 Craig

#define JTAGoff_GPIOEnable 1            //20170809 Craig

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void GPIOS_ResetLEDIndicator(void);
#if (CONFIG_CLOUD_SELECT != CLOUD_DISABLE)
void GPIOS_BeepCtrl(u8 turn_on);
#endif
#endif /* __GPIOS_H__ */
