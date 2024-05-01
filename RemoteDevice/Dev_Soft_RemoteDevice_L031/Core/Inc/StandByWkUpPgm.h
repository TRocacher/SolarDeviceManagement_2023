/* =================================================================================
* ==================   Main_StandByWkUpPgm.h     ===================================
* =================================================================================*/

#include <GLOBAL_RmDv.h>
#include <FSKStack.h>
#include <RmDv_SGw_Protocol.h>
#include "RmDv_ADT7410.h"
#include "RmDv_Boost.h"
#include "RmDv_IO.h"
#include "RmDv_TelecoIR.h"


#ifndef INC_STANDBYWKUPPGM_H_
#define INC_STANDBYWKUPPGM_H_



/* Donne l'état courant de la phase de réveil
 * Utilisé si WDog pour identifier le lieu du plantage*/
/*!!** New 13/04/24 ** !!*/
typedef enum {
	RmDv_SM_OK=0,
	BoostActivation=1,
	TemperatureMeasure=2,
	WakeUpMssgToUC=3,
	ClimUpdate=4,
	RTCAdjust=5,
	WarningMssg=6,
}RmDv_WkUp_CurrentState;

RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void);
void Main_StandByWkUpPgm(void);
void FactoryReset_StandByWkUpPgm(void);


void DevPgmWup(void);

#endif /* INC_STANDBYWKUPPGM_H_ */
