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
	RmDvData->LastTempSet=0;
	RmDvData->NewTempSet=0;
	RmDvData->ReadyToRead = 0;
	
	/* Pour RTC Adjust*/
	RmDvData->Delay.NextDesiredWkupDelay_sec = 0;
	RmDvData->Delay.NextCorrWkupDelay_sec = 0;
	RmDvData->Delay.LastDesiredWkupDelay_sec = 0;
	RmDvData->Delay.RTCAdjFactor = 1.0;
	RmDvData->Delay.NowRmDvTimeStamp.Sec = 0;
	RmDvData->Delay.NowRmDvTimeStamp.Min =0;
	RmDvData->Delay.NowRmDvTimeStamp.Hour =0;
	RmDvData->Delay.NowRmDvTimeStamp.Day = 0;
	RmDvData->Delay.NowRmDvTimeStamp.Month = 0;
	RmDvData->Delay.NowRmDvTimeStamp.Year =0;
	RmDvData->Delay.LastRmDvTimeStamp.Sec = 0;
	RmDvData->Delay.LastRmDvTimeStamp.Min =0;
	RmDvData->Delay.LastRmDvTimeStamp.Hour =0;
	RmDvData->Delay.LastRmDvTimeStamp.Day = 0;
	RmDvData->Delay.LastRmDvTimeStamp.Month = 0;
	RmDvData->Delay.LastRmDvTimeStamp.Year =0;
	
	RmDvData->Status = Status_NoWarning;
	RmDvData->Temperature = 0.0;
	

		
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

	RmDvData->Delay.NowRmDvTimeStamp.Sec = TimePtr->Sec;
	RmDvData->Delay.NowRmDvTimeStamp.Min = TimePtr->Min;
	RmDvData->Delay.NowRmDvTimeStamp.Hour = TimePtr->Hour;
	RmDvData->Delay.NowRmDvTimeStamp.Day = TimePtr->Day;
	RmDvData->Delay.NowRmDvTimeStamp.Month = TimePtr->Month;
	RmDvData->Delay.NowRmDvTimeStamp.Year = TimePtr->Year;
	
	
	
	/* 
	RmDvData->Delay.LastRealWupDelay_sec = \
	TimeStamp_substract(&RmDvData->Delay.NowRmDvTimeStamp,&RmDvData->Delay.LastRmDvTimeStamp);
	
	RmDvData->Delay.LastRmDvTimeStamp.Sec = TimePtr->Sec;;
	RmDvData->Delay.LastRmDvTimeStamp.Min = TimePtr->Min;
	RmDvData->Delay.LastRmDvTimeStamp.Hour = TimePtr->Hour;
	RmDvData->Delay.LastRmDvTimeStamp.Day = TimePtr->Day;
	RmDvData->Delay.LastRmDvTimeStamp.Month = TimePtr->Month;
	RmDvData->Delay.LastRmDvTimeStamp.Year = TimePtr->Year;
	*/
	
}

/**
  * @brief  Sauvegarde de la date actuelle pour le prochain run
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_BackUpStamp(RmDvDataTypedef* RmDvData)
{
	RmDvData->Delay.LastRmDvTimeStamp.Sec = RmDvData->Delay.NowRmDvTimeStamp.Sec;
	RmDvData->Delay.LastRmDvTimeStamp.Min = RmDvData->Delay.NowRmDvTimeStamp.Min;
	RmDvData->Delay.LastRmDvTimeStamp.Hour = RmDvData->Delay.NowRmDvTimeStamp.Hour;
	RmDvData->Delay.LastRmDvTimeStamp.Day = RmDvData->Delay.NowRmDvTimeStamp.Day;
	RmDvData->Delay.LastRmDvTimeStamp.Month = RmDvData->Delay.NowRmDvTimeStamp.Month;
	RmDvData->Delay.LastRmDvTimeStamp.Year = RmDvData->Delay.NowRmDvTimeStamp.Year;
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
//	RmDvData->NextTimeInterval_sec = NextTimeInterval_sec;
//	RmDvData->RmDvTimeStamp.Sec = TimePtr->Sec;
//	RmDvData->RmDvTimeStamp.Min = TimePtr->Min;
//	RmDvData->RmDvTimeStamp.Hour = TimePtr->Hour;
//	RmDvData->RmDvTimeStamp.Day = TimePtr->Day;
//	RmDvData->RmDvTimeStamp.Month = TimePtr->Month;
//	RmDvData->RmDvTimeStamp.Year = TimePtr->Year;
	RmDvData->ReadyToRead = 1;

}
