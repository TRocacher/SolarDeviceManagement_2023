/* =================================================================================
* ==================   InfoLCD.c	     =================================
 *
 *   Created on: 07/04/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/
#ifndef INFOLCD_H_
#define INFOLCD_H_

#include "MyLCD.h"
#include "DataFromRmDv.h"
#include "RmDv_SGw_Protocol.h" 
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"

/*============================================================================
 *
 * 		Les modes d'affichage LCD
 *
============================================================================*/

/* Gestion LCD, screen */

typedef enum {
	/* Main Sceen (dwn)*/
	Screen_Start,
	Screen_WaitForHMI,
	Screen_HowToUse,
	Screen_System,
	Screen_AllTemperatures,
	Screen_RmDvSalon,
	Screen_RmDvSaM,
	Screen_RmDvEntree,
	Screen_RmDvCouloir,
	Screen_RmDvExt,
	
	/* SubSceen system (Right)*/	
	SubScr_SysMode,
	SubScr_SysHMIStamp,
	SubScr_SysPowInverter,
	SubScr_SysPowerExcess,
	SubScr_SysDeltaStamp,
	
	/* SubSceen RmDv (Right)*/	
	SubScr_RmDv_LastStamp,
	SubScr_RmDv_Temperature,
	SubScr_RmDv_Status,
	SubScr_RmDv_NextInterval,
	SubScr_RmDv_DelayFactor,
	
} Screen;


/*  ========= LES FONCTIONS D'AFFICHAGE ===========*/ 

/**
	* @brief  Affiche l'écran de démarrage
	* @Note		Exemple : 
											SGw : Rev x.yz
											
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenStart(void);


/**
	* @brief  Affiche l'écran d'attente connexion HMI
	* @Note		Exemple : 
											Waiting for HMI
											...
											
  * @param  
  * @retval 
  **/
void InfoLCD_Screen_WaitForHMI(void);

/**
	* @brief  Affiche l'écran how to use
	* @Note		Exemple : 
											Push V main menu
											Push > sub-menu
											
  * @param  
  * @retval 
  **/
void InfoLCD_Screen_HowToUse(void);



/**
	* @brief  Affiche heure système format xx:xx:xx
	* @Note		Exemple : | System Clock :
											10:34:56
  * @param  
  * @retval 
  **/
	

void InfoLCD_ScreenSystem(void);

/**
* @brief  : Affichage des 5 températures
						Récupère les 5 températures via les 5 variables RmDv_Data
						du module DataFromRmDv.c
* @Note : Format :
					XX.X XX.X XX.X
					XX.X XX.X
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenAllTemp(void);

/**
	* @brief  : Affichage le ID du RmDv en question
	* @Note		Exemple : 
											|> Salon ID :
											0xD1
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenRmDvID(int ID);


/**
	* @brief  : Affichage le Mode du RmDv en question
	* @Note		Exemple : 
											> Mode :
											Programmation
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysMode(void);


/**
	* @brief  : Affichage de l'heure de la dernière requête HMI
	* @Note		Exemple : 
											> Last HMI stamp
											10:34:00
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysHMIStamp(void);


/**
	* @brief  : Affichage de la puissance onduleur
	* @Note		Exemple : 
											> Power Inverter
											1234.3 W
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysPowerInverter(void);



/**
	* @brief  : Affichage de la puissance en excès, disponible
	* @Note		Exemple : 
											> Power Excess :
											1234.3 W
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysPowerExcess(void);
	

/**
	* @brief  : Affichage de l'écart en secondes entre
							le stamp système et le stamp HMI
	* @Note		Exemple : 
											> Delta Stamp 
											HMI/SGw : -5 s
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysDeltaStamp(void);
	

/**
	* @brief  : Affichage de l'heure de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Last stamp
											10:34:00
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvLastStamp(int ID);


/**
	* @brief  : Affichage de la température de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Temperature
											21°C
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvTemp(int ID);


/**
	* @brief  : Affichage du statut de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Status :
											No Warning
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvStatus(int ID);


/**
	* @brief  : Affichage du prochain intervalle programmé lors
						de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Next interval:
											00:34:03
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvNextInterval(int ID);


/**
	* @brief  : Affichage facteur de correction de délai lors
						de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Delay Factor :
											1.24
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvDelayFactor(int ID);


// faire le tri...


	
typedef enum {
	Temperature=0,
	HeureCourante=1,
	Salon_1=2,
	Salon_2,
	SaM_1,
	SaM_2,
	Entree_1,
	Entree_2,
	Couloir_1,
	Couloir_2,
	Ext_1,
	Ext_2,
	Temp_18,
	Temp_19,
	Temp_20,
	Temp_21,
	Temp_22,
	Temp_23,
	Stop,
}TerminalMode;
#define ModeNb 19     /* pour le test des IR des RmDv*/
//#define ModeNb 12


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Init(void);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Status_LastTempSet( RmDv_WarningCode Status,char LastTempSet);




/**
* @brief  : Affichage des 5 températures
						Récupère les 5 températures via les 5 variables RmDv_Data
						du module DataFromRmDv.c
* @Note : Format :
					XX.X XX.X XX.X
					XX.X XX.X
  * @param  
  * @retval 
  **/
void InfoLCD_Print5Temp(void);


/**
* @brief  : Affichage du Stamp ID

* @Note : Format :
					"Salon Stamp"
					hh::mn:ss
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRmDv_Stamp(int ID);

/**
* @brief  : Affichage du Stamp ID, seconde partie (Status, Factor, NewSet)

* @Note : Format :
					"xxxxx" (statut)
					x.xx ; Set: yy
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRmDv_StatFactNewSet(int ID);
	



/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintHour(char * PtrLineUpStr,TimeStampTypedef* TimeStamp);



/**
* @brief  Affiche sur le LCD l'ID du RmDv ainsi que sa vession logicielle
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRevision(char * RevisionStr,int Len, char ID);


/*----------------------------------------------
 Gestion des Tamp String pour maintenance/debug
----------------------------------------------*/

/**
  * @brief 
  * @Note
  * @param  
  * @retval 
  **/

void InfoLCD_MemStampStrInit(void);


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
int InfoLCD_AddTimeStampToMem(TimeStampTypedef* TimeStamp, char ID);


/* Test IR */
RmDv_TelecoIR_Cmde InfoLCD_PrintNewSet(TerminalMode Mode);

#endif 
