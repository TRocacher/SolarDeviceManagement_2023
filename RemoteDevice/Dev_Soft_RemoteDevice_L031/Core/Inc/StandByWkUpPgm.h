/* =================================================================================
* ==================   Main_StandByWkUpPgm.h     ===================================
* =================================================================================*/

#include "RmDv_ADT7410.h"
#include "RmDv_Boost.h"
#include "RmDv_IO.h"
#include "MACPhyUART.h"
#include "RmDv_TelecoIR.h"
#include "LowPower_L031.h"
#include "RmDv_ErrorWDG.h"
#include "ExchangeLayer.h"


#ifndef INC_STANDBYWKUPPGM_H_
#define INC_STANDBYWKUPPGM_H_

#define UC_Adress 0xBB /* @MAC de l'UC*/
#define My 0xA0

RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void);
void Main_StandByWkUpPgm(void);

#endif /* INC_STANDBYWKUPPGM_H_ */
