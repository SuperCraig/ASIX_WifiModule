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
 * Module Name: azure.c
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
#include "azure.h"
#include "MQTTClient.h"
#include "wifi_constants.h"
#include "wifi_conf.h"
#include "gconfig.h"
#include "cJSON.h"
#include "rtc.h"
#include "ths.h"
#include "misc.h"

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)

/* NAMING CONSTANT DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */
unsigned char		AZ_XmitBuf[AZ_TX_BUFFER_SIZE], AZ_RecvBuf[AZ_RX_BUFFER_SIZE];
char				AZ_HostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];										// MQTT hostname (Microsoft azure host name)
char				AZ_DeviceID[GCONFIG_MAX_AZURE_DEVICE_ID_LEN];										// MQTT Client ID (IoT device ID)
char				AZ_UserName[GCONFIG_MAX_AZURE_HOSTNAME_LEN + GCONFIG_MAX_AZURE_DEVICE_ID_LEN + 1];	// MQTT Username (hostname/iot_device_id)
char				AZ_Sas[GCONFIG_MAX_AZURE_SAS_LEN];													// MQTT Password (IoT device shared access signature)
char				AZ_HubName[GCONFIG_MAX_AZURE_HUBNAME_LEN];											// Microsoft azure IoT HUB name
char				AZ_SignedKey[GCONFIG_MAX_AZURE_SIGNED_KEY_LEN];										// Microsoft azure device signed key
char				AZ_ExpiryTime[GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN];									// Microsoft azure device expiry time
char				AZ_SubscribeTopic[64];																// Subscribe topic
char				AZ_PublishTopic[64];																// Publish topic
static cJSON_Hooks	memoryHook;
unsigned char		jsonBuffer[1024];
cJSON				*pDevSubNodeRoot = NULL;
cJSON				*pDevSubNodeD = NULL;
cJSON				*pDevSubItemBeep = NULL;
unsigned char		AZ_BeepValue = 0;
unsigned char		AZ_State = AZ_STATE_IDLE;

/* LOCAL VARIABLES DECLARATIONS */

