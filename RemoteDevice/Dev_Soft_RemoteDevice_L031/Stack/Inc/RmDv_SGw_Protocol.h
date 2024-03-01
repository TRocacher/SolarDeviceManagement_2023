/*
 * ExchangeLayer.h
 *
 *  Created on: 30 août 2023
 *      Author: trocache
 */

#ifndef RMDV_SGW_P_H_
#define RMDV_SGW_P_H_

#include "TimeManagement_RmDv.h"
#include <GLOBAL_RmDv.h>

#define AckToRmDv 0xAB
#define RMDV_ChronoName Chrono_Protocole
#define RMDV_TimeOutReq	TimeOutReq
#define RMDV_StatusReqTrialNb  StatusReqTrialNb
#define RMDV_InfoReqTrialNb StatusReqTrialNb


/* Liste des warnings, fonctionnement normal*/
typedef enum {
	Status_NoWarning=1,
	Status_Trial_2=2,
	Status_Trial_3=3,
	Status_Trial_4=4,
	Status_Trial_5=5,
	Status_Trial_6=6,
	Status_Trial_7=7,
	Status_Trial_8=8,
	Status_Trial_9=9,
	Status_Trial_10=10,

	Status_Error_TempI2C=20,
	Status_Error_NewTempSetNotReceived=21,
	Status_NoStatusReceived=22,
}RmDv_WarningCode;


/*
TRAME QUELCONQUE :

			|Code 		| Value |




TRAME MssgReq_SendInfo
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =6

TRAME MssgAns_SendInfo
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière| NextTimeInterval_sec (unsigned short int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1| Longueur = 4

TRAME MssgReq_SendStatus
		|MssgReq_SendStatus		|Value = RmDv_WarningCode |
		|MssgReq_SendStatus		|byte 0 | Longueur = 2

TRAME MssgAns_Ack
		|MssgAns_Ack		|Value = AckToRmDv |
		|MssgAns_Ack		|byte 0 |  Longueur = 2






!!! Les codes suivants ne sont plus utilisés !!!

TRAME ERREUR :
		|Code : MssgErrorCode 		|Value = RmDv_WarningCode

TRAME TEMPERATURE
			|MssgTempCode		|Value = float brut
			|MssgTempCode|byte0|byte1|byte2|byte3| longueur 5 // byte0..3 = float


TRAME CLIM ORDER CODE
			|TimeClimOrderCode		|Value = String formaté HHMnSec ; Clim Order
			|TimeClimOrderCode|Hdiz|Hunit|Mndiz|Mnunit|Secdiz|Secunit|ClimOrder|  longueur 8


			
			
	*/

/* Liste des des codes d'identification de trame*/
typedef enum {
	MssgReq_SendInfo = 0x50,
	MssgAns_SendInfo = 0x51,
	MssgReq_SendStatus = 0x52,
	MssgAns_Ack = 0x53,
		
	MssgWarningCode=100,
	MssgTempCode=101,
	MssgTimeClimOrderCode=102,
	MssgAckCode=103,
	MssgErrorCode=104,	
}MssgCode;



/***************************************************************
		  		Emission de requêtes (pour le RmDv ...)
***************************************************************/
typedef struct
{
	char DestAdr;
	float Temp;
	char LastSet;
	int TimeOut_ms;
	char TrialMaxNb;
	char TrialActualNb;
	char success;
	char NewSet;
	int NextInterval;

}RmDv_SGw_FSKP_ReqInfoTypedef;

void RmDv_SGw_FSKP_ReqInfo(RmDv_SGw_FSKP_ReqInfoTypedef* Req);


typedef struct
{
	char DestAdr;
	RmDv_WarningCode Status;
	int TimeOut_ms;
	char TrialMaxNb;
	char TrialActualNb;
	char success;
}RmDv_SGw_FSKP_ReqStatusTypedef;

void RmDv_SGw_FSKP_ReqStatus(RmDv_SGw_FSKP_ReqStatusTypedef* Req);

/***************************************************************
		  	 Liste des fonction d'émission simple
***************************************************************/
void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, float Temp, char LastSet);
void RmDv_SGw_FSKP_SendMssgAns_SendInfo(char DestAdr, char NewSet, unsigned short int NextWupInterval);
void RmDv_SGw_FSKP_SenddMssgReq_SendStatus(char DestAdr,  char Status);
void RmDv_SGw_FSKP_SendMssgAns_Ack(char DestAdr);





/***************************************************************
		  	Liste des fonctions d'extraction de champs
***************************************************************/
MssgCode RmDv_SGw_FSKP_ExtractMssgcode(char * MssgTempStr);
float RmDv_SGw_FSKP_ExtractTemp(char * MssgTempStr);
char RmDv_SGw_FSKP_ExtracLastSet(char * MssgTempStr);
char  RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr);
unsigned short int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr);
RmDv_WarningCode   RmDv_SGw_FSKP_ExtracStatus(char * MssgTempStr);




#endif /* INC_PROTOCOLEFCTS_H_ */
