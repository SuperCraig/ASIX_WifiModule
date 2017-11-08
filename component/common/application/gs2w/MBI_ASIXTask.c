#include "MBI_ASIXTask.h"
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
#include "rtc.h"                       // Edit by Hsinhua, Add RTC, 0727---------------------------
#include "i2c_api.h"                   // Edit by Hsinhua, Add RTC, 0727---------------------------
#include "PM25ModuleFunction.h"
#include "gudpbc.h"
#include "tncom.h"
#include "modbus_gateway.h"
#include "analogin_api.h"    //Edit by Hsinhua, 08/20----------------
#include <sys_api.h>         //Edit by Hsinhua, 08/20----------------


char Co2_1[18]="Co2.n0.val=";           //TP paramerters
char PM_1[17]="PM.n0.val=";
char PM_2[17]="PM.n1.val=";
char PM_3[17]="PM.n2.val=";
char Temp_Hum_1[19]="Temp.n0.val=";     
char Temp_Hum_2[19]="Temp.n1.val=";
char SensorValue[4]={0};
char Suffix[3]={0xff,0xff,0xff};
////////////////

void MBI_UDPTask();
void MBI_TCPTask();
void RTC_READ_Task(void *pParam);      // Edit by Hsinhua, Add RTC, 0727---------------------------
extern xQueueHandle RTC_Queue;                // Edit by Hsinhua, Add RTC, 0727---------------------------


// Edit by Hsinhua, Add TEMP_sensor, 0820---------------------------
extern xQueueHandle tempQueue;

// Edit by Hsinhua, Add ADC_input, 0820---------------------------
void ADC_READ_Task(void *pParam);
#define OFFSET 		0x298							
#define GAIN_DIV	0x34C							
#define AD2MV(ad,offset,gain) (((ad/16)-offset)*1000/gain)
uint16_t	ADC_data_0, ADC_data_1, ADC_data_2, ADC_data_3;

//#define DS_CO2_20_Length 7
//uint8_t DS_CO2_20[7] = {0x42,0x4D,0xE3,0x00,0x00,0x01,0x72};

uint16_t temp_u16;      //for other scope can use it    171030 Craig
uint16_t hum_u16;       //for other scope can use it    171030 Craig
        
xQueueHandle UDPTxQueue;	
xQueueHandle UDPRxQueue;	
xQueueHandle TCPTxQueue;
xQueueHandle TCPRxQueue;

xQueueHandle Cmd16Queue;


portTickType    xTimeToSendCmd16=NULL;
portTickType    xTimeTemp=NULL;
portTickType    xTimeSensorInterval=NULL;
portTickType    xTimeSensorTemp=NULL;

void MBI_Init(void){
	
	//xTaskCreate(pvTaskCode,pcName,usStaskDepth,pvParameters,uxPriority,pxCreatTask);
        
	xTaskCreate(MBI_UDPTask,"UDPTask",700,NULL,1,NULL);	
	xTaskCreate(MBI_TCPTask,"TCPTask",700,NULL,1,NULL);
        
	xTaskCreate(Sensor_DataBack_Task,"SensorTask",250,NULL,1,NULL);        // Craig
        
        
	//xQueueHandle xQueue = xQueueCreate(uxQueueLength,uxItemSize);
        
        // Edit by Hsinhua, Add RTC, 0727---------------------------
        RTC_Queue = xQueueCreate( 2 , sizeof( RTC_TIME )); 

        if(xTaskCreate( RTC_READ_Task, ( signed portCHAR * ) "RTC_READ_Task", 200, NULL, tskIDLE_PRIORITY+1, NULL ) == pdPASS)
      	{
              printf("\n Create RTC_READ_Task pass! \n");
	}
	
        
        // Edit by Hsinhua, Add TEMP_sensor, 0820---------------------------
        tempQueue = xQueueCreate( 2 , sizeof( TEMP_SENSOR ));  
        
        // Edit by Hsinhua, Add ADC_input, 0820---------------------------
        if(xTaskCreate( ADC_READ_Task, ( signed portCHAR * ) "RTC_READ_Task", 200, NULL, tskIDLE_PRIORITY+1, NULL ) == pdPASS)
      	{
          printf("\n Create ADC_READ_Task pass! \n");
        }
        
        
	UDPTxQueue=xQueueCreate(1,sizeof(uint16_t));	
	UDPRxQueue=xQueueCreate(1,sizeof(uint16_t));	
	TCPTxQueue=xQueueCreate(1,sizeof(uint16_t));	
	TCPRxQueue=xQueueCreate(1,sizeof(uint16_t));
}

