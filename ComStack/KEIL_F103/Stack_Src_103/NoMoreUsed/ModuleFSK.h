#ifndef _FSK_F103_H__
#define _FSK_F103_H__


#include "stm32f10x.h"
#include "GPIO_F103.h"
#include "UART_F103.h"

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



// Choix de l'UART
#define UART_FSK USART3

// Autorisation UART IT seulement si CD est détecté.
#define UseCarrierDetect  


// Carrier Detect !! ATTENTION !! Pgm prévu seulement pour broche 5 à 9. En dehors, revoir le .c
// Aux endroits repérés //---- !!! valable 5 à 9, sinon modifier ici ----///
#define CD_GPIO GPIOB 
#define CD_Pin  7

#define Rx_Cmde_GPIO GPIOB 
#define Rx_Cmde_Pin  8
#define Tx_Cmde_GPIO GPIOB
#define Tx_Cmde_Pin  9

/**************************************************************************************************
 Configure l'UART spécifiée en #define, avec le débit indiqué (en Baud), la priorité utilsée pour
 la détection de porteuse, la priorité d'interrution de réception, et enfin, on précise la fonction
 callback associée à l'interruption.
 Exemple :
 USART_FSK_Init(PhyUART_BdRate,UART_Prio_CD,UART_Prio,UART_Callback);
***************************************************************************************************/
void USART_FSK_Init(int Baud_Rate_bits_par_Sec, char Prio_USART_CD, char Prio_USART, void (*IT_function) (void));

/**************************************************************************************************
 Renvoie le dernier caractère lu par l'UART
 Exemple :
 if (USART_FSK_GetByte()==HeaderCar)
***************************************************************************************************/
char USART_FSK_GetByte(void);

/**************************************************************************************************
 Envoie caractère par caractère la chaîne dont l'adresse est spécifiée. Le nombre d'octet à émettre
 est spécifiée par le paramètre Len.
 Exemple :
 USART_FSK_Print("1234",4);
***************************************************************************************************/
void USART_FSK_Print(char* Mssg, int Len);

/**************************************************************************************************
 Fixe le module FSK en position de réception
***************************************************************************************************/
void USART_FSK_SetReceiveAntenna(void);

/**************************************************************************************************
 Fixe le module FSK en position d'émission
***************************************************************************************************/
void USART_FSK_SetTransmAntenna(void);


#endif
