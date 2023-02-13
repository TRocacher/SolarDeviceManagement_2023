#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "stm32f10x.h"

// Priorité d'interruption
#define UART_Prio_CD 0							// priorité au niveau de l'UART , Carrier Detect (par défaut priorité maximale)	
#define UART_Prio (UART_Prio_CD+1) 	// priorité au niveau de l'UART (juste au dessous de CD)
#define Phy_UART_FSM (UART_Prio+1)  // priorité de la FSM (par défaut juste en dessous de celle de l'UART)

// Baudrate
#define PhyUART_BdRate 38400

// longueur max des chaïnes
#define StringLenMax 30
#define StringLenMin 7


/*---------------------------------
 états possibles de la FSM
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

int  PhyUART_GetNewMssg (char * AdrString, int Len); // Len est la longueur maximale du tableau de réception
int PhyUART_SendNewMssg (char * AdrString, int Len);
#endif

