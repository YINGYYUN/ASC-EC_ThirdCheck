#include "stm32f10x.h"  // Device header

#include "Delay.h"
#include "Timer.h"

#include "LED.h"
#include "Key.h"
#include "Serial.h"
#include "Motor.h"
#include "OLED.h"
#include "LineSensor.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

//uint8_t i;

uint8_t H_KeyNum;
uint8_t Key_Event[3] = {0}; // 分别对应UP、DOWN、CONFIRM(BACK)







int main(void)
{	

	LED_Init();
	Key_Init();
	Serial_Init();
	Motor_Init();
	OLED_Init();
	LineSensor_Init();
	
	Timer_Init();
	
//	OLED_Clear();
//	
//	OLED_Printf(0, 0, OLED_8X16,"TxPacket");
//	OLED_Printf(0, 32, OLED_8X16,"RxPacket");
//	
//	OLED_Update();




/* =================== [START] 菜单初始化模块 [START] =================== */	
	char Main_Menu[][17] = {"Tracking Mode", "Manual Mode"};
	#define F_Mian_Menu				0
	#define F_Tracking_Mode			1
	#define F_Manual_Mode			2
	//光标位置
	uint8_t Main_Menu_Location = F_Tracking_Mode;
	//功能状态
	uint8_t FUNCTION_State = F_Mian_Menu;
	
	OLED_Clear();
	
	OLED_Printf(0, 0, OLED_8X16, "Main Menu");
	OLED_Printf(16, 16, OLED_8X16, Main_Menu[0]);
	OLED_Printf(16, 32, OLED_8X16, Main_Menu[1]);
	
	OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, ">");
	
	OLED_Update();
	
	printf("[display-clear]");
	
	printf("[display,0,0,Main Menu]");
	printf("[display,16,16,%s],", Main_Menu[0]);
	printf("[display,16,32,%s],", Main_Menu[1]);
	printf("[display,0,%d,>]", 16 * Main_Menu_Location);
/* =================== [END] 菜单初始化模块 [END] =================== */	


	
	
	//存放红外对射式传感器数据
	uint8_t sensorData[4];
	
	while(1)
	{
		
		
		
		
/* =================== [START] 按键处理模块 [START]==================== */		
		H_KeyNum = Key_GetNum();
		
		if(H_KeyNum == UP)
		{
			Key_Event[UP - 1] ++;
//			i++;
		}
		if(H_KeyNum == DOWN)
		{
			Key_Event[DOWN - 1] ++;
//			i++;
		}
		if(H_KeyNum == CONFIRM)
		{
			Key_Event[CONFIRM - 1] ++;
//			i++;
		}
		
//		OLED_ShowNum(2,1,i,2);
//		OLED_ShowNum(2,3,Key_Event[0],2);
//		OLED_ShowNum(2,5,Key_Event[1],2);
//		OLED_ShowNum(2,7,Key_Event[2],2);
		
/* =================== [END] 按键处理模块 [END]==================== */
		
		
		
		
/* =================== [START] 蓝牙收发与处理模块 [START]==================== */		
		if (Serial_RxFlag == 1)
		{
//			OLED_Printf(0, 48, OLED_8X16, "                ");
//			OLED_Printf(0, 48, OLED_8X16, Serial_RxPacket);
//			
//			OLED_Update();
			
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
//					i++;
//					printf("Key,UP\r\n");
				}
				else if (strcmp(Name, "DOWN") == 0 && strcmp(Action, "down") == 0)
				{
					Key_Event[DOWN - 1] ++;
//					i++;
//					printf("Key,DOWN\r\n");
				}
				else if (strcmp(Name, "CONFIRM") == 0 && strcmp(Action, "down") == 0)
				{
					Key_Event[CONFIRM - 1] ++;
//					i++;
//					printf("Key,CONFIRM\r\n");
				}
			}
			
