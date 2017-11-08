/*
 *********************************************************************************
 *     Copyright (c) 2016	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: duia.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __DUIA_H__
#define __DUIA_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define DUIA_DEBUG_ENABLE				1   
#define DUIA_SECTOR_BUF_SIZE			4096
#define DUIA_IMAGE_SIGNATURE			"81958711"
#define DUIA_CLR_IMAGE_SIGNATURE		"00000000"   
#define DUIA_UPGRADE_BUF_ADDR			0x30100000
#define DUIA_SYSTEM_DATA_ADDR			0x9000
#define DUIA_UPGRADE_IMAGE_1_ADDR		0xB000
#define DUIA_UPGRADE_IMAGE_SIZE			0xF7000   

/* MACRO DECLARATIONS */
#if (DUIA_DEBUG_ENABLE)
	#define DUIA_PRINT(f, ...)	{printf("DUIA: "); printf(f, ## __VA_ARGS__);}
#else
	#define DUIA_PRINT(f, ...)	{}
#endif
   
/* TYPE DECLARATIONS */
typedef enum {
	UPGRADE_IMAGE_1 = 0,
	UPGRADE_IMAGE_2,
} DUIA_UPGRADE_IMAGE_INDEX;

typedef struct
{
	u32		Length;
	u32		Address;
	u8		Signature[8];
} DUIA_IMAGE_HEADER;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int DUIA_CheckRuntimeImage(void);
int DUIA_SwitchUpgradeImage(DUIA_UPGRADE_IMAGE_INDEX upgradeImgId);
u32 DUIA_Checksum32(u32 *buffer, u32 length);
int DUIA_ValidateUpgradeImage(DUIA_UPGRADE_IMAGE_INDEX upgradeImgId);
u32 DUIA_GetUpgradeImage2Offset(void);
#endif /* __DUIA_H__ */

