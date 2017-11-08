/*
 ******************************************************************************
 *     Copyright (c) ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
/*=============================================================================
 * Module Name: filesys.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *=============================================================================
 */

#ifndef __FILESYS_H__
#define __FILESYS_H__

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"

/* NAMING CONSTANT DECLARATIONS */
#define FILE_IMG_ASIX			0
#define FILE_IMG_LOGO			1
#define FILE_IMG_TBBG			2
#define FILE_ADVANCED			3
#define FILE_AXCSS0			4
#define FILE_AXCSS1			5
#define FILE_AXJS0			6
#define FILE_BASIC			7
#define FILE_CGIREQ			8
#define FILE_HCTENT			9
#define FILE_INDEX			10
#define FILE_RTC			11
#define FILE_SECURITY			12
#define FILE_STATUS			13
#define FILE_SYSCONFIRM			14
#define FILE_SYSMSG			15
#define FILE_WIFI			16
#define FILE_WIFIWZ_CFIRM			17
#define FILE_WIFIWZ_KEY			18
#define FILE_WIFIWZ_SVY			19
#define FILE_WIFI_WZ			20

#define MAX_STORE_FILE_NUM	21
#define MAX_FILE_NAME_LEN	30

#define FILE_TYPE_HTML		0
#define FILE_TYPE_BIN		1
#define FILE_TYPE_JS		2
#define FILE_TYPE_CSS		3
#define FILE_TYPE_CGI		4

#define MAX_POST_RECORDS	108
#define MAX_POST_BUF_SUBMIT	9
#define MAX_POST_BUF_TEXT	61
#define MAX_POST_BUF_PASSWORD	7
#define MAX_POST_BUF_TAG	0
#define MAX_POST_BUF_RADIO	1
#define MAX_POST_BUF_SELECT	29
#define MAX_POST_BUF_CHECKBOX	1
#define MAX_POST_NAME_LEN	15
#define MAX_POST_VALUE_LEN	65
#define NORM_POST_VALUE_LEN	16
#define MAX_POST_COUNT	27//File id=7
#define MAX_DIVIDE_NUM	40//File id=7
#define MAX_SELECT_OPTION_NUM	25
#define MAX_RADIO_OPTION_NUM	2

#define POST_TYPE_SUBMIT	1
#define POST_TYPE_TEXT	2
#define POST_TYPE_RADIO	3
#define POST_TYPE_SELECT	4
#define POST_TYPE_TAG	5
#define POST_TYPE_PASSWORD	6
#define POST_TYPE_CHECKBOX	7

