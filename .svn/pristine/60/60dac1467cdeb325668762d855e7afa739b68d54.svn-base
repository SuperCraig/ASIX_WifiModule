/*
 ******************************************************************************
 *     Copyright (c) 2012	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
 /*============================================================================
 * Module Name: httpdap.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */
#define HTTPDAP_DEBUG				0
#define HTTPDAP_RFC2617_ENABLE		0
#define HTTPDAP_TEMP_BUFFER_SIZE	2048

/* INCLUDE FILE DECLARATIONS */
#include "httpdap.h"
#include "httpdapi.h"
#include "filesys.h"
#include <string.h>
#include <stdio.h>
#include "flash_api.h"
#include "autoconf.h"
#include "lwip_netconf.h"
#include "wifi_structures.h"
#include "email.h"
#include "gtcpdat.h"
#include "gudpdat.h"
#include "rtc.h"
#include "timers.h"
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
#include "gnapp.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
#include "bluemix.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
#include "azure.h"
#endif
#include "misc.h"
#include "main.h"
#include "wireless.h"
#include "platform_opts.h"
#include "gs2w.h"
#include "wlan_intf.h"

extern struct netif xnetif[NET_IF_NUM];
extern const s32 security_map[];

extern const U8_T WebWifi_wz[]; //20170726 Craig

/* NAMING CONSTANT DECLARATIONS */
#define HTTPDAP_SAVE_OK			"Save ok!<br/>Please press \'Apply\' to submit configuration and reboot device!"
#define HTTPDAP_SETTING_ERR		"Settings validation error!"
#define HTTPDAP_DEVICE_REBOOT	"Device reboot! please Wait"
#define HTTPDAP_OFF				"off"
#define HTTPDAP_ON				"on"
#define	HTTPDAP_SETTING_BACKUPMODE_ERR	"Network backup setting only support Force Wifi now"
#define HTTPDAP_RESTORE_DEFAULT			"Device restore to default! please Wait"
const u32 HTTPDAP_BAUDRATE[9] = {921600, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200};

#define	INPUT_TEXT		"text"
#define INPUT_HIDDEN	"hidden"

#define	PROPERTY_NAME	"name"
#define PROPERTY_ID		"id"
#define PROPERTY_VALUE	"value"

/* LOCAL VARIABLES DECLARATIONS */
static U8_T *pHttpap_buf;
static U8_T httpdap_tmp8;
static U16_T httpdap_tmp16;
static U32_T httpdap_tmp32;
static U16_T httpdap_AuthenTimerEvent = 0;
HTTPDAP_INFO httpdap;

U8_T HTTP_TmpBuf[HTTPDAP_TEMP_BUFFER_SIZE];
static U8_T httpdap_WepKeyMap[4] = {RECORD_TEXT_key_index_0, RECORD_TEXT_key_index_1, RECORD_TEXT_key_index_2, RECORD_TEXT_key_index_3};
static RTC_TIME httpdap_RtcDateTime;
static TimerHandle_t httpdapTimerHandle = NULL;
static xSemaphoreHandle httpdapMutex;
U8_T httpdap_WifiDone=0, httpdap_WifiScanBuf[HTTPDAP_TEMP_BUFFER_SIZE];
	/* For Azure cloud service */
U8_T Azure_HubName[GCONFIG_MAX_AZURE_HUBNAME_LEN];
U8_T Azure_DeviceID[GCONFIG_MAX_AZURE_DEVICE_ID_LEN];
U8_T Azure_SignedKey[GCONFIG_MAX_AZURE_SIGNED_KEY_LEN];
U8_T Azure_ExpiryTime[GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN];

/* GLOBAL VARIABLES DECLARATIONS */
HTTPD_CALLBACK HTTPDAP_Cbs =
{
	HTTPDAP_Init,
	HTTPDAP_ResponseComplete,
	HTTPDAP_GetMethod,
	HTTPDAP_CgiScript,
	HTTPDAP_PostMethod,
	HTTPDAP_NewConn,
};

/* LOCAL SUBPROGRAM DECLARATIONS */
static void httpdap_Timer(void *pParam);
static u8 httpdap_SetAction(HTTPD_SERVER_CONN *pConn, u8 action, u16 delaySec);
static void httpdap_system_reset(void);
static S16_T httpdap_InputHdrBuild(U8_T *pbuf, U8_T *ptype);
static S16_T httpdap_InputPropertyBuild(U8_T *pbuf, U8_T *pproperty, U8_T *pval, U8_T tag_end);
static S16_T httpdap_TextBuild(U8_T *pbuf, U8_T is_hidden, U8_T *pname, U8_T *pid, U8_T *pvalue);
static HTTPD_STATUS httpdap_WarningPageSend(HTTPD_SERVER_CONN *pconn, U8_T *pmsg, U8_T sec_wait, U8_T *pctrl, U8_T redir_fid);
static HTTPD_STATUS httpdap_ChkPrint(U8_T *pBuf, U16_T Len);
static HTTPD_STATUS httpdap_LoginUserRecord(HTTPD_SERVER_CONN *pConn);
static void httpdap_WifiConn(void *pParam);

HTTPD_STATUS httpdap_SecurityProcess(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetIndexPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostIndexPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetAdvancePage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostAdvancePage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetSecurityPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostSecurityPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetWiFiWizardPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostWiFiWizardPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_GetRtcPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_PostRtcPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_CgiHctentPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS httpdap_RequestCgi(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * void httpdap_Timer()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void httpdap_Timer(void *pParam)
{
	u8 i;
	HTTPDAP_FILE_UPLOAD *pUpload = (HTTPDAP_FILE_UPLOAD*)&httpdap.Upload;
	HTTPDAP_ACTION *pAction = &httpdap.Action;
	int upgrade_status;

	while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
	for (i=0; i<HTTPDAP_MAX_LOGIN_NUM; i++)
	{
		if (httpdap.Login[i].ExpiryTime)
			httpdap.Login[i].ExpiryTime--;
	}
	xSemaphoreGive(httpdapMutex);

	/* Action process */
	if (pAction->Type)
	{
		while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
		if (pAction->DelayTime)
			pAction->DelayTime--;
		xSemaphoreGive(httpdapMutex);

		if (pAction->DelayTime==0)
		{
			switch (pAction->Type & 0x7f)
			{
			case HTTPDAP_REBOOT:
				httpdap_system_reset();
				break;

			case HTTPDAP_RESTORE:
				GCONFIG_ReadDefaultConfigData();
				GCONFIG_WriteConfigData();
				httpdap_system_reset();
				break;

			case HTTPDAP_WIFI_CONNECT:
 				xTaskCreate(httpdap_WifiConn, (u8*)"", 1280, &httpdap.WifiInfo, tskIDLE_PRIORITY + 1, NULL);
				break;

			default:
				break;
			}
			while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
			pAction->Type = 0;
			xSemaphoreGive(httpdapMutex);
		}
	}

	/* Firmware upgrade timeout handle */
	if (pUpload->pConn!=0)
	{
		if (pUpload->Status == UPLOAD_PROGRAMMING)
		{
			upgrade_status = UPGRADE_ImageProgramming(NONBLOCKED_UPGRADE);
			if (upgrade_status == UGSTATUS_ERR_SIGNATURE)
			{
				pUpload->pErrMessage = "Write signature fail!";
				goto httpdap_Timer_Error;
			}
			else if (upgrade_status == UGSTATUS_ERR_WRITE_FLASH)
			{
				pUpload->pErrMessage = "Write flash fail!";
				goto httpdap_Timer_Error;
			}
			else if (upgrade_status < 0)
			{
				goto httpdap_Timer_Error;
			}
			else
			{
				pUpload->WrittenLen	+= upgrade_status;
				if (pUpload->WrittenLen >= (pUpload->FileLen - UPGRADE_ASIX_RT_HDR_SIZE))
				{
					/* Record image file name */
					GCONFIG_SetFilename(pUpload->FileName, pUpload->FileNameLen);
					GCONFIG_WriteConfigData();

					httpdap_SetAction(pUpload->pConn, HTTPDAP_REBOOT, 3);
					httpdap_WarningPageSend(pUpload->pConn, "Firmware upgrade OK! rebooting... please wait", 20, HTTPDAP_ON, pUpload->pInfo->LoginPage);
					pUpload->Status = UPLOAD_DONE;
				}
				else
				{
					while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
					pUpload->Timer = HTTPDAP_FIRMWARE_UPGRADE_TIMEOUT;
					xSemaphoreGive(httpdapMutex);
				}
			}
		}

		while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
		if (pUpload->Timer)
			pUpload->Timer--;
		xSemaphoreGive(httpdapMutex);

		if (pUpload->Timer==0)
		{
			printf("Firmware upload timeout, abort the request!\r\n");
httpdap_Timer_Error:
			pUpload->pConn->State = HTTPD_STATE_FREE;//Close this connection
			memset(pUpload, 0, sizeof(HTTPDAP_FILE_UPLOAD));
			UPGRADE_UnLock();
		}
	}
} /* End of httpdap_Timer() */

/*
 * ----------------------------------------------------------------------------
 * void httpdap_SetAction(void)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static u8 httpdap_SetAction(HTTPD_SERVER_CONN *pConn, u8 action, u16 delaySec)
{
	httpdap.ActionTemp[pConn->Index].Type = action;
	httpdap.ActionTemp[pConn->Index].DelayTime = delaySec;
	return 1;
} /* End of httpdap_SetAction() */

/*
 * ----------------------------------------------------------------------------
 * void httpdap_system_reset(void)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static void httpdap_system_reset(void)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
} /* End of httpdap_system_reset() */

/*
 * ----------------------------------------------------------------------------
 * S16_T httpdap_InputHdrBuild(U8_T *pbuf, U8_T *ptype)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static S16_T httpdap_InputHdrBuild(U8_T *pbuf, U8_T *ptype)
{
	S16_T len;
	len = HTTPAPI_MsgBuild(pbuf, "<input type=\"%s\" ", ptype);
	return len;
} /* End of httpdap_InputHdrBuild() */

/*
 * ----------------------------------------------------------------------------
 * S16_T httpdap_InputPropertyBuild(U8_T *pbuf, U8_T *pproperty, U8_T *pval, U8_T tag_end)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static S16_T httpdap_InputPropertyBuild(U8_T *pbuf, U8_T *pproperty, U8_T *pval, U8_T tag_end)
{
	S16_T len;
	len = HTTPAPI_MsgBuild(pbuf, "%s=\"%s\" %s", pproperty, pval, tag_end ? "/>":"");
	return len;
} /* End of httpdap_InputPropertyBuild() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_WarningPageSend()
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static HTTPD_STATUS httpdap_WarningPageSend(HTTPD_SERVER_CONN *pconn, U8_T *pmsg, U8_T sec_wait, U8_T *pctrl, U8_T redir_fid)
{
	S16_T len;

	len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_SYSMSG);
	len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
//	len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
	len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "sysmsg_setting", 0);
	len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"%s&%d&sec&%s&%s\" />\r\n"
								, pmsg
								, sec_wait
								, pctrl
								, FSYS_Manage[redir_fid].FName
								);
	HTTPAPI_MsgCombineSend(pconn, HTTP_TmpBuf, len, FILE_SYSMSG);
	return HTTPD_OK;
} /* End of httpdap_WarningPageSend() */

/*
 * ----------------------------------------------------------------------------
 * HTTPD_STATUS httpdap_ChkPrint(U8_T *pBuf, U16_T Len)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
static HTTPD_STATUS httpdap_ChkPrint(U8_T *pBuf, U16_T Len)
{
	U16_T i;

	for (i=0; i<Len; i++)
	{
		if (!(pBuf[i]>=0x20 && pBuf[i]<=0x7E))
			return HTTPD_ERR;
	}

	return HTTPD_OK;
} /* End of httpdap_ChkPrint() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_LoginUserRecord()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static HTTPD_STATUS httpdap_LoginUserRecord(HTTPD_SERVER_CONN *pConn)
{
	u16 smallestExpiryTime;
	HTTPDAP_LOGIN *pLogin, *pReleased;

  	smallestExpiryTime = HTTPDAP_LOGIN_EXPIRY_TIME;
	for (pLogin = &httpdap.Login[0]; pLogin!=&httpdap.Login[HTTPDAP_MAX_LOGIN_NUM-1]; pLogin++)
	{
		/* Fine available login user recorder */
		if (pLogin->ExpiryTime == 0)
		{
			while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
			pLogin->ExpiryTime = HTTPDAP_LOGIN_EXPIRY_TIME;
			xSemaphoreGive(httpdapMutex);
			pLogin->UserIP = pConn->Ip;
			break;
		}

		/* Fine the recorder that most seldom use */
		if (smallestExpiryTime > pLogin->ExpiryTime)
		{
			smallestExpiryTime = pLogin->ExpiryTime;
			pReleased = pLogin;
		}
	}

	if (pLogin==&httpdap.Login[HTTPDAP_MAX_LOGIN_NUM-1])
	{
		while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
		pReleased->ExpiryTime = HTTPDAP_LOGIN_EXPIRY_TIME;
		xSemaphoreGive(httpdapMutex);
		pReleased->UserIP = pConn->Ip;
		return HTTPD_ERR;
	}
	return HTTPD_OK;
} /* End of httpdap_LoginUserRecord() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_SecurityProcess()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_SecurityProcess(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
#if HTTPDAP_RFC2617_ENABLE
	U8_T *pUsername, *pPassword, *pbuf1, *pbuf2, i;

	if ((FSYS_Manage[pConn->FileId].FType != FILE_TYPE_HTML) && (FSYS_Manage[pConn->FileId].FType != FILE_TYPE_CGI))
		return HTTPD_OK;

	if (HTTPAPI_GetWwwAuthenBasic(pInfo->pRxBuf, pInfo->RxLen, &pUsername, &pPassword) == HTTPD_OK)
	{
#if 0//HTTPDAP_DEBUG
		printf("HTTPDAP: header is found, user name is %s, password is %s.\r\n", pUsername, pPassword);
#endif
		/* Get uername and password for reference */
		pbuf1 = HTTP_TmpBuf+100;
		pbuf2 = HTTP_TmpBuf+150;
		GCONFIG_GetAdmin(pbuf1, pbuf2, &i);
               
                
		/* Check username and password */
		if (strlen(pUsername)==strlen(pbuf1) && strcmp(pUsername,pbuf1)==0)
		{
			if (strlen(pPassword)==strlen(pbuf2) && strcmp(pPassword,pbuf2)==0)
			{
				httpdap_LoginUserRecord(pConn);
				return HTTPD_OK;
			}
		}
                
                GCONFIG_GetUser(pbuf1, pbuf2, &i);
                if(strlen(pUsername)==strlen(pbuf1) && strcmp(pUsername,pbuf1) == 0){      //20170726 Craig Compare with User' account
                  if(strlen(pPassword)==strlen(pbuf2) && strcmp(pPassword,pbuf2) == 0){
                    httpdap_LoginUserRecord(pConn);
                    return HTTPD_OK;
                  }
                }
	}

	HTTPAPI_UnAuthenRspSend(pConn, HTTP_TmpBuf, "Please enter username and password");
	return HTTPD_ERR;

