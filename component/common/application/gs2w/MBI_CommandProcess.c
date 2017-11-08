#include "device.h"
#include "serial_api.h"
#include "sockets.h"
#include "api.h"
#include "main.h"
#include "gconfig.h"
#include "gconf_ap.h"
#include "gudpdat.h"
#include "guart.h"
#include "gs2w.h"
#include "platform_opts.h"
#include "gconf_ap.h"
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include "misc.h"
#include "rtc.h"                       // Edit by Hsinhua, Add RTC, 0727---------------------------
#include "MBI_CommandIdentify.h"

uint16_t MBI_FW_VER = 0x0102;//fw ver. 1.2

uint16_t	ACK_ResponseNo;
uint16_t	NACK_ResponseNo;
uint16_t	ERROR_ResponseNo;
uint16_t	WAIT_ResponseNo;
uint16_t	WAIT_Timestamp;
uint8_t	Reserved58_71[14]={0};


uint8_t	Disconnect_Connect_Setting_Cmd12;
uint8_t	MAC_AddressFromReq_Cmd12[6];
uint8_t	IPv4_IPv6Type_Cmd12;
uint8_t	IPv4_IPv6Address1_Cmd12[4];
uint8_t	IPv6_Address2_Cmd12[12];
uint16_t	IP_PortNo_Cmd12;
uint16_t	Reserved_Cmd12;

uint8_t	DisConnect_Connect_Ack_Cmd13;
uint8_t	MAC_AddressFromReq_Cmd13[6];
uint8_t	IPv4_IPv6Type_Cmd13;
uint8_t	IPv4_IPv6Address1_Cmd13[4];
uint8_t	IPv6_Address2_Cmd13[12];
uint16_t	IP_PortNo_Cmd13;
uint16_t	Reserved2_Cmd13;

Command16Content Cmd16Content;
Command20Content Cmd20Content;
Command21Content Cmd21Content;
Command22Content Cmd22Content;
//Command23Content Cmd23Content;
Command25Content Cmd25Content;
//uint8_t ArbitraryData[??];

xQueueHandle RTC_Queue;


// Edit by Hsinhua, Add TEMP_sensor, 0820---------------------------
xQueueHandle tempQueue;
//uint16_t	ADC_data[3];


extern void cmd_wifi_on(int argc, char **argv);
extern void cmd_wifi_off(int argc, char **argv);

void systemReboot(int argc, char **argv);



void CommandToDo(uint8_t CmdCode,uint8_t* buf,uint8_t iptype){
  ReceiveCommandFactory(CmdCode,buf,iptype);
}

