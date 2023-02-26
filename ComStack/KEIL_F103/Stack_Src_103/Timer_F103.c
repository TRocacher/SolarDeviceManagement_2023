#include "stm32f10x.h"
#include "Timer_F103.h"



//**************************************************************************************************************
//**************************************************************************************************************
// 							GESTION TIMEOUT / Systick
//**************************************************************************************************************
//**************************************************************************************************************

int Cpt_100us;

void SystickStart(void)
{
	Cpt_100us=0;
	SysTick->LOAD=100*9-1; // 72MHz/8 = 9Mhz, config à 100µs
  SysTick->CTRL|=1<<1; // Interruption locale validée
	SysTick->CTRL|=1<<0; // systick on
}


int SystickGet(void)
{
	return Cpt_100us;
}


/******************************************************************************************************************
	Interruption 100us

Rôle :
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





void MyTimer_CkEnable(TIM_TypeDef *Timer)
{
	
//  activation horloge periph :
if (Timer==TIM1)  (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM1EN;
if (Timer==TIM2)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM2EN;
if (Timer==TIM3)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM3EN;
if (Timer==TIM4)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM4EN;
}

void MyTimer_Set_Period(TIM_TypeDef *Timer, int ARR, int PSC )
{
Timer->CR1&=~TIM_CR1_CEN;
Timer->PSC=PSC;
Timer->ARR=ARR;
Timer->CNT=0;
Timer->CR1|=TIM_CR1_CEN;
}



static void (*Ptr_TIM1)(void);	
static void (*Ptr_TIM2)(void);
static void (*Ptr_TIM3)(void);
static void (*Ptr_TIM4)(void);

void MyTimer_IT_Enable( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void))
{
//  activation horloge periph :
if (Timer==TIM1)  (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM1EN;
if (Timer==TIM2)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM2EN;
if (Timer==TIM3)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM3EN;
if (Timer==TIM4)  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_TIM4EN;


// position (numéro d'interruption de NVIC)
// Timer 2,3,4 : 28,29,30
// Les priorités tiennent sur 4 bits. C'est jeu de registres IPRx qui permet de régler
// la prio. Chaque canal a un champ de 8 bits, mais seuls les 4 bits de pds fort
// sont considérer
if (Prio > 15)	Prio=15; // saturation prio par précaution
// un registre IPR = 4 canaux.
// Les timers 2,3,4 sont donc ds le registre 28/4 = 7, soit IPR[7]
if (Timer==TIM1)
	{
	Ptr_TIM1=IT_function;
	NVIC->IP[25]=Prio<<4;
	// validation d'IT au niveau NVIC
	NVIC->ISER[0] = (NVIC->ISER[0])|(1<<25);
	}

if (Timer==TIM2)
	{
	Ptr_TIM2=IT_function;
	NVIC->IP[28]=Prio<<4;
	// validation d'IT au niveau NVIC
	NVIC->ISER[0] = (NVIC->ISER[0])|(1<<28);
	}
if (Timer==TIM3)
	{
	Ptr_TIM3=IT_function;
	NVIC->IP[29]=Prio<<4;
	// validation d'IT au niveau NVIC
	NVIC->ISER[0] = (NVIC->ISER[0])|(1<<29);
	}
if (Timer==TIM4)
	{
	Ptr_TIM4=IT_function;
	NVIC->IP[30]=Prio<<4;
	// validation d'IT au niveau NVIC
	NVIC->ISER[0] = (NVIC->ISER[0])|(1<<30);
	}
	// validation locale de l'interruption, registre EGR , update event (over/underflow)
	Timer->DIER=(Timer->DIER)|(1<<0);
}


void TIM1_UP_IRQHandler (void)
{
TIM1->SR &=~(0x1);
Ptr_TIM1();
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
void TIM3_IRQHandler (void)
{
	TIM3->SR &=~(0x1);
	Ptr_TIM3();
}

//______________________________________________________________________________________
//______________________________________________________________________________________
void TIM4_IRQHandler (void)
{
	
	TIM4->SR &=~(0x1);
	Ptr_TIM4();

}


