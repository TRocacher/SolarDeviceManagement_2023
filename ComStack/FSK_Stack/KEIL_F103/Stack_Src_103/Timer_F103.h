#ifndef _TIMER_F103_H__
#define _TIMER_F103_H__


#include "stm32f10x.h"


void SystickStart(void);
int SystickGet(void);

void Timer_IT_Enable( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
void Timer_CkEnable(TIM_TypeDef *Timer);
void Timer_Set_Period(TIM_TypeDef *Timer, int ARR, int PSC );

#endif