void MBI_UDPTask(void *param){
	//portBASE_TYPE xQueueSendToFront(xQueueHandle xQueue,const void *pvItemToQueue,portTickType xTicksToWait);
	//portBASE_TYPE xQueueSendToBack(xQueueHandle xQueue,const void *pvItemToQueue,portTicksType xTickToWait);	
	//portBASE_TYPE xQueueReceive(xQueueHandle xQueue,const void *pvBuffer,portTicksType xTicksToWait);
	//port_BASE_TYPE xQueuePeek(xQueueHandle xQueue,const void *pvBuffer,portTicksType xTickToWait);
	
	u16 lValueToSend = 40;			
	u16 lReceivedLen;		
        u16 i;
	portBASE_TYPE xUDPTxStatus;
	portBASE_TYPE xUDPRxStatus;
        portBASE_TYPE xCmd16Status;

	const portTickType xTickToWait=100/portTICK_RATE_MS; 	//100ms
	
	//lValueToSend=(long)param;
	
	while(1){
		/*
			TODO
		*/     
          if(uxQueueMessagesWaiting(UDPRxQueue) == 0){          
			//vPrintString("Queue normal should have been empty!\r\n"); 
            xUDPRxStatus=xQueueReceive(UDPRxQueue,&lReceivedLen,xTickToWait);
            if(xUDPRxStatus == pdPASS){
                    printf("\n\r xUDPRxStatus receive pass! %d\n", lReceivedLen);
                    //CommandToDo(Cmd20,buf12,1);
                //=====================================================================
                      
                    UDPRxBuffer[13]+=8;
                    UDPRxBuffer[15]+=8;
                    

                    CommandCode = IdentifyCommand(UDPRxBuffer,1);
                    printf("\nMBI_UDPTask, CmdCode:%x\n",CommandCode);
                    
                    //CommandToDo(CommandCode,UDPRxBuffer,0);
                    CommandToDo(CommandCode,UDPRxBuffer,1);
                    
                //=====================================================================                    
                    CommandCode = IdentifyCommand(UDPTxBuffer,0); 
                    lValueToSend = GetCommandLength(CommandCode);
                    if (lValueToSend != NULL)
                        xUDPTxStatus = xQueueSendToBack(UDPTxQueue,&lValueToSend,1);
                    
            }
          }
      
          if(Cmd16Content.Duration!=0 || gconfig_ConfigData.MBI_FactorySettings.Duration!=0){
            if(Cmd16Content.Duration==0)
              Cmd16Content.Duration = gconfig_ConfigData.MBI_FactorySettings.Duration;
            else
              Cmd16Content.Duration = Cmd16Content.Duration;
            
            xTimeToSendCmd16 = xTaskGetTickCount();
            if ((xTimeToSendCmd16-xTimeTemp) >= Cmd16Content.Duration*1000){
                SendCommandFactory(Cmd16);
                xTimeTemp=xTimeToSendCmd16;
                lValueToSend=Cmd16Len;
                xCmd16Status=xQueueSendToBack(Cmd16Queue,&lValueToSend,1);
                
                //sendto(gudpbc_SocketFD, &Cmd16Buffer[0], lValueToSend, 0, &gudpbc_DestAddr, sizeof(struct sockaddr_in));           // Craig
                sendto(gudpbc_SocketFD_MBI, &Cmd16Buffer[0], lValueToSend, 0, &gudpbc_DestAddr_MBI, sizeof(struct sockaddr_in));           // Craig
                printf("xCmd16Status: %d",xCmd16Status);
             }
           }        
          
		//xUDPTxStatus=xQueueSendToBack(UDPTxQueue,&lValueToSend,0);
		//if(xUDPTxStatus!=pdPASS){
        //          printf("xUDPTxStatus creat!");
		//	//UDP command want to send
		//}
	}
}

void MBI_TCPTask(void *param){
	u16 lValueToSend;			//the value used to announce send the TCP Packet already
	u16 lReceivedValue;		//the value used to decide to send TCP Packet yes or not
	
	portBASE_TYPE xTCPTxStatus;
	portBASE_TYPE xTCPRxStatus;
	
	const portTickType xTickToWait=100/portTICK_RATE_MS; 	//100ms
	
	lValueToSend=(long)param;
	
	while(1){
          /*
          TODO
          */
          
          if(uxQueueMessagesWaiting(TCPRxQueue) == 0){            
            //vPrintString("Queue normal should have been empty!\r\n");
            xTCPRxStatus = xQueueReceive(TCPRxQueue,&lReceivedValue,xTickToWait);
            if(xTCPRxStatus == pdPASS){
              //vPrintStringAndNumber("Received= ",lReceivedValue);
              printf("\n\r xTCPRxStatus receive pass!");  
              
            }
            
          }
          
          
          //xTCPTxStatus=xQueueSendToBack(TCPTxQueue,&lValueToSend,0);
          //if(xTCPTxStatus!=pdPASS){
          //           printf("xTCPTxStatus not creat!");
          //	//TCP Command want to send
          //}
	}
}

