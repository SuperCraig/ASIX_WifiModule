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
 * Module Name: watson_api.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "watson_api.h"
#include "gconfig.h"
#include "axhttpc.h"


/* NAMING CONSTANT DECLARATIONS */
#define WATSON_XFER_BUF_SIZE		1024
#define WATSON_PORT					443

#define WATSON_TypeId				"\"typeId\""
#define WATSON_DeviceId				"\"deviceId\""
#define WATSON_AuthToken			"\"authToken\""
#define WATSON_DeviceInfo			"\"deviceInfo\""
#define WATSON_SerialNumber			"\"serialNumber\""
#define WATSON_Manufacturer			"\"manufacturer\""
#define WATSON_Model				"\"model\""
#define WATSON_DeviceClass			"\"deviceClass\""
#define WATSON_Description			"\"description\""
#define WATSON_FwVersion			"\"fwVersion\""
#define WATSON_HwVersion			"\"hwVersion\""
#define WATSON_DescriptiveLocation	"\"descriptiveLocation\""

/* GLOBAL VARIABLES DECLARATIONS */


/* LOCAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */
static int watson_BuildJsonAddDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, unsigned short bufSize);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: int watson_BuildJsonAddDevice()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int watson_BuildJsonAddDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, unsigned short bufSize)
{
	int len = 0;

	if (pWsObj == 0 || pBuf == 0)
		return -1;
	
	len += snprintf(&pBuf[len], bufSize-len, "{");
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_DeviceId, pWsObj->DeviceId);
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_AuthToken, pWsObj->AuthToken);
	len += snprintf(&pBuf[len], bufSize-len, "%s: {", WATSON_DeviceInfo);
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_SerialNumber, pWsObj->SerialNumber);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_Manufacturer, pWsObj->Manufacturer);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_Model, pWsObj->Model);		
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_DeviceClass, pWsObj->DeviceClass);
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_Description, pWsObj->Description);
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"v%s\",", WATSON_FwVersion, pWsObj->FwVersion);
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\",", WATSON_HwVersion, pWsObj->HwVersion);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\"", WATSON_DescriptiveLocation, pWsObj->DescriptiveLocation);		
	len += snprintf(&pBuf[len], bufSize-len, "},");
	len += snprintf(&pBuf[len], bufSize-len, "%s: {", "\"location\"");
	len += snprintf(&pBuf[len], bufSize-len, "%s: %d,", "\"longitude\"", 0);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: %d,", "\"latitude\"", 0);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: %d,", "\"elevation\"", 0);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: %d,", "\"accuracy\"", 0);	
	len += snprintf(&pBuf[len], bufSize-len, "%s: \"%s\"", "\"measuredDateTime\"", "");	
	len += snprintf(&pBuf[len], bufSize-len, "},");
	len += snprintf(&pBuf[len], bufSize-len, "%s: {}", "\"metadata\"");	
	len += snprintf(&pBuf[len], bufSize-len, "}");

	return len;
} /* End of watson_BuildJsonAddDevice() */

