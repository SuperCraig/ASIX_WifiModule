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
 /*============================================================================
 * Module Name: rtc.c
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

/* INCLUDE FILE DECLARATIONS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "basic_types.h"
#include <string.h>
#include "i2c_api.h"
#include "gconfig.h"
#include "sntpc.h"
#include "rtc.h"

/* NAMING CONSTANT DECLARATIONS */

/* MACRO DECLARATIONS */

/* TYPE DECLARATIONS */

/* GLOBAL VARIABLES DECLARATIONS */
RTC_CTRL RTC_Ctrl;

/* LOCAL VARIABLES DECLARATIONS */
static xSemaphoreHandle rtcMutex;

/* LOCAL SUBPROGRAM DECLARATIONS */
static void rtc_Task(void *pParam);

/* LOCAL SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: void rtc_Task(void *pParam)
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void rtc_Task(void *pParam)
{
	RTC_SetTimeConfig(1, 0, 1);
	RTC_SetAlarmConfig(0, 0, RTC_MASK_ALL_MATCH, RTC_OUTPUT_HIGH_LEVEL);
  
    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{
		while (1)
		{
			vTaskDelay(3000);
		
			if (SNTPC_ReqEnqueue(RTC_NtpQuery, 0) != FALSE)
				break;
		}
	}
	vTaskDelete(NULL);
} /* End of rtc_Task() */