#else
	HTTPDAP_LOGIN *pLogin;

	if (pConn->FileId == pInfo->LoginPage)
		return HTTPD_OK;

	if ((FSYS_Manage[pConn->FileId].FType != FILE_TYPE_HTML) && (FSYS_Manage[pConn->FileId].FType != FILE_TYPE_CGI))
		return HTTPD_OK;

	/* Check access authority */
	for (pLogin=&httpdap.Login[0]; pLogin!=&httpdap.Login[HTTPDAP_MAX_LOGIN_NUM-1]; pLogin++)
	{
		if ((pLogin->ExpiryTime != 0) && (pConn->Ip == pLogin->UserIP))
		{
			while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
			pLogin->ExpiryTime = HTTPDAP_LOGIN_EXPIRY_TIME; //Update expiry time
			xSemaphoreGive(httpdapMutex);
			return HTTPD_OK;
		}
	}

	/* Not authenticated yet */
	HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, pInfo->LoginPage);
	return HTTPD_ERR;
#endif
} /* httpdap_SecurityProcess() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_GetIndexPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetIndexPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
#if HTTPDAP_RFC2617_ENABLE

	/* Pop out logout page for prompting */
	httpdap_tmp16 = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_SYSMSG);
	httpdap_tmp16 += httpdap_InputHdrBuild(HTTP_TmpBuf+httpdap_tmp16, INPUT_HIDDEN);
	httpdap_tmp16 += httpdap_InputPropertyBuild(HTTP_TmpBuf+httpdap_tmp16, PROPERTY_NAME, "", 0);
	httpdap_tmp16 += httpdap_InputPropertyBuild(HTTP_TmpBuf+httpdap_tmp16, PROPERTY_ID, "sysmsg_setting", 0);
	httpdap_tmp16 += httpdap_InputPropertyBuild(HTTP_TmpBuf+httpdap_tmp16, PROPERTY_VALUE, "Logout", 1);
	HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, httpdap_tmp16, FILE_SYSMSG);
	return HTTPD_OK;
#else
	HTTPDAP_LOGIN *pLogin;

	for (pLogin=&httpdap.Login[0]; pLogin!=&httpdap.Login[HTTPDAP_MAX_LOGIN_NUM-1]; pLogin++)
	{
		if (pLogin->UserIP== pConn->Ip)
		{
			while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
			pLogin->ExpiryTime = 0;
			xSemaphoreGive(httpdapMutex);
			break;
		}
	}
	HTTPAPI_SetElementValueById(RECORD_TEXT_username, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_password, 0, 0);
	return HTTPD_OK;
#endif
} /* End of httpdap_GetIndexPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_PostIndexPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostIndexPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U8_T i, j;
	U8_T *pTmp, *pbuf1, *pbuf2;     //20170725 Craig pbuf1 for username pbuf2 for password 

	if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_username) && HTTPAPI_ElementUpdateChkById(RECORD_PASSWORD_password))
	{
		/* Get uername and password for reference */
		pbuf1 = HTTP_TmpBuf;
		pbuf2 = HTTP_TmpBuf + 100;
                
		GCONFIG_GetAdmin(pbuf1, pbuf2, &i);
                
                
		/* Check username */
		i = HTTPAPI_GetElementValueById(RECORD_TEXT_username, &pTmp);
		j = strlen(pbuf1);
		if (i != j || memcmp(pbuf1, pTmp, j))
		{       
			goto GOTO_CHECK_USER_ACCOUNT;
		}
                
                
		/* Check password */
		i =	HTTPAPI_GetElementValueById(RECORD_PASSWORD_password, &pTmp);
		j = strlen(pbuf2);
		if (i != j || memcmp(pbuf2, pTmp, j))
		{
			goto GOTO_CHECK_USER_ACCOUNT;
		}
		httpdap_LoginUserRecord(pConn);
		HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, pInfo->HomePage);
		return HTTPD_OK;
	}
        
GOTO_CHECK_USER_ACCOUNT:        //Craig For checking another account
  GCONFIG_GetUser(pbuf1,pbuf2,&i);
  
  /* Check username */
  i = HTTPAPI_GetElementValueById(RECORD_TEXT_username, &pTmp);
  j = strlen(pbuf1);
  if(i != j || memcmp(pbuf1, pTmp, j)){
    goto HTTPDAP_INDEX_POST_ERR;
  }
  
  /* Check password */
  i = HTTPAPI_GetElementValueById(RECORD_PASSWORD_password, &pTmp);
  j = strlen(pbuf2);
  if(i != j || memcmp(pbuf2, pTmp, j)){
    goto HTTPDAP_INDEX_POST_ERR;
  }
  httpdap_LoginUserRecord(pConn);
  HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, pInfo->HomePage);

  return HTTPD_OK;
  
HTTPDAP_INDEX_POST_ERR:
	EMAIL_SendAuthenFail();
        
        httpdap_WarningPageSend(pConn, "Username or Password Typed Error(MBI)!", 2, HTTPDAP_OFF, pConn->FileId);        //20170725 Craig Error Msg for Username and PSW Error
	
        return HTTPD_OK;
} /* End of httpdap_PostIndexPage() */

/*
 * ----------------------------------------------------------------------------
 * HTTPD_STATUS httpdap_GetBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
  	U32_T tmp32;

/* ---- Serial Settings ---- */
	/* Load device name */
	pHttpap_buf = (U8_T *)(GCONFIG_GetDeviceName());
	httpdap_tmp8 = strlen(pHttpap_buf);
	if (httpdap_tmp8 > 16)
		httpdap_tmp8 = 16;
	HTTPAPI_SetElementValueById(RECORD_TEXT_dsm_name, pHttpap_buf, httpdap_tmp8);

	/* Get baudrate */
	tmp32 = GCONFIG_GetUrBaudRate();
	for (httpdap_tmp8 = 0; httpdap_tmp8 < 9; httpdap_tmp8++)
	{
		if (HTTPDAP_BAUDRATE[httpdap_tmp8] == tmp32)
		{
			HTTPAPI_SetElementValueById(RECORD_SELECT_dbr, &httpdap_tmp8, 1);
			break;
		}
	}

	/* Get data bit */
	httpdap_tmp8 = GCONFIG_GetUrDataBits() - 7;// Minimal data bits is 7
	HTTPAPI_SetElementValueById(RECORD_SELECT_data, &httpdap_tmp8, 1);

	/* Get parity */
	httpdap_tmp8 = GCONFIG_GetUrParity();
	HTTPAPI_SetElementValueById(RECORD_SELECT_parity, &httpdap_tmp8, 1);

	/* Get stop bit */
	httpdap_tmp8 = (GCONFIG_GetUrStopBits()==2) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_stop, &httpdap_tmp8, 1);

	/* Get flow control */
	httpdap_tmp8 = GCONFIG_GetUrFlowCtrl();
	HTTPAPI_SetElementValueById(RECORD_SELECT_flow, &httpdap_tmp8, 1);

	/* Get RS485 status */
	httpdap_tmp8 = GCONFIG_GetRs485Status();
	HTTPAPI_SetElementValueById(RECORD_SELECT_rs485, &httpdap_tmp8, 1);
/* ---- Serial to Network Settings ---- */
	httpdap_tmp16 = GCONFIG_GetNetwork();

	/* Get serial to network mode */
	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_NETWORK_R2WMODEMASK);
	if (httpdap_tmp8 >= GCONFIG_MAX_R2W_MODE)
		httpdap_tmp8 = R2WMODE_Socket;
	HTTPAPI_SetElementValueById(RECORD_SELECT_s2n_mode, &httpdap_tmp8, 1);

	/* Load Connection type */
	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_NETWORK_PROTO_UDP) ? 1 : 0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_conntype, &httpdap_tmp8, 1);

	/* Load Connection role */
	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_NETWORK_CLIENT) ? 1 : 0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_cs_mode, &httpdap_tmp8, 1);

	/* Load transmit timer */
	httpdap_tmp16 = GCONFIG_GetEthernetTxTimer();
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_txtimer, HTTP_TmpBuf, httpdap_tmp8);

	/* Load server listen port */
	httpdap_tmp16 = GCONFIG_GetDeviceDataPktListenPort();
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_s_lstport, HTTP_TmpBuf, httpdap_tmp8);

	/* Load Destination host name */
	httpdap_tmp8 = GCONFIG_GetDestHostName(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_c_deshn, HTTP_TmpBuf, httpdap_tmp8);

	/* Load destination port */
	httpdap_tmp16 = GCONFIG_GetClientDestPort();
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_c_desport, HTTP_TmpBuf, httpdap_tmp8);

	/* Load R2W maximum connections */
	httpdap_tmp8 = GCONFIG_GetR2wMaxConnections();
	if (httpdap_tmp8)
		httpdap_tmp8--;
	HTTPAPI_SetElementValueById(RECORD_SELECT_r2w_conns, &httpdap_tmp8, 1);

	/* Load Auto/Manual Connection mode */
	httpdap_tmp8 = (GCONFIG_GetClientConnectionMode()==GCONFIG_AUTO_CONNECTION ? 0:1);
	HTTPAPI_SetElementValueById(RECORD_SELECT_cliconnmode, &httpdap_tmp8, 1);

/* ---- Statis IP Settings ---- */
	/* Load static IP address */
	httpdap_tmp32 = htonl(GCONFIG_GetDeviceStaticIP());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_static_ip, HTTP_TmpBuf, httpdap_tmp8);

	/* Load static subnet mask */
	httpdap_tmp32 = htonl(GCONFIG_GetNetmask());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_mask, HTTP_TmpBuf, httpdap_tmp8);

	/* Load static gateway */
	httpdap_tmp32 = htonl(GCONFIG_GetGateway());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_gateway_ip, HTTP_TmpBuf, httpdap_tmp8);

	/* Load static DNS */
	httpdap_tmp32 = htonl(GCONFIG_GetDNS());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_dns_ip, HTTP_TmpBuf, httpdap_tmp8);

/* ---- DHCP Settings ---- */
	httpdap_tmp16 = GCONFIG_GetNetwork();

	/* Load DHCP client status */
	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_NETWORK_DHCP_ENABLE) ? 1 : 0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_dhcpc, &httpdap_tmp8, 1);

	/* Load DHCP server status */
	httpdap_tmp8 = GCONFIG_GetDhcpSrvStatus() ? 1 : 0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_dhcps, &httpdap_tmp8, 1);

	/* Load DHCP server IP pool start address */
	httpdap_tmp32 = htonl(GCONFIG_GetDhcpSrvStartIp());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_dhcps_startip, HTTP_TmpBuf, httpdap_tmp8);

	/* Load DHCP server IP pool end address */
	httpdap_tmp32 = htonl(GCONFIG_GetDhcpSrvEndIp());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_dhcps_endip, HTTP_TmpBuf, httpdap_tmp8);

	/* Load DHCP server lease time */
	httpdap_tmp16 = GCONFIG_GetDhcpSrvLeaseTime();
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_dhcps_lt, HTTP_TmpBuf, httpdap_tmp8);

	/* Load DHCP server gateway */
	httpdap_tmp32 = htonl(GCONFIG_GetDhcpSrvDefGateway());
	httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_dhcps_gw, HTTP_TmpBuf, httpdap_tmp8);

	return HTTPD_OK;
} /* End of httpdap_GetBasicPage() */

