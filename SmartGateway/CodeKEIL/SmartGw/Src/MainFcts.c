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

#include "DelayMngt.h"
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
* @brief  MainFcts_SetTime
					Mets à l'heure le système (pour l'instant arbitraire)
					Lance aussi la progression de l'index tps réel pour pointer les fuseaux hooraires
					Initialise la table des fuseaux horaires
* @note :_
* @retval :_
**/
void MainFcts_SetTime(void)
{
	TimeStampTypedef LocalStamp;
	LocalStamp.Year=2024;
	LocalStamp.Month=4;
	LocalStamp.Day=9;
	LocalStamp.Hour=6;
	LocalStamp.Min=0;	
	LocalStamp.Sec=0;
	
	TimeStamp_SetClock(&LocalStamp);
	DelayMngt_UpdateRealTimeIdx();
	/* Remplissage de la table des fuseaux horaires */
	Init_ScheduleTab();
}
