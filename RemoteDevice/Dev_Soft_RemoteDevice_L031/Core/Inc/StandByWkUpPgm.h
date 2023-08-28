/* =================================================================================
* ==================   Main_StandByWkUpPgm.h     ===================================
* =================================================================================*/

#include "RmDv_ADT7410.h"
#include "RmDv_Boost.h"
#include "RmDv_IO.h"
#include "MACPhyUART.h"
#include "RmDv_TelecoIR.h"
#include "LowPower_L031.h"

#ifndef INC_STANDBYWKUPPGM_H_
#define INC_STANDBYWKUPPGM_H_

#define UC_Adress 0xBB /* @MAC de l'UC*/

void Main_StandByWkUpPgm(void);

#endif /* INC_STANDBYWKUPPGM_H_ */
