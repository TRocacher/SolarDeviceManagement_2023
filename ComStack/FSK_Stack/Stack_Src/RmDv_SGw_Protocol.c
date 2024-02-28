#include "RmDv_SGw_Protocol.h"


#include "FSKStack.h"

/* =================================================================================
* ==================   Main_StandByWkUpPgm	     ===================================
 *
 *   Created on: Aout 30, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 *

* =================================================================================*/





/* =================================================================================

				Fonction d'émissions de requêtes ou de réponse

   ================================================================================*/


/**
  * @brief  
  * @Note
	TRAME MssgReq_SendInfo
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =6
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, float Temp, char LastSet)
{
	float *PtrFloat;
	char *PtrChar;
	char MssgToSend[10];

	PtrFloat=&Temp;
	PtrChar=(char*)PtrFloat;
	MssgToSend[0]=MssgReq_SendInfo;
	MssgToSend[1]=*PtrChar;
	MssgToSend[2]=*(PtrChar+1);
	MssgToSend[3]=*(PtrChar+2);
	MssgToSend[4]=*(PtrChar+3);
	MssgToSend[5]=LastSet;
	
	 /*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 6);
}


/**
  * @brief  
  * @Note
TRAME MssgAns_SendInfo
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec (unsigned short int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1| Longueur = 4		
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SendMssgAns_SendInfo(char DestAdr, char NewSet, unsigned short int NextWupInterval)
{
	unsigned short int *PtrShort;
	char *PtrChar;
	char MssgToSend[10];
	PtrShort=&NextWupInterval;
	PtrChar=(char*)PtrShort;
	MssgToSend[0]=MssgAns_SendInfo;
	MssgToSend[1]=NewSet;
	MssgToSend[2]=*(PtrChar);
	MssgToSend[3]=*(PtrChar+1);
	/*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 4);
}



/**
  * @brief  
  * @Note
TRAME MssgReq_SendStatus
		|MssgReq_SendStatus		|Value = RmDv_WarningCode |
		|MssgReq_SendStatus		|byte 0 | Longueur = 2							
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SenddMssgReq_SendStatus(char DestAdr, char Status)
{
	char MssgToSend[10]; 
	MssgToSend[0]=MssgReq_SendStatus;
	MssgToSend[1]=Status;
	/*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 2);
}



/**
  * @brief  
  * @Note
TRAME MssgAns_Ack
		|MssgAns_Ack		|Value = AckToRmDv |
		|MssgAns_Ack		|byte 0 |  Longueur = 2	
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SendMssgAns_Ack(char DestAdr)
{
	char MssgToSend[10];
	MssgToSend[0]=MssgAns_Ack;
	MssgToSend[1]=AckToRmDv;
	/*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 2);
}



/* =================================================================================

				Fonction d'extraction de champs de données

   ================================================================================*/

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
MssgCode RmDv_SGw_FSKP_ExtractMssgcode(char * MssgTempStr)
{
	char Val;
	Val=(*MssgTempStr);
	return ((MssgCode)Val);
}

/**
  * @brief  
  * @Note
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =6
  * @param  
  * @retval 
  **/
float RmDv_SGw_FSKP_ExtractTemp(char * MssgTempStr)
{
	float Value;
	float *PtrFloat;
	char *PtrChar;
	/* |MssgTemp|byte0|byte1|byte2|byte3| longeur 5 // byte0..3 = float */
	PtrFloat=&Value;
	PtrChar=(char*)PtrFloat; /* volontaire */
	/* Reconstruction float octet par octet ...*/
	*PtrChar=*(MssgTempStr+1);
	*(PtrChar+1)=*(MssgTempStr+2);
	*(PtrChar+2)=*(MssgTempStr+3);
	*(PtrChar+3)=*(MssgTempStr+4);

	return Value;	
}


/**
  * @brief  
  * @Note
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =6
  * @param  
  * @retval 
  **/
char RmDv_SGw_FSKP_ExtracLastSet(char * MssgTempStr)
{
	return *(MssgTempStr+5);
}
	

/**
  * @brief  
  * @Note
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec (unsigned short int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1| Longueur = 4		
  * @param  
  * @retval 
  **/
char RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr)
{
	return *(MssgTempStr+1);
}


/**
  * @brief  
  * @Note
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec (unsigned short int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1| Longueur = 4		
  * @param  
  * @retval 
  **/
unsigned short int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr)
{
	unsigned short int Value;
	unsigned short int *PtrShort;
	char *PtrChar;
	PtrShort=&Value;
	PtrChar=(char*)PtrShort; /* volontaire */
	/* Reconstruction float octet par octet ...*/
	*(PtrChar)=*(MssgTempStr+2); /* low byte */
	*(PtrChar+1)=*(MssgTempStr+3); /* high byte */
	return Value;	
}


/**
  * @brief  
  * @Note
		|MssgReq_SendStatus		|Value = RmDv_WarningCode |
		|MssgReq_SendStatus		|byte 0 | Longueur = 2
  * @param  
  * @retval 
  **/
RmDv_WarningCode  RmDv_SGw_FSKP_ExtracStatus(char * MssgTempStr)
{
	char Val;
	Val= *(MssgTempStr+1);
	return ((RmDv_WarningCode)Val);
}










