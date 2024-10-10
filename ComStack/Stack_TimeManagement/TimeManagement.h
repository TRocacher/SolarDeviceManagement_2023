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
 * @Brief  Fonction bloquante qui redonne la main après x ms
 * @note : Elabore la date d'échéance d'un chrono interne (autre que les 6 définis),
					attend l'échéance
 * @param : in x : nombre de ms à compter (1ms près par définition)
 * @return : _
 * Exemple :Time_Delay_x_ms(100); // attend 100ms (1ms près)
 **/ 
void TimeManag_Delay_x_ms(int x);

/* fonction obsolète ...*/
void _Delay_x_ms(int x);

void TimeManag_TimeOutInit(void);
void TimeManag_TimeOutStart(TimeBaseName Name , int ms);
char TimeManag_GetTimeOutStatus(TimeBaseName Name);

void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us);
void TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us);

#endif
