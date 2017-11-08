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
 * Module Name: gtcpdat.c
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
#include "gtcpdat.h"
#include "guart.h"
#include "gs2w.h"
#include "platform_opts.h"
#include "misc.h"
#include "wifi_constants.h"
#include "stdio.h"
#include "rtc.h"
#include "ths.h"

/* NAMING CONSTANT DECLARATIONS */


/* GLOBAL VARIABLES DECLARATIONS */
SECTION(".sdram.data")
GTCPDAT_CONN_T	gtcpdat_Conns[GTCPDAT_MAX_CONNS];

/* LOCAL VARIABLES DECLARATIONS */
static u8		gtcpdat_TcpClient;
static s16		gtcpdat_SocketFD;
static u8		*pTcpRcvrBuf;
static struct sockaddr_in	gtcpdat_SourAddr;
static struct sockaddr_in	gtcpdat_DestAddr;
TickType_t		gtcpdat_TickTime;
static u16		gtcpdat_TxTimeOut;
static u8		gtcpdat_ForVirtCom;
static u8		gtcpdat_VirtBuf[6] = {0};
static u8		gtcpdat_MaxConns = 0;
static u8		gtcpdat_ConnectionMode = GCONFIG_AUTO_CONNECTION;

//ian test modify
static u8		gtcpdat_hello_enable = 0;
portTickType    xTimeLatch_hello;
xQueueHandle    Global_xQueue_ssid;
u8              ian_ssid[32];


/* LOCAL SUBPROGRAM DECLARATIONS */
static void gtcpdat_TransferHandleTask(void *param);
static void gtcpdat_CopyUrDataToNetTxBuffers(void);
static s16 gtcpdat_TcpXmitPacket(GTCPDAT_CONN_T *tcpConn);
static s16 gtcpdat_TcpRecvPacketToUr(GTCPDAT_CONN_T *tcpConn);
static s16 gtcpdat_HandleFlowControlPacket(u8 *pData, u16 length, GTCPDAT_CONN_T *tcpConn);
static void gtcpdat_FreeBuf(void);
static s16 gtcpdat_SendModemStatusChange(GTCPDAT_CONN_T *tcpConn, u8 mStatus, u8 mCtrl);
static void gtcpdat_SetDeviceStatusIdle(u8 maxConn);

/* LOCAL SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: gtcpdat_TransferHandleTask()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gtcpdat_TransferHandleTask(void *param)
{
	struct timeval	timeout;
	fd_set			fdRead;
	s16				ret, newSocket;
	u16				iAddrSize = sizeof(struct sockaddr_in);
	u8				idConn, mStatus, mCtrl, mStatusTmp;
	u32				XonElapseTime, XonTickTime;
	u8				wifi_mode;
	  
	wifi_mode = (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP) ? RTW_AP_INTERFACE:RTW_STA_INTERFACE;
	
	while (1)
	{
		vTaskDelay(1);

		/* Wait for network is available */
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
		if (MISC_NetInterfaceReadyCheck() != IF_STATE_IP_READY)
#else
 		if (wifi_is_ready_to_transceive(wifi_mode) != RTW_SUCCESS)
