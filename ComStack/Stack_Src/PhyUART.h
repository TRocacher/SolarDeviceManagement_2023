#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "stm32f10x.h"


// longueur max des chaïnes
#define StringLenMax 30
#define StringLenMin 8

// les valeurs possibles de Status
typedef enum {
	Ready,
	Listening,
	ReceivingMssg,
	SendingMssg,
}PhyUART_StatusType;

// les valeurs possibles de Status
typedef enum {
	NoError,
	CheckSumError,
	LenError,
	TimeOutError,
	OverRunError
}PhyUART_ErrorType;




// FSM & Init fcts
void PhyUART_Init(void);
void PhyUART_StartFSM(void);
void PhyUART_FSM_Progress(void);

// IO Fcts
char PhyUART_IsNewMssg(void);
PhyUART_StatusType PhyUART_Get_Status(void);
PhyUART_ErrorType PhyUART_Get_Error(void);

int  PhyUART_GetNewMssg (char * AdrString, int Len); // Len est la longueur maximale du tableau de réception
void PhyUART_SendNewMssg (char * AdrString, int Len);
#endif

