
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * Architecture logicielle
  *
  * main.h : toutes les définitions des IO
  * RmDv_IO.c .h : toutes les initialisations des IO. NB Boost disable à l'init.
  *
  ******************************************************************************
  */




/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RmDv_IO.h"
#include "RmDv_RT606FSK.h"
#include "RmDv_Boost.h"
#include "RmDv_TelecoIR.h"

#define RTC_WUT_TIME 10 // timing wakeup



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void My_RTC_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC_Init(void);

void My_EnterStandbyMode(void);



/*======================================================
 * Define pour les divers tests
 ======================================================*/
//#define UART_Emission // émet en boucle à 38400 bds Phrase[]. Besoin d'une carte telecom 103 pour test
#define UART_Reception  // 9600 bds Boucle infinie : attend /CR ou 50 caractères reçu dont /CR dans ReceptString[55];
					    // test avec module PC
//#define Test_LED_IR



/*======================================================
 * USART2 Test variables / callback
 ======================================================*/
// émission
uint8_t Phrase[]="Coucou !  ";
uint8_t * PtrChar;




/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Init clock */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  SystemClock_Config();


  /* Initialize all configured peripherals */
  RmDv_IO_Init();
  RmDv_RT606FSK_Init();


  My_RTC_Init();
  MX_I2C1_Init();
  MX_ADC_Init();




#ifdef Test_LED_IR
  /*======================================================
   * LED IR
   ======================================================*/


RmDv_EnableBoost;

while(1)
{
	RmDv_LED_On;
	LL_mDelay(100);
	RmDv_LED_Off;
	LL_mDelay(900);
}

#endif




  /*======================================================
   * USART2 Test and Boost
   ======================================================*/

#ifdef UART_Emission
  RmDv_EnableBoost;
  // attendre 0.1 seconde pour établissement boost
  LL_mDelay(100);
  RmDv_RT606FSK_EnableTx();
  while(1)
  {
	  RmDv_RT606FSK_SendString(Phrase,9);
  }
  RmDv_RT606FSK_DisableRxTx();

#endif

#ifdef UART_Reception

  //enable boost
  LL_GPIO_ResetOutputPin(nBoost_En_GPIO_Port, nBoost_En_Pin);
  // attendre 0.1 seconde pour établissement boost
  LL_mDelay(100);
  RmDv_RT606FSK_EnableRx();

  while(1)
  {
  while(RmDv_RT606FSK_IsStrReceived()==0);
  PtrChar=RmDv_RT606FSK_GetString();
  }
  RmDv_RT606FSK_DisableRxTx();




#endif

