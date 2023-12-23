#ifndef __FLASHSTORE_H
#define __FLASHSTORE_H

//存储 FLASH 的数组
extern uint16_t Store_Data[];

void Store_Init(void);
void Store_Save(void);
void Store_Clear(void);
#endif
