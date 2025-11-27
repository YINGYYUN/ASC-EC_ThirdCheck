#include "stm32f10x.h"  // Device header

#include "Delay.h"
#include "Timer.h"

#include "LED.h"
#include "Key.h"
#include "Serial.h"
#include "Car.h"
#include "Motor.h"
//#include "OLED.h"
#include "LineSensor.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>


uint8_t H_KeyNum;
uint8_t Key_Event[3] = {0}; // 分别对应UP、DOWN、CONFIRM(BACK)

//存储摇杆数据
int8_t g_joystick_LH = 0; // 左摇杆横向 (Left Horizontal)
int8_t g_joystick_LV = 0; // 左摇杆纵向 (Left Vertical)
int8_t g_joystick_RH = 0; // 右摇杆横向 (Right Horizontal)
int8_t g_joystick_RV = 0; // 右摇杆纵向 (Right Vertical)

uint8_t g_joystick_data_ready = 0;//数据就绪标志

#define DEAD_ZONE 10    // 摇杆死区
#define MAX_SPEED 99    // 最大PWM值 (电机函数限制为99)

void Handle_Manual_Control(void);

//存放红外对射式传感器数据
uint8_t Pre_sensorData[4];
uint8_t Cur_sensorData[4];
uint8_t Out_sensorData[4];

//数据帧确认与状态（用于定时判定十字路口、单线等）
uint8_t Confirm_sensorData_Flag = 1;

uint8_t Tracking_Mode_ENABLE = 0;

uint8_t Tracking_Error_Flag;
uint16_t Tracking_Error_Timeticks;

// PID参数定义
static float KP = 6.6f;   
// 比例系数（范围0.5~5.0，步距0.1）
static float KI = 0.0f;   
// 积分系数（范围0~0.5，步距0.01）
static float KD = 0.2f;   
// 微分系数（范围0~2.0，步距0.1）

// 巡线方向权重定义
static int8_t W14 = 4;     
// X1权重（1~3）
static int8_t W23 = 1;     
// X2权重（2~5）

uint8_t Cur_Flag, Pre_Flag;

// 基础速度（全局默认 - 与车上固定 PWM 行为接近）
static int16_t BASE_SPEED = 70; // 建议初始高速组

void Handle_Tracking_Control(void);

uint16_t i;

