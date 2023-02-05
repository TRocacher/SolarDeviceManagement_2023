#ifndef _UART_FSK_H__
#define _UART_FSK_H__


#include "stm32f10x.h"

// Choix de l'UART
#define UART_FSK USART3

// Autorisation UART IT seulement si CD est d�tect�.
#define UseCarrierDetect  


// Carrier Detect !! ATTENTION !! Pgm pr�vu seulement pour broche 5 � 9. En dehors, revoir le .c
// Aux endroits rep�r�s //---- !!! valable 5 � 9, sinon modifier ici ----///
#define CD_GPIO GPIOB 
#define CD_Pin  7

#define Rx_Cmde_GPIO GPIOB 
#define Rx_Cmde_Pin  8
#define Tx_Cmde_GPIO GPIOB
#define Tx_Cmde_Pin  9


void USART_FSK_Init(int Baud_Rate_bits_par_Sec, char Prio_USART_CD, char Prio_USART, void (*IT_function) (void));
char USART_FSK_GetByte(void);
void USART_FSK_Print(char* Mssg, int Len);
void USART_FSK_SetReceiveAntenna(void);
void USART_FSK_SetTransmAntenna(void);


#endif


/* TEST Couche Sup OK

#include "UART_FSK_103.h"

char Test[5];
int i;

void UART_Callback(void)
{
	Test[i]=USART_FSK_GetByte();
	i++;
	if (i==5) USART_FSK_Print(Test,5);
		
}

int main (void)
{
	i=0;
	USART_FSK_Init(9600,0,UART_Callback);
	
	
	
	while(1);
}
*/
