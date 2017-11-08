/*
 *********************************************************************************
 *     Copyright (c) 2016   ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
/*================================================================================
 * Module Name : modbus_gateway.h
 * Purpose     : 
 * Author      : 
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef MODBUS_GATEWAY_H
#define MODBUS_GATEWAY_H

/* INCLUDE FILE DECLARATIONS */
#include "modbus.h"
#include "modbus_tcp.h"

/* NAMING CONSTANT DECLARATIONS */
#define MBGW_DEBUG_ENABLE			1
#define MBGW_BUF_SIZE				300
#define MBGW_SERIAL_PORTS			1
#define MBGW_SERIAL_BUF_SIZE		MBGW_BUF_SIZE
#define MBGW_DEFAULT_PORT_NUM		502
#define MBGW_MAX_TCP_CONNS			1
#define MBGW_TCP_BUF_SIZE			MBGW_BUF_SIZE
#define MBGW_SLAVE_ID				0xff

#if (MBGW_DEBUG_ENABLE)
#define MBGW_DEBUG(f, ...)	{printf("MBGW: "); printf(f, ## __VA_ARGS__);}
#else
#define MBGW_DEBUG(f, ...)	{}
#endif

/* TYPE DECLARATIONS */
typedef struct _MBGW_CONTROL
{
	MODBUS_NODE *pMaster[MBGW_SERIAL_PORTS];
	u32 RxCnt[MBGW_SERIAL_PORTS];
	u32 TxCnt[MBGW_SERIAL_PORTS];	
	MBTCP_SERVER *pServer;
	u8 Buf[MBGW_MAX_TCP_CONNS][MBGW_BUF_SIZE];
	u16 DataLen[MBGW_MAX_TCP_CONNS];
} MBGW_CONTROL;

/* GLOBAL VARIABLES */
extern MBGW_CONTROL MBGW_Ctrl;

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int MBGW_Init(void);
void MBGW_DeInit(void);
void MBGW_GetRxTxCounter(u8 SerialPortId, u32 *rxCounter, u32 *txCounter);
int MBGW_GetConnStatus(void);
#endif /* End of MODBUS_GATEWAY_H */
