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
 * Module Name : modbus_tcp.h
 * Purpose     : 
 * Author      : 
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

/* INCLUDE FILE DECLARATIONS */
#include "modbus.h"

/* NAMING CONSTANT DECLARATIONS */
#define MBTCP_MODBUS_PROTOCOL		0
#define MBTCP_MBAP_HDR_LENGTH		7

/* TYPE DECLARATIONS */
typedef enum
{
	MBTCP_CLOSED = 0,  
	MBTCP_CONNECTED,
	MBTCP_REQUEST,
	MBTCP_CONFIRMATION,
	MBTCP_INDICATION,
	MBTCP_RESPONSE,	
} MBTCP_CONN_STATE;

typedef enum
{
	MBTCP_NEW_CONN = 1,  
	MBTCP_OK = 0,
	MBTCP_ERR = -1,
	MBTCP_NOT_ENOUGH_BUF = -2,	
	MBTCP_SEND_PKT_FAIL = -3,
	MBTCP_RECV_PKT_FAIL = -4,
	MBTCP_PROTOCOL_ID_ERR = -5,	
	MBTCP_TRANSACTION_ID_ERR = -6,		
	MBTCP_LENGTH_FIELD_ERR = -7,			
	MBTCP_INVALID_STATE = -8,	
	MBTCP_CONN_BUF_FULL = -9,
	MBTCP_REMOTE_CONN_CLOSE = -10,	
} MBTCP_STATUS;

typedef enum
{
	MBTCP_MODBUS_TCP = 0,
	MBTCP_TRANSPARENT_TCP,  	
} MBTCP_PROTOCOL_TYPE;

typedef struct _MBTCP_FRAMING
{
	unsigned short XactId;
	unsigned short ProtoId;
	unsigned short Length;	
	unsigned char UnitId;
	unsigned char Data[1];
} MBTCP_FRAMING;

typedef struct _MBTCP_CONNECTION
{
	signed short Socket;  
	MBTCP_CONN_STATE State;
	unsigned char *Buf;
	unsigned short BufSize;
	MODBUS_DEVICE_ROLE DevRole;	
	unsigned char SlaveId;	
	MBTCP_PROTOCOL_TYPE ProtoType;
	MODBUS_ENDIAN_SWAP EndianSwap;
	MBTCP_FRAMING LastXferInfo;
	unsigned long LastXferTime;
} MBTCP_CONNECTION;

typedef struct _MBTCP_SERVER
{
	signed short Socket;
	unsigned short Port;		
	unsigned char MaxConns;
	MBTCP_CONNECTION *Conns;
} MBTCP_SERVER;

/* GLOBAL VARIABLES */


/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int MBTCP_Init(void);
MBTCP_SERVER* MBTCP_OpenServer(unsigned short ServerPort, unsigned char MaxConns, unsigned short BufSize);
int MBTCP_ConfigServer(MBTCP_SERVER *pServer, unsigned char SlaveId, MBTCP_PROTOCOL_TYPE ProtoType, MODBUS_ENDIAN_SWAP EndianSwap);
void MBTCP_CloseServer(MBTCP_SERVER *pServer);
void MBTCP_Close(MBTCP_CONNECTION *pConn);
int MBTCP_MasterSend(MBTCP_CONNECTION *pConn, unsigned short XactId, unsigned char UnitId, unsigned char *pData, unsigned short DataLen);
int MBTCP_SlaveSend(MBTCP_CONNECTION *pConn, unsigned char *pData, unsigned short DataLen);
int MBTCP_Recv(MBTCP_CONNECTION *pConn, unsigned char *pDataBuf, unsigned short BufSize);

#endif /* End of MODBUS_TCP_H */
