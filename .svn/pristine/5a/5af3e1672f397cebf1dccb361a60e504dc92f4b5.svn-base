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
 * Module Name: upgrade.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "upgrade.h"
#include "gconfig.h"
#include <string.h>
#include <stdio.h>
#include "flash_api.h"
#include "misc.h"
#include "lwip_netconf.h"
#include "duia.h"
#include "platform_opts.h"

/* NAMING CONSTANT DECLARATIONS */
#define UPGRADE_IMAGE_BUF_ADDR		0x30100000
#define UPGRADE_IMAGE_BUF_SIZE		0xffffC
#define UPGRADE_MAX_IMAGE_SIZE		1011712//988KB
#define MAX_NONBLOCK_PROCESS_BYTES	(4096*6)

/* LOCAL VARIABLES DECLARATIONS */
static UPGRADE_IMAGE_DESCRIPTOR upgrade_desc;
u8 upgrade_imageBuf[UPGRADE_IMAGE_BUF_SIZE] @ UPGRADE_IMAGE_BUF_ADDR;//Must have start and end addresses that are even multiples of 4.
	  
/* GLOBAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */
static u32 upgrade_checksum(u32 addr, u32 length);

/* LOCAL SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function Name: upgrade_checksum()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static u32 upgrade_checksum(u32 addr, u32 length)
{
	u32 i;
	u32 count = (length >> 1);
	u32 cksum = 0;
	u16 *buffer = (u16*)addr;	
	
	for (i = 0; i < count; i++)
	{
		cksum += *buffer++;
		if (0xF0000000 & cksum)
		{
			while (0xFFFF0000 & cksum)
			{
				cksum = (cksum >> 16) + (cksum & 0xffff);
			}
		}
	}

	if (length % 2)
		cksum += *(unsigned char*)buffer;

	while (0xFFFF0000 & cksum)
	{
		cksum = (cksum >> 16) + (cksum & 0xffff);
	}
	return cksum;	
} /* End of upgrade_checksum() */

