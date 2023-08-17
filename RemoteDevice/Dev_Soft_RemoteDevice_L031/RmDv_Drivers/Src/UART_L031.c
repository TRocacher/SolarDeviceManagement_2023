#include "UART_L031.h"




void InfiniteLoop(void)
{
	while(1); // si vous �tes l� c'est qu'il y a un pb d'init callback UART
}



	//******************************************
	// Variables globales
	//******************************************



void (* UART2_ByteCallback)(void)=InfiniteLoop;




/******************************************************************************************************************
	USART_Init

R�le :
Param : 
*****************************************************************************************************************/
void USART_Init(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*IT_function) (void))
{
		//******************************************
		// Variables locales
		//******************************************


	


	//******************************************
	// UART 2 , confi IO, Conf IT locale
	//******************************************

  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_USART2EN;
    NVIC_SetPriority(USART2_IRQn, Prio_USART);
	NVIC_EnableIRQ(USART2_IRQn);
	UART2_ByteCallback=IT_function;




	//******************************************
	// UART x , conf UART x, conf NVIC
	//******************************************


// configuration �mission
USART->CR1=(USART->CR1)|USART_CR1_UE; // UART On
USART->CR2=(USART->CR1)&~(0x1<<12); // 8 bits de data !!!!!!!!!!!! to check
USART->CR2=(USART->CR2)&~(0x3<<12); // 1 bit de stop
USART->BRR=(36000000/Baud_Rate_bits_par_Sec); // Baud Rate
USART->CR1=(USART->CR1)|USART_CR1_TE; // Transmit Enable

//Configuration r�ception
USART->CR1=(USART->CR1)|USART_CR1_RE; // Transmit Enable
USART->CR1=(USART->CR1)|USART_CR1_RXNEIE; // validation IT locale en r�ception

}




/******************************************************************************************************************
	getter

R�le :
Param : 
*****************************************************************************************************************/
char USART_GetByte(USART_TypeDef * USART)
{
	return USART->RDR;
}



/******************************************************************************************************************
	USART_FSK_Print

R�le :
Param : 
*****************************************************************************************************************/

void USART_Print(USART_TypeDef * USART,char* Mssg, int Len)
{
	int i;
	
	for(i=0;i<Len;i++)
	{
			USART->TDR = *Mssg;
			while ((( USART->ISR)& USART_ISR_TXE)==0);
			Mssg++;
	}
	// attendre que le dernier octet soit parti...
	while((USART->ISR&USART_ISR_TC)!=USART_ISR_TC);
}
	


/******************************************************************************************************************
	Les handlers d'IT

R�le :
Param : 
*******************************************************************************************************************/



void USART2_IRQHandler(void)
{
	UART2_ByteCallback();
}


