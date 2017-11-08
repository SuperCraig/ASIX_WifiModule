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
 * Module Name:gconfig.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __GCONFIG_H__
#define __GCONFIG_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define GCONFIG_VERSION_STRING		"0.7.5"

#define FLASH_SYSTEM_DATA_BASE		0x9000
#define FLASH_CALIBRATION_BASE		0xA000
#define FLASH_UPGRADE_IMAGE_1_ADDR	0xB000
#define FLASH_APP_CONFIG_BASE		0x7F000
#define FLASH_SINGLE_IMAGE_ADDRESS	0x80000
#define FLASH_RESERVED_BASE			0x1F9000
#define FLASH_APP_CONFIG_BASE_2		0x1FE000

#define GCONFIG_NETWORK_SERVER			0x0000
#define GCONFIG_NETWORK_CLIENT			0x8000
#define GCONFIG_NETWORK_DHCP_ENABLE		0x4000
#define GCONFIG_NETWORK_DHCP_DISABLE	0x0000
#define GCONFIG_NETWORK_PROTO_UDP		0x0800
#define GCONFIG_NETWORK_PROTO_TCP		0x0400
#define GCONFIG_NETWORK_PROTO_UDP_BCAST	0x0100
#define GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP		0x0020
#define GCONFIG_NETWORK_RTC_TIME_SETUP_BY_MANUAL	0x0000
#define GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE	0x0010
#define GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_DISABLE	0x0000
#define GCONFIG_NETWORK_R2WMODEMASK		0x0003

#define GCONFIG_SMTP_EVENT_COLDSTART    0x0001
#define GCONFIG_SMTP_EVENT_WARMSTART    0x0002
#define GCONFIG_SMTP_EVENT_AUTH_FAIL    0x0004
#define GCONFIG_SMTP_EVENT_IP_CHANGED   0x0008
#define GCONFIG_SMTP_EVENT_PSW_CHANGED  0x0010

#define GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE        65
#define GCONFIG_MAX_FILENAME_LEN        64
#define GCONFIG_MAX_MAILADDR_LEN        36
#define GCONFIG_MAX_HOSTNAME_LEN        36
#define GCONFIG_MAX_CLOUD_HOSTNAME_LEN	64

#define GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN	65
#define GCONFIG_MAX_BLUEMIX_CLIENT_ID_LEN	65
#define GCONFIG_MAX_BLUEMIX_ORG_ID_LEN		7
#define GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN		6
#define GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN	12
#define GCONFIG_MAX_BLUEMIX_USERNAME_LEN	33
#define GCONFIG_MAX_BLUEMIX_PASSWORD_LEN	33

#define GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN		13
#define GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN		33
#define GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN	33

#define GCONFIG_MAX_AZURE_HOSTNAME_LEN		65
#define GCONFIG_MAX_AZURE_HUBNAME_LEN		33
#define GCONFIG_MAX_AZURE_DEVICE_ID_LEN		12
#define GCONFIG_MAX_AZURE_SIGNED_KEY_LEN	65
#define GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN	11
#define GCONFIG_MAX_AZURE_SAS_LEN			255

#define GCONFIG_MAX_TIME_ZONE			25
#define GCONFIG_MAX_NTP_SERVERS			3

#define GCONFIG_R2W_MODULE		0x00000001
#define GCONFIG_HTTPD_MODULE	0x00000002
#define GCONFIG_TFTP_MODULE		0x00000004
#define GCONFIG_SMTP_MODULE		0x00000008
#define GCONFIG_RFC2217_MODULE	0x00000010
#define GCONFIG_SNTP_MODULE		0x00000020
#define GCONFIG_HSURCMD_MODULE	0x00000040

#define	STA_SCANNED_BSS_MAX	24

/* TYPE DECLARATIONS */
typedef struct _GCONFIG_TIMEZONE
{
	s16 Offset;
	u8 Description[40];
} GCONFIG_TIMEZONE;

typedef struct
{
	u8	SrvHostName[GCONFIG_MAX_NTP_SERVERS][GCONFIG_MAX_HOSTNAME_LEN];
	u8	TimeZone;
	u8	Rsved[3];
} GCONFIG_NTP_CONFIG;

typedef struct _GCONFIG_GID
{
	u8	id[8];
} GCONFIG_GID;

typedef struct _GCONFIG_DEV_NAME
{
	u8	DevName[16];
} GCONFIG_DEV_NAME;

