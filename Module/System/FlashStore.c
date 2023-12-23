#include "stm32f10x.h"                  // Device header
#include "MyFLASH.h"


#define STORE_START_ADDRESS  0x0800FC00

//使用SRAM对FLASH的数据进行管理
uint16_t	Store_Data[512];

void Store_Init(void){
	
	//闪存默认是FF，SRAM默认是0，所以如果是第一次使用我们就把闪存默认置0
	
	//怎么知道是第一次呢，我们把闪存的最后一页的最后一个半字设置为标志位，也就是给他一个值，例如0xA5A5
	//这样弄会有一定的问题，用户如果用到了最后一页，这样我们就不能正常存储了，所以我们得在Options/Read Only设置最大可用范围
	//0x10000->0xFC00
	if(MyFLASH_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5){
		//先擦除后写入	
		MyFLASH_ErasePage(STORE_START_ADDRESS);
		//写入我们自定义的标志位
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS,0xA5A5);
		
		//将剩余的存储空间内容设置为0
		//注意是i是从1开始的
		for(uint16_t i =1;i<512;i++){
			//为什么要成i要*2？因为一个半字占用的内容是两个地址
			MyFLASH_ProgramHalfWord(STORE_START_ADDRESS+i*2,0x0000);
		}
	}
	
	//将闪存备份的数据恢复到SRAM数组
	for(uint16_t i=0;i<512;i++){
		Store_Data[i] = MyFLASH_ReadHalfWord(STORE_START_ADDRESS +i*2);
	}
}

//存储闪存
void Store_Save(void){
	//先擦除后存储
	MyFLASH_ErasePage(STORE_START_ADDRESS);
	for(uint16_t i =0;i<512;i++){
		//写入数据
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS+i*2,Store_Data[i]);
	}
}

void Store_Clear(void){
	//全部置0
	for(uint16_t i =1;i<512;i++){
		Store_Data[i] = 0x0000;
	}
	//置0完了进行SRAM保存
	Store_Save();
}


