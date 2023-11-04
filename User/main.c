#include "stm32f10x.h"                  // Device header

int main(void)
{
	//寄存器点灯
	//16进制 4位2进制组成一位
	//我们是想打开在APB2ENR 寄存器下的 第5位二进制的IOP C定时器接口
	//为什么打开是的这个接口？ 因为我们需要点的灯是在C口
	//也就是打开C口的定时器
	//RCC->APB2ENR=0x0000010;
	
	
	//打开寄存器端口的C 13口 控制权限
	//GPIOC->CRH =0x00300000;
	
	//控制灯光的开关
	//ODR意味着 是端口输出寄存器
	//这里的C13就是我们的灯
	//灯是低电平点亮，所以是为0点亮
	//GPIOC->ODR=0x00000000;
	
	//库函数点灯
	//开启定时器
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//打开端口权限
	//使用GPIO_Init(端口，包含配置的结构体)
	GPIO_InitTypeDef GPIO_InitStructure;
	
/*typedef enum
{ GPIO_Mode_AIN = 0x0,
  GPIO_Mode_IN_FLOATING = 0x04,
  GPIO_Mode_IPD = 0x28,
  GPIO_Mode_IPU = 0x48,
  GPIO_Mode_Out_OD = 0x14,
  GPIO_Mode_Out_PP = 0x10,
  GPIO_Mode_AF_OD = 0x1C,
  GPIO_Mode_AF_PP = 0x18
}GPIOMode_TypeDef;*/

//GPIO设置的模式
GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;

//13号口
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
//GPIO的速度
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	//初始化GPIO
	GPIO_Init(GPIOC,&GPIO_InitStructure);
		
	//设置高电平
	//GPIO_SetBits(GPIOC,GPIO_Pin_13);
	//设置低电平
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
	while(1){
	
	}
	
}
