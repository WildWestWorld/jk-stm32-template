#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

//0xD0 是默认的 I2C从机地址 就是110 1000 =>0x68 
//但因为我们还需要一个读写位 所以0x68 得 左移一位 也就是 0xD0
#define MPU6050_ADDRESS 0xD0

//指定地址写
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data){
	MyI2C_Start();
	//发送从机地址
	MyI2C_SendByte(MPU6050_ADDRESS);
	//接收应答位
	MyI2C_ReceiveAck();
	
	//指定寄存地址
	MyI2C_SendByte(RegAddress);
	//接收应答位
	MyI2C_ReceiveAck();
	
	
	//发送数据
	MyI2C_SendByte(Data);
	//接收应答位
	MyI2C_ReceiveAck();
	
	MyI2C_Stop();
}

//指定地址读
uint8_t MPU6050_ReadReg(uint8_t RegAddress){
	
	//发送从机地址，指定寄存器 通知寄存器存放数据到I2C从机 ->读取I2C从机的数据
	
	//接收数据
	uint8_t Data;
	
	
	MyI2C_Start();
	//发送从机地址
	MyI2C_SendByte(MPU6050_ADDRESS);
	//接收应答位
	MyI2C_ReceiveAck();
	
	//指定寄存地址 发送信号到寄存器地址，寄存器地址会把当前的数据 放到从机地址里面
	MyI2C_SendByte(RegAddress);
	//接收应答位
	MyI2C_ReceiveAck();
	
	
	
	MyI2C_Start();
	//给从机地址 发送读取信号
	//获取寄存器地址存放的数据
	MyI2C_SendByte(MPU6050_ADDRESS|0x01);
	//接收应答位
	MyI2C_ReceiveAck();
		
	//接收数据
	Data =MyI2C_ReceiveByte();
	//发送Ack1停止接收
	MyI2C_SendAck(1);
			
	MyI2C_Stop();

	return Data;
}


void MPU6050_Init(void){
	MyI2C_Init();									//先初始化底层的I2C
	
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
	
}

uint8_t MPU6050_GetID(void){
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);

}


/**
  * 函    数：MPU6050获取数据
  * 参    数：AccX AccY AccZ 加速度计X、Y、Z轴的数据，使用输出参数的形式返回，范围：-32768~32767
  * 参    数：GyroX GyroY GyroZ 陀螺仪X、Y、Z轴的数据，使用输出参数的形式返回，范围：-32768~32767
  * 返 回 值：无
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
	*AccX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
	*AccY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
	*AccZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);		//读取陀螺仪X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);		//读取陀螺仪X轴的低8位数据
	*GyroX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);		//读取陀螺仪Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);		//读取陀螺仪Y轴的低8位数据
	*GyroY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	*GyroZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
};




//硬件

/**
  * 函    数：MPU6050等待事件
  * 参    数：同I2C_CheckEvent
  * 返 回 值：无
  */
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//给定超时计数时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
	{
		Timeout --;										//等待时，计数值自减
		if (Timeout == 0)								//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;										//跳出等待，不等了
		}
	}
}

//指定地址写
void MPU6050_WriteReg_Hardware(uint8_t RegAddress,uint8_t Data){
	/*MyI2C_Start();
	//发送从机地址
	MyI2C_SendByte(MPU6050_ADDRESS);
	//接收应答位
	MyI2C_ReceiveAck();
	
	//指定寄存地址
	MyI2C_SendByte(RegAddress);
	//接收应答位
	MyI2C_ReceiveAck();
	
	
	//发送数据
	MyI2C_SendByte(Data);
	//接收应答位
	MyI2C_ReceiveAck();
	
	MyI2C_Stop();*/
	
	//开启 起始条件
	//因为硬件不是阻塞式的，我们需要进行手动阻塞
	 I2C_GenerateSTART(I2C2,ENABLE);
	//检测对应的事件是否 执行
	//检测EV5
	//while(I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5



	//发送从机地址
	//I2C_Send7bitAddress(I2C2,从机地址,发送/接收)
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);
	
	//检测EV6
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);					//等待EV5

	//发送寄存器地址
	I2C_SendData(I2C2,RegAddress);
	//检测EV8
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);				

	//发送 数据
	I2C_SendData(I2C2,Data);
		MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				
	I2C_GenerateSTOP(I2C2, ENABLE);											//硬件I2C生成终止条件

	
};



uint8_t MPU6050_ReadReg_Hardware(uint8_t RegAddress){
	
	//发送从机地址，指定寄存器 通知寄存器存放数据到I2C从机 ->读取I2C从机的数据
	
	//接收数据
	uint8_t Data;
	
	//开启 起始条件
	//因为硬件不是阻塞式的，我们需要进行手动阻塞
	 I2C_GenerateSTART(I2C2,ENABLE);
	//检测对应的事件是否 执行
	//检测EV5
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					
	//发送从机地址
	//I2C_Send7bitAddress(I2C2,从机地址,发送/接收)
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);
	
	//检测EV6
		MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);					

	//发送寄存器地址
	I2C_SendData(I2C2,RegAddress);
	//检测EV8
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);					

	
	//开启 起始条件
	//因为硬件不是阻塞式的，我们需要进行手动阻塞	
	I2C_GenerateSTART(I2C2,ENABLE);
	//检测对应的事件是否 执行
	//检测EV5
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					

	//发送 读取信号给 从机地址 
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);
	//检测EV6
	//这次EV6结束后得立刻 ACK置0 STOP 置1
	//因为 时序不是阻塞式的，如果ACK不置0，此时你想停止收发，ACK还是1，就自动应答会继续接收了
	
		MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);					

	//ACK置0
	I2C_AcknowledgeConfig(I2C2, DISABLE);									//在接收最后一个字节之前提前将应答失能
	I2C_GenerateSTOP(I2C2, ENABLE);											//在接收最后一个字节之前提前申请停止条件
	
			MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);					

	Data = I2C_ReceiveData(I2C2);											//接收数据寄存器
	//ACK置1
	I2C_AcknowledgeConfig(I2C2, ENABLE);									//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
	
	return Data;
}




void MPU6050_Init_Hardware(void){

	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);		//开启I2C2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
		/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为复用开漏输出
	
		/*I2C初始化*/
	I2C_InitTypeDef I2C_InitStructure;						//定义结构体变量
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;				//模式，选择为I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 50000;				//时钟速度，选择为50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		//时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				//应答，选择使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;				//自身地址，从机模式下才有效
	I2C_Init(I2C2, &I2C_InitStructure);						//将结构体变量交给I2C_Init，配置I2C2
	
	/*I2C使能*/
	I2C_Cmd(I2C2, ENABLE);									//使能I2C2，开始运行
	
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
	
}

uint8_t MPU6050_GetID_Hardware(void){
	return MPU6050_ReadReg_Hardware(MPU6050_WHO_AM_I);
}



void MPU6050_GetData_Hardware(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
	*AccX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
	*AccY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
	*AccZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_GYRO_XOUT_H);		//读取陀螺仪X轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_GYRO_XOUT_L);		//读取陀螺仪X轴的低8位数据
	*GyroX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_GYRO_YOUT_H);		//读取陀螺仪Y轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_GYRO_YOUT_L);		//读取陀螺仪Y轴的低8位数据
	*GyroY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg_Hardware(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	DataL = MPU6050_ReadReg_Hardware(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	*GyroZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
};