/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_InitObj()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_InitObj(WATSON_OBJECT *pWsObj, unsigned char *pHostname, unsigned char *pApiVer, unsigned char *pApiKey, unsigned char *pApiToken)
{
	if (pWsObj == 0 || pHostname == 0 || pApiVer == 0 || pApiKey == 0 || pApiToken == 0)
		return -1;

	memset(pWsObj, 0, sizeof(WATSON_OBJECT));
	
	strncpy(pWsObj->Hostname, pHostname, WATSON_HOST_NAME_LEN-1);
	strncpy(pWsObj->ApiVer, pApiVer, WATSON_API_VER_LEN-1);
	strncpy(pWsObj->ApiKey, pApiKey, WATSON_API_KEY_LEN-1);
	strncpy(pWsObj->ApiToken, pApiToken, WATSON_API_TOKEN_LEN-1);
	return 0;
} /* End of WATSON_InitObj() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_SetDeviceBasic()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_SetDeviceBasic(WATSON_OBJECT *pWsObj, unsigned char *pTypeId, unsigned char *pAuthToken, unsigned char *pDeviceId, unsigned char *pSerialNumber)
{
  	if (pWsObj == 0)
		return -1;

	if (pTypeId)
	{
		strncpy(pWsObj->TypeId, pTypeId, WATSON_TYPE_ID_LEN-1);
	}

	if (pAuthToken)
	{
		strncpy(pWsObj->AuthToken, pAuthToken, WATSON_AUTH_LEN-1);
	}
	
	if (pDeviceId)
	{
		strncpy(pWsObj->DeviceId, pDeviceId, WATSON_DEVICE_ID_LEN-1);
	}
	
	if (pSerialNumber)
	{
		strncpy(pWsObj->SerialNumber, pSerialNumber, WATSON_DEVICE_ID_LEN-1);
	}
	
	return 0;	
} /* End of WATSON_SetDeviceBasic() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_SetDeviceDescription()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_SetDeviceDescription(WATSON_OBJECT *pWsObj, unsigned char *pManufacturer, unsigned char *pModel, unsigned char *pDeviceClass, unsigned char *pDescription)
{
    if (pWsObj == 0)
		return -1;
		
 	if (pManufacturer)
	{
		strncpy(pWsObj->Manufacturer, pManufacturer, WATSON_MANUFACTURER_LEN-1);
	}
	
	if (pModel)
	{
		strncpy(pWsObj->Model, pModel, WATSON_TYPE_ID_LEN-1);
	}	
	
	if (pDeviceClass)
	{
		strncpy(pWsObj->DeviceClass, pDeviceClass, WATSON_DEVICE_ID_LEN-1);
	}	
	
	if (pDescription)
	{
		strncpy(pWsObj->Description, pDescription, WATSON_DESCRIPTION_LEN-1);
	}		
	
	return 0;	
} /* End of WATSON_SetDeviceDescription() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_SetDeviceVersion()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_SetDeviceVersion(WATSON_OBJECT *pWsObj, unsigned char *pFwVersion, unsigned char *pHwVersion)
{
    if (pWsObj == 0)
		return -1;
	  
  	if (pFwVersion)
	{
		strncpy(pWsObj->FwVersion, pFwVersion, WATSON_VERSION_LEN-1);
	}
	
	if (pHwVersion)
	{
		strncpy(pWsObj->HwVersion, pHwVersion, WATSON_VERSION_LEN-1);
	}
	
	return 0;
} /* End of WATSON_SetDeviceVersion() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_GetDevice()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_GetDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, int bufSize, int *pRcvLen)
{
	AXHTTPC_SESSION session;
	unsigned char *pJsonStr = 0;	
	int retval = -1;
	
	if (pWsObj == 0)
		return -1;
	
	/* Init HTTP session */
	if (AXHTTPC_Init(&session, 1, WATSON_XFER_BUF_SIZE) < 0)
		return -1;

	/* Set method, URL */
	snprintf(pWsObj->URL, WATSON_URL_LEN, "/api/%s/device/types/%s/devices/%s", pWsObj->ApiVer, pWsObj->TypeId, pWsObj->DeviceId);
	AXHTTPC_SetRequest(&session, AXHTTPC_GET_METHOD, pWsObj->URL, 0);

	/* Set authentication */
	AXHTTPC_SetAuthentication(&session, pWsObj->ApiKey, pWsObj->ApiToken, BASIC_AUTH);
	
	/* Add mandatory headers */
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_HOST, pWsObj->Hostname);	
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_USER_AGENT, WATSON_USER_AGENT);	
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_CONTENT_TYPE, AXHTTPC_CTYPE_APP_JSON);
	
	/* Connect to target host and open HTTP section */
	if (AXHTTPC_Connect(&session, pWsObj->Hostname, WATSON_PORT) < 0)
		goto WATSON_GetDevice_Exit;
	
	/* Send request */
	if (AXHTTPC_Send(&session, 0, 0) < 0)
		goto WATSON_GetDevice_Exit;
	
	/* Receive response */
	pJsonStr = pvPortMalloc(WATSON_XFER_BUF_SIZE);
	if (pJsonStr == 0)
		goto WATSON_GetDevice_Exit;
	
	if (pBuf != 0 && pRcvLen != 0)
	{
		retval = AXHTTPC_Recv(&session, pBuf, bufSize);
		if (retval < 0)
			goto WATSON_GetDevice_Exit;	  
		
		(*pRcvLen) = retval;
	}
	else
	{
		retval = AXHTTPC_Recv(&session, pJsonStr, WATSON_XFER_BUF_SIZE);
		if (retval < 0)
			goto WATSON_GetDevice_Exit;	  
	}
	
	retval = session.statusCode;
	
