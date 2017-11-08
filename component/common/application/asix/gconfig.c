/*
 ******************************************************************************
 *     Copyright (c) 2011	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */
 /*============================================================================
 * Module Name: gconfig.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include "autoconf.h"
#include "lwip_netconf.h"
#include "gconfig.h"
#include "gconf_ap.h"
#include "flash_api.h"
#include <string.h>
#include "wifi_constants.h"
#include "gs2w.h"
#include "duia.h"
#include "gtcpdat.h"
#include "rtc.h"
#include "MBI_CommandProcess.h"   

/* NAMING CONSTANT DECLARATIONS */
#define DEVICE_STATUS_IDLE			0
#define DEVICE_STATUS_CONNECTED		1

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */
/* GLOBAL VARIABLES DECLARATIONS */
extern struct netif xnetif[NET_IF_NUM];
//GCONFIG_GID GCONFIG_Gid = {'A','S','I','X','X','I','S','A'};
GCONFIG_GID GCONFIG_Gid = {'M','B','I','-','N','e','t',0x01};
xSemaphoreHandle gConfigMutex;

/* LOCAL VARIABLES DECLARATIONS */
static const u8 default_Wep64Key[4][5] = {0x31,0x32,0x33,0x34,0x35, 0x36,0x37,0x38,0x39,0x30,
                                            0x35,0x34,0x33,0x32,0x31, 0x30,0x39,0x38,0x37,0x36};
static const u8 default_Wep128Key[4][13] = {
0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32,
0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30};

const s32 security_map[] = {
	RTW_SECURITY_OPEN,
	RTW_SECURITY_WEP_PSK,
	RTW_SECURITY_WPA2_AES_PSK,
	RTW_SECURITY_WEP_SHARED,
	RTW_SECURITY_WPA_TKIP_PSK,
	RTW_SECURITY_WPA_AES_PSK,
	RTW_SECURITY_WPA2_TKIP_PSK,
	RTW_SECURITY_WPA2_MIXED_PSK,
};

typedef struct
{
	u8 regionCode;
	u8 lowestChannel;
	u8 highestChannel;
	u8 countryId;
} GCONFIG_WifiRegionChannelRange;

#define GCONFIG_NUM_REGION_CODE 7
const GCONFIG_WifiRegionChannelRange GCONFIG_WifiRegionChannelList[GCONFIG_NUM_REGION_CODE] =
{
	{0x20, 1, 13, RTW_COUNTRY_WORLD1},	/* 2G_WORLD: Worldwide 13 */
	{0x21, 1, 13, RTW_COUNTRY_ETSI1},	/* 2G_ETSI1: Europe 2G */
	{0x22, 1, 11, RTW_COUNTRY_FCC1},	/* 2G_FCC1: US 2G */
	{0x23, 1, 14, RTW_COUNTRY_MKK1},	/* 2G_MKK1: Japan 2G */
	{0x24, 10, 13, RTW_COUNTRY_ETSI2},	/* 2G_ETSI2: France 2G */
	{0x2A, 1, 13, RTW_COUNTRY_FCC2},	/* 2G_FCC2: US 2G */
	{0x58, 1, 13, RTW_COUNTRY_MKK2},	/* 2G_MKK2:  */
};

//static GCONFIG_CFG_DATA gconfig_ConfigData;   Craig
GCONFIG_CFG_DATA gconfig_ConfigData;    //20170731 Craig 

static GCONFIG_ADMIN UserAccount;       //20170726 Craig

SECTION(".sdram.data")
static GCONFIG_CFG_DATA bk_ConfigData;

//static u8	GCONFIG_wifiRegionIndex = 1; /* default value */
static u16	gConfigInit = 0;
static u32	gConfigAppConfigBase = 0;
static u32	gConfigUpgradeImage2Offset = 0;

char* GCONFIG_WIFI_DEFAULT_BSSID = "SD-TEST";   //20170727 Craig 

const GCONFIG_TIMEZONE GCONFIG_TimeZoneTable[GCONFIG_MAX_TIME_ZONE] =
{
	{-1200,	"GMT-12.0 Eniwetok, Kwajalein"},
	{-1100,	"GMT-11.0 Midway Is., Samoa"},
	{-1000,	"GMT-10.0 Hawaii"},
	{-900, 	"GMT-9.0 Alaska"},
	{-800,	"GMT-8.0 Los Angeles, Tijuana"},
	{-700,	"GMT-7.0 Denver Arizona"},
	{-600,	"GMT-6.0 Chicago, Mexico City"},
	{-500,	"GMT-5.0 New York, Bogota"},
	{-400,	"GMT-4.0 Santiago"},
	{-300,	"GMT-3.0 Brasilia, Montevideo"},
	{-200,	"GMT-2.0 Fernando de Noronha"},
	{-100,	"GMT-1.0 Azores"},
	{0,		"GMT+0.0 Lisbon, London"},
	{100,	"GMT+1.0 Berlin, Paris"},
	{200,	"GMT+2.0 Helsinki, Cairo"},
	{300,	"GMT+3.0 Moscow, Nairobi"},
	{400,	"GMT+4.0 Abu Dhabi, Baku"},
	{500,	"GMT+5.0 Karachi, Islamabad"},
	{600,	"GMT+6.0 Almaty, Dhaka"},
	{700,	"GMT+7.0 Bangkok, Jakarta"},
	{800,	"GMT+8.0 Taiwan, Hong Kong"},
	{900,	"GMT+9.0 Seoul, Tokyo"},
	{1000,	"GMT+10.0 Melbourne, Sydney"},
	{1100,	"GMT+11.0 Solomon Is."},
	{1200,	"GMT+12.0 Fiji, Wellington"},
};

const u8 GCONFIG_WEEK_TABLE[7][10] = {{"Sunday"},{"Monday"},{"Tuesday"},{"Wednesday"},{"Thursday"},{"Friday"},{"Saturday"}};
u32 GCONFIG_LibraryModule = (
				GCONFIG_R2W_MODULE |
				GCONFIG_HTTPD_MODULE |
				GCONFIG_TFTP_MODULE |
				GCONFIG_SMTP_MODULE |
				GCONFIG_RFC2217_MODULE |
				GCONFIG_SNTP_MODULE |
#if CONFIG_HSUART_COMMAND_MODE
				GCONFIG_HSURCMD_MODULE |
#endif
				0);

/* LOCAL SUBPROGRAM DECLARATIONS */
static BOOL	gconfig_ReStoreParameter(u32 addr, GCONFIG_CFG_DATA *pConfig, u16 len);
static BOOL gconfig_StoreParameter(u32 addr, GCONFIG_CFG_DATA *pSramBase, u16 len);
static u16 gconfig_Checksum(u16 *pBuf, u32 length);
static void	gconfig_DisplayConfigData(GCONFIG_CFG_DATA *pConfig);
static u8 gconfig_CheckRegionCode(u8 rCode);
static void gconfig_ReadConfigData(void);

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_Task
 * Purpose: Main function
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
//void GCONFIG_Task(void)
//{
//
//} /* End of GCONFIG_Task() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_Init
 * Purpose: Initialization
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_Init(void)
{
	printf("\n\rAXM23001 Serial to WiFi Firmware v%s %s\n\r", GCONFIG_VERSION_STRING, (GCONFIG_GetUpgradeMode() == SINGLE_UPGRADE_IMAGE_MODE) ? "(Single)":"(Dual)");

	gConfigMutex = xSemaphoreCreateMutex();
	gConfigInit = 1;
	gConfigAppConfigBase = (GCONFIG_GetUpgradeMode() == SINGLE_UPGRADE_IMAGE_MODE) ? FLASH_APP_CONFIG_BASE:FLASH_APP_CONFIG_BASE_2;
	// Restore last-saved configuration if applicable
	gconfig_ReadConfigData();

} /* End of GCONFIG_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUpgradeImage2Offset()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetUpgradeImage2Offset(void)
{

	if (gConfigUpgradeImage2Offset == 0)
	{
		gConfigUpgradeImage2Offset = DUIA_GetUpgradeImage2Offset();
	}

	return gConfigUpgradeImage2Offset;
} /* End of GCONFIG_GetUpgradeImage2Offset() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUpgradeMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int GCONFIG_GetUpgradeMode(void)
{
	if (GCONFIG_GetUpgradeImage2Offset() == FLASH_SINGLE_IMAGE_ADDRESS)
	{
		return SINGLE_UPGRADE_IMAGE_MODE;
	}

	return DUAL_UPGRADE_IMAGE_MODE;
} /* End of GCONFIG_GetUpgradeMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SysReboot()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SysReboot(void)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
} /* End of GCONFIG_SysReboot() */

