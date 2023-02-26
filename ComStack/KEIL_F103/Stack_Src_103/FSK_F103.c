#include "stm32f10x.h"
#include "FSK_F103.h"
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





	//******************************************
	// Variables globales
	//******************************************




/******************************************************************************************************************
	USART_FSK_Init

Rôle :
Param : 
*****************************************************************************************************************/
void USART_FSK_Init(int Baud_Rate_bits_par_Sec, char Prio_USART_CD, char Prio_USART, void (*IT_function) (void))
/*

*/

{
	
	//******************************************
	// Variables locales
	//******************************************
USART_Init(UART_FSK, Baud_Rate_bits_par_Sec, Prio_USART, IT_function); 
	
	
	//******************************************
	// Utlisation CD pour valider IT : config IO
	//******************************************
	
#ifdef UseCarrierDetect
	
if (CD_GPIO==GPIOA) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPAEN;
else if (CD_GPIO==GPIOB) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPBEN;	
else if (CD_GPIO==GPIOC) (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPCEN;
else (RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPDEN;

if (CD_Pin<8)
	{
	CD_GPIO->CRL&=~(0xF<<(CD_Pin%8)*4); // input floaing
	CD_GPIO->CRL|=(0x4<<(CD_Pin%8)*4);
	}
else
	{
	CD_GPIO->CRH&=~(0xF<<(CD_Pin%8)*4); // input floaing
	CD_GPIO->CRH|=(0x4<<(CD_Pin%8)*4);
	}
	
//---- !!! valable 5 à 9, sinon modifier ici ----///	
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[1]&=~(0xF<<12); //  GPIOx x 0 à 3-> EXTI0, 4 à 7 -> EXTI1 etc...
	AFIO->EXTICR[1]|=(0x1<<12);
	EXTI->RTSR|=0x1<<CD_Pin; // fronts up and down
	EXTI->FTSR|=0x1<<CD_Pin;
	EXTI->IMR|=0x1<<CD_Pin;  // validation locale
  NVIC_SetPriority(EXTI9_5_IRQn, Prio_USART_CD);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	
//---- FIN !!! valable 5 à 9, sinon modifier ici ----///	
	
#endif
	

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
	return USART_GetByte(UART_FSK);
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

void EXTI9_5_IRQHandler(void)
{
	EXTI->PR|=(1<<CD_Pin); // abaissement du flag de réception
	if ((CD_GPIO->IDR&(1<<CD_Pin))==1<<CD_Pin) // front montant
	{
		// neutraliser IT UART
		UART_FSK->CR1=(UART_FSK->CR1)&~USART_CR1_RE; // receive disable
		// USART->CR1=(USART->CR1)&~USART_CR1_RXNEIE;
		//NVIC_DisableIRQ(USART3_IRQn);
	}
	else 
	{
		UART_FSK->CR1=(UART_FSK->CR1)|USART_CR1_RE; // Receive Enable
		//USART->CR1=(USART->CR1)|USART_CR1_RXNEIE;
		//NVIC_EnableIRQ(USART3_IRQn);
	}
}
