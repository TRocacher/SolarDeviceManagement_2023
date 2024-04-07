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
          la structure d'échange MACPhyUART 
================================================== */

/* ----  types structurés intermédiaires ....  ---- */

/* Mode automatique */
typedef struct 
{
	float TempMinExt;							/* 32 bits*/
	float PowExcessStart;					/* 32 bits*/
	float PowExcessStop;					/* 32 bits*/
	char ClimPrio[4];							/* 32 bits*/
	short int TempMinHC;					/* 16 bits*/
	short int TempMinHP;					/* 16 bits*/	
}DFH_AutoModeDataTypedef; 			/* Total 20 bytes*/

/* Mode program */
typedef struct 
{
	char Temperature_6h[4];					/* Total 24 bytes*/
	char Temperature_8h[4];
	char Temperature_10h[4];	
	char Temperature_15h[4];	
	char Temperature_18h[4];
	char Temperature_23h[4];		
}DFH_ProgramModeDataTypedef;


/* Mode Hollidays */
typedef struct 
{
	char Mode;	/* */
	char TempHG_bleu;
	char TempHG_blanc;
	char TempHG_rouge;
	TimeStampTypedef ArrivalDate;	/* 12 bytes*/	
}DFH_HollidaysModeDataTypedef;  /* Total 16 bytes*/

/* Power data & Option  */
typedef struct 
{
	float PowExcess;							/* 32 bits*/
	float PowL1_Home;							/* 32 bits*/
	float PowInverter;						/* 32 bits*/
	char  ReapeatBeep;						/* 8 bits*/
	char  PrioVE;									/* 8 bits*/
	char  CoupureNuit;						/* 8 bits*/
	char  CouleurTempo;						/* 8 bits*/
}DFH_PowDataOptTypedef;					/* Total 16 bytes*/



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

}DFH_CentralData, test;														





void Test(void)
{
//	int i;
	
	test.LastHMITimeStamp.Sec=01;
	test.LastHMITimeStamp.Min=02;
	test.LastHMITimeStamp.Hour=03;
	test.LastHMITimeStamp.Day=4;
	test.LastHMITimeStamp.Month=5;
	test.LastHMITimeStamp.Year=2024;
	
	test.Mode=HMI_Mode_Auto;
	
	test.Auto.TempMinExt=3.14159;
	test.Auto.PowExcessStart=3.14159;
	test.Auto.PowExcessStop=3.14159;
	test.Auto.ClimPrio[0]=4;
	test.Auto.ClimPrio[1]=3;
	test.Auto.ClimPrio[2]=2;
	test.Auto.ClimPrio[3]=1;
	test.Auto.TempMinHC=01;
	test.Auto.TempMinHP=02;
	
//	for (i=0;i<24;i++)
//	{
//		test.Program.TempPerHour[i]=i;
//	}
	
	//test.Forced.Temp=15;
	
	//test.Hollidays.Temp_ArrRedWhiteBlue=0x00332211;
	test.Hollidays.ArrivalDate.Sec=01;
	test.Hollidays.ArrivalDate.Min=02;
	test.Hollidays.ArrivalDate.Hour=03;
	test.Hollidays.ArrivalDate.Day=4;
	test.Hollidays.ArrivalDate.Month=5;
	test.Hollidays.ArrivalDate.Year=2024;
	
	test.OptPowData.PowExcess=3.14159;
	test.OptPowData.PowL1_Home=3.14159;
	test.OptPowData.PowInverter=3.14159;
//	test.OptPowData.Option[0]=9;
//	test.OptPowData.Option[1]=8;
//	test.OptPowData.Option[2]=7;
//	test.OptPowData.Option[3]=6;
	
	DFH_Update_All((char*)&test, 96);
	
}


float TempMinExt;				/* 32 bits*/
	float PowExcessStart;		/* 32 bits*/
	float PowExcessStop;		/* 32 bits*/
	char ClimPrio[4];				/* 32 bits*/
	char TempMinHC;					/* 8 bits*/
	char TempMinHP;					/* 8 bits*/	


/* ===============================================
          Les fonctions du module...
================================================== */


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
char DFH_ExtractReqCode(char * AdrString)
{
	return (*AdrString);
	
}




/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
TimeStampTypedef* DFH_ReadStampFromCentralData(void)
{
	return (&DFH_CentralData.LastHMITimeStamp);
	
}



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
void DFH_Update_CentralData(char * AdrString)
{
	int ReqCode;
	ReqCode=(int)DFH_ExtractReqCode(AdrString);
	
}




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
	
	int i;
	char * PtrChar, * PtrParam;
	TimeStampTypedef * PtrTime;
	
	/* Time stamp fill*/
	PtrTime=&(DFH_CentralData.LastHMITimeStamp);
	PtrChar=(char *)PtrTime;
	PtrParam=AdrString+OFFSET_AUTO_Stamp;
	for (i=0;i<12;i++)
	{
		*PtrChar=*PtrParam;
		PtrChar++;
		PtrParam++;
	}
	/* Mode/TempMinHC/TempMinHP   */	
	DFH_CentralData.Mode=*(AdrString+OFFSET_AUTO_Mode);
	DFH_CentralData.Auto.TempMinHC=*(AdrString+OFFSET_AUTO_TempMinHC);
	DFH_CentralData.Auto.TempMinHP=*(AdrString+OFFSET_AUTO_TempMinHP);
	
	/* Time stamp fill*/
	PtrTime=&(DFH_CentralData.LastHMITimeStamp);
	PtrChar=(char *)PtrTime;
	PtrParam=AdrString+OFFSET_AUTO_Stamp;
	for (i=0;i<12;i++)
	{
		*PtrChar=*PtrParam;
		PtrChar++;
		PtrParam++;
	}
	
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
