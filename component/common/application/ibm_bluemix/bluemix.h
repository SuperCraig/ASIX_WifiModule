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
 * Module Name: bluemix.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __BLUEMIX_H__
#define __BLUEMIX_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
/* Task */
#define BM_DEBUG					1			// 0 = normal, 1 = debug
#define BM_PRINT_HEADER				"Bluemix: "	// message header string
#define BM_STACK_SIZE				2048		// byte
#define BM_TX_BUFFER_SIZE			1024		// byte
#define BM_RX_BUFFER_SIZE			1024		// byte
#define BM_YIELD_TIMEOUT			1000		// ms, this field will supplant BM_SOCKET_TIMEOUT while rx polling
#define BM_PUBLISH_PERIOD			3000		// ms
#define BM_TASK_DELAY				(BM_PUBLISH_PERIOD - BM_YIELD_TIMEOUT)	// ms
#define BM_SOCKET_TIMEOUT			(BM_PUBLISH_PERIOD * 4)	// ms
#define BM_ENABLE_SSL_TLS			0			// 0 = disable, 1 = enable SSL/TLS connection
#define BM_AUTO_REG_ENABLE			0			// 0 = disable, 1 = enable
#define BM_MANUFACTURER				"ASIX Electronics Corporation"
/* Protocol */
#define BM_MQTT_BROKER_TCP_PORT		1883		// TCP port of MQTT broker
#define BM_MQTT_BROKER_SSL_PORT		8883		// TCP port of MQTT broker over SSL/TLS
#define BM_MQTT_PROTOCOL_VERSION	3			// MQTT version: 3 = 3.1, 4 = 3.1.1
#define BM_MQTT_KEEPALIVE_TIMEOUT	60			// second
#define BM_MQTT_CLEAN_SESSION		1			// 0 = not clean, 1 = clean
#define BM_TCP_CONNECT_PERIOD		1000		// ms
#define BM_TCP_CONNECT_CHKCNT_MAX	10			// max check count
#define BM_MQTT_CONNECT_PERIOD		1000		// ms
#define BM_MQTT_CONNECT_CHKCNT_MAX	10			// max check count
#define BM_MQTT_SUBSCRIBE_TOPIC		"iot-2/cmd/beep/fmt/json";	// format = "iot-2/cmd/<cmd-type>/fmt/<format-id>"
#define BM_MQTT_PUBLISH_TOPIC		"iot-2/evt/001/fmt/json";	// format = "iot-2/evt/<event-id>/fmt/<format>"
/* Application */
#define BM_ITEM_SERIAL_NUMBER		"serial_number"		// serial number
#define BM_ITEM_NAME				"name"				// device name
#define BM_ITEM_TEMPERATURE			"temperature"		// temperature
#define BM_ITEM_RH					"relative_humidity"	// relative humidity
#define BM_ITEM_TIME_STAMP			"time_stamp"		// timestamp
#define BM_ITEM_BEEP				"beep"				// beep

/* TYPE DECLARATIONS */
/* State */
typedef enum {
	BM_STATE_IDLE = 0,
	BM_STATE_INIT,
	BM_STATE_NETWORK_CHECKING,
	BM_STATE_LOAD_SETTINGS,	
	BM_STATE_AUTO_REG,
	BM_STATE_TCP_CONNECTING,
	BM_STATE_MQTT_CONNECTING,
	BM_STATE_MQTT_SUBSCRIBING,
	BM_STATE_RUNNING,
	BM_STATE_ERROR_HOSTNAME,
	BM_STATE_ERROR_ACCOUNT,
	BM_STATE_ERROR_REGISTER_INFO,
} BLUEMIX_STATE;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void BLUEMIX_Init(void);
char *BLUEMIX_CheckConnectionState(void);

#endif /* End of __BLUEMIX_H__ */


/* End of bluemix.h */