/////////////////////////////////////////////////////////////////////////////////

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetConfigPacket
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetConfigPacket(GCONFIG_CFG_PKT* pCfgPkt)
{
	u8 i;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Option = pCfgPkt->Option;
 	memcpy((char *)&gconfig_ConfigData.DevName, pCfgPkt->DevName, 16);
	gconfig_ConfigData.Network = (u16)pCfgPkt->Network[1] | ((u16)pCfgPkt->Network[0] << 8);

	/* Auto configure DHCP client while switching wifi mode */
	if (gconfig_ConfigData.WifiConfig.WifiNetworkMode != pCfgPkt->WifiConfig.WifiNetworkMode)
	{
		if (pCfgPkt->WifiConfig.WifiNetworkMode == RTW_MODE_AP)//STA->AP
		{
			gconfig_ConfigData.Network &= (~GCONFIG_NETWORK_DHCP_ENABLE);
			pCfgPkt->DhcpSrv.Status = 1;
		}
		else//AP->STA
		{
			//gconfig_ConfigData.Network |= GCONFIG_NETWORK_DHCP_ENABLE;
			//pCfgPkt->DhcpSrv.Status = 0;
                        gconfig_ConfigData.Network &= GCONFIG_NETWORK_DHCP_ENABLE;      //20170728 Craig DHCP setting  
                        pCfgPkt->DhcpSrv.Status = 1;    //20170728 Craig DHCP setting
		}
	}

	gconfig_ConfigData.DeviceStaticIP = (u32)pCfgPkt->DeviceStaticIP[3] | (u32)pCfgPkt->DeviceStaticIP[2] << 8
										| (u32)pCfgPkt->DeviceStaticIP[1] << 16 | (u32)pCfgPkt->DeviceStaticIP[0] << 24;
	gconfig_ConfigData.DeviceDataPktListenPort = (u16)pCfgPkt->DeviceDataPktListenPort[1] | (u16)pCfgPkt->DeviceDataPktListenPort[0] << 8;
	gconfig_ConfigData.DeviceBroadcastListenPort = (u16)pCfgPkt->DeviceBroadcastListenPort[1] | (u16)pCfgPkt->DeviceBroadcastListenPort[0] << 8;
	gconfig_ConfigData.ClientDestPort = (u16)pCfgPkt->ClientDestPort[1] | (u16)pCfgPkt->ClientDestPort[0] << 8;
	gconfig_ConfigData.Netmask = (u32)pCfgPkt->Netmask[3] | (u32)pCfgPkt->Netmask[2] << 8
								| (u32)pCfgPkt->Netmask[1] << 16 | (u32)pCfgPkt->Netmask[0] << 24;
	gconfig_ConfigData.Gateway = (u32)pCfgPkt->Gateway[3] | (u32)pCfgPkt->Gateway[2] << 8
								| (u32)pCfgPkt->Gateway[1] << 16 | (u32)pCfgPkt->Gateway[0] << 24;
	gconfig_ConfigData.Dns = (u32)pCfgPkt->Dns[3] | (u32)pCfgPkt->Dns[2] << 8
								| (u32)pCfgPkt->Dns[1] << 16 | (u32)pCfgPkt->Dns[0] << 24;
	gconfig_ConfigData.EthernetTxTimer = (u16)pCfgPkt->EthernetTxTimer[1] | ((u16)pCfgPkt->EthernetTxTimer[0] << 8);

	memcpy(&gconfig_ConfigData.UrConfig, &pCfgPkt->UrConfig, sizeof(GCONFIG_URDef));
    memcpy((char *)&gconfig_ConfigData.Smtp, (char *)&pCfgPkt->Smtp, sizeof(GCONFIG_SMTP_CONFIG));
    memcpy((char *)gconfig_ConfigData.DestHostName, (char *)pCfgPkt->DestHostName, 36);

    memcpy((char *)gconfig_ConfigData.UpgradeCfg.FileName, (char *)pCfgPkt->Filename, 63);
    gconfig_ConfigData.UpgradeCfg.FileName[63] = 0;
    gconfig_ConfigData.UpgradeCfg.FileNameLen = strlen(gconfig_ConfigData.UpgradeCfg.FileName);

	gconfig_ConfigData.WifiConfig.WifiRfEnable = pCfgPkt->WifiConfig.WifiRfEnable;
	gconfig_ConfigData.WifiConfig.WifiNetworkMode = pCfgPkt->WifiConfig.WifiNetworkMode;
	if (gconfig_ConfigData.WifiConfig.WifiNetworkMode != RTW_MODE_STA)
		gconfig_ConfigData.WifiConfig.WifiChannel = pCfgPkt->WifiConfig.WifiChannel;
	gconfig_ConfigData.WifiConfig.WifiSsidLen = pCfgPkt->WifiConfig.WifiSsidLen;
	memcpy((char *)gconfig_ConfigData.WifiConfig.WifiSsid, (char *)pCfgPkt->WifiConfig.WifiSsid, 33);
	if (pCfgPkt->WifiConfig.WifiEncryptMode >= GCONFIG_MAX_WIFI_SECURITY_TYPES)
		pCfgPkt->WifiConfig.WifiEncryptMode = GCONFIG_WIFI_WPA2_AES_PSK;
	gconfig_ConfigData.WifiConfig.WifiEncryptMode = pCfgPkt->WifiConfig.WifiEncryptMode;
	gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen = pCfgPkt->WifiConfig.WifiPreShareKeyLen;
	memcpy((char *)gconfig_ConfigData.WifiConfig.WifiPreShareKey, (char *)pCfgPkt->WifiConfig.WifiPreShareKey, GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE);

	gconfig_ConfigData.WifiConfig.WifiWepKeyIndex = pCfgPkt->WifiConfig.WifiWepKeyIndex;
    gconfig_ConfigData.WifiConfig.WifiWepKeyLength = pCfgPkt->WifiConfig.WifiWepKeyLength;
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[0], (char *)pCfgPkt->WifiConfig.WifiWep64Key[0], 5);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[1], (char *)pCfgPkt->WifiConfig.WifiWep64Key[1], 5);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[2], (char *)pCfgPkt->WifiConfig.WifiWep64Key[2], 5);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[3], (char *)pCfgPkt->WifiConfig.WifiWep64Key[3], 5);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[0], (char *)pCfgPkt->WifiConfig.WifiWep128Key[0], 13);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[1], (char *)pCfgPkt->WifiConfig.WifiWep128Key[1], 13);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[2], (char *)pCfgPkt->WifiConfig.WifiWep128Key[2], 13);
    memcpy((char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[3], (char *)pCfgPkt->WifiConfig.WifiWep128Key[3], 13);

    gconfig_ConfigData.DhcpSrv.IpPoolAddrStart = pCfgPkt->DhcpSrv.IpPoolAddrStart;
    gconfig_ConfigData.DhcpSrv.IpPoolAddrEnd = pCfgPkt->DhcpSrv.IpPoolAddrEnd;
    gconfig_ConfigData.DhcpSrv.Netmask = pCfgPkt->DhcpSrv.Netmask;
    gconfig_ConfigData.DhcpSrv.DefaultGateway = pCfgPkt->DhcpSrv.DefaultGateway;
    gconfig_ConfigData.DhcpSrv.LeaseTime = pCfgPkt->DhcpSrv.LeaseTime;
    gconfig_ConfigData.DhcpSrv.Status = pCfgPkt->DhcpSrv.Status;

    gconfig_ConfigData.Ntp.TimeZone = pCfgPkt->Ntp.TimeZone;
    for (i=0; i<GCONFIG_MAX_NTP_SERVERS; i++)
        strcpy(gconfig_ConfigData.Ntp.SrvHostName[i], pCfgPkt->Ntp.SrvHostName[i]);

    gconfig_ConfigData.Modbus.ServerPortNumber = pCfgPkt->Modbus.ServerPortNumber;
    gconfig_ConfigData.Modbus.XferMode = pCfgPkt->Modbus.XferMode;
    gconfig_ConfigData.Modbus.ResponseTimeOut = pCfgPkt->Modbus.ResponseTimeOut;
    gconfig_ConfigData.Modbus.InterFrameDelay = pCfgPkt->Modbus.InterFrameDelay;
    gconfig_ConfigData.Modbus.InterCharDelay = pCfgPkt->Modbus.InterCharDelay;
	xSemaphoreGive(gConfigMutex);

	GCONFIG_WriteConfigData();
} /* End of GCONFIG_SetConfigPacket() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetConfigPacket
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_GetConfigPacket(GCONFIG_CFG_PKT* pCfgPkt)
{
	u8 i;

    memcpy((char *)&pCfgPkt->Gid, (char *)&GCONFIG_Gid, sizeof(GCONFIG_Gid));
	pCfgPkt->Option = gconfig_ConfigData.Option;
    memcpy(pCfgPkt->DevName, (char *)&gconfig_ConfigData.DevName, 16);
    memcpy(pCfgPkt->MacAddr, GCONFIG_GetMacAddress(), 6);
	pCfgPkt->Network[0] = (u8)(gconfig_ConfigData.Network >> 8);
	pCfgPkt->Network[1] = (u8)(gconfig_ConfigData.Network);
	*((u32*)pCfgPkt->DeviceCurrentIP) = MISC_GetCurrentIPAddr();

	pCfgPkt->DeviceStaticIP[0] = (u8)(gconfig_ConfigData.DeviceStaticIP >> 24);
	pCfgPkt->DeviceStaticIP[1] = (u8)(gconfig_ConfigData.DeviceStaticIP >> 16);
	pCfgPkt->DeviceStaticIP[2] = (u8)(gconfig_ConfigData.DeviceStaticIP >> 8);
	pCfgPkt->DeviceStaticIP[3] = (u8)(gconfig_ConfigData.DeviceStaticIP);

	pCfgPkt->DeviceDataPktListenPort[0] = (u8)(gconfig_ConfigData.DeviceDataPktListenPort >> 8);
	pCfgPkt->DeviceDataPktListenPort[1] = (u8)(gconfig_ConfigData.DeviceDataPktListenPort);

	pCfgPkt->DeviceBroadcastListenPort[0] = (u8)(gconfig_ConfigData.DeviceBroadcastListenPort >> 8);
	pCfgPkt->DeviceBroadcastListenPort[1] = (u8)(gconfig_ConfigData.DeviceBroadcastListenPort);

	pCfgPkt->ClientDestPort[0] = (u8)(gconfig_ConfigData.ClientDestPort >> 8);
	pCfgPkt->ClientDestPort[1] = (u8)(gconfig_ConfigData.ClientDestPort);

	pCfgPkt->Netmask[0] = (u8)(gconfig_ConfigData.Netmask >> 24);
	pCfgPkt->Netmask[1] = (u8)(gconfig_ConfigData.Netmask >> 16);
	pCfgPkt->Netmask[2] = (u8)(gconfig_ConfigData.Netmask >> 8);
	pCfgPkt->Netmask[3] = (u8)(gconfig_ConfigData.Netmask);

	pCfgPkt->Gateway[0] = (u8)(gconfig_ConfigData.Gateway >> 24);
	pCfgPkt->Gateway[1] = (u8)(gconfig_ConfigData.Gateway >> 16);
	pCfgPkt->Gateway[2] = (u8)(gconfig_ConfigData.Gateway >> 8);
	pCfgPkt->Gateway[3] = (u8)(gconfig_ConfigData.Gateway);

	pCfgPkt->Dns[0] = (u8)(gconfig_ConfigData.Dns >> 24);
	pCfgPkt->Dns[1] = (u8)(gconfig_ConfigData.Dns >> 16);
	pCfgPkt->Dns[2] = (u8)(gconfig_ConfigData.Dns >> 8);
	pCfgPkt->Dns[3] = (u8)(gconfig_ConfigData.Dns);

	pCfgPkt->EthernetTxTimer[0] = (u8)(gconfig_ConfigData.EthernetTxTimer >> 8);
	pCfgPkt->EthernetTxTimer[1] = (u8)(gconfig_ConfigData.EthernetTxTimer);

	memcpy(&pCfgPkt->UrConfig, &gconfig_ConfigData.UrConfig, sizeof(GCONFIG_URDef));
    memcpy((char *)&pCfgPkt->Smtp, (char *)&gconfig_ConfigData.Smtp, sizeof(GCONFIG_SMTP_CONFIG));
    memcpy((char *)pCfgPkt->DestHostName, (char *)gconfig_ConfigData.DestHostName, 36);
 	memcpy((char *)&pCfgPkt->Filename[0], (char *)&gconfig_ConfigData.UpgradeCfg.FileName[0], 64);

	pCfgPkt->WifiConfig.WifiRfEnable = gconfig_ConfigData.WifiConfig.WifiRfEnable;
	if (GS2W_GetTaskState() == GS2W_STATE_IDLE)
	{
		pCfgPkt->DeviceStatus = DEVICE_STATUS_IDLE;
	}
	else
	{
		pCfgPkt->DeviceStatus = DEVICE_STATUS_CONNECTED;
	}

	pCfgPkt->WifiConfig.WifiNetworkMode = gconfig_ConfigData.WifiConfig.WifiNetworkMode;
	if (gconfig_ConfigData.WifiConfig.WifiNetworkMode == RTW_MODE_STA)
		wifi_get_channel(&(pCfgPkt->WifiConfig.WifiChannel));
	else
		pCfgPkt->WifiConfig.WifiChannel = gconfig_ConfigData.WifiConfig.WifiChannel;

	pCfgPkt->WifiConfig.WifiSsidLen = gconfig_ConfigData.WifiConfig.WifiSsidLen;
	memcpy((char *)pCfgPkt->WifiConfig.WifiSsid, (char *)gconfig_ConfigData.WifiConfig.WifiSsid, 33);
	pCfgPkt->WifiConfig.WifiEncryptMode = gconfig_ConfigData.WifiConfig.WifiEncryptMode;

	pCfgPkt->WifiConfig.WifiWepKeyIndex = gconfig_ConfigData.WifiConfig.WifiWepKeyIndex;
    pCfgPkt->WifiConfig.WifiWepKeyLength = gconfig_ConfigData.WifiConfig.WifiWepKeyLength;
	memcpy((char *)pCfgPkt->WifiConfig.WifiWep64Key[0], (char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[0], 5);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep64Key[1], (char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[1], 5);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep64Key[2], (char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[2], 5);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep64Key[3], (char *)gconfig_ConfigData.WifiConfig.WifiWep64Key[3], 5);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep128Key[0], (char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[0], 13);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep128Key[1], (char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[1], 13);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep128Key[2], (char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[2], 13);
    memcpy((char *)pCfgPkt->WifiConfig.WifiWep128Key[3], (char *)gconfig_ConfigData.WifiConfig.WifiWep128Key[3], 13);

	pCfgPkt->WifiConfig.WifiPreShareKeyLen = gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen;
	memcpy((char *)pCfgPkt->WifiConfig.WifiPreShareKey, (char *)gconfig_ConfigData.WifiConfig.WifiPreShareKey, GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE);

    pCfgPkt->DhcpSrv.IpPoolAddrStart = gconfig_ConfigData.DhcpSrv.IpPoolAddrStart;
    pCfgPkt->DhcpSrv.IpPoolAddrEnd = gconfig_ConfigData.DhcpSrv.IpPoolAddrEnd;
    pCfgPkt->DhcpSrv.Netmask = gconfig_ConfigData.DhcpSrv.Netmask;
    pCfgPkt->DhcpSrv.DefaultGateway = gconfig_ConfigData.DhcpSrv.DefaultGateway;
    pCfgPkt->DhcpSrv.LeaseTime = gconfig_ConfigData.DhcpSrv.LeaseTime;
    pCfgPkt->DhcpSrv.Status = gconfig_ConfigData.DhcpSrv.Status;

    pCfgPkt->Ntp.TimeZone = gconfig_ConfigData.Ntp.TimeZone;
    for (i=0; i<GCONFIG_MAX_NTP_SERVERS; i++)
        strcpy(pCfgPkt->Ntp.SrvHostName[i], gconfig_ConfigData.Ntp.SrvHostName[i]);

    pCfgPkt->Modbus.ServerPortNumber = gconfig_ConfigData.Modbus.ServerPortNumber;
    pCfgPkt->Modbus.XferMode = gconfig_ConfigData.Modbus.XferMode;
    pCfgPkt->Modbus.SlaveId = gconfig_ConfigData.Modbus.SlaveId;
    pCfgPkt->Modbus.ResponseTimeOut = gconfig_ConfigData.Modbus.ResponseTimeOut;
    pCfgPkt->Modbus.InterFrameDelay = gconfig_ConfigData.Modbus.InterFrameDelay;
    pCfgPkt->Modbus.InterCharDelay = gconfig_ConfigData.Modbus.InterCharDelay;
} /* End of GCONFIG_GetConfigPacket() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: gconfig_ReadConfigData
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void gconfig_ReadConfigData(void)
{
	BOOL bRet;
        u8 i;
        
	printf("Read configuration data from addr: 0x%x", gConfigAppConfigBase);
	bRet = gconfig_ReStoreParameter(gConfigAppConfigBase, &gconfig_ConfigData, sizeof(gconfig_ConfigData));       //20170725 Craig
        
        strcpy(gconfig_ConfigData.Admin.Username, "Admin");      //20170725 Craig To set the wifi module username
        strcpy(gconfig_ConfigData.Admin.Passwd, "admin");        //20170725 Craig To set the wifi module password
        
        strcpy(UserAccount.Username, "user");      //20170726 Craig Initialize the user account username
        strcpy(UserAccount.Passwd, "user");        //20170726 Craig Initialize the user account password
        UserAccount.Level = 5;        //20170726 Craig Initialize the user account password
        
	if (bRet == FALSE)
	{
		GCONFIG_ReadDefaultConfigData();
		GCONFIG_WriteConfigData();
		gconfig_DisplayConfigData(&gconfig_ConfigData);
	}
        
} /* End of gconfig_ReadConfigData() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_WriteConfigData
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_WriteConfigData(void)      //20170727 Craig Write the struct data into ROM
{
	if (!gConfigInit)
		return;
        
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_StoreParameter(gConfigAppConfigBase, &gconfig_ConfigData, (u16)sizeof(gconfig_ConfigData));
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_WriteConfigData() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_ReadDefaultConfigData
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_ReadDefaultConfigData(void)        //20170727 Craig Read the Rom struct data
{       
        u8 i;
	printf("Read default configuration data. \n\r");

	memset((char *)&gconfig_ConfigData, 0, sizeof(gconfig_ConfigData));
	gconfig_ConfigData.Signature[0] = 'A';
	gconfig_ConfigData.Signature[1] = 'S';
	gconfig_ConfigData.Signature[2] = 'I';
	gconfig_ConfigData.Signature[3] = 'X';
	
	//strcpy((char *)gconfig_ConfigData.UpgradeCfg.FileName, "ota_r2w.bin");			/* File name for firmware upgrade */
	strcpy((char *)gconfig_ConfigData.UpgradeCfg.FileName, "MBIFirmware.bin");			/* File name for firmware upgrade */
	gconfig_ConfigData.UpgradeCfg.FileNameLen = strlen(gconfig_ConfigData.UpgradeCfg.FileName);

	gconfig_ConfigData.WifiConfig.WifiRfEnable = 1;							/* 0:Disable
																			   1:Enable */
	gconfig_ConfigData.WifiConfig.WifiNetworkMode = RTW_MODE_STA;			/* 1:RTW_MODE_STA
																			   2:RTW_MODE_AP */
	gconfig_ConfigData.WifiConfig.WifiChannel = 11;							/* 1 ~ 14 */

