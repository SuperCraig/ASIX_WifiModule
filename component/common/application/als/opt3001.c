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
 * Module Name: opt3001.c
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
#include "opt3001.h"

/* NAMING CONSTANT DECLARATIONS */


/* GLOBAL VARIABLES DECLARATIONS */
extern xSemaphoreHandle i2cMstMutex_0;


/* LOCAL VARIABLES DECLARATIONS */
//static xSemaphoreHandle		alsMutex;
static ALS_CTRL				alsCtrl;


/* LOCAL SUBPROGRAM DECLARATIONS */


/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function: u8 als_WriteReg(u8 regAddr, u16 regData)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 als_WriteReg(u8 regAddr, u16 regData)
{
	i2c_t *obj = alsCtrl.pBusObj;
	PSAL_I2C_HND pi2ccfg;
	u8	pByte[3];
	u16	ret;

	while(xSemaphoreTake(i2cMstMutex_0, portMAX_DELAY) != pdTRUE);

	pi2ccfg = (PSAL_I2C_HND)&(obj->SalI2CMngtAdpt.pSalHndPriv->SalI2CHndPriv);
	pi2ccfg->I2CExd = I2C_EXD_MTR_ADDR_RTY;

	pByte[0] = (u8)regAddr;
	pByte[1] = (u8)(regData >> 8);
	pByte[2] = (u8)regData;
	ret = i2c_write(alsCtrl.pBusObj, ALS_SLAVE_ADDR, &pByte[0], ALS_WRITE_SEQ_LENGTH, 1);

	xSemaphoreGive(i2cMstMutex_0);
	if (ret < ALS_WRITE_SEQ_LENGTH)
		return FALSE;

	return TRUE;
}

/*
 * ----------------------------------------------------------------------------
 * Function: u8 als_ReadReg(u8 regAddr, u16 *pRegData)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 als_ReadReg(u8 regAddr, u16 *pRegData)
{
	i2c_t *obj = alsCtrl.pBusObj;
	PSAL_I2C_HND pi2ccfg;
	u16	ret1, ret2;
	u8	pByte[2];

	while(xSemaphoreTake(i2cMstMutex_0, portMAX_DELAY) != pdTRUE);
	
	pi2ccfg = (PSAL_I2C_HND)&(obj->SalI2CMngtAdpt.pSalHndPriv->SalI2CHndPriv);
	pi2ccfg->I2CExd = I2C_EXD_MTR_ADDR_RTY;

	pByte[0] = (u8)regAddr;
	ret1 = i2c_write(alsCtrl.pBusObj, ALS_SLAVE_ADDR, &pByte[0], ALS_REG_ADDR_SIZE, 1);
	ret2 = i2c_read(alsCtrl.pBusObj, ALS_SLAVE_ADDR, (char*)&pByte[0], ALS_REG_DATA_SIZE, 1);
	xSemaphoreGive(i2cMstMutex_0);

	if ((ret1 < ALS_REG_ADDR_SIZE) || (ret2 < ALS_REG_DATA_SIZE))
		return FALSE;

	*pRegData = ((u16)pByte[0] << 8) | (u16)pByte[1];
	
	return TRUE;
}


/* EXPORTED SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: ALS_Init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ALS_Init(void *pObj)
{
//	alsMutex = xSemaphoreCreateMutex();
	memset(&alsCtrl, 0, sizeof(ALS_CTRL));
	alsCtrl.pBusObj = pObj;
}

/*
 * ----------------------------------------------------------------------------
 * Function: void ALS_SetConfigureReg(u16 regData)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void ALS_SetConfigureReg(u16 regData)
{
	als_WriteReg(ALS_REG_CONFIG, regData);
}

/*
 * ----------------------------------------------------------------------------
 * Function: u8 ALS_GetManufacturerId(u16 idManuf)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 ALS_GetManufacturerId(u16 *idManuf)
{
	if (als_ReadReg(ALS_REG_MANUFACTURER_ID, idManuf) == FALSE)
		return FALSE;
	
	return TRUE;	
}

/*
 * ----------------------------------------------------------------------------
 * Function: u8 ALS_GetDeviceId(u16 idDevice)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 ALS_GetDeviceId(u16 *idDevice)
{
	if (als_ReadReg(ALS_REG_DEVICE_ID, idDevice) == FALSE)
		return FALSE;

	return TRUE;	
}

/*
 * ----------------------------------------------------------------------------
 * Function: u8 ALS_GetResultLux(float *pLuxValue)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 ALS_GetResultLux(float *pLuxValue)
{
	u16		regData16b = 0;
	u16		exp, result;

	if (als_ReadReg(ALS_REG_RESULT, &regData16b) == FALSE)
		return FALSE;
	printf("Result: %x\n\r", regData16b);
	result = regData16b & 0x0FFF;
	exp = (regData16b & 0xF000) >> 12;
	*pLuxValue = (float)(0.01 * (1 << exp) * result);

	return TRUE;	
}

/***
example :
	ALS_SetConfigureReg(0xc400);
	ALS_GetManufacturerId(&reg16b);
	printf("\n\rManufId: %x\n\r", reg16b);
	ALS_GetDeviceId(&reg16b);
	printf("\n\rDeviceId: %x\n\r", reg16b);
	ALS_GetResultLux(&lux);
	printf("\n\rLuxValue: %f\n\r", lux);
***/