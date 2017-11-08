/*
 ******************************************************************************
 *     Copyright (c) 2016	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
 /*============================================================================
 * Module Name: upgrade.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __UPGRADE_H__
#define __UPGRADE_H__

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
   
/* NAMING CONSTANT DECLARATIONS */
#define UPGRADE_DEBUG_ENABLE	1
#if (UPGRADE_DEBUG_ENABLE)
	#define UPGRADE_PRINT(f, ...)	{printf("UPGRADE: "); printf(f, ## __VA_ARGS__);}
#else
	#define UPGRADE_PRINT(f, ...)	{}
#endif

#define UPGRADE_ASIX_RT_HDR_SIZE	32
#define UPGRADE_RTL_IMG_HDR_SIZE	16
#define UPGRADE_OTA_FW_SIGNATURE	"ASIX"

#define	UGSTATUS_PROG_DONE			1
#define	UGSTATUS_OK					0
#define	UGSTATUS_ERR				-1		
#define	UGSTATUS_ERR_SIGNATURE		-2
#define	UGSTATUS_ERR_TARGET_ADDR	-3		
#define	UGSTATUS_ERR_IMAGE_TYPE		-4
#define	UGSTATUS_ERR_IMAGE_SIZE		-5
#define	UGSTATUS_ERR_FILE_SIZE		-6   
#define	UGSTATUS_ERR_CHECKSUM		-7
#define	UGSTATUS_ERR_WRITE_FLASH	-8	
	
/* TYPE DECLARATIONS */
typedef enum
{
	UGSTATE_INITED = 0,	
	UGSTATE_READY_TO_UPGARDE,
	UGSTATE_PROGRAMMING,
} UPGRADE_STATE;

typedef enum
{
	UGTYPE_OTA_FIRMWARE = 0,
	MAX_IMAGE_TYPE,
} UPGRADE_TYPE;

typedef enum
{
	BLOCKED_UPGRADE = 0,
	NONBLOCKED_UPGRADE,	
} UPGRADE_MODE;

typedef struct _UPGRADE_RUNTIME_HEADER
{
	u8		Signature[4];
	u16		ImageType;
	u16		Compress;
	u32		ContentOffset;
	u32		FileLen;
	u16		Checksum;
	u16		Reserved0;
	u32		Checksum32;	
	u32		Reserved1[2];
} UPGRADE_RT_HEAD;

typedef struct _UPGRADE_RUNTIME_TAILER
{
	u32		Checksum32;	
} UPGRADE_RT_TAIL;

typedef struct _UPGRADE_IMAGE_DESCRIPTOR
{
	UPGRADE_STATE	State;
	UPGRADE_TYPE	Type;
	u32		ImageBufSize;
	u32		ImageSize;
	u8		*pImageAddr;
	u32		TargetAddr;	
	u32		CopiedLen;
	u32		WrittenLen;
	u32		Offset;
	u32		UpgradeMode;
	u8		RuntimeImage;	
} UPGRADE_IMAGE_DESCRIPTOR;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int UPGRADE_Lock(void);
void UPGRADE_InitImageBufDesc(void);
void UPGRADE_InitImageBuf(void);
int UPGRADE_CheckFileHeader(u8 *pFile, u32 Len);
int UPGRADE_CopyToImageBuf(u8 *pFile, u32 Len);
int UPGRADE_IntegrityCheck(void);
int UPGRADE_ImageProgramming(UPGRADE_MODE mode);
void UPGRADE_UnLock(void);
#endif /* End of __UPGRADE_H__ */


/* End of upgrade.h */
