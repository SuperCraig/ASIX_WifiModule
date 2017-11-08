/*
 ******************************************************************************
 *     Copyright (c) 2012	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
/*=============================================================================
 * Module Name: httpd.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__

/* INCLUDE FILE DECLARATIONS */
   
/* NAMING CONSTANT DECLARATIONS */
#define HTTPD_DEBUG_ENABLE		0   
#define HTTPD_DEFAULT_MAX_POST_COUNT	35
#define HTTPD_DEFAULT_MAX_DIVIDE_NUM	35
#define HTTPD_MAX_TX_BUF_SIZE	512
#define HTTPD_MAX_RX_BUF_SIZE	1520
#define HTTPD_DEFAULT_MAX_CONNECTS		8
#define HTTPD_SERVER_PORT		80

/* TYPE DECLARATIONS */
typedef enum
{
	HTTPD_STATE_FREE = 0,
	HTTPD_STATE_RESERVED,
	HTTPD_STATE_ACTIVE,
	HTTPD_STATE_RX_DATA,		
	HTTPD_STATE_SEND_HEADER,
	HTTPD_STATE_SEND_DATA,
	HTTPD_STATE_SEND_FINAL,
	HTTPD_STATE_SEND_NONE,
} HTTPD_STATE;

typedef enum _HTTPD_STATUS
{
	HTTPD_OK = 0,  
	HTTPD_ERR,
	HTTPD_BLANK_LINE_NOT_FOUND,	
	HTTPD_PARTIAL_POST,	/* added for compatibility with firefox v2.0.0.20 */
	HTTPD_NO_MATCHED_RESOURCE = -1,
	HTTPD_UNSPECIFIED_RESOURCE = -2,	
} HTTPD_STATUS;

typedef struct _FILE_DIVIDE
{
	U8_T	Fragment;
	U8_T	CurIndex;
	U8_T	PadFlag;
	U8_T*	PData;
	U16_T	LeftLen;
	U16_T	*Offset;
	U8_T	*RecordIndex;
	U8_T	*PostType;
	U8_T	*SetFlag;
} FILE_DIVIDE;

typedef struct _HTTPD_SERVER_CONN
{
	U32_T	Ip;
	U16_T	Port;
	U8_T	State;
	U8_T	Socket;
	U16_T	Timer;
	U8_T	FileId;
	U8_T	Method;
	U8_T	IsCgiPost;
	U8_T	PartialPost;
	U8_T	ContentType;
	FILE_DIVIDE	Divide;
	U8_T	Index;
} HTTPD_SERVER_CONN;

typedef struct _HTTPD_POST_ITEM
{
	U8_T	*pName;
	U8_T	NameLen;	
	U8_T	*pValue;
	U8_T	ValueLen;
} HTTPD_POST_ITEM;

typedef struct _HTTPD_SERVER_INFO
{
	struct _HTTPD_CALLBACK *pCbs;  
  	U8_T	*pRxBuf;
  	U16_T	RxLen;	
	U8_T	*pTxBuf;
  	U8_T	LoginPage;
	U8_T	HomePage;
	U8_T	PostItems;
  	U8_T	MaxConnNumber;	
  	U8_T	MaxPostCount;
  	U8_T	MaxDivideNumber;	
	HTTPD_POST_ITEM	*Posts;	
	U8_T	NativePostItems;
	U8_T	*NativePosts;
	U8_T	*TmpBuf;
} HTTPD_SERVER_INFO;

typedef struct _HTTPD_CALLBACK
{
	HTTPD_STATUS (*Init)(HTTPD_SERVER_INFO *pInfo);
	HTTPD_STATUS (*ResponseDone)(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
	HTTPD_STATUS (*GetMethod)(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
	HTTPD_STATUS (*CgiScript)(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);	
	HTTPD_STATUS (*PostMethod)(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
	HTTPD_STATUS (*NewConn)(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);	
} HTTPD_CALLBACK;

/* GLOBAL VARIABLES */
extern HTTPD_SERVER_INFO HTTPD_Info;

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void HTTPD_Init(HTTPD_CALLBACK *);
void HTTPD_Task(void *pvParameters);
U8_T HTTPD_NewConn(U32_T*, U16_T, U8_T);
void HTTPD_Event(U8_T, U8_T);
void HTTPD_Receive(U8_T*, U16_T, U8_T);

#endif /* End of __HTTPD_H__ */


/* End of httpd.h */