#endif
		{
			vTaskDelay(500);
			continue;
		}
		
		gtcpdat_CopyUrDataToNetTxBuffers();
		
		if (gtcpdat_TcpClient == 0) // For TCP Server
		{
			// Wait a new connection
			timeout.tv_sec = 0;
			timeout.tv_usec = (GTCPDAT_TIMEOUT * 1000);
			FD_ZERO(&fdRead);
			FD_SET(gtcpdat_SocketFD, &fdRead);
			ret = select((gtcpdat_SocketFD + 1), &fdRead, NULL, NULL, &timeout);
			if ((ret > 0) && (FD_ISSET(gtcpdat_SocketFD, &fdRead) > 0))
			{
				newSocket = accept(gtcpdat_SocketFD, (struct sockaddr *)&gtcpdat_DestAddr, (socklen_t *)&iAddrSize);
				if (newSocket >= 0)
				{
					for (idConn = 0; idConn < gtcpdat_MaxConns; idConn++)
					{
						if (gtcpdat_Conns[idConn].State == GTCPDAT_STATE_FREE)
						{
							gtcpdat_Conns[idConn].State = GTCPDAT_STATE_CONNECTED;
							gtcpdat_Conns[idConn].RemoteIp = (u32)gtcpdat_DestAddr.sin_addr.s_addr;
							gtcpdat_Conns[idConn].RemotePort = (u16)gtcpdat_DestAddr.sin_port;
							gtcpdat_Conns[idConn].Socket = newSocket;
							gtcpdat_Conns[idConn].UrRxBytes = 0;
							gtcpdat_Conns[idConn].UrTxBytes = 0;
							//gtcpdat_Conns[idConn].TxBufIndex = 0;
							gtcpdat_Conns[idConn].TxWaitTime = 0;
							gtcpdat_Conns[idConn].FlowControlXonRx = TRUE;
							GUART_ControlRTSSignal(&urObj, 1);
							GS2W_SetTaskState(GS2W_STATE_TCP_DATA_PROCESS);
							printf("\n\rR2W: Server New TCP connection %d OK", idConn);
							break;
						}
					}
					if (idConn >= gtcpdat_MaxConns)
					{
						printf("R2W: Connection buffer is full!\r\n");
						close(newSocket);
					}
				}
			}
		}
		
		for (idConn = 0; idConn < gtcpdat_MaxConns; idConn++)
		{
			if (gtcpdat_Conns[idConn].State == GTCPDAT_STATE_FREE)
			{
				if (gtcpdat_TcpClient != 0 && gtcpdat_ConnectionMode == GCONFIG_AUTO_CONNECTION)  // For TCP Client
				{
					/* Wait for network is available */
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
					if (MISC_NetInterfaceReadyCheck() != IF_STATE_IP_READY)
#else
					if (wifi_is_ready_to_transceive(wifi_mode) != RTW_SUCCESS)
#endif
					{
						break;
					}
				
					ret = GTCPDAT_Connect(idConn, GS2W_CheckDestIP(), GCONFIG_GetClientDestPort());
					if (ret < 0)
					{
						printf("\n\rR2W: Connect TCP server fail.");
						continue;
					}
					printf("\n\rR2W: Connect TCP server successfully.");
				}
			}
			else if (gtcpdat_Conns[idConn].State == GTCPDAT_STATE_CONNECTED)
			{
				if (gtcpdat_Conns[idConn].Socket >= 0)
				{
					// Do the TCP receiving function and copy to UART Tx buffer
					if (gtcpdat_TcpRecvPacketToUr(&gtcpdat_Conns[idConn]) < 0)
					{
						continue;
					}

					// TCP transmitting function
					if (gtcpdat_ForVirtCom) // For Virtual COM Flow Control
					{
						/* Xoff timeout check */
						if (gtcpdat_Conns[idConn].FlowControlXonRx==FALSE)
						{
							XonTickTime = xTaskGetTickCount();
							if (XonTickTime >= gtcpdat_Conns[idConn].FlowControlXonLastTime)
							{
								XonElapseTime = XonTickTime - gtcpdat_Conns[idConn].FlowControlXonLastTime;
							}
							else
							{
								XonElapseTime = portMAX_DELAY - gtcpdat_Conns[idConn].FlowControlXonLastTime + XonTickTime;
							}
			
							if ((XonElapseTime * portTICK_RATE_MS) >= GTCPDAT_XOFF_TIMEOUT)
							{
								gtcpdat_Conns[idConn].FlowControlXonRx = TRUE;
								printf("R2W: Xoff timeout, set Xon\r\n");
							}
						}

						/* Send modem status change packet and handle the virtual DSR/DTR for better compatibility */
						mStatus = serial_raed_msr(&urObj) | GSMSR_DSR;
						if (GUART_GetXmitEmptyCount() < (GUART_TX_BUF_SIZE/2))
						{
							mStatus &= ~GSMSR_CTS;
						}
						else if (GUART_GetXmitEmptyCount() > (GUART_TX_BUF_SIZE/2))
						{
							mStatus |= GSMSR_CTS;
						}
						mCtrl = (serial_raed_mcr(&urObj) & GSMCR_RTS) | (gtcpdat_Conns[idConn].FlowControlVirModemStatus & GSMCR_DTR);
						
						if (gtcpdat_Conns[idConn].FlowControlModemStatus != mStatus || gtcpdat_Conns[idConn].FlowControlModemCtrl != mCtrl)
						{
							mStatusTmp = mStatus;
							
							if ((gtcpdat_Conns[idConn].FlowControlModemStatus & GSMSR_CTS) != (mStatus & GSMSR_CTS)) // Check CTS
							{
								mStatusTmp |= GSMSR_DCTS; // Set DCTS
							}
							if ((gtcpdat_Conns[idConn].FlowControlModemStatus & GSMSR_DSR) != (mStatus & GSMSR_DSR)) // Check DSR
							{
								mStatusTmp |= GSMSR_DDSR; // Set DDSR
							}
							
							gtcpdat_Conns[idConn].FlowControlModemStatus = mStatus;
							gtcpdat_Conns[idConn].FlowControlModemCtrl = mCtrl;
							
							if (gtcpdat_SendModemStatusChange(&gtcpdat_Conns[idConn], mStatusTmp, mCtrl) < 0)
							{
								continue;
							}
						}
						
						if (gtcpdat_Conns[idConn].FlowControlXonRx == FALSE)
						{
							continue;
						}
               		}

					// Do the TCP transmitting function that copy data from UART Rx buffer
					if (gtcpdat_TcpXmitPacket(&gtcpdat_Conns[idConn]) < 0)
					{
						continue;
					}
				}
			}
		}
	}
transferHandleTask_Exit:
	gtcpdat_FreeBuf();
	vTaskDelete(NULL);
}

