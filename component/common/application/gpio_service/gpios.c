/*
*********************************************************************************
*     Copyright (c) 2015   ASIX Electronic Corporation      All rights reserved.
*
*     This is unpublished proprietary source code of ASIX Electronic Corporation
*
*     The copyright notice above does not evidence any actual or intended
*     publication of such source code.
*********************************************************************************
*/
/*============================================================================
 * Module name: gpios.c
 * Purpose: Service related user application in GPIO
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "basic_types.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "gpios.h"
#include "gpio_irq_api.h"
#include "gconfig.h"
#include "os_support.h"
#include "flash_api.h"
#include "wifi_constants.h"
#include "wps/wps_defs.h"
#include "wifi_structures.h"
#include "main.h"
#include "semphr.h"
#include <lwip_netconf.h>

extern void MISC_EnterWiFiBusySection(void);
extern void MISC_LeaveWiFiBusySection(void);

extern xQueueHandle            Global_xQueue_ssid;




#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif
extern const s32 security_map[];

u8 SwitchMemoryState=0; //20170801 Craig
u8 DoneTimes_MBIDefault = 0;       //20170802 Craig
u8 DoneTimes_Cmd23Default = 0;     //20170802 Craig

/* NAMING CONSTANT DECLARATIONS */
enum sc_result {
	SC_ERROR = -1,	/* default error code*/
	SC_NO_CONTROLLER_FOUND = 1, /* cannot get sta(controller) in the air which starts a simple config session */
	SC_CONTROLLER_INFO_PARSE_FAIL, /* cannot parse the sta's info  */
	SC_TARGET_CHANNEL_SCAN_FAIL, /* cannot scan the target channel */
	SC_JOIN_BSS_FAIL, /* fail to connect to target ap */
	SC_DHCP_FAIL, /* fail to get ip address from target ap */
	 /* fail to create udp socket to send info to controller. note that client isolation
		must be turned off in ap. we cannot know if ap has configured this */
	SC_UDP_SOCKET_CREATE_FAIL,
	SC_SUCCESS,	/* default success code */
};

/* GLOBAL VARIABLES DECLARATIONS */
gpio_t			gpio_ledStatus;
#if (CONFIG_CLOUD_SELECT != CLOUD_DISABLE)
gpio_t			gpio_beep;
#endif
gpio_irq_t		gpio_wpsBtn, gpio_swDefaultBtn;
U8_T			wps_fall = 0;
U8_T			wps_trigger = 0;
U8_T			wps_checkLink = 0;
U32_T			wps_fallTick = 0;
enum			sc_result sc_ret;
U8_T			GPIOS_LEDIndicated = 0;
TimerHandle_t	GPIOS_xTimer = NULL;
U8_T			GPIOS_LEDAlternative = 0;
U8_T			swDefault_trigger = 0;
U8_T			swDefault_fall = 0;
U32_T			swDefault_fallTick = 0;
S8_T			GPIOS_SerialNumber[12];
S8_T			*pGPIOS_PinCode=NULL;

typedef struct clientInfo_
{
	int			count;
	rtw_mac_t	mac_list[AP_STA_NUM];
} clientInfo;

clientInfo	GPIOS_ClientInfo;

/* STATIC VARIABLE DECLARATIONS */