/*
 * ----------------------------------------------------------------------------
 * HTTPD_STATUS httpdap_PostBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostBasicPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U16_T len;

	if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_dsm_name) && HTTPAPI_ElementUpdateChkById(RECORD_TEXT_dhcps_lt))
	{
/* ---- Serial Settings ---- */
		/* Save device name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dsm_name, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		memset(HTTP_TmpBuf+httpdap_tmp8, 0 , 16-httpdap_tmp8);// To fill the unused data with terminal char('\0').
		GCONFIG_SetDeviceName((GCONFIG_DEV_NAME*)HTTP_TmpBuf);

		/* Set baudrate */
		HTTPAPI_GetElementValueById(RECORD_SELECT_dbr, &pHttpap_buf);
		if (pHttpap_buf[0] < 9)
		{
			GCONFIG_SetUrBaudRate(HTTPDAP_BAUDRATE[pHttpap_buf[0]]);
		}

		/* Set data bit */
		HTTPAPI_GetElementValueById(RECORD_SELECT_data, &pHttpap_buf);
		GCONFIG_SetUrDataBits(pHttpap_buf[0] + 7);// Minimal data bits is 7

		/* Set parity */
		HTTPAPI_GetElementValueById(RECORD_SELECT_parity, &pHttpap_buf);
		GCONFIG_SetUrParity(pHttpap_buf[0]);

		/* Set stop bit */
		HTTPAPI_GetElementValueById(RECORD_SELECT_stop, &pHttpap_buf);
		GCONFIG_SetUrStopBits(pHttpap_buf[0] + 1);// offset = 1

		/* Set flow control */
		HTTPAPI_GetElementValueById(RECORD_SELECT_flow, &pHttpap_buf);
		GCONFIG_SetUrFlowCtrl(pHttpap_buf[0]);

		/* Set RS485 status */
		HTTPAPI_GetElementValueById(RECORD_SELECT_rs485, &pHttpap_buf);
		GCONFIG_SetRs485Status(pHttpap_buf[0]);
/* ---- Serial to Network Settings ---- */
		httpdap_tmp16 = GCONFIG_GetNetwork();

		/* Save serial to network mode */
		HTTPAPI_GetElementValueById(RECORD_SELECT_s2n_mode, &pHttpap_buf);
		if (*pHttpap_buf >= GCONFIG_MAX_R2W_MODE)
			*pHttpap_buf = R2WMODE_Socket;
		httpdap_tmp16 = (httpdap_tmp16 & (~GCONFIG_NETWORK_R2WMODEMASK)) | (*pHttpap_buf);

		/* Save connection type */
		HTTPAPI_GetElementValueById(RECORD_SELECT_conntype, &pHttpap_buf);
		httpdap_tmp16 = (httpdap_tmp16 & 0xF3FF) | ((*pHttpap_buf)? GCONFIG_NETWORK_PROTO_UDP:GCONFIG_NETWORK_PROTO_TCP);

		/* Save connection role */
		HTTPAPI_GetElementValueById(RECORD_SELECT_cs_mode, &pHttpap_buf);
		httpdap_tmp16 = (httpdap_tmp16 & 0x7FFF) | ((*pHttpap_buf)? GCONFIG_NETWORK_CLIENT:GCONFIG_NETWORK_SERVER);

		GCONFIG_SetNetwork(httpdap_tmp16);

		/* Save transmit timer */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_txtimer, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
		GCONFIG_SetEthernetTxTimer(httpdap_tmp16);

		/* Save server listen port */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_s_lstport, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
		GCONFIG_SetDeviceDataPktListenPort(httpdap_tmp16);

		/* Save destination port */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_c_desport, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
		GCONFIG_SetClientDestPort(httpdap_tmp16);

		/* Save Destination host name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_c_deshn, &pHttpap_buf);
		GCONFIG_SetDestHostName(pHttpap_buf, httpdap_tmp8);

		/* Save R2W maximum connections */
		HTTPAPI_GetElementValueById(RECORD_SELECT_r2w_conns, &pHttpap_buf);
		GCONFIG_SetR2wMaxConnections(pHttpap_buf[0] + 1);// offset = 1

		/* Save Auto/Manual Connection mode */
		HTTPAPI_GetElementValueById(RECORD_SELECT_cliconnmode, &pHttpap_buf);
		httpdap_tmp8 = (*pHttpap_buf)? GCONFIG_MANUAL_CONNECTION:GCONFIG_AUTO_CONNECTION;
		GCONFIG_SetClientConnectionMode(httpdap_tmp8);

/* ---- Statis IP Settings ---- */
		/* Save IP address */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_static_ip, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetDeviceStaticIP(httpdap_tmp32);

		/* Save subnet mask */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_mask, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetNetmask(httpdap_tmp32);

		/* Save gateway */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_gateway_ip, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetGateway(httpdap_tmp32);

		/* Save DNS */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dns_ip, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetDNS(httpdap_tmp32);

/* ---- DHCP Settings ---- */
		httpdap_tmp16 = GCONFIG_GetNetwork();

		/* Save DHCP client status */
		HTTPAPI_GetElementValueById(RECORD_SELECT_dhcpc, &pHttpap_buf);
		httpdap_tmp16 = (httpdap_tmp16 & 0xBFFF) | ((*pHttpap_buf)? GCONFIG_NETWORK_DHCP_ENABLE:GCONFIG_NETWORK_DHCP_DISABLE);
		GCONFIG_SetNetwork(httpdap_tmp16);

		/* Save DHCP server status */
		HTTPAPI_GetElementValueById(RECORD_SELECT_dhcps, &pHttpap_buf);
		GCONFIG_SetDhcpSrvStatus(pHttpap_buf[0]);

		/* Save DHCP server IP pool start address */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dhcps_startip, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetDhcpSrvStartIp(httpdap_tmp32);

		/* Save DHCP server IP pool end address */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dhcps_endip, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetDhcpSrvEndIp(httpdap_tmp32);

		/* Save DHCP server lease time */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dhcps_lt, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
		GCONFIG_SetDhcpSrvLeaseTime(httpdap_tmp16);

		/* Save DHCP server gateway */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_dhcps_gw, &pHttpap_buf);
		httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
		GCONFIG_SetDhcpSrvDefGateway(httpdap_tmp32);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_BASIC_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_BASIC_APPLY;
		}
	}

HTTPDAP_POST_BASIC_ERR:
	httpdap_WarningPageSend(pConn, HTTPDAP_SETTING_ERR, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_BASIC_SAVE:
	/* Save configuration only */
	GCONFIG_WriteConfigData();
	httpdap_WarningPageSend(pConn, HTTPDAP_SAVE_OK, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_OK;

HTTPDAP_POST_BASIC_APPLY:
	/* Save configuration and system reboot */
	GCONFIG_WriteConfigData();
	httpdap_SetAction(pConn, HTTPDAP_REBOOT, 3);
	httpdap_WarningPageSend(pConn, HTTPDAP_DEVICE_REBOOT, 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;
} /* End of httpdap_PostBasicPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_GetAdvancePage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetAdvancePage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	u16 tmp16[2];
	u8 XferMode, SlaveId;

	/* Load SMTP Domain name */
	httpdap_tmp8 = GCONFIG_GetSMTPDomainName(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_email_addr, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SMTP security type & server port */
	GCONFIG_GetSMTPSecurity(&httpdap_tmp8, &httpdap_tmp16);
	HTTPAPI_SetElementValueById(RECORD_SELECT_smtp_security, &httpdap_tmp8, 1);

	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_smtp_port, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SMTP From addr */
	httpdap_tmp8 = GCONFIG_GetSMTPFrom(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_email_from, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SMTP To1 addr */
	httpdap_tmp8 = GCONFIG_GetSMTPTo1(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_email_to1, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SMTP To2 addr */
	httpdap_tmp8 = GCONFIG_GetSMTPTo2(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_email_to2, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SMTP To3 addr */
	httpdap_tmp8 = GCONFIG_GetSMTPTo3(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_email_to3, HTTP_TmpBuf, httpdap_tmp8);

	/* Load Auto Warning enable bits */
	httpdap_tmp16 = GCONFIG_GetAutoWarning();

	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_SMTP_EVENT_COLDSTART) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_coldstart, &httpdap_tmp8, 1);

	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_SMTP_EVENT_AUTH_FAIL) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_authfail, &httpdap_tmp8, 1);

	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_SMTP_EVENT_IP_CHANGED) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_ipchg, &httpdap_tmp8, 1);

	httpdap_tmp8 = (httpdap_tmp16 & GCONFIG_SMTP_EVENT_PSW_CHANGED) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_pswchg, &httpdap_tmp8, 1);

	GCONFIG_GetModbusTcp(&httpdap_tmp16, &SlaveId, &XferMode);
	/* Load Modbus TCP transfer mode */
	HTTPAPI_SetElementValueById(RECORD_SELECT_mbtcp_xfer, &XferMode, 1);

	/* Load Modbus TCP server port */
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_mbtcp_port, HTTP_TmpBuf, httpdap_tmp8);

	GCONFIG_GetModbusSerialTiming(&httpdap_tmp16, &tmp16[0], &tmp16[1]);
	/* Load Modbus serial response time */
	httpdap_tmp8 = HTTPAPI_Short2Str(httpdap_tmp16, HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_mb_rspTime, HTTP_TmpBuf, httpdap_tmp8);

	/* Load Modbus serial inter-frame delay */
	httpdap_tmp8 = HTTPAPI_Short2Str(tmp16[0], HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_mb_frameIntv, HTTP_TmpBuf, httpdap_tmp8);

	/* Load Modbus serial inter-character delay */
	httpdap_tmp8 = HTTPAPI_Short2Str(tmp16[1], HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_mb_CharDelay, HTTP_TmpBuf, httpdap_tmp8);

	/* Network Backup Setting */
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	httpdap_tmp8 = GCONFIG_GetNetworkBackupMode();
	HTTPAPI_SetElementValueById(RECORD_TEXT_hideBackupMode, "1", 1);
#else
	httpdap_tmp8 = GCONFIG_NETWORK_BACKUP_FORCE_WIFI;
	HTTPAPI_SetElementValueById(RECORD_TEXT_hideBackupMode, "0", 1);
