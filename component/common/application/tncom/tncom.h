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
/*=============================================================================
 * Module Name: TELNET COM
 * Purpose: The purpose of this package provides the services to TELNET COM
 * Author:
 * Date:
 * Notes:
 * $Log$
*=============================================================================
*/

#ifndef __TELNET_H__
#define __TELNET_H__

/* INCLUDE FILE DECLARATIONS */
/* NAMING CONSTANT DECLARATIONS */
/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */ 
void TNCom_Init(void *pObj, u16 tnComPort);
void TNCOM_PutRxChar(void *pObj, u8 c);
u8 TNCOM_GetRxChar(void *pObj);
void TNCOM_ApplyBaudrate(void *pObj, u32 br);
void TNCOM_ApplyFormat(void *pObj, u8 databit, u8 parity, u8 stopbit);
void TNCOM_ApplyFlowCtrl(void *pObj, u8 flowCtrl);
void TNCOM_PutRxString(void *pObj, u8 *buf, u16 len);
void TNCOM_GetRxString(void *pObj, u8 *buf, u16 len);
u32 TNCOM_UrRcvdCount(void *pObj);
void TNCOM_GetRxTxCounter(u32 *rxCounter, u32 *txCounter);
u8 TNCOM_CheckHsuartCmdMode(u8 *buf, u16 len);
int TNCOM_CopyUrRxToApp(void *pObj, u8 *buf, u16 len);

#endif /* __TELNET_H__ */

/* End of tncom.h */