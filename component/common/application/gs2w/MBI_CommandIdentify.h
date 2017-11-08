
#ifndef MBI_CommandIdentify_H
#define MBI_CommandIdentify_H

#include <stdint.h>                      /* standard types definitions                      */

#define Wifi_Server 0x0D
#define wifi_Client 0x0E
#define Ignore 0x00

#define MTUofEthernet 1500 
#define Cmd16BufferLen 256


//Variable declare
extern const uint8_t ID_Array[8];
extern const uint8_t Version;		
extern const uint8_t PacketHeaderLen;
extern const uint16_t PacketNo;
extern uint16_t PacketTotLen;
extern uint16_t PacketHeaderChkSum;
extern uint8_t Protocol;
extern uint8_t Reserved[3];
extern uint8_t SourceAddress[8];
extern uint8_t DestinationAddress[8];

extern uint8_t CommandType;
extern uint8_t CommandCode;
extern uint16_t CommandCount;
extern uint16_t CommandLen;
extern uint16_t CommandChkSum;
extern uint8_t MemoryStartAddress[4];
extern uint8_t PacketFormat[36];

extern uint8_t SourceAddr;
extern uint8_t DestinationAddr;
/*UDP / TCP send received flag*/
extern uint8_t ReadyToSendFlag;
extern uint8_t ReceivedFlag;

/*UDP TCP Packet define*/
extern uint8_t UDPRxBuffer[MTUofEthernet];
extern uint8_t UDPTxBuffer[MTUofEthernet];
extern uint8_t TCPRxBuffer[MTUofEthernet];
extern uint8_t TCPTxBuffer[MTUofEthernet];

extern uint8_t Cmd16Buffer[Cmd16BufferLen];


//Function declare
uint8_t* GetSourceAddress(uint8_t Source,uint8_t ignore);
uint8_t* GetDestinationAddress(uint8_t Source,uint8_t ignore);
uint8_t IdentifyCommand(uint8_t *buf, uint8_t check_flag);
void GetPacketHeaderChkSum();
uint8_t* GetPacketFormat();

uint16_t GetCommandLength(uint8_t CmdCode);
uint16_t GetRearCommandChkSum(uint8_t *buf);

#endif /* xxx_H */




