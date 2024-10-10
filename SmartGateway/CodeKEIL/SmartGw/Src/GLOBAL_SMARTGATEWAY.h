#ifndef _GLOBAL_SMARTGATEWAY_H__
#define _GLOBAL_SMARTGATEWAY_H__

#include "RessourcesPeriph.h"

/* =================================================================================
* ==================    GLOBAL_SMARTGATEWAY.h	     =================================
 *
 *   Created on: 04/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *   Revision : 2.01 Voir Fichier révision .txt. 
 *  ------------------------------------------------------------------------------
 *  Donne les constantes nécessaires à l'ensemble des échanges (haut niveau)
* =================================================================================*/


#define RevisionSoft	"Rev 2.01"
/* 1.01 Ajout de l'ensemble HMI + RmDv*/
/* 1.02 Ajout fonctionnalité LCD + fonctionnalité "no beep" en cours de dév.*/
/* Rev 1.03 : Validation mode beep/nobeep sur un seul RmDv (vers 1.00)
Le mode programmation est OK sur les premiers tests.*/
/*Rev 1.04 : quelques bug d'affichage LCD fixé, InfoLCD nettoyé (faire le .h)*/


/*============================================================================
 *
 * 		Adresses réseau wireless FSK
 *
============================================================================*/

/* Ident des 4 climatisations NB : Les codes doivent se suivrent dans l'ordre et sans saut
	depuis ID_Clim_Salon jusqu'à ID_Ext !! */
#define	ID_Clim_Salon 0xD1
#define	ID_Clim_SaManger 0xD2
#define	ID_Clim_Entree 0xD3
#define	ID_Clim_Couloir 0xD4
#define ID_Ext 0xD5
#define  My_ 0xDD

/*--  Les timeout  --*/
#define Chrono_FSKStack Chrono_1 /* file FSKStack.c (timeout Wait for header, FSM)*/
#define Chrono_WaitTransactionEnd Chrono_2 /* file SmartGwMain (timeout d'attente seconde requête)*/
#define Chrono_Debounce_BpDwn Chrono_3 /* permet de gérer l'antirebond sur le BP Down */
#define Chrono_Debounce_BpRight Chrono_4 /* permet de gérer l'antirebond sur le BP Right */
#define Chrono_TimeOutMenuLCD Chrono_5 /* permet un retour à l'écran principal au bout de n sec*/
/* message max = req info =
 *  |MssgReq_SendInfo 		| Temperature (float) | LastTempSet (char) |
 *  soit 6 caractères. 
 *  On ajoute 4 préambules (0xFF) puis 5# puis Long 1, puis payload 6 + CRC 1
 *  cela donne 19 caractères, margeons à N = 25 
 *  Temps de vol = N * 10 / Debit = 250/Débit
 *  Aller et retour (tps de calcule négligé, Temps de vol A/R = 500/débit
 * au maximum 2 requêtes, jusqu'à 3 tentatives, à régler....
 */
 
#define Tentatives	3
#define ReqNb 2
#define TimeOutTransaction  (Tentatives*ReqNb*800000/Rate_UART_FSK) 


/*============================================================================
 *
 * 		Gestion des priorités
 *
============================================================================*/

#define Prio_FSKStack_SystickTimeOut 0
#define Prio_TimeStamp 1
#define Prio_FSKStack_UART_CD 2
#define Prio_FSKStack_UART_Rec (Prio_FSKStack_UART_CD+1)
#define Prio_FSKStack_TimerFSM (Prio_FSKStack_UART_Rec+1)


/*============================================================================
 *
 * 		Commande Clim
 *
============================================================================*/
/* Liste des code de télécommande */
typedef enum {
	_Chaud_18_VanBas_FanAuto = 0xC1,
	_Chaud_19_VanBas_FanAuto = 0xC2,
	_Chaud_20_VanBas_FanAuto = 0xC3,
	_Chaud_21_VanBas_FanAuto = 0xC4,
	_Chaud_22_VanBas_FanAuto = 0xC5,
	_Chaud_23_VanBas_FanAuto = 0xC6,
	_NoCommandToSend = 0xC7,
	_Stop = 0xC0,
}RmDv_TelecoIR_Cmde;




#endif

