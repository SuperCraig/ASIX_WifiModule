#include "FreeRTOS.h"
#include "task.h"
#include "device.h"
#include "main.h"
#include <example_entry.h>
#include "gconfig.h"
#include "misc.h"
#include "MBI_ASIXTask.h"


extern void console_init(void);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	if ( rtl_cryptoEngine_init() != 0 ) {
		DiagPrintf("crypto engine init failed\r\n");
	}
        
	/* Initialize log uart and at command service */
	console_init();

	MISC_DefaultImageReloadCheck(DEFAULT_IMAGE_RELOAD_TIMEOUT, DEFAULT_IMAGE_RELOAD_BUTTON, DEFAULT_IMAGE_RELOAD_INDICATOR_LED);

	/* Initialize the SW configuration */
	GCONFIG_Init();

	/* pre-processor of application example */
	pre_example_entry();

	/* wlan intialization */
#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	wlan_network();
#endif

	/* Execute application example */
	example_entry();

    MBI_Init();
    
    
    /*Enable Schedule, Start Kernel*/
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif
}

/* End of main.c */
