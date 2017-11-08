/*
 *  Hello World
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" 
#include "serial_api.h"
#include "main.h"
#include "lwip_netconf.h"
#include <platform/platform_stdlib.h>
#include "startentry.h"
#include "upgrade.h"

#ifndef CONFIG_INTERACTIVE_MODE
#define CONFIG_INTERACTIVE_MODE     1
#endif

#define STACKSIZE                   (512 + 768)

extern serial_t    urObj;
xSemaphoreHandle uart_rx_interrupt_sema = NULL;

static void application_init(void);

void init_thread(void *param)
{
#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif
	UPGRADE_InitImageBuf();
#if CONFIG_WLAN
//	wifi_on(RTW_MODE_STA);
#if CONFIG_LWIP_LAYER
	StartAsixAppTask(&urObj);
#endif
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(1);
#endif
//	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, xPortGetFreeHeapSize());	
#endif

#if CONFIG_INTERACTIVE_MODE
 	/* Initial uart rx swmaphore*/
	vSemaphoreCreateBinary(uart_rx_interrupt_sema);
	xSemaphoreTake(uart_rx_interrupt_sema, 1/portTICK_RATE_MS);
	start_interactive_mode();
#endif

	/* Init user application */
	application_init();

	/* Kill init thread after all init tasks done */
	vTaskDelete(NULL);
}

static void application_init(void)
{
#if (CONFIG_CLOUD_SELECT == CLOUD_SELECT_GOOGLE_NEST)
	GNAPP_Init();
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_IBM_BLUEMIX)
	BLUEMIX_Init();
#elif (CONFIG_CLOUD_SELECT == CLOUD_SELECT_MICROSOFT_AZURE)
	AZURE_Init();
#endif
}

void wlan_network()
{
	if(xTaskCreate(init_thread, ((const char*)"init"), STACKSIZE, NULL, tskIDLE_PRIORITY + 3 + PRIORITIE_OFFSET, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
