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
/*=============================================================================
 * Module Name: pl7223.c
 * Purpose:		Handle needed functions of Prolific PL7223 through SPI
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "pl7223.h"
#include <stdio.h>
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"

/* NAMING CONSTANT DECLARATIONS */
#define SPI0_MOSI  	PC_2
#define SPI0_MISO  	PC_3
#define SPI0_SCLK  	PC_1
#define SPI0_CS    	PC_0

#define GPIO_RESET	PE_4
#define GPIO_CS		PC_5

/* EXTERNAL VARIABLES DECLARATIONS */
extern xSemaphoreHandle	spi0_mutex;

/* GLOBAL VARIABLES DECLARATIONS */
unsigned char		PL7223_InitSuccess = 0;
unsigned char		SPDAT;					// SPI data that read MOSI back after each write
unsigned char		DSP_STATUS = 0;			// DSP status
unsigned char		Read_Data_PL7223[146];	// Read_Data; 256Bytes = 1Page
unsigned char		Write_Data_PL7223[146];	// Write_Data; 256Bytes = 1Page
long				EE_Temp = 0;
float				VA_rms = 0;
float				IA_rms = 0;
float				PA = 0;
float				SA = 0;
float				QA = 0;
float				PF_A = 0;
float				Theta_A = 0;
float				Frequency = 0;
int					Sample_cnt0 = 0;
int					ZCC_cnt = 0;
int					ZCC_Start = 0;
int					ZCC_Stop = 0;

/* STATIC VARIABLES DECLARATIONS */
static spi_t		spi0_master;
static gpio_t		gpio_reset;
static gpio_t		gpio_cs;

/* LOCAL SUBPROGRAM DECLARATIONS */

/* SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_Init(void)
 * Purpose : Init related pins, SPI, reset PL7223 and do DSP check
 * Params  : None
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_Init(void)
{
	unsigned char	checkTimes = 0;
	
	/*
		This driver will use GPIO_E4 & E2 so that
		we should disable JTAG function
	*/
	if (GCONFIG_GetJtagOff())
	{
		sys_jtag_off();
		
		/* Check if need to create semaphore here */
		spi0_mutex = xSemaphoreCreateMutex();
		
		/* Init reset pin */
		gpio_init(&gpio_reset, GPIO_RESET);
		GpioFunctionChk(GPIO_RESET, DISABLE);
		gpio_dir(&gpio_reset, PIN_OUTPUT);
		gpio_mode(&gpio_reset, PullUp);
		gpio_write(&gpio_reset, 1);
		
		/* Init chip select pin */
		gpio_init(&gpio_cs, GPIO_CS);
		GpioFunctionChk(GPIO_CS, DISABLE);
		gpio_dir(&gpio_cs, PIN_OUTPUT);
		gpio_mode(&gpio_cs, PullUp);
		gpio_write(&gpio_cs, 1);

		/* Init SPI for PL7223 */
		spi_init(&spi0_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
		spi_format(&spi0_master, 8, 3, 0);
		spi_frequency(&spi0_master, PL7223_SPI_CLK);
		
		/* Reset PL7223 and do DSP version check */
		printf("%sReset PL7223 and do DSP version check...\n\r", PL7223_PRINT_HEADER);
		
		do
		{
			pl7223_Reset();
			pl7223_Read(&Read_Data_PL7223[0], PL7223_DBMA_DSP_VER, 1);	//DSP version :20130322 ver02, 0x3860=0x04
																		//DSP version :20141009 ver01, 0x3860=0x03
#if PL7223_DEBUG
			printf("%sDSP version = %x\n\r", PL7223_PRINT_HEADER, Read_Data_PL7223[0]);
#endif
			if (++checkTimes >= PL7223_DSP_CHK_MAX_TIMES)
			{
				printf("%sInit PL7223 failed!!\n\r", PL7223_PRINT_HEADER);
				PL7223_InitSuccess = 0;
				gpio_deinit(&gpio_reset);
				gpio_deinit(&gpio_cs);
				spi_free(&spi0_master);
				return;
			}
			
		} while (((Read_Data_PL7223[0]) != PL7223_DSP_VER2) && ((Read_Data_PL7223[0]) != PL7223_DSP_VER1));
		
		printf("%sDSP version = %x\n\r", PL7223_PRINT_HEADER, Read_Data_PL7223[0]);
		
		/* Turn off relay by default */
		pl7223_Delay(PL7223_INIT_DELAY_CNT);
		pl7223_RelayControl(0);	// OFF
		pl7223_Delay(PL7223_INIT_DELAY_CNT);
		
		PL7223_InitSuccess = 1;
	}
	else
	{
		PL7223_InitSuccess = 0;
	}
	
} /* End of pl7223_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function: unsigned char pl7223_IsReadyNow(void)
 * Purpose : Confirm if PL7223 be initialized successfully
 * Params  : None
 * Returns : 0 = Failed or not yet, 1 = Success
 * Note    : None
 * ----------------------------------------------------------------------------
 */
