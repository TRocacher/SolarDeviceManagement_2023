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

void LowPower_L031_RTC_Init(int WakeUpPeriodSec)
{

 /* PREREQUIS
  On entre en supposant :
  LSI bien sélectionné en entrée RTC (fait dans SystemClock_Config)
  */

  /*
  OBJECTIF :
   Configurer l'ensemble du WUT (prescaler, etc...)
   A la fin, le WUT est disable.
   Il ne sera validé que lors de la mise en sommeil

   Au démarrage et à la fin on permet puis interdit l'accès (DBP - system reset et 0xCA pui 0x53 - bkp domain reset)
   */
  // Validation Pwr domain
  RCC->APB1ENR|=RCC_APB1ENR_PWREN;

  // levée de la protection après syst reset (DBP=1)
  LL_PWR_EnableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RCC_EnableRTC();
  LL_RTC_DisableWriteProtection(RTC);


  //Désactiver WUT (notamment pour pouvoir écrire dans WUTR)
  LL_RTC_WAKEUP_Disable(RTC);


  /* réglage des prescalers */
  /* LSI -> /(AsyncPre +1)*(SyncPre+1)
   * LSI à 38kHz (Min 26kHz, Max 56kHz)
   * Dérive -10% à +4% en fonction de la température
   *
   * Hypothèse 38kHz -> /(127+1) donne 296.8Hz
   * pour obtenir 1Hz, il faut encore diviser par 296.8 soit 297, soit 296+1*/
  LL_RTC_SetAsynchPrescaler(RTC,127); //1 seconde
  LL_RTC_SetSynchPrescaler(RTC, 296);
  // sélectionner l'horloge RTC (par exemple LSI Valeur 37KHz)
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
  // Attendre l'autorisation d'écriture dans WUTR
  while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
  {
  }
  if (WakeUpPeriodSec==0) WakeUpPeriodSec = 1; /* la valeur nulle mènerai à FFFF secondes
   	   	   	   	   	   	   	   	   	   	   	   	 on évite cet écueil avec ce if...*/
  LL_RTC_WAKEUP_SetAutoReload(RTC, (WakeUpPeriodSec-1)); /* Aurotereload ts les WUT+1*/
  // Autoriser le flag de sortie WUTF (WUTIE=1)
  LL_RTC_EnableIT_WUT(RTC);

  // A CE STADE LE WUT EST CONFIGURE, AUTORELOAD CHARGE. MANQUE PLUS QU'A LANCER WUT (LL_RTC_WAKEUP_Enable(RTC);)

  // Verrouiller backup domain
  // levée de la protection après syst reset (DBP=1)
  LL_PWR_DisableBkUpAccess();
  //déverrouiller après backup domain reset
  LL_RTC_EnableWriteProtection(RTC);
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