//Hsinhua, Add RTC, 0727---------------------------
void RTC_READ_Task(void *pParam)  
{      
  for( ;; )  
  {  
    RTC_TIME time_1;
    
    RTC_GetCurrTime(&time_1);
    
    xQueueSend( RTC_Queue, ( void* )&time_1, 1 );  
    
    // Edit by Hsinhua, Add Temp_sensor, 0820---------------------------
    TEMP_SENSOR temp_1;
    
    THS_GetTemperature(&temp_1.Temperature);
    
    THS_GetRelativeHumidity(&temp_1.Humidity);
    
    temp_u16 = (uint16_t)temp_1.Temperature;            //for other scope can use it    171030 Craig
    hum_u16 = (uint16_t)temp_1.Humidity;                //for other scope can use it    171030 Craig
           
    xQueueSend( tempQueue, ( void* )&temp_1, 1 ); 
    
    vTaskDelay( 1000/portTICK_RATE_MS );  
    
  }  
} 


// Edit by Hsinhua, Add ADC_input, 0820---------------------------
void ADC_READ_Task(void *pParam)  
{      
  for( ;; )  
  {  
    int32_t v_mv1;
    uint16_t offset, gain;
    offset = OFFSET;
    gain = GAIN_DIV;
    
    analogin_t   adc;
    u16 adcdat;
    
    analogin_init(&adc, AD_3);
    adcdat = analogin_read_u16(&adc);
    v_mv1 = AD2MV(adcdat, offset, gain);
    analogin_deinit(&adc);
    ADC_data_2 = ADC_data_1;
    ADC_data_1 = ADC_data_0;
    ADC_data_0 = v_mv1 >> 8;
    if((ADC_data_2 == ADC_data_1) && (ADC_data_2 == ADC_data_0))
    { 
      ADC_data_3 = v_mv1;
      //printf("AD0:%x = %d mv \n", adcdat, v_mv1); 
    }
    
    vTaskDelay( 300/portTICK_RATE_MS );  
    
  }  
} 

