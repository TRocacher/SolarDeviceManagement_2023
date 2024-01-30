
#include "ModuleFSK_RmDv.h"


/* =================================================================================
* ==================   Module_FSK.c	     =================================
 *
 *   Created on: 16/08/23
 *   Author: T.Rocacher
 *   Tool : Cube IDE 1.12.1
 *   Target : STM32L31F6P7
 *  ------------------------------------------------------------------------------
 *   G�re le module FSK RT606 433MHz
 *   Modifié légèrement pour coller au nouveau processeur
 *   - inclusion main.h pour les définitions
 *   - suppression configuration des IO puisque fait ailleurs.
 *
* =================================================================================*/




	//******************************************
	// Variables globales
	//******************************************

void CD_Callback(void);


/******************************************************************************************************************
	USART_FSK_Init

R�le :
Param : 
*****************************************************************************************************************/
void USART_FSK_Init(int Baud_Rate_bits_par_Sec, char Prio_USART_CD, char Prio_USART, void (*IT_function) (void))
/*

*/

{
	
	//******************************************
	// Variables locales
	//******************************************
USART_Init(UART_FSK, Baud_Rate_bits_par_Sec, Prio_USART, IT_function); 
	
	
	//******************************************
	// Utlisation CD pour valider IT : config IO
	//******************************************
	
#ifdef UseCarrierDetect

NVIC_Ext_IT (_CD_GPIO, _CD_Pin, FALLING_RISING_EDGE, INPUT_FLOATING, Prio_USART_CD, CD_Callback);
	
#endif
	

USART_FSK_SetReceiveAntenna();
}





/******************************************************************************************************************
	getter

R�le :
Param : 
*****************************************************************************************************************/
char USART_FSK_GetByte(void)
{
	return USART_GetByte(UART_FSK);
}



/******************************************************************************************************************
	USART_FSK_Print

R�le :
Param : 
*****************************************************************************************************************/

void USART_FSK_Print(char* Mssg, int Len)
{
	USART_Print(UART_FSK, Mssg, Len);
}
	

/******************************************************************************************************************
	USART_FSK_SetReceiveAntena et USART_FSK_SetTransmAntena

R�le :
Param : 
*****************************************************************************************************************/

void USART_FSK_SetReceiveAntenna(void)
{
	GPIO_Clear(_Tx_Cmde_GPIO,_Tx_Cmde_Pin);
	GPIO_Set(_Rx_Cmde_GPIO,_Rx_Cmde_Pin);
}
void USART_FSK_SetTransmAntenna(void)
{
	GPIO_Clear(_Rx_Cmde_GPIO,_Rx_Cmde_Pin);
	GPIO_Set(_Tx_Cmde_GPIO,_Tx_Cmde_Pin);
}

void USART_FSK_RT606_OFF(void)
{
	GPIO_Clear(_Rx_Cmde_GPIO,_Rx_Cmde_Pin);
	GPIO_Clear(_Tx_Cmde_GPIO,_Tx_Cmde_Pin);
}


/******************************************************************************************************************
	Les handlers d'IT

R�le :
Param : 
*****************************************************************************************************************/

void CD_Callback(void)
{
	if (GPIO_Read(_CD_GPIO,_CD_Pin)==1<<_CD_Pin) // front montant
	{
		// neutraliser IT UART
		USART_ReceivDisable(UART_FSK);
	}
	else 
	{
		USART_ReceivEnable(UART_FSK); // Receive Enable
	}
}
