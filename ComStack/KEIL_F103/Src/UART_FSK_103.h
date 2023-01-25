#ifndef _UART_FSK_H__
#define _UART_FSK_H__


#include "stm32f10x.h"

#define UART_FSK USART2
#define Rx_Cmde_GPIO GPIOA 
#define Rx_Cmde_Pin  4
#define Tx_Cmde_GPIO GPIOA
#define Tx_Cmde_Pin  0


void USART_FSK_Init(int Baud_Rate_bits_par_Sec,  char Prio_USART, void (*IT_function) (void));
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