#define RECORD_SUBMIT_Upgrade		0
#define RECORD_TEXT_email_addr		1
#define RECORD_SELECT_smtp_security		2
#define RECORD_TEXT_smtp_port		3
#define RECORD_TEXT_email_from		4
#define RECORD_TEXT_email_to1		5
#define RECORD_TEXT_email_to2		6
#define RECORD_TEXT_email_to3		7
#define RECORD_SELECT_coldstart		8
#define RECORD_SELECT_authfail		9
#define RECORD_SELECT_ipchg		10
#define RECORD_SELECT_pswchg		11
#define RECORD_SUBMIT_Save		12
#define RECORD_SUBMIT_Apply		13
#define RECORD_SELECT_mbtcp_xfer		14
#define RECORD_TEXT_mbtcp_port		15
#define RECORD_TEXT_mb_rspTime		16
#define RECORD_TEXT_mb_frameIntv		17
#define RECORD_TEXT_mb_CharDelay		18
#define RECORD_SELECT_netback_mode		19
#define RECORD_TEXT_hideBackupMode		20
#define RECORD_SUBMIT_restore		21
#define RECORD_SUBMIT_reboot		22
#define RECORD_TEXT_dsm_name		23
#define RECORD_SELECT_dbr		24
#define RECORD_SELECT_data		25
#define RECORD_SELECT_parity		26
#define RECORD_SELECT_stop		27
#define RECORD_SELECT_flow		28
#define RECORD_SELECT_rs485		29
#define RECORD_SELECT_s2n_mode		30
#define RECORD_SELECT_conntype		31
#define RECORD_TEXT_txtimer		32
#define RECORD_SELECT_cs_mode		33
#define RECORD_TEXT_s_lstport		34
#define RECORD_SELECT_r2w_conns		35
#define RECORD_TEXT_c_deshn		36
#define RECORD_TEXT_c_desport		37
#define RECORD_SELECT_cliconnmode		38
#define RECORD_TEXT_static_ip		39
#define RECORD_TEXT_gateway_ip		40
#define RECORD_TEXT_mask		41
#define RECORD_TEXT_dns_ip		42
#define RECORD_SELECT_dhcpc		43
#define RECORD_SELECT_dhcps		44
#define RECORD_TEXT_dhcps_startip		45
#define RECORD_TEXT_dhcps_endip		46
#define RECORD_TEXT_dhcps_gw		47
#define RECORD_TEXT_dhcps_lt		48
#define RECORD_TEXT_cmd		49
#define RECORD_TEXT_username		50
#define RECORD_PASSWORD_password		51
#define RECORD_SUBMIT_Login		52
#define RECORD_RADIO_rtc_clb		53
#define RECORD_SELECT_dst_onoff		54
#define RECORD_TEXT_rtc_yy		55
#define RECORD_TEXT_rtc_mm		56
#define RECORD_TEXT_rtc_dd		57
#define RECORD_TEXT_rtc_hh		58
#define RECORD_TEXT_rtc_nn		59
#define RECORD_TEXT_rtc_ss		60
#define RECORD_SELECT_ntps_tz		61
#define RECORD_TEXT_ntps_name1		62
#define RECORD_TEXT_ntps_name2		63
#define RECORD_TEXT_ntps_name3		64
#define RECORD_TEXT_new_usn		65
#define RECORD_PASSWORD_old_psw		66
#define RECORD_PASSWORD_new_psw		67
#define RECORD_PASSWORD_cfm_psw		68
#define RECORD_TEXT_smtp_un		69
#define RECORD_PASSWORD_smtp_pw		70
#define RECORD_TEXT_gn_host		71
#define RECORD_TEXT_bm_host		72
#define RECORD_TEXT_bm_un		73
#define RECORD_PASSWORD_bm_pw		74
#define RECORD_TEXT_bm_orgid		75
#define RECORD_TEXT_bm_typid		76
#define RECORD_TEXT_bm_devid		77
#define RECORD_TEXT_bm_apiver		78
#define RECORD_TEXT_bm_apikey		79
#define RECORD_TEXT_bm_apitoken		80
#define RECORD_TEXT_az_host		81
#define RECORD_TEXT_az_hn		82
#define RECORD_TEXT_az_di		83
#define RECORD_PASSWORD_az_sk		84
#define RECORD_TEXT_az_et		85
#define RECORD_SELECT_network_mode		86
#define RECORD_SELECT_channel		87
#define RECORD_TEXT_ssid		88
#define RECORD_CHECKBOX_hide_ssid		89
#define RECORD_SELECT_security_mode		90
#define RECORD_SELECT_key_length		91
#define RECORD_SELECT_key_index		92
#define RECORD_TEXT_key_index_0		93
#define RECORD_TEXT_key_index_1		94
#define RECORD_TEXT_key_index_2		95
#define RECORD_TEXT_key_index_3		96
#define RECORD_TEXT_aes_passphrase		97
#define RECORD_SELECT_wz_dhcpc		98
#define RECORD_TEXT_wz_static_ip		99
#define RECORD_TEXT_wz_gateway_ip		100
#define RECORD_TEXT_wz_mask		101
#define RECORD_TEXT_wz_dns_ip		102
#define RECORD_SUBMIT_Accept		103
#define RECORD_SUBMIT_Cancel		104
#define RECORD_SELECT_wz_keyid		105
#define RECORD_TEXT_wz_key		106
#define RECORD_SUBMIT_Next		107

/* TYPE DECLARATIONS */
/*-------------------------------------------------------------*/
typedef struct _FILE_MANAGEMEMT
{
	U8_T			Occupy;
	U8_T			FType;
	U8_T			FName[MAX_FILE_NAME_LEN];
	U16_T			FileSize;
	U8_T			*PBuf;
	U8_T			CgiRef;
	void			*CgiCall;

} FILE_MANAGEMEMT;

/*-------------------------------------------------------------*/
typedef struct _POST_RECORD
{
	U8_T			Occupy;
	U8_T			PostType;
	U8_T			Name[MAX_POST_NAME_LEN];
	U8_T			NameLen;
	U8_T			FileIndex;
	U8_T			UpdateSelf;
	void*			PValue;

} POST_RECORD;

/*-------------------------------------------------------------*/
typedef struct _BUF_SUBMIT
{
	U8_T			Value[NORM_POST_VALUE_LEN];
	U8_T			DefaultVlaueLen;
	U8_T			IsApply;

} BUF_SUBMIT;

