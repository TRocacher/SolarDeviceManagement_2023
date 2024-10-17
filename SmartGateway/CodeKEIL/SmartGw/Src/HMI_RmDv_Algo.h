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
 *   D�pendance : _
 *   Revision : 
 *  ------------------------------------------------------------------------------
 *  Rassemble les algorithmes permettant de r�pondre aux requ�tes des RmDv, c'est � dire :
 *	Elaborer puis fournir la nouvelle consigne de temp�rature �  partir de la derni�re
 *  mise � jour de la variable HMI_CentralData
* =================================================================================*/


/**
  * @brief  G�n�re la consigne de temp�rature en fonction du mode sp�cifi� dans la CentralData
  * @Note
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_ComputeTempCmde(char ID);


/**
  * @brief  Fonction qui remplit tous les champs
	de la grosse variable HMIRmDvAlgo_AutoData
	notamment les temperatures voulues (TempSet)
	Cette fonction est appel�e normalement par l'IT
	HMI.
  * @Note   
  * @param  _
  * @retval _
  **/
void HMIRmDvAlgo_AutoModeDataUpdateFromHMI(void);




/**
  * @brief  Convertit une temp�rature en son code commande IR correspondant
  * @Note
  * @param  Temp�rature : 18 � 23, 0 si arr�t
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_Temp2CmdeIR(char Temp);

/**
  * @brief  Convertit un code commande IR en sa temp�rature correspondante
  * @Note   Si la commande est stop, la temp�rature vaut 0
  * @param  Code commande IR
  * @retval  Temp�rature (0 si stop)
  **/
char HMIRmDvAlgo_CmdeIR2Temp(RmDv_TelecoIR_Cmde Cmde);
#endif
