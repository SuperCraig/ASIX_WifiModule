
#ifndef MBI_ASIXTask_H
#define MBI_ASIXTask_H

#include <stdint.h>                      /* standard types definitions                      */
#include <MBI_CommandProcess.h>
#include <MBI_CommandIdentify.h>


void MBI_Init(void);
void MBI_UDPTask(void *param);
void MBI_TCPTask(void *param);
void RTC_READ_Task(void *pParam);
void Sensor_DataBack_Task(void *param);

extern char Co2_1[18];           //TP paramerters
extern char PM_1[17];
extern char PM_2[17];
extern char PM_3[17];
extern char Temp_Hum_1[19];     
extern char Temp_Hum_2[19];
#endif /* xxx_H */




