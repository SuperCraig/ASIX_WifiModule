/*
 ******************************************************************************
 *     Copyright (c) 2016	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 ******************************************************************************
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"
#include "portmacro.h"
#include "rtl8195a.h"
#include <lwip_netconf.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <lwip/sockets.h>
#include <lwip/raw.h>
#include <lwip/icmp.h>
#include <lwip/inet_chksum.h>
#include <platform_stdlib.h>
#include "tcpip.h"
#include "hal_api.h"
#include "gconfig.h"
#include "console.h"
#include "clicmd.h"
#include "gpios.h"
#include "misc.h"
#include "gs2w.h"
#include "gtcpdat.h"
#include "gudpdat.h"
#include "rtc.h"
#include "ths.h"
#include "dhcpsrv.h"
#include "guart.h"
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
#include "gnapp.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
#include "bluemix.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
#include "azure.h"
#endif
#include "wifi_simple_config_parser.h"

/* NAMING CONSTANT DECLARATIONS */
#define IsDigit(x) ((x < 0x3a && x > 0x2f) ? 1 : 0)
#define PING_DataSize 120
#define PING_BufferSize 200

/* GLOBAL VARIABLES DECLARATIONS */
extern struct netif xnetif[NET_IF_NUM];
extern wifi_scaned_bss_info_t	wifi_scaned_bss_table[STA_SCANNED_BSS_MAX];
extern const s32	security_map[];
extern void GPIOS_SaveCurrentWifiConfig(void);
extern void GPIOS_CheckAndRestoreAPMode(U8_T *everChanged);