void Sensor_DataBack_Task(void *param){
  uint16_t pm25value;
  uint16_t pm1value;
  uint16_t pm2_5value;
  uint16_t pm10_0value;
  uint16_t co2value;
  uint8_t times = 0;
  u8 i;
  u32		urDataLen;      // Craig
  u16		gudpdat_BufIndex;       // Craig
  GUDPDAT_CONN_T *udpConn = &gudpdat_Conns[0];
  
  while(1){
    xTimeSensorInterval = xTaskGetTickCount();
//    if ((xTimeSensorInterval-xTimeSensorTemp) >= 3*1000 && (xTimeSensorInterval-xTimeSensorTemp) < 5*1000){
//      if(MBI_Baudrate != 2400){
//        serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
//        serial_baud(&urObj, 2400);           // Craig
//        MBI_Baudrate = 2400;
//      }
//      pm25value = GetPM25Value(&gudpdat_Conns[0]);
//      if(pm25value > 0){
//        xTimeSensorTemp = xTimeSensorInterval;
//        Cmd16Content.PM25 = pm25value;
//        printf("Density Value: %d\n",Cmd16Content.PM25);
//      }
//      
////      xTimeSensorTemp = xTimeSensorInterval;
//    }
    if((xTimeSensorInterval-xTimeSensorTemp) >= 2*1000&& (xTimeSensorInterval-xTimeSensorTemp) < 4*1000 && times==1){
      if(MBI_Baudrate != 4800){
        serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
        serial_baud(&urObj, 4800);           // Craig
        MBI_Baudrate = 4800;
      }
      

      IntToString(SensorValue,co2value);
      for(i=0;i<4;i++)
        Co2_1[11+i]=SensorValue[i];

      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],Co2_1);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      IntToString(SensorValue,pm1value);
      for(i=0;i<4;i++)
        PM_1[10+i]=SensorValue[i];
      
      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],PM_1);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      IntToString(SensorValue,pm2_5value);
      for(i=0;i<4;i++)
        PM_2[10+i]=SensorValue[i];
      
      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],PM_2);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      IntToString(SensorValue,pm10_0value);
      for(i=0;i<4;i++)
        PM_3[10+i]=SensorValue[i];
      
      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],PM_3);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      IntToString(SensorValue,temp_u16);
        printf("SensorValue = %s\n",SensorValue);
      for(i=0;i<4;i++)
        Temp_Hum_1[12+i]=SensorValue[i];
      
      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],Temp_Hum_1);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      IntToString(SensorValue,hum_u16);
      for(i=0;i<4;i++)
        Temp_Hum_2[12+i]=SensorValue[i];
      
      for(i=0;i<3;i++){
        vTaskDelay(20);
        SendTo_TP(&gudpdat_Conns[0],Temp_Hum_2);
        SendTo_TP(&gudpdat_Conns[0],Suffix);
      }
      
      times++;
    }
    if((xTimeSensorInterval-xTimeSensorTemp) >= 1*1000 && (xTimeSensorInterval-xTimeSensorTemp) < 2*1000 && times==0){
      if(MBI_Baudrate != 9600){
        serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
        serial_baud(&urObj, 9600);           // Craig
        MBI_Baudrate = 9600;
      }
       
      PLANTOWERSensor(&gudpdat_Conns[0]);
      
      
    
      Cmd16Content.CO_CO2 = GetCo2Value();
      //Cmd16Content.PM25 = GetPM2_5Value();
      printf("Co2 value: %d\n",Cmd16Content.CO_CO2);
      printf("pm1.0 value: %d\n",GetPM1Value());
      printf("pm2.5 value: %d\n", GetPM2_5Value());
      printf("pm10.0 value: %d\n",GetPM10_0Value());
      
      
      co2value = GetCo2Value();
      pm1value = GetPM1Value();
      pm2_5value = GetPM2_5Value();
      pm10_0value = GetPM10_0Value();
      Cmd16Content.PM25 = pm2_5value;
      Cmd16Content.CO_CO2 = co2value;
      

      times++;
//      uint8_t len;
//      uint8_t PMLen,Co2Len;
//      uint8_t *string1 = "mbi:";
//      uint8_t *pm25String = malloc(4*sizeof(uint8_t));
//      sprintf(pm25String, "%d", GetPM2_5Value());
//      PMLen = strlen(pm25String);
//      
//      
//      uint8_t *Co2String = malloc(4*sizeof(uint8_t));
//      sprintf(Co2String, "%d", GetCo2Value());
//      Co2Len = strlen(Co2String);
//      
//      len = strlen(string1)+1+strlen(pm25String)+1+strlen(Co2String)+1;
//      uint8_t *string3 = malloc(len*sizeof(uint8_t));
//      
//      snprintf(string3,len,"%s%d%s%d%s",string1, PMLen, pm25String, Co2Len, Co2String);  
//      printf("%s\n",string3);
//      
//      GUART_CopyAppToUrTx(string3,len);         // Craig
//      udpConn->UrTxBytes += len;              // Craig
//      if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig
//        udpConn->UrTxBytes = 0;  
//      
//      free(string3);
//      xTimeSensorTemp = xTimeSensorInterval;
    }
    if((xTimeSensorInterval-xTimeSensorTemp) >= 4*1000){
      if(MBI_Baudrate != 115200){
        serial_format(&urObj, GCONFIG_GetUrDataBits(), GCONFIG_GetUrParity(), GCONFIG_GetUrStopBits());		// Craig
        serial_baud(&urObj, 115200);           // Craig
        MBI_Baudrate = 115200;
      }
      
      uint8_t len;
      uint8_t PMLen,Co2Len;
      uint8_t *string1 = "mbi:";
      
      //uint8_t *pm25String = malloc(4*sizeof(uint8_t));
      uint8_t pm25String[5] = {0};
      sprintf(pm25String, "%d", pm2_5value);
      
      if(pm2_5value >= 1000){
        PMLen = 4;
      }
      else if(pm2_5value >= 100){
        PMLen = 3;
      }
      else if(pm2_5value >= 10){
        PMLen = 2;
      }
      else{
        PMLen = 1;
      }
      //PMLen = strlen(pm25String);
      
      
      //uint8_t *Co2String = malloc(4*sizeof(uint8_t));
      uint8_t Co2String[5] = {0};
      sprintf(Co2String, "%d", co2value);
      
      if(co2value >= 1000){
        Co2Len = 4;
      }
      else if(co2value >= 100){
        Co2Len = 3;
      }
      else if(co2value >= 10){
        Co2Len = 2;
      }
      else{
        Co2Len = 1;
      }
      
      //Co2Len = strlen(Co2String);
      
      len = strlen(string1)+1+strlen(pm25String)+1+strlen(Co2String)+1;
      //uint8_t *string3 = malloc(len*sizeof(uint8_t));
      uint8_t string3[15];
      snprintf(string3,len,"%s%d%s%d%s",string1, PMLen, pm25String, Co2Len, Co2String);  
      printf("%s\n",string3);
      
      GUART_CopyAppToUrTx(string3,len);         // Craig
      udpConn->UrTxBytes += len;              // Craig
      
      
      if(udpConn->UrTxBytes > GUDPDAT_TX_BUF_SIZE)     // Craig
        udpConn->UrTxBytes = 0;  
      
      //free(string3);
      //snprintf(string3,len,"%s","               ");  
      times = 0;
      xTimeSensorTemp = xTimeSensorInterval;
    }
  }
}
