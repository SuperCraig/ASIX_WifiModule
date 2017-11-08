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
 * Module Name:gconf_ap.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __GCONF_AP_H__
#define __GCONF_AP_H__
/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define GCONFIG_DEFAULT_DATA_PORT		5000  // Default data port for both server and client modes

// UDP broadcast with Device Server Manager utility
#define GCONFIG_UDP_BCAST_SERVER_PORT	25122 // UDP broadcast listening ports

//#define GCONFIG_WIFI_DEFAULT_BSSID		"AXM23001"
#define GCONFIG_MBI_WIFI_DEFAULT_SSID		"SD-TEST"        //Craig Default SSID
extern char* GCONFIG_WIFI_DEFAULT_BSSID;      //20170727 Craig
#define GCONFIG_DEFAULT_STATIC_IP		0xc0a80003
#define GCONFIG_DEFAULT_NETMASK			0xffffff00
#define GCONFIG_DEFAULT_GATEWAY			0xc0a80001

#define GCONFIG_DEFAULT_ADAP_THRES_ETSI	0xF5
#define GCONFIG_DEFAULT_ADAP_THRES_MKK	0x0A

#define MBI_SetFactoryEnable 1          //20170731 Craig for save the factory settings
#ifdef MBI_SetFactoryEnable
  #include "MBI_CommandProcess.h"
#endif 

typedef struct
{
	u8	WifiRfEnable;			/* RF enable */
	u8	WifiNetworkMode;		/* None, STA, AP, STA+AP, PROMISC */
	u8	WifiChannel;			/* Channel */
	u8	WifiSsidLen;			/* SSID length */
	u8	WifiSsid[33];			/* SSID */
	u8	WifiEncryptMode;		/* Open, WEP Open, WEP shared, WPA1_TKIP, WPA1_AES, WPA2_AES, WPA2_TKIP, WPA2_AES_TKIP, WPS_Open, WPS_AES */
	u8	WifiWepKeyIndex;		/* WEP key index */
	u8	WifiWepKeyLength;		/* WEP key length, 0:64 bits, 1:128 bits*/
	u8	WifiWep64Key[4][5];		/* WEP 64bit key */
	u8	WifiWep128Key[4][13];	/* WEP 128bit key */
	u8	WifiPreShareKeyLen;		/* WPAx pre-share key length */
	u8	WifiPreShareKey[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];	/* WPAx pre-share key */
	u8	WifiCountryId;			/* Country ID */
	u8	WifiOtherConfiguration;	/* Bit 0 = hidden SSID, Bit 1 ~ 7 = reserved */

} GCONFIG_WIFI;

typedef struct
{
	u8	FileName[GCONFIG_MAX_FILENAME_LEN];
	u8	FileNameLen;
	u8	Reserved[3];
} GCONFIG_UPGRADE;

typedef struct
{
	u8	Username[16];
	u8	Passwd[16];
	u8	Level;
	u8	Rsvd[3];
} GCONFIG_ADMIN;

typedef struct _GCONFIG_CFG_DATA
{
	u8					Signature[4];
	u16					Checksum;
	u16					SwCfgLength;
	GCONFIG_UPGRADE		UpgradeCfg;
	GCONFIG_WIFI		WifiConfig;
	GCONFIG_DEV_NAME	DevName;
	u8 					Reserved[5];
	u8					ClientConnectionMode;
	u16					Network;
	u32					DeviceStaticIP;
	u16					DeviceDataPktListenPort;
	u16					UrXmitDelay;
	u16					DeviceBroadcastListenPort;
	u16					ClientDestPort;
	u8 					Reserved2[4];
	u32					Netmask;
	u32					Gateway;
	u32					Dns;
	GCONFIG_URDef		UrConfig;
	u8					Option;
	u8					JtagOff;
	u16					EthernetTxTimer;
	GCONFIG_SMTP_CONFIG	Smtp;
	u8					DestHostName[36];
	GCONFIG_ADMIN		Admin;
	GCONFIG_DHCPSRV		DhcpSrv;
	GCONFIG_NTP_CONFIG	Ntp;
	u8					SimpleCfgPinCodeEnable;
	u8					Rs485Enable;
	u8					R2wMaxConnections;
	u8					Reserved3[1];
	GCONFIG_GOOGLE_NEST	GoogleNest;
	GCONFIG_BLUEMIX		Bluemix;
	GCONFIG_MODBUS		Modbus;
	GCONFIG_BLUEMIX_AR	BluemixAr;
	GCONFIG_AZURE		Azure;
	u8					NetworkBackupMode;
	u8					AdaptivityThreshold;
	u8					Reserved4[2];
        
        Command23Content                        MBI_FactorySettings;            //20170731 Craig
} GCONFIG_CFG_DATA;

extern GCONFIG_CFG_DATA gconfig_ConfigData;     //20170731 Craig

#endif /* End of __GCONF_AP_H__ */

/* End of gconf_ap.h */