int main(void)
{	

	LED_Init();
	Key_Init();
	Serial_Init();
	Motor_Init();
//	OLED_Init();
	LineSensor_Init();
	
	Timer_Init();

/* =================== [START] 菜单初始化模块 [START] =================== */	
	char Main_Menu[][17] = {"Tracking Mode", "Manual Mode"};
	#define F_Mian_Menu				0
	#define F_Tracking_Mode			1
	#define F_Manual_Mode			2
	//光标位置
	uint8_t Main_Menu_Location = F_Tracking_Mode;
	//功能状态
	uint8_t FUNCTION_State = F_Mian_Menu;
	
//	OLED_Clear();
//	
//	OLED_Printf(0, 0, OLED_8X16, "Main Menu");
//	OLED_Printf(16, 16, OLED_8X16, Main_Menu[0]);
//	OLED_Printf(16, 32, OLED_8X16, Main_Menu[1]);
//	
//	OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, ">");
//	
//	OLED_Update();
	
	printf("[display-clear]");
	
	printf("[display,0,0,Main Menu]");
	printf("[display,16,16,%s],", Main_Menu[0]);
	printf("[display,16,32,%s],", Main_Menu[1]);
	printf("[display,0,%d,>]", 16 * Main_Menu_Location);
	
/* =================== [END] 菜单初始化模块 [END] =================== */	
	
	
	
	
	while(1)
	{
		
		
		
		
/* =================== [START] 按键处理模块 [START]==================== */		
		H_KeyNum = Key_GetNum();
		
		if(H_KeyNum == UP)
		{
			Key_Event[UP - 1] ++;
		}
		if(H_KeyNum == DOWN)
		{
			Key_Event[DOWN - 1] ++;
		}
		if(H_KeyNum == CONFIRM)
		{
			Key_Event[CONFIRM - 1] ++;
		}		
/* =================== [END] 按键处理模块 [END]==================== */
		
		
		
		
/* =================== [START] 蓝牙收发与处理模块 [START]==================== */		
		if (Serial_RxFlag == 1)
		{			
			//字符串分割
			char * Tag = strtok(Serial_RxPacket, ",");
			
			//按键解析
			if (strcmp(Tag, "key") == 0)
			{
				//NULL表示为后续分割
				char * Name = strtok(NULL, ",");
				//如果没有新的子串，函数会返回空指针
				char * Action = strtok(NULL, ",");
				
				if (strcmp(Name, "UP") == 0 && strcmp(Action, "down") == 0)
				{
					Key_Event[UP - 1] ++;
				}
				else if (strcmp(Name, "DOWN") == 0 && strcmp(Action, "down") == 0)
				{
					Key_Event[DOWN - 1] ++;
				}
				else if (strcmp(Name, "CONFIRM") == 0 && strcmp(Action, "down") == 0)
				{
					Key_Event[CONFIRM - 1] ++;
				}
			}
			
			//滑杆解析
			else if (strcmp(Tag, "slider") == 0)
			{
				char * Name = strtok(NULL, ",");
				char * Value = strtok(NULL, ",");
				
				if (strcmp(Name, "KP") == 0)
				{
					float FloatValue = atof(Value);					
					KP = FloatValue;
				}
				else
				if (strcmp(Name, "KI") == 0)
				{
					float FloatValue = atof(Value);				
					KI = FloatValue;
				}
				else 
				if (strcmp(Name, "KD") == 0)
				{
					float FloatValue = atof(Value);				
					KD = FloatValue;
				}
				else
				if (strcmp(Name, "W23") == 0)
				{
					uint8_t IntValue = atoi(Value);				
					W23 = IntValue;
				}
				else
				if (strcmp(Name, "W14") == 0)
				{
					uint8_t IntValue = atoi(Value);				
					W14 = IntValue;
				}
			}

			//摇杆解析
			else if (strcmp(Tag, "joystick") == 0 && FUNCTION_State == F_Manual_Mode)
			{
				
				// 关闭全局中断，防止在写入全局变量时被打断
//				__set_PRIMASK(1); 
								
				//左摇杆横向值
				g_joystick_LH = atoi(strtok(NULL, ","));
				//左摇杆纵向值
				g_joystick_LV = atoi(strtok(NULL, ","));
				//右摇杆横向值
				g_joystick_RH = atoi(strtok(NULL, ","));
				//右摇杆纵向值
				g_joystick_RV = atoi(strtok(NULL, ","));
						
				g_joystick_data_ready = 1;
								// 开启全局中断
//				__set_PRIMASK(0);
			}			
			Serial_RxFlag = 0;
		}
/* =================== [END] 蓝牙收发与处理模块 [END]==================== */
		
		
		
		
/* =================== [START] 主菜单显示响应模块 [START] =================== */
		
		//UP
		if (Key_Event[UP - 1])
		{
			Key_Event[UP - 1] --;
			
			if(FUNCTION_State == F_Mian_Menu)
			{
//			OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, " ");
			printf("[display,0,%d, ]", 16 * Main_Menu_Location);
			Main_Menu_Location = (Main_Menu_Location - 1 -1 + 2) % 2 + 1;
//			OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, ">");		
			printf("[display,0,%d,>]", 16 * Main_Menu_Location);				
//			OLED_Update();
			}
			
		}
		
		//DOWN
		else if (Key_Event[DOWN - 1])
		{
			Key_Event[DOWN - 1] --;
			
			if(FUNCTION_State == F_Mian_Menu)
			{				
//			OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, " ");
			printf("[display,0,%d, ]", 16 * Main_Menu_Location);
			Main_Menu_Location = (Main_Menu_Location -1 + 1) % 2 + 1;
//			OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, ">");
			printf("[display,0,%d,>]", 16 * Main_Menu_Location);				
//			OLED_Update();
			}
			
		}
		
		//CONFIRM(BACK)
		else if (Key_Event[CONFIRM - 1])
		{
			Key_Event[CONFIRM - 1] --;
			if(FUNCTION_State == F_Mian_Menu)
			{
				//更改功能状态
				FUNCTION_State = Main_Menu_Location;
				
				switch(FUNCTION_State)
				{
					//循迹模式
					case F_Tracking_Mode:
//						OLED_Clear();					
//						OLED_Printf(0, 0, OLED_8X16, "Tracking Mode");
//						OLED_Printf(0, 16, OLED_8X16, "[2]  [1][3]  [4]");
//						OLED_Printf(0, 32, OLED_8X16, " %d    %d  %d    %d ",
//								Cur_sensorData[1], Cur_sensorData[0], Cur_sensorData[2], Cur_sensorData[3]);						
//						OLED_Update();
						printf("[display-clear]");
						printf("[display,0,0,Tracking Mode]");
//						printf("[display,0,16,|2|  |1||3|  |4|]");
//						printf("[display,0,32, %d    %d  %d    %d ",
//								Cur_sensorData[1], Cur_sensorData[0], Cur_sensorData[2], Cur_sensorData[3]);
						break;
					
					//手动模式
					case F_Manual_Mode:
//						OLED_Clear();				
//						OLED_Printf(0, 0, OLED_8X16, "Manual Mode");						
//						OLED_Update();
						
						printf("[display-clear]");					
						printf("[display,0,0,Manual Mode]");
					
						break;
					
					default:
						//留一手					
						break;
				}
				
			}
			else
			{
				//返回主菜单
				FUNCTION_State = F_Mian_Menu;
				
					//一次停止指令
					Car_Stop();
				
					Tracking_Mode_ENABLE = 0;
				
//					OLED_Clear();
//	
//					OLED_Printf(0, 0, OLED_8X16, "Main Menu");
//					OLED_Printf(16, 16, OLED_8X16, Main_Menu[0]);
//					OLED_Printf(16, 32, OLED_8X16, Main_Menu[1]);					
//					OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, ">");				
//					OLED_Update();
					
					printf("[display-clear]");
					printf("[display,0,0,Main Menu]");
					printf("[display,16,16,%s],", Main_Menu[0]);
					printf("[display,16,32,%s],", Main_Menu[1]);
					printf("[display,0,%d,>]", 16 * Main_Menu_Location);
			}
			
		}
		
/* =================== [END] 主菜单显示响应模块 [END] =================== */
		
		
		
		
/* =================== [START] 功能响应模块 [START] =================== */	
		switch(FUNCTION_State)
		{
			case F_Mian_Menu:
				
		 		//二次停止指令
				Car_Stop();
			
				//数据重置
				break;
			
			case F_Tracking_Mode:
				
				Tracking_Mode_ENABLE = 1;
			
			
				//传感器示意图 [M2]			 [M1] [M3]			[M4]
				
//				OLED_Update();			
				Handle_Tracking_Control();
			
				break;
			
			case F_Manual_Mode:
				if (g_joystick_data_ready)
				{
					Handle_Manual_Control();
				}
			
				break;
			
			default:
				//留一手
			
				break;
		}
	
/* =================== [END] 功能响应模块 [END] =================== */
					
		
		
		
	}//while(1)
}//int main(void)

