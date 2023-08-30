/*
 * RmDv_ErrorWatchDog.h
 *
 *  Created on: 28 août 2023
 *      Author: trocache
 */

#ifndef INC_RMDV_ERRORWDG_H_
#define INC_RMDV_ERRORWDG_H_

#include "stm32l0xx.h"

typedef enum {
	BoostActivation=0,
	TemperatureMeasure=1,
	WakeUpMssgToUC=2,
	ClimUpdate=3,
	RTCAdjust=4,
	WarningMssg=5,
}RmDv_WkUp_CurrentState;



#define Prio_LPTIM 0
void RmDv_ErrorWDG_LPTIMConf(int TimeOut_sec,char Prio, void (*IT_function) (void));

#define StartLPTM (LPTIM1->CR|=LPTIM_CR_SNGSTRT) /* démarrage LPtimer en continu*/
#define StartLPTMOneShot (LPTIM1->CR|=LPTIM_CR_SNGSTRT) /* démarrage LPtimer en one shot*/

#endif /* INC_RMDV_ERRORWDG_H_ */
