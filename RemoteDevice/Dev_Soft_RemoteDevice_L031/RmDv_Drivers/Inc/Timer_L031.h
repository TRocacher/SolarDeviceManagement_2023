#ifndef _TIMER_L031_H__
#define _TIMER_L031_H__


#include "stm32l0xx.h"


void SystickStart(void);
int SystickGet(void);

void Timer_IT_Enable( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
void Timer_CkEnable(TIM_TypeDef *Timer);
void Timer_CkDisable(TIM_TypeDef *Timer);
void Timer_Set_Period(TIM_TypeDef *Timer, int ARR, int PSC );
void Timer_ConfPWM_Ch2(TIM_TypeDef *Timer);

typedef enum {	PWM=0x6,
				Clear=0x4
}TimerMode;
void Timer_SetOutputMode(TIM_TypeDef *Timer, TimerMode Mode);

#define Timer_Enable_IT_Callback(Timer) ((Timer->DIER)|=(1<<0))
#define Timer_Disable_IT_Callback(Timer) ((Timer->DIER)&=~(1<<0))

#define Timer_Set_Duty(Timer,Channel,Pcent) (Timer->CCR##Channel=(Pcent*(Timer->ARR))/100)
#define Timer_EnablePWMChan(Timer,Channel) (Timer->CCER|=TIM_CCER_CC##Channel##E)
#define Timer_DisablePWMChan(Timer,Channel) (Timer->CCER&=~(TIM_CCER_CC##Channel##E))



#endif

