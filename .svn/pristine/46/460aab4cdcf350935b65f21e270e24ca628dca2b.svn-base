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
 /*============================================================================
 * Module Name: ths.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "basic_types.h"
#include <string.h>
#include "i2c_api.h"
#include "gconfig.h"
#include "ths.h"

/* NAMING CONSTANT DECLARATIONS */

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */
THS_CTRL THS_Ctrl;

/* LOCAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */

/*
 * ----------------------------------------------------------------------------
 * Function Name: THS_Init
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void THS_Init(void *pObj)
{
	memset((u8*)&THS_Ctrl, 0, sizeof(THS_CTRL));

	THS_Ctrl.pObj = pObj;
} /* End of THS_Init() */


/*
 * ----------------------------------------------------------------------------
 * Function Name: THS_ReadFirmwareVersion
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 THS_ReadFirmwareVersion(u8 *pFirmwareVer)
{
	if (i2c_sem_read(THS_Ctrl.pObj
				, THS_I2C_SLAVE_ADDR
				, Si7006_RD_FW_VER
				, (u8*)&THS_Ctrl.FirmwareVer
				, 1
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG | I2C_EXD_USER_TWOB) == FALSE)		
	{
		return FALSE;
	}
	*pFirmwareVer = THS_Ctrl.FirmwareVer;
	return TRUE;
} /* End of THS_ReadFirmwareVersion() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: THS_GetTemperature
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 THS_GetTemperature(float *pTemperature)
{
	u16 tmp16;
	if (i2c_sem_read(THS_Ctrl.pObj
				, THS_I2C_SLAVE_ADDR
				, Si7006_TEMP_HOLD_MASTER
				, (u8*)&tmp16
				, 2
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	tmp16 = WAP16(tmp16);
	*pTemperature = ((tmp16 * 175.72)/65536) - 46.85;
	return TRUE;
} /* End of THS_GetTemperature() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: THS_GetRelativeHumidity
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 THS_GetRelativeHumidity(float *pRH)
{
  	u16 tmp16;
	if (i2c_sem_read(THS_Ctrl.pObj
				, THS_I2C_SLAVE_ADDR
				, Si7006_RH_HOLD_MASTER
				, (u8*)&tmp16
				, 2
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	tmp16 = WAP16(tmp16);
	*pRH = ((tmp16 * 125)/65536) - 6;
	return TRUE;
} /* End of THS_GetRelativeHumidity() */

/* End of ths.c */
