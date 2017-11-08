#include "MBI_CommandIdentify.h"
#include "MBI_ASIXTask.h"
#include "device.h"
#include "serial_api.h"
#include "sockets.h"
#include "api.h"
#include "main.h"
#include "gconfig.h"
#include "gudpdat.h"
#include "guart.h"
#include "gs2w.h"
#include "gconf_ap.h"
#include "guart.h"


typedef struct PM25{
	uint8_t StartBit;
	uint8_t Vout_H;
	uint8_t Vout_L;
	uint8_t Vref_H;
	uint8_t Vref_L;
	uint16_t Chksum;
	uint8_t StopBit;
	uint16_t DensityVoltage;	
}PM25;

extern PM25 pm1;
extern uint16_t Coefficient[14];

extern uint16_t pm1value;
extern uint16_t pm2_5value;
extern uint16_t pm10_0value;
extern uint16_t co2value;
  
uint16_t CoefDecision(uint8_t voltage);

uint16_t GetDensityValue(PM25 *pm25);

uint16_t GetPM25Value(GUDPDAT_CONN_T *udpConn);

void PLANTOWERSensor(GUDPDAT_CONN_T *udpConn);

uint16_t GetPM1Value();
uint16_t GetPM2_5Value();
uint16_t GetPM10_0Value();
uint16_t GetCo2Value();


uint8_t stringCount(char *string);
void SendTo_TP(GUDPDAT_CONN_T *udpConn,char *string);
void IntToString(char* string,int num);