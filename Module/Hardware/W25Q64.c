#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "W25Q64_Ins.h"

void W25Q64_Init(void){
	MySPI_Init();

}

//读取设备ID
//MID 8位的厂商ID
//DID 16位的设备ID
void W25Q64_ReadID(uint8_t *MID,uint16_t *DID){
	//SS引脚置低，开始传输
	MySPI_Start();
	//发送字节 0x9F 
	//为什么发送0x9F 因为是手册规定的
	//W25Q64_JEDEC_ID = 0x9F
	MySPI_SwapByte(W25Q64_JEDEC_ID);
	
	//只要发送任意信息就能收到厂商ID
	//0xFF 任意信息
	//W25Q64_DUMMY_BYTE = 0xFF
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	
	//再次发送得到设备高8位
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	
	//高八位放到数据的高八位
	*DID <<= 8;
	
	//再次发送得到设别低8位
	//设备低8位 整合到数据
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	
	MySPI_Stop();
}

/**
  * 函    数：W25Q64写使能
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WriteEnable(void)
{
	MySPI_Start();								//SPI起始
	//W25Q64_WRITE_ENABLE = 0x06
	//发送0x06 就开启使能了
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);		//交换发送写使能的指令
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64等待忙 用于检测W25Q64 是否处于繁忙状态
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 100000;							//给定超时计数时间
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;								//跳出等待，不等了
		}
	}
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64页编程
  * 参    数：Address 页编程的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray	用于写入数据的数组
  * 参    数：Count 要写入数据的数量，范围：0~256
  * 返 回 值：无
  * 注意事项：写入的地址范围不能跨页
  */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
		uint16_t i;
	
		//进行写入操作时必须 使能
	  W25Q64_WriteEnable();						//写使能
		
		MySPI_Start();								//SPI起始
		MySPI_SwapByte(W25Q64_PAGE_PROGRAM);		//交换发送页编程的指令
		
	//发送 地址
	//假设Address = 0x 123456  右移动16位(一个数=4位)=> 0x12
		MySPI_SwapByte(Address>>16);
		//假设Address = 0x 123456  右移动8位(一个数=4位)=> 0x1234 SwapByte 限定2个字节，所以低位才会被接收
		MySPI_SwapByte(Address>>8);
		MySPI_SwapByte(Address);
		
	//根据Count 来发送DataArray的数据
		for(i=0;i<Count;i++){
				MySPI_SwapByte(DataArray[i]);
		}
		MySPI_Stop();
	
		
		W25Q64_WaitBusy();							//等待忙

}


/**
  * 函    数：W25Q64扇区擦除（4KB）
  * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
  * 返 回 值：无
  */
void W25Q64_SectorErase(uint32_t Address)
{
	//进行写入操作时必须 使能
	W25Q64_WriteEnable();						//写使能
	
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	MySPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte(Address);					//交换发送地址7~0位
	MySPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy();							//等待忙
}

/**
  * 函    数：W25Q64读取数据
  * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
  * 参    数：Count 要读取数据的数量，范围：0~0x800000
  * 返 回 值：无
  */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte(W25Q64_READ_DATA);			//交换发送读取数据的指令
	//发送地址
	MySPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		//W25Q64_DUMMY_BYTE = 0xFF  发送无效的数据，置换回需要的数据
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
	MySPI_Stop();								//SPI终止
}




	
//硬件

void W25Q64_Init_Hardware(void){
	MySPI_Init_Hardware();

}

//读取设备ID
//MID 8位的厂商ID
//DID 16位的设备ID
void W25Q64_ReadID_Hardware(uint8_t *MID,uint16_t *DID){
	//SS引脚置低，开始传输
	MySPI_Start();
	//发送字节 0x9F 
	//为什么发送0x9F 因为是手册规定的
	//W25Q64_JEDEC_ID = 0x9F
	MySPI_SwapByte_Hardware(W25Q64_JEDEC_ID);
	
	//只要发送任意信息就能收到厂商ID
	//0xFF 任意信息
	//W25Q64_DUMMY_BYTE = 0xFF
	*MID = MySPI_SwapByte_Hardware(W25Q64_DUMMY_BYTE);
	
	//再次发送得到设备高8位
	*DID = MySPI_SwapByte_Hardware(W25Q64_DUMMY_BYTE);
	
	//高八位放到数据的高八位
	*DID <<= 8;
	
	//再次发送得到设别低8位
	//设备低8位 整合到数据
	*DID |= MySPI_SwapByte_Hardware(W25Q64_DUMMY_BYTE);
	
	MySPI_Stop();
}

