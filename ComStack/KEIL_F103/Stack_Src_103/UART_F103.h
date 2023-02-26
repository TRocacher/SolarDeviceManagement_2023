#ifndef _UART_F103_H__
#define _UART_F103_H__


#include "stm32f10x.h"


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
// 
// NB : Les IO sont automatiquement fixées lors de l'intialisation
//
//=======================================================================================




/**************************************************************************************************
 Configure l'UART spécifiée avec le débit indiqué (en Baud),  la priorité d'interrution de réception,
 et enfin, on précise la fonction callback associée à l'interruption.
***************************************************************************************************/
void USART_Init(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*IT_function) (void));



/**************************************************************************************************
 Renvoie le dernier caractère lu par l'UART
 Exemple :

***************************************************************************************************/
char USART_GetByte(USART_TypeDef * USART);

/**************************************************************************************************
 Envoie caractère par caractère la chaîne dont l'adresse est spécifiée. Le nombre d'octet à émettre
 est spécifiée par le paramètre Len.
 Exemple :
 USART_FSK_Print("1234",4);
***************************************************************************************************/
void USART_Print(USART_TypeDef * USART,char* Mssg, int Len);





#endif

