/*
 * RmDv_TelecoIR.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */

#include <GLOBAL_RmDv.h>
#include "Timer_L031.h"

#ifndef INC_RMDV_TELECOIR_H_
#define INC_RMDV_TELECOIR_H_

/* Liste des code de télécommande */
typedef enum {
	_Chaud_18_VanBas_FanAuto = 0xC1,
	_Chaud_19_VanBas_FanAuto = 0xC2,
	_Chaud_20_VanBas_FanAuto = 0xC3,
	_Chaud_21_VanBas_FanAuto = 0xC4,
	_Chaud_22_VanBas_FanAuto = 0xC5,
	_Chaud_23_VanBas_FanAuto = 0xC6,
	_NoCommandToSend = 0xC7,
	_Stop = 0xC0,
}RmDv_TelecoIR_Cmde;


#define RmDv_TelecoIR_Timer_PWM Timer_Teleco_PWM
#define RmDv_TelecoIR_Timer_Bit Timer_Teleco_bit
#define RmDv_TelecoIR_Prio_IT_Bit prio_Teleco_Timer_Bit

void RmDv_TelecoIR_Init(void);
void RmDv_TelecoIR_DeInit(void);
void RmDv_TelecoIR_SetCmde(RmDv_TelecoIR_Cmde Cmde);




#define RmDv_LED_On LL_GPIO_SetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)
#define RmDv_LED_Off LL_GPIO_ResetOutputPin(LED_IR_GPIO_Port, LED_IR_Pin)




#endif /* INC_RMDV_TELECOIR_H_ */
