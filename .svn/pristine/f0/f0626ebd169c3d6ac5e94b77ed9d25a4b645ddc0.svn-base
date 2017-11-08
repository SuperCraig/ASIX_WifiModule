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
 * Module name: tftps_api.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __TFTPS_API_H__
#define __TFTPS_API_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
/* Program setting */
#define TFTPS_API_DEBUG_ENABLE			1				// Debug disable or enable
#define TFTPS_API_PRINT_HEADER			"TFTPS_API: "	// Print header
#define TFTPS_API_FILE_NAME_VALIDATION	0				// File name validation mechanism
#define TFTPS_API_APPROVED_FILE_NAME	"ota_r2w.bin"	// Approved file name

#if (TFTPS_API_DEBUG_ENABLE)
	#define TFTPS_API_PRINT(f, ...)	{printf(TFTPS_API_PRINT_HEADER); printf(f, ## __VA_ARGS__);}
#else
	#define TFTPS_API_PRINT(f, ...)	{}
#endif

/* Protocol setting */

/* OTA setting */

/* Protocol definition*/

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */
typedef enum TFTP_UPDFW_STATUS_CODE_ {
	TFTP_UPDFW_FINE_OR_IGNORE = 0,
	TFTP_UPDFW_INVALID_SIGNATURE = 1,
	TFTP_UPDFW_IMG2_ADDR_ERROR = 2,
	TFTP_UPDFW_FLASH_ERROR = 3,
	TFTP_UPDFW_CHECKSUM_ERROR = 4,
	TFTP_UPDFW_OVERSIZE = 5,
	TFTP_UPDFW_UNREASONABLE_LENGTH = 6,
	TFTP_UPDFW_BUFFER_ERROR = 7
} TFTPS_UPDFW_STATUS_CODE;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */

#endif /* __TFTPS_API_H__ */