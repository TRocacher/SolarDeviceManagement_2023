#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "stm32f10x.h"

// Priorit� d'interruption
#define UART_Prio_CD 0							// priorit� au niveau de l'UART , Carrier Detect (par d�faut priorit� maximale)	
#define UART_Prio (UART_Prio_CD+1) 	// priorit� au niveau de l'UART (juste au dessous de CD)
#define Phy_UART_FSM (UART_Prio+1)  // priorit� de la FSM (par d�faut juste en dessous de celle de l'UART)

// Baudrate
#define PhyUART_BdRate 38400

// longueur max des cha�nes
#define StringLenMax 30
#define StringLenMin 7


/*---------------------------------
 �tats possibles de la FSM
----------------------------------*/

typedef enum {
	Init,
	WaitForHeader,
	ReadingFrame,
	CheckSum,
	UpdateMssgForMAC,
	Framing,
	SendMssg
}PhyUART_FSM_StateType;


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

int  PhyUART_GetNewMssg (char * AdrString, int Len); // Len est la longueur maximale du tableau de r�ception
int PhyUART_SendNewMssg (char * AdrString, int Len);
#endif