/* FUNCTION DECLARATIONS */
void GPIOS_Detector(void *pvParameters);
void GPIOS_WpsISR(uint32_t id, gpio_irq_event event);
void GPIOS_LEDFlashTimerCallback(TimerHandle_t xTimer);
void GPIOS_SwDefaultISR(uint32_t id, gpio_irq_event event);
void GPIOS_PlatformReset(void);
void GPIOS_SaveCurrentWifiConfig(void);
void GPIOS_CheckAndRestoreAPMode(U8_T *everChanged);

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_Init(void)
 * Purpose: Setup GPIO PIN definition/ISR for service and create the task
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_Init(void)
{	
	//if (GCONFIG_GetJtagOff())
        if(JTAGoff_GPIOEnable)          //20170809 Craig
	{
		printf("%sDisable JTAG function\n\r", GPIOS_PRINT_HEADER);
		sys_jtag_off();
		
		/* Init LED status PIN */
		printf("%sInit status LED PIN\n\r", GPIOS_PRINT_HEADER);
		gpio_init(&gpio_ledStatus, PE_0);
		gpio_dir(&gpio_ledStatus, PIN_OUTPUT);
		gpio_mode(&gpio_ledStatus, PullUp);
		gpio_write(&gpio_ledStatus, 1);
#if (CONFIG_CLOUD_SELECT != CLOUD_DISABLE)
		/* Init Beep control PIN */
		printf("%sInit Beep PIN\n\r", GPIOS_PRINT_HEADER);
		gpio_init(&gpio_beep, PE_4);
		gpio_dir(&gpio_beep, PIN_OUTPUT);
		gpio_mode(&gpio_beep, PullUp);
		gpio_write(&gpio_beep, 1);
#endif		
		/* Init WPS pbc or Simple Config PIN */
		printf("%sInit WPS pbc/Simple Config PIN\n\r", GPIOS_PRINT_HEADER);
		gpio_init(&gpio_wpsBtn, PE_1);
		gpio_dir(&gpio_wpsBtn, PIN_INPUT);
		gpio_mode(&gpio_wpsBtn, PullUp);

		/* Init ISR for pbc PIN */
		gpio_irq_init(&gpio_wpsBtn, PE_1, GPIOS_WpsISR, (uint32_t)(&gpio_ledStatus));
		gpio_irq_set(&gpio_wpsBtn, IRQ_FALL, 1);
		gpio_irq_enable(&gpio_wpsBtn);

		/* Init SW default PIN */
		printf("%sInit SW default PIN\n\r", GPIOS_PRINT_HEADER);
		gpio_init(&gpio_swDefaultBtn, PE_3);
		gpio_dir(&gpio_swDefaultBtn, PIN_INPUT);
		gpio_mode(&gpio_swDefaultBtn, PullUp);

		/* Init ISR for SW default PIN */
		gpio_irq_init(&gpio_swDefaultBtn, PE_3, GPIOS_SwDefaultISR, (uint32_t)(&gpio_ledStatus));
		gpio_irq_set(&gpio_swDefaultBtn, IRQ_FALL, 1);
		gpio_irq_enable(&gpio_swDefaultBtn);
		
		/* Get serial number */
		MISC_GetSerialNum((U8_T *)GPIOS_SerialNumber, 12);
		printf("%sSerial number = %s\n\r", GPIOS_PRINT_HEADER, GPIOS_SerialNumber);
		pGPIOS_PinCode = GCONFIG_GetSimpleCfgPinCodeStatus() ? GPIOS_SerialNumber:NULL;
		
		/* Create task for GPIOS detector */
		if(xTaskCreate(GPIOS_Detector, (const char *)"gpios_detector", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		{
			printf("%sCreate task fail!\r\n", GPIOS_PRINT_HEADER);
		}
	}

} /* End of GPIOS_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_ResetLEDIndicator(void)
 * Purpose: GPIO service main program for handling WPS PBC and SW reload default
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_ResetLEDIndicator(void)
{
	if (GPIOS_xTimer)
	{
#ifdef PLATFORM_FREERTOS
		SaveAndCli();
#else
    	SaveAndCli(Flags);
#endif
		xTimerStop(GPIOS_xTimer, 0);
		xTimerDelete(GPIOS_xTimer, 0);
		GPIOS_xTimer = NULL;
#ifdef PLATFORM_FREERTOS
    	RestoreFlags();
#else
		RestoreFlags(Flags);
#endif
		gpio_write(&gpio_ledStatus, 1);
	}
	
	if (GPIOS_LEDIndicated)
	{
		GPIOS_LEDIndicated = 0;
	}
	
} /* End of GPIOS_ResetLEDIndicator() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_Detector(void *pvParameters)
 * Purpose: GPIO service main program for handling WPS PBC and SW reload default
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_Detector(void *pvParameters)
{
	char 			*ssid;        
    char temp_ssid[33]={0};                
    char rcv_ssid[33]={0};
        
	U8_T	mode;
	U32_T	tick;
#ifndef PLATFORM_FREERTOS
	U32_T	Flags;
#endif
	U8_T	apToStationEver = 0;
	S32_T	wpsRet;
    char 	i;
        

	printf("%sStart gpio service success\n\r", GPIOS_PRINT_HEADER);
	portBASE_TYPE xStatus;
        
            

	while (1)
	{
		/* Check WIFI connection and handle LED blinking */
		if (!GPIOS_LEDIndicated)
		{
			if (GCONFIG_GetWifiRfEnable())
			{
				mode = GCONFIG_GetWifiNetworkMode();
				if (mode == RTW_MODE_STA)
				{
					vTaskDelay(100);

					if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) == RTW_SUCCESS)
					{
						gpio_write(&gpio_ledStatus, 0);
					}
					else
					{
						gpio_write(&gpio_ledStatus, 1);
					}
				}
				else if (mode == RTW_MODE_AP)
				{
					if (GPIOS_xTimer = xTimerCreate("LED_timer", GPIOS_AP_MODE_BLINKIN_INTERVAL, pdTRUE, NULL, GPIOS_LEDFlashTimerCallback))
					{
						GPIOS_LEDIndicated = 1;
						xTimerStart(GPIOS_xTimer, 0);
					}
				}
			}
			else
			{
				gpio_write(&gpio_ledStatus, 1);
			}
		}
		
		/* Detect WPS PBC trigger from GPIOE1 */
		if (wps_fall)
		{
			wps_fall = 0;
			
			/* Check AP mode */
			if (GCONFIG_GetWifiNetworkMode() == RTW_MODE_AP)
			{
				GPIOS_ClientInfo.count = AP_STA_NUM;
				wifi_get_associated_client_list(&GPIOS_ClientInfo, sizeof(GPIOS_ClientInfo));
				if (GPIOS_ClientInfo.count)
				{
					printf("\n\r%sAP mode with associated client, abort WPS/Simple_config.\n\r", GPIOS_PRINT_HEADER);
					gpio_write(&gpio_ledStatus, 1);
					continue;
				}
				else
				{
					apToStationEver = 1;
				}
			}
			
			/* Accept WPS/Simple_config trigger */
			wps_trigger = 1;
			
			gpio_init(&gpio_wpsBtn, PE_1);
			gpio_dir(&gpio_wpsBtn, PIN_INPUT);
			gpio_mode(&gpio_wpsBtn, PullNone);
			wps_fallTick = xTaskGetTickCount();
			
			/* Check and init WPS/Simple_config session */
			while (1)
			{
				tick = (xTaskGetTickCount() - wps_fallTick) * portTICK_RATE_MS;
				
				if (gpio_read(&gpio_wpsBtn) == 1)
				{
					if (tick >= GPIOS_SC_TRIGGER_TIME) // Simple config
					{
						printf("\n\r%sStart Simple Configuration.\n\r", GPIOS_PRINT_HEADER);

						MISC_EnterWiFiBusySection();
						GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);
						wifi_enter_promisc_mode();
						
						GPIOS_ResetLEDIndicator();
						GPIOS_LEDIndicated = 1;
						GPIOS_xTimer = xTimerCreate("LED_timer", 100, pdTRUE, NULL, GPIOS_LEDFlashTimerCallback);
					
						if (GPIOS_xTimer)
						{
							xTimerStart(GPIOS_xTimer, 0);
						}
						else
						{
							printf("\n\r%sStart LED timer failed.\n\r", GPIOS_PRINT_HEADER);
							GPIOS_LEDIndicated = 0;
							apToStationEver = 0;
							MISC_LeaveWiFiBusySection();
							goto GPIOS_Detector_Exit;
						}
							
						/* We use serial number as Simple Config PIN */
						if (init_test_data(pGPIOS_PinCode) == 0)														
						{
							filter_add_enable();
							sc_ret = simple_config_test();
							print_simple_config_result(sc_ret);
							remove_filter();
							wps_trigger = 0;
#ifdef PLATFORM_FREERTOS
							SaveAndCli();
#else
							SaveAndCli(Flags);
#endif
							xTimerStop(GPIOS_xTimer, 0);
							xTimerDelete(GPIOS_xTimer, 0);
							GPIOS_xTimer = NULL;
#ifdef PLATFORM_FREERTOS
							RestoreFlags();
#else
							RestoreFlags(Flags);
#endif
							GPIOS_LEDIndicated = 0;
								
							if (sc_ret == SC_SUCCESS)
							{
								/* Auto enable DHCP client and disable DHCP server while switching to station mode */								  
								if (apToStationEver)
								{
									GCONFIG_SetNetwork((GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE));
									GCONFIG_SetDhcpSrvStatus(0);
								}
								gpio_write(&gpio_ledStatus, 0);
								apToStationEver = 0;
								GPIOS_SaveCurrentWifiConfig();
							}
							else
							{
								gpio_write(&gpio_ledStatus, 1);
								GPIOS_CheckAndRestoreAPMode(&apToStationEver);
							}
						}
						else
						{
							wps_trigger = 0;
#ifdef PLATFORM_FREERTOS
							SaveAndCli();
#else
							SaveAndCli(Flags);
#endif
							xTimerStop(GPIOS_xTimer, 0);
							xTimerDelete(GPIOS_xTimer, 0);
							GPIOS_xTimer = NULL;
#ifdef PLATFORM_FREERTOS
							RestoreFlags();
#else
							RestoreFlags(Flags);
#endif
							GPIOS_LEDIndicated = 0;
							gpio_write(&gpio_ledStatus, 1);
							GPIOS_CheckAndRestoreAPMode(&apToStationEver);
						}
						MISC_LeaveWiFiBusySection();
					}
					else if (tick >= GPIOS_WPS_TRIGGER_TIME) // WPS
					{
						printf("\n\r%sStart WPS PBC.\n\r", GPIOS_PRINT_HEADER);
						MISC_EnterWiFiBusySection();
						GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);
						wps_judge_staion_disconnect();
						GPIOS_ResetLEDIndicator();
						GPIOS_LEDIndicated = 1;
						GPIOS_xTimer = xTimerCreate("LED_timer", 333, pdTRUE, NULL, GPIOS_LEDFlashTimerCallback);
						if (GPIOS_xTimer)
						{
							xTimerStart(GPIOS_xTimer, 0);
							wps_checkLink = 1;
						}
						else
						{
							printf("\n\r%sStart LED timer failed.\n\r", GPIOS_PRINT_HEADER);
							GPIOS_LEDIndicated = 0;
							GPIOS_CheckAndRestoreAPMode(&apToStationEver);
							MISC_LeaveWiFiBusySection();
							goto GPIOS_Detector_Exit;
						}
						wpsRet = wps_start(WPS_CONFIG_PUSHBUTTON, NULL, 0, NULL);
					}
					else // Abort
					{
						wps_trigger = 0;
						gpio_write(&gpio_ledStatus, 1);
						gpio_mode(&gpio_wpsBtn, PullUp);
						apToStationEver = 0;
					}
					break;
				}
				vTaskDelay(10);
			}
		}
		
		/* Check WPS STA linked */
		if (wps_checkLink)
		{
#ifdef PLATFORM_FREERTOS
			SaveAndCli();
#else
    		SaveAndCli(Flags);
#endif
			wps_trigger = 0;
			wps_checkLink = 0;
			xTimerStop(GPIOS_xTimer, 0);
			xTimerDelete(GPIOS_xTimer, 0);
			GPIOS_xTimer = NULL;
#ifdef PLATFORM_FREERTOS
    		RestoreFlags();
#else
			RestoreFlags(Flags);
#endif
			GPIOS_LEDIndicated = 0;
			
			if ((wifi_is_ready_to_transceive(RTW_STA_INTERFACE) == RTW_SUCCESS) && (wpsRet == 0))
			{
				/* Auto enable DHCP client and disable DHCP server while switching to station mode */								  
				if (apToStationEver)
				{
					GCONFIG_SetNetwork((GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE));
					GCONFIG_SetDhcpSrvStatus(0);
				}
				printf("WPS: BSS linked.");
				gpio_write(&gpio_ledStatus, 0);
				apToStationEver = 0;
				GPIOS_SaveCurrentWifiConfig();
			}
			else
			{
				printf("WPS: BSS not linked.");
				gpio_write(&gpio_ledStatus, 1);
				GPIOS_CheckAndRestoreAPMode(&apToStationEver);
			}
			MISC_LeaveWiFiBusySection();
		}
		
		/* Detect SW default trigger from GPIOE3 */    //20170728 Craig 
		if (swDefault_fall)
		{
                        
			/* Accept SW default trigger */
			swDefault_trigger = 1;
			swDefault_fall = 0;
			gpio_init(&gpio_swDefaultBtn, PE_3);
                        gpio_dir(&gpio_swDefaultBtn, PIN_INPUT);
                        gpio_mode(&gpio_swDefaultBtn, PullNone);
			swDefault_fallTick = xTaskGetTickCount();
			
			/* Check and init SW default session */

                        while(1){
				tick = (xTaskGetTickCount() - swDefault_fallTick) * portTICK_RATE_MS;
                                
                                if(tick > 3000){
                                  if(tick >= GPIOS_LOAD_GCONFIG_DEFAULT_TRIGGER_TIME/3 && DoneTimes_Cmd23Default<1){  /*20170728 Craig Need to add code here to set default value*/
                                    DoneTimes_Cmd23Default++;
                                    SwitchMemoryState = Cmd23Default;
                                    //GCONFIG_ReadMBIdataFromROM();
                                    //GCONFIG_WriteConfigData();
                                    //printf("Stage1 Clear!!");
                                    //GPIOS_PlatformReset();
                                    printf("Stage1!!");
                                    gpio_write(&gpio_ledStatus, 1);
                                    vTaskDelay(200);
                                    gpio_write(&gpio_ledStatus, 0);
                                  }
                                  if (tick >= GPIOS_LOAD_GCONFIG_DEFAULT_TRIGGER_TIME && DoneTimes_MBIDefault<1)
                                  {
                                      DoneTimes_Cmd23Default=2;
                                      DoneTimes_MBIDefault++;
                                      SwitchMemoryState = MBIDefault;
					/* Reload GCONFIG default */
					printf("\n\r%sLoad SW default.\n\r", GPIOS_PRINT_HEADER);
					//GCONFIG_ReadDefaultConfigData();
					//GCONFIG_WriteConfigData();
                                        //GPIOS_PlatformReset();
                                      printf("Stage2!!");
                                      gpio_write(&gpio_ledStatus, 1);
                                      vTaskDelay(200);
                                      gpio_write(&gpio_ledStatus, 0);
                                      vTaskDelay(200);
                                      gpio_write(&gpio_ledStatus, 1);
                                      vTaskDelay(200);
                                      gpio_write(&gpio_ledStatus, 0);
                                  }
                                }
				else if (gpio_read(&gpio_swDefaultBtn) == 1)
				{
					swDefault_trigger = 0;
					gpio_write(&gpio_ledStatus, 1);
					gpio_mode(&gpio_swDefaultBtn, PullUp);
					break;
				}
				
				vTaskDelay(10);
			}
		}

		vTaskDelay(100);
	}
