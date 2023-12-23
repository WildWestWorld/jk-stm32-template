#include "stm32f10x.h"                  // Device header

//print 是输出到屏幕，单片机没有屏幕需要引入stdio.h 进行重定向
#include <stdio.h>
//用于封装sprint
#include <stdarg.h>




uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

//Hex专用
uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];

//Text专用
//Text 的发送部分直接用printf/sendString
char Serial_RxPacket_Text[100];
uint8_t Serial_RxFlag_Text;


void Serial_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	//浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//为什么是PA9  因为是TX口就(发送口)在PA9 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	//上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//为什么是PA9  因为是RX口就(接收口)在PA10 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;			//模式，选择为发送模式 与 接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	//进阶功能中断
	//开启中断 开启完毕后可以在中断中接收数据
	//开启RXNE标志位到NVIC的输出
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//配置NVIC
	//设置优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	//选择中断通道 我们是用USART1 所以选择USART1的通道
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART1,Byte);//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	//查看发送是否完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		
	
}
//上传多条信息
void Serial_SendArray(uint8_t *Array,uint16_t Length){
	uint16_t i;
	for(i =0;i<Length;i++){
		Serial_SendByte(Array[i]);
	}
	
}
void Serial_SendString(char *String){
	uint8_t i;
	//'\0' = 空字符串
	for(i=0;String[i] !='\0';i++){
		Serial_SendByte(String[i]);
	}
	
}

//生成 10/100/1000 数 方便计算
uint32_t Serial_Pow(uint32_t X,uint32_t Y){
	uint32_t Result =1;
	while(Y--){
		Result *=X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number,uint8_t Length){
	uint8_t i;
	for(i=0;i<Length;i++){
		//Length -i -1 =>从最大位开始
		//+0 转成字符串
		Serial_SendByte(Number/(Serial_Pow(10,Length -i -1)) %10 +  '0') ;
	}
	
}

//print 是输出到屏幕，单片机没有屏幕需要引入stdio.h 进行重定向
//fputc是 printf 函数底层
int fputc(int ch,FILE *f){
	Serial_SendByte(ch);
	return ch;
	
}

//封装Sprint
///... 用来接受后面的可变参数列表
void Serial_Printf(char *format,...){
	char String[100];
	//va_list 类型名 arg 变量名
	va_list arg;
	//从format位置开始接受参数表 放在arg里面
	va_start(arg,format);
	//这里的sprintf 得改成vsprintf  因为sprintf无法携带参数
	vsprintf(String,format,arg);
	//释放参数表
	va_end(arg);
	Serial_SendString(String);
}

//处理中断
void USART1_IRQHandler(void){
//如果中断开启
	if(USART_GetITStatus(USART1,USART_IT_RXNE)== SET){
	//接收数据
		Serial_RxData =USART_ReceiveData(USART1);
		//设置自定义的标志位
		Serial_RxFlag=1;
		
		//清除标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
	}

	
	/*//HEX
	static uint8_t RxState =0;
	//记录接收了几个变量 记录了四个就该从状态2变为状态3
	static uint8_t RxPacketNum = 0;
	
		if(USART_GetITStatus(USART1,USART_IT_RXNE)){
		
			//接收参数
		uint8_t RxData = USART_ReceiveData(USART1);
			
		if(RxState ==0){
			if(RxData == 0xFF){
				RxState =1;
				//重置RxPacketNum 
				RxPacketNum = 0;
			}
		}else if(RxState ==1){
			//因为字符是一个个传入的所以我们直接赋值
			Serial_RxPacket[RxPacketNum] =RxData;
			if(RxPacketNum>=3){
				RxState=2;
			}
			
						RxPacketNum++;

		}else if(RxState ==2){
				if(RxData == 0xFE){
					RxState =0;
					Serial_RxFlag =1;
				}
			
		}*/
		
	/*		//Text
	static uint8_t RxState =0;
	//记录接收了几个变量 记录了四个就该从状态2变为状态3
	static uint8_t RxPacketNum = 0;
	
		if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET){
		
			//接收参数
		uint8_t RxData = USART_ReceiveData(USART1);
			
		if(RxState ==0){
			if(RxData == '@' && Serial_RxFlag ==0){
				RxState =1;
				//重置RxPacketNum 
				RxPacketNum = 0;
			}
		}else if(RxState ==1){
			//不是换行符就接收参数
			if(RxData =='\r'){
				RxState =2;
			}else{
						Serial_RxPacket_Text[RxPacketNum] =RxData;
						RxPacketNum++;
				
			}
			


		}else if(RxState ==2){
				if(RxData == '\n'){
					RxState =0;
					Serial_RxPacket_Text[RxPacketNum] = '\0';			//将收到的字符数据包添加一个字符串结束标志

					Serial_RxFlag_Text =1;
					//Serial_RxFlag =1;
				}
			
		}
			
		
		
		//清除标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
	}*/
	
	
}

//获取自定义标志位
//已读就置0
uint8_t Serial_GetRxFlag(void){
	if(Serial_RxFlag==1){
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}

//获取存储的数据
uint8_t Serial_GetRxData(void){
	return Serial_RxData;
}

//Hex 发送数据包
void Serial_SendPacket(void){
	//发送包头 0xff
	Serial_SendByte(0xFF);
	//发送包的内容
	Serial_SendArray(Serial_TxPacket,4);
	//发送包尾 0xfe
	Serial_SendByte(0xFE);
	
}
