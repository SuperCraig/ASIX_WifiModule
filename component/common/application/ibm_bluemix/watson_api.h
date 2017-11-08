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
 * Module Name: watson_api.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __WATSON_API_H__
#define __WATSON_API_H__

/* INCLUDE FILE DECLARATIONS */
#include "ssl.h"
#include "cJSON.h"
#include "gconfig.h"

/* NAMING CONSTANT DECLARATIONS */
#define WATSON_API_DEBUG				1			// 0 = normal, 1 = debug
#define WATSON_HOST_NAME_LEN			64
#define WATSON_API_VER_LEN				GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN
#define WATSON_API_KEY_LEN				GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN
#define WATSON_API_TOKEN_LEN			GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN   
#define WATSON_AUTH_LEN					32
#define WATSON_URL_LEN					128
#define WATSON_TYPE_ID_LEN				GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN
#define WATSON_DEVICE_ID_LEN			GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN
#define WATSON_MANUFACTURER_LEN			64
#define WATSON_DESCRIPTION_LEN			64
#define WATSON_VERSION_LEN				12
#define WATSON_USER_AGENT				"AXM23001"

/* TYPE DECLARATIONS */
typedef struct _WATSON_OBJECT
{
	unsigned char Hostname[WATSON_HOST_NAME_LEN];  
	unsigned char ApiVer[WATSON_API_VER_LEN];		
	unsigned char ApiKey[WATSON_API_KEY_LEN];
	unsigned char ApiToken[WATSON_API_TOKEN_LEN];
	unsigned char URL[WATSON_URL_LEN];
	
	unsigned char TypeId[WATSON_TYPE_ID_LEN];
	unsigned char AuthToken[WATSON_AUTH_LEN];	
	unsigned char DeviceId[WATSON_DEVICE_ID_LEN];
	unsigned char SerialNumber[WATSON_DEVICE_ID_LEN];
	unsigned char Manufacturer[WATSON_MANUFACTURER_LEN];
	unsigned char Model[WATSON_TYPE_ID_LEN];
	unsigned char DeviceClass[WATSON_DEVICE_ID_LEN];
	unsigned char Description[WATSON_DESCRIPTION_LEN];
	unsigned char FwVersion[WATSON_VERSION_LEN];
	unsigned char HwVersion[WATSON_VERSION_LEN];
	unsigned char DescriptiveLocation[WATSON_DESCRIPTION_LEN];	
} WATSON_OBJECT;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int WATSON_InitObj(WATSON_OBJECT *pWsObj, unsigned char *pHostname, unsigned char *pApiVer, unsigned char *pApiKey, unsigned char *pApiToken);
int WATSON_SetDeviceBasic(WATSON_OBJECT *pWsObj, unsigned char *pTypeId, unsigned char *pAuthToken, unsigned char *pDeviceId, unsigned char *pSerialNumber);
int WATSON_SetDeviceDescription(WATSON_OBJECT *pWsObj, unsigned char *pManufacturer, unsigned char *pModel, unsigned char *pDeviceClass, unsigned char *pDescription);
int WATSON_SetDeviceVersion(WATSON_OBJECT *pWsObj, unsigned char *pFwVersion, unsigned char *pHwVersion);
int WATSON_GetDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, int bufSize, int *pRcvLen);
int WATSON_AddDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, int bufSize, int *pRcvLen);

#endif /* End of __WATSON_API_H__ */


/* End of watson_api.h */
