
#ifndef MBI_CommandProcess_H
#define MBI_CommandProcess_H

#include <stdint.h>                      /* standard types definitions                      */

#define Cmd00	0x00		//NULL Cmd
#define Cmd12	0x12		//SA: Server	DA:	Client
#define Cmd13 	0x13		//SA: Client	DA:Server
#define Cmd15	0x15		//SA: Server	DA:	Client
#define Cmd16	0x16		//SA: Client	DA:Server
#define Cmd19	0x19		//SA: Server	DA:	Client
#define Cmd1A	0x1A		//SA: Client	DA:Server
#define Cmd1B   0x1B            //SA: Server	DA:	Client
#define Cmd1C   0x1C            //SA: Client	DA:Server
#define Cmd10	0x10		//SA: Server	DA:	Client
#define Cmd11	0x11		//SA: Client	DA:Server
#define Cmd20	0x20		//SA: Server	DA:	Client
#define Cmd21	0x21		//SA: Server	DA:	Client
#define Cmd22	0x22		//SA: Server	DA:	Client
#define Cmd23	0x23		//SA: Server	DA:	Client
#define Cmd25	0x25		//SA: Server	DA:	Client

#define Cmd12Len 76
#define Cmd13Len 100
#define Cmd15Len 48
#define Cmd16Len 256
#define Cmd19Len 48
#define Cmd1ALen 72
#define Cmd1BLen 52
#define Cmd1CLen 72
#define Cmd10Len 64
#define Cmd11Len 1025
#define Cmd20Len 192
#define Cmd21Len 192
#define Cmd22Len 192
#define Cmd23Len 184
#define Cmd25Len 60            //Edit by Hsinhua


/*Command Format Struct*/
typedef struct CommandPayload{
	uint8_t MessageType;
	uint8_t MessageCode;
	uint16_t MessageCount;
	uint16_t MessageLength;
	uint16_t MessageChkSum;
	uint32_t MemoryStartAddress;
}Command20Fore;

typedef struct Command16Content{
 uint16_t ACK_ResponseNo;
 uint16_t NACK_ResponseNo;
 uint16_t ERROR_ResponseNo;
 uint16_t WAIT_ResponseNo;
 uint16_t WAIT_Timestamp;
 uint8_t Reserved[14];
 uint8_t MAC_Address[6];
 uint8_t IP_Address[16];
 uint8_t IPv4_Subnetmask[4];
 uint8_t DHCP_Status;
 uint8_t AP_StationMode;
 uint8_t SSID[32];
 uint8_t EncryptionMode;
 uint8_t EncryptionIndex;
 uint8_t PassWord[64];
 uint8_t Wifi_Channel;
 uint8_t TxPower;
 uint8_t Rx_LQI_RSSI;
 uint8_t CodeVersion[16];
 uint8_t TimeZone;
 uint8_t PresentSec;
 uint8_t PresentMin;
 uint8_t PresentHour;
 uint8_t PresentWeek;
 uint8_t PresentDate;
 uint8_t PresentMonth;
 uint16_t PresentYear;
 uint8_t Date_ReqMode;
 uint16_t Duration;
 uint16_t TempSensor;
 uint16_t HumiditySensor;
 uint16_t LightSensor;
 uint16_t PM25;
 uint16_t Smoke;
 uint16_t CO_CO2;
 uint8_t ReservedCmd16[15];
}Command16Content;

typedef struct Command20Content{
  uint8_t Parameter_GroupSel[16]; 
  uint8_t IPv4_IPv6Type;
  uint8_t IPv4_IPv6Address1[4];
  uint8_t IPv6_Address2[12];
  uint8_t IPv4_IPv6SubnetMask[4];
  uint8_t EnableDHCP;
  uint8_t AP_StationMode;
  uint8_t SSID[32];
  uint8_t EncryptionMode;
  uint8_t EncryptionIndex;
  uint8_t Password[64];
  uint8_t WiFi_Channel;
  uint8_t Reserved[6];

  uint8_t SSID_len; // Eric added
  uint8_t Password_len; // Eric added
}Command20Content;


typedef struct Command21Content{
 uint8_t Parameter_GroupSel[16];  
 uint8_t Date_ReqMode;
 uint16_t Duration;
 uint8_t TxPower;
 uint8_t PowerSavingMode;
 uint8_t Reserved[123];
}Command21Content;

typedef struct Command22Content{
 uint8_t Parameter_GroupSel[16];
 uint8_t DataFormat;
 uint8_t Reserved[3];
 uint8_t Direction_GPIOA;
 uint8_t Direction_GPIOB;
 uint8_t Direction_GPIOC;
 uint8_t Direction_GPIOD;
 uint8_t Setting_GPIOA;
 uint8_t Setting_GPIOB;
 uint8_t Setting_GPIOC;
 uint8_t Setting_GPIOD;
 uint8_t Reserved116[116];
}Command22Content;

typedef struct Command23Content{
 uint8_t IP_Address[16];
 uint8_t IPv4_Subnetmask[4];
 uint8_t Enable_DHCP;
 uint8_t AP_StationMode;
 //uint8_t SSID[32];
 uint8_t SSID[33];
 uint8_t EncryptionMode;
 uint8_t EcryptionIndex;
 uint8_t Password[64];
 uint8_t Wifi_Channel;
 uint8_t TxPower;
 uint8_t TimeZone;
 uint8_t PresentSec;
 uint8_t PresentMin;
 uint8_t PresentHour;
 uint8_t PresentWeek;
 uint8_t PresentDate;
 uint8_t PresentMonth;
 uint16_t PresentYear;
 uint8_t Date_ReqMode;
 uint16_t Duration;
 uint16_t Reserved;
 
 uint8_t SSID_len;      // Eric added
 uint8_t Password_len; // Eric added
}Command23Content;

typedef struct Command25Content{
 uint8_t TimeZone;
 uint8_t PresentSec;
 uint8_t PresentMin;
 uint8_t PresentHour;
 uint8_t PresentWeek;
 uint8_t PresentDate;
 uint8_t PresentMonth;
 uint16_t PresentYear;
}Command25Content;

//Declaretion of all the variable
/*Command 12 */
extern uint8_t	Disconnect_Connect_Setting_Cmd12;
extern uint8_t	MAC_AddressFromReq_Cmd12[6];
extern uint8_t	IPv4_IPv6Type_Cmd12;
extern uint8_t	IPv4_IPv6Address1_Cmd12[4];
extern uint8_t	IPv6_Address2_Cmd12[12];
extern uint16_t	IP_PortNo_Cmd12;
extern uint16_t	Reserved_Cmd12;
/*Command 13 */
extern uint16_t	ACK_ResponseNo;
extern uint16_t	NACK_ResponseNo;
extern uint16_t	ERROR_ResponseNo;
extern uint16_t	WAIT_ResponseNo;
extern uint16_t	WAIT_Timestamp;
extern uint8_t	Reserved58_71[14];
		
		
extern uint8_t	DisConnect_Connect_Ack_Cmd13;
extern uint8_t	MAC_AddressFromReq_Cmd13[6];
extern uint8_t	IPv4_IPv6Type_Cmd13;
extern uint8_t	IPv4_IPv6Address1_Cmd13[4];
extern uint8_t	IPv6_Address2_Cmd13[12];
extern uint16_t	IP_PortNo_Cmd13;
extern uint16_t	Reserved2_Cmd13;


extern Command16Content Cmd16Content;
extern Command20Content Cmd20Content;
extern Command21Content Cmd21Content;
extern Command22Content Cmd22Content;
//extern Command23Content Cmd23Content;
extern Command25Content Cmd25Content;

//extern uint8_t ArbitraryData[??];

void CommandToDo(uint8_t CmdCode,uint8_t* buf,uint8_t iptype);

/*Send to Host command processes with a command return*/
void ReceiveCmd12(uint8_t* buf);
void ReceiveCmd15(uint8_t* buf);
void ReceiveCmd19(uint8_t* buf);
void ReceiveCmd10(uint8_t* buf);
void ReceiveCmd1B(uint8_t* buf);

/*Send to Host command processes without a command return*/
void ReceiveCmd20(uint8_t* buf,Command20Content* Command20);
void ReceiveCmd21(uint8_t* buf,Command21Content* Command21);
void ReceiveCmd22(uint8_t* buf,Command22Content* Command22);
void ReceiveCmd23(uint8_t* buf,Command23Content* Command23);
void ReceiveCmd25(uint8_t* buf,Command25Content* Command25);

/*Receive from host to do*/
void SendCommand13();
void SendCommand16(Command16Content* Command16);
void SendCommand1A();
void SendCommand11();
void SendCommand1C();


/*Command Headquater*/
void ReceiveCommandFactory(uint8_t CmdCode,uint8_t* buf, uint8_t iptype);

void SendCommandFactory(uint8_t CmdCode);


// Edit by Hsinhua, Add TEMP_sensor, 0820---------------------------
typedef struct _TEMP_SENSOR
{
	float	Temperature;
	float	Humidity;	
} TEMP_SENSOR;

extern uint16_t	ADC_data_0, ADC_data_1, ADC_data_2, ADC_data_3;

#endif /* xxx_H */


