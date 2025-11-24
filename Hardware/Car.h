#ifndef __CAR_H
#define __CAR_H

//#define Standard_Speed_1 75
//#define Standard_Speed_2 75
//#define Standard_Speed_3 75
//#define Standard_Speed_4 60

void Go_Ahead(void);//1
void Go_Back(void);//1
void Turn_Left(void);//3
void Turn_Right(void);//3
void Self_Left1(void);//1
void Self_Right1(void);//1
void Self_Left2(void);//2
void Self_Right2(void);//2
void Car_Stop(void);//0

#define F_Go_Ahead			0
#define F_Go_Back			1
#define F_Turn_Left			2
#define F_Turn_Right		3
#define F_Self_Left1		4
#define F_Self_Right1		5
#define F_Self_Left2		6
#define F_Self_Right2		7
#define F_Car_Stop			8

#endif
