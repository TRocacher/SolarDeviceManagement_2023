/*
 * GLOBAL_RmDv..h
 *   Created on: August 4, 2024
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *   Revision : 1.00
 */

#ifndef INC_GLOBAL_RMDV_H_
#define INC_GLOBAL_RMDV_H_


/*============================================================================
 *
 * 		LES GRANDES PARTIES DE L'ARCHITECTURE
 *
 * Pile FSKStack (en interruption x3 - FSM timer, Systick (timeout), UART
 * Mesure de température I2C (pas d'IT)
 * LED IR télécommande (en interruption timer x2, un seul en IT, l'autre en PWM)
 * Watchdog (IT)
 * RTC (low power)
 * Gestion du son (non gérée encore)
============================================================================*/


/*============================================================================
 *
 * 		IO utilisés
 *
============================================================================*/

/* IO  VOIR FICHER RmDv_io.h dans le repertoire /RmDv_Services  */
/*
 *
 *                  STM32L041F6P7 / STM32L031F6P7  / TSSOP 20
 *           _________________________________________
 * 			|                                         |
 *      -->-|Boot0                           PA14 SWCK|----
 *      --<-|Boost_En PC14 (OD)             PA13 SWDIO|----
 *      -->-|CD PC15                     (OD) PA10 SDA|-><-
 *      -->-|nRST                        (OD) PA9  SCL|->--
 *      ->>-|VDDA                                  VDD|-<<-
 *      --<-|TxCmde PA0 (OD)                       VSS|-<<-
 *      --<-|LED_IR PA1 (PPUL)                  PB1 CT|-<--
 *      --<-|Tx PA2 (OD)                       PA7 INT|-<--
 *      -->-|Rx PA3                (analog)PA6 ADC in6|-<--
 *      --<-|RxCmde PA4 (OD)       (pull up)PA5 UserBP|-<--
 *          |_________________________________________|
 *
 *
 *
*/


/*============================================================================
 *
 * 		Revision number
 *
============================================================================*/

#define Revision	"Rev 1.00"

/*============================================================================
 *
 * 		Timers utilisés
 *
============================================================================*/

#define Timer_FSK_Stack_FSM TIM22  /* utilisé pour la FSM de la machine à état 1ms/100µs */
/*
 * -> SYSTICK pour la gestion des timeouts (FSK Stack et au niveau pgm principal), lib dédiée.
 */
#define Timer_Teleco_bit TIM21
#define Timer_Teleco_PWM TIM2
#define Timer_WDG
/*
 * -> LPTIM pour la gestion du watchdog, lib dédiée.
 */

/*============================================================================
 *
 * 		UART utilisé
 *
============================================================================*/
#define UART_FSKStack USART2
// Autorisation UART IT seulement si CD est d�tect�.
#define UseCarrierDetect

/*============================================================================
 *
 * 		I2C utilisé
 *
============================================================================*/
#define I2C_TempSensor I2C1


/*============================================================================
 *
 * 		TimeOut values et Vitesse de transmission
 *
============================================================================*/
/*--  Vitesse de tranmssion  --*/
#define Rate_UART_FSK 9600  /* NB : certains timeout peuvent sont élaborés avec cette valeur
 	 	 	 	 	 	 	 	- voir FSKStack.h
 	 	 	 	 	 	 	 	- voir RmDv_SGw_Protocol */
/*--  Les timeout  --*/
#define  Chrono_FSKStack Chrono_1 // file FSKStack.c (timeout Wait for header, FSM)
#define  Chrono_Protocole Chrono_2 // file StandByWkUpPgm.c (timeout au niveau protocole)
/* message max = req info =
 *  |MssgReq_SendInfo 		| Temperature (float) | LastTempSet (char) |
 *  soit 6 caractères.
 *  On ajoute 4 préambules (0xFF) puis 5# puis Long 1, puis payload 6 + org/dest 2 + CRC 1
 *  cela donne 21 caractères, margeons à N = 40 pour inclure le tps de calcul
 *  Temps de vol = N * 10 / Debit = 400/Débit
 *  Aller et retour (tps de calcule négligé, Temps de vol A/R = 800/débit */

#define TimeOutReq (800000/Rate_UART_FSK) /* pour 9600 Bds, 83ms*/
#define StatusReqTrialNb 3
#define  StatusReqTrialNb 3


/*============================================================================
 *
 * 		Gestion des priorités !! attention uniquement 3 niveaux de priorité !!
 *
============================================================================*/

#define Prio_FSKStack_SystickTimeOut 0						/* Non Bloquant*/
#define prio_Teleco_Timer_Bit 1								/* Bloquant*/
#define Prio_FSKStack_UART_CD 1								/* Non Bloquant*/
#define Prio_FSKStack_UART_Rec (Prio_FSKStack_UART_CD+1)	/* Non Bloquant*/
#define Prio_FSKStack_TimerFSM (Prio_FSKStack_UART_Rec+1)	/* Non Bloquant*/
#define prio_WDG 0											/* Non Bloquant*/



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
/* #define  My 0xDD */
#define My_ ID_Clim_Salon
#define SGw_ 0xDD
#define BroadCast 0xFF

/*============================================================================
 *
 * 		Définition des Backup registers 16 en tout
 *
============================================================================*/
#define BKPReg_TempSet BKP0R
//#define BKPReg_NextDelay_sec  BKP1R /* ne sert pas*/
#define BKPReg_RmDv_State BKP2R

#endif /* INC_GLOBAL_RMDV_H_ */
