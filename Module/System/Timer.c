#include "stm32f10x.h"                  // Device header

//引入外部变量 uint16_t Num
//extern uint16_t Num;

void Timer_Init(void){
	//开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//ETR引脚在PA0上，所以我们要 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	//配置默认时钟
	//TIM_InternalClockConfig(TIM2);
	//配置ETR引脚外部时钟模式2
	//TIM_ETRClockMode2Config(TIM2,
	//TIM_ExtTRGPSC_OFF不分频
	//TIM_ExtTRGPolarity_NonInverted 高电平，上升沿有效
	//0x00无滤波
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00);
	
	
	//配置时基电路
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	//设置分频TIM_CKD_DIV1 一分频
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	//计数器模式 向上计数模式
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
//定时频率 =72M/(PSC+1)/(ARR+1)
//因为预分频器和计数器都有一个数的偏差所以在这里减1	
	//TIM_TimeBaseInitStructure.TIM_Period=10000-1;
	//TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;
	
		TIM_TimeBaseInitStructure.TIM_Period=10-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler=1-1;
	
	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;

		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure); 
	
	//处理 复位后 中断在显示前 就开始 的问题
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	
	//开启 使能更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	//初始化NVIC
	//设置NVIC优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//配置NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	
	NVIC_Init(&NVIC_InitStructure);
	
	//启动定时器
	TIM_Cmd(TIM2,ENABLE);
	
}

uint16_t Timer_GetCounter(void){
	return TIM_GetCounter(TIM2);
}

/*void TIM2_IRQHandler(void){
	//获取中断标志位 
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){
		
		Num ++;
		
		//清楚中断标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}*/