/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_Lock()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int UPGRADE_Lock(void)
{
	int upgrade_idle;
	  
	upgrade_idle = MISC_LockFwUpgradeSection();
	if (upgrade_idle)
	{
		UPGRADE_PRINT("Lock image buffer\r\n");	  
	}
	return upgrade_idle;
} /* End of UPGRADE_Lock() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_UnLock()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void UPGRADE_UnLock(void)
{
	MISC_UnLockFwUpgradeSection();
	UPGRADE_PRINT("UnLock image buffer\r\n");
} /* End of UPGRADE_UnLock() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_InitImageBuf()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void UPGRADE_InitImageBuf(void)
{
	memset((u8*)&upgrade_desc, 0, sizeof(upgrade_desc));
	upgrade_desc.pImageAddr = upgrade_imageBuf;
	upgrade_desc.ImageBufSize = UPGRADE_IMAGE_BUF_SIZE;
	/* Get upgraded image 2 addr from offset field in system data sector */
	upgrade_desc.Offset = GCONFIG_GetUpgradeImage2Offset();	
	upgrade_desc.UpgradeMode = GCONFIG_GetUpgradeMode();
	upgrade_desc.RuntimeImage = DUIA_CheckRuntimeImage();
	if (upgrade_desc.UpgradeMode == SINGLE_UPGRADE_IMAGE_MODE)
	{
		/* will write to upgrade image 2(obtain it's address from offset value) */	  
		upgrade_desc.TargetAddr	= upgrade_desc.Offset;
	}
	else
	{
#if CONFIG_UPGRADE_IMAGE_2_ONLY
		upgrade_desc.TargetAddr	= upgrade_desc.Offset;
#else
		if (upgrade_desc.RuntimeImage == UPGRADE_IMAGE_2)
		{
			/* will write to upgrade image 1 */
			upgrade_desc.TargetAddr	= FLASH_UPGRADE_IMAGE_1_ADDR;
		}
		else
		{
			/* will write to upgrade image 2(obtain it's address from offset value) */	  
			upgrade_desc.TargetAddr	= upgrade_desc.Offset;
		}
#endif
	}


} /* End of UPGRADE_InitImageBuf() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_InitImageBufDesc()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void UPGRADE_InitImageBufDesc(void)
{
	upgrade_desc.State = UGSTATE_INITED;
	upgrade_desc.Type = UGTYPE_OTA_FIRMWARE;
	upgrade_desc.CopiedLen = 0;
	upgrade_desc.WrittenLen = 0;

} /* End of UPGRADE_InitImageBufDesc() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_CheckFileHeader()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int UPGRADE_CheckFileHeader(u8 *pFile, u32 Len)
{
	UPGRADE_RT_HEAD	*pRtHdr = (UPGRADE_RT_HEAD*)pFile;  

	if (Len < (UPGRADE_ASIX_RT_HDR_SIZE + UPGRADE_RTL_IMG_HDR_SIZE))
	{
		return UGSTATUS_ERR;
	}
		
	pRtHdr->FileLen = htonl(pRtHdr->FileLen);
	
	/* Check signature */
	if (memcmp(pRtHdr->Signature, UPGRADE_OTA_FW_SIGNATURE, strlen(UPGRADE_OTA_FW_SIGNATURE)))
	{
		return UGSTATUS_ERR_SIGNATURE;
	}

	/* Check file length */
	if (pRtHdr->FileLen >= upgrade_desc.ImageBufSize)
	{
		return UGSTATUS_ERR_FILE_SIZE;
	}	

	/*
		Validate ram2 length in Realtek's ram 2 header
		The ram2 length can not bigger than ram2+ram3 length in ASIX's header
	*/
	if (pRtHdr->FileLen < (*((u32 *)(pFile+UPGRADE_ASIX_RT_HDR_SIZE)) + UPGRADE_RTL_IMG_HDR_SIZE + UPGRADE_ASIX_RT_HDR_SIZE))
	{
		return UGSTATUS_ERR_IMAGE_SIZE;
	}

	/* Check image size */
	upgrade_desc.ImageSize = pRtHdr->FileLen - UPGRADE_ASIX_RT_HDR_SIZE;
	if (upgrade_desc.ImageSize >= UPGRADE_MAX_IMAGE_SIZE)
	{
		return UGSTATUS_ERR_IMAGE_SIZE;
	}

	/* Check offset value */
	if (!(upgrade_desc.UpgradeMode == DUAL_UPGRADE_IMAGE_MODE && upgrade_desc.RuntimeImage == UPGRADE_IMAGE_2))
	{
		if (upgrade_desc.Offset < FLASH_UPGRADE_IMAGE_1_ADDR ||	upgrade_desc.Offset == 0xFFFFFFFF)
		{
			return UGSTATUS_ERR_TARGET_ADDR;
		}
	}
	else if ((FLASH_UPGRADE_IMAGE_1_ADDR + upgrade_desc.ImageSize) > upgrade_desc.Offset)
	{
		return UGSTATUS_ERR_TARGET_ADDR;
	}
	
	/* Check image type */
	if (upgrade_desc.UpgradeMode == DUAL_UPGRADE_IMAGE_MODE)
	{
		if (pRtHdr->ImageType != 0x0001)
		{
			return UGSTATUS_ERR_IMAGE_TYPE;
		}
	}
	UPGRADE_PRINT("Check file header ok! FileLen=0x%lx, Checksum32=0x%08lx\r\n", pRtHdr->FileLen, pRtHdr->Checksum32);		

	return pRtHdr->FileLen;
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_CopyToImageBuf()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int UPGRADE_CopyToImageBuf(u8 *pFile, u32 Len)
{
	UPGRADE_RT_HEAD	*pRtHdr = (UPGRADE_RT_HEAD*)upgrade_desc.pImageAddr;  
	
	/* Check parameters */
	if (pFile == 0)
	{
		return UGSTATUS_ERR;
	}
	
	if (upgrade_desc.CopiedLen == 0)
	{
		pRtHdr = (UPGRADE_RT_HEAD*)pFile;
		UPGRADE_PRINT("Copy to image buffer start... FileLen=%d, ImageSize=%d, ts=%d\r\n", pRtHdr->FileLen, upgrade_desc.ImageSize, xTaskGetTickCount());
	}

	/* Check copied length */
	if (pRtHdr->FileLen >= upgrade_desc.ImageBufSize || ((upgrade_desc.CopiedLen + Len) > pRtHdr->FileLen))
	{
		return UGSTATUS_ERR_IMAGE_SIZE;
	}
	
	/* Copy image to buffer */
	memcpy((u8*)(upgrade_desc.pImageAddr + upgrade_desc.CopiedLen), pFile, Len);
	upgrade_desc.CopiedLen += Len;

#if UPGRADE_DEBUG_ENABLE		
	/* Is last copy ? */
	if (upgrade_desc.CopiedLen >= pRtHdr->FileLen)
	{
		UPGRADE_PRINT("Copy to image buffer ok! CopiedLen=%d, ts=%d\r\n", upgrade_desc.CopiedLen, xTaskGetTickCount());
	}
#endif
	
	return Len;

} /* End of UPGRADE_CopyToImageBuf() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_IntegrityCheck()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int UPGRADE_IntegrityCheck(void)
{
	u32 checksum;
	UPGRADE_RT_HEAD	*pRtHdr = (UPGRADE_RT_HEAD*)upgrade_desc.pImageAddr; 
	
	/* Integrity check */
	UPGRADE_PRINT("Integrity check start...ts=%d\r\n", xTaskGetTickCount());

	if (pRtHdr->ImageType == 0)	
	{
		checksum = upgrade_checksum((u32)upgrade_desc.pImageAddr+sizeof(UPGRADE_RT_HEAD), upgrade_desc.ImageSize);
		checksum = (unsigned short)(~checksum & 0xffff);
		if (checksum != pRtHdr->Checksum)
		{
			UPGRADE_PRINT("Integrity check fail! ref_checksum=0x%04x != cal_checksum=0x%04x\r\n", pRtHdr->Checksum, checksum);
			return UGSTATUS_ERR_CHECKSUM;
		}
	}
	else
	{
		checksum = DUIA_Checksum32((u32*)(upgrade_desc.pImageAddr+sizeof(UPGRADE_RT_HEAD)), upgrade_desc.ImageSize);
		if (checksum != ~pRtHdr->Checksum32)
		{
			UPGRADE_PRINT("Integrity check fail! ref_checksum=0x%08lx != cal_checksum=0x%08lx\r\n", pRtHdr->Checksum32, checksum);
			return UGSTATUS_ERR_CHECKSUM; 
		}
	}
	UPGRADE_PRINT("Integrity check ok! ts=%d\r\n", xTaskGetTickCount());
	upgrade_desc.State = UGSTATE_READY_TO_UPGARDE;
	return UGSTATUS_OK;	
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: UPGRADE_ImageProgramming()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int UPGRADE_ImageProgramming(UPGRADE_MODE mode)
{
	flash_t flash;
	int ret;
	u32 i, sector_num, write_len;
	u8 signature[10];
#if UPGRADE_DEBUG_ENABLE	
	u8 tmp[40];
#endif
	
	/* Check state */
	if (upgrade_desc.State != UGSTATE_READY_TO_UPGARDE &&
		upgrade_desc.State != UGSTATE_PROGRAMMING)
	{
		return UGSTATUS_ERR;
	}

	if (upgrade_desc.State == UGSTATE_READY_TO_UPGARDE)
	{
		/* Erase flash memory */
		sector_num = ((upgrade_desc.ImageSize - 1)/4096) + 1;	  
		UPGRADE_PRINT("Erase flash start... address=0x%lx, sector num=%d, ts=%d\r\n", upgrade_desc.TargetAddr, sector_num, xTaskGetTickCount());	  		
		for(i = 0; i < sector_num; i++)
		{
			flash_erase_sector(&flash, upgrade_desc.TargetAddr + i * 4096);
		}
		UPGRADE_PRINT("Erase flash ok! ts=%d\r\n", xTaskGetTickCount());
		upgrade_desc.WrittenLen = 0;
		upgrade_desc.State = UGSTATE_PROGRAMMING;
		
		UPGRADE_PRINT("Write flash start... address=0x%lx, ImageSize=%d, ts=%d\r\n", upgrade_desc.TargetAddr, upgrade_desc.ImageSize, xTaskGetTickCount());
#if UPGRADE_DEBUG_ENABLE
		flash_stream_read(&flash, upgrade_desc.TargetAddr, 10, tmp);
		printf("Dump flash header1: 0x");		
		for (i=0; i<10; i++)
		{
			printf("%02x ", tmp[i]);
		}
		printf("\r\n");		
		
		flash_stream_read(&flash, upgrade_desc.TargetAddr+upgrade_desc.ImageSize-10, 10, tmp);
		printf("Dump flash tail1: 0x");		
		for (i=0; i<10; i++)
		{
			printf("%02x ", tmp[i]);
		}
		printf("\r\n");		
#endif
		if (mode == NONBLOCKED_UPGRADE)		
		{
			ret = 0;
			goto UPGRADE_ImageProgramming_Exit;
		}
	}
	
	/* Image programming */
	write_len = upgrade_desc.ImageSize - upgrade_desc.WrittenLen;
	if (mode == NONBLOCKED_UPGRADE)
	{	
		if (write_len > MAX_NONBLOCK_PROCESS_BYTES)
		{
			write_len = MAX_NONBLOCK_PROCESS_BYTES;
		}
	}	
	if (flash_stream_write(&flash, upgrade_desc.TargetAddr+upgrade_desc.WrittenLen, write_len, upgrade_desc.pImageAddr+sizeof(UPGRADE_RT_HEAD)+upgrade_desc.WrittenLen) < 0)
	{
		ret = UGSTATUS_ERR_WRITE_FLASH;
		goto UPGRADE_ImageProgramming_Exit;
	}
	upgrade_desc.WrittenLen	+= write_len;
	ret = write_len;
			
	/* Check programming is completely done */
	if (upgrade_desc.WrittenLen >= upgrade_desc.ImageSize)	
	{
		/* Write signature */
		flash_write_word(&flash, upgrade_desc.TargetAddr + 8, 0x35393138);
		flash_write_word(&flash, upgrade_desc.TargetAddr + 12, 0x31313738);
		UPGRADE_PRINT("Write flash ok! WrittenLen=%d, ts=%d\r\n", upgrade_desc.WrittenLen, xTaskGetTickCount());		
				
		/* Check signature */
		flash_read_word(&flash, upgrade_desc.TargetAddr + 8, &signature[0]);
		flash_read_word(&flash, upgrade_desc.TargetAddr + 12, &signature[4]);

		if (*((u32*)&signature[0]) != 0x35393138 || *((u32*)&signature[4]) != 0x31313738)
		{
			ret = UGSTATUS_ERR_SIGNATURE;
		}
		
		/* Switch to upgrade image 1 */
		if (upgrade_desc.TargetAddr == FLASH_UPGRADE_IMAGE_1_ADDR)
		{
			DUIA_SwitchUpgradeImage(UPGRADE_IMAGE_1);
		}
#if UPGRADE_DEBUG_ENABLE
		flash_stream_read(&flash, upgrade_desc.TargetAddr, 10, tmp);
		printf("Dump flash header2: 0x");		
		for (i=0; i<10; i++)
		{
			printf("%02x ", tmp[i]);
		}
		printf("\r\n");			
		flash_stream_read(&flash, upgrade_desc.TargetAddr+upgrade_desc.ImageSize-10, 10, tmp);
		printf("Dump flash2: 0x");		
		for (i=0; i<10; i++)
		{
			printf("%02x ", tmp[i]);
		}
		printf("\r\n");		
#endif		
	}		
	
UPGRADE_ImageProgramming_Exit:
	return ret;

} /* End of UPGRADE_ImageProgramming() */

/* End of upgrade.c */


