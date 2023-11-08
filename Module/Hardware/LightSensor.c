#include "stm32f10x.h"                  // Device header

void LightSensor_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//获取光敏传感器输入状态
uint8_t LightSensor_Get(void){
	//光敏电阻是上拉，所以端口初始为1，光敏电阻有光时，电阻为0，输入为0
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13);
}
