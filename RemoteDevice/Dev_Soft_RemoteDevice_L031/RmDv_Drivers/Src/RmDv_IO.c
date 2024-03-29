/* ===============================================================================
 * ==================    RemDev_IO.c   ===========================================
 *
 *   Created on: Jul 16, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1, from CubeMx for initialization
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 * Configure TOUTES les IO du remote Device. La plupart sont nommée dans RmDv_IO.h
 * sauf celles de l'UART2 (PA2 et PA3)
 *      celles de l'I2C1 (PA9 et PA10)
 *      celle de l'ADC in6 (PA6)
 *
 * -> configure aussi l'entrée user BP en EXTI
 * -> contient le handler d'IT du BP
 *
 *
 * ===============================================================================*/

//!!!!!!!!!!!!!!!!!!!!!!!! mettre en OD le pilotage UART et TXcmde...
// après avoir bricolé la carte.


#include "RmDv_IO.h"
void RmDv_IO_Init(void)
{
/***************************************************************
	définitions des structures d'initialisation
***************************************************************/
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_EXTI_InitTypeDef EXTI_InitStruct= {0};

/***************************************************************
  	Configurations de toutes les IO
****************************************************************/
  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /**/
  /*disable boost (OD actif low)*/
  LL_GPIO_SetOutputPin(nBoost_En_GPIO_Port, nBoost_En_Pin);
  /*disable HF Transmission */
  LL_GPIO_ResetOutputPin(TxCmde_GPIO_Port, TxCmde_Pin);
  /*disable HF Reception */
  LL_GPIO_ResetOutputPin(RxCmde_GPIO_Port, RxCmde_Pin);
  /*disable IR LED */
  LL_GPIO_ResetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin);


  /* nBoost Output Open Drain (TC 3.3V) */
  GPIO_InitStruct.Pin = nBoost_En_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType =  LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(nBoost_En_GPIO_Port, &GPIO_InitStruct);

  /*Carrier Detecte CD  input floating (TC 3.3V protégé par R1 100k)  */
  GPIO_InitStruct.Pin = CD_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(CD_GPIO_Port, &GPIO_InitStruct);

  /* TxCmde Output PushPull (TC 3.3V) */
  GPIO_InitStruct.Pin = TxCmde_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(TxCmde_GPIO_Port, &GPIO_InitStruct);

  /*LED_IR Output PushPull (FT 5V)*/
  GPIO_InitStruct.Pin = LED_IR_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_IR_GPIO_Port, &GPIO_InitStruct);

  /* TxCmde Output PushPull (TC 3.3V) */
  GPIO_InitStruct.Pin = RxCmde_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(RxCmde_GPIO_Port, &GPIO_InitStruct);

  /* BPpin Input pull up (TC 3,3V)*/
  GPIO_InitStruct.Pin = User_BP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(User_BP_GPIO_Port, &GPIO_InitStruct);

  /* INT ADT7410 Input pull up  */
  GPIO_InitStruct.Pin = INT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);

  /* CT ADT7410 Input pull up  */
  GPIO_InitStruct.Pin = CT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(CT_GPIO_Port, &GPIO_InitStruct);


  /* NOUVEAU user BP input pull up + conf EXTI IT (TC 3,3V)*/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE5);
  LL_GPIO_SetPinPull(User_BP_GPIO_Port, User_BP_Pin, LL_GPIO_PULL_UP);
  LL_GPIO_SetPinMode(User_BP_GPIO_Port, User_BP_Pin, LL_GPIO_MODE_INPUT);
  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_5;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);
  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);



  /* UART 2  */
  /**USART2 GPIO Configuration
  //PA2   ------> USART2_TX  Alternate Output Ppull (TC 3,3V)
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN; /////////////////LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //PA3   <------ USART2_RX  Alternate input (FT 5V)
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*PA6   ------> ADC_IN6 (FT 5V) */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /**I2C1 GPIO Configuration
  PA9   ------> I2C1_SCL
  PA10   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}



/*======================================================
 * User BP Test variables / callback
 ======================================================*/
void EXTI4_15_IRQHandler(void)
{
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
  }

}

