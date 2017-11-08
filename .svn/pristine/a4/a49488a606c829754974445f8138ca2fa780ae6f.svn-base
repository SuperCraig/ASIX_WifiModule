/*
 ******************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended 
 *     publication of such source code.
 ******************************************************************************
 */
/*=============================================================================
 * Module Name: ths.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __THS_H__
#define __THS_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define THS_DEBUG_ENABLE 	0
#define THS_I2C_SLAVE_ADDR 	0x40

	/* Si7006 THS commands */
#define Si7006_RH_HOLD_MASTER		0xE5
#define Si7006_RH_NO_HOLD_MASTER	0xF5
#define Si7006_TEMP_HOLD_MASTER		0xE3
#define Si7006_TEMP_NO_HOLD_MASTER	0xF3
#define Si7006_RD_TEMP_PRIV_RH		0xE0
#define Si7006_RESET				0xFE
#define Si7006_WR_USER_REG_1		0xE6
#define Si7006_RD_USER_REG_1		0xE7
#define Si7006_RD_EID_BYTE1			0xFA0F
#define Si7006_RD_EID_BYTE2			0xFCC9
#define Si7006_RD_FW_VER			0x84B8

/* TYPE DECLARATIONS */
typedef struct _THS_CTRL
{
	u8	FirmwareVer;
	void *pObj;
} THS_CTRL;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void THS_Init(void *pObj);
u8 THS_ReadFirmwareVersion(u8 *pFirmwareVer);
u8 THS_GetTemperature(float *pTemperature);
u8 THS_GetRelativeHumidity(float *pRH);
#endif /* End of __THS_H__ */

/* End of rtc.h */
