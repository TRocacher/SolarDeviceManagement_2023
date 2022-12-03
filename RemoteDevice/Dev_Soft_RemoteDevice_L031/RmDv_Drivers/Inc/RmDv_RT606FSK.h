/*
 * RmDv_RT606FSK.h
 *
 *  Created on: Jul 3, 2022
 *      Author: trocache
 */

#ifndef INC_RMDV_RT606FSK_H_
#define INC_RMDV_RT606FSK_H_

void RmDv_RT606FSK_Init(void);

void RmDv_RT606FSK_EnableTx(void);
void RmDv_RT606FSK_EnableRx(void);
void RmDv_RT606FSK_DisableRxTx(void);


char RmDv_RT606FSK_IsStrReceived(void);
uint8_t * RmDv_RT606FSK_GetString(void);
void RmDv_RT606FSK_SendString(uint8_t * PtrChar, int Longueur);

#endif /* INC_RMDV_RT606FSK_H_ */
