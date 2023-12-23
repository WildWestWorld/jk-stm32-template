#include "stm32f10x.h"                  // Device header

//读取FLASH数据
/**
  * 函    数：FLASH读取一个32位的字
  * 参    数：Address 要读取数据的字地址
  * 返 回 值：指定地址下的数据
  */
uint32_t MyFLASH_ReadWord(uint32_t Address)
{
	//__IO = volatile 不优化，例如，如果不加IO，我们让函数空运行会被程序优化掉，认为代码无用就不执行了
	//加了IO就不会执行优化
	//uint32_t * =uint32_t *类型的指针 也就是一种指针
	//最外层的 * 取地址
	
	return *((__IO uint32_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个16位的半字
  * 参    数：Address 要读取数据的半字地址
  * 返 回 值：指定地址下的数据
  */
uint16_t MyFLASH_ReadHalfWord(uint32_t Address)
{
	return *((__IO uint16_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个8位的字节
  * 参    数：Address 要读取数据的字节地址
  * 返 回 值：指定地址下的数据
  */
uint8_t MyFLASH_ReadByte(uint32_t Address)
{
	return *((__IO uint8_t *)(Address));	//使用指针访问指定地址下的数据并返回
}
             
//擦除FLASH全部Page数据
void MyFLASH_EraseAllPages(void){
	//在擦出之前 必须解锁
	FLASH_Unlock();
	//擦除
	FLASH_EraseAllPages();
	//擦除完毕后再次上锁
	FLASH_Lock();
}
//擦除单个Page 必须传入地址
void MyFLASH_ErasePage(uint32_t PageAddress){
	//在擦出之前 必须解锁
	FLASH_Unlock();
	//擦除
	FLASH_ErasePage(PageAddress);
	//擦除完毕后再次上锁
	FLASH_Lock();	
}

/**
  * 函    数：FLASH编程字
  * 参    数：Address 要写入数据的字地址
  * 参    数：Data 要写入的32位数据
  * 返 回 值：无
  */
void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
	FLASH_Unlock();							//解锁
	FLASH_ProgramWord(Address, Data);		//编程字
	FLASH_Lock();							//加锁
}

/**
  * 函    数：FLASH编程半字
  * 参    数：Address 要写入数据的半字地址
  * 参    数：Data 要写入的16位数据
  * 返 回 值：无
  */
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
	FLASH_Unlock();							//解锁
	FLASH_ProgramHalfWord(Address, Data);	//编程半字
	FLASH_Lock();							//加锁
}




