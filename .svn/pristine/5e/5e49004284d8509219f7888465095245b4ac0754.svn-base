/*
 ******************************************************************************
 *     Copyright (c) 2010	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
 /*============================================================================
 * Module Name: httpdap.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 * $Log: httpdap.h,v $
 * no message
 *
 *=============================================================================
 */

#ifndef __HTTPDAP_H__
#define __HTTPDAP_H__

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include "httpd.h"
#include "filesys.h"
#include "gconfig.h"
#include "upgrade.h"
   
/* NAMING CONSTANT DECLARATIONS */
#define HTTPDAP_MAX_CONNECTS		8
#define HTTPDAP_MAX_LOGIN_NUM		HTTPDAP_MAX_CONNECTS
#define HTTPDAP_LOGIN_EXPIRY_TIME	3600//unit in sec
#define HTTPDAP_FIRMWARE_UPGRADE_TIMEOUT	30//unit in sec
#define HTTPDAP_MAX_POST_COUNT		MAX_POST_COUNT
#define HTTPDAP_MAX_DIVIDE_NUM		MAX_DIVIDE_NUM   

/* TYPE DECLARATIONS */
typedef enum
{
	UPLOAD_IDLE = 0,
	UPLOAD_ENTITY_START,
	UPLOAD_RECV_FILE,
	UPLOAD_PROGRAMMING,
	UPLOAD_FAIL,
	UPLOAD_DONE,
} HTTPDAP_UPLOAD_STATUS;

typedef enum {
	HTTPDAP_IDLE = 0,
	HTTPDAP_REBOOT,
	HTTPDAP_RESTORE,
	HTTPDAP_WIFI_CONNECT,	
	HTTPDAP_NETBACKUP_MODE,
} HTTPDAP_ACTION_TYPE;

typedef struct _HTTPDAP_LOGIN
{
	U32_T	ExpiryTime;
	U32_T	UserIP;
} HTTPDAP_LOGIN;

typedef struct _HTTPDAP_ACTION
{
	U8_T	Type;
	U16_T	DelayTime;
} HTTPDAP_ACTION;

typedef struct _HTTPDAP_FILE_UPLOAD
{
	U8_T	Status;
	U32_T	FileLen;
	HTTPD_SERVER_CONN *pConn;
	U8_T	FileName[GCONFIG_MAX_FILENAME_LEN];	
	U8_T	FileNameLen;
	U8_T	*pErrMessage;
	U16_T	Timer;
	U32_T	CopiedLen;	
	U32_T	WrittenLen;	
	HTTPD_SERVER_INFO	*pInfo;	
} HTTPDAP_FILE_UPLOAD;

typedef struct
{
	u8	Used;
	u8	Channel;			/* Channel */
	u8	SsidLen;			/* SSID length */
	u8	Ssid[33];			/* SSID */
	u32	EncryptMode;		/* Open, WEP Open, WEP shared, WPA1_TKIP, WPA1_AES, WPA2_AES, WPA2_TKIP, WPA2_AES_TKIP, WPS_Open, WPS_AES */
	u8	WepKeyIndex;		/* WEP key index */
	u8	PasswordLen;		/* WPAx pre-share key length */
	u8	Password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];	/* WPAx pre-share key */
} HTTPDAP_WIFI_INFO;

typedef struct _HTTPDAP_INFO
{
	HTTPDAP_LOGIN Login[HTTPDAP_MAX_LOGIN_NUM];
	HTTPDAP_ACTION ActionTemp[HTTPDAP_MAX_CONNECTS];
	HTTPDAP_ACTION Action;
	HTTPDAP_FILE_UPLOAD	Upload;	
	HTTPDAP_WIFI_INFO WifiInfo;
} HTTPDAP_INFO;

/* GLOBAL VARIABLES */
extern HTTPD_CALLBACK HTTPDAP_Cbs;

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
HTTPD_STATUS HTTPDAP_Init(HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS HTTPDAP_ResponseComplete(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS HTTPDAP_GetMethod(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS HTTPDAP_CgiScript(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS HTTPDAP_PostMethod(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
HTTPD_STATUS HTTPDAP_NewConn(HTTPD_SERVER_CONN *pConn, HTTPD_SERVER_INFO *pInfo);
void HTTPAP_Task(void);

#endif /* End of __HTTPDAP_H__ */


/* End of httpdap.h */
