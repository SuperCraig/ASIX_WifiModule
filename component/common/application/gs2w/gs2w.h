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
/*================================================================================
 * Module Name : gs2w.h
 * Purpose     : 
 * Author      : 
 * Date        :
 * Notes       :
 *================================================================================
 */
#ifndef GS2W_H
#define GS2W_H

/* INCLUDE FILE DECLARATIONS */


/* NAMING CONSTANT DECLARATIONS */
#define GUDPDAT_INCLUDE							1

#define GS2W_STATE_IDLE							0 
#define GS2W_STATE_TCP_DATA_PROCESS 			1
#define GS2W_STATE_UDP_DATA_PROCESS 			2

#define GS2W_CONN_UDP							1
#define GS2W_CONN_TCP							2

#define GS2W_CLOSE_INDICATOR					0x30
#define GS2W_CONNECT_INDICATOR					0x31
#define GS2W_DATA_INDICATOR						0x32
#define GS2W_FLOW_CONTROL_INDICATOR				0x33

// Serial port flow control commands
#define MODEM_STATUS_CHANGE						0x81
#define IOCTL_SERIAL_GET_DTRRTS					0x01
#define IOCTL_SERIAL_GET_MODEM_CONTROL			0x02
#define IOCTL_SERIAL_GET_MODEM_STATUS			0x03
#define IOCTL_SERIAL_GET_DTRRTS_REPLY			0xA1
#define IOCTL_SERIAL_GET_MODEM_CONTROL_REPLY	0xA2
#define IOCTL_SERIAL_GET_MODEM_STATUS_REPLY		0xA3
#define IOCTL_SERIAL_CLR_DTR					0x11
#define IOCTL_SERIAL_CLR_RTS					0x12
#define IOCTL_SERIAL_SET_DTR					0x13
#define IOCTL_SERIAL_SET_RTS					0x14
#define IOCTL_SERIAL_SET_BAUD_RATE				0x15
//#define IOCTL_SERIAL_SET_BREAK_OFF			0x16
//#define IOCTL_SERIAL_SET_BREAK_ON				0x17
//#define IOCTL_SERIAL_SET_FIFO_CONTROL			0x18
//#define IOCTL_SERIAL_SET_LINE_CONTROL			0x19
#define IOCTL_SERIAL_SET_MODEM_CONTROL			0x1A
#define IOCTL_SERIAL_SET_XOFF					0x21
#define IOCTL_SERIAL_SET_XON					0x22
#define IOCTL_SERIAL_XOFF_COUNTER				0x23
#define IOCTL_SERIAL_SET_HARDFLOW				0x24

#define GSMCR_DTR			0x01
#define GSMCR_RTS			0x02
#define GSMSR_CTS			0x10
#define GSMSR_DSR			0x20
#define GSMSR_DCTS			0x01
#define GSMSR_DDSR			0x02

typedef enum
{
	SERIAL_DATA_BITS_5 = 0,
	SERIAL_DATA_BITS_6,
	SERIAL_DATA_BITS_7,
	SERIAL_DATA_BITS_8,
} IOCTL_SERIAL_DATA_BITS;

typedef enum
{
	SERIAL_STOP_BITS_1 = 0,
	SERIAL_STOP_BITS_1_5,
	SERIAL_STOP_BITS_2 = 1,
} IOCTL_SERIAL_STOP_BITS;

typedef enum
{
	SERIAL_PARITY_NONE = 0,
	SERIAL_PARITY_ODD,
	SERIAL_PARITY_EVEN = 3,
} IOCTL_SERIAL_PARITY_BITS;

typedef enum
{
	FLOW_CTRL_NONE = 0,
	FLOW_CTRL_HARDWARE,
	FLOW_CTRL_XON_XOFF,
} IOCTL_SERIAL_FLOW_CTRL;

/* TYPE DECLARATIONS */
typedef struct _GS2W_IOCTL_PKT
{
	u8 Indicator;
	u8 Reserved;
	u8 DataLen;
	u8 IoCtrlType;
	union
	{
		/* Set DTR/RTS/Break/Xon_off/hardware flow control */
		struct
		{
			u8 CtrlValue;
			u8 AlignBytes[1];
		}SetHardFlow;
		
		/* Set baudrate */
		struct
		{
			u8 BaudrateIndex;
			
			/* LSb */
			u8 DataBits: 2;		  
			u8 StopBits: 1;			
			u8 ParityBits: 3;			
			u8 Reserved: 2;
			/* MSb */
			
		}SetBaudRate;
	} Data;
	
} GS2W_IOCTL_PKT;

typedef struct _GS2W_CMD_MODE
{
	u8	Enable;
	u8	MatchedIndex;
	u8	Pattern[10];
	u32	LastTime;
} GS2W_CMD_MODE;

/* GLOBAL VARIABLES */
extern GS2W_CMD_MODE GS2W_CmdMode;

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void GS2W_Init(void);
u8 GS2W_GetTaskState(void);
void GS2W_SetTaskState(u8 state);
u8 GS2W_GetConnType(void);
u32 GS2W_CheckDestIP(void);
u8 GS2W_CongestionCheck(u16 apPort, u16 length);
u8 GS2W_CmdModeSwitchCheck(GS2W_CMD_MODE *pCmdMode, u8 *pStr, u16 len);
u8 GS2W_CmdModeIsEnable(GS2W_CMD_MODE *pCmdMode);

extern u32 MBI_Baudrate;                // Craig 20170823
#endif /* End of GS2W_H */
