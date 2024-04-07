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
		|* TransacIndex *|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|* TransacIndex *|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =7
  * @param  
  * @retval 
  **/
/* Modif 1/04/24 ajout chp TransactionIdx*/
void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, char TransIdx, float Temp, char LastSet)
{
	float *PtrFloat;
	char *PtrChar;
	char MssgToSend[10];

	PtrFloat=&Temp;
	PtrChar=(char*)PtrFloat;
	MssgToSend[0]=TransIdx;
	MssgToSend[1]=MssgReq_SendInfo;
	MssgToSend[2]=*PtrChar;
	MssgToSend[3]=*(PtrChar+1);
	MssgToSend[4]=*(PtrChar+2);
	MssgToSend[5]=*(PtrChar+3);
	MssgToSend[6]=LastSet;
	
	 /*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 7);
}


/**
  * @brief  
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3|    | byte0|byte1|byte2|byte3| Longueur = 11	
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SendMssgAns_SendInfo(char DestAdr, char NextTransIdx, char NewSet, int NextWupInterval, int RealPreviousInterval)
/* Modif 1/04/24 ajout chp TransactionIdx*/
{
	int *PtrInt;
	char *PtrChar;
	char MssgToSend[15];
	PtrInt=&NextWupInterval;
	PtrChar=(char*)PtrInt;
	MssgToSend[0]=NextTransIdx;
	MssgToSend[1]=MssgAns_SendInfo;
	MssgToSend[2]=NewSet;
	/* NextWupInterval*/
	MssgToSend[3]=*(PtrChar);
	MssgToSend[4]=*(PtrChar+1);
	MssgToSend[5]=*(PtrChar+2);
	MssgToSend[6]=*(PtrChar+3);	
	/* RealPreviousInterval*/
	PtrInt=&RealPreviousInterval;
	PtrChar=(char*)PtrInt;
	MssgToSend[7]=*(PtrChar);
	MssgToSend[8]=*(PtrChar+1);
	MssgToSend[9]=*(PtrChar+2);
	MssgToSend[10]=*(PtrChar+3);	
	/*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 11);
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
void RmDv_SGw_FSKP_SendMssgAns_Ack(char DestAdr,char NextTransIdx)
{
	char MssgToSend[10];
	MssgToSend[0]=NextTransIdx;
	MssgToSend[1]=MssgAns_Ack;
	MssgToSend[2]=AckToRmDv;
	/*émission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 3);
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
MssgCode RmDv_SGw_FSKP_ExtractMssgcode(char * MssgTempStr)/* Modif 1/04/24 ajout chp TransactionIdx*/
{
	char Val;
	Val=*(MssgTempStr+1);
	return ((MssgCode)Val);
}

/**
  * @brief  
  * @Note
		|* TransacIndex *|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|* TransacIndex *|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =7
  * @param  
  * @retval 
  **/
float RmDv_SGw_FSKP_ExtractTemp(char * MssgTempStr) /* Modif 1/04/24 ajout chp TransactionIdx*/
{
	float Value;
	float *PtrFloat;
	char *PtrChar;
	/* |* TransacIndex *|MssgTemp|byte0|byte1|byte2|byte3| longeur 5 // byte0..3 = float */
	PtrFloat=&Value;
	PtrChar=(char*)PtrFloat; /* volontaire */
	/* Reconstruction float octet par octet ...*/
	*PtrChar=*(MssgTempStr+2);
	*(PtrChar+1)=*(MssgTempStr+3);
	*(PtrChar+2)=*(MssgTempStr+4);
	*(PtrChar+3)=*(MssgTempStr+5);

	return Value;	
}


/**
  * @brief  
  * @Note
		|* TransacIndex *|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val entière|
		|* TransacIndex *|MssgReq_SendInfo 		|  byte0|byte1|byte2|byte3 |  byte0 | Longueur =7
  * @param  
  * @retval 
  **/
char RmDv_SGw_FSKP_ExtracLastSet(char * MssgTempStr) /* Modif 1/04/24 ajout chp TransactionIdx*/
{
	return *(MssgTempStr+6);
}
	

/**
  * @brief  
  * @Note
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec (unsigned short int) |
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1| Longueur = 5		
  * @param  
  * @retval 
  **/
char RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr) /* Modif 1/04/24 ajout chp TransactionIdx*/
{ 
	return *(MssgTempStr+2);
}


/**
  * @brief  
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3|    | byte0|byte1|byte2|byte3| Longueur = 11	
  * @param  
  * @retval 
  **/
int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr) /* Modif 1/04/24 ajout chp TransactionIdx*/
{
	int Value;
	int *PtrInt;
	char *PtrChar;
	PtrInt=&Value;
	PtrChar=(char*)PtrInt; /* volontaire */
	/* Reconstruction int octet par octet ...*/
	*(PtrChar)=*(MssgTempStr+3); /* low byte */
	*(PtrChar+1)=*(MssgTempStr+4); /* higher byte */
	*(PtrChar+2)=*(MssgTempStr+5); /* higher byte */
	*(PtrChar+3)=*(MssgTempStr+6); /* highest byte */
	return Value;	
}


/**
  * @brief  
  * @Note
		|* TransacIndex *|MssgReq_SendStatus		|Value = RmDv_WarningCode |
		|* TransacIndex *|MssgReq_SendStatus		|byte 0 | Longueur = 3
  * @param  
  * @retval 
  **/
RmDv_WarningCode  RmDv_SGw_FSKP_ExtracStatus(char * MssgTempStr)  /* Modif 1/04/24 ajout chp TransactionIdx*/
{
	char Val;
	Val= *(MssgTempStr+2);
	return ((RmDv_WarningCode)Val);
}


/**
  * @brief  
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3|    | byte0|byte1|byte2|byte3| Longueur = 11	
  * @param  
  * @retval valeur int de l'intervalle réel de la précédente transaction 
  **/
int  RmDv_SGw_FSKP_ExtractRealPreviousInterval(char * MssgTempStr)
{
	int Value;
	int *PtrInt;
	char *PtrChar;
	PtrInt=&Value;
	PtrChar=(char*)PtrInt; /* volontaire */
	/* Reconstruction int octet par octet ...*/
	*(PtrChar)=*(MssgTempStr+7); /* low byte */
	*(PtrChar+1)=*(MssgTempStr+8); /* higher byte */
	*(PtrChar+2)=*(MssgTempStr+9); /* higher byte */
	*(PtrChar+3)=*(MssgTempStr+10); /* highest byte */
	return Value;	
}
