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
 * Module Name: email.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "email.h"
#include "gconfig.h"
#include <lwip/sockets.h>

/* NAMING CONSTANT DECLARATIONS */
#define EMAIL_MIME_VERSION	"MIME-Version: 1.0\r\n"
#define EMAIL_CONTENT_TYPE	"Content-Type: text/plain\r\n"

/* GLOBAL VARIABLES DECLARATIONS */

/* LOCAL VARIABLES DECLARATIONS */
static u8 *pemail_ptr;
static u8 email_tmp8;
static u16 email_tmp16;

/* LOCAL SUBPROGRAM DECLARATIONS */
static void email_Task(void *pParam);

/* LOCAL SUBPROGRAM BODIES */

/* EXPORTED SUBPROGRAM BODIES */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void email_Task(void *pParam)
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void email_Task(void *pParam)
{
	SMTPC_REQUEST *pReq;
	
	if (GCONFIG_GetAutoWarning() & GCONFIG_SMTP_EVENT_COLDSTART)
	{
 		pReq = SMTPC_AllocateRequest(3);
		if(pReq)
    	{	
			GCONFIG_GetSMTPFrom(pReq->MailBoxFrom);
			pReq->ValidMailBoxToNum = 0;
			GCONFIG_GetSMTPTo1(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo2(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo3(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);			
			pReq->pMailBuildCb = EMAIL_BuildColdStart;
			SMTPC_MailEnqueue(pReq);	
		}
	}
	
	vTaskDelete(NULL);	
} /* End of email_Task() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_SendColdStart
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 EMAIL_SendColdStart(void)
{
	if(xTaskCreate(email_Task, (u8*)"email_task", 1280, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)                  
	{
		printf("EMAIL: Create task fail!\r\n");
	}
	return -1;
} /* End of EMAIL_SendColdStart() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_BuildColdStart
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
SMTPC_MSG_BUILD_STATUS EMAIL_BuildColdStart(SMTPC_REQUEST *pReq)
{
	switch (pReq->MailBuildStatus)
	{
	case 0:
		email_tmp16 = SMTPC_MsgBuild(pReq->Txbuf, EMAIL_MIME_VERSION);
		email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, EMAIL_CONTENT_TYPE);
		email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Subject: Auto Warning: Cold start event from %s\r\n", GCONFIG_GetDeviceName());
		SMTPC_MsgSend(pReq->Txbuf, email_tmp16, MSG_MORE);		
		pReq->MailBuildStatus++;
		break;

	case 1:
		email_tmp16 = SMTPC_MsgBuild(pReq->Txbuf, "Cold start event\r\n");
		SMTPC_MsgSend(pReq->Txbuf, email_tmp16, MSG_MORE);		
		return MSG_BUILD_DONE;
	}
	return MSG_BUILD_BUSY;
} /* End of EMAIL_BuildColdStart() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_SendAuthenFail
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 EMAIL_SendAuthenFail(void)
{
	SMTPC_REQUEST *pReq;
	
	if (GCONFIG_GetAutoWarning() & GCONFIG_SMTP_EVENT_AUTH_FAIL)
	{
 		pReq = SMTPC_AllocateRequest(3);
		if(pReq)
    	{	
			GCONFIG_GetSMTPFrom(pReq->MailBoxFrom);
			pReq->ValidMailBoxToNum = 0;
			GCONFIG_GetSMTPTo1(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo2(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo3(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);	
			pReq->pMailBuildCb = EMAIL_BuildAuthenFail;			
			SMTPC_MailEnqueue(pReq);	
			return 0;
		}
	}
	return -1;
} /* End of EMAIL_SendAuthenFail() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_BuildAuthenFail
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
SMTPC_MSG_BUILD_STATUS EMAIL_BuildAuthenFail(SMTPC_REQUEST *pReq)
{
	email_tmp16 = SMTPC_MsgBuild(pReq->Txbuf, EMAIL_MIME_VERSION);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, EMAIL_CONTENT_TYPE);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Subject: Auto Warning: Authentication fail event from %s\r\n", GCONFIG_GetDeviceName());
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Authentication fail event\r\n");		
	SMTPC_MsgSend(pReq->Txbuf, email_tmp16, MSG_MORE);	
	return MSG_BUILD_DONE;
} /* End of EMAIL_BuildAuthenFail() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_SendIpChanged
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 EMAIL_SendIpChanged(void)
{
	SMTPC_REQUEST *pReq;
	
	if (GCONFIG_GetAutoWarning() & GCONFIG_SMTP_EVENT_IP_CHANGED)
	{
 		pReq = SMTPC_AllocateRequest(3);
		if(pReq)
    	{	
			GCONFIG_GetSMTPFrom(pReq->MailBoxFrom);
			pReq->ValidMailBoxToNum = 0;
			GCONFIG_GetSMTPTo1(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo2(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo3(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);		
			pReq->pMailBuildCb = EMAIL_BuildIpChanged;						
			SMTPC_MailEnqueue(pReq);	
			return 0;
		}
	}
	return -1;
} /* End of EMAIL_SendIpChanged() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_BuildIpChanged
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
SMTPC_MSG_BUILD_STATUS EMAIL_BuildIpChanged(SMTPC_REQUEST *pReq)
{
	email_tmp16 = SMTPC_MsgBuild(pReq->Txbuf, EMAIL_MIME_VERSION);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, EMAIL_CONTENT_TYPE);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Subject: Auto Warning: Local IP changed event from %s\r\n", GCONFIG_GetDeviceName());
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Local IP changed event\r\n");
	SMTPC_MsgSend(pReq->Txbuf, email_tmp16, MSG_MORE);		
	return MSG_BUILD_DONE;
} /* End of EMAIL_BuildIpChanged() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_SendPasswordChanged
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 EMAIL_SendPasswordChanged(void)
{
	SMTPC_REQUEST *pReq;
	
	if (GCONFIG_GetAutoWarning() & GCONFIG_SMTP_EVENT_PSW_CHANGED)
	{
 		pReq = SMTPC_AllocateRequest(3);
		if(pReq)
    	{	
			GCONFIG_GetSMTPFrom(pReq->MailBoxFrom);
			pReq->ValidMailBoxToNum = 0;
			GCONFIG_GetSMTPTo1(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo2(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);
			GCONFIG_GetSMTPTo3(pReq->MailBoxTo[pReq->ValidMailBoxToNum++]);		
			pReq->pMailBuildCb = EMAIL_BuildPasswordChanged;									
			SMTPC_MailEnqueue(pReq);	
			return 0;
		}
	}
	return -1;
} /* End of EMAIL_SendPasswordChanged() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: EMAIL_BuildPasswordChanged
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
SMTPC_MSG_BUILD_STATUS EMAIL_BuildPasswordChanged(SMTPC_REQUEST *pReq)
{
	email_tmp16 = SMTPC_MsgBuild(pReq->Txbuf, EMAIL_MIME_VERSION);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, EMAIL_CONTENT_TYPE);
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Subject: Auto Warning: Password changed event from %s\r\n", GCONFIG_GetDeviceName());
	email_tmp16 += SMTPC_MsgBuild(pReq->Txbuf+email_tmp16, "Password changed event\r\n");
	SMTPC_MsgSend(pReq->Txbuf, email_tmp16, MSG_MORE);		
	return MSG_BUILD_DONE;
} /* End of EMAIL_BuildPasswordChanged() */

/* End of email.c */

