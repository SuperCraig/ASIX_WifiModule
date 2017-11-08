#include "MBI_CommandIdentify.h"
#include "MBI_CommandProcess.h"
#include <stdlib.h>
#include <stdio.h>
#include "device.h"
#include "serial_api.h"
#include "sockets.h"
#include "api.h"
#include "main.h"
#include "gconfig.h"
#include "gudpdat.h"
#include "guart.h"
#include "gs2w.h"
#include "platform_opts.h"

const uint8_t ID_Array[8]={'M','B','I','-','N','e','t',0x01};//802.3 Ethernet 0x00, 
                                                          //802.11 Wifi 0x01, 
							 //802.15.1 Bluetooth Low Energy 0x02
const uint8_t Version=0x8F;		
const uint8_t PacketHeaderLen=0x1C;
const uint16_t PacketNo=0x0000;
uint16_t PacketTotLen;
uint16_t PacketHeaderChkSum;
uint8_t Protocol=1;
uint8_t Reserved[3]={0};
uint8_t SourceAddress[8]={0};
uint8_t DestinationAddress[8]={0};

uint8_t CommandType;
uint8_t CommandCode;
uint16_t CommandCount;
uint16_t CommandLen;
uint16_t CommandChkSum;
uint8_t MemoryStartAddress[4]={0};
uint8_t PacketFormat[36]={0};

uint8_t UDPRxBuffer[MTUofEthernet];
uint8_t UDPTxBuffer[MTUofEthernet];
uint8_t TCPRxBuffer[MTUofEthernet];
uint8_t TCPTxBuffer[MTUofEthernet];

uint8_t Cmd16Buffer[Cmd16BufferLen];


/*UDP / TCP send received flag*/
uint8_t ReadyToSendFlag;
uint8_t ReceivedFlag;

uint8_t SourceAddr=0;
uint8_t DestinationAddr=0;


uint8_t* GetSourceAddress(uint8_t Source,uint8_t ignore){
	uint8_t i;
	SourceAddress[0]=Source;
	for(i=0;i<7;i++)
		SourceAddress[1+i]=ignore;
	
	return SourceAddress;
}

uint8_t* GetDestinationAddress(uint8_t Source,uint8_t ignore){
	uint8_t i;
	DestinationAddress[0]=Source;
	for(i=0;i<7;i++)
		DestinationAddress[1+i]=ignore;
		
	return DestinationAddress;
}

uint8_t* GetPacketFormat(){
	uint8_t index;
	uint8_t *Source=GetSourceAddress(SourceAddr,Ignore);
	uint8_t *Destination=GetDestinationAddress(DestinationAddr,Ignore);
	GetPacketHeaderChkSum();
	
	for(index=0;index<sizeof(ID_Array)/sizeof(uint8_t);index++)
		*(PacketFormat+index)=*(ID_Array+index);
		
	index=sizeof(ID_Array)/sizeof(uint8_t);
	*(PacketFormat+index++)=Version;
	*(PacketFormat+index++)=PacketHeaderLen;
	*(PacketFormat+index++)=(uint8_t)(PacketNo >> 8);
	*(PacketFormat+index++)=(uint8_t)(PacketNo & 0x00FF);
	*(PacketFormat+index++)=(uint8_t)(PacketTotLen >> 8);
	*(PacketFormat+index++)=(uint8_t)(PacketTotLen & 0x00FF);
	*(PacketFormat+index++)=(uint8_t)(PacketHeaderChkSum >> 8);
	*(PacketFormat+index++)=(uint8_t)(PacketHeaderChkSum & 0x00FF);
	*(PacketFormat+index++)=Protocol;
	
	for(index=0;index<3;index++){
		*(PacketFormat+index+17)=*(Reserved+index);
	}
	for(index=0;index<8;index++){
		*(PacketFormat+index+20)=*(Source+index);
	}
	for(index=0;index<8;index++){
		*(PacketFormat+index+28)=*(Destination+index);	
	}
        return PacketFormat;
}



