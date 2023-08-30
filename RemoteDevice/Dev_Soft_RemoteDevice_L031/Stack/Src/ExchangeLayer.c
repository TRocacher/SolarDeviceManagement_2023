#include "ExchangeLayer.h"

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

float ExchLayer_ExtractTemperature(char * MssgTempStr)
{
	float Value;
	float *PtrFloat;
	char *PtrChar;
	/* |MssgTemp|byte0|byte1|byte2|byte3| longeur 5 // byte0..3 = float */
	PtrFloat=&Value;
	PtrChar=PtrFloat; /* volontaire */
	/* Reconstruction float octet par octet ...*/
	*PtrChar=*(MssgTempStr+1);
	*(PtrChar+1)=*(MssgTempStr+2);
	*(PtrChar+2)=*(MssgTempStr+3);
	*(PtrChar+3)=*(MssgTempStr+4);

	return Value;
}

void ExchLayer_BuildMssgTemp(char * MssgTempStr, float Temp)
{
	float *PtrFloat;
	char *PtrChar;

	 PtrFloat=&Temp;
	 PtrChar=PtrFloat;
	 MssgTempStr[0]=MssgTempCode;
	 MssgTempStr[1]=*PtrChar;
	 MssgTempStr[2]=*(PtrChar+1);
	 MssgTempStr[3]=*(PtrChar+2);
	 MssgTempStr[4]=*(PtrChar+3);
}

void ExchLayer_BuildMssgWarning(char * MssgTempStr, RmDv_WarningCode WarningCode)
{
	MssgTempStr[0]=MssgWarningCode;
	MssgTempStr[1]=WarningCode;
}

void ExchLayer_BuildMssgError(char * MssgTempStr, RmDv_WkUp_CurrentState ErrorCode)
{
	MssgTempStr[0]=MssgErrorCode;
	MssgTempStr[1]=ErrorCode;
}

char ExchLayer_ExtractMssgcode(char * MssgTempStr)
{
	return (*MssgTempStr);
}

RmDv_TelecoIR_Cmde ExchLayer_ExtractClimOrder(char * MssgTempStr)
{
	return (* (MssgTempStr+7));
}