/**
  * 函    数：W25Q64写使能
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WriteEnable_Hardware(void)
{
	MySPI_Start();								//SPI起始
	//W25Q64_WRITE_ENABLE = 0x06
	//发送0x06 就开启使能了
	MySPI_SwapByte_Hardware(W25Q64_WRITE_ENABLE);		//交换发送写使能的指令
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64等待忙 用于检测W25Q64 是否处于繁忙状态
  * 参    数：无
  * 返 回 值：无
  */
void W25Q64_WaitBusy_Hardware(void)
{
	uint32_t Timeout;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte_Hardware(W25Q64_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 100000;							//给定超时计数时间
	while ((MySPI_SwapByte_Hardware(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;								//跳出等待，不等了
		}
	}
	MySPI_Stop();								//SPI终止
}

/**
  * 函    数：W25Q64页编程
  * 参    数：Address 页编程的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray	用于写入数据的数组
  * 参    数：Count 要写入数据的数量，范围：0~256
  * 返 回 值：无
  * 注意事项：写入的地址范围不能跨页
  */
void W25Q64_PageProgram_Hardware(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
		uint16_t i;
	
		//进行写入操作时必须 使能
	  W25Q64_WriteEnable_Hardware();						//写使能
		
		MySPI_Start();								//SPI起始
		MySPI_SwapByte_Hardware(W25Q64_PAGE_PROGRAM);		//交换发送页编程的指令
		
	//发送 地址
	//假设Address = 0x 123456  右移动16位(一个数=4位)=> 0x12
		MySPI_SwapByte_Hardware(Address>>16);
		//假设Address = 0x 123456  右移动8位(一个数=4位)=> 0x1234 SwapByte 限定2个字节，所以低位才会被接收
		MySPI_SwapByte_Hardware(Address>>8);
		MySPI_SwapByte_Hardware(Address);
		
	//根据Count 来发送DataArray的数据
		for(i=0;i<Count;i++){
				MySPI_SwapByte_Hardware(DataArray[i]);
		}
		MySPI_Stop();
	
		
		W25Q64_WaitBusy_Hardware();							//等待忙

}


/**
  * 函    数：W25Q64扇区擦除（4KB）
  * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
  * 返 回 值：无
  */
void W25Q64_SectorErase_Hardware(uint32_t Address)
{
	//进行写入操作时必须 使能
	W25Q64_WriteEnable_Hardware();						//写使能
	
	MySPI_Start();								//SPI起始
	MySPI_SwapByte_Hardware(W25Q64_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	MySPI_SwapByte_Hardware(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte_Hardware(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte_Hardware(Address);					//交换发送地址7~0位
	MySPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy_Hardware();							//等待忙
}

/**
  * 函    数：W25Q64读取数据
  * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
  * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
  * 参    数：Count 要读取数据的数量，范围：0~0x800000
  * 返 回 值：无
  */
void W25Q64_ReadData_Hardware(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{	
	
	uint32_t i;
	MySPI_Start();								//SPI起始
	MySPI_SwapByte_Hardware(W25Q64_READ_DATA);			//交换发送读取数据的指令
	//发送地址
	MySPI_SwapByte_Hardware(Address >> 16);				//交换发送地址23~16位
	MySPI_SwapByte_Hardware(Address >> 8);				//交换发送地址15~8位
	MySPI_SwapByte_Hardware(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		//W25Q64_DUMMY_BYTE = 0xFF  发送无效的数据，置换回需要的数据
		DataArray[i] = MySPI_SwapByte_Hardware(W25Q64_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
	MySPI_Stop();								//SPI终止
}