/* LOCAL SUBPROGRAM DECLARATIONS */
void AZURE_ConnectDataInit(MQTTPacket_connectData *pConnectData);
void AZURE_Task(void *pvParameters);
void AZURE_MessageArrived(MessageData* data);
size_t AZURE_SasGeneration(u8 *pSasBuf, u8 sizeSasBuf, u8 *pIoTHubName, u8 *pDeviceId, u8 *pSignedKey, u8 *pExpiryTime);
static cJSON* AZURE_ConvertJsonStrToObj(u8 *pJsonStr);
static cJSON* AZURE_AllocNode(void);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: void AZURE_Init(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void AZURE_Init(void)
{
	if (GCONFIG_GetWifiNetworkMode() != RTW_MODE_STA)
		return;
	
	if (xTaskCreate(AZURE_Task, "AzureTask", AZ_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) == pdPASS)
	{
		/* Azure state init */
		AZ_State = AZ_STATE_INIT;
	}
	else
	{
		/* Azure create task failed */
		printf("%sCreate task failed!\r\n", AZ_PRINT_HEADER);
		AZ_State = AZ_STATE_IDLE;
	}
	
} /* End of AZURE_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: char *AZURE_CheckConnectionState(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
char *AZURE_CheckConnectionState(void)
{
	switch (AZ_State)
	{
		case AZ_STATE_IDLE:
			return "Task idle";
		case AZ_STATE_INIT:
			return "Variable init";
		case AZ_STATE_NETWORK_CHECKING:
			return "Network connection checking...";
		case AZ_STATE_TCP_CONNECTING:
			return "TCP connecting...";
		case AZ_STATE_MQTT_CONNECTING:
			return "MQTT connecting...";
		case AZ_STATE_MQTT_SUBSCRIBING:
			return "MQTT subscribing...";
		case AZ_STATE_RUNNING:
			return "MQTT running";
		case AZ_STATE_ERROR_HOSTNAME:
			return "Invalid MQTT host name, task abort";
		case AZ_STATE_ERROR_ACCOUNT:
			return "Invalid MQTT account, task abort";
	}
	
	return "Unknow";
	
} /* End of AZURE_CheckConnectionState() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void AZURE_ConnectDataInit(MQTTPacket_connectData *pConnectData)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void AZURE_ConnectDataInit(MQTTPacket_connectData *pConnectData)
{
	pConnectData->struct_id[0] = 'M';
	pConnectData->struct_id[1] = 'Q';
	pConnectData->struct_id[2] = 'T';
	pConnectData->struct_id[3] = 'C';
	pConnectData->struct_version = 0;
	/* MQTT version: 3 = 3.1, 4 = 3.1.1 */
#if (AZ_MQTT_PROTOCOL_VERSION == 4)
	pConnectData->MQTTVersion = 4;
#else
	pConnectData->MQTTVersion = 3;
#endif
	pConnectData->clientID.cstring = NULL;
	pConnectData->clientID.lenstring.len = 0;
	pConnectData->clientID.lenstring.data = NULL;
	pConnectData->keepAliveInterval = AZ_MQTT_KEEPALIVE_TIMEOUT;
	pConnectData->cleansession = AZ_MQTT_CLEAN_SESSION;
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

} /* End of AZURE_ConnectDataInit() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void AZURE_Task(void *pvParameters)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void AZURE_Task(void *pvParameters)
{
	MQTTClient				client;
	Network					network;
	int						rc;
	unsigned char			connectCheckCount;
	MQTTPacket_connectData	connectData;
	MQTTMessage				message;
	cJSON					*pDevNodeRoot;
//	cJSON					*pDevNodeD;
	cJSON					*pDevItemTimestamp;
	RTC_TIME				rtc_time;
	unsigned char			tmpTime[30];
	float					ftmp;
	float					frh;
	unsigned short			server_port;
#if AZ_ENABLE_SSL_TLS
	unsigned char			enable_ssl_tls = 1;	
#else
	unsigned char			enable_ssl_tls = 0;	
#endif
	
AZURE_Task_Restart:

	while (1)
	{
		switch (AZ_State)
		{
			case AZ_STATE_INIT:
				/* Init local variable */
				rc = 0;
				connectCheckCount = 0;
				AZURE_ConnectDataInit(&connectData);
				pDevNodeRoot = NULL;
//				pDevNodeD = NULL;
				pDevItemTimestamp = NULL;
				
				/* Init network and client data structure */
				NetworkInit(&network, enable_ssl_tls);
				MQTTClientInit(&client, &network, AZ_SOCKET_TIMEOUT, AZ_XmitBuf, AZ_TX_BUFFER_SIZE, AZ_RecvBuf, AZ_RX_BUFFER_SIZE);

				/* Do network checking */
				AZ_State = AZ_STATE_NETWORK_CHECKING;
				continue;
			case AZ_STATE_NETWORK_CHECKING:
				/* Check WIFI connection */
				printf("\n\r%sWait for the device to connect Wi-Fi...\n\r", AZ_PRINT_HEADER);
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
				while (MISC_NetInterfaceReadyCheck() != IF_STATE_IP_READY)
#else
				while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
