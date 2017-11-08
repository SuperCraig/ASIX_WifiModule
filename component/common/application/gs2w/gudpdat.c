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
 * Module Name: gudpdat.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "device.h"
#include "serial_api.h"
#include "sockets.h"
#include "api.h"
#include "main.h"
#include "gconfig.h"
#include "gudpdat.h"
#include "guart.h"
#include "gs2w.h"
#include "platform_opts.h"
#include "PM25ModuleFunction.h"         // Craig

#if (GUDPDAT_INCLUDE)
/* NAMING CONSTANT DECLARATIONS */


/* GLOBAL VARIABLES DECLARATIONS */
GUDPDAT_CONN_T	gudpdat_Conns[GUDPDAT_MAX_CONNS];

/* LOCAL VARIABLES DECLARATIONS */
static u8		gudpdat_UdpClient;
static u8		*pUdpRcvrBuf;
static struct sockaddr_in	gudpdat_SourAddr;
static struct sockaddr_in	gudpdat_DestAddr;
TickType_t		gudpdat_TickTime;
static u16		gudpdat_TxTimeOut;
static u8		gudpdat_ForVirtCom;
static u8		gudpdat_VirtBuf[6] = {0};
static u8		gudpdat_ConnectionMode = GCONFIG_AUTO_CONNECTION;

/* LOCAL SUBPROGRAM DECLARATIONS */                   // Craig 20170822
static void gudpdat_FreeBuf(void);
static void gudpdat_TransferHandleTask(void *param);
static void gudpdat_GetUrData(GUDPDAT_CONN_T *udpConn);
static void gudpdat_UdpXmitPacket(GUDPDAT_CONN_T *udpConn);
static void gudpdat_UdpRecvPacketToUr(GUDPDAT_CONN_T *udpConn);
static void gudpdat_HandleFlowControlPacket(u8 *pData, u16 length, GUDPDAT_CONN_T *udpConn);
static s16 gudpdat_SendModemStatusChange(GUDPDAT_CONN_T *udpConn, u8 mStatus, u8 mCtrl);


/* LOCAL SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: gudpdat_FreeBuf()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_FreeBuf(void)
{
  u16 i;
  
  if (pUdpRcvrBuf)
  {
    vPortFree(pUdpRcvrBuf);
    pUdpRcvrBuf = NULL;
  }
  
  for (i = 0; i < GUDPDAT_MAX_CONNS; i++)
  {
    if (gudpdat_Conns[i].TxBuf)
    {
      vPortFree(gudpdat_Conns[i].TxBuf);
      gudpdat_Conns[i].TxBuf = NULL;
    }
    
    if (gudpdat_Conns[i].AccessProtection)
    {
      vSemaphoreDelete(gudpdat_Conns[i].AccessProtection);
    }
  }
}

/*
 * ----------------------------------------------------------------------------
 * Function: gudpdat_TransferHandleTask()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_TransferHandleTask(void *param)
{
	u32		ipAddr;
	u8		idConn, mStatus, mCtrl, mStatusTmp;
	s16		iStatus;
	u32		XonElapseTime, XonTickTime;
	s16		ret;
	
	while (1)
	{
		vTaskDelay(1);
		if (gudpdat_UdpClient) // For UDP Client
		{
			for (idConn = 0; idConn < GUDPDAT_MAX_CONNS; idConn ++)
			{
#if CONFIG_HSUART_COMMAND_MODE
				if (gudpdat_Conns[idConn].State != GUDPDAT_STATE_CONNECTED)
				{
					gudpdat_GetUrData(&gudpdat_Conns[idConn]);
				}
#endif
				if (gudpdat_Conns[idConn].State == GUDPDAT_STATE_FREE)
				{

					if (gudpdat_ConnectionMode == GCONFIG_AUTO_CONNECTION)
					{
						ret = GUDPDAT_Connect(idConn, GS2W_CheckDestIP(), GCONFIG_GetClientDestPort());
						if (ret < 0)
						{
							printf("\n\rR2W: Connect UDP server fail.");
							continue;
						}
					}

				}
				else if (gudpdat_Conns[idConn].State == GUDPDAT_STATE_CONNECTED)
				{
					if (gudpdat_Conns[idConn].Socket >= 0)
					{
						// Getting packets and moving into UART transmitting buffer
						gudpdat_UdpRecvPacketToUr(&gudpdat_Conns[idConn]);

						if (gudpdat_ForVirtCom) // For Virtual COM Flow Control
						{
							/* Xoff timeout check */
							if (gudpdat_Conns[idConn].FlowControlXonRx==FALSE)
							{
								XonTickTime = xTaskGetTickCount();
								if (XonTickTime >= gudpdat_Conns[idConn].FlowControlXonLastTime)
									XonElapseTime = XonTickTime - gudpdat_Conns[idConn].FlowControlXonLastTime;
								else
									XonElapseTime = portMAX_DELAY - gudpdat_Conns[idConn].FlowControlXonLastTime + XonTickTime;
			
								if ((XonElapseTime * portTICK_RATE_MS) >= GUDPDAT_XOFF_TIMEOUT)
								{
									gudpdat_Conns[idConn].FlowControlXonRx = TRUE;
									printf("R2W: Xoff timeout, set Xon\r\n");
								}
							}

							/* Send modem status change packet and handle the virtual DSR/DTR for better compatibility */
							mStatus = serial_raed_msr(&urObj) | GSMSR_DSR;
							if (GUART_GetXmitEmptyCount() < (GUART_TX_BUF_SIZE/2))
								mStatus &= ~GSMSR_CTS;
							else if (GUART_GetXmitEmptyCount() > (GUART_TX_BUF_SIZE/2))
								mStatus |= GSMSR_CTS;
							mCtrl = (serial_raed_mcr(&urObj) & RUART_MCR_RTS) | (gudpdat_Conns[idConn].FlowControlVirModemStatus & GSMCR_DTR);
							if (gudpdat_Conns[idConn].FlowControlModemStatus != mStatus || gudpdat_Conns[idConn].FlowControlModemCtrl != mCtrl)
							{
								mStatusTmp = mStatus;
								
								if ((gudpdat_Conns[idConn].FlowControlModemStatus & GSMSR_CTS) != (mStatus & GSMSR_CTS)) // Check CTS
									mStatusTmp |= GSMSR_DCTS; // Set DCTS
								if ((gudpdat_Conns[idConn].FlowControlModemStatus & GSMSR_DSR) != (mStatus & GSMSR_DSR)) // Check DSR
									mStatusTmp |= GSMSR_DDSR; // Set DDSR
								
								gudpdat_Conns[idConn].FlowControlModemStatus = mStatus;
								gudpdat_Conns[idConn].FlowControlModemCtrl = mCtrl;
							
								if (gudpdat_SendModemStatusChange(&gudpdat_Conns[idConn], mStatusTmp, mCtrl) < 0)
									break;
							}

							if (gudpdat_Conns[idConn].TxBufIndex == 0)
							{
								gudpdat_Conns[idConn].TxBuf[0] = GS2W_DATA_INDICATOR; // transfer type of data packet indicator
								gudpdat_Conns[idConn].TxBufIndex += 3; // 1byte indicator + 2byte data length
							}
							if (gudpdat_Conns[idConn].FlowControlXonRx == FALSE)
							{
					  			GUART_CopyUrRxToApp(0, 0);/* UART RX trigger */
								break;
							}
						}

						// Sending packets from UART receiving buffer
						gudpdat_GetUrData(&gudpdat_Conns[idConn]);
						gudpdat_UdpXmitPacket(&gudpdat_Conns[idConn]);
					}
				}
				else if (gudpdat_Conns[idConn].State == GUDPDAT_STATE_ACTIVE)
				{
					if (gudpdat_Conns[idConn].Socket >= 0)
					{
						gudpdat_UdpRecvPacketToUr(&gudpdat_Conns[idConn]);

						if (gudpdat_ForVirtCom) // For Virtual COM Flow Control
						{
							// Send connected message
							gudpdat_VirtBuf[0] = GS2W_CONNECT_INDICATOR;
							gudpdat_VirtBuf[1] = 0;
							gudpdat_VirtBuf[2] = 0;
							iStatus = sendto(gudpdat_Conns[idConn].Socket, &gudpdat_VirtBuf[0], 3, 0,
											&gudpdat_DestAddr, sizeof(gudpdat_DestAddr));
							if (iStatus <= 0)
							{
								printf("\n\rR2W: udp send control error!");
								GUDPDAT_Close(idConn);
								break;
							}
						}
						else
						{
							gudpdat_Conns[idConn].State = GUDPDAT_STATE_CONNECTED;
							GS2W_SetTaskState(GS2W_STATE_UDP_DATA_PROCESS);
						}
						vTaskDelay(1000);
					}
				}
				else
					vTaskDelay(1);
			}
		}
		else // For UDP Server
		{
			if ((gudpdat_Conns[0].State == GUDPDAT_STATE_FREE || gudpdat_Conns[0].State == GUDPDAT_STATE_CONNECTED) && (gudpdat_Conns[0].Socket >= 0))
			{
				// Getting packets and moving into UART transmitting buffer
				gudpdat_UdpRecvPacketToUr(&gudpdat_Conns[0]);

 				if (gudpdat_Conns[0].State == GUDPDAT_STATE_CONNECTED)
				{
					if (gudpdat_ForVirtCom) // For Virtual COM Flow Control
					{
						/* Xoff timeout check */
						if (gudpdat_Conns[0].FlowControlXonRx==FALSE)
						{
							XonTickTime = xTaskGetTickCount();
							if (XonTickTime >= gudpdat_Conns[0].FlowControlXonLastTime)
								XonElapseTime = XonTickTime - gudpdat_Conns[0].FlowControlXonLastTime;
							else
								XonElapseTime = portMAX_DELAY - gudpdat_Conns[0].FlowControlXonLastTime + XonTickTime;
			
							if ((XonElapseTime * portTICK_RATE_MS) >= GUDPDAT_XOFF_TIMEOUT)
							{
								gudpdat_Conns[0].FlowControlXonRx = TRUE;
								printf("R2W: Xoff timeout, set Xon\r\n");
							}
						}

						/* Send modem status change packet and handle the virtual DSR/DTR for better compatibility */
						mStatus = serial_raed_msr(&urObj) | GSMSR_DSR;
						if (GUART_GetXmitEmptyCount() < (GUART_TX_BUF_SIZE/2))
							mStatus &= ~GSMSR_CTS;
						else if (GUART_GetXmitEmptyCount() > (GUART_TX_BUF_SIZE/2))
							mStatus |= GSMSR_CTS;
						mCtrl = (serial_raed_mcr(&urObj) & GSMCR_RTS) | (gudpdat_Conns[0].FlowControlVirModemStatus & GSMCR_DTR);
						if (gudpdat_Conns[0].FlowControlModemStatus != mStatus || gudpdat_Conns[0].FlowControlModemCtrl != mCtrl)
						{
							mStatusTmp = mStatus;
							
							if ((gudpdat_Conns[0].FlowControlModemStatus & GSMSR_CTS) != (mStatus & GSMSR_CTS)) // Check CTS
								mStatusTmp |= GSMSR_DCTS; // Set DCTS
							if ((gudpdat_Conns[0].FlowControlModemStatus & GSMSR_DSR) != (mStatus & GSMSR_DSR)) // Check DSR
								mStatusTmp |= GSMSR_DDSR; // Set DDSR

							gudpdat_Conns[0].FlowControlModemStatus = mStatus;
							gudpdat_Conns[0].FlowControlModemCtrl = mCtrl;
							if (gudpdat_SendModemStatusChange(&gudpdat_Conns[0], mStatusTmp, mCtrl) < 0)
								break;
						}

						if (gudpdat_Conns[0].TxBufIndex == 0)
						{
							gudpdat_Conns[0].TxBuf[0] = GS2W_DATA_INDICATOR; // transfer type of data packet indicator
							gudpdat_Conns[0].TxBufIndex += 3; // 1byte indicator + 2byte data length
						}
						if (gudpdat_Conns[0].FlowControlXonRx == FALSE)
						{
				  			GUART_CopyUrRxToApp(0, 0);/* UART RX trigger */
							break;
						}
					}
					// Sending packets from UART receiving buffer
					gudpdat_GetUrData(&gudpdat_Conns[0]);
					gudpdat_UdpXmitPacket(&gudpdat_Conns[0]);
				}
#if CONFIG_HSUART_COMMAND_MODE			  
				else
				{
					gudpdat_GetUrData(&gudpdat_Conns[0]);
				}
#endif				
                                else{
                                        gudpdat_GetUrData(&gudpdat_Conns[0]);
                                }
			}
                        if(GUART_GetRcvrAvailCount()>0){        // Craig         
                            gudpdat_GetUrData(&gudpdat_Conns[0]);
                        } 
		}
	}
