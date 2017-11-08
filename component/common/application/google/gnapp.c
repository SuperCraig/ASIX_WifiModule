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
 * Module Name: gnapp.c
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
#include "task.h"
#include "gnapp.h"
#include "gconf_ap.h"
#include "misc.h"
#include <lwip/sockets.h>
#include "wifi_constants.h"
#include "wifi_conf.h"
#include "google_nest.h"
#include "timers.h"
#include "cJSON.h"
#include "gpios.h"
#include "rtc.h"
#include "ths.h"

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)

/* NAMING CONSTANT DECLARATIONS */
#define gnapp_NULL_STR			"null"

#define GNAPP_ITEM_NAME			"name"
#define GNAPP_ITEM_TEMPERATURE	"temperature"
#define GNAPP_ITEM_RH			"relative_humidity"
#define GNAPP_ITEM_TIME_STAMP	"time_stamp"
#define GNAPP_ITEM_BEEP			"beep"
								  
/* GLOBAL VARIABLES DECLARATIONS */
GNAPP_CTRL	GNAPP_Ctrl;

static TimerHandle_t gnappTimerHandle = NULL;
static xSemaphoreHandle gnappMutex;
static cJSON_Hooks memoryHook;
static xTaskHandle gnappTaskHandle = NULL;
/* LOCAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */
static void gnapp_timer(void *pParam);
static void gnapp_Task(void *pParam);

