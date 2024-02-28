#ifndef _FSK_STACK_H__
#define _FSK_STACK_H__

#include <GLOBAL_RmDv.h>
#include <ModuleFSK_RmDv.h>
#include <TimeManagement_RmDv.h>

//**************************************************************************************************************
//**************************************************************************************************************
// 							API FSK_Stack
//**************************************************************************************************************
//**************************************************************************************************************

void FSKStack_Init(char My);

#define FSKStack_StartFSM PhyUART_StartFSM
#define FSKStack_Get_Status PhyUART_Get_Status 
#define FSKStack_Get_Error PhyUART_Get_Error

/* Remet à 0 les attributs d'échanges de l'interface
   ne modifie pas l'@ My */
void FSKStack_Reset_Restart_KeepMy(void);

char FSKStack_IsNewMssg(void);

int FSKStack_GetNewMssg (char * AdrString, int Len); 

char FSKStack_GetSrcAdress(void);

char FSKStack_GetLen(void);

int FSKStack_SendNewMssg (char DestAdr, char * AdrString, int Len);


//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE PHY UART
//**************************************************************************************************************
//**************************************************************************************************************

// Priorit� d'interruption
// USER DEFINE (recommand�)
#define UART_Prio_CD Prio_FSKStack_UART_CD							// priorit� au niveau de l'UART , Carrier Detect (par d�faut priorit� maximale)
// NE PAS TOUCHER
#define UART_Prio Prio_FSKStack_UART_Rec	// priorit� au niveau de l'UART (juste au dessous de CD)
#define PhyUART_FSM_Prio Prio_FSKStack_TimerFSM  // priorit� de la FSM (par d�faut juste en dessous de celle de l'UART)

// USER DEFINE
#define TIM_PhyUART_FSM Timer_FSK_Stack_FSM

// USER DEFINE
#define PhyUART_BdRate  Rate_UART_FSK

// USER DEFINE
// longueur max des cha�nes
#define StringLenMax 30   // |Len | data (dont @) |Checksum| , la quantit� de data (dont Src@ et Dest @) est donc de 28 octets
#define StringLenMin 5    // Len + Src@ + Dest@ + 1 octet Data + Checksum| 

	// calcul time Out on pr�voit la dur�e d'une cha�ne maximale +10% 
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
 Configure le module PhyUART (Timer, UART, �tat initial de la FSM...
***************************************************************************************************/
//void PhyUART_Init(void);

/**************************************************************************************************
 Configure le module PhyUART (Timer, UART, état initial de la FSM...
UTILE UNIQUEMENT SI LA PILE N EST PAS UTILISE JUSQU AU BOUT 	(Couche MAC, cad FSKStack non utilisé)
***************************************************************************************************/
//void PhyUART_Init(void);



//char PhyUART_IsNewMssg(void);
//PhyUART_StatusType PhyUART_Get_Status(void);
//PhyUART_ErrorType PhyUART_Get_Error(void);

/**************************************************************************************************
 Permet de recopier la donn�e valid�e par la couche PhyUART.
 Len est la longueur Maximale pour cette recopie.
 ATTENTION ! la cha�ne sampl�e par l'UART est enti�rement recopi�e � partir de l'adresse point�e.
 Un test est donc fait pour savoir si la longueur du tableau recevant est suffisant.
 La fonction r�pond -1 en cas d'erreur sur ce test ou 0 si tout est OK
***************************************************************************************************/
//int PhyUART_GetNewMssg (char * AdrString, int Len);
//int PhyUART_SendNewMssg (char * AdrString, int Len);

#endif

