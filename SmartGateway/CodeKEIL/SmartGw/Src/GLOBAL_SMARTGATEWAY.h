#ifndef _GLOBAL_SMARTGATEWAY_H__
#define _GLOBAL_SMARTGATEWAY_H__

#include "RessourcesPeriph.h"

/* =================================================================================
* ==================    GLOBAL_SMARTGATEWAY.h	     =================================
 *
 *   Created on: 22/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : _
 *  ------------------------------------------------------------------------------
 *  Donne les constantes n�cessaires � l'ensemble des �changes (haut niveau)
* =================================================================================*/


/*============================================================================
 *
 * 		Adresses r�seau wireless FSK
 *
============================================================================*/

/* Ident des 4 climatisations NB : Les codes doivent se suivrent dans l'ordre et sans saut
	depuis ID_Clim_Salon jusqu'� ID_Ext !! */
#define	ID_Clim_Salon 0xD1
#define	ID_Clim_SaManger 0xD2
#define	ID_Clim_Entree 0xD3
#define	ID_Clim_Couloir 0xD4
#define ID_Ext 0xD5
#define  My_ 0xDD

/*--  Les timeout  --*/
#define Chrono_FSKStack Chrono_1 /* file FSKStack.c (timeout Wait for header, FSM)*/
#define Chrono_WaitTransactionEnd Chrono_2 /* fileSmartGwMain (timeout d'attente seconde requ�te)*/
#define Chrono_Debounce Chrono_3 /* permet de g�rer l'antirebond*/

/* message max = req info =
 *  |MssgReq_SendInfo 		| Temperature (float) | LastTempSet (char) |
 *  soit 6 caract�res. 
 *  On ajoute 4 pr�ambules (0xFF) puis 5# puis Long 1, puis payload 6 + CRC 1
 *  cela donne 19 caract�res, margeons � N = 25 
 *  Temps de vol = N * 10 / Debit = 250/D�bit
 *  Aller et retour (tps de calcule n�glig�, Temps de vol A/R = 500/d�bit
 * au maximum 2 requ�tes, jusqu'� 3 tentatives, � r�gler....
 */
 
#define Tentatives	3
#define ReqNb 2
#define TimeOutTransaction  (Tentatives*ReqNb*800000/Rate_UART_FSK) 


/*============================================================================
 *
 * 		Gestion des priorit�s
 *
============================================================================*/

#define Prio_FSKStack_SystickTimeOut 0
#define Prio_FSKStack_UART_CD 0
#define Prio_FSKStack_UART_Rec (Prio_FSKStack_UART_CD+1)
#define Prio_FSKStack_TimerFSM (Prio_FSKStack_UART_Rec+1)


/*============================================================================
 *
 * 		Commande Clim
 *
============================================================================*/
/* Liste des code de t�l�commande */
typedef enum {
	_Chaud_18_VanBas_FanAuto = 0xC1,
	_Chaud_19_VanBas_FanAuto = 0xC2,
	_Chaud_20_VanBas_FanAuto = 0xC3,
	_Chaud_21_VanBas_FanAuto = 0xC4,
	_Chaud_22_VanBas_FanAuto = 0xC5,
	_Chaud_23_VanBas_FanAuto = 0xC6,
	_Stop = 0xC0,
}RmDv_TelecoIR_Cmde;


/*============================================================================
 *
 * 		Les modes d'affichage LCD
 *
============================================================================*/

/* Type d'affichage ...*/
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
	Stop,
	Temp_18,
	Temp_19,
	Temp_20,
	Temp_21,
	Temp_22,
	Temp_23,
}TerminalMode;
#define ModeNb 19     /* pour le test des IR des RmDv*/
//#define ModeNb 12


#endif