/*
 * ----------------------------------------------------------------------------
 * Function: static s16 gtcpdat_SendModemStatusChange()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static s16 gtcpdat_SendModemStatusChange(GTCPDAT_CONN_T *tcpConn, u8 mStatus, u8 mCtrl)
{
	u8 tmp[10];

	tmp[0] = GS2W_FLOW_CONTROL_INDICATOR;
	tmp[1] = 0;
	tmp[2] = 3;
	tmp[3] = MODEM_STATUS_CHANGE;
	tmp[4] = mStatus;
	tmp[5] = mCtrl;

	if (send(tcpConn->Socket, tmp, 6, 0) <= 0)
	{
		printf("\n\rR2W: tcp send control error!");
		GTCPDAT_Close(tcpConn->Id);
		return -1;
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gtcpdat_CopyUrDataToNetTxBuffers()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gtcpdat_CopyUrDataToNetTxBuffers(void)
{
	int urDataLen, copyLen = -1;
	u8 *pRef=0, i, temp;
	GTCPDAT_CONN_T *tcpConn;
    //char demo_send_str[7] = {'h','e','l','l','o','\r','\n'};
    char demo_send_str[] = {'R','T','C',' ','0','0',':','0','0',':','0','0','\r','\n', //14 char
                            'T','e','m','p',':','0','0','.','0','0','^','C','\r','\n', //14 char
                            'H','u','m','i',':','0','0','.','0','0',' ',' ','\r','\n'  //14 char
                            };
    const portTickType xDelay3000ms = 3000 / portTICK_RATE_MS;
    portTickType    xTimeLatch_tmp;
    RTC_TIME    time_1;
    float       ftmp;
    float       frh;
	
#if CONFIG_HSUART_COMMAND_MODE
	if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))
	{
		return;
	}
#endif

	/* Find copy length */
	urDataLen = GUART_GetRcvrAvailCount();
	if (urDataLen)
	{
		for (i = 0; i < gtcpdat_MaxConns; i++)
		{
			tcpConn = &gtcpdat_Conns[i];			  
			if (tcpConn->State == GTCPDAT_STATE_CONNECTED)
			{
				/* Add header for virtual COM mode */
				if (gtcpdat_ForVirtCom)
				{
					if (tcpConn->TxBufIndex == 0)
					{
						tcpConn->TxBuf[0] = GS2W_DATA_INDICATOR; // transfer type of data packet indicator
						tcpConn->TxBufIndex += 3; // 1byte indicator + 2byte data length
					}
				}
				
				if (copyLen == -1 || (copyLen > (GTCPDAT_TX_BUF_SIZE - tcpConn->TxBufIndex)))
				{
					copyLen = GTCPDAT_TX_BUF_SIZE - tcpConn->TxBufIndex;
				}
			}
		}
		
		if (copyLen > urDataLen)
		{
			copyLen = urDataLen;
		}
	}
	else{ //ian test modify
        if (gtcpdat_hello_enable){  
            
            xTimeLatch_tmp = xTaskGetTickCount();
            if ((xTimeLatch_tmp - xTimeLatch_hello) >= 3000){
            
                tcpConn = &gtcpdat_Conns[0];			  
                if (tcpConn->State == GTCPDAT_STATE_CONNECTED)
                {               

                      copyLen =  sizeof(demo_send_str);//
                      xTimeLatch_hello = xTimeLatch_tmp;    
                      
                      RTC_GetCurrTime(&time_1);
                      demo_send_str[4] = (time_1.Hour/10)+0x30;//ASCII
                      demo_send_str[5] = (time_1.Hour%10)+0x30;//ASCII
                      demo_send_str[7] = (time_1.Minute/10)+0x30;//ASCII
                      demo_send_str[8] = (time_1.Minute%10)+0x30;//ASCII
                      demo_send_str[10] = (time_1.Second/10)+0x30;//ASCII
                      demo_send_str[11] = (time_1.Second%10)+0x30;//ASCII
                      /*printf("\n RTC timer = %d/%d/%d  D%d  %d:%d:%d \n",
                             time_1.Year, 
                             time_1.Month, 
                             time_1.Date, 
                             time_1.Day, 
                             time_1.Hour, 
                             time_1.Minute,
                             time_1.Second ); */
                  
                      THS_GetTemperature(&ftmp);                      
                      //printf("\n Temperature = %.2f C \n", ftmp);
                      demo_send_str[19] = ((char)ftmp/10)+0x30;//ASCII
                      demo_send_str[20] = ((char)ftmp%10)+0x30;//ASCII
                      ftmp *= 100; temp = (int)ftmp%100;
                      demo_send_str[22] = (temp/10)+0x30;//ASCII
                      demo_send_str[23] = (temp%10)+0x30;//ASCII
                  
                      THS_GetRelativeHumidity(&frh);
                      //printf("\n Relative Humidity = %.2f \n", frh););
                      demo_send_str[33] = ((char)frh/10)+0x30;//ASCII
                      demo_send_str[34] = ((char)frh%10)+0x30;//ASCII
                      frh *= 100; temp = (int)frh%100;
                      demo_send_str[36] = (temp/10)+0x30;//ASCII
                      demo_send_str[37] = (temp%10)+0x30;//ASCII
                  
                  //===== don't put the delay into other's task ============
                  //xTimeLatch_hello = xTaskGetTickCount();
                  //copyLen = 7;
                  //vTaskDelay(xDelay3000ms);//(3000);
                   //printf("\n\r\t TaskGetTickCount  => %d", xTaskGetTickCount()-xTimeLatch_hello);
                  //====== Realtek : this sample code, 1 tick = 1 ms =====
                
                }
              
            }
          
        }
	}
	
	if (copyLen > 0)
	{
		/* Copy UART received data to each TCP connection buffer */
		for (i=0; i<gtcpdat_MaxConns; i++)
		{
			tcpConn = &gtcpdat_Conns[i];
			if (tcpConn->State == GTCPDAT_STATE_CONNECTED)
			{
				if (pRef==0)
				{
					//ian test modify
                 	if (urDataLen > 0){ //original code, (urDataLen > 0) no change
						GUART_CopyUrRxToApp(&(tcpConn->TxBuf[tcpConn->TxBufIndex]), copyLen);
					}
                    
					pRef = &(tcpConn->TxBuf[tcpConn->TxBufIndex]);
#if CONFIG_HSUART_COMMAND_MODE
					if (GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, pRef, copyLen))
					{
						return;
					}
#endif
				}
				else
				{
					memcpy(&(tcpConn->TxBuf[tcpConn->TxBufIndex]), pRef, copyLen);
				}
                
				//ian test modify
                //copyLen = ;
                if (urDataLen <= 0){
                	memcpy(&(tcpConn->TxBuf[tcpConn->TxBufIndex]), demo_send_str, copyLen);
                    //printf("\n\r\t ian test send tcp string");
                    /* Keep UART RX interrupt is active */
                    GUART_CopyUrRxToApp(0, 0);
                }
                
				tcpConn->UrRxBytes += copyLen;
				tcpConn->TxBufIndex += copyLen;
			}
		}
	}
	else
	{
#if CONFIG_HSUART_COMMAND_MODE
		/* Check HSUART command mode pattern */		  
		if (urDataLen != 0 && copyLen < 0)
		{
			copyLen = (urDataLen < 10) ? urDataLen : 10;
			while (copyLen--)
			{
				GUART_CopyUrRxToApp(&i, 1);
				GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, &i, 1);
			}
		}