WATSON_GetDevice_Exit:
  	/* Close HTTP session */
	AXHTTPC_Close(&session);

	/* Free JSON buffer */
	if (pJsonStr)
		vPortFree(pJsonStr);
	
	return retval;
} /* End of WATSON_GetDevice() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: int WATSON_AddDevice()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int WATSON_AddDevice(WATSON_OBJECT *pWsObj, unsigned char *pBuf, int bufSize, int *pRcvLen)
{
	int len = -1;
	AXHTTPC_SESSION session;
	unsigned char *pJsonStr = 0;
	int retval = -1;
	
	if (pWsObj == 0)
		return -1;
	
	/* Init HTTP session */
	if (AXHTTPC_Init(&session, 1, WATSON_XFER_BUF_SIZE) < 0)
		return -1;
	
	/* Set method, URL */
	snprintf(pWsObj->URL, WATSON_URL_LEN, "/api/%s/device/types/%s/devices", pWsObj->ApiVer, pWsObj->TypeId);
	AXHTTPC_SetRequest(&session, AXHTTPC_POST_METHOD, pWsObj->URL, 0);

	/* Set authentication */
	AXHTTPC_SetAuthentication(&session, pWsObj->ApiKey, pWsObj->ApiToken, BASIC_AUTH);
	
	/* Add mandatory headers */
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_HOST, pWsObj->Hostname);	
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_USER_AGENT, WATSON_USER_AGENT);	
	AXHTTPC_AddHeader(&session, AXHTTPC_HDR_CONTENT_TYPE, AXHTTPC_CTYPE_APP_JSON);
	
	/* Connect to target host and open HTTP section */
	if (AXHTTPC_Connect(&session, pWsObj->Hostname, WATSON_PORT) < 0)
		goto WATSON_AddDevice_Exit; 

	/* Build payload (JSON data) */
	pJsonStr = pvPortMalloc(WATSON_XFER_BUF_SIZE);	
	if (pJsonStr == 0)
		goto WATSON_AddDevice_Exit; 
	
	len = watson_BuildJsonAddDevice(pWsObj, pJsonStr, WATSON_XFER_BUF_SIZE);
	if (len < 0)
		goto WATSON_AddDevice_Exit; 
	
	/* Send request */	
	if (AXHTTPC_Send(&session, pJsonStr, len) < 0)
		goto WATSON_AddDevice_Exit; 
	
	/* Receive response */
	if (pBuf != 0 && pRcvLen != 0)
	{
		len = AXHTTPC_Recv(&session, pBuf, bufSize);
		if (len < 0)
			goto WATSON_AddDevice_Exit;
		
		(*pRcvLen) = len;
	}
	else
	{
		len = AXHTTPC_Recv(&session, pJsonStr, WATSON_XFER_BUF_SIZE);
		if (len < 0)
			goto WATSON_AddDevice_Exit;
	}
	
	retval = session.statusCode;
	
WATSON_AddDevice_Exit:	
	/* Close HTTP session */  
	AXHTTPC_Close(&session);
	
	/* Free JSON buffer */
	if (pJsonStr)
		vPortFree(pJsonStr);
	
	return retval;
} /* End of WATSON_AddDevice() */

/* End of watson_api.c */