#endif
	HTTPAPI_SetElementValueById(RECORD_SELECT_netback_mode, &httpdap_tmp8, 1);

	return HTTPD_OK;
} /* End of httpdap_GetAdvancePage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_PostAdvancePage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostAdvancePage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U16_T off, len, tmp16[2];
	U8_T tmp8, *pbuf, i, *pbuftmp, SlaveId, XferMode;
	HTTPDAP_FILE_UPLOAD	*pUpload = &httpdap.Upload;
	int upgrade_status;

	if (pConn->ContentType == CTYPE_MULTIPART_FORM_DATA)
	{
		if (pConn->State==HTTPD_STATE_ACTIVE)
		{
			/* Check firmware upgarde section is available */
			if (UPGRADE_Lock()==0)
			{
				printf("Firmware upgrading is conflicting!\r\n");
				HTTPAPI_IncompRspSend(pConn, HTTPD_Info.pTxBuf, STATUS_403_FORBIDDEN);
				return HTTPD_OK;
			}
			pUpload->pConn = pConn;
			pUpload->pInfo = pInfo;
		}
		else if (pConn->State==HTTPD_STATE_SEND_NONE)
		{
			/* Waiting for all packets transmission done */
			if (pInfo->pRxBuf[pInfo->RxLen-4]=='-' &&
				pInfo->pRxBuf[pInfo->RxLen-3]=='-' &&
				pInfo->pRxBuf[pInfo->RxLen-2]=='\r' &&
				pInfo->pRxBuf[pInfo->RxLen-1]=='\n')
			{
				/* Firmware uploading error and terminated */
				httpdap_WarningPageSend(pConn, pUpload->pErrMessage, 2, HTTPDAP_OFF, pConn->FileId);
				memset(pUpload, 0, sizeof(HTTPDAP_FILE_UPLOAD));
			}
			return HTTPD_OK;
		}
		else if (pConn->State!=HTTPD_STATE_RX_DATA)
		{
			return HTTPD_OK;
		}

		pbuf = pInfo->pRxBuf;
		while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
		pUpload->Timer = HTTPDAP_FIRMWARE_UPGRADE_TIMEOUT;
		xSemaphoreGive(httpdapMutex);

		switch (pUpload->Status)
		{
		case UPLOAD_IDLE:
			pConn->State = HTTPD_STATE_RX_DATA;
			/* Find file entity section */
			pbuftmp = HTTPAPI_GetHdrValue(pbuf, pInfo->RxLen, "name=\"img_path\"; filename=", &len);
			if (pbuftmp==0)
			{
				return HTTPD_OK;
			}

			/* Record image file name */
			len -= 2;//Subtract two bytes of double quotes
			if (len >= GCONFIG_MAX_FILENAME_LEN)
		  		len = GCONFIG_MAX_FILENAME_LEN-1;
			pbuftmp += 1;//Skip single quote
			memcpy(pUpload->FileName, pbuftmp, len);
			pUpload->FileName[len] = '\0';
			pUpload->FileNameLen = len;

			pInfo->RxLen -= pbuftmp-pbuf;
			pbuf = pbuftmp;

			/* Pointer to file entity start address */
			if (HTTPAPI_FindStr(pbuf, pInfo->RxLen, "\r\n\r\n", &off)==0)
			{
				pInfo->RxLen = 0;
			}
			else
			{
  				off += 4;//Skip "\r\n\r\n"
				pInfo->RxLen -= off;
				pbuf += off;
			}
			pUpload->Status = UPLOAD_ENTITY_START;

		case UPLOAD_ENTITY_START:
			if (pInfo->RxLen==0)
			{
				return HTTPD_OK;
			}

			/* Backup file header */
			UPGRADE_InitImageBufDesc();
			upgrade_status = UPGRADE_CheckFileHeader(pbuf, pInfo->RxLen);
			if (upgrade_status == UGSTATUS_ERR_SIGNATURE)
			{
				pUpload->pErrMessage = "Invalid image file!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status == UGSTATUS_ERR_FILE_SIZE)
			{
				pUpload->pErrMessage = "Over maximum OTA file length!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status == UGSTATUS_ERR_IMAGE_SIZE)
			{
				pUpload->pErrMessage = "Unreasonable file length!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status == UGSTATUS_ERR_TARGET_ADDR)
			{
				pUpload->pErrMessage = "Wrong upgrade image 2 address!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status == UGSTATUS_ERR_IMAGE_TYPE)
			{
				pUpload->pErrMessage = "Invalid file type!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status < 0)
			{
				pUpload->pErrMessage = "Init image buffer error!";
				goto PostAdvancePage_UploadFail;
			}
			pUpload->FileLen = upgrade_status;
			pUpload->CopiedLen = 0;
			pUpload->Status = UPLOAD_RECV_FILE;

		case UPLOAD_RECV_FILE:
			if ((pUpload->CopiedLen + pInfo->RxLen) > pUpload->FileLen)
			{
				pInfo->RxLen = pUpload->FileLen - pUpload->CopiedLen;
			}
			upgrade_status = UPGRADE_CopyToImageBuf(pbuf, pInfo->RxLen);
			if (upgrade_status == UGSTATUS_ERR_IMAGE_SIZE)
			{
				pUpload->pErrMessage = "Unreasonable file length!";
				goto PostAdvancePage_UploadFail;
			}
			else if (upgrade_status < 0)
			{
				pUpload->pErrMessage = "Fail to copy image to buffer!";
				goto PostAdvancePage_UploadFail;
			}
			else
			{
				pUpload->CopiedLen += upgrade_status;
				if (pUpload->CopiedLen < pUpload->FileLen)
				{
					return HTTPD_OK;
				}
			}

			if (UPGRADE_IntegrityCheck() != UGSTATUS_OK)
			{
				pUpload->pErrMessage = "Integrity check fail!";
				goto PostAdvancePage_UploadFail;
			}
			pUpload->Status = UPLOAD_PROGRAMMING;

		default:
			return HTTPD_OK;
		}

PostAdvancePage_UploadFail:
		pUpload->Status = UPLOAD_FAIL;
		pConn->State = HTTPD_STATE_SEND_NONE;
		UPGRADE_UnLock();
		return HTTPD_ERR;
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_email_addr) &&
		HTTPAPI_ElementUpdateChkById(RECORD_SELECT_pswchg))
	{
		/* Save SMTP Domain name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_email_addr, &pHttpap_buf);
		GCONFIG_SetSMTPDomainName(pHttpap_buf, httpdap_tmp8);

		/* Save SMTP security type & server port */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_SELECT_smtp_security, &pHttpap_buf);
		tmp8 = *pHttpap_buf;

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_smtp_port, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
		GCONFIG_SetSMTPSecurity(tmp8, httpdap_tmp16);

		/* Save SMTP From addr */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_email_from, &pHttpap_buf);
		GCONFIG_SetSMTPFrom(pHttpap_buf, httpdap_tmp8);

		/* Save SMTP To1 addr */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_email_to1, &pHttpap_buf);
		GCONFIG_SetSMTPTo1(pHttpap_buf, httpdap_tmp8);

		/* Save SMTP To2 addr */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_email_to2, &pHttpap_buf);
		GCONFIG_SetSMTPTo2(pHttpap_buf, httpdap_tmp8);

		/* Save SMTP To3 addr */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_email_to3, &pHttpap_buf);
		GCONFIG_SetSMTPTo3(pHttpap_buf, httpdap_tmp8);

		/* Save Auto Warning enable bits */
		httpdap_tmp16 = GCONFIG_GetAutoWarning();
		HTTPAPI_GetElementValueById(RECORD_SELECT_coldstart, &pHttpap_buf);
		if (*pHttpap_buf)
			httpdap_tmp16 |= GCONFIG_SMTP_EVENT_COLDSTART;
		else
			httpdap_tmp16 &= (~GCONFIG_SMTP_EVENT_COLDSTART);

		HTTPAPI_GetElementValueById(RECORD_SELECT_authfail, &pHttpap_buf);
		if (*pHttpap_buf)
			httpdap_tmp16 |= GCONFIG_SMTP_EVENT_AUTH_FAIL;
		else
			httpdap_tmp16 &= (~GCONFIG_SMTP_EVENT_AUTH_FAIL);

		HTTPAPI_GetElementValueById(RECORD_SELECT_ipchg, &pHttpap_buf);
		if (*pHttpap_buf)
			httpdap_tmp16 |= GCONFIG_SMTP_EVENT_IP_CHANGED;
		else
			httpdap_tmp16 &= (~GCONFIG_SMTP_EVENT_IP_CHANGED);

		HTTPAPI_GetElementValueById(RECORD_SELECT_pswchg, &pHttpap_buf);
		if (*pHttpap_buf)
			httpdap_tmp16 |= GCONFIG_SMTP_EVENT_PSW_CHANGED;
		else
			httpdap_tmp16 &= (~GCONFIG_SMTP_EVENT_PSW_CHANGED);
		GCONFIG_SetAutoWarning(httpdap_tmp16);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_ADVANCE_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_ADVANCE_APPLY;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_SELECT_mbtcp_xfer) &&
		HTTPAPI_ElementUpdateChkById(RECORD_TEXT_mbtcp_port))
	{
		GCONFIG_GetModbusTcp(&httpdap_tmp16, &SlaveId, &XferMode);
		/* Save Modbus transfer mode */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_SELECT_mbtcp_xfer, &pHttpap_buf);
		XferMode = *pHttpap_buf;

		/* Save Modebus Server port */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_mbtcp_port, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);

		GCONFIG_SetModbusTcp(httpdap_tmp16, SlaveId, XferMode);

		/* Save Modbus serial response time */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_mb_rspTime, &pHttpap_buf);
		httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);

		/* Save Modbus serial inter-frame delay */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_mb_frameIntv, &pHttpap_buf);
		tmp16[0] = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);

		/* Save Modbus serial inter-character delay */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_mb_CharDelay, &pHttpap_buf);
		tmp16[1] = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);

		GCONFIG_SetModbusSerialTiming(httpdap_tmp16, tmp16[0], tmp16[1]);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_ADVANCE_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_ADVANCE_APPLY;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_SELECT_netback_mode))
	{
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
/* ---- Network Backup Setting ---- */
		/* Save Network Backup Mode */
		HTTPAPI_GetElementValueById(RECORD_SELECT_netback_mode, &pHttpap_buf);

		GCONFIG_SetNetworkBackupMode(pHttpap_buf[0]);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_ADVANCE_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_ADVANCE_APPLY;
		}
#else
		goto HTTPDAP_POST_ADVANCE_ERR;
#endif
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_reboot))
	{
		/* Device reboot process */
		goto HTTPDAP_POST_ADVANCE_REBOOT;
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_restore))
	{
		/* Device restore process */
		goto HTTPDAP_POST_ADVANCE_RESTORE;
	}

