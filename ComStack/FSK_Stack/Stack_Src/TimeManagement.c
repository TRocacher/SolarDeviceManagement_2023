
/* =================================================================================
* ==================   TimeManagement_F103.c	     =================================
 *
 *   Created on: 15/08/23
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *  ------------------------------------------------------------------------------
 *
 *
* =================================================================================*/


#include "TimeManagement.h"

/*======================================================================================================
========================================================================================================
		Délais par boucle, peu précis...			
========================================================================================================		
=======================================================================================================*/


/*______________________________________________________________________________
_______________________ void Delay_x_ms(int x)	________________________________
 
    Rôle:Fonction permettant d'attendre environ x ms
			Procède par boucle while , donc peu précis.
			Testé pour CPU à 72MHz
    Param : int x, durée demandée en ms

* __________________________________________________________________________________*/

void Delay_x_ms(int x)
{
	int i;
	int j;
	i=0;
	while (i<x)
	{
		j=0;
		while (j<11990)
		{
			j++;
		}
		i++;
	}		
}


/*======================================================================================================
========================================================================================================
	Gestion du temps par Systick (TimeOut) 	
  5 bases de tmps peuvent être utilisées, voir .h. Facilement modifiable.
Principe :
- Le Systick doit être lancé au démarrage : TimeManag_TimeOutInit
- le user choisit un chronomètre (une base de temps) puis date le time out : TimeManag_TimeOutStart
- le user interroge régulièrement le module pour savoir si le terme est échu ou pas : TimeManag_GetTimeOutStatus
========================================================================================================
=======================================================================================================*/


int TimeManag_TimeOutDate[TimeBaseName_Nb];  // la date d'échéance pour les diverses base de tps

/*______________________________________________________________________________
_______________________ TimeManag_TimeOutInit(void)	____________
 
    Rôle: Démarre le systick avec un cadencement de 100µs
    Param : 
* __________________________________________________________________________________*/
void TimeManag_TimeOutInit(void)
{
	SystickStart();
}



/*______________________________________________________________________________
________________ void TimeManag_TimeOutStart(TimeBaseName Name , int ms)  ______
 
   Rôle: Fixe la date de TimeOut
   Param : 
Name : nom de la base de tps utilisée (5 possibles) Chrono_1 à Chrono_5
int ms,  durée demandée en ms, à partir de l'instant présent.
Le temps écoulé, provoquera un timeout.

 _______________________________________________________________________________*/
void TimeManag_TimeOutStart(TimeBaseName Name , int ms)
{
	TimeManag_TimeOutDate[Name]=10*ms+SystickGet();
}


/*______________________________________________________________________________
________________ char TimeManag_GetTimeOutStatus(TimeBaseName Name) ____________
 
   Rôle: renvoie 1 si le temps est écoulé, 0 sinon
   Param : Name , nom de la base de tps utilisée (5 possibles) Chrono_1 à Chrono_5
 _______________________________________________________________________________*/
char TimeManag_GetTimeOutStatus(TimeBaseName Name)
{
	if ((SystickGet()-TimeManag_TimeOutDate[Name])>=0) return 1;
	else return 0;
}



/*======================================================================================================
========================================================================================================
	Gestion du timer de la FSM
========================================================================================================
=======================================================================================================*/
/*______________________________________________________________________________
__void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us)

   Rôle: Initialise le Timer de la FSM
   Param : 
- Timer : le timer associé
- Prio : la priorité d'IT
- IT_function : nom du callback associé
- FSMPeriod_us : période exprimée en µs. Attention max = 65 535µs (prescaler à 0) Limite non vérifée
par soft.
 _______________________________________________________________________________*/
void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us)
{
	Timer_CkEnable(Timer);
	Timer_Set_Period(Timer, FSMPeriod_us*72-1, 2-1 );
	Timer_IT_Enable( Timer, Prio, IT_function);
}

/*______________________________________________________________________________
________ TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us)) __________
 
   Rôle: Ajuste une nouvelle période pour le Timer
   Param : FSMPeriod_us : période exprimée en µs. Attention max = 65 535µs 
(prescaler à 0) Limite non vériféepar soft.
 _______________________________________________________________________________*/
void TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us)
{
	Timer_Set_Period(Timer, FSMPeriod_us*72-1, 2-1 ); 

}
