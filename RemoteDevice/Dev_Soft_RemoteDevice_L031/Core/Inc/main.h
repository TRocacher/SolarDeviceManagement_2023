/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */



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






/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
