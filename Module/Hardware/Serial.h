#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];

extern char Serial_RxPacket_Text[100];
extern uint8_t Serial_RxFlag_Text;



void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_Printf(char *format,...);


uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxFlag(void);


void Serial_SendPacket(void);
#endif
