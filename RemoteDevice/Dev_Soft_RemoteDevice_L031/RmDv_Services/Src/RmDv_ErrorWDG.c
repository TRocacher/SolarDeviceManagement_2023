/*
 * RmDv_ErrorWatchDog.c
 *
 *  Created on: 28 août 2023
 *      Author: trocache
 */


#include "RmDv_ErrorWDG.h"

static void (*Ptr_LPTIM1)(void);

void RmDv_ErrorWDG_LPTIMConf(int TimeOut_sec,char Prio, void (*IT_function) (void))
{
	RCC->CCIPR|=1<<RCC_CCIPR_LPTIM1SEL_Pos;  /* LSI sélectionné, donc 37kHz*/
	RCC->APB1ENR|=RCC_APB1ENR_LPTIM1EN;
	Ptr_LPTIM1=IT_function;
	LPTIM1->IER|=LPTIM_IER_ARRMIE; /* validation locale IT*/
	LPTIM1->CFGR|=0x7<<LPTIM_CFGR_PRESC_Pos; /* Prescaler 128*/

	LPTIM1->CR|=LPTIM_CR_ENABLE;   /* Validation Timer */
	LPTIM1->ARR=(TimeOut_sec*37000/128-1);  /* ARR = TimeOut_sec / periode Ck = TimeOut_sec * Freq Ck  */

	/* Validation IT NVIC*/
	NVIC_SetPriority(LPTIM1_IRQn, Prio);
	NVIC_EnableIRQ(LPTIM1_IRQn);
}


void LPTIM1_IRQHandler(void)
{
	LPTIM1->ICR|=LPTIM_ICR_ARRMCF;
	Ptr_LPTIM1();
}
