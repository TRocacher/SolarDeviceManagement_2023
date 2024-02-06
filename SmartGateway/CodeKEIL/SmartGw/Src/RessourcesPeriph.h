#ifndef _RESSOURCESPERIPH_H__
#define _RESSOURCESPERIPH_H__


/* =================================================================================
* ==================   Ressources périphs.h	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *  ------------------------------------------------------------------------------
 *  Donne la liste des périphériques utilisés
* =================================================================================*/


/*  FSK Stack  */
#define TIM_PhyUART_FSM TIM2 	/* Pour l'animation du graphe d'état : used in FSKStack.c*/
#define UART_FSK USART3				/* Pour le module RT606 FSK  : used in ModuleFSK.c*/
// SYSTICK 										/* Pour les timeout utilisés dans la pile FSK, used in FSKStack.c*/

/*  Horodatage   */
#define TIMER_TimeStamp TIM4	/* Pour l'incrémentation TimeStamp, used in TimeStampManagement.c*/

/*  UART Stack  */
#define UART_HMI USART2				/* Pour l'HMI PA2 (Tx)  et ¨PA3 (Rx) donc */
#define TIM_UARTStack TIM3 		/* Pour gérer le timeout : used in UARTStack.c*/
#define UART_HMI_BaudRate 9600

#endif