GPIOS_Detector_Exit:
	vTaskDelete(NULL);

} /* End of GPIOS_Detector() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_WpsISR(uint32_t id, gpio_irq_event event)
 * Purpose: WPS PBC button interrupt service routine
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_WpsISR(uint32_t id, gpio_irq_event event)
{
	gpio_t	*gpio_led;

	if (wps_fall || wps_trigger || wps_checkLink)
	{
		return;
	}
	gpio_led = (gpio_t *)id;
	gpio_write(gpio_led, 0);
	wps_fall = 1;

} /* End of GPIOS_WpsISR() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_LEDFlashTimerCallback(TimerHandle_t xTimer)
 * Purpose: Timer callback for flashing the LED
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_LEDFlashTimerCallback(TimerHandle_t xTimer)
{
	if (GPIOS_LEDAlternative)
	{
		GPIOS_LEDAlternative = 0;
		gpio_write(&gpio_ledStatus, 1);
	}
	else
	{
		GPIOS_LEDAlternative = 1;
		gpio_write(&gpio_ledStatus, 0);
	}

} /* End of GPIOS_LEDFlashTimerCallback() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_SwDefaultISR(uint32_t id, gpio_irq_event event)
 * Purpose: SW reload default button interrupt service routine
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_SwDefaultISR(uint32_t id, gpio_irq_event event)
{
	gpio_t	*gpio_led;
        
        if(SwitchMemoryState == Cmd23Default && DoneTimes_Cmd23Default < 2){
          printf("Cmd23Default Run");   //MBI default
          SwitchMemoryState = 0;
          GCONFIG_ReadMBIdataFromROM();
          GCONFIG_WriteConfigData();
          GPIOS_PlatformReset();
        }
        if(SwitchMemoryState == MBIDefault && DoneTimes_MBIDefault > 0){
          printf("MBIDefault Run");    //asix default
          SwitchMemoryState = 0;
          GCONFIG_ReadDefaultConfigData();
          GCONFIG_WriteConfigData();
          GPIOS_PlatformReset();
        }
         
	if (swDefault_fall || swDefault_trigger)
	{       
		return;
	}
	gpio_led = (gpio_t *)id;
	gpio_write(gpio_led, 0);
	swDefault_fall = 1;
        
} /* End of GPIOS_SwDefaultISR() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_PlatformReset(void)
 * Purpose: Reset the platform
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_PlatformReset(void)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);

} /* End of GPIOS_PlatformReset() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_SaveCurrentWifiConfig(void)
 * Purpose: Get current Wifi config and save it to GCONFIG
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_SaveCurrentWifiConfig(void)
{
	rtw_wifi_setting_t	setting;
	U16_T				len;
	
	/* Get current Wifi config of driver */
	wifi_get_setting(WLAN0_NAME, &setting);
	
	/* Save them to our GCONFIG */
	GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);
	GCONFIG_SetWifiSsid((U8_T *)setting.ssid, strlen(setting.ssid));
	switch (setting.security_type)
	{
		case RTW_SECURITY_OPEN:
			GCONFIG_SetWifiEncryptMode(RTW_SECURITY_OPEN);
			break;
		case RTW_SECURITY_WEP_PSK:
		case RTW_SECURITY_WEP_SHARED:
			GCONFIG_SetWifiEncryptMode(setting.security_type);
			len = strlen(setting.password);
			if (len == 5)
			{
				GCONFIG_SetWifiWepKeyLength(0);
				GCONFIG_SetWifiWep64Key(setting.key_idx, setting.password);
				GCONFIG_SetWifiWepKeyIndex(setting.key_idx);
			}
			else if (len == 13)
			{
				GCONFIG_SetWifiWepKeyLength(1);
				GCONFIG_SetWifiWep128Key(setting.key_idx, setting.password);
				GCONFIG_SetWifiWepKeyIndex(setting.key_idx);
			}
			else
			{
				printf("\n\r%sUnknow WEP password length!\n\r", GPIOS_PRINT_HEADER);
				return;
			}
			break;
		case RTW_SECURITY_WPA_TKIP_PSK:
		case RTW_SECURITY_WPA_AES_PSK:
		case RTW_SECURITY_WPA2_AES_PSK:
		case RTW_SECURITY_WPA2_TKIP_PSK:
		case RTW_SECURITY_WPA2_MIXED_PSK:
		case RTW_SECURITY_WPA_WPA2_MIXED:
			GCONFIG_SetWifiEncryptMode(setting.security_type);
			GCONFIG_SetWifiPreShareKey(setting.password, strlen(setting.password));
			break;
		default:
			printf("\n\r%sUnknow security type!\n\r", GPIOS_PRINT_HEADER);
			return;
	}
	GCONFIG_WriteConfigData();
	
