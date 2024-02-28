/*
 * RmDv_TelecoIR.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */

#include <RmDv_SGw_Protocol.h>
#include "Timer_L031.h"

#ifndef INC_RMDV_TELECOIR_H_
#define INC_RMDV_TELECOIR_H_




#define RmDv_TelecoIR_Timer_PWM TIM2
#define RmDv_TelecoIR_Timer_Bit TIM21
#define RmDv_TelecoIR_Prio_IT_Bit 1

void RmDv_TelecoIR_Init(void);
void RmDv_TelecoIR_DeInit(void);
void RmDv_TelecoIR_SetCmde(RmDv_TelecoIR_Cmde Cmde);




#define RmDv_LED_On LL_GPIO_SetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)
#define RmDv_LED_Off LL_GPIO_ResetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)




#endif /* INC_RMDV_TELECOIR_H_ */
