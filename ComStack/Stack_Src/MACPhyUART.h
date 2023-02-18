#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "stm32f10x.h"


//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE MAC
//**************************************************************************************************************
//**************************************************************************************************************

void MACPhyUART_Init(char My);

#define MACPhyUART_StartFSM PhyUART_StartFSM

char MACPhyUART_IsNewMssg(void);

int MACPhyUART_GetNewMssg (char * AdrString, int Len); 

int MACPhyUART_SendNewMssg (char DestAdr, char * AdrString, int Len);


//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE PHY UART
//**************************************************************************************************************
//**************************************************************************************************************

// Priorité d'interruption
#define UART_Prio_CD 0							// priorité au niveau de l'UART , Carrier Detect (par défaut priorité maximale)	
#define UART_Prio (UART_Prio_CD+1) 	// priorité au niveau de l'UART (juste au dessous de CD)
#define PhyUART_FSM_Prio (UART_Prio+1)  // priorité de la FSM (par défaut juste en dessous de celle de l'UART)

#define TIM_PhyUART_FSM TIM2

// Baudrate
#define PhyUART_BdRate 9600

// longueur max des chaïnes
#define StringLenMax 30   // |Len | data (dont @) |Checksum| , la quantité de data (dont Src@ et Dest @) est donc de 28 octets
#define StringLenMin 7    // Len + Src@ + Dest@ + 4 octets Data + Checksum| (4 octets data seront typiquement ID + Type_LenData + 1 data + Trial )


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




/**************************************************************************************************
 Configure le module PhyUART (Timer, UART, état initial de la FSM...
***************************************************************************************************/
void PhyUART_Init(void);

/**************************************************************************************************
 Lance la FSM en validant la transition vers l'état WaitForHeader
***************************************************************************************************/
void PhyUART_StartFSM(void);



char PhyUART_IsNewMssg(void);
PhyUART_StatusType PhyUART_Get_Status(void);
PhyUART_ErrorType PhyUART_Get_Error(void);

/**************************************************************************************************
 Permet de recopier la donnée validée par la couche PhyUART.
 Len est la longueur Maximale pour cette recopie.
 ATTENTION ! la chaîne samplée par l'UART est entièrement recopiée à partir de l'adresse pointée.
 Un test est donc fait pour savoir si la longueur du tableau recevant est suffisant.
 La fonction répond -1 en cas d'erreur sur ce test ou 0 si tout est OK
***************************************************************************************************/
int PhyUART_GetNewMssg (char * AdrString, int Len); 
int PhyUART_SendNewMssg (char * AdrString, int Len);

#endif