#if GPIOS_RESTART_AFTER_CONFIG_SAVE
	/* Reset system */
	GPIOS_PlatformReset();
#endif
	
} /* End of GPIOS_SaveCurrentWifiConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_CheckAndRestoreAPMode(U8_T *everChanged)
 * Purpose: Restore AP mode from station mode
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_CheckAndRestoreAPMode(U8_T *everChanged)
{
	char			ssid[33], password[64];
	u8				ssid_len, password_len, key_id;
	rtw_security_t	security_type;
#if CONFIG_LWIP_LAYER 
	struct ip_addr	ipaddr;
	struct ip_addr	netmask;
	struct ip_addr	gw;
	struct netif	*pnetif = &xnetif[0];
#endif
	int				timeout = 20;
	
	if (*everChanged)
	{
		printf("\n\r%sRestore to AP mode ...\n\r", GPIOS_PRINT_HEADER);
		*everChanged = 0;
#if CONFIG_LWIP_LAYER
		ipaddr.addr = ntohl(GCONFIG_GetDeviceStaticIP());
		netmask.addr = ntohl(GCONFIG_GetNetmask());
		gw.addr = ntohl(GCONFIG_GetGateway());
		netif_set_addr(pnetif, &ipaddr, &netmask,&gw);
	#ifdef CONFIG_DONT_CARE_TP
		pnetif->flags |= NETIF_FLAG_IPSWITCH;
	#endif
#endif
		wifi_off();
		vTaskDelay(20);
		
		if (wifi_on(RTW_MODE_AP) < 0)
		{
			printf("\n\r%sWifi on failed!", GPIOS_PRINT_HEADER);
			return;
		}
		
		ssid_len = GCONFIG_GetWifiSsid(ssid);
		ssid[ssid_len] = '\0';
		security_type = GCONFIG_GetWifiEncryptMode();
		if (security_type == GCONFIG_WIFI_OPEN)
		{
			/* No Security */
			password_len = 0;
			key_id = 0;
		}
		else
		{
			/* WPAx Pre-Shared Key */
			password_len = GCONFIG_GetWifiPreShareKey(password);
			key_id = 0;
			security_type = GCONFIG_WIFI_WPA2_AES_PSK;
		}
		
		if (wifi_start_ap(ssid,
						  security_map[security_type],
						  password,
						  ssid_len,
						  password_len,
						  GCONFIG_GetChannel()) == RTW_SUCCESS)
		{
			printf("\n\r%sStart AP(%s) successfully!", GPIOS_PRINT_HEADER, ssid);
		}
		else
		{
			printf("\n\r%sStart AP(%s) failed!", GPIOS_PRINT_HEADER, ssid);
			return;
		}
		
		
		while (1)
		{
			char essid[33];

			if (wext_get_ssid(WLAN0_NAME, (unsigned char *) essid) > 0)
			{
				if (strcmp((const char *) essid, (const char *)ssid) == 0)
				{
					printf("\n\r%s%s started\n", GPIOS_PRINT_HEADER, ssid);
					/* Save wifi parameters to gconfig */
					GCONFIG_SetWifiRfEnable(1);						/* WIFI RF Enable */
					GCONFIG_SetWifiNetworkMode(RTW_MODE_AP);		/* Network Mode */
					GCONFIG_SetWifiSsid(essid, strlen(essid));		/* SSID */
					GPIOS_ResetLEDIndicator();
					break;
				}
			}

			if (timeout == 0)
			{
				printf("\n\r%sERROR: Start AP timeout!", GPIOS_PRINT_HEADER);
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
#if (CONFIG_CLOUD_SELECT != CLOUD_DISABLE)
/*
 * ----------------------------------------------------------------------------
 * Function Name: void GPIOS_BeepCtrl(u8 turn_on)
 * Purpose:
 * Params:	None
 * Returns:	None
 * Note:	None
 * ----------------------------------------------------------------------------
 */
void GPIOS_BeepCtrl(u8 turn_on)
{
	gpio_write(&gpio_beep, turn_on ? 0:1);
} /* End of GPIOS_BeepCtrl() */
#endif
/* End of gpios.c */