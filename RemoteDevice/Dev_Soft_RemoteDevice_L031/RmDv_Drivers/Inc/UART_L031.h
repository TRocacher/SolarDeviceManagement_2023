#ifndef _UART_L031_H__
#define _UART_L031_H__


#include "stm32l0xx.h"


//=======================================================================================
// Rappel sur les ressources sur le STM32L031
// Une seule UART classique, USART2, PA2 et PA3 Tx, Rx
// NB : Les IO sont pas intialisées ds le modules. Elles doivent l'être ailleurs.
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
 USART_Print("1234",4);
***************************************************************************************************/
void USART_Print(USART_TypeDef * USART,char* Mssg, int Len);


#define USART_ReceivDisable(UART) (UART->CR1=(UART->CR1)&~USART_CR1_RE)
#define USART_ReceivEnable(UART) (UART->CR1=(UART->CR1)|USART_CR1_RE)



#endif

