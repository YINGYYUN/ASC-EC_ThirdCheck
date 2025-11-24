#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Delay.h"

#include "Car.h"

//黑0白1

//0000
//1001
void Go_Ahead(void)
{
	Motor_SetPWM1(75);
	Motor_SetPWM2(75);
}
void Go_Back(void)
{
	Motor_SetPWM1(-75);
	Motor_SetPWM2(-60);
}

//1011
void Turn_Left(void)
{
	Motor_SetPWM1(60);
	Motor_SetPWM2(75);
}

//1101
void Turn_Right(void)
{
	Motor_SetPWM1(60);
	Motor_SetPWM2(75);
}

//0001
//0111
void Self_Left1(void)
{
	Motor_SetPWM1(-75);
	Motor_SetPWM2(75);
}

//1000
//1110
void Self_Right1(void)
{
	Motor_SetPWM1(75);
	Motor_SetPWM2(-75);
}

//0011
void Self_Left2(void)
{
	Motor_SetPWM1(-75);
	Motor_SetPWM2(75);
}

//1100
void Self_Right2(void)
{
	Motor_SetPWM1(75);
	Motor_SetPWM2(-75);
}


void Car_Stop(void)
{
	Motor_SetPWM1(0);
	Motor_SetPWM2(0);
}
