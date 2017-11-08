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
 * Module Name : httpapi.h
 * Purpose     : The header file of httpapi.c
 * Author      :
 * Date        :
 * Notes       :
 *=============================================================================
 */

#ifndef __HTTPAPI_H__
#define __HTTPAPI_H__

/* INCLUDE FILE DECLARATIONS */
#include "httpd.h"

/* NAMING CONSTANT DECLARATIONS */
#define HTTPAPI_VERSION				"HTTP/1.1"
	/* Response code definition */
		// Request successful code
#define STATUS_200_OK				"200 Ok"
	
		// Request redirected code
#define STATUS_302_FOUND			"302 Re-direction"
	
		// Request incomplete code
#define STATUS_400_BAD_REQUEST		"400 Bad_Request"
#define STATUS_401_UNAUTHEN			"401 Unauthorized"
#define STATUS_403_FORBIDDEN		"403 Forbidden"
#define STATUS_404_NOT_FOUND		"404 Not_Found"
#define STATUS_405_METHOD_NOT_ALLOW	"405 Method_Not_Allowed"
#define STATUS_406_NOT_ACCEPTABLE	"406 Not_Acceptable"

	/* Response code definition */
		// General headers
#define HDR_CACHE_CONTROL			"Cache-control: "
#define HDR_CONNECTION				"Connection: "
#define HDR_DATE					"Date: "
#define HDR_UPGRADE					"Upgrade: "

		// Request headers
#define HDR_ACCEPT					"Accept: "
#define HDR_ACCEPT_LANGUAGE			"Accept-language: "
#define HDR_ACCEPT_CHARSET			"Accept-charset: "
#define HDR_ACCEPT_ENCODING			"Accept-encoding: "
#define HDR_AUTHORIZATION			"Authorization: "
#define HDR_HOST					"Host: "
#define HDR_USER_AGENT				"User-agent: "
	
		// Entity headers
#define HDR_ALLOW					"Allow: "
#define HDR_CONTENT_ENCODING		"Content-encoding: "
#define HDR_CONTENT_LANGUAGE		"Content-language: "
#define HDR_CONTENT_LENGTH			"Content-Length: "
#define HDR_CONTENT_LOCATION		"Content-location: "
#define HDR_CONTENT_TYPE			"Content-Type: "
	
		// Response headers
#define HDR_LOCATION				"Location: "
#define HDR_WWW_AUTHENTICATE		"WWW-Authenticate: "
	
/* MACRO DECLARATIONS */
#define	HTTPAPI_MSGSEND(sockid, pmsg, msglen, flag) (send(sockid, pmsg, msglen, flag));
#define HTTPAPI_MsgBuild sprintf

/* TYPE DECLARATIONS */
typedef enum
{
	HTTPAPI_OK = 0,  
	HTTPAPI_ERR,
} HTTPAPI_STATUS;

typedef enum
{
	NO_METHOD = 0,  
	METHOD_POST,
	METHOD_GET,
	METHOD_PUT,	
	METHOD_DELETE,		
} HTTPAPI_METHODS;

typedef enum
{
	CTYPE_APP_URL_ENCODED = 0,
	CTYPE_APP_JAVASCRIPT,	
	CTYPE_APP_JSON,		
	CTYPE_IMAGE_JPEG,
	CTYPE_MULTIPART_FORM_DATA,
	CTYPE_TEXT_HTML,	
	CTYPE_TEXT_CSS,		
	MAX_CONTENT_TYPES,
} HTTPAPI_CONTENT_TYPES;

typedef enum
{
	CONNECTION_KEEP_ALIVE = 0,
	CONNECTION_UPGRADE,
	CONNECTION_CLOSE,	
	MAX_CONNECTION_TYPES,	
	HIDE_CONNECTION_HDR,		
} HTTPAPI_CONNECTION_TYPES;

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
extern const u8 HTTPAPI_METHOD_STRING[][10];
extern const u8 HTTPAPI_CONTENT_TYPE_STRING[][50];
extern const u8 HTTPAPI_CONNECTION_STRING[][20];

HTTPAPI_STATUS HTTPAPI_DecText2Char(U8_T *pbuf, U8_T len, U8_T *pChar);
HTTPAPI_STATUS HTTPAPI_DecText2Long(U8_T *pbuf, U8_T len, U32_T *pLong);
U16_T HTTPAPI_Str2Short(U8_T* pbuf, U8_T len);
U8_T HTTPAPI_Short2Str(U16_T port, U8_T* pBuf);
U8_T HTTPAPI_Ulong2IpAddr(U32_T ip, U8_T* pbuf);
U32_T HTTPAPI_IpAddr2Ulong(U8_T* pBuf, U8_T len);
U8_T HTTPAPI_Num2HexText(U8_T *pHexText, U8_T *pNum, U8_T NumLen);
U8_T HTTPAPI_HexText2Num(U8_T *pNum, U8_T *pHexText, U8_T HexTextLen);
U16_T HTTPAPI_MIMEbase64Decode(U8_T *pDst, U8_T *pSur, U16_T len);
U8_T HTTPAPI_UrlDecode(U8_T *psrc, U8_T srclen, U8_T *pdst);

	/* HTTP header process */
U8_T* HTTPAPI_FindStr(U8_T *pSrc, U16_T Range, U8_T *pRef, U16_T *Offset);
U8_T* HTTPAPI_GetHdrValue(U8_T *pSrc, U16_T Range, U8_T *pHdr, U16_T *ValueLen);
HTTPAPI_STATUS HTTPAPI_GetWwwAuthenBasic(U8_T *pSrc, U16_T SrcLen, U8_T **pUsername, U8_T **pPassword);

	/* HTTP response process */
HTTPAPI_STATUS HTTPAPI_OkRspSend(HTTPD_SERVER_CONN *pconn, U8_T *pbuf, U8_T fid);
HTTPAPI_STATUS HTTPAPI_UnAuthenRspSend(HTTPD_SERVER_CONN *pconn, U8_T *pbuf, U8_T *prealm_msg);
HTTPAPI_STATUS HTTPAPI_RedirRspSend(HTTPD_SERVER_CONN *pconn, U8_T *pbuf, U8_T fid);
HTTPAPI_STATUS HTTPAPI_IncompRspSend(HTTPD_SERVER_CONN *pconn, U8_T *pbuf, U8_T *pincomp_msg);
S16_T HTTPAPI_OkHdrBuild(U8_T *pbuf, U8_T fid);
HTTPAPI_STATUS HTTPAPI_MsgCombineSend(HTTPD_SERVER_CONN *pconn, U8_T *pbuf, U16_T len, U8_T fid);

	/* Filesys element access */
U8_T HTTPAPI_ElementUpdateChkByName(U8_T *pname);
U8_T HTTPAPI_GetElementValueByName(U8_T *pname, U8_T *pval, U8_T bufSize);
U8_T HTTPAPI_ElementUpdateChkById(U8_T eid);
U8_T HTTPAPI_GetElementValueById(U8_T eid, U8_T **pval);
U8_T HTTPAPI_SetElementValueById(U8_T eid, U8_T *pval, U8_T vallen);

#endif /* End of __HTTPAPI_H__ */


/* End of httpapi.h */
