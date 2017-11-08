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
/*=============================================================================
 * Module Name: rtc.h
 * Purpose:
 * Author:
 * Date:
 * Notes:
 *
 *=============================================================================
 */

#ifndef __RTC_H__
#define __RTC_H__

/* INCLUDE FILE DECLARATIONS */

/* NAMING CONSTANT DECLARATIONS */
#define RTC_DEBUG_ENABLE 	0
#define RTC_I2C_SLAVE_ADDR 	0x6f
#define RTC_NTP_QUERY_PERIOD	86400 //sec

#define	RTC_MAX_SECOND		59// 0 ~ 59 seconds
#define	RTC_MAX_MINUTE		59// 0 ~ 59 minutes
#define	RTC_MAX_HOUR		23// 0 ~ 23 hours
#define	RTC_MAX_DAY			6 // 0 ~ 06 ; 6 for Saturday, 0 for Sunday
#define	RTC_MAX_DATE		31// 1 ~ 31 ; Date 31 of Month
#define	RTC_MAX_MONTH		12// 1 ~ 12 months
#define RTC_MAX_YEAR		2099
#define RTC_MIN_YEAR		2000

	/* MCP7940 RTC register address */
#define MCP7940_TIME_KEEPING_ADDR		0x00
#define MCP7940_OUTPUT_CFIG_ADDR		0x07
#define MCP7940_ALARM_0_ADDR			0x0A
#define MCP7940_ALARM_1_ADDR			0x11
#define MCP7940_SRAM_ADDR				0x20

/* TYPE DECLARATIONS */
typedef enum {
	RTC_MASK_SECOND_MATCH = 0x00,
	RTC_MASK_MINUTES_MATCH = 0x01,
	RTC_MASK_HOURS_MATCH	= 0x02,
	RTC_MASK_WEEKDAY_MATCH = 0x03,
	RTC_MASK_DATE_MATCH = 0x04,
	RTC_MASK_ALL_MATCH = 0x07,
} RTC_ALARM_MASK_TYPES;

typedef enum {
	RTC_OUTPUT_LOW_LEVEL = 0,
	RTC_OUTPUT_HIGH_LEVEL,
} RTC_OUTPUT_POLAR;

typedef struct _MCP7940_TIMEKEEPING
{
	union
	{
		u8 d8;
		struct
		{
			u8 SecOne: 4;
			u8 SecTen: 3;			
			u8 OscEnable: 1;			
		} bm;
	} RTCSEC;

	union
	{
		u8 d8;
		struct
		{
			u8 MinOne: 4;
			u8 MinTen: 3;			
			u8 NA: 1;			
		} bm;
	} RTCMIN;
	
	union
	{
		u8 d8;
		struct
		{
			u8 HourOne: 4;
			u8 HourTen: 1;			
			u8 IsPM: 1;			
			u8 Is12HourFormat: 1;			
			u8 NA: 1;			
		} bm12;			
		struct
		{
			u8 HourOne: 4;		  
			u8 HourTen: 2;
			u8 Is12HourFormat: 1;			
			u8 NA: 1;			
		} bm24;
	} RTCHOUR;
	
	union
	{
		u8 d8;
		struct
		{
			u8 WeekDay: 3;
			u8 ExtBatteryEnable: 1;					
			u8 PrimaryPowerLost: 1;			
			u8 OscIsRunning: 1;			
			u8 NA: 2;			
		} bm;
	} RTCWKDAY;
	
	union
	{
		u8 d8;
		struct
		{
			u8 DateOne: 4;
			u8 DateTen: 2;			
			u8 NA: 2;			
		} bm;
	} RTCDATE;	
	
	union
	{
		u8 d8;
		struct
		{
			u8 MonthOne: 4;
			u8 MonthTen: 1;			
			u8 IsLeapYear: 1;			
			u8 NA: 2;			
		} bm;
	} RTCMTH;
	  
	union
	{
		u8 d8;
		struct
		{
			u8 YearOne: 4;
			u8 YearTen: 4;			
		} bm;
	} RTCYEAR;		  
} MCP7940_CURR_TIME;

typedef struct _MCP7940_ALARM
{
	union
	{
		u8 d8;
		struct
		{
			u8 SecOne: 4;
			u8 SecTen: 3;			
			u8 NA: 1;			
		} bm;
	} ALMxSEC;

	union
	{
		u8 d8;
		struct
		{
			u8 MinOne: 4;
			u8 MinTen: 3;			
			u8 NA: 1;			
		} bm;
	} ALMxMIN;
	
	union
	{
		u8 d8;
		struct
		{
			u8 HourOne: 4;
			u8 HourTen: 1;			
			u8 IsPM: 1;			
			u8 Is12HourFormat: 1;			
			u8 NA: 1;			
		} bm12;			
		struct
		{
			u8 HourOne: 4;		  
			u8 HourTen: 2;
			u8 Is12HourFormat: 1;			
			u8 NA: 1;			
		} bm24;
	} ALMxHOUR;
	
	union
	{
		u8 d8;
		struct
		{
			u8 WeekDay: 3;
			u8 AlarmIsMatch: 1;			
			u8 AlarmMask: 3;			
			u8 AlarmOutputHigh: 1;			
		} bm;
	} ALMxWKDAY;
	
	union
	{
		u8 d8;
		struct
		{
			u8 DateOne: 4;
			u8 DateTen: 2;			
			u8 NA: 2;			
		} bm;
	} ALMxDATE;	
	
	union
	{
		u8 d8;
		struct
		{
			u8 MonthOne: 4;
			u8 MonthTen: 1;			
			u8 NA: 3;
		} bm;
	} ALMxMTH;
} MCP7940_ALARM_TIME;

typedef struct _RTC_TIME
{
	u8	Second;
	u8	Minute;
	u8	Hour;
	u8	Day;
	u8	Date;
	u8	Month;
	u16	Year;
	u8	IsLeapYear;	
} RTC_TIME;
	
typedef struct _RTC_CTRL
{
	MCP7940_CURR_TIME MCP7940Time;
	MCP7940_ALARM_TIME MCP7940Alarm;
	u8 PeriodicNtpQuery;
	void *pObj;
} RTC_CTRL;

/* GLOBAL VARIABLES */
extern RTC_CTRL RTC_Ctrl;

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void RTC_Init(void *pObj);
u8 RTC_SetTimeConfig(u8 ModuleEnable, u8 _12HourFormat, u8 ExtBatteryEnable);
u8 RTC_SetAlarmConfig(u8 Group, u8 _12HourFormat, RTC_ALARM_MASK_TYPES MaskType, RTC_OUTPUT_POLAR OutputPolar);
u8 RTC_GetCurrTime(RTC_TIME *pRtcTime);
u8 RTC_SetCurrTime(RTC_TIME *pRtcTime);
u8 RTC_GetAlarmTime(RTC_TIME *pRtcTime, u8 Group);
u8 RTC_SetAlarmTime(RTC_TIME *pRtcTime, u8 Group);
void RTC_NtpQuery(void *pInfo);
void RTC_NtpPeriodicQuery(void *pInfo);

#endif /* End of __RTC_H__ */

/* End of rtc.h */
