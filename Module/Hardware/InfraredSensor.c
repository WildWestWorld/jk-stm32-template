#include "stm32f10x.h"                  // Device header

//遮挡次数
uint16_t InfraredSensor_Count;


void InfraredSensor_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//上拉输入 默认高电平
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
		
	//开启中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;	
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	//因为原本是上拉模式.一般为高电平 所以是下降沿触发
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	//设置NVIC
	//设置优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//使用中断的话，NVIC的Channel就要选 EXTI15_10_IRQn
	NVIC_InitStructure.NVIC_IRQChannel =EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	//设置中断优先级和响应优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);

	
	

	EXTI_Init(&EXTI_InitStructure);
}
//设置遮挡次数
uint16_t InfraredSensor_Get(void){
	return InfraredSensor_Count;
}



//操作中断
void EXTI15_10_IRQHandler(void){
	//确认是否我们选择中断是否正常运转
	if(EXTI_GetITStatus(EXTI_Line14) == SET){
		InfraredSensor_Count++;
			//清除中断标志位，否则系统会一直请求中断
		 EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

