
#include "Timer_L031.h"



//**************************************************************************************************************
//**************************************************************************************************************
// 							GESTION TIMEOUT / Systick
//		Pour le STM32L031 on trouve TIM2, TIM21 et TIM22 (TIM2 légèrement différent)
//						il existe aussi LPTIM non utilisé ici
//**************************************************************************************************************
//**************************************************************************************************************

int Cpt_100us;

void SystickStart(void)
{
	Cpt_100us=0;
	SysTick->LOAD=100*9-1; // 72MHz/8 = 9Mhz, config � 100�s
	SysTick->CTRL|=1<<1; // Interruption locale valid�e
	SysTick->CTRL|=1<<0; // systick on
}


int SystickGet(void)
{
	return Cpt_100us;
}


/******************************************************************************************************************
	Interruption 100us

R�le :
Param : 
*****************************************************************************************************************/
void SysTick_Handler(void)
{
	Cpt_100us++;
}


//**************************************************************************************************************
//**************************************************************************************************************
// 							Timers
//**************************************************************************************************************
//**************************************************************************************************************





void Timer_CkEnable(TIM_TypeDef *Timer)
{
	
//  activation horloge periph :
if (Timer==TIM2)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM2EN;
if (Timer==TIM21)  (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM21EN;
if (Timer==TIM22)  (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM22EN;
}

void Timer_Set_Period(TIM_TypeDef *Timer, int ARR, int PSC )
{
Timer->CR1&=~TIM_CR1_CEN;
Timer->PSC=PSC;
Timer->ARR=ARR;
Timer->CNT=0;
Timer->CR1|=TIM_CR1_CEN;
}




static void (*Ptr_TIM2)(void);
static void (*Ptr_TIM21)(void);
static void (*Ptr_TIM22)(void);

void Timer_IT_Enable( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void))
{
	//  activation horloge periph :
	Timer_CkEnable(Timer);


// position (num�ro d'interruption de NVIC)
// Timer 2,3,4 : 28,29,30
// Les priorit�s tiennent sur 4 bits. C'est jeu de registres IPRx qui permet de r�gler
// la prio. Chaque canal a un champ de 8 bits, mais seuls les 4 bits de pds fort
// sont consid�rer
if (Prio > 15)	Prio=15; // saturation prio par pr�caution
// un registre IPR = 4 canaux.
// Les timers 2,3,4 sont donc ds le registre 28/4 = 7, soit IPR[7]

if (Timer==TIM2)
	{
	Ptr_TIM2=IT_function;
	NVIC_SetPriority(TIM2_IRQn, Prio);
	NVIC_EnableIRQ(TIM2_IRQn);
	}
if (Timer==TIM21)
	{
	Ptr_TIM21=IT_function;
	NVIC_SetPriority(TIM21_IRQn, Prio);
	NVIC_EnableIRQ(TIM21_IRQn);
	}
if (Timer==TIM22)
	{
	Ptr_TIM22=IT_function;
	NVIC_SetPriority(TIM22_IRQn, Prio);
	NVIC_EnableIRQ(TIM22_IRQn);
	}
	// validation locale de l'interruption, registre EGR , update event (over/underflow)
	Timer->DIER=(Timer->DIER)|(1<<0);
}



//______________________________________________________________________________________
//______________________________________________________________________________________
void TIM2_IRQHandler (void)
{
	TIM2->SR &=~(0x1);
	Ptr_TIM2();
}
//______________________________________________________________________________________
//______________________________________________________________________________________
void TIM21_IRQHandler (void)
{
	TIM21->SR &=~(0x1);
	Ptr_TIM21();
}

//______________________________________________________________________________________
//______________________________________________________________________________________
void TIM22_IRQHandler (void)
{
	
	TIM22->SR &=~(0x1);
	Ptr_TIM22();

}


