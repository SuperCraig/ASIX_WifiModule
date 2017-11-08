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
 * Module Name: gnapp.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __GNAPP_H__
#define __GNAPP_H__

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include "gconfig.h"
   
/* NAMING CONSTANT DECLARATIONS */
#define GNAPP_DEBUG_ENABLE	0
#define GNAPP_SERVER_PORT	443
#define GNAPP_TX_BUF_SIZE	1024
#define GNAPP_RX_BUF_SIZE	1024
#define GNAPP_POLLING_INTERVAL	0//sec
#define GNAPP_ALIVE_TIMEOUT		30//sec
   
#define GNAPP_ON	"ON"
#define GNAPP_OFF	"OFF"

typedef enum {
	GNAPP_OK = 0,
	GNAPP_ERR,
	GNAPP_CONNECTED,	
	GNAPP_DISCONNECTED,		
} GNAPP_STATUS;

typedef enum {
	GNAPP_INIT = 0,
	GNAPP_CHECK_DEVICE_NODE,
	GNAPP_CREATE_NEW_DEVICE_NODE,	
	GNAPP_PREODIC_UPDATE,		
} GNAPP_TASK_STATE;

typedef struct _GNAPP_CTRL_
{
	GNAPP_TASK_STATE  TaskState;
	u8	HostName[GCONFIG_MAX_CLOUD_HOSTNAME_LEN];
	u8	DevNodeName[30];
	u16 PollingTime;
	u8	TxBuf[GNAPP_TX_BUF_SIZE];
	u8	RxBuf[GNAPP_RX_BUF_SIZE];
	u32	AliveTime;
	u8	CloudDataPatched;	
} GNAPP_CTRL;

/* TYPE DECLARATIONS */


/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void GNAPP_Init(void);
GNAPP_STATUS GNAPP_CheckConnectionStatus(void);

#endif /* End of __GNAPP_H__ */


/* End of gnapp.h */
