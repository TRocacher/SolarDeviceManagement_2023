
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


#include <TimeManagement_RmDv.h>

/*======================================================================================================
========================================================================================================
		D�lais par boucle, peu pr�cis...			
========================================================================================================		
=======================================================================================================*/
// vérifier les timing !!
#define SysClk_MHz 24
#define N ((11990*SysClk_MHz)/72)

/*______________________________________________________________________________
_______________________ void Delay_x_ms(int x)	________________________________
 
    R�le:Fonction permettant d'attendre environ x ms
			Proc�de par boucle while , donc peu pr�cis.

    Param : int x, dur�e demand�e en ms

* __________________________________________________________________________________*/

void Delay_x_ms(int x)
{
	int i;
	int j;
	i=0;
	while (i<x)
	{
		j=0;
		while (j<N)
		{
			j++;
		}
		i++;
	}		
}


/*======================================================================================================
========================================================================================================
	Gestion du temps par Systick (TimeOut) 	
  5 bases de tmps peuvent �tre utilis�es, voir .h. Facilement modifiable.
Principe :
- Le Systick doit �tre lanc� au d�marrage : TimeManag_TimeOutInit
- le user choisit un chronom�tre (une base de temps) puis date le time out : TimeManag_TimeOutStart
- le user interroge r�guli�rement le module pour savoir si le terme est �chu ou pas : TimeManag_GetTimeOutStatus
========================================================================================================
=======================================================================================================*/


int TimeManag_TimeOutDate[TimeBaseName_Nb];  // la date d'�ch�ance pour les diverses base de tps

/*______________________________________________________________________________
_______________________ TimeManag_TimeOutInit(void)	____________
 
    R�le: D�marre le systick avec un cadencement de 100�s
    Param : 
* __________________________________________________________________________________*/
void TimeManag_TimeOutInit(void)
{
	SystickStart();
}



/*______________________________________________________________________________
________________ void TimeManag_TimeOutStart(TimeBaseName Name , int ms)  ______
 
   R�le: Fixe la date de TimeOut
   Param : 
Name : nom de la base de tps utilis�e (5 possibles) Chrono_1 � Chrono_5
int ms,  dur�e demand�e en ms, � partir de l'instant pr�sent.
Le temps �coul�, provoquera un timeout.

 _______________________________________________________________________________*/
void TimeManag_TimeOutStart(TimeBaseName Name , int ms)
{
	TimeManag_TimeOutDate[Name]=10*ms+SystickGet();
}


/*______________________________________________________________________________
________________ char TimeManag_GetTimeOutStatus(TimeBaseName Name) ____________
 
   R�le: renvoie 1 si le temps est �coul�, 0 sinon
   Param : Name , nom de la base de tps utilis�e (5 possibles) Chrono_1 � Chrono_5
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

   R�le: Initialise le Timer de la FSM
   Param : 
- Timer : le timer associ�
- Prio : la priorit� d'IT
- IT_function : nom du callback associ�
- FSMPeriod_us : p�riode exprim�e en �s. Attention max = 65 535�s (prescaler � 0) Limite non v�rif�e
par soft.
 _______________________________________________________________________________*/
void TimeManag_FSMTimerInit(TIM_TypeDef *Timer, char Prio, void (*IT_function) (void),int FSMPeriod_us)
{
	Timer_CkEnable(Timer);
	Timer_Set_Period(Timer, FSMPeriod_us*72-1, 2-1 );
	Timer_IT_Enable(Timer, Prio, IT_function);
}

/*______________________________________________________________________________
________ TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us)) __________
 
   R�le: Ajuste une nouvelle p�riode pour le Timer
   Param : FSMPeriod_us : p�riode exprim�e en �s. Attention max = 65 535�s 
(prescaler � 0) Limite non v�rif�epar soft.
 _______________________________________________________________________________*/
void TimeManag_SetFSMPeriod(TIM_TypeDef *Timer,int FSMPeriod_us)
{
	Timer_Set_Period(Timer, FSMPeriod_us*72-1, 2-1 ); 

}
