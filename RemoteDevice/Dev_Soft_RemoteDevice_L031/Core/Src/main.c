/* Règle commentaires encadrés*/

/* Entête de fichier*/

/* =================================================================================
* ==================   Nom Fichier.c	     ===================================
 *
 *   Created on:
 *   Author:
 *   Tool :
 *   Target :
 *  ------------------------------------------------------------------------------
 *
 *
* =================================================================================*/


/* Entête de fonction*/
/*______________________________________________________________________________
*_______________________ Nom fonction	________________________________________
 *
 *   Rôle:
 *   Appelant :
 *   Appelés :
 *   Param :
 *
 *
 *
* __________________________________________________________________________________*/


/* Commentaire fonction */
/***************************************************************
		blabla
***************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "RmDv_ADT7410.h"
#include "RmDv_Boost.h"
#include "RmDv_IO.h"
#include "FctDiverses.h"


#include "RmDv_RT606FSK.h"
#include "RmDv_TelecoIR.h"



/* =================================================================================
* ==================    Main pgm	     ===========================================
  General Configurations (Clock, all io, I2C1, ADC1 ain6)
  Si Pwr reset ou BP reset
  	  FactoryReset
  Sinon (wkup RTC depuis Standby Mode)
  	  StandByWkUpPgm
  FinSi

* =================================================================================*/




/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);






int main(void)
{
/***************************************************************
		Configurations générales
***************************************************************/
  /* Init clock */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  SystemClock_Config();
  /* Conf all IO */
  RmDv_IO_Init();
  /* Alimentation 3,3V pour ADT7410, OPAmp µphone, OpAmp LED IR Xbee (si câblé)
   * Alimentation 5V pour RT606 (FSK, si câblé) */
  RmDv_EnableBoost;
  Delay_x_ms(10); /* attendre 10ms pour que le ADT7410 se réveille*/
  /* check I2C*/
  ADT7410_Init();




while(1)
{

}


}



/* =================================================================================
* ==================    Conf pgm	     ===========================================
*
 (Clock, all io, I2C1, ADC1 ain6)

* ===============================================================================*/

/***************************************************************
		Clock Sysclk = 24MHz
***************************************************************/
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