void ReceiveCommandFactory(uint8_t CmdCode,uint8_t* buf,uint8_t iptype)
{  
  if (iptype == 0){//UDP
      printf("Receive UDP pkt \r\n");
    
      switch(CmdCode){
                case Cmd12:
			ReceiveCmd12(buf);
			SendCommandFactory(Cmd13);
			/*
				When receive Cmd12 need to do 
			*/
			break;
		case Cmd15:
			ReceiveCmd15(buf);
			SendCommandFactory(Cmd16);
			/*
				When receive Cmd15 need to do 
			*/
			break;
		case Cmd19:
			ReceiveCmd19(buf);
			SendCommandFactory(Cmd1A);
			/*
				When receive Cmd19 need to do 
			*/
			break;
		case Cmd10:
			ReceiveCmd10(buf);
			SendCommandFactory(Cmd11);
			/*
				When receive Cmd10 need to do 
			*/
			break;
		case Cmd20:
                  printf("ReceiveCommandFactory -> ReceiveCmd20 \r\n");
			ReceiveCmd20(buf,&Cmd20Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd20 need to do 
			*/
			break;
		case Cmd21:
			ReceiveCmd21(buf,&Cmd21Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd21 need to do 
			*/
			break;
		case Cmd22:
			ReceiveCmd22(buf,&Cmd22Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd22 need to do 
			*/
			break;
		case Cmd23:
			ReceiveCmd23(buf,&gconfig_ConfigData.MBI_FactorySettings);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd23 need to do 
			*/
			break;
		case Cmd25:
			ReceiveCmd25(buf,&Cmd25Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd25 need to do 
			*/
			break;
                case Cmd1B:
                          ReceiveCmd1B(buf);
                          SendCommandFactory(Cmd1C);
                        break;
		default:
			SendCommandFactory(Cmd00);
			break;
      }
  }
  else{//TCP
      switch(CmdCode){
		case Cmd12:
			ReceiveCmd12(buf);
			SendCommandFactory(Cmd13);
			/*
				When receive Cmd12 need to do 
			*/
			break;
		case Cmd15:
			ReceiveCmd15(buf);
			SendCommandFactory(Cmd16);
			/*
				When receive Cmd15 need to do 
			*/
			break;
		case Cmd19:
			ReceiveCmd19(buf);
			SendCommandFactory(Cmd1A);
			/*
				When receive Cmd19 need to do 
			*/
			break;
		case Cmd10:
			ReceiveCmd10(buf);
			SendCommandFactory(Cmd11);
			/*
				When receive Cmd10 need to do 
			*/
			break;
		case Cmd20:
                  printf("ReceiveCommandFactory -> ReceiveCmd20 \r\n");
			ReceiveCmd20(buf,&Cmd20Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd20 need to do 
			*/
			break;
		case Cmd21:
			ReceiveCmd21(buf,&Cmd21Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd21 need to do 
			*/
			break;
		case Cmd22:
			ReceiveCmd22(buf,&Cmd22Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd22 need to do 
			*/
			break;
		case Cmd23:
			ReceiveCmd23(buf,&gconfig_ConfigData.MBI_FactorySettings);      //20170731 Craig Write the MBI factory param. into the ROM
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd23 need to do 
			*/
			break;
		case Cmd25:
			ReceiveCmd25(buf,&Cmd25Content);
                        SendCommandFactory(Cmd00);
			/*
				When receive Cmd25 need to do 
			*/
			break;
                case Cmd1B:
                          ReceiveCmd1B(buf);
                          SendCommandFactory(Cmd1C);
                        break;
		default:
			SendCommandFactory(Cmd00);
			break;
      }
  }
}

void ClearTxCmdCode(void)
{
    UDPTxBuffer[37] = Cmd00;
}

void SendCommandFactory(uint8_t CmdCode){
	switch(CmdCode){
            case Cmd00:
              ClearTxCmdCode();
              break;
            case Cmd13:
              SendCommand13();
              break;
            case Cmd16:
              SendCommand16(&Cmd16Content);
              break;
            case Cmd1A:
              SendCommand1A();
              break;
            case Cmd11:
              SendCommand11();
              break;
            case Cmd1C:
              SendCommand1C();
              break;
            default:
              printf("Error command code!!");
              break;
	}
}

void SendCommand13(){		//Connection Reply
	uint8_t *px;
        u8 *mac = GCONFIG_GetMacAddress();
	uint16_t CmdLen;
        uint16_t index;
	uint8_t i;
	
	PacketTotLen=Cmd13Len;
	CmdLen=PacketTotLen-36;
	
	GetCommandLength(Cmd13);

	GetPacketHeaderChkSum();
	
	px=GetPacketFormat();
	

	for(index=0;index<36;index++)
		UDPTxBuffer[index]=*(px+index);
	index=36;
	UDPTxBuffer[index++]=0x00;
	UDPTxBuffer[index++]=Cmd13;
	UDPTxBuffer[index++]=0x00;
	UDPTxBuffer[index++]=0x00;
	UDPTxBuffer[index++]=(CmdLen>>8);
	UDPTxBuffer[index++]=(CmdLen&0x00FF);
	UDPTxBuffer[index++]=(CommandChkSum>>8);
	UDPTxBuffer[index++]=(CommandChkSum&0x00FF);
	UDPTxBuffer[index++]=0;//MemoryStartAddress[0];
	UDPTxBuffer[index++]=0;//MemoryStartAddress[1];
	UDPTxBuffer[index++]=0;//MemoryStartAddress[2];
	UDPTxBuffer[index++]=0;//MemoryStartAddress[3];
	UDPTxBuffer[index++]=0;//(ACK_ResponseNo>>8);
	UDPTxBuffer[index++]=1;//(ACK_ResponseNo&0x00FF);
	UDPTxBuffer[index++]=0;//(NACK_ResponseNo>>8);
	UDPTxBuffer[index++]=0;//(NACK_ResponseNo&0x00FF);
	UDPTxBuffer[index++]=0;//(ERROR_ResponseNo>>8);
	UDPTxBuffer[index++]=0;//(ERROR_ResponseNo&0x00FF);
	UDPTxBuffer[index++]=0;//(WAIT_ResponseNo>>8);
	UDPTxBuffer[index++]=0;//(WAIT_ResponseNo&0x00FF);
	UDPTxBuffer[index++]=0;//(WAIT_Timestamp>>8);
	UDPTxBuffer[index++]=0;//(WAIT_Timestamp&0x00FF);
	for(i=0;i<14;i++)
		UDPTxBuffer[index++] = 0;//Reserved58_71[i];
	UDPTxBuffer[index++] = 0;//Disconnect_Connect_Setting_Cmd12;
	for(i=0;i<6;i++,index++)
		UDPTxBuffer[index] = mac[i];//MAC_AddressFromReq_Cmd13[i];
	UDPTxBuffer[index++] = 0;//only ip4
	for(i=0;i<4;i++,index++)
		UDPTxBuffer[index] = IPv4_IPv6Address1_Cmd13[i];
	for(i=0;i<12;i++,index++)
		UDPTxBuffer[index] = 0;
	UDPTxBuffer[index++] = (IP_PortNo_Cmd13>>8);
	UDPTxBuffer[index++] = (IP_PortNo_Cmd13&0x00FF);
	UDPTxBuffer[index++] = MBI_FW_VER >> 8;
	UDPTxBuffer[index]   = MBI_FW_VER &0xff;
	
	CommandChkSum=GetRearCommandChkSum(UDPTxBuffer);
	index=42;
	UDPTxBuffer[index++] = (CommandChkSum>>8);
	UDPTxBuffer[index] = (CommandChkSum&0xFF);
	
	printf("Process Command13\n");
}

void SendCommand16(Command16Content *Command16){		//Get Wi-fi Module Informaion Reply
        u8 *mac = GCONFIG_GetMacAddress();
        u32 Netmask = GCONFIG_GetNetmask();
	uint8_t *px;
        uint16_t CmdLen;
	uint16_t index;
        uint8_t i;
	
	px=GetPacketFormat();
        PacketTotLen=Cmd16Len;
	CmdLen=PacketTotLen-36;
	
	GetCommandLength(Cmd16);

	GetPacketHeaderChkSum();
	
	px=GetPacketFormat();
        
          // Edit by Hsinhua, Add RTC, 0727---------------------------
        RTC_TIME time_2;
        if( xQueueReceive( RTC_Queue, &time_2, 100/portTICK_RATE_MS ) == pdPASS)  
        {  
            Command16->PresentSec = time_2.Second;
            Command16->PresentMin = time_2.Minute;
            Command16->PresentHour = time_2.Hour;
            Command16->PresentWeek = time_2.Day;
            Command16->PresentDate= time_2.Date;
            Command16->PresentMonth = time_2.Month;
            Command16->PresentYear = time_2.Year;
         }
        
         //  Edit by Hsinhua, Add TEMP_sensor, 0820---------------------------
        TEMP_SENSOR temp_2; 
        uint16_t temp_u16;
        uint16_t hum_u16;
        if( xQueueReceive( tempQueue, &temp_2, 100/portTICK_RATE_MS ) == pdPASS)  
         {    
           temp_u16 = (uint16_t)temp_2.Temperature;
           hum_u16 = (uint16_t)temp_2.Humidity;
           Command16->TempSensor = temp_u16; 
           Command16->HumiditySensor = hum_u16;
          //  printf("\n Temperature = %.2f C \n", temp_2.Temperature);
          //  printf("\n Relative Humidity = %.2f \n", temp_2.Humidity);
         }
        Command16->LightSensor = ADC_data_3;
        
        
        
        Command16->ACK_ResponseNo=0;
        Command16->NACK_ResponseNo=0;
        Command16->ERROR_ResponseNo=0;
        Command16->WAIT_ResponseNo=0;
        Command16->WAIT_Timestamp=0;
        for(i=0;i<14;i++)
          Command16->Reserved[i]=0;
        for(i=0;i<6;i++)
          Command16->MAC_Address[i]=mac[i];
        for(i=0;i<12;i++)
          Command16->IP_Address[i] = Cmd20Content.IPv4_IPv6Address1[i]; 
        
        Command16->IPv4_Subnetmask[0] = (Netmask & 0xFF);
        Command16->IPv4_Subnetmask[1] = ((Netmask & 0xFF00)>>8);
        Command16->IPv4_Subnetmask[2] = ((Netmask & 0xFF0000)>>16);
        Command16->IPv4_Subnetmask[3] = ((Netmask & 0xFF000000)>>24);
        Command16->DHCP_Status = gconfig_ConfigData.DhcpSrv.Status;
        Command16->AP_StationMode = gconfig_ConfigData.WifiConfig.WifiNetworkMode;
        for(i=0;i<32;i++)
          Command16->SSID[i] = gconfig_ConfigData.WifiConfig.WifiSsid[i];
        Command16->EncryptionMode = gconfig_ConfigData.WifiConfig.WifiEncryptMode;
        Command16->EncryptionIndex = Cmd20Content.EncryptionIndex;
        for(i=0;i<64;i++)
          Command16->PassWord[i] = Cmd20Content.Password[i];
        Command16->Wifi_Channel = gconfig_ConfigData.WifiConfig.WifiChannel;
        Command16->TxPower = Cmd21Content.TxPower;
        Command16->Rx_LQI_RSSI = 0;
        for(i=0;i<16;i++)
          Command16->CodeVersion[i]=i;
        Command16->TimeZone = Cmd25Content.TimeZone;
        Command16->PresentSec = Cmd25Content.PresentSec;
        Command16->PresentMin = Cmd25Content.PresentMin;
        Command16->PresentHour = Cmd25Content.PresentHour;
        Command16->PresentWeek = Cmd25Content.PresentWeek;
        Command16->PresentDate = Cmd25Content.PresentDate;
        Command16->PresentMonth = Cmd25Content.PresentMonth;
        Command16->PresentYear = Cmd25Content.PresentYear;
        Command16->Date_ReqMode = Cmd21Content.Date_ReqMode;
        Command16->Duration=Cmd21Content.Duration;
        //Command16->TempSensor=0.;
        //Command16->HumiditySensor=0;
        //Command16->LightSensor=0;
        //Command16->PM25=0;
        Command16->Smoke=0;
        //Command16->CO_CO2=0;
        for(i=0;i<15;i++)
          Command16->ReservedCmd16[i]=0;
        

        if(Command16->Duration==0 || gconfig_ConfigData.MBI_FactorySettings.Duration==0){
          for(index=0;index<36;index++)
            UDPTxBuffer[index]=*(px+index);
            index=36;
            UDPTxBuffer[index++]=0x00;
            UDPTxBuffer[index++]=Cmd16;
            UDPTxBuffer[index++]=0x00;
            UDPTxBuffer[index++]=0x00;
            UDPTxBuffer[index++]=(CmdLen>>8);
            UDPTxBuffer[index++]=(CmdLen&0x00FF);
            UDPTxBuffer[index++]=(CommandChkSum>>8);
            UDPTxBuffer[index++]=(CommandChkSum&0x00FF);
            UDPTxBuffer[index++]=MemoryStartAddress[0];
            UDPTxBuffer[index++]=MemoryStartAddress[1];
            UDPTxBuffer[index++]=MemoryStartAddress[2];
            UDPTxBuffer[index++]=MemoryStartAddress[3];
            UDPTxBuffer[index++]=(Command16->ACK_ResponseNo>>8);
            UDPTxBuffer[index++]=(Command16->ACK_ResponseNo&0x00FF);
            UDPTxBuffer[index++]=(Command16->NACK_ResponseNo>>8);
            UDPTxBuffer[index++]=(Command16->NACK_ResponseNo&0x00FF);
            UDPTxBuffer[index++]=(Command16->ERROR_ResponseNo>>8);
            UDPTxBuffer[index++]=(Command16->ERROR_ResponseNo&0x00FF);
            UDPTxBuffer[index++]=(Command16->WAIT_ResponseNo>>8);
            UDPTxBuffer[index++]=(Command16->WAIT_ResponseNo&0x00FF);
            UDPTxBuffer[index++]=(Command16->WAIT_Timestamp>>8);
            UDPTxBuffer[index++]=(Command16->WAIT_Timestamp&0x00FF);
            for(i=0;i<14;i++,index++)
               UDPTxBuffer[index]=(Command16->Reserved[i]);
            for(i=0;i<6;i++,index++)
               UDPTxBuffer[index]=(Command16->MAC_Address[i]);
            for(i=0;i<16;i++,index++)
               UDPTxBuffer[index]=(Command16->IP_Address[i]);
            for(i=0;i<4;i++,index++)
               UDPTxBuffer[index]=(Command16->IPv4_Subnetmask[i]);
            UDPTxBuffer[index++]=Command16->DHCP_Status;
            UDPTxBuffer[index++]=Command16->AP_StationMode;
            for(i=0;i<32;i++,index++)
               UDPTxBuffer[index]=(Command16->SSID[i]);
            UDPTxBuffer[index++]=Command16->EncryptionMode;
            UDPTxBuffer[index++]=Command16->EncryptionIndex;
            for(i=0;i<64;i++,index++)
               UDPTxBuffer[index]=(Command16->PassWord[i]);
            UDPTxBuffer[index++]=Command16->Wifi_Channel;
            UDPTxBuffer[index++]=Command16->TxPower;
            UDPTxBuffer[index++]=Command16->Rx_LQI_RSSI;
            for(i=0;i<16;i++,index++)
               UDPTxBuffer[index]=(Command16->CodeVersion[i]);
            UDPTxBuffer[index++]=Command16->TimeZone;
            UDPTxBuffer[index++]=Command16->PresentSec;
            UDPTxBuffer[index++]=Command16->PresentMin;
            UDPTxBuffer[index++]=Command16->PresentHour;
            UDPTxBuffer[index++]=Command16->PresentWeek;
            UDPTxBuffer[index++]=Command16->PresentDate;
            UDPTxBuffer[index++]=Command16->PresentMonth;
            UDPTxBuffer[index++]=(Command16->PresentYear>>8);
            UDPTxBuffer[index++]=(Command16->PresentYear&0x00FF);
            UDPTxBuffer[index++]=Command16->Date_ReqMode;
            UDPTxBuffer[index++]=(Command16->Duration>>8);
            UDPTxBuffer[index++]=(Command16->Duration&0x00FF);
            UDPTxBuffer[index++]=(Command16->TempSensor>>8);
            UDPTxBuffer[index++]=(Command16->TempSensor&0x00FF);
            UDPTxBuffer[index++]=(Command16->HumiditySensor>>8);
            UDPTxBuffer[index++]=(Command16->HumiditySensor&0x00FF);
            UDPTxBuffer[index++]=(Command16->LightSensor>>8);
            UDPTxBuffer[index++]=(Command16->LightSensor&0x00FF);
            UDPTxBuffer[index++]=(Command16->PM25>>8);
            UDPTxBuffer[index++]=(Command16->PM25&0x00FF);
            UDPTxBuffer[index++]=(Command16->Smoke>>8);
            UDPTxBuffer[index++]=(Command16->Smoke&0x00FF);
            UDPTxBuffer[index++]=(Command16->CO_CO2>>8);
            UDPTxBuffer[index++]=(Command16->CO_CO2&0x00FF);
            for(i=0;i<15;i++,index++)
               UDPTxBuffer[index]=(Command16->ReservedCmd16[i]);
        }
        if(Command16->Duration!=0 || gconfig_ConfigData.MBI_FactorySettings.Duration!=0){
          for(index=0;index<36;index++)
            Cmd16Buffer[index]=*(px+index);
            index=36;
            Cmd16Buffer[index++]=0x00;
            Cmd16Buffer[index++]=Cmd16;
            Cmd16Buffer[index++]=0x00;
            Cmd16Buffer[index++]=0x00;
            Cmd16Buffer[index++]=(CmdLen>>8);
            Cmd16Buffer[index++]=(CmdLen&0x00FF);
            Cmd16Buffer[index++]=(CommandChkSum>>8);
            Cmd16Buffer[index++]=(CommandChkSum&0x00FF);
            Cmd16Buffer[index++]=MemoryStartAddress[0];
            Cmd16Buffer[index++]=MemoryStartAddress[1];
            Cmd16Buffer[index++]=MemoryStartAddress[2];
            Cmd16Buffer[index++]=MemoryStartAddress[3];
            Cmd16Buffer[index++]=(Command16->ACK_ResponseNo>>8);
            Cmd16Buffer[index++]=(Command16->ACK_ResponseNo&0x00FF);
            Cmd16Buffer[index++]=(Command16->NACK_ResponseNo>>8);
            Cmd16Buffer[index++]=(Command16->NACK_ResponseNo&0x00FF);
            Cmd16Buffer[index++]=(Command16->ERROR_ResponseNo>>8);
            Cmd16Buffer[index++]=(Command16->ERROR_ResponseNo&0x00FF);
            Cmd16Buffer[index++]=(Command16->WAIT_ResponseNo>>8);
            Cmd16Buffer[index++]=(Command16->WAIT_ResponseNo&0x00FF);
            Cmd16Buffer[index++]=(Command16->WAIT_Timestamp>>8);
            Cmd16Buffer[index++]=(Command16->WAIT_Timestamp&0x00FF);
            for(i=0;i<14;i++,index++)
              Cmd16Buffer[index]=(Command16->Reserved[i]);
            for(i=0;i<6;i++,index++)
              Cmd16Buffer[index]=(Command16->MAC_Address[i]);
            for(i=0;i<16;i++,index++)
              Cmd16Buffer[index]=(Command16->IP_Address[i]);
            for(i=0;i<4;i++,index++)
              Cmd16Buffer[index]=(Command16->IPv4_Subnetmask[i]);
            Cmd16Buffer[index++]=Command16->DHCP_Status;
            Cmd16Buffer[index++]=Command16->AP_StationMode;
            for(i=0;i<32;i++,index++)
               Cmd16Buffer[index]=(Command16->SSID[i]);
            Cmd16Buffer[index++]=Command16->EncryptionMode;
            Cmd16Buffer[index++]=Command16->EncryptionIndex;
            for(i=0;i<64;i++,index++)
               Cmd16Buffer[index]=(Command16->PassWord[i]);
            Cmd16Buffer[index++]=Command16->Wifi_Channel;
            Cmd16Buffer[index++]=Command16->TxPower;
            Cmd16Buffer[index++]=Command16->Rx_LQI_RSSI;
            for(i=0;i<16;i++,index++)
               Cmd16Buffer[index]=(Command16->CodeVersion[i]);
            Cmd16Buffer[index++]=Command16->TimeZone;
            Cmd16Buffer[index++]=Command16->PresentSec;
            Cmd16Buffer[index++]=Command16->PresentMin;
            Cmd16Buffer[index++]=Command16->PresentHour;
            Cmd16Buffer[index++]=Command16->PresentWeek;
            Cmd16Buffer[index++]=Command16->PresentDate;
            Cmd16Buffer[index++]=Command16->PresentMonth;
            Cmd16Buffer[index++]=(Command16->PresentYear>>8);
            Cmd16Buffer[index++]=(Command16->PresentYear&0x00FF);
            Cmd16Buffer[index++]=Command16->Date_ReqMode;
            Cmd16Buffer[index++]=(Command16->Duration>>8);
            Cmd16Buffer[index++]=(Command16->Duration&0x00FF);
            Cmd16Buffer[index++]=(Command16->TempSensor>>8);
            Cmd16Buffer[index++]=(Command16->TempSensor&0x00FF);
            Cmd16Buffer[index++]=(Command16->HumiditySensor>>8);
            Cmd16Buffer[index++]=(Command16->HumiditySensor&0x00FF);
            Cmd16Buffer[index++]=(Command16->LightSensor>>8);
            Cmd16Buffer[index++]=(Command16->LightSensor&0x00FF);
            Cmd16Buffer[index++]=(Command16->PM25>>8);
            Cmd16Buffer[index++]=(Command16->PM25&0x00FF);
            Cmd16Buffer[index++]=(Command16->Smoke>>8);
            Cmd16Buffer[index++]=(Command16->Smoke&0x00FF);
            Cmd16Buffer[index++]=(Command16->CO_CO2>>8);
            Cmd16Buffer[index++]=(Command16->CO_CO2&0x00FF);
            for(i=0;i<15;i++,index++)
               Cmd16Buffer[index]=(Command16->ReservedCmd16[i]);
         }
	printf("Process Command16\n");	
}

void SendCommand1A(){		//Echo Reply
	uint16_t Cmd1ATotLen=1024;
	uint8_t *px;
	uint8_t index;
	
	PacketTotLen=Cmd1ATotLen;
	px=GetPacketFormat();
	/*
		ACK_ResponseNo=
		NACK_ResponseNo=
		ERROR_ResponseNo=
		WAIT_ResponseNo=
		WAIT_Timestamp=
		Reserved=
		
		
		ArbitraryData=
	*/
	printf("Process Command1A\n");
}

void SendCommand11(){		//Memory Data Reply
	uint8_t *px;
	uint8_t index;
	
	px=GetPacketFormat();
	/*
		ACK_ResponseNo=
		NACK_ResponseNo=
		ERROR_ResponseNo=
		WAIT_ResponseNo=
		WAIT_Timestamp=
		Reserved=
		
	
		Memory_DataLen=
		Memory_Content=	
	*/
	printf("Process Command11\n");
}


/*The command wifi received*/
void ReceiveCmd12(uint8_t* buf){		//Connection Request
	/*
		Disconnect_Connect=
		MAC_Address=
		IPv4_IPv6Type=
		IPv4_IPv6Address1=
		IPv6Address2=
		IP_PortNo=
		
	*/
    uint8_t index=0;
	uint8_t i;
        
	index=48;
	Disconnect_Connect_Setting_Cmd12=*(buf+index++);
	for(i=0;i<6;i++)
		MAC_AddressFromReq_Cmd12[i]=*(buf+i+index++);
	IPv4_IPv6Type_Cmd12=*(buf+index++);
	for(i=0;i<4;i++)
		IPv4_IPv6Address1_Cmd12[i]=*(buf+i+index++);
	for(i=0;i<12;i++)
		IPv6_Address2_Cmd12[i]=*(buf+i+index++);
	IP_PortNo_Cmd12=(*(buf+index)<<8)|*(buf+(index+1));index+=2;
	Reserved_Cmd12=(*(buf+index)<<8)|*(buf+(index+1));
}

void ReceiveCmd15(uint8_t* buf){		//Get Device Information Request
	/*
		TODO
	*/
	printf("Received Command15\n");
}

void ReceiveCmd19(uint8_t* buf){		//Echo Request
	/*
		ArbitraryData=
	*/
	printf("Received Command19\n");
}

void ReceiveCmd10(uint8_t* buf){		//Memory Data Request
	/*
		Memory_DataLen=
		Memory_DeviceName=
		Memory_DeviceNo=
		Reserved=
	*/
	printf("Received Command10\n");
}


void cmd20_wifi_connect(Command20Content* Command20)
{
        int     ret = RTW_ERROR;
	unsigned long	tick1 = xTaskGetTickCount();
	unsigned long	tick2;
	int				mode;
	//char 			*ssid;
	
	void			*semaphore;
	char			wifi_mode_change = 0;
        char argc;
        char temp[32]={0}; 
        char essid[32]={0}; 
        char i=0;
        char p=0;
        char ap_sta_mode = 0;
        
        u32 ip;
        
        u8 ip_str_len=0;
        u16 temp16;  
        
        
       // in-use
        u32 addr = 0;
        u32 temp32 = 0; 
	rtw_security_t	security_type;
	int 			ssid_len;        
        int 			key_id;     
        char 			*password;
        u8			password_len;
        
        
        

        cmd_wifi_off(2,Command20->SSID);
        cmd_wifi_on(2,Command20->SSID);

        if (rltk_wlan_running(WLAN0_IDX) == 0)
	{
		printf("\n\rReject command due to Wifi is turned off now");
		return;
	}

	MISC_EnterWiFiBusySection();

        
	//Check if in AP mode
	wext_get_mode(WLAN0_NAME, &mode);

	if(mode == IW_MODE_MASTER)
	{
#if CONFIG_LWIP_LAYER
		DHCPS_DeInit();
#endif
		wifi_off();
		vTaskDelay(20);
		if (wifi_on(RTW_MODE_STA) < 0)
		{
			printf("\n\rERROR: Wifi on failed!");
			goto cmd20_wifi_connect_exit;
		}
	}
    
        
        
        
        
        
        // STA mode        
        if (Cmd20Content.AP_StationMode == 1) // MODE = 1(STA), 2(AP)
        {
          
          temp16 = GCONFIG_GetNetwork();

          temp16 &= ~GCONFIG_NETWORK_DHCP_ENABLE; //clear DHCP_ENABLE bit
        
          //DHCP Enable or Disable  
          if (Cmd20Content.EnableDHCP == 1) 
            temp16 |= GCONFIG_NETWORK_DHCP_ENABLE; //enable DHCP_ENABLE bit
          else
            temp16 &= ~GCONFIG_NETWORK_DHCP_ENABLE; //disable DHCP_ENABLE bit

          GCONFIG_SetNetwork(temp16);

        }
	
        
        //get ipv4_ip        
        for(i=0;i<4;i++)
        {        
          temp32 = Command20->IPv4_IPv6Address1[i];
          temp32 = (temp32 << 8*i) ;
          addr += temp32;
        }
        
        printf("addr = 0x%08lx \r\n",addr);
        
        //get ipv6_ip
        
        
        printf("Cmd20Content.EncryptionMode = %d \r\n",Cmd20Content.EncryptionMode);
        
        
        ssid_len = strlen((const char *)Cmd20Content.SSID);
        
        // EncryptionMode 
	if(Cmd20Content.EncryptionMode == 0) // SECURITY:OPEN
	{
		security_type = RTW_SECURITY_OPEN;
		password = NULL;
		password_len = 0;
		key_id = 0;
		semaphore = NULL;
	}
        else if(Cmd20Content.EncryptionMode == 2) // SECURITY:WPA  (AES)
	{
		security_type = RTW_SECURITY_WPA2_AES_PSK;
		password = (char *)Cmd20Content.Password;
		password_len = Cmd20Content.Password_len;
		key_id = 0;
		if (password_len < 8 || password_len > 63)                
		{
			printf("\n\rWrong PSK length. Must between 8 ~ 63 ASCII characters.");
			goto cmd20_wifi_connect_exit;
		}
		semaphore = NULL;
	}
	else if(Cmd20Content.EncryptionMode == 1)
	{
		security_type = RTW_SECURITY_WEP_PSK; // SECURITY:WEP		
		password = (char *)Cmd20Content.Password;
                password_len = Cmd20Content.Password_len;
                key_id = Cmd20Content.EncryptionIndex;
		if(( password_len != 5) && (password_len != 13))
		{
			printf("\n\rWrong WEP key length. Must be 5 or 13 ASCII characters.");
			goto cmd20_wifi_connect_exit;
		}
		if((key_id < 0) || (key_id > 3))
		{
			printf("\n\rWrong WEP key id. Must be one of 0,1,2, or 3.");
			goto cmd20_wifi_connect_exit;
		}
		semaphore = NULL;
	}

        
        printf("Cmd20Content.SSID = %s \r\n",Cmd20Content.SSID);
        printf("ssid_len = %d \r\n",ssid_len);
        printf("\r\n");
        
	ret = wifi_connect(Cmd20Content.SSID,
					security_type,
					password,
					ssid_len,
					password_len,
					key_id,
					semaphore);

	tick2 = xTaskGetTickCount();
	printf("\r\nConnected after %dms.\n", (tick2-tick1));
        
	if(ret != RTW_SUCCESS)
	{
		printf("\n\rERROR: Operation failed!");
		goto cmd20_wifi_connect_exit;
	}
	else
	{
		// Save wifi parameters to gconfig 
		GCONFIG_SetWifiRfEnable(1);					// WIFI RF Enable 
                
		// Auto configure DHCP client and server 
		if (GCONFIG_GetWifiNetworkMode()==RTW_MODE_AP)
			wifi_mode_change = 1;
                
                // config to STA mode
		GCONFIG_SetWifiNetworkMode(RTW_MODE_STA);	// Network Mode 
		GCONFIG_SetWifiSsid(Cmd20Content.SSID, ssid_len);		// SSID 
		GCONFIG_SetWifiEncryptMode(security_type);	// Security Type 
                
		if (security_type == RTW_SECURITY_WEP_PSK)
		{
			GCONFIG_SetWifiWepKeyIndex(key_id);		// WEP Key Index 
			if (password_len == 5)
			{
                            GCONFIG_SetWifiWepKeyLength(0);		// WEP 64bits
                            GCONFIG_SetWifiWep64Key(key_id, (u8 *)Cmd20Content.Password);
                                
                         // for (i = 0; i < password_len; i ++)
                          //  pConfigData->WifiConfig.WifiWep64Key[key_id][i] = Cmd20Content.Password[i];
			}
			else
			{
                            GCONFIG_SetWifiWepKeyLength(1);		// WEP 128bits 
                            GCONFIG_SetWifiWep128Key(key_id, (u8 *)Cmd20Content.Password);
                           // for (i = 0; i < password_len; i ++)
                           //   pConfigData->WifiConfig.WifiWep128Key[key_id][i] = Cmd20Content.Password[i];
			}
		}
		else if (security_type == RTW_SECURITY_WPA2_AES_PSK)
		{
			GCONFIG_SetWifiPreShareKey(password, password_len);	// WPAx Pre-Shared Key 
		}

		// System reboot check 
		if (wifi_mode_change)  //ap to sta
		{
			// Enable DHCP client 
			//GCONFIG_SetNetwork(GCONFIG_GetNetwork() | GCONFIG_NETWORK_DHCP_ENABLE);

			// Disable DHCP server 
			GCONFIG_SetDhcpSrvStatus(0);
			GCONFIG_WriteConfigData();                        
                        
			systemReboot(0, 0);      
		}
		else
		{
			GCONFIG_WriteConfigData();
#if CONFIG_LWIP_LAYER
			// Start DHCP Client 
			ret = MISC_DHCP(0, DHCPC_START, 1);
#endif
			GPIOS_ResetLEDIndicator();
		}
	}

	if (ret != DHCPC_ERROR)
	{
		printf("\r\n\nGot IP after %dms.\n", (xTaskGetTickCount() - tick1));
	}     
        
       
        
cmd20_wifi_connect_exit:
	MISC_LeaveWiFiBusySection();
}


void ReceiveCmd20(uint8_t* buf, Command20Content* Command20){		// Edit by eric 0801
     
        uint8_t i;
        uint8_t index = 0;
        /*
		TODO
	*/

        printf("into  Command20, Cmd20Len = %d\n",Cmd20Len);
        
        for(i=0;i<Cmd20Len;i++)
          printf("%x ",*(buf+i));
        printf(" \r\n ");
        
        //index = 36;
        //Parameter group 1~16 selection
        for(i=0;i<16;i++)
          Command20->Parameter_GroupSel[i]= buf[48 + i];
        
        //IPv4 / IPv6 Type
        Command20->IPv4_IPv6Type=buf[64];
        printf("IPv4 / IPv6 Type: %s\n\r", (Command20->IPv4_IPv6Type == 0) ? "IPv4":"IPv6");

        //IPv4 / IPv6 Address1
        for(i=0;i<4;i++)
        {
          Command20->IPv4_IPv6Address1[i]= buf[65+i];
          printf("%x ", Command20->IPv4_IPv6Address1[i]);
        }
         printf(" \r\n ");
        printf("save Cmd20Content.IPv4_IPv6Address1 = 0x%lx \r\n",Cmd20Content.IPv4_IPv6Address1);
        
        
        //IPv6 Address2  *(Command20).
        for(i=0;i<12;i++)
          Command20->IPv6_Address2[i]= buf[69+i];
        printf("save Cmd20Content.IPv6_Address2 = %s \r\n",Cmd20Content.IPv6_Address2);
        
        //IPv4 / IPv6 Subnet Mask
        for(i=0;i<4;i++)
          Command20->IPv4_IPv6SubnetMask[i]= buf[81+i];
        printf("save Cmd20Content.IPv4_IPv6SubnetMask = %s \r\n",Cmd20Content.IPv4_IPv6SubnetMask);
        
        //Enable DHCP
        Command20->EnableDHCP = buf[85];
           printf("save Command20->EnableDHCP = %d \r\n",Command20->EnableDHCP);
           
        //AP / Station Mode
        Command20->AP_StationMode= buf[86];
         printf("save Command20->AP_StationMode = %d \r\n",Command20->AP_StationMode);
         
        //SSID         
        memset( Cmd20Content.SSID, 0, sizeof(Cmd20Content.SSID));
        Command20->SSID_len = 0;
        for(i=0;i<32;i++)
        {          
          if (buf[87+i] == '\n')
            break;
          else            
            Command20->SSID[i]= buf[87+i];
        }
        Command20->SSID_len = i;        
        printf("save  Command20->SSID_len = %s \r\n", Command20->SSID_len);
        printf("save Cmd20Content.SSID = %s \r\n",Cmd20Content.SSID);
        
        //Encryption Mode        
        Command20->EncryptionMode= buf[119];
        
        //Encryption Index
        Command20->EncryptionIndex= buf[120];
        
        //Password
        Command20->Password_len = 0;
        
        printf("save Cmd20Content->Password :\r\n ");
        for(i=0;i<64;i++)
        {
          if (buf[121+i] == '\n')
            break;
          else
          {
            Command20->Password[i] = buf[121+i];
            printf("%c ",Command20->Password[i]);
          }
        }
        //Command20->Password_len = i+1;
        Command20->Password_len = i;
        printf("\r\n");
        printf("save Cmd20Content->Password_len = %d \r\n",Cmd20Content.Password_len);
        
        //Wi-Fi Channel
        Command20->WiFi_Channel= buf[185];
        printf("save Cmd20Content.WiFi_Channel = %d \r\n",Cmd20Content.WiFi_Channel);
        
        
        //Reserved
        for(i=0;i<6;i++)
          Command20->Reserved[i]= buf[186+i];
        
        
        // buf copy to 
        

        cmd20_wifi_connect(Command20);
        
	
}


void ReceiveCmd21(uint8_t* buf,Command21Content* Command21){		// Edit by Eric, 0808
        uint8_t i,index;
	/*
		TODO
	*/
        index=48;
        for(i=0;i<16;i++,index++)
          Command21->Parameter_GroupSel[i] = *(buf+index);
        
        Command21->Date_ReqMode = *(buf+index++);
        
        Command21->Duration = ((*(buf+index)<<8)|*(buf+(index+1)));
        index+=2;
        
        // not support TxPower
        //Command21->TxPower=*(buf+index++);
        
        // not support Power Saving Mode
        //Command21->PowerSavingMode=*(buf+index++);
        
        for(i=0;i<123;i++,index++)
          Command21->Reserved[i]=*(buf+index);
        
        Cmd16Content.Duration=Command21->Duration;
        printf("Duration of Cmd16: %d",Cmd16Content.Duration);
	printf("Received Command21\n");
}

void ReceiveCmd22(uint8_t* buf,Command22Content* Command22){		//Set GPIO
        uint8_t i,index;
        gpio_t gpio_A7,gpio_A6,gpio_A5,gpio_A3;
        gpio_t gpio_B3,gpio_B2,gpio_B1,gpio_B0;
        gpio_t gpio_C5,gpio_C4,gpio_C3,gpio_C2,gpio_C1,gpio_C0;
        gpio_t gpio_D4,gpio_D3,gpio_D2,gpio_D1,gpio_D0;
        
	/*
	*/
        index=48;
        for(i=0;i<16;i++,index++)
          Command22->Parameter_GroupSel[i]=*(buf+index);
        Command22->DataFormat=*(buf+index++);
        for(i=0;i<3;i++,index++)
          Command22->Reserved[i]=*(buf+index);
        Command22->Direction_GPIOA=*(buf+index++);
        Command22->Direction_GPIOB=*(buf+index++);
        Command22->Direction_GPIOC=*(buf+index++);
        Command22->Direction_GPIOD=*(buf+index++);
        Command22->Setting_GPIOA=*(buf+index++);
        Command22->Setting_GPIOB=*(buf+index++);
        Command22->Setting_GPIOC=*(buf+index++);
        Command22->Setting_GPIOD=*(buf+index++);
        for(i=0;i<116;i++,index++)
          Command22->Reserved116[i]=*(buf+index);

        /*Process the command content action*/
        i=0;
        if(Command22->Parameter_GroupSel[i]&0x08==TRUE){        //Direction of A Port
          if(Command22->Direction_GPIOA&0x80==TRUE){
            gpio_init(&gpio_A7, PA_7);
            gpio_dir(&gpio_A7, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_A7, PA_7);
            gpio_dir(&gpio_A7, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOA&0x40==TRUE){
            gpio_init(&gpio_A6, PA_6);
            gpio_dir(&gpio_A6, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_A6, PA_6);
            gpio_dir(&gpio_A6, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOA&0x20==TRUE){
            gpio_init(&gpio_A5, PA_5);
            gpio_dir(&gpio_A5, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_A5, PA_5);
            gpio_dir(&gpio_A5, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOA&0x08==TRUE){
            gpio_init(&gpio_A3, PA_3);
            gpio_dir(&gpio_A3, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_A3, PA_3);
            gpio_dir(&gpio_A3, PIN_OUTPUT);
          }  
        }
        if(Command22->Parameter_GroupSel[i]&0x04==TRUE){        //Direction of B Port
          if(Command22->Direction_GPIOB&0x08==TRUE){
            gpio_init(&gpio_B3, PB_3);
            gpio_dir(&gpio_B3, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_B3, PB_3);
            gpio_dir(&gpio_B3, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOB&0x04==TRUE){
            gpio_init(&gpio_B2, PB_2);
            gpio_dir(&gpio_B2, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_B2, PB_2);
            gpio_dir(&gpio_B2, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOB&0x02==TRUE){
            gpio_init(&gpio_B1, PB_1);
            gpio_dir(&gpio_B1, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_B1, PB_1);
            gpio_dir(&gpio_B1, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOB&0x01==TRUE){
            gpio_init(&gpio_B0, PB_0);
            gpio_dir(&gpio_B0, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_B0, PB_0);
            gpio_dir(&gpio_B0, PIN_OUTPUT);
          }
        }
        if(Command22->Parameter_GroupSel[i]&0x02==TRUE){        //Direction of C Port
          if(Command22->Direction_GPIOC&0x20==TRUE){
            gpio_init(&gpio_C5, PC_5);
            gpio_dir(&gpio_C5, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C5, PC_5);
            gpio_dir(&gpio_C5, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOC&0x10==TRUE){
            gpio_init(&gpio_C4, PC_4);
            gpio_dir(&gpio_C4, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C4, PC_4);
            gpio_dir(&gpio_C4, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOC&0x08==TRUE){
            gpio_init(&gpio_C3, PC_3);
            gpio_dir(&gpio_C3, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C3, PC_3);
            gpio_dir(&gpio_C3, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOC&0x04==TRUE){
            gpio_init(&gpio_C2, PC_2);
            gpio_dir(&gpio_C2, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C2, PC_2);
            gpio_dir(&gpio_C2, PIN_OUTPUT);
          }
          if(Command22->Direction_GPIOC&0x02==TRUE){
            gpio_init(&gpio_C1, PC_1);
            gpio_dir(&gpio_C1, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C1, PC_1);
            gpio_dir(&gpio_C1, PIN_OUTPUT);
          }
          if(Command22->Direction_GPIOC&0x01==TRUE){
            gpio_init(&gpio_C0, PC_0);
            gpio_dir(&gpio_C0, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_C0, PC_0);
            gpio_dir(&gpio_C0, PIN_OUTPUT);
          }
        }
        if(Command22->Parameter_GroupSel[i]&0x01==TRUE){        //Direction of D Port
          if(Command22->Direction_GPIOD&0x10==TRUE){
            gpio_init(&gpio_D4, PE_4);
            gpio_dir(&gpio_D4, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_D4, PE_4);
            gpio_dir(&gpio_D4, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOD&0x08==TRUE){
            gpio_init(&gpio_D3, PE_3);
            gpio_dir(&gpio_D3, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_D3, PE_3);
            gpio_dir(&gpio_D3, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOD&0x04==TRUE){
            gpio_init(&gpio_D2, PE_2);
            gpio_dir(&gpio_D2, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_D2, PE_2);
            gpio_dir(&gpio_D2, PIN_OUTPUT);
          }  
          if(Command22->Direction_GPIOD&0x02==TRUE){
            gpio_init(&gpio_D1, PE_1);
            gpio_dir(&gpio_D1, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_D1, PE_1);
            gpio_dir(&gpio_D1, PIN_OUTPUT);
          }
          if(Command22->Direction_GPIOD&0x01==TRUE){
            gpio_init(&gpio_D0, PE_0);
            gpio_dir(&gpio_D0, PIN_INPUT);
          }
          else{
            gpio_init(&gpio_D0, PE_0);
            gpio_dir(&gpio_D0, PIN_OUTPUT);
          }
        }
       
        i++;
        
        if(Command22->Parameter_GroupSel[i]&0x80==TRUE){        //Direction of A Port
          if(Command22->Setting_GPIOA&0x80==TRUE){
            gpio_mode(&gpio_A7, PullUp);
          }
          else{
            gpio_mode(&gpio_A7, PullDown);
          }  
          if(Command22->Setting_GPIOA&0x40==TRUE){
            gpio_mode(&gpio_A6, PullUp);
          }
          else{
            gpio_mode(&gpio_A6, PullDown);
          }  
          if(Command22->Setting_GPIOA&0x20==TRUE){
            gpio_mode(&gpio_A5, PullUp);
          }
          else{
            gpio_mode(&gpio_A5, PullDown);
          }  
          if(Command22->Setting_GPIOA&0x08==TRUE){
            gpio_mode(&gpio_A3, PullUp);
          }
          else{
            gpio_mode(&gpio_A3, PullDown);
          }  
        }
        if(Command22->Parameter_GroupSel[i]&0x40==TRUE){        //Direction of B Port
          if(Command22->Setting_GPIOB&0x08==TRUE){
            gpio_mode(&gpio_B3, PullUp);
          }
          else{
            gpio_mode(&gpio_B3, PullDown);
          }  
          if(Command22->Setting_GPIOB&0x04==TRUE){
            gpio_mode(&gpio_B2, PullUp);
          }
          else{
            gpio_mode(&gpio_B2, PullDown);
          }  
          if(Command22->Setting_GPIOB&0x02==TRUE){
            gpio_mode(&gpio_B1, PullUp);
          }
          else{
            gpio_mode(&gpio_B1, PullDown);
          }  
          if(Command22->Setting_GPIOB&0x01==TRUE){
            gpio_mode(&gpio_B0, PullUp);
          }
          else{
            gpio_mode(&gpio_B0, PullDown);
          }
        }
        if(Command22->Parameter_GroupSel[i]&0x20==TRUE){        //Direction of C Port
          if(Command22->Direction_GPIOC&0x20==TRUE){
            gpio_mode(&gpio_C5, PullUp);
          }
          else{
            gpio_mode(&gpio_C5, PullDown);
          }  
          if(Command22->Direction_GPIOC&0x10==TRUE){
            gpio_mode(&gpio_C4, PullUp);
          }
          else{
            gpio_mode(&gpio_C4, PullDown);
          }  
          if(Command22->Direction_GPIOC&0x08==TRUE){
            gpio_mode(&gpio_C3, PullUp);
          }
          else{
            gpio_mode(&gpio_C3, PullDown);
          }  
          if(Command22->Direction_GPIOC&0x04==TRUE){
            gpio_mode(&gpio_C2, PullUp);
          }
          else{
            gpio_mode(&gpio_C2, PullDown);
          }
          if(Command22->Direction_GPIOC&0x02==TRUE){
            gpio_mode(&gpio_C1, PullUp);
          }
          else{
            gpio_mode(&gpio_C1, PullDown);
          }
          if(Command22->Direction_GPIOC&0x01==TRUE){
            gpio_mode(&gpio_C0, PullUp);
          }
          else{
            gpio_mode(&gpio_C0, PullDown);
          }
        }
        if(Command22->Parameter_GroupSel[i]&0x10==TRUE){        //Direction of D Port
          if(Command22->Direction_GPIOD&0x10==TRUE){
            gpio_mode(&gpio_D4, PullUp);
          }
          else{
            gpio_mode(&gpio_D4, PullDown);
          }  
          if(Command22->Direction_GPIOD&0x08==TRUE){
            gpio_mode(&gpio_D3, PullUp);
          }
          else{
            gpio_mode(&gpio_D3, PullDown);
          }  
          if(Command22->Direction_GPIOD&0x04==TRUE){
            gpio_mode(&gpio_D2, PullUp);
          }
          else{
            gpio_mode(&gpio_D2, PullDown);
          }  
          if(Command22->Direction_GPIOD&0x02==TRUE){
            gpio_mode(&gpio_D1, PullUp);
          }
          else{
            gpio_mode(&gpio_D1, PullDown);
          }
          if(Command22->Direction_GPIOD&0x01==TRUE){
            gpio_mode(&gpio_D0, PullUp);
          }
          else{
            gpio_mode(&gpio_D0, PullDown);
          }
        }
        i++;
        
        gpio_write(&gpio_D4,0);
        
        gpio_write(&gpio_D4,1);
        HalDelayUs(2000/2);
        gpio_write(&gpio_D4,0);
        HalDelayUs(2000/2);
        
        
        gpio_deinit(&gpio_D4);
	printf("Received Command22\n");
}

void ReceiveCmd23(uint8_t* buf,Command23Content* Command23){		//Set Factory Parameter
        uint8_t i,index;
        /*
	*/
        index = 48;
        for(i=0;i<16;i++,index++)
          Command23->IP_Address[i]=*(buf+index);
        for(i=0;i<4;i++,index++)
          Command23->IPv4_Subnetmask[i]=*(buf+index);
        Command23->Enable_DHCP=*(buf+index++);
        Command23->AP_StationMode=*(buf+index++);
        
        
         for(i=0;i<32;i++,index++)              // Edit by Eric
        {          
          if (*(buf+index) == '\n')
            break;
          else            
            Command23->SSID[i]=*(buf+index);
        }
        Command23->SSID_len = i;        
        
        
        Command23->EncryptionMode=*(buf+index++);
        Command23->EcryptionIndex=*(buf+index++);
        
        for(i=0;i<64;i++,index++)
          Command23->Password[i]=*(buf+index);
        
        //Password
        Command23->Password_len = 0;     
        
        for(i=0;i<64;i++,index++)
        {
          if (*(buf+index) == '\n')
            break;
          else          
            Command23->Password[i] = *(buf+index);
        }        
        Command23->Password_len = i;
        
        Command23->Wifi_Channel=*(buf+index++);
        Command23->TxPower=*(buf+index++);
        Command23->TimeZone=*(buf+index++);
        Command23->PresentSec=*(buf+index++);
        Command23->PresentMin=*(buf+index++);
        Command23->PresentHour=*(buf+index++);
        Command23->PresentWeek=*(buf+index++);
        Command23->PresentDate=*(buf+index++);
        Command23->PresentMonth=*(buf+index++);
        Command23->PresentYear=((*(buf+index)<<8)|*(buf+(index+1)));index+=2;
        Command23->Date_ReqMode=*(buf+index++);
        Command23->Duration=((*(buf+index)<<8)|*(buf+(index+1)));index+=2;
        Command23->Reserved=((*(buf+index)<<8)|*(buf+(index+1)));index+=2;
        
        //GCONFIG_WIFI_DEFAULT_BSSID=Command23->SSID;
        
        //strcpy(gconfig_ConfigData.WifiConfig.WifiSsid,GCONFIG_WIFI_DEFAULT_BSSID);      //20170728 Craig
        //gconfig_ConfigData.WifiConfig.WifiSsidLen = strlen(GCONFIG_WIFI_DEFAULT_BSSID);
        
        printf("SSID: ");
        for(i=0;i<strlen(GCONFIG_WIFI_DEFAULT_BSSID);i++)
          printf("%x ",*(GCONFIG_WIFI_DEFAULT_BSSID+i));
       
        GCONFIG_WriteConfigData();
        
        //GCONFIG_ReadDefaultConfigData();
        
	printf("Received Command23\n");
}

void ReceiveCmd25(uint8_t* buf,Command25Content* Command25){		//Set RTC
         uint8_t i,index;
	// Edit by Hsinhua, Add RTC, 0727---------------------------
        index = 48;
        Command25->TimeZone = buf[48];
        Command25->PresentSec = buf[49];
        Command25->PresentMin = buf[50];
        Command25->PresentHour = buf[51];
        Command25->PresentWeek = buf[52];
        Command25->PresentDate = buf[53];
        Command25->PresentMonth = buf[54];
        Command25->PresentYear = buf[55]*256 + buf[56];
        
        
        RTC_TIME time_1;   
        time_1.Second = Command25->PresentSec;
        time_1.Minute = Command25->PresentMin;
        time_1.Hour = Command25->PresentHour;
        time_1.Day = Command25->PresentWeek;
        time_1.Date = Command25->PresentDate;
        time_1.Month = Command25->PresentMonth;
        time_1.Year = Command25->PresentYear;
        
        printf("\n time_1 = %d/%d/%d  D%d  %d:%d:%d \n",
                             time_1.Year, 
                             time_1.Month, 
                             time_1.Date, 
                             time_1.Day, 
                             time_1.Hour, 
                             time_1.Minute,
                             time_1.Second );
        
        RTC_SetCurrTime(&time_1);
        
        
	printf("Received Command25\n");
}

void ReceiveCmd1B(uint8_t* buf){
  /*
    Total IC number 
    Reserved
  */
  printf("Received Command1B\n");
}

void SendCommand1C(){
  /*
    Reserved
  */
  printf("Process Command1C\n");
}

// Edit by Eric, 0801
static void systemReboot(int argc, char **argv)
{
	// Set processor clock to default before system reset
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x14, 0x00000021);
	osDelay(100);

	// Cortex-M3 SCB->AIRCR
	HAL_WRITE32(0xE000ED00, 0x0C, (0x5FA << 16) |                             // VECTKEY
	                              (HAL_READ32(0xE000ED00, 0x0C) & (7 << 8)) | // PRIGROUP
	                              (1 << 2));                                  // SYSRESETREQ
	while(1) osDelay(1000);
}



