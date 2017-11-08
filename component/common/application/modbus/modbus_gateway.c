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
 * Module Name: modbus_gateway.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "sockets.h"
#include "task.h"
#include "gconfig.h"
#include "modbus_gateway.h"

/* NAMING CONSTANT DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */
MBGW_CONTROL MBGW_Ctrl;

/* LOCAL VARIABLES DECLARATIONS */


/* LOCAL SUBPROGRAM DECLARATIONS */
static void mbgw_Task(void *param);
static MBTCP_CONNECTION* mbgw_ListenConnection(MBTCP_SERVER *pServer);
static int mbgw_UartRx(int Inst, void *pBuf, unsigned short BufSize);
static int mbgw_UartTx(int Inst, void *pData, unsigned short DataLen);
#if MBGW_DEBUG_ENABLE	
static void mbgw_DumpBuf(u8 *pBuf, u16 Len);
#endif

/* LOCAL SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: mbgw_Task()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void mbgw_Task(void *param)
{
	u8 i;
	int ret;
	MBTCP_CONNECTION *pConn;
	MODBUS_NODE *pMaster;
	MODBUS_STATUS modbus_stu;
	u16 ServerPort;
	u8 SlaveId, XferMode;
	u16 ResponseTimeout;
	u16 InterFrameDelay;
	u16 InterCharDelay;

	GCONFIG_GetModbusTcp(&ServerPort, &SlaveId, &XferMode);

	/* Allocate MODBUS serial sub-networks */
	for (i=0; i<MBGW_SERIAL_PORTS; i++)
	{
		MBGW_Ctrl.pMaster[i] = MODBUS_Allocate(MODBUS_MASTER, MBGW_SERIAL_BUF_SIZE, i, mbgw_UartRx, mbgw_UartTx);
		if (MBGW_Ctrl.pMaster[i]==0)
			goto mbgw_Task_Exit;
		
		MODBUS_XferConfig(MBGW_Ctrl.pMaster[i], (XferMode==GCONFIG_MBGW_TCP) ? MODBUS_RTU_MODE:MODBUS_TRANSPARENT_MODE, SlaveId, MODBUS_ENDIAN_SWAP_ENABLE);
		GCONFIG_GetModbusSerialTiming(&ResponseTimeout, &InterFrameDelay, &InterCharDelay);
//		printf("ResponseTimeout=%d, InterFrameDelay=%d, InterCharDelay=%d\r\n", ResponseTimeout, InterFrameDelay, InterCharDelay);
		MODBUS_TimeConfig(MBGW_Ctrl.pMaster[i], ResponseTimeout, InterFrameDelay, InterCharDelay);
	}
	
	/* Allocate MODBUS TCP server */
	MBGW_Ctrl.pServer = MBTCP_OpenServer(ServerPort, MBGW_MAX_TCP_CONNS, MBGW_TCP_BUF_SIZE);
	if (MBGW_Ctrl.pServer == 0)
		goto mbgw_Task_Exit;
	MBTCP_ConfigServer(MBGW_Ctrl.pServer, SlaveId, (XferMode==GCONFIG_MBGW_TCP) ? MBTCP_MODBUS_TCP:MBTCP_TRANSPARENT_TCP, MODBUS_ENDIAN_SWAP_ENABLE);
	  
	while (1)
	{
		vTaskDelay(1);
		
		pConn = mbgw_ListenConnection(MBGW_Ctrl.pServer);
		if (pConn)
			MBGW_DEBUG("New connection established\r\n");
		
		for (i=0; i<MBGW_MAX_TCP_CONNS; i++)
		{
			pConn = &MBGW_Ctrl.pServer->Conns[i];

			if (pConn->State == MBTCP_CONNECTED)
			{
				ret = MBTCP_Recv(pConn, MBGW_Ctrl.Buf[i], MBGW_BUF_SIZE);
				if (ret <= 0)
				{
					if (ret == MBTCP_REMOTE_CONN_CLOSE)
					{
						MBGW_DEBUG("Connection[%d] closed\r\n", i);
					}
					else if (ret < 0)
					{
						MBGW_DEBUG("Receive data error %d\r\n", ret);
					}
					continue;
				}
				MBGW_Ctrl.DataLen[i] = ret;
				pConn->State = MBTCP_INDICATION;
			}
			else if (pConn->State == MBTCP_INDICATION)
			{
				pMaster = MBGW_Ctrl.pMaster[0];			  
//				if (pConn->LastXferInfo.UnitId != pConn->SlaveId)
				{
			  		ret = MODBUS_MasterSend(pMaster, pConn->LastXferInfo.UnitId, MBGW_Ctrl.Buf[i], MBGW_Ctrl.DataLen[i]);
					if (ret <= 0)
						continue;					
#if 0//MBGW_DEBUG_ENABLE						
					MBGW_DEBUG("Serial TX len=%d, conn[%d]\r\n", ret, i);
					mbgw_DumpBuf(pMaster->Buf+3, ret);
#endif						
					MBGW_Ctrl.TxCnt[0] += ret;
					pConn->State = MBTCP_REQUEST;
				}
			}
			else if (pConn->State == MBTCP_REQUEST)
			{
				pMaster = MBGW_Ctrl.pMaster[0];			  
				ret = MODBUS_Recv(pMaster, MBGW_Ctrl.Buf[i], MBGW_BUF_SIZE);
				if (ret > 0)
				{
#if 0//MBGW_DEBUG_ENABLE
					MBGW_DEBUG("Serial RX len=%d\r\n", ret);
					mbgw_DumpBuf(MBGW_Ctrl.Buf[i], ret);
#endif					
					MBGW_Ctrl.RxCnt[0] += ret;	
					MBGW_Ctrl.DataLen[i] = ret;					
					ret = MBTCP_SlaveSend(pConn, MBGW_Ctrl.Buf[i], MBGW_Ctrl.DataLen[i]);
					if (ret < 0)
					{
						MBGW_DEBUG("Fail to send TCP response\r\n");
						continue;
					}
				}
				else
				{
#if MBGW_DEBUG_ENABLE
					MBGW_DEBUG("Serial RX fail=%d\r\n", ret);
#endif							  
				}
				pConn->State = MBTCP_CONNECTED;
			}
		}
	}
