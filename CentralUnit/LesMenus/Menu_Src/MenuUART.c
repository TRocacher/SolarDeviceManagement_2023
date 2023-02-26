/*
 * MenuUART.c
 *
 *  Created on: Apr 22, 2021
 *      Author: trocache
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : pilotage, régulation de systèmes de puissance. Partie analogique
 *  DEPENDENCES : Le code s'appuie sur les registres du stm32L476 donc
 *  essentiellement sur stm32l476xx.h via stm32l4xx.h (voir Drivers/CMSIS/ST/STM324xx/Include
 *
 *
 *  Notes sur les lib
 *
 *  PROGRAMMATION au niveau registres, .h utiles :
 *  		stm32l476xx.h (pour les registres et  Irqn )
 *  		core_cm4.h (pour les fct bas niveau du NVIC, enable, prio)
 *
 *  IMBRICATION des .h :
 *
 * stm32l4xx_hal.h
 * -> stm32l4xx_hal_conf.h
 * 		-> stm32l4xx_hal_xxperiphxx.h comme par ex stm32l4xx_hal_uart.h
 *			-> stm32l4xx_hal_def.h
 *				-> stm32l4xx.h
 *					-> stm32l476xx.h (les Irqn les typedef niveau registres
 *						-> core_cm4.h (les fct NVIC, reg du coeur, Systick, nvic...)
 */


#include "MenuUART.h"


/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

/**
  * @brief Variable d'état du module
  * Cmde, RecepFlag, TransmFlag accessibles
  **/
struct {
	char Cmde;      // le caractère filtré
	char CurrentByte; // caractère effectivement reçu non filtré
	char RecepFlag;  // indique qu'une commande est arrivée
	char TransmFlag; // indique qu'un print est en cours
	void (*MenuUART_Callback)(void);
}MenuUART_Status;




static char Interrupt;



/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions d'initialisation
******************************************************************************/

/**
  * @brief  Initialise le module UARTCom en mode interruption avec
  * 		callback sur la réception d'une commande (caractère reçu filtré)
  * @param  
  * @param  Callback = nom de la fonction appelé sur réception d'une commande
  * @retval None
  **/
void MenuUART_Init_IT(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*Callback)(void))
{
	MenuUART_Status.Cmde=0; // aucune cmde valide
	MenuUART_Status.CurrentByte=0;
  MenuUART_Status.MenuUART_Callback=Callback;
	MenuUART_Status.RecepFlag=0;
	MenuUART_Status.TransmFlag=0;
	Interrupt=1;
	
	USART_Init(USART, Baud_Rate_bits_par_Sec, Prio_USART, Callback);
}


/**
  * @brief  Initialise le module MenuUART en mode polling sur la réception
  * @param  
  * @retval None
  **/
void MenuUART_Init(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*Callback)(void))
{
	MenuUART_Status.Cmde=0; // aucune cmde valide
	MenuUART_Status.CurrentByte=0;
  MenuUART_Status.MenuUART_Callback=0;
	MenuUART_Status.RecepFlag=0;
	MenuUART_Status.TransmFlag=0;
	Interrupt=0;
	
	USART_Init(USART, Baud_Rate_bits_par_Sec, Prio_USART, Callback);

}


/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions d'écriture (print)
******************************************************************************/

/**
  * @brief  Renseigne sur l'état du module en terme de transmission
  * @param  none
  * @retval 1 si transmission en cours, 0 si Tx libre
  **/
char MenuUART_IsTansmitting(void)
{
	return MenuUART_Status.TransmFlag;
}


/**
  * @brief  Printe une chaîne de caractère sur l'USART (Tx)
  * @note : utilise fct HAL_UART_Transmit, timing testé à 9600Bds,
  * 		correspond au débit attendu
  * @param  char * str adresse de la châine à envoyer
  * @param  Lenght longueur de la chaîne à transmettre
  * @retval none
  **/
void MenuUART_Print(char * Str, char Lenght)
// testé avec IO set/reset entrée et sortie de fct.
// la durée est parfaitement respectée.
{
	MenuUART_Status.TransmFlag=1;
	HAL_UART_Transmit(MenuUART_HandlerPtr,(uint8_t *) Str, Lenght, HAL_MAX_DELAY);
	MenuUART_Status.TransmFlag=0;
}


/******************************************************************************
*  PUBLIC FUNCTION
*  Fonctions de lecture MenuUART
******************************************************************************/

/**
  * @brief  Renseigne sur l'arrivée d'une nouvelle commande
  * @note  : Ne pas utiliser si init avec MenuUART_Init_IT.
  * 		prévu en mode polling pour savoir si une cmde est arrivée
  * 	    Les cmdes sont définies dans le .h
  * @param  none
  * @retval 1 si réception d'une Cmde
  **/
char MenuUART_IsNewCmde(void)
{
	return MenuUART_Status.RecepFlag;
}

/**
  * @brief  Efface le drapeau de réception commande
  * @note  : Ne pas utiliser si init avec MenuUART_Init_IT
  *          Doit être appelé immédiatement après lecture de la Cmde
  * @param  none
  * @retval none
  **/
void MenuUART_ClrReceptFlag(void)
{
	MenuUART_Status.RecepFlag=0;
}

/**
  * @brief  Lit la Cmde contenu dans la variable d'état
  * @note   à utiliser quelque soit le mode d'init.
  * @param  none
  * @retval char, la Cmde.
  **/
char MenuUART_GetCmd(void)
{
	return MenuUART_Status.Cmde;
}



/******************************************************************************
*  Private Interrupt FUNCTION
*  Fonction associée exclusivement à la réception d'un caractère quelconque
******************************************************************************/

/**
  * @brief  IT déclenchée quelque soit le mode d'init, sur réception d'un byte
  * @note   filtre les commandes
  * 		met à jour le Flag de réception en mode init non interruptif.
  * 		ne gère pas le flag en mode interruptif.
  * 		lance le callback si Cmde détectée et en mode init interruptif
  * @param  none
  * @retval char, la Cmde.
  **/
void USART2_IRQHandler(void)
{
	char Byte;
	MenuUART_HandlerPtr->Instance->ICR|=USART_ICR_ORECF; // mise à 0 si l'IT provient d'un overrun
	Byte=MenuUART_HandlerPtr->Instance->RDR;
	MenuUART_Status.CurrentByte=Byte;
	switch (Byte)
	{
	case Uart_Up_R:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Up_L:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Down_R:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Down_L:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Left_R:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Left_L:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Right_R:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	case Uart_Right_L:MenuUART_Status.Cmde=Byte;MenuUART_Status.RecepFlag=1;break;
	default : MenuUART_Status.Cmde=0;

	}

	if ((Interrupt==1) && (MenuUART_Status.RecepFlag==1))
		{
		MenuUART_Status.RecepFlag=0;
		MenuUART_Status.MenuUART_Callback();
		}
}
