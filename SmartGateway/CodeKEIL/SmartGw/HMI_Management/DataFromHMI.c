/* =================================================================================
* ==================   DataFromHMI.c	     =================================
 *
 *   Created on: 5/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/



#include "DataFromHMI.h"



/* ===============================================
          les structures de données DataFromHMI
================================================== */



/* ---- La variable principale :  ---- */
struct 
{
	TimeStampTypedef LastHMITimeStamp;  				/* Total 12 bytes*/
	DFH_HMIMode Mode; 													/* Total 4 bytes*/
	DFH_AutoModeDataTypedef Auto;								/* Total 20 bytes*/
	DFH_ProgramModeDataTypedef Program;					/* Total 24 bytes*/
	DFH_HollidaysModeDataTypedef Hollidays;			/* Total 16 bytes*/
	DFH_PowDataOptTypedef OptPowData;						/* Total 16 bytes*/
																				/* TOTAL 92 bytes*/

}DFH_CentralData;														





void DFH_Init_CentralData(void)
{
	int i;
	
	/*TimeStampTypedef LastHMITimeStamp = 1 Jan 2024, 12h00 00sec*/
	DFH_CentralData.LastHMITimeStamp.Sec=0;
	DFH_CentralData.LastHMITimeStamp.Min=0;
	DFH_CentralData.LastHMITimeStamp.Hour=12;
	DFH_CentralData.LastHMITimeStamp.Day=1;
	DFH_CentralData.LastHMITimeStamp.Month=1;
	DFH_CentralData.LastHMITimeStamp.Year=2024;
	
	DFH_CentralData.Mode=HMI_Mode_Program;
	
	DFH_CentralData.Auto.TempMinExt=10.0;
	DFH_CentralData.Auto.PowExcessStart=500.0;
	DFH_CentralData.Auto.PowExcessStop=200.0;
	DFH_CentralData.Auto.ClimPrio[0]=4;
	DFH_CentralData.Auto.ClimPrio[1]=3;
	DFH_CentralData.Auto.ClimPrio[2]=2;
	DFH_CentralData.Auto.ClimPrio[3]=1;
	DFH_CentralData.Auto.TempMinHC=18;
	DFH_CentralData.Auto.TempMinHP=18;
	
	/* All temp = 17*/
	for (i=0;i<3;i++)
	{
		DFH_CentralData.Program.Temperature_6h[i]=0;
		DFH_CentralData.Program.Temperature_8h[i]=0;
		DFH_CentralData.Program.Temperature_10h[i]=0;
		DFH_CentralData.Program.Temperature_15h[i]=0;
		DFH_CentralData.Program.Temperature_18h[i]=0;
		DFH_CentralData.Program.Temperature_23h[i]=0;
	}
	
	DFH_CentralData.Hollidays.Mode=HMI_Mode_Program;
	DFH_CentralData.Hollidays.TempHG_bleu=0;
	DFH_CentralData.Hollidays.TempHG_blanc=0;
	DFH_CentralData.Hollidays.TempHG_rouge=0;
	DFH_CentralData.Hollidays.ArrivalDate.Sec=00;
	DFH_CentralData.Hollidays.ArrivalDate.Min=00;
	DFH_CentralData.Hollidays.ArrivalDate.Hour=12;
	DFH_CentralData.Hollidays.ArrivalDate.Day=2;
	DFH_CentralData.Hollidays.ArrivalDate.Month=1;
	DFH_CentralData.Hollidays.ArrivalDate.Year=2024;
	
	
	DFH_CentralData.OptPowData.PowExcess=1000.0;
	DFH_CentralData.OptPowData.PowL1_Home=100.0;
	DFH_CentralData.OptPowData.PowInverter=1000.0;
	DFH_CentralData.OptPowData.ReapeatBeep=0;
	DFH_CentralData.OptPowData.PrioVE=0;
	DFH_CentralData.OptPowData.CoupureNuit=0;
	DFH_CentralData.OptPowData.CouleurTempo=1;
		
}


float TempMinExt;				/* 32 bits*/
	float PowExcessStart;		/* 32 bits*/
	float PowExcessStop;		/* 32 bits*/
	char ClimPrio[4];				/* 32 bits*/
	char TempMinHC;					/* 8 bits*/
	char TempMinHP;					/* 8 bits*/	




/*====================================================================================
============== Fonctions de lecture des champs de la variable Central Data    ========
======================================================================================*/

/**
  * @brief  Renvoie l'adresse du TimeStamp de la CentralData
  * @Note
  * @param  
  * @retval @ variable
  **/
TimeStampTypedef* DFH_GetCentralData_Stamp(void)
{
	return (&DFH_CentralData.LastHMITimeStamp);
	
}

