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

//#include "RmDv_SGw_Protocol.h"
#include "TimeStampManagement.h"
#include "DataFromRmDv.h"
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
	RmDvInfo_typedef RmDvInfoTab[5];
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
* @note : fonction qui est appelée automatiquement toutes les 30mn avec un décalage de 15mn
					elle doit être appelée après une mise à l'heure afin d'avoir une synchronisation entre
					la variable temps réel RealTimeIdx et l'horloge (voir TimeManagement.h /.c) 
  * @retval none
  **/
void DelayMngt_UpdateRealTimeIdx(void);


/**
* @brief  Met à jour la le prochain délai pour un RmDv donné. 
* @note : 
  * @retval renvoi le délai corrigé en seconde (délai normal avec ajout du gap).
	Exemple 35mn car prochain intervalle 30mn et avance de 5mn.
	Si 0xFF c'est qu'une erreur s'est produite (ID inconnue)
  **/
int DelayMngt_CalculateNextDelay(char TransactionIdx, RmDvDataTypedef* RmDvData);


/**
* @brief  Détermine l'index de transaction suivant pour émission vers RmDv
* @note : l'index futur est calculé à partir de l'index temps réel et non à partir
					de l'index extrait de la requête. Cela permet de recaler l'ensemble si
					il existe un mismatch entre la réalité et l'index arrivant
  * @retval renvoi l'index futur
  **/
char DelayMngt_CalculateNextTransactionIdx(char TransactionIdx);
#endif 
