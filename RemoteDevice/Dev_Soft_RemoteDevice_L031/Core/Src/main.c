

/* =================================================================================
* ==================   Nom Fichier.c	     ===================================
 *
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
* =================================================================================*/



/* Includes ------------------------------------------------------------------*/
#include <FSKStack.h>
#include <RmDv_SGw_Protocol.h>
#include <RmDv_WDG_LPTM.h>
#include "main.h"
#include "StandByWkupPgm.h"
#include "RmDv_IO.h"
#include "LowPower_L031.h"
#include <GLOBAL_RmDv.h>


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void BP_User_Callback(void);
void LPTIM1_User_Callback(void);


#define PlantageTimeOut 2

int DelayNextWup_sec;
int main(void)
{


/***************************************************************
		Configurations générales
***************************************************************/
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	SystemClock_Config();
	LowPower_L031_RTC_Init();

/***************************************************************
		Réglage du LPTIM1 pour watchdog en cas de plantage
		Rem : possibilité, à des fins de débug, de configurer
		LPTIM1 en IT périodique pour travailler en IT sans sleep.
***************************************************************/
	/* Réglage durée watchgog*/
	RmDv_ErrorWDG_LPTIMConf(PlantageTimeOut,prio_WDG, LPTIM1_User_Callback);
	// !!! revoir la prio si utilisé en watchdog. En IT simple, prio à 2 pour pas bloquer
	// le systick
	//	StartLPTM;
	//<<<<<>>>>/StartLPTMOneShot; /* Démarrage Timing Wdog LPTIM1*/


/***************************************************************
	  	Configurations I/O Remote Device
***************************************************************/
	RmDv_IO_Init();
	USART_FSK_RT606_OFF();
	RmDv_IO_AssociateFct_UserBP(BP_User_Callback);


/***************************************************************
	  		Run code Standby
***************************************************************/
	Main_StandByWkUpPgm();
	//<<<<<>>>>/
	while(1);/////////////////

	/* Lancement WUT et sleep */
	LowPower_L031_GoToStdbySleep();

  while(1)
  {

  }

}



void BP_User_Callback(void)
{

}




void  LPTIM1_User_Callback(void)
{
	RmDv_WkUp_CurrentState LocalState;

	/* Récupération de l'état de la SM */
	LocalState = StandByWkUpPgm_GetCurrentState();
	/* Donner accès au BKP reg */
	LL_PWR_EnableBkUpAccess();
	LL_RTC_DisableWriteProtection(RTC);
	/* Ecriture */
	LL_RTC_WriteReg(RTC,BKPReg_RmDv_State,LocalState);
	/* Blocage accès BKP Reg */
	LL_PWR_DisableBkUpAccess();
	LL_RTC_EnableWriteProtection(RTC);


	/* Réglage Période RTC*/
	LowPower_L031_WUTConf(30*60); /* sleep pour 30mn*/

	LowPower_L031_GoToStdbySleep(); /* Lancement WUT et sleep */


}













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

  //LL_Init1msTick(24000000);       // probablement inutile, à virer ! utilise ll_utils

  //LL_SetSystemCoreClock(24000000);
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
