/* =================================================================================
* ==================   GPIO_L031.c	     ===================================
 *
 *   Created on: 16/08/23
 *   Author: t.rocacher
 *   Tool : CubeIDE
 *   Target : STM32L031F6P7
 *  ------------------------------------------------------------------------------
 *   Le but est juste de faire une interface entre les fonctions GPIO appelées
 *   dans la pile et les fonctions GPIO LL de la puce
 *
* =================================================================================*/


/* Includes ------------------------------------------------------------------*/

#include "GPIO_L031.h"



//=======================================================================================
// 	Auteur T.R.
//  02 Oct 2010, Modif 18 Jan 2013
//  Processeur : STM32F103RB
//  Logiciel dev : �Vision 4.53
//  Module qui configure le timer syst�me Systick
//=======================================================================================


//========================================================================================
// D�finition des pointeurs de fonction d'IT
//=======================================================================================

static void (*Ptr_EXTI5)(void);
static void (*Ptr_EXTI15)(void);


void ExtPlantage(void)
{
	while(1); // MAUVAISE INIT PTR FCT ! IT ext n°5 ou 15 uniquement !!
	// Ou mauvais flag IT détecté, ce qui serait bien curieux !
}


//========================================================================================
// place la broche du port indiqu�e en entree IT sur front up/down/les deux
//=======================================================================================

void NVIC_Ext_IT (GPIO_TypeDef * Port, char Broche, char Front, char Techno, char Prio, void (*IT_function) (void))
{
char NumPort;
int SYSCFG_EXTI_LINE;
LL_EXTI_InitTypeDef EXTI_InitStruct= {0};




	if (Broche==5)
	{
		Ptr_EXTI5=IT_function;
		SYSCFG_EXTI_LINE=LL_SYSCFG_EXTI_LINE5;
	}
	else if (Broche==15)
	{
		Ptr_EXTI15=IT_function;
		SYSCFG_EXTI_LINE=LL_SYSCFG_EXTI_LINE15;
	}
	else
	{
		Ptr_EXTI5=ExtPlantage;
		Ptr_EXTI15=ExtPlantage;
		SYSCFG_EXTI_LINE=LL_SYSCFG_EXTI_LINE15;
	}
	if (Port==GPIOA) NumPort=0;
	else if (Port==GPIOB) NumPort=1;
	else if (Port==GPIOC) NumPort=2;
	else NumPort=4;

	LL_SYSCFG_SetEXTISource(NumPort, SYSCFG_EXTI_LINE);

	EXTI_InitStruct.Line_0_31 = Broche;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = Front;
	LL_EXTI_Init(&EXTI_InitStruct);
	/* EXTI interrupt init*/
	NVIC_SetPriority(EXTI4_15_IRQn, Prio);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}




//========================================================================================
// Handlers, saut aux fonctions associ�es
//=======================================================================================


void EXTI4_15_IRQHandler (void)
{
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
	{
	    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
	    Ptr_EXTI5();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_15) != RESET)
	{
		   LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
		   Ptr_EXTI15();
	}
	else ExtPlantage();
}


