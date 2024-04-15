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


/* Proto fcts privées*/
void RmDvData_CalculateStampTarget(TimeStampTypedef* PtrA, TimeStampTypedef* TargetStamp );
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
	RmDvData->Status = Status_NoWarning;
	RmDvData->Temperature = 0.0;
	
	/* Pour RTC Adjust*/
	TimeStamp_ResetStamp(&RmDvData->Delay.StampNow);
	TimeStamp_ResetStamp(&RmDvData->Delay.StampPrevious);
	TimeStamp_ResetStamp(&RmDvData->Delay.StampNextTarget);
	
	RmDvData->Delay.TimeExpansionFactor=1.0;
	
	RmDvData->Delay.TimeIntervalCorNow=0;
	RmDvData->Delay.TimeIntervalCorPrevious = 0;
	RmDvData->Delay.TimeIntervalMeasPrevious = 0;	
	RmDvData->Delay.TimeIntervalTheoNow =0;
				
}


/**
  * @brief  Met à jour le NowStamp
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 

  * @retval none
  **/
void RmDvData_StampReceivedData(RmDvDataTypedef* RmDvData)
{
	/* Stamp Data*/
	TimeStamp_GetClock(&RmDvData->Delay.StampNow);	
}




/**
  * @brief  Met à jour les données du RmDv spécifié à partir d'un nouveau set de réception
  * @Note
  * @param  RmDvData : pointeur sur la structure à mettre à jour, 
						NewReceivedData : pointeur sur les données fraîches
  * @retval 
  **/
void RmDvData_Update(RmDvDataTypedef* RmDvData, float Temp,char lastSet,char newSet, \
										RmDv_WarningCode status, RmDv_WkUp_CurrentState PrevState)
/* Mets à jour les données du RmDv en question*/
{
	RmDvData->NewTempSet = newSet;
	RmDvData->Temperature = Temp;
	RmDvData->LastTempSet =  lastSet;
	RmDvData->Status =  status;
	RmDvData->PrevState = PrevState;
	RmDvData->ReadyToRead = 1;

}




/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/

// fct bricolée
void TestRmDvData_CalculateStampTarget(TimeStampTypedef* PtrA, TimeStampTypedef* TargetStamp );

int RmDvData_GenerateNextTimeInterval(RmDvDataTypedef* RmDvData)
{
	TimeStampTypedef* PtrNow;	
	TimeStampTypedef* PtrPrev;
	TimeStampTypedef* PtrTarget;
	int CorrInterval;
	float Factor;
	
	PtrNow = &RmDvData->Delay.StampNow;
	PtrPrev= &RmDvData->Delay.StampPrevious;
	PtrTarget =  &RmDvData->Delay.StampNextTarget;
	
	/*	Stamp Data  */
	RmDvData_StampReceivedData(RmDvData);
	
	/* mesure intervalle reel */
	RmDvData->Delay.TimeIntervalMeasPrevious = TimeStamp_substract(PtrNow,PtrPrev); 
	
	/* calcul de la prochaine cible */   
	RmDvData_CalculateStampTarget(PtrNow,PtrTarget);
	//TestRmDvData_CalculateStampTarget(PtrNow,PtrTarget);
	
	/* calcul Time Interval Théorique */   
	RmDvData->Delay.TimeIntervalTheoNow = TimeStamp_substract(PtrTarget,PtrNow);
	
	/* calcul du facteur de dilatation temps au niveau du RmDv*/
	if (RmDvData->Delay.TimeIntervalCorPrevious !=0)
	{
		Factor=(float)(RmDvData->Delay.TimeIntervalMeasPrevious)/(float)(RmDvData->Delay.TimeIntervalCorPrevious);
	}
	else Factor=1.0;
	
	if (Factor>1.2) Factor = 1.2;
	if (Factor<0.8) Factor = 0.8;
	
	RmDvData->Delay.TimeExpansionFactor=Factor;
	
	/* CorrInterval : Ajustement de la durée décidées*/
	CorrInterval= (int)((float)RmDvData->Delay.TimeIntervalTheoNow/RmDvData->Delay.TimeExpansionFactor); 
			
	/* Stamp Previous <- Now*/
	RmDvData->Delay.StampPrevious.Sec = RmDvData->Delay.StampNow.Sec;
	RmDvData->Delay.StampPrevious.Min = RmDvData->Delay.StampNow.Min;
	RmDvData->Delay.StampPrevious.Hour = RmDvData->Delay.StampNow.Hour;
	RmDvData->Delay.StampPrevious.Day = RmDvData->Delay.StampNow.Day;
	RmDvData->Delay.StampPrevious.Month = RmDvData->Delay.StampNow.Month;
	RmDvData->Delay.StampPrevious.Year = RmDvData->Delay.StampNow.Year;
	
	/* Corrected Time Interval  Previous <- Now*/
	RmDvData->Delay.TimeIntervalCorNow=CorrInterval;
	RmDvData->Delay.TimeIntervalCorPrevious = CorrInterval; /* interval previous <- now */
	
		
	return CorrInterval;
}



/*---------------------------------
 FONCTIONS PRIVEES
----------------------------------*/

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/


void RmDvData_CalculateStampTarget(TimeStampTypedef* PtrA, TimeStampTypedef* TargetStamp )
{

	int Min, Hour;
	TimeStampTypedef LocalStamp;
	
	Min = PtrA->Min;
	Hour = PtrA->Hour;
	LocalStamp.Day=PtrA->Day;
	LocalStamp.Month=PtrA->Month;
	LocalStamp.Year=PtrA->Year;
	
	if ((Hour>=22) || (Hour<5))  // exemple 22h01 -> 6h00, 4h20->6h00 , 4h59 -> 6h00
																// 5h05 -> 5h30 // 6h01 -> 6h30.
	{
			TargetStamp->Hour=6;
			TargetStamp->Min =0;
			TimeStamp_DayInc(&LocalStamp); /* LocalStamp += 1 day*/
			/* Achèvement remplissage*/
			TargetStamp->Sec=0;
			TargetStamp->Day=LocalStamp.Day;
			TargetStamp->Month=LocalStamp.Month;
			TargetStamp->Year=LocalStamp.Year;
		
	}
	else
	{
		if ((Min>15)&&(Min<=45))	TargetStamp->Min = 0;		
		else TargetStamp->Min = 30;
		
		if (Min>15) TargetStamp->Hour = Hour+1;
		else TargetStamp->Hour = Hour;
		
		/* Achèvement remplissage*/
		TargetStamp->Sec=0;
		TargetStamp->Day=PtrA->Day;
		TargetStamp->Month=PtrA->Month;
		TargetStamp->Year=PtrA->Year;
		
	}

}


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void TestRmDvData_CalculateStampTarget(TimeStampTypedef* PtrA, TimeStampTypedef* TargetStamp )
{

	HourStampTypedef StampH_A,StampH_B;
		
	StampH_A.Hour=PtrA->Hour;
	StampH_A.Min=PtrA->Min;
	StampH_A.Sec=PtrA->Sec;
	
	StampH_B.Hour=0;
	StampH_B.Min=0;
	StampH_B.Sec=60;
	HourStampAdd(&StampH_A,&StampH_B );

	/* Achèvement remplissage*/
	
	TargetStamp->Sec=StampH_A.Sec;
	TargetStamp->Min=StampH_A.Min;
	TargetStamp->Hour=StampH_A.Hour;
	
	TargetStamp->Day=PtrA->Day;
	TargetStamp->Month=PtrA->Month;
	TargetStamp->Year=PtrA->Year;
}
