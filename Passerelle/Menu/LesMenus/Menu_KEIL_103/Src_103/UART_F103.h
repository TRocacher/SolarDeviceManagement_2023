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
// NB : Les IO sont automatiquement fix�es lors de l'intialisation
//
//=======================================================================================




/**************************************************************************************************
 Configure l'UART sp�cifi�e avec le d�bit indiqu� (en Baud),  la priorit� d'interrution de r�ception,
 et enfin, on pr�cise la fonction callback associ�e � l'interruption.
***************************************************************************************************/
void USART_Init(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*IT_function) (void));



/**************************************************************************************************
 Renvoie le dernier caract�re lu par l'UART
 Exemple :

***************************************************************************************************/
char USART_GetByte(USART_TypeDef * USART);

/**************************************************************************************************
 Envoie caract�re par caract�re la cha�ne dont l'adresse est sp�cifi�e. Le nombre d'octet � �mettre
 est sp�cifi�e par le param�tre Len.
 Exemple :
 USART_FSK_Print("1234",4);
***************************************************************************************************/
void USART_Print(USART_TypeDef * USART,char* Mssg, int Len);





#endif