HTTPDAP_POST_ADVANCE_ERR:
	httpdap_WarningPageSend(pConn, HTTPDAP_SETTING_ERR, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_ADVANCE_SAVE:
	/* Save configuration only */
	GCONFIG_WriteConfigData();
	httpdap_WarningPageSend(pConn, HTTPDAP_SAVE_OK, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_OK;

HTTPDAP_POST_ADVANCE_APPLY:
	/* Save configuration and system reboot */
	GCONFIG_WriteConfigData();
HTTPDAP_POST_ADVANCE_REBOOT:
	httpdap_SetAction(pConn, HTTPDAP_REBOOT, 3);
	httpdap_WarningPageSend(pConn, HTTPDAP_DEVICE_REBOOT, 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;

HTTPDAP_POST_ADVANCE_RESTORE:
	httpdap_SetAction(pConn, HTTPDAP_RESTORE, 3);
	httpdap_WarningPageSend(pConn, "Device restore to default! please Wait", 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;
} /* End of httpdap_PostAdvancePage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_GetSecurityPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetSecurityPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	GCONFIG_BLUEMIX *pClientId;
	GCONFIG_BLUEMIX_AR *pBMAR;

	/* Clear new user and password */
	HTTPAPI_SetElementValueById(RECORD_TEXT_new_usn, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_old_psw, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_new_psw, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_cfm_psw, 0, 0);

	HTTPAPI_SetElementValueById(RECORD_TEXT_smtp_un, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_smtp_pw, 0, 0);

	/* Load Google nest information */
	HTTPAPI_SetElementValueById(RECORD_TEXT_gn_host, 0, 0);

	/* Load Bluemix server hostname */
	GCONFIG_GetBluemixHostName(HTTP_TmpBuf, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN);
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_host, HTTP_TmpBuf, strlen(HTTP_TmpBuf));

	/* Load Bluemix device ID and username/password */
	pClientId = (GCONFIG_BLUEMIX*)HTTP_TmpBuf;
	GCONFIG_GetBluemixAccount(pClientId->OrganizationID, sizeof(pClientId->OrganizationID)
								, pClientId->TypeID, sizeof(pClientId->TypeID)
								, pClientId->DeviceID, sizeof(pClientId->DeviceID)
								, pClientId->UserName, sizeof(pClientId->UserName)
								, pClientId->PassWord, sizeof(pClientId->PassWord));

	/* Show on WEB page */
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_un, pClientId->UserName, strlen(pClientId->UserName));
	HTTPAPI_SetElementValueById(RECORD_PASSWORD_bm_pw, 0, 0);
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_orgid, pClientId->OrganizationID, strlen(pClientId->OrganizationID));
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_typid, pClientId->TypeID, strlen(pClientId->TypeID));
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_devid, pClientId->DeviceID, strlen(pClientId->DeviceID));

	/* Load Bluemix auto registered information */
	pBMAR = (GCONFIG_BLUEMIX_AR*)HTTP_TmpBuf;
	GCONFIG_GetBluemixAutoReg(pBMAR->ApiVersion, sizeof(pBMAR->ApiVersion)
								, pBMAR->ApiKey, sizeof(pBMAR->ApiKey)
								, pBMAR->ApiToken, sizeof(pBMAR->ApiToken));
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_apiver, pBMAR->ApiVersion, strlen(pBMAR->ApiVersion));
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_apikey, pBMAR->ApiKey, strlen(pBMAR->ApiKey));
	HTTPAPI_SetElementValueById(RECORD_TEXT_bm_apitoken, pBMAR->ApiToken, strlen(pBMAR->ApiToken));

	/* Load Azure information */
	if (GCONFIG_GetAzureHostName(HTTP_TmpBuf, GCONFIG_MAX_AZURE_HOSTNAME_LEN))
	{
		HTTPAPI_SetElementValueById(RECORD_TEXT_az_host, HTTP_TmpBuf, strlen(HTTP_TmpBuf));
	}

	if (GCONFIG_GetAzureAccount(Azure_HubName, GCONFIG_MAX_AZURE_HUBNAME_LEN,
									Azure_DeviceID, GCONFIG_MAX_AZURE_DEVICE_ID_LEN,
									Azure_SignedKey, GCONFIG_MAX_AZURE_SIGNED_KEY_LEN,
									Azure_ExpiryTime, GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN))
	{
		HTTPAPI_SetElementValueById(RECORD_TEXT_az_hn, Azure_HubName, strlen(Azure_HubName));
		HTTPAPI_SetElementValueById(RECORD_TEXT_az_di, Azure_DeviceID, strlen(Azure_DeviceID));
		HTTPAPI_SetElementValueById(RECORD_PASSWORD_az_sk, 0, 0);
		HTTPAPI_SetElementValueById(RECORD_TEXT_az_et, Azure_ExpiryTime, strlen(Azure_ExpiryTime));
	}

	return HTTPD_OK;
} /* End of httpdap_GetSecurityPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_PostSecurityPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostSecurityPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U8_T *pTmp, *pbuf1, *pbuf2, tmp;
	GCONFIG_BLUEMIX *pClientId;
	pbuf1 = HTTP_TmpBuf;
	pbuf2 = HTTP_TmpBuf + (HTTPDAP_TEMP_BUFFER_SIZE/2);
	GCONFIG_BLUEMIX_AR *pBMAR;

	if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_new_usn))
	{
		/* Save new username */
		GCONFIG_GetAdmin(pbuf1, pbuf2, &httpdap_tmp8);
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_new_usn, &pHttpap_buf);

		memcpy(pbuf1, pHttpap_buf, httpdap_tmp8);
		pbuf1[httpdap_tmp8] = '\0';
		GCONFIG_SetAdmin(pbuf1, pbuf2);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			/* Save new username and redirect to login page */
			GCONFIG_WriteConfigData();
			httpdap_WarningPageSend(pConn, "Change username ok!", 2, HTTPDAP_OFF, pInfo->LoginPage);
			return HTTPD_OK;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_PASSWORD_old_psw) &&
			 HTTPAPI_ElementUpdateChkById(RECORD_PASSWORD_cfm_psw))
	{
		/* check old password */
		GCONFIG_GetAdmin(pbuf1, pbuf2, &httpdap_tmp8);
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_PASSWORD_old_psw, &pHttpap_buf);
		if (strlen(pbuf2) != httpdap_tmp8 || memcmp(pbuf2, pHttpap_buf, httpdap_tmp8) != 0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		/* check new password and confirm password */
		tmp = HTTPAPI_GetElementValueById(RECORD_PASSWORD_cfm_psw, &pTmp);
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_PASSWORD_new_psw, &pHttpap_buf);
		if (tmp != httpdap_tmp8 || memcmp(pTmp, pHttpap_buf, httpdap_tmp8) != 0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		/* save new password */
		memcpy(pbuf2, pHttpap_buf, httpdap_tmp8);
		pbuf2[httpdap_tmp8] = '\0';
		GCONFIG_SetAdmin(pbuf1, pbuf2);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			/* Save new password and redirect to login page */
			GCONFIG_WriteConfigData();
			httpdap_WarningPageSend(pConn, "Change password ok!", 2, HTTPDAP_OFF, pInfo->LoginPage);
			return HTTPD_OK;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_smtp_un) &&
			 HTTPAPI_ElementUpdateChkById(RECORD_PASSWORD_smtp_pw))
	{
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_smtp_un, &pHttpap_buf);
		memcpy(pbuf1, pHttpap_buf, httpdap_tmp8);
		pbuf1[httpdap_tmp8] = '\0';

		tmp = HTTPAPI_GetElementValueById(RECORD_PASSWORD_smtp_pw, &pTmp);
		memcpy(pbuf2, pTmp, tmp);
		pbuf2[tmp] = '\0';

		GCONFIG_SetSMTPAccount(pbuf1, pbuf2);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_SECURITY_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_SECURITY_APPLY;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_gn_host))
	{
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_gn_host, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		if (GCONFIG_SetGoogleNestHostName(HTTP_TmpBuf)==0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_SECURITY_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_SECURITY_APPLY;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_bm_host) &&
			 HTTPAPI_ElementUpdateChkById(RECORD_TEXT_bm_devid))
	{
		/* Save Bluemix host name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_host, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		if (GCONFIG_SetBluemixHostName(HTTP_TmpBuf)==0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		pClientId = (GCONFIG_BLUEMIX*)HTTP_TmpBuf;
		/* Save Bluemix user name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_un, &pHttpap_buf);
		memcpy(pClientId->UserName, pHttpap_buf, httpdap_tmp8);
		pClientId->UserName[httpdap_tmp8] = '\0';

		/* Save Bluemix pass word */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_PASSWORD_bm_pw, &pHttpap_buf);
		memcpy(&pClientId->PassWord, pHttpap_buf, httpdap_tmp8);
		pClientId->PassWord[httpdap_tmp8] = '\0';

		/* Save Bluemix client ID */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_orgid, &pHttpap_buf);
		memcpy(pClientId->OrganizationID, pHttpap_buf, httpdap_tmp8);
		pClientId->OrganizationID[httpdap_tmp8] = '\0';

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_typid, &pHttpap_buf);
		memcpy(pClientId->TypeID, pHttpap_buf, httpdap_tmp8);
		pClientId->TypeID[httpdap_tmp8] = '\0';

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_devid, &pHttpap_buf);
		memcpy(pClientId->DeviceID, pHttpap_buf, httpdap_tmp8);
		pClientId->DeviceID[httpdap_tmp8] = '\0';

		if (GCONFIG_SetBluemixAccount(pClientId->OrganizationID, pClientId->TypeID, pClientId->DeviceID, pClientId->UserName, pClientId->PassWord)==0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		/* Set Bluemix auto registered information */
		pBMAR = (GCONFIG_BLUEMIX_AR*)HTTP_TmpBuf;

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_apiver, &pHttpap_buf);
		memcpy(pBMAR->ApiVersion, pHttpap_buf, httpdap_tmp8);
		pBMAR->ApiVersion[httpdap_tmp8] = '\0';

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_apikey, &pHttpap_buf);
		memcpy(pBMAR->ApiKey, pHttpap_buf, httpdap_tmp8);
		pBMAR->ApiKey[httpdap_tmp8] = '\0';

		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_bm_apitoken, &pHttpap_buf);
		memcpy(pBMAR->ApiToken, pHttpap_buf, httpdap_tmp8);
		pBMAR->ApiToken[httpdap_tmp8] = '\0';

		if (GCONFIG_SetBluemixAutoReg(pBMAR->ApiVersion, pBMAR->ApiKey, pBMAR->ApiToken) == 0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_SECURITY_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_SECURITY_APPLY;
		}
	}
	else if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_az_host) &&
			 HTTPAPI_ElementUpdateChkById(RECORD_TEXT_az_di))
	{
		/* Save Azure host name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_az_host, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		printf("hn:%s/%d\r\n", HTTP_TmpBuf, httpdap_tmp8);
		if (GCONFIG_SetAzureHostName(HTTP_TmpBuf)==0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		/* Save Azure HUB name */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_az_hn, &pHttpap_buf);
		memcpy(Azure_HubName, pHttpap_buf, httpdap_tmp8);
		Azure_HubName[httpdap_tmp8] = '\0';

		/* Save Azure device ID */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_az_di, &pHttpap_buf);
		memcpy(Azure_DeviceID, pHttpap_buf, httpdap_tmp8);
		Azure_DeviceID[httpdap_tmp8] = '\0';

		/* Save Azure signed key */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_PASSWORD_az_sk, &pHttpap_buf);
		memcpy(Azure_SignedKey, pHttpap_buf, httpdap_tmp8);
		Azure_SignedKey[httpdap_tmp8] = '\0';

		/* Save Azure expiry time */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_az_et, &pHttpap_buf);
		memcpy(Azure_ExpiryTime, pHttpap_buf, httpdap_tmp8);
		Azure_ExpiryTime[httpdap_tmp8] = '\0';

		if (GCONFIG_SetAzureAccount(Azure_HubName, Azure_DeviceID, Azure_SignedKey, Azure_ExpiryTime) == 0)
		{
			goto HTTPDAP_POST_SECURITY_ERR;
		}

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_SECURITY_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_SECURITY_APPLY;
		}
	}
HTTPDAP_POST_SECURITY_ERR:
	httpdap_WarningPageSend(pConn, HTTPDAP_SETTING_ERR, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_SECURITY_SAVE:
	/* Save configuration only */
	GCONFIG_WriteConfigData();
	httpdap_WarningPageSend(pConn, HTTPDAP_SAVE_OK, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_OK;

HTTPDAP_POST_SECURITY_APPLY:
	/* Save configuration and system reboot */
	GCONFIG_WriteConfigData();
	httpdap_SetAction(pConn, HTTPDAP_REBOOT, 3);
	httpdap_WarningPageSend(pConn, HTTPDAP_DEVICE_REBOOT, 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;

} /* End of httpdap_PostSecurityPage() */

/*
 * ----------------------------------------------------------------------------
 * HTTPD_STATUS httpdap_GetWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
  	U8_T i;

	/* Network mode */
	httpdap_tmp8 = (GCONFIG_GetWifiNetworkMode() > RTW_MODE_STA) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_network_mode, &httpdap_tmp8, 1);

	/* Channel */
	httpdap_tmp8 = GCONFIG_GetChannel()-1;//offset convert
	HTTPAPI_SetElementValueById(RECORD_SELECT_channel, &httpdap_tmp8, 1);

	/* Service Area Name/SSID */
	httpdap_tmp8 = GCONFIG_GetWifiSsid(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_ssid, HTTP_TmpBuf, httpdap_tmp8);

	/* Hide SSID */
	httpdap_tmp8 = GCONFIG_GetWifiHiddenSsid();
	HTTPAPI_SetElementValueById(RECORD_CHECKBOX_hide_ssid, &httpdap_tmp8, 1);

	/* Secirity Mode */
	httpdap_tmp8 = GCONFIG_GetWifiEncryptMode();
	HTTPAPI_SetElementValueById(RECORD_SELECT_security_mode, &httpdap_tmp8, 1);

	/* Key Length */
	httpdap_tmp8 = GCONFIG_GetWifiWepKeyLength() ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_key_length, &httpdap_tmp8, 1);

	for (i=0; i<4; i++)
	{
		if (httpdap_tmp8)
		{
			/* WEP 128bits Key 0~3 */
			GCONFIG_GetWifiWep128Key(i, HTTP_TmpBuf);
			if (httpdap_ChkPrint(HTTP_TmpBuf, 13)==HTTPD_ERR)
			{
				HTTPAPI_Num2HexText(HTTP_TmpBuf+100, HTTP_TmpBuf, 13);
				HTTPAPI_SetElementValueById(httpdap_WepKeyMap[i], HTTP_TmpBuf+100, 26);
			}
			else
			{
				HTTP_TmpBuf[13] = '\0';
				HTTPAPI_SetElementValueById(httpdap_WepKeyMap[i], HTTP_TmpBuf, 13);
			}
		}
		else
		{
			/* WEP 64bits Key 0~3 */
			GCONFIG_GetWifiWep64Key(i, HTTP_TmpBuf);
			if (httpdap_ChkPrint(HTTP_TmpBuf, 5)==HTTPD_ERR)
			{
				HTTPAPI_Num2HexText(HTTP_TmpBuf+100, HTTP_TmpBuf, 5);
				HTTPAPI_SetElementValueById(httpdap_WepKeyMap[i], HTTP_TmpBuf+100, 10);
			}
			else
			{
				HTTP_TmpBuf[5] = '\0';
				HTTPAPI_SetElementValueById(httpdap_WepKeyMap[i], HTTP_TmpBuf, 5);
			}
		}
	}

	/* Key Index Select */
	httpdap_tmp8 = GCONFIG_GetWifiWepKeyIndex();
	HTTPAPI_SetElementValueById(RECORD_SELECT_key_index, &httpdap_tmp8, 1);

	/* AES/TKIP Passphrase */
	httpdap_tmp8 = GCONFIG_GetWifiPreShareKey(HTTP_TmpBuf);
	HTTPAPI_SetElementValueById(RECORD_TEXT_aes_passphrase, HTTP_TmpBuf, httpdap_tmp8);

	return HTTPD_OK;
} /* End of httpdap_GetWiFiPage() */

/*
 * ----------------------------------------------------------------------------
 * HTTPD_STATUS httpdap_PostWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostWiFiPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U8_T i, j, minChNum, maxChNum;

	if (HTTPAPI_ElementUpdateChkById(RECORD_SELECT_network_mode) &&
		HTTPAPI_ElementUpdateChkById(RECORD_TEXT_aes_passphrase))
	{
		/* Network mode */
		HTTPAPI_GetElementValueById(RECORD_SELECT_network_mode, &pHttpap_buf);
		(*pHttpap_buf) ++;//+1 offset convert

		/* Auto configure DHCP client and server while switching wifi mode */
		if ((*pHttpap_buf) != GCONFIG_GetWifiNetworkMode())
		{
			if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_STA)//Change wifi mode STA->AP
			{
				GCONFIG_SetNetwork(GCONFIG_GetNetwork() & (~GCONFIG_NETWORK_DHCP_ENABLE));
				GCONFIG_SetDhcpSrvStatus(1);
			}
			else//Change wifi mode AP->STA
			{
				GCONFIG_SetNetwork(GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE);
				GCONFIG_SetDhcpSrvStatus(0);
			}
		}
		GCONFIG_SetWifiNetworkMode(*pHttpap_buf);

		/* Channel */
		HTTPAPI_GetElementValueById(RECORD_SELECT_channel, &pHttpap_buf);
		(*pHttpap_buf) ++;//+1 offset convert
		GCONFIG_SetChannel(*pHttpap_buf);

		/* Service Area Name/SSID */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_ssid, &pHttpap_buf);
		GCONFIG_SetWifiSsid(pHttpap_buf, httpdap_tmp8);

		/* Hide SSID */
		if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
		{
			if (HTTPAPI_ElementUpdateChkById(RECORD_CHECKBOX_hide_ssid))
			{
				GCONFIG_SetWifiHiddenSsid(1);
			}
			else
			{
				GCONFIG_SetWifiHiddenSsid(0);
			}
		}

		/* Secirity Mode */
		HTTPAPI_GetElementValueById(RECORD_SELECT_security_mode, &pHttpap_buf);
		GCONFIG_SetWifiEncryptMode(security_map[*pHttpap_buf]);

		/* Key Length */
		HTTPAPI_GetElementValueById(RECORD_SELECT_key_length, &pHttpap_buf);
		GCONFIG_SetWifiWepKeyLength(*pHttpap_buf);
		i = *pHttpap_buf;

		/* Key Index Select */
		HTTPAPI_GetElementValueById(RECORD_SELECT_key_index, &pHttpap_buf);
		GCONFIG_SetWifiWepKeyIndex(*pHttpap_buf);

		/* WEP 64/128 bits key 1~4 */
		for (j = 0; j < 4; j ++)
		{
			httpdap_tmp8 = HTTPAPI_GetElementValueById(httpdap_WepKeyMap[j], &pHttpap_buf);
			if (i==0)/* WEP-64 */
			{
				if (httpdap_tmp8==5)
				{
					GCONFIG_SetWifiWep64Key(j, pHttpap_buf);
				}
				else if (httpdap_tmp8==10)
				{
 					if (!HTTPAPI_HexText2Num(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8))
					{
						goto HTTPDAP_POST_WIFI_ERR;
					}
					GCONFIG_SetWifiWep64Key(j, HTTP_TmpBuf);
				}
				else
				{
					goto HTTPDAP_POST_WIFI_ERR;
				}
			}
			else/* WEP-128 */
			{
				if (httpdap_tmp8==13)
				{
					GCONFIG_SetWifiWep128Key(j, pHttpap_buf);
				}
				else if (httpdap_tmp8==26)
				{
 					if (!HTTPAPI_HexText2Num(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8))
					{
						goto HTTPDAP_POST_WIFI_ERR;
					}
					GCONFIG_SetWifiWep128Key(j, HTTP_TmpBuf);
				}
				else
				{
					goto HTTPDAP_POST_WIFI_ERR;
				}
			}
		}

		/* AES/TKIP Passphrase */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_aes_passphrase, &pHttpap_buf);
		if (httpdap_tmp8 < 8 || httpdap_tmp8 > 63)
			goto HTTPDAP_POST_WIFI_ERR;
		GCONFIG_SetWifiPreShareKey(pHttpap_buf, httpdap_tmp8);

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_WIFI_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_WIFI_APPLY;
		}
	}

