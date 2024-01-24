#ifndef _UART_STACK_H__
#define _UART_STACK_H__

#include "UART_F103.h"
#include "TimeManagement.h"

/* les valeurs possibles d'ereur */
typedef enum {
	_NoError,
	_CheckSumError,
	_TooLong,
	_TimeOut,
}UARTStack_ErrorType;

#define UART_HMI USART2
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