#if CONFIG_ASSIGN_DEFAULT_SSID_WITH_MAC
	{
		u8 *pmac;
		pmac = GCONFIG_GetMacAddress();
		gconfig_ConfigData.WifiConfig.WifiSsidLen = snprintf(gconfig_ConfigData.WifiConfig.WifiSsid, 
														 sizeof(gconfig_ConfigData.WifiConfig.WifiSsid), 
														 "%s_%02x%02x%02x", GCONFIG_MBI_WIFI_DEFAULT_SSID, pmac[3], pmac[4], pmac[5]);
	}
#else
	gconfig_ConfigData.WifiConfig.WifiSsidLen = strlen(GCONFIG_MBI_WIFI_DEFAULT_SSID);			/* 1 ~ 32 */
	strcpy(gconfig_ConfigData.WifiConfig.WifiSsid, GCONFIG_MBI_WIFI_DEFAULT_SSID);				/* 1 ~ 32 characters */
#endif

	gconfig_ConfigData.WifiConfig.WifiEncryptMode = GCONFIG_WIFI_OPEN;		/* 0:GCONFIG_WIFI_OPEN
																			   1:GCONFIG_WIFI_WEP
																			   2:GCONFIG_WIFI_WPA2_AES_PSK */

	gconfig_ConfigData.WifiConfig.WifiWepKeyIndex = 0;						/* WEP key index, 0 ~ 3 */
	gconfig_ConfigData.WifiConfig.WifiWepKeyLength = 0;						/* WEP key length, 0:64 bits, 1:128 bits*/

	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep64Key[0][0], (char *)&default_Wep64Key[0][0], 5);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep64Key[1][0], (char *)&default_Wep64Key[1][0], 5);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep64Key[2][0], (char *)&default_Wep64Key[2][0], 5);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep64Key[3][0], (char *)&default_Wep64Key[3][0], 5);

	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep128Key[0][0], (char *)&default_Wep128Key[0][0], 13);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep128Key[1][0], (char *)&default_Wep128Key[1][0], 13);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep128Key[2][0], (char *)&default_Wep128Key[2][0], 13);
	memcpy((char *)&gconfig_ConfigData.WifiConfig.WifiWep128Key[3][0], (char *)&default_Wep128Key[3][0], 13);

	gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen = 8;						/* 5 or 13: WEP
																			   	   8 ~ 63: WPA1/WPA2 PSK*/
	strcpy((char *)gconfig_ConfigData.WifiConfig.WifiPreShareKey, "12345678");	/* 8 ~ 63 characters */
	gconfig_ConfigData.WifiConfig.WifiCountryId = RTW_COUNTRY_WORLD1;			/* WORLD1 */
	gconfig_ConfigData.AdaptivityThreshold = 0;									/* If WifiCountryId = RTW_COUNTRY_ETSI1 or RTW_COUNTRY_ETSI2,
																				   please set default adaptivity threshold = 0xF5.
																				   If WifiCountryId = RTW_COUNTRY_MKK1 or RTW_COUNTRY_MKK2,
                                                                                                                                                                    please set default adaptivity threshold = 0x0A. */
        
	gconfig_ConfigData.WifiConfig.WifiOtherConfiguration = 0;					/* 0: Normal SSID 1: Hidden SSID */

	gconfig_ConfigData.Checksum = 0x00;
    gconfig_ConfigData.SwCfgLength = sizeof(gconfig_ConfigData);
	gconfig_ConfigData.ClientConnectionMode = GCONFIG_AUTO_CONNECTION;
	gconfig_ConfigData.Option = 0;
 	strcpy(&gconfig_ConfigData.DevName, "DSM1");

	gconfig_ConfigData.Network = (GCONFIG_NETWORK_SERVER | GCONFIG_NETWORK_PROTO_TCP | GCONFIG_NETWORK_PROTO_UDP_BCAST | R2WMODE_Socket);

	gconfig_ConfigData.DeviceStaticIP = GCONFIG_DEFAULT_STATIC_IP;	// 192.168.0.3
	gconfig_ConfigData.DeviceDataPktListenPort = GCONFIG_DEFAULT_DATA_PORT;
	gconfig_ConfigData.DeviceBroadcastListenPort = GCONFIG_UDP_BCAST_SERVER_PORT;
//	gconfig_ConfigData.ClientDestIP = 0xc0a80002; 	// 192.168.0.2
	gconfig_ConfigData.ClientDestPort = GCONFIG_DEFAULT_DATA_PORT;
	gconfig_ConfigData.Netmask = GCONFIG_DEFAULT_NETMASK; 		// 255.255.255.0
	gconfig_ConfigData.Gateway = GCONFIG_DEFAULT_GATEWAY; 		// 192.168.0.1
	gconfig_ConfigData.Dns = 0xa85f0101; 			// 168.95.1.1

	gconfig_ConfigData.UrXmitDelay = 0;
	gconfig_ConfigData.UrConfig.stopBits = 1; //1~2; 1:one stop bit; 2:two stop bit
	gconfig_ConfigData.UrConfig.dataBits = 8; //7~8; 8:8 bit data length, 7:7 bit data length
	gconfig_ConfigData.UrConfig.parity = 0; // 0~2; 0:None, 1:Odd, 2:Even.
	gconfig_ConfigData.UrConfig.flowCtrl = 0; // 0~1; 0:None, 1:Auto CTS/RTS Flow Control
	gconfig_ConfigData.UrConfig.baudRate = 115200;

	gconfig_ConfigData.EthernetTxTimer = 100; // 100 ms
	gconfig_ConfigData.R2wMaxConnections = 1; // Maximum connections for serial to network.

    strcpy(gconfig_ConfigData.Admin.Username, "admin");         //20170724 Craig default value
    strcpy(gconfig_ConfigData.Admin.Passwd, "admin");
    gconfig_ConfigData.Admin.Level = 5;

    strcpy(gconfig_ConfigData.Smtp.DomainName, "macroblock.com.tw");    //20170725 Craig
    strcpy(gconfig_ConfigData.Smtp.FromAddr, "ds@macroblock.com.tw");
    strcpy(gconfig_ConfigData.Smtp.ToAddr1, "to1@macroblock.com.tw");
    strcpy(gconfig_ConfigData.Smtp.ToAddr2, "to2@macroblock.com.tw");
    strcpy(gconfig_ConfigData.Smtp.ToAddr3, "to3@macroblock.com.tw");

    gconfig_ConfigData.Smtp.EventEnableBits = 0x0000;
    gconfig_ConfigData.Smtp.Port = 465;
	gconfig_ConfigData.Smtp.Security = GCONFIG_SMTP_SSL;
    strcpy(gconfig_ConfigData.Smtp.Username, "user@macroblock.com.tw");
    strcpy(gconfig_ConfigData.Smtp.Password, "password");

    strcpy(gconfig_ConfigData.DestHostName, "192.168.0.2"); /* 192.168.0.2 */

    /* DHCP server configuration */
    gconfig_ConfigData.DhcpSrv.IpPoolAddrStart = 0xc0a80004; /* 192.168.0.4  */
    gconfig_ConfigData.DhcpSrv.IpPoolAddrEnd = 0xc0a8000a;   /* 192.168.0.10 */
    gconfig_ConfigData.DhcpSrv.Netmask = GCONFIG_DEFAULT_NETMASK;
    gconfig_ConfigData.DhcpSrv.DefaultGateway = GCONFIG_DEFAULT_STATIC_IP;  /* 192.168.0.3  */
    gconfig_ConfigData.DhcpSrv.LeaseTime = 1440;			 /* minutes */
    gconfig_ConfigData.DhcpSrv.Status = 1;                   /* 0: disable  1: enable */
    gconfig_ConfigData.JtagOff = 1;							 /* 0: enable	1: disable */

    gconfig_ConfigData.Ntp.TimeZone = 20;      /* Hong Kong, Singapore */
    strcpy(gconfig_ConfigData.Ntp.SrvHostName[0], "time.stdtime.gov.tw");
    strcpy(gconfig_ConfigData.Ntp.SrvHostName[1], "tick.stdtime.gov.tw");
    strcpy(gconfig_ConfigData.Ntp.SrvHostName[2], "tock.stdtime.gov.tw");

    gconfig_ConfigData.SimpleCfgPinCodeEnable = 0;/* PIN code isn't required */
    gconfig_ConfigData.Rs485Enable = 0;/* Disable RS485 mode */

	/* Set cloud info */
    strcpy(gconfig_ConfigData.GoogleNest.HostName, "");
    strcpy(gconfig_ConfigData.GoogleNest.UserName, "");
    strcpy(gconfig_ConfigData.GoogleNest.PassWord, "");

	/* Set bluemix info */
	strcpy(gconfig_ConfigData.Bluemix.HostName, "");
	strcpy(gconfig_ConfigData.Bluemix.OrganizationID, "");
	strcpy(gconfig_ConfigData.Bluemix.TypeID, "");
	strcpy(gconfig_ConfigData.Bluemix.DeviceID, "");
	strcpy(gconfig_ConfigData.Bluemix.UserName, "");
	strcpy(gconfig_ConfigData.Bluemix.PassWord, "");

	/* Set bluemix auto registration info */
	strcpy(gconfig_ConfigData.BluemixAr.ApiVersion, "");
	strcpy(gconfig_ConfigData.BluemixAr.ApiKey, "");
	strcpy(gconfig_ConfigData.BluemixAr.ApiToken, "");

	/* Set modbus configuration */
    gconfig_ConfigData.Modbus.ServerPortNumber = 502;//502 for standard MODBUS TCP port
    gconfig_ConfigData.Modbus.XferMode = GCONFIG_MBGW_TRANSPARENT;
    gconfig_ConfigData.Modbus.SlaveId = 0xff;//0xff for gateway slave ID
    gconfig_ConfigData.Modbus.ResponseTimeOut = 3000;//3sec
    gconfig_ConfigData.Modbus.InterFrameDelay = 10;//10ms
    gconfig_ConfigData.Modbus.InterCharDelay = 10;//10ms

	/* Set azure info */
	strcpy(gconfig_ConfigData.Azure.HostName, "");
	strcpy(gconfig_ConfigData.Azure.HubName, "");
	strcpy(gconfig_ConfigData.Azure.DeviceID, "");
	strcpy(gconfig_ConfigData.Azure.SignedKey, "");
	strcpy(gconfig_ConfigData.Azure.ExpiryTime, "");
       
	/* Set network backup mode */
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	gconfig_ConfigData.NetworkBackupMode = GCONFIG_NETWORK_BACKUP_WIFI_AS_MAJOR;
#else
	gconfig_ConfigData.NetworkBackupMode = GCONFIG_NETWORK_BACKUP_FORCE_WIFI;