HTTPDAP_POST_WIFI_ERR:
	httpdap_WarningPageSend(pConn, HTTPDAP_SETTING_ERR, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_WIFI_SAVE:
	/* Save configuration only */
	GCONFIG_WriteConfigData();
	httpdap_WarningPageSend(pConn, HTTPDAP_SAVE_OK, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_OK;

HTTPDAP_POST_WIFI_APPLY:
	/* Save configuration and system reboot */
	GCONFIG_WriteConfigData();
	httpdap_SetAction(pConn, HTTPDAP_REBOOT, 3);
	httpdap_WarningPageSend(pConn, HTTPDAP_DEVICE_REBOOT, 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;
} /* End of httpdap_PostWiFiPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_GetWiFiWizardPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetWiFiWizardPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	HTTPDAP_WIFI_INFO *pWifiInfo = &httpdap.WifiInfo;

	switch (pConn->FileId)
	{
	case FILE_WIFIWZ_KEY:
		HTTPAPI_SetElementValueById(RECORD_TEXT_wz_key, 0, 0);
		httpdap_tmp16 = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, pConn->FileId);
		httpdap_tmp16 += httpdap_InputHdrBuild(HTTP_TmpBuf+httpdap_tmp16, INPUT_HIDDEN);
		httpdap_tmp16 += httpdap_InputPropertyBuild(HTTP_TmpBuf+httpdap_tmp16, PROPERTY_ID, "enc_type", 0);
		httpdap_tmp16 += HTTPAPI_MsgBuild(HTTP_TmpBuf+httpdap_tmp16, "value=\"%s\" />\r\n", (pWifiInfo->EncryptMode==RTW_SECURITY_WEP_PSK) ? "wep":"psk");
		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, httpdap_tmp16, pConn->FileId);
		return HTTPD_ERR;
		break;

	case FILE_WIFIWZ_CFIRM:
		/* Load DHCP client status */
		httpdap_tmp8 = 1;//Force enable DHCP client
		HTTPAPI_SetElementValueById(RECORD_SELECT_wz_dhcpc, &httpdap_tmp8, 1);

		/* ---- Statis IP Settings ---- */
		/* Load static IP address */
		httpdap_tmp32 = htonl(GCONFIG_GetDeviceStaticIP());
		httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
		HTTPAPI_SetElementValueById(RECORD_TEXT_wz_static_ip, HTTP_TmpBuf, httpdap_tmp8);

		/* Load static subnet mask */
		httpdap_tmp32 = htonl(GCONFIG_GetNetmask());
		httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
		HTTPAPI_SetElementValueById(RECORD_TEXT_wz_mask, HTTP_TmpBuf, httpdap_tmp8);

		/* Load static gateway */
		httpdap_tmp32 = htonl(GCONFIG_GetGateway());
		httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
		HTTPAPI_SetElementValueById(RECORD_TEXT_wz_gateway_ip, HTTP_TmpBuf, httpdap_tmp8);

		/* Load static DNS */
		httpdap_tmp32 = htonl(GCONFIG_GetDNS());
		httpdap_tmp8 = HTTPAPI_Ulong2IpAddr(httpdap_tmp32, HTTP_TmpBuf);
		HTTPAPI_SetElementValueById(RECORD_TEXT_wz_dns_ip, HTTP_TmpBuf, httpdap_tmp8);

		httpdap_tmp16 = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, pConn->FileId);
		httpdap_tmp16 += httpdap_InputHdrBuild(HTTP_TmpBuf+httpdap_tmp16, INPUT_HIDDEN);
		httpdap_tmp16 += httpdap_InputPropertyBuild(HTTP_TmpBuf+httpdap_tmp16, PROPERTY_ID, "wifi_info", 0);
		httpdap_tmp16 += HTTPAPI_MsgBuild(HTTP_TmpBuf+httpdap_tmp16, "value=\"%s&%d&%s\" />\r\n"
										 	, pWifiInfo->Ssid
											, pWifiInfo->Channel
											, (pWifiInfo->EncryptMode==RTW_SECURITY_OPEN) ? "Open":((pWifiInfo->EncryptMode==RTW_SECURITY_WEP_PSK) ? "WEP":"WPA/WPA2 AES")
											  );
		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, httpdap_tmp16, pConn->FileId);
		return HTTPD_ERR;
		break;
	}
	return HTTPD_OK;
} /* End of httpdap_GetWiFiWizardPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_PostWiFiWizardPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostWiFiWizardPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U8_T i, j, *pTmp, req[20];
	U16_T len;
	HTTPDAP_WIFI_INFO *pWifiInfo = &httpdap.WifiInfo;

	switch (pConn->FileId)
	{
	case FILE_WIFIWZ_KEY:
		if (HTTPAPI_ElementUpdateChkByName("Cancel"))
		{
			HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, FILE_WIFIWZ_SVY);
			return HTTPD_ERR;
		}
		else if (HTTPAPI_ElementUpdateChkByName("Next"))
		{
			if (pWifiInfo->EncryptMode == RTW_SECURITY_WEP_PSK)
			{
				httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_key, &pHttpap_buf);
				if (httpdap_tmp8)
				{
					/* Set WEP key */
					memcpy(pWifiInfo->Password, pHttpap_buf, httpdap_tmp8);
					pWifiInfo->Password[httpdap_tmp8] = '\0';
					pWifiInfo->PasswordLen = httpdap_tmp8;

					/* Set WEP key index */
					HTTPAPI_GetElementValueById(RECORD_SELECT_wz_keyid, &pHttpap_buf);
					pWifiInfo->WepKeyIndex = *pHttpap_buf;
				}
			}
			else
			{
				httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_key, &pHttpap_buf);
				if (httpdap_tmp8)
				{
					/* Set PSK */
					memcpy(pWifiInfo->Password, pHttpap_buf, httpdap_tmp8);
					pWifiInfo->Password[httpdap_tmp8] = '\0';
					pWifiInfo->PasswordLen = httpdap_tmp8;

					pWifiInfo->WepKeyIndex = 0;
				}
			}

			HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, FILE_WIFIWZ_CFIRM);
			return HTTPD_ERR;
		}
		break;

	case FILE_WIFIWZ_CFIRM:
		if (HTTPAPI_ElementUpdateChkByName("Cancel"))
		{
			HTTPAPI_RedirRspSend(pConn, HTTP_TmpBuf, FILE_WIFIWZ_SVY);
			return HTTPD_ERR;
		}
		else if (HTTPAPI_ElementUpdateChkByName("Accept"))
		{
			/* Save DHCP client status */
			httpdap_tmp16 = GCONFIG_GetNetwork();
			HTTPAPI_GetElementValueById(RECORD_SELECT_wz_dhcpc, &pHttpap_buf);
			httpdap_tmp16 = (httpdap_tmp16 & 0xBFFF) | ((*pHttpap_buf)? GCONFIG_NETWORK_DHCP_ENABLE:GCONFIG_NETWORK_DHCP_DISABLE);
			GCONFIG_SetNetwork(httpdap_tmp16);

			/* ---- Statis IP Settings ---- */
			/* Save IP address */
			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_static_ip, &pHttpap_buf);
			httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
			GCONFIG_SetDeviceStaticIP(httpdap_tmp32);

			/* Save subnet mask */
			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_mask, &pHttpap_buf);
			httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
			GCONFIG_SetNetmask(httpdap_tmp32);

			/* Save gateway */
			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_gateway_ip, &pHttpap_buf);
			httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
			GCONFIG_SetGateway(httpdap_tmp32);

			/* Save DNS */
			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_wz_dns_ip, &pHttpap_buf);
			httpdap_tmp32 = htonl(HTTPAPI_IpAddr2Ulong(pHttpap_buf, httpdap_tmp8));
			GCONFIG_SetDNS(httpdap_tmp32);

			pWifiInfo->Used	= 1;
			httpdap_SetAction(pConn, HTTPDAP_WIFI_CONNECT, 2);

			len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_SYSMSG);
			len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
			len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "sysmsg_setting", 0);
			len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"Device Rebooting...\" />\r\n");
			HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, FILE_SYSMSG);
			return HTTPD_ERR;
		}
		break;
	}

HTTPDAP_POST_WIFIWZ_ERR:
	httpdap_WarningPageSend(pConn, "Settings validation error!", 2, HTTPDAP_OFF, FILE_WIFI_WZ);
	return HTTPD_ERR;
} /* End of httpdap_PostWiFiWizardPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_GetRtcPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_GetRtcPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	/* Load RTC Time Setup Mode */
	httpdap_tmp8 = (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_RADIO_rtc_clb, &httpdap_tmp8, 1);

	/* Load RTC Daylight Saving Time */
	httpdap_tmp8 = (GCONFIG_GetNetwork() & GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE) ? 1:0;
	HTTPAPI_SetElementValueById(RECORD_SELECT_dst_onoff, &httpdap_tmp8, 1);

	/* Load RTC current date time */
	RTC_GetCurrTime(&httpdap_RtcDateTime);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Year);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_yy, HTTP_TmpBuf, httpdap_tmp8);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Month);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_mm, HTTP_TmpBuf, httpdap_tmp8);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Date);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_dd, HTTP_TmpBuf, httpdap_tmp8);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Hour);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_hh, HTTP_TmpBuf, httpdap_tmp8);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Minute);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_nn, HTTP_TmpBuf, httpdap_tmp8);

	httpdap_tmp8 = HTTPAPI_MsgBuild(HTTP_TmpBuf, "%d", httpdap_RtcDateTime.Second);
	HTTPAPI_SetElementValueById(RECORD_TEXT_rtc_ss, HTTP_TmpBuf, httpdap_tmp8);

	/* Load SNTP timezone and server IP */
	GCONFIG_GetNtpTimeZone(&httpdap_tmp8);
	HTTPAPI_SetElementValueById(RECORD_SELECT_ntps_tz, &httpdap_tmp8, 1);

	pHttpap_buf = GCONFIG_GetNtpSrvHostName(0);
	HTTPAPI_SetElementValueById(RECORD_TEXT_ntps_name1, pHttpap_buf, strlen(pHttpap_buf));

	pHttpap_buf = GCONFIG_GetNtpSrvHostName(1);
	HTTPAPI_SetElementValueById(RECORD_TEXT_ntps_name2, pHttpap_buf, strlen(pHttpap_buf));

	pHttpap_buf = GCONFIG_GetNtpSrvHostName(2);
	HTTPAPI_SetElementValueById(RECORD_TEXT_ntps_name3, pHttpap_buf, strlen(pHttpap_buf));

	return HTTPD_OK;
} /* End of httpdap_GetRtcPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_PostRtcPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_PostRtcPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	if (HTTPAPI_ElementUpdateChkById(RECORD_RADIO_rtc_clb) && HTTPAPI_ElementUpdateChkById(RECORD_SELECT_ntps_tz))
	{
		httpdap_tmp16 = GCONFIG_GetNetwork();
		HTTPAPI_GetElementValueById(RECORD_RADIO_rtc_clb, &pHttpap_buf);
		if (pHttpap_buf[0])
			httpdap_tmp16 |= GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;
		else
			httpdap_tmp16 &= ~GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;

		HTTPAPI_GetElementValueById(RECORD_SELECT_dst_onoff, &pHttpap_buf);
		if (pHttpap_buf[0])
			httpdap_tmp16 |= GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE;
		else
			httpdap_tmp16 &= ~GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE;
		GCONFIG_SetNetwork(httpdap_tmp16);

		if ((GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP) == 0)
		{
			httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_yy, &pHttpap_buf));
			httpdap_RtcDateTime.Year = httpdap_tmp16;

			httpdap_tmp8 = (U8_T)(HTTPAPI_Str2Short(pHttpap_buf, HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_mm, &pHttpap_buf)));
			httpdap_RtcDateTime.Month = httpdap_tmp8;

			httpdap_tmp8 = (U8_T)(HTTPAPI_Str2Short(pHttpap_buf, HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_dd, &pHttpap_buf)));
    		httpdap_RtcDateTime.Date = httpdap_tmp8;

			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_hh, &pHttpap_buf);
			httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
			httpdap_RtcDateTime.Hour = (U8_T)httpdap_tmp16;

			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_nn, &pHttpap_buf);
			httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
			httpdap_RtcDateTime.Minute = (U8_T)httpdap_tmp16;

			httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_rtc_ss, &pHttpap_buf);
			httpdap_tmp16 = HTTPAPI_Str2Short(pHttpap_buf, httpdap_tmp8);
			httpdap_RtcDateTime.Second = (U8_T)httpdap_tmp16;

			RTC_SetCurrTime(&httpdap_RtcDateTime);
		}
		/* Set time zone */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_SELECT_ntps_tz, &pHttpap_buf);
		GCONFIG_SetNtpTimeZone(pHttpap_buf[0]);

		/* Set NTP server host name 1 */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_ntps_name1, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		GCONFIG_SetNtpSrvHostName(0, HTTP_TmpBuf);

		/* Set NTP server host name 2 */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_ntps_name2, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		GCONFIG_SetNtpSrvHostName(1, HTTP_TmpBuf);

		/* Set NTP server host name 3 */
		httpdap_tmp8 = HTTPAPI_GetElementValueById(RECORD_TEXT_ntps_name3, &pHttpap_buf);
		memcpy(HTTP_TmpBuf, pHttpap_buf, httpdap_tmp8);
		HTTP_TmpBuf[httpdap_tmp8] = '\0';
		GCONFIG_SetNtpSrvHostName(2, HTTP_TmpBuf);

		if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
			SNTPC_ReqEnqueue(RTC_NtpQuery, 0);//Run SNTP Client again

		if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Save))
		{
			goto HTTPDAP_POST_RTC_SAVE;
		}
		else if (HTTPAPI_ElementUpdateChkById(RECORD_SUBMIT_Apply))
		{
			goto HTTPDAP_POST_RTC_APPLY;
		}
	}