#endif
		/* Keep UART RX interrupt is active */
		GUART_CopyUrRxToApp(0, 0);
	}	
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gtcpdat_TcpXmitPacket(GTCPDAT_CONN_T *tcpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static s16 gtcpdat_TcpXmitPacket(GTCPDAT_CONN_T *tcpConn)
{
	u32		txTime;
	u16		txSendLen=0;
	s16		iStatus;
	
	if (tcpConn->TxBufIndex >= GTCPDAT_TX_MAX_LEN)
	{
		if (gtcpdat_ForVirtCom) // For Virtual COM Flow Control
		{
			txSendLen = GTCPDAT_TX_MAX_LEN;
		}
		else
		{
			txSendLen = tcpConn->TxBufIndex;
		}
	}
	else
	{
		gtcpdat_TickTime = xTaskGetTickCount();
		if (tcpConn->TxBufIndex == 0 || tcpConn->TxWaitTime == 0)
		{
			tcpConn->TxWaitTime = gtcpdat_TickTime;
		}
		else
		{
			if (gtcpdat_TickTime >= tcpConn->TxWaitTime)
			{
				txTime = gtcpdat_TickTime - tcpConn->TxWaitTime;
			}
			else
			{
				txTime = portMAX_DELAY - tcpConn->TxWaitTime + gtcpdat_TickTime;
			}
			
			if ((txTime * portTICK_RATE_MS) >= gtcpdat_TxTimeOut)
			{
				txSendLen = tcpConn->TxBufIndex;
			}
		}
	}

	if (txSendLen > (gtcpdat_ForVirtCom * 3))
	{
		if (gtcpdat_ForVirtCom) // For Virtual COM Flow Control
		{
#if 0/* Notice: Must mask this code section for VSPD(virtual serial port driver) v3.1.0.x or later */
			if (lwip_chksndqlen(tcpConn->Socket))
			{
				return 0;
			}
#endif
			tcpConn->TxBuf[1] = (u8)((txSendLen - 3) >> 8);
			tcpConn->TxBuf[2] = (u8)(txSendLen - 3);
		}
		
		/* Use non-block mode to transmit data for better throughput under bidirectional transfer */
		iStatus = send(tcpConn->Socket, tcpConn->TxBuf, txSendLen, MSG_DONTWAIT);
		if (iStatus <= 0)
		{
			return 0;
		}
		else
		{
			// TCP transmitting successfully.
			tcpConn->TxWaitTime = 0;
			txSendLen = iStatus;
			tcpConn->TxBufIndex -= txSendLen; // Updated the TX buffer index
			if (tcpConn->TxBufIndex) // Check the remained data length
			{
				if (gtcpdat_ForVirtCom) // For Virtual COM Flow Control
				{
					memcpy(&(tcpConn->TxBuf[3]), &(tcpConn->TxBuf[txSendLen]), tcpConn->TxBufIndex);
					tcpConn->TxBuf[0] = GS2W_DATA_INDICATOR; // TCP data packet indicator
					tcpConn->TxBufIndex += 3; // 1byte indicator + 2byte data length
				}
				else
				{
					//printf("R2W: Remain %d\n\r", gtcpdat_Conns[idConn].TxBufIndex);
					memcpy(&(tcpConn->TxBuf[0]), &(tcpConn->TxBuf[txSendLen]), tcpConn->TxBufIndex);
				}
			}
		}
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: static s16 gtcpdat_TcpRecvPacketToUr(GTCPDAT_CONN_T *tcpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static s16 gtcpdat_TcpRecvPacketToUr(GTCPDAT_CONN_T *tcpConn)
{
	u16		urDataLen = 0, dataLen;
	s16		ret, recvLen = 0;
	fd_set	fdRead;
	struct timeval	timeout;
	u8		*pbuf;
    u8      temp;
	
	urDataLen = GUART_GetXmitEmptyCount();
	if (urDataLen)
	{
		if (urDataLen > GTCPDAT_RX_BUF_SIZE)
		{
			urDataLen = GTCPDAT_RX_BUF_SIZE;
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = (GTCPDAT_TIMEOUT * 1000);
		FD_ZERO(&fdRead);
		FD_SET(tcpConn->Socket, &fdRead);
		ret = select((tcpConn->Socket + 1), &fdRead, NULL, NULL, &timeout);
		if ((ret > 0) && (FD_ISSET(tcpConn->Socket, &fdRead) > 0))
		{
			recvLen = recv(tcpConn->Socket, pTcpRcvrBuf, urDataLen, 0);
			if (recvLen <= 0)
			{
				if (recvLen == 0)
				{
					printf("\n\rR2W: Close connection id:%d ", tcpConn->Id);
				}
				else
				{
					printf("\n\rR2W: recv data error recvLen:%d ", recvLen);
				}
				GTCPDAT_Close(tcpConn->Id);
				return -1;
			}
			else
			{
				// receiving data successfully and moving data to UART Tx buffer
#if CONFIG_HSUART_COMMAND_MODE	
				if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))//Discard received packets in command mode
				{
					return 0;
				}
#endif
				if (gtcpdat_ForVirtCom) // For Virtual COM Flow Control
				{
					pbuf = pTcpRcvrBuf;
gtcpdat_MoreData:
					dataLen = ((u16)pbuf[1] << 8) + (u16)pbuf[2] + 3;
					if (dataLen > recvLen)
					{
						dataLen = recvLen;
					}
					
					if (pbuf[0] == GS2W_FLOW_CONTROL_INDICATOR)
					{
						if (gtcpdat_HandleFlowControlPacket(pbuf, dataLen, tcpConn) < 0)
						{
							return -1;
						}
					}
					else if (pbuf[0] == GS2W_DATA_INDICATOR)
					{
						GUART_CopyAppToUrTx((pbuf + 3), (dataLen - 3));
						tcpConn->UrTxBytes += (dataLen - 3);
					}
					
                	recvLen -= dataLen;
                	if (recvLen > 0)
                	{
						/* point to the next packet header */
						pbuf += dataLen;
						goto gtcpdat_MoreData;
                	}
				}
				else
				{
					GUART_CopyAppToUrTx(pTcpRcvrBuf, recvLen);
					tcpConn->UrTxBytes += recvLen;
                                        
                                        //ian test modify
                                        if ((pTcpRcvrBuf[0] == '3') && (pTcpRcvrBuf[1] == '6') && (pTcpRcvrBuf[2] == '9')){    
                                              gtcpdat_hello_enable = 1;
                                              
                                              //ian test
                                              printf("\n\r gtcpdat_hello_enable, pTcpRcvrBuf[0 - 2] UrTx= %x, %x, %x " , pTcpRcvrBuf[0],pTcpRcvrBuf[1],pTcpRcvrBuf[2]);
					
                                              xTimeLatch_hello = xTaskGetTickCount();
                                              
                                              pTcpRcvrBuf[1] = 0;
                                              pTcpRcvrBuf[2] = 0;
                                        }
                                        else                            gtcpdat_hello_enable = 0;
                                        
                                        //ian test modify
                                        if ((pTcpRcvrBuf[0] == '3') && (pTcpRcvrBuf[1] == '9') && (pTcpRcvrBuf[2] == ':')){   
                                          
                                            
                                            if(recvLen <= 3){
                                                printf("\n\r xQueueSend SSID length Err");                                            
                                            }
                                            else{
                                                for (temp = 3; temp < recvLen; temp++){
                                                    ian_ssid[temp-3] = pTcpRcvrBuf[temp];
                                                }
                                                ian_ssid[temp] = '\n';
                                                
                                                if ( xQueueSend(Global_xQueue_ssid, &ian_ssid, 0) != pdPASS ){
                                                    printf("\n\r xQueueSend pTcpRcvrBuf[%d] failed", temp);
                                                }
                                            }
                                            
                                          
                                        }
            
                                        //ian modify : real receive data
                                        //printf("\n\r pTcpRcvrBuf[0 - 4] UrTx= %x, %x, %x, %x, %x, " , pTcpRcvrBuf[0],pTcpRcvrBuf[1],pTcpRcvrBuf[2],pTcpRcvrBuf[3],pTcpRcvrBuf[4]);
					
                                        //ian modify : real receive length
                                        printf("\n\r pTcpRcvrBuf: UrTx= %d : %d", recvLen, tcpConn->UrTxBytes);
                                        
					//printf("R2W: UrTx= %d : %d\n\r", recvLen, tcpConn->UrTxBytes);
				}
			}
		}
	}
	GUART_TxIntrIdleCheck();
	return 0;
}

/* EXPORTED SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function: GTCPDAT_ServerInit()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
u8 GTCPDAT_ServerInit(void)
{
	s16		iStatus;
	int		option;
	
	/* Creating a TCP socket */
	gtcpdat_SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (gtcpdat_SocketFD < 0)
	{
		printf("\n\rR2W: create tcp server socket fd error!");
		goto ServerError;
	}
	
	/* Enable max connection monitor */
	if (lwip_monitorMaxConn(gtcpdat_SocketFD, 1) < 0)
	{
		printf("\n\rR2W: enable max conn monitor failed!");
		goto ServerError;
	}
	
	/* Enable reuse addr */
	option = 1;
	setsockopt(gtcpdat_SocketFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option));
	
	/* Enable keep alive */
	option = 1;
	setsockopt(gtcpdat_SocketFD, SOL_SOCKET, SO_KEEPALIVE, (const char *)&option, sizeof(option));	

	/* Binding the TCP socket to the TCP server address */
	iStatus = bind(gtcpdat_SocketFD, (struct sockaddr *)&gtcpdat_SourAddr, sizeof(struct sockaddr_in));
	if (iStatus < 0)
	{
		printf("\n\rR2W: bind tcp server socket fd error!");
		close(gtcpdat_SocketFD);
		goto ServerError;
	}
	
	/* Putting the socket for listening to the incoming TCP connection */
	iStatus = listen(gtcpdat_SocketFD, gtcpdat_MaxConns);
	if (iStatus != 0)
	{
		printf("\n\rR2W: listen tcp server socket fd error!");
		close(gtcpdat_SocketFD);
		goto ServerError;
	}

	/* Create the R2W TCP receiving and transmitting function */
	if(xTaskCreate(gtcpdat_TransferHandleTask, "gtcp_ServerHandle", GTCPDAT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) == pdPASS)
	{
		goto ServerExit;
	}
	else
	{
		close(gtcpdat_SocketFD);
		printf("\n\rR2W: Create gtcpdat_TransferHandleTask() task failed.");
	}

ServerError:
	gtcpdat_FreeBuf();

ServerExit:
	vTaskDelete(NULL);
	
	return TRUE;
}

/*
 * ----------------------------------------------------------------------------
 * Function: void gtcpdat_HandleFlowControlPacket(u8 *pData, u16 length, GTCPDAT_CONN_T *tcpConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static s16 gtcpdat_HandleFlowControlPacket(u8 *pData, u16 length, GTCPDAT_CONN_T *tcpConn)
{
	u32	bRate;
	GS2W_IOCTL_PKT *pPkt = (GS2W_IOCTL_PKT*)pData;
	int iStatus = 1;

	switch (pPkt->IoCtrlType)
	{
		case IOCTL_SERIAL_GET_DTRRTS:
			gtcpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gtcpdat_VirtBuf[1] = 0;
			gtcpdat_VirtBuf[2] = 2;
			gtcpdat_VirtBuf[3] = IOCTL_SERIAL_GET_DTRRTS_REPLY;
			gtcpdat_VirtBuf[4] = tcpConn->FlowControlModemCtrl;/* Handle virtual DTR for better compatibility */
			iStatus = send(tcpConn->Socket, &gtcpdat_VirtBuf[0], 5, 0);
			break;
		case IOCTL_SERIAL_GET_MODEM_CONTROL:
			gtcpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gtcpdat_VirtBuf[1] = 0;
			gtcpdat_VirtBuf[2] = 2;
			gtcpdat_VirtBuf[3] = IOCTL_SERIAL_GET_MODEM_CONTROL_REPLY;
			gtcpdat_VirtBuf[4] = tcpConn->FlowControlModemCtrl;/* Handle virtual DTR for better compatibility */
			iStatus = send(tcpConn->Socket, &gtcpdat_VirtBuf[0], 5, 0);
			break;
		case IOCTL_SERIAL_GET_MODEM_STATUS:
			gtcpdat_VirtBuf[0] = GS2W_FLOW_CONTROL_INDICATOR;
			gtcpdat_VirtBuf[1] = 0;
			gtcpdat_VirtBuf[2] = 2;
			gtcpdat_VirtBuf[3] = IOCTL_SERIAL_GET_MODEM_STATUS_REPLY;
			gtcpdat_VirtBuf[4] = tcpConn->FlowControlModemStatus;/* Handle virtual DSR for better compatibility */
			iStatus = send(tcpConn->Socket, &gtcpdat_VirtBuf[0], 5, 0);
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
					return 0;
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
					return 0;
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
					return 0;
			}
			printf("R2W: Set flow control to %s\n\r", (pPkt->Data.SetHardFlow.CtrlValue==FLOW_CTRL_NONE) ? "None":"Hardware");
           	GUART_SetFlowControl(&urObj, GCONFIG_GetUrFlowCtrl());
			break;
		case IOCTL_SERIAL_SET_XOFF:
			printf("R2W: Xoff\r\n");
			tcpConn->FlowControlXonRx = FALSE;
			tcpConn->FlowControlXonLastTime = xTaskGetTickCount();			
			break;
		case IOCTL_SERIAL_SET_XON:
			printf("R2W: Xon\r\n");
			tcpConn->FlowControlXonRx = TRUE;
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
			tcpConn->FlowControlVirModemStatus &= ~GSMCR_DTR;/* Handle virtual DTR for better compatibility */
			break;
		case IOCTL_SERIAL_SET_DTR:
			printf("R2W: Set DTR\r\n");
			tcpConn->FlowControlVirModemStatus |= GSMCR_DTR;/* Handle virtual DTR for better compatibility */
			break;
		default:
			printf("R2W: Ignore cmd %x\n\r", pPkt->IoCtrlType);
			break;
	}

	if (iStatus <= 0)
	{
		GTCPDAT_Close(tcpConn->Id);	  
		return -1;
	}
	return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: gtcpdat_FreeBuf()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gtcpdat_FreeBuf(void)
{
	u16 i;

	if (pTcpRcvrBuf)
	{
		vPortFree(pTcpRcvrBuf);
		pTcpRcvrBuf = NULL;
	}
	
	for (i = 0; i < gtcpdat_MaxConns; i++)
    {
		if (gtcpdat_Conns[i].AccessProtection)
		{
			vSemaphoreDelete(gtcpdat_Conns[i].AccessProtection);
		}
	}	
}