#endif
				{
					vTaskDelay(1000 / portTICK_PERIOD_MS);
				}
				printf("%sWi-Fi connected\n\r", AZ_PRINT_HEADER);
				
				/* Do TCP connection */
				AZ_State = AZ_STATE_TCP_CONNECTING;
				continue;
			case AZ_STATE_TCP_CONNECTING:
				/* Get azure host name */
				GCONFIG_GetAzureHostName(AZ_HostName, GCONFIG_MAX_AZURE_HOSTNAME_LEN);
				
				/* Check if host name is not null */
				if (AZ_HostName[0] == 0)
				{
					printf("%sInvalid host name, exit the task!!\n\r", AZ_PRINT_HEADER);
					
					/* Record error and abort the task */
					AZ_State = AZ_STATE_ERROR_HOSTNAME;
					goto AZURE_Task_Exit;
				}

				/* Get azure IoT HUB name, device id, signed key and expiry time */
				GCONFIG_GetAzureAccount(AZ_HubName, GCONFIG_MAX_AZURE_HUBNAME_LEN,
										AZ_DeviceID, GCONFIG_MAX_AZURE_DEVICE_ID_LEN,
										AZ_SignedKey, GCONFIG_MAX_AZURE_SIGNED_KEY_LEN,
										AZ_ExpiryTime, GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN);
				
				/* Generate SAS */
				if (AZURE_SasGeneration(AZ_Sas,
										GCONFIG_MAX_AZURE_SAS_LEN,
										AZ_HubName,
										AZ_DeviceID,
										AZ_SignedKey,
										AZ_ExpiryTime) < 0)
				{
					printf("%sAZURE_SasGeneration() failed\n\r", AZ_PRINT_HEADER);
					return;
				}
				
				/* Check if account information is null */
				if ((AZ_HubName[0] == 0) || (AZ_DeviceID[0] == 0) || (AZ_SignedKey[0] == 0) ||
					(AZ_ExpiryTime[0] == 0) || (AZ_Sas[0] == 0))
				{
					printf("%sInvalid account information, exit the task!!\n\r", AZ_PRINT_HEADER);
					
					/* Record error and abort the task */
					AZ_State = AZ_STATE_ERROR_ACCOUNT;
					goto AZURE_Task_Exit;
				}

				/* Init the socket and do TCP connection with MQTT broker */
				server_port = (enable_ssl_tls == 0) ? AZ_MQTT_BROKER_TCP_PORT:AZ_MQTT_BROKER_SSL_PORT;
				printf("%sConnect to host \"%s:%d\" ...\n\r", AZ_PRINT_HEADER, AZ_HostName, server_port);
				while ((rc = NetworkConnect(&network, AZ_HostName, server_port)) < 0)
				{
					printf("%sReturn code from NetworkConnect() is %d\n", AZ_PRINT_HEADER, rc);
					/* Error handling for TCP connection failed */
					if (++connectCheckCount > AZ_TCP_CONNECT_CHKCNT_MAX)
					{
#if AZ_DEBUG
						printf("%sTCP connect failed, redo the MQTT client session\n\r", AZ_PRINT_HEADER);
#endif
						/* NetworkConnect() will close socket while failed so that unnecessary redo here */
						/* Restart MQTT client session */
						AZ_State = AZ_STATE_INIT;
						goto AZURE_Task_Restart;
					}
					vTaskDelay(AZ_TCP_CONNECT_PERIOD / portTICK_PERIOD_MS);
				}
				printf("%s\"%s\" connected\n\r", AZ_PRINT_HEADER, AZ_HostName);
				
				/* Do MQTT connection */
				AZ_State = AZ_STATE_MQTT_CONNECTING;
				continue;
			case AZ_STATE_MQTT_CONNECTING:
				/* Reset check count */
				if (connectCheckCount)
				{
					connectCheckCount = 0;
				}
				
				/* Prepare username */
				sprintf(AZ_UserName, "%s/%s", AZ_HostName, AZ_DeviceID);
				
				/* Fill connection setting */
				connectData.clientID.cstring = AZ_DeviceID;
				connectData.username.cstring = AZ_UserName;
				connectData.password.cstring = AZ_Sas;

				/* Do MQTT connection with MQTT broker */
				printf("%sDo MQTT connection ...\n\r", AZ_PRINT_HEADER);
				while ((rc = MQTTConnect(&client, &connectData)) != 0)
				{
					printf("%sReturn code from MQTTConnect() is %d\n", AZ_PRINT_HEADER, rc);
					/* Error handling for MQTT connection failed */
					if (++connectCheckCount > AZ_MQTT_CONNECT_CHKCNT_MAX)
					{
#if AZ_DEBUG
						printf("%sMQTT connect failed, redo the MQTT client session\n\r", AZ_PRINT_HEADER);
#endif
						/* Close socket */
						network.disconnect(&network);
						
						/* Restart MQTT client session */
						AZ_State = AZ_STATE_INIT;
						goto AZURE_Task_Restart;
					}
					vTaskDelay(AZ_MQTT_CONNECT_PERIOD / portTICK_PERIOD_MS);
				}
				printf("%sMQTT connected\n\r", AZ_PRINT_HEADER);
				
				/* Prepare contents of subscribe and publish topic */
				sprintf(AZ_SubscribeTopic, "devices/%s/messages/devicebound/#", AZ_DeviceID);
				sprintf(AZ_PublishTopic, "devices/%s/messages/events/", AZ_DeviceID);
				
				/* Do subscribe topic */
				AZ_State = AZ_STATE_MQTT_SUBSCRIBING;
				continue;
			case AZ_STATE_MQTT_SUBSCRIBING:
				/* Subscribe the topic */
				printf("%sSubscribe command \"%s\"\n\r", AZ_PRINT_HEADER, AZ_SubscribeTopic);
				if ((rc = MQTTSubscribe(&client, AZ_SubscribeTopic, QOS1, AZURE_MessageArrived)) != 0)
				{
					printf("%sReturn code from MQTTSubscribe() is %d\n", AZ_PRINT_HEADER, rc);
					/* Error handling for subscribe failed */
					if (rc == TX_FAILURE)
					{
#if AZ_DEBUG
						printf("%sSubscribe failed, redo the MQTT client session\n\r", AZ_PRINT_HEADER);
#endif
						/* Disconnect to MQTT broker */
						MQTTDisconnect(&client);
						
						/* Close socket */
						network.disconnect(&network);
						
						/* Restart MQTT client session */
						AZ_State = AZ_STATE_INIT;
						goto AZURE_Task_Restart;
					}
				}
				else
				{
					/* Hook JSON */
					memoryHook.malloc_fn = pvPortMalloc;
					memoryHook.free_fn = vPortFree;
					cJSON_InitHooks(&memoryHook);
				
					/* Enter running task */
					AZ_State = AZ_STATE_RUNNING;
					continue;
				}
				break;
			case AZ_STATE_RUNNING:
				/* Create new node */
				pDevNodeRoot = AZURE_AllocNode();
				
				if (pDevNodeRoot == 0)
				{
					printf("%sAllocate device JSON node failed!\n\r", AZ_PRINT_HEADER);
					break;
				}

				/* Assign QoS and retain field for publish message */
				message.qos = QOS1;
				message.retained = 1;
				
				/* Find out "d" object and update related members */
//				pDevNodeD = cJSON_GetObjectItem(pDevNodeRoot, "d");
				
//				if (pDevNodeD)
				{
					/* Update serial number */
					cJSON_DeleteItemFromObject(pDevNodeRoot, AZ_ITEM_SERIAL_NUMBER);
					cJSON_AddStringToObject(pDevNodeRoot, AZ_ITEM_SERIAL_NUMBER, AZ_DeviceID);
					
					/* Update device name */
					cJSON_DeleteItemFromObject(pDevNodeRoot, AZ_ITEM_NAME);
					cJSON_AddStringToObject(pDevNodeRoot, AZ_ITEM_NAME, (char*)GCONFIG_GetDeviceName());
					
					/* Update timestamp */
					if (RTC_GetCurrTime(&rtc_time) == TRUE)
					{
						pDevItemTimestamp = cJSON_GetObjectItem(pDevNodeRoot, AZ_ITEM_TIME_STAMP);
						
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
							cJSON_DeleteItemFromObject(pDevNodeRoot, AZ_ITEM_TIME_STAMP);
							cJSON_AddStringToObject(pDevNodeRoot, AZ_ITEM_TIME_STAMP, tmpTime);
						}
					}
					
					/* Update temperature */
					if (THS_GetTemperature(&ftmp) == TRUE)
					{
						cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeRoot, AZ_ITEM_TEMPERATURE), ftmp);
					}

					/* Update relative humidity */
					if (THS_GetRelativeHumidity(&ftmp) == TRUE)
					{
						cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeRoot, AZ_ITEM_RH), ftmp + 0.001); // +0.001 be able to print out decimal point('.')
					}
				
					/* Update beep state */
					cJSON_SetIntValue(cJSON_GetObjectItem(pDevNodeRoot, AZ_ITEM_BEEP), AZ_BeepValue);

					message.payload = cJSON_Print(pDevNodeRoot);
					message.payloadlen = strlen(message.payload);