typedef struct _GCONFIG_SMTP_CONFIG
{
	u8	DomainName[GCONFIG_MAX_HOSTNAME_LEN];
	u8	FromAddr[GCONFIG_MAX_MAILADDR_LEN];
	u8	ToAddr1[GCONFIG_MAX_MAILADDR_LEN];
	u8	ToAddr2[GCONFIG_MAX_MAILADDR_LEN];
	u8	ToAddr3[GCONFIG_MAX_MAILADDR_LEN];
	u8	Username[GCONFIG_MAX_MAILADDR_LEN];
	u8	Password[GCONFIG_MAX_MAILADDR_LEN];
	u16	EventEnableBits;
	u16	Port;
	u8	Security;
	u8	Rsved[3];
} GCONFIG_SMTP_CONFIG;

typedef struct
{
	u32	IpPoolAddrStart;
	u32	IpPoolAddrEnd;
	u32	Netmask;
	u32	DefaultGateway;
	u16	LeaseTime;	/* minutes */
	u8	Status;		/* 0: disable   1: enable */
	u8	Rsvd;
} GCONFIG_DHCPSRV;

/* This is used for tool */
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
	u8	Rsvd[2];				/* Reserved for multiple of 4 bytes */
} GCONFIG_PKT_WIFI;

typedef struct
{
	u8 dataBits;
	u8 stopBits;
	u8 parity;
	u8 flowCtrl;
	u32 baudRate;
} GCONFIG_URDef;

typedef struct
{
	u8	HostName[GCONFIG_MAX_CLOUD_HOSTNAME_LEN];
	u8	UserName[GCONFIG_MAX_MAILADDR_LEN];
	u8	PassWord[GCONFIG_MAX_MAILADDR_LEN];
} GCONFIG_GOOGLE_NEST;

typedef struct
{
	u8	HostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];
	u8	OrganizationID[GCONFIG_MAX_BLUEMIX_ORG_ID_LEN];
	u8	TypeID[GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN];
	u8	DeviceID[GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN];
	u8	UserName[GCONFIG_MAX_BLUEMIX_USERNAME_LEN];
	u8	PassWord[GCONFIG_MAX_BLUEMIX_PASSWORD_LEN];
} GCONFIG_BLUEMIX;

typedef struct
{
	u8	ApiVersion[GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN];
	u8	ApiKey[GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN];
	u8	ApiToken[GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN];
	u8	Rsvd[1];
} GCONFIG_BLUEMIX_AR;

typedef struct
{
	u8	HostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];
	u8	HubName[GCONFIG_MAX_AZURE_HUBNAME_LEN];
	u8	DeviceID[GCONFIG_MAX_AZURE_DEVICE_ID_LEN];
	u8	SignedKey[GCONFIG_MAX_AZURE_SIGNED_KEY_LEN];
	u8	ExpiryTime[GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN];
	u8	Rsvd[2];
} GCONFIG_AZURE;

typedef struct
{
	/* For Modbus TCP */
	u16	ServerPortNumber;
	u8	XferMode;
	u8	SlaveId;

	/* For Modbus Serial */
	u16 ResponseTimeOut;
	u16 InterFrameDelay;
	u16 InterCharDelay;
	u8	Reserved[2];
} GCONFIG_MODBUS;

typedef struct _GCONFIG_CFG_PKT
{
	GCONFIG_GID			Gid;
	u8					Opcode;
	u8					Option;
	u8					DevName[16];
	u8					MacAddr[6];
	u8					Network[2];
	u8					DeviceCurrentIP[4];
	u8					DeviceStaticIP[4];
	u8					DeviceDataPktListenPort[2];
	u8					DeviceBroadcastListenPort[2];
	u8					ClientDestPort[2];
	u8					DestHostName[36];
	u8					Netmask[4];
	u8					Gateway[4];
	u8					Dns[4];
	GCONFIG_URDef		UrConfig;
	u8					DeviceStatus;
	u8					Rsved0;
	u8					EthernetTxTimer[2];
	GCONFIG_SMTP_CONFIG	Smtp;
	u8					Filename[GCONFIG_MAX_FILENAME_LEN];
	GCONFIG_PKT_WIFI	WifiConfig;
	GCONFIG_DHCPSRV		DhcpSrv;
	GCONFIG_NTP_CONFIG	Ntp;
	GCONFIG_MODBUS		Modbus;
} GCONFIG_CFG_PKT;

