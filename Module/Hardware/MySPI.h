#ifndef __MYSPI_H
#define __MYSPI_H

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
uint8_t MySPI_SwapByteV2(uint8_t ByteSend);


//硬件
void MySPI_Init_Hardware(void);
uint8_t MySPI_SwapByte_Hardware(uint8_t ByteSend);

#endif
