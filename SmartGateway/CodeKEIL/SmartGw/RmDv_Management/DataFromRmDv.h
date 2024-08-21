/* =================================================================================
* ==================   DataFromHMI.c	     =================================
 *
 *   Created on: 5/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : "GLOBAL_SMARTGATEWAY.h" pour les ID des clim (cste) 
 *  ------------------------------------------------------------------------------
 * Contient les 5 "objets" RmDev

* La logique est la suivante : 
	MyPtrOnSalon = RmDvData_GetObjectAdress(ID_Clim_Salon);
	RmDvData_Reset(MyPtrOnSalon, ID_Clim_Salon);
	
	Ces deux lignes permettent 
	- de récupérer l'@ de la structure de données du Salon à partir de son ID
	- d'initialiser la structure de donnée encapsulée dans le présent module.
	
	
	
* =================================================================================*/
#ifndef DATAFROMRMDV_H_
#define DATAFROMRMDV_H_

#include "RmDv_SGw_Protocol.h"
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"


/*---------------------------------
 STRUCTURES DES DONNES
----------------------------------*/

typedef struct 
{
	TimeStampTypedef StampNow;				/* Stamp à la réception */
	TimeStampTypedef StampPrevious;   /* Stamp à la réception précedente */
	TimeStampTypedef StampNextTarget; /* Stamp cible prochaine transaction */
	
	int TimeIntervalTheoNow;					/* Intervalle théorique d'ici prochaine transaction */	
	int TimeIntervalCorNow;						/* Intervalle corrigé d'ici prochaine transaction */	
	int TimeIntervalCorPrevious;			/* Intervalle corrigé précédent*/	
	int TimeIntervalMeasPrevious;							/* Intervalle de temps précédent mesuré*/	
	
	float TimeExpansionFactor;					/* facteur de correction entre 0.8 et 1.2 */															

}Delay_Typedef;	

typedef struct 
{
	/*réception*/
	float Temperature;
	RmDv_TelecoIR_Cmde LastTempSet;
	RmDv_WarningCode Status;
	RmDv_WkUp_CurrentState PrevState;
	
	/*émission*/
	RmDv_TelecoIR_Cmde NewTempSet;
	Delay_Typedef Delay;
	
	/* variable d'état */
	char ID;
	char ReadyToRead;
	
}RmDvDataTypedef;				


#define RmDvData_OffsetSec 5 /* Décalage stamp pour chaque RmDv pour éviter congestion traffic 
																lors des réveils. Donné en seconde. */


/*---------------------------------
 FONCTIONS PUBLIQUES
----------------------------------*/

/**
  * @brief  Récupère les adresses de structure data des 5 RmDv
  * @Note
  * @param  ID : l'ID de la clim
  * @retval  RmDvDataTypedef* l'adresse de l'objet data correpondant à l'ID de la clim
  **/
RmDvDataTypedef* RmDvData_GetObjectAdress( int ID);



/**
  * @brief  Met à 0 les variables de chaque objet. Fixe son ID ,
						fixe le set temp sur stop , et le status sur nowarning
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à 0, 
						ID : l'ID de la clim
  * @retval 
  **/
void RmDvData_Reset(RmDvDataTypedef* RmDvData, char ID);



/**
  * @brief  Met à jour le champ StampNow du RmDvData spécifié
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_StampReceivedData(RmDvDataTypedef* RmDvData);
	



/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,RmDv_TelecoIR_Cmde lastSet,RmDv_TelecoIR_Cmde newSet, \
										RmDv_WarningCode status, RmDv_WkUp_CurrentState PrevState);


/**
  * @brief  Dure 27µs (test simu) calcule le prochain déali à partir du délai actuel (supposé inscrit dans RmDvData
  * @Note		l'ensemble des chps délai est rempli, le RmDvData est à jour du pt de vue du chp délai
  * @param  
  * @retval la valeur de l'intervalle en seconde, corrigé.
  **/
int RmDvData_GenerateNextTimeInterval(RmDvDataTypedef* RmDvData);


/**
  * @brief  calcule de manière brute la prochaine date cible à partir de Now.
  * @Note   ajoute 5seconde pour le RmDv n°1, 10sec pour ne n°2 etc... pour éviter
						les collisions.
  * @param  l'ID du RmDv, l'@ du Stamp A qui est la date à partir de laquelle le calcul est fait
  * @retval  l @ du stamp cible
  **/
void RmDvData_CalculateStampTarget(int ID,TimeStampTypedef* PtrA, TimeStampTypedef* TargetStamp );

#endif 
