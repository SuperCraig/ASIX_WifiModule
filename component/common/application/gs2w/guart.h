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
 * Module Name: guart.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */
#ifndef __GUART_H__
#define __GUART_H__
/* INCLUDE FILE DECLARATIONS */
#include "serial_api.h"


/* NAMING CONSTANT DECLARATIONS */
#define GUART_INTERRUPT_MODE		1
#define GUART_TASK_MODE				0

#define GUART_TX_BUF_SIZE			(2048 * 24)
#define GUART_RX_BUF_SIZE			(2048 * 32)
#define GUART_TX_PIN				PA_7
#define GUART_RX_PIN				PA_6

/* TYPE DECLARATIONS */
typedef enum
{
	RS485_DIR_LOW_RX = 0,
	RS485_DIR_HIGH_RX
} GUART_RS485_DIR_RX_POLARITY;

/* GLOBAL VARIABLES DECLARATIONS */
extern serial_t		urObj;


/* LOCAL VARIABLES DECLARATIONS */


/* EXPORTED SUBPROGRAM DECLARATIONS */
void GUART_Init(u8 mode, PinName txPin, PinName rxPin);
u32 GUART_GetRcvrAvailCount(void);
u16 GUART_GetXmitEmptyCount(void);
void GUART_CopyUrRxToApp(u8 *pAppBuf, u16 dataLen);
void GUART_CopyAppToUrTx(u8 *pAppBuf, u16 dataLen);
void GUART_SetFlowControl(serial_t *pUrObj, u8 enable);
void GUART_SetBaudrate(serial_t *pUrObj, u32 bRate);
u32 GUART_GetUrDiscardBytes(serial_t *pUrObj);
void GUART_ControlRTSSignal(serial_t *pUrObj, u8 onOff);
void GUART_TxIntrIdleCheck(void);
void GUART_RS485Init(PinName dirPin, GUART_RS485_DIR_RX_POLARITY rxPolarity);
void GUART_RS485Dir(u8 TxDir);
#endif