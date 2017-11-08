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
 * Module Name: azure.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __AZURE_H__
#define __AZURE_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
/* Task */
#define AZ_DEBUG					1			// 0 = normal, 1 = debug
#define AZ_PRINT_HEADER				"Azure: "	// message header string
#define AZ_STACK_SIZE				2048		// byte
#define AZ_TX_BUFFER_SIZE			1024		// byte
#define AZ_RX_BUFFER_SIZE			1024		// byte
#define AZ_YIELD_TIMEOUT			1000		// ms, this field will supplant AZ_SOCKET_TIMEOUT while rx polling
#define AZ_PUBLISH_PERIOD			10000		// ms
#define AZ_TASK_DELAY				(AZ_PUBLISH_PERIOD - AZ_YIELD_TIMEOUT)	// ms
#define AZ_SOCKET_TIMEOUT			(AZ_PUBLISH_PERIOD * 4)	// ms
#define AZ_ENABLE_SSL_TLS			1			// 0 = disable, 1 = enable SSL/TLS connection
/* Protocol */
#define AZ_MQTT_BROKER_TCP_PORT		1883		// TCP port of MQTT broker
#define AZ_MQTT_BROKER_SSL_PORT		8883		// TCP port of MQTT broker over SSL/TLS
#define AZ_MQTT_PROTOCOL_VERSION	4			// MQTT version: 3 = 3.1, 4 = 3.1.1
#define AZ_MQTT_KEEPALIVE_TIMEOUT	60			// second
#define AZ_MQTT_CLEAN_SESSION		1			// 0 = not clean, 1 = clean
#define AZ_TCP_CONNECT_PERIOD		1000		// ms
#define AZ_TCP_CONNECT_CHKCNT_MAX	10			// max check count
#define AZ_MQTT_CONNECT_PERIOD		1000		// ms
#define AZ_MQTT_CONNECT_CHKCNT_MAX	10			// max check count

/* Application */
#define AZ_ITEM_SERIAL_NUMBER		"serial_number"		// serial number
#define AZ_ITEM_NAME				"name"				// device name
#define AZ_ITEM_TEMPERATURE			"temperature"		// temperature
#define AZ_ITEM_RH					"relative_humidity"	// relative humidity
#define AZ_ITEM_TIME_STAMP			"time_stamp"		// timestamp
#define AZ_ITEM_BEEP				"beep"				// beep

/* TYPE DECLARATIONS */
/* State */
typedef enum {
	AZ_STATE_IDLE = 0,
	AZ_STATE_INIT,
	AZ_STATE_NETWORK_CHECKING,
	AZ_STATE_TCP_CONNECTING,
	AZ_STATE_MQTT_CONNECTING,
	AZ_STATE_MQTT_SUBSCRIBING,
	AZ_STATE_RUNNING,
	AZ_STATE_ERROR_HOSTNAME,
	AZ_STATE_ERROR_ACCOUNT,
} AZURE_STATE;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void AZURE_Init(void);
char *AZURE_CheckConnectionState(void);

#endif /* End of __AZURE_H__ */


/* End of azure.h */
