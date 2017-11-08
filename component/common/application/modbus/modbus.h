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
 * Module Name : modbus.h
 * Purpose     : 
 * Author      : 
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef MODBUS_H
#define MODBUS_H

/* INCLUDE FILE DECLARATIONS */


/* NAMING CONSTANT DECLARATIONS */
#define MODBUS_RTU_HDR_LENGTH		1
#define MODBUS_ASCII_HDR_LENGTH		5

	//Address definition
#define MODBUS_BROADCAST_ADDR		0

	//Exception code definition
#define MODBUS_ILLEGAL_FUNCTION_CODE	0x01
#define MODBUS_ILLEGAL_DATA_ADDR		0x02
#define MODBUS_ILLEGAL_DATA_VALUE		0x03
#define MODBUS_SERVER_FAILURE			0x04
#define MODBUS_ACKNOWLEDGE				0x05
#define MODBUS_SERVER_BUSY				0x06
#define MODBUS_GATEWAY_PATH_ERR			0x07
#define MODBUS_DEVICE_RESPONSE_TIMEOUT	0x08

/* TYPE DECLARATIONS */
typedef enum
{
	MODBUS_MASTER = 0,
	MODBUS_SLAVE,
} MODBUS_DEVICE_ROLE;

typedef enum
{
	MODBUS_RTU_MODE = 0,
	MODBUS_ASCII_MODE,
	MODBUS_TRANSPARENT_MODE,
} MODBUS_TRANSFER_MODE;

typedef enum
{
	MODBUS_IDLE = 0,
	MODBUS_REQUEST,
} MODBUS_STATE;

typedef enum
{
	MODBUS_ENDIAN_SWAP_DISABLE = 0,
	MODBUS_ENDIAN_SWAP_ENABLE,
} MODBUS_ENDIAN_SWAP;

typedef enum
{
	MODBUS_OK = 0,
	MODBUS_ERR = -1,
	MODBUS_NOT_ENOUGH_BUF = -2,	
	MODBUS_ILLEGAL_RSP_ADDR = -3,
	MODBUS_ILLEGAL_FUNC_CODE = -4,	
	MODBUS_DEV_RSP_TIMEOUT = -5,
	MODBUS_CHECK_SUM_ERR = -6,	
	MODBUS_BUSY = -7,		
} MODBUS_STATUS;

typedef struct _MODBUS_NODE
{
	MODBUS_STATE State;	
	unsigned char *Buf;
	MODBUS_DEVICE_ROLE	DevRole;
	unsigned short BufSize;	
	int (*RxFunc)(int InstId, void *pBuf, unsigned short BufSize);		
	int (*TxFunc)(int InstId, void *pData, unsigned short DataLen);
	unsigned short ResponseTimeOut;
	unsigned short InterFrameDelay;
	unsigned short InterCharDelay;	
	unsigned char SlaveId;
	MODBUS_TRANSFER_MODE XferMode;
	unsigned char EndianSwap;
	int InstId;
	unsigned long LastRequestTime;	
} MODBUS_NODE;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
int MODBUS_Init(void);
unsigned char MODBUS_LRC(unsigned char *pData, unsigned short DataLen);
unsigned short MODBUS_CRC(unsigned char *pData, unsigned short DataLen);
MODBUS_NODE* MODBUS_Allocate(MODBUS_DEVICE_ROLE DevRole
							 , unsigned short BufSize
							 , int InstId
							 , int (*RxFunc)(int InstId, void *pBuf, unsigned short BufSize)
							 , int (*TxFunc)(int InstId, void *pData, unsigned short DataLen));
void MODBUS_DeAllocate(MODBUS_NODE *pNode);
int MODBUS_XferConfig(MODBUS_NODE *pNode, MODBUS_TRANSFER_MODE XferMode, unsigned char SlaveId, unsigned char EndianSwap);
int MODBUS_TimeConfig(MODBUS_NODE *pNode, unsigned short ResponseTimeOut, unsigned short InterFrameDelay, unsigned short InterCharDelay);
int MODBUS_MasterSend(MODBUS_NODE *pNode, unsigned char SlaveId, unsigned char *pData, unsigned short DataLen);
int MODBUS_SlaveSend(MODBUS_NODE *pNode, unsigned char *pData, unsigned short DataLen);
int MODBUS_Recv(MODBUS_NODE *pNode, unsigned char *pBuf, unsigned short BufSize);
#endif /* End of MODBUS_SERIAL_H */
