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
 * Module name: smtpc.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __SMTPC_H__
#define __SMTPC_H__

/* INCLUDE FILE DECLARATIONS */
#include "ssl.h"
#include "gconfig.h"

/* NAMING CONSTANT DECLARATIONS */
#define SMTPC_DEBUG_ENABLE		0   
#define SMTP_SERVER_PORT		25
#define SMTP_SERVER_SSL_PORT	465
#define SMTP_SERVER_TLS_PORT	587
#define SMTPC_MAX_TX_BUF_SIZE	512
#define SMTPC_MAX_RX_BUF_SIZE	512
#define SMTPC_MAX_REQ_QUEUE		5
#define SMTPC_MAX_MAIL_ADDR_LEN	GCONFIG_MAX_MAILADDR_LEN
#define SMTPC_MAX_CONN_RETRY	3

/* MACRO DECLARATIONS */
#define SMTPC_MsgBuild sprintf
   
/* TYPE DECLARATIONS */
typedef enum
{
	SMTPC_OK = 0,	
	SMTPC_ERR,	
	SMTPC_AUTH_ERR,
	SMTPC_SECURITY_ERR,	
	SMTPC_BUSY,		
} SMTPC_STATUS;

typedef enum
{
	SMTPC_IDLE = 0,
	SMTPC_SUCCESS,	
	SMTPC_FAIL,	
	SMTPC_FREE_BLOCK,
	SMTPC_SET_SECURITY,
	SMTPC_GET_SERVER_IP,
	SMTPC_CHECK_QUEUE,
	SMTPC_CONNECT_TO_SERVER,
	SMTPC_MSG_EXCHANGE,	
} SMTPC_TASK_STATE;

typedef enum
{
	MSG_IDLE = 0,		
	MSG_READY_SENT,
	MSG_STARTTLS_SENT,	
	MSG_STARTTLS_CHK,	
	MSG_EHLO_SENT,	
	MSG_AUTH,
	MSG_MAIL_SEND_START,
	MSG_MAIL_FROM_SENT,
	MSG_RCV_TO_SENT,
	MSG_DATA_SENT,
	MSG_HEADER_SENT,
	MSG_BODY_SENT,
	MSG_QUIT_SENT,
} SMTPC_MSG_STATE;

typedef enum
{
	SMTPC_NO_AUTH = 0,
	SMTPC_AUTH_OAUTH2,	
} SMTPC_AUTH_TYPE;

typedef enum
{
	OAUTH2_IDLE = 0,	
	OAUTH2_SEND_USERNAME,
	OAUTH2_SEND_PASSWORD,
	OAUTH2_DONE,
} AUTH_OAUTH2_STATE;

typedef enum
{
	MSG_BUILD_DONE = 0,	
	MSG_BUILD_BUSY,	
} SMTPC_MSG_BUILD_STATUS;

typedef struct _SMTPC_REQUEST SMTPC_REQ;
typedef struct _SMTPC_REQUEST
{
	SMTPC_MSG_STATE	State;
	SMTPC_STATUS Status;
	u8	AuthState;
	u8	AuthType;	
	u8 *Txbuf;
	u8	CurrMailBoxToIndex;
	u8	MailBuildStatus;  
	u8	ValidMailBoxToNum;  
	SMTPC_MSG_BUILD_STATUS (*pMailBuildCb)(SMTPC_REQ *pReq);	  
	u8	MailBoxFrom[SMTPC_MAX_MAIL_ADDR_LEN];
	u8	MailBoxTo[1][SMTPC_MAX_MAIL_ADDR_LEN];
	/* Don't allocate any new member behind "MailBoxTo" */
} SMTPC_REQUEST;

typedef struct _SMTPC_CTRL
{
	SMTPC_TASK_STATE State;
	s16	Socket;
	ssl_context SSL;
	u8	CurrReqIndex;
	u8	ServerName[SMTPC_MAX_MAIL_ADDR_LEN];
	u32	ServerIp;
	u16	ServerPort;
	SMTPC_REQUEST *pReqs[SMTPC_MAX_REQ_QUEUE];
	u8	ConnRetryCnt;
	u8	Security;
	u8	CurrSecurity;
	u8	SecurityConnected;
} SMTPC_CTRL;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void SMTPC_Init(void);
SMTPC_STATUS SMTPC_MsgSend(u8 *pbuf, u16 len, u8 flag);
SMTPC_REQUEST* SMTPC_AllocateRequest(u8 mailToNum);
SMTPC_STATUS SMTPC_MailEnqueue(SMTPC_REQUEST *pReq);

#endif /* __SMTPC_H__ */