/* LOCAL SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void gnapp_timer(void *pParam)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void gnapp_timer(void *pParam)
{
	while(xSemaphoreTake(gnappMutex, portMAX_DELAY) != pdTRUE);	  
	if (GNAPP_Ctrl.PollingTime)
		GNAPP_Ctrl.PollingTime--;
	if (GNAPP_Ctrl.AliveTime)
		GNAPP_Ctrl.AliveTime--;
	xSemaphoreGive(gnappMutex);		
} /* End of gnapp_timer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: cJSON* gnapp_ConvertJsonStrToObj()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* gnapp_ConvertJsonStrToObj(u8 *pJsonStr)
{
	cJSON *pJsonObj=0;
	
	if (pJsonStr==0)
		return pJsonObj;
	
	pJsonObj = cJSON_Parse(pJsonStr);
	if (pJsonObj==0)
		printf("GNAPP: Convert JSON string to object fail!\r\n");
	
	return pJsonObj;
} /* End of gnapp_ConvertJsonStrToObj() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: cJSON* gnapp_AllocNode()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* gnapp_AllocNode(void)
{
	cJSON *pJsonObj=0;
	
	sprintf(GNAPP_Ctrl.TxBuf,	"{"
								"\"%s\":\"%s\","
								"\"%s\":\"%s\","								  
								"\"%s\":%d,"
								"\"%s\":%d,"
								"\"%s\":%d"
								"}"
								, GNAPP_ITEM_NAME, ""
								, GNAPP_ITEM_TIME_STAMP, ""								  
								, GNAPP_ITEM_TEMPERATURE, 0
								, GNAPP_ITEM_RH, 0
								, GNAPP_ITEM_BEEP, 0
								);
	
	pJsonObj = gnapp_ConvertJsonStrToObj(GNAPP_Ctrl.TxBuf);
	return pJsonObj;
} /* End of gnapp_AllocNode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GNAPP_STATUS gnapp_GetNode()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static GNAPP_STATUS gnapp_GetNode(u8 *pHostName, u8 *pNodePath, cJSON **pNode)
{
	googlenest_context googlenest;
	GNAPP_STATUS err=GNAPP_ERR;
	int i;
	u8 *pbuf;
	
	if (pHostName==0 || pNodePath==0 || pNode==0)
		return err;
	
	(*pNode) = 0;
	memset(&googlenest, 0, sizeof(googlenest_context));
	if (gn_connect(&googlenest, pHostName, GNAPP_SERVER_PORT))
		return err;
	
	while(xSemaphoreTake(gnappMutex, portMAX_DELAY) != pdTRUE);	  	
	GNAPP_Ctrl.AliveTime = GNAPP_ALIVE_TIMEOUT;	
	xSemaphoreGive(gnappMutex);	
	
	if (gn_get(&googlenest, pNodePath, GNAPP_Ctrl.RxBuf, GNAPP_RX_BUF_SIZE)==0)
	{
		/* Find out jaon string */
		pbuf = GNAPP_Ctrl.RxBuf;
		for (i=0; i<GNAPP_RX_BUF_SIZE; i++)
		{
			if (pbuf[i] == '\0')
			{
				break;
			}
			if (memcmp(&pbuf[i], "\r\n\r\n", 4)==0 && (i+4)<GNAPP_RX_BUF_SIZE)
			{
				pbuf += (i+4);
				break;
			}
		}
		
		(*pNode) = gnapp_ConvertJsonStrToObj(pbuf);
	  	if (*pNode)
 			err = GNAPP_OK;
	}
	gn_close(&googlenest);
	vTaskDelay(10);	
	return err;
} /* End of gnapp_GetNode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GNAPP_STATUS gnapp_PutNode()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static GNAPP_STATUS gnapp_PutNode(u8 *pHostName, u8 *pNodePath, cJSON *pNode)
{
	googlenest_context googlenest;
	GNAPP_STATUS err=GNAPP_ERR;
	char *pJsonStr=0;
	
	if (pHostName==0 || pNodePath==0 || pNode==0)
		return err;
	
	memset(&googlenest, 0, sizeof(googlenest_context));
	if (gn_connect(&googlenest, pHostName, GNAPP_SERVER_PORT))
		return err;
	
	while(xSemaphoreTake(gnappMutex, portMAX_DELAY) != pdTRUE);	  	
	GNAPP_Ctrl.AliveTime = GNAPP_ALIVE_TIMEOUT;	
	xSemaphoreGive(gnappMutex);	
	
	pJsonStr = cJSON_PrintUnformatted(pNode);
	if (pJsonStr==0)
		goto gnapp_PutNode_Exit;

	if (gn_put(&googlenest, pNodePath, pJsonStr)==0)
	  	err = GNAPP_OK;
	else
		printf("GNAPP: Fail to put device node\r\n");
	
gnapp_PutNode_Exit:	
	if (pJsonStr)
		vPortFree(pJsonStr);
	gn_close(&googlenest);
	vTaskDelay(10);	
	return err;
} /* End of gnapp_PutNode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GNAPP_STATUS gnapp_PatchNode()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static GNAPP_STATUS gnapp_PatchNode(u8 *pHostName, u8 *pNodePath, cJSON *pNode)
{
	googlenest_context googlenest;
	GNAPP_STATUS err=GNAPP_ERR;
	cJSON *pPatchObj=0, *pItem;
	char *pJsonStr=0;
	
	if (pHostName==0 || pNodePath==0 || pNode==0)
		return err;
	
	memset(&googlenest, 0, sizeof(googlenest_context));
	if (gn_connect(&googlenest, pHostName, GNAPP_SERVER_PORT))
		return err;
	
	while(xSemaphoreTake(gnappMutex, portMAX_DELAY) != pdTRUE);	  	
	GNAPP_Ctrl.AliveTime = GNAPP_ALIVE_TIMEOUT;	
	xSemaphoreGive(gnappMutex);	
	
	pPatchObj = cJSON_CreateObject();
	if (pPatchObj==0)
	{
		goto gnapp_PatchNode_Exit;
	}

	/* Patch Timestamp */
	pItem = cJSON_GetObjectItem(pNode, GNAPP_ITEM_TIME_STAMP);
	if (pItem)
		cJSON_AddStringToObject(pPatchObj, GNAPP_ITEM_TIME_STAMP, pItem->valuestring);
	  
	/* Patch Temperature */
	pItem = cJSON_GetObjectItem(pNode, GNAPP_ITEM_TEMPERATURE);
	if (pItem)	
		cJSON_AddNumberToObject(pPatchObj, GNAPP_ITEM_TEMPERATURE, pItem->valuedouble);

	/* Patch Relative Humidity */
	pItem = cJSON_GetObjectItem(pNode, GNAPP_ITEM_RH);
	if (pItem)	
		cJSON_AddNumberToObject(pPatchObj, GNAPP_ITEM_RH, pItem->valuedouble);

	/* Update Name only in first "patch" request */
	if (GNAPP_Ctrl.CloudDataPatched == 0)
	{
		pItem = cJSON_GetObjectItem(pNode, GNAPP_ITEM_NAME);
		if (pItem)	
			cJSON_AddStringToObject(pPatchObj, GNAPP_ITEM_NAME, pItem->valuestring);		
	}
	  
	pJsonStr = cJSON_PrintUnformatted(pPatchObj);
	if (pJsonStr==0)
	{
		goto gnapp_PatchNode_Exit;
	}		
	if (gn_patch(&googlenest, pNodePath, pJsonStr)==0)	  
	{
	  	err = GNAPP_OK;	  
		GNAPP_Ctrl.CloudDataPatched	= 1;			
	}
	else
		printf("GNAPP: Fail to patch device node\r\n");
	
gnapp_PatchNode_Exit:
	if (pPatchObj)
		cJSON_Delete(pPatchObj);
	if (pJsonStr)
		vPortFree(pJsonStr);
	gn_close(&googlenest);
	vTaskDelay(10);		
	return err;
} /* End of gnapp_PatchNode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GNAPP_STATUS gnapp_UpdateNode()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static GNAPP_STATUS gnapp_UpdateNode(cJSON *pNode)
{
	RTC_TIME rtc_time;  
	float ftmp;
	u8 tmp[30];
	
	if (pNode==0)
		return GNAPP_ERR;
	
	/* Update temperature */
	if (THS_GetTemperature(&ftmp)==TRUE)
	{
		cJSON_SetIntValue(cJSON_GetObjectItem(pNode, GNAPP_ITEM_TEMPERATURE), ftmp);
	}

	/* Update relative humidity */
	if (THS_GetRelativeHumidity(&ftmp)==TRUE)
	{
		cJSON_SetIntValue(cJSON_GetObjectItem(pNode, GNAPP_ITEM_RH), ftmp+0.001);//+0.001 be able to print out decimal point('.')
	}
	
	/* Update time stamp */
	if (RTC_GetCurrTime(&rtc_time)==TRUE)
	{
		sprintf(tmp, "%d/%d/%d %d:%d:%d"
							, rtc_time.Year
							, rtc_time.Month
							, rtc_time.Date							  
							, rtc_time.Hour
							, rtc_time.Minute
							, rtc_time.Second);	
		cJSON_DeleteItemFromObject(pNode, GNAPP_ITEM_TIME_STAMP);
		cJSON_AddStringToObject(pNode, GNAPP_ITEM_TIME_STAMP, tmp);
	}

	/* Update device name */
	cJSON_DeleteItemFromObject(pNode, GNAPP_ITEM_NAME);
	cJSON_AddStringToObject(pNode, GNAPP_ITEM_NAME, (char*)GCONFIG_GetDeviceName());

	return GNAPP_OK;
} /* End of gnapp_UpdateNode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void gnapp_Task(void *pParam)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void gnapp_Task(void *pParam)
{
	u8 write_cfg_data=0;
	cJSON *pDevNodeLoc=NULL, *pItemLoc, *pDevNodeRmt=NULL, *pItemRmt;

	/* Load serial number as a device node name */
	MISC_GetSerialNum(GNAPP_Ctrl.TxBuf, 255);
	sprintf(GNAPP_Ctrl.DevNodeName, "%s.json", GNAPP_Ctrl.TxBuf);
	
	while (1)
	{
		vTaskDelay(1);

		/* Wait for network is available */
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
		if (MISC_NetInterfaceReadyCheck() != IF_STATE_IP_READY)
#else
 		if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
#endif
		{
			vTaskDelay(5000);
			continue;
		}
		
		switch (GNAPP_Ctrl.TaskState)
		{
		case GNAPP_INIT:
			GNAPP_Ctrl.TaskState = GNAPP_CHECK_DEVICE_NODE;
			break;
			
		case GNAPP_CHECK_DEVICE_NODE:
			printf("GNAPP: Check device node...");
			if (gnapp_GetNode(GNAPP_Ctrl.HostName, GNAPP_Ctrl.DevNodeName, &pDevNodeRmt)!=GNAPP_OK)
			{
				printf("Connection fail\r\n");
				vTaskDelay(5000);//Try again after 5 sec
				break;
			}
			
			/* Check device node is valid */
			if (pDevNodeRmt==NULL)
			{
				GNAPP_Ctrl.TaskState = GNAPP_CREATE_NEW_DEVICE_NODE;						  
				printf("fail to get node\r\n");				
				break;
			}
			
		  	pItemRmt = cJSON_GetObjectItem(pDevNodeRmt, GNAPP_ITEM_NAME);			  
 			if (pDevNodeRmt->type==cJSON_NULL || pItemRmt==NULL)
			{
				cJSON_Delete(pDevNodeRmt);	
				pDevNodeRmt = NULL;
				GNAPP_Ctrl.TaskState = GNAPP_CREATE_NEW_DEVICE_NODE;						  
				printf("fail to node validation\r\n");			  				
				break;
			}
			
			pDevNodeLoc = pDevNodeRmt;
			GNAPP_Ctrl.TaskState = GNAPP_PREODIC_UPDATE;			
			printf("Ok\r\n");								
			break;
			
		case GNAPP_CREATE_NEW_DEVICE_NODE:
			printf("GNAPP: Create new node...");		  
			pDevNodeLoc = gnapp_AllocNode();
			if (pDevNodeLoc==0)
			{
				printf("allocation fail\r\n");			  
				vTaskDelay(1000);//Try again after 1 sec
				break;			  
			}
			gnapp_UpdateNode(pDevNodeLoc);

			gnapp_PutNode(GNAPP_Ctrl.HostName, GNAPP_Ctrl.DevNodeName, pDevNodeLoc);
			cJSON_Delete(pDevNodeLoc);		
			pDevNodeLoc = NULL;
			GNAPP_Ctrl.TaskState = GNAPP_CHECK_DEVICE_NODE;				
			printf("Ok\r\n");			
			vTaskDelay(1000);//Try again after 1 sec			
			break;
			
		case GNAPP_PREODIC_UPDATE:
			if (GNAPP_Ctrl.PollingTime==0)
			{
				/* Update device node to cloud */
				gnapp_UpdateNode(pDevNodeLoc);
				gnapp_PatchNode(GNAPP_Ctrl.HostName, GNAPP_Ctrl.DevNodeName, pDevNodeLoc);
				
				if (gnapp_GetNode(GNAPP_Ctrl.HostName, GNAPP_Ctrl.DevNodeName, &pDevNodeRmt)==GNAPP_OK)
				{
					if (pDevNodeRmt==NULL)
					{
						GNAPP_Ctrl.TaskState = GNAPP_CHECK_DEVICE_NODE;						  
						break;
					}
					
			  		pItemRmt = cJSON_GetObjectItem(pDevNodeRmt, GNAPP_ITEM_NAME);
					if (pItemRmt==NULL)
					{
						cJSON_Delete(pDevNodeRmt);
						pDevNodeRmt = NULL;
						cJSON_Delete(pDevNodeLoc);		
						pDevNodeLoc = NULL;						
						GNAPP_Ctrl.TaskState = GNAPP_CHECK_DEVICE_NODE;						  
						break;
					}
					
					/* Update beep status */
				  	pItemRmt = cJSON_GetObjectItem(pDevNodeRmt, GNAPP_ITEM_BEEP);
				  	pItemLoc = cJSON_GetObjectItem(pDevNodeLoc, GNAPP_ITEM_BEEP);
					if (pItemRmt!=0)
					{
						if (pItemRmt->valueint!=pItemLoc->valueint)
							cJSON_SetIntValue(pItemLoc, pItemRmt->valueint);
						GPIOS_BeepCtrl(pItemLoc->valueint==0 ? 0:1);
					}
					  
					/* Update device name */
				  	pItemRmt = cJSON_GetObjectItem(pDevNodeRmt, GNAPP_ITEM_NAME);
				  	pItemLoc = cJSON_GetObjectItem(pDevNodeLoc, GNAPP_ITEM_NAME);
					if (pItemRmt!=0 && strcmp(pItemRmt->valuestring, pItemLoc->valuestring)!=0)
					{
						cJSON_DeleteItemFromObject(pDevNodeLoc, GNAPP_ITEM_NAME);					
						cJSON_AddStringToObject(pDevNodeLoc, GNAPP_ITEM_NAME, pItemRmt->valuestring);
								
						GCONFIG_SetDeviceName(pItemRmt->valuestring);
						write_cfg_data = 1;
					}

					cJSON_Delete(pDevNodeRmt);	
					pDevNodeRmt = NULL;
				}

				while(xSemaphoreTake(gnappMutex, portMAX_DELAY) != pdTRUE);	  			  
				GNAPP_Ctrl.PollingTime = GNAPP_POLLING_INTERVAL;
				xSemaphoreGive(gnappMutex);	
			}
			
			if (write_cfg_data)
			{
				write_cfg_data = 0;
			  	GCONFIG_WriteConfigData();	
			}
			break;
		}
	}
gnapp_Task_Exit:
	if (gnappTimerHandle)
	{
		xTimerStop(gnappTimerHandle, 0);  
		xTimerDelete(gnappTimerHandle, 0);  		
	}	
	vSemaphoreDelete(gnappMutex);	  
	vTaskDelete(NULL);		
} /* End of gnapp_Task() */

/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: void GNAPP_Init(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GNAPP_Init(void)
{
	if (GCONFIG_GetWifiNetworkMode() != RTW_MODE_STA)
		return;

	memset(&GNAPP_Ctrl, 0, sizeof(GNAPP_Ctrl));
		
	/* Init local device node */
	GCONFIG_GetGoogleNestHostName(GNAPP_Ctrl.HostName, sizeof(GNAPP_Ctrl.HostName));
	if (strlen(GNAPP_Ctrl.HostName)==0)
		return;
	
	if (!(gnappTimerHandle = xTimerCreate("", 1000, pdTRUE, NULL, gnapp_timer)))
		return;
	xTimerStart(gnappTimerHandle, 0);
	
 	if(xTaskCreate(gnapp_Task, (u8*)"gnapp_task", 1280, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)                  
	{
		printf("GNAPP: Create task fail!\r\n");
	}
	else
	{
		/* Hooks cJSON module */
		memoryHook.malloc_fn = pvPortMalloc;
		memoryHook.free_fn = vPortFree;
		cJSON_InitHooks(&memoryHook);

		gnappMutex = xSemaphoreCreateMutex();
	}
} /* End of GNAPP_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GNAPP_STATUS GNAPP_CheckConnectionStatus()
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
GNAPP_STATUS GNAPP_CheckConnectionStatus(void)
{
	if (GNAPP_Ctrl.AliveTime)	
		return GNAPP_CONNECTED;
	return GNAPP_DISCONNECTED;
} /* End of GNAPP_Init() */
#endif

/* End of gnapp.c */