unsigned char pl7223_IsReadyNow(void)
{
	return PL7223_InitSuccess;
	
} /* End of pl7223_IsReadyNow() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_RelayControl(int sw)
 * Purpose : Control relay of PL7223
 * Params  : sw: 0 = off, 1 = on
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_RelayControl(int sw)
{
	pl7223_Read(&Read_Data_PL7223[0], PL7223_DBMA_RELAY, 1);
	
	if (sw)
	{
		Read_Data_PL7223[0] |= PL7223_RELAY_MASK;
	}
	else
	{
		Read_Data_PL7223[0] &= (~PL7223_RELAY_MASK);
	}
	
	pl7223_Write(&Read_Data_PL7223[0], PL7223_DBMA_RELAY, 1);

} /* End of pl7223_RelayControl() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_Reset(void)
 * Purpose : Let PL7223 enter MCU mode
 * Params  : None
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_Reset(void)
{
	while(xSemaphoreTake(spi0_mutex, portMAX_DELAY) != pdTRUE);
	
	gpio_write(&gpio_cs, 0);
	pl7223_Delay(PL7223_RESET_T1_CNT);	//need delay 10ms
	gpio_write(&gpio_reset, 1);
	pl7223_Delay(PL7223_RESET_T1_CNT);	//need delay 10ms
	gpio_write(&gpio_reset, 0);
	pl7223_Delay(PL7223_RESET_T1_CNT);	//need delay 10ms
	gpio_write(&gpio_reset, 1);
	pl7223_Delay(PL7223_RESET_T1_CNT);	//need delay 10ms
	gpio_write(&gpio_cs, 1);
	
	xSemaphoreGive(spi0_mutex);
	
	pl7223_Delay(PL7223_RESET_T2_CNT);

} /* End of pl7223_Reset() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_ReadStatus(void)
 * Purpose : Make sure if DSP of PL7223 is in ready state
 * Params  : None
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_ReadStatus(void)
{
	while(xSemaphoreTake(spi0_mutex, portMAX_DELAY) != pdTRUE);
	
	gpio_write(&gpio_cs, 0);
	
	pl7223_Send((unsigned char)(DSPSTATUS_PL7223 >> 8) & 0xFF);	// RDSR command
	pl7223_Send((unsigned char)(DSPSTATUS_PL7223 & 0x00FF));	// RDSR command

	/* Check DSP flag state */
	do
	{
		pl7223_Send(DUM_PL7223);
		DSP_STATUS = SPDAT;
		
	} while ((DSP_STATUS & PL7223_DSP_READY_MASK) == 0x00);	// Bit7=1 is Ready
	
	gpio_write(&gpio_cs, 1);
	
	xSemaphoreGive(spi0_mutex);

} /* End of pl7223_ReadStatus() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_Read(unsigned char* buf, unsigned int addr, unsigned int len)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void pl7223_Read(unsigned char* buf, unsigned int addr, unsigned int len)
{
	static unsigned int	i;

	while(xSemaphoreTake(spi0_mutex, portMAX_DELAY) != pdTRUE);
	
	gpio_write(&gpio_cs, 0);

	addr |= READ_PL7223;	// Read command
	pl7223_Send((unsigned char)(addr >> 8) & 0xFF);	// Write middle byte address
	pl7223_Send((unsigned char)(addr & 0x00FF));	// Write low byte address

	for (i = 0; i < len; i++)	// Read 256 Bytes/Page to Flash Memory
	{
		pl7223_Send(DUM_PL7223);	// Send Dummy Data to Read righ Data
		buf[i] = SPDAT;				// Read SPIDAT and clear TX complete flag
	}

	gpio_write(&gpio_cs, 1);
	
	xSemaphoreGive(spi0_mutex);

} /* End of pl7223_Read() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_Write(unsigned char* buf, unsigned int addr, unsigned int len)
 * Purpose :
 * Params  :
 * Returns :
 * Note    :
 * ----------------------------------------------------------------------------
 */
void pl7223_Write(unsigned char* buf, unsigned int addr, unsigned int len)
{
	unsigned int	i;
	
	while(xSemaphoreTake(spi0_mutex, portMAX_DELAY) != pdTRUE);
	
	gpio_write(&gpio_cs, 0);
	
	addr |= WRITE_PL7223;	// Write command
	pl7223_Send((unsigned char)(addr >> 8) & 0xFF);	// Write middle byte address
	pl7223_Send((unsigned char)(addr & 0xFF));		// Write low byte address
	
	for (i = 0; i < len ; i++)
	{
		pl7223_Send(buf[i]);
	}
	
	gpio_write(&gpio_cs, 1);
	
	xSemaphoreGive(spi0_mutex);
	
	pl7223_Delay(PL7223_WRITE_DELAY_CNT);	// for CS:Hi to Low need 100nsec, Delay-Time 27usec

} /* End of pl7223_Write() */