#if AZ_DEBUG
					printf("%sSend publish = %s\n\r", AZ_PRINT_HEADER, (char *)message.payload);
#endif
					if ((rc = MQTTPublish(&client, AZ_PublishTopic, &message)) != 0)
					{
						printf("%sReturn code from MQTT publish is %d\n", AZ_PRINT_HEADER, rc);
					}
					vPortFree(message.payload);
				}
				cJSON_Delete(pDevNodeRoot);
#if !defined(MQTT_TASK)
				/* Handle rx polling by assigned socket timeout */
				if ((rc = MQTTYield(&client, AZ_YIELD_TIMEOUT)) != 0)
				{
					printf("%sReturn code from yield is %d\n", AZ_PRINT_HEADER, rc);
				}
#endif
				break;
		}
		
		/* Task delay */
		vTaskDelay(AZ_TASK_DELAY);
	}
	
AZURE_Task_Exit:
	vTaskDelete(NULL);

} /* End of AZURE_Task() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void AZURE_MessageArrived(MessageData* data)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void AZURE_MessageArrived(MessageData* data)
{
	char	*pTemp;
#if AZ_DEBUG
	int		i;
#endif
	pTemp = (char*)data->message->payload;

#if AZ_DEBUG
	printf("%sRecv publish on topic \"%s", AZ_PRINT_HEADER);
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
	pDevSubNodeRoot = AZURE_ConvertJsonStrToObj((U8_T *)pTemp);
	
	/* Parse beep item and reference it */
	if (pDevSubNodeRoot)
	{
		pDevSubNodeD = cJSON_GetObjectItem(pDevSubNodeRoot, "a");
		if (pDevSubNodeD)
		{
			pDevSubItemBeep = cJSON_GetObjectItem(pDevSubNodeD, AZ_ITEM_BEEP);
			if (pDevSubItemBeep)
			{
				AZ_BeepValue = pDevSubItemBeep->valueint;
#if AZ_DEBUG
				printf("%s\n\r", AZ_BeepValue ? "Alarm" : "Normal");
#endif
				GPIOS_BeepCtrl(AZ_BeepValue);
			}
		}
		cJSON_Delete(pDevSubNodeRoot);
	}

} /* End of AZURE_MessageArrived() */

