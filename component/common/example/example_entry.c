/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "i2c_api.h"
#include "rtc.h"
#include "ths.h"
//#include "opt3001.h"
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
#include "ax88796c.h"
#endif

static i2c_t		i2c_bus3;

xSemaphoreHandle	spi0_mutex = NULL;

/*
	Preprocessor of example
*/
void pre_example_entry(void)
{
	/* Init I2C3 */
	i2c_sem_init(&i2c_bus3, PB_3, PB_2, 100000);

        
#if CONFIG_NETWORK_BACKUP_SWITCH_MODE
	/* Init AX88796 */
	ax88796c_spi_init();
#elif CONFIG_SMART_PLUG
	/* Init PL7223 */
	pl7223_Init();
#endif
}

/*
  	All of the examples are disabled by default for code size consideration
   	The configuration is enabled in platform_opts.h
*/
void example_entry(void)
{
	RTC_Init(&i2c_bus3);
	THS_Init(&i2c_bus3);
//	ALS_Init(&i2c_bus3);
}