//			//滑杆解析
//			else if (strcmp(Tag, "slider") == 0)
//			{
//				char * Name = strtok(NULL, ",");
//				char * Value = strtok(NULL, ",");
//				
//				if (strcmp(Name, "1") == 0)
//				{
//					//字符串转换为int
//					uint8_t IntValue = atoi(Value);					
//					printf("slider,1,%d\r\n", IntValue);
//				}
//				if (strcmp(Name, "2") == 0)
//				{
//					//字符串转换为double
//					float FloatValue = atof(Value);				
//					printf("slider,2,%f\r\n", FloatValue);
//				}
//			}
//
//			//摇杆解析
//			else if (strcmp(Tag, "joystick") == 0)
//			{
//				//左摇杆横向值
//				int8_t LH = atoi(strtok(NULL, ","));
//				//左摇杆纵向值
//				int8_t LV = atoi(strtok(NULL, ","));
//				//右摇杆横向值
//				int8_t RH = atoi(strtok(NULL, ","));
//				//右摇杆纵向值
//				int8_t RV = atoi(strtok(NULL, ","));
//				
//				printf("joystick, %d, %d, %d, %d\r\n", LH, LV, RH, RV);
//			}			
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
				
			OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, " ");
			printf("[display,0,%d, ]", 16 * Main_Menu_Location);
			Main_Menu_Location = (Main_Menu_Location - 1 -1 + 2) % 2 + 1;
			OLED_Printf(0, 16 * Main_Menu_Location , OLED_8X16, ">");
			printf("[display,0,%d,>]", 16 * Main_Menu_Location );
			
			OLED_Update();
			}
			
		}
		
		//DOWN
		else if (Key_Event[DOWN - 1])
		{
			Key_Event[DOWN - 1] --;
			
			if(FUNCTION_State == F_Mian_Menu)
			{
				
			OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, " ");
			printf("[display,0,%d, ]", 16 * Main_Menu_Location);
			Main_Menu_Location = (Main_Menu_Location -1 + 1) % 2 + 1;
			OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, ">");
			printf("[display,0,%d,>]", 16 * Main_Menu_Location);
			
			OLED_Update();
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
						OLED_Clear();
					
						OLED_Printf(0, 0, OLED_8X16, "Tracking Mode");
						OLED_Printf(0, 16, OLED_8X16, "M2134:%d %d %d %d",
								sensorData[1], sensorData[0], sensorData[2], sensorData[3]);
						
						OLED_Update();
						
						printf("[display-clear]");
					
						printf("[display,0,0,Tracking Mode]");
						printf("[display,0,16,M2134:%d %d %d %d]", 
								sensorData[1], sensorData[0], sensorData[2], sensorData[3]);					
					
						break;
					
					//手动模式
					case F_Manual_Mode:
						OLED_Clear();
					
						OLED_Printf(0, 0, OLED_8X16, "Manual Mode");
						
						OLED_Update();
						
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
					Motor_SetPWM1(0);
					Motor_SetPWM2(0);
				
					OLED_Clear();
	
					OLED_Printf(0, 0, OLED_8X16, "Main Menu");
					OLED_Printf(16, 16, OLED_8X16, Main_Menu[0]);
					OLED_Printf(16, 32, OLED_8X16, Main_Menu[1]);
					
					OLED_Printf(0, 16 * Main_Menu_Location, OLED_8X16, ">");
					
					OLED_Update();
					
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
				Motor_SetPWM1(0);
				Motor_SetPWM2(0);
			
				//数据重置
				break;
			
			case F_Tracking_Mode:
				//传感器数据读取
				LineSensor_Read(sensorData);
			
				OLED_Printf(0, 16, OLED_8X16, "M2134:%d %d %d %d",
						sensorData[1], sensorData[0], sensorData[2], sensorData[3]);
			
				//传感器示意图 [M2]			 [M1] [M3]			[M4]
				
				OLED_Update();
				
				printf("[display,0,16,M2134:%d %d %d %d]", 
						sensorData[1], sensorData[0], sensorData[2], sensorData[3]);					
			
			
			
				break;
			
			case F_Manual_Mode:
				
			
				break;
			
			default:
				//留一手
			
				break;
		}
	
/* =================== [END] 功能响应模块 [END] =================== */
					
		
		
		
	}//while(1)
}//int main(void)


void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
