#ifndef __MPU6050_H
#define __MPU6050_H


void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data);

//指定地址读
uint8_t MPU6050_ReadReg(uint8_t RegAddress);


void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);


void MPU6050_WriteReg_Hardware(uint8_t RegAddress,uint8_t Data);

//指定地址读
uint8_t MPU6050_ReadReg_Hardware(uint8_t RegAddress);


void MPU6050_Init_Hardware(void);
uint8_t MPU6050_GetID_Hardware(void);

void MPU6050_GetData_Hardware(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);


#endif