uint8_t X1, X2, X3, X4;
/* =================== [START] 自动巡线控制模块 [START] =================== */
void Handle_Tracking_Control(void)
{
    LineSensor_Read(Cur_sensorData);		
	
//	if( memcmp (Pre_sensorData, Cur_sensorData, 4) == 0) 
//	{
//		Confirm_sensorData_Flag ++;
//	}
//	else
//	{
//		Confirm_sensorData_Flag = 1;
//	}
//	memcpy(Pre_sensorData, Cur_sensorData, 4);
//	
//	if(Confirm_sensorData_Flag >= 2)memcpy(Out_sensorData, Cur_sensorData, 4);
    
    // 传感器位置映射
//    X1 = Out_sensorData[1];  // 左外侧
//    X2 = Out_sensorData[0];  // 左内侧
//    X3 = Out_sensorData[2];  // 右内侧
//    X4 = Out_sensorData[3];  // 右外侧
	
	X1 = Cur_sensorData[1];  // 左外侧
	X2 = Cur_sensorData[0];  // 左内侧
	X3 = Cur_sensorData[2];  // 右内侧
	X4 = Cur_sensorData[3];  // 右外侧
	
//	printf("[display,0,32, %d    %d  %d    %d ]",X1, X2, X3, X4);

	// PID 状态（保持在函数间）
	if(!(X1 == 1 && X2 == 1 && X3 == 1 && X4 == 1))
	{
		if ( X1 == 0 &&
			 X2 == 1 &&
			 X3 == 1 &&
			 X4 == 1)
		{
			Cur_Flag = F_Self_Right1;
			Self_Right1();
			if(Pre_Flag != Cur_Flag)
			{
				printf("%d\n",i);
				i = 0;
				Pre_Flag = Cur_Flag;
			}
			return;
		}
		else 
		if ( X1 == 1 &&
			 X2 == 1 &&
			 X3 == 1 &&
			 X4 == 0)
		{
			Cur_Flag = F_Self_Left1;
			Self_Left1();
			if(Pre_Flag != Cur_Flag)
			{
				printf("%d\n",i);
				i = 0;
				Pre_Flag = Cur_Flag;
			}
			return;
		}
		Tracking_Error_Flag = 0;
		
		static float prev_error = 0.0f;
		static float integral = 0.0f;

		// 计算误差（按要求的权重组合）
		float error = (float)(- (int)W14 * (int)X1
							 - (int)W23 * (int)X2
							 + (int)W23 * (int)X3
							 + (int)W14 * (int)X4);

		// 积分项累加并限幅（防止积分饱和）
		integral += error;
		const float INTEGRAL_LIMIT = 200.0f;
		if (integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
		if (integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;

		// 微分项（位置式PID使用当前误差与上次误差差值）
		float derivative = error - prev_error;

		// PID 输出（作为转向修正量）
		float pid_output = KP * error + KI * integral + KD * derivative;

		// 保存当前误差供下次微分使用
		prev_error = error;

		// 计算左右轮目标速度
		int16_t left_speed  = (int16_t)roundf((float)BASE_SPEED + pid_output);
		int16_t right_speed = (int16_t)roundf((float)BASE_SPEED - pid_output);

		// 对速度进行限幅（电机驱动允许范围 -MAX_SPEED..MAX_SPEED）
		if (left_speed > MAX_SPEED) left_speed = MAX_SPEED;
		if (left_speed < -MAX_SPEED) left_speed = -MAX_SPEED;
		if (right_speed > MAX_SPEED) right_speed = MAX_SPEED;
		if (right_speed < -MAX_SPEED) right_speed = -MAX_SPEED;

		// 输出到电机（M1/M2 接口）
		Motor_SetPWM1(left_speed);
		Motor_SetPWM2(right_speed);	
	}
	else
	{
		Tracking_Error_Flag = 1;
		if (Tracking_Error_Timeticks > 10000)
		{
			Motor_SetPWM1(0);
			Motor_SetPWM2(0);	
		}
	}
}
/* =================== [END] 自动巡线控制模块 [END] =================== */




/* =================== [START] 手动摇杆控制模块 [START] =================== */

void Handle_Manual_Control(void)
{
    // 1. 读取并清零标志和数据，这是原子操作，要快
    int8_t LH = g_joystick_LH;
    int8_t LV = g_joystick_LV;
    // int8_t RH = g_joystick_RH; // 目前未使用
    // int8_t RV = g_joystick_RV; // 目前未使用
    
    g_joystick_data_ready = 0; // 关键：立即清零标志

    // 2. 死区处理
    if (abs(LV) < DEAD_ZONE) LV = 0;
    if (abs(LH) < DEAD_ZONE) LH = 0;
    
    // 3. 差分驱动计算
    // 基础速度由纵向摇杆控制
    int16_t base_speed = LV; 
    
    // 转向速度差由横向摇杆控制
    // 将LH从[-100, 100]映射到[-base_speed, base_speed]
    int16_t turn_speed = (int16_t)((float)LH * (float)abs(base_speed) / 100.0f);
    
    // 计算左右轮最终速度
    int16_t left_motor_pwm = base_speed + turn_speed;
    int16_t right_motor_pwm = base_speed - turn_speed;
    
    // 4. 速度限幅，防止超出电机控制范围
    if (left_motor_pwm > MAX_SPEED) left_motor_pwm = MAX_SPEED;
    if (left_motor_pwm < -MAX_SPEED) left_motor_pwm = -MAX_SPEED;
    if (right_motor_pwm > MAX_SPEED) right_motor_pwm = MAX_SPEED;
    if (right_motor_pwm < -MAX_SPEED) right_motor_pwm = -MAX_SPEED;

    // 5. 输出到电机

    Motor_SetPWM1(left_motor_pwm);
    Motor_SetPWM2(right_motor_pwm);
}

/* =================== [END] 手动摇杆控制模块 [END] =================== */




void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		Key_Tick();
		
		i ++;
		
		if(Tracking_Error_Flag)
		{
			Tracking_Error_Timeticks ++;
		}
		else
		{
			Tracking_Error_Timeticks = 0;
		}

		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
