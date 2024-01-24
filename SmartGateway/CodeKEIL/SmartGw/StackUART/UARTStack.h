#ifndef _UART_STACK_H__
#define _UART_STACK_H__


/* =================================================================================
* ==================   UARTStack.c	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : TimeStampManagement (TimeOut)
 *  ------------------------------------------------------------------------------
 * Pile de comùmunication UART
 * Principe :
 * Format de trame UART : 
 
     |NbOctets| Data Bytes|CheckSum|
		  NbOctets = Data byte nb + 1 (checksum). Le byte NbOctets n'est pas compté
			
			La chaîne récupéré ressemble à ceci :
			|5|'T'|'O'|'T'|'O'| checkS|
        0  1  2   3   4    5
				
 * L'UART fonctionne en IT sur réception byte.
   à la première réception, on capte la longueur
	 au bytes suivant la chaîne se construit
	 si elle est incomplète alors un timout stoppe le processus
	 
	 des getter permettent d'accéder au erreurs et au tableau de 256 bytes
 *
* =================================================================================*/


#include "UART_F103.h"
#include "Timer_F103.h"
#include "RessourcesPeriph.h"

/* les valeurs possibles d'ereur */
typedef enum {
	_NoError,
	_CheckSumError,
	_TimeOut,
}UARTStack_ErrorType;


#define Chrono_TimeOut_UARTStack Chrono_5


/**
  * @brief  Initialise la stack (UART et TimeOut. Voir .h pour
	* 				identifier UART et Timeout 
	* 
**/
void UARTStack_Init(void);


/**
  * @brief  Permet de savoir si un message est reçu.
	* @ret   1 si un message est reçu, 0 sinon
**/
char UARTStack_IsHMIMssg(void);



/**
  * @brief  Getter error type
	* @ret   UARTStack_ErrorType voir .h pour les valeurs
**/
UARTStack_ErrorType UARTStack_GetErrorStatus(void);


/**
  * @brief  Getter mssg 
	* @ret    @ de string de réception (256 max).
	* @Note   la longueur est inscrite dans le premier byte
**/
char * UARTStack_GetHMIMssg(void);



#endif
