#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "stm32f10x.h"



void PhyUART_Init(void);
void PhyUART_StartFSM(void);
void PhyUART_FSM_Progress(void);

#endif