transferHandleTask_Exit:
	gudpdat_FreeBuf();
	vTaskDelete(NULL);
}


/*
 * ----------------------------------------------------------------------------
 * Function: static s16 gudpdat_SendModemStatusChange()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static s16 gudpdat_SendModemStatusChange(GUDPDAT_CONN_T *udpConn, u8 mStatus, u8 mCtrl)
{
	u8 tmp[10];
	
	tmp[0] = GS2W_FLOW_CONTROL_INDICATOR;
	tmp[1] = 0;
	tmp[2] = 3;
	tmp[3] = MODEM_STATUS_CHANGE;
	tmp[4] = mStatus;
	tmp[5] = mCtrl;

	if (sendto(udpConn->Socket, tmp, 6, 0, &gudpdat_DestAddr, sizeof(gudpdat_DestAddr)) <= 0)
	{
		printf("\n\rR2W: udp send control error!");
		GUDPDAT_Close(udpConn->Id);
		return -1;
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gudpdat_GetUrData(GUDPDAT_CONN_T *udpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_GetUrData(GUDPDAT_CONN_T *udpConn)
{
	u32		urDataLen;
	u16		gudpdat_BufIndex;
	u16 i;
        
#if CONFIG_HSUART_COMMAND_MODE	
	if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))
	{
		return;
	}
#endif

	// Check the UART received buffer
	urDataLen = GUART_GetRcvrAvailCount();
	if (urDataLen > 0)
	{
		gudpdat_BufIndex = udpConn->TxBufIndex;
		if (urDataLen > (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex))
		{
			urDataLen = (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex);
		}
		if(gudpdat_BufIndex > (GUDPDAT_TX_BUF_SIZE-1)){          // Craig
                  udpConn->TxBufIndex=0;
                  gudpdat_BufIndex=0;
                }
                                        
		GUART_CopyUrRxToApp(&(udpConn->TxBuf[gudpdat_BufIndex]), urDataLen);
                
                for(i=gudpdat_BufIndex ; i<urDataLen+gudpdat_BufIndex ; i++){    // Craig
                  //printf("%x ",*(udpConn->TxBuf+i));     // Craig
                  
                  if(*(udpConn->TxBuf+i) == 0xaa && *(udpConn->TxBuf+i+6) == 0xff){
                    /*PM 25 Module initialize struct value*/
                    pm1.StartBit = *(udpConn->TxBuf+i+0);
                    pm1.Vout_H = *(udpConn->TxBuf+i+1);
                    pm1.Vout_L = *(udpConn->TxBuf+i+2);
                    pm1.Vref_H = *(udpConn->TxBuf+i+3);
                    pm1.Vref_L = *(udpConn->TxBuf+i+4);
                    pm1.Chksum = *(udpConn->TxBuf+i+5);
                    pm1.StopBit = *(udpConn->TxBuf+i+6);
                    printf("Density Value: %d",GetDensityValue(&pm1));
                  }
                }


                /*      Buffer content to UART interface
                
                GUART_CopyAppToUrTx((udpConn->TxBuf+gudpdat_BufIndex), urDataLen);         // Craig
                udpConn->UrTxBytes += urDataLen;              // Craig
                if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig            
                  udpConn->UrTxBytes = 0;               // Craig
                
                */       
                                        
