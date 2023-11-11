#ifndef __INFRARED_SENSOR_H
#define __INFRARED_SENSOR_H

void InfraredSensor_Init(void);
//void EXTI15_10_IRQHandler(void);
uint16_t InfraredSensor_Get(void);
#endif
