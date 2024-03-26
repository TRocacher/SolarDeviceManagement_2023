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
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val enti�re|
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
	
	 /*�mission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 6);
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
	/*�mission effective*/
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
	/*�mission effective*/
	FSKStack_SendNewMssg (DestAdr,MssgToSend, 2);
}



/* =================================================================================

				Fonction d'extraction de champs de donn�es

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
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val enti�re|
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
		|MssgReq_SendInfo 		| Temperature (float)      | LastTempSet (char) = temperature val enti�re|
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
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val enti�re | NextTimeInterval_sec (int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3| Longueur = 6
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
		|MssgAns_SendInfo 		| NewTempSet (char) = temperature val enti�re | NextTimeInterval_sec (int) |
		|MssgAns_SendInfo 		| byte 0 													      			| byte0|byte1|byte2|byte3| Longueur = 6
  * @param  
  * @retval 
  **/
int  RmDv_SGw_FSKP_ExtractNextWupInterval(char * MssgTempStr)
{
	int Value;
	int *PtrInt;
	char *PtrChar;
	PtrInt=&Value;
	PtrChar=(char*)PtrInt; /* volontaire */
	/* Reconstruction int octet par octet ...*/
	*(PtrChar)=*(MssgTempStr+2); /* low byte */
	*(PtrChar+1)=*(MssgTempStr+3); /* higher byte */
	*(PtrChar+2)=*(MssgTempStr+4); /* higher byte */
	*(PtrChar+3)=*(MssgTempStr+5); /* highest byte */
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
RmDv_WarningCode  RmDv_SGw_FSKP_ExtractStatus(char * MssgTempStr)
{
	char Val;
	Val= *(MssgTempStr+1);
	return ((RmDv_WarningCode)Val);
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
		RmDv_SGw_FSKP_SendMssgReq_SendInfo(Req->DestAdr, Req->Temp, Req->LastSet);
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
		RmDv_SGw_FSKP_SenddMssgReq_SendStatus(Req->DestAdr,  Req->Status);
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