/*-------------------------------------------------------------*/
#if (MAX_POST_BUF_RADIO)
typedef struct _BUF_RADIO
{
	U16_T			Offset[MAX_RADIO_OPTION_NUM];
	U8_T			Value[MAX_RADIO_OPTION_NUM][NORM_POST_VALUE_LEN];
	U8_T			Length[MAX_RADIO_OPTION_NUM];
	U8_T			Count;
	U8_T			DefaultSet;
	U8_T			CurrentSet;
	U8_T			UserSet;

} BUF_RADIO;
#endif

/*-------------------------------------------------------------*/
typedef struct _BUF_TEXT
{
	U16_T			Offset;
	U8_T			*CurrValue;
	U8_T			*UserValue;
	U8_T			DefaultLength;
	U8_T			CurrLength;
	U8_T			UserLength;

} BUF_TEXT;

/*-------------------------------------------------------------*/
typedef struct _BUF_PASSWORD
{
	U16_T			Offset;
	U8_T			*CurrValue;
	U8_T			*UserValue;
	U8_T			DefaultLength;
	U8_T			CurrLength;
	U8_T			UserLength;

} BUF_PASSWORD;

/*-------------------------------------------------------------*/
typedef struct _BUF_TAG
{
	U16_T			Offset;
	U8_T			CurrValue[NORM_POST_VALUE_LEN];
	U8_T			DefaultLength;
	U8_T			CurrLength;

} BUF_TAG;

/*-------------------------------------------------------------*/
typedef struct _BUF_SELECT
{
	U16_T			Offset[MAX_SELECT_OPTION_NUM];
	U8_T			Count;
	U8_T			DefaultSet;
	U8_T			CurrentSet;
	U8_T			UserSet;

} BUF_SELECT;

/*-------------------------------------------------------------*/
typedef struct _BUF_CHECKBOX
{
	U16_T			Offset;
	U8_T			Value[NORM_POST_VALUE_LEN];
	U8_T			DefaultVlaueLen;
	U8_T			DefaultSet;
	U8_T			CurrentSet;
	U8_T			UserSet;

} BUF_CHECKBOX;

/*-------------------------------------------------------------*/
 typedef struct {
	U8_T* name;
	U8_T* val;
} entry;
	extern entry entries[];
	extern U8_T num_parms;

/*-------------------------------------------------------------*/

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
#if (MAX_STORE_FILE_NUM)
//extern const FILE_MANAGEMEMT FSYS_Manage[MAX_STORE_FILE_NUM];
extern FILE_MANAGEMEMT FSYS_Manage[MAX_STORE_FILE_NUM];         //20170726 Craig For changing the web page set
        
#endif
#if (MAX_POST_RECORDS)
extern POST_RECORD POST_Record[MAX_POST_RECORDS];
#endif

void FSYS_Init(void);
U8_T FSYS_FindFile(U8_T*);
U16_T FSYS_HtmlExpansion(U8_T);

#if (MAX_POST_RECORDS)
extern void (*fun_tbl[])(void*);

