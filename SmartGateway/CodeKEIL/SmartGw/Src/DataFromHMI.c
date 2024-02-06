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
#include "TimeStampManagement.h"




/* ===============================================
          la structure d'échange MACPhyUART 
================================================== */

/* ----  types structurés intermédiaires ....  ---- */

/* Mode automatique */
typedef struct 
{
	float TempMinExt;				/* 32 bits*/
	float PowExcessStart;		/* 32 bits*/
	float PowExcessStop;		/* 32 bits*/
	char ClimPrio[4];				/* 32 bits*/
	short int TempMinHC;					/* 16 bits*/
	short int TempMinHP;					/* 16 bits*/	
}DFH_AutoModeDataTypedef; /* Total 20 bytes*/

/* Mode program */
typedef struct 
{
	char TempPerHour[24];							/* Total 24 bytes*/
}DFH_ProgramModeDataTypedef;

/* Mode forcé */
typedef struct 
{
	short int Temp;										/* Total 2 bytes*/	
}DFH_ForcedModeDataTypedef;


/* Mode Hollidays */
typedef struct 
{
	int Temp_ArrRedWhiteBlue;					/* 32 bits (Arrival MinRouge MinBanc MinBleu */
	TimeStampTypedef ArrivalDate;			/* 12 bytes*/	
}DFH_HollidaysModeDataTypedef;  		/* Total 16 bytes*/

/* Power data & Option  */
typedef struct 
{
	float PowExcess;					/* 32 bits*/
	float PowL1_Home;					/* 32 bits*/
	short int  Option;				/* 16 bits*/
}DFH_PowDataOptTypedef;			/* Total 10 bytes*/

/* ---- La variable principale :  ---- */
struct 
{
	DFH_HollidaysModeDataTypedef Hollidays;			/* Total 16 bytes*/
	DFH_PowDataOptTypedef OptPowData;						/* Total 10 bytes*/
	TimeStampTypedef LastHMITimeStamp;  				/* Total 12 bytes*/
	DFH_HMIMode Mode; 													/* Total 4 bytes*/
	DFH_AutoModeDataTypedef Auto;								/* Total 20 bytes*/
	DFH_ProgramModeDataTypedef Program;					/* Total 24 bytes*/
	DFH_ForcedModeDataTypedef Forced;						/* Total 2 bytes*/	

	
	
}DFH_CentralData;															/* Total 88 bytes*/	



void Test(void)
{
	int i;
	
	DFH_CentralData.LastHMITimeStamp.Sec=01;
	DFH_CentralData.LastHMITimeStamp.Min=02;
	DFH_CentralData.LastHMITimeStamp.Hour=03;
	DFH_CentralData.LastHMITimeStamp.Day=4;
	DFH_CentralData.LastHMITimeStamp.Month=5;
	DFH_CentralData.LastHMITimeStamp.Year=2024;
	
	DFH_CentralData.Mode=HMI_Mode_Auto;
	
	DFH_CentralData.Auto.TempMinExt=3.14159;
	DFH_CentralData.Auto.PowExcessStart=3.14159;
	DFH_CentralData.Auto.PowExcessStop=3.14159;
	DFH_CentralData.Auto.ClimPrio[0]=4;
	DFH_CentralData.Auto.ClimPrio[1]=3;
	DFH_CentralData.Auto.ClimPrio[2]=2;
	DFH_CentralData.Auto.ClimPrio[3]=1;
	DFH_CentralData.Auto.TempMinHC=01;
	DFH_CentralData.Auto.TempMinHP=02;
	
	for (i=0;i<24;i++)
	{
		DFH_CentralData.Program.TempPerHour[i]=i;
	}
	
	DFH_CentralData.Forced.Temp=15;
	
	DFH_CentralData.Hollidays.ArrivalDate.Sec=01;
	DFH_CentralData.Hollidays.ArrivalDate.Min=02;
	DFH_CentralData.Hollidays.ArrivalDate.Hour=03;
	DFH_CentralData.Hollidays.ArrivalDate.Day=4;
	DFH_CentralData.Hollidays.ArrivalDate.Month=5;
	DFH_CentralData.Hollidays.ArrivalDate.Year=2024;
	
	DFH_CentralData.OptPowData.PowExcess=3.14159;
	DFH_CentralData.OptPowData.PowL1_Home=3.14159;
	DFH_CentralData.OptPowData.Option=0x1234;
	
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
