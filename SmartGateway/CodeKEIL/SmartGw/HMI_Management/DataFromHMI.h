/* =================================================================================
* ==================   DataFromHMI.c	     =================================
 *
 *   Created on: 5/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 * Gère la data centrale qui est chargée par les données transmise par la HMI.
 * 
 * Le module fait aussi office de protocole au dessus de la couche UARTStack.
 * La payload est ainsi constituée :
 * 
 *		| Code Transmission | data alignées sur la structure correspondante|

 *
* =================================================================================*/
#ifndef DATAFROMHMI_H_
#define DATAFROMHMI_H_

#include "TimeStampManagement.h"


typedef enum {
	HMI_Mode_Off = 0xA0,
	HMI_Mode_Auto=0xA1,
	HMI_Mode_Program=0xA2,
	HMI_Mode_Hollidays = 0xA3,
}DFH_HMIMode;


/* ----  types structurés intermédiaires pour la var HMI_CentralData  ---- */

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





/* Code des trames émises par la HMI */
#define HMICode_UpdateAll
#define HMICode_UpdateTime
#define HMICode_UpdateMode
#define HMICode_UpdateAuto
#define HMICode_UpdateProgram
#define HMICode_UpdateForced
#define HMICode_UpdateHolliday
#define HMICode_UpdatePowerOption


/**
  * @brief   initialise la variable en attendant la première réception IHM...
  * @Note		 Voir le source ou le pdf SGw pour voir les valeurs d'init
  * @param  
  * @retval 
  **/
void DFH_Init_CentralData(void);

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_Update_CentralData(char * AdrString);

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_Update_All(char * AdrString, int Long);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
char DFH_ExtractReqCode(char * AdrString);

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void DFH_UpdateModeAuto(char * AdrString); 

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




/*====================================================================================
============== Fonctions de lecture des champs de la variable Central Data    ========
======================================================================================*/


/**
  * @brief  Renvoie l'adresse du TimeStamp de la CentralData
  * @Note
  * @param  
  * @retval @ variable
  **/
TimeStampTypedef* DFH_GetCentralData_Stamp(void);


/**
  * @brief  Renvoie l'adresse du Mode de la CentralData
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_HMIMode* DFH_GetCentralData_Mode(void);


/**
  * @brief  Renvoie les paramètres du mode Auto
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_AutoModeDataTypedef* DFH_GetCentralData_AutoParam(void);


/**
  * @brief  Renvoie les paramètres du mode Programmation
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_ProgramModeDataTypedef* DFH_GetCentralData_ProgParam(void);



/**
  * @brief  Renvoie les paramètres du mode Holliday
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_HollidaysModeDataTypedef* DFH_GetCentralData_HollidayParam(void);


/**
  * @brief  Renvoie les données d'option et de puissance
  * @Note
  * @param  
  * @retval @ variable
  **/
DFH_PowDataOptTypedef* DFH_GetCentralData_OptPowerData(void);



#endif 
