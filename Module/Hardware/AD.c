#include "stm32f10x.h"                  // Device header

void AD_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//断开 GPIO_Mode_AIN GPIO
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	//填充通道
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_0,Rank,Any)
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	 
	ADC_InitTypeDef ADC_InitStructure;
	//设置为独立模式
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	//数据对齐方式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	//设置触发模式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	//单次转换
	ADC_InitStructure.ADC_ContinuousConvMode=	DISABLE;
	//是否为扫描模式 扫描模式多通道 非扫描模式单通道
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	
	
	ADC_InitStructure.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	//开启ADC电源
	//复位校准
	ADC_ResetCalibration(ADC1);
	//检测 是否复位校准 完成
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){};
	
	//复位完毕 后开启校验
	ADC_StartCalibration(ADC1);
	//检测校准 是否完成
	while(ADC_GetCalibrationStatus(ADC1)== SET){};
} 




uint16_t AD_Value[4];
void AD_DMA_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//初始化DMA定时器
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//断开 GPIO_Mode_AIN GPIO
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	//填充通道
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_0,Rank,Any)
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	 
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);
	
	ADC_InitTypeDef ADC_InitStructure;
	//设置为独立模式
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	//数据对齐方式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	//设置触发模式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	//单次转换
	//ADC_InitStructure.ADC_ContinuousConvMode=	DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode=	ENABLE;
	//是否为扫描模式 扫描模式多通道 非扫描模式单通道
	
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;
	//因为有4个位置所以这里得写4
	ADC_InitStructure.ADC_NbrOfChannel=4;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	

	
	//初始化DMA
	DMA_InitTypeDef DMA_InitStructure;
	//设置地址，我们这的地址 设置成系统默认地址 (uint32_t)&ADC1->DR =0x4001244C
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&ADC1->DR;
	//数据宽度 我们想要DR寄存器低16位的数据 所以我们这里选HalfWord
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	//地址是否自增 选择不自增 始终转运 同一个位置的数据
	DMA_InitStructure.DMA_PeripheralInc =DMA_PeripheralInc_Disable;
	
	//内存地址，也是外部传入
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)AD_Value;
	//数据宽度
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	//地址是否自增
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	//传输方向
	//传输到源头 外部站点作为数据源
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	//传输寄存器 这里的数值就是传输的次数 从外部传入
	//因为有4个通道所以是4
	DMA_InitStructure.DMA_BufferSize=4;
	//模式 循环模式自减到0 会循环 	正常模式传输计数器不重装 自减到0停下来
	//DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;
	//是否软件触发
	// 硬件触发 厨师做好了，再叫我去端菜
	//硬件触发 就必须要选择Channel1
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	//设置优先级
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	//初始化 DMA
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);

  //启动DMA
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//开启DMA 触发AD信号开关
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	//开启ADC电源
	//复位校准
	ADC_ResetCalibration(ADC1);
	//检测 是否复位校准 完成
	while(ADC_GetResetCalibrationStatus(ADC1) == SET){};
	
	//复位完毕 后开启校验
	ADC_StartCalibration(ADC1);
	//检测校准 是否完成
	while(ADC_GetCalibrationStatus(ADC1)== SET){};
		
		
			
	//开启 AD、转换器(循环模式)
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
} 



/*
uint16_t AD_GetValue(void){
	//开启 AD、转换器
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//检测是否完成开启完毕
	//ADC_GetFlagStatus(ADC1,需要检测的状态)
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET){};
	//我们在RegularChannelConfig设置了ADC_SampleTime_55Cycles5=> 采样周期是55.5
//转换周期是固定的12.5 =>68个周期
		//ADCCLK是72Mhz的6分频=>12MHz
		//执行一个周期时间=周期/频率  =>68/12MHz=5.6us
		//所以我们花了5.6us等待命令执行
		
		//取结果 获得AD转换的结果
		//ADC_GetConversionValue  = 直接读取DR寄存器
		return ADC_GetConversionValue(ADC1);
}*/


//根据Channel 获取AD
uint16_t AD_GetValue(uint8_t ADC_Channel){
	//填充通道
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_55Cycles5);

	
	//开启 AD、转换器
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//检测是否完成开启完毕
	//ADC_GetFlagStatus(ADC1,需要检测的状态)
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET){};
	//我们在RegularChannelConfig设置了ADC_SampleTime_55Cycles5=> 采样周期是55.5
//转换周期是固定的12.5 =>68个周期
		//ADCCLK是72Mhz的6分频=>12MHz
		//执行一个周期时间=周期/频率  =>68/12MHz=5.6us
		//所以我们花了5.6us等待命令执行
		
		//取结果 获得AD转换的结果
		//ADC_GetConversionValue  = 直接读取DR寄存器
		return ADC_GetConversionValue(ADC1);
}


//根据Channel 获取AD
void AD_DMA_GetValue(void){
		DMA_Cmd(DMA1_Channel1,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel1,4);
		DMA_Cmd(DMA1_Channel1,ENABLE);

	
	//开启 AD、转换器
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
		//查看转换是否完成
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET){};
	DMA_ClearFlag(DMA1_FLAG_TC1);

}




