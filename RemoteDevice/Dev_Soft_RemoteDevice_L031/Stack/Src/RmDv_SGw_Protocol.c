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

				Fonction d'�missions de requ�tes ou de r�ponse

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
void RmDv_SGw_FSKP_SendMssgReq_SendInfo(char DestAdr, char TransIdx, float Temp, char LastSet) /* New 1/04/24*/
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
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val enti�re | NextTimeInterval_sec (unsigned short int) |
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
	/*�mission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 4);
}



/**
  * @brief  
  * @Note
TRAME MssgReq_SendStatus
		|* TransacIndex *|MssgReq_SendStatus		|Value = RmDv_WarningCode |
		|* TransacIndex *|MssgReq_SendStatus		|byte 0 | Longueur = 3
  * @param  
  * @retval 
  **/
void RmDv_SGw_FSKP_SenddMssgReq_SendStatus(char DestAdr,char TransIdx, char Status)
{
	char MssgToSend[10]; 
	MssgToSend[0]=TransIdx;
	MssgToSend[1]=MssgReq_SendStatus;
	MssgToSend[2]=Status;
	/*�mission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 3);
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
	/*�mission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 2);
}



/* =================================================================================

				Fonction d'extraction de champs de donn�es

   ================================================================================*/

/**
  * @brief
  * @Note
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0
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
TRAME MssgAns_SendInfo
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val entière | NextTimeInterval_sec ( int) | RealPreviousInterval (int)|
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0
  * @retval
  **/
char RmDv_SGw_FSKP_ExtracTransIdx(char * MssgTempStr) /* New 07/04/24*/
{
	char Val;
	Val=*(MssgTempStr);
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
		|* TransacIndex *|MssgAns_SendInfo 		| NewTempSet (char) = temperature val enti�re | NextTimeInterval_sec (int) |
		|* TransacIndex *|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3| Longueur = 6
  * @param  
  * @retval 
  **/
char RmDv_SGw_FSKP_ExtracNewTempSet(char * MssgTempStr)
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








/**
  * @brief
  * @Note
	* @param
  * @retval
  **/
void RmDv_SGw_FSKP_ReqInfo(RmDv_SGw_FSKP_ReqInfoTypedef* Req)
{
	int i;
	int Long;
	char ReceivedMssg[30];
	char LocalSuccess;


	LocalSuccess=0;
	for (i=0;i<Req->TrialMaxNb;i++)
	{
		TimeManag_TimeOutStart(RMDV_ChronoName,Req->TimeOut_ms);
		/*émission requête i*/
		RmDv_SGw_FSKP_SendMssgReq_SendInfo(Req->DestAdr,Req->TransIdx ,Req->Temp, Req->LastSet);
		while(TimeManag_GetTimeOutStatus(RMDV_ChronoName)==0)
		{
				if (FSKStack_IsNewMssg()==1)
				{
					Long=FSKStack_GetLen();
					FSKStack_GetNewMssg(ReceivedMssg, Long);
					if (RmDv_SGw_FSKP_ExtractMssgcode(ReceivedMssg)==MssgAns_SendInfo)
					{
							Req->NewSet	= RmDv_SGw_FSKP_ExtracNewTempSet(ReceivedMssg);
							Req->NextInterval = RmDv_SGw_FSKP_ExtractNextWupInterval(ReceivedMssg);
							Req->TransIdx =  RmDv_SGw_FSKP_ExtracTransIdx(ReceivedMssg);
							LocalSuccess=1;
							break;
					}
				}
		}
		if (LocalSuccess == 1) break;
	}
	if (i == Req->TrialMaxNb) /* Sortie de boucle sans aucun succès*/
	{
		Req->success=0;
	}
	else
	{
		Req->success=1;
		Req->TrialActualNb=i+1;
	}
}




/**
  * @brief
  * @Note
	* @param
  * @retval
  **/
void RmDv_SGw_FSKP_ReqStatus(RmDv_SGw_FSKP_ReqStatusTypedef* Req)
{
	int i;
	int Long;
	char ReceivedMssg[30];
	char LocalSuccess;


	LocalSuccess=0;
	for (i=0;i<Req->TrialMaxNb;i++)
	{
		TimeManag_TimeOutStart(RMDV_ChronoName,Req->TimeOut_ms);
		/*émission requête i*/
		RmDv_SGw_FSKP_SenddMssgReq_SendStatus(Req->DestAdr,Req->TransIdx ,Req->Status);
		while(TimeManag_GetTimeOutStatus(RMDV_ChronoName)==0)
		{
				if (FSKStack_IsNewMssg()==1)
				{
					Long=FSKStack_GetLen();
					FSKStack_GetNewMssg(ReceivedMssg, Long);
					if (RmDv_SGw_FSKP_ExtractMssgcode(ReceivedMssg)==MssgAns_Ack)
					{
							LocalSuccess=1;
							break;
					}
				}
		}
		if (LocalSuccess == 1) break;
	}
	if (i == Req->TrialMaxNb) /* Sortie de boucle sans aucun succès*/
	{
		Req->success=0;
	}
	else
	{
		Req->success=1;
		Req->TrialActualNb=i+1;
	}
}