/*
 * ----------------------------------------------------------------------------
 * Function: pl7223_Masurement(void)
 * Purpose : Get related masurement result and show them
 * Params  : None
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_Masurement(void)
{
	/* Wait DSP ready */
	pl7223_ReadStatus();
	
	/* Get masurement result */
	pl7223_Read(&Read_Data_PL7223[0], PL7223_DBMA_VRMS, 144); // 0x3000~0x308F, 144 bytes
	pl7223_Read(&Read_Data_PL7223[144], PL7223_DBMA_SAMPLE_CNT, 2); // Sample_cnt0
	
	/* Calculate all */
	/*
		Vrms address : 0x3002~0x3003
		VA_rms = (Read_Data_PL7223[3] * 256 + Read_Data_PL7223[2]) / 64;
	*/
	EE_Temp =  Read_Data_PL7223[3];
	EE_Temp =  EE_Temp << 8;
	EE_Temp += Read_Data_PL7223[2];
	VA_rms = (float)EE_Temp / 64.00;

	/*
		Irms address : 0x3008~0x3009
		IA_rms = Read_Data_PL7223[3] + Read_Data_PL7223[2] / 256;
	*/
	EE_Temp =  Read_Data_PL7223[8];
	IA_rms = (float)EE_Temp / 256.00;
	EE_Temp = Read_Data_PL7223[9];
	IA_rms = IA_rms + (float)EE_Temp;

	/*
		Active power address : 0x3078~0x307D
		PA = Read_Data_PL7223[124] * 256 + Read_Data_PL7223[123];
	*/
	EE_Temp =  Read_Data_PL7223[124];
	EE_Temp =  EE_Temp << 8;
	EE_Temp += Read_Data_PL7223[123];
	PA = (float)EE_Temp;

	/*
		PF Calculate
		SA = VA_rms*IA_rms;
		PF_A = PA/SA
	*/
	SA = VA_rms * IA_rms;
	PF_A = SA == 0 ? 0 : PA / SA;
	Theta_A = acos(PF_A);
	QA = SA * sin(Theta_A);
	
	if (IA_rms == 0)
	{
		Theta_A = 0;
	}
	else
	{
		Theta_A = Theta_A * (180.00 / (3.141592653589));
	}

	/*
		Frequency = [Sample_cnt0 / (ZCC_STOP - ZCC_START)] * [(ZCC_CNT - 1) / 2]
	*/
	Sample_cnt0 = Read_Data_PL7223[145];	// Sample_cnt01
	Sample_cnt0 = Sample_cnt0 << 8;
	Sample_cnt0 += Read_Data_PL7223[144];	// Sample_cnt00

	ZCC_cnt = Read_Data_PL7223[91];			// ZCC_cnt1
	ZCC_cnt = ZCC_cnt << 8;
	ZCC_cnt += Read_Data_PL7223[90];		// ZCC_cnt0

	ZCC_Stop = Read_Data_PL7223[97];		// ZCC_STOP1
	ZCC_Stop = ZCC_Stop << 8;
	ZCC_Stop += Read_Data_PL7223[96];		// ZCC_STOP0

	ZCC_Start = Read_Data_PL7223[103];		// ZCC_START1
	ZCC_Start = ZCC_Start << 8;
	ZCC_Start += Read_Data_PL7223[102];		// ZCC_START0
	
	Frequency = (float)((float)Sample_cnt0 / (ZCC_Stop - ZCC_Start)) * (((float)ZCC_cnt - 1.0) / 2);

	/* Show all results */
	PL7223_Display(VA_rms);
	PL7223_Display(IA_rms);
	PL7223_Display(Frequency);
	PL7223_Display(PA);
	PL7223_Display(QA);
	PL7223_Display(SA);
	PL7223_Display(PF_A);
	PL7223_Display(Theta_A);

} /* End of pl7223_Masurement() */

/*
 * ----------------------------------------------------------------------------
 * Function: pl7223_Delay()
 * Purpose : Delay routine for each important access
 * Params  : dely_cnt: count multiple of PL7223_DELAY_CNT_MULP for us
 * Returns : None
 * Note    : MCUCLK 4MHz, Delay-Time 9usec/clock
 * ----------------------------------------------------------------------------
 */
void pl7223_Delay(int dely_cnt)
{
	HalDelayUs(dely_cnt * PL7223_DELAY_CNT_MULP);

} /* End of pl7223_Delay() */

/*
 * ----------------------------------------------------------------------------
 * Function: void pl7223_Send(unsigned char spicmd)
 * Purpose : Write SPI data by 1 byte and read respone back
 * Params  : spicmd: SPI data
 * Returns : None
 * Note    : None
 * ----------------------------------------------------------------------------
 */
void pl7223_Send(unsigned char spicmd)
{
	SPDAT = (char)spi_master_write(&spi0_master, (int)spicmd);

} /* End of pl7223_Send() */

/* End of pl7223.c */