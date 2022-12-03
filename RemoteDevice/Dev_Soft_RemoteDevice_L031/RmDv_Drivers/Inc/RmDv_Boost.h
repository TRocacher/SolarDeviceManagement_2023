/*
 * MyBoost.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */

#ifndef INC_RMDV_BOOST_H_
#define INC_RMDV_BOOST_H_

#include "main.h"

#define RmDv_EnableBoost LL_GPIO_ResetOutputPin(nBoost_En_GPIO_Port, nBoost_En_Pin)
#define RmDv_DisableBoost LL_GPIO_SetOutputPin(nBoost_En_GPIO_Port, nBoost_En_Pin)

#endif /* INC_RMDV_BOOST_H_ */