/* EXPORTED SUBPROGRAM BODIES */
/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_Init
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void RTC_Init(void *pObj)
{
	memset((u8*)&RTC_Ctrl, 0, sizeof(RTC_CTRL));
	rtcMutex = xSemaphoreCreateMutex();

	RTC_Ctrl.pObj = pObj;
	
	if(xTaskCreate(rtc_Task, (u8*)"rtc_task", 1280, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)                  
	{
		printf("RTC: Create task fail!\r\n");
	}
} /* End of RTC_Init() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_NtpQuery
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void RTC_NtpQuery(void *pInfo)
{
	SNTPC_INFO *pNtpInfo = pInfo;
	RTC_TIME RtcTime;
	
	if (pNtpInfo)
	{

		RtcTime.Second = pNtpInfo->SS;
		RtcTime.Minute = pNtpInfo->MI;
		RtcTime.Hour = pNtpInfo->HH;
		RtcTime.Day = pNtpInfo->day_total % 7;
		RtcTime.Day++;
		if (RtcTime.Day >= 7)
			RtcTime.Day = 0;
		RtcTime.Date = pNtpInfo->DD;
		RtcTime.Month = pNtpInfo->MM;
		RtcTime.Year = pNtpInfo->YY;

		if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE)
			SNTPC_AdjustTimeforDST(&RtcTime);
		if (RTC_SetCurrTime(&RtcTime) == FALSE)
			printf("Failed to set RTC time\r\n");
	}
	
    if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP)
	{  
		if (RTC_Ctrl.PeriodicNtpQuery == 0)
		{
			RTC_Ctrl.PeriodicNtpQuery = 1;
			SNTPC_ReqEnqueue(RTC_NtpPeriodicQuery, RTC_NTP_QUERY_PERIOD);
		}
	}
} /* End of RTC_NtpQuery() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_NtpPeriodicQuery
 * Purpose: 
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void RTC_NtpPeriodicQuery(void *pInfo)
{
	SNTPC_INFO *pNtpInfo = pInfo;
	RTC_TIME RtcTime;

	if (pNtpInfo)
	{
		RtcTime.Second = pNtpInfo->SS;
		RtcTime.Minute = pNtpInfo->MI;
		RtcTime.Hour = pNtpInfo->HH;
		RtcTime.Day = pNtpInfo->day_total % 7;
		RtcTime.Day++;
		if (RtcTime.Day >= 7)
			RtcTime.Day = 0;
		RtcTime.Date = pNtpInfo->DD;
		RtcTime.Month = pNtpInfo->MM;
		RtcTime.Year = pNtpInfo->YY;

		if (GCONFIG_GetNetwork() & GCONFIG_NETWORK_DAYLIGHT_SAVING_TIME_ENABLE)
			SNTPC_AdjustTimeforDST(&RtcTime);
		if (RTC_SetCurrTime(&RtcTime) == FALSE)
			printf("Failed to set RTC time\r\n");
	}
		
    if ((GCONFIG_GetNetwork() & GCONFIG_NETWORK_RTC_TIME_SETUP_BY_NTP) == 0)
	{  
	  	/* Stop periodic NTP query */
		RTC_Ctrl.PeriodicNtpQuery = 0;	  
	}
	else
	{
	  	/* Continue next NTP query after a period time */
		SNTPC_ReqEnqueue(RTC_NtpPeriodicQuery, RTC_NTP_QUERY_PERIOD);				  		
	}
} /* End of RTC_NtpPeriodicQuery() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_SetTimeConfig
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_SetTimeConfig(u8 ModuleEnable, u8 _12HourFormat, u8 ExtBatteryEnable)
{
	MCP7940_CURR_TIME *pMCP7940Time = &RTC_Ctrl.MCP7940Time;

	if (i2c_sem_read(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, MCP7940_TIME_KEEPING_ADDR
				, (u8*)pMCP7940Time
				, sizeof(MCP7940_CURR_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	
	while(xSemaphoreTake(rtcMutex, portMAX_DELAY) != pdTRUE);
	pMCP7940Time->RTCSEC.bm.OscEnable = ModuleEnable ? 1:0;
	pMCP7940Time->RTCHOUR.bm24.Is12HourFormat = (_12HourFormat!=0) ? 1:0;
//	pMCP7940Time->RTCWKDAY.bm.OscIsRunning: 1;
	pMCP7940Time->RTCWKDAY.bm.PrimaryPowerLost = 0;
	pMCP7940Time->RTCWKDAY.bm.ExtBatteryEnable = ExtBatteryEnable ? 1:0;		
	xSemaphoreGive(rtcMutex);	
	
	if (i2c_sem_write(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, MCP7940_TIME_KEEPING_ADDR
				, (u8*)pMCP7940Time
				, sizeof(MCP7940_CURR_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
	  	return FALSE;
	}
	return TRUE;
} /* End of RTC_SetTimeConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_SetAlarmConfig
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_SetAlarmConfig(u8 Group, u8 _12HourFormat, RTC_ALARM_MASK_TYPES MaskType, RTC_OUTPUT_POLAR OutputPolar)
{
	MCP7940_ALARM_TIME *pMCP7940Alarm = &RTC_Ctrl.MCP7940Alarm;

	if (i2c_sem_read(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, Group ? MCP7940_ALARM_1_ADDR : MCP7940_ALARM_0_ADDR
				, (u8*)pMCP7940Alarm
				, sizeof(MCP7940_ALARM_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	
	while(xSemaphoreTake(rtcMutex, portMAX_DELAY) != pdTRUE);	
	pMCP7940Alarm->ALMxHOUR.bm24.Is12HourFormat = _12HourFormat;
	pMCP7940Alarm->ALMxWKDAY.bm.AlarmIsMatch = 0;
	pMCP7940Alarm->ALMxWKDAY.bm.AlarmMask = MaskType;
	pMCP7940Alarm->ALMxWKDAY.bm.AlarmOutputHigh = OutputPolar ? 1:0;
	xSemaphoreGive(rtcMutex);
	
	if (i2c_sem_write(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, Group ? MCP7940_ALARM_1_ADDR : MCP7940_ALARM_0_ADDR
				, (u8*)pMCP7940Alarm
				, sizeof(MCP7940_ALARM_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
} /* End of RTC_SetAlarmConfig() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_GetCurrTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_GetCurrTime(RTC_TIME *pRtcTime)
{
	MCP7940_CURR_TIME *pMCP7940Time = &RTC_Ctrl.MCP7940Time;

	if (i2c_sem_read(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, MCP7940_TIME_KEEPING_ADDR
				, (u8*)pMCP7940Time
				, sizeof(MCP7940_CURR_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}

#if 0//RTC_DEBUG_ENABLE		
	printf("RTC: RTCSEC=%2x, RTCMIN=%2x, RTCHOUR=%2x, RTCWKDAY=%2x, RTCDATE=%2x, RTCMTH=%2x, RTCYEAR=%2x\r\n"
			, pMCP7940Time->RTCSEC.d8
			, pMCP7940Time->RTCMIN.d8			 
			, pMCP7940Time->RTCHOUR.d8
			, pMCP7940Time->RTCWKDAY.d8
			, pMCP7940Time->RTCDATE.d8			 
			, pMCP7940Time->RTCMTH.d8			  
			, pMCP7940Time->RTCYEAR.d8			  
			 );	
#endif
	
	/* If the oscillator is disabled, return FALSE */
	if (pMCP7940Time->RTCWKDAY.bm.OscIsRunning)
	{
		pRtcTime->Second = (pMCP7940Time->RTCSEC.bm.SecTen*10) + pMCP7940Time->RTCSEC.bm.SecOne;
		pRtcTime->Minute = (pMCP7940Time->RTCMIN.bm.MinTen*10) + pMCP7940Time->RTCMIN.bm.MinOne;
		pRtcTime->Hour = (pMCP7940Time->RTCHOUR.bm24.HourTen*10) + pMCP7940Time->RTCHOUR.bm24.HourOne;
		pRtcTime->Day = pMCP7940Time->RTCWKDAY.bm.WeekDay;
		pRtcTime->Date = (pMCP7940Time->RTCDATE.bm.DateTen*10) + pMCP7940Time->RTCDATE.bm.DateOne;
		pRtcTime->Month = (pMCP7940Time->RTCMTH.bm.MonthTen*10) + pMCP7940Time->RTCMTH.bm.MonthOne;
		pRtcTime->Year = RTC_MIN_YEAR + (pMCP7940Time->RTCYEAR.bm.YearTen*10) + pMCP7940Time->RTCYEAR.bm.YearOne;
		pRtcTime->IsLeapYear = pMCP7940Time->RTCMTH.bm.IsLeapYear;
		
		/* Check value range */
		if (pRtcTime->Date < 1)
			pRtcTime->Date = 1;
		if (pRtcTime->Date > 31)
			pRtcTime->Date = 31;		
		if (pRtcTime->Month < 1)
			pRtcTime->Month = 1;
		if (pRtcTime->Month > 12)
			pRtcTime->Month = 12;		
		return TRUE;
	}
	else
	{
		printf("RTC: Oscillator is disabled!\r\n");
	}
	return FALSE;
} /* End of RTC_GetCurrTime() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_SetCurrTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_SetCurrTime(RTC_TIME *pRtcTime)
{
	MCP7940_CURR_TIME *pMCP7940Time = &RTC_Ctrl.MCP7940Time;
	
	pRtcTime->Day = SNTPC_GetWeekDay(pRtcTime->Year, pRtcTime->Month, pRtcTime->Date);

	if ((pRtcTime->Year < RTC_MIN_YEAR) || (pRtcTime->Year > RTC_MAX_YEAR))
	{
#if 0//RTC_DEBUG_ENABLE	  
		printf("Wrong year value(%d) to be setted\r\n", pRtcTime->Year);
#endif		
		return FALSE;
	}
	if ((pRtcTime->Month > RTC_MAX_MONTH) || (pRtcTime->Date > RTC_MAX_DATE) || (pRtcTime->Day > RTC_MAX_DAY)
		|| (pRtcTime->Hour > RTC_MAX_HOUR) || (pRtcTime->Minute > RTC_MAX_MINUTE) || (pRtcTime->Second > RTC_MAX_SECOND))
	{
#if 0//RTC_DEBUG_ENABLE			  
		printf("Wrong Month=%d, Date=%d, Day=%d, Hour=%d, Minute=%d, Second=%d\r\n", pRtcTime->Month, pRtcTime->Date, pRtcTime->Day, pRtcTime->Hour, pRtcTime->Minute, pRtcTime->Second);	  
#endif		
		return FALSE;
	}
	
	while(xSemaphoreTake(rtcMutex, portMAX_DELAY) != pdTRUE);
	pMCP7940Time->RTCSEC.bm.SecTen = pRtcTime->Second / 10;
	pMCP7940Time->RTCSEC.bm.SecOne = pRtcTime->Second % 10;	
	pMCP7940Time->RTCMIN.bm.MinTen = pRtcTime->Minute / 10;
	pMCP7940Time->RTCMIN.bm.MinOne = pRtcTime->Minute % 10;
	pMCP7940Time->RTCHOUR.bm24.HourTen = pRtcTime->Hour / 10;
	pMCP7940Time->RTCHOUR.bm24.HourOne = pRtcTime->Hour % 10;	
	pMCP7940Time->RTCWKDAY.bm.PrimaryPowerLost = 0;
	pMCP7940Time->RTCWKDAY.bm.WeekDay = pRtcTime->Day;
	pMCP7940Time->RTCDATE.bm.DateTen = pRtcTime->Date / 10;
	pMCP7940Time->RTCDATE.bm.DateOne = pRtcTime->Date % 10;		
	pMCP7940Time->RTCMTH.bm.MonthTen = pRtcTime->Month / 10;
	pMCP7940Time->RTCMTH.bm.MonthOne = pRtcTime->Month % 10;	
	pMCP7940Time->RTCYEAR.bm.YearTen = (pRtcTime->Year - RTC_MIN_YEAR) / 10;
	pMCP7940Time->RTCYEAR.bm.YearOne = (pRtcTime->Year - RTC_MIN_YEAR) % 10;
	xSemaphoreGive(rtcMutex);	

	if (i2c_sem_write(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, MCP7940_TIME_KEEPING_ADDR
				, (u8*)pMCP7940Time
				, sizeof(MCP7940_CURR_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	
#if 0//RTC_DEBUG_ENABLE		
	printf("RTC: RTCSEC=%2x, RTCMIN=%2x, RTCHOUR=%2x, RTCWKDAY=%2x, RTCDATE=%2x, RTCMTH=%2x, RTCYEAR=%2x\r\n"
			, pMCP7940Time->RTCSEC.d8
			, pMCP7940Time->RTCMIN.d8			 
			, pMCP7940Time->RTCHOUR.d8
			, pMCP7940Time->RTCWKDAY.d8
			, pMCP7940Time->RTCDATE.d8			 
			, pMCP7940Time->RTCMTH.d8			  
			, pMCP7940Time->RTCYEAR.d8			  
			 );	
#endif	
	return TRUE;
} /* End of RTC_SetCurrTime() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_GetAlarmTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_GetAlarmTime(RTC_TIME *pRtcTime, u8 Group)
{
   	MCP7940_ALARM_TIME *pMCP7940Alarm = &RTC_Ctrl.MCP7940Alarm;

	if (i2c_sem_read(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, Group ? MCP7940_ALARM_1_ADDR : MCP7940_ALARM_0_ADDR
				, (u8*)pMCP7940Alarm
				, sizeof(MCP7940_ALARM_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}

	pRtcTime->Second = (pMCP7940Alarm->ALMxSEC.bm.SecTen*10) + pMCP7940Alarm->ALMxSEC.bm.SecOne;
	pRtcTime->Minute = (pMCP7940Alarm->ALMxMIN.bm.MinTen*10) + pMCP7940Alarm->ALMxMIN.bm.MinOne;
	pRtcTime->Hour = (pMCP7940Alarm->ALMxHOUR.bm24.HourTen*10) + pMCP7940Alarm->ALMxHOUR.bm24.HourOne;
	pRtcTime->Day = pMCP7940Alarm->ALMxWKDAY.bm.WeekDay;
	pRtcTime->Date = (pMCP7940Alarm->ALMxDATE.bm.DateTen*10) + pMCP7940Alarm->ALMxDATE.bm.DateOne;
	pRtcTime->Month = (pMCP7940Alarm->ALMxMTH.bm.MonthTen*10) + pMCP7940Alarm->ALMxMTH.bm.MonthOne;
	pRtcTime->Year = 0;
	
#if 0//RTC_DEBUG_ENABLE	
	printf("GetAlarmTime: ALMxSEC=%2x, ALMxMIN=%2x, ALMxHOUR=%2x, ALMxWKDAY=%2x, ALMxDATE=%2x, ALMxMTH=%2x\r\n"
			, pMCP7940Alarm->ALMxSEC.d8
			, pMCP7940Alarm->ALMxMIN.d8			 
			, pMCP7940Alarm->ALMxHOUR.d8
			, pMCP7940Alarm->ALMxWKDAY.d8
			, pMCP7940Alarm->ALMxDATE.d8			 
			, pMCP7940Alarm->ALMxMTH.d8			  
			 );	
#endif	
	return TRUE;
} /* End of RTC_GetAlarmTime() */

