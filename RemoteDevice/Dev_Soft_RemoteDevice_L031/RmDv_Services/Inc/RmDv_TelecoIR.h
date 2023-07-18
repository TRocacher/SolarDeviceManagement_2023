/*
 * RmDv_TelecoIR.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */

#include "main.h"

#ifndef INC_RMDV_TELECOIR_H_
#define INC_RMDV_TELECOIR_H_

#define RmDv_LED_On LL_GPIO_SetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)
#define RmDv_LED_Off LL_GPIO_ResetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)


#endif /* INC_RMDV_TELECOIR_H_ */