mbgw_Task_Exit:
	MBGW_DeInit();
	vTaskDelete(NULL);		
}

/*
 * ----------------------------------------------------------------------------
 * Function: mbgw_ListenConnection()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
MBTCP_CONNECTION* mbgw_ListenConnection(MBTCP_SERVER *pServer)
{
	unsigned char i, ExpiredConnId=-1;
	struct timeval timeout;
	signed short InSocket;
	static struct sockaddr_in InAddr;	
	unsigned short InAddrSize = sizeof(struct sockaddr_in);
	unsigned long CurrTime, ElapseTime, UnusedTime=0;
	fd_set	fdRead;
	MBTCP_CONNECTION *pConn;

	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	FD_ZERO(&fdRead);
	FD_SET(pServer->Socket, &fdRead);
	if ((select((pServer->Socket + 1), &fdRead, NULL, NULL, &timeout) > 0) && (FD_ISSET(pServer->Socket, &fdRead) > 0))
	{
		InSocket = accept(pServer->Socket, (struct sockaddr *)&InAddr, (socklen_t *)&InAddrSize);
		if (InSocket >= 0)
		{
			for (i=0; i<pServer->MaxConns; i++)
			{
				pConn = &pServer->Conns[i];
MBTCP_EstablishNewConn:
				if (pConn->State == MBTCP_CLOSED)
				{		  
					pConn->Socket = InSocket;
					pConn->State = MBTCP_CONNECTED;
					return pConn;
				}
			}
			if (i >= pServer->MaxConns)
			{
				/* Close oldest unused connection, accept new connection */
				CurrTime = xTaskGetTickCount();
				for (i=0; i<pServer->MaxConns; i++)
				{				
					pConn = &pServer->Conns[i];				  
					if (CurrTime >= pConn->LastXferTime)
						ElapseTime = CurrTime - pConn->LastXferTime;
					else
						ElapseTime = portMAX_DELAY - pConn->LastXferTime + CurrTime;
		
					if (ElapseTime > UnusedTime)
					{
						UnusedTime = ElapseTime;
						ExpiredConnId = i;
					}
				}
				if (ExpiredConnId >= 0)
				{
					pConn = &pServer->Conns[ExpiredConnId];	
					MBTCP_Close(pConn);					
					MBGW_DEBUG("Close oldest connection[%d]!\r\n", ExpiredConnId);
					goto MBTCP_EstablishNewConn;
				}
			}			
		}
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: mbgw_UartRx()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int mbgw_UartRx(int Inst, void *pBuf, unsigned short BufSize)
{
	if (GUART_GetRcvrAvailCount() < BufSize)
		return 0;
	
	GUART_CopyUrRxToApp(pBuf, BufSize);
	return BufSize;
}