typedef enum _GCONFIG_WIFI_SECURITY_INDEX
{
	GCONFIG_WIFI_OPEN = 0,
	GCONFIG_WIFI_WEP,
	GCONFIG_WIFI_WPA2_AES_PSK,
//	GCONFIG_WIFI_WPA_WPA2_AUTO_PSK = GCONFIG_WIFI_WPA2_AES_PSK,
	GCONFIG_MAX_WIFI_SECURITY_TYPES,
} GCONFIG_WIFI_SECURITY_INDEX;

typedef enum _GCONFIG_SMTP_SECURITY
{
	GCONFIG_SMTP_NO_SECURITY = 0,
	GCONFIG_SMTP_SSL,
	GCONFIG_SMTP_TLS,
	GCONFIG_SMTP_AUTO,
	GCONFIG_MAX_SMTP_SECURITY,
} GCONFIG_SMTP_SECURITY;

typedef enum _GCONFIG_MODEBUS_GATEWAY_XFER_MODE
{
	GCONFIG_MBGW_TCP = 0,
	GCONFIG_MBGW_TRANSPARENT,
	GCONFIG_MAX_MBGW_XFER_MODE,
} GCONFIG_MODEBUS_GATEWAY_XFER_MODE;

typedef enum _GCONFIG_R2W_MODE
{
	R2WMODE_Socket = 0,
	R2WMODE_VCom,
	R2WMODE_Rfc2217,
	R2WMODE_MODBUS_GW,
	GCONFIG_MAX_R2W_MODE,
} GCONFIG_R2W_MODE;

typedef enum _GCONFIG_CLIENT_CONN_MODE
{
	GCONFIG_AUTO_CONNECTION = 0,
	GCONFIG_MANUAL_CONNECTION,
	GCONFIG_MAX_CLIENT_CONN_MODE,
} GCONFIG_CLIENT_CONN_MODE;

typedef enum {
	SINGLE_UPGRADE_IMAGE_MODE = 0,
	DUAL_UPGRADE_IMAGE_MODE,
} GCONFIG_UPGARDE_MODE;

typedef enum {
	GCONFIG_NETWORK_BACKUP_FORCE_WIFI = 0,
	GCONFIG_NETWORK_BACKUP_FORCE_ETH,
	GCONFIG_NETWORK_BACKUP_WIFI_AS_MAJOR,
	GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR,
} GCONFIG_NETWORK_BACKUP_MODE;

/* GLOBAL VARIABLES */
extern GCONFIG_GID GCONFIG_Gid;
extern const GCONFIG_TIMEZONE GCONFIG_TimeZoneTable[];
extern const u8 GCONFIG_WEEK_TABLE[7][10];


/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void GCONFIG_Init(void);
u32 GCONFIG_GetUpgradeImage2Offset(void);
int GCONFIG_GetUpgradeMode(void);
void GCONFIG_SysReboot(void);
void GCONFIG_SetConfigPacket(GCONFIG_CFG_PKT*);
void GCONFIG_GetConfigPacket(GCONFIG_CFG_PKT*);
void GCONFIG_WriteConfigData(void);
void GCONFIG_ReadDefaultConfigData(void);

void GCONFIG_ReadMBIdataFromROM(void);          //20170731 Craig Read the MBI Data From ROM

u32 GCONFIG_IpAddr2Ulong(u8* pBuf, u8 len);

void GCONFIG_SetDeviceName(GCONFIG_DEV_NAME*);
void GCONFIG_SetMacAddress(u8 *pAddr);
void GCONFIG_SetNetwork(u16);
void GCONFIG_SetDeviceStaticIP(u32);
void GCONFIG_SetDeviceDataPktListenPort(u16);
void GCONFIG_SetDeviceBroadcastListenPort(u16);
void GCONFIG_SetClientDestPort(u16);
void GCONFIG_SetNetmask(u32);
void GCONFIG_SetGateway(u32);
void GCONFIG_SetDNS(u32);
void GCONFIG_SetUrStopBits(u8 stopBits);
void GCONFIG_SetUrBaudRate(u32 baudRate);
void GCONFIG_SetUrDataBits(u8 dataBits);
void GCONFIG_SetUrFlowCtrl(u8 flowCtrl);
void GCONFIG_SetUrParity(u8 parity);
void GCONFIG_SetUrMode(u8 mode);
void GCONFIG_SetUrXmitDelay(u16 delay);
void GCONFIG_SetEthernetTxTimer(u16);
void GCONFIG_SetUdpAutoConnectClientIp(u32);
void GCONFIG_SetClientConnectionMode(u8);
u8 GCONFIG_SetFilename(u8 *filename, u8 len);
void GCONFIG_SetJtagOff(u8);
s16 GCONFIG_SetWifiCountryId(u8 val);

