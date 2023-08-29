/* ==========================================================================================
* ==================   LowPower_L031.h	     ===================================
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
 *   LSI 37kHz--->| PredivAsy /128 |---> |PredivSync /290 |---> 1Hz --->| WUT | --> WUF
 *                |________________|     |________________|             |_____|   (StdByWakeup)
 *                                                                         ^
 *                                                                         |
 *                                                                         |
 *                                                                        Enable Timer
 *                                                            ( fct RmDv_LowPower_GoToStbySleep)
* ============================================================================================*/


#ifndef INC_LOWPOWER_L031_H_
#define INC_LOWPOWER_L031_H_

#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_rcc.h"

void LowPower_L031_RTC_Init(int WakeUpPeriodSec);
void LowPower_L031_GoToStdbySleep(void);

#endif /* INC_RMDV_LOWPOWER_H_ */