/* LOCAL VARIABLES DECLARATIONS */
static void clicmd_Reboot(CONSOLE_CLICtl *inst);
static int clicmd_IpConfig(CONSOLE_CLICtl *pInst, int argc, char **argv);
static int clicmd_IpConfigHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetIpAddr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetIpAddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetNetMask(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetNetMaskHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetGateway(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetGatewayHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDnsIp(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDnsIpHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetMode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetModeHelp(CONSOLE_CLICtl *inst);

static int clicmd_R2wMode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_R2wModeHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetConnectType(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetConnectTypeHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetServerPort(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetServerPortHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDestPort(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDestPortHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDestHostName(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDestHostNameHelp(CONSOLE_CLICtl *inst);

static int clicmd_ConnectStatus(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_ConnectStatusHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiConnect(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiConnectHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiDisconnect(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiDisconnectHelp(CONSOLE_CLICtl *inst);

static int clicmd_SystemReboot(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SystemRebootHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiMode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiModeHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiScan(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiScanHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiJbss(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiJbssHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiOn(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiOnHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiOff(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiOffHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiChannel(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiChannelHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiSsid(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiSsidHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiEncrypt(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiEncryptHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiKeyId(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiKeyIdHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiWepKey(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiWepKeyHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiWepKeyLen(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiWepKeyLenHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiWpaKey(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiWpaKeyHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetUrDataMode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetUrDataModeHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDefaultSettings(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDefaultSettingsHelp(CONSOLE_CLICtl *inst);

static int clicmd_SaveConfigure(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SaveConfigureHelp(CONSOLE_CLICtl *inst);

static int clicmd_Ping(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_PingHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiInfo(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiInfoHelp(CONSOLE_CLICtl *inst);

static int clicmd_WifiAP(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_WifiAPHelp(CONSOLE_CLICtl *inst);

static int clicmd_UrConfig(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_UrConfigHelp(CONSOLE_CLICtl *inst);

static int clicmd_DhcpClient(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_DhcpClientHelp(CONSOLE_CLICtl *inst);

static int clicmd_EnableDhcpServer(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_EnableDhcpServerHelp(CONSOLE_CLICtl *inst);

static int clicmd_NtpServer(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_NtpServerHelp(CONSOLE_CLICtl *inst);

static int clicmd_NtpServer(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_NtpServerHelp(CONSOLE_CLICtl *inst);

static int clicmd_RtcTimeSetup(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_RtcTimeSetupHelp(CONSOLE_CLICtl *inst);

static int clicmd_CurrentTime(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_CurrentTimeHelp(CONSOLE_CLICtl *inst);

static int clicmd_CurrentDate(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_CurrentDateHelp(CONSOLE_CLICtl *inst);

static int clicmd_GetTemperatureHumidity(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_GetTemperatureHumidityHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEmailServerAddr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEmailServerAddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEmailFromAddr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEmailFromAddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEmailTo1Addr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEmailTo1AddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEmailTo2Addr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEmailTo2AddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEmailTo3Addr(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEmailTo3AddrHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetSMTPSecurity(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetSMTPSecurityHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetSMTPAccount(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetSMTPAccountHelp(CONSOLE_CLICtl *inst);

static int clicmd_GetEmailConfig(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_GetEmailConfigHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetAWConfig(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetAWConfigHelp(CONSOLE_CLICtl *inst);

static int clicmd_JtagOff(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_JtagOffHelp(CONSOLE_CLICtl *inst);

static int clicmd_JtagOff(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_JtagOffHelp(CONSOLE_CLICtl *inst);

static int clicmd_GetOTAName(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_GetOTANameHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetCountryId(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetCountryIdHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDhcpServer(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDhcpServerHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetEtherTimer(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetEtherTimerHelp(CONSOLE_CLICtl *inst);

static int clicmd_DhcpServerAllocTbl(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_DhcpServerAllocTblHelp(CONSOLE_CLICtl *inst);

static int clicmd_SimpleCfgPinCode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SimpleCfgPinCodeHelp(CONSOLE_CLICtl *inst);

static int clicmd_UrCounter(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_UrCounterHelp(CONSOLE_CLICtl *inst);

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
static int clicmd_SetGoogleNestInfo(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetGoogleNestInfoHelp(CONSOLE_CLICtl *inst);
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
static int clicmd_SetBluemixHostName(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetBluemixHostNameHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetBluemixAccount(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetBluemixAccountHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetBluemixAutoReg(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetBluemixAutoRegHelp(CONSOLE_CLICtl *inst);
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
static int clicmd_SetAzureHostName(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetAzureHostNameHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetAzureAccount(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetAzureAccountHelp(CONSOLE_CLICtl *inst);
#endif

static int clicmd_SetRs485Status(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetRs485StatusHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetModbusTcp(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetModbusTcpHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetModbusSerialTiming(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetModbusSerialTimingHelp(CONSOLE_CLICtl *inst);

#if CONFIG_INCLUDE_SIMPLE_CONFIG
static int clicmd_SimpleConfig(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SimpleConfigHelp(CONSOLE_CLICtl *inst);
#endif

static int clicmd_SetClientConnectionMode(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetClientConnectionModeHelp(CONSOLE_CLICtl *inst);

static int clicmd_ClientConnect(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_ClientConnectHelp(CONSOLE_CLICtl *inst);

static int clicmd_ClientClose(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_ClientCloseHelp(CONSOLE_CLICtl *inst);

static int clicmd_SetDeviceName(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetDeviceNameHelp(CONSOLE_CLICtl *inst);

#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
static int clicmd_SetNetworkBackup(CONSOLE_CLICtl *inst, int argc, char **argv);
static int clicmd_SetNetworkBackupHelp(CONSOLE_CLICtl *inst);
#endif
CONSOLE_Account CLICMD_userTable[MAX_USER_ACCOUNT];

CONSOLE_CmdEntry CLICMD_userCmdTable[]=
{
    {"ipconfig", clicmd_IpConfig, clicmd_IpConfigHelp, 5},
    {"setip", clicmd_SetIpAddr, clicmd_SetIpAddrHelp, 5},
    {"setnetmask", clicmd_SetNetMask, clicmd_SetNetMaskHelp, 5},
    {"setgateway", clicmd_SetGateway, clicmd_SetGatewayHelp, 5},
    {"setdns", clicmd_SetDnsIp, clicmd_SetDnsIpHelp, 5},
    {"setmode", clicmd_SetMode, clicmd_SetModeHelp, 5},
	{"r2wmode", clicmd_R2wMode, clicmd_R2wModeHelp, 5},
	{"setsrvport", clicmd_SetServerPort, clicmd_SetServerPortHelp, 5},
	{"setdstport", clicmd_SetDestPort, clicmd_SetDestPortHelp, 5},
	{"setdsthn", clicmd_SetDestHostName, clicmd_SetDestHostNameHelp, 5},
	{"connectype", clicmd_SetConnectType, clicmd_SetConnectTypeHelp, 5},
	{"connstatus", clicmd_ConnectStatus, clicmd_ConnectStatusHelp, 5},
	{"wifi_connect", clicmd_WifiConnect, clicmd_WifiConnectHelp, 5},
	{"wifi_disconnect", clicmd_WifiDisconnect, clicmd_WifiDisconnectHelp, 5},
	{"wifi_mode", clicmd_WifiMode, clicmd_WifiModeHelp, 5},
	{"wifi_scan", clicmd_WifiScan, clicmd_WifiScanHelp, 5},
	{"wifi_jbss", clicmd_WifiJbss, clicmd_WifiJbssHelp, 5},
	{"wifi_on", clicmd_WifiOn, clicmd_WifiOnHelp, 5},
	{"wifi_off", clicmd_WifiOff, clicmd_WifiOffHelp, 5},
	{"wifi_channel", clicmd_WifiChannel, clicmd_WifiChannelHelp, 5},
	{"wifi_ssid", clicmd_WifiSsid, clicmd_WifiSsidHelp, 5},
	{"wifi_enc", clicmd_WifiEncrypt, clicmd_WifiEncryptHelp, 5},
	{"wifi_keyid", clicmd_WifiKeyId, clicmd_WifiKeyIdHelp, 5},
	{"wifi_wepkey", clicmd_WifiWepKey, clicmd_WifiWepKeyHelp, 5},
	{"wifi_wepkeylen", clicmd_WifiWepKeyLen, clicmd_WifiWepKeyLenHelp, 5},
	{"wifi_wpakey", clicmd_WifiWpaKey, clicmd_WifiWpaKeyHelp, 5},
	{"reboot", clicmd_SystemReboot, clicmd_SystemRebootHelp, 5},
    {"urdatamode", clicmd_SetUrDataMode, clicmd_SetUrDataModeHelp, 5},
    {"setdef", clicmd_SetDefaultSettings, clicmd_SetDefaultSettingsHelp, 5},
    {"saveconfig", clicmd_SaveConfigure, clicmd_SaveConfigureHelp, 5},
    {"ping", clicmd_Ping, clicmd_PingHelp, 5},
	{"wifi_info", clicmd_WifiInfo, clicmd_WifiInfoHelp, 5},
	{"wifi_ap", clicmd_WifiAP, clicmd_WifiAPHelp, 5},
	{"ur_config", clicmd_UrConfig, clicmd_UrConfigHelp, 5},
	{"dhcpclient", clicmd_DhcpClient, clicmd_DhcpClientHelp, 5},
	{"setdhcpsrv", clicmd_EnableDhcpServer, clicmd_EnableDhcpServerHelp, 5},
	{"ntpsrv", clicmd_NtpServer, clicmd_NtpServerHelp, 5},
	{"rtcts", clicmd_RtcTimeSetup, clicmd_RtcTimeSetupHelp, 5},
	{"time", clicmd_CurrentTime, clicmd_CurrentTimeHelp, 5},
	{"date", clicmd_CurrentDate, clicmd_CurrentDateHelp, 5},
	{"getths", clicmd_GetTemperatureHumidity, clicmd_GetTemperatureHumidityHelp, 5},
    {"setems", clicmd_SetEmailServerAddr, clicmd_SetEmailServerAddrHelp, 5},
    {"setemf", clicmd_SetEmailFromAddr, clicmd_SetEmailFromAddrHelp, 5},
    {"setemt1", clicmd_SetEmailTo1Addr, clicmd_SetEmailTo1AddrHelp, 5},
    {"setemt2", clicmd_SetEmailTo2Addr, clicmd_SetEmailTo2AddrHelp, 5},
    {"setemt3", clicmd_SetEmailTo3Addr, clicmd_SetEmailTo3AddrHelp, 5},
	{"setemsc", clicmd_SetSMTPSecurity, clicmd_SetSMTPSecurityHelp, 5},
	{"setemac", clicmd_SetSMTPAccount, clicmd_SetSMTPAccountHelp, 5},
    {"emconfig", clicmd_GetEmailConfig, clicmd_GetEmailConfigHelp, 5},
    {"setaw", clicmd_SetAWConfig, clicmd_SetAWConfigHelp, 5},
    {"jtagoff", clicmd_JtagOff, clicmd_JtagOffHelp, 5},
    {"getotaname", clicmd_GetOTAName, clicmd_GetOTANameHelp, 5},
    {"countryid", clicmd_SetCountryId, clicmd_SetCountryIdHelp, 5},
    {"dhcpsrv", clicmd_SetDhcpServer, clicmd_SetDhcpServerHelp, 5},
    {"transmitimer", clicmd_SetEtherTimer, clicmd_SetEtherTimerHelp, 5},
    {"dhcpstbl", clicmd_DhcpServerAllocTbl, clicmd_DhcpServerAllocTblHelp, 5},
    {"scpincode", clicmd_SimpleCfgPinCode, clicmd_SimpleCfgPinCodeHelp, 5},
//    {"urCounter", clicmd_UrCounter, clicmd_UrCounterHelp, 5},
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
    {"cloud", clicmd_SetGoogleNestInfo, clicmd_SetGoogleNestInfoHelp, 5},
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
	{"bmh", clicmd_SetBluemixHostName, clicmd_SetBluemixHostNameHelp, 5},
	{"bma", clicmd_SetBluemixAccount, clicmd_SetBluemixAccountHelp, 5},
	{"bmr", clicmd_SetBluemixAutoReg, clicmd_SetBluemixAutoRegHelp, 5},
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
	{"azh", clicmd_SetAzureHostName, clicmd_SetAzureHostNameHelp, 5},
	{"aza", clicmd_SetAzureAccount, clicmd_SetAzureAccountHelp, 5},
#endif
	{"rs485", clicmd_SetRs485Status, clicmd_SetRs485StatusHelp, 5},
	{"mbtcp", clicmd_SetModbusTcp, clicmd_SetModbusTcpHelp, 5},
	{"mbst", clicmd_SetModbusSerialTiming, clicmd_SetModbusSerialTimingHelp, 5},
#if CONFIG_INCLUDE_SIMPLE_CONFIG
	{"wifi_simple_config", clicmd_SimpleConfig, clicmd_SimpleConfigHelp, 5},
#endif
	{"clientconnmode", clicmd_SetClientConnectionMode, clicmd_SetClientConnectionModeHelp, 5},
	{"clientconn", clicmd_ClientConnect, clicmd_ClientConnectHelp, 5},
	{"clientclose", clicmd_ClientClose, clicmd_ClientCloseHelp, 5},
	{"setdevname", clicmd_SetDeviceName, clicmd_SetDeviceNameHelp, 5},
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	{"setnetbackup", clicmd_SetNetworkBackup, clicmd_SetNetworkBackupHelp, 5},
#endif
};

static u16 pingId, pingSeq=0;
static u8 StrBuf[128];
static DHCPS_POOL dhcps_allocation_table[DHCPS_MAX_IP_POOL_NUM];

static s16 checkIpInput(u8 *pBuf)
{
	u8 *point = pBuf;
	u8 len = 0;

    if (!pBuf || (len=strlen(pBuf)) == 0)
        return 0;

	while (len > 0)
	{
		if (IsDigit(*point) || *point == '.')
        {
            point++;
            len --;
        }
        else
            return 0;
    }

	return 1;
}

static u8 getIp2Ulong(u8 *pBuf, u8 len, u32 *ipAddr)
{
	u32 addr = 0;
	u8 count = 0, digits;
    u16 value;

	while (len > 0)
	{
		digits = 0;
        value = 0;
		while ((*pBuf != '.') && len > 0)
		{
			digits++;
			if (digits > 3)
				return 0;

            value *= 10;

            if (IsDigit(*pBuf))
                value += (*pBuf - 0x30);
			else
				return 0;

			pBuf++; len--;
		}

        if (!digits)
            return 0;

        if (value > 255)
            return 0;

        addr = (addr << 8) + (u8)(value & 0xFF);
		if (len == 0)
			break;

		pBuf++; len--;
		count++;

		if (count > 3)
			return 0;
	}

	if (count != 3)
		return 0;

	*ipAddr = addr;

	return 1;
}

static u8 validIp(u32 ipAddr)
{
    u8 *tmp8 = (u8 *)&ipAddr;

    /* class D/E */
    if ((tmp8[3] & 0xF0) >= 0xE0)
        return 0;

    if (tmp8[0] == 0xFF || tmp8[0] == 0)
        return 0;

    /* Class C */
    if ((tmp8[3] & 0xC0) && tmp8[1] == 0xFF)
        return 0;

    return 1;
}

/*
 * ----------------------------------------------------------------------------
 * Function Name: CLICMD_GetCmdTableSize()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u16 CLICMD_GetCmdTableSize(void)
{
    return sizeof(CLICMD_userCmdTable)/sizeof(CONSOLE_CmdEntry);
} /* End of CLICMD_GetCmdTableSize() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_IpConfig()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_IpConfig(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *tmp8;

	if (netif_default != NULL)
	{
		tmp8 = (u8 *)&(netif_default->ip_addr);
		CONSOLE_PutMessage(inst, "IP address: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = (u8 *)&(netif_default->netmask);
		CONSOLE_PutMessage(inst, "Netmask: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = (u8 *)&(netif_default->gw);
		CONSOLE_PutMessage(inst, "Gateway: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = GCONFIG_GetMacAddress();
		CONSOLE_PutMessage(inst, "Mac: 0x%02x%02x%02x%02x%02x%02x\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3], tmp8[4], tmp8[5]);
		CONSOLE_PutMessage(inst, "Interface: %s\r\n", (MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) ? "Ethernet":"WiFi");
	}
	else
	{
		CONSOLE_PutMessage(inst, "No available network interface!\r\n");
	}
    return 1;
} /* End of clicmd_IpConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_IpConfigHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_IpConfigHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: ipconfig\r\n");
	return 1;
} /* End of clicmd_IpConfigHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetIpAddr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetIpAddr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *tmp8;
	u32 addr;

	if (argc == 1) /* current setting */
    {
		clicmd_SetIpAddrHelp(inst);
        addr = GCONFIG_GetDeviceStaticIP();
		tmp8 = (u8 *)&addr;
		CONSOLE_PutMessage(inst, "IP address: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);
        return 1;
    }
    else if (argc > 2)
        return -1;

	if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) > 0)
	{
		if (validIp(addr))
		{
			GCONFIG_SetDeviceStaticIP(addr);
			EMAIL_SendIpChanged();
		}
	}

	return 1;
} /* End of clicmd_SetIpAddr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetIpAddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetIpAddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setip <IP address>\r\n");
	return 1;
} /* End of clicmd_SetIpAddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetNetMask()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetNetMask(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *tmp8;
	u32 netmask;

    if (argc == 1) /* current setting */
    {
		clicmd_SetNetMaskHelp(inst);
        netmask = GCONFIG_GetNetmask();
		tmp8 = (u8 *)&netmask;
		CONSOLE_PutMessage(inst, "Netmask: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);

        return 1;
    }
    else if (argc > 2)
        return -1;

    if (getIp2Ulong(argv[1], strlen(argv[1]), &netmask) > 0)
        GCONFIG_SetNetmask(netmask);

	return 1;
} /* End of clicmd_SetNetMask() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetNetMaskHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetNetMaskHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setnetmask <netmask>\r\n");
	return 1;
} /* End of clicmd_SetNetMaskHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetGateway()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetGateway(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *tmp8;
	u32 gateway;

    if (argc == 1) /* current setting */
    {
		clicmd_SetGatewayHelp(inst);
		gateway = GCONFIG_GetGateway();
		tmp8 = (u8 *)&gateway;
        CONSOLE_PutMessage(inst, "Gateway: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);
        return 1;
    }
    else if (argc > 2)
        return -1;

    if (getIp2Ulong(argv[1], strlen(argv[1]), &gateway) > 0)
    {
        if (validIp(gateway))
        	GCONFIG_SetGateway(gateway);
		else
			return -1;
    }

	return 1;
} /* End of clicmd_SetGateway() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetGatewayHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetGatewayHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setgateway <gateway IP address>\r\n");

	return 1;
} /* End of clicmd_SetGatewayHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDnsIp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDnsIp(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u32 addr;

	if (argc == 1) /* current setting */
	{
		addr = GCONFIG_GetDNS();
		CONSOLE_PutMessage(inst, "DNS IP: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
				(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
		return 1;
	}
	else if (argc > 2)
		return -1;

	if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) > 0)
	{
		if (validIp(addr))
			GCONFIG_SetDNS(addr);
		else
			return -1;
	}

	return 1;
} /* End of clicmd_SetDnsIp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDnsIpHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDnsIpHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdns <DNS IP address>\r\n");

	return 1;
} /* End of clicmd_SetDnsIpHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetMode(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u16 temp16;
	int iTemp;

	temp16 = GCONFIG_GetNetwork();
	if (argc == 1) /* current setting */
	{
		clicmd_SetModeHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent Device Mode: %s mode\r\n", (temp16 & GCONFIG_NETWORK_CLIENT) ? "CLIENT":"SERVER");
		return 1;
	}
	else if (argc > 2)
		return -1;

	iTemp = atoi(argv[1]);
	if (iTemp > 1)
		return -1;

	temp16 &= ~GCONFIG_NETWORK_CLIENT;
	if (iTemp)
		temp16 |= GCONFIG_NETWORK_CLIENT;

	GCONFIG_SetNetwork(temp16);

	return 1;
} /* End of clicmd_SetMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetModeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetModeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setmode <mode>\r\n");
	CONSOLE_PutMessage(inst, "       <mode>: 0: SERVER     1: CLIENT\r\n");
	return 1;
} /* End of clicmd_SetModeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_R2wMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_R2wMode(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;
	u16 temp16;

	temp16 = GCONFIG_GetNetwork();
	if (argc == 1)
	{
	  	clicmd_R2wModeHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent R2W mode: %d, Server maximum connections: %d\r\n", (temp16 & GCONFIG_NETWORK_R2WMODEMASK), GCONFIG_GetR2wMaxConnections());
	}
	else if (argc > 1 && argc < 4)
	{
		val = atoi(argv[1]);
		if (val < GCONFIG_MAX_R2W_MODE && val >= 0)
		{
			temp16 &= ~GCONFIG_NETWORK_R2WMODEMASK;
			temp16 |= val;
			GCONFIG_SetNetwork(temp16);
		}
		else
			CONSOLE_PutMessage(inst, "Invalid mode value input\r\n");

		if (argc == 3)
		{
			val = atoi(argv[2]);
			if(GCONFIG_SetR2wMaxConnections((u8)val) < 0)
				CONSOLE_PutMessage(inst, "Invalid maxConns value input\r\n");
		}
	}
	return 1;
} /* End of clicmd_R2wMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_R2wModeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_R2wModeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: r2wmode <mode> <maxConns>\r\n");
	CONSOLE_PutMessage(inst, "       <mode>\r\n");
	CONSOLE_PutMessage(inst, "       0: Socket\r\n");
	CONSOLE_PutMessage(inst, "       1: VCOM\r\n");
	CONSOLE_PutMessage(inst, "       2: RFC2217\r\n");
	CONSOLE_PutMessage(inst, "       3: Modbus Gateway\r\n");
	CONSOLE_PutMessage(inst, "       <maxConns>\r\n");
	CONSOLE_PutMessage(inst, "       Available value range is 1~4.\r\n");
	CONSOLE_PutMessage(inst, "       Note this parameter is effective only for TCP server under Socket or VCOM mode.\r\n");
	return 1;
} /* End of clicmd_R2wModeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetConnectType()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetConnectType(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u16 temp16;
	int iTemp;

    temp16 = GCONFIG_GetNetwork();
    if (argc == 1) /* current setting */
    {
		clicmd_SetConnectTypeHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent connection type: %s\r\n",
				 ((temp16 & (GCONFIG_NETWORK_PROTO_UDP | GCONFIG_NETWORK_PROTO_TCP))==GCONFIG_NETWORK_PROTO_UDP) ? "UDP":"TCP");
        return 1;
    }
    else if (argc > 2)
        return -1;

    iTemp = atoi(argv[1]);

    if (iTemp > 1)
		return -1;

    temp16 &= ~(GCONFIG_NETWORK_PROTO_UDP | GCONFIG_NETWORK_PROTO_TCP);
    if (iTemp)
        temp16 |= GCONFIG_NETWORK_PROTO_UDP;
    else
        temp16 |= GCONFIG_NETWORK_PROTO_TCP;

    GCONFIG_SetNetwork(temp16);
	return 1;
} /* End of clicmd_SetConnectType() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetConnectTypeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetConnectTypeHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: connectype <protocol>\r\n");
    CONSOLE_PutMessage(inst, "       <protocol>: 0: TCP     1: UDP\r\n");
	return 1;
} /* End of clicmd_SetConnectTypeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ConnectStatus()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ConnectStatus(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 is_conn;

    if (argc != 0)
	{
		if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_PROTO_TCP)
		{
			is_conn = (GS2W_GetTaskState()==GS2W_STATE_TCP_DATA_PROCESS) ? 1:0;
		}
#if (GUDPDAT_INCLUDE)
		else
		{
			is_conn = (GS2W_GetTaskState()==GS2W_STATE_UDP_DATA_PROCESS) ? 1:0;
		}
#endif
		CONSOLE_PutMessage(inst, "\r\nCurrent r2w connection status: %s\r\n", is_conn ? "Connected":"Idle");
	}
	return 1;
} /* End of clicmd_ConnectStatus() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ConnectStatusHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ConnectStatusHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: connstatus\r\n");
	return 1;
} /* End of clicmd_ConnectStatusHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetServerPort()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetServerPort(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;

    if (argc == 1) /* current setting */
    {
		clicmd_SetServerPortHelp(inst);
        CONSOLE_PutMessage(inst, "Server Port: %d\r\n", GCONFIG_GetDeviceDataPktListenPort());
        return 1;
    }
    else if (argc > 2)
        return -1;

    iTemp = atoi(argv[1]);

    if (iTemp < 1024 || iTemp > 65535)
        return -1;

    GCONFIG_SetDeviceDataPktListenPort((u16)iTemp);
	return 1;
} /* End of clicmd_SetServerPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetServerPortHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetServerPortHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setsrvport <port number>\r\n");
	return 1;
} /* End of clicmd_SetServerPortHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDestPort()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDestPort(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;

    if (argc == 1) /* current setting */
    {
	  	clicmd_SetDestPortHelp(inst);
        CONSOLE_PutMessage(inst, "\r\nClient Dest Port: %d\r\n", GCONFIG_GetClientDestPort());
        return 1;
    }
    else if (argc > 2)
        return -1;

    iTemp = atoi(argv[1]);

    if (iTemp < 1024 || iTemp > 65535)
        return -1;

    GCONFIG_SetClientDestPort((u16)iTemp);

	return 1;
} /* End of clicmd_SetDestPort() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDestPortHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDestPortHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdstport <port number>\r\n");
	return 1;
} /* End of clicmd_SetDestPortHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDestHostName()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDestHostName(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u32 addr;

    if (argc == 1) /* current setting */
    {
	  	clicmd_SetDestHostNameHelp(inst);
		GCONFIG_GetDestHostName(StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent destination host name: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;

    if (checkIpInput(argv[1]))
    {
        if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
            return -1;

        if (validIp(addr) == 0)
            return -1;
    }

    GCONFIG_SetDestHostName(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetDestHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDestHostNameHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDestHostNameHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdsthn <host name | IP address>\r\n");
	return 1;
} /* End of clicmd_SetDestHostNameHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetUrDataMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetUrDataMode(CONSOLE_CLICtl *inst, int argc, char **argv)
{

	/* HSUART back to data mode */
	CONSOLE_PutMessage(inst, "Switching to data mode\r\n");
	GS2W_CmdMode.Enable = 0;
	return 1;
} /* End of clicmd_SetUrDataMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetUrDataModeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetUrDataModeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: urdatamode\r\n");

	return 1;
} /* End of clicmd_SetUrDataModeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiConnect()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiConnect(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int				ret = RTW_ERROR;
	unsigned long	tick1 = xTaskGetTickCount();
	unsigned long	tick2;
	int				mode;
	char 			*ssid;
	rtw_security_t	security_type;
	char 			*password;
	int 			ssid_len;
	int 			password_len;
	int 			key_id;
	void			*semaphore;
	char			wifi_mode_change = 0;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		CONSOLE_PutMessage(inst, "\n\rReject command due to Wifi is turned off now");
		return 1;
	}

	MISC_EnterWiFiBusySection();

	if((argc != 2) && (argc != 3) && (argc != 4))
	{
		clicmd_WifiConnectHelp(inst);
		argv[1] = &argv[0][13];
		ssid_len = GCONFIG_GetWifiSsid(argv[1]);
		argv[1][ssid_len] = '\0';

		switch (GCONFIG_GetWifiEncryptMode())
		{
			case GCONFIG_WIFI_OPEN:
				argc = 2;
				break;
			case GCONFIG_WIFI_WEP:
				argv[2] = &argv[1][ssid_len + 1];
				if (GCONFIG_GetWifiWepKeyLength())
				{
					GCONFIG_GetWifiWep128Key(GCONFIG_GetWifiWepKeyIndex(), argv[2]);
					argv[2][13] = '\0';
					argv[3] = &argv[2][14];
				}
				else
				{
					GCONFIG_GetWifiWep64Key(GCONFIG_GetWifiWepKeyIndex(), argv[2]);
					argv[2][5] = '\0';
					argv[3] = &argv[2][6];
				}
				argv[3][0] = GCONFIG_GetWifiWepKeyIndex() + 0x30;
				argv[3][1] = '\0';
				argc = 4;
				break;
			case GCONFIG_WIFI_WPA2_AES_PSK:
				argv[2] = &argv[1][ssid_len + 1];
				password_len = GCONFIG_GetWifiPreShareKey(argv[2]);
				argv[2][password_len] = '\0';
				argc = 3;
				break;
			default:
				CONSOLE_PutMessage(inst, "\n\rERROR: unsupported security type");
				goto clicmd_WifiConnectExit;
		}
	}

	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if(mode == IW_MODE_MASTER)
	{
#if CONFIG_LWIP_LAYER
		DHCPS_DeInit();
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0)
		{
			CONSOLE_PutMessage(inst, "\n\rERROR: Wifi on failed!");
			goto clicmd_WifiConnectExit;
		}
	}

	ssid = argv[1];
	if(argc == 2)
	{
		security_type = RTW_SECURITY_OPEN;
		password = NULL;
		ssid_len = strlen((const char *)argv[1]);
		password_len = 0;
		key_id = 0;
		semaphore = NULL;
	}
	else if(argc == 3)
	{
		security_type = RTW_SECURITY_WPA2_AES_PSK;
		password = argv[2];
		ssid_len = strlen((const char *)argv[1]);
		password_len = strlen((const char *)argv[2]);
		key_id = 0;
		if (password_len < 8 || password_len > 63)
		{
			CONSOLE_PutMessage(inst, "\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
			goto clicmd_WifiConnectExit;
		}
		semaphore = NULL;
	}
	else
	{
		security_type = RTW_SECURITY_WEP_PSK;
		password = argv[2];
		ssid_len = strlen((const char *)argv[1]);
		password_len = strlen((const char *)argv[2]);
		key_id = atoi(argv[3]);
		if(( password_len != 5) && (password_len != 13))
		{
			CONSOLE_PutMessage(inst, "\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			goto clicmd_WifiConnectExit;
		}
		if((key_id < 0) || (key_id > 3))
		{
			CONSOLE_PutMessage(inst, "\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			goto clicmd_WifiConnectExit;
		}
		semaphore = NULL;
	}

	CONSOLE_PutMessage(inst, "\n\rConnect to %s ...", ssid);
	ret = wifi_connect(ssid,
					security_type,
					password,
					ssid_len,
					password_len,
					key_id,
					semaphore);

	tick2 = xTaskGetTickCount();
	CONSOLE_PutMessage(inst, "\r\nConnected after %dms.\n", (tick2-tick1));
	if(ret != RTW_SUCCESS)
	{
		CONSOLE_PutMessage(inst, "\n\rERROR: Operation failed!");
		goto clicmd_WifiConnectExit;
	}
	else
	{
		CONSOLE_PutMessage(inst, "OK!\r\n");

		/* Save wifi parameters to gconfig */
		GCONFIG_SetWifiRfEnable(1);					/* WIFI RF Enable */
		/* Auto configure DHCP client and server */
		if (GCONFIG_GetWifiNetworkMode()==RTW_MODE_AP)
		{
			wifi_mode_change = 1;
		}

		GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);	/* Network Mode */
		GCONFIG_SetWifiSsid(ssid, ssid_len);		/* SSID */
		GCONFIG_SetWifiEncryptMode(security_type);	/* Security Type */
		if (security_type == RTW_SECURITY_WEP_PSK)
		{
			GCONFIG_SetWifiWepKeyIndex(key_id);		/* WEP Key Index */
			if (password_len == 5)
			{
				GCONFIG_SetWifiWepKeyLength(0);		/* WEP 64bits */
			}
			else
			{
				GCONFIG_SetWifiWepKeyLength(1);		/* WEP 128bits */
			}
		}
		else if (security_type == RTW_SECURITY_WPA2_AES_PSK)
		{
			GCONFIG_SetWifiPreShareKey(password, password_len);	/* WPAx Pre-Shared Key */
		}

		/* System reboot check */
		if (wifi_mode_change)
		{
			/* Enable DHCP client */
			GCONFIG_SetNetwork(GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE);

			/* Disable DHCP server */
			GCONFIG_SetDhcpSrvStatus(0);
			GCONFIG_WriteConfigData();
			clicmd_Reboot(inst);
		}
		else
		{
			GCONFIG_WriteConfigData();
#if CONFIG_LWIP_LAYER
			/* Start DHCP Client */
			ret = MISC_DHCP(0, DHCPC_START, 1);
#endif
			GPIOS_ResetLEDIndicator();
		}
	}

	if (ret != DHCPC_ERROR)
	{
		CONSOLE_PutMessage(inst, "\r\n\nGot IP after %dms.\n", (xTaskGetTickCount() - tick1));
	}
clicmd_WifiConnectExit:
	MISC_LeaveWiFiBusySection();
	return 1;
} /* End of clicmd_WifiConnect() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiConnectHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiConnectHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_connect [SSID] [WPA PASSWORD / WEP KEY(5 or 13)] [WEP KEY ID]\r\n");
	CONSOLE_PutMessage(inst, "       SSID = 1 ~ 32 ASCII characters\r\n");
	CONSOLE_PutMessage(inst, "       WPA PASSWORD = 8 ~ 63 ASCII characters\r\n");
	CONSOLE_PutMessage(inst, "       ASCII WEP KEY = 5 (WEP64) or 13 (WEP128) ASCII characters\r\n");
	CONSOLE_PutMessage(inst, "       WEP KEY ID = 0 ~ 3\r\n");
	return 1;
} /* End of clicmd_WifiConnectHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiDisconnect()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiDisconnect(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int timeout = 20;
	char essid[33];

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		CONSOLE_PutMessage(inst, "\n\rReject command due to Wifi is turned off now");
		return 1;
	}

	CONSOLE_PutMessage(inst, "\n\rDeassociating AP ...\r\n");

	if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0)
	{
		CONSOLE_PutMessage(inst, "WIFI disconnected\r\n");
		return -1;
	}

	if (wifi_disconnect() < 0)
	{
		CONSOLE_PutMessage(inst, "Operation failed!\r\n");
		return -1;
	}

	while (1)
	{
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0)
		{
			CONSOLE_PutMessage(inst, "WIFI disconnected\r\n");
			break;
		}

		if (timeout == 0)
		{
			CONSOLE_PutMessage(inst, "Deassoc timeout!\r\n");
			break;
		}

		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}

	return 1;
} /* End of clicmd_WifiDisconnect() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiDisconnectHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiDisconnectHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_disconnect\r\n");
	return 1;
} /* End of clicmd_WifiDisconnectHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_Reboot()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void clicmd_Reboot(CONSOLE_CLICtl *inst)
{
	if (inst)
		CONSOLE_PutMessage(inst, "System reboot!\r\n");
	vTaskDelay(100);

	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
} /* End of clicmd_Reboot() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SystemReboot()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SystemReboot(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	clicmd_Reboot(inst);
	return 1;
} /* End of clicmd_SystemReboot() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SystemRebootHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SystemRebootHelp(CONSOLE_CLICtl *inst)
{
	// Set processor clock to default before system reset
	CONSOLE_PutMessage(inst, "Usage: reboot\r\n");
	return 1;
} /* End of clicmd_SystemRebootHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiMode(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	mode = GCONFIG_GetWifiNetworkMode();

	if (argc != 2)
	{
		clicmd_WifiModeHelp(inst);
		CONSOLE_PutMessage(inst, "WIFI Network Mode: %s\r\n", (mode == RTW_MODE_STA) ? "STA": ((mode == RTW_MODE_AP) ? "AP":"Unsupported mode"));
		return 1;
	}

	switch (*(argv[1]))
	{
		case '1':
			if (mode==RTW_MODE_STA)
				break;

			/* Enable DHCP client */
			GCONFIG_SetNetwork(GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE);

			/* Disable DHCP server */
			GCONFIG_SetDhcpSrvStatus(0);
			GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);
			break;
		case '2':
			if (mode==RTW_MODE_AP)
				break;

			if (GCONFIG_GetWifiEncryptMode() == GCONFIG_WIFI_WEP)
			{
				CONSOLE_PutMessage(inst, "The WEP is unsupported under AP mode, please change to available security type before switching to AP\r\n.");
				return -1;
			}

			/* Disable DHCP client */
			GCONFIG_SetNetwork(GCONFIG_GetNetwork() & (~GCONFIG_NETWORK_DHCP_ENABLE));

			/* Enable DHCP server */
			GCONFIG_SetDhcpSrvStatus(1);
			GCONFIG_SetWifiNetworkMode(RTW_MODE_AP);
			break;
		default:
			clicmd_WifiModeHelp(inst);
			return -1;
	}

	return 1;
} /* End of clicmd_WifiMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiModeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiModeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_mode [MODE]\r\n");
	CONSOLE_PutMessage(inst, "       MODE = 1(STA), 2(AP)\r\n");
	return 1;
} /* End of clicmd_WifiModeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ScanResultHandler()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static rtw_result_t clicmd_ScanResultHandler( rtw_scan_handler_result_t *malloced_scan_result )
{
	static int ApNum = 0, i;

	if (malloced_scan_result->scan_complete != RTW_TRUE)
	{
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		CONSOLE_CLICtl *inst = (CONSOLE_CLICtl *)malloced_scan_result->user_data;
		if (ApNum==0)
	    	CONSOLE_PutMessage(inst, "\r\nID BSSID             SSID                             TYPE  CH SIGNAL SECURITY\r\n");
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */
		if (ApNum < STA_SCANNED_BSS_MAX)
		{
			wifi_scaned_bss_table[ApNum].occupied = 1;
			wifi_scaned_bss_table[ApNum].network_type = record->bss_type;
			strcpy(wifi_scaned_bss_table[ApNum].ssid, record->SSID.val);
			wifi_scaned_bss_table[ApNum].security_type = record->security;
			CONSOLE_PutMessage(inst, "%02d ", ApNum);
			CONSOLE_PutMessage(inst, "%02x:%02x:%02x:%02x:%02x:%02x "
											, record->BSSID.octet[0]
											, record->BSSID.octet[1]
											, record->BSSID.octet[2]
											, record->BSSID.octet[3]
										  	, record->BSSID.octet[4]
											, record->BSSID.octet[5]);
			CONSOLE_PutMessage(inst, "%s ", record->SSID.val);
			i = 32-record->SSID.len;
			while (i--)
			{
				CONSOLE_PutMessage(inst, " ");
			}
			CONSOLE_PutMessage(inst, "%s ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
			CONSOLE_PutMessage(inst, "%02d ", record->channel);
			CONSOLE_PutMessage(inst, "%02d    ", record->signal_strength);
			CONSOLE_PutMessage(inst, "%s", (record->security == RTW_SECURITY_OPEN) ? "Open" :
								(record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
								(record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
								(record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
								(record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
								(record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
								(record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
								(record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" : "Unknown");
			CONSOLE_PutMessage(inst, "\r\n");
		}
		ApNum++;
	}
	else
	{
		ApNum = 0;
	}

	return RTW_SUCCESS;
} /* End of clicmd_ScanResultHandler() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiScan()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiScan(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;
	int ret = -1;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		CONSOLE_PutMessage(inst, "\n\rReject command due to Wifi is turned off now");
		return 1;
	}

	if (argc > 2)
	{
		int i = 0;
		int num_channel = atoi(argv[1]);

		channel_list = (u8*)pvPortMalloc(num_channel);
		if (!channel_list)
		{
			CONSOLE_PutMessage(inst, "ERROR: Can't malloc memory for channel list\r\n");
			goto clicmd_WifiScanExit;
		}

		pscan_config = (u8*)pvPortMalloc(num_channel);
		if (!pscan_config)
		{
			CONSOLE_PutMessage(inst, "ERROR: Can't malloc memory for pscan_config\r\n");
			goto clicmd_WifiScanExit;
		}

		//parse command channel list
		for(i = 2; i <= argc -1 ; i++){
			*(channel_list + i - 2) = (u8)atoi(argv[i]);
			*(pscan_config + i - 2) = PSCAN_ENABLE;
		}

		if (wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0)
		{
		    CONSOLE_PutMessage(inst, "ERROR: wifi set partial scan channel fail\r\n");
		    goto clicmd_WifiScanExit;
		}
	}

	memset(wifi_scaned_bss_table, 0, sizeof(wifi_scaned_bss_info_t) * STA_SCANNED_BSS_MAX);
	if (wifi_scan_networks(clicmd_ScanResultHandler, inst) != RTW_SUCCESS)
	{
		CONSOLE_PutMessage(inst, "ERROR: wifi scan failed\r\n");
	    goto clicmd_WifiScanExit;
	}
	ret = 1;

clicmd_WifiScanExit:
	if(argc > 2)
	{
		if (channel_list)
		{
			vPortFree(channel_list);
		}
		if (pscan_config)
		{
			vPortFree(pscan_config);
		}
	}
	return ret;
} /* End of clicmd_WifiScan() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiScanHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiScanHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_scan\r\n");
	return 1;
} /* End of clicmd_WifiScanHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiJbss()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiJbss(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8						index;
	rtw_network_info_t		wifi;
	wifi_scaned_bss_info_t	*pBssInfo;
	u8						encryption_mode;
	u8						password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];
	u8						password_len;
	u8						key_id;
	unsigned long			tick1 = xTaskGetTickCount();
	unsigned long			tick2;
	int						ret = RTW_ERROR;
	int						retCode = -1;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		CONSOLE_PutMessage(inst, "\n\rReject command due to Wifi is turned off now");
		return 1;
	}

	MISC_EnterWiFiBusySection();

	if (argc != 2)
	{
		clicmd_WifiJbssHelp(inst);
		goto clicmd_WifiJbssExit;
	}

	if (GCONFIG_GetWifiNetworkMode() != RTW_MODE_STA)
	{
		CONSOLE_PutMessage(inst, "Warning: please switch wifi network mode to STA\r\n");
		goto clicmd_WifiJbssExit;
	}

	index = atoi((const char *)argv[1]);
	if (index >= STA_SCANNED_BSS_MAX)
	{
		CONSOLE_PutMessage(inst, "maximum %d BSSs supported\r\n", STA_SCANNED_BSS_MAX);
		goto clicmd_WifiJbssExit;
	}

	pBssInfo = &wifi_scaned_bss_table[index];

	if (pBssInfo->occupied)
	{
		encryption_mode = GCONFIG_GetWifiEncryptMode();
		switch (pBssInfo->security_type)
		{
			case RTW_SECURITY_OPEN:
				password_len = 0;
				key_id = 0;
				break;
			case RTW_SECURITY_WEP_PSK:
			case RTW_SECURITY_WEP_SHARED:
				key_id = GCONFIG_GetWifiWepKeyIndex();
				if (GCONFIG_GetWifiWepKeyLength())
				{
					GCONFIG_GetWifiWep128Key(key_id, password);
					password_len = 13;
				}
				else
				{
					GCONFIG_GetWifiWep64Key(key_id, password);
					password_len = 5;
				}
				break;
			case RTW_SECURITY_WPA_TKIP_PSK:
			case RTW_SECURITY_WPA_AES_PSK:
			case RTW_SECURITY_WPA2_TKIP_PSK:
			case RTW_SECURITY_WPA2_AES_PSK:
			case RTW_SECURITY_WPA2_MIXED_PSK:
			default:
				key_id = 0;
				password_len = GCONFIG_GetWifiPreShareKey(password);
				break;

//				CONSOLE_PutMessage(inst, "Error: unsupported security type\r\n");
//				break;
		}

		ret = wifi_connect(pBssInfo->ssid,
						   pBssInfo->security_type,
						   password,
						   strlen(pBssInfo->ssid),
						   password_len,
						   key_id,
						   NULL);

		tick2 = xTaskGetTickCount();
		CONSOLE_PutMessage(inst, "Connected after %dms.\r\n", (tick2-tick1));
		if(ret != RTW_SUCCESS)
		{
			CONSOLE_PutMessage(inst, "ERROR: Operation failed!\r\n");
			goto clicmd_WifiJbssExit;
		}
		else
		{
#if CONFIG_LWIP_LAYER
			/* Start DHCP Client */
			ret = MISC_DHCP(0, DHCPC_START, 1);
#endif
			/* Save wifi parameters to gconfig */
			GCONFIG_SetWifiRfEnable(1);					/* WIFI RF Enable */
			GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);	/* Network Mode */
			GCONFIG_SetWifiSsid(pBssInfo->ssid, strlen(pBssInfo->ssid));	/* SSID */
			GCONFIG_SetWifiEncryptMode(pBssInfo->security_type);	/* Security Type */
			if (pBssInfo->security_type == RTW_SECURITY_WEP_PSK)
			{
				GCONFIG_SetWifiWepKeyIndex(key_id);		/* WEP Key Index */
				if (password_len == 5)
				{
					GCONFIG_SetWifiWepKeyLength(0);		/* WEP 64bits */
				}
				else
				{
					GCONFIG_SetWifiWepKeyLength(1);		/* WEP 128bits */
				}
			}
			else if (pBssInfo->security_type == RTW_SECURITY_WPA2_AES_PSK)
			{
				GCONFIG_SetWifiPreShareKey(password, password_len);	/* WPAx Pre-Shared Key */
			}
		}

		if (ret != DHCPC_ERROR)
		{
			CONSOLE_PutMessage(inst, "Got IP after %dms.\r\n", (xTaskGetTickCount() - tick1));
		}
		retCode = 1;
	}
	else
	{
		CONSOLE_PutMessage(inst, "Error: invalid selection\r\n\n");
	}
clicmd_WifiJbssExit:
	MISC_LeaveWiFiBusySection();
	return retCode;
} /* End of clicmd_WifiJbss() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiJbssHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiJbssHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_jbss <INDEX>\r\n");
	CONSOLE_PutMessage(inst, "       INDEX = Index of bss scan table, maximum %d BSSs supported\r\n", STA_SCANNED_BSS_MAX);
	return 1;
} /* End of clicmd_WifiJbssHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiOn()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiOn(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8				mode = GCONFIG_GetWifiNetworkMode();
	u8				minChNum, maxChNum;
	u8				ssid_len;
	u8				ssid[33];
	u8				password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];
	u8				password_len;
	int				channel;
	int				timeout = 20;
#if CONFIG_LWIP_LAYER
	struct ip_addr	ipaddr;
	struct ip_addr	netmask;
	struct ip_addr	gw;
	struct netif	*pnetif = &xnetif[0];
#endif
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	u8		networkBackupMode = GCONFIG_GetNetworkBackupMode();

	if (networkBackupMode == GCONFIG_NETWORK_BACKUP_FORCE_ETH)
	{
		if (mode == RTW_MODE_AP)
		{
			CONSOLE_PutMessage(inst, "Reject, AP mode under force ethernet.\r\n");
			return 1;
		}
	}
	else if (networkBackupMode == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR)
	{
		if ((mode == RTW_MODE_AP) &&
			(MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) &&
			(MISC_NetInterfaceIsLinkUp(IF_ID_ETH)))
		{
			CONSOLE_PutMessage(inst, "Reject, AP mode under ethernet as major and ethernet linked.\r\n");
			return 1;
		}
	}
#endif

	if (GCONFIG_GetWifiRfEnable() == 0)
	{
		if (mode == RTW_MODE_STA)
		{
			if (wifi_on(mode) < 0)
			{
				CONSOLE_PutMessage(inst, "ERROR: Wifi on failed!\r\n");
				return -1;
			}
			GCONFIG_SetWifiRfEnable(1);
		}
		else if (mode == RTW_MODE_AP)
		{
#if CONFIG_LWIP_LAYER
			DHCPS_DeInit();
			ipaddr.addr = ntohl(GCONFIG_GetDeviceStaticIP());
			netmask.addr = ntohl(GCONFIG_GetNetmask());
			gw.addr = ntohl(GCONFIG_GetGateway());
			netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
#ifdef CONFIG_DONT_CARE_TP
			pnetif->flags |= NETIF_FLAG_IPSWITCH;
#endif
#endif
			vTaskDelay(20);
			if (wifi_on(RTW_MODE_AP) < 0)
	{
				CONSOLE_PutMessage(inst, "ERROR: Wifi on failed!\r\n");
				return -1;
			}

			CONSOLE_PutMessage(inst, "Starting AP ...\r\n");
#ifdef CONFIG_WPS_AP
			wpas_wps_dev_config(pnetif->hwaddr, 1);
#endif
			/* Get SSID */
			ssid_len = GCONFIG_GetWifiSsid(ssid);
			ssid[ssid_len] = '\0';

			/* Get channel */
			channel = GCONFIG_GetChannel();

			/* Get WIFI pre-shared key */
			password_len = GCONFIG_GetWifiPreShareKey(password);

			if (GCONFIG_GetWifiEncryptMode() == RTW_SECURITY_OPEN)
			{
				if (wifi_start_ap(ssid,
								  RTW_SECURITY_OPEN,
								  NULL,
								  ssid_len,
								  0,
								  channel) != RTW_SUCCESS)
				{
					CONSOLE_PutMessage(inst, "ERROR: Operation failed!\r\n");
					return -1;
				}
	}
			else
			{
				if (wifi_start_ap(ssid,
								  RTW_SECURITY_WPA2_AES_PSK,
								  password,
								  ssid_len,
								  password_len,
								  channel) != RTW_SUCCESS)
				{
					CONSOLE_PutMessage(inst, "ERROR: Operation failed!\r\n");
					return -1;
				}
			}

			while (1)
			{
				char essid[33];

				if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0)
				{
					if (strcmp((const char *) essid, (const char *)ssid) == 0)
					{
						CONSOLE_PutMessage(inst, "%s started\r\n", ssid);
						GCONFIG_SetWifiRfEnable(1);
						break;
					}
				}

				if (timeout == 0)
				{
					CONSOLE_PutMessage(inst, "ERROR: Start AP timeout!\r\n");
					return -1;
				}

				vTaskDelay(1 * configTICK_RATE_HZ);
				timeout --;
			}

#if CONFIG_LWIP_LAYER
			if (GCONFIG_GetDhcpSrvStatus())
			{
				DHCPS_ReInit();
			}
#endif
		}
	}
	else
	{
		CONSOLE_PutMessage(inst, "WIFI is already running\r\n");
	}
	return 1;

} /* End of clicmd_WifiOn() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiOnHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiOnHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_on\r\n");
	return 1;
} /* End of clicmd_WifiOnHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiOff()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiOff(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	wifi_off();
	GCONFIG_SetWifiRfEnable(0);
	return 1;
} /* End of clicmd_WifiOff() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiOffHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiOffHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_off\r\n");
	return 1;
} /* End of clicmd_WifiOffHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDefaultSettings()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDefaultSettings(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	GCONFIG_ReadDefaultConfigData();
	GCONFIG_WriteConfigData();
	clicmd_Reboot(inst);
	return 1;
} /* End of clicmd_SetDefaultSettings() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDefaultSettingsHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDefaultSettingsHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdef\r\n");
	return 1;
} /* End of clicmd_SetDefaultSettingsHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SaveConfigure()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SaveConfigure(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	GCONFIG_WriteConfigData();
	return 1;
} /* End of clicmd_SaveConfigure() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SaveConfigureHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SaveConfigureHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: saveconfig\r\n");
	return 1;
} /* End of clicmd_SaveConfigureHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GeneratePingEcho()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void clicmd_GeneratePingEcho(unsigned char *buf, int size)
{
	int i;
	struct icmp_echo_hdr *pecho;

	for(i = 0; i < size; i ++) {
		buf[sizeof(struct icmp_echo_hdr) + i] = (unsigned char) i;
	}

	pecho = (struct icmp_echo_hdr *) buf;
	ICMPH_TYPE_SET(pecho, ICMP_ECHO);
	ICMPH_CODE_SET(pecho, 0);
	pecho->chksum = 0;
	pecho->id = pingId;
	pecho->seqno = htons(++ pingSeq);

	//Checksum includes icmp header and data. Need to calculate after fill up icmp header
	pecho->chksum = inet_chksum(pecho, sizeof(struct icmp_echo_hdr) + size);
} /* End of clicmd_GeneratePingEcho() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_Ping()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_Ping(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int i, ping_socket;
	int pint_timeout = 1000;
	struct sockaddr_in to_addr, from_addr;
	int from_addr_len =  sizeof(struct sockaddr_in);
	int ping_size, reply_size;
	unsigned char ping_buf[PING_BufferSize], reply_buf[PING_BufferSize];
	unsigned int ping_time, reply_time;
	struct ip_hdr *iphdr;
	struct icmp_echo_hdr *pecho;

	if (argc != 2)
		return -1;

	//Ping size = icmp header(8 bytes) + data size
	ping_size = sizeof(struct icmp_echo_hdr) + PING_DataSize;
	CONSOLE_PutMessage(inst, "\n\rPING %s %d(%d) bytes of data\n", argv[1], PING_DataSize, sizeof(struct ip_hdr) + ping_size);

	pingId = (u16)xTaskGetTickCount();
	ping_socket = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
	if (ping_socket < 0)
		return -1;

	setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &pint_timeout, sizeof(pint_timeout));
	to_addr.sin_len = sizeof(to_addr);
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = inet_addr(argv[1]);

	for(i = 0; i < 5; i ++)
	{
		clicmd_GeneratePingEcho(ping_buf, PING_DataSize);
		if (sendto(ping_socket, ping_buf, ping_size, 0, (struct sockaddr *) &to_addr, sizeof(to_addr)) < 0)
		{
			CONSOLE_PutMessage(inst, "Failed to send ICMP echo request\r\n");
			break;
		}

		ping_time = xTaskGetTickCount();
		if((reply_size = recvfrom(ping_socket, reply_buf, PING_BufferSize, 0, (struct sockaddr *)&from_addr, (socklen_t *)&from_addr_len))
									>= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)))
		{
			reply_time = xTaskGetTickCount();
			iphdr = (struct ip_hdr *)reply_buf;
			pecho = (struct icmp_echo_hdr *)(reply_buf + (IPH_HL(iphdr) * 4));

			if((pecho->id == pingId) && (pecho->seqno == htons(pingSeq))) {
				CONSOLE_PutMessage(inst, "\n\r %d bytes from %s: icmp_seq=%d time=%d ms", reply_size - sizeof(struct ip_hdr),
								inet_ntoa(from_addr.sin_addr), htons(pecho->seqno), (reply_time - ping_time) * portTICK_RATE_MS);
			}
		}
		else
			CONSOLE_PutMessage(inst, "\n\rRequest timeout for icmp_seq %d\n", pingSeq);

		vTaskDelay(1 * configTICK_RATE_HZ);
	}

	close(ping_socket);
	CONSOLE_PutMessage(inst, "\r\n");
	return 1;
} /* End of clicmd_Ping() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_PingHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_PingHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: ping <IP address>\r\n");
	return 1;
} /* End of clicmd_PingHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiChannel()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiChannel(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	mode;
	u8	channel, minChNum, maxChNum;

	if (GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum)==0)
	{
		CONSOLE_PutMessage(inst, "Invalid country id, set 0 as default\r\n");
		GCONFIG_SetWifiCountryId(RTW_COUNTRY_WORLD1);
		GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);
	}

	if (argc != 2)
	{
		clicmd_WifiChannelHelp(inst);
		CONSOLE_PutMessage(inst, "WIFI Channel: %d\r\n", GCONFIG_GetChannel());
		return 1;
	}

	mode = GCONFIG_GetWifiNetworkMode();

	if (mode == RTW_MODE_AP)
	{
		channel = atoi((const char *)argv[1]);

		if (!((channel >= minChNum) && (channel <= maxChNum)))
		{
			CONSOLE_PutMessage(inst, "Please select a channel between %d ~ %d to match current channel plant.\r\n", minChNum, maxChNum);
			channel = 1;
		}

		/* Save channel to GCONFIG */
		GCONFIG_SetChannel(channel);
	}
	else
	{
		CONSOLE_PutMessage(inst, "Channel parameter is used by AP mdoe\r\n");
	}
	return 1;
} /* End of clicmd_WifiChannel() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiChannelHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiChannelHelp(CONSOLE_CLICtl *inst)
{
	u8	minChNum, maxChNum;

	GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);

	CONSOLE_PutMessage(inst, "Usage: wifi_channel <CHANNEL>\r\n");
	CONSOLE_PutMessage(inst, "       CHANNEL = %d ~ %d\r\n", minChNum, maxChNum);
	return 1;
} /* End of clicmd_WifiChannelHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiSsid()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiSsid(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	ssid_len;
	u8	ssid[33];

	if (argc != 2)
	{
		clicmd_WifiSsidHelp(inst);
		if ((ssid_len = GCONFIG_GetWifiSsid(ssid)))
		{
			ssid[ssid_len] = '\0';
			CONSOLE_PutMessage(inst, "SSID = %s\r\n", ssid);
		}
		else
		{
			CONSOLE_PutMessage(inst, "ERROR: GCONFIG_GetWifiSsid() failed\r\n");
		}

		return 1;
	}

	GCONFIG_SetWifiSsid(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_WifiSsid() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiSsidHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiSsidHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_ssid <SSID>\r\n");
	CONSOLE_PutMessage(inst, "       SSID = 1 ~ 32 ASCII characters\r\n");
	return 1;
} /* End of clicmd_WifiSsidHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiEncrypt()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiEncrypt(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	encryption_mode;
	u8	mode;

	mode = GCONFIG_GetWifiNetworkMode();

	if (argc != 2)
	{
		encryption_mode = GCONFIG_GetWifiEncryptMode();
		CONSOLE_PutMessage(inst, "\n\rWIFI Encryption Mode: %s\r\n", (encryption_mode == GCONFIG_WIFI_OPEN) ? "OPEN" :
				   							   (encryption_mode == GCONFIG_WIFI_WEP && mode == RTW_MODE_STA) ? "WEP" :
											   (encryption_mode == GCONFIG_WIFI_WEP && mode == RTW_MODE_AP) ? "WEP (AP mode not support)" :
											   (encryption_mode == GCONFIG_WIFI_WPA2_AES_PSK && mode == RTW_MODE_STA) ? "WPA/WPA2_AUTO_PSK" :
											   (encryption_mode == GCONFIG_WIFI_WPA2_AES_PSK && mode == RTW_MODE_AP) ? "WPA2_AES_PSK" : "Unknow encryption type");
		clicmd_WifiEncryptHelp(inst);
		return 1;
	}

	encryption_mode = (u8)atoi((const char *)argv[1]);
	if (encryption_mode >= GCONFIG_MAX_WIFI_SECURITY_TYPES)
	{
		return -1;
	}
	if (mode == RTW_MODE_AP && encryption_mode == GCONFIG_WIFI_WEP)
	{
		CONSOLE_PutMessage(inst, "Not support WEP in AP mode\r\n");
		return -1;
	}

	GCONFIG_SetWifiEncryptMode(security_map[encryption_mode]);
	return 1;
} /* End of clicmd_WifiEncrypt() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiEncryptHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiEncryptHelp(CONSOLE_CLICtl *inst)
{
	u8	mode;

	mode = GCONFIG_GetWifiNetworkMode();
	CONSOLE_PutMessage(inst, "Usage: wifi_enc <ENC_MODE>\r\n");
	CONSOLE_PutMessage(inst, "       ENC_MODE = 0(OPEN)\r\n");
	CONSOLE_PutMessage(inst, "                  1(WEP)%s\r\n", (mode == RTW_MODE_STA) ? "" : "(AP mode not support)");
	CONSOLE_PutMessage(inst, "                  2(%s)\r\n", (mode == RTW_MODE_STA) ? "WPA/WPA2_AUTO_PSK" : "WPA2_AES_PSK");
	return 1;
} /* End of clicmd_WifiEncryptHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiKeyId()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiKeyId(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	key_index;

	if (argc != 2)
	{
		CONSOLE_PutMessage(inst, "WIFI WEP Key index: %d\r\n", GCONFIG_GetWifiWepKeyIndex());
		clicmd_WifiKeyIdHelp(inst);
		return 1;
	}

	if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
	{
		CONSOLE_PutMessage(inst, "Not support WEP in AP mode\r\n");
		return -1;
	}

	key_index = (u8)atoi((const char *)argv[1]);

	/* Save WEP Key Index to GCONFIG */
	GCONFIG_SetWifiWepKeyIndex(key_index);
	return 1;
} /* End of clicmd_WifiKeyId() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiKeyIdHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiKeyIdHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_keyid <INDEX>\r\n");
	CONSOLE_PutMessage(inst, "       INDEX = 0 ~ 3\r\n");
	return 1;
} /* End of clicmd_WifiKeyIdHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWepKey()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWepKey(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	i;
	u8	password[13];
	u8	password_len;
	u8	key_index;

	if (argc != 3)
	{
		for (i = 0; i < 4; i++)
		{
			GCONFIG_GetWifiWep64Key(i, password);
			CONSOLE_PutMessage(inst, "WIFI WEP64 Key %d: %c%c%c%c%c\r\n", i,
				   					password[0], password[1], password[2], password[3],	password[4]);
		}
		for (i = 0; i < 4; i++)
		{
			GCONFIG_GetWifiWep128Key(i, password);
			CONSOLE_PutMessage(inst, "WIFI WEP128 Key %d: %c%c%c%c%c%c%c%c%c%c%c%c%c\r\n", i,
				   					password[0], password[1], password[2], password[3],
									password[4], password[5], password[6], password[7],
									password[8], password[9], password[10], password[11], password[12]);
		}
		clicmd_WifiWepKeyHelp(inst);
		return 1;
	}

	if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
	{
		CONSOLE_PutMessage(inst, "Not support WEP in AP mode\r\n");
		return -1;
	}

	key_index = atoi((const char *)argv[1]);
	password_len = strlen(argv[2]);

	if (password_len == 5)
	{
		GCONFIG_SetWifiWep64Key(key_index, (u8 *)argv[2]);
	}
	else if (password_len == 13)
	{
		GCONFIG_SetWifiWep128Key(key_index, (u8 *)argv[2]);
	}

	return 1;
} /* End of clicmd_WifiWepKey() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWepKeyHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWepKeyHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_wepkey <INDEX> <KEY>\r\n");
	CONSOLE_PutMessage(inst, "       INDEX = 0 ~ 3\r\n");
	CONSOLE_PutMessage(inst, "       KEY = 5 or 13 ASCII characters\r\n");
	return 1;
} /* End of clicmd_WifiWepKeyHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWepKeyLen()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWepKeyLen(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 value;

	if (argc != 2)
	{
		CONSOLE_PutMessage(inst, "WEP key length: ");
		if (GCONFIG_GetWifiWepKeyLength()== 0)
            CONSOLE_PutMessage(inst, "64 bits\r\n");
        else
            CONSOLE_PutMessage(inst, "128 bits\r\n");
		return 1;
	}

	value = (u8)atoi((const char *)argv[1]);
    if (value > 1)
	{
		CONSOLE_PutMessage(inst, "Invalid parameter\r\n");
		return -1;
	}

    GCONFIG_SetWifiWepKeyLength(value);
	return 1;
} /* End of clicmd_WifiWepKeyLen() */


/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWepKeyLenHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWepKeyLenHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_wepkeylen <0:64 bits, 1:128 bits>\r\n");
	return 1;
} /* End of clicmd_WifiWepKeyLenHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWpaKey()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWpaKey(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];
	u8	password_len;

	if (argc != 2)
	{
		password_len = GCONFIG_GetWifiPreShareKey(password);
		password[password_len] = '\0';
		CONSOLE_PutMessage(inst, "WIFI WPA Key: %s\r\n", password);
		clicmd_WifiWpaKeyHelp(inst);
		return 1;
	}

	password_len = strlen(argv[1]);
	if (password_len < 8 || password_len > 63)
	{
		CONSOLE_PutMessage(inst, "Wrong PSK length. Must between 8 ~ 63 ASCII characters.\r\n");
		return -1;
	}

	/* Save WPA Key into GCONFIG */
	GCONFIG_SetWifiPreShareKey(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_WifiWpaKey() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiWpaKeyHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiWpaKeyHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_wpakey <KEY>\r\n");
	CONSOLE_PutMessage(inst, "       KEY = 8 ~ 63 ASCII characters\r\n");
	return 1;
} /* End of clicmd_WifiWpaKeyHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiInfo()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiInfo(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int i = 0;
#if CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(&xnetif[0]);
	u8 *ip = LwIP_GetIP(&xnetif[0]);
	u8 *gw = LwIP_GetGW(&xnetif[0]);
#endif
	u8 *ifname[2] = {WLAN0_NAME,WLAN1_NAME};
	int error;

	rtw_wifi_setting_t setting;
	for (i=0;i<NET_IF_NUM;i++)
	{
		/* Filter out wlan interface */
		if (xnetif[i].name[0] != 'r')
			continue;

		if (rltk_wlan_running(i))
		{
#if CONFIG_LWIP_LAYER
			mac = LwIP_GetMAC(&xnetif[i]);
			ip = LwIP_GetIP(&xnetif[i]);
			gw = LwIP_GetGW(&xnetif[i]);
#endif
			CONSOLE_PutMessage(inst, "\n\r\nWIFI %s Status: Running",  ifname[i]);
			CONSOLE_PutMessage(inst, "\n\r==============================");

			rltk_wlan_statistic(i);

			wifi_get_setting((const char*)ifname[i],&setting);

			/* Show current wifi driver setting */
			CONSOLE_PutMessage(inst, "\n\r\nWIFI  %s Setting:", ifname[i]);
			CONSOLE_PutMessage(inst, "\n\r==============================");
			switch(setting.mode)
			{
			case RTW_MODE_AP:
				CONSOLE_PutMessage(inst, "\n\r      MODE => AP");
				break;
			case RTW_MODE_STA:
				CONSOLE_PutMessage(inst, "\n\r      MODE => STATION");
				break;
			default:
				CONSOLE_PutMessage(inst, "\n\r      MODE => UNKNOWN");
				break;
			}
			CONSOLE_PutMessage(inst, "\n\r      SSID => %s", setting.ssid);
			CONSOLE_PutMessage(inst, "\n\r   CHANNEL => %d", setting.channel);
			switch(setting.security_type)
			{
			case RTW_SECURITY_OPEN:
				CONSOLE_PutMessage(inst, "\n\r  SECURITY => OPEN");
				break;
			case RTW_SECURITY_WEP_PSK:
				CONSOLE_PutMessage(inst, "\n\r  SECURITY => WEP");
				CONSOLE_PutMessage(inst, "\n\r KEY INDEX => %d", setting.key_idx);
				break;
			case RTW_SECURITY_WPA_TKIP_PSK:
				CONSOLE_PutMessage(inst, "\n\r  SECURITY => TKIP");
				break;
			case RTW_SECURITY_WPA2_AES_PSK:
				CONSOLE_PutMessage(inst, "\n\r  SECURITY => AES");
				break;
			default:
				CONSOLE_PutMessage(inst, "\n\r  SECURITY => UNKNOWN");
				break;
			}
			CONSOLE_PutMessage(inst, "\n\r  PASSWORD => %s", setting.password);
			CONSOLE_PutMessage(inst, "\n\r");

#if CONFIG_LWIP_LAYER
			CONSOLE_PutMessage(inst, "\n\rInterface (%s)", ifname[i]);
			CONSOLE_PutMessage(inst, "\n\r==============================");
			CONSOLE_PutMessage(inst, "\n\r\tMAC => %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			CONSOLE_PutMessage(inst, "\n\r\tIP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			CONSOLE_PutMessage(inst, "\n\r\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
#endif
			if(setting.mode == RTW_MODE_AP || i == 1)
			{
				int client_number;
				struct {
					int    count;
					rtw_mac_t mac_list[AP_STA_NUM];
				} client_info;

				client_info.count = AP_STA_NUM;
				wifi_get_associated_client_list(&client_info, sizeof(client_info));

				CONSOLE_PutMessage(inst, "\n\rAssociated Client List:");
				CONSOLE_PutMessage(inst, "\n\r==============================");

				if(client_info.count == 0)
				{
					CONSOLE_PutMessage(inst, "\n\rClient Num: 0\n\r", client_info.count);
				}
				else
				{
					CONSOLE_PutMessage(inst, "\n\rClient Num: %d", client_info.count);
				    for( client_number=0; client_number < client_info.count; client_number++ )
				    {
						CONSOLE_PutMessage(inst, "\n\rClient %d:", client_number);
						CONSOLE_PutMessage(inst, "\n\r\tMAC => "MAC_FMT"", MAC_ARG(client_info.mac_list[client_number].octet));
					}
					CONSOLE_PutMessage(inst, "\n\r");
				}
			}

			error = wifi_get_last_error();
			if(setting.mode == RTW_MODE_AP)
			{
				error = RTW_NO_ERROR;
			}
			CONSOLE_PutMessage(inst, "\n\rLast Link Error");
			CONSOLE_PutMessage(inst, "\n\r==============================");
			switch(error)
			{
			case RTW_NO_ERROR:
				CONSOLE_PutMessage(inst, "\n\r\tNo Error");
				break;
			case RTW_NONE_NETWORK:
				CONSOLE_PutMessage(inst, "\n\r\tTarget AP Not Found");
				break;
			case RTW_CONNECT_FAIL:
				CONSOLE_PutMessage(inst, "\n\r\tAssociation Failed");
				break;
			case RTW_WRONG_PASSWORD:
				CONSOLE_PutMessage(inst, "\n\r\tWrong Password");
				break;
			case RTW_DHCP_FAIL:
				CONSOLE_PutMessage(inst, "\n\r\tDHCP Failed");
				break;
			default:
				CONSOLE_PutMessage(inst, "\n\r\tUnknown Error(%d)", error);
				break;
			}
			CONSOLE_PutMessage(inst, "\n\r");
		}
	}
	return 1;
} /* End of clicmd_WifiInfo() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiInfoHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiInfoHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_info\r\n");
	return 1;
} /* End of clicmd_WifiInfoHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiAP()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiAP(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int				timeout = 20;
#if CONFIG_LWIP_LAYER
	struct ip_addr	ipaddr;
	struct ip_addr	netmask;
	struct ip_addr	gw;
	struct netif	*pnetif = &xnetif[0];
#endif
	int				channel;
	rtw_security_t	security_type;
	u8				tmp, minChNum, maxChNum;
	u8				wifi_mode_change = 0;
	int				hiddenSsid = -1;
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	u8				networkBackupMode = GCONFIG_GetNetworkBackupMode();

	if (networkBackupMode == GCONFIG_NETWORK_BACKUP_FORCE_ETH)
	{
		CONSOLE_PutMessage(inst, "Reject, AP mode under force ethernet.\r\n");
		return 1;
	}
	else if (networkBackupMode == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR)
	{
		if ((MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) &&
			(MISC_NetInterfaceIsLinkUp(IF_ID_ETH)))
		{
			CONSOLE_PutMessage(inst, "Reject, AP mode under ethernet as major and ethernet linked.\r\n");
			return 1;
		}
	}
#endif

	if ((argc != 3) && (argc != 4) && (argc != 5))
	{
		clicmd_WifiAPHelp(inst);

		/* Assign argv[1] address and get SSID */
		argv[1] = &argv[0][8];
		tmp = GCONFIG_GetWifiSsid(argv[1]);
		argv[1][tmp] = '\0';

		/* Assign argv[2] address and get channel */
		argv[2] = &argv[1][tmp + 1];

		if (GCONFIG_GetChannel() >= 10)
		{
			argv[2][0] = '1';
			argv[2][1] = ((u8)GCONFIG_GetChannel() - 10) + 0x30;
			argv[2][2] = '\0';
			tmp = 3;
		}
		else
		{
			argv[2][0] = (u8)GCONFIG_GetChannel() + 0x30;
			argv[2][1] = '\0';
			tmp = 2;
		}

		switch (GCONFIG_GetWifiEncryptMode())
		{
			case GCONFIG_WIFI_OPEN:
				/* No security */
				security_type = RTW_SECURITY_OPEN;
				break;
			case GCONFIG_WIFI_WPA2_AES_PSK:
				/* Set security to RTW_SECURITY_WPA2_AES_PSK */
				security_type = RTW_SECURITY_WPA2_AES_PSK;

				/* Assign password to argv[3] */
				argv[3] = &argv[2][tmp];
				tmp = GCONFIG_GetWifiPreShareKey(argv[3]);
				argv[3][tmp] = '\0';
				break;
			default:
				CONSOLE_PutMessage(inst, "ERROR: unsupported security type\r\n");
				return -1;
		}
	}
	else
	{
		if (argc == 3)
		{
			/* No security */
			security_type = RTW_SECURITY_OPEN;

			/* Do not hide SSID */
			hiddenSsid = 0;
		}
		else if (argc == 4)
		{
			tmp = strlen((const char *)argv[3]);

			if (tmp == 1)
			{
				/* No security */
				security_type = RTW_SECURITY_OPEN;

				/* Get hidden SSID parameter */
				hiddenSsid = atoi((const char *)argv[3]);
			}
			else if (tmp < 8 || tmp > 63)
			{
				CONSOLE_PutMessage(inst, "Wrong PSK length. Must between 8 ~ 63 ASCII characters.\r\n");
			return -1;
		}
			else
			{
				/* Set security to RTW_SECURITY_WPA2_AES_PSK */
				security_type = RTW_SECURITY_WPA2_AES_PSK;

				/* Do not hide SSID */
				hiddenSsid = 0;
			}
		}
		else
		{
			/* Check length of password parameter */
			tmp = strlen((const char *)argv[3]);
			if (tmp < 8 || tmp > 63)
			{
				CONSOLE_PutMessage(inst, "Wrong PSK length. Must between 8 ~ 63 ASCII characters.\r\n");
				return -1;
			}

			/* Set security to RTW_SECURITY_WPA2_AES_PSK */
			security_type = RTW_SECURITY_WPA2_AES_PSK;

			/* Get hidden SSID parameter */
			hiddenSsid = atoi((const char *)argv[4]);
		}
	}

	/* Check channel plan */
	channel = atoi((const char *)argv[2]);
	if (GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum)==0)
	{
		CONSOLE_PutMessage(inst, "Invalid country id, set 0 as default\r\n");
		GCONFIG_SetWifiCountryId(RTW_COUNTRY_WORLD1);
		GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);
	}

	if ((channel < minChNum) || (channel > maxChNum))
	{
		CONSOLE_PutMessage(inst, "Please select a channel between %d ~ %d to match current channel plant.\r\n", minChNum, maxChNum);
		return -1;
	}

	/* Set hidden SSID into GCONFIG */
	if (hiddenSsid != -1)
	{
		if (GCONFIG_SetWifiHiddenSsid((u8) hiddenSsid) < 0)
		{
			CONSOLE_PutMessage(inst, "Wrong HIDE value. Must between 0 ~ 1.\r\n");
			return -1;
		}
	}

#if CONFIG_LWIP_LAYER
	DHCPS_DeInit();
	ipaddr.addr = ntohl(GCONFIG_GetDeviceStaticIP());
	netmask.addr = ntohl(GCONFIG_GetNetmask());
	gw.addr = ntohl(GCONFIG_GetGateway());
	netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
#ifdef CONFIG_DONT_CARE_TP
	pnetif->flags |= NETIF_FLAG_IPSWITCH;
#endif
#endif

	/* Start AP mode */
	wifi_off();
	vTaskDelay(20);
	if (wifi_on(RTW_MODE_AP) < 0)
	{
		CONSOLE_PutMessage(inst, "ERROR: Wifi on failed!\r\n");
		return -1;
	}


	CONSOLE_PutMessage(inst, "Starting AP ...");
#ifdef CONFIG_WPS_AP
	wpas_wps_dev_config(pnetif->hwaddr, 1);
#endif

	if (wifi_start_ap(	argv[1],
						security_type,
					  (security_type == RTW_SECURITY_WPA2_AES_PSK) ? argv[3] : NULL,
						strlen((const char *)argv[1]),
					  (security_type == RTW_SECURITY_WPA2_AES_PSK) ? strlen((const char *)argv[3]) : 0,
						channel) != RTW_SUCCESS)
	{
		CONSOLE_PutMessage(inst, "ERROR: Operation failed!\r\n");
		return -1;
	}
	else
	{
		CONSOLE_PutMessage(inst, "OK!\r\n");
	}

	/* Save wifi configuration */
	while (1)
	{
		char essid[33];

		if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0)
		{
			if (strcmp((const char *) essid, (const char *)argv[1]) == 0)
			{
				CONSOLE_PutMessage(inst, "%s started\r\n", argv[1]);

				/* Save wifi parameters to gconfig */
				GCONFIG_SetWifiRfEnable(1);						/* WIFI RF Enable */
				if (GCONFIG_GetWifiNetworkMode()==RTW_MODE_STA)
					wifi_mode_change = 1;
				GCONFIG_SetWifiNetworkMode(RTW_MODE_AP);		/* Network Mode */
				GCONFIG_SetChannel((u8)channel);				/* Channel */
				GCONFIG_SetWifiSsid(essid, strlen(essid));		/* SSID */
				GCONFIG_SetWifiEncryptMode(security_type);		/* Security Type */
				if (security_type == RTW_SECURITY_WPA2_AES_PSK)
				{
					GCONFIG_SetWifiPreShareKey(argv[3], strlen((const char *)argv[3]));	/* WPAx Pre-Shared Key */
				}
				GPIOS_ResetLEDIndicator();

				/* System reboot check */
				if (wifi_mode_change)
				{
					/* Disable DHCP client */
					GCONFIG_SetNetwork(GCONFIG_GetNetwork() & (~GCONFIG_NETWORK_DHCP_ENABLE));

					/* Enable DHCP server */
					GCONFIG_SetDhcpSrvStatus(1);
					GCONFIG_WriteConfigData();
					clicmd_Reboot(inst);
				}
				else
				{
					GCONFIG_WriteConfigData();
				}
				break;
			}
		}

		if (timeout == 0)
		{
			CONSOLE_PutMessage(inst, "ERROR: Start AP timeout!\r\n");
			break;
		}

		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}

	/* Re-Start DHCP server if need */
#if CONFIG_LWIP_LAYER
	if (GCONFIG_GetDhcpSrvStatus())
	{
		DHCPS_ReInit();
	}
#endif
	return 1;
} /* End of clicmd_WifiAP() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_WifiAPHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_WifiAPHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_ap <ssid> <channel> <wpa key> <hide>\r\n");
	CONSOLE_PutMessage(inst, "       <ssid>: 1~32 ASCII characters\r\n");
	CONSOLE_PutMessage(inst, "       <channel>: 1~14\r\n");
	CONSOLE_PutMessage(inst, "       <wpa key>: WPA(8~63 ASCII characters) key\r\n");
	CONSOLE_PutMessage(inst, "       <hide>: 0~1\r\n");
	return 1;
} /* End of clicmd_WifiAPHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_UrConfig()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_UrConfig(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1)
	{
		clicmd_UrConfigHelp(inst);
		CONSOLE_PutMessage(inst, "\n\rUart Settings\r\n");
		CONSOLE_PutMessage(inst, "Baudrate: %d\r\n", GCONFIG_GetUrBaudRate());
		CONSOLE_PutMessage(inst, "Databits: %d\r\n", GCONFIG_GetUrDataBits());
		CONSOLE_PutMessage(inst, "Stopbits: %d\r\n", GCONFIG_GetUrStopBits());
		CONSOLE_PutMessage(inst, "Parity: %d\r\n", GCONFIG_GetUrParity());
		CONSOLE_PutMessage(inst, "FlowCtrl: %d\r\n", GCONFIG_GetUrFlowCtrl());
		return 1;
	}
	else if (argc < 6)
	{
		CONSOLE_PutMessage(inst, "\n\rUsage: ur_config baudrate databits stopbit parity flowctrl\r\n");
		return -1;
	}

	GCONFIG_SetUrBaudRate(atol(argv[1]));
	GCONFIG_SetUrDataBits(atoi(argv[2]));
	GCONFIG_SetUrStopBits(atoi(argv[3]));
	GCONFIG_SetUrParity(atoi(argv[4]));
	GCONFIG_SetUrFlowCtrl(atoi(argv[5]));
	return 1;
} /* End of clicmd_UrConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_UrConfigHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_UrConfigHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: ur_config <baud_rate> <databits> <stop_bits> <parity> <flow_contrl>\r\n");
	CONSOLE_PutMessage(inst, "       <baud_rate>: \r\n");
	CONSOLE_PutMessage(inst, "                    1200 bps\r\n");
	CONSOLE_PutMessage(inst, "                    2400 bps\r\n");
	CONSOLE_PutMessage(inst, "                    4800 bps\r\n");
	CONSOLE_PutMessage(inst, "                    9600 bps\r\n");
	CONSOLE_PutMessage(inst, "                    19200 bps\r\n");
	CONSOLE_PutMessage(inst, "                    38400 bps\r\n");
	CONSOLE_PutMessage(inst, "                    57600 bps\r\n");
	CONSOLE_PutMessage(inst, "                    115200 bps\r\n");
	CONSOLE_PutMessage(inst, "                    921600 bps\r\n");
	CONSOLE_PutMessage(inst, "       <databits>: 7 or 8 bits\r\n");
	CONSOLE_PutMessage(inst, "       <stop_bits>: 1 or 2 bit(s)\r\n");
	CONSOLE_PutMessage(inst, "       <parity>: 0 = none, 1 = odd, 2 = even\r\n");
	CONSOLE_PutMessage(inst, "       <flow_contrl>: 0 = disable, 1 = enable CTS/RTS flow control\r\n");
	return 1;
} /* End of clicmd_UrConfigHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_DhcpClient()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_DhcpClient(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u16 temp16;
	int iVal;

	temp16 = GCONFIG_GetNetwork();
	if (argc == 1)
	{
		clicmd_DhcpClientHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent DHCP client status: %s\r\n", (temp16 & GCONFIG_NETWORK_DHCP_ENABLE) ? "enable":"disable");
		return 1;
	}
	else if (argc > 2)
	{
		return -1;
	}

	iVal = atoi(argv[1]);
	temp16 &= ~GCONFIG_NETWORK_DHCP_ENABLE;
    if (iVal > 0)
        temp16 |= GCONFIG_NETWORK_DHCP_ENABLE;

	GCONFIG_SetNetwork(temp16);
	return 1;
} /* End of clicmd_DhcpClient() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_DhcpClientHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_DhcpClientHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: dhcpclient <status>\r\n");
	CONSOLE_PutMessage(inst, "       <status>: 0: disable     1: enable\r\n");
	return 1;
} /* End of clicmd_DhcpClientHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_EnableDhcpServer()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_EnableDhcpServer(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;

	if (argc == 1) /* current setting */
    {
		clicmd_EnableDhcpServerHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent DHCP server status: %s\r\n", GCONFIG_GetDhcpSrvStatus() ? "enable":"disable");
        return 1;
    }
    else if (argc > 2)
        return -1;

    iTemp = atoi(argv[1]);
    if (iTemp > 1)
        return -1;

    GCONFIG_SetDhcpSrvStatus(iTemp);
	return 1;
} /* End of clicmd_EnableDhcpServer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_EnableDhcpServerHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_EnableDhcpServerHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdhcpsrv <status>\r\n");
	CONSOLE_PutMessage(inst, "       <status>: 0: disable     1: enable\r\n");
	return 1;
} /* End of clicmd_EnableDhcpServerHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_NtpServer()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_NtpServer(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u8 temp8, *ptmp;

    if (argc == 1) /* current setting */
    {
		clicmd_NtpServerHelp(inst);
        CONSOLE_PutMessage(inst, "\r\nCurrent settings:\r\n");
		for (temp8=0; temp8<GCONFIG_MAX_NTP_SERVERS; temp8++)
		{
			ptmp = GCONFIG_GetNtpSrvHostName(temp8);
			CONSOLE_PutMessage(inst, "NTP Server Name[%d]: %s\r\n", temp8, (ptmp[0]=='\0') ? "N/A":(char*)ptmp);
		}
        GCONFIG_GetNtpTimeZone(&temp8);
        CONSOLE_PutMessage(inst, "Time Zone: %s\r\n", GCONFIG_TimeZoneTable[temp8].Description);
        return 1;
    }
    else if (argc != 5)
        return -1;

    temp8 = (U8_T)atoi(argv[1]);
    if (temp8 > 24)
        return -1;
    GCONFIG_SetNtpTimeZone(temp8);

	for (temp8=0; temp8<GCONFIG_MAX_NTP_SERVERS; temp8++)
	{
    	GCONFIG_SetNtpSrvHostName(temp8, argv[temp8+2]);
	}

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
		SNTPC_ReqEnqueue(RTC_NtpQuery, 0);
	return 1;
} /* End of clicmd_NtpServer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_NtpServerHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_NtpServerHelp(CONSOLE_CLICtl *inst)
{
	u8 i;
	CONSOLE_PutMessage(inst, "Usage: ntpsrv <time zone> <ntp server1> <ntp server2> <ntp server3>\r\n");
	CONSOLE_PutMessage(inst, "       <time zone>\r\n");
    for (i=0; i<GCONFIG_MAX_TIME_ZONE; i++)
   	{
   		CONSOLE_PutMessage(inst, "       %d: %s\r\n", i, GCONFIG_TimeZoneTable[i].Description);
   	}
	return 1;
} /* End of clicmd_NtpServerHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_RtcTimeSetup()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_RtcTimeSetup(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u8 temp8, *argP;
    u16 temp16;

    if (argc == 1)
    {
		clicmd_RtcTimeSetupHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent RTC time setup: %s\r\n", (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP) ? "NTP server":"manual");
        return 1;
    }
    else if (argc != 2)
        return -1;

    argP = (u8*)argv[1];
    if ((strlen(argP) == 1) && IsDigit(*argP))
        temp8 = *argP - 0x30;
    else
        return -1;

    if (temp8 > 1)
        return -1;

    temp16 = GCONFIG_GetNetwork();
    if (temp8)
        temp16 |= GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;
    else
        temp16 &= ~GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;
    GCONFIG_SetNetwork(temp16);

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
		SNTPC_ReqEnqueue(RTC_NtpQuery, 0);
	return 1;
} /* End of clicmd_RtcTimeSetup() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_RtcTimeSetupHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_RtcTimeSetupHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: rtcts <mode>\r\n");
	CONSOLE_PutMessage(inst, "       <mode>: 0: manual    1: NTP server\r\n");
	return 1;
} /* End of clicmd_RtcTimeSetupHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_CurrentTime()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_CurrentTime(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u8 temp8;
	RTC_TIME timeNow;

    if (argc == 1)
    {
        if (RTC_GetCurrTime(&timeNow) == TRUE)
        {
            CONSOLE_PutMessage(inst, "Current Time: %d:%d:%d\r\n", timeNow.Hour, timeNow.Minute, timeNow.Second);
        }
        else
            CONSOLE_PutMessage(inst, "Failed to get current time\r\n");

        return 1;
    }
    else if (argc != 4)
        return -1;

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{
        CONSOLE_PutMessage(inst, "Cannot set time in NTP time setup mode\r\n");
		return -1;
	}

    if (RTC_GetCurrTime(&timeNow) == FALSE)
    {
        CONSOLE_PutMessage(inst, "Failed to get current time\r\n");
        return -1;
    }

    timeNow.Hour = (u8)atoi(argv[1]);
    timeNow.Minute = (u8)atoi(argv[2]);
    timeNow.Second = (u8)atoi(argv[3]);
    if (RTC_SetCurrTime(&timeNow) == FALSE)
        CONSOLE_PutMessage(inst, "Failed to set current time\r\n");
	return 1;
} /* End of clicmd_CurrentTime() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_CurrentTimeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_CurrentTimeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: time <hour> <minute> <second>\r\n");
	CONSOLE_PutMessage(inst, "       <hour>: 0~%d\r\n", RTC_MAX_HOUR);
	CONSOLE_PutMessage(inst, "       <minute>: 0~%d\r\n", RTC_MAX_MINUTE);
	CONSOLE_PutMessage(inst, "       <second>: 0~%d\r\n", RTC_MAX_SECOND);
	return 1;
} /* End of clicmd_CurrentTimeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_CurrentDate()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_CurrentDate(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u8 temp8;
    u16 temp16;
    u32 tmp32 = 0;
	RTC_TIME timeNow;

    if (argc == 1)
    {
        if (RTC_GetCurrTime(&timeNow) == TRUE)
            CONSOLE_PutMessage(inst, "Current Date: %d-%d-%d %s\r\n", timeNow.Year, timeNow.Month, timeNow.Date, GCONFIG_WEEK_TABLE[timeNow.Day]);
        else
            CONSOLE_PutMessage(inst, "Failed to get current date\r\n");

        return 1;
    }
    else if (argc != 4)
        return -1;

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{
        CONSOLE_PutMessage(inst, "Cannot set date in NTP time setup mode\r\n");
		return -1;
	}

    if (RTC_GetCurrTime(&timeNow) == FALSE)
    {
        CONSOLE_PutMessage(inst, "Failed to get current date\r\n");
        return -1;
    }

    /* year */
    temp16 = (u16)atoi(argv[1]);
    if (temp16 > 2099 || temp16 < 2000)
        return -1;
    timeNow.Year = temp16;
    /* month */
    temp8 = (u8)atoi(argv[2]);
    if (temp8 > 12 || temp8 == 0)
        return -1;
    timeNow.Month = temp8;
    /* date */
    temp8 = (u8)atoi(argv[3]);
    if (temp8 > 31 || temp8 == 0)
        return -1;

    /* leap year */
    temp16 = (temp16 % 4);
    if (timeNow.Month == 2)
    {
        if (!temp16 && temp8 > 29)
            return -1;
        else if (temp16 && temp8 > 28)
            return -1;
    }
    else if (timeNow.Month < 8)
    {
        if (!(timeNow.Month&0x1) && temp8 > 30)
            return -1;
    }
    else if (timeNow.Month > 7 && (timeNow.Month&0x1) && temp8 > 30)
        return -1;

    timeNow.Date = temp8;

    if (RTC_SetCurrTime(&timeNow) == FALSE)
        CONSOLE_PutMessage(inst, "Failed to set current date\r\n");
	return 1;
} /* End of clicmd_CurrentDate() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_CurrentDateHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_CurrentDateHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: date <year> <month> <date>\r\n");
	CONSOLE_PutMessage(inst, "       <year>: %d~%d\r\n", RTC_MIN_YEAR, RTC_MAX_YEAR);
	CONSOLE_PutMessage(inst, "       <month>: 1~%d\r\n", RTC_MAX_MONTH);
	CONSOLE_PutMessage(inst, "       <date>: 1~%d\r\n", RTC_MAX_DATE);
	return 1;
} /* End of clicmd_CurrentDateHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetTemperatureHumidity()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetTemperatureHumidity(CONSOLE_CLICtl *inst, int argc, char **argv)
{
 	float fval;

    if (argc == 1)
    {
       	if (THS_GetTemperature(&fval) == TRUE)
           	CONSOLE_PutMessage(inst, "Current temperature: %.2fdegC\r\n", fval);
       	else
           	CONSOLE_PutMessage(inst, "Failed to get current temperature\r\n");

       	if (THS_GetRelativeHumidity(&fval) == TRUE)
           	CONSOLE_PutMessage(inst, "Current relative humidity: %.2f%%\r\n", fval);
       	else
           	CONSOLE_PutMessage(inst, "Failed to get current relative humidity\r\n");
    }
	return 1;
} /* End of clicmd_GetTemperatureHumidity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetTemperatureHumidityHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetTemperatureHumidityHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: getths\r\n");
	return 1;
} /* End of clicmd_GetTemperatureHumidityHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailServerAddr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailServerAddr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    u32 addr;

    if (argc == 1) /* current setting */
    {
		clicmd_SetEmailServerAddrHelp(inst);
		GCONFIG_GetSMTPDomainName((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent e-mail server address: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;

    if (checkIpInput(argv[1]))
    {
        if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
            return -1;

        if (validIp(addr) == 0)
            return -1;
    }

    GCONFIG_SetSMTPDomainName(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetEmailServerAddr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailServerAddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailServerAddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setems <e-mail server domain name>\r\n");
	return 1;
} /* End of clicmd_SetEmailServerAddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailFromAddr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailFromAddr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
    if (argc == 1) /* current setting */
    {
		clicmd_SetEmailFromAddrHelp(inst);
		GCONFIG_GetSMTPFrom((U8_T*)StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent e-mail from address: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;

	GCONFIG_SetSMTPFrom(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetEmailFromAddr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailFromAddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailFromAddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemf <e-mail address>\r\n");
	return 1;
} /* End of clicmd_SetEmailFromAddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo1Addr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo1Addr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		clicmd_SetEmailTo1AddrHelp(inst);
		GCONFIG_GetSMTPTo1((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent e-mail to1 address: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;
	GCONFIG_SetSMTPTo1(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetEmailTo1Addr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo1AddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo1AddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemt1 <e-mail address>\r\n");
	return 1;
} /* End of clicmd_SetEmailTo1AddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo2Addr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo2Addr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		clicmd_SetEmailTo2AddrHelp(inst);
		GCONFIG_GetSMTPTo2((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent e-mail to2 address: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;
	GCONFIG_SetSMTPTo2(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetEmailTo2Addr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo2AddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo2AddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemt2 <e-mail address>\r\n");
	return 1;
} /* End of clicmd_SetEmailTo2AddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo3Addr()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo3Addr(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		clicmd_SetEmailTo3AddrHelp(inst);
		GCONFIG_GetSMTPTo3((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "\r\nCurrent e-mail to3 address: %s\r\n", StrBuf);
        return 1;
    }
    else if (argc > 2)
        return -1;
	GCONFIG_SetSMTPTo3(argv[1], strlen(argv[1]));
	return 1;
} /* End of clicmd_SetEmailTo3Addr() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEmailTo3AddrHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEmailTo3AddrHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemt3 <e-mail address>\r\n");
	return 1;
} /* End of clicmd_SetEmailTo3AddrHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetSMTPSecurity()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetSMTPSecurity(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 tmp8, *pdsc;
	u16 tmp16;

	if (argc == 1) /* current setting */
    {
		clicmd_SetSMTPSecurityHelp(inst);
		GCONFIG_GetSMTPSecurity(&tmp8, &tmp16);
		switch (tmp8)
		{
		case GCONFIG_SMTP_NO_SECURITY:
			pdsc = "No security";
			break;
		case GCONFIG_SMTP_SSL:
			pdsc = "SSL";
			break;
		case GCONFIG_SMTP_TLS:
			pdsc = "TLS";
			break;
		case GCONFIG_SMTP_AUTO:
			pdsc = "Auto";
			break;
		}
		CONSOLE_PutMessage(inst, "\n\rCurrent SMTP security type: %s, port number:%u\r\n", pdsc, tmp16);
        return 1;
    }
    else if (argc != 3)
        return -1;
	GCONFIG_SetSMTPSecurity((u8)atoi(argv[1]), atoi(argv[2]));
	return 1;
} /* End of clicmd_SetSMTPSecurity() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetSMTPSecurityHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetSMTPSecurityHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemsc <SecurityType> <PortNumber>\r\n");
	CONSOLE_PutMessage(inst, "       <SecurityType>:\r\n");
	CONSOLE_PutMessage(inst, "       0=No security\r\n");
	CONSOLE_PutMessage(inst, "       1=SSL\r\n");
	CONSOLE_PutMessage(inst, "       2=TLS\r\n");
	CONSOLE_PutMessage(inst, "       3=Auto\r\n\n");
	CONSOLE_PutMessage(inst, "       <PortNumber>:\r\n");
	CONSOLE_PutMessage(inst, "       25 or 587 for regular transfer port\r\n");
	CONSOLE_PutMessage(inst, "       465 for SSL port\r\n");
	return 1;
} /* End of clicmd_SetSMTPSecurityHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetSMTPAccount()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetSMTPAccount(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		clicmd_SetSMTPAccountHelp(inst);
		GCONFIG_GetSMTPAccount(StrBuf, GCONFIG_MAX_MAILADDR_LEN, StrBuf+GCONFIG_MAX_MAILADDR_LEN, GCONFIG_MAX_MAILADDR_LEN);
		CONSOLE_PutMessage(inst, "\r\nCurrent Username/Password: %s/%s\r\n", StrBuf, StrBuf+GCONFIG_MAX_MAILADDR_LEN);
        return 1;
    }
    else if (argc != 3)
        return -1;
	GCONFIG_SetSMTPAccount(argv[1], argv[2]);
	return 1;
} /* End of clicmd_SetSMTPAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetSMTPAccountHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetSMTPAccountHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setemac <UserName> <PassWord>\r\n");
	return 1;
} /* End of clicmd_SetSMTPAccountHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetEmailConfig()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetEmailConfig(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8 *ptmp;

    if (argc == 1)
    {
		CONSOLE_PutMessage(inst, "Current E-mail Settings:\r\n");
		GCONFIG_GetSMTPDomainName((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "Mail Server address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPFrom((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "Mailbox from address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo1((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "Mailbox To1 address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo2((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "Mailbox To2 address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo3((u8*)StrBuf);
 		CONSOLE_PutMessage(inst, "Mailbox To3 address: %s\r\n", StrBuf);

		GCONFIG_GetSMTPSecurity((u8*)StrBuf, (u16*)StrBuf+2);
		switch (StrBuf[0])
		{
		case GCONFIG_SMTP_NO_SECURITY:
		 	ptmp = "No security";
			break;
		case GCONFIG_SMTP_SSL:
		 	ptmp = "SSL";
			break;
		case GCONFIG_SMTP_TLS:
		 	ptmp = "TLS";
			break;
		case GCONFIG_SMTP_AUTO:
		 	ptmp = "AUTO";
			break;
		}
 		CONSOLE_PutMessage(inst, "SMTP Security: %s, port=%d\r\n", ptmp, *((u16*)StrBuf+2));

		GCONFIG_GetSMTPAccount(StrBuf, GCONFIG_MAX_MAILADDR_LEN, StrBuf+GCONFIG_MAX_MAILADDR_LEN, GCONFIG_MAX_MAILADDR_LEN);
		CONSOLE_PutMessage(inst, "Username/Password: %s/%s\r\n", StrBuf, StrBuf+GCONFIG_MAX_MAILADDR_LEN);
    }
    return 1;
} /* End of clicmd_GetEmailConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetEmailConfigHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetEmailConfigHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: emconfig\r\n");
	return 1;
} /* End of clicmd_GetEmailConfigHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAWConfig()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAWConfig(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;
    u16 temp16;

    temp16 = GCONFIG_GetAutoWarning();
    if (argc == 1) /* current setting */
    {
		clicmd_SetAWConfigHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent auto warning settings:\r\n");
		CONSOLE_PutMessage(inst, "  cold start:           %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_COLDSTART) ? "Enable":"Disable");
		CONSOLE_PutMessage(inst, "  authentication fail:  %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_AUTH_FAIL) ? "Enable":"Disable");
		CONSOLE_PutMessage(inst, "  ip changed:           %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_IP_CHANGED) ? "Enable":"Disable");
		CONSOLE_PutMessage(inst, "  password changed:     %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_PSW_CHANGED) ? "Enable":"Disable");
        return 1;
    }
    else if (argc != 5)
        return -1;

    /* cold start */
	val = atoi(argv[1]);
	if (val == 0)
		temp16 &= ~GCONFIG_SMTP_EVENT_COLDSTART;
	else if (val == 1)
		temp16 |= GCONFIG_SMTP_EVENT_COLDSTART;

    /* authentication fail */
	val = atoi(argv[2]);
	if (val == 0)
		temp16 &= ~GCONFIG_SMTP_EVENT_AUTH_FAIL;
	else if (val == 1)
		temp16 |= GCONFIG_SMTP_EVENT_AUTH_FAIL;

    /* IP changed */
	val = atoi(argv[3]);
	if (val == 0)
		temp16 &= ~GCONFIG_SMTP_EVENT_IP_CHANGED;
	else if (val == 1)
		temp16 |= GCONFIG_SMTP_EVENT_IP_CHANGED;

    /* password changed */
	val = atoi(argv[4]);
	if (val == 0)
		temp16 &= ~GCONFIG_SMTP_EVENT_PSW_CHANGED;
	else if (val == 1)
		temp16 |= GCONFIG_SMTP_EVENT_PSW_CHANGED;

    GCONFIG_SetAutoWarning(temp16);
	return 1;
} /* End of clicmd_SetAWConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAWConfigHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAWConfigHelp(CONSOLE_CLICtl *inst)
{
   	CONSOLE_PutMessage(inst, "Usage: setaw <cold start> <authentication fail> <ip changed> <password changed>\r\n");
   	CONSOLE_PutMessage(inst, "       <cold start>:             0: Disable     1: Enable\r\n");
   	CONSOLE_PutMessage(inst, "       <authentication fail>:    0: Disable     1: Enable\r\n");
   	CONSOLE_PutMessage(inst, "       <ip changed>:             0: Disable     1: Enable\r\n");
   	CONSOLE_PutMessage(inst, "       <password changed>:       0: Disable     1: Enable\r\n");
	return 1;
} /* End of clicmd_SetAWConfigHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_JtagOff()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_JtagOff(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;

	val = GCONFIG_GetJtagOff();
	if (argc == 1)
	{
		clicmd_JtagOffHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent jtag status: %s\r\n", val ? "disable":"enable");
	}
	else if (argc == 2)
	{
		val = atoi(argv[1]);
		if (val < 2 && val >= 0)
		{
			GCONFIG_SetJtagOff((u8)val);
		}
		else
		{
			CONSOLE_PutMessage(inst, "Invalid value input\r\n");
		}
	}
	return 1;
} /* End of clicmd_JtagOff() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_JtagOffHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_JtagOffHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: jtagoff <status>\r\n");
    CONSOLE_PutMessage(inst, "       <status>: 0: enable     1: disable\r\n");
	return 1;
} /* End of clicmd_JtagOffHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetOTAName()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetOTAName(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	GCONFIG_GetFilename(&StrBuf[0], GCONFIG_MAX_FILENAME_LEN);
	CONSOLE_PutMessage(inst, "\n\rOTA name for firmware upgrade: %s\n\r", (char *)StrBuf);
	return 1;
} /* End of clicmd_GetOTAName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_GetOTANameHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_GetOTANameHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: getotaname\r\n");
	return 1;
} /* End of clicmd_GetOTANameHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetCountryId()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetCountryId(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
		clicmd_SetCountryIdHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent country id: %d\r\n", GCONFIG_GetWifiCountryId());
	}
	else if (argc == 2)
	{
		val = atoi(argv[1]);
		if (GCONFIG_SetWifiCountryId((u8)val) < 0)
			return -1;
	}
	return 1;
} /* End of clicmd_SetCountryId() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetCountryIdHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetCountryIdHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: countryid <id>\r\n");
   	CONSOLE_PutMessage(inst, "       <id>:\r\n");
	CONSOLE_PutMessage(inst, "       %d=World wide 13(2G_WORLD: 1~13)\r\n", RTW_COUNTRY_WORLD1);
	CONSOLE_PutMessage(inst, "       %d=Europe(2G_ETSI1: 1~13)\r\n", RTW_COUNTRY_ETSI1);
	CONSOLE_PutMessage(inst, "       %d=United States(2G_FCC1: 1~11)\r\n", RTW_COUNTRY_FCC1);
	CONSOLE_PutMessage(inst, "       %d=Japan(2G_MKK1: 1~14)\r\n", RTW_COUNTRY_MKK1);
	CONSOLE_PutMessage(inst, "       %d=France(2G_ETSI2: 10~13)\r\n", RTW_COUNTRY_ETSI2);
	CONSOLE_PutMessage(inst, "       %d=United States(2G_FCC2: 1~13)\r\n", RTW_COUNTRY_FCC2);
	CONSOLE_PutMessage(inst, "       %d=Japan(2G_MKK2: 1~13)\r\n", RTW_COUNTRY_MKK2);
	CONSOLE_PutMessage(inst, "       Otherwise=Unavailable\r\n");
	return 1;
} /* End of clicmd_SetCountryIdHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDhcpServer()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDhcpServer(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;
	u32 addr;

	if (argc == 1) /* current setting */
    {
		clicmd_SetDhcpServerHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent DHCP server settings:\r\n");

        addr = GCONFIG_GetDhcpSrvStartIp();
        CONSOLE_PutMessage(inst, "Start IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
													(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
        addr = GCONFIG_GetDhcpSrvEndIp();
        CONSOLE_PutMessage(inst, "End IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
												(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
		addr = GCONFIG_GetDhcpSrvDefGateway();
		CONSOLE_PutMessage(inst, "Default gateway IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
												(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));

        CONSOLE_PutMessage(inst, "Lease Time: %d min\r\n", GCONFIG_GetDhcpSrvLeaseTime());

        return 1;
    }
    else if (argc > 5)
        return -1;

	if(argc > 1)
	{
	  /*Set Start IP*/
	  if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
		  return -1;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvStartIp(addr);
	}

	if(argc > 2)
	{
	  /*Set End IP*/
	  if (getIp2Ulong(argv[2], strlen(argv[2]), &addr) < 0)
		  return -1;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvEndIp(addr);
	}

	if(argc > 3)
	{
	  /*Set Gateway IP*/
	  if (getIp2Ulong(argv[3], strlen(argv[3]), &addr) < 0)
		  return -1;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvDefGateway(addr);
	}

	if(argc > 4)
	{
	  iTemp = atoi(argv[4]);
	  GCONFIG_SetDhcpSrvLeaseTime((u16)iTemp);
	}

	return 1;
} /* End of clicmd_SetDhcpServer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDhcpServerHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDhcpServerHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: dhcpsrv <start addr> <end addr> <default gateway addr> <lease>\r\n");
	return 1;
} /* End of clicmd_SetDhcpServerHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEtherTimer()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEtherTimer(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
		clicmd_SetEtherTimerHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent transmitimer: %d\r\n", GCONFIG_GetEthernetTxTimer());
		return 1;
	}
    else if (argc > 2)
        return -1;

	val = atoi(argv[1]);
    if (val < 10 || val > 65535)
        return -1;

    GCONFIG_SetEthernetTxTimer((u16)val);
	return 1;
} /* End of clicmd_SetEtherTimer() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetEtherTimerHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetEtherTimerHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: transmitimer <time>\r\n");
    CONSOLE_PutMessage(inst, "       <time>: time in ms, available value range is 10~65535 ms\r\n");
	return 1;
} /* End of clicmd_SetEtherTimerHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_DhcpServerAllocTbl()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_DhcpServerAllocTbl(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;
	u32 addr;

	memset(dhcps_allocation_table, 0, sizeof(dhcps_allocation_table));
	addr = DHCPS_AllocationTable(dhcps_allocation_table, DHCPS_MAX_IP_POOL_NUM);
	CONSOLE_PutMessage(inst, "\r\nCurrent IP allocation table%s\r\n", addr==0 ? " is empty":"...");
	if (addr)
	{
		CONSOLE_PutMessage(inst, "Index IP              LeaseTime  MacAddr\r\n");
		for (iTemp=0; iTemp < addr; iTemp++)
		{
				CONSOLE_PutMessage(inst, "%02d    %03d.%03d.%03d.%03d %08d   %02x:%02x:%02x:%02x:%02x:%02x\r\n"
					, (iTemp+1)
					, (u8)((dhcps_allocation_table[iTemp].IpAddr>>24)&0x000000FF)
					, (u8)((dhcps_allocation_table[iTemp].IpAddr>>16)&0x000000FF)
					, (u8)((dhcps_allocation_table[iTemp].IpAddr>>8)&0x000000FF)
					, (u8)(dhcps_allocation_table[iTemp].IpAddr&0x000000FF)
					, dhcps_allocation_table[iTemp].Lease
					, dhcps_allocation_table[iTemp].MacAddr[0]
					, dhcps_allocation_table[iTemp].MacAddr[1]
					, dhcps_allocation_table[iTemp].MacAddr[2]
					, dhcps_allocation_table[iTemp].MacAddr[3]
					, dhcps_allocation_table[iTemp].MacAddr[4]
					, dhcps_allocation_table[iTemp].MacAddr[5]
					 );
		}
	}
	return 1;
} /* End of clicmd_DhcpServerAllocTbl() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_DhcpServerAllocTblHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_DhcpServerAllocTblHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: dhcpstbl\r\n");
	return 1;
} /* End of clicmd_DhcpServerAllocTblHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SimpleCfgPinCode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SimpleCfgPinCode(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int iTemp;

	if (argc == 1) /* current setting */
    {
		clicmd_SimpleCfgPinCodeHelp(inst);
		CONSOLE_PutMessage(inst, "\r\nCurrent simple config pin code status: %s\r\n", GCONFIG_GetSimpleCfgPinCodeStatus() ? "enable":"disable");
        return 1;
    }
    else if (argc > 2)
        return -1;

    iTemp = atoi(argv[1]);
	GCONFIG_SetSimpleCfgPinCodeStatus(iTemp ? 1:0);
	return 1;
} /* End of clicmd_SimpleCfgPinCode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SimpleCfgPinCodeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SimpleCfgPinCodeHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: scpincode <status>\r\n");
    CONSOLE_PutMessage(inst, "       <status>: 0: disable     1: enable\r\n");
	return 1;
} /* End of clicmd_SimpleCfgPinCodeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_UrCounter()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_UrCounter(CONSOLE_CLICtl *inst, int argc, char **argv)
{
  	u8 i, maxConns;
	u32 rxCounter, txCounter;
	u16 network;

	CONSOLE_PutMessage(inst, "Uart discard bytes: %d\r\n", GUART_GetUrDiscardBytes(&urObj));
	CONSOLE_PutMessage(inst, "Uart Rx available bytes: %u, Tx empty bytes: %u, msr=0x%02x\r\n", GUART_GetRcvrAvailCount(), GUART_GetXmitEmptyCount(), serial_raed_msr(&urObj));
	network = GCONFIG_GetNetwork();
	maxConns = (network & GCONFIG_NETWORK_CLIENT) ? 1:GCONFIG_GetR2wMaxConnections();

	if (network & GCONFIG_NETWORK_PROTO_TCP)
	{
		for (i=0; i<maxConns; i++)
		{
			CONSOLE_PutMessage(inst, "TCP connection[%d]: status=%s, tx bytes=%u, rx bytes=%u\r\n",
				   i,
				   (GTCPDAT_GetConnStatus(i)==GTCPDAT_STATE_CONNECTED) ? "Connected":"Disconnected",
				   GTCPDAT_GetUrRxBytes(i),
				   GTCPDAT_GetUrTxBytes(i));
		}
	}
	else
	{
		for (i=0; i<maxConns; i++)
		{
			CONSOLE_PutMessage(inst, "UDP connection[%d]: status=%s, tx bytes=%u, rx bytes=%u\r\n",
				   i,
				   (GUDPDAT_GetConnStatus(i)==GUDPDAT_STATE_CONNECTED) ? "Connected":"Disconnected",
				   GUDPDAT_GetUrRxBytes(i),
				   GUDPDAT_GetUrTxBytes(i));
		}
	}
	TNCOM_GetRxTxCounter(&rxCounter, &txCounter);
	CONSOLE_PutMessage(inst, "tnCom rx bytes: %d\r\n", rxCounter);
	CONSOLE_PutMessage(inst, "tnCom tx bytes: %d\r\n", txCounter);
	return 1;
} /* End of clicmd_UrCounter() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_UrCounterHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_UrCounterHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: urCounter\r\n");
	return 1;
} /* End of clicmd_UrCounterHelp() */

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetGoogleNestInfo()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetGoogleNestInfo(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		clicmd_SetGoogleNestInfoHelp(inst);
		if (GCONFIG_GetGoogleNestHostName(StrBuf, GCONFIG_MAX_CLOUD_HOSTNAME_LEN))
		{
			CONSOLE_PutMessage(inst, "Current server host name is %s\r\n", StrBuf[0]==0 ? "unspecified":(char*)StrBuf);
			CONSOLE_PutMessage(inst, "Current connection status is %s\r\n", GNAPP_CheckConnectionStatus()==GNAPP_CONNECTED ? "connected":"disconnected");
		}
        return 1;
    }
    else if (argc > 2)
        return -1;

	if (argv[1][0]=='-' && strlen(argv[1])==1)
	{
		/* Clear host name */
		StrBuf[0] = '\0';
		GCONFIG_SetGoogleNestHostName(StrBuf);
	}
	else if (GCONFIG_SetGoogleNestHostName(argv[1]) == 0)
	{
		CONSOLE_PutMessage(inst, "Fail to set cloud server host name\r\n");
		return -1;
	}
	return 1;
} /* End of clicmd_SetGoogleNestInfo() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetGoogleNestInfoHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetGoogleNestInfoHelp(CONSOLE_CLICtl *inst)
{
    CONSOLE_PutMessage(inst, "Usage: cloud <hostname>\r\n");
	return 1;
} /* End of clicmd_SetGoogleNestInfoHelp() */
#endif

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixHostName()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixHostName(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];

	if (argc == 1) /* current setting */
    {
		clicmd_SetBluemixHostNameHelp(inst);
		if (GCONFIG_GetBluemixHostName(tmpHostName, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN))
		{
			CONSOLE_PutMessage(inst, "Current bluemix host name is %s\r\n", tmpHostName[0] == 0 ? "unspecified" : (char*)tmpHostName);
			CONSOLE_PutMessage(inst, "Current state: %s\r\n", BLUEMIX_CheckConnectionState());
		}
        return 1;
	}
	else if (argc > 2)
	{
        return -1;
	}

	if (argv[1][0] == '-' && strlen(argv[1]) == 1)
	{
		/* Clear host name */
		tmpHostName[0] = '\0';
		GCONFIG_SetBluemixHostName(tmpHostName);
	}
	else if (GCONFIG_SetBluemixHostName(argv[1]) == 0)
	{
		CONSOLE_PutMessage(inst, "Fail to set bluemix host name\r\n");
		return -1;
	}
	return 1;
} /* End of clicmd_SetBluemixHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixHostNameHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixHostNameHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: bmh <hostname>\r\n");
	return 1;
} /* End of clicmd_SetBluemixHostNameHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixAccount()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixAccount(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];
	u8	tmpOrgID[GCONFIG_MAX_BLUEMIX_ORG_ID_LEN];
	u8	tmpTypeID[GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN];
	u8	tmpDeviceID[GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN];
	u8	tmpUserName[GCONFIG_MAX_BLUEMIX_USERNAME_LEN];
	u8	tmpPassWord[GCONFIG_MAX_BLUEMIX_PASSWORD_LEN];

	if (argc == 1) /* current setting */
    {
		clicmd_SetBluemixAccountHelp(inst);
		if (GCONFIG_GetBluemixAccount(tmpOrgID, GCONFIG_MAX_BLUEMIX_ORG_ID_LEN,
									  tmpTypeID, GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN,
									  tmpDeviceID, GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN,
									  tmpUserName, GCONFIG_MAX_BLUEMIX_USERNAME_LEN,
									  tmpPassWord, GCONFIG_MAX_BLUEMIX_PASSWORD_LEN))
		{
			CONSOLE_PutMessage(inst, "Current bluemix organization ID is %s\r\n", tmpOrgID[0] == 0 ? "unspecified" : (char *)tmpOrgID);
			CONSOLE_PutMessage(inst, "Current bluemix type ID is %s\r\n", tmpTypeID[0] == 0 ? "unspecified" : (char *)tmpTypeID);
			CONSOLE_PutMessage(inst, "Current bluemix device ID is %s\r\n", tmpDeviceID[0] == 0 ? "unspecified" : (char *)tmpDeviceID);
			CONSOLE_PutMessage(inst, "Current bluemix username is %s\r\n", tmpUserName[0] == 0 ? "unspecified" : (char*)tmpUserName);
			CONSOLE_PutMessage(inst, "Current bluemix password is %s\r\n", tmpPassWord[0] == 0 ? "unspecified" : (char*)tmpPassWord);
		}
		return 1;
	}
	else if (argc == 6)
	{
		/* Set them all into GCONFIG */
		if (GCONFIG_SetBluemixAccount(argv[1], argv[2], argv[3], argv[4], argv[5]) == 0)
		{
			CONSOLE_PutMessage(inst, "\n\rError length\n\r");
			return -1;
		}

		/* If host name field is null, help to set it */
		if (GCONFIG_GetBluemixHostName(tmpHostName, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN))
		{
			if (tmpHostName[0] == 0)
			{
				sprintf(tmpHostName, "%s.messaging.internetofthings.ibmcloud.com", argv[1]);
				GCONFIG_SetBluemixHostName(tmpHostName);
			}
		}
		return 1;
	}
	return -1;
} /* End of clicmd_SetBluemixAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixAccountHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixAccountHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: bma <org_id> <type_id> <device_id> <username> <password>\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <org_id> is 6 bytes\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <type_id> is 5 bytes, where is \"23001\"\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <device_id> is 11 bytes, where is serial number\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <username> <= 32 bytes\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <password> <= 32 bytes\r\n");
	CONSOLE_PutMessage(inst, "Client ID format = d:<org_id>:<type_id>:<device_id>\r\n");
	return 1;
} /* End of clicmd_SetBluemixAccountHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixAutoReg()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixAutoReg(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	tmpApiVersion[GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN];
	u8	tmpApiKey[GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN];
	u8	tmpApiToken[GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN];

	if (argc == 1) /* current setting */
    {
		clicmd_SetBluemixAutoRegHelp(inst);
		if (GCONFIG_GetBluemixAutoReg(tmpApiVersion, GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN,
									  tmpApiKey, GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN,
									  tmpApiToken, GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN))
		{
			CONSOLE_PutMessage(inst, "\r\nCurrent bluemix API version is %s\r\n", tmpApiVersion[0] == 0 ? "unspecified" : (char *)tmpApiVersion);
			CONSOLE_PutMessage(inst, "\r\nCurrent bluemix API key is %s\r\n", tmpApiKey[0] == 0 ? "unspecified" : (char *)tmpApiKey);
			CONSOLE_PutMessage(inst, "\r\nCurrent bluemix API token is %s\r\n", tmpApiToken[0] == 0 ? "unspecified" : (char *)tmpApiToken);
		}
		return 1;
	}
	else if (argc == 4)
	{
		/* Set them all into GCONFIG */
		if (GCONFIG_SetBluemixAutoReg(argv[1], argv[2], argv[3]) == 0)
		{
			CONSOLE_PutMessage(inst, "\n\rError length\n\r");
			return -1;
		}
		return 1;
	}
	return -1;
} /* End of clicmd_SetBluemixAutoReg() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetBluemixAutoRegHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetBluemixAutoRegHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: bmr <api_ver> <api_key> <api_token>\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <api_ver> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN-1);
	CONSOLE_PutMessage(inst, "Lenght of <api_key> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN-1);
	CONSOLE_PutMessage(inst, "Lenght of <api_token> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN-1);
	return 1;
} /* End of clicmd_SetBluemixAutoRegHelp() */
#endif

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAzureHostName()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAzureHostName(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];

	if (argc == 1) /* current setting */
    {
		clicmd_SetAzureHostNameHelp(inst);
		if (GCONFIG_GetAzureHostName(tmpHostName, GCONFIG_MAX_AZURE_HOSTNAME_LEN))
		{
			CONSOLE_PutMessage(inst, "\r\nCurrent azure host name is %s\r\n", tmpHostName[0] == 0 ? "unspecified" : (char*)tmpHostName);
			CONSOLE_PutMessage(inst, "\r\nCurrent state: %s\r\n", AZURE_CheckConnectionState());
		}
        return 1;
	}
	else if (argc > 2)
	{
        return -1;
	}

	if (argv[1][0] == '-' && strlen(argv[1]) == 1)
	{
		/* Clear host name */
		tmpHostName[0] = '\0';
		GCONFIG_SetAzureHostName(tmpHostName);
	}
	else if (GCONFIG_SetAzureHostName(argv[1]) == 0)
	{
		CONSOLE_PutMessage(inst, "Fail to set azure host name\r\n");
		return -1;
	}
	return 1;
} /* End of clicmd_SetAzureHostName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAzureHostNameHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAzureHostNameHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: azh <hostname>\r\n");
	return 1;
} /* End of clicmd_SetAzureHostNameHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAzureAccount()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAzureAccount(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];
	u8	tmpHubName[GCONFIG_MAX_AZURE_HUBNAME_LEN];
	u8	tmpDeviceID[GCONFIG_MAX_AZURE_DEVICE_ID_LEN];
	u8	tmpSignedKey[GCONFIG_MAX_AZURE_SIGNED_KEY_LEN];
	u8	tmpExpiryTime[GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN];

	if (argc == 1) /* current setting */
    {
		clicmd_SetAzureAccountHelp(inst);
		if (GCONFIG_GetAzureAccount(tmpHubName, GCONFIG_MAX_AZURE_HUBNAME_LEN,
									tmpDeviceID, GCONFIG_MAX_AZURE_DEVICE_ID_LEN,
									tmpSignedKey, GCONFIG_MAX_AZURE_SIGNED_KEY_LEN,
									tmpExpiryTime, GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN))
		{
			CONSOLE_PutMessage(inst, "\r\nCurrent azure IoT HUB name is %s\r\n", tmpHubName[0] == 0 ? "unspecified" : (char *)tmpHubName);
			CONSOLE_PutMessage(inst, "\r\nCurrent azure device ID is %s\r\n", tmpDeviceID[0] == 0 ? "unspecified" : (char *)tmpDeviceID);
			CONSOLE_PutMessage(inst, "\r\nCurrent azure device signed key is %s\r\n", tmpSignedKey[0] == 0 ? "unspecified" : (char*)tmpSignedKey);
			CONSOLE_PutMessage(inst, "\r\nCurrent azure device expiry time is %s\r\n", tmpExpiryTime[0] == 0 ? "unspecified" : (char*)tmpExpiryTime);
		}
		return 1;
	}
	else if (argc == 5)
	{
		/* Check length of IoT HUB name */
		if (strlen(argv[1]) >= GCONFIG_MAX_AZURE_HUBNAME_LEN)
		{
			CONSOLE_PutMessage(inst, "\n\rError <hub_name> length\n\r");
			return -1;
		}

		/* Check length of device id */
		if (strlen(argv[2]) != (GCONFIG_MAX_AZURE_DEVICE_ID_LEN - 1))
		{
			CONSOLE_PutMessage(inst, "\n\rError <device_id> length\n\r");
			return -1;
		}

		/* Check length of signed key */
		if (strlen(argv[3]) >= GCONFIG_MAX_AZURE_SIGNED_KEY_LEN)
		{
			CONSOLE_PutMessage(inst, "\n\rError <signed_key> length\n\r");
			return -1;
		}

		/* Check length of expiry time */
		if (strlen(argv[4]) != (GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN - 1))
		{
			CONSOLE_PutMessage(inst, "\n\rError <expiry_time> length\n\r");
			return -1;
		}

		/* Set them all into GCONFIG */
		if (GCONFIG_SetAzureAccount(argv[1], argv[2], argv[3], argv[4]) == 0)
		{
			CONSOLE_PutMessage(inst, "\n\rError length\n\r");
			return -1;
		}

		/* If host name field is null, help to set it */
		if (GCONFIG_GetAzureHostName(tmpHostName, GCONFIG_MAX_AZURE_HOSTNAME_LEN))
		{
			if (tmpHostName[0] == 0)
			{
				sprintf(tmpHostName, "%s.azure-devices.net", argv[1]);
				GCONFIG_SetAzureHostName(tmpHostName);
			}
		}
		return 1;
	}
	return -1;
} /* End of clicmd_SetAzureAccount() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetAzureAccountHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetAzureAccountHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: aza <hub_name> <device_id> <signed_key> <expiry_time>\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <hub_name> <= %d bytes\r\n", GCONFIG_MAX_AZURE_HUBNAME_LEN - 1);
	CONSOLE_PutMessage(inst, "Lenght of <device_id> is 11 bytes, where is serial number\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <signed_key> <= 64 bytes\r\n");
	CONSOLE_PutMessage(inst, "Lenght of <expiry_time> = 10 bytes, where is timestamp in UNIX form\r\n");
	return 1;
} /* End of clicmd_SetAzureAccountHelp() */
#endif

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetRs485Status()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetRs485Status(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
		clicmd_SetRs485StatusHelp(inst);
		CONSOLE_PutMessage(inst, "Current RS485 status: %s\r\n", GCONFIG_GetRs485Status() ? "enable":"disable");
	}
	else if (argc == 2)
	{
		val = atoi(argv[1]);
		if (val < 2 && val >= 0)
		{
			GCONFIG_SetRs485Status((u8)val);
		}
		else
		{
			CONSOLE_PutMessage(inst, "Invalid value input\r\n");
			return -1;
		}
		return 1;
	}
	return -1;
} /* End of clicmd_SetRs485Status() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetRs485StatusHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetRs485StatusHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: rs485 <status>\r\n");
   	CONSOLE_PutMessage(inst, "       <status>: 0: enable     1: disable\r\n");
	return 1;
} /* End of clicmd_SetRs485StatusHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetModbusTcp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetModbusTcp(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u16 ServerPort;
	u8 SlaveId, XferMode, *pMsg;

	GCONFIG_GetModbusTcp(&ServerPort, &SlaveId, &XferMode);
	if (argc == 1) /* current setting */
    {
		switch (XferMode)
		{
		case GCONFIG_MBGW_TCP:
			pMsg = "MODBUS TCP";
			break;
		case GCONFIG_MBGW_TRANSPARENT:
			pMsg = "Transparent TCP";
			break;
		}
		CONSOLE_PutMessage(inst, "\r\nCurrent TransferMode=%s, server port=%d\r\n", pMsg, ServerPort);
        return 1;
    }
    else if (argc != 3)
        return -1;

	if (argc > 1)
    	XferMode = atoi(argv[1]);
	if (argc > 2)
    	ServerPort = atoi(argv[2]);
//	if (argc > 3)
 //   	SlaveId = atoi(argv[3]);
	if (GCONFIG_SetModbusTcp(ServerPort, SlaveId, XferMode) == 0)
	{
		CONSOLE_PutMessage(inst, "Fail to set modbus transfer mode\r\n");
		return -1;
	}
	return 1;
} /* End of clicmd_SetModbusTcp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetModbusTcpHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetModbusTcpHelp(CONSOLE_CLICtl *inst)
{
   	CONSOLE_PutMessage(inst, "Usage: mbtcp <xferMode> <port>\r\n");
   	CONSOLE_PutMessage(inst, "       <xferMode>: 0: MODBUS TCP   1: Transparent TCP\r\n");
   	CONSOLE_PutMessage(inst, "       <port>: TCP server port number, default is 502\r\n");
	return 1;
} /* End of clicmd_SetModbusTcpHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetModbusSerialTiming()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetModbusSerialTiming(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	u16 ResponseTimeOut, InterFrameDelay, InterCharDelay;

	GCONFIG_GetModbusSerialTiming(&ResponseTimeOut, &InterFrameDelay, &InterCharDelay);
	if (argc == 1) /* current setting */
    {
		CONSOLE_PutMessage(inst, "\r\nCurrent ResponseTimeOut=%d, InterFrameDelay=%d, InterCharDelay=%d\r\n", ResponseTimeOut, InterFrameDelay, InterCharDelay);
        return 1;
    }
    else if (argc > 4)
        return -1;

	if (argc > 1)
    	ResponseTimeOut = atoi(argv[1]);
	if (argc > 2)
    	InterFrameDelay = atoi(argv[2]);
	if (argc > 3)
    	InterCharDelay = atoi(argv[3]);
	if (GCONFIG_SetModbusSerialTiming(ResponseTimeOut, InterFrameDelay, InterCharDelay) == 0)
	{
		CONSOLE_PutMessage(inst, "Fail to set modbus transfer mode\r\n");
		return -1;
	}

	return 1;
} /* End of clicmd_SetModbusSerialTiming() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetModbusSerialTimingHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetModbusSerialTimingHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: mbst <ResponseTimeOut> <InterFrameDelay> <InterCharDelay>\r\n");
    CONSOLE_PutMessage(inst, "       <ResponseTimeOut>: Response timeout(10~65000ms)\r\n");
    CONSOLE_PutMessage(inst, "       <InterFrameDelay>: Interval time of frame sending(10~500ms)\r\n");
    CONSOLE_PutMessage(inst, "       <InterCharDelay>:  Inter-Character timeout for frame receiving(10~500ms)\r\n");
	return 1;
} /* End of clicmd_SetModbusSerialTimingHelp() */

#if CONFIG_INCLUDE_SIMPLE_CONFIG
/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SimpleConfig()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SimpleConfig(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	char *custom_pin_code = NULL;
	enum sc_result ret = SC_ERROR;
	u8 apToStationEver = 0;

	if (argc != 1 && argc != 2) /* current setting */
    {
		clicmd_SimpleConfigHelp(inst);
        return 1;
    }

	/* Check AP mode */
	if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
	{
		struct {
				int count;
				rtw_mac_t mac_list[AP_STA_NUM];
				} client_info;

		client_info.count = AP_STA_NUM;
		wifi_get_associated_client_list(&client_info, sizeof(client_info));
		if (client_info.count)
		{
			CONSOLE_PutMessage(inst, "\n\rAP mode with associated client, abort Simple_config.\n\r");
			return 1;
		}
		else
		{
			apToStationEver = 1;
		}
	}

	/* Accept pin code */
	if (argc == 2)
	{
		custom_pin_code = (argv[0]);
	}

	/* Start Simple config process */
	CONSOLE_PutMessage(inst, "\n\rStart simple configuration.\n\r");
	MISC_EnterWiFiBusySection();
	wifi_enter_promisc_mode();
	if (init_test_data(custom_pin_code) == 0)
	{
		filter_add_enable();
		ret = simple_config_test();
		print_simple_config_result(ret);
		remove_filter();
	}

	if (ret == SC_SUCCESS)
	{
		CONSOLE_PutMessage(inst, "\n\rSimple configuration success, device is rebooting!\n\r");
		/* Auto enable DHCP client and disable DHCP server while switching to station mode */
		if (apToStationEver)
		{
			GCONFIG_SetNetwork((GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE));
			GCONFIG_SetDhcpSrvStatus(0);
		}
		GPIOS_SaveCurrentWifiConfig();
	}
	else
	{
		CONSOLE_PutMessage(inst, "\n\rSimple configuration fail!\n\r");
		GPIOS_CheckAndRestoreAPMode(&apToStationEver);
	}
	MISC_LeaveWiFiBusySection();

	return ((ret == SC_SUCCESS) ? 1:-1);
} /* End of clicmd_SimpleConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SimpleConfigHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SimpleConfigHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: wifi_simple_config <pinCode>\r\n");
	return 1;
} /* End of clicmd_SimpleConfigHelp() */
#endif

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetClientConnectionMode()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetClientConnectionMode(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int	val;

	if (argc == 1)
	{
		clicmd_SetClientConnectionModeHelp(inst);
		CONSOLE_PutMessage(inst, "Current setting is %s connection mode\r\n", GCONFIG_GetClientConnectionMode()?"Manual":"Auto");
		return 1;
	}
	else if (argc == 2)
	{
		val = atoi((const char *)argv[1]);
		GCONFIG_SetClientConnectionMode(val?GCONFIG_MANUAL_CONNECTION:GCONFIG_AUTO_CONNECTION);
		CONSOLE_PutMessage(inst, "Set %s connection mode OK\r\n", GCONFIG_GetClientConnectionMode()?"Manual":"Auto");
		return 1;
	}
	return -1;
} /* End of clicmd_SetClientConnectionMode() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetClientConnectionModeHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetClientConnectionModeHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: clientconnmode <mode>\r\n");
   	CONSOLE_PutMessage(inst, "       <mode>: 0 = Auto connection mode, 1 = Manual connection mode\r\n");
	return 1;
} /* End of clicmd_SetClientConnectionModeHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ClientConnect()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ClientConnect(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	struct ip_addr	iHostAddr;
	u32				DestIp;
	u16				DestPort;
	int				ret;

	if (argc == 1)
	{
		clicmd_ClientConnectHelp(inst);
		return 1;
	}
	else if (argc == 3)
	{
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
		{
			ret = GTCPDAT_IsManualConnectionMode();
		}
		else
		{
			ret = GUDPDAT_IsManualConnectionMode();
		}

		if (ret == 0)
		{
			CONSOLE_PutMessage(inst, "Fail to use command! please enable both client mode and manual connection mode first.\r\n");
			return -1;
		}

		/* Get Destination IP address */
		DestIp = htonl(GCONFIG_IpAddr2Ulong(argv[1], strlen(argv[1])));
		if (DestIp == 0xFFFFFFFF) // Not a valid IP address
		{
			/* Execute a DNS query or get the host IP */
			if (netconn_gethostbyname(argv[1], &iHostAddr) != ERR_OK)
			{
				CONSOLE_PutMessage(inst, "Unresolved host name.\r\n");
				return -1;
			}
			DestIp = htonl(iHostAddr.addr);
		}

		/* Get Destination port number */
		DestPort = (u16)atoi(argv[2]);

		/* Establish connection */
		CONSOLE_PutMessage(inst, "Connect to %s:%d...", argv[1], DestPort);
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
		{
			ret = GTCPDAT_Connect(0, DestIp, DestPort);
		}
		else
		{
 			ret = GUDPDAT_Connect(0, DestIp, DestPort);
		}

		CONSOLE_PutMessage(inst, "%s.\r\n", (ret < 0)?"Fail":"Success");
		return 1;
	}
	return -1;
} /* End of clicmd_ClientConnect() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ClientConnectHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ClientConnectHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: clientconn <ip/hostname> <port>, establish a new connection with remote IP/Hostname:Port\r\n");
	return 1;
} /* End of clicmd_ClientConnectHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ClientClose()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ClientClose(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int ret;

	if (argc == 1)
	{
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
		{
			ret = GTCPDAT_IsManualConnectionMode();
		}
		else
		{
			ret = GUDPDAT_IsManualConnectionMode();
		}

		if (ret == 0)
		{
			CONSOLE_PutMessage(inst, "Fail to use command! please enable both client mode and manual connection mode first.\r\n");
			return -1;
		}

		CONSOLE_PutMessage(inst, "Close connection ");
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
		{
			ret = GTCPDAT_Close(0);
		}
		else
		{
			ret = GUDPDAT_Close(0);
		}

		CONSOLE_PutMessage(inst, "%s.\r\n", (ret < 0) ? "Fail" : "Success");
		return 1;
	}
	return -1;
} /* End of clicmd_ClientClose() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_ClientCloseHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_ClientCloseHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: clientclose\r\n");
	return 1;
} /* End of clicmd_ClientCloseHelp() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDeviceName()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDeviceName(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	if (argc == 1)
	{
		clicmd_SetDeviceNameHelp(inst);
		CONSOLE_PutMessage(inst, "Current device name: %s\r\n", GCONFIG_GetDeviceName());
		return 1;
	}
	GCONFIG_SetDeviceName(argv[1]);
	return 1;
} /* End of clicmd_SetDeviceName() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetDeviceNameHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetDeviceNameHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setdevname <name>\r\n");
	return 1;
} /* End of clicmd_SetDeviceNameHelp() */

#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetNetworkBackup()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetNetworkBackup(CONSOLE_CLICtl *inst, int argc, char **argv)
{
	int	val;

	if (argc == 1)
	{
		clicmd_SetNetworkBackupHelp(inst);
		CONSOLE_PutMessage(inst, "Current network backup mode: %s\r\n",
						   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_FORCE_WIFI) ? "Force Wifi" :
						   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_FORCE_ETH) ? "Force Ethernet" :
						   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_WIFI_AS_MAJOR) ? "Wifi as major" :
						   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR) ? "Ethernet as major" : "Unknow");
		return 1;
	}

	val = atoi(argv[1]);
	if ((val < GCONFIG_NETWORK_BACKUP_FORCE_WIFI) || (val > GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR))
	{
		clicmd_SetNetworkBackupHelp(inst);
		return 1;
	}

	GCONFIG_SetNetworkBackupMode(val);
	return 1;
} /* End of clicmd_SetNetworkBackup() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: clicmd_SetNetworkBackupHelp()
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static int clicmd_SetNetworkBackupHelp(CONSOLE_CLICtl *inst)
{
	CONSOLE_PutMessage(inst, "Usage: setnetbackup <mode>\r\n");
	CONSOLE_PutMessage(inst, "<mode>: 0 = Force Wifi, 1 = Force Ethernet, 2 = Wifi as major, 3 = Ethernet as major\r\n");
	return 1;
} /* End of clicmd_SetNetworkBackupHelp() */
#endif
/* End of clicmd.c */
