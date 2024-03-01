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


void InfiniteLoop(void)
{
	while(1); // si vous êtes là c'est qu'il y a un pb d'init callback UART
}



	//******************************************
	// Variables globales
	//******************************************


void (* UART1_ByteCallback)(void)=InfiniteLoop;
void (* UART2_ByteCallback)(void)=InfiniteLoop;
void (* UART3_ByteCallback)(void)=InfiniteLoop;



/******************************************************************************************************************
	USART_Init

Rôle :
Param : 
*****************************************************************************************************************/
void USART_Init(USART_TypeDef * USART,  int Baud_Rate_bits_par_Sec, char Prio_USART, void (*IT_function) (void))
{
		//******************************************
		// Variables locales
		//******************************************
unsigned int Frequence_Ck_USART_Hz;	
int USART_Div;
int Mantisse,Fract;

	
	//******************************************
	// UART 1 , confi IO, Conf IT locale
	//******************************************
if (USART==USART1)  
{
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_USART1EN;
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPAEN;
	// USART_1_TX = TIM1_CH2 = PA9 (Alt Output Ppull 2MHz)
	// USART_1_RX = TIM1_CH3 = PA10 (input floating)
	GPIOA->CRH&=~(0xF<<(9-8)*4);
	GPIOA->CRH|=(0xA<<(9-8)*4);
	GPIOA->CRH&=~(0xF<<(10-8)*4);
	GPIOA->CRH|=(0x4<<(10-8)*4);	
  NVIC_SetPriority(USART1_IRQn, Prio_USART);
	NVIC_EnableIRQ(USART1_IRQn);
	Frequence_Ck_USART_Hz=72000000;
	UART1_ByteCallback=IT_function;

}
	//******************************************
	// UART 2 , confi IO, Conf IT locale
	//******************************************
if (USART==USART2)
{
  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_USART2EN;
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPAEN;
	// USART_2_TX = TIM2_CH3 = PA2 (Alt Output Ppull 2MHz)
	// USART_2_RX = TIM2_CH4 = PA3 (input floating)
	GPIOA->CRL&=~(0xF<<(2)*4);
	GPIOA->CRL|=(0xA<<(2)*4);
	GPIOA->CRL&=~(0xF<<(3)*4);
	GPIOA->CRL|=(0x4<<(3)*4);	
  NVIC_SetPriority(USART2_IRQn, Prio_USART);
	NVIC_EnableIRQ(USART2_IRQn);
	Frequence_Ck_USART_Hz=36000000;
	UART2_ByteCallback=IT_function;
}
	//******************************************
	// UART 3 , confi IO, Conf IT locale
	//******************************************
if (USART==USART3)  
{
	(RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_USART3EN;
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPBEN;
	// USART_3_TX = PB10 (Alt Output Ppull 2MHz)
	// USART_3_RX = PB11 (input floating)
	GPIOB->CRH&=~(0xF<<(10-8)*4);
	GPIOB->CRH|=(0xA<<(10-8)*4);
	GPIOB->CRH&=~(0xF<<(11-8)*4);
	GPIOB->CRH|=(0x4<<(11-8)*4);	
  NVIC_SetPriority(USART3_IRQn, Prio_USART);
	NVIC_EnableIRQ(USART3_IRQn);
	Frequence_Ck_USART_Hz=36000000;
	UART3_ByteCallback=IT_function;
}	

	//******************************************
	// UART x , conf UART x, conf NVIC
	//******************************************
// Détermination du Baud Rate:
USART_Div=(Frequence_Ck_USART_Hz)/(Baud_Rate_bits_par_Sec); 
//  USART_Div est en format 28.4
Mantisse = USART_Div>>4;
Fract=USART_Div&0x0000000F; 
// on ne garde que les 4 bits fract de poids fort

// configuration émission
USART->CR1=(USART->CR1)|USART_CR1_UE; // UART On
USART->CR2=(USART->CR1)&~(0x1<<12); // 8 bits de data !!!!!!!!!!!! to check
USART->CR2=(USART->CR2)&~(0x3<<12); // 1 bit de stop
USART->BRR=(Mantisse<<4)+Fract; // Baud Rate
USART->CR1=(USART->CR1)|USART_CR1_TE; // Transmit Enable

//Configuration réception
USART->CR1=(USART->CR1)|USART_CR1_RE; // Transmit Enable
USART->CR1=(USART->CR1)|USART_CR1_RXNEIE; // validation IT locale en réception

}




/******************************************************************************************************************
	getter

Rôle :
Param : 
*****************************************************************************************************************/
char USART_GetByte(USART_TypeDef * USART)
{
	return USART->DR;
}



/******************************************************************************************************************
	USART_FSK_Print

Rôle :
Param : 
*****************************************************************************************************************/

void USART_Print(USART_TypeDef * USART,char* Mssg, int Len)
{
	int i;
	
	for(i=0;i<Len;i++)
	{
			USART->DR = *Mssg;
			while ((( USART->SR)& USART_SR_TXE)==0);
			Mssg++;
	}
	// attendre que le dernier octet soit parti...
	while((USART->SR&USART_SR_TC)!=USART_SR_TC);
}
	


/******************************************************************************************************************
	Les handlers d'IT

Rôle :
Param : 
*******************************************************************************************************************/

void USART1_IRQHandler(void)
{
	/* MODIF RECENTE 01/03/2024 pour être certain de ne pas rester en boucle sur l'UART
	Effacement FlagStatus RXNE */
	USART1->SR&=~USART_SR_RXNE;
	UART1_ByteCallback();
}

void USART2_IRQHandler(void)
{
	/* MODIF RECENTE 01/03/2024 pour être certain de ne pas rester en boucle sur l'UART
	Effacement FlagStatus RXNE */
	USART2->SR&=~USART_SR_RXNE;
	UART2_ByteCallback();
}

void USART3_IRQHandler(void)
{
		/* MODIF RECENTE 01/03/2024 pour être certain de ne pas rester en boucle sur l'UART
	Effacement FlagStatus RXNE */
	USART3->SR&=~USART_SR_RXNE;
	UART3_ByteCallback();
}
