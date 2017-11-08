/*
 *********************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: sntpc.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __SNTPC_H__
#define __SNTPC_H__


/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define SNTPC_DEBUG_ENABLE			0   
#define SNTP_SERVER_PORT			123
#define SNTPC_MAX_REQ_QUEUE			3
#define SNTPC_MAX_REQ_RETRY			3
#define SNTPC_REQUEST_TIMEOUT		5 //sec

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */
typedef enum
{
	SNTPC_IDLE = 0,
	SNTPC_OK,	
	SNTPC_FAIL,	
	SNTPC_GET_SERVER_IP,
	SNTPC_GET_SERVER_IP_WAIT,
	SNTPC_CHECK_REQUEST_QUEUE,
	SNTPC_GET_SOCKET,
	SNTPC_SEND_REQUEST,	
	SNTPC_WAIT_RESPONSE,		
} SNTPC_TASK_STATE;

typedef struct _SNTP_HEADER
{
	u8	flags;
	u8	stratum;
	u8	poll;
	u8	precision;
	u32	root_delay;
	u32	root_dispersion;
	u32	reference_ID;
	u32	reference_time1;
	u32	reference_time2;
	u32	originate_time1;
	u32	originate_time2;
	u32	receive_time1;
	u32	receive_time2;
	u32	transmit_time1;
	u32	transmit_time2;
} SNTP_HEADER;

typedef struct _SNTPC_TIME
{
	u8	Second;
	u8	Minute;
	u8	Hour;
	u8	Day;
	u8	Date;
	u8	Month;
	u16	Year;
	u8	IsLeapYear;	
} SNTPC_TIME;

typedef struct _SNTPC_INFO
{
	SNTP_HEADER	*sntpcPktPtr;
	u32		timestamp;
	u32		second_remain;
	u32		day_total;
	u16		YY;
	u8		MM;
	u16		DD;
	s16		DD_r;
	u8		HH;
	u8		MI;
	u16		MI_r;
	u8		SS;
} SNTPC_INFO;

typedef struct _SNTPC_REQUEST
{
	void (*pCb)(SNTPC_INFO *pInfo);	
	u32	LatencyTimer;	
} SNTPC_REQUEST;

typedef struct _SNTPC_CTRL
{
	SNTPC_TASK_STATE State;
	s16	Socket;  	
	u32	ServerIp;	
	u8	ReqRetryCnt;
	u16	Timeout;	
	u8 *Txbuf;
	u8 *Rxbuf;	
	SNTPC_INFO Info;

	u8	CurrSrvIndex;	
	u8	CurrReqIndex;	
	SNTPC_REQUEST *pReqs[SNTPC_MAX_REQ_QUEUE];
} SNTPC_CTRL;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void SNTPC_Init(void);
u8 SNTPC_ReqEnqueue(void (*pCb)(SNTPC_INFO *pInfo), u32 LatencyTime);
u8 SNTPC_GetWeekDay(u16 year, u8 month, u8 day);
void SNTPC_AdjustTimeforDST(SNTPC_TIME *pTime);
#endif /* __SNTPC_H__ */

