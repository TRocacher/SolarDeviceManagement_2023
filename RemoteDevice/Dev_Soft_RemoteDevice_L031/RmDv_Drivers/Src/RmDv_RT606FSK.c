/*
 * RmDv_RT606FSK.c
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 *
 *  -> Configure l'UART 2 (les IO sont réglées dans RmDv_IO
 *
 *  -> Réception sur IT, remplissage automatique d'un tableau suudimensionné à 55.
 *  Principe : Un flag passe à 1 si:
 *  - 50 caractères sont reçus : le dernier prend alors la valeur Null
 *  - le caractère Ox0D (CR) est détecté : il prend la valeur Null
 *
 *
 *  Recevoir une chaîne de caractères :
 *  -> polling :
 *  while(RmDv_RT606FSK_IsStrReceived()==0);
 *  PtrChar=RmDv_RT606FSK_GetString();
 *
 *
 *
 */

#include "main.h"

/*======================================================
    Variables du module
 ======================================================*/

#define L_Str 50
uint8_t UART_ReceptString[L_Str]; // String qui se remplit automatiquement sur IT char
uint8_t FinalReceptString[L_Str];
int Indice;				  // Repère du caractère courant dans la chaîne
char Flag_RecStr;		  // Flag signalant l'arrivée d'un string (/CR trouvé ou longueur
						  // ayant atteint 50.





/*======================================================
    User Functions
 ======================================================*/

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType =LL_GPIO_OUTPUT_OPENDRAIN;                  //!!!! // LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, 0);
  NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE BEGIN USART2_Init 1 */
  LL_USART_EnableIT_RXNE(USART2);
  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}




void RmDv_RT606FSK_Init(void)
{

	MX_USART2_UART_Init();


//  LL_USART_InitTypeDef USART_InitStruct = {0};
//  //Peripheral clock enable
//  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
//
//  // GPIO réglée dans RmDv_IO.c
//
//
//  USART_InitStruct.BaudRate = 9600;
//  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
//  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
//  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
//  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
//  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
//  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
//  LL_USART_Init(USART2, &USART_InitStruct);
//  LL_USART_ConfigAsyncMode(USART2);
//  LL_USART_Enable(USART2);
//
//  //Flag_RecStr=0; 	// reset du flag de réception
//
//  // USART2 interrupt Init
//  NVIC_SetPriority(USART2_IRQn, 0);
//  NVIC_EnableIRQ(USART2_IRQn);
//  LL_USART_EnableIT_RXNE(USART2);

}

//____________________________________________________________________________________
//

char RmDv_RT606FSK_IsStrReceived(void)
{
	return Flag_RecStr;
}
//____________________________________________________________________________________
//
uint8_t * RmDv_RT606FSK_GetString(void)
{
	Flag_RecStr=0; 	// reset du flag de réception
	return FinalReceptString;
}
//____________________________________________________________________________________
//

void RmDv_RT606FSK_SendString(uint8_t * PtrChar, int Longueur)
{
for(int i=0;i<Longueur;i++)
	{
	while (LL_USART_IsActiveFlag_TXE(USART2)==0);
	LL_USART_TransmitData8(USART2, *PtrChar);
	PtrChar++;
	}
}

//____________________________________________________________________________________
//

void RmDv_RT606FSK_EnableTx(void)
{
	LL_GPIO_ResetOutputPin(RxCmde_GPIO_Port, RxCmde_Pin);
	LL_GPIO_SetOutputPin(TxCmde_GPIO_Port, TxCmde_Pin);
}

//____________________________________________________________________________________
//

void RmDv_RT606FSK_EnableRx(void)
{
	LL_GPIO_ResetOutputPin(TxCmde_GPIO_Port, TxCmde_Pin);
	LL_GPIO_SetOutputPin(RxCmde_GPIO_Port, RxCmde_Pin);
}

//____________________________________________________________________________________
//

void RmDv_RT606FSK_DisableRxTx(void)
{
	LL_GPIO_ResetOutputPin(TxCmde_GPIO_Port, TxCmde_Pin);
	LL_GPIO_ResetOutputPin(RxCmde_GPIO_Port, RxCmde_Pin);
}


/*======================================================
    Interrupt Functions
 ======================================================*/
char ReceptString [50];
void USART2_IRQHandler(void)
{

	if ((LL_USART_ReceiveData8(USART2)==0x0D)|| (Indice==50))
	{
		Flag_RecStr=1;
		ReceptString[Indice]=0; // null à la fin
		Indice = 0;
	}
	else
	{
		ReceptString[Indice]=LL_USART_ReceiveData8(USART2);
		Indice++;
	}
}






//char Caractere;
//void USART2_IRQHandler(void)
//{
//
//	Caractere = LL_USART_ReceiveData8(USART2);
//	if ((Caractere==0x0D)|| (Indice==L_Str-1))
//	{
//		// remplissage du tableau final de 0 à l'indice d'arrêt
//		for (int i=0; i<=Indice ;i++)
//		{
//			FinalReceptString[i]=UART_ReceptString[i];
//		}
//		for (int i=Indice; i<=(L_Str-1) ;i++)
//		{
//			FinalReceptString[i]=0;
//		}
//
//		Flag_RecStr=1;
//		Indice = 0;     // prêt pour nouvelle acquisition
//
//	}
//	else
//	{
//		UART_ReceptString[Indice]=LL_USART_ReceiveData8(USART2);
//		Indice++;
//	}
//}

