#ifndef _TIMER_F103_H__
#define _TIMER_F103_H__


#include "stm32f10x.h"
#include "GLOBAL_SMARTGATEWAY.h"

#define PrioSystick Prio_FSKStack_SystickTimeOut

void SystickStart(void);
int SystickGet(void);

void Timer_IT_Enable( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
void Timer_CkEnable(TIM_TypeDef *Timer);
void Timer_Set_Period(TIM_TypeDef *Timer, int ARR, int PSC );

#define TimerOn(Timer) (Timer->CR1|=1)
#define TimerOff(Timer) (Timer->CR1&=~1)


#endif

