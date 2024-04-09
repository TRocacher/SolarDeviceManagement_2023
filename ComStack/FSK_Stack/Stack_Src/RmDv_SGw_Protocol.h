/*
 * RmDv_Sgw_Protocol.h
 *   Created on: 30/08/23
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : "GLOBAL_SMARTGATEWAY.h" pour les ID des clim (cste) 
 
 *    ** NEW **
		Ajout du champ TransacIndex qui correspond au numéro de transaction lié au temps 
		qui défile : 
		Dans le sens RmDv vers SGw (donc req), si TransactionIdx = 2 alors cette valeur doit
			correspondre à l'index temps réel, donc égal à 2. Cela permet de faire un correspondance
			avec le "créneau horaire n°2" et donc d'en déduire immédiatement le prochain délai, la gigue
			temporelle...
 *  ------------------------------------------------------------------------------
 */

#ifndef RMDV_SGW_P_H_
#define RMDV_SGW_P_H_

#include "TimeManagement.h"

#define AckToRmDv 0xAB



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
			  	 Liste des fonction d'émission simple
	***************************************************************/
//void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, char TransIdx, float Temp, char LastSet); /* New 1/04/24*/

/**
  * @brief  
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3|    | byte0|byte1|byte2|byte3| Longueur = 11	
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SendMssgAns_SendInfo(char DestAdr, char NextTransIdx, char NewSet, int NextWupInterval, int RealPreviousInterval); /* New 06/04/24*/
//void RmDv_SGw_FSKP_SenddMssgReq_SendStatus(char DestAdr,  char Status);
void RmDv_SGw_FSKP_SendMssgAns_Ack(char DestAdr,char NextTransIdx);



	/***************************************************************
			  	Liste des fonctions d'extraction de champs
	***************************************************************/

char RmDv_SGw_FSKP_ExtracTransIdx(char * MssgTempStr); /* New 08/04/24*/


MssgCode RmDv_SGw_FSKP_ExtractMssgcode(char * MssgTempStr); /* New 1/04/24*/
float RmDv_SGw_FSKP_ExtractTemp(char * MssgTempStr); /* New 1/04/24*/
char RmDv_SGw_FSKP_ExtracLastSet(char * MssgTempStr); /* New 1/04/24*/
char  RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr); /* New 1/04/24*/
int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr); /* New 1/04/24*/
RmDv_WarningCode   RmDv_SGw_FSKP_ExtracStatus(char * MssgTempStr); /* New 1/04/24*/

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