HTTPDAP_POST_RTC_ERR:
	httpdap_WarningPageSend(pConn, HTTPDAP_SETTING_ERR, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_RTC_SAVE:
	/* Save coniguration only */
	GCONFIG_WriteConfigData();
	httpdap_WarningPageSend(pConn, HTTPDAP_SAVE_OK, 2, HTTPDAP_OFF, pConn->FileId);
	return HTTPD_ERR;

HTTPDAP_POST_RTC_APPLY:
	/* Save configuration and system reboot */
	GCONFIG_WriteConfigData();
	httpdap_SetAction(pConn, HTTPDAP_REBOOT, 3);
	httpdap_WarningPageSend(pConn, HTTPDAP_DEVICE_REBOOT, 13, HTTPDAP_ON, pConn->FileId);
	return HTTPD_OK;
} /* End of httpap_PostRtcPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_CgiHctentPage()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_CgiHctentPage(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	U8_T i, *pbuf, *pbuf2, buf[64], *pip, *pmac, is_conn=0, minChNum, maxChNum;
	U32_T tmp32, rx_bytes=0, tx_bytes=0;
	S16_T len;
	HTTPDAP_FILE_UPLOAD	*pUpload = &httpdap.Upload;
	float ftmp, frh;

	if (HTTPAPI_ElementUpdateChkById(RECORD_TEXT_cmd)==0)
		return HTTPD_OK;

	i = HTTPAPI_GetElementValueById(RECORD_TEXT_cmd, &pbuf);

	if (!memcmp(pbuf,"getwep", i))
	{
		/* for WEP64bits and WEP128bit dynamic option */
		pbuf = HTTP_TmpBuf + 400;
		pbuf2 = HTTP_TmpBuf + 450;
		len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_HCTENT);
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "0", 0);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"");
		for (i = 0; i < 4; i ++)
		{
			GCONFIG_GetWifiWep64Key(i, pbuf);
			if (httpdap_ChkPrint(pbuf, 5)==HTTPD_ERR)
			{
				HTTPAPI_Num2HexText(pbuf2, pbuf, 5);
				pbuf2[10] = '\0';
				len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%s&", pbuf2);
			}
			else
			{
				pbuf[5] = '\0';
				len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%s&", pbuf);
			}
		}
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "\"/>");

		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "1", 0);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"");
		for (i = 0; i < 4; i ++)
		{
			GCONFIG_GetWifiWep128Key(i, pbuf);
			if (httpdap_ChkPrint(pbuf, 13)==HTTPD_ERR)
			{
				HTTPAPI_Num2HexText(pbuf2, pbuf, 13);
				pbuf2[26] = '\0';
				len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%s&", pbuf2);
			}
			else
			{
				pbuf[13] = '\0';
				len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%s&", pbuf);
			}
		}
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "\"/>");

		/* wifi channel plan verification information */
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "2", 0);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"");
		GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%d&%d", minChNum, maxChNum);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "\"/>");

		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
		return HTTPD_OK;
	}
	else if (!memcmp(pbuf, "getstatus", i))
	{
		memset(buf, 0, sizeof(buf));

	  	/* Get device name */
		pbuf = (U8_T *)(GCONFIG_GetDeviceName());
		memcpy(buf, pbuf, sizeof(GCONFIG_DEV_NAME));

		/* Get local ip adderss */
		pmac = GCONFIG_GetMacAddress();
		tmp32 = MISC_GetCurrentIPAddr();
		pip = (u8*)&tmp32;
		switch (GCONFIG_GetNetwork() & GCONFIG_NETWORK_R2WMODEMASK)
		{
		case R2WMODE_Socket:
		case R2WMODE_VCom:
		default:
			if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_PROTO_TCP)
			{
				is_conn = (GS2W_GetTaskState()==GS2W_STATE_TCP_DATA_PROCESS) ? 1:0;
				rx_bytes = GTCPDAT_GetUrRxBytes(0);
				tx_bytes = GTCPDAT_GetUrTxBytes(0);
			}
#if (GUDPDAT_INCLUDE)
			else
			{
				is_conn = (GS2W_GetTaskState()==GS2W_STATE_UDP_DATA_PROCESS) ? 1:0;
				rx_bytes = GUDPDAT_GetUrRxBytes(0);
				tx_bytes = GUDPDAT_GetUrTxBytes(0);
			}
#endif
			break;
		case R2WMODE_Rfc2217:
			TNCOM_GetRxTxCounter(&rx_bytes, &tx_bytes);
			break;
		case R2WMODE_MODBUS_GW:
			is_conn = MBGW_GetConnStatus() ? 1:0;
			MBGW_GetRxTxCounter(0, &rx_bytes, &tx_bytes);
			break;
		}

		len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_HCTENT);
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "0", 0);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"%s&%d.%d.%d.%d&%s%s&0x%02x%02x%02x%02x%02x%02x&0x%02x&%s&%s&%d&%d&"
									, buf/* Device name */
									, pip[0], pip[1], pip[2], pip[3]/* Local IP address */
									, GCONFIG_VERSION_STRING/* Firmware version */
									, (GCONFIG_GetUpgradeMode() == SINGLE_UPGRADE_IMAGE_MODE) ? "(Single)":"(Dual)" /* Image upgrade mode */
									, pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]/* MAC */
									, HAL_RUART_READ8(0, RUART_MODEM_STATUS_REG_OFF)
									, (GCONFIG_GetNetwork() & GCONFIG_NETWORK_PROTO_TCP)?"TCP":"UDP"/* Protocol type */
									, (is_conn!=0) ? "Connected" : "Idle"/* Connection status */
									, tx_bytes/* UART TX counter */
									, rx_bytes/* UART RX counter */
									);

		RTC_GetCurrTime(&httpdap_RtcDateTime);
		THS_GetTemperature(&ftmp);
		THS_GetRelativeHumidity(&frh);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%.2f&%.2f&%d/%d/%d %s&%d:%d:%d&"
									, ftmp/* Temperature */
									, frh/* Relative humidity */
									, httpdap_RtcDateTime.Year/* RTC date */
									, httpdap_RtcDateTime.Month
									, httpdap_RtcDateTime.Date
									, GCONFIG_WEEK_TABLE[httpdap_RtcDateTime.Day]
									, httpdap_RtcDateTime.Hour/* RTC time */
									, httpdap_RtcDateTime.Minute
									, httpdap_RtcDateTime.Second
									);

		GCONFIG_GetFilename(buf, 64);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "%s&%s %s\" />"
									, buf/* Image file name */
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
									, (GNAPP_CheckConnectionStatus()==GNAPP_CONNECTED) ? "Connected":"Disconnected"
									, "with Google Nest Firebase"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
									, BLUEMIX_CheckConnectionState()
									, "with IBM Bluemix"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
									, AZURE_CheckConnectionState()
									, "with Microsoft Azure"
#else
									, "No cloud service", ""
