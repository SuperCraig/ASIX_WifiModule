#include "MBI_CommandIdentify.h"
#include "MBI_CommandProcess.h"
#include "PM25ModuleFunction.h"
#include <stdlib.h>
#include <stdio.h>

#define DS_CO2_20_Length 7
uint8_t DS_CO2_20[7] = {0x42,0x4D,0xE3,0x00,0x00,0x01,0x72};

uint16_t Coefficient[14]={200,400,600,750,900,1000,1250,1400,1700,1800,1900,2000,2200,3000};
PM25 pm1;


uint16_t pm1value = 0;
uint16_t pm2_5value = 0;
uint16_t pm10_0value = 0;
uint16_t co2value = 0;


uint16_t CoefDecision(uint8_t voltage){
	if(voltage <= 45){
		return Coefficient[0];
	}
	else if(voltage >= 46 && voltage <= 48){
		return Coefficient[1];
	}
	else if(voltage >= 49 && voltage <= 51){
		return Coefficient[2];
	}
	else if(voltage >= 52 && voltage <= 54){
		return Coefficient[3];
	}
	else if(voltage >= 55 && voltage <= 58){
		return Coefficient[4];
	}
	else if(voltage >= 59 && voltage <= 64){
		return Coefficient[5];
	}
	else if(voltage >= 65 && voltage <= 70){
		return Coefficient[6];
	}
	else if(voltage >= 71 && voltage <= 75){
		return Coefficient[7];
	}
	else if(voltage >= 76 && voltage <= 80){
		return Coefficient[8];
	}
	else if(voltage >= 81 && voltage <= 85){
		return Coefficient[9];
	}
	else if(voltage >= 86 && voltage <= 90){
		return Coefficient[10];
	}
	else if(voltage >= 91 && voltage <= 100){
		return Coefficient[11];
	}
	else if(voltage >= 101 && voltage <= 110){
		return Coefficient[12];
	}
	else if(voltage >= 111){
		return Coefficient[13];
	}
        else{
          return 0;
        }
}

uint16_t GetDensityValue(PM25 *pm25){
	uint16_t DensityValue;
	uint16_t Chksum;
	Chksum = pm25->Vout_H + pm25->Vout_L + pm25->Vref_H + pm25->Vref_L;
	if(pm25->StartBit != 0xaa)
		return 0;
	if(pm25->Chksum != Chksum)
		return 0;
	if(pm25->StopBit != 0xff)
		return 0;
		
	pm25->DensityVoltage = (uint16_t)(((((pm25->Vout_H)*256+pm25->Vout_L)*2.5)/1024)*1000);
	DensityValue = (uint16_t)(CoefDecision(pm25->DensityVoltage) * ((((pm25->Vout_H)*256+pm25->Vout_L)*2.5)/1024));
	return DensityValue;
}
uint16_t GetPM25Value(GUDPDAT_CONN_T *udpConn){        
    u32		urDataLen;
    u16		gudpdat_BufIndex;
    u16 i;
    u16         Pm25Value = 0;
    // Check the UART received buffer
    urDataLen = GUART_GetRcvrAvailCount();
    if (urDataLen > 0)
    {
      gudpdat_BufIndex = udpConn->TxBufIndex;
      if (urDataLen > (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex))
      {
        urDataLen = (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex);
      }
      if(gudpdat_BufIndex > (GUDPDAT_TX_BUF_SIZE-1)){          // Craig
        udpConn->TxBufIndex=0;
        gudpdat_BufIndex=0;
      }

      GUART_CopyUrRxToApp(&(udpConn->TxBuf[gudpdat_BufIndex]), urDataLen);
      
      for(i=gudpdat_BufIndex ; i<urDataLen+gudpdat_BufIndex ; i++){    // Craig
        //printf("%x ",*(udpConn->TxBuf+i));     // Craig
        
        if(*(udpConn->TxBuf+i) == 0xaa && *(udpConn->TxBuf+i+6) == 0xff){
          /*PM 25 Module initialize struct value*/
          pm1.StartBit = *(udpConn->TxBuf+i+0);
          pm1.Vout_H = *(udpConn->TxBuf+i+1);
          pm1.Vout_L = *(udpConn->TxBuf+i+2);
          pm1.Vref_H = *(udpConn->TxBuf+i+3);
          pm1.Vref_L = *(udpConn->TxBuf+i+4);
          pm1.Chksum = *(udpConn->TxBuf+i+5);
          pm1.StopBit = *(udpConn->TxBuf+i+6);
          //printf("Density Value: %d",GetDensityValue(&pm1));
          Pm25Value = GetDensityValue(&pm1);
        }
      }
      
      /*      Buffer content to UART interface          
      GUART_CopyAppToUrTx((udpConn->TxBuf+gudpdat_BufIndex), urDataLen);         // Craig
      udpConn->UrTxBytes += urDataLen;              // Craig
      if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig            
      udpConn->UrTxBytes = 0;               // Craig
      */       
      
      
      gudpdat_BufIndex += urDataLen;
      udpConn->UrRxBytes += urDataLen;
      udpConn->TxBufIndex = gudpdat_BufIndex;
      return Pm25Value;
    }
    else
    {
      GUART_CopyUrRxToApp(0, 0);/* UART RX trigger */	
      return Pm25Value;
    }
}

