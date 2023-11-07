#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//GPIO_Mode_IPU 上拉输入模式 按下低电平 松开高电平
	
/*	上拉输入模式（Input Pull-Up）： 
	在上拉输入模式下，引脚在未连接到外部电平时会被拉高至高电平状态（逻辑值为1）。
	
当你按下连接到上拉输入引脚的按钮或开关时，引脚的状态会从高电平切换到低电平（逻辑值为0）。
	所以按钮左侧得接负极
	这样可以检测到按钮的按下。*/
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//检测按钮 是否按下
//uint8_t = unsigned char 无符号整形
uint8_t Key_GetNum(void){
	uint8_t KeyNum= 0;
	//读取端口的高低电平情况
	//如果按键按下
 if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0){
	//按键按下需要去除抖动
		Delay_ms(20);
	 //如果一直按下 程序就不要往下执行
	 while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0){};
		Delay_ms(20);
	 KeyNum =1;
 
 }
 
 
 	//读取端口的高低电平情况
	//如果按键按下
 if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0){
	//按键按下需要去除抖动
		Delay_ms(20);
	 //如果一直按下 程序就不要往下执行
	 while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0){};
		Delay_ms(20);
	 KeyNum =2;
 
 }
 
	
	
	
	return KeyNum;
}

