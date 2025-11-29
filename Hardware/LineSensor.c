// LineSensor.c

#include "LineSensor.h"
#include <stddef.h>

/**
  * @brief  初始化四路循迹传感器的GPIO引脚.
  * @param  None
  * @retval None
  */
void LineSensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能所有传感器引脚的GPIO时钟（只需要使能一次GPIOA时钟）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置所有引脚为上拉输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // 同时配置所有4个引脚
    GPIO_InitStructure.GPIO_Pin = LINE_SENSOR_X1_PIN | LINE_SENSOR_X2_PIN | 
                                 LINE_SENSOR_X3_PIN | LINE_SENSOR_X4_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  读取四路循迹传感器的状态。
  * @param  pData: 指向一个uint8_t类型的数组指针，用于存储读取到的状态。
  * @retval None
  */
void LineSensor_Read(uint8_t* pData)
{
    if (pData == NULL) // 安全检查：防止传入空指针
    {
        return;
    }

    // 读取每个引脚的电平状态并存储到数组中
    // GPIO_ReadInputDataBit返回值为Bit_RESET(0)或Bit_SET(1)
    pData[0] = GPIO_ReadInputDataBit(LINE_SENSOR_X1_PORT, LINE_SENSOR_X1_PIN);
    pData[1] = GPIO_ReadInputDataBit(LINE_SENSOR_X2_PORT, LINE_SENSOR_X2_PIN);
    pData[2] = GPIO_ReadInputDataBit(LINE_SENSOR_X3_PORT, LINE_SENSOR_X3_PIN);
    pData[3] = GPIO_ReadInputDataBit(LINE_SENSOR_X4_PORT, LINE_SENSOR_X4_PIN);
}