void FUN_TEXT_email_addr (void *pWebData);
void FUN_SELECT_smtp_security (void *pWebData);
void FUN_TEXT_smtp_port (void *pWebData);
void FUN_TEXT_email_from (void *pWebData);
void FUN_TEXT_email_to1 (void *pWebData);
void FUN_TEXT_email_to2 (void *pWebData);
void FUN_TEXT_email_to3 (void *pWebData);
void FUN_SELECT_coldstart (void *pWebData);
void FUN_SELECT_authfail (void *pWebData);
void FUN_SELECT_ipchg (void *pWebData);
void FUN_SELECT_pswchg (void *pWebData);
void FUN_SELECT_mbtcp_xfer (void *pWebData);
void FUN_TEXT_mbtcp_port (void *pWebData);
void FUN_TEXT_mb_rspTime (void *pWebData);
void FUN_TEXT_mb_frameIntv (void *pWebData);
void FUN_TEXT_mb_CharDelay (void *pWebData);
void FUN_SELECT_netback_mode (void *pWebData);
void FUN_TEXT_hideBackupMode (void *pWebData);
void FUN_TEXT_dsm_name (void *pWebData);
void FUN_SELECT_dbr (void *pWebData);
void FUN_SELECT_data (void *pWebData);
void FUN_SELECT_parity (void *pWebData);
void FUN_SELECT_stop (void *pWebData);
void FUN_SELECT_flow (void *pWebData);
void FUN_SELECT_rs485 (void *pWebData);
void FUN_SELECT_s2n_mode (void *pWebData);
void FUN_SELECT_conntype (void *pWebData);
void FUN_TEXT_txtimer (void *pWebData);
void FUN_SELECT_cs_mode (void *pWebData);
void FUN_TEXT_s_lstport (void *pWebData);
void FUN_SELECT_r2w_conns (void *pWebData);
void FUN_TEXT_c_deshn (void *pWebData);
void FUN_TEXT_c_desport (void *pWebData);
void FUN_SELECT_cliconnmode (void *pWebData);
void FUN_TEXT_static_ip (void *pWebData);
void FUN_TEXT_gateway_ip (void *pWebData);
void FUN_TEXT_mask (void *pWebData);
void FUN_TEXT_dns_ip (void *pWebData);
void FUN_SELECT_dhcpc (void *pWebData);
void FUN_SELECT_dhcps (void *pWebData);
void FUN_TEXT_dhcps_startip (void *pWebData);
void FUN_TEXT_dhcps_endip (void *pWebData);
void FUN_TEXT_dhcps_gw (void *pWebData);
void FUN_TEXT_dhcps_lt (void *pWebData);
void FUN_TEXT_cmd (void *pWebData);
void FUN_TEXT_username (void *pWebData);
void FUN_PASSWORD_password (void *pWebData);
void FUN_RADIO_rtc_clb (void *pWebData);
void FUN_SELECT_dst_onoff (void *pWebData);
void FUN_TEXT_rtc_yy (void *pWebData);
void FUN_TEXT_rtc_mm (void *pWebData);
void FUN_TEXT_rtc_dd (void *pWebData);
void FUN_TEXT_rtc_hh (void *pWebData);
void FUN_TEXT_rtc_nn (void *pWebData);
void FUN_TEXT_rtc_ss (void *pWebData);
void FUN_SELECT_ntps_tz (void *pWebData);
void FUN_TEXT_ntps_name1 (void *pWebData);
void FUN_TEXT_ntps_name2 (void *pWebData);
void FUN_TEXT_ntps_name3 (void *pWebData);
void FUN_TEXT_new_usn (void *pWebData);
void FUN_PASSWORD_old_psw (void *pWebData);
void FUN_PASSWORD_new_psw (void *pWebData);
void FUN_PASSWORD_cfm_psw (void *pWebData);
void FUN_TEXT_smtp_un (void *pWebData);
void FUN_PASSWORD_smtp_pw (void *pWebData);
void FUN_TEXT_gn_host (void *pWebData);
void FUN_TEXT_bm_host (void *pWebData);
void FUN_TEXT_bm_un (void *pWebData);
void FUN_PASSWORD_bm_pw (void *pWebData);
void FUN_TEXT_bm_orgid (void *pWebData);
void FUN_TEXT_bm_typid (void *pWebData);
void FUN_TEXT_bm_devid (void *pWebData);
void FUN_TEXT_bm_apiver (void *pWebData);
void FUN_TEXT_bm_apikey (void *pWebData);
void FUN_TEXT_bm_apitoken (void *pWebData);
void FUN_TEXT_az_host (void *pWebData);
void FUN_TEXT_az_hn (void *pWebData);
void FUN_TEXT_az_di (void *pWebData);
void FUN_PASSWORD_az_sk (void *pWebData);
void FUN_TEXT_az_et (void *pWebData);
void FUN_SELECT_network_mode (void *pWebData);
void FUN_SELECT_channel (void *pWebData);
void FUN_TEXT_ssid (void *pWebData);
void FUN_CHECKBOX_hide_ssid (void *pWebData);
void FUN_SELECT_security_mode (void *pWebData);
void FUN_SELECT_key_length (void *pWebData);
void FUN_SELECT_key_index (void *pWebData);
void FUN_TEXT_key_index_0 (void *pWebData);
void FUN_TEXT_key_index_1 (void *pWebData);
void FUN_TEXT_key_index_2 (void *pWebData);
void FUN_TEXT_key_index_3 (void *pWebData);
void FUN_TEXT_aes_passphrase (void *pWebData);
void FUN_SELECT_wz_dhcpc (void *pWebData);
void FUN_TEXT_wz_static_ip (void *pWebData);
void FUN_TEXT_wz_gateway_ip (void *pWebData);
void FUN_TEXT_wz_mask (void *pWebData);
void FUN_TEXT_wz_dns_ip (void *pWebData);
void FUN_SELECT_wz_keyid (void *pWebData);
void FUN_TEXT_wz_key (void *pWebData);
#endif

#endif /* End of __FILE_SYSTEM_H__ */


/* End of filesys.h */