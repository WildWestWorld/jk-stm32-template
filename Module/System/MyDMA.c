#include "stm32f10x.h"                  // Device header

uint16_t MyDMA_Size;

void MyDMA_Init(uint32_t AddrA,uint32_t AddrB,uint16_t Size){
	MyDMA_Size= Size;
	
	
	//初始化DMA定时器
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//初始化DMA
	DMA_InitTypeDef DMA_InitStructure;
	//设置地址，我们这的地址 都是外部传入进来
	DMA_InitStructure.DMA_PeripheralBaseAddr =AddrA;
	//数据宽度
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	//地址是否自增
	DMA_InitStructure.DMA_PeripheralInc =DMA_PeripheralInc_Enable;
	
	//内存地址，也是外部传入
	DMA_InitStructure.DMA_MemoryBaseAddr=AddrB;
	//数据宽度
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	//地址是否自增
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	//传输方向
	//传输到源头 外部站点作为数据源
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	//传输寄存器 这里的数值就是传输的次数 从外部传入
	DMA_InitStructure.DMA_BufferSize=Size;
	//模式 循环模式自减到0 会循环 	正常模式传输计数器不重装 自减到0停下来
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	//是否软件触发
	DMA_InitStructure.DMA_M2M=DMA_M2M_Enable;
	//设置优先级
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	//初始化 DMA
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	  //先不启动DMA
	//DMA_Cmd(DMA1_Channel1,ENABLE);
	
  //先不启动DMA
	DMA_Cmd(DMA1_Channel1,DISABLE);
}

//数据变化时，用于重新传输的函数
void MyDMA_Transfer(void){
	//传输计数器赋值，必须要给DMA失能
	DMA_Cmd(DMA1_Channel1,DISABLE);
	//重新传入数值
	DMA_SetCurrDataCounter(DMA1_Channel1,MyDMA_Size);
	//重新开启DMA
	DMA_Cmd(DMA1_Channel1,ENABLE);
	//查看转换是否完成
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET){};
	DMA_ClearFlag(DMA1_FLAG_TC1);
	
}
