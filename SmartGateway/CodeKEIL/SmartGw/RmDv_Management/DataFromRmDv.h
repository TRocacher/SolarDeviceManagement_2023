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
#define Fract 26
typedef struct 
{
	/* maximum 48h soit 172800 seconde (18 bits, 32.0 )
	Coeff entre 0.5 et 2 (prenons large 0.25 et 4), on prend 6 bits de garde
	-> Fractionnaire  6.26 (précision 14e-9, exemple sur 30mn, 25µs environ !
	-> K = K*Nb Sec / Nb sec 
	-> multiplication (6.26 * 32.0)/32.0 = 38.26 / 32.0 = 6.26.
	
	-> exemple K = 1.5 = 0x0600 0000, Num = 1800 Den = 1200 
	
	OK DANS LE PRINCIPE VOIR MAIN
	par contre comment éviter lesexplosin de bornes à la division ?
	
	nveau chp fract
		int RTCAdjFactor;					 facteur de correction entre 0.5 et 2.0 en 22.10		
	
	MAIN

long long resu;
int k = 1<<Fract;
	resu=((long long)k*86400);
	resu=resu/(long long)(80000);
	*/
	
	int NextDesiredWkupDelay_sec;			/* Intervalle de temps voulu prochain wup*/
	int NextCorrWkupDelay_sec;	/* Intervalle de temps corrigé prochain wup : celui réllement envoyé*/
	int LastDesiredWkupDelay_sec;			/* Intervalle de temps voulu lors du précédent échange*/
	int LastRealWupDelay_sec;	/* Intervalle de temps effectivement mesuré correspondant
																		à la dernière transaction */
	float RTCAdjFactor;					/* facteur de correction entre 0.5 et 2.0 */															

	TimeStampTypedef LastRmDvTimeStamp;		/* permettra de mesurer la dernière durée demandée */
	TimeStampTypedef NowRmDvTimeStamp;    /* Time stamp à la réception */
	
}Delay_Typedef;	

typedef struct 
{
	/*réception*/
	float Temperature;
	char LastTempSet;
	RmDv_WarningCode Status;
	
	/*émission*/
	char NewTempSet;
	Delay_Typedef Delay;
	
	/* variable d'état */
	char ID;
	char ReadyToRead;
	
}RmDvDataTypedef;				



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
  * @brief  Met à jour le NowStamp
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_StampReceivedData(RmDvDataTypedef* RmDvData);
	
	
/**
  * @brief  Sauvegarde de la date actuelle pour le prochain run
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_BackUpStamp(RmDvDataTypedef* RmDvData);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,char lastSet,char newSet, \
										RmDv_WarningCode status,unsigned short int NextTimeInterval_sec);
#endif 
