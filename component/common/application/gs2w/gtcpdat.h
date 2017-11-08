/*
 *********************************************************************************
 *     Copyright (c) 2015   ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
/*================================================================================
 * Module Name : gtcpdat.h
 * Purpose     :
 * Author      :
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef GTCPDAT_H
#define GTCPDAT_H

/* INCLUDE FILE DECLARATIONS */
#include "semphr.h"

/* NAMING CONSTANT DECLARATIONS */
#define GTCPDAT_STACK_SIZE			1024
#define GTCPDAT_MAX_CONNS			4
#define GTCPDAT_TX_MAX_LEN			1460
#define GTCPDAT_TX_BUF_SIZE			(GTCPDAT_TX_MAX_LEN * 2)
#define GTCPDAT_RX_BUF_SIZE			1460
#define GTCPDAT_TIMEOUT				1
#define GTCPDAT_XOFF_TIMEOUT		30000//msec

#define GTCPDAT_STATE_FREE			0
#define GTCPDAT_STATE_ACTIVE		1
#define	GTCPDAT_STATE_CONNECTED		2
#define GTCPDAT_NO_NEW_CONN			0xFF


/* TYPE DECLARATIONS */
typedef struct _GTCPDAT_CONN_T
{
	u8		Id;
	u8		State;
	u32		RemoteIp;
	u16		RemotePort;
	s16		Socket;
	u32		TxWaitTime;
	u32		UrRxBytes;
    u32		UrTxBytes;
    u16		TxBufIndex;
    u8		TxBuf[GTCPDAT_TX_BUF_SIZE];
    u8		FlowControlXonRx;
	u32		FlowControlXonLastTime;
	u8		FlowControlModemStatus;
	u8		FlowControlModemCtrl;
	u8		FlowControlVirModemStatus;
	xSemaphoreHandle	AccessProtection;
} GTCPDAT_CONN_T;


/* GLOBAL VARIABLES */


/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void GTCPDAT_Init(void);
s32 GTCPDAT_GetUrTxBytes(u8 connId);
s32 GTCPDAT_GetUrRxBytes(u8 connId);
s32 GTCPDAT_GetConnStatus(u8 connId);
s16 GTCPDAT_Connect(u8 connId, u32 DestIp, u16 DestPort);
s16 GTCPDAT_Close(u8 connId);
s16 GTCPDAT_IsManualConnectionMode();

#endif /* End of GTCPDAT_H */