void GetPacketHeaderChkSum(){
	int CheckSum=
		(Version<<8|PacketHeaderLen)+(PacketNo)+
		(PacketTotLen)+(Protocol<<8|Reserved[0])+
		(Reserved[1]<<8|Reserved[2])+(SourceAddress[0]<<8|SourceAddress[1])+
		(SourceAddress[2]<<8|SourceAddress[3])+(SourceAddress[4]<<8|SourceAddress[5])+
		(SourceAddress[6]<<8|SourceAddress[7])+(DestinationAddress[0]<<8|DestinationAddress[1])+
		(DestinationAddress[2]<<8|DestinationAddress[3])+(DestinationAddress[4]<<8|DestinationAddress[5])+
		(DestinationAddress[6]<<8|DestinationAddress[7]);
	
	PacketHeaderChkSum=(uint16_t) CheckSum;
}


uint8_t IdentifyCommand(uint8_t *buf, uint8_t check_flag){
    /*
		TODO
	*/
	uint8_t index;
        uint8_t *px;
	uint8_t CmdCode=*(buf+37);
	uint16_t PacketSize=((*(buf+12)<<8)|*(buf+13));
        uint16_t CmdChkSum=(*(buf+42)<<8)|*(buf+43);
        
    if (!check_flag) return ( CmdCode );
	
	PacketTotLen = GetCommandLength(CmdCode);
	CommandChkSum=CmdChkSum;
        px=GetPacketFormat();
        

        printf ("IdentifyCommand function, *(buf+37) = %x,  CmdCode = %x\r\n",*(buf+37),CmdCode);
        
        if(PacketTotLen != PacketSize)
        {
          printf (" PacketTotLen = %d, PacketPacketSizeTotLen = %d,\r\n",PacketTotLen,PacketSize);
          return NULL;
        }
        
	for(index=0;index<36;index++){
          if(*(buf+index)!= *(px+index))
          {
              printf ("index =%d, 0x%x,  x%x,\r\n", index, *(buf+index),*(px+index));
              printf ("hdr error \r\n");
              return NULL;  
          } 
	}
        
	if(GetRearCommandChkSum(buf)!=CommandChkSum)
        {
          printf ("CommandChkSum error \r\n");
          return NULL;
        }
	else
        {
           printf ("return CmdCode = %x, \r\n",CmdCode);
           return CmdCode;
        }
}

uint16_t GetCommandLength(uint8_t CmdCode){
	switch(CmdCode){
		case Cmd12:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd12Len;
			break;
		case Cmd13:
			SourceAddr=wifi_Client;
			DestinationAddr=Wifi_Server;
			return Cmd13Len;
			break;
		case Cmd15:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd15Len;
			break ;	
		case Cmd16:
			SourceAddr=wifi_Client;
			DestinationAddr=Wifi_Server;
			return Cmd16Len;
			break;
		case Cmd19:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd19Len;
			break;		
		case Cmd1A:
			SourceAddr=wifi_Client;
			DestinationAddr=Wifi_Server;
			return Cmd1ALen;
			break;
		case Cmd10:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd10Len;
			break;
		case Cmd11:
			SourceAddr=wifi_Client;
			DestinationAddr=Wifi_Server;
			return Cmd11Len;
			break;
		case Cmd20:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd20Len;
			break;	
		case Cmd21:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd21Len;
			break;	
		case Cmd22:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd22Len;
			break;	
		case Cmd23:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd23Len;
			break;	
		case Cmd25:
			SourceAddr=Wifi_Server;
			DestinationAddr=wifi_Client;
			return Cmd25Len;
			break;	
        default:
            return NULL;
            break;
	}
}

uint16_t GetRearCommandChkSum(uint8_t *buf){
	uint8_t index=0;
	uint8_t CommandPreCount=36;
	uint16_t PacketSize=PacketTotLen-CommandPreCount;
	uint16_t PacketChkSum=0;
	for(index=0;index<(PacketSize/2);index++){
		if((index*2+0)+CommandPreCount!=42)
			PacketChkSum+=(*(buf+(index*2+0)+CommandPreCount)<<8)|*(buf+(index*2+1)+CommandPreCount);
		else
			PacketChkSum=PacketChkSum;
	}
	return PacketChkSum;
}