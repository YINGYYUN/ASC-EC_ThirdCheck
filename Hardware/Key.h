#ifndef __KEY_H
#define __KEY_H

#define UP 			1
#define DOWN 		2
#define CONFIRM 	3


void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);

#endif
