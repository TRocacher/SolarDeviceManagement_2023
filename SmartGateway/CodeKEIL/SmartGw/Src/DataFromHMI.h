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
#ifndef DATAFROMHMI_H_
#define DATAFROMHMI_H_




typedef enum {
	HMI_Mode_Off = 0xA0,
	HMI_Mode_Auto=0xA1,
	HMI_Mode_Program=0xA2,
	HMI_Mode_Forced = 0xA3,
	HMI_Mode_Hollidays = 0xA4,
	HMI_Option_PowData = 0xA5,
}DFH_HMIMode;


#define	ID_Clim_Salon 1
#define	ID_Clim_SaManger 2
#define	ID_Clim_Entree 3
#define	ID_Clim_Couloir 4

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