/*
 * ----------------------------------------------------------------------------
 * Function: mbgw_UartTx()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static int mbgw_UartTx(int Inst, void *pData, unsigned short DataLen)
{
	if (GUART_GetXmitEmptyCount() < DataLen)
		return 0;
	
	GUART_CopyAppToUrTx(pData, DataLen);	
	return DataLen;
}
#if MBGW_DEBUG_ENABLE	
/*
 * ----------------------------------------------------------------------------
 * Function: mbgw_DumpBuf()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void mbgw_DumpBuf(u8 *pBuf, u16 Len)
{
	u16 i;

	for (i=0; i<Len; i++)
	{
		printf("0x%02x%s", (u16)(pBuf[i]), ((i+1) == Len) ? "\r\n":", ");
		if ((i%16)==(16-1))
		{
			printf("\r\n");
		}
	}
	printf("\r\n");	
}
#endif
/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function: MBGW_Init()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
int MBGW_Init(void)
{
	/* MODBUS Serial initialization */
	MODBUS_Init();
	
	/* MODBUS TCP initialization */	
	MBTCP_Init();

	/* MODBUS gateway initialization */
	memset((char*)&MBGW_Ctrl, 0, sizeof(MBGW_Ctrl));	
	if(xTaskCreate(mbgw_Task, "", 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	{
		MBGW_DEBUG("Create task failed\r\n");
		goto MBGW_InitFail;
	}	
	return 0;

MBGW_InitFail:
	MBGW_DeInit();
	return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: MBGW_DeInit()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void MBGW_DeInit(void)
{
	u8 i;
	
	/* DeAllocate MODBUS serial sub-networks */
	for (i=0; i<MBGW_SERIAL_PORTS; i++)
	{
		MODBUS_DeAllocate(MBGW_Ctrl.pMaster[i]);
	}
	
	/* DeAllocate MODBUS TCP server */
	MBTCP_CloseServer(MBGW_Ctrl.pServer);
}

/*
 * ----------------------------------------------------------------------------
 * Function: MBGW_GetRxTxCounter()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void MBGW_GetRxTxCounter(u8 SerialPortId, u32 *rxCounter, u32 *txCounter)
{
	*rxCounter = MBGW_Ctrl.RxCnt[SerialPortId];
	*txCounter = MBGW_Ctrl.TxCnt[SerialPortId];	
}

/*
 * ----------------------------------------------------------------------------
 * Function: MBGW_GetConnStatus()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
int MBGW_GetConnStatus()
{
	u8 i;
	for (i=0; i<MBGW_MAX_TCP_CONNS; i++)
	{
		if (MBGW_Ctrl.pServer->Conns[i].State != MBTCP_CLOSED)
			return 1;
	}
	return 0;
}

/* End of modbus_gateway.c */