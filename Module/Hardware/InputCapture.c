#include "stm32f10x.h"                  // Device header


void IC_Init(void)
{
	//因为PWM已经输出了TIM2了 所以我们在这需要输出TIM2
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA6引脚初始化为 上拉输入
																	//受外设控制的引脚，均需要配置为复用模式
	 
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM3为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	//这里采用的是测周法测频率，前面讲过测周法测频率要N的值越大越好，这里N对应着CNT传给CCR的值
	//这里是通过输入捕获的方式，在上升沿时将时基单元里面CNT的值传递给CCR 
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;                 //计数周期，即ARR的值，尽可能要设置大的值，防止数值溢出
	//频率=72M/(PSC+1)/100
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元



	//初始化IC
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;
	//滤波 越大滤波 越强
	TIM_ICInitStructure.TIM_ICFilter=0xF;
	//上升沿触发
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	//分频
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	//选择捕获通道
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
	
	//TIM_ICInit(TIM3,&TIM_ICInitStructure);
	//配置了一个 就自动设置另外的所有端口
		TIM_PWMIConfig(TIM3,&TIM_ICInitStructure);
		
	
	//选择触发源
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	//设置从模式
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	TIM_Cmd(TIM3,ENABLE);
}

//获取频率
uint32_t IC_GetFreq(void){
	//fc=72M/(PSC+1)
	//fc = 72 000 000 / 72 = 1000 000
	//+1 去除误差
	return 1000000 / (TIM_GetCapture1(TIM3)+1);
}
//获取占空比
uint32_t IC_GetDuty(void){
	
	//TIM_GetCapture2(TIM3) / TIM_GetCapture1(TIM3) 
	//得出的范围是(0,1)
	//*100范围就是(0,100)
	return (TIM_GetCapture2(TIM3) + 1) * 100 / (TIM_GetCapture1(TIM3) + 1);	//占空比Duty = CCR2 / CCR1 * 100，这里不执行+1的操作也可 
}