while(1)
{

}

  /* Gestion des modes */

  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);

  if ( ((RCC->CSR&RCC_CSR_PINRSTF) == RCC_CSR_PINRSTF) && ((PWR->CSR&PWR_CSR_SBF) != PWR_CSR_SBF))
  {
	  // reset reset flag
	  RCC->CSR|=RCC_CSR_RMVF;
	  // Allumer LED durant 6s
	  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	  LL_mDelay(6000);
  }
  else if ( ((RCC->CSR&RCC_CSR_PINRSTF) == RCC_CSR_PINRSTF) && ((PWR->CSR&PWR_CSR_SBF) == PWR_CSR_SBF))
   // nRST from stdby mode PINRST =1, SBF=1
  {
	  // reset reset flag
	  RCC->CSR|=RCC_CSR_RMVF;
	  // clear SBF
	  PWR->CR|=PWR_CR_CSBF;
	  // Allumer LED durant 3s
	  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	  LL_mDelay(3000);
  }

  else // retour d'un standby mode par le WUTF  // PINRST =0, SBF=1
  // Allumer LED durant 1s
  {
	  // clear SBF
	  PWR->CR|=PWR_CR_CSBF;
	  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	  LL_mDelay(1000);
  }

  My_EnterStandbyMode();

	  // bug entrée Standby LED on
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);


  while (1)
  {

  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_PWR_EnableBkUpAccess();
  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  }
  LL_RCC_EnableRTC();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_3, LL_RCC_PLL_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }

  LL_Init1msTick(24000000);

  LL_SetSystemCoreClock(24000000);
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void My_RTC_Init(void)
{
  /* PREREQUIS
  On entre en supposant :
  PWR enable (main)
  LSI bien sélectionné en entrée RTC (fait dans SystemClock_Config)
  */

  /*
  OBJECTIF :
   Configurer l'ensemble du WUT (prescaler, etc...)
   A la fin, le WUT est disable.
   Il ne sera validé que lors de la mise en sommeil

   Au démarrage et à la fin on permet puis interdit l'accès (DBP - system reset et 0xCA pui 0x53 - bkp domain reset)
   */


  // levée de la protection après syst reset (DBP=1)
  LL_PWR_EnableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RCC_EnableRTC();
  LL_RTC_DisableWriteProtection(RTC);


  //Désactiver WUT (notamment pour pouvoir écrire dans WUTR)
  LL_RTC_WAKEUP_Disable(RTC);


  // réglage des prescalers
  LL_RTC_SetAsynchPrescaler(RTC, 127);
  LL_RTC_SetSynchPrescaler(RTC, 296);
  // sélectionner l'horloge RTC (par exemple LSI Valeur 37KHz)
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
  // Attendre l'autorisation d'écriture dans WUTR
  while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
  {
  }
  LL_RTC_WAKEUP_SetAutoReload(RTC, RTC_WUT_TIME);
  // Autoriser le flag de sortie WUTF (WUTIE=1)
  LL_RTC_EnableIT_WUT(RTC);

  // A CE STADE LE WUT EST CONFIGURE, AUTORELOAD CHARGE. MANQUE PLUS QU'A LANCER WUT (LL_RTC_WAKEUP_Enable(RTC);)

  // Verrouiller backup domain
  // levée de la protection après syst reset (DBP=1)
  LL_PWR_DisableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RTC_EnableWriteProtection(RTC);
}

void My_EnterStandbyMode(void)
{
	  /* PREREQUIS
	  On entre en supposant :
	  PWR enable (fait dans main)
	  RTC enable ( fais dans My_RTC_Init)
	  */


	  /*
	  OBJECTIF :
	  Enchainer les actions pour mise en stby
	  -SLEEPDEEP core =1 , PDDS =1 (standby et pas stop)
	  - mettre WUF à 0 (flag du PWR CSR)
	  - Effacer le flag WUT
	  - Lancer le WUT
	  - WFI

	   Au démarrage et à la fin on permet puis interdit l'accès (DBP - system reset et 0xCA pui 0x53 - bkp domain reset)
	   */


  //activer le bit SLEEPDEEP (SCB → SCR),
  LL_LPM_EnableDeepSleep();
  // Activer le bit PDDS(PWR -> CR) pour être en standby mode qd deepsleep
  LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

  // Effacer WUF dans PWR->CSR
  LL_PWR_ClearFlag_WU();


  // levée de la protection après syst reset (DBP=1)
  LL_PWR_EnableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RTC_DisableWriteProtection(RTC);
  /* Reset Internal Wake up flag */
  LL_RTC_ClearFlag_WUT(RTC); // codage un peu curieux...
  // lancement WUT
  LL_RTC_WAKEUP_Enable(RTC);
  // Verrouiller backup domain
  // levée de la protection après syst reset (DBP=1)
  LL_PWR_DisableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RTC_EnableWriteProtection(RTC);


  /* Request Wait For Interrupt */
  __WFI();

}







/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_InitTypeDef ADC_InitStruct = {0};



  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  //ADC GPIO Configuration voir RmDv_IO.c





  // Configure Regular Channel

  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_6);
  /** Common config  */
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
  LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
  LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
  LL_ADC_DisableIT_EOC(ADC1);
  LL_ADC_DisableIT_EOS(ADC1);
  ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);

  /* Enable ADC internal voltage regulator */
  LL_ADC_EnableInternalRegulator(ADC1);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  uint32_t wait_loop_index;
  wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}


static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};



  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /**I2C1 GPIO Configuration voir RmDv_IO.c
  PA9   ------> I2C1_SCL
  PA10   ------> I2C1_SDA
  */


  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x00506682;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
