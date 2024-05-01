/* ==========================================================================================
* ==================   LowPower_031.c	     ===================================
 *
 *   Created on: 27/08/23/2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *   Dépendance :
 *  ------------------------------------------------------------------------------
 *
 *
 *
 *  			(fct RmDv_LowPower_RTC_Init(int WakeUpPeriodSec)
 *   _______________________________|__________________________________________
 *  /                                                                          \
 *                 ________________       ________________               _____
 *   LSI 38kHz--->| PredivAsy /128 |---> |PredivSync /297 |---> 1Hz --->| WUT | --> WUF
 *                |________________|     |________________|             |_____|   (StdByWakeup)
 *                                                                         ^
 *                                                                         |
 *                                                                         |
 *                                                                        Enable Timer
 *                                                            ( fct RmDv_LowPower_GoToStbySleep)
* ============================================================================================*/

#include "LowPower_L031.h"
#include <TimeManagement_RmDv.h>

void LowPower_L031_EnableBKP(void);
void LowPower_L031_DisableBKP(void);


/**
  * @brief  Configure la RTC, en particulier les prescaler pour avoir 1sec de cadence WUT
  * 		Sélectionne le 1Hz comme clk du WUT
  * @Note
  * @param
  * @retval
  **/
void LowPower_L031_RTC_Init(void)
{

 /* PREREQUIS
  On entre en supposant :
  LSI bien sélectionné en entrée RTC (fait dans SystemClock_Config)*/

  /* Validation Pwr domain*/
  RCC->APB1ENR|=RCC_APB1ENR_PWREN;
  LowPower_L031_EnableBKP();
  LL_RCC_EnableRTC();

 /* configuration pour  1sec*/
  SET_BIT(RTC->ISR, RTC_ISR_INIT);				/* entrée dans le processus d'initialisation Prescaler*/
  //while (LL_RTC_IsActiveFlag_INITS(RTC)==0);	/* attente flag confirmant l'entrée ds le processus */
  Delay_x_ms(2);
  LL_RTC_SetAsynchPrescaler(RTC, 127);
  LL_RTC_SetSynchPrescaler(RTC, 292); // normalement 296 Mais les chiffres montre un tps trop rapide
  	  	  	  	  	  	  	  	  	  // de 10% environ, donc on ralentit

  CLEAR_BIT(RTC->ISR, RTC_ISR_INIT);	/* sortie du mode init*/


  /* sélectionner l'horloge 1sec (CKSPRE) pour le WU timer*/
  LL_RTC_WAKEUP_Disable(RTC);  /*Désactiver WUT (notamment pour pouvoir écrire dans WUTR)*/
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

  LowPower_L031_DisableBKP();
}



void LowPower_L031_WUTConf(int WakeUpPeriodSec)
{
	LowPower_L031_EnableBKP();
	/*Désactiver WUT (notamment pour pouvoir écrire dans WUTR)*/
	LL_RTC_WAKEUP_Disable(RTC);

	/* Attendre l'autorisation d'écriture dans WUTR*/
 	while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
 	{
 	}

 	if (WakeUpPeriodSec==0) WakeUpPeriodSec=1; /* la valeur nulle mènerait à FFFF secondes
   	   	   	   	   	   	   	   	   	   	   	   	 on évite cet écueil avec ce if...*/
 	LL_RTC_WAKEUP_SetAutoReload(RTC, WakeUpPeriodSec-1);
  	/* Autoriser le flag de sortie WUTF (WUTIE=1) */
 	LL_RTC_EnableIT_WUT(RTC);

 	/* A CE STADE LE WUT EST CONFIGURE, AUTORELOAD CHARGE. MANQUE PLUS QU'A LANCER WUT
 	*     (LL_RTC_WAKEUP_Enable(RTC);)*/
 	/* Verrouiller backup domain*/
 	LowPower_L031_DisableBKP();
}





void LowPower_L031_GoToStdbySleep(void)
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
	SCB->SCR|=SCB_SCR_SLEEPDEEP_Msk;

	// Activer_ le bit PDDS(PWR -> CR) pour être en standby mode qd deepsleep
	LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

	// Effacer WUF dans PWR->CSR
	LL_PWR_ClearFlag_WU();


	LowPower_L031_EnableBKP();
	/* Reset Internal Wake up flag */
	LL_RTC_ClearFlag_WUT(RTC); // codage un peu curieux...
	// lancement WUT
	LL_RTC_WAKEUP_Enable(RTC);

	LowPower_L031_DisableBKP();

	/* Request Wait For Interrupt */
	__WFI();

}


void LowPower_L031_EnableBKP(void)
{
	// levée de la protection après syst reset (DBP=1)
	LL_PWR_EnableBkUpAccess();
	//déverrouiller après backup domain reset
	LL_RTC_DisableWriteProtection(RTC);  /* WPR = 0xCA puis 0x53*/
}

void LowPower_L031_DisableBKP(void)
{
	// Verrouiller backup domain
	// levée de la protection après syst reset (DBP=1)
	LL_PWR_DisableBkUpAccess();
	//déverrouiller après backup domain reset
	LL_RTC_EnableWriteProtection(RTC);
}



int LowPower_L031_Is_WkupStdBy_NotPinRst(void)
{
	int WkupStdBy;
	/* test flag SBF*/
	if 	(((PWR->CSR&PWR_CSR_SBF)==PWR_CSR_SBF) && ((RCC->CSR&RCC_CSR_PINRSTF)==0))
	{
		/* wkup from stdby*/
		PWR->CR|=PWR_CR_CSBF;	 /* Effacement du flag indiquant que l'état précédent étair StdBy*/
		WkupStdBy = 1;
	}
	else
	{
		/*wkup from bp rst*/
		RCC->CSR|=RCC_CSR_RMVF; /* PINRST repasse à 0 pour pouvoir retomber dans MainTask au prochaine débordement RTC */
		WkupStdBy = 0;
	}

	return WkupStdBy;

}
