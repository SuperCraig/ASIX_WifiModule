/*
 *********************************************************************************
 *     Copyright (c) 2015	ASIX Electronic Corporation      All rights reserved.
 *
 *     This is unpublished proprietary source code of ASIX Electronic Corporation
 *
 *     The copyright notice above does not evidence any actual or intended
 *     publication of such source code.
 *********************************************************************************
 */
 /*============================================================================
 * Module name: misc.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __MISC_H__
#define __MISC_H__

/* INCLUDE FILE DECLARATIONS */
#include "lwip_netconf.h"

/* NAMING CONSTANT DECLARATIONS */
#define DEFAULT_WIFI_DISCONN_CHECK_TIMEOUT	5//(unit in 1 sec)
#define DEFAULT_WIFI_AUTOCONN_INTERVAL		10//(unit in 1 sec)
#define DEFAULT_IMAGE_RELOAD_TIMEOUT		15000//15 sec(unit in 1 msec)
#define DEFAULT_IMAGE_RELOAD_BUTTON			PE_3
#define DEFAULT_IMAGE_RELOAD_INDICATOR_LED	PE_0
   
typedef enum {
  	NETWORK_SWITCH_IDLE = 0,
	NETWORK_SWITCH_TO_WIFI_IF,
	NETWORK_SWITCH_TO_ETH_IF,
} NETWORK_BACKUP_SWITCH_EVENTS, NETWORK_BACKUP_SWITCH_STATUS;

typedef enum {
	DHCPC_ADDRESS_ASSIGNED = 0,
	DHCPC_SET_STATIC,
	DHCPC_RELEASE_IP,
	DHCPC_TIMEOUT,
	DHCPC_ERROR,
	DHCPC_STOP,
  	DHCPC_START,
  	DHCPC_ENFORCED_START,
	DHCPC_WAIT_ADDRESS,
} DHCP_CLIENT_STATE;

typedef enum {
	IF_ID_WIFI = 0,
	IF_ID_WIFI_CONCURRENT,
	IF_ID_ETH,
} NETWORK_BACKUP_PHYSICAL_INTERFACE_ID;

typedef enum {
	DHCPC_REQUEST_IDLE = 0,
	DHCPC_REQUEST_ETH_START,
	DHCPC_REQUEST_WIFI_START,
} NETWORK_BACKUP_DHCP_CLIENT_REQUEST;

typedef enum {
	IF_IP_STATE_NONE = 0,
	IF_IP_STATE_STATIC,
	IF_IP_STATE_DHCP,
	IF_IP_STATE_DHCP_FAIL,	
} NETWORK_BACKUP_INTERFACE_IP_STATE;

typedef enum {
	IF_STATE_NOT_READY = 0,
	IF_STATE_LINK_READY,
	IF_STATE_IP_READY,
} NETWORK_BACKUP_INTERFACE_READY_STATE;

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES */

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
NETWORK_BACKUP_SWITCH_STATUS MISC_GetNetworkBackupSwitchStatus(void);
//void MISC_BackupNetworkDhcpStart(void);
//void MISC_NetworkBackupSwitch(u8 enable, u8 backupNetId);
void MISC_SetNetworkBackupMode(u8 mode);
void MISC_SetNetworkBackupInterfaceIPState(u8 PhysicalNetIfId, u8 state);
int MISC_GetNetworkBackupInterfaceIPState(u8 PhysicalNetIfId);
void MISC_DefaultImageReloadCheck(u16 ReloadTimeout, u8 ReloadButton, u8 IndicateLED);
u8 MISC_GetSerialNum(u8 *pdst, u8 dstlen);
void MISC_EnterWiFiBusySection(void);
void MISC_LeaveWiFiBusySection(void);
u32 MISC_GetCurrentIPAddr(void);
u8 MISC_LockFwUpgradeSection(void);
void MISC_UnLockFwUpgradeSection(void);
u8 MISC_DHCP(u8 PhysicalNetIfId, u8 Ctrl, u8 BlockMode);
void MISC_SetWifiAutoConnInterval(u16 sec);
void MISC_SetWifiDisconnTimeout(u16 sec);
void MISC_GetMacAddr(u8 *pdst);
void MISC_SetWifiAutoConn(u8 Enable);
u8 MISC_GetWifiAutoConn(void);
int MISC_GetRandom(void *p_rng, unsigned char *prng, size_t rnglen);
u8 MISC_StartWifi(u8 network_mode, struct netif *pnetif);
u8 MISC_NetInterfaceReadyCheck(void);
u8 MISC_NetInterfaceIsLinkUp(u8 PhysicalNetIfId);
#endif /* __MISC_H__ */