#if CONFIG_HSUART_COMMAND_MODE
		if (GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, &(udpConn->TxBuf[gudpdat_BufIndex]), urDataLen))
		{
			return;
		}
#endif
		gudpdat_BufIndex += urDataLen;
		udpConn->UrRxBytes += urDataLen;
		udpConn->TxBufIndex = gudpdat_BufIndex;
	}
	else
	{
		GUART_CopyUrRxToApp(0, 0);/* UART RX trigger */	
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gudpdat_UdpXmitPacket(GUDPDAT_CONN_T *udpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_UdpXmitPacket(GUDPDAT_CONN_T *udpConn)
{
	u32		txTime;
	u16		txSendLen;
	s16		iStatus;

	if (udpConn->Socket < 0)
		return;

	txSendLen = 0;
	if (udpConn->TxBufIndex >= GUDPDAT_TX_BUF_SIZE)
	{
		txSendLen = GUDPDAT_TX_BUF_SIZE;
	}
	else
	{
		gudpdat_TickTime = xTaskGetTickCount();
		if (udpConn->TxBufIndex == 0 || udpConn->TxWaitTime == 0)
		{
			udpConn->TxWaitTime = gudpdat_TickTime;
		}
		else
		{
			if (gudpdat_TickTime >= udpConn->TxWaitTime)
				txTime = gudpdat_TickTime - udpConn->TxWaitTime;
			else
				txTime = portMAX_DELAY - udpConn->TxWaitTime + gudpdat_TickTime;

			if ((txTime * portTICK_RATE_MS) >= gudpdat_TxTimeOut)
			{
				txSendLen = udpConn->TxBufIndex;
			}
		}
	}
	
	if (txSendLen > (gudpdat_ForVirtCom * 3))
	{
		if (gudpdat_ForVirtCom) // For Virtual COM Flow Control
		{
			udpConn->TxBuf[1] = (u8)((txSendLen - 3) >> 8);
			udpConn->TxBuf[2] = (u8)(txSendLen - 3);
		}
		else
		{
			if (txSendLen == 0)
				return;
		}
		iStatus = sendto(udpConn->Socket, udpConn->TxBuf, txSendLen, 0,
						(struct sockaddr*)&gudpdat_DestAddr, sizeof(struct sockaddr_in));
		if (iStatus <= 0)
		{
			printf("\n\rR2W: UDP sendto() error!  iStatus:%d", iStatus);
			GUDPDAT_Close(udpConn->Id);
		}
		else
		{
			// UDP transmitting successfully.
			udpConn->TxBufIndex = 0; // Updated the TX buffer index
			udpConn->TxWaitTime = 0;
		}
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gudpdat_UdpRecvPacketToUr(GUDPDAT_CONN_T *udpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_UdpRecvPacketToUr(GUDPDAT_CONN_T *udpConn)
{
	u16		urDataLen, dataLen;
	s16		ret, recvLen = 0;
	u16		iAddrSize = sizeof(struct sockaddr_in);
	fd_set	fdRead;
	struct timeval	timeout;
	u8		*pbuf;
	
	urDataLen = GUART_GetXmitEmptyCount();
	if (urDataLen)
	{
		if (urDataLen > GUDPDAT_RX_BUF_SIZE)
		{
			urDataLen = GUDPDAT_RX_BUF_SIZE;
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = (GUDPDAT_TIMEOUT * 1000);
		FD_ZERO(&fdRead);
		FD_SET(udpConn->Socket, &fdRead);
		ret = select((udpConn->Socket + 1), &fdRead, NULL, NULL, &timeout);
		if ((ret > 0) && (FD_ISSET(udpConn->Socket, &fdRead) > 0))
		{
			if (urDataLen > GUDPDAT_RX_BUF_SIZE)
			{
				urDataLen = GUDPDAT_RX_BUF_SIZE;
			}

			recvLen = recvfrom(udpConn->Socket, pUdpRcvrBuf, urDataLen, 0,
							   (struct sockaddr *)&gudpdat_DestAddr, (socklen_t *)&iAddrSize);
                        
			if (recvLen <= 0)
			{
				// receiving data error
				printf("\n\rR2W: recvfrom error recvLen:%d ", recvLen);
				GUDPDAT_Close(udpConn->Id);
			}
			else if (recvLen > 0)
			{
				printf("\n\rR2W: recvfrom:%d", recvLen);//Ian test open
				// receiving data successfully and moving data to UART Tx buffer
#if CONFIG_HSUART_COMMAND_MODE	
				if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))//Discard received packets in command mode
				{
					return;
				}
#endif			  
				udpConn->RemoteIp = gudpdat_DestAddr.sin_addr.s_addr;
				udpConn->RemotePort = gudpdat_DestAddr.sin_port;
				
				if (gudpdat_ForVirtCom) // For Virtual COM Flow Control
				{
					pbuf = pUdpRcvrBuf;
gudpdat_MoreData:
					dataLen = ((u16)pbuf[1] << 8) + (u16)pbuf[2] + 3;
					if (dataLen > recvLen)
					{
						dataLen = recvLen;
					}
					
					if (pbuf[0] == GS2W_DATA_INDICATOR)
					{
						GUART_CopyAppToUrTx((pbuf + 3), (dataLen - 3));
						udpConn->UrTxBytes += (dataLen - 3);
					}
					else if (pbuf[0] == GS2W_FLOW_CONTROL_INDICATOR)
					{
						gudpdat_HandleFlowControlPacket(pbuf, dataLen, udpConn);
					}
					else if (pbuf[0] == GS2W_CONNECT_INDICATOR)
					{
						udpConn->State = GUDPDAT_STATE_CONNECTED;
						udpConn->TxBufIndex = 0;
						udpConn->TxWaitTime = 0;
						GS2W_SetTaskState(GS2W_STATE_UDP_DATA_PROCESS);

						if (gudpdat_UdpClient == FALSE)
						{
//							udpConn->TxBuf[0] = GS2W_CONNECT_INDICATOR;
//							udpConn->TxBufIndex += 3;
//							gudpdat_UdpXmitPacket(udpConn);
							gudpdat_VirtBuf[0] = GS2W_CONNECT_INDICATOR;
							gudpdat_VirtBuf[1] = 0;
							gudpdat_VirtBuf[2] = 0;
							ret = sendto(udpConn->Socket, &gudpdat_VirtBuf[0], 3, 0,
												&gudpdat_DestAddr, sizeof(gudpdat_DestAddr));
							if (ret <= 0)
							{
 								udpConn->State = GUDPDAT_STATE_FREE;
								GS2W_SetTaskState(GS2W_STATE_IDLE);
								udpConn->FlowControlXonRx = TRUE;
								return;
							}
						}
					}
					else if (pbuf[0] == GS2W_CLOSE_INDICATOR)
					{
						GUDPDAT_Close(udpConn->Id);
					}
					
                	recvLen -= dataLen;
                	if (recvLen > 0)
                	{
						/* point to the next packet header */
						pbuf += dataLen;
						goto gudpdat_MoreData;
                	}					
				}
				else
				{
					if (udpConn->State == GUDPDAT_STATE_FREE)
					{
						udpConn->State = GUDPDAT_STATE_CONNECTED;
						GS2W_SetTaskState(GS2W_STATE_UDP_DATA_PROCESS);
					}
					GUART_CopyAppToUrTx(pUdpRcvrBuf, recvLen);
					udpConn->UrTxBytes += recvLen;
				}
			}
		}
	}
	GUART_TxIntrIdleCheck();
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gudpdat_HandleFlowControlPacket(u8 *pData, u16 length, GUDPDAT_CONN_T *udpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gudpdat_HandleFlowControlPacket(u8 *pData, u16 length, GUDPDAT_CONN_T *udpConn)
{
	u32	bRate;
	GS2W_IOCTL_PKT *pPkt = (GS2W_IOCTL_PKT*)pData;
	int iStatus = 1;
	
	switch (pPkt->IoCtrlType)
	{
		case IOCTL_SERIAL_GET_DTRRTS:
			gudpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gudpdat_VirtBuf[1] = 0;
			gudpdat_VirtBuf[2] = 2;
			gudpdat_VirtBuf[3] = IOCTL_SERIAL_GET_DTRRTS_REPLY;
			gudpdat_VirtBuf[4] = udpConn->FlowControlModemCtrl;/* Handle virtual DTR for better compatibility */
			iStatus = sendto(udpConn->Socket, &gudpdat_VirtBuf[0], 5, 0, (struct sockaddr*)&gudpdat_DestAddr, sizeof(struct sockaddr_in));
			break;
		case IOCTL_SERIAL_GET_MODEM_CONTROL:
			gudpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gudpdat_VirtBuf[1] = 0;
			gudpdat_VirtBuf[2] = 2;
			gudpdat_VirtBuf[3] = IOCTL_SERIAL_GET_MODEM_CONTROL_REPLY;
			gudpdat_VirtBuf[4] = udpConn->FlowControlModemCtrl;/* Handle virtual DTR for better compatibility */
			iStatus = sendto(udpConn->Socket, &gudpdat_VirtBuf[0], 5, 0, (struct sockaddr*)&gudpdat_DestAddr, sizeof(struct sockaddr_in));
			break;
		case IOCTL_SERIAL_GET_MODEM_STATUS:
			gudpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gudpdat_VirtBuf[1] = 0;
			gudpdat_VirtBuf[2] = 2;
			gudpdat_VirtBuf[3] = IOCTL_SERIAL_GET_MODEM_STATUS_REPLY;
			gudpdat_VirtBuf[4] = udpConn->FlowControlModemStatus;/* Handle virtual DSR for better compatibility */
			iStatus = sendto(udpConn->Socket, &gudpdat_VirtBuf[0], 5, 0, (struct sockaddr*)&gudpdat_DestAddr, sizeof(struct sockaddr_in));
			break;
		case IOCTL_SERIAL_SET_BAUD_RATE:
			switch (pPkt->Data.SetBaudRate.BaudrateIndex)
			{
				case 0:
					bRate = 921600;
					break;
				case 1:
					bRate = 115200;
					break;
				case 2:
					bRate = 57600;
					break;
				case 3:
					bRate = 38400;
					break;
				case 4:
					bRate = 19200;
					break;
				case 5:
					bRate = 9600;
					break;
				case 6:
					bRate = 4800;
					break;
				case 7:
					bRate = 2400;
					break;
				case 8:
					bRate = 1200;
					break;
				default:
					printf("R2W: Unsupported baudrate\r\n");
					return;					
			}
			/* Set parity bits */
			switch (pPkt->Data.SetBaudRate.ParityBits)
			{
				case SERIAL_PARITY_NONE:
					GCONFIG_SetUrParity(ParityNone);
					break;
				case SERIAL_PARITY_ODD:	
					GCONFIG_SetUrParity(ParityOdd);
					break;
				case SERIAL_PARITY_EVEN:
					GCONFIG_SetUrParity(ParityEven);			  
					break;
				default:
					printf("R2W: Unsupported parity type\r\n");
					return;
			}

			/* Set data bits */
			GCONFIG_SetUrDataBits(pPkt->Data.SetBaudRate.DataBits+5);/* Converts to 5~8 bits */
			/* Set stop bits */
			GCONFIG_SetUrStopBits(pPkt->Data.SetBaudRate.StopBits+1);/* Converts to 1~2 bits */
			/* Set baudrate */
			GCONFIG_SetUrBaudRate(bRate);
			serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());			
			GUART_SetBaudrate(&urObj, GCONFIG_GetUrBaudRate());

			printf("R2W: Set Baudrate=%d, DataBits=%d, StopBits=%d, ParityBits=%s\r\n"
					, GCONFIG_GetUrBaudRate()
					, GCONFIG_GetUrDataBits()
					, GCONFIG_GetUrStopBits()
					, (GCONFIG_GetUrParity()==ParityNone) ? "None":((GCONFIG_GetUrParity()==ParityOdd)?"Odd":"Even"));
			break;
		case IOCTL_SERIAL_SET_HARDFLOW:
			/* Set flow control */
			switch (pPkt->Data.SetHardFlow.CtrlValue)
			{
				case FLOW_CTRL_NONE:
					GCONFIG_SetUrFlowCtrl(0);
					break;
				case FLOW_CTRL_HARDWARE:
					GCONFIG_SetUrFlowCtrl(1);			  
					break;
				default:
					printf("R2W: Unsupported flow control type\n\r");
					return;
			}
			printf("R2W: Set flow control to %s\n\r", (pPkt->Data.SetHardFlow.CtrlValue==FLOW_CTRL_NONE) ? "None":"Hardware");
           	GUART_SetFlowControl(&urObj, GCONFIG_GetUrFlowCtrl());
			break;
		case IOCTL_SERIAL_SET_XOFF:
			printf("R2W: Xoff\r\n");
			udpConn->FlowControlXonRx = FALSE;
			udpConn->FlowControlXonLastTime = xTaskGetTickCount();				
			break;
		case IOCTL_SERIAL_SET_XON:
			printf("R2W: Xon\r\n");
			udpConn->FlowControlXonRx = TRUE;
			break;
		case IOCTL_SERIAL_SET_RTS:
			printf("R2W: Set RTS\r\n");
			GUART_ControlRTSSignal(&urObj, 1);
			break;
		case IOCTL_SERIAL_CLR_RTS:
			printf("R2W: Clear RTS\r\n");
			GUART_ControlRTSSignal(&urObj, 0);
			break;				
		case IOCTL_SERIAL_CLR_DTR:
			printf("R2W: Clear DTR\r\n");
			udpConn->FlowControlVirModemStatus &= ~GSMCR_DTR;/* Handle virtual DTR for better compatibility */
			break;
		case IOCTL_SERIAL_SET_DTR:
			printf("R2W: Set DTR\r\n");
			udpConn->FlowControlVirModemStatus |= GSMCR_DTR;/* Handle virtual DTR for better compatibility */
			break;			
		default:
			printf("R2W: Ignore cmd %x\n\r", pPkt->IoCtrlType);
			break;
	}
	if (iStatus <= 0)
	{
		GUDPDAT_Close(udpConn->Id);
	}
}


/* EXPORTED SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: GUDPDAT_ServerInit()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8_t GUDPDAT_ServerInit(void)
{
	u16		iAddrSize;
	GUDPDAT_CONN_T	*udpConn = &gudpdat_Conns[0];

	// filling the UDP socket address
	iAddrSize = sizeof(struct sockaddr_in);
	gudpdat_SourAddr.sin_family = AF_INET;
	gudpdat_SourAddr.sin_len = iAddrSize;
	gudpdat_SourAddr.sin_port = htons(GCONFIG_GetDeviceDataPktListenPort());
	gudpdat_SourAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// binding the UDP socket to the UDP server address
	udpConn->Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpConn->Socket < 0)
	{
		printf("\n\rR2W: create udp server socket fd error!");
		goto ServerError;
	}

	if (bind(udpConn->Socket, (struct sockaddr *)&gudpdat_SourAddr, iAddrSize) < 0)
	{
		printf("\n\rR2W: bind udp server socket fd error!");
		close(udpConn->Socket);
		goto ServerError;
	}

	printf("\n\rR2W: UDP server socket %d successfully", udpConn->Socket);

	/* Creat the R2W UDP receiving and transmitting function */
	if(xTaskCreate(gudpdat_TransferHandleTask, "gudp_ServerHandle", GUDPDAT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) == pdPASS)
	{
		goto ServerExit;
	}
	else
	{
		close(udpConn->Socket);
		printf("\n\rR2W: Create gudpdat_TransferHandleTask() task failed.");
	}

ServerError:
	gudpdat_FreeBuf();

ServerExit:
	vTaskDelete(NULL);
	
	return TRUE;
}

/*
 * ----------------------------------------------------------------------------
 * Function: GUDPDAT_Init()
 * Purpose : Initialization
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
//void GUDPDAT_Init()
void GUDPDAT_Init(u16 Baudrate)         // Craig 20170823
{
	u8	i;
        u8 ret;

	pUdpRcvrBuf = pvPortMalloc(GUDPDAT_RX_BUF_SIZE);
	memset(pUdpRcvrBuf, 0, GUDPDAT_RX_BUF_SIZE);
	memset(&gudpdat_SourAddr, 0, sizeof(struct sockaddr_in));
	memset(&gudpdat_DestAddr, 0, sizeof(struct sockaddr_in));
	gudpdat_TxTimeOut = GCONFIG_GetEthernetTxTimer();
	if (GCONFIG_GetUrMode() == R2WMODE_VCom)
	{
		gudpdat_ForVirtCom = 1;
	}
	else
	{
		gudpdat_ForVirtCom = 0;
	}
	
	gudpdat_ConnectionMode = GCONFIG_GetClientConnectionMode();
        
	for (i = 0; i < GUDPDAT_MAX_CONNS; i++)
        {
          gudpdat_Conns[i].Id = i;
          gudpdat_Conns[i].State = GUDPDAT_STATE_FREE;
          gudpdat_Conns[i].RemoteIp = 0;
          gudpdat_Conns[i].RemotePort = 0;
          gudpdat_Conns[i].Socket = -1;
          gudpdat_Conns[i].TxWaitTime = 0;
          gudpdat_Conns[i].UrRxBytes = 0;
          gudpdat_Conns[i].UrTxBytes = 0;
          gudpdat_Conns[i].TxBufIndex = 0;
          gudpdat_Conns[i].FlowControlXonRx = TRUE;
          gudpdat_Conns[i].FlowControlModemStatus = 0;
          gudpdat_Conns[i].FlowControlModemCtrl = 0;
          gudpdat_Conns[i].FlowControlVirModemStatus = 0;		
          gudpdat_Conns[i].AccessProtection = xSemaphoreCreateMutex();		
          gudpdat_Conns[i].TxBuf = pvPortMalloc(GUDPDAT_TX_BUF_SIZE);
        }
        //GUDPDAT_ServerInit();           // Craig
        //xTaskCreate(gudpdat_TransferHandleTask, "gudp_ServerHandle", GUDPDAT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);     // Craig
        
        serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
        GUART_SetBaudrate(&urObj, Baudrate);       // Craig
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GUDPDAT_GetUrTxBytes(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GUDPDAT_GetUrTxBytes(u8 connId)
{
	if (connId < GUDPDAT_MAX_CONNS)
        return (gudpdat_Conns[connId].UrTxBytes);

    return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GUDPDAT_GetUrRxBytes(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GUDPDAT_GetUrRxBytes(u8 connId)
{
	if (connId < GUDPDAT_MAX_CONNS)
        return (gudpdat_Conns[connId].UrRxBytes);

    return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GUDPDAT_GetConnStatus(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GUDPDAT_GetConnStatus(u8 connId)
{
	if (connId < GUDPDAT_MAX_CONNS)
        return (gudpdat_Conns[connId].State);

    return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GUDPDAT_Connect(u8 connId, u32 DestIp, u16 DestPort)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GUDPDAT_Connect(u8 connId, u32 DestIp, u16 DestPort)
{
	GUDPDAT_CONN_T *udpConn;

	/* Check input parameters */
	if (DestIp == 0 || connId >= GUDPDAT_MAX_CONNS)
	{
		return -1;
	}	
	
 	udpConn = (GUDPDAT_CONN_T*)&gudpdat_Conns[connId];

	/* Check state */
	if (udpConn->State != GUDPDAT_STATE_FREE)
	{
		return -2;
	}
	
	while(xSemaphoreTake(udpConn->AccessProtection, portMAX_DELAY) != pdTRUE);//Enable protection

	/* Creating a UDP socket */
	udpConn->Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpConn->Socket < 0)
	{
		printf("\n\rR2W: create socket failed");
		xSemaphoreGive(udpConn->AccessProtection);//Disable protection			
		return -3;
	}
	
	/* Connecting to UDP server */	
	gudpdat_DestAddr.sin_family = AF_INET;
	gudpdat_DestAddr.sin_len = sizeof(struct sockaddr_in);
	gudpdat_DestAddr.sin_port = htons(DestPort);
	gudpdat_DestAddr.sin_addr.s_addr = htonl(DestIp);

	udpConn->Id = connId;
	udpConn->State = GUDPDAT_STATE_ACTIVE;	
	udpConn->RemoteIp = DestIp;
	udpConn->RemotePort = DestPort;
	udpConn->UrRxBytes = 0;
	udpConn->UrTxBytes = 0;
//	udpConn->TxBufIndex = 0;
	udpConn->TxWaitTime = 0;
	udpConn->FlowControlXonRx = TRUE;
	udpConn->FlowControlModemStatus = 0;
	udpConn->FlowControlModemCtrl = 0;
	udpConn->FlowControlVirModemStatus = 0;
	GUART_ControlRTSSignal(&urObj, 1);
	
	xSemaphoreGive(udpConn->AccessProtection);//Disable protection
	return connId;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GUDPDAT_Close(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GUDPDAT_Close(u8 connId)
{
	GUDPDAT_CONN_T *udpConn;
	
	/* Check input parameters */
	if (connId >= GUDPDAT_MAX_CONNS)
	{
		return -1;
	}	
	
 	udpConn = (GUDPDAT_CONN_T*)&gudpdat_Conns[connId];
	
	if (udpConn->State == GUDPDAT_STATE_FREE)
	{
		return 0;
	}
	while(xSemaphoreTake(udpConn->AccessProtection, portMAX_DELAY) != pdTRUE);//Enable protection	
	if (gudpdat_UdpClient)
	{
		close(udpConn->Socket);
		udpConn->Socket = -1;
	}
	udpConn->State = GUDPDAT_STATE_FREE;
	udpConn->FlowControlXonRx = TRUE;
	GS2W_SetTaskState(GS2W_STATE_IDLE);
	xSemaphoreGive(udpConn->AccessProtection);//Disable protection	
	return connId;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GUDPDAT_IsManualConnectionMode(void)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GUDPDAT_IsManualConnectionMode(void)
{
	if (gudpdat_UdpClient != 0 && gudpdat_ConnectionMode == GCONFIG_MANUAL_CONNECTION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: void SetBaudrate(u32 Baudrate)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :Added by Craig 20170824
 * ----------------------------------------------------------------------------
 */
//void SetBaudrate(u32 Baudrate){
//  //serial_free(&urObj);         // Craig
//  //GUART_Init(GUART_INTERRUPT_MODE, GUART_TX_PIN, GUART_RX_PIN);        // Craig
//  serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
//  serial_baud(&urObj, Baudrate);           // Craig
//  //GUART_SetBaudrate(&urObj, Baudrate);       // Craig
//}
#endif //#if (GUDPDAT_INCLUDE)

    
    
    
    
    
    
    