#endif

} /* End of GCONFIG_ReadDefaultConfigData */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_ReadDefaultConfigData
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_ReadMBIdataFromROM(void){          //20170731 Craig Read the MBI Data From ROM
   u8 i,nLen,nIndex;
   u32 temp32;
   RTC_TIME timeNow;
  
  gconfig_ConfigData.JtagOff = 1;							 /* 0: enable	1: disable */
  
  printf("MBI Factory: ");
  
  // copy IP Address
  for(i=0;i<33;i++){
    if(gconfig_ConfigData.MBI_FactorySettings.SSID[i] != 0){
      gconfig_ConfigData.WifiConfig.WifiSsid[i]=gconfig_ConfigData.MBI_FactorySettings.SSID[i];
      gconfig_ConfigData.WifiConfig.WifiSsidLen ++;
      printf("%X ",gconfig_ConfigData.MBI_FactorySettings.SSID[i]);
    }
    else 
      break;
  }
  
  // copy IPv4 Subnet Mask  
   gconfig_ConfigData.Netmask = 0;
  for(i=0;i<4;i++)
  {        
    temp32 = gconfig_ConfigData.MBI_FactorySettings.IPv4_Subnetmask[i];
    temp32 = (temp32 << 8*i) ;
    gconfig_ConfigData.Netmask += temp32;
  }
  
   // copy Enable DHCP , but gconfig not use
  //gconfig_ConfigData. ??  =  gconfig_ConfigData.MBI_FactorySettings.Enable_DHCP;;
    
  // copy AP / Station Mode
  gconfig_ConfigData.WifiConfig.WifiNetworkMode = gconfig_ConfigData.MBI_FactorySettings.AP_StationMode;
  
  // copy SSID 
    for(i=0;i<32;i++)
    gconfig_ConfigData.WifiConfig.WifiSsid[i] = gconfig_ConfigData.MBI_FactorySettings.SSID[i];    
  

  // ssid len   
  gconfig_ConfigData.WifiConfig.WifiSsidLen = gconfig_ConfigData.MBI_FactorySettings.SSID_len;
  


  // copy Encryption Mode
  gconfig_ConfigData.WifiConfig.WifiEncryptMode = gconfig_ConfigData.MBI_FactorySettings.EncryptionMode;

  // copy Encryption Index 
  gconfig_ConfigData.WifiConfig.WifiWepKeyIndex = gconfig_ConfigData.MBI_FactorySettings.EcryptionIndex;
  nIndex = gconfig_ConfigData.MBI_FactorySettings.EcryptionIndex;
  
  
  // copy Password
  if ( gconfig_ConfigData.MBI_FactorySettings.EncryptionMode == 1) //wep
  {
    //Password_len    
    nLen = gconfig_ConfigData.MBI_FactorySettings.Password_len;
    gconfig_ConfigData.WifiConfig.WifiWepKeyLength = gconfig_ConfigData.MBI_FactorySettings.Password_len;
    
    if (nLen == 5)
    {
      for(i=0;i<5;i++)
        gconfig_ConfigData.WifiConfig.WifiWep64Key[nIndex][i] =  gconfig_ConfigData.MBI_FactorySettings.Password[i];
    }
    else if(nLen == 13)
    {
      for(i=0;i<13;i++)
        gconfig_ConfigData.WifiConfig.WifiWep64Key[nIndex][i] =  gconfig_ConfigData.MBI_FactorySettings.Password[i];
    }
      
  }  
  else if  ( gconfig_ConfigData.MBI_FactorySettings.EncryptionMode == 2) //wpa wpa2
  {
    //Password_len    
    nLen = gconfig_ConfigData.MBI_FactorySettings.Password_len;
    gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen = gconfig_ConfigData.MBI_FactorySettings.Password_len;
    for(i=0;i<nLen;i++)
        gconfig_ConfigData.WifiConfig.WifiPreShareKey[i] =  gconfig_ConfigData.MBI_FactorySettings.Password[i];   
     
  }
  
  // copy Wi-Fi Channel
  gconfig_ConfigData.WifiConfig.WifiChannel = gconfig_ConfigData.MBI_FactorySettings.Wifi_Channel;
  
  
  // copy Tx Power (not support)
  
  timeNow.Second = gconfig_ConfigData.MBI_FactorySettings.PresentSec;
  timeNow.Minute = gconfig_ConfigData.MBI_FactorySettings.PresentMin;
  timeNow.Hour = gconfig_ConfigData.MBI_FactorySettings.PresentHour;
  timeNow.Date = gconfig_ConfigData.MBI_FactorySettings.PresentDate;
  timeNow.Month = gconfig_ConfigData.MBI_FactorySettings.PresentMonth;
  timeNow.Year = gconfig_ConfigData.MBI_FactorySettings.PresentYear;
  //timeNow.Day
  
  printf("\n timeNow = %d/%d/%d  D%d  %d:%d:%d \n",
                             timeNow.Year, 
                             timeNow.Month, 
                             timeNow.Date, 
                             timeNow.Day, 
                             timeNow.Hour, 
                             timeNow.Minute,
                             timeNow.Second );

  RTC_SetCurrTime(&timeNow);
  

} /* End of GCONFIG_ReadMBIdataFromROM */

/*
 * ----------------------------------------------------------------------------
 * Function Name: gconfig_CheckRegionCode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static u8 gconfig_CheckRegionCode(u8 countryId)
{
	u8 idx;

	for (idx = 0; idx < GCONFIG_NUM_REGION_CODE; idx++)
	{
		if (GCONFIG_WifiRegionChannelList[idx].countryId == countryId)
		{
			return 1;
		}
	}

	return 0;
} /* End of gconfig_CheckRegionCode() */

/////////////////////////////////////////////////////////////////////////////////

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_IpAddr2Ulong()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_IpAddr2Ulong(u8* pBuf, u8 len)
{
	u32	ip = 0;
	u8*	point = (u8*)&ip;
	u8	count = 0;
	u8	count2 = 0;

	while (1)
	{
		count2 = 0;
		while ((*pBuf != '.') && len)
		{
			count2++;
			if (count2 > 3) return 0xffffffff;

			*point *= 10;

			if ((*pBuf < 0x3a) && (*pBuf > 0x2f))
				*point += (*pBuf - 0x30);
			else
				return 0xffffffff;

			pBuf++;
			len--;
		}

		if (len == 0) break;

		pBuf++;
		len--;
		count++;
		point++;

		if (count > 3) return 0xffffffff;
	}

	if (count != 3) return 0xffffffff;

	return ip;
} /* End of GCONFIG_IpAddr2Ulong() */
/////////////////////////////////////////////////////////////////////////////////
// Set Functions

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDeviceName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDeviceName(GCONFIG_DEV_NAME* pDevName)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(&gconfig_ConfigData.DevName, pDevName, sizeof(GCONFIG_DEV_NAME));
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_GetDeviceName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetMacAddress
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetMacAddress(u8 *pAddr)
{
//	if (!gConfigInit)
//		return;

//	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
//	memcpy(gconfig_MacAddr, pAddr, 6);
//	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetMacAddress() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetJtagOff
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetJtagOff(u8 val)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.JtagOff = val;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetJtagOff() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiCountryId
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 GCONFIG_SetWifiCountryId(u8 val)
{
	if (!gConfigInit)
		return -1;

	if (gconfig_CheckRegionCode(val))
	{
		while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
		gconfig_ConfigData.WifiConfig.WifiCountryId = val;
		
		if (((val == RTW_COUNTRY_ETSI1) || (val == RTW_COUNTRY_ETSI2)) && (gconfig_ConfigData.AdaptivityThreshold == 0x00))
		{
			gconfig_ConfigData.AdaptivityThreshold = GCONFIG_DEFAULT_ADAP_THRES_ETSI;
		}
		else if (((val == RTW_COUNTRY_MKK1) || (val == RTW_COUNTRY_MKK2)) && (gconfig_ConfigData.AdaptivityThreshold == 0x00))
		{
			gconfig_ConfigData.AdaptivityThreshold = GCONFIG_DEFAULT_ADAP_THRES_MKK;
		}
		
		xSemaphoreGive(gConfigMutex);
		return 1;
	}
	return -1;
} /* End of GCONFIG_SetWifiCountryId() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetNetwork
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetNetwork(u16 val)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Network = val;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetNetwork() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDeviceStaticIP
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDeviceStaticIP(u32 ip)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DeviceStaticIP = ip;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDeviceStaticIP() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDeviceDataPktListenPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDeviceDataPktListenPort(u16 port)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DeviceDataPktListenPort = port;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDeviceDataPktListenPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDeviceBroadcastListenPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDeviceBroadcastListenPort(u16 port)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DeviceBroadcastListenPort = port;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDeviceBroadcastListenPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetClientDestPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetClientDestPort(u16 port)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.ClientDestPort = port;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetClientDestPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetNetmask
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetNetmask(u32 netmask)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Netmask = netmask;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetNetmask() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetGateway
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetGateway(u32 gateway)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Gateway = gateway;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetGateway() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDNS
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDNS(u32 dns)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Dns = dns;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDNS() */


/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetEthernetTxTimer
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetEthernetTxTimer(u16 timer)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.EthernetTxTimer = timer;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetEthernetTxTimer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetClientConnectionMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetClientConnectionMode(u8 connMode)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.ClientConnectionMode = (connMode ? GCONFIG_MANUAL_CONNECTION:GCONFIG_AUTO_CONNECTION);
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetClientConnectionMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetFilename
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetFilename(u8 *filename, u8 len)
{
	if (!gConfigInit)
		return 0;

	if (!filename)
        return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	if (len > (GCONFIG_MAX_FILENAME_LEN - 1))
		len = (GCONFIG_MAX_FILENAME_LEN - 1);

    memcpy(gconfig_ConfigData.UpgradeCfg.FileName, filename, len);
    gconfig_ConfigData.UpgradeCfg.FileName[len] = '\0';
	gconfig_ConfigData.UpgradeCfg.FileNameLen = len;

	xSemaphoreGive(gConfigMutex);
    return len;
} /* End of GCONFIG_SetFilename() */

/////////////////////////////////////////////////////////////////////////////////
// Get Functions

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetFilename
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetFilename(u8 *filename, u8 len)
{
    if (!filename || len < GCONFIG_MAX_FILENAME_LEN)
        return 0;

    memcpy(filename, gconfig_ConfigData.UpgradeCfg.FileName, GCONFIG_MAX_FILENAME_LEN);
    len = gconfig_ConfigData.UpgradeCfg.FileNameLen;

    return len;
} /* End of GCONFIG_GetFilename() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDeviceName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
GCONFIG_DEV_NAME* GCONFIG_GetDeviceName(void)
{
	return &gconfig_ConfigData.DevName;
} /* End of GCONFIG_GetDeviceName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetMacAddress
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 *GCONFIG_GetMacAddress(void)
{
	static u8 mac[6];

	MISC_GetMacAddr(mac);
	return mac;
} /* End of GCONFIG_GetMacAddress() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetJtagOff
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetJtagOff(void)
{
        gconfig_ConfigData.JtagOff=1;           //20170808 Craig CloseJTAG
	return gconfig_ConfigData.JtagOff;
} /* End of GCONFIG_GetJtagOff() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiCountryId
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiCountryId(void)
{
	return gconfig_ConfigData.WifiConfig.WifiCountryId;
} /* End of GCONFIG_GetWifiCountryId() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiHiddenSsid
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiHiddenSsid(void)
{
	return (gconfig_ConfigData.WifiConfig.WifiOtherConfiguration & BIT0);
} /* End of GCONFIG_GetWifiHiddenSsid() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiHiddenSsid
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s8 GCONFIG_SetWifiHiddenSsid(u8 val)
{
	if (!gConfigInit)
	{
		return -1;
	}

	if (val > 1)
	{
		return -1;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.WifiConfig.WifiOtherConfiguration &= 0xFE;
	gconfig_ConfigData.WifiConfig.WifiOtherConfiguration |= val;
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetWifiHiddenSsid() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetNetwork
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetNetwork(void)
{
	return gconfig_ConfigData.Network;
} /* End of GCONFIG_GetNetwork() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDeviceStaticIP
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDeviceStaticIP(void)
{
	return gconfig_ConfigData.DeviceStaticIP;
} /* End of GCONFIG_GetDeviceStaticIP() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDeviceDataPktListenPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetDeviceDataPktListenPort(void)
{
	return gconfig_ConfigData.DeviceDataPktListenPort;
} /* End of GCONFIG_GetDeviceDataPktListenPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDeviceBroadcastListenPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetDeviceBroadcastListenPort(void)
{
	return gconfig_ConfigData.DeviceBroadcastListenPort;
} /* End of GCONFIG_GetDeviceBroadcastListenPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetClientDestPort
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetClientDestPort(void)
{
	return gconfig_ConfigData.ClientDestPort;
} /* End of GCONFIG_GetClientDestPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetNetmask
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetNetmask(void)
{
	return gconfig_ConfigData.Netmask;
} /* End of GCONFIG_GetNetmask() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetGateway
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetGateway(void)
{
	return gconfig_ConfigData.Gateway;
} /* End of GCONFIG_GetGateway() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDNS
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDNS(void)
{
	return gconfig_ConfigData.Dns;
} /* End of GCONFIG_GetDNS() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrStopBits
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetUrStopBits(void)
{
	return gconfig_ConfigData.UrConfig.stopBits;
} /* End of GCONFIG_GetUrStopBits() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrBaudRate
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetUrBaudRate(void)
{
	return gconfig_ConfigData.UrConfig.baudRate;
} /* End of GCONFIG_GetUrBaudRate() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrDataBits
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetUrDataBits(void)
{
	return gconfig_ConfigData.UrConfig.dataBits;
} /* End of GCONFIG_GetUrDataBits() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrFlowCtrl
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetUrFlowCtrl(void)
{
	return gconfig_ConfigData.UrConfig.flowCtrl;
} /* End of GCONFIG_GetUrFlowCtrl() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrParity
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetUrParity(void)
{
	return gconfig_ConfigData.UrConfig.parity;
} /* End of GCONFIG_GetUrParity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetUrMode(void)
{
	return ((u8)(gconfig_ConfigData.Network & GCONFIG_NETWORK_R2WMODEMASK));
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetUrXmitDelay
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetUrXmitDelay(void)
{
	return gconfig_ConfigData.UrXmitDelay;
} /* End of GCONFIG_GetUrXmitDelay() */

/***************************************************************************************/
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrStopBits
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrStopBits(u8 stopBits)
{
	if (stopBits < 1 || stopBits > 2)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrConfig.stopBits = stopBits;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrStopBits() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrBaudRate
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrBaudRate(u32 baudRate)
{
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrConfig.baudRate = baudRate;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrBaudRate() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrDataBits
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrDataBits(u8 dataBits)
{
	if (dataBits < 7 || dataBits > 8)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrConfig.dataBits = dataBits;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrDataBits() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrFlowCtrl
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrFlowCtrl(u8 flowCtrl)
{
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrConfig.flowCtrl = flowCtrl;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrFlowCtrl() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrParity
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrParity(u8 parity)
{
	if (parity > 2)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrConfig.parity = parity;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrParity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrMode(u8 mode)
{
	if (mode > 3)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Network &= ~GCONFIG_NETWORK_R2WMODEMASK;
	gconfig_ConfigData.Network |= (u16)mode & GCONFIG_NETWORK_R2WMODEMASK;
	xSemaphoreGive(gConfigMutex);
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetUrXmitDelay
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetUrXmitDelay(u16 delay)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.UrXmitDelay = delay;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetUrXmitDelay() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetEthernetTxTimer
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetEthernetTxTimer(void)
{
	return gconfig_ConfigData.EthernetTxTimer;
} /* End of GCONFIG_GetEthernetTxTimer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetClientConnectionMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetClientConnectionMode(void)
{
	return gconfig_ConfigData.ClientConnectionMode;
} /* End of GCONFIG_GetClientConnectionMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAdmin
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 GCONFIG_GetAdmin(u8 *username, u8 *passwd, u8 *level)
{
    if (!username || !passwd)
        return -1;
    memcpy(username, gconfig_ConfigData.Admin.Username, 16);    //20170724 Craig 
    memcpy(passwd, gconfig_ConfigData.Admin.Passwd, 16);

    *level = gconfig_ConfigData.Admin.Level;

    return 1;
} /* End of GCONFIG_GetAdmin() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAdmin
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */

s16 GCONFIG_GetUser(u8 *username, u8* passwd, u8 *level){
    if (!username || !passwd)
        return -1;
    memcpy(username, UserAccount.Username, 16);    //20170724 Craig 
    memcpy(passwd, UserAccount.Passwd, 16);

    *level = UserAccount.Level;

    return 1;
}/* End of GCONFIG_GetUser() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAdmin
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 GCONFIG_SetAdmin(u8 *username, u8 *passwd)
{
    if (!gConfigInit || !username || !passwd)
        return -1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    memcpy(gconfig_ConfigData.Admin.Username, username, 16);    //20170725 Craig
    memcpy(gconfig_ConfigData.Admin.Passwd, passwd, 16);
	xSemaphoreGive(gConfigMutex);
    return 1;
} /* End of GCONFIG_SetAdmin() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAdmin
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 GCONFIG_SetUser(u8 *username, u8 *passwd){
    if (!gConfigInit || !username || !passwd)
        return -1;

    while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    memcpy(UserAccount.Username, username, 16);    
    memcpy(UserAccount.Passwd, passwd, 16);
    xSemaphoreGive(gConfigMutex);
    return 1;
}/* End of GCONFIG_SetUser() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPDomainName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPDomainName(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

	if (len > sizeof(gconfig_ConfigData.Smtp.DomainName) - 1)
		len = sizeof(gconfig_ConfigData.Smtp.DomainName) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.Smtp.DomainName, pIn, len);
	gconfig_ConfigData.Smtp.DomainName[len] = '\0';
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPDomainName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: u8 GCONFIG_GetSMTPDomainName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSMTPDomainName(u8 *pOut)
{
	gconfig_ConfigData.Smtp.DomainName[sizeof(gconfig_ConfigData.Smtp.DomainName) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.Smtp.DomainName);

	return strlen(gconfig_ConfigData.Smtp.DomainName);
} /* GCONFIG_GetSMTPDomainName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPFrom
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPFrom(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

	if (len > sizeof(gconfig_ConfigData.Smtp.FromAddr) - 1)
		len = sizeof(gconfig_ConfigData.Smtp.FromAddr) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.Smtp.FromAddr, pIn, len);
	gconfig_ConfigData.Smtp.FromAddr[len] = '\0';
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPFrom() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: u8 GCONFIG_GetSMTPFrom()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSMTPFrom(u8 *pOut)
{
	gconfig_ConfigData.Smtp.FromAddr[sizeof(gconfig_ConfigData.Smtp.FromAddr) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.Smtp.FromAddr);

	return strlen(gconfig_ConfigData.Smtp.FromAddr);
} /* GCONFIG_GetSMTPFrom() */
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPTo1
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPTo1(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

	if (len > sizeof(gconfig_ConfigData.Smtp.ToAddr1) - 1)
		len = sizeof(gconfig_ConfigData.Smtp.ToAddr1) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.Smtp.ToAddr1, pIn, len);
	gconfig_ConfigData.Smtp.ToAddr1[len] = '\0';
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPTo1() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: u8 GCONFIG_GetSMTPTo1()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSMTPTo1(u8 *pOut)
{
	gconfig_ConfigData.Smtp.ToAddr1[sizeof(gconfig_ConfigData.Smtp.ToAddr1) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.Smtp.ToAddr1);

	return strlen(gconfig_ConfigData.Smtp.ToAddr1);
} /* GCONFIG_GetSMTPTo1() */
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPTo2
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPTo2(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

	if (len > sizeof(gconfig_ConfigData.Smtp.ToAddr2) - 1)
		len = sizeof(gconfig_ConfigData.Smtp.ToAddr2) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.Smtp.ToAddr2, pIn, len);
	gconfig_ConfigData.Smtp.ToAddr2[len] = '\0';
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPTo2() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: u8 GCONFIG_GetSMTPTo2()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSMTPTo2(u8 *pOut)
{
	gconfig_ConfigData.Smtp.ToAddr2[sizeof(gconfig_ConfigData.Smtp.ToAddr2) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.Smtp.ToAddr2);

	return strlen(gconfig_ConfigData.Smtp.ToAddr2);
} /* GCONFIG_GetSMTPTo2() */
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPTo3
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPTo3(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

	if (len > sizeof(gconfig_ConfigData.Smtp.ToAddr3) - 1)
		len = sizeof(gconfig_ConfigData.Smtp.ToAddr3) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.Smtp.ToAddr3, pIn, len);
	gconfig_ConfigData.Smtp.ToAddr3[len] = '\0';
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPTo3() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: u8 GCONFIG_GetSMTPTo3()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSMTPTo3(u8 *pOut)
{
	gconfig_ConfigData.Smtp.ToAddr3[sizeof(gconfig_ConfigData.Smtp.ToAddr3) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.Smtp.ToAddr3);

	return strlen(gconfig_ConfigData.Smtp.ToAddr3);
} /* GCONFIG_GetSMTPTo3() */
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAutoWarning
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetAutoWarning(u16 ValueIn)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Smtp.EventEnableBits = ValueIn;
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetAutoWarning() */
/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAutoWarning
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetAutoWarning(void)
{
	return gconfig_ConfigData.Smtp.EventEnableBits;
} /* GCONFIG_GetAutoWarning() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPSecurity
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPSecurity(u8 Security, u16 Port)
{
	if (!gConfigInit)
		return;

	if (Security >= GCONFIG_MAX_SMTP_SECURITY)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Smtp.Security = Security;
	gconfig_ConfigData.Smtp.Port = Port;
	xSemaphoreGive(gConfigMutex);
} /* GCONFIG_SetSMTPSecurity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetSMTPSecurity
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_GetSMTPSecurity(u8 *pSecurity, u16 *pPort)
{
	if (pSecurity)
		*pSecurity = gconfig_ConfigData.Smtp.Security;

	if (pPort)
		*pPort = gconfig_ConfigData.Smtp.Port;
} /* GCONFIG_GetSMTPSecurity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSMTPAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSMTPAccount(u8 *pus, u8 *pps)
{
	if (!gConfigInit)
		return;

	if (pus)
	{
		if (strlen(pus) < sizeof(gconfig_ConfigData.Smtp.Username))
		{
			while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
			strcpy(gconfig_ConfigData.Smtp.Username, pus);
			xSemaphoreGive(gConfigMutex);
		}
	}

	if (pps)
	{
		if (strlen(pps) < sizeof(gconfig_ConfigData.Smtp.Password))
		{
			while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
			strcpy(gconfig_ConfigData.Smtp.Password, pps);
			xSemaphoreGive(gConfigMutex);
		}
	}
} /* GCONFIG_SetSMTPAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetSMTPAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_GetSMTPAccount(u8 *pun, u16 unlen, u8 *pps, u16 pslen)
{
	if (pun)
	{
		if (unlen > strlen(gconfig_ConfigData.Smtp.Username))
		{
			strcpy(pun, gconfig_ConfigData.Smtp.Username);
		}
	}

	if (pps)
	{
		if (pslen > strlen(gconfig_ConfigData.Smtp.Password))
		{
			strcpy(pps, gconfig_ConfigData.Smtp.Password);
		}
	}
} /* GCONFIG_GetSMTPAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDestHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDestHostName(u8 *pIn, u8 len)
{
	if (!gConfigInit)
		return;

    if (len > (sizeof(gconfig_ConfigData.DestHostName)-1))
        len = sizeof(gconfig_ConfigData.DestHostName) - 1;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    memcpy(gconfig_ConfigData.DestHostName, pIn, len);
    gconfig_ConfigData.DestHostName[len] = '\0';
	xSemaphoreGive(gConfigMutex);

} /* End of GCONFIG_SetDestHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDestHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetDestHostName(u8 *pOut)
{
    gconfig_ConfigData.DestHostName[sizeof(gconfig_ConfigData.DestHostName) - 1] = '\0';
    strcpy(pOut, gconfig_ConfigData.DestHostName);

	return strlen(gconfig_ConfigData.DestHostName);
} /* End of GCONFIG_GetDestHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiRfEnable
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiRfEnable(void)
{
	return gconfig_ConfigData.WifiConfig.WifiRfEnable;
} /* End of GCONFIG_GetWifiRfEnable() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiRfEnable
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiRfEnable(u8 enable)
{
	if (enable > 1)
	{
		return;
	}
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.WifiConfig.WifiRfEnable = enable;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiRfEnable() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiNetworkMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiNetworkMode(void)
{
	return gconfig_ConfigData.WifiConfig.WifiNetworkMode;
} /* End of GCONFIG_GetWifiNetworkMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiNetworkMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiNetworkMode(u8 value)
{
#if CONFIG_ENABLE_P2P
	if (!gConfigInit || value > RTW_MODE_P2P)
#else
	if (!gConfigInit || value > RTW_MODE_PROMISC)
#endif
	{
		return;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.WifiConfig.WifiNetworkMode = value;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiNetworkMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetChannel
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetChannel(void)
{
	return gconfig_ConfigData.WifiConfig.WifiChannel;
} /* End of GCONFIG_GetChannel() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetChannel
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetChannel(u8 channel)
{
	if (!gConfigInit || channel > 14)
	{
		return;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.WifiConfig.WifiChannel = channel;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetChannel() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiSsid
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiSsid(u8 *pOut)
{
	if (!pOut)
	{
		return 0;
	}

	memcpy(pOut, gconfig_ConfigData.WifiConfig.WifiSsid, gconfig_ConfigData.WifiConfig.WifiSsidLen);

	return gconfig_ConfigData.WifiConfig.WifiSsidLen;
} /* End of GCONFIG_GetWifiSsid() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiSsid
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiSsid(u8 *pIn, u8 len)
{
	if (!gConfigInit)
	{
		return;
	}

	if (len > (sizeof(gconfig_ConfigData.WifiConfig.WifiSsid)-1))
	{
		len = sizeof(gconfig_ConfigData.WifiConfig.WifiSsid)-1;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.WifiConfig.WifiSsid, pIn, len);
	gconfig_ConfigData.WifiConfig.WifiSsid[len] = '\0';
	gconfig_ConfigData.WifiConfig.WifiSsidLen = len;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiSsid() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiEncryptMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiEncryptMode(void)
{
	return gconfig_ConfigData.WifiConfig.WifiEncryptMode;
} /* End of GCONFIG_GetWifiEncryptMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiEncryptMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetWifiEncryptMode(u32 value)
{
	u8	i;

	if (!gConfigInit || value > RTW_SECURITY_WPS_SECURE)
	{
		return 0;
	}

	for (i = 0; i < (sizeof(security_map) - 1); i++)
	{
		if (security_map[i] == value)
		{
			break;
		}
	}

	if (i == sizeof(security_map))
	{
		return 0;
	}

	if (i >= GCONFIG_MAX_WIFI_SECURITY_TYPES)
	{
		i = GCONFIG_WIFI_WPA2_AES_PSK;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.WifiConfig.WifiEncryptMode = i;
	xSemaphoreGive(gConfigMutex);
	return 1;
} /* End of GCONFIG_SetWifiEncryptMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiWepKeyIndex
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiWepKeyIndex(void)
{
	return gconfig_ConfigData.WifiConfig.WifiWepKeyIndex;
} /* End of GCONFIG_GetWifiWepKeyIndex() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiWepKeyIndex
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetWifiWepKeyIndex(u8 value)
{
    if (!gConfigInit || value > 3)
        return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    gconfig_ConfigData.WifiConfig.WifiWepKeyIndex = value;
	xSemaphoreGive(gConfigMutex);
    return 1;
} /* End of GCONFIG_SetWifiWepKeyIndex() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiWepKeyLength
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiWepKeyLength(void)
{
	return gconfig_ConfigData.WifiConfig.WifiWepKeyLength;
} /* End of GCONFIG_GetWifiWepKeyLength() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiWepKeyLength
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiWepKeyLength(u8 value)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    gconfig_ConfigData.WifiConfig.WifiWepKeyLength = value ? 1:0;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiWepKeyLength() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiWep64Key
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiWep64Key(u8 index, u8 *pBuf)
{
    if (!pBuf)
        return 0;
    else if (index > 3)
		index = 0;

    memcpy(pBuf, gconfig_ConfigData.WifiConfig.WifiWep64Key[index], 5);
    return 5;
} /* End of GCONFIG_GetWifiWep64Key() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiWep64Key
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiWep64Key(u8 index, u8 *pBuf)
{
	if (!gConfigInit || index > 3)
        return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.WifiConfig.WifiWep64Key[index], pBuf, 5);
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiWep64Key() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiWep128Key
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiWep128Key(u8 index, u8 *pBuf)
{
    if (!pBuf)
        return 0;
    else if (index > 3)
		index = 0;

    memcpy(pBuf, gconfig_ConfigData.WifiConfig.WifiWep128Key[index], 13);
    return 13;
} /* End of GCONFIG_GetWifiWep128Key() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiWep128Key
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiWep128Key(u8 index, u8 *pBuf)
{
	if (!gConfigInit || index > 3)
        return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.WifiConfig.WifiWep128Key[index], pBuf, 13);
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiWep128Key() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetWifiPreShareKey
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetWifiPreShareKey(u8 *pBuf)
{
	if (!pBuf)
	{
		return 0;
	}
	memcpy(pBuf, gconfig_ConfigData.WifiConfig.WifiPreShareKey, gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen);
	return gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen;
} /* End of GCONFIG_GetWifiPreShareKey() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetWifiPreShareKey
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetWifiPreShareKey(u8 *pBuf, u8 len)
{
	if (!gConfigInit)
	{
		return;
	}

	if (len > sizeof(gconfig_ConfigData.WifiConfig.WifiPreShareKey))
	{
		len = sizeof(gconfig_ConfigData.WifiConfig.WifiPreShareKey);
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	memcpy(gconfig_ConfigData.WifiConfig.WifiPreShareKey, pBuf, len);
	gconfig_ConfigData.WifiConfig.WifiPreShareKeyLen = len;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetWifiPreShareKey() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvStartIp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDhcpSrvStartIp(void)
{
	return gconfig_ConfigData.DhcpSrv.IpPoolAddrStart;
} /* End of GCONFIG_GetDhcpSrvStartIp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvStartIp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvStartIp(u32 startIp)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.IpPoolAddrStart = startIp;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvStartIp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvEndIp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDhcpSrvEndIp(void)
{
	return gconfig_ConfigData.DhcpSrv.IpPoolAddrEnd;
} /* End of GCONFIG_GetDhcpSrvEndIp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvEndIp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvEndIp(u32 endIp)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.IpPoolAddrEnd = endIp;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvEndIp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvNetmask
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDhcpSrvNetmask(void)
{
	return gconfig_ConfigData.DhcpSrv.Netmask;
} /* End of GCONFIG_GetDhcpSrvNetmask() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvNetmask
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvNetmask(u32 mask)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.Netmask = mask;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvNetmask() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvDefGateway
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u32 GCONFIG_GetDhcpSrvDefGateway(void)
{
	return gconfig_ConfigData.DhcpSrv.DefaultGateway;
} /* End of GCONFIG_GetDhcpSrvDefGateway() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvDefGateway
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvDefGateway(u32 gateway)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.DefaultGateway = gateway;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvDefGateway() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvStatus
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetDhcpSrvStatus(void)
{
	return gconfig_ConfigData.DhcpSrv.Status;
} /* End of GCONFIG_GetDhcpSrvStatus() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvStatus
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvStatus(u8 status)
{
    if (!gConfigInit || status > 1)
        return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.Status = status;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvStatus() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetDhcpSrvLeaseTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 GCONFIG_GetDhcpSrvLeaseTime(void)
{
	return gconfig_ConfigData.DhcpSrv.LeaseTime;
} /* End of GCONFIG_GetDhcpSrvLeaseTime() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetDhcpSrvLeaseTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetDhcpSrvLeaseTime(u16 lease)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.DhcpSrv.LeaseTime = lease;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetDhcpSrvLeaseTime() */


/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetNtpSrvHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8* GCONFIG_GetNtpSrvHostName(u8 index)
{
	if (index >= GCONFIG_MAX_NTP_SERVERS)
		return 0;
	return ((u8*)gconfig_ConfigData.Ntp.SrvHostName[index]);
} /* End of GCONFIG_GetNtpSrvHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetNtpSrvHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetNtpSrvHostName(u8 index, u8 *pHostName)
{
	if (!gConfigInit)
		return;
	if (index >= GCONFIG_MAX_NTP_SERVERS)
		return;
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.Ntp.SrvHostName[index], pHostName);
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetNtpSrvHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetNtpTimeZone
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
s16 GCONFIG_GetNtpTimeZone(u8 *pTimeZoneIndex)
{
    if (pTimeZoneIndex != NULL)
        *pTimeZoneIndex = gconfig_ConfigData.Ntp.TimeZone;
    return GCONFIG_TimeZoneTable[gconfig_ConfigData.Ntp.TimeZone].Offset;
} /* End of GCONFIG_GetNtpTimeZone() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetTimeZone
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetNtpTimeZone(u8 TimeZoneIndex)
{
	if (!gConfigInit)
		return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
    if (TimeZoneIndex < GCONFIG_MAX_TIME_ZONE)
        gconfig_ConfigData.Ntp.TimeZone = TimeZoneIndex;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetTimeZone() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetSimpleCfgPinCodeStatus
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetSimpleCfgPinCodeStatus(void)
{
	return gconfig_ConfigData.SimpleCfgPinCodeEnable;
} /* End of GCONFIG_GetSimpleCfgPinCodeStatus() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetSimpleCfgPinCodeStatus
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetSimpleCfgPinCodeStatus(u8 status)
{
    if (!gConfigInit || status > 1)
        return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.SimpleCfgPinCodeEnable = status;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetSimpleCfgPinCodeStatus() */


/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetRs485Status
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetRs485Status(void)
{
	return gconfig_ConfigData.Rs485Enable;
} /* End of GCONFIG_GetRs485Status() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetRs485Status
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetRs485Status(u8 status)
{
    if (!gConfigInit || status > 1)
        return;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Rs485Enable = status;
	xSemaphoreGive(gConfigMutex);
} /* End of GCONFIG_SetRs485Status() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetChannelPlan
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetChannelPlan(u8 countryId, u8 *pminChNum, u8 *pmaxChNum)
{
	u8 idx;

	if (pminChNum==0 || pmaxChNum==0)
		return 0;

	for (idx = 0; idx < GCONFIG_NUM_REGION_CODE; idx++)
	{
		if (GCONFIG_WifiRegionChannelList[idx].countryId == countryId)
		{
			*pminChNum = GCONFIG_WifiRegionChannelList[idx].lowestChannel;
			*pmaxChNum = GCONFIG_WifiRegionChannelList[idx].highestChannel;
			return 1;
		}
	}

	return 0;
} /* End of GCONFIG_GetChannelPlan() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetGoogleNestHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetGoogleNestHostName(u8 *pHostName, u8 BufSize)
{
	u8 i;
	i = strlen(gconfig_ConfigData.GoogleNest.HostName);
	if (i < BufSize)
	{
		strcpy(pHostName, gconfig_ConfigData.GoogleNest.HostName);
		pHostName[i] = 0;
		return 1;
	}
	return 0;
} /* End of GCONFIG_GetGoogleNestHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetGoogleNestHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetGoogleNestHostName(u8 *pHostName)
{
	if (!gConfigInit)
		return 0;
	if (strlen(pHostName) >= GCONFIG_MAX_CLOUD_HOSTNAME_LEN)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.GoogleNest.HostName, pHostName);
	xSemaphoreGive(gConfigMutex);
	return 1;
} /* End of GCONFIG_SetGoogleNestHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetGoogleNestAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetGoogleNestAccount(u8 *pUserName, u8 UnBufSize, u8 *pPassWord, u8 PwBufSize)
{
	if (pUserName)
	{
		if (strlen(gconfig_ConfigData.GoogleNest.UserName) >= UnBufSize)
			return 0;
		strcpy(pUserName, gconfig_ConfigData.GoogleNest.UserName);
	}
	if (pPassWord)
	{
		if (strlen(gconfig_ConfigData.GoogleNest.PassWord) >= PwBufSize)
			return 0;
		strcpy(pPassWord, gconfig_ConfigData.GoogleNest.PassWord);
	}
	return 1;
} /* End of GCONFIG_GetGoogleNestAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetGoogleNestAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetGoogleNestAccount(u8 *pUserName, u8 *pPassWord)
{
	if (!gConfigInit)
		return 0;
	if (strlen(pUserName) >= sizeof(gconfig_ConfigData.GoogleNest.UserName))
		return 0;
	if (strlen(pPassWord) >= sizeof(gconfig_ConfigData.GoogleNest.PassWord))
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.GoogleNest.UserName, pUserName);
	strcpy(gconfig_ConfigData.GoogleNest.PassWord, pPassWord);
	xSemaphoreGive(gConfigMutex);
	return 1;
} /* End of GCONFIG_SetGoogleNestAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetModbusTcp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetModbusTcp(u16 *pServerPortNumber, u8 *pSlaveId, u8 *pXferMode)
{
	if (pServerPortNumber)
		*pServerPortNumber = gconfig_ConfigData.Modbus.ServerPortNumber;

	if (pSlaveId)
		*pSlaveId = gconfig_ConfigData.Modbus.SlaveId;

	if (pXferMode)
		*pXferMode = gconfig_ConfigData.Modbus.XferMode;

	return 1;
} /* End of GCONFIG_GetModbusTcp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetModbusTcp
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetModbusTcp(u16 ServerPortNumber, u8 SlaveId, u8 XferMode)
{
	if (!gConfigInit)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.Modbus.ServerPortNumber = ServerPortNumber;
	if (SlaveId != 0)//0 is used for broadcast address.
		gconfig_ConfigData.Modbus.SlaveId = SlaveId;
	gconfig_ConfigData.Modbus.XferMode = XferMode;
	xSemaphoreGive(gConfigMutex);
	return 1;
} /* End of GCONFIG_SetModbusTcp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetModbusSerialTiming
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetModbusSerialTiming(u16 *pResponseTimeOut, u16 *pInterFrameDelay, u16 *pInterCharDelay)
{
	if (pResponseTimeOut)
		*pResponseTimeOut = gconfig_ConfigData.Modbus.ResponseTimeOut;

	if (pInterFrameDelay)
		*pInterFrameDelay = gconfig_ConfigData.Modbus.InterFrameDelay;

	if (pInterCharDelay)
		*pInterCharDelay = gconfig_ConfigData.Modbus.InterCharDelay;

	return 1;
} /* End of GCONFIG_GetModbusSerialTiming() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetModbusSerialTiming
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetModbusSerialTiming(u16 ResponseTimeOut, u16 InterFrameDelay, u16 InterCharDelay)
{
	if (!gConfigInit)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	if (ResponseTimeOut >= 10 && ResponseTimeOut <= 65000)
		gconfig_ConfigData.Modbus.ResponseTimeOut = ResponseTimeOut;
	if (InterFrameDelay >= 10 && InterFrameDelay <= 500)
		gconfig_ConfigData.Modbus.InterFrameDelay = InterFrameDelay;
	if (InterCharDelay >= 10 && InterCharDelay <= 500)
		gconfig_ConfigData.Modbus.InterCharDelay = InterCharDelay;
	xSemaphoreGive(gConfigMutex);
	return 1;
} /* End of GCONFIG_SetModbusSerialTiming() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetBluemixHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetBluemixHostName(u8 *pHostName, u8 BufSize)
{
	u8	i;

	i = strlen(gconfig_ConfigData.Bluemix.HostName);
	if (i < BufSize)
	{
		strcpy(pHostName, gconfig_ConfigData.Bluemix.HostName);
		pHostName[i] = 0;
		return 1;
	}
	return 0;

} /* End of GCONFIG_GetBluemixHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetBluemixHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetBluemixHostName(u8 *pHostName)
{
	if (!gConfigInit)
	{
		return 0;
	}

	if (strlen(pHostName) >= GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN)
	{
		return 0;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.Bluemix.HostName, pHostName);
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetBluemixHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetBluemixAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetBluemixAccount(u8 *pOrgID, u8 OiBufSize,
							 u8 *pTypeID, u8 TiBufSize,
							 u8 *pDeviceID, u8 DiBufSize,
							 u8 *pUserName, u8 UnBufSize,
							 u8 *pPassWord, u8 PwBufSize)
{
	if (pOrgID)
	{
		if (strlen(gconfig_ConfigData.Bluemix.OrganizationID) >= OiBufSize)
		{
			return 0;
		}
		strcpy(pOrgID, gconfig_ConfigData.Bluemix.OrganizationID);
	}

	if (pTypeID)
	{
		if (strlen(gconfig_ConfigData.Bluemix.TypeID) >= TiBufSize)
		{
			return 0;
		}
		strcpy(pTypeID, gconfig_ConfigData.Bluemix.TypeID);
	}

	if (pDeviceID)
	{
		if (strlen(gconfig_ConfigData.Bluemix.DeviceID) >= DiBufSize)
		{
			return 0;
		}
		strcpy(pDeviceID, gconfig_ConfigData.Bluemix.DeviceID);
	}

	if (pUserName)
	{
		if (strlen(gconfig_ConfigData.Bluemix.UserName) >= UnBufSize)
		{
			return 0;
		}
		strcpy(pUserName, gconfig_ConfigData.Bluemix.UserName);
	}

	if (pPassWord)
	{
		if (strlen(gconfig_ConfigData.Bluemix.PassWord) >= PwBufSize)
		{
			return 0;
		}
		strcpy(pPassWord, gconfig_ConfigData.Bluemix.PassWord);
	}
	return 1;

} /* End of GCONFIG_GetBluemixAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetBluemixAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetBluemixAccount(u8 *pOrgID, u8 *pTypeID, u8 *pDeviceID, u8 *pUserName, u8 *pPassWord)
{
	if (!gConfigInit)
		return 0;
	if (strlen(pOrgID) != (GCONFIG_MAX_BLUEMIX_ORG_ID_LEN - 1))
		return 0;
	if (strlen(pTypeID) != (GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN - 1))
		return 0;
	if (strlen(pDeviceID) != (GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN - 1))
		return 0;
	if (strlen(pUserName) >= GCONFIG_MAX_BLUEMIX_USERNAME_LEN)
		return 0;
	if (strlen(pPassWord) >= GCONFIG_MAX_BLUEMIX_PASSWORD_LEN)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.Bluemix.OrganizationID, pOrgID);
	strcpy(gconfig_ConfigData.Bluemix.TypeID, pTypeID);
	strcpy(gconfig_ConfigData.Bluemix.DeviceID, pDeviceID);
	strcpy(gconfig_ConfigData.Bluemix.UserName, pUserName);
	strcpy(gconfig_ConfigData.Bluemix.PassWord, pPassWord);
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetBluemixAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetBluemixAutoReg
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetBluemixAutoReg(u8 *pApiVersion, u8 ApiVersionBufSize,
							 u8 *pApiKey, u8 ApiKeyBufSize,
							 u8 *pApiToken, u8 ApiTokenBufSize)
{
	if (pApiVersion)
	{
		if (strlen(gconfig_ConfigData.BluemixAr.ApiVersion) >= ApiVersionBufSize)
		{
			return 0;
		}
		strcpy(pApiVersion, gconfig_ConfigData.BluemixAr.ApiVersion);
	}

	if (pApiKey)
	{
		if (strlen(gconfig_ConfigData.BluemixAr.ApiKey) >= ApiKeyBufSize)
		{
			return 0;
		}
		strcpy(pApiKey, gconfig_ConfigData.BluemixAr.ApiKey);
	}

	if (pApiToken)
	{
		if (strlen(gconfig_ConfigData.BluemixAr.ApiToken) >= ApiTokenBufSize)
		{
			return 0;
		}
		strcpy(pApiToken, gconfig_ConfigData.BluemixAr.ApiToken);
	}

	return 1;

} /* End of GCONFIG_GetBluemixAutoReg() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetBluemixAutoReg
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetBluemixAutoReg(u8 *pApiVersion, u8 *pApiKey, u8 *pApiToken)
{
	if (!gConfigInit)
		return 0;
	if (strlen(pApiVersion) >= GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN)
		return 0;
	if (strlen(pApiKey) >= GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN)
		return 0;
	if (strlen(pApiToken) >= GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.BluemixAr.ApiVersion, pApiVersion);
	strcpy(gconfig_ConfigData.BluemixAr.ApiKey, pApiKey);
	strcpy(gconfig_ConfigData.BluemixAr.ApiToken, pApiToken);
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetBluemixAutoReg() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAzureHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetAzureHostName(u8 *pHostName, u8 BufSize)
{
	u8	i;

	i = strlen(gconfig_ConfigData.Azure.HostName);
	if (i < BufSize)
	{
		strcpy(pHostName, gconfig_ConfigData.Azure.HostName);
		pHostName[i] = 0;
		return 1;
	}
	return 0;

} /* End of GCONFIG_GetAzureHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAzureHostName
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetAzureHostName(u8 *pHostName)
{
	if (!gConfigInit)
	{
		return 0;
	}

	if (strlen(pHostName) >= GCONFIG_MAX_AZURE_HOSTNAME_LEN)
	{
		return 0;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.Azure.HostName, pHostName);
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetAzureHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAzureAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetAzureAccount(u8 *pHubName, u8 HnBufSize,
						   u8 *pDeviceID, u8 DiBufSize,
						   u8 *pSignedKey, u8 SignedKeyBufSize,
						   u8 *pExpiryTime, u8 ExpiryTimeBufSize)
{
	if ((pHubName == 0) || (strlen(gconfig_ConfigData.Azure.HubName) >= HnBufSize))
	{
		return 0;
	}

	if ((pDeviceID == 0) || (strlen(gconfig_ConfigData.Azure.DeviceID) >= DiBufSize))
	{
		return 0;
	}

	if ((pSignedKey == 0) || (strlen(gconfig_ConfigData.Azure.SignedKey) >= SignedKeyBufSize))
	{
		return 0;
	}

	if ((pExpiryTime == 0) || (strlen(gconfig_ConfigData.Azure.ExpiryTime) >= ExpiryTimeBufSize))
	{
		return 0;
	}

	strcpy(pHubName, gconfig_ConfigData.Azure.HubName);
	strcpy(pDeviceID, gconfig_ConfigData.Azure.DeviceID);
	strcpy(pSignedKey, gconfig_ConfigData.Azure.SignedKey);
	strcpy(pExpiryTime, gconfig_ConfigData.Azure.ExpiryTime);
	return 1;

} /* End of GCONFIG_GetAzureAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAzureAccount
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_SetAzureAccount(u8 *pHubName, u8 *pDeviceID, u8 *pSignedKey, u8 *pExpiryTime)
{
	if (!gConfigInit)
		return 0;
	if (strlen(pHubName) >= GCONFIG_MAX_AZURE_HUBNAME_LEN)
		return 0;
	if (strlen(pDeviceID) != (GCONFIG_MAX_AZURE_DEVICE_ID_LEN - 1))
		return 0;
	if (strlen(pSignedKey) >= GCONFIG_MAX_AZURE_SIGNED_KEY_LEN)
		return 0;
	if (strlen(pExpiryTime) >= GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN)
		return 0;

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	strcpy(gconfig_ConfigData.Azure.HubName, pHubName);
	strcpy(gconfig_ConfigData.Azure.DeviceID, pDeviceID);
	strcpy(gconfig_ConfigData.Azure.SignedKey, pSignedKey);
	strcpy(gconfig_ConfigData.Azure.ExpiryTime, pExpiryTime);
	xSemaphoreGive(gConfigMutex);
	return 1;

} /* End of GCONFIG_SetAzureAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetR2wMaxConnections
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetR2wMaxConnections(void)
{
	return (gconfig_ConfigData.R2wMaxConnections);
} /* End of GCONFIG_GetR2wMaxConnections() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetR2wMaxConnections
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
int GCONFIG_SetR2wMaxConnections(u8 MaxConns)
{
	if (!gConfigInit)
		return -1;
	if (MaxConns<1 || MaxConns>GTCPDAT_MAX_CONNS)
		return -2;
	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.R2wMaxConnections = MaxConns;
	xSemaphoreGive(gConfigMutex);

	return 0;
} /* End of GCONFIG_SetR2wMaxConnections() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetNetworkBackupMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetNetworkBackupMode(void)
{
	return (gconfig_ConfigData.NetworkBackupMode);
} /* End of GCONFIG_GetNetworkBackupMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetNetworkBackupMode
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetNetworkBackupMode(u8 mode)
{
	if (!gConfigInit)
	{
		return;
	}

	if (mode > GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR)
	{
		return;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.NetworkBackupMode = mode;
	xSemaphoreGive(gConfigMutex);

} /* End of GCONFIG_SetNetworkBackupMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_GetAdaptivityThreshold
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 GCONFIG_GetAdaptivityThreshold(void)
{
	return (gconfig_ConfigData.AdaptivityThreshold);
} /* End of GCONFIG_GetAdaptivityThreshold() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: GCONFIG_SetAdaptivityThreshold
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void GCONFIG_SetAdaptivityThreshold(u8 threshold)
{
	if (!gConfigInit)
	{
		return;
	}

	while(xSemaphoreTake(gConfigMutex, portMAX_DELAY) != pdTRUE);
	gconfig_ConfigData.AdaptivityThreshold = threshold;
	xSemaphoreGive(gConfigMutex);

} /* End of GCONFIG_SetAdaptivityThreshold() */

/////////////////////////////////////////////////////////////////////////////////

/*
 * ----------------------------------------------------------------------------
 * Function Name: gconfig_ReStoreParameter
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static BOOL gconfig_ReStoreParameter(u32 addr, GCONFIG_CFG_DATA *pConfig, u16 len)      //20170725 Restore the configdata from flash
{
	flash_t         flash;
	flash_stream_read(&flash, gConfigAppConfigBase, len, (u8 *)&bk_ConfigData);     //20170725 Craig Read date from flash
        

	if (bk_ConfigData.Signature[0] == 'A' && bk_ConfigData.Signature[1] == 'S' &&
		bk_ConfigData.Signature[2] == 'I' && bk_ConfigData.Signature[3] == 'X' &&
		bk_ConfigData.SwCfgLength < len)
	{
		if (0xffff != gconfig_Checksum((u16 *)&bk_ConfigData, bk_ConfigData.SwCfgLength))
    	{
        	goto IntegrityCheck;
    	}
    	else 
    	{
        	GCONFIG_ReadDefaultConfigData();
			/* Keep the old configuration */
			memcpy((char *)&pConfig->UpgradeCfg, (char *)&bk_ConfigData.UpgradeCfg, bk_ConfigData.SwCfgLength-8);
			gconfig_StoreParameter(gConfigAppConfigBase, pConfig, len);
    	}
	}
	else
	{
IntegrityCheck:
		memcpy((char *)pConfig, (char *)&bk_ConfigData, len);
    	if (0xffff != gconfig_Checksum((u16 *)pConfig, len))
    	{
        	printf(" failed! (wrong checksum)\n\r");
			return FALSE;
    	}
    	else if (pConfig->SwCfgLength != len)
    	{
        	printf(" Invalid configuration data!\n\r");
			return FALSE;
    	}
	}

	printf(" ok.\n\r");

	gconfig_DisplayConfigData(pConfig);
	return TRUE;

} /* End of gconfig_ReStoreParameter() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: gconfig_StoreParameter
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static BOOL gconfig_StoreParameter(u32 addr, GCONFIG_CFG_DATA *pConfigData, u16 len)
{
	u16 w;

    pConfigData->Checksum = 0;
    pConfigData->SwCfgLength = len;
    pConfigData->Checksum = ~gconfig_Checksum((u16 *)pConfigData, len);
    
    /*
    Craig put the struct data that you want to put into the ROM
    */
    
    flash_erase_sector(&flash, addr);
	if (len & 0x3)
		w = len/4 + 1;
	else
		w = len/4;

	flash_stream_write(&flash, gConfigAppConfigBase, w*4, (u8 *)pConfigData);

	return 1;
} /* End of gconfig_StoreParameter() */

/*
* -----------------------------------------------------------------------------
 * Function Name: gconfig_Checksum
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static u16 gconfig_Checksum(u16 *pBuf, u32 length)
{
	u32	cksum = 0;
	u32	i, count;

	count = length/2;

	for (i = 0; i < count; i++)
	{
		cksum += *pBuf++;
		if (!(i % 0x8000))
		{
			cksum = (cksum >> 16) + (cksum & 0xffff);
			cksum += (cksum >> 16);
			cksum &= 0xffff;
		}
	}

	if (length % 2)
		cksum += (*pBuf & 0xff00);

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	cksum &= 0xffff;

	return ((u16)cksum);
} /* End of gconfig_Checksum() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: gconfig_DisplayConfigData
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void	gconfig_DisplayConfigData(GCONFIG_CFG_DATA *pConfigData)
{
	u8 i, *pData;
	u8 dat;
	u16 val;

//	if (GS2W_GetTaskState() == GS2W_STATE_IDLE)
	{
		printf("  Total software configuration size=%d bytes\n\r", sizeof(GCONFIG_CFG_DATA));

		printf("  ---------- Network Configuration ---------- \n\r");
		printf("  Device Name: ");

		for (i = 0; i < 16; i++)
		{
			printf ("%02x ", pConfigData->DevName.DevName[i]);
		}
		printf("\n\r");

		printf("  Username: %s",pConfigData->Admin.Username);   //In the begining of system boot will show these info. 20170725 Craig
		printf("\n\r");

		printf("  Password: %s",pConfigData->Admin.Passwd);
		printf("\n\r");
/*
		printf("  WiFi MAC Addr: ");
		for (i = 0; i < 6; i++)
			printf ("%02x ", macAddr[i]);

		printf("\n\r");
*/
		printf("  Network Setting: ");
		printf ("%04x", pConfigData->Network);
		printf("\n\r");

		val = pConfigData->Network;
		if (val & GCONFIG_NETWORK_CLIENT)
		{
			printf("  SC: Client");
		}
		else
		{
			printf("  SC: Server");
		}
		printf("\n\r");

		if (val & GCONFIG_NETWORK_DHCP_ENABLE)
		{
			printf("  DHCP: On");
		}
		else
		{
			printf("  DHCP: Off");
		}
		printf("\n\r");

		if (val & GCONFIG_NETWORK_PROTO_UDP)
		{
			printf("  UDP: On");
		}
		else
		{
			printf("  UDP: Off");
		}
		printf("\n\r");

		if (val & GCONFIG_NETWORK_PROTO_TCP)
		{
			printf("  TCP: On");
		}
		else
		{
			printf("  TCP: Off");
		}
		printf("\n\r");

		if (val & GCONFIG_NETWORK_PROTO_UDP_BCAST)
		{
			printf("  BC: On");
		}
		else
		{
			printf("  BC: Off");
		}
		printf("\n\r");
		printf("  Client Connection Mode: %s\r\n", (GCONFIG_GetClientConnectionMode() == GCONFIG_AUTO_CONNECTION)?"Auto":"Manual");
		printf("  SIP: ");
		pData = (u8 *)&pConfigData->DeviceStaticIP;
		printf ("%d.%d.%d.%d ", pData[3], pData[2], pData[1], pData[0]);
		printf("\n\r");

		printf("  DA Port: %d", pConfigData->DeviceDataPktListenPort);
		printf("\n\r");

		printf("  BC Port: %d", pConfigData->DeviceBroadcastListenPort);
		printf("\n\r");

		printf("  Dest Host: %s",pConfigData->DestHostName);
		printf("\n\r");

		printf("  Dest Port: %d", pConfigData->ClientDestPort);
		printf("\n\r");

		printf("  Netmask: ");
        pData = (u8 *)&pConfigData->Netmask;
		printf ("%d.%d.%d.%d ", pData[3], pData[2], pData[1], pData[0]);
		printf("\n\r");

		printf("  Gateway: ");
        pData = (u8 *)&pConfigData->Gateway;
		printf ("%d.%d.%d.%d ", pData[3], pData[2], pData[1], pData[0]);
		printf("\n\r");

		printf("  DNS: ");
        pData = (u8 *)&pConfigData->Dns;
		printf ("%d.%d.%d.%d ", pData[3], pData[2], pData[1], pData[0]);
		printf("\n\r");

		printf("  Tx Time Interval: %d", pConfigData->EthernetTxTimer);
		printf("\n\r");

		printf("  TCP Server Connections: %d\r\n", pConfigData->R2wMaxConnections);

		printf("  E-Mail Server: %s",pConfigData->Smtp.DomainName);
		printf("\n\r");

		printf("  From: %s",pConfigData->Smtp.FromAddr);
		printf("\n\r");

		printf("  To1: %s",pConfigData->Smtp.ToAddr1);
		printf("\n\r");

		printf("  To2: %s",pConfigData->Smtp.ToAddr2);
		printf("\n\r");

		printf("  To3: %s",pConfigData->Smtp.ToAddr3);
		printf("\n\r");

		switch (pConfigData->Smtp.Security)
		{
		case GCONFIG_SMTP_NO_SECURITY:
		  	pData = "No security";
			break;
		case GCONFIG_SMTP_SSL:
		  	pData = "SSL";
			break;
		case GCONFIG_SMTP_TLS:
		  	pData = "TLS";
			break;
		case GCONFIG_SMTP_AUTO:
		  	pData = "Auto";
			break;
		}
		printf("  SMTP security: %s, port= %d ", pData, pConfigData->Smtp.Port);
		printf("\n\r");

		printf("  SMTP username: %s, password: %s ", pConfigData->Smtp.Username, pConfigData->Smtp.Password);
		printf("\n\r");

		val = pConfigData->Smtp.EventEnableBits;
		printf("  AutoWarning: %x ", val);
		printf("\n\r");

		printf("  CS: ");
		if (val & GCONFIG_SMTP_EVENT_COLDSTART)
		{
			printf("On");
		}
		else
		{
			printf("Off");
		}
		printf("\n\r");

		printf("  AF: ");
		if (val & GCONFIG_SMTP_EVENT_AUTH_FAIL)
		{
			printf("On");
		}
		else
		{
			printf("Off");
		}
		printf("\n\r");

		printf("  IPC: ");
		if (val & GCONFIG_SMTP_EVENT_IP_CHANGED)
		{
			printf("On");
		}
		else
		{
			printf("Off");
		}
		printf("\n\r");

		printf("  PC: ");
		if (val & GCONFIG_SMTP_EVENT_PSW_CHANGED)
		{
			printf("On");
		}
		else
		{
			printf("Off");
		}
		printf("\r\n");
		for (i=0; i<GCONFIG_MAX_NTP_SERVERS; i++)
		{
			printf("  NTP Server[%d]: %s\r\n", i, (gconfig_ConfigData.Ntp.SrvHostName[i][0]=='\0') ? "N/A":(char*)gconfig_ConfigData.Ntp.SrvHostName[i]);
		}
		printf("  Daylight Saving Time: %s\r\n", (pConfigData->Network & GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE) ? "Enable":"Disable");
		printf("  Time Zone: %s", GCONFIG_TimeZoneTable[gconfig_ConfigData.Ntp.TimeZone].Description);
		printf("\r\n");

		printf("  ---------- Serial Configuration ---------- \n\r");
		printf("  R2W Mode: ");
		switch (pConfigData->Network & GCONFIG_NETWORK_R2WMODEMASK)
		{
			case R2WMODE_Socket:
				printf("Socket.");
				break;
			case R2WMODE_VCom:
				printf("VCOM.");
				break;
			case R2WMODE_Rfc2217:
				printf("RFC2217.");
				break;
			case R2WMODE_MODBUS_GW:
				printf("Modbus Gateway.");
				break;
		}
		printf("\n\r");

		printf("  StopBit: %d", gconfig_ConfigData.UrConfig.stopBits);
		printf("\n\r");

		printf("  ParityBit: %d", gconfig_ConfigData.UrConfig.parity);
		printf("\n\r");

		printf("  FlowCtrl: %d", gconfig_ConfigData.UrConfig.flowCtrl);
		printf("\n\r");

		printf("  DataBits: %d", gconfig_ConfigData.UrConfig.dataBits);
		printf("\n\r");

		printf("  BaudRate: %d", gconfig_ConfigData.UrConfig.baudRate);
		printf("\n\r");

		printf("  RS485: %s", gconfig_ConfigData.Rs485Enable ? "Enable":"Disable");
		printf("\n\r");

		printf("  ---------- Modbus Configuration ---------- \r\n");
		printf("  Server Port: %d\r\n", gconfig_ConfigData.Modbus.ServerPortNumber);
		printf("  Slave ID: %d\r\n", gconfig_ConfigData.Modbus.SlaveId);
		printf("  Transfer Mode: ");
		switch (gconfig_ConfigData.Modbus.XferMode)
		{
		case GCONFIG_MBGW_TCP:
			printf("MODBUS TCP\r\n");
			break;
		case GCONFIG_MBGW_TRANSPARENT:
			printf("Transparent TCP\r\n");
			break;
		}
		printf("  Response Time Out: %d\r\n", gconfig_ConfigData.Modbus.ResponseTimeOut);
		printf("  Inter-Frame Delay: %d\r\n", gconfig_ConfigData.Modbus.InterFrameDelay);
		printf("  Inter-Character Delay: %d\r\n", gconfig_ConfigData.Modbus.InterCharDelay);

		// Add for wifi configuration //
		printf("  ---------- Wifi Configuration ---------- \r\n");

		/* RF Enable */
		i = pConfigData->WifiConfig.WifiRfEnable;
		printf("  RF: %s", i ? "Enable" : "Disable");
		printf("\r\n");

		/* Network Mode */
		i = pConfigData->WifiConfig.WifiNetworkMode;
		printf("  Network Mode: %s", (i == RTW_MODE_NONE) ? "None" :
				 						  (i == RTW_MODE_STA) ? "STA" :
										  (i == RTW_MODE_AP) ? "AP" :
										  (i == RTW_MODE_STA_AP) ? "STA+AP" : "PROMISC");
		printf("\r\n");

		/* Channel */
		printf("  Channel: %d", pConfigData->WifiConfig.WifiChannel);
		printf("\r\n");

		/* SSID */
		printf("  SSID: ");
		for (i = 0; i < pConfigData->WifiConfig.WifiSsidLen; i ++)
		{
			printf("%c", pConfigData->WifiConfig.WifiSsid[i]);
		}
		printf("\r\n");

		/* SSID hide status */
		printf("  Hidden SSID: %s\r\n", GCONFIG_GetWifiHiddenSsid() ? "Yes" : "No");

		/* Security Mode */
		i = pConfigData->WifiConfig.WifiEncryptMode;
		printf("  Security: ");
		if (i == GCONFIG_WIFI_OPEN)
		{
			printf("No security ");
		}
		else if (i == GCONFIG_WIFI_WEP)
		{
			dat = pConfigData->WifiConfig.WifiWepKeyLength;
			printf("WEP %sbits", (dat == 0) ? "64" : "128");
		}
		else if (i == GCONFIG_WIFI_WPA2_AES_PSK)
		{
			printf("%s", (pConfigData->WifiConfig.WifiNetworkMode == RTW_MODE_STA) ? "WPA/WPA2_AUTO_PSK" : "WPA2_AES_PSK");
		}
		printf("\r\n");

		/* WEP Key Index */
		i = pConfigData->WifiConfig.WifiWepKeyIndex;
		printf("  WEP Key Index: %d", i);
		printf("\r\n");

		/* WEP 64 bits Key Index 1 ~ 4 */
		for (val = 0; val < 4; val ++)
		{
			printf("  WEP64 bits Key index %d: 0x", val);
			for (i = 0; i < 5; i ++)
			{
				printf("%02x", pConfigData->WifiConfig.WifiWep64Key[val][i]);
			}
			printf("\r\n");
		}

		/* WEP 128 bits Key Index 1 ~ 4 */
		for (val = 0; val < 4; val ++)
		{
			printf("  WEP128 bits Key index %d: 0x", val);
			for (i = 0; i < 13; i ++)
			{
				printf("%02x", pConfigData->WifiConfig.WifiWep128Key[val][i]);
			}
			printf("\r\n");
		}

		/* AES/TKIP Passphrase */
		dat = pConfigData->WifiConfig.WifiPreShareKeyLen;
		printf("  AES/TKIP Passphrase: ");
		for (i = 0; i < dat; i ++)
		{
			printf("%c", pConfigData->WifiConfig.WifiPreShareKey[i]);
		}
		printf("\r\n");
                
                        
        
		printf("  ---------- Cloud Service Configuration ---------- \r\n");
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
		printf("  Enable Google Nest cloud service\r\n");
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
		printf("  Enable IBM BlueMix cloud service\r\n");
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
		printf("  Enable Microsoft Azure cloud service\r\n");
#else
		printf("  No cloud service\r\n");
#endif

		printf("  ---------- Other Configuration ---------- \n\r");
		printf("  JTAG: %s", gconfig_ConfigData.JtagOff ? "Off" : "On");
		printf("\n\r");

		printf("  OTA Name: %s", gconfig_ConfigData.UpgradeCfg.FileName);
		printf("\n\r");

		printf("  Simple config PIN code: %s", gconfig_ConfigData.SimpleCfgPinCodeEnable ? "Enable":"Disable");
		printf("\n\r");

		// End of power on information //
		printf("\n\r\n\r");
                printf("Date: %x",gconfig_ConfigData.MBI_FactorySettings.PresentDate);           //0808 Test Craig
                printf("Year: %x",gconfig_ConfigData.MBI_FactorySettings.PresentYear);
                printf("SSID: ");
                for(i=0;i<33;i++)
                  printf("%c ",*(gconfig_ConfigData.MBI_FactorySettings.SSID+i));
	}
} /* End of gconfig_DisplayConfigData() */

/* End of gconfig.c */
