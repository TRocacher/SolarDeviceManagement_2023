#ifndef _HMI_RMDV_ALGO_H__
#define _HMI_RMDV_ALGO_H__

#include "GLOBAL_SMARTGATEWAY.h"


/* =================================================================================
* ==================    HMI_RmDv_Algo.h	     =================================
 *
 *   Created on: 19/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *   Revision : 
 *  ------------------------------------------------------------------------------
 *  Rassemble les algorithmes permettant de répondre aux requêtes des RmDv, c'est à dire :
 *	Elaborer puis fournir la nouvelle consigne de température à  partir de la dernière
 *  mise à jour de la variable HMI_CentralData
* =================================================================================*/


/**
  * @brief  Génère la consigne de température en fonction du mode spécifié dans la CentralData
  * @Note
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_ComputeTempCmde(char ID);


/**
  * @brief  Fonction qui remplit tous les champs
	de la grosse variable HMIRmDvAlgo_AutoData
	notamment les temperatures voulues (TempSet)
	Cette fonction est appelée normalement par l'IT
	HMI.
  * @Note   
  * @param  _
  * @retval _
  **/
void HMIRmDvAlgo_AutoModeDataUpdateFromHMI(void);




/**
  * @brief  Convertit une température en son code commande IR correspondant
  * @Note
  * @param  Température : 18 à 23, 0 si arrêt
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_Temp2CmdeIR(char Temp);

/**
  * @brief  Convertit un code commande IR en sa température correspondante
  * @Note   Si la commande est stop, la température vaut 0
  * @param  Code commande IR
  * @retval  Température (0 si stop)
  **/
char HMIRmDvAlgo_CmdeIR2Temp(RmDv_TelecoIR_Cmde Cmde);
#endif
