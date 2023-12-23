#ifndef __AD_H
#define __AD_H

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);


extern uint16_t AD_Value[4];
void AD_DMA_Init(void);
void AD_DMA_GetValue(void);

#endif