/*
 * ----------------------------------------------------------------------------
 * Function: static void gtcpdat_SetStatusIdle(u8 maxConn)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void gtcpdat_SetDeviceStatusIdle(u8 maxConn)
{
	u8	i;

	for (i = 0; i < maxConn; i++)
	{
		if (gtcpdat_Conns[i].State != GTCPDAT_STATE_FREE)
			return;
	}

	GS2W_SetTaskState(GS2W_STATE_IDLE);
}

/*
 * ----------------------------------------------------------------------------
 * Function: GTCPDAT_Init()
 * Purpose : Initialization
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void GTCPDAT_Init(void)
{
	u8	i;        
        
    // ian test modify        
    Global_xQueue_ssid = xQueueCreate( 2, sizeof(ian_ssid) );

	/* Allocate maximum connections */
	gtcpdat_MaxConns = (GCONFIG_GetNetwork() & GCONFIG_NETWORK_CLIENT) ? 1:GCONFIG_GetR2wMaxConnections();
	if (gtcpdat_MaxConns > GTCPDAT_MAX_CONNS)
		gtcpdat_MaxConns = GTCPDAT_MAX_CONNS;
	
	pTcpRcvrBuf = pvPortMalloc(GTCPDAT_RX_BUF_SIZE);
	if (pTcpRcvrBuf == NULL)
		return;

	memset(pTcpRcvrBuf, 0, GTCPDAT_RX_BUF_SIZE);
	memset(&gtcpdat_SourAddr, 0, sizeof(struct sockaddr_in));
	memset(&gtcpdat_DestAddr, 0, sizeof(struct sockaddr_in));
	gtcpdat_TxTimeOut = GCONFIG_GetEthernetTxTimer();
	if (GCONFIG_GetUrMode() == R2WMODE_VCom)
		gtcpdat_ForVirtCom = 1;
	else
		gtcpdat_ForVirtCom = 0;
	
	gtcpdat_ConnectionMode = GCONFIG_GetClientConnectionMode();

	gtcpdat_SourAddr.sin_family = AF_INET;
	gtcpdat_SourAddr.sin_len = sizeof(struct sockaddr_in);
	gtcpdat_SourAddr.sin_port = htons(GCONFIG_GetDeviceDataPktListenPort());
	gtcpdat_SourAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	for (i = 0; i < gtcpdat_MaxConns; i++)
    {
		gtcpdat_Conns[i].Id = i;
		gtcpdat_Conns[i].State = GTCPDAT_STATE_FREE;
		gtcpdat_Conns[i].RemoteIp = 0;
		gtcpdat_Conns[i].RemotePort = 0;
		gtcpdat_Conns[i].Socket = -1;
		gtcpdat_Conns[i].TxWaitTime = 0;
		gtcpdat_Conns[i].UrRxBytes = 0;
		gtcpdat_Conns[i].UrTxBytes = 0;
		gtcpdat_Conns[i].TxBufIndex = 0;
		gtcpdat_Conns[i].FlowControlXonRx = TRUE;
		gtcpdat_Conns[i].FlowControlModemStatus = 0;
		gtcpdat_Conns[i].FlowControlModemCtrl = 0;
		gtcpdat_Conns[i].FlowControlVirModemStatus = 0;
		gtcpdat_Conns[i].AccessProtection = xSemaphoreCreateMutex();
		memset(gtcpdat_Conns[i].TxBuf, 0, GTCPDAT_TX_BUF_SIZE);
    }
	
	if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_CLIENT)
	{
		/* Initial a TCP Client */
		gtcpdat_TcpClient = 1;
		if(xTaskCreate(gtcpdat_TransferHandleTask, "gtcp_client", GTCPDAT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		{
			gtcpdat_FreeBuf();
			printf("\n\rR2W: Create gtcpdat_TransferHandleTask() task failed.");
			return;
		}
	}
	else
	{
		/* Initial a TCP Server */
		gtcpdat_TcpClient = 0;
		if(xTaskCreate(GTCPDAT_ServerInit, "gtcp_server", GTCPDAT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		{
			gtcpdat_FreeBuf();
			printf("\n\rR2W: Create GTCPDAT_ServerInit() task failed.");
			return;
		}
	}
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GTCPDAT_GetUrTxBytes(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GTCPDAT_GetUrTxBytes(u8 connId)
{
	if (connId < gtcpdat_MaxConns)
        return (gtcpdat_Conns[connId].UrTxBytes);

    return 0;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GTCPDAT_GetUrRxBytes(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GTCPDAT_GetUrRxBytes(u8 connId)
{
	if (connId < gtcpdat_MaxConns)
        return (gtcpdat_Conns[connId].UrRxBytes);

    return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s32 GTCPDAT_GetConnStatus(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s32 GTCPDAT_GetConnStatus(u8 connId)
{
	if (connId < gtcpdat_MaxConns)
        return (gtcpdat_Conns[connId].State);

    return -1;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GTCPDAT_Connect(u8 connId, u32 DestIp, u16 DestPort)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GTCPDAT_Connect(u8 connId, u32 DestIp, u16 DestPort)
{
	static struct sockaddr_in DestInfo;  
	GTCPDAT_CONN_T *tcpConn;
	int option;

	/* Check input parameters */
	if (DestIp == 0 || connId >= gtcpdat_MaxConns)
	{
		return -1;
	}	
	
 	tcpConn = (GTCPDAT_CONN_T*)&gtcpdat_Conns[connId];

	/* Check state */
	if (tcpConn->State != GTCPDAT_STATE_FREE)
	{
		return -2;
	}
	
	while(xSemaphoreTake(tcpConn->AccessProtection, portMAX_DELAY) != pdTRUE);//Enable protection

	/* Creating a TCP socket */
	tcpConn->Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpConn->Socket < 0)
	{
		printf("\n\rR2W: create tcp client socket fd error!");
		xSemaphoreGive(tcpConn->AccessProtection);//Disable protection
		return -3;
	}
	option = 1;
	setsockopt(tcpConn->Socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&option, sizeof(option));
	
	/* Connecting to TCP server */
	DestInfo.sin_family = AF_INET;
	DestInfo.sin_len = sizeof(struct sockaddr_in);
	DestInfo.sin_port = htons(DestPort);
	DestInfo.sin_addr.s_addr = htonl(DestIp);

	if (connect(tcpConn->Socket, (struct sockaddr *)&DestInfo, sizeof(DestInfo)) < 0)
	{
		close(tcpConn->Socket);
		tcpConn->Socket = -1;
		xSemaphoreGive(tcpConn->AccessProtection);//Disable protection
		return -4;
	}
	
	tcpConn->Id = connId;
	tcpConn->State = GTCPDAT_STATE_CONNECTED;
	tcpConn->RemoteIp = DestIp;
	tcpConn->RemotePort = DestPort;
	tcpConn->UrRxBytes = 0;
	tcpConn->UrTxBytes = 0;
//	tcpConn->TxBufIndex = 0;
	tcpConn->TxWaitTime = 0;
	tcpConn->FlowControlXonRx = TRUE;
	tcpConn->FlowControlModemStatus = 0;
	tcpConn->FlowControlModemCtrl = 0;
	tcpConn->FlowControlVirModemStatus = 0;
	GUART_ControlRTSSignal(&urObj, 1);
	GS2W_SetTaskState(GS2W_STATE_TCP_DATA_PROCESS);

	xSemaphoreGive(tcpConn->AccessProtection);//Disable protection
	return connId;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GTCPDAT_Close(u8 connId)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GTCPDAT_Close(u8 connId)
{
	GTCPDAT_CONN_T *tcpConn;
	
	/* Check input parameters */
	if (connId >= gtcpdat_MaxConns)
	{
		return -1;
	}	
	
 	tcpConn = (GTCPDAT_CONN_T*)&gtcpdat_Conns[connId];
	
	if (tcpConn->State == GTCPDAT_STATE_FREE)
	{
		return 0;
	}
	while(xSemaphoreTake(tcpConn->AccessProtection, portMAX_DELAY) != pdTRUE);//Enable protection	
	shutdown(tcpConn->Socket, SHUT_RDWR);
	close(tcpConn->Socket);
	tcpConn->Socket = -1;
	tcpConn->State = GTCPDAT_STATE_FREE;
	gtcpdat_SetDeviceStatusIdle(gtcpdat_MaxConns);
	xSemaphoreGive(tcpConn->AccessProtection);//Disable protection
	return connId;
}

/*
 * ----------------------------------------------------------------------------
 * Function: s16 GTCPDAT_IsManualConnectionMode(void)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
s16 GTCPDAT_IsManualConnectionMode(void)
{
	if (gtcpdat_TcpClient != 0 && gtcpdat_ConnectionMode == GCONFIG_MANUAL_CONNECTION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#if 1
/*** For Debug ***/
void DEBUG_DumpTcpInfo(void)
{
	u16		i;

	printf("R2W TcpTx:\n\r");
	for (i=0; i<GTCPDAT_TX_BUF_SIZE; i++)
	{
		printf(" %02x", gtcpdat_Conns[0].TxBuf[i]);
		if ((i+1) % 20 == 0)
			printf("\n\r");
	}
	printf("\n\r");
	printf("R2W TcpRx:\n\r");
	for (i=0; i<GTCPDAT_RX_BUF_SIZE; i++)
	{
		printf(" %02x", pTcpRcvrBuf[i]);
		if ((i+1) % 20 == 0)
			printf("\n\r");
	}
	printf("\n\r");
}
#endif

