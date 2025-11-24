// LineSensor.h

#ifndef __LINESENSOR_H
#define __LINESENSOR_H

#include "stm32f10x.h" // 引入STM32标准库头文件

// --- 引脚定义 ---
// 根据你的硬件连接修改这里
#define LINE_SENSOR_X1_PIN     GPIO_Pin_3
#define LINE_SENSOR_X1_PORT    GPIOA
#define LINE_SENSOR_X1_RCC     RCC_APB2Periph_GPIOA

#define LINE_SENSOR_X2_PIN     GPIO_Pin_4
#define LINE_SENSOR_X2_PORT    GPIOA
#define LINE_SENSOR_X2_RCC     RCC_APB2Periph_GPIOA

#define LINE_SENSOR_X3_PIN     GPIO_Pin_5
#define LINE_SENSOR_X3_PORT    GPIOA
#define LINE_SENSOR_X3_RCC     RCC_APB2Periph_GPIOA

#define LINE_SENSOR_X4_PIN     GPIO_Pin_6
#define LINE_SENSOR_X4_PORT    GPIOA
#define LINE_SENSOR_X4_RCC     RCC_APB2Periph_GPIOA

// --- 函数声明 ---

/**
  * @brief  初始化四路循迹传感器的GPIO引脚.
  * @param  None
  * @retval None
  */
void LineSensor_Init(void);

/**
  * @brief  读取四路循迹传感器的状态。
  * @param  pData: 指向一个uint8_t类型的数组指针，用于存储读取到的状态。
  *         数组长度应至少为4。
  *         pData[0] -> X1 (黑线)
  *         pData[1] -> X2 (黄线)
  *         pData[2] -> X3 (绿线)
  *         pData[3] -> X4 (蓝线)
  * @retval None
  * @note   传感器检测到黑线时，对应位返回 0，未检测到返回 1。
  *         （这取决于你的传感器模块，大部分是低电平有效）
  */
void LineSensor_Read(uint8_t* pData);

#endif
