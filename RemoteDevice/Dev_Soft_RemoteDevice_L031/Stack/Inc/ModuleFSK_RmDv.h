#ifndef _MODULE_FSK_H__
#define _MODULE_FSK_H__






#include "GPIO_L031.h"
#include "UART_L031.h"

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







// Choix de l'UART
#define UART_FSK USART2

// Autorisation UART IT seulement si CD est d�tect�.
#define UseCarrierDetect  

// Redefinitions des E/S spécifiques au module pour utilisation
// Pour ce module, _CD_Pin = 15, alors que dans RmDv_IO CD_Pin = 1<<15 !!
// La conf des 3 IO est faite dans RmDv_IO
#define _CD_GPIO GPIOB
#define _CD_Pin  15

#define _Rx_Cmde_GPIO GPIOA
#define _Rx_Cmde_Pin  4
#define _Tx_Cmde_GPIO GPIOA
#define _Tx_Cmde_Pin  0

/**************************************************************************************************
 Configure l'UART sp�cifi�e en #define, avec le d�bit indiqu� (en Baud), la priorit� utils�e pour
 la d�tection de porteuse, la priorit� d'interrution de r�ception, et enfin, on pr�cise la fonction
 callback associ�e � l'interruption.
 Exemple :
 USART_FSK_Init(PhyUART_BdRate,UART_Prio_CD,UART_Prio,UART_Callback);
***************************************************************************************************/
void USART_FSK_Init(int Baud_Rate_bits_par_Sec, char Prio_USART_CD, char Prio_USART, void (*IT_function) (void));

/**************************************************************************************************
 Renvoie le dernier caract�re lu par l'UART
 Exemple :
 if (USART_FSK_GetByte()==HeaderCar)
***************************************************************************************************/
char USART_FSK_GetByte(void);

/**************************************************************************************************
 Envoie caract�re par caract�re la cha�ne dont l'adresse est sp�cifi�e. Le nombre d'octet � �mettre
 est sp�cifi�e par le param�tre Len.
 Exemple :
 USART_FSK_Print("1234",4);
***************************************************************************************************/
void USART_FSK_Print(char* Mssg, int Len);

/**************************************************************************************************
 Fixe le module FSK en position de r�ception
***************************************************************************************************/
void USART_FSK_SetReceiveAntenna(void);

/**************************************************************************************************
 Fixe le module FSK en position d'�mission
***************************************************************************************************/
void USART_FSK_SetTransmAntenna(void);


#endif
