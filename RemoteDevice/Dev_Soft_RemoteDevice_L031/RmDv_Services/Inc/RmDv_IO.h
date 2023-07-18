/*
 * RemoteDevice_IO.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */
#include "main.h"
#ifndef INC_RMDV_IO_H_
#define INC_RMDV_IO_H_


/*
 *                  STM32L041F6P7 / STM32L031F6P7  / TSSOP 20
 *           _________________________________________
 * 			|                                         |
 *      -->-|Boot0                           PA14 SWCK|----
 *      --<-|Boost_En PC14 (OD)             PA13 SWDIO|----
 *      -->-|CD PC15                     (OD) PA10 SDA|-><-
 *      -->-|nRST                        (OD) PA9  SCL|->--
 *      ->>-|VDDA                                  VDD|-<<-
 *      --<-|TxCmde PA0 (OD)                       VSS|-<<-
 *      --<-|LED_IR PA1 (PPUL)                  PB1 CT|-<--
 *      --<-|Tx PA2 (OD)                       PA7 INT|-<--
 *      -->-|Rx PA3                (analog)PA6 ADC in6|-<--
 *      --<-|RxCmde PA4 (OD)       (pull up)PA5 UserBP|-<--
 *          |_________________________________________|
 *
 *
 *
 *
 *
 */

#define nBoost_En_Pin LL_GPIO_PIN_14
#define nBoost_En_GPIO_Port GPIOC
#define CD_Pin LL_GPIO_PIN_15
#define CD_GPIO_Port GPIOC
#define TxCmde_Pin LL_GPIO_PIN_0
#define TxCmde_GPIO_Port GPIOA
#define LED_IR_Pin LL_GPIO_PIN_1
#define LED_IR_GPIO_Port GPIOA
#define RxCmde_Pin LL_GPIO_PIN_4
#define RxCmde_GPIO_Port GPIOA
#define User_BP_Pin LL_GPIO_PIN_5
#define User_BP_GPIO_Port GPIOA
#define INT_Pin LL_GPIO_PIN_7
#define INT_GPIO_Port GPIOA
#define CT_Pin LL_GPIO_PIN_1
#define CT_GPIO_Port GPIOB


void RmDv_IO_Init(void);

#endif /* INC_RMDV_IO_H_ */
