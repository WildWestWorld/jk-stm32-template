#include "stm32f10x.h"                  // Device header
#include "PWM.h"

//依赖PWM文件
void Servo_Init(void){

	PWM_Init();
}

void Servo_SetAngle(float Angle){
		// 500->0度  2500->180度	
   PWM_SetCompare1(Angle/180 *2000 +500);
}
