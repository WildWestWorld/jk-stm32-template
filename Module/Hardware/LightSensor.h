#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

void LightSensor_Init(void);

//获取光敏传感器输入状态
uint8_t LightSensor_Get(void);
#endif
