#include "stm32f10x.h"
#include "ModuleFSK.h"


/* =================================================================================
* ==================   Module_FSK.c	     =================================
 *
 *   Created on: 15/08/23
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *  ------------------------------------------------------------------------------
 *   Gère le module FSK RT606 433MHz
 *
* =================================================================================*/



//=======================================================================================
// Rappel sur les ressources sur le STM32F103RB 
//
// 3 USART possibles :
// USART_1_TX = TIM1_CH2 = PA9
// USART_1_RX = TIM1_CH3 = PA10
// USART_2_TX = TIM2_CH3 = PA2
// USART_2_RX = TIM2_CH4 = PA3
// USART_3_TX = PB10
// USART_3_RX = PB11
//=======================================================================================





	//******************************************
	// Variables globales
	//******************************************

void CD_Callback(void);


/******************************************************************************************************************
	USART_FSK_Init

Rôle :
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

GPIO_Configure(CD_GPIO, CD_Pin, INPUT, INPUT_FLOATING);	
NVIC_Ext_IT (CD_GPIO, CD_Pin, FALLING_RISING_EDGE, INPUT_FLOATING, Prio_USART_CD, CD_Callback);
	
#endif
	

	//******************************************
	// Configuration RxCmde et TxCmde pin
	//******************************************
GPIO_Configure(CD_GPIO, CD_Pin, INPUT, INPUT_FLOATING);	
GPIO_Configure(Rx_Cmde_GPIO, Rx_Cmde_Pin, OUTPUT, OUTPUT_PPULL);	
GPIO_Configure(Tx_Cmde_GPIO, Tx_Cmde_Pin, OUTPUT, OUTPUT_PPULL);
USART_FSK_SetReceiveAntenna();
}





/******************************************************************************************************************
	getter

Rôle :
Param : 
*****************************************************************************************************************/
char USART_FSK_GetByte(void)
{
	return USART_GetByte(UART_FSK);
}



/******************************************************************************************************************
	USART_FSK_Print

Rôle :
Param : 
*****************************************************************************************************************/

void USART_FSK_Print(char* Mssg, int Len)
{
	USART_Print(UART_FSK, Mssg, Len);
}
	

/******************************************************************************************************************
	USART_FSK_SetReceiveAntena et USART_FSK_SetTransmAntena

Rôle :
Param : 
*****************************************************************************************************************/

void USART_FSK_SetReceiveAntenna(void)
{
	GPIO_Clear(Tx_Cmde_GPIO,Tx_Cmde_Pin);
	GPIO_Set(Rx_Cmde_GPIO,Rx_Cmde_Pin);
}
void USART_FSK_SetTransmAntenna(void)
{
	GPIO_Clear(Rx_Cmde_GPIO,Rx_Cmde_Pin);
	GPIO_Set(Tx_Cmde_GPIO,Tx_Cmde_Pin);
}




/******************************************************************************************************************
	Les handlers d'IT

Rôle :
Param : 
*****************************************************************************************************************/

void CD_Callback(void)
{
	if (GPIO_Read(CD_GPIO,CD_Pin)==1<<CD_Pin) // front montant
	{
		// neutraliser IT UART
		USART_ReceivDisable(UART_FSK);
	}
	else 
	{
		USART_ReceivEnable(UART_FSK); // Receive Enable
	}
}
