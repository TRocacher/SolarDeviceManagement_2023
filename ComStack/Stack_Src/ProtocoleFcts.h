/*
 * ExchangeLayer.h
 * VERSION F103 KEIL
 *  Created on: 30 août 2023
 *      Author: trocache
 */

#ifndef INC_PROTOCOLEFCTS_H_
#define INC_PROTOCOLEFCTS_H_

//#include "RmDv_ErrorWDG.h"
typedef enum {
	BoostActivation=0,
	TemperatureMeasure=1,
	WakeUpMssgToUC=2,
	ClimUpdate=3,
	RTCAdjust=4,
	WarningMssg=5,
}RmDv_WkUp_CurrentState;

//#include "RmDv_TelecoIR.h"
typedef enum {
	_Chaud_18_VanBas_FanAuto,
	_Chaud_19_VanBas_FanAuto,
	_Chaud_20_VanBas_FanAuto,
	_Chaud_21_VanBas_FanAuto,
	_Chaud_22_VanBas_FanAuto,
	_Chaud_23_VanBas_FanAuto,
	_Stop
}RmDv_TelecoIR_Cmde;

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

float Protocole_ExtractTemperature(char * MssgTempStr);
void Protocole_BuildMssgTemp(char * MssgTempStr, float Temp);
char Protocole_ExtractMssgcode(char * MssgTempStr);
RmDv_TelecoIR_Cmde Protocole_ExtractClimOrder(RmDv_TelecoIR_Cmde * MssgTempStr);
void Protocole_BuildMssgWarning(char * MssgTempStr, RmDv_WarningCode Warning);
void Protocole_BuildMssgError(char * MssgTempStr, RmDv_WkUp_CurrentState ErrorCode);

#endif /* INC_PROTOCOLEFCTS_H_ */
