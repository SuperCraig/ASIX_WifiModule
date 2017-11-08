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
 * Module Name: bluemix.c
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
#include "bluemix.h"
#include "MQTTClient.h"
#include "wifi_constants.h"
#include "wifi_conf.h"
#include "gconfig.h"
#include "cJSON.h"
#include "rtc.h"
#include "ths.h"
#include "misc.h"
#include "watson_api.h"

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)

/* NAMING CONSTANT DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */
unsigned char		BM_XmitBuf[BM_TX_BUFFER_SIZE], BM_RecvBuf[BM_RX_BUFFER_SIZE];
char				BM_HostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];		// IBM bluemix host name
char				BM_ClientID[GCONFIG_MAX_BLUEMIX_CLIENT_ID_LEN];		// Client ID
char				BM_OrganizationID[GCONFIG_MAX_BLUEMIX_ORG_ID_LEN];	// Organization ID
char				BM_TypeID[GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN];			// Type ID
char				BM_DeviceID[GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN];		// Device ID
char				BM_UserName[GCONFIG_MAX_BLUEMIX_USERNAME_LEN];		// Username
char				BM_PassWord[GCONFIG_MAX_BLUEMIX_PASSWORD_LEN];		// Password
char				BM_SubscribeTopic[] = BM_MQTT_SUBSCRIBE_TOPIC;		// Subscribe topic
char				BM_PublishTopic[] = BM_MQTT_PUBLISH_TOPIC;			// Publish topic
static cJSON_Hooks	memoryHook;
static unsigned char	jsonBuffer[1024];
cJSON				*pDevSubNodeRoot = NULL;
cJSON				*pDevSubNodeD = NULL;
cJSON				*pDevSubItemBeep = NULL;
unsigned char		BM_BeepValue = 0;
unsigned char		BM_State = BM_STATE_IDLE;
char				BMAR_ApiHostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];		// API hostname
char				BMAR_ApiVersion[GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN];	// API version
char				BMAR_ApiKey[GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN];		// API key
char				BMAR_ApiToken[GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN];	// API token

/* LOCAL VARIABLES DECLARATIONS */
static WATSON_OBJECT WsObj;

