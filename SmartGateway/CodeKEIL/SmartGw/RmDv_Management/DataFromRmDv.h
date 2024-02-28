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
	/*réception*/
	float Temperature;
	char LastTempSet;
	RmDv_WarningCode Status;
	
	/*émission*/
	char NewTempSet;
	unsigned short intNextTimeInterval_sec; /* 18h max */
	
	/* variable d'état */
	char ID;
	TimeStampTypedef RmDvTimeStamp;
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
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,char lastSet,char newSet, \
										RmDv_WarningCode status,unsigned short int NextTimeInterval_sec);
#endif 