size_t AZURE_SasGeneration(u8 *pSasBuf, u8 sizeSasBuf, u8 *pIoTHubName, u8 *pDeviceId, u8 *pSignedKey, u8 *pExpiryTime)
{
	s8		temporaryBuf[GCONFIG_MAX_AZURE_SAS_LEN];
	u8		b64Buf[128];
	u8		sha256Output[32];
	size_t	b64DecodeLen;
	size_t	b64EncodeLen;
	size_t	tempLen;
	u16		i;
	
	/* Prepare request string (resourceURI + '\n' + expiryTime) */
	sprintf(temporaryBuf, "%s.azure-devices.net%sdevices%s%s\n%s", pIoTHubName, "%2f", "%2f", pDeviceId, pExpiryTime);
	
	/* Decode key by base64 */
	memset(b64Buf, 0, 128);
	if (base64_decode(b64Buf, &b64DecodeLen, pSignedKey, strlen(pSignedKey)) != 0)
	{
		printf("%sbase64_decode() failed\n\r", AZ_PRINT_HEADER);
		return -1;
	}
	
	/* Do SHA-256 HMAC */
	sha256_hmac(b64Buf, b64DecodeLen, (u8 *)temporaryBuf, strlen(temporaryBuf), sha256Output, 0);
	
	/* Encode SHA-256 output by base64 */
	memset(b64Buf, 0, 128);
	if (base64_encode(b64Buf, &b64EncodeLen, sha256Output, 32) != 0)
	{
		printf("%sbase64_encode() failed\n\r", AZ_PRINT_HEADER);
		return -1;
	}

	/* Prepare entire SAS */
	/* Part1: SAS header + resourceURI + sig header */
	sprintf(temporaryBuf,
			"%s%s.azure-devices.net%sdevices%s%s%s",
			"SharedAccessSignature sr=",
			pIoTHubName,
			"%2f",
			"%2f",
			pDeviceId,
			"&sig=");
	
	/* Copy and convert sig field into SAS output */
	tempLen = strlen(temporaryBuf);
	for (i = 0; i < b64EncodeLen; i++)
	{
		if (b64Buf[i] == 0x2B)
		{
			temporaryBuf[i + tempLen] = '%';
			temporaryBuf[i + tempLen + 1] = '2';
			temporaryBuf[i + tempLen + 2] = 'b';
			tempLen += 2;
		}
		else if (b64Buf[i] == 0x2F)
		{
			temporaryBuf[i + tempLen] = '%';
			temporaryBuf[i + tempLen + 1] = '2';
			temporaryBuf[i + tempLen + 2] = 'f';
			tempLen += 2;
		}
		else if (b64Buf[i] == 0x3D)
		{
			temporaryBuf[i + tempLen] = '%';
			temporaryBuf[i + tempLen + 1] = '3';
			temporaryBuf[i + tempLen + 2] = 'd';
			temporaryBuf[i + tempLen + 3] = 0x00;
			tempLen += 3;
		}
		else
		{
			temporaryBuf[i + tempLen] = b64Buf[i];
		}
	}
	
	/* Append expiry time field */
	strcat(temporaryBuf, "&se=");
	strcat(temporaryBuf, pExpiryTime);
	
	/* Final buffer length check */
	tempLen = strlen(temporaryBuf);
	if (tempLen >= sizeSasBuf)
	{
		printf("%sout of buffer length\n\r", AZ_PRINT_HEADER);
		return -1;
	}
	
	/* Copy final SAS result */
	strcpy(pSasBuf, temporaryBuf);
	
	return tempLen;
	
} /* End of AZURE_SasGeneration() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void cJSON* AZURE_ConvertJsonStrToObj(u8 *pJsonStr)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* AZURE_ConvertJsonStrToObj(u8 *pJsonStr)
{
	cJSON *pJsonObj = 0;
	
	if (pJsonStr == 0)
	{
		return pJsonObj;
	}
	
	pJsonObj = cJSON_Parse(pJsonStr);
	
	if (pJsonObj == 0)
	{
		printf("%sConvert JSON string to object fail!\r\n", AZ_PRINT_HEADER);
	}
	
	return pJsonObj;

} /* End of AZURE_ConvertJsonStrToObj() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void cJSON* AZURE_AllocNode(void)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static cJSON* AZURE_AllocNode(void)
{
	cJSON	*pJsonObj = 0;
	
	sprintf(jsonBuffer, "{"
//						"\"d\": {"
						"\"%s\":\"%s\","
						"\"%s\":\"%s\","
						"\"%s\":\"%s\","								  
						"\"%s\":%d,"
						"\"%s\":%d,"
						"\"%s\":%d"
//						"}"
						"}"
						, AZ_ITEM_SERIAL_NUMBER, ""
						, AZ_ITEM_NAME, ""
						, AZ_ITEM_TIME_STAMP, ""
						, AZ_ITEM_TEMPERATURE, 0
						, AZ_ITEM_RH, 0
						, AZ_ITEM_BEEP, 0
						);
	
	pJsonObj = AZURE_ConvertJsonStrToObj(jsonBuffer);
	return pJsonObj;

} /* End of AZURE_AllocNode() */
#endif
/* End of azure.c */

