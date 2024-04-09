/* =================================================================================
* ==================   DataFromRmDv.c	     =================================
 *
 *   Created on: 22/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/



#include "DataFromRmDv.h"
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"



/* ===============================================
          la structure d'échange MACPhyUART 
================================================== */

/*---------------------------------
 DEFINITIONS DES VARIABLES RMDVDATA
----------------------------------*/
RmDvDataTypedef RmDvData_Salon, RmDvData_SaManger,\
								RmDvData_Entree, RmDvData_Couloir, RmDvData_Ext;



/*---------------------------------
 FONCTIONS PUBLIQUES
----------------------------------*/

/**
  * @brief  Récupère les adresses de structure data des 5 RmDv
  * @Note
  * @param  ID : l'ID de la clim
  * @retval  RmDvDataTypedef* l'adresse de l'objet data correpondant à l'ID de la clim
  **/
RmDvDataTypedef* RmDvData_GetObjectAdress( int ID)
{
	RmDvDataTypedef* Adr;
	if (ID == ID_Clim_Salon) Adr = &RmDvData_Salon;
	else if (ID == ID_Clim_SaManger) Adr = &RmDvData_SaManger;
	else if (ID == ID_Clim_Entree) Adr = &RmDvData_Entree;
	else if (ID == ID_Clim_Couloir) Adr = &RmDvData_Couloir;	
	else if (ID == ID_Ext) Adr = &RmDvData_Ext;
	else while(1); ///// plantage
	return Adr;
}



/**
  * @brief  Met à 0 les variables de chaque objet. Fixe son ID ,
						fixe le set temp sur stop , et le status sur nowarning
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à 0, 
						ID : l'ID de la clim
  * @retval 
  **/
void RmDvData_Reset(RmDvDataTypedef* RmDvData, char ID)
{
	/*réception*/
	RmDvData->Temperature = 0.0;
	RmDvData->LastTempSet=0;
	RmDvData->Status = Status_NoWarning;
	RmDvData->TransactionIdx=0;
	/*émission*/
	RmDvData->LastTempSet=0;
	RmDvData->NextTimeInterval_sec=0;
	RmDvData->NextTransactionIdx=0;
	/* variable d'état */
	RmDvData->ID = ID;
	RmDvData->RmDvTimeStamp.Sec=0;
	RmDvData->RmDvTimeStamp.Min=0;
	RmDvData->RmDvTimeStamp.Hour=0;
	RmDvData->RmDvTimeStamp.Day=0;
	RmDvData->RmDvTimeStamp.Month=0;
	RmDvData->RmDvTimeStamp.Year=0;
	RmDvData->ReadyToRead=0;		
}


/**
  * @brief  Met à jour le NowStamp
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_StampReceivedData(RmDvDataTypedef* RmDvData)
{
	TimeStampTypedef * TimePtr;
	TimePtr = TimeStamp_GetClock();

	RmDvData->RmDvTimeStamp.Sec = TimePtr->Sec;
	RmDvData->RmDvTimeStamp.Min = TimePtr->Min;
	RmDvData->RmDvTimeStamp.Hour = TimePtr->Hour;
	RmDvData->RmDvTimeStamp.Day = TimePtr->Day;
	RmDvData->RmDvTimeStamp.Month = TimePtr->Month;
	RmDvData->RmDvTimeStamp.Year = TimePtr->Year;
	
}


/**
  * @brief  Met à jour les données du RmDv spécifié à partir d'un nouveau set de réception
	* @Note   chps concernés :  Température / LastTempSet / Status 
						Le stamp est fait par la fonction RmDvData_StampReceivedData
						Le flag readyToRead passe à 1.
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 
						
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,char lastSet,RmDv_WarningCode status, char TransIdx)
/* Mets à jour les données du RmDv en question*/
{
	RmDvData->Temperature = Temp;
	RmDvData->LastTempSet =  lastSet;
	RmDvData->Status =  status;
	RmDvData->TransactionIdx=TransIdx;
	RmDvData->ReadyToRead = 1;

}
