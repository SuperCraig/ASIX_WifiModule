
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"
#include "rtl8195a.h"
#include <lwip_netconf.h>
#include "tcpip.h"
#include <osdep_service.h>
#include <wlan/wlan_test_inc.h>
//#include <dhcp/dhcps.h>
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <platform/platform_stdlib.h>
#include "gconfig.h"
#include "hal_api.h"
#include "email.h"
#include "gpios.h"
#include "rtc.h"
#include "ths.h"
#include "dhcpsrv.h"
#include "misc.h"
#include "gtcpdat.h"
#include "gudpdat.h"
#include "guart.h"
#include "tncom.h"
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
#include "gnapp.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
#include "bluemix.h"
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
#include "azure.h"
#endif
#include "gs2w.h"

#ifndef CONFIG_INTERACTIVE_EXT
#define CONFIG_INTERACTIVE_EXT  0
#endif
#ifndef CONFIG_SSL_CLIENT
#if defined(CONFIG_PLATFORM_8711B)
#define CONFIG_SSL_CLIENT       0
#else
#define CONFIG_SSL_CLIENT       1
#endif
#endif

#define SCAN_WITH_SSID			0

#ifdef CONFIG_WPS
#define STACKSIZE               1280
#else
#define STACKSIZE               1024
#endif

/*Wifi*/
/* Moved to gconfig.h */
//#if CONFIG_WLAN
//#define	STA_SCANNED_BSS_MAX	24
//#endif

static u8 StrBuf[128];
static DHCPS_POOL dhcps_allocation_table[DHCPS_MAX_IP_POOL_NUM];
static void cmd_help(int argc, char **argv);
#if CONFIG_SSL_CLIENT
extern void cmd_ssl_client(int argc, char **argv);
#endif
#if CONFIG_WLAN
//static void cmd_wifi_on(int argc, char **argv);
void cmd_wifi_on(int argc, char **argv);
//static void cmd_wifi_off(int argc, char **argv);
void cmd_wifi_off(int argc, char **argv);
static void cmd_wifi_disconnect(int argc, char **argv);
extern void cmd_promisc(int argc, char **argv);
extern void cmd_simple_config(int argc, char **argv);

extern const s32	security_map[];
extern u8			wext_current_ssid_len;
extern u8			wext_current_ssid[32];


s8 getIp2Ulong(u8 *pBuf, u8 len, u32 *ipAddr);


wifi_scaned_bss_info_t	wifi_scaned_bss_table[STA_SCANNED_BSS_MAX];
static void wifi_scanned_bss_info_maintain(u8 opt, u8 num, rtw_scan_result_t *record);

#ifdef CONFIG_WPS
#if CONFIG_ENABLE_WPS
extern void cmd_wps(int argc, char **argv);
#endif
#ifdef CONFIG_WPS_AP
extern void cmd_ap_wps(int argc, char **argv);
extern int wpas_wps_dev_config(u8 *dev_addr, u8 bregistrar);
#endif //CONFIG_WPS_AP
#endif //CONFIG_WPS
#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif
#define IsDigit(x) ((x < 0x3a && x > 0x2f) ? 1 : 0)
static void cmd_urConfig(int argc, char **argv);
static void cmd_saveConfig(int argc, char **argv);
static void cmd_systemReboot(int argc, char **argv);
static void cmd_ipConfig(int argc, char **argv);
static void cmd_dhcpClient(int argc, char **argv);
static void cmd_setIpAddr(int argc, char **argv);
static void cmd_setNetmask(int argc, char **argv);
static void cmd_setGateway(int argc, char **argv);
static void cmd_setMode(int argc, char **argv);
static void cmd_setConnectType(int argc, char **argv);
static void cmd_enableDhcpServer(int argc, char **argv);
static void cmd_setNtpServer(int argc, char **argv);
static void cmd_rtcTimeSetup(int argc, char **argv);
static void cmd_currentTime(int argc, char **argv);
static void cmd_currentDate(int argc, char **argv);
static void cmd_getTemperatureHumidity(int argc, char **argv);
static void cmd_setDefaultSettings(int argc, char **argv);
static void cmd_setDnsIp(int argc, char **argv);
static void cmd_setServerPort(int argc, char **argv);
static void cmd_setDestPort(int argc, char **argv);
static void cmd_setDestHostName(int argc, char **argv);
static void cmd_SetEmailServerAddr(int argc, char **argv);
static void cmd_SetEmailFromAddr(int argc, char **argv);
static void cmd_SetEmailTo1Addr(int argc, char **argv);
static void cmd_SetEmailTo2Addr(int argc, char **argv);
static void cmd_SetEmailTo3Addr(int argc, char **argv);
static void cmd_SetSMTPSecurity(int argc, char **argv);
static void cmd_SetSMTPAccount(int argc, char **argv);
static void cmd_GetEmailConfig(int argc, char **argv);
static void cmd_SetAWConfig(int argc, char **argv);
static void cmd_R2wMode(int argc, char **argv);
static void cmd_JtagOff(int argc, char **argv);
static void cmd_GetOTAName(int argc, char **argv);
static void cmd_SetCountryId(int argc, char **argv);
static void cmd_setDhcpServer(int argc, char **argv);
static void cmd_SetEtherTimer(int argc, char **argv);
static void cmd_ConnectStatus(int argc, char **argv);
static void cmd_dhcpServerAllocTbl(int argc, char **argv);
static void cmd_simpleCfgPinCode(int argc, char **argv);
static void cmd_urCounter(int argc, char **argv);
static void cmd_setRs485Status(int argc, char **argv);
#if CONFIG_SMART_PLUG
static void cmd_SmartPlug(int argc, char **argv);
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
static void cmd_setGoogleNestInfo(int argc, char **argv);
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
static void cmd_setBluemixHostName(int argc, char **argv);
static void cmd_setBluemixAccount(int argc, char **argv);
static void cmd_setBluemixAutoReg(int argc, char **argv);
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
static void cmd_setAzureHostName(int argc, char **argv);
static void cmd_setAzureAccount(int argc, char **argv);
#endif
static void cmd_setModbusTcp(int argc, char **argv);
static void cmd_setModbusSerialTiming(int argc, char **argv);
static void cmd_setClientConnectionMode(int argc, char **argv);
static void cmd_ClientConnect(int argc, char **argv);
static void cmd_ClientClose(int argc, char **argv);
static void cmd_setDeviceName(int argc, char **argv);
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
static void cmd_setNetworkBackup(int argc, char **argv);
#endif

static void cmd_setAdaptivityThreshold(int argc, char **argv);

//extern void cmd_tcp(int argc, char **argv);

#ifdef  CONFIG_CONCURRENT_MODE
static void cmd_wifi_sta_and_ap(int argc, char **argv)
{
	int timeout = 20;//, mode;
#if CONFIG_LWIP_LAYER
	struct netif * pnetiff = (struct netif *)&xnetif[1];
#endif
	int channel;

	if((argc != 3) && (argc != 4)) {
		printf("\n\rUsage: wifi_ap SSID CHANNEL [PASSWORD]");
		return;
	}

	if(atoi((const char *)argv[2]) > 14){
		printf("\n\r bad channel!Usage: wifi_ap SSID CHANNEL [PASSWORD]");
		return;
	}
#if CONFIG_LWIP_LAYER
	DHCPS_DeInit();
#endif

#if 0
	//Check mode
	wext_get_mode(WLAN0_NAME, &mode);

	switch(mode) {
		case IW_MODE_MASTER:	//In AP mode
			cmd_wifi_off(0, NULL);
			cmd_wifi_on(0, NULL);
			break;
		case IW_MODE_INFRA:		//In STA mode
			if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
				cmd_wifi_disconnect(0, NULL);
	}
#endif
	wifi_off();
	vTaskDelay(20);
	if (wifi_on(RTW_MODE_STA_AP) < 0){
		printf("\n\rERROR: Wifi on failed!");
		return;
	}

	printf("\n\rStarting AP ...");
	channel = atoi((const char *)argv[2]);
	if (channel > 13)
	{
		printf("\n\rChannel is from 1 to 13. Set channel 1 as default!\n");
		channel = 1;
	}

	if (argc == 4)
	{
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_WPA2_AES_PSK,
							 argv[3],
							 strlen((const char *)argv[1]),
							 strlen((const char *)argv[3]),
							 channel
							 ) != RTW_SUCCESS)
		{
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}
	else
	{
		if(wifi_start_ap(argv[1],
							 RTW_SECURITY_OPEN,
							 NULL,
							 strlen((const char *)argv[1]),
							 0,
							 channel
							 ) != RTW_SUCCESS)
		{
			printf("\n\rERROR: Operation failed!");
			return;
		}
	}

	while(1)
	{
		char essid[33];

		if (wext_get_ssid(WLAN1_NAME, (unsigned char *) essid) > 0)
		{
			if (strcmp((const char *) essid, (const char *)argv[1]) == 0)
			{
				printf("\n\r%s started", argv[1]);
				break;
			}
		}

		if (timeout == 0)
		{
			printf("\n\rERROR: Start AP timeout!");
			break;
		}

		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}
#if CONFIG_LWIP_LAYER
	LwIP_UseStaticIP(&xnetif[1]);
#ifdef CONFIG_DONT_CARE_TP
	pnetiff->flags |= NETIF_FLAG_IPSWITCH;
#endif
	if (GCONFIG_GetDhcpSrvStatus())
		DHCPS_ReInit();
#endif
}
#endif

static void cmd_wifi_ap(int argc, char **argv)
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
		printf("\n\rReject, AP mode under force ethernet");
		return;
	}
	else if (networkBackupMode == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR)
	{
		if ((MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) &&
			(MISC_NetInterfaceIsLinkUp(IF_ID_ETH)))
		{
			printf("\n\rReject, AP mode under ethernet as major and ethernet linked");
			return;
		}
	}
