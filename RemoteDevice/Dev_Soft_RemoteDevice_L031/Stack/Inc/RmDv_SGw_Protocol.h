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

			|* TransacIndex *|Code 		| Value |

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
	char DestAdr;		/* Adresse de destinataire*/
	char TransIdx;		/* Index de transaction (celui reçu précédemment)*/
	float Temp;			/* Température : out */
	char LastSet;		/* dernière consigne : out */
	int TimeOut_ms;		/* durée maximale pour un essai */
	char TrialMaxNb;	/* nbre maximum de tentatives autorisées */
	char TrialActualNb;	/* nbre effectif de tentatives */
	char success;		/* 1 : la requête est OK, 0 sinon*/
	char NewSet;		/* nouvelle consigne de température : in*/
	int NextInterval;	/* nombre de secondes à attendre pour
	 	 	 	 	 	 prochain réveil : in*/

}RmDv_SGw_FSKP_ReqInfoTypedef;

void RmDv_SGw_FSKP_ReqInfo(RmDv_SGw_FSKP_ReqInfoTypedef* Req);


typedef struct
{
	char DestAdr;				/* Adresse de destinataire*/
	char TransIdx;				/* Index de transaction (celui reçu précédemment)*/
	RmDv_WarningCode Status;	/* le status de l'échange côté RmDv : out*/
	int TimeOut_ms;				/* durée maximale pour un essai */
	char TrialMaxNb;			/* nbre maximum de tentatives autorisées */
	char TrialActualNb;			/* nbre effectif de tentatives */
	char success;				/* 1 : la requête est OK, 0 sinon*/
}RmDv_SGw_FSKP_ReqStatusTypedef;

void RmDv_SGw_FSKP_ReqStatus(RmDv_SGw_FSKP_ReqStatusTypedef* Req);

/***************************************************************
		  	 Liste des fonction d'émission simple
***************************************************************/

void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, char TransIdx, float Temp, char LastSet); /* New 1/04/24*/

//void RmDv_SGw_FSKP_SendMssgAns_SendInfo(char DestAdr, char NewSet, unsigned short int NextWupInterval);
void RmDv_SGw_FSKP_SenddMssgReq_SendStatus(char DestAdr, char TransIdx, char Status);
//void RmDv_SGw_FSKP_SendMssgAns_Ack(char DestAdr);





/***************************************************************
		  	Liste des fonctions d'extraction de champs
***************************************************************/

MssgCode RmDv_SGw_FSKP_ExtractMssgcode(char * MssgTempStr); /* New 1/04/24*/
float RmDv_SGw_FSKP_ExtractTemp(char * MssgTempStr); /* New 1/04/24*/
char RmDv_SGw_FSKP_ExtracLastSet(char * MssgTempStr); /* New 1/04/24*/
char  RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr); /* New 1/04/24*/
int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr); /* New 1/04/24*/
RmDv_WarningCode   RmDv_SGw_FSKP_ExtracStatus(char * MssgTempStr); /* New 1/04/24*/
char RmDv_SGw_FSKP_ExtracTransIdx(char * MssgTempStr); /* New 07/04/24*/

/**
  * @brief
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3|    | byte0|byte1|byte2|byte3| Longueur = 11
  * @param
  * @retval valeur int de l'intervalle réel de la précédente transaction
  **/
int  RmDv_SGw_FSKP_ExtractRealPreviousInterval(char * MssgTempStr);  /* New 06/04/24*/




#endif /* INC_PROTOCOLEFCTS_H_ */
