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
 * Module Name: axhttpc.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * no message
 *
 *=============================================================================
 */

#ifndef __AXHTTPC_H__
#define __AXHTTPC_H__

/* INCLUDE FILE DECLARATIONS */
#include "ssl.h"

/* NAMING CONSTANT DECLARATIONS */
#define AXHTTPC_RX_TIMEOUT			10000	// unit in ms
#define AXHTTPC_RX_FRAGMENT_TIMEOUT	100		// unit in ms   
#define AXHTTPC_TX_TIMEOUT			1000	// unit in ms
#define AXHTTPC_DEF_XFER_BUF_SIZE	1024

   
	/* Error code */
#define AXHTTPC_FAIL					-0x0001   
#define AXHTTPC_BAD_ARGUMENT			-0x0002
#define AXHTTPC_MEM_ALLOCATION_FAIL		-0x0003
#define AXHTTPC_BAD_RESPONSE			-0x0004
#define AXHTTPC_CONNECTION_ERR			-0x0005
#define AXHTTPC_RECV_TIMEOUT			-0x0006

	/* HTTP version */
#define AXHTTPC_HTTP_V1_1				"HTTP/1.1"

	/* Method */
#define AXHTTPC_GET_METHOD				"GET"
#define AXHTTPC_POST_METHOD				"POST"
#define AXHTTPC_PUT_METHOD				"PUT"
#define AXHTTPC_DELETE_METHOD			"DELETE"

	/* HTTP header Name */
#define AXHTTPC_HDR_USER_AGENT			"User-Agent"
#define AXHTTPC_HDR_CONTENT_TYPE		"Content-Type"
#define AXHTTPC_HDR_CONTENT_LENGTH		"Content-Length"
#define AXHTTPC_HDR_HOST				"Host"
#define AXHTTPC_HDR_ACCEPT				"Accept"

	/* HTTP header value */
#define AXHTTPC_CTYPE_APP_JSON			"application/json"

/* TYPE DECLARATIONS */
typedef enum
{
	BASIC_AUTH = 0,
} AXHTTPC_AUTH_TYPE;

typedef struct _AXHTTPC_HEADER AXHTTPC_HEADER;
struct _AXHTTPC_HEADER
{
	unsigned char *pName;
	unsigned char *pValue;
	AXHTTPC_HEADER *pNext;
};

typedef struct _AXHTTPC_SESSION AXHTTPC_SESSION;
struct _AXHTTPC_SESSION
{
	unsigned char	state;
	int				socket;
	unsigned char	sslEnable;
	unsigned char	sslInited;
	ssl_context		SSL;
	int (*Read)(AXHTTPC_SESSION *pSession, unsigned char *buffer, int len, int timeout_ms);
	int (*Write)(AXHTTPC_SESSION *pSession, unsigned char *buffer, int len, int timeout_ms);
	void (*Close)(AXHTTPC_SESSION *pSession);
	int				HdrLen;
	unsigned char	*pXfrBuf;
	int				XfrBufSize;
	int				RcvLen;
	unsigned short		port;
	AXHTTPC_AUTH_TYPE	authType;
	int					statusCode;
	int	RxTimeout;
	int	RxFragmentTimeout;	
	int	TxTimeout;	
};

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int AXHTTPC_Init(AXHTTPC_SESSION *pSession, unsigned char sslEnable, int xferBufSize);
int AXHTTPC_SetTimeout(AXHTTPC_SESSION *pSession, int RxTimeout, int RxFragmentTimeout, int TxTimeout);
int AXHTTPC_SetRequest(AXHTTPC_SESSION *pSession, unsigned char *pMethod, unsigned char *pUrl, unsigned char *pUrlEncodeData);
int AXHTTPC_SetAuthentication(AXHTTPC_SESSION *pSession, unsigned char *pUsername, unsigned char *pPassword, AXHTTPC_AUTH_TYPE authType);
int AXHTTPC_AddHeader(AXHTTPC_SESSION *pSession, unsigned char *pName, unsigned char *pValue);
int AXHTTPC_Connect(AXHTTPC_SESSION *pSession, unsigned char *pHostname, unsigned short port);
int AXHTTPC_Send(AXHTTPC_SESSION *pSession, unsigned char *pPayload, int payloadLength);
int AXHTTPC_Recv(AXHTTPC_SESSION *pSession, unsigned char *pBuffer, int bufferSize);
int AXHTTPC_Close(AXHTTPC_SESSION *pSession);

#endif /* End of __AXHTTPC_H__ */


/* End of axhttpc.h */
