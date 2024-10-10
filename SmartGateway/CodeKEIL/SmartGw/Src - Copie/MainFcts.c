/* =================================================================================
* ==================   AlgoClim.c	     =================================
 *
 *   Created on: 06/04/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/



#include "MainFcts.h"

#include "FSKStack.h"
#include "UARTStack.h"
#include "TimeManagement.h"


#include "TimeStampManagement.h"


/**
* @brief  MainFcts_SystemStart
					Lance la pile FSK(RmDv), la pile UART (HMI)
					Lance le système de gestion des timeout
					
* @note :_
* @retval :_
**/
void MainFcts_SystemStart(void)
{
	TimeManag_TimeOutInit(); 	/* obligatoire pour la gestion des TimeOut à tous les étages...*/
	FSKStack_Init(My_);				/* init de la stack wireless*/
	UARTStack_Init();  				/* init de la stack UART pour HMI*/
}


/**
* @brief  MainFcts_SetDefaultTime
					Mets à l'heure le système arbitraire 
* @note :_
* @retval :_
**/


void MainFcts_SetArbitraryTime(void)
{

	
	TimeStampTypedef LocalStamp;
	LocalStamp.Year=2024;
	LocalStamp.Month=1;
	LocalStamp.Day=1;
	LocalStamp.Hour=12;
	LocalStamp.Min=00;	
	LocalStamp.Sec=0;
	
	TimeStamp_SetClock(&LocalStamp);
	/* Flag d'attente HMI pour mise à l'heure à 0*/
	TimeStamp_ClearClockUpdated_Flag();
	TimeStamp_SetTimeStampDeltaStamp(0);
}


