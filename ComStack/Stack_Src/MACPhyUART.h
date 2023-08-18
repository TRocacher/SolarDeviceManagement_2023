#ifndef _PHYUART_H__
#define _PHYUART_H__


#include "ModuleFSK.h"
#include "TimeManagement.h"

//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE MAC
//**************************************************************************************************************
//**************************************************************************************************************

void MACPhyUART_Init(char My);

#define MACPhyUART_StartFSM PhyUART_StartFSM
#define MACPhyUART_Get_Status PhyUART_Get_Status 
#define MACPhyUART_Get_Error PhyUART_Get_Error

char MACPhyUART_IsNewMssg(void);

int MACPhyUART_GetNewMssg (char * AdrString, int Len); 

char MACPhyUART_GetSrcAdress(void);

char MACPhyUART_GetLen(void);

int MACPhyUART_SendNewMssg (char DestAdr, char * AdrString, int Len);


//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE PHY UART
//**************************************************************************************************************
//**************************************************************************************************************

// Priorité d'interruption
// USER DEFINE (recommandé)
#define UART_Prio_CD 0							// priorité au niveau de l'UART , Carrier Detect (par défaut priorité maximale)	
// NE PAS TOUCHER
#define UART_Prio (UART_Prio_CD+1) 	// priorité au niveau de l'UART (juste au dessous de CD)
#define PhyUART_FSM_Prio (UART_Prio+1)  // priorité de la FSM (par défaut juste en dessous de celle de l'UART)

// USER DEFINE
#define TIM_PhyUART_FSM TIM2

// USER DEFINE
#define PhyUART_BdRate 38400

// USER DEFINE
// longueur max des chaïnes
#define StringLenMax 30   // |Len | data (dont @) |Checksum| , la quantité de data (dont Src@ et Dest @) est donc de 28 octets
#define StringLenMin 5    // Len + Src@ + Dest@ + 1 octet Data + Checksum| 

	// calcul time Out on prévoit la durée d'une chaîne maximale +10% 
	// calcul en ms : T = NbBit*NbMaxOctet*1.1*Tbit = NbBit*NbMaxOctet*1.1/R 
	//			            = 1000*10*NbMaxOctet*1.1/R = (1100*NbBit*NbMaxOctet1)/R
	
#define	PhyUART_TimeOut ((11000*StringLenMax)/PhyUART_BdRate)


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
	OverRunError,
	MACOverRunError
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