/**
  * @brief  Renvoie l'adresse du Mode de la CentralData
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_HMIMode* DFH_GetCentralData_Mode(void)
{
	return (&DFH_CentralData.Mode);
	
}

/**
  * @brief  Renvoie les paramètres du mode Auto
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_AutoModeDataTypedef* DFH_GetCentralData_AutoParam(void)
{
	return (&DFH_CentralData.Auto);
	
}


/**
  * @brief  Renvoie les paramètres du mode Programmation
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_ProgramModeDataTypedef* DFH_GetCentralData_ProgParam(void)
{
	return (&DFH_CentralData.Program);
	
}


/**
  * @brief  Renvoie les paramètres du mode Holliday
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_HollidaysModeDataTypedef* DFH_GetCentralData_HollidayParam(void)
{
	return (&DFH_CentralData.Hollidays);
	
}

/**
  * @brief  Renvoie les données d'option et de puissance
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_PowDataOptTypedef* DFH_GetCentralData_OptPowerData(void)
{
	return (&DFH_CentralData.OptPowData);
	
}

/*====================================================================================
============== Fonctions de mise à jour des champs de la variable Central Data    ====
==============  par la HMI (recopie de la châine reçue via UART)                  ====
======================================================================================*/

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_Update_All(char * AdrString, int Long)
{
	char * PtrChar;
	int i;
	
	PtrChar= (char*)&DFH_CentralData; 
	for (i=0;i<Long;i++)
	{
		*PtrChar=*AdrString;
		PtrChar++;
		AdrString++;
	}
}



/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
//void DFH_Update_CentralData(char * AdrString)
//{
//	int ReqCode;
//	ReqCode=(int)DFH_ExtractReqCode(AdrString);
//	
//}




/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
#define OFFSET_AUTO_Stamp 1
#define OFFSET_AUTO_Mode (OFFSET_AUTO_Stamp+6*2)
#define OFFSET_AUTO_TempMinHC (OFFSET_AUTO_Mode+1)
#define OFFSET_AUTO_TempMinHP (OFFSET_AUTO_TempMinHC+1)
#define OFFSET_AUTO_TempMinExt (OFFSET_AUTO_TempMinHP+4)
#define OFFSET_AUTO_PowExcessStart (OFFSET_AUTO_TempMinExt+4)
#define OFFSET_AUTO_PowExcessStop (OFFSET_AUTO_PowExcessStart+4)
#define OFFSET_AUTO_ClimPrio (OFFSET_AUTO_PowExcessStop +4)
#define OFFSET_AUTO_NightCut (OFFSET_AUTO_ClimPrio +4)
#define OFFSET_AUTO_EVChargePrio (OFFSET_AUTO_NightCut +1)
#define OFFSET_AUTO_RepeatSameSet (OFFSET_AUTO_EVChargePrio +1)
#define OFFSET_AUTO_PowExcess (OFFSET_AUTO_RepeatSameSet  +1)
#define OFFSET_AUTO_PowL1_Home (OFFSET_AUTO_PowExcess +4)


void DFH_UpdateModeAuto(char * AdrString)
{
	/*  Champs de données devant être mis à jour :
	struct 
	{
	-> TimeStampTypedef LastHMITimeStamp;
	-> DFH_HMIMode Mode;
	-> DFH_AutoModeDataTypedef Auto;
	DFH_ProgramModeDataTypedef Program;
	DFH_ForcedModeDataTypedef Forced;
	DFH_HollidaysModeDataTypedef Hollidays;
	-> DFH_OptionDataTypedef Option;
	-> DFH_PowDataTypedef PowData;
	
	
}DFH_CentralData;



	Description de la chaîne transmise 
	N = 40 (non exploité)          
	ReqCode = 0xA0 (non exploité)  0
	6 (6*2) short int (time)       1
	1 (1)char (mode)
		auto :
		1 (1)char TempMinHC;
		1 (1)char TempMinHP;
	  1 (4)float TempMinExt;
	  1 (4)float PowExcessStart;
		1 (4)float PowExcessStop;
	  1 (4)char DFH_ClimID ClimPrio[4];
			Options :
			1(1) char NightCut;
			1(1) char EVChargePrio;
			1(1) char RepeatSameSet;
				PowData :
				1(4) float PowExcess;
				1(4)float PowL1_Home;
	Checksum
	
	Total PayLoad : 39 octets. +1 avec le checksum -> N= 40
	
	*/
	
//	int i;
//	char * PtrChar, * PtrParam;
//	TimeStampTypedef * PtrTime;
//	
//	/* Time stamp fill*/
//	PtrTime=&(DFH_CentralData.LastHMITimeStamp);
//	PtrChar=(char *)PtrTime;
//	PtrParam=AdrString+OFFSET_AUTO_Stamp;
//	for (i=0;i<12;i++)
//	{
//		*PtrChar=*PtrParam;
//		PtrChar++;
//		PtrParam++;
//	}
//	/* Mode/TempMinHC/TempMinHP   */	
//	DFH_CentralData.Mode=*(AdrString+OFFSET_AUTO_Mode);
//	DFH_CentralData.Auto.TempMinHC=*(AdrString+OFFSET_AUTO_TempMinHC);
//	DFH_CentralData.Auto.TempMinHP=*(AdrString+OFFSET_AUTO_TempMinHP);
//	
//	/* Time stamp fill*/
//	PtrTime=&(DFH_CentralData.LastHMITimeStamp);
//	PtrChar=(char *)PtrTime;
//	PtrParam=AdrString+OFFSET_AUTO_Stamp;
//	for (i=0;i<12;i++)
//	{
//		*PtrChar=*PtrParam;
//		PtrChar++;
//		PtrParam++;
//	}
	
}

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_UpdateModeProgram(char * AdrString, int Len); 

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_UpdateModeForced(char * AdrString, int Len); 

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_UpdateModeHollidays(char * AdrString, int Len); 

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_UpdateModeOff(void); /* System inactif mais dt les RmDv continuent à transmettre 1 fois par 24h*/
