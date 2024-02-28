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
								RmDvData__Entree, RmDvData_Couloir, RmDvData_Ext;



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
	else if (ID == ID_Clim_Entree) Adr = &RmDvData__Entree;
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
	RmDvData->ID = ID;
	RmDvData->intNextTimeInterval_sec = 0;
	RmDvData->LastTempSet=0;
	RmDvData->NewTempSet=0;
	RmDvData->ReadyToRead = 0;
	
	RmDvData->RmDvTimeStamp.Sec = 0;
	RmDvData->RmDvTimeStamp.Min =0;
	RmDvData->RmDvTimeStamp.Hour =0;
	RmDvData->RmDvTimeStamp.Day = 0;
	RmDvData->RmDvTimeStamp.Month = 0;
	RmDvData->RmDvTimeStamp.Year =0;

	RmDvData->Status = NoWarning;
	RmDvData->Temperature = 0.0;
	
	
}

/**
  * @brief  Met à jour les données du RmDv spécifié à partir d'un nouveau set de réception
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 
						NewReceivedData : pointeur sur les données fraîches
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,char lastSet,char newSet, \
										RmDv_WarningCode status,unsigned short int NextTimeInterval_sec)
/* Mets à jour les données du RmDv en question*/
{
	TimeStampTypedef * TimePtr;
	TimePtr = TimeStamp_GetClock();
	RmDvData->NewTempSet = newSet;
	RmDvData->Temperature = Temp;
	RmDvData->LastTempSet =  lastSet;
	RmDvData->Status =  status;
	RmDvData->intNextTimeInterval_sec = NextTimeInterval_sec;
	RmDvData->RmDvTimeStamp.Sec = TimePtr->Sec;
	RmDvData->RmDvTimeStamp.Min = TimePtr->Min;
	RmDvData->RmDvTimeStamp.Hour = TimePtr->Hour;
	RmDvData->RmDvTimeStamp.Day = TimePtr->Day;
	RmDvData->RmDvTimeStamp.Month = TimePtr->Month;
	RmDvData->RmDvTimeStamp.Year = TimePtr->Year;
	RmDvData->ReadyToRead = 1;

}
