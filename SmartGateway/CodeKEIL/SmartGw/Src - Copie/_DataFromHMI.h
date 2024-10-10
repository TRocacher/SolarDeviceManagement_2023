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




typedef enum {
	HMI_Mode_Off = 0xA0,
	HMI_Mode_Auto=0xA1,
	HMI_Mode_Program=0xA2,
	HMI_Mode_Hollidays = 0xA3,
}DFH_HMIMode;


/* Code des trames émises par la HMI */
#define HMICode_UpdateAll
#define HMICode_UpdateTime
#define HMICode_UpdateMode
#define HMICode_UpdateAuto
#define HMICode_UpdateProgram
#define HMICode_UpdateForced
#define HMICode_UpdateHolliday
#define HMICode_UpdatePowerOption



void Test(void);

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

#endif 