/*
 * ----------------------------------------------------------------------------
 * Function Name: RTC_SetAlarmTime
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
u8 RTC_SetAlarmTime(RTC_TIME *pRtcTime, u8 Group)
{
	MCP7940_ALARM_TIME *pMCP7940Alarm = &RTC_Ctrl.MCP7940Alarm;
	
	if ((pRtcTime->Year < RTC_MIN_YEAR) || (pRtcTime->Year > RTC_MAX_YEAR))
	{
		return FALSE;
	}
	if ((pRtcTime->Month > RTC_MAX_MONTH) || (pRtcTime->Date > RTC_MAX_DATE) || (pRtcTime->Day > RTC_MAX_DAY)
		|| (pRtcTime->Hour > RTC_MAX_HOUR) || (pRtcTime->Minute > RTC_MAX_MINUTE) || (pRtcTime->Second > RTC_MAX_SECOND))
	{
		return FALSE;
	}
	while(xSemaphoreTake(rtcMutex, portMAX_DELAY) != pdTRUE);	
	pMCP7940Alarm->ALMxSEC.bm.SecTen = pRtcTime->Second / 10;
	pMCP7940Alarm->ALMxSEC.bm.SecOne = pRtcTime->Second % 10;	
	pMCP7940Alarm->ALMxMIN.bm.MinTen = pRtcTime->Minute / 10;
	pMCP7940Alarm->ALMxMIN.bm.MinOne = pRtcTime->Minute % 10;
	pMCP7940Alarm->ALMxHOUR.bm24.HourTen = pRtcTime->Hour / 10;
	pMCP7940Alarm->ALMxHOUR.bm24.HourOne = pRtcTime->Hour % 10;	
	pMCP7940Alarm->ALMxWKDAY.bm.WeekDay = SNTPC_GetWeekDay(pRtcTime->Year, pRtcTime->Month, pRtcTime->Date);
	pMCP7940Alarm->ALMxDATE.bm.DateTen = pRtcTime->Date / 10;
	pMCP7940Alarm->ALMxDATE.bm.DateOne = pRtcTime->Date % 10;		
	pMCP7940Alarm->ALMxMTH.bm.MonthTen = pRtcTime->Month / 10;
	pMCP7940Alarm->ALMxMTH.bm.MonthOne = pRtcTime->Month % 10;	
	xSemaphoreGive(rtcMutex);	

	if (i2c_sem_write(RTC_Ctrl.pObj
				, RTC_I2C_SLAVE_ADDR
				, Group ? MCP7940_ALARM_1_ADDR : MCP7940_ALARM_0_ADDR
				, (u8*)pMCP7940Alarm
				, sizeof(MCP7940_ALARM_TIME)
				, I2C_EXD_MTR_ADDR_RTY | I2C_EXD_USER_REG) == FALSE)
	{
		return FALSE;
	}
	
#if 0//RTC_DEBUG_ENABLE		
	printf("SetAlarmTime: ALMxSEC=%2x, ALMxMIN=%2x, ALMxHOUR=%2x, ALMxWKDAY=%2x, ALMxDATE=%2x, ALMxMTH=%2x\r\n"
			, pMCP7940Alarm->ALMxSEC.d8
			, pMCP7940Alarm->ALMxMIN.d8			 
			, pMCP7940Alarm->ALMxHOUR.d8
			, pMCP7940Alarm->ALMxWKDAY.d8
			, pMCP7940Alarm->ALMxDATE.d8			 
			, pMCP7940Alarm->ALMxMTH.d8			  
			 );	
#endif	
	return TRUE;
} /* End of RTC_SetAlarmTime() */

/* End of rtc.c */