#endif

	if ((argc != 3) && (argc != 4) && (argc != 5))
	{
		printf("\n\rUsage: wifi_ap [SSID] [CHANNEL] [WPA PASSWORD] [HIDE]");
		printf("\n\rSSID = 1 ~ 32 ASCII characters");
		printf("\n\rCHANNEL = 1 ~ 14 (refer to country ID)");
		printf("\n\rWPA PASSWORD = 8 ~ 63 ASCII characters");
		printf("\n\rHIDE = 0 (not hide) or 1 (hide)");

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

		/* Assign argv[3] address and password */
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
				printf("\n\rERROR: unsupported security type");
				return;
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
				printf("\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
				return;
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
				printf("\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
				return;
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
		printf("\r\nInvalid country id, set 0 as default");
		GCONFIG_SetWifiCountryId(RTW_COUNTRY_WORLD1);
		GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);
	}

	if ((channel < minChNum) || (channel > maxChNum))
	{
		printf("\r\nPlease select a channel between %d ~ %d to match current channel plan.", minChNum, maxChNum);
		return;
	}

	/* Set hidden SSID into GCONFIG */
	if (hiddenSsid != -1)
	{
		if (GCONFIG_SetWifiHiddenSsid((u8) hiddenSsid) < 0)
		{
			printf("\n\rWrong HIDE value. Must between 0 ~ 1.");
			return;
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
#if 0
	//Check mode
	wext_get_mode(WLAN0_NAME, &mode);

	switch (mode)
	{
		case IW_MODE_MASTER:	//In AP mode
			wifi_off();
			wifi_on(1);
			break;
		case IW_MODE_INFRA:		//In STA mode
			if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
				cmd_wifi_disconnect(0, NULL);
	}
#else
	wifi_off();
	vTaskDelay(20);
	if (wifi_on(RTW_MODE_AP) < 0)
	{
		printf("\n\rERROR: Wifi on failed!");
		return;
	}
#endif

	printf("\n\rStarting AP ...");

#ifdef CONFIG_WPS_AP
	wpas_wps_dev_config(pnetif->hwaddr, 1);
#endif

		if (wifi_start_ap(argv[1],
						  security_type,
						  (security_type == RTW_SECURITY_WPA2_AES_PSK) ? argv[3] : NULL,
						  strlen((const char *)argv[1]),
						  (security_type == RTW_SECURITY_WPA2_AES_PSK) ? strlen((const char *)argv[3]) : 0,
						  channel) != RTW_SUCCESS)
		{
			printf("\n\rERROR: Operation failed!");
			return;
		}

	/* Save wifi configuration */
	while (1)
	{
		char essid[33];

		if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0)
		{
			if (strcmp((const char *) essid, (const char *)argv[1]) == 0)
			{
				printf("\n\r%s started\n", argv[1]);

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
					cmd_systemReboot(0, 0);
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
			printf("\n\rERROR: Start AP timeout!");
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
}

static void cmd_wifi_connect(int argc, char **argv)
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


        cmd_wifi_off(2,argv[1]);
        cmd_wifi_on(2,argv[1]);

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}
        
	MISC_EnterWiFiBusySection();

	if((argc != 2) && (argc != 3) && (argc != 4))
	{
		printf("\n\rUsage: wifi_connect [SSID] [WPA PASSWORD / WEP KEY(5 or 13)] [WEP KEY ID]");
		printf("\n\rSSID = 1 ~ 32 ASCII characters");
		printf("\n\rWPA PASSWORD = 8 ~ 63 ASCII characters");
		printf("\n\rASCII WEP KEY = 5 (WEP64) or 13 (WEP128) ASCII characters");
		printf("\n\rWEP KEY ID = 0 ~ 3");
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
				printf("\n\rERROR: unsupported security type");
				goto cmd_wifi_connect_exit;
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
			printf("\n\rERROR: Wifi on failed!");
			goto cmd_wifi_connect_exit;
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
			printf("\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
			goto cmd_wifi_connect_exit;
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
			printf("\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			goto cmd_wifi_connect_exit;
		}
		if((key_id < 0) || (key_id > 3))
		{
			printf("\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			goto cmd_wifi_connect_exit;
		}
		semaphore = NULL;
	}

	ret = wifi_connect(ssid,
					security_type,
					password,
					ssid_len,
					password_len,
					key_id,
					semaphore);

	tick2 = xTaskGetTickCount();
	printf("\r\nConnected after %dms.\n", (tick2-tick1));
	if(ret != RTW_SUCCESS)
	{
		printf("\n\rERROR: Operation failed!");
		goto cmd_wifi_connect_exit;
	}
	else
	{
		/* Save wifi parameters to gconfig */
		GCONFIG_SetWifiRfEnable(1);					/* WIFI RF Enable */
		/* Auto configure DHCP client and server */
		if (GCONFIG_GetWifiNetworkMode()==RTW_MODE_AP)
			wifi_mode_change = 1;

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
			cmd_systemReboot(0, 0);
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
		printf("\r\n\nGot IP after %dms.\n", (xTaskGetTickCount() - tick1));
	}
cmd_wifi_connect_exit:
	MISC_LeaveWiFiBusySection();
}

static void cmd_wifi_connect_bssid(int argc, char **argv)
{
	int				ret = RTW_ERROR;
	unsigned long	tick1 = xTaskGetTickCount();
	unsigned long	tick2;
	int				mode;
	unsigned char	bssid[ETH_ALEN];
	char 			*ssid = NULL;
	rtw_security_t	security_type;
	char 			*password;
	int 			bssid_len;
	int 			ssid_len = 0;
	int 			password_len;
	int 			key_id;
	void			*semaphore;
	u32 			mac[ETH_ALEN];
	u32				i;
	u32				index = 0;

	if ((argc != 3) && (argc != 4) && (argc != 5) && (argc != 6))
	{
		printf("\n\rUsage: wifi_connect_bssid 0/1 [SSID] BSSID / xx:xx:xx:xx:xx:xx [WPA PASSWORD / (5 or 13) ASCII WEP KEY] [WEP KEY ID 0/1/2/3]");
		return;
	}
	MISC_EnterWiFiBusySection();
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if (mode == IW_MODE_MASTER)
	{
#if CONFIG_LWIP_LAYER
		DHCPS_DeInit();
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0)
		{
			printf("\n\rERROR: Wifi on failed!");
			goto cmd_wifi_connect_bssid_exit;
		}
	}
	//check ssid
	if (memcmp(argv[1], "0", 1))
	{
		index = 1;
		ssid_len = strlen((const char *)argv[2]);
		if ((ssid_len <= 0) || (ssid_len > 32))
		{
			printf("\n\rWrong ssid. Length must be less than 32.");
			goto cmd_wifi_connect_bssid_exit;
		}
		ssid = argv[2];
	}
	sscanf(argv[2 + index], MAC_FMT, mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
	for (i=0; i<ETH_ALEN; i++)
	{
		bssid[i] = (u8)mac[i]&0xFF;
	}

	if (argc == 3 + index)
	{
		security_type = RTW_SECURITY_OPEN;
		password = NULL;
		bssid_len = ETH_ALEN;
		password_len = 0;
		key_id = 0;
		semaphore = NULL;
	}
	else if(argc ==4 + index)
	{
		security_type = RTW_SECURITY_WPA2_AES_PSK;
		password = argv[3 + index];
		bssid_len = ETH_ALEN;
		password_len = strlen((const char *)argv[3 + index]);
		key_id = 0;
		semaphore = NULL;
	}
	else
	{
		security_type = RTW_SECURITY_WEP_PSK;
		password = argv[3 + index];
		bssid_len = ETH_ALEN;
		password_len = strlen((const char *)argv[3 + index]);
		key_id = atoi(argv[4 + index]);
		if ((password_len != 5) && (password_len != 13))
		{
			printf("\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			goto cmd_wifi_connect_bssid_exit;
		}
		if ((key_id < 0) || (key_id > 3))
		{
			printf("\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			goto cmd_wifi_connect_bssid_exit;
		}
		semaphore = NULL;
	}

	ret = wifi_connect_bssid(bssid,
					ssid,
					security_type,
					password,
					bssid_len,
					ssid_len,
					password_len,
					key_id,
					semaphore);

	tick2 = xTaskGetTickCount();
	printf("\r\nConnected after %dms.\n", (tick2-tick1));
	if(ret != RTW_SUCCESS)
	{
		printf("\n\rERROR: Operation failed!");
		goto cmd_wifi_connect_bssid_exit;
	}
	else
	{
#if CONFIG_LWIP_LAYER
		/* Start DHCP Client */
		ret = MISC_DHCP(0, DHCPC_START, 1);
#endif
	}

	if (ret != DHCPC_ERROR)
	{
		printf("\r\n\nGot IP after %dms.\n", (xTaskGetTickCount() - tick1));
	}
cmd_wifi_connect_bssid_exit:
	MISC_LeaveWiFiBusySection();
}

static void cmd_wifi_disconnect(int argc, char **argv)
{
	int timeout = 20;
	char essid[33];

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}

	printf("\n\rDeassociating AP ...");

	if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0)
	{
		printf("\n\rWIFI disconnected");
		return;
	}

	if (wifi_disconnect() < 0)
	{
		printf("\n\rERROR: Operation failed!");
		return;
	}

	while (1)
	{
		if(wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) < 0)
		{
			printf("\n\rWIFI disconnected");
			break;
		}

		if (timeout == 0)
		{
			printf("\n\rERROR: Deassoc timeout!");
			break;
		}

		vTaskDelay(1 * configTICK_RATE_HZ);
		timeout --;
	}
}



static void cmd_wifi_info(int argc, char **argv)
{
	int i = 0;
#if CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(&xnetif[0]);
	u8 *ip = LwIP_GetIP(&xnetif[0]);
	u8 *gw = LwIP_GetGW(&xnetif[0]);
#endif
	u8 *ifname[2] = {WLAN0_NAME,WLAN1_NAME};
#ifdef CONFIG_MEM_MONITOR
	extern int min_free_heap_size;
#endif
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
			printf("\n\r\nWIFI %s Status: Running",  ifname[i]);
			printf("\n\r==============================");

			rltk_wlan_statistic(i);

			wifi_get_setting((const char*)ifname[i],&setting);
			wifi_show_setting((const char*)ifname[i],&setting);
#if CONFIG_LWIP_LAYER
			printf("\n\rInterface (%s)", ifname[i]);
			printf("\n\r==============================");
			printf("\n\r\tMAC => %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			printf("\n\r\tIP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			printf("\n\r\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
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

				printf("\n\rAssociated Client List:");
				printf("\n\r==============================");

				if(client_info.count == 0)
				{
					printf("\n\rClient Num: 0\n\r", client_info.count);
				}
				else
				{
				    printf("\n\rClient Num: %d", client_info.count);
				    for( client_number=0; client_number < client_info.count; client_number++ )
				    {
						printf("\n\rClient %d:", client_number);
						printf("\n\r\tMAC => "MAC_FMT"", MAC_ARG(client_info.mac_list[client_number].octet));
					}
					printf("\n\r");
				}
			}
			error = wifi_get_last_error();
			if(setting.mode == RTW_MODE_AP)
			{
				error = RTW_NO_ERROR;
			}
			printf("\n\rLast Link Error");
			printf("\n\r==============================");
			switch(error)
			{
				case RTW_NO_ERROR:
					printf("\n\r\tNo Error");
					break;
				case RTW_NONE_NETWORK:
					printf("\n\r\tTarget AP Not Found");
					break;
				case RTW_CONNECT_FAIL:
					printf("\n\r\tAssociation Failed");
					break;
				case RTW_WRONG_PASSWORD:
					printf("\n\r\tWrong Password");
					break;
				case RTW_DHCP_FAIL:
					printf("\n\r\tDHCP Failed");
					break;
				default:
					printf("\n\r\tUnknown Error(%d)", error);
					break;
			}
			printf("\n\r");
		}
	}

#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1)
	{
		signed char pcWriteBuffer[1024];
		vTaskList((char*)pcWriteBuffer);
		printf("\n\rTask List: \n%s", pcWriteBuffer);
	}
#endif
#ifdef CONFIG_MEM_MONITOR
	printf("\n\rMemory Usage");
	printf("\n\r==============================");
	printf("\r\nMin Free Heap Size:  %d", min_free_heap_size);
	printf("\r\nCur Free Heap Size:  %d\n", xPortGetFreeHeapSize());
#endif
}

//static void cmd_wifi_on(int argc, char **argv)
void cmd_wifi_on(int argc, char **argv)
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
	u8	networkBackupMode = GCONFIG_GetNetworkBackupMode();

	if (networkBackupMode == GCONFIG_NETWORK_BACKUP_FORCE_ETH)
	{
		if (mode == RTW_MODE_AP)
		{
			printf("\n\rReject, AP mode under force ethernet");
			return;
		}
	}
	else if (networkBackupMode == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR)
	{
		if ((mode == RTW_MODE_AP) &&
			(MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) &&
			(MISC_NetInterfaceIsLinkUp(IF_ID_ETH)))
		{
			printf("\n\rReject, AP mode under ethernet as major and ethernet linked");
			return;
		}
	}
#endif

	if (GCONFIG_GetWifiRfEnable() == 0)
	{
		if (mode == RTW_MODE_STA)
		{
			if (wifi_on(mode) < 0)
			{
				printf("\n\rERROR: Wifi on failed!\n");
				return;
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
			if (wifi_on(RTW_MODE_AP) < 0){
				printf("\n\rERROR: Wifi on failed!");
				return;
			}

			printf("\n\rStarting AP ...");
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
					printf("\n\rERROR: Operation failed!");
					return;
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
					printf("\n\rERROR: Operation failed!");
					return;
				}
			}

			while (1)
			{
				char essid[33];

				if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0)
				{
					if (strcmp((const char *) essid, (const char *)ssid) == 0)
					{
						printf("\n\r%s started\n", ssid);
						GCONFIG_SetWifiRfEnable(1);
						break;
					}
				}

				if (timeout == 0)
				{
					printf("\n\rERROR: Start AP timeout!");
					break;
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
		printf("\n\rWIFI is already running\n");
	}
}

//static void cmd_wifi_off(int argc, char **argv)
void cmd_wifi_off(int argc, char **argv)
{


	wifi_off();
	GCONFIG_SetWifiRfEnable(0);
}

static void print_scan_result( rtw_scan_result_t* record )
{
	int i;
    RTW_API_INFO( ( MAC_FMT, MAC_ARG(record->BSSID.octet) ) );
    RTW_API_INFO( ( " ") );
    RTW_API_INFO( ( "%s ", record->SSID.val ) );
	i = 32-record->SSID.len;
	while (i--)
	{
	    RTW_API_INFO( ( " ") );
	}
    RTW_API_INFO( ( "%s ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra" ) );
    RTW_API_INFO( ( "%02d ", record->channel ) );
//    RTW_API_INFO( ( " %d\t  ", record->wps_type ) );
    RTW_API_INFO( ( "%02d    ", record->signal_strength ) );
	RTW_API_INFO(("%s", (record->security == RTW_SECURITY_OPEN) ? "Open" :
						(record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
						(record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
						(record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
						(record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
						(record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
						(record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
						(record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" : "Unknown" ) );
    RTW_API_INFO( ( "\r\n" ) );
}

static void wifi_scanned_bss_info_maintain(u8 opt, u8 num, rtw_scan_result_t *record)
{
	if (opt == 0)
	{
		memset(wifi_scaned_bss_table, 0, sizeof(wifi_scaned_bss_info_t) * STA_SCANNED_BSS_MAX);
	}
	else if (num < STA_SCANNED_BSS_MAX)
	{
		wifi_scaned_bss_table[num].occupied = 1;
		wifi_scaned_bss_table[num].network_type = record->bss_type;
		strcpy(wifi_scaned_bss_table[num].ssid, record->SSID.val);
		wifi_scaned_bss_table[num].security_type = record->security;
		RTW_API_INFO(("%02d ", num));
		print_scan_result(record);
	}
	else
	{
//		printf("\n\rWARNING: wifi_scanned_bss_info_maintain() oversize");
	}
}

static rtw_result_t app_scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	static int ApNum = 0;

	if (malloced_scan_result->scan_complete != RTW_TRUE)
	{
		if (ApNum==0)
	    	printf("\r\nID BSSID             SSID                             TYPE  CH SIGNAL SECURITY\r\n");
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */
		wifi_scanned_bss_info_maintain(1, ApNum, record);
		ApNum++;
	}
	else
	{
		ApNum = 0;
	}
	return RTW_SUCCESS;
}

#if SCAN_WITH_SSID
static void cmd_wifi_scan_with_ssid(int argc, char **argv)
{
	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;

	char *ssid = NULL;
	int ssid_len = 0;
	int scan_buf_len = 500;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}

	//Fully scan
	if (argc == 3 && argv[1] && argv[2])
	{
		ssid = argv[1];
		ssid_len = strlen((const char *)argv[1]);
		if((ssid_len <= 0) || (ssid_len > 32)) {
			printf("\n\rWrong ssid. Length must be less than 32.");
			goto exit;
		}
		scan_buf_len = atoi(argv[2]);
		if(scan_buf_len < 36){
			printf("\n\rBUFFER_LENGTH too short\n\r");
			goto exit;
		}
	}
	else if (argc > 3)
	{
		int i = 0;
		int num_channel = atoi(argv[2]);
		ssid = argv[1];
		ssid_len = strlen((const char *)argv[1]);
		if((ssid_len <= 0) || (ssid_len > 32)) {
			printf("\n\rWrong ssid. Length must be less than 32.");
			goto exit;
		}
		channel_list = (u8*)pvPortMalloc(num_channel);
		if (!channel_list)
		{
			printf("\n\r ERROR: Can't malloc memory for channel list");
			goto exit;
		}
		pscan_config = (u8*)pvPortMalloc(num_channel);
		if(!pscan_config){
			printf("\n\r ERROR: Can't malloc memory for pscan_config");
			goto exit;
		}
		//parse command channel list
		for (i = 3; i <= argc -1 ; i++)
		{
			*(channel_list + i - 3) = (u8)atoi(argv[i]);
			*(pscan_config + i - 3) = PSCAN_ENABLE;
		}

		if(wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0)
		{
		    printf("\n\rERROR: wifi set partial scan channel fail");
		    goto exit;
		}
	}
	else
	{
	    printf("\n\r For Scan all channel Usage: wifi_scan_with_ssid ssid BUFFER_LENGTH");
	    printf("\n\r For Scan partial channel Usage: wifi_scan_with_ssid ssid num_channels channel_num1 ...");
	    return;
	}

	if (wifi_scan_networks_with_ssid(NULL, &scan_buf_len, ssid, ssid_len) != RTW_SUCCESS)
	{
		printf("\n\rERROR: wifi scan failed");
		goto exit;
	}

exit:
	if (pscan_config)
	{
		vPortFree(pscan_config);
	}

	if(argc > 2 && channel_list)
	{
		vPortFree(channel_list);
	}
}
#endif
static void cmd_wifi_scan(int argc, char **argv)
{
	u8 *channel_list = NULL;
	u8 *pscan_config = NULL;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}

	if (argc > 2)
	{
		int i = 0;
		int num_channel = atoi(argv[1]);

		channel_list = (u8*)pvPortMalloc(num_channel);
		if (!channel_list)
		{
			printf("\n\r ERROR: Can't malloc memory for channel list");
			goto exit;
		}

		pscan_config = (u8*)pvPortMalloc(num_channel);
		if (!pscan_config)
		{
			printf("\n\r ERROR: Can't malloc memory for pscan_config");
			goto exit;
		}

		//parse command channel list
		for(i = 2; i <= argc -1 ; i++){
			*(channel_list + i - 2) = (u8)atoi(argv[i]);
			*(pscan_config + i - 2) = PSCAN_ENABLE;
		}

		if (wifi_set_pscan_chan(channel_list, pscan_config, num_channel) < 0)
		{
		    printf("\n\rERROR: wifi set partial scan channel fail");
		    goto exit;
		}
	}

	if (wifi_scan_networks(app_scan_result_handler, NULL ) != RTW_SUCCESS)
	{
		printf("\n\rERROR: wifi scan failed");
		goto exit;
	}

exit:
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
}

static void cmd_wifi_iwpriv(int argc, char **argv)
{
	if (argc == 2 && argv[1])
	{
		wext_private_command(WLAN0_NAME, argv[1], 1);
	}
	else
	{
		printf("\n\rUsage: iwpriv COMMAND PARAMETERS");
	}
}

static void cmd_wifi_mode(int argc, char **argv)
{
	u8	mode = GCONFIG_GetWifiNetworkMode();

}

static void cmd_wifi_channel(int argc, char **argv)
{
	u8	mode;
	u8	channel, minChNum, maxChNum;

	if (GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum)==0)
	{
		printf("\n\rInvalid country id, set 0 as default\n");
		GCONFIG_SetWifiCountryId(RTW_COUNTRY_WORLD1);
		GCONFIG_GetChannelPlan(GCONFIG_GetWifiCountryId(), &minChNum, &maxChNum);
	}

	if (argc != 2)
	{
		printf("\n\rWIFI Channel: %d", GCONFIG_GetChannel());
		printf("\n\rUsage: wifi_channel [CHANNEL]");
		printf("\n\rCHANNEL = %d ~ %d", minChNum, maxChNum);
		return;
	}

	mode = GCONFIG_GetWifiNetworkMode();

	if (mode == RTW_MODE_AP)
	{
		channel = atoi((const char *)argv[1]);

		if (!((channel >= minChNum) && (channel <= maxChNum)))
		{
			printf("\n\rPlease select a channel between %d ~ %d to match current channel plant.\n", minChNum, maxChNum);
			return;
		}

		/* Save channel to GCONFIG */
		GCONFIG_SetChannel(channel);
	}
	else
	{
		printf("\n\rChannel parameter is used by AP mdoe");
	}
}

static void cmd_wifi_ssid(int argc, char **argv)
{
	u8	ssid_len;
	u8	ssid[33];
        u8      i;

	if (argc != 2)
	{
		if ((ssid_len = GCONFIG_GetWifiSsid(ssid)))
		{
			ssid[ssid_len] = '\0';
			printf("\n\rSSID = %s", ssid);
		}
		else
		{
			printf("\n\rERROR: GCONFIG_GetWifiSsid() failed");
		}
		printf("\n\rUsage: wifi_ssid [SSID]");
		printf("\n\rSSID = 1 ~ 32 ASCII characters");
		return;
	}

//Eric modify
//        ssid_len = strlen(argv[1]);        
//	GCONFIG_SetWifiSsid(argv[1], strlen(argv[1]));
}

static void cmd_wifi_enc(int argc, char **argv)
{
	U8_T	encryption_mode;
	U8_T	mode;

	mode = GCONFIG_GetWifiNetworkMode();

	if (argc != 2)
	{
		encryption_mode = GCONFIG_GetWifiEncryptMode();
		printf("\n\rWIFI Encryption Mode: %s", (encryption_mode == GCONFIG_WIFI_OPEN) ? "OPEN" :
				   							   (encryption_mode == GCONFIG_WIFI_WEP && mode == RTW_MODE_STA) ? "WEP" :
											   (encryption_mode == GCONFIG_WIFI_WEP && mode == RTW_MODE_AP) ? "WEP (AP mode not support)" :
											   (encryption_mode == GCONFIG_WIFI_WPA2_AES_PSK && mode == RTW_MODE_STA) ? "WPA/WPA2_AUTO_PSK" :
											   (encryption_mode == GCONFIG_WIFI_WPA2_AES_PSK && mode == RTW_MODE_AP) ? "WPA2_AES_PSK" : "Unknow encryption type");
		printf("\n\rUsage: wifi_enc [ENC_MODE]");
		printf("\n\rENC_MODE = 0(OPEN)");
		printf("\n\r           1(WEP)%s", (mode == RTW_MODE_STA) ? "" : "(AP mode not support)");
		printf("\n\r           2(%s)", (mode == RTW_MODE_STA) ? "WPA/WPA2_AUTO_PSK" : "WPA2_AES_PSK");
		return;
	}

	encryption_mode = (u8)atoi((const char *)argv[1]);
	if (encryption_mode >= GCONFIG_MAX_WIFI_SECURITY_TYPES)
	{
		printf("n\rError!");
		return;
	}
	if (mode == RTW_MODE_AP && encryption_mode == GCONFIG_WIFI_WEP)
	{
		printf("\n\rNot support WEP in AP mode");
		return;
	}

	GCONFIG_SetWifiEncryptMode(security_map[encryption_mode]);
}

static void cmd_wifi_keyid(int argc, char **argv)
{
	u8	key_index;

	if (argc != 2)
	{
		printf("\n\rWIFI WEP Key index: %d", GCONFIG_GetWifiWepKeyIndex());
		printf("\n\rUsage: wifi_keyid [INDEX]");
		printf("\n\rINDEX = 0 ~ 3");
		return;
	}

	if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
	{
		printf("\n\rNot support WEP in AP mode");
		return;
	}

	key_index = (u8)atoi((const char *)argv[1]);

	/* Save WEP Key Index to GCONFIG */
	GCONFIG_SetWifiWepKeyIndex(key_index);
}

static void cmd_wifi_wepkey(int argc, char **argv)
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
			printf("\n\rWIFI WEP64 Key %d: %c%c%c%c%c", i,
				   										password[0],
														password[1],
														password[2],
														password[3],
														password[4]);
		}
		for (i = 0; i < 4; i++)
		{
			GCONFIG_GetWifiWep128Key(i, password);
			printf("\n\rWIFI WEP128 Key %d: %c%c%c%c%c%c%c%c%c%c%c%c%c", i,
				   										 password[0],
														 password[1],
														 password[2],
														 password[3],
														 password[4],
														 password[5],
														 password[6],
														 password[7],
														 password[8],
														 password[9],
														 password[10],
														 password[11],
														 password[12]);
		}
		printf("\n\rUsage: wifi_wepkey [INDEX] [KEY]");
		printf("\n\rINDEX = 0 ~ 3");
		printf("\n\rKEY = 5 or 13 ASCII characters");
		return;
	}

	if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
	{
		printf("\n\rNot support WEP in AP mode");
		return;
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
}

static void cmd_wifiWepKeyLen(int argc, char **argv)
{
	u8 value;

	if (argc != 2)
	{
		printf("\n\rUsage: wifi_wepkeylen <0:64 bits, 1:128 bits>");
		printf("\r\nWEP key length: ");
		if (GCONFIG_GetWifiWepKeyLength()== 0)
            printf("64 bits");
        else
            printf("128 bits");
		return;
	}

	value = (u8)atoi((const char *)argv[1]);
    if (value > 1)
	{
		printf("\n\rInvalid parameter");
		return;
	}

    GCONFIG_SetWifiWepKeyLength(value);
	return;
}

static void cmd_wifi_wpakey(int argc, char **argv)
{
	u8			password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];
	u8			password_len;

	if (argc != 2)
	{
		printf("\n\rUsage: wifi_wpakey [KEY]");
		printf("\n\rKEY = 8 ~ 63 ASCII characters");
		password_len = GCONFIG_GetWifiPreShareKey(password);
		password[password_len] = '\0';
		printf("\n\rWIFI WPA Key: %s", password);
		return;
	}

	password_len = strlen(argv[1]);
	if (password_len < 8 || password_len > 63)
	{
		printf("\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
		return;
	}

	/* Save WPA Key into GCONFIG */
	GCONFIG_SetWifiPreShareKey(argv[1], strlen(argv[1]));
}

static void cmd_wifi_jbss(int argc, char **argv)
{
	u8						index;
	rtw_network_info_t		wifi;
	wifi_scaned_bss_info_t	*pBssInfo;
	u8						encryption_mode;
	u8						password[GCONFIG_WIFI_PRESHARE_KEY_BUFSIZE];
	u8						password_len;
	u8						key_id;
	unsigned long			tick1 = xTaskGetTickCount();
	unsigned long			tick2, tick3;
	int						ret = RTW_ERROR;

	if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}

	MISC_EnterWiFiBusySection();

	if (argc != 2)
	{
		printf("\n\rUsage: wifi_jbss [INDEX]");
		printf("\n\rINDEX = Index of bss scan table, maximum %d BSSs supported", STA_SCANNED_BSS_MAX);
		goto cmd_wifi_jbss_exit;
	}

	if (GCONFIG_GetWifiNetworkMode() != RTW_MODE_STA)
	{
		printf("\n\rWarning: please switch wifi network mode to STA");
		goto cmd_wifi_jbss_exit;
	}

	index = atoi((const char *)argv[1]);
	if (index >= STA_SCANNED_BSS_MAX)
	{
		printf("\n\rUsage: wifi_jbss [INDEX]");
		printf("\n\rINDEX = Index of bss scan table, maximum %d BSSs supported", STA_SCANNED_BSS_MAX);
		goto cmd_wifi_jbss_exit;
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

//				printf("\n\rError: unsupported security type");
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
		printf("\r\nConnected after %dms.\n", (tick2-tick1));
		if(ret != RTW_SUCCESS)
		{
			printf("\n\rERROR: Operation failed!");
			goto cmd_wifi_jbss_exit;
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
			printf("\r\n\nGot IP after %dms.\n", (xTaskGetTickCount() - tick1));
		}
	}
	else
	{
		printf("\n\rError: invalid selection\n");
	}
cmd_wifi_jbss_exit:
	MISC_LeaveWiFiBusySection();
}
#endif	//#if CONFIG_WLAN

static void cmd_ping(int argc, char **argv)
{
	if(argc == 2) {
		do_ping_call(argv[1], 0, 5);	//Not loop, count=5
	}
	else if(argc == 3) {
		if(strcmp(argv[2], "loop") == 0)
			do_ping_call(argv[1], 1, 0);	//loop, no count
		else
			do_ping_call(argv[1], 0, atoi(argv[2]));	//Not loop, with count
	}
	else {
		printf("\n\rUsage: ping IP [COUNT/loop]");
	}
}
#if ( configGENERATE_RUN_TIME_STATS == 1 )
static char cBuffer[ 512 ];
static void cmd_cpustat(int argc, char **argv)
{
	vTaskGetRunTimeStats( ( char * ) cBuffer );
	printf( cBuffer );
}
#endif

static void cmd_debug(int argc, char **argv)
{
	if(strcmp(argv[1], "ready_trx") == 0)
	{
		printf("\r\n%d", wifi_is_ready_to_transceive((rtw_interface_t)rtw_atoi((u8*)argv[2])));
	}
	else if(strcmp(argv[1], "is_up") == 0)
	{
		printf("\r\n%d", wifi_is_up((rtw_interface_t)rtw_atoi((u8*)argv[2])));
	}
	else if(strcmp(argv[1], "set_mac") == 0)
	{
		printf("\r\n%d", wifi_set_mac_address(argv[2]));
	}
	else if(strcmp(argv[1], "get_mac") == 0)
	{
		u8 mac[18] = {0};
		wifi_get_mac_address((char*)mac);
		printf("\r\n%s", mac);
	}
	else if(strcmp(argv[1], "ps_on") == 0)
	{
		printf("\r\n%d", wifi_enable_powersave());
	}
	else if(strcmp(argv[1], "ps_off") == 0)
	{
		printf("\r\n%d", wifi_disable_powersave());
#if 0 //TODO
	}
	else if(strcmp(argv[1], "get_txpwr") == 0)
	{
		int idx;
		wifi_get_txpower(&idx);
		printf("\r\n%d", idx);
	}
	else if(strcmp(argv[1], "set_txpwr") == 0)
	{
		printf("\r\n%d", wifi_set_txpower(rtw_atoi((u8*)argv[2])));
#endif
	}
	else if(strcmp(argv[1], "get_clientlist") == 0)
	{
		int client_number;
		struct {
			int    count;
			rtw_mac_t mac_list[3];
		} client_info;

		client_info.count = 3;

		printf("\r\n%d\r\n", wifi_get_associated_client_list(&client_info, sizeof(client_info)));

        if( client_info.count == 0 )
        {
            RTW_API_INFO(("Clients connected 0..\r\n"));
        }
        else
        {
            RTW_API_INFO(("Clients connected %d..\r\n", client_info.count));
            for( client_number=0; client_number < client_info.count; client_number++ )
            {
				RTW_API_INFO(("------------------------------------\r\n"));
				RTW_API_INFO(("| %d | "MAC_FMT" |\r\n",
									client_number,
									MAC_ARG(client_info.mac_list[client_number].octet)
							));
            }
            RTW_API_INFO(("------------------------------------\r\n"));
        }
	}
	else if(strcmp(argv[1], "get_apinfo") == 0)
	{
		rtw_bss_info_t ap_info;
		rtw_security_t sec;
		if(wifi_get_ap_info(&ap_info, &sec) == RTW_SUCCESS)
		{
			RTW_API_INFO( ("\r\nSSID  : %s\r\n", (char*)ap_info.SSID ) );
			RTW_API_INFO( ("BSSID : "MAC_FMT"\r\n", MAC_ARG(ap_info.BSSID.octet)) );
			RTW_API_INFO( ("RSSI  : %d\r\n", ap_info.RSSI) );
			//RTW_API_INFO( ("SNR   : %d\r\n", ap_info.SNR) );
			RTW_API_INFO( ("Beacon period : %d\r\n", ap_info.beacon_period) );
			RTW_API_INFO( ( "Security : %s\r\n", ( sec == RTW_SECURITY_OPEN )           ? "Open" :
													( sec == RTW_SECURITY_WEP_PSK )        ? "WEP" :
													( sec == RTW_SECURITY_WPA_TKIP_PSK )   ? "WPA TKIP" :
													( sec == RTW_SECURITY_WPA_AES_PSK )    ? "WPA AES" :
													( sec == RTW_SECURITY_WPA2_AES_PSK )   ? "WPA2 AES" :
													( sec == RTW_SECURITY_WPA2_TKIP_PSK )  ? "WPA2 TKIP" :
												 ( sec == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" : "Unknown" ));
		}
	}
	else if(strcmp(argv[1], "reg_mc") == 0)
	{
		rtw_mac_t mac;
		sscanf(argv[2], MAC_FMT, (int*)(mac.octet+0), (int*)(mac.octet+1), (int*)(mac.octet+2), (int*)(mac.octet+3), (int*)(mac.octet+4), (int*)(mac.octet+5));
		printf("\r\n%d", wifi_register_multicast_address(&mac));
	}
	else if(strcmp(argv[1], "unreg_mc") == 0)
	{
		rtw_mac_t mac;
		sscanf(argv[2], MAC_FMT, (int*)(mac.octet+0), (int*)(mac.octet+1), (int*)(mac.octet+2), (int*)(mac.octet+3), (int*)(mac.octet+4), (int*)(mac.octet+5));
		printf("\r\n%d", wifi_unregister_multicast_address(&mac));
	}
	else if(strcmp(argv[1], "get_rssi") == 0)
	{
		int rssi = 0;
		wifi_get_rssi(&rssi);
		printf("\n\rwifi_get_rssi: rssi = %d", rssi);
	}
	else if (strcmp(argv[1], "adap") == 0)
	{
		wext_private_command(WLAN0_NAME, "dbg 0x7fee0000 0xff", 1);
		wext_private_command(WLAN0_NAME, "dbg 0x7fc10000 0x20000", 1);
	}
	else
	{
		printf("\r\nUnknown CMD\r\n");
	}
}

typedef struct _cmd_entry {
	char *command;
	void (*function)(int, char **);
} cmd_entry;

static const cmd_entry cmd_table[] = {
#if CONFIG_WLAN
	{"wifi_connect", cmd_wifi_connect},
//	{"wifi_connect_bssid", cmd_wifi_connect_bssid},
	{"wifi_disconnect", cmd_wifi_disconnect},
	{"wifi_info", cmd_wifi_info},
	{"wifi_on", cmd_wifi_on},
	{"wifi_off", cmd_wifi_off},
	{"wifi_ap", cmd_wifi_ap},
	{"wifi_scan", cmd_wifi_scan},
#if SCAN_WITH_SSID
	{"wifi_scan_with_ssid", cmd_wifi_scan_with_ssid},
#endif
	{"iwpriv", cmd_wifi_iwpriv},
//	{"wifi_promisc", cmd_promisc},
	{"wifi_mode", cmd_wifi_mode},
	{"wifi_channel", cmd_wifi_channel},
	{"wifi_ssid", cmd_wifi_ssid},
	{"wifi_enc", cmd_wifi_enc},
	{"wifi_keyid", cmd_wifi_keyid},
	{"wifi_wepkey", cmd_wifi_wepkey},
	{"wifi_wepkeylen", cmd_wifiWepKeyLen},
	{"wifi_wpakey", cmd_wifi_wpakey},
	{"wifi_jbss", cmd_wifi_jbss},
#if (CONFIG_INCLUDE_SIMPLE_CONFIG)
	{"wifi_simple_config", cmd_simple_config},
#endif
#ifdef CONFIG_WPS
#if CONFIG_ENABLE_WPS
//	{"wifi_wps", cmd_wps},
#endif
#ifdef CONFIG_WPS_AP
	{"wifi_ap_wps", cmd_ap_wps},
#endif
#endif
#ifdef CONFIG_CONCURRENT_MODE
//	{"wifi_sta_ap",cmd_wifi_sta_and_ap},
#endif

#if CONFIG_SSL_CLIENT
	{"ssl_client", cmd_ssl_client},
#endif
#endif

#if CONFIG_LWIP_LAYER
	{"wifi_debug", cmd_debug},
	{"ping", cmd_ping},
#endif
#if ( configGENERATE_RUN_TIME_STATS == 1 )
	{"cpu", cmd_cpustat},
#endif
	{"help", cmd_help},
	{"ur_config", cmd_urConfig},
	{"saveconfig", cmd_saveConfig},
	{"ipconfig", cmd_ipConfig},
	{"reboot", cmd_systemReboot},
	{"dhcpclient", cmd_dhcpClient},
	{"setip", cmd_setIpAddr},
	{"setnetmask", cmd_setNetmask},
	{"setgateway", cmd_setGateway},
	{"setmode", cmd_setMode},
	{"connectype", cmd_setConnectType},
	{"setdhcpsrv", cmd_enableDhcpServer},
    {"ntpsrv", cmd_setNtpServer},
    {"rtcts", cmd_rtcTimeSetup},
    {"time", cmd_currentTime},
    {"date", cmd_currentDate},
    {"getths", cmd_getTemperatureHumidity},
	{"setdef", cmd_setDefaultSettings},
	{"setdns", cmd_setDnsIp},
	{"setsrvport", cmd_setServerPort},
	{"setdstport", cmd_setDestPort},
	{"setdsthn", cmd_setDestHostName},
	{"setems", cmd_SetEmailServerAddr},
    {"setemf", cmd_SetEmailFromAddr},
    {"setemt1", cmd_SetEmailTo1Addr},
    {"setemt2", cmd_SetEmailTo2Addr},
    {"setemt3", cmd_SetEmailTo3Addr},
    {"setemsc", cmd_SetSMTPSecurity},
    {"setemac", cmd_SetSMTPAccount},
    {"emconfig", cmd_GetEmailConfig},
    {"setaw", cmd_SetAWConfig},
	{"r2wmode", cmd_R2wMode},
	{"jtagoff", cmd_JtagOff},
	{"getotaname", cmd_GetOTAName},
	{"countryid", cmd_SetCountryId},
	{"dhcpsrv", cmd_setDhcpServer},
	{"transmitimer", cmd_SetEtherTimer},
	{"connstatus", cmd_ConnectStatus},
	{"dhcpstbl", cmd_dhcpServerAllocTbl},
	{"scpincode", cmd_simpleCfgPinCode},
	{"urCounter", cmd_urCounter},
	{"rs485", cmd_setRs485Status},
#if CONFIG_SMART_PLUG
	{"smartplug", cmd_SmartPlug},
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
	{"cloud", cmd_setGoogleNestInfo},
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
	{"bmh", cmd_setBluemixHostName},
	{"bma", cmd_setBluemixAccount},
	{"bmr", cmd_setBluemixAutoReg},
#endif
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
	{"azh", cmd_setAzureHostName},
	{"aza", cmd_setAzureAccount},
#endif
	{"mbtcp", cmd_setModbusTcp},
	{"mbst", cmd_setModbusSerialTiming},
	{"clientconnmode", cmd_setClientConnectionMode},
	{"clientconn", cmd_ClientConnect},
	{"clientclose", cmd_ClientClose},
	{"setdevname", cmd_setDeviceName},
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	{"setnetbackup", cmd_setNetworkBackup},
#endif
	{"setadapthres", cmd_setAdaptivityThreshold},
//	{"tcp", cmd_tcp},
};

static void cmd_urConfig(int argc, char **argv)
{
	if (argc != 6)
	{
		printf("\n\rUsage: ur_config <baudrate> <databits> <stopbit> <parity flowctrl>\r\n");
		printf("<baud_rate>: \r\n");
		printf("             1200 bps\r\n");
		printf("             2400 bps\r\n");
		printf("             4800 bps\r\n");
		printf("             9600 bps\r\n");
		printf("             19200 bps\r\n");
		printf("             38400 bps\r\n");
		printf("             57600 bps\r\n");
		printf("             115200 bps\r\n");
		printf("             921600 bps\r\n");
		printf("<databits>: 7 or 8 bits\r\n");
		printf("<stop_bits>: 1 or 2 bit(s)\r\n");
		printf("<parity>: 0 = none, 1 = odd, 2 = even\r\n");
		printf("<flow_contrl>: 0 = disable, 1 = enable CTS/RTS flow control\r\n");
		printf("\n\rCurrent Settings\r\n");
		printf("Baudrate: %d\r\n", GCONFIG_GetUrBaudRate());
		printf("Databits: %d\r\n", GCONFIG_GetUrDataBits());
		printf("Stopbits: %d\r\n", GCONFIG_GetUrStopBits());
		printf("Parity: %d\r\n", GCONFIG_GetUrParity());
		printf("FlowCtrl: %d\r\n", GCONFIG_GetUrFlowCtrl());
		return;
	}

	GCONFIG_SetUrBaudRate(atol(argv[1]));
	GCONFIG_SetUrDataBits(atoi(argv[2]));
	GCONFIG_SetUrStopBits(atoi(argv[3]));
	GCONFIG_SetUrParity(atoi(argv[4]));
	GCONFIG_SetUrFlowCtrl(atoi(argv[5]));
}

static void cmd_saveConfig(int argc, char **argv)
{
	GCONFIG_WriteConfigData();
}

static void cmd_ipConfig(int argc, char **argv)
{
	u8 *tmp8;

	if (netif_default != NULL)
	{
		tmp8 = (u8 *)&(netif_default->ip_addr);
		printf("IP address: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = (u8 *)&(netif_default->netmask);
		printf("Netmask: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = (u8 *)&(netif_default->gw);
		printf("Gateway: %d.%d.%d.%d\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3]);
		tmp8 = GCONFIG_GetMacAddress();
		printf("Mac: 0x%02x%02x%02x%02x%02x%02x\r\n",tmp8[0], tmp8[1], tmp8[2], tmp8[3], tmp8[4], tmp8[5]);
		printf("Interface: %s\r\n", (MISC_GetNetworkBackupSwitchStatus() == NETWORK_SWITCH_TO_ETH_IF) ? "Ethernet":"WiFi");
	}
	else
	{
		printf("No available network interface!\r\n");
	}
}

static void cmd_systemReboot(int argc, char **argv)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
}

static void cmd_dhcpClient(int argc, char **argv)
{
	u16 temp16;
	int iVal;

	temp16 = GCONFIG_GetNetwork();
	if (argc == 1)
	{
    	printf("Usage: dhcpclient <status>\r\n");
    	printf("       <status>: 0: disable     1: enable\r\n");

		printf("\r\nCurrent DHCP client status: %s\r\n", (temp16 & GCONFIG_NETWORK_DHCP_ENABLE) ? "enable":"disable");
		return;
	}
	else if (argc > 2)
	{
		return;
	}

	iVal = atoi(argv[1]);
	temp16 &= ~GCONFIG_NETWORK_DHCP_ENABLE;
    if (iVal > 0)
        temp16 |= GCONFIG_NETWORK_DHCP_ENABLE;

	GCONFIG_SetNetwork(temp16);
}

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
		{
            return 0;
    }
    }

	return 1;
}

//static s8 getIp2Ulong(u8 *pBuf, u8 len, u32 *ipAddr)
s8 getIp2Ulong(u8 *pBuf, u8 len, u32 *ipAddr)
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
				return -1;

            value *= 10;

            if (IsDigit(*pBuf))
                value += (*pBuf - 0x30);
			else
				return -1;

			pBuf++; len--;
		}

        if (!digits)
            return -1;

        if (value > 255)
            return -1;

        addr = (addr << 8) + (u8)(value & 0xFF);
		if (len == 0)
			break;

		pBuf++; len--;
		count++;

		if (count > 3)
			return -1;
	}

	if (count != 3)
		return -1;

	*ipAddr = addr;

	return 1;
}

u8 validIp(u32 ipAddr)
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

static void cmd_setIpAddr(int argc, char **argv)
{
	u8 *tmp8;
	u32 addr;

	if (argc == 1) /* current setting */
    {
        addr = GCONFIG_GetDeviceStaticIP();
		tmp8 = (u8 *)&addr;
		printf("IP address: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);
        return;
    }
    else if (argc > 2)
        return;


	if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) > 0)
	{
		if (validIp(addr))
		{
			GCONFIG_SetDeviceStaticIP(addr);
			EMAIL_SendIpChanged();
		}
	}
}

static void cmd_setNetmask(int argc, char **argv)
{
	u8 *tmp8;
	u32 netmask;

    if (argc == 1) /* current setting */
    {
        netmask = GCONFIG_GetNetmask();
		tmp8 = (u8 *)&netmask;
		printf("Netmask: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);

        return;
    }
    else if (argc > 2)
        return;

    if (getIp2Ulong(argv[1], strlen(argv[1]), &netmask) > 0)
        GCONFIG_SetNetmask(netmask);
}

static void cmd_setGateway(int argc, char **argv)
{
	u8 *tmp8;
	u32 gateway;

    if (argc == 1) /* current setting */
    {
		gateway = GCONFIG_GetGateway();
		tmp8 = (u8 *)&gateway;
        printf("Gateway: %d.%d.%d.%d\r\n", tmp8[3], tmp8[2], tmp8[1], tmp8[0]);
        return;
    }
    else if (argc > 2)
        return;

    if (getIp2Ulong(argv[1], strlen(argv[1]), &gateway) > 0)
    {
        if (validIp(gateway))
        	GCONFIG_SetGateway(gateway);
    }
}

static void cmd_setMode(int argc, char **argv)
{
	u16 temp16;
	int iTemp;

    temp16 = GCONFIG_GetNetwork();
    if (argc == 1) /* current setting */
    {
    	printf("Usage: setmode <mode>\r\n");
    	printf("       <mode>: 0: SERVER     1: CLIENT\r\n");
		printf("\r\nCurrent Device Mode: %s mode\r\n", (temp16 & GCONFIG_NETWORK_CLIENT) ? "CLIENT":"SERVER");
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);
    if (iTemp > 1)
		return;

    temp16 &= ~GCONFIG_NETWORK_CLIENT;
    if (iTemp)
        temp16 |= GCONFIG_NETWORK_CLIENT;

    GCONFIG_SetNetwork(temp16);
}

static void cmd_setConnectType(int argc, char **argv)
{
	u16 temp16;
	int iTemp;

    temp16 = GCONFIG_GetNetwork();
    if (argc == 1) /* current setting */
    {
    	printf("Usage: connectype <protocol>\r\n");
    	printf("       <protocol>: 0: TCP     1: UDP\r\n");
		printf("\r\nCurrent connection type: %s\r\n", ((temp16 & (GCONFIG_NETWORK_PROTO_UDP | GCONFIG_NETWORK_PROTO_TCP))==GCONFIG_NETWORK_PROTO_UDP) ? "UDP":"TCP");
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);

    if (iTemp > 1)
		return;

    temp16 &= ~(GCONFIG_NETWORK_PROTO_UDP | GCONFIG_NETWORK_PROTO_TCP);
    if (iTemp)
        temp16 |= GCONFIG_NETWORK_PROTO_UDP;
    else
        temp16 |= GCONFIG_NETWORK_PROTO_TCP;

    GCONFIG_SetNetwork(temp16);
}

static void cmd_setDhcpServer(int argc, char **argv)
{
	int iTemp;
	u32 addr;

	if (argc == 1) /* current setting */
    {
    	printf("Usage: dhcpsrv <start addr> <end addr> <default gateway addr> <lease>\r\n");

		printf("\r\nCurrent DHCP server settings:\r\n");
        addr = GCONFIG_GetDhcpSrvStartIp();
        printf("Start IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
													(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
        addr = GCONFIG_GetDhcpSrvEndIp();
        printf("End IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
												(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));

		addr = GCONFIG_GetDhcpSrvDefGateway();
		printf("Default gateway IP address: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
												(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
        printf("Lease Time: %d min\r\n", GCONFIG_GetDhcpSrvLeaseTime());

        return;
    }
    else if (argc > 5)
        return;

	if(argc > 1)
	{
	  /*Set Start IP*/
	  if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
		  return;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvStartIp(addr);
	}

	if(argc > 2)
	{
	  /*Set End IP*/
	  if (getIp2Ulong(argv[2], strlen(argv[2]), &addr) < 0)
		  return;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvEndIp(addr);
	}

	if(argc > 3)
	{
	  /*Set Gateway IP*/
	  if (getIp2Ulong(argv[3], strlen(argv[3]), &addr) < 0)
		  return;

	  if (validIp(addr))
		  GCONFIG_SetDhcpSrvDefGateway(addr);
	}

	if(argc > 4)
	{
	  iTemp = atoi(argv[4]);
	  GCONFIG_SetDhcpSrvLeaseTime((u16)iTemp);
	}

}

static void cmd_dhcpServerAllocTbl(int argc, char **argv)
{
	int iTemp;
	u32 addr;

	memset(dhcps_allocation_table, 0, sizeof(dhcps_allocation_table));
	addr = DHCPS_AllocationTable(dhcps_allocation_table, DHCPS_MAX_IP_POOL_NUM);
	printf("\r\nCurrent IP allocation table%s\r\n", addr==0 ? " is empty":"...");
	if (addr)
	{
		printf("Index IP              LeaseTime  MacAddr\r\n");
		for (iTemp=0; iTemp < addr; iTemp++)
		{
				printf("%02d    %03d.%03d.%03d.%03d %08d   %02x:%02x:%02x:%02x:%02x:%02x\r\n"
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
}

static void cmd_simpleCfgPinCode(int argc, char **argv)
{
	int iTemp;

	if (argc == 1) /* current setting */
    {
    	printf("Usage: scpincode <status>\r\n");
    	printf("       <status>: 0: disable     1: enable\r\n");
		printf("\r\nCurrent simple config pin code status: %s\r\n", GCONFIG_GetSimpleCfgPinCodeStatus() ? "enable":"disable");
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);
	GCONFIG_SetSimpleCfgPinCodeStatus(iTemp ? 1:0);
}

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
static void cmd_setGoogleNestInfo(int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
    	printf("Usage: cloud <hostname>\r\n");
		if (GCONFIG_GetGoogleNestHostName(StrBuf, GCONFIG_MAX_CLOUD_HOSTNAME_LEN))
		{
			printf("\r\nCurrent server host name is %s\r\n", StrBuf[0]==0 ? "unspecified":(char*)StrBuf);
			printf("\r\nCurrent connection status is %s\r\n", GNAPP_CheckConnectionStatus()==GNAPP_CONNECTED ? "connected":"disconnected");
		}
        return;
    }
    else if (argc > 2)
        return;

	if (argv[1][0]=='-' && strlen(argv[1])==1)
	{
		/* Clear host name */
		StrBuf[0] = '\0';
		GCONFIG_SetGoogleNestHostName(StrBuf);
	}
	else if (GCONFIG_SetGoogleNestHostName(argv[1]) == 0)
	{
		printf("Fail to set cloud server host name\r\n");
		return;
	}
}
#endif

static void cmd_setModbusTcp(int argc, char **argv)
{
	u16 ServerPort;
	u8 SlaveId, XferMode, *pMsg;

	GCONFIG_GetModbusTcp(&ServerPort, &SlaveId, &XferMode);
	if (argc == 1) /* current setting */
    {
    	printf("Usage: mbtcp <xferMode> <port>\r\n");
    	printf("       <xferMode>: 0: MODBUS TCP   1: Transparent TCP\r\n");
    	printf("       <port>: TCP server port number, default is 502\r\n");
//    	printf("       <slaveId>: Gateway slave id, available range is 1~255\r\n");
		switch (XferMode)
		{
		case GCONFIG_MBGW_TCP:
			pMsg = "MODBUS TCP";
			break;
		case GCONFIG_MBGW_TRANSPARENT:
			pMsg = "Transparent TCP";
			break;
		}
		printf("\r\nCurrent TransferMode=%s, server port=%d\r\n", pMsg, ServerPort);
        return;
    }
    else if (argc > 3)
        return;

	if (argc > 1)
    	XferMode = atoi(argv[1]);
	if (argc > 2)
    	ServerPort = atoi(argv[2]);
//	if (argc > 3)
 //   	SlaveId = atoi(argv[3]);
	if (GCONFIG_SetModbusTcp(ServerPort, SlaveId, XferMode) == 0)
	{
		printf("Fail to set modbus transfer mode\r\n");
		return;
	}
}

static void cmd_setModbusSerialTiming(int argc, char **argv)
{
	u16 ResponseTimeOut, InterFrameDelay, InterCharDelay;

	GCONFIG_GetModbusSerialTiming(&ResponseTimeOut, &InterFrameDelay, &InterCharDelay);
	if (argc == 1) /* current setting */
    {
    	printf("Usage: mbst <ResponseTimeOut> <InterFrameDelay> <InterCharDelay>\r\n");
    	printf("       <ResponseTimeOut>: Response timeout(10~65000ms)\r\n");
    	printf("       <InterFrameDelay>: Interval time of frame sending(10~500ms)\r\n");
    	printf("       <InterCharDelay>:  Inter-Character timeout for frame receiving(10~500ms)\r\n");
		printf("\r\nCurrent ResponseTimeOut=%d, InterFrameDelay=%d, InterCharDelay=%d\r\n", ResponseTimeOut, InterFrameDelay, InterCharDelay);
        return;
    }
    else if (argc > 4)
        return;

	if (argc > 1)
    	ResponseTimeOut = atoi(argv[1]);
	if (argc > 2)
    	InterFrameDelay = atoi(argv[2]);
	if (argc > 3)
    	InterCharDelay = atoi(argv[3]);
	if (GCONFIG_SetModbusSerialTiming(ResponseTimeOut, InterFrameDelay, InterCharDelay) == 0)
	{
		printf("Fail to set modbus transfer mode\r\n");
		return;
	}
}

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
static void cmd_setBluemixHostName(int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];

	if (argc == 1) /* current setting */
    {
		printf("Usage: bmh <hostname>\r\n");
		if (GCONFIG_GetBluemixHostName(tmpHostName, GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN))
		{
			printf("\r\nCurrent bluemix host name is %s\r\n", tmpHostName[0] == 0 ? "unspecified" : (char*)tmpHostName);
			printf("\r\nCurrent state: %s\r\n", BLUEMIX_CheckConnectionState());
		}
        return;
	}
	else if (argc > 2)
	{
        return;
	}

	if (argv[1][0] == '-' && strlen(argv[1]) == 1)
	{
		/* Clear host name */
		tmpHostName[0] = '\0';
		GCONFIG_SetBluemixHostName(tmpHostName);
	}
	else if (GCONFIG_SetBluemixHostName(argv[1]) == 0)
	{
		printf("Fail to set bluemix host name\r\n");
		return;
	}
}

static void cmd_setBluemixAccount(int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_BLUEMIX_HOSTNAME_LEN];
	u8	tmpOrgID[GCONFIG_MAX_BLUEMIX_ORG_ID_LEN];
	u8	tmpTypeID[GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN];
	u8	tmpDeviceID[GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN];
	u8	tmpUserName[GCONFIG_MAX_BLUEMIX_USERNAME_LEN];
	u8	tmpPassWord[GCONFIG_MAX_BLUEMIX_PASSWORD_LEN];

	if (argc == 1) /* current setting */
    {
		printf("Usage: bma <org_id> <type_id> <device_id> <username> <password>\r\n");
		printf("Lenght of <org_id> is 6 bytes\r\n");
		printf("Lenght of <type_id> is 5 bytes, where is \"23001\"\r\n");
		printf("Lenght of <device_id> is 11 bytes, where is serial number\r\n");
		printf("Lenght of <username> <= 32 bytes\r\n");
		printf("Lenght of <password> <= 32 bytes\r\n");
		printf("Client ID format = d:<org_id>:<type_id>:<device_id>\r\n");
		if (GCONFIG_GetBluemixAccount(tmpOrgID, GCONFIG_MAX_BLUEMIX_ORG_ID_LEN,
									  tmpTypeID, GCONFIG_MAX_BLUEMIX_TYPE_ID_LEN,
									  tmpDeviceID, GCONFIG_MAX_BLUEMIX_DEVICE_ID_LEN,
									  tmpUserName, GCONFIG_MAX_BLUEMIX_USERNAME_LEN,
									  tmpPassWord, GCONFIG_MAX_BLUEMIX_PASSWORD_LEN))
		{
			printf("\r\nCurrent bluemix organization ID is %s\r\n", tmpOrgID[0] == 0 ? "unspecified" : (char *)tmpOrgID);
			printf("\r\nCurrent bluemix type ID is %s\r\n", tmpTypeID[0] == 0 ? "unspecified" : (char *)tmpTypeID);
			printf("\r\nCurrent bluemix device ID is %s\r\n", tmpDeviceID[0] == 0 ? "unspecified" : (char *)tmpDeviceID);
			printf("\r\nCurrent bluemix username is %s\r\n", tmpUserName[0] == 0 ? "unspecified" : (char*)tmpUserName);
			printf("\r\nCurrent bluemix password is %s\r\n", tmpPassWord[0] == 0 ? "unspecified" : (char*)tmpPassWord);
		}
	}
	else if (argc == 6)
	{
		/* Set them all into GCONFIG */
		if (GCONFIG_SetBluemixAccount(argv[1], argv[2], argv[3], argv[4], argv[5]) == 0)
		{
			printf("\n\rError length\n\r");
			return;
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
	}
}

static void cmd_setBluemixAutoReg(int argc, char **argv)
{
	u8	tmpApiVersion[GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN];
	u8	tmpApiKey[GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN];
	u8	tmpApiToken[GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN];

	if (argc == 1) /* current setting */
    {
		printf("Usage: bmr <api_ver> <api_key> <api_token>\r\n");
		printf("Lenght of <api_ver> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN-1);
		printf("Lenght of <api_key> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN-1);
		printf("Lenght of <api_token> should be less than or equal to %d bytes\r\n", GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN-1);

		if (GCONFIG_GetBluemixAutoReg(tmpApiVersion, GCONFIG_MAX_BLUEMIX_AR_API_VER_LEN,
									  tmpApiKey, GCONFIG_MAX_BLUEMIX_AR_API_KEY_LEN,
									  tmpApiToken, GCONFIG_MAX_BLUEMIX_AR_API_TOKEN_LEN))
		{
			printf("\r\nCurrent bluemix API version is %s\r\n", tmpApiVersion[0] == 0 ? "unspecified" : (char *)tmpApiVersion);
			printf("\r\nCurrent bluemix API key is %s\r\n", tmpApiKey[0] == 0 ? "unspecified" : (char *)tmpApiKey);
			printf("\r\nCurrent bluemix API token is %s\r\n", tmpApiToken[0] == 0 ? "unspecified" : (char *)tmpApiToken);
		}
	}
	else if (argc == 4)
	{
		/* Set them all into GCONFIG */
		if (GCONFIG_SetBluemixAutoReg(argv[1], argv[2], argv[3]) == 0)
		{
			printf("\n\rError length\n\r");
			return;
		}
	}
}
#endif

#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
static void cmd_setAzureHostName(int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];

	if (argc == 1) /* current setting */
    {
		printf("Usage: azh <hostname>\r\n");
		if (GCONFIG_GetAzureHostName(tmpHostName, GCONFIG_MAX_AZURE_HOSTNAME_LEN))
		{
			printf("\r\nCurrent azure host name is %s\r\n", tmpHostName[0] == 0 ? "unspecified" : (char*)tmpHostName);
			printf("\r\nCurrent state: %s\r\n", AZURE_CheckConnectionState());
		}
        return;
	}
	else if (argc > 2)
	{
        return;
	}

	if (argv[1][0] == '-' && strlen(argv[1]) == 1)
	{
		/* Clear host name */
		tmpHostName[0] = '\0';
		GCONFIG_SetAzureHostName(tmpHostName);
	}
	else if (GCONFIG_SetAzureHostName(argv[1]) == 0)
	{
		printf("Fail to set azure host name\r\n");
		return;
	}
}

static void cmd_setAzureAccount(int argc, char **argv)
{
	u8	tmpHostName[GCONFIG_MAX_AZURE_HOSTNAME_LEN];
	u8	tmpHubName[GCONFIG_MAX_AZURE_HUBNAME_LEN];
	u8	tmpDeviceID[GCONFIG_MAX_AZURE_DEVICE_ID_LEN];
	u8	tmpSignedKey[GCONFIG_MAX_AZURE_SIGNED_KEY_LEN];
	u8	tmpExpiryTime[GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN];

	if (argc == 1) /* current setting */
    {
		printf("Usage: aza <hub_name> <device_id> <signed_key> <expiry_time>\r\n");
		printf("Lenght of <hub_name> <= %d bytes\r\n", GCONFIG_MAX_AZURE_HUBNAME_LEN - 1);
		printf("Lenght of <device_id> is 11 bytes, where is serial number\r\n");
		printf("Lenght of <signed_key> <= 64 bytes\r\n");
		printf("Lenght of <expiry_time> = 10 bytes, where is timestamp in UNIX form\r\n");
		if (GCONFIG_GetAzureAccount(tmpHubName, GCONFIG_MAX_AZURE_HUBNAME_LEN,
									tmpDeviceID, GCONFIG_MAX_AZURE_DEVICE_ID_LEN,
									tmpSignedKey, GCONFIG_MAX_AZURE_SIGNED_KEY_LEN,
									tmpExpiryTime, GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN))
		{
			printf("\r\nCurrent azure IoT HUB name is %s\r\n", tmpHubName[0] == 0 ? "unspecified" : (char *)tmpHubName);
			printf("\r\nCurrent azure device ID is %s\r\n", tmpDeviceID[0] == 0 ? "unspecified" : (char *)tmpDeviceID);
			printf("\r\nCurrent azure device signed key is %s\r\n", tmpSignedKey[0] == 0 ? "unspecified" : (char*)tmpSignedKey);
			printf("\r\nCurrent azure device expiry time is %s\r\n", tmpExpiryTime[0] == 0 ? "unspecified" : (char*)tmpExpiryTime);
		}
	}
	else if (argc == 5)
	{
		/* Check length of IoT HUB name */
		if (strlen(argv[1]) >= GCONFIG_MAX_AZURE_HUBNAME_LEN)
		{
			printf("\n\rError <hub_name> length\n\r");
			return;
		}

		/* Check length of device id */
		if (strlen(argv[2]) != (GCONFIG_MAX_AZURE_DEVICE_ID_LEN - 1))
		{
			printf("\n\rError <device_id> length\n\r");
			return;
		}

		/* Check length of signed key */
		if (strlen(argv[3]) >= GCONFIG_MAX_AZURE_SIGNED_KEY_LEN)
		{
			printf("\n\rError <signed_key> length\n\r");
			return;
		}

		/* Check length of expiry time */
		if (strlen(argv[4]) != (GCONFIG_MAX_AZURE_EXPIRY_TIME_LEN - 1))
		{
			printf("\n\rError <expiry_time> length\n\r");
			return;
		}

		/* Set them all into GCONFIG */
		if (GCONFIG_SetAzureAccount(argv[1], argv[2], argv[3], argv[4]) == 0)
		{
			printf("\n\rError length\n\r");
			return;
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
	}
}
#endif

static void cmd_enableDhcpServer(int argc, char **argv)
{
	int iTemp;

	if (argc == 1) /* current setting */
    {
    	printf("Usage: setdhcpsrv <status>\r\n");
    	printf("       <status>: 0: disable     1: enable\r\n");
		printf("\r\nCurrent DHCP server status: %s\r\n", GCONFIG_GetDhcpSrvStatus() ? "enable":"disable");
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);
    if (iTemp > 1)
        return;

    GCONFIG_SetDhcpSrvStatus(iTemp);
}

static void cmd_setNtpServer(int argc, char **argv)
{
    u8 temp8, *ptmp;

    if (argc == 1) /* current setting */
    {
        printf("Usage: ntpsrv <time zone> <ntp server1> <ntp server2> <ntp server3>\r\n");
        printf("       <time zone>\r\n");
	    for (temp8=0; temp8<GCONFIG_MAX_TIME_ZONE; temp8++)
    	{
    		printf("       %d: %s\r\n", temp8, GCONFIG_TimeZoneTable[temp8].Description);
    	}

        printf("\r\nCurrent settings:\r\n");
		for (temp8=0; temp8<GCONFIG_MAX_NTP_SERVERS; temp8++)
		{
			ptmp = GCONFIG_GetNtpSrvHostName(temp8);
			printf("NTP Server Name[%d]: %s\r\n", temp8, (ptmp[0]=='\0') ? "N/A":(char*)ptmp);
		}
        GCONFIG_GetNtpTimeZone(&temp8);
        printf("Time Zone: %s\r\n", GCONFIG_TimeZoneTable[temp8].Description);
        return;
    }
    else if (argc != 5)
        return;

    temp8 = (U8_T)atoi(argv[1]);
    if (temp8 > 24)
        return;
    GCONFIG_SetNtpTimeZone(temp8);

	for (temp8=0; temp8<GCONFIG_MAX_NTP_SERVERS; temp8++)
	{
    	GCONFIG_SetNtpSrvHostName(temp8, argv[temp8+2]);
	}

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
		SNTPC_ReqEnqueue(RTC_NtpQuery, 0);
}

static void cmd_rtcTimeSetup(int argc, char **argv)
{
    u8 temp8, *argP;
    u16 temp16;

    if (argc == 1)
    {
    	printf("Usage: rtcts <mode> \r\n");
    	printf("       <mode>: 0: manual    1: NTP server \r\n");
		printf("\r\nCurrent RTC time setup: %s\r\n", (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP) ? "NTP server":"manual");
        return;
    }
    else if (argc != 2)
        return;

    argP = (u8*)argv[1];
    if ((strlen(argP) == 1) && IsDigit(*argP))
        temp8 = *argP - 0x30;
    else
        return;

    if (temp8 > 1)
        return;

    temp16 = GCONFIG_GetNetwork();
    if (temp8)
        temp16 |= GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;
    else
        temp16 &= ~GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP;
    GCONFIG_SetNetwork(temp16);

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
		SNTPC_ReqEnqueue(RTC_NtpQuery, 0);
}

static void cmd_currentTime(int argc, char **argv)
{
    u8 temp8;
	RTC_TIME timeNow;

    if (argc == 1)
    {
        if (RTC_GetCurrTime(&timeNow) == TRUE)
        {
            printf("Current Time: %d:%d:%d\r\n", timeNow.Hour, timeNow.Minute, timeNow.Second);
        }
        else
            printf("Failed to get current time\r\n");

        return;
    }
    else if (argc != 4)
        return;

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{
        printf("Cannot set time in NTP time setup mode\r\n");
		return;
	}

    if (RTC_GetCurrTime(&timeNow) == FALSE)
    {
        printf("Failed to get current time\r\n");
        return;
    }

    timeNow.Hour = (u8)atoi(argv[1]);
    timeNow.Minute = (u8)atoi(argv[2]);
    timeNow.Second = (u8)atoi(argv[3]);
    if (RTC_SetCurrTime(&timeNow) == FALSE)
        printf("Failed to set current time\r\n");
}

static void cmd_currentDate(int argc, char **argv)
{
    u8 temp8;
    u16 temp16;
    u32 tmp32 = 0;
	RTC_TIME timeNow;

    if (argc == 1)
    {
        if (RTC_GetCurrTime(&timeNow) == TRUE)
            printf("Current Date: %d-%d-%d %s\r\n", timeNow.Year, timeNow.Month, timeNow.Date, GCONFIG_WEEK_TABLE[timeNow.Day]);
        else
            printf("Failed to get current date\r\n");

        return;
    }
    else if (argc != 4)
        return;

    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{
        printf("Cannot set date in NTP time setup mode\r\n");
		return;
	}

    if (RTC_GetCurrTime(&timeNow) == FALSE)
    {
        printf("Failed to get current date\r\n");
        return;
    }

    /* year */
    temp16 = (u16)atoi(argv[1]);
    if (temp16 > 2099 || temp16 < 2000)
        return;
    timeNow.Year = temp16;
    /* month */
    temp8 = (u8)atoi(argv[2]);
    if (temp8 > 12 || temp8 == 0)
        return;
    timeNow.Month = temp8;
    /* date */
    temp8 = (u8)atoi(argv[3]);
    if (temp8 > 31 || temp8 == 0)
        return;

    /* leap year */
    temp16 = (temp16 % 4);
    if (timeNow.Month == 2)
    {
        if (!temp16 && temp8 > 29)
            return;
        else if (temp16 && temp8 > 28)
            return;
    }
    else if (timeNow.Month < 8)
    {
        if (!(timeNow.Month&0x1) && temp8 > 30)
            return;
    }
    else if (timeNow.Month > 7 && (timeNow.Month&0x1) && temp8 > 30)
        return;

    timeNow.Date = temp8;

    if (RTC_SetCurrTime(&timeNow) == FALSE)
        printf("Failed to set current date\r\n");
}

static void cmd_getTemperatureHumidity(int argc, char **argv)
{
	float fval;

    if (argc == 1)
    {
       	if (THS_GetTemperature(&fval) == TRUE)
           	printf("Current temperature: %.2fdegC\r\n", fval);
       	else
           	printf("Failed to get current temperature\r\n");

       	if (THS_GetRelativeHumidity(&fval) == TRUE)
           	printf("Current relative humidity: %.2f%%\r\n", fval);
       	else
           	printf("Failed to get current relative humidity\r\n");
    }
}

static void cmd_setDefaultSettings(int argc, char **argv)
{
	GCONFIG_ReadDefaultConfigData();
	GCONFIG_WriteConfigData();
	cmd_systemReboot(0, 0);
}

static void cmd_setDnsIp(int argc, char **argv)
{
	u32 addr;

	if (argc == 1) /* current setting */
	{
		addr = GCONFIG_GetDNS();
		printf("DNS IP: %d.%d.%d.%d\r\n", (u8)((addr>>24)&0x000000FF), (u8)((addr>>16)&0x000000FF),
				(u8)((addr>>8)&0x000000FF), (u8)(addr&0x000000FF));
		return;
	}
	else if (argc > 2)
		return;

	if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) > 0)
	{
		if (validIp(addr))
			GCONFIG_SetDNS(addr);
	}
}

static void cmd_setServerPort(int argc, char **argv)
{
	int iTemp;

    if (argc == 1) /* current setting */
    {
		printf("Usage: setsrvport <port>\r\n");
        printf("Server Port: %d\r\n", GCONFIG_GetDeviceDataPktListenPort());
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);

    if (iTemp < 1024 || iTemp > 65535)
        return;

    GCONFIG_SetDeviceDataPktListenPort((u16)iTemp);
}

static void cmd_setDestPort(int argc, char **argv)
{
	int iTemp;

    if (argc == 1) /* current setting */
    {
		printf("Usage: setdstport <port>\r\n");
        printf("Client Dest Port: %d\r\n", GCONFIG_GetClientDestPort());
        return;
    }
    else if (argc > 2)
        return;

    iTemp = atoi(argv[1]);

    if (iTemp < 1024 || iTemp > 65535)
        return;

    GCONFIG_SetClientDestPort((u16)iTemp);
}

static void cmd_setDestHostName(int argc, char **argv)
{
	u32 addr;

    if (argc == 1) /* current setting */
    {
		GCONFIG_GetDestHostName((U8_T*)&StrBuf[0]);
		printf("Usage: setdsthn <IP/hostname>\r\n");
 		printf("Current destination host name: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;

    if (checkIpInput(argv[1]))
    {
        if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
            return;

        if (validIp(addr) == 0)
            return;
    }

    GCONFIG_SetDestHostName(argv[1], strlen(argv[1]));
}

static void cmd_SetEmailServerAddr(int argc, char **argv)
{
    u32 addr;

    if (argc == 1) /* current setting */
    {
		GCONFIG_GetSMTPDomainName((u8*)StrBuf);
 		printf("Current e-mail server address: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;

    if (checkIpInput(argv[1]))
    {
        if (getIp2Ulong(argv[1], strlen(argv[1]), &addr) < 0)
            return;

        if (validIp(addr) == 0)
            return;
    }

    GCONFIG_SetSMTPDomainName(argv[1], strlen(argv[1]));
}

static void cmd_SetEmailFromAddr(int argc, char **argv)
{
    if (argc == 1) /* current setting */
    {
		GCONFIG_GetSMTPFrom((U8_T*)StrBuf);
 		printf("Current e-mail from address: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;

	GCONFIG_SetSMTPFrom(argv[1], strlen(argv[1]));
}

static void cmd_SetEmailTo1Addr(int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		GCONFIG_GetSMTPTo1((u8*)StrBuf);
 		printf("Current e-mail to1 address: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;
	GCONFIG_SetSMTPTo1(argv[1], strlen(argv[1]));
}

static void cmd_SetEmailTo2Addr(int argc, char **argv)
{
    if (argc == 1) /* current setting */
    {
		GCONFIG_GetSMTPTo2((u8*)StrBuf);
 		printf("Current e-mail to2 address: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;
	GCONFIG_SetSMTPTo2(argv[1], strlen(argv[1]));
}

static void cmd_SetEmailTo3Addr(int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
		GCONFIG_GetSMTPTo3((u8*)StrBuf);
 		printf("Current e-mail to3 address: %s\r\n", StrBuf);
        return;
    }
    else if (argc > 2)
        return;
	GCONFIG_SetSMTPTo3(argv[1], strlen(argv[1]));
}

static void cmd_SetSMTPSecurity(int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
 		printf("Usage: setemsc SecurityType PortNumber\r\n");
		printf("SecurityType:\r\n");
		printf("  0=No security\r\n");
		printf("  1=SSL\r\n");
		printf("  2=TLS\r\n");
		printf("  3=Auto\r\n");
		printf("PortNumber:\r\n");
		printf("  25 or 587 for regular transfer port\r\n");
		printf("  465 for SSL port\r\n");
        return;
    }
    else if (argc != 3)
        return;
	GCONFIG_SetSMTPSecurity((u8)atoi(argv[1]), atoi(argv[2]));
}

static void cmd_SetSMTPAccount(int argc, char **argv)
{
	if (argc == 1) /* current setting */
    {
 		printf("Usage: setemac UserName PassWord\r\n");
        return;
    }
    else if (argc != 3)
        return;
	GCONFIG_SetSMTPAccount(argv[1], argv[2]);
}

static void cmd_GetEmailConfig(int argc, char **argv)
{
	u8 *ptmp;

    if (argc == 1)
    {
 		printf("Current E-mail Settings\r\n");
		GCONFIG_GetSMTPDomainName((u8*)StrBuf);
 		printf("Mail Server address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPFrom((u8*)StrBuf);
 		printf("Mailbox from address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo1((u8*)StrBuf);
 		printf("Mailbox To1 address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo2((u8*)StrBuf);
 		printf("Mailbox To2 address: %s\r\n", StrBuf);
		GCONFIG_GetSMTPTo3((u8*)StrBuf);
 		printf("Mailbox To3 address: %s\r\n", StrBuf);

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
 		printf("SMTP Security: %s, port=%d\r\n", ptmp, *((u16*)StrBuf+2));

		GCONFIG_GetSMTPAccount(StrBuf, GCONFIG_MAX_MAILADDR_LEN, StrBuf+GCONFIG_MAX_MAILADDR_LEN, GCONFIG_MAX_MAILADDR_LEN);
		printf("Username/Password: %s/%s\r\n", StrBuf, StrBuf+GCONFIG_MAX_MAILADDR_LEN);
    }
    return;
}

static void cmd_SetAWConfig(int argc, char **argv)
{
	int val;
    u16 temp16;

    temp16 = GCONFIG_GetAutoWarning();
    if (argc == 1) /* current setting */
    {
    	printf("Usage: setaw <cold start> <authentication fail> <ip changed> <password changed>\r\n");
    	printf("       <cold start>:             0: Disable     1: Enable\r\n");
    	printf("       <authentication fail>:    0: Disable     1: Enable\r\n");
    	printf("       <ip changed>:             0: Disable     1: Enable\r\n");
    	printf("       <password changed>:       0: Disable     1: Enable\r\n");

		printf("\r\nCurrent auto warning settings:\r\n");
		printf("  cold start:           %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_COLDSTART) ? "Enable":"Disable");
		printf("  authentication fail:  %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_AUTH_FAIL) ? "Enable":"Disable");
		printf("  ip changed:           %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_IP_CHANGED) ? "Enable":"Disable");
		printf("  password changed:     %s\r\n", (temp16 & GCONFIG_SMTP_EVENT_PSW_CHANGED) ? "Enable":"Disable");
        return;
    }
    else if (argc != 5)
        return;

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
}

static void cmd_R2wMode(int argc, char **argv)
{
	int val;
	u16 temp16;

	temp16 = GCONFIG_GetNetwork();
	if (argc == 1)
	{
		printf("Usage: r2wmode <mode> <maxConns>\r\n");
		printf("       <mode>\r\n");
		printf("       0: Socket\r\n");
		printf("       1: VCOM\r\n");
		printf("       2: RFC2217\r\n");
		printf("       3: Modbus Gateway\r\n");
		printf("       <maxConns>\r\n");
		printf("       Available value range is 1~4.\r\n");
		printf("       Note this parameter is effective only for TCP server under Socket or VCOM mode.\r\n");
		printf("\r\nCurrent R2W mode: %d, Server maximum connections: %d\r\n", (temp16 & GCONFIG_NETWORK_R2WMODEMASK), GCONFIG_GetR2wMaxConnections());
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
			printf("Invalid mode value input\r\n");

		if (argc == 3)
		{
			val = atoi(argv[2]);
			if(GCONFIG_SetR2wMaxConnections((u8)val) < 0)
				printf("Invalid maxConns value input\r\n");
		}
	}
}

static void cmd_JtagOff(int argc, char **argv)
{
	int val;

	val = GCONFIG_GetJtagOff();
	if (argc == 1)
	{
		printf("Usage: jtagoff <status>\r\n");
    	printf("       <status>: 0: enable     1: disable\r\n");
		printf("\r\nCurrent jtag status: %s\r\n", val ? "disable":"enable");
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
			printf("Invalid value input\r\n");
		}
	}
}

static void cmd_GetOTAName(int argc, char **argv)
{
	GCONFIG_GetFilename(&StrBuf[0], GCONFIG_MAX_FILENAME_LEN);
	printf("\n\rOTA name for firmware upgrade: %s\n\r", (char *)StrBuf);
}

static void cmd_SetCountryId(int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
		printf("Usage: countryid <id>\r\n");
    	printf("       <id>:\r\n");
		printf("       %d=World wide 13(2G_WORLD: 1~13)\r\n", RTW_COUNTRY_WORLD1);
		printf("       %d=Europe(2G_ETSI1: 1~13)\r\n", RTW_COUNTRY_ETSI1);
		printf("       %d=United States(2G_FCC1: 1~11)\r\n", RTW_COUNTRY_FCC1);
		printf("       %d=Japan(2G_MKK1: 1~14)\r\n", RTW_COUNTRY_MKK1);
		printf("       %d=France(2G_ETSI2: 10~13)\r\n", RTW_COUNTRY_ETSI2);
		printf("       %d=United States(2G_FCC2: 1~13)\r\n", RTW_COUNTRY_FCC2);
		printf("       %d=Japan(2G_MKK2: 1~13)\r\n", RTW_COUNTRY_MKK2);
		printf("       Otherwise=Unavailable\r\n");

		printf("\r\nCurrent country id: %d\r\n", GCONFIG_GetWifiCountryId());
	}
	else if (argc == 2)
	{
		val = atoi(argv[1]);
		GCONFIG_SetWifiCountryId((u8)val);
	}
}

static void cmd_SetEtherTimer(int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
    	printf("Usage: transmitimer <time>\r\n");
    	printf("       <time>: time in ms, available value range is 10~65535 ms\r\n");

		printf("\r\nCurrent transmitimer: %d\r\n", GCONFIG_GetEthernetTxTimer());
	}
    else if (argc > 2)
        return;

	val = atoi(argv[1]);
    if (val < 10 || val > 65535)
        return;

    GCONFIG_SetEthernetTxTimer((u16)val);
}

static void cmd_ConnectStatus(int argc, char **argv)
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
		printf("\r\nCurrent r2w connection status: %s\r\n", is_conn ? "Connected":"Idle");
	}
}

static void cmd_urCounter(int argc, char **argv)
{
	u8 i, maxConns;
	u32 rxCounter, txCounter;
	u16 network;

	printf("Uart discard bytes: %d\r\n", GUART_GetUrDiscardBytes(&urObj));
	printf("Uart Rx available bytes: %u, Tx empty bytes: %u, msr=0x%02x\r\n", GUART_GetRcvrAvailCount(), GUART_GetXmitEmptyCount(), serial_raed_msr(&urObj));
	network = GCONFIG_GetNetwork();
	maxConns = (network & GCONFIG_NETWORK_CLIENT) ? 1:GCONFIG_GetR2wMaxConnections();

	if (network & GCONFIG_NETWORK_PROTO_TCP)
	{
		for (i=0; i<maxConns; i++)
		{
			printf("TCP connection[%d]: status=%s, tx bytes=%u, rx bytes=%u\r\n",
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
			printf("UDP connection[%d]: status=%s, tx bytes=%u, rx bytes=%u\r\n",
				   i,
				   (GUDPDAT_GetConnStatus(i)==GUDPDAT_STATE_CONNECTED) ? "Connected":"Disconnected",
				   GUDPDAT_GetUrRxBytes(i),
				   GUDPDAT_GetUrTxBytes(i));
		}
	}
	TNCOM_GetRxTxCounter(&rxCounter, &txCounter);
	printf("tnCom rx bytes: %d\r\n", rxCounter);
	printf("tnCom tx bytes: %d\r\n", txCounter);
}

static void cmd_setRs485Status(int argc, char **argv)
{
	int val;

	if (argc == 1)
	{
		printf("Usage: rs485 <status>\r\n");
    	printf("       <status>: 0: enable     1: disable\r\n");
		printf("\r\nCurrent RS485 status: %s\r\n", GCONFIG_GetRs485Status() ? "enable":"disable");
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
			printf("Invalid value input\r\n");
		}
	}
}

#if CONFIG_SMART_PLUG
static void cmd_SmartPlug(int argc, char **argv)
{
	unsigned int	val;

	if (pl7223_IsReadyNow())
	{
		if (argc == 3)
		{
			if (memcmp(argv[1], "relay", 5) == 0)
			{
				val = atoi(argv[2]);
				if (val <= 1)
				{
					printf("Turn relay %s\n\r", val ? "on" : "off");
					pl7223_RelayControl(val);
					return;
				}
			}
		}
		else if (argc == 2)
		{
			if (memcmp(argv[1], "info", 4) == 0)
			{
				pl7223_Masurement();
				return;
			}
		}
		printf("Usage: smartplug <option> <param>\r\n");
   		printf("       <option>: relay, info\r\n");
		printf("       <param>: 0 = relay off, 1 = relay on\r\n");
	}
	else
	{
		printf("PL7223 init failed or not ready now\r\n");
	}
}
#endif

static void cmd_setClientConnectionMode(int argc, char **argv)
{
	int	val;

	if (argc == 1)
	{
		printf("Usage: clientconnmode <mode>\r\n");
   		printf("       <mode>: 0 = Auto connection mode, 1 = Manual connection mode\r\n");
		printf("Current setting is %s connection mode\r\n", GCONFIG_GetClientConnectionMode()?"Manual":"Auto");
		return;
	}
	else if (argc == 2)
	{
		val = atoi((const char *)argv[1]);
		GCONFIG_SetClientConnectionMode(val?GCONFIG_MANUAL_CONNECTION:GCONFIG_AUTO_CONNECTION);
		printf("Set %s connection mode OK\r\n", GCONFIG_GetClientConnectionMode()?"Manual":"Auto");
		return;
	}
	printf("Error\r\n");
}

static void cmd_ClientConnect(int argc, char **argv)
{
	struct ip_addr iHostAddr;
	u32	DestIp;
	u16 DestPort;
	int ret;

	if (argc == 1)
	{
		printf("Usage: clientconn <ip/hostname> <port>, establish a new connection with remote IP/Hostname:Port\r\n");
		return;
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
			printf("Fail to use command! please enable both client mode and manual connection mode first.\r\n");
			return;
		}

		/* Get Destination IP address */
		DestIp = htonl(GCONFIG_IpAddr2Ulong(argv[1], strlen(argv[1])));
		if (DestIp == 0xFFFFFFFF) // Not a valid IP address
		{
			/* Execute a DNS query or get the host IP */
			if (netconn_gethostbyname(argv[1], &iHostAddr) != ERR_OK)
			{
				printf("Unresolved host name\r\n");
				return;
			}
			DestIp = htonl(iHostAddr.addr);
		}

		/* Get Destination port number */
		DestPort = (u16)atoi(argv[2]);

		/* Establish connection */
		printf("Connect to %s:%d...", argv[1], DestPort);
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
			ret = GTCPDAT_Connect(0, DestIp, DestPort);
		else
 			ret = GUDPDAT_Connect(0, DestIp, DestPort);

		printf("%s.\r\n", (ret < 0)?"Fail":"Success");
		return;
	}
	printf("Error\r\n");
}

static void cmd_ClientClose(int argc, char **argv)
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
			printf("Fail to use command! please enable both client mode and manual connection mode first.\r\n");
			return;
		}

		printf("Close connection ");
		if (GS2W_GetConnType() == GS2W_CONN_TCP)
		{
			ret = GTCPDAT_Close(0);
		}
		else
		{
			ret = GUDPDAT_Close(0);
		}

		printf("%s.\r\n", (ret < 0) ? "Fail" : "Success");
		return;
	}
	printf("Error\r\n");
}

static void cmd_setDeviceName(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("Usage: setdevname <host name | IP address>\r\n");
		printf("Current device name: %s\r\n", GCONFIG_GetDeviceName());
		return;
	}
	GCONFIG_SetDeviceName(argv[1]);
}

#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
static void cmd_setNetworkBackup(int argc, char **argv)
{
	int	val;

	if (argc == 1)
	{
		printf("Usage: setnetbackup <mode>\r\n");
		printf("<mode>: 0 = Force Wifi, 1 = Force Ethernet, 2 = Wifi as major, 3 = Ethernet as major\r\n");
		printf("Current network backup mode: %s\r\n",
			   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_FORCE_WIFI) ? "Force Wifi" :
			   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_FORCE_ETH) ? "Force Ethernet" :
			   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_WIFI_AS_MAJOR) ? "Wifi as major" :
			   (GCONFIG_GetNetworkBackupMode() == GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR) ? "Ethernet as major" : "Unknow");
		return;
	}

	val = atoi(argv[1]);
	if ((val < GCONFIG_NETWORK_BACKUP_FORCE_WIFI) || (val > GCONFIG_NETWORK_BACKUP_ETH_AS_MAJOR))
	{
		printf("<mode> error\r\n");
		return;
	}

	GCONFIG_SetNetworkBackupMode(val);
}
#endif

int asciiString2HexBytes(char *pAscii, int lengthAscii, unsigned char *pHex, int lengthHex)
{
	int		i;
	bool	odd;
	char	c;
	
	if (lengthAscii > (lengthHex * 2))
	{
		return -1;
	}
	
	if (lengthAscii % 2)
	{
		odd = TRUE;
	}
	else
	{
		odd = FALSE;
	}
	
	for (i = 0; i < lengthAscii; i++)
	{
		c = *pAscii;
		if (c >= 0x30 && c <= 0x39)
		{
			c -= 0x30;
		}
		else if (c >= 0x41 && c <= 0x46)
		{
			c -= 0x37;
		}
		else if (c >= 0x61 && c <= 0x66)
		{
			c -= 0x57;
		}
		else
		{
			return -1;
		}
		
		if (odd)
		{
			if (i % 2)
			{
				*pHex = (c << 4);
			}
			else
			{
				if (i == 0)
				{
					*pHex = c;
				}
				else
				{
					*pHex |= c;
				}
				pHex++;
			}
		}
		else
		{
			if (i % 2)
			{
				*pHex |= c;
				pHex++;
			}
			else
			{
				*pHex = (c << 4);
			}
		}
		pAscii++;
	}
	
	return (odd ? ((lengthAscii + 1) / 2) : (lengthAscii / 2));
}

static void cmd_setAdaptivityThreshold(int argc, char **argv)
{
	unsigned char threshold;
	
	if (argc != 2)
	{
		printf("Usage: setadapthres <threshold>\r\n");
		printf("<threshold>: 00 ~ ff\r\n");
		printf("Threshold sugestion for Europe: f1 ~ f5\r\n");
		printf("Threshold sugestion for Japan: 06 ~ 0a\r\n");
		printf("Current adaptivity threshold: 0x%02x\r\n", GCONFIG_GetAdaptivityThreshold());
		return;
	}
	
	if (asciiString2HexBytes(argv[1], strlen(argv[1]), &threshold, 1) < 0)
	{
		printf("<threshold> error\r\n");
		return;
	}
	
	GCONFIG_SetAdaptivityThreshold(threshold);
}

#if CONFIG_INTERACTIVE_EXT
/* must include here, ext_cmd_table in wifi_interactive_ext.h uses struct cmd_entry */
#include <wifi_interactive_ext.h>
#endif

static void cmd_help(int argc, char **argv)
{
	int i;

	printf("\n\rCOMMAND LIST:");
	printf("\n\r==============================");

	for(i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i ++)
		printf("\n\r    %s", cmd_table[i].command);
#if CONFIG_INTERACTIVE_EXT
	for(i = 0; i < sizeof(ext_cmd_table) / sizeof(ext_cmd_table[0]); i ++)
		printf("\n\r    %s", ext_cmd_table[i].command);
#endif
}

#define MAX_ARGC	6

static int parse_cmd(char *buf, char **argv)
{
	int argc = 0;

	memset(argv, 0, sizeof(argv) * MAX_ARGC);
	while((argc < MAX_ARGC) && (*buf != '\0')) {
		argv[argc] = buf;
		argc ++;
		buf ++;

		while((*buf != ' ') && (*buf != '\0'))
			buf ++;

		while(*buf == ' ') {
			*buf = '\0';
			buf ++;
		}
		// Don't replace space
		if(argc == 1){
			if(strcmp(argv[0], "iwpriv") == 0){
				if(*buf != '\0'){
					argv[1] = buf;
					argc ++;
				}
				break;
			}
		}
	}

	return argc;
}

char uart_buf[LOG_SERVICE_BUFLEN];
int interactive_mode(void *param)
{
	int i, argc, ret = 1;
	char *argv[MAX_ARGC];
//	extern xSemaphoreHandle	uart_rx_interrupt_sema;

//	printf("\n\rEnter INTERACTIVE MODE\n\r");
//	printf("\n\r# ");

//	while(1){
//		while(xSemaphoreTake(uart_rx_interrupt_sema, portMAX_DELAY) != pdTRUE);

//		if((argc = parse_cmd(uart_buf, argv)) > 0) {
		if((argc = parse_cmd((char *)param, argv)) > 0) {
			int found = 0;

			for(i = 0; i < sizeof(cmd_table) / sizeof(cmd_table[0]); i ++) {
				if(strcmp((const char *)argv[0], (const char *)(cmd_table[i].command)) == 0) {
					cmd_table[i].function(argc, argv);
					found = 1;
					break;
				}
			}
#if CONFIG_INTERACTIVE_EXT
			if(!found) {
				for(i = 0; i < sizeof(ext_cmd_table) / sizeof(ext_cmd_table[0]); i ++) {
					if(strcmp(argv[0], ext_cmd_table[i].command) == 0) {
						ext_cmd_table[i].function(argc, argv);
						found = 1;
						break;
					}
				}
			}
#endif
			if(!found)
			{
				ret = -1;
//				printf("\n\runknown command '%s'", argv[0]);
			}
//			printf("\n\r[MEM] After do cmd, available heap %d\n\r", xPortGetFreeHeapSize());
		}

//		printf("\r\n# ");
//		uart_buf[0] = '\0';
		return ret;
//	}
}

void start_interactive_mode(void)
{
#ifdef SERIAL_DEBUG_RX
	if(xTaskCreate(interactive_mode, (char const *)"interactive_mode", STACKSIZE, NULL, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
#else
	printf("\n\rERROR: No SERIAL_DEBUG_RX to support interactive mode!");
#endif
}

#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
VOID WlanNormal( IN  u16 argc, IN  u8  *argv[])
{
	u8 i, j= 0;
	u8*	pbuf = (u8*)uart_buf;
	extern xSemaphoreHandle	uart_rx_interrupt_sema;

	memset(uart_buf, 0, sizeof(uart_buf));

	printf("argc=%d\n", argc);
	for(i = 0 ;  i < argc ; i++)
	{
		printf("command element [%d] = %s\n", i, argv[i]);
		for(j = 0; j<strlen((char const*)argv[i]) ;j++)
		{
			*pbuf = argv[i][j];
			pbuf++;
		}
		if(i < (argc-1))
		{
			*pbuf = ' ';
			pbuf++;
		}
	}
	printf("command %s\n", uart_buf);

	xSemaphoreGive( uart_rx_interrupt_sema);

}
#endif


