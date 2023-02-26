/*
 * MenuUART.h
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
 *   ======= GENERALITE  ===================
 *
 *  ECRITURE Tx: une fonction print permet d'envoyer une chaîne de caractère. Elle est entièrement
 *  gérée en polling. Pourra être interrompue sans problème.
 *
 *  LECTURE Rx: la lecture est uniquement prévue à l'octet puisque les commandes (8 possibles
 *  duplication de 4 commandes up, Down, Left, Right pour jouer sur la partie gauche ou droite
 *  du clavier, voir #define).
 *
 *  Une interruption est déclenchée sur réception d'un caractère. Si le caractère est reconnu
 *  comme étant une commande, cette dernière est mémorisée.
 *
 *  Deux initialisations sont possibles, l'une MenuUART_Init conduit l'utilisateur à scruter
 *  un flag d'arrivée d'une nouvelle commande.
 *  L'autre MenuUART_Init_IT permet à l'utilisateur d'indiquer une fonction callback que sera
 *  appelée dès reconnaissance d'une commande dans la fonction d'interruption.
 *
 *  En mode polling (MenuUART_Init), le user doit gérer le flag de réception d'une nouvelle
 *  commande, et également l'effacer après lecture. Le user doit tout gérer.
 *  En mode IT (MenuUART_Init_IT), le user ne se soucit de rien. L'entrée dans le callback est
 *  automatique dès qu'une commande est reconnue, aucun flag à gérer.
 *
 */

#ifndef MenuUART_MenuUART_H_
#define MenuUART_MenuUART_H_

#include "UART.h"



/******************************************************************************
*  Priorité IT du module
*
******************************************************************************/
#define MenuUART_Prio 6


/******************************************************************************
*  Les deux Fonctions d'initialisations
*
******************************************************************************/

/**
  * @brief  Initialise le module UARTCom en mode polling sur la réception
  * @param  UsedUSART = le handle de l'usart au sens de Hal
  * @retval None
  **/
void MenuUART_Init(UART_HandleTypeDef * UsedUSART);


/**
  * @brief  Initialise le module UARTCom en mode interruption avec
  * 		callback sur la réception d'une commande (caractère reçu filtré)
  * @param  UsedUSART = le handle de l'usart au sens de Hal
  * @param  Callback = nom de la fonction appelé sur réception d'une commande
  * @retval None
  **/
void MenuUART_Init_IT(UART_HandleTypeDef * UsedUSART, void (*Callback)(void));


/******************************************************************************
*  Les fonctions de lecture de commande (attention au mode d'init...)
*
******************************************************************************/

/**
  * @brief  Lit la Cmde contenu dans la variable d'état
  * @note   à utiliser quelque soit le mode d'init.
  * @param  none
  * @retval char, la Cmde.
  **/
char MenuUART_GetCmd(void);

/** UNIQUEMENT MODE INIT SIMPLE
  * @brief  Efface le drapeau de réception commande
  * @note  : Ne pas utiliser si init avec MenuUART_Init_IT
  *          Doit être appelé immédiatement après lecture de la Cmde
  * @param  none
  * @retval none
  **/
void MenuUART_ClrReceptFlag(void);

/** UNIQUEMENT MODE INIT SIMPLE
  * @brief  Renseigne sur l'arrivée d'une nouvelle commande
  * @note  : Ne pas utiliser si init avec MenuUART_Init_IT.
  * 		prévu en mode polling pour savoir si une cmde est arrivée
  * 	    Les cmdes sont définies dans le .h
  * @param  none
  * @retval 1 si réception d'une Cmde
  **/
char MenuUART_IsNewCmde(void);


/******************************************************************************
*  Les fonctions de d'écriture print (quelque soit le mode d'init
*
******************************************************************************/

/**
  * @brief  Print une chaîne de caractère sur l'USART (Tx)
  * @note : utilise fct HAL_UART_Transmit, timing testé à 9600Bds,
  * 		correspond au débit attendu
  * @param  char * str adresse de la châine à envoyer
  * @param  Lenght longueur de la chaîne à transmettre
  * @retval none
  **/
void MenuUART_Print(char * Str, char Lenght);

/**
  * @brief  Renseigne sur l'état du module en terme de transmission
  * @param  none
  * @retval 1 si transmission en cours, 0 si Tx libre
  **/
char MenuUART_IsTansmitting(void);





/******************************************************************************
*  Les commandes (code ASCII touches clavier)
*
******************************************************************************/

// pour les droitiers
#define Uart_Up_R 'o'
#define Uart_Down_R 'l'
#define Uart_Left_R 'k'
#define Uart_Right_R 'm'
// pour les gauchers
#define Uart_Up_L 'z'
#define Uart_Down_L 's'
#define Uart_Left_L 'q'
#define Uart_Right_L 'd'



#endif /* MenuUART_MenuUART_H_ */
