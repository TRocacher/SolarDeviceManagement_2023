#include <ProtocoleFcts.h>

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

float Protocole_ExtractTemperature(char * MssgTempStr)
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

void Protocole_BuildMssgTemp(char * MssgTempStr, float Temp)
{
	float *PtrFloat;
	char *PtrChar;

	 PtrFloat=&Temp;
	 PtrChar=(char*)PtrFloat;
	 MssgTempStr[0]=MssgTempCode;
	 MssgTempStr[1]=*PtrChar;
	 MssgTempStr[2]=*(PtrChar+1);
	 MssgTempStr[3]=*(PtrChar+2);
	 MssgTempStr[4]=*(PtrChar+3);
}

void Protocole_BuildMssgWarning(char * MssgTempStr, RmDv_WarningCode WarningCode)
{
	MssgTempStr[0]=MssgWarningCode;
	MssgTempStr[1]=WarningCode;
}

void Protocole_BuildMssgError(char * MssgTempStr, RmDv_WkUp_CurrentState ErrorCode)
{
	MssgTempStr[0]=MssgErrorCode;
	MssgTempStr[1]=ErrorCode;
}

MssgCode Protocole_ExtractMssgcode(char * MssgTempStr)
{
	char Val;
	Val=(*MssgTempStr);
	return ((MssgCode)Val);
}

RmDv_TelecoIR_Cmde Protocole_ExtractClimOrder(char * MssgTempStr)
{
	char Val;
	Val= *(MssgTempStr+7);
	return ((RmDv_TelecoIR_Cmde)Val);
}

void Protocole_BuildMssgTelecoHeure(char * MssgStr, RmDv_TelecoIR_Cmde Cmde)
{
	
	int i;
	MssgStr[0]=MssgTimeClimOrderCode;
	// for = heure bidon pour l'instant
	for (i=1;i<7;i++)
	{
		MssgStr[i]=i;
	}
	MssgStr[7]=Cmde;
}

RmDv_WarningCode Protocole_ExtractWarningCode(char * MssgTempStr)
{
	char Val;
	Val= *(MssgTempStr+1);
	return ((RmDv_WarningCode)Val);
}


/* GESTION des n essais successifs.... à faire
Stop=1;  On stoppe par d�faut
		 for (i=0;i<3;i++)
		 {
			 MACPhyUART_SendNewMssg (UC_Adress,TransmitMssg, 5);
			 TimeManag_TimeOutStart(Chrono_3 , 100);
			 while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
			 {
				 if (MACPhyUART_IsNewMssg()==1)
				 {
					 Long=MACPhyUART_GetLen();
					 MACPhyUART_GetNewMssg(ReceivedMssg, Long);
					 Stop=0;
					 break;
				 }
			 }
			 if (Stop==0) break;
			 else StandByWkUpPgm_WCode=Transm_1_Attempt+i;
		 }
*/


