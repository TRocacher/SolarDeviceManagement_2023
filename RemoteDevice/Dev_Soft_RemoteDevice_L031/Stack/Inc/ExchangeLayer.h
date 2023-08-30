/*
 * ExchangeLayer.h
 *
 *  Created on: 30 août 2023
 *      Author: trocache
 */

#ifndef INC_EXCHANGELAYER_H_
#define INC_EXCHANGELAYER_H_
#include "RmDv_ErrorWDG.h"
#include "RmDv_TelecoIR.h"

typedef enum {
	NoWarning=10,
	Transm_1_Attempt=11,
	Transm_2_Attempt=12,
	Transm_3_Attempt=13,
	Temp_Error=14,
	Transm_Error_NoTimeClimCodeReceived=15,
	WrongCmdeWhenReceivingTimeClimCode=16,
}RmDv_WarningCode;


/*
Trame :
			|Code 		| Value |

			Error 		|Value = code erreur string

			Temp 		|Value = float brut
			|MssgTempCode|byte0|byte1|byte2|byte3| longueur 5 // byte0..3 = float


			TimeClimOrderCode		|Value = String formaté HHMnSec ; Clim Order
			|TimeClimOrderCode|Hdiz|Hunit|Mndiz|Mnunit|Secdiz|Secunit|ClimOrder|  longueur 8

			Ack   		|Value = no value
	*/
typedef enum {
	MssgWarningCode=100,
	MssgTempCode=101,
	MssgTimeClimOrderCode=102,
	MssgAckCode=103,
	MssgErrorCode=104,

}MssgCode;

float ExchLayer_ExtractTemperature(char * MssgTempStr);
void ExchLayer_BuildMssgTemp(char * MssgTempStr, float Temp);
char ExchLayer_ExtractMssgcode(char * MssgTempStr);
RmDv_TelecoIR_Cmde ExchLayer_ExtractClimOrder(char * MssgTempStr);
void ExchLayer_BuildMssgWarning(char * MssgTempStr, RmDv_WarningCode Warning);
void ExchLayer_BuildMssgError(char * MssgTempStr, RmDv_WkUp_CurrentState ErrorCode);

#endif /* INC_EXCHANGELAYER_H_ */
