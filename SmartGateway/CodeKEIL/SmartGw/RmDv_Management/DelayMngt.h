/* =================================================================================
* ==================   DelayMngt.c	     =================================
 *
 *   Created on: 5/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : "GLOBAL_SMARTGATEWAY.h" pour les ID des clim (cste) 
 *  ------------------------------------------------------------------------------
 * 
 * Contient 
 - Transaction_Tab[] contenant chaque structure Transaction
 - ...
	
	
	
* =================================================================================*/
#ifndef DELAYMNGT_H_
#define DELAYMNGT_H_

#include "RmDv_SGw_Protocol.h"
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"


/*---------------------------------
 STRUCTURES DES DONNES
----------------------------------*/
typedef struct 
{
	HourStampTypedef Stamp;
	int StampGap;
	char MssgOnTime;
	RmDv_WarningCode TransactionStatus;
}RmDvInfo_typedef;

typedef struct 
{
	HourStampTypedef DesiredStamp; 
	RmDvInfo_typedef RmDvInfoTab[4];
}ScheduleStep_typedef;		



/**
* @brief  Initialise la table SheduleTab en particulier :
	- fixe les heures souhaitées de 6h00 à 23h00 inclu
	- tous les champs à 0 / noWarning de chaque structure RmDvInfo_typedef
  * @retval le nombre de seconde
  **/
void Init_ScheduleTab(void);

/**
* @brief  Met à jour la grandeur globale interne DelayMngt_RealTimeIdx
* @note : fonction qui est appelée automatiquement toutes les 30m avec un décalage de 15mn
  * @retval none
  **/
void DelayMngt_UpdateRealTimeIdx(void);

#endif 
