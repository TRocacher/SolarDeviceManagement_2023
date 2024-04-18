/* =================================================================================
* ==================   InfoLCD.c	     =================================
 *
 *   Created on: 07/04/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/
#ifndef INFOLCD_H_
#define INFOLCD_H_

#include "MyLCD.h"
#include "DataFromRmDv.h"
#include "RmDv_SGw_Protocol.h" 
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"



/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Init(void);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Status_LastTempSet( RmDv_WarningCode Status,char LastTempSet);




/**
* @brief  : Affichage des 5 températures
						Récupère les 5 températures via les 5 variables RmDv_Data
						du module DataFromRmDv.c
* @Note : Format :
					XX.X XX.X XX.X
					XX.X XX.X
  * @param  
  * @retval 
  **/
void InfoLCD_Print5Temp(void);


/**
* @brief  : Affichage du Stamp ID

* @Note : Format :
					"Salon Stamp"
					hh::mn:ss
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRmDv_Stamp(int ID);

/**
* @brief  : Affichage du Stamp ID, seconde partie (Status, Factor, NewSet)

* @Note : Format :
					"xxxxx" (statut)
					x.xx ; Set: yy
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRmDv_StatFactNewSet(int ID);
	



/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintHour(char * PtrLineUpStr,TimeStampTypedef* TimeStamp);






/*----------------------------------------------
 Gestion des Tamp String pour maintenance/debug
----------------------------------------------*/

/**
  * @brief 
  * @Note
  * @param  
  * @retval 
  **/

void InfoLCD_MemStampStrInit(void);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
int InfoLCD_AddTimeStampToMem(TimeStampTypedef* TimeStamp, char ID);

#endif 