#endif
									);

		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
		return HTTPD_OK;
	}
	else if (!memcmp(pbuf, "get_ugr_stu", i))
	{
		len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_HCTENT);
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_NAME, "", 0);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "0", 0);
		switch (pUpload->Status)
		{
		case UPLOAD_IDLE:
		case UPLOAD_ENTITY_START:
		default:
			len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"%s&0&0&\" />", (pUpload->Status==UPLOAD_IDLE)?"ready":"busy");
			break;
		case UPLOAD_RECV_FILE:
			len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"busy&%d&%d&\" />", pUpload->CopiedLen>>1, pUpload->FileLen);
			break;
		case UPLOAD_PROGRAMMING:
		case UPLOAD_DONE:
			len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"busy&%d&%d&\" />", (pUpload->WrittenLen+pUpload->CopiedLen+UPGRADE_ASIX_RT_HDR_SIZE)>>1, pUpload->FileLen);
			break;

		case UPLOAD_FAIL:
		  	len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"fail&%s&0&\" />", (pUpload->pErrMessage)?pUpload->pErrMessage:"unknown fail");
			break;
		}
		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
	}
	return HTTPD_OK;
} /* End of httpdap_CgiHctentPage() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_WifiScanCb()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static rtw_result_t httpdap_WifiScanCb(rtw_scan_handler_result_t* malloced_scan_result)
{
	static int ApNum=0, bufSize=sizeof(httpdap_WifiScanBuf), len=0;
	rtw_scan_result_t *precord;

	if (malloced_scan_result->scan_complete != RTW_TRUE)
	{
		precord = &malloced_scan_result->ap_details;
		precord->SSID.val[precord->SSID.len] = 0; /* Ensure the SSID is null terminated */
		if (ApNum < STA_SCANNED_BSS_MAX)
		{
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%s%d,", (ApNum==0) ? "":";", ApNum);
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%02x:%02x:%02x:%02x:%02x:%02x,"
										, precord->BSSID.octet[0]
										, precord->BSSID.octet[1]
										, precord->BSSID.octet[2]
										, precord->BSSID.octet[3]
										, precord->BSSID.octet[4]
										, precord->BSSID.octet[5]
										  );
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%s,", precord->SSID.val);
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%s,", (precord->bss_type==RTW_BSS_TYPE_ADHOC) ? "Adhoc":"Infra");
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%d,", precord->channel);
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%d,", precord->signal_strength);
//			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%s,", precord->security==RTW_SECURITY_OPEN ? "no":"yes");
			len += snprintf(httpdap_WifiScanBuf+len, bufSize-len, "%s",
								(precord->security==RTW_SECURITY_OPEN) ? "Open":
								(precord->security==RTW_SECURITY_WEP_PSK) ? "WEP":
								(precord->security==RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP":
								(precord->security==RTW_SECURITY_WPA_AES_PSK) ? "WPA AES":
								(precord->security==RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES":
								(precord->security==RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP":
								(precord->security==RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed":
								(precord->security==RTW_SECURITY_WPA_WPA2_MIXED) ? "WPA/WPA2 AES":"Unknown");
		}
		ApNum ++;
	}
	else
	{
		httpdap_WifiScanBuf[len] = '\0';
		httpdap_WifiDone = 1;
		ApNum = 0;
		bufSize=sizeof(httpdap_WifiScanBuf);
		len = 0;
	}
	return RTW_SUCCESS;
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_WifiConn()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void httpdap_WifiConn(void *pParam)
{
	int	ret, pw[26];
	HTTPDAP_WIFI_INFO *pWifiInfo = pParam;

	printf("HTTPDAP: Connect to ssid=%s, pw=%s, pwlen=%d, keyid=%d\r\n", pWifiInfo->Ssid, pWifiInfo->Password, pWifiInfo->PasswordLen, pWifiInfo->WepKeyIndex);
	MISC_EnterWiFiBusySection();
	/* Switch to STA mode if need */
	wext_get_mode(WLAN0_NAME, &ret);
	if(ret == IW_MODE_MASTER)
	{
#if CONFIG_LWIP_LAYER
		DHCPS_DeInit();
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0)
		{
			MISC_LeaveWiFiBusySection();
			goto httpdap_WifiConn_Exit;
		}
	}

	/* Connect to specified ssid */
	ret = wifi_connect(pWifiInfo->Ssid, pWifiInfo->EncryptMode, pWifiInfo->Password, pWifiInfo->SsidLen, pWifiInfo->PasswordLen, pWifiInfo->WepKeyIndex, NULL);
	if(ret != RTW_SUCCESS)
	{
		MISC_LeaveWiFiBusySection();
		goto httpdap_WifiConn_Exit;
	}
	else
	{
		/* Save wifi parameters to gconfig */
		GCONFIG_SetWifiRfEnable(1);					/* WIFI RF Enable */

		GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);	/* Network Mode */
		GCONFIG_SetWifiSsid(pWifiInfo->Ssid, pWifiInfo->SsidLen);	/* SSID */
		GCONFIG_SetWifiEncryptMode(pWifiInfo->EncryptMode);	/* Security Type */
		if (pWifiInfo->EncryptMode == RTW_SECURITY_WEP_PSK)
		{
			GCONFIG_SetWifiWepKeyIndex(pWifiInfo->WepKeyIndex);		/* WEP Key Index */
			if (pWifiInfo->PasswordLen == 10 || pWifiInfo->PasswordLen == 26)
			{
				pWifiInfo->PasswordLen = HTTPAPI_HexText2Num(pw, pWifiInfo->Password, pWifiInfo->PasswordLen);
				memcpy(pWifiInfo->Password, pw, pWifiInfo->PasswordLen);
				pWifiInfo->Password[pWifiInfo->PasswordLen] = '\0';
			}
			if (pWifiInfo->PasswordLen == 5)
			{
				GCONFIG_SetWifiWepKeyLength(0);
				GCONFIG_SetWifiWep64Key(pWifiInfo->WepKeyIndex, pWifiInfo->Password);
			}
			else if (pWifiInfo->PasswordLen == 13)
			{
				GCONFIG_SetWifiWepKeyLength(1);
				GCONFIG_SetWifiWep128Key(pWifiInfo->WepKeyIndex, pWifiInfo->Password);
			}
		}
		else if (pWifiInfo->EncryptMode == RTW_SECURITY_WPA2_AES_PSK)
		{
			GCONFIG_SetWifiPreShareKey(pWifiInfo->Password, pWifiInfo->PasswordLen);	/* WPAx Pre-Shared Key */
		}

		/* Disable DHCP server */
		GCONFIG_SetDhcpSrvStatus(0);
		GCONFIG_WriteConfigData();
		httpdap_system_reset();
	}
	MISC_LeaveWiFiBusySection();

httpdap_WifiConn_Exit:
	pWifiInfo->Used = 0;
	vTaskDelete(NULL);
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: httpdap_RequestCgi()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS httpdap_RequestCgi(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	S16_T				len;
	U8_T				req[20], *pStu = 0;
	int					ret = RTW_ERROR;
	HTTPDAP_WIFI_INFO	*pWifiInfo = &httpdap.WifiInfo;
	U8_T				RejectCommand = 0;

	if (HTTPAPI_GetElementValueByName("req", req, 20)==0)
		goto httpdap_RequestCgi_Err;

	if (!strcmp(req, "wifi_scan"))
	{
		httpdap_WifiDone = 0;

		if (HTTPAPI_GetElementValueByName("row", HTTP_TmpBuf, 100) == 0)
		{
	  		goto httpdap_RequestCgi_Err;
		}

		if (rltk_wlan_running(WLAN0_IDX) == 0)
		{
			RejectCommand = 1;
			goto httpdap_RequestCgi_Err;
		}

		MISC_EnterWiFiBusySection();
		if (wifi_scan_networks(httpdap_WifiScanCb, NULL) != RTW_SUCCESS)
		{
			MISC_LeaveWiFiBusySection();
			goto httpdap_RequestCgi_Err;
		}
		while (httpdap_WifiDone==0)
			vTaskDelay(100);
		MISC_LeaveWiFiBusySection();

		len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_CGIREQ);
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "rspdata", 0);
		len += snprintf(HTTP_TmpBuf+len, HTTPDAP_TEMP_BUFFER_SIZE-len, "value=\"rsp=%s&stu=ok&value=%s\" />", req, httpdap_WifiScanBuf);
		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
	}
	else if (!strcmp(req, "wifi_conn"))
	{
		if (pWifiInfo->Used)
			goto httpdap_RequestCgi_Err;

		/* Get ssid */
		if (HTTPAPI_GetElementValueByName("ssid", pWifiInfo->Ssid, 33)==0)
	  		goto httpdap_RequestCgi_Err;
		pWifiInfo->SsidLen = strlen(pWifiInfo->Ssid);

		/* Get channel */
		if (HTTPAPI_GetElementValueByName("ch", HTTP_TmpBuf, 100)==0)
	  		goto httpdap_RequestCgi_Err;
		HTTPAPI_DecText2Char(HTTP_TmpBuf, strlen(HTTP_TmpBuf), &pWifiInfo->Channel);

		/* Get security type */
		if (HTTPAPI_GetElementValueByName("security", HTTP_TmpBuf, 100)==0)
	  		goto httpdap_RequestCgi_Err;

		if (!strcmp(HTTP_TmpBuf, "Open"))
		{
			pWifiInfo->WepKeyIndex = 0;
			pWifiInfo->PasswordLen = 0;
			pWifiInfo->Password[pWifiInfo->PasswordLen] = '\0';
			pWifiInfo->EncryptMode = RTW_SECURITY_OPEN;
			pStu="Open";
		}
		else if (!strcmp(HTTP_TmpBuf, "WEP"))
		{
			pWifiInfo->WepKeyIndex = GCONFIG_GetWifiWepKeyIndex();
			if (GCONFIG_GetWifiWepKeyLength())
			{
				GCONFIG_GetWifiWep128Key(pWifiInfo->WepKeyIndex, pWifiInfo->Password);
				pWifiInfo->Password[13] = '\0';
				pWifiInfo->PasswordLen = 13;
			}
			else
			{
				GCONFIG_GetWifiWep64Key(pWifiInfo->WepKeyIndex, pWifiInfo->Password);
				pWifiInfo->Password[5] = '\0';
				pWifiInfo->PasswordLen = 5;
			}
			pWifiInfo->EncryptMode = RTW_SECURITY_WEP_PSK;
			pStu="Key";
		}
		else
		{
			/* Load default pre-share key */
			pWifiInfo->PasswordLen = GCONFIG_GetWifiPreShareKey(pWifiInfo->Password);
			pWifiInfo->Password[pWifiInfo->PasswordLen] = '\0';
			pWifiInfo->EncryptMode = RTW_SECURITY_WPA2_AES_PSK;
			pStu="Key";
		}

		len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_CGIREQ);
		len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
		len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "rspdata", 0);
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"rsp=%s&stu=%s\" />", req, pStu);
		HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
	}
	return HTTPD_OK;

httpdap_RequestCgi_Err:
	len = HTTPAPI_OkHdrBuild(HTTP_TmpBuf, FILE_CGIREQ);
	len += httpdap_InputHdrBuild(HTTP_TmpBuf+len, INPUT_HIDDEN);
	len += httpdap_InputPropertyBuild(HTTP_TmpBuf+len, PROPERTY_ID, "rspdata", 0);
	if (RejectCommand)
	{
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"rsp=%s&stu=%s\" />", req, (pStu==0) ? "fail due to wifi be turned off":(char*)pStu);
	}
	else
	{
		len += HTTPAPI_MsgBuild(HTTP_TmpBuf+len, "value=\"rsp=%s&stu=%s\" />", req, (pStu==0) ? "fail":(char*)pStu);
	}
	HTTPAPI_MsgCombineSend(pConn, HTTP_TmpBuf, len, pConn->FileId);
	return HTTPD_ERR;
} /* End of httpdap_RequestCgi() */

/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_Init()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_Init(HTTPD_SERVER_INFO *pInfo)             //20170726 Craig Web Page Init
{
	pInfo->LoginPage = FILE_INDEX;
	pInfo->HomePage = FILE_BASIC;
	pInfo->MaxConnNumber = HTTPDAP_MAX_CONNECTS;
	pInfo->MaxPostCount	= HTTPDAP_MAX_POST_COUNT;
	pInfo->MaxDivideNumber = HTTPDAP_MAX_DIVIDE_NUM;
#if HTTPDAP_RFC2617_ENABLE
	pInfo->LoginPage = pInfo->HomePage;
#endif
	memset((U8_T *)&httpdap, 0, sizeof(HTTPDAP_INFO));
	if (!(httpdapTimerHandle = xTimerCreate("", 1000, pdTRUE, NULL, httpdap_Timer)))
		return HTTPD_ERR;

	httpdapMutex = xSemaphoreCreateMutex();
	xTimerStart(httpdapTimerHandle, 0);

	return HTTPD_OK;
} /* End of HTTPDAP_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_ResponseComplete()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_ResponseComplete(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	HTTPDAP_ACTION *pActionTemp = &httpdap.ActionTemp[pConn->Index];

	/* Enqueue action */
	if (pActionTemp->Type)
	{
		if (httpdap.Action.Type==0)
		{
			while(xSemaphoreTake(httpdapMutex, portMAX_DELAY) != pdTRUE);
			httpdap.Action.Type = pActionTemp->Type;
			httpdap.Action.DelayTime = pActionTemp->DelayTime;
			xSemaphoreGive(httpdapMutex);
			return HTTPD_OK;
		}
	}
	return HTTPD_ERR;
} /* End of HTTPDAP_ResponseComplete() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_GetMethod()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_GetMethod(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
  	if (pConn->State == HTTPD_STATE_ACTIVE)
	{
		if (httpdap_SecurityProcess(pConn, pInfo) == HTTPD_ERR)
			return HTTPD_ERR;
	}

	switch (pConn->FileId)
	{
	case FILE_INDEX:
		return (httpdap_GetIndexPage(pConn, pInfo));
		break;
	case FILE_BASIC:
		return (httpdap_GetBasicPage(pConn, pInfo));
		break;
	case FILE_ADVANCED:
		return (httpdap_GetAdvancePage(pConn, pInfo));
		break;
	case FILE_SECURITY:
		return (httpdap_GetSecurityPage(pConn, pInfo));
		break;
	case FILE_WIFI:
		return (httpdap_GetWiFiPage(pConn, pInfo));
		break;
	case FILE_WIFIWZ_CFIRM:
	case FILE_WIFIWZ_KEY:
	case FILE_WIFIWZ_SVY:
	case FILE_WIFI_WZ:
		return (httpdap_GetWiFiWizardPage(pConn, pInfo));
		break;
	case FILE_RTC:
		return (httpdap_GetRtcPage(pConn, pInfo));
		break;
	default:
		break;
	}
	return HTTPD_OK;
} /* End of HTTPDAP_GetMethod() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_PostMethod()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_PostMethod(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
  	if (pConn->State == HTTPD_STATE_ACTIVE)
	{
		if (httpdap_SecurityProcess(pConn, pInfo) == HTTPD_ERR)
			return HTTPD_ERR;
	}

	switch (pConn->FileId)
	{
	case FILE_INDEX:
		return (httpdap_PostIndexPage(pConn, pInfo));
		break;
	case FILE_BASIC:
		return (httpdap_PostBasicPage(pConn, pInfo));
		break;
	case FILE_ADVANCED:
		return (httpdap_PostAdvancePage(pConn, pInfo));
		break;
	case FILE_SECURITY:
		return (httpdap_PostSecurityPage(pConn, pInfo));
		break;
	case FILE_WIFI:
		return (httpdap_PostWiFiPage(pConn, pInfo));
		break;
	case FILE_WIFIWZ_CFIRM:
	case FILE_WIFIWZ_KEY:
	case FILE_WIFIWZ_SVY:
	case FILE_WIFI_WZ:
		return (httpdap_PostWiFiWizardPage(pConn, pInfo));
		break;
	case FILE_RTC:
		return (httpdap_PostRtcPage(pConn, pInfo));
		break;
//	case FILE_HCTENT:
//		return (httpdap_PostHctentPage(pConn, pInfo));
//		break;
	default:
		break;
	}
	return HTTPD_ERR;

} /* End of HTTPDAP_PostMethod() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_CgiScript()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_CgiScript(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
  	if (pConn->State == HTTPD_STATE_ACTIVE)
	{
		if (httpdap_SecurityProcess(pConn, pInfo) == HTTPD_ERR)
			return HTTPD_ERR;
	}

	switch (pConn->FileId)
	{
	case FILE_HCTENT:
		return (httpdap_CgiHctentPage(pConn, pInfo));
		break;
	case FILE_WIFI_WZ:
		return (httpdap_PostWiFiWizardPage(pConn, pInfo));
		break;
	case FILE_CGIREQ:
		return (httpdap_RequestCgi(pConn, pInfo));
		break;
	default:
		break;
	}
  	return HTTPD_OK;
} /* End of HTTPDAP_CgiScript() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: HTTPDAP_NewConn()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
HTTPD_STATUS HTTPDAP_NewConn(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo)
{
	memset((U8_T*)&httpdap.ActionTemp[pConn->Index], 0, sizeof(HTTPDAP_ACTION));
	return HTTPD_OK;
} /* End of HTTPDAP_NewConn() */

/* End of httpdap.c */