void PLANTOWERSensor(GUDPDAT_CONN_T *udpConn){
  u32		urDataLen;
  u16		gudpdat_BufIndex;
  u16 i;
//  uint16_t pm1value;
//  uint16_t pm2_5value;
//  uint16_t pm10_0value;
//  uint16_t co2value;
  
     
  GUART_CopyAppToUrTx(DS_CO2_20, DS_CO2_20_Length);         // Craig
  udpConn->UrTxBytes += DS_CO2_20_Length;              // Craig
  if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig            
    udpConn->UrTxBytes = 0;               // Craig
  
  
  urDataLen = GUART_GetRcvrAvailCount();
  //printf("urDaraLen: %d\n",urDataLen);
  if(urDataLen > 0){        
    gudpdat_BufIndex = udpConn->TxBufIndex;
    
    if (urDataLen > (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex))
    {
      urDataLen = (GUDPDAT_TX_BUF_SIZE - gudpdat_BufIndex);
    }
    if(gudpdat_BufIndex >= (GUDPDAT_TX_BUF_SIZE-1)){          // Craig
      udpConn->TxBufIndex=0;
      gudpdat_BufIndex=0;
    }
    
    
    GUART_CopyUrRxToApp(&(udpConn->TxBuf[gudpdat_BufIndex]), urDataLen);
    
    for(i=gudpdat_BufIndex ; i<urDataLen+gudpdat_BufIndex ; i++){    // Craig
      if(*(udpConn->TxBuf+i) == 0x42 && *(udpConn->TxBuf+i+1) == 0x4d && *(udpConn->TxBuf+i+2) == 0x00 && *(udpConn->TxBuf+i+3) == 0x08){
        co2value = *(udpConn->TxBuf+i+4)<<8 | *(udpConn->TxBuf+i+5);
//        Cmd16Content.CO_CO2 = co2value;
//        printf("Co2 value: %d\n",co2value);
      }
      else if(*(udpConn->TxBuf+i) == 0x42 && *(udpConn->TxBuf+i+1) == 0x4d && *(udpConn->TxBuf+i+2) == 0x00 && *(udpConn->TxBuf+i+3) == 0x1C){
        pm1value = *(udpConn->TxBuf+i+4)<<8 | *(udpConn->TxBuf+i+5);
        pm2_5value = *(udpConn->TxBuf+i+6)<<8 | *(udpConn->TxBuf+i+7);
        pm10_0value = *(udpConn->TxBuf+i+8)<<8 | *(udpConn->TxBuf+i+9);
//        printf("pm1.0 value: %d\n",pm1value);
//        printf("pm2.5 value: %d\n",pm2_5value);
//        printf("pm10.0 value: %d\n",pm10_0value);
      }
    }
    
    //        for(i=gudpdat_BufIndex ; i<urDataLen+gudpdat_BufIndex ; i++){    // Craig
    //          printf("%x ",*(udpConn->TxBuf+i));     // Craig
    //        }
    
    gudpdat_BufIndex += urDataLen;
    udpConn->UrRxBytes += urDataLen;
    udpConn->TxBufIndex = gudpdat_BufIndex;
  }
  else{
    GUART_CopyUrRxToApp(0, 0);/* UART RX trigger */	
  }
}

uint16_t GetPM1Value(){
  return pm1value;
}

uint16_t GetPM2_5Value(){
  return pm2_5value;
}

uint16_t GetPM10_0Value(){
  return pm10_0value;
}

uint16_t GetCo2Value(){
  return co2value;
}

uint8_t stringCount(char *string){
  uint8_t count = 0;
  while(*string != '\0'){
    count++;
    *string++;
  }
  return count;
}

void SendTo_TP(GUDPDAT_CONN_T *udpConn,char* string){
  u32		urDataLen;
  u16		gudpdat_BufIndex;
  u8 i;
  uint8_t count = stringCount(string);
  
  GUART_CopyAppToUrTx(string, count);         // Craig
  udpConn->UrTxBytes += count;              // Craig
  
//  char string[9]={0x70,0x61,0x67,0x65,0x20,0x30,0xff,0xff,0xff};
//  GUART_CopyAppToUrTx(string, 9);         // Craig
//  udpConn->UrTxBytes += 9;              // Craig
  
  if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig            
    udpConn->UrTxBytes = 0;               // Craig
}

void IntToString(char *string,int num){
	//char string[4];
	char num1,num2,num3,num4;
	if(num>=1000){
		num4=num/1000;
		string[0]=(char)(num4+0x30);
		num3=(num-(num4*1000))/100;
		string[1]=(char)(num3+0x30);
		num2=(num-(num4*1000)-(num3*100))/10;
		string[2]=(char)(num2+0x30);
		num1=(num-(num4*1000)-(num3*100)-(num2*10));
		string[3]=(char)(num1+0x30);
	}
	else if(num<1000&&num>=100){
		num4=num/100;
		string[0]=(char)(num4+0x30);
		num3=(num-(num4*100))/10;
		string[1]=(char)(num3+0x30);
		num2=(num-(num4*100)-(num3*10));
		string[2]=(char)(num2+0x30);
		string[3]=NULL;
	}
	else if(num<100&&num>=10){
		num4=num/10;
		string[0]=(char)(num4+0x30);
		num3=(num-(num4*10));
		string[1]=(char)(num3+0x30);
		string[2]=NULL;
		string[3]=NULL;
	}
	else{
		num4=num;
		string[0]=(char)(num4+0x30);
		string[1]=NULL;
		string[2]=NULL;
		string[3]=NULL;
	}
}