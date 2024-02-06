#ifndef _RESSOURCESPERIPH_H__
#define _RESSOURCESPERIPH_H__


/* =================================================================================
* ==================   Ressources p�riphs.h	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : _
 *  ------------------------------------------------------------------------------
 *  Donne la liste des p�riph�riques utilis�s
* =================================================================================*/


/*  FSK Stack  */
#define TIM_PhyUART_FSM TIM2 	/* Pour l'animation du graphe d'�tat : used in FSKStack.c*/
#define UART_FSK USART3				/* Pour le module RT606 FSK  : used in ModuleFSK.c*/
// SYSTICK 										/* Pour les timeout utilis�s dans la pile FSK, used in FSKStack.c*/

/*  Horodatage   */
#define TIMER_TimeStamp TIM4	/* Pour l'incr�mentation TimeStamp, used in TimeStampManagement.c*/

/*  UART Stack  */
#define UART_HMI USART2				/* Pour l'HMI PA2 (Tx)  et �PA3 (Rx) donc */
#define TIM_UARTStack TIM3 		/* Pour g�rer le timeout : used in UARTStack.c*/
#define UART_HMI_BaudRate 9600

#endif

