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


/**
 * @name  Time_Delay_x_ms(...)
 * @Brief  Fonction bloquante qui redonne la main apr�s x ms
 * @note : Elabore la date d'�ch�ance d'un chrono interne (autre que les 6 d�finis),
					attend l'�ch�ance
 * @param : in x : nombre de ms � compter (1ms pr�s par d�finition)
 * @return : _
 * Exemple :Time_Delay_x_ms(100); // attend 100ms (1ms pr�s)
 **/ 
void TimeManag_Delay_x_ms(int x);

/* fonction obsol�te ...*/
void _Delay_x_ms(int x);

void TimeManag_TimeOutInit(void);
void TimeManag_TimeOutStart(TimeBaseName Name , int ms);
char TimeManag_GetTimeOutStatus(TimeBaseName Name);

void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us);
void TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us);

#endif
