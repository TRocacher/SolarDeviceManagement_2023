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
#include "RmDv_SGw_Protocol.h" 
#include "TimeStampManagement.h"

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Status_LastTempSet( RmDv_WarningCode Status,char LastTempSet);


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
void InfoLCD_PrintHMIHour(TimeStampTypedef* TimeStampIHM);


#endif 
