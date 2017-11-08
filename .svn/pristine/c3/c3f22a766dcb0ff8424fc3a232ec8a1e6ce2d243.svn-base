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
 * Module Name: tftps_api.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include "sockets.h"
#include "api.h"
#include "gconfig.h"
#include "platform_opts.h"
#include "tftps_api.h"
#include "upgrade.h"

/* NAMING CONSTANT DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */

/* LOCAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */

/* LOCAL SUBPROGRAM BODIES */

/* EXPORTED SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_InitImageBuffer()
 * Purpose : TFTP server init image buffer
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void TFTPS_InitImageBuffer(void)
{
	UPGRADE_InitImageBufDesc();
	
} /* End of TFTPS_InitImageBuffer() */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_ValidateFirmwareFileName()
 * Purpose : TFTP server validate firmware file name
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
S32_T TFTPS_ValidateFirmwareFileName(U8_T *fileName)
{
	/* TFTP server can validate uploaded firmware file by checking specified file name. */
#if (TFTPS_API_FILE_NAME_VALIDATION)
	if (strcmp(fileName, TFTPS_API_APPROVED_FILE_NAME))
	{
		return -1;
	}
#endif
	return 0;

} /* End of TFTPS_ValidateFirmwareFileName() */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_ValidateFirmwareFileHeader()
 * Purpose : TFTP server validate firmware file header
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
S32_T TFTPS_ValidateFirmwareFileHeader(U8_T *pPacketBuf, U32_T packetLen)
{
	S32_T	result;
	
	/* Do file header check */
	result = UPGRADE_CheckFileHeader(pPacketBuf, packetLen);
	if (result == UGSTATUS_ERR_SIGNATURE)
	{
		TFTPS_API_PRINT("Invalid signature\n\r");
	}
	else if (result == UGSTATUS_ERR_FILE_SIZE)
	{
		TFTPS_API_PRINT("Over maximum OTA file length!\n\r");
	}	  
	else if (result == UGSTATUS_ERR_IMAGE_SIZE)
	{
		TFTPS_API_PRINT("Unreasonable length\n\r");
	}
	else if (result == UGSTATUS_ERR_TARGET_ADDR)
	{
		TFTPS_API_PRINT("Upload failed, wrong image 2 address\n\r");
	}
	else if (result == UGSTATUS_ERR_IMAGE_TYPE)
	{
		TFTPS_API_PRINT("Invalid file type!\n\r");
	}
	else if (result < 0)
	{
		TFTPS_API_PRINT("Buffer error(%d)\n\r", result);
	}
	
	return result;
	
} /* End of TFTPS_ValidateFirmwareFileHeader() */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_CopyFirmwareContentToImageBuffer()
 * Purpose : TFTP server copy firmware content(from payload) to image buffer
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
S32_T TFTPS_CopyFirmwareContentToImageBuffer(U8_T *pPacketBuf, U32_T packetLen)
{
	S32_T	result;
	
	/* Copy payload to image buffer and check result */
	result = UPGRADE_CopyToImageBuf(pPacketBuf, packetLen);
	if (result == UGSTATUS_ERR_IMAGE_SIZE)
	{
		TFTPS_API_PRINT("Unreasonable length\n\r");
	}
	else if (result < 0)
	{
		TFTPS_API_PRINT("Buffer error(%d)\n\r", result);
	}
	
	return result;
	
} /* End of TFTPS_CopyFirmwareContentToImageBuffer() */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_IntegrityCheck()
 * Purpose : TFTP server do integrity check in image buffer
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
S32_T TFTPS_IntegrityCheck(void)
{
	S32_T	result;
	
	/* Do integrity check */
	result = UPGRADE_IntegrityCheck();
	if (result != UGSTATUS_OK)
	{
		TFTPS_API_PRINT("Integrity check fail!\n\r");
	}
	
	return result;
	
} /* End of TFTPS_IntegrityCheck() */

/*
 * ----------------------------------------------------------------------------
 * Function: TFTPS_WriteFirmwareToFlash()
 * Purpose : TFTP server write firmware to flash
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
S32_T TFTPS_WriteFirmwareToFlash(U32_T writeLen)
{
	S32_T	result;
	
	/* Write firmware to flash and check result */
	result = UPGRADE_ImageProgramming(BLOCKED_UPGRADE);
	if (result == UGSTATUS_ERR_SIGNATURE)
	{
		TFTPS_API_PRINT("Write signature fail\n\r");
	}
	else if (result == UGSTATUS_ERR_WRITE_FLASH)
	{
		TFTPS_API_PRINT("Write flash fail\n\r");	  
	}
	else if (result < 0)
	{
		TFTPS_API_PRINT("Write flash error(%d)\n\r", result);
	}
	
	return result;
	
} /* End of TFTPS_WriteFirmwareToFlash() */

/* End of tftps_api.c */