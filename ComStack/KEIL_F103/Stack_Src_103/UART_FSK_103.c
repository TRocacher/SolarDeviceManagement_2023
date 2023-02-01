#include "stm32f10x.h"
#include "UART_FSK_103.h"

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

char UART_FSK_Byte;
void (* UART_FSK_ByteCallback)(void)=InfiniteLoop;







/******************************************************************************************************************
	USART_FSK_Init

Rôle :
Param : 
*****************************************************************************************************************/
void USART_FSK_Init(int Baud_Rate_bits_par_Sec,  char Prio_USART, void (*IT_function) (void))
/*

*/

{
	
	//******************************************
	// Variables locales
	//******************************************
unsigned int Frequence_Ck_USART_Hz;	
int USART_Div;
int Mantisse,Fract;
USART_TypeDef *USART;
USART=UART_FSK;
	
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

UART_FSK_ByteCallback=IT_function;

	//******************************************
	// Configuration RxCmde et TxCmde pin
	//******************************************
if ((Rx_Cmde_GPIO==GPIOA) || (Tx_Cmde_GPIO==GPIOA)) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPAEN;
else if ((Rx_Cmde_GPIO==GPIOB) || (Tx_Cmde_GPIO==GPIOB)) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPBEN;	
else if ((Rx_Cmde_GPIO==GPIOC) || (Tx_Cmde_GPIO==GPIOC)) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPCEN;
else (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPDEN;

if (Rx_Cmde_Pin<8)
	{
	Rx_Cmde_GPIO->CRL&=~(0xF<<(Rx_Cmde_Pin%8)*4); // output ppull 2MHz
	Rx_Cmde_GPIO->CRL|=(0x1<<(Rx_Cmde_Pin%8)*4);
	}
else
	{
	Rx_Cmde_GPIO->CRH&=~(0xF<<(Rx_Cmde_Pin%8)*4); // output ppull 2MHz
	Rx_Cmde_GPIO->CRH|=(0x1<<(Rx_Cmde_Pin%8)*4);
	}
	
if (Tx_Cmde_Pin<8)
	{
	Tx_Cmde_GPIO->CRL&=~(0xF<<(Tx_Cmde_Pin%8)*4); // output ppull 2MHz
	Tx_Cmde_GPIO->CRL|=(0x1<<(Tx_Cmde_Pin%8)*4);
	}
else
	{
	Tx_Cmde_GPIO->CRH&=~(0xF<<(Tx_Cmde_Pin%8)*4); // output ppull 2MHz
	Tx_Cmde_GPIO->CRH|=(0x1<<(Tx_Cmde_Pin%8)*4);
	}
	
	
	USART_FSK_SetReceiveAntenna();
}





/******************************************************************************************************************
	getter

Rôle :
Param : 
*****************************************************************************************************************/
char USART_FSK_GetByte(void)
{
	return UART_FSK_Byte;
}



/******************************************************************************************************************
	USART_FSK_Print

Rôle :
Param : 
*****************************************************************************************************************/

void USART_FSK_Print(char* Mssg, int Len)
{
	int i;
	
	for(i=0;i<Len;i++)
	{
			UART_FSK->DR = *Mssg;
			while (((UART_FSK->SR)& USART_SR_TXE)==0);
			Mssg++;
	}
	// attendre que le dernier octet soit parti...
	while((UART_FSK->SR&USART_SR_TC)!=USART_SR_TC);
}
	

/******************************************************************************************************************
	USART_FSK_SetReceiveAntena et USART_FSK_SetTransmAntena

Rôle :
Param : 
*****************************************************************************************************************/

void USART_FSK_SetReceiveAntenna(void)
{
	Rx_Cmde_GPIO->ODR|=1<<Rx_Cmde_Pin;
	Tx_Cmde_GPIO->ODR&=~(1<<Tx_Cmde_Pin);
}
void USART_FSK_SetTransmAntenna(void)
{
	Tx_Cmde_GPIO->ODR|=1<<Tx_Cmde_Pin;
	Rx_Cmde_GPIO->ODR&=~(1<<Rx_Cmde_Pin);
}




/******************************************************************************************************************
	Les handlers d'IT

Rôle :
Param : 
*****************************************************************************************************************/

void USART1_IRQHandler(void)
{
	UART_FSK_Byte=USART1->DR;
	UART_FSK_ByteCallback();
}

void USART2_IRQHandler(void)
{
	UART_FSK_Byte=USART2->DR;
	UART_FSK_ByteCallback();
}

void USART3_IRQHandler(void)
{
	UART_FSK_Byte=USART3->DR;
	UART_FSK_ByteCallback();
}
