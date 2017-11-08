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
 * Module Name: TELNET COM API
 * Purpose: The purpose of this package provides the services to TELNET COM
 * Author:
 * Date:
 * Notes:
 * $Log$
*=============================================================================
*/
/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>
//#include "sockets.h"
#include "main.h"
#include "gconfig.h"
#include "serial_api.h"
#include "guart.h"
#include "tncom.h"
#include "gs2w.h"
#include "platform_opts.h"

/* NAMING CONSTANT DECLARATIONS */
/* GLOBAL VARIABLES DECLARATIONS */
/* LOCAL VARIABLES DECLARATIONS */
/* LOCAL SUBPROGRAM DECLARATIONS */
/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_PutRxChar()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_PutRxChar(void *pObj, u8 c)
{
	serial_t *pUrObj = pObj;

#if CONFIG_HSUART_COMMAND_MODE
	if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))//Discard received packets in command mode
		return;
#endif
	GUART_CopyAppToUrTx(&c, 1);
} /* End of TNCOM_PutRxChar() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_PutRxString()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_PutRxString(void *pObj, u8 *buf, u16 len)
{
	serial_t *pUrObj = pObj;

#if CONFIG_HSUART_COMMAND_MODE
	if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))//Discard received packets in command mode
		return;
#endif
	GUART_CopyAppToUrTx(buf, len);
} /* End of TNCOM_PutRxString() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_GetRxChar()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 TNCOM_GetRxChar(void *pObj)
{
	serial_t *pUrObj = pObj;
	u8 c;

	GUART_CopyUrRxToApp(&c, 1);
#if CONFIG_HSUART_COMMAND_MODE		
	GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, &c, 1);
#endif
	return c;
} /* End of TNCOM_GetRxChar() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_GetRxString()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_GetRxString(void *pObj, u8 *buf, u16 len)
{
	serial_t *pUrObj = pObj;

	GUART_CopyUrRxToApp(buf, len);
#if CONFIG_HSUART_COMMAND_MODE		
	GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, buf, len);
#endif
} /* End of TNCOM_GetRxString() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_ApplyBaudrate()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_ApplyBaudrate(void *pObj, u32 br)
{
	serial_t *pUrObj = pObj;
	serial_baud(pUrObj, br);
} /* End of TNCOM_ApplyBaudrate() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_ApplyFormat()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_ApplyFormat(void *pObj, u8 databit, u8 parity, u8 stopbit)
{
	serial_t *pUrObj = pObj;
	serial_format(pUrObj, databit, parity, stopbit);
} /* End of TNCOM_ApplyFormat() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_ApplyFlowCtrl()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void TNCOM_ApplyFlowCtrl(void *pObj, u8 flowCtrl)
{
	serial_t *pUrObj = pObj;
	GUART_SetFlowControl(pUrObj, flowCtrl);
} /* End of TNCOM_ApplyFlowCtrl() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_UrRcvdCount()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 TNCOM_UrRcvdCount(void *pObj)
{
	serial_t *pUrObj = pObj;
#if CONFIG_HSUART_COMMAND_MODE
	if (GS2W_CmdModeIsEnable(&GS2W_CmdMode))//Discard received packets in command mode
		return 0;
#endif	
	return GUART_GetRcvrAvailCount();
} /* End of TNCOM_UrRcvdCount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_CheckHsuartCmdMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 TNCOM_CheckHsuartCmdMode(u8 *buf, u16 len)
{
#if CONFIG_HSUART_COMMAND_MODE		
	return GS2W_CmdModeSwitchCheck(&GS2W_CmdMode, buf, len);
#else
	return 0;
#endif
} /* End of TNCOM_CheckHsuartCmdMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: TNCOM_CopyUrRxToApp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int TNCOM_CopyUrRxToApp(void *pObj, u8 *buf, u16 len)
{
	GUART_CopyUrRxToApp(buf, len);

	return len;
} /* End of TNCOM_CopyUrRxToApp() */

/* End of tncom_api.c */