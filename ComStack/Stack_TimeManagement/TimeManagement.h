#ifndef _TimeManag_H__
#define _TimeManag_H__



#include "Timer_F103.h"


typedef enum {
	Chrono_1,
	Chrono_2,
	Chrono_3,
	Chrono_4,
	Chrono_5,
	Chrono_6, /* 28/08/24:    */ 
}TimeBaseName;
#define TimeBaseName_Nb 6 /* 28/08/24:   5 */

void Delay_x_ms(int x);

void TimeManag_TimeOutInit(void);
void TimeManag_TimeOutStart(TimeBaseName Name , int ms);
char TimeManag_GetTimeOutStatus(TimeBaseName Name);

void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us);
void TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us);

#endif