GCONFIG_DEV_NAME* GCONFIG_GetDeviceName(void);
u8 *GCONFIG_GetMacAddress(void);
u16 GCONFIG_GetNetwork(void);
u32 GCONFIG_GetDeviceStaticIP(void);
u16 GCONFIG_GetDeviceDataPktListenPort(void);
u16 GCONFIG_GetDeviceBroadcastListenPort(void);
u16 GCONFIG_GetClientDestPort(void);
u32 GCONFIG_GetNetmask(void);
u32 GCONFIG_GetGateway(void);
u32 GCONFIG_GetDNS(void);
u8 GCONFIG_GetUrStopBits(void);
u8 GCONFIG_GetUrDataBits(void);
u8 GCONFIG_GetUrParity(void);
u8 GCONFIG_GetUrFlowCtrl(void);
u32 GCONFIG_GetUrBaudRate(void);
u8 GCONFIG_GetUrMode(void);
u16 GCONFIG_GetUrXmitDelay(void);
u16 GCONFIG_GetEthernetTxTimer(void);
u32 GCONFIG_GetUdpAutoConnectClientIp(void);
u8 GCONFIG_GetClientConnectionMode(void);
u8 GCONFIG_GetJtagOff(void);
u8 GCONFIG_GetWifiCountryId(void);

u8 GCONFIG_GetWifiHiddenSsid(void);
s8 GCONFIG_SetWifiHiddenSsid(u8 val);

s16 GCONFIG_GetAdmin(u8 *username, u8 *passwd, u8 *level);
s16 GCONFIG_SetAdmin(u8 *username, u8 *passwd);

s16 GCONFIG_GetUser(u8 *username, u8* passwd, u8 *level);       //20170726 Craig Creat another account
s16 GCONFIG_SetUser(u8 *username, u8 *passwd);          //20170726 Craig Creat another account

void GCONFIG_SetSMTPDomainName(u8*, u8);
u8 GCONFIG_GetSMTPDomainName(u8*);
void GCONFIG_SetSMTPFrom(u8*, u8);
u8 GCONFIG_GetSMTPFrom(u8*);
void GCONFIG_SetSMTPTo1(u8*, u8);
u8 GCONFIG_GetSMTPTo1(u8*);
void GCONFIG_SetSMTPTo2(u8*, u8);
u8 GCONFIG_GetSMTPTo2(u8*);
void GCONFIG_SetSMTPTo3(u8*, u8);
u8 GCONFIG_GetSMTPTo3(u8*);
void GCONFIG_SetAutoWarning(u16);
u16 GCONFIG_GetAutoWarning(void);
void GCONFIG_GetSMTPSecurity(u8*, u16*);
void GCONFIG_SetSMTPSecurity(u8, u16);
void GCONFIG_GetSMTPAccount(u8 *pun, u16 unlen, u8 *pps, u16 pslen);
void GCONFIG_SetSMTPAccount(u8 *pus, u8 *pps);

void GCONFIG_SetDestHostName(u8*, u8);
u8 GCONFIG_GetDestHostName(u8*);
u8 GCONFIG_GetFilename(u8 *filename, u8 len);
u8 GCONFIG_GetWifiRfEnable(void);
void GCONFIG_SetWifiRfEnable(u8 on);
u8 GCONFIG_GetWifiNetworkMode(void);
void GCONFIG_SetWifiNetworkMode(u8 value);
u8 GCONFIG_GetWifiSsid(u8 *pOut);
void GCONFIG_SetWifiSsid(u8 *pIn, u8 len);
u8 GCONFIG_GetWifiEncryptMode(void);
u8 GCONFIG_SetWifiEncryptMode(u32 value);
u8 GCONFIG_GetWifiPreShareKey(u8 *pOut);
void GCONFIG_SetWifiPreShareKey(u8 *pIn, u8 len);
u32 GCONFIG_GetDhcpSrvStartIp(void);
void GCONFIG_SetDhcpSrvStartIp(u32 startIp);
u32 GCONFIG_GetDhcpSrvEndIp(void);
void GCONFIG_SetDhcpSrvEndIp(u32 endIp);
u32 GCONFIG_GetDhcpSrvNetmask(void);
void GCONFIG_SetDhcpSrvNetmask(u32 mask);
u32 GCONFIG_GetDhcpSrvDefGateway(void);
void GCONFIG_SetDhcpSrvDefGateway(u32 gateway);
u16 GCONFIG_GetDhcpSrvLeaseTime(void);
void GCONFIG_SetDhcpSrvLeaseTime(u16 lease);
u8 GCONFIG_GetDhcpSrvStatus(void);
void GCONFIG_SetDhcpSrvStatus(u8 status);

u8* GCONFIG_GetNtpSrvHostName(u8 index);
void GCONFIG_SetNtpSrvHostName(u8 index, u8 *pHostName);
s16 GCONFIG_GetNtpTimeZone(u8 *pTimeZoneIndex);
void GCONFIG_SetNtpTimeZone(u8 TimeZoneIndex);
u8 GCONFIG_GetSimpleCfgPinCodeStatus(void);
void GCONFIG_SetSimpleCfgPinCodeStatus(u8 status);
u8 GCONFIG_GetRs485Status(void);
void GCONFIG_SetRs485Status(u8 status);
u8 GCONFIG_GetChannelPlan(u8 countryId, u8 *pminChNum, u8 *pmaxChNum);
u8 GCONFIG_GetGoogleNestHostName(u8 *pHostName, u8 BufSize);
u8 GCONFIG_SetGoogleNestHostName(u8 *pHostName);
u8 GCONFIG_GetGoogleNestAccount(u8 *pUserName, u8 UnBufSize, u8 *pPassWord, u8 PwBufSize);
u8 GCONFIG_SetGoogleNestAccount(u8 *pUserName, u8 *pPassWord);
u8 GCONFIG_GetModbusTcp(u16 *pServerPortNumber, u8 *pSlaveId, u8 *pXferMode);
u8 GCONFIG_SetModbusTcp(u16 ServerPortNumber, u8 SlaveId, u8 pXferMode);
u8 GCONFIG_GetModbusSerialTiming(u16 *pResponseTimeOut, u16 *pInterFrameDelay, u16 *pInterCharDelay);
u8 GCONFIG_SetModbusSerialTiming(u16 ResponseTimeOut, u16 InterFrameDelay, u16 InterCharDelay);
u8 GCONFIG_GetBluemixHostName(u8 *pHostName, u8 BufSize);
u8 GCONFIG_SetBluemixHostName(u8 *pHostName);
u8 GCONFIG_GetBluemixAccount(u8 *pOrgID, u8 OiBufSize,
							 u8 *pTypeID, u8 TiBufSize,
							 u8 *pDeviceID, u8 DiBufSize,
							 u8 *pUserName, u8 UnBufSize,
							 u8 *pPassWord, u8 PwBufSize);
u8 GCONFIG_SetBluemixAccount(u8 *pOrgID, u8 *pTypeID, u8 *pDeviceID, u8 *pUserName, u8 *pPassWord);
u8 GCONFIG_GetBluemixAutoReg(u8 *pApiVersion, u8 ApiVersionBufSize,
							 u8 *pApiKey, u8 ApiKeyBufSize,
							 u8 *pApiToken, u8 ApiTokenBufSize);
u8 GCONFIG_SetBluemixAutoReg(u8 *pApiVersion, u8 *pApiKey, u8 *pApiToken);
u8 GCONFIG_GetAzureHostName(u8 *pHostName, u8 BufSize);
u8 GCONFIG_SetAzureHostName(u8 *pHostName);
u8 GCONFIG_GetAzureAccount(u8 *pHubName, u8 HnBufSize,
						   u8 *pDeviceID, u8 DiBufSize,
						   u8 *pSignedKey, u8 SignedKeyBufSize,
						   u8 *pExpiryTime, u8 ExpiryTimeBufSize);
u8 GCONFIG_SetAzureAccount(u8 *pHubName, u8 *pDeviceID, u8 *pSignedKey, u8 *pExpiryTime);
u8 GCONFIG_GetR2wMaxConnections(void);
int GCONFIG_SetR2wMaxConnections(u8 MaxConns);
u8 GCONFIG_GetNetworkBackupMode(void);
void GCONFIG_SetNetworkBackupMode(u8 mode);
#endif /* End of __GCONFIG_H__ */

/* End of gconfig.h */