/* LOCAL SUBPROGRAM DECLARATIONS */
void BLUEMIX_ConnectDataInit(MQTTPacket_connectData *pConnectData);
void BLUEMIX_Task(void *pvParameters);
void BLUEMIX_MessageArrived(MessageData* data);
static cJSON* BLUEMIX_ConvertJsonStrToObj(u8 *pJsonStr);
static cJSON* BLUEMIX_AllocNode(void);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: void BLUEMIX_Init(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void BLUEMIX_Init(void)
{
	if (GCONFIG_GetWifiNetworkMode() != RTW_MODE_STA)
		return;
	
	if (xTaskCreate(BLUEMIX_Task, "BluemixTask", BM_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) == pdPASS)
	{
		/* Bluemix state init */
		BM_State = BM_STATE_INIT;
	}
	else
	{
		/* Bluemix create task failed */
		printf("%sCreate task failed!\r\n", BM_PRINT_HEADER);
		BM_State = BM_STATE_IDLE;
	}
	
} /* End of BLUEMIX_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: char *BLUEMIX_CheckConnectionState(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
char *BLUEMIX_CheckConnectionState(void)
{
	switch (BM_State)
	{
		case BM_STATE_IDLE:
			return "Task idle";
		case BM_STATE_INIT:
			return "Variable init";
		case BM_STATE_NETWORK_CHECKING:
			return "Network connection checking...";
		case BM_STATE_TCP_CONNECTING:
			return "TCP connecting...";
		case BM_STATE_MQTT_CONNECTING:
			return "MQTT connecting...";
		case BM_STATE_MQTT_SUBSCRIBING:
			return "MQTT subscribing...";
		case BM_STATE_RUNNING:
			return "MQTT running";
		case BM_STATE_ERROR_HOSTNAME:
			return "Invalid MQTT host name, task abort";
		case BM_STATE_ERROR_ACCOUNT:
			return "Invalid MQTT account, task abort";
		case BM_STATE_ERROR_REGISTER_INFO:
			return "Invalid auto registered info, task abort";		  
	}
	
	return "Unknow";
	
} /* End of BLUEMIX_CheckConnectionState() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void BLUEMIX_ConnectDataInit(MQTTPacket_connectData *pConnectData)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void BLUEMIX_ConnectDataInit(MQTTPacket_connectData *pConnectData)
{
	pConnectData->struct_id[0] = 'M';
	pConnectData->struct_id[1] = 'Q';
	pConnectData->struct_id[2] = 'T';
	pConnectData->struct_id[3] = 'C';
	pConnectData->struct_version = 0;
	/* MQTT version: 3 = 3.1, 4 = 3.1.1 */
#if (BM_MQTT_PROTOCOL_VERSION == 4)
	pConnectData->MQTTVersion = 4;
#else
	pConnectData->MQTTVersion = 3;
#endif
	pConnectData->clientID.cstring = NULL;
	pConnectData->clientID.lenstring.len = 0;
	pConnectData->clientID.lenstring.data = NULL;
	pConnectData->keepAliveInterval = BM_MQTT_KEEPALIVE_TIMEOUT;
	pConnectData->cleansession = BM_MQTT_CLEAN_SESSION;
	pConnectData->willFlag = 0;
	pConnectData->will.struct_id[0] = 'M';
	pConnectData->will.struct_id[1] = 'Q';
	pConnectData->will.struct_id[2] = 'T';
	pConnectData->will.struct_id[3] = 'W';
	pConnectData->will.struct_version = 0;
	pConnectData->will.topicName.cstring = NULL;
	pConnectData->will.topicName.lenstring.len = 0;
	pConnectData->will.topicName.lenstring.data = NULL;
	pConnectData->will.message.cstring = NULL;
	pConnectData->will.message.lenstring.len = 0;
	pConnectData->will.message.lenstring.data = NULL;
	pConnectData->will.retained = 0;
	pConnectData->will.qos = 0;
	pConnectData->username.cstring = NULL;
	pConnectData->username.lenstring.len = 0;
	pConnectData->username.lenstring.data = NULL;
	pConnectData->password.cstring = NULL;
	pConnectData->password.lenstring.len = 0;
	pConnectData->password.lenstring.data = NULL;

} /* End of BLUEMIX_ConnectDataInit() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void BLUEMIX_Task(void *pvParameters)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void BLUEMIX_Task(void *pvParameters)
{
	MQTTClient				client;
	Network					network;
	int						rc;
	unsigned char			connectCheckCount;
	MQTTPacket_connectData	connectData;
	MQTTMessage				message;
	cJSON					*pDevNodeRoot;
	cJSON					*pDevNodeD;
	cJSON					*pDevItemTimestamp;
	RTC_TIME				rtc_time;
	unsigned char			tmpTime[30];
	float					ftmp;
	float					frh;
	unsigned short			server_port;
#if BM_ENABLE_SSL_TLS
	unsigned char			enable_ssl_tls = 1;	
#else
	unsigned char			enable_ssl_tls = 0;	
#endif
	int 					stuCode;
	
BLUEMIX_Task_Restart:

	while (1)
	{
		switch (BM_State)
		{
			case BM_STATE_INIT:
				/* Init local variable */
				rc = 0;
				connectCheckCount = 0;
				BLUEMIX_ConnectDataInit(&connectData);
				pDevNodeRoot = NULL;
				pDevNodeD = NULL;
				pDevItemTimestamp = NULL;
				
				/* Init network and client data structure */
				NetworkInit(&network, enable_ssl_tls);
				MQTTClientInit(&client, &network, BM_SOCKET_TIMEOUT, BM_XmitBuf, BM_TX_BUFFER_SIZE, BM_RecvBuf, BM_RX_BUFFER_SIZE);

				/* Do network checking */
				BM_State = BM_STATE_NETWORK_CHECKING;
				continue;
			case BM_STATE_NETWORK_CHECKING:
				/* Check WIFI connection */
				printf("\n\r%sWait for the device to connect Wi-Fi...\n\r", BM_PRINT_HEADER);
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
				while (MISC_NetInterfaceReadyCheck() != IF_STATE_IP_READY)
#else
				while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
#endif
				{
					vTaskDelay(1000 / portTICK_PERIOD_MS);
				}
				printf("%sWi-Fi connected\n\r", BM_PRINT_HEADER);
				
				BM_State = BM_STATE_LOAD_SETTINGS;
				continue;
			case BM_STATE_LOAD_SETTINGS:
				/* Get bluemix host name */
				GCONFIG_GetBluemixHostName(BM_HostName, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN);
				
				/* Check if host name is not null */
				if (BM_HostName[0] == 0)
				{
					printf("%sInvalid host name, exit the task!!\n\r", BM_PRINT_HEADER);
					
					/* Record error and abort the task */
					BM_State = BM_STATE_ERROR_HOSTNAME;
					goto BLUEMIX_Task_Exit;
				}
				
				/* Get bluemix client ID, username and password */
				GCONFIG_GetBluemixAccount(BM_OrganizationID, GCONFIG_MAX_BLUEMIX_ORG_ID_LEN,
										  BM_TypeID, GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN,
										  BM_DeviceID, GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN,
										  BM_UserName, GCONFIG_MAX_BLUEMIX_USERNAME_LEN,
										  BM_PassWord, GCONFIG_MAX_BLUEMIX_PASSWORD_LEN);
				
				/* Check if account information is null */
				if ((BM_OrganizationID[0] == 0) || 
					(BM_TypeID[0] == 0) ||
					(BM_DeviceID[0] == 0) ||
					(BM_UserName[0] == 0) ||
					(BM_PassWord[0] == 0))
				{
					printf("%sInvalid account information, exit the task!!\n\r", BM_PRINT_HEADER);
					
					/* Record error and abort the task */
					BM_State = BM_STATE_ERROR_ACCOUNT;
					goto BLUEMIX_Task_Exit;
				}

				/* Generate API hostname */
				snprintf(BMAR_ApiHostName, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN, "%s.internetofthings.ibmcloud.com", BM_OrganizationID);
				
				/* Get bluemix auto registration information */
				GCONFIG_GetBluemixAutoReg(	BMAR_ApiVersion, GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN,
							 				BMAR_ApiKey, GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN,
							 				BMAR_ApiToken, GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN);
				
#if BM_AUTO_REG_ENABLE				
				/* Check registered information */
				if (BMAR_ApiVersion[0] != 0 &&	BMAR_ApiKey[0] != 0 &&	BMAR_ApiToken[0] != 0)
				{
					/* Do device auto registration */
					BM_State = BM_STATE_AUTO_REG;
					continue;
				}
				else
				{
					/* Skip auto registration */
					printf("%sInvalid registered information, skip auto registration!!\n\r", BM_PRINT_HEADER);
				}
#endif
				
				BM_State = BM_STATE_TCP_CONNECTING;
				continue;
			case BM_STATE_AUTO_REG:
				WATSON_InitObj(&WsObj, BMAR_ApiHostName, BMAR_ApiVersion, BMAR_ApiKey, BMAR_ApiToken);
				WATSON_SetDeviceBasic(&WsObj, BM_TypeID, BM_PassWord, BM_DeviceID, BM_DeviceID);
				WATSON_SetDeviceDescription(&WsObj, BM_MANUFACTURER, BM_TypeID, 0, 0);
				WATSON_SetDeviceVersion(&WsObj, GCONFIG_VERSION_STRING, 0);				

				stuCode = WATSON_GetDevice(&WsObj, 0, 0, 0);
				if (stuCode <= 0)//HTTP fail
				{
					vTaskDelay(3000 / portTICK_PERIOD_MS);
		  			continue;					
				}
				else if (stuCode == 404)//Device doesn't exist
				{
					stuCode	= WATSON_AddDevice(&WsObj, 0, 0, 0);
					printf("%sCreate device %s\r\n", BM_PRINT_HEADER, (stuCode==201)?"success":"fail");

					/* Check it again */
					vTaskDelay(3000 / portTICK_PERIOD_MS);
		  			continue;					
				}

				/* Do TCP connection */
				BM_State = BM_STATE_TCP_CONNECTING;			  
		  		continue;
			case BM_STATE_TCP_CONNECTING:
				/* Init the socket and do TCP connection with MQTT broker */
				server_port = (enable_ssl_tls==0) ? BM_MQTT_BROKER_TCP_PORT:BM_MQTT_BROKER_SSL_PORT;
				printf("%sConnect to host \"%s:%d\" ...\n\r", BM_PRINT_HEADER, BM_HostName, server_port);
				while ((rc = NetworkConnect(&network, BM_HostName, server_port)) < 0)
				{
					printf("%sReturn code from NetworkConnect() is %d\n", BM_PRINT_HEADER, rc);
					/* Error handling for TCP connection failed */
					if (++connectCheckCount > BM_TCP_CONNECT_CHKCNT_MAX)
					{
#if BM_DEBUG
						printf("%sTCP connect failed, redo the MQTT client session\n\r", BM_PRINT_HEADER);
#endif
						/* NetworkConnect() will close socket while failed so that unnecessary redo here */
						/* Restart MQTT client session */
						BM_State = BM_STATE_INIT;
						goto BLUEMIX_Task_Restart;
					}
					vTaskDelay(BM_TCP_CONNECT_PERIOD / portTICK_PERIOD_MS);
				}
				printf("%s\"%s\" connected\n\r", BM_PRINT_HEADER, BM_HostName);
				BM_State = BM_STATE_MQTT_CONNECTING;
				continue;
			case BM_STATE_MQTT_CONNECTING:
				/* Reset check count */
				if (connectCheckCount)
				{
					connectCheckCount = 0;
				}
				
				/* Prepare client ID */
				sprintf(BM_ClientID, "d:%s:%s:%s", BM_OrganizationID, BM_TypeID, BM_DeviceID);
				
				/* Fill connection setting */
				connectData.clientID.cstring = BM_ClientID;
				connectData.username.cstring = BM_UserName;
				connectData.password.cstring = BM_PassWord;

				/* Do MQTT connection with MQTT broker */
				printf("%sDo MQTT connection ...\n\r", BM_PRINT_HEADER);
				while ((rc = MQTTConnect(&client, &connectData)) != 0)
				{
					printf("%sReturn code from MQTTConnect() is %d\n", BM_PRINT_HEADER, rc);
					/* Error handling for MQTT connection failed */
					if (++connectCheckCount > BM_MQTT_CONNECT_CHKCNT_MAX)
					{
#if BM_DEBUG
						printf("%sMQTT connect failed, redo the MQTT client session\n\r", BM_PRINT_HEADER);
#endif
						/* Close socket */
						network.disconnect(&network);
						
						/* Restart MQTT client session */
						BM_State = BM_STATE_INIT;
						goto BLUEMIX_Task_Restart;
					}
					vTaskDelay(BM_MQTT_CONNECT_PERIOD / portTICK_PERIOD_MS);
				}
				printf("%sMQTT connected\n\r", BM_PRINT_HEADER);
				BM_State = BM_STATE_MQTT_SUBSCRIBING;
				continue;
			case BM_STATE_MQTT_SUBSCRIBING:
				/* Subscribe the topic */
				printf("%sSubscribe command \"%s\"\n\r", BM_PRINT_HEADER, BM_SubscribeTopic);
				if ((rc = MQTTSubscribe(&client, BM_SubscribeTopic, QOS2, BLUEMIX_MessageArrived)) != 0)
				{
					printf("%sReturn code from MQTTSubscribe() is %d\n", BM_PRINT_HEADER, rc);
					/* Error handling for subscribe failed */
					if (rc == TX_FAILURE)
					{
#if BM_DEBUG
						printf("%sSubscribe failed, redo the MQTT client session\n\r", BM_PRINT_HEADER);
#endif
						/* Disconnect to MQTT broker */
						MQTTDisconnect(&client);
						
						/* Close socket */
						network.disconnect(&network);
						
						/* Restart MQTT client session */
						BM_State = BM_STATE_INIT;
						goto BLUEMIX_Task_Restart;
					}
				}
				else
				{
					/* Hook JSON */
					memoryHook.malloc_fn = pvPortMalloc;
					memoryHook.free_fn = vPortFree;
					cJSON_InitHooks(&memoryHook);

					/* Enter running task */
					BM_State = BM_STATE_RUNNING;
					continue;
				}
				break;
			case BM_STATE_RUNNING:
				/* Create new node */
				pDevNodeRoot = BLUEMIX_AllocNode();
				
				if (pDevNodeRoot == 0)
				{
					printf("%sAllocate device JSON node failed!\n\r", BM_PRINT_HEADER);
					break;
				}

				/* Assign QoS and retain field for publish message */
				message.qos = QOS1;
				message.retained = 1;
				
				/* Find out "d" object and update related members */
				pDevNodeD = cJSON_GetObjectItem(pDevNodeRoot, "d");
				
				if (pDevNodeD)
				{
					/* Update serial number */
					cJSON_DeleteItemFromObject(pDevNodeD, BM_ITEM_SERIAL_NUMBER);
					cJSON_AddStringToObject(pDevNodeD, BM_ITEM_SERIAL_NUMBER, BM_DeviceID);
					
					/* Update device name */
					cJSON_DeleteItemFromObject(pDevNodeD, BM_ITEM_NAME);
					cJSON_AddStringToObject(pDevNodeD, BM_ITEM_NAME, (char*)GCONFIG_GetDeviceName());
					
					/* Update timestamp */
					if (RTC_GetCurrTime(&rtc_time) == TRUE)
					{
						pDevItemTimestamp = cJSON_GetObjectItem(pDevNodeD, BM_ITEM_TIME_STAMP);
						
						if (pDevItemTimestamp)
						{
							sprintf(tmpTime,
									"%d/%d/%d %d:%d:%d",
									rtc_time.Year,
									rtc_time.Month,
									rtc_time.Date,
									rtc_time.Hour,
									rtc_time.Minute,
									rtc_time.Second);
							cJSON_DeleteItemFromObject(pDevNodeD, BM_ITEM_TIME_STAMP);
							cJSON_AddStringToObject(pDevNodeD, BM_ITEM_TIME_STAMP, tmpTime);
						}
					}
					
					/* Update temperature */
					if (THS_GetTemperature(&ftmp) == TRUE)
					{
						cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeD, BM_ITEM_TEMPERATURE), ftmp);
					}

					/* Update relative humidity */
					if (THS_GetRelativeHumidity(&ftmp) == TRUE)
					{
						cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeD, BM_ITEM_RH), ftmp + 0.001); // +0.001 be able to print out decimal point('.')
					}
				
					/* Update beep state */
					cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeD, BM_ITEM_BEEP), BM_BeepValue);
					
					message.payload = cJSON_Print(pDevNodeRoot);
					message.payloadlen = strlen(message.payload);
	#if BM_DEBUG
					printf("%sSend publish = %s\n\r", BM_PRINT_HEADER, (char *)message.payload);
	#endif
					if ((rc = MQTTPublish(&client, BM_PublishTopic, &message)) != 0)
					{
						printf("%sReturn code from MQTT publish is %d\n", BM_PRINT_HEADER, rc);
					}
					vPortFree(message.payload);
				}
				cJSON_Delete(pDevNodeRoot);
#if !defined(MQTT_TASK)
				/* Handle rx polling by assigned socket timeout */
				if ((rc = MQTTYield(&client, BM_YIELD_TIMEOUT)) != 0)
				{
					printf("%sReturn code from yield is %d\n", BM_PRINT_HEADER, rc);
				}
#endif
				break;
		}
		
		/* Task delay */
		vTaskDelay(BM_TASK_DELAY);
	}
	
BLUEMIX_Task_Exit:
	vTaskDelete(NULL);

} /* End of BLUEMIX_Task() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void BLUEMIX_MessageArrived(MessageData* data)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void BLUEMIX_MessageArrived(MessageData* data)
{
	char	*pTemp;
#if BM_DEBUG
	int		i;
#endif
	pTemp = (char*)data->message->payload;

#if BM_DEBUG
	printf("%sRecv publish on topic \"%s", BM_PRINT_HEADER);
	for (i = 0; i < data->topicName->lenstring.len; i++)
	{
		printf("%c", data->topicName->lenstring.data[i]);
	}
	printf("\"");
	printf(" = ");
	for (i = 0; i < data->message->payloadlen; i++)
	{
		printf("%c", pTemp[i]);
	}
	printf("\n\r");
#endif
	
	/* Convert JSON string to object */
	pDevSubNodeRoot = BLUEMIX_ConvertJsonStrToObj((U8_T *)pTemp);
	
	/* Parse beep item and reference it */
	if (pDevSubNodeRoot)
	{
		pDevSubNodeD = cJSON_GetObjectItem(pDevSubNodeRoot, "a");
		if (pDevSubNodeD)
		{
			pDevSubItemBeep = cJSON_GetObjectItem(pDevSubNodeD, BM_ITEM_BEEP);
			if (pDevSubItemBeep)
			{
				BM_BeepValue = pDevSubItemBeep->valueint;
#if BM_DEBUG
				printf("%s\n\r", BM_BeepValue ? "Alarm" : "Normal");
#endif
				GPIOS_BeepCtrl(BM_BeepValue);
			}
		}
		cJSON_Delete(pDevSubNodeRoot);
	}

} /* End of BLUEMIX_MessageArrived() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void cJSON* BLUEMIX_ConvertJsonStrToObj(u8 *pJsonStr)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* BLUEMIX_ConvertJsonStrToObj(u8 *pJsonStr)
{
	cJSON *pJsonObj = 0;
	
	if (pJsonStr == 0)
	{
		return pJsonObj;
	}
	
	pJsonObj = cJSON_Parse(pJsonStr);
	
	if (pJsonObj == 0)
	{
		printf("%sConvert JSON string to object fail!\r\n", BM_PRINT_HEADER);
	}
	
	return pJsonObj;

} /* End of BLUEMIX_ConvertJsonStrToObj() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void cJSON* BLUEMIX_AllocNode(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* BLUEMIX_AllocNode(void)
{
	cJSON	*pJsonObj = 0;
	
	sprintf(jsonBuffer, "{"
						"\"d\": {"
						"\"%s\":\"%s\","
						"\"%s\":\"%s\","								  
						"\"%s\":\"%s\","								  
						"\"%s\":%d,"
						"\"%s\":%d,"
						"\"%s\":%d"
						"}"
						"}"
						, BM_ITEM_SERIAL_NUMBER, ""
						, BM_ITEM_NAME, ""
						, BM_ITEM_TIME_STAMP, ""
						, BM_ITEM_TEMPERATURE, 0
						, BM_ITEM_RH, 0
						, BM_ITEM_BEEP, 0
						);
	
	pJsonObj = BLUEMIX_ConvertJsonStrToObj(jsonBuffer);
	return pJsonObj;

} /* End of BLUEMIX_AllocNode() */
#endif
/* End of bluemix.c */

