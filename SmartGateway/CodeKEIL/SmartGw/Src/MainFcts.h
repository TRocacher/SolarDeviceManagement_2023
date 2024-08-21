/* =================================================================================
* ==================   MainFcts.h	     =================================
 *
 *   Created on: 5/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/
#ifndef MAINFCT_H_
#define MAINFCT_H_


/**
* @brief  MainFcts_SystemStart
					Lance la pile FSK(RmDv), la pile UART (HMI)
					Lance le système de gestion des timeout
					
* @note :_
* @retval :_
**/
void MainFcts_SystemStart(void);	



/**
* @brief  MainFcts_SetArbitraryTime
					Mets à l'heure le système (pour l'instant arbitraire)
					Lance aussi la progression de l'index tps réel pour pointer les fuseaux hooraires
					Initialise la table des fuseaux horaires
* @note :_
* @retval :_
**/
void MainFcts_SetArbitraryTime(void);



#endif 
