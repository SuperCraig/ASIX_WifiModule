/*
 *********************************************************************************
 *     Copyright (c) 2015   ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
/*================================================================================
 * Module Name : als3001.h
 * Purpose     : 
 * Author      : 
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef ALS3001_H
#define ALS3001_H

/* INCLUDE FILE DECLARATIONS */


/* NAMING CONSTANT DECLARATIONS */
#define ALS_SLAVE_ADDR				0x45
#define ALS_REG_RESULT				0x00
#define ALS_REG_CONFIG				0x01
#define ALS_REG_LO_LMT				0x02
#define ALS_REG_HI_LMT				0x03
#define ALS_REG_MANUFACTURER_ID		0x7E
#define ALS_REG_DEVICE_ID			0x7F

#define CFG_RANGE_NUM				0xF000 /* [15..12] Range Number */
#define CFG_CONV_TIME				0x0800 /* [11] Conversion Time */
#define CFG_CONV_MODE				0x0600 /* [10..9] Mode of Conversion */
#define CFG_OVF						0x0100 /* [8] Overflow */
#define CFG_CONV_RDY				0x0080 /* [7] Conversion Ready Field */
#define CFG_FLAG_HI					0x0040 /* [6] Flag High */
#define CFG_FLAG_LO					0x0020 /* [5] Flag Low */
#define CFG_LATCH					0x0010 /* [4] Latch */
#define CFG_POLARITY				0x0008 /* [3] Polarity */
#define CFG_MASK_EXPT				0x0004 /* [2] Mask Exponent */
#define CFG_FAULT_CNT				0x0003 /* [1..0] Fault Count */

#define ALS_REG_ADDR_SIZE			1
#define ALS_REG_DATA_SIZE			2
#define ALS_WRITE_SEQ_LENGTH		(ALS_REG_ADDR_SIZE + ALS_REG_DATA_SIZE)


/* TYPE DECLARATIONS */
typedef struct _ALS_CTRL
{
	u16		ManufId;
	u16		DeviceId;
	void	*pBusObj;
} ALS_CTRL;


/* GLOBAL VARIABLES */


/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void ALS_Init(void *pObj);
u8 ALS_GetManufacturerId(u16 *idManuf);
u8 ALS_GetDeviceId(u16 *idDevice);


#endif /* End of ALS3001_H */
