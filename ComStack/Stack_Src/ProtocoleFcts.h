/*
 * ExchangeLayer.h
 *
 *  Created on: 30 août 2023
 *      Author: trocache
 */

#ifndef INC_PROTOCOLEFCTS_H_
#define INC_PROTOCOLEFCTS_H_


/* Donne l'état courant de la phase de réveil
 * Utilisé si WDog pour identifier le lieu du plantage*/
typedef enum {
	BoostActivation=0,
	TemperatureMeasure=1,
	WakeUpMssgToUC=2,
	ClimUpdate=3,
	RTCAdjust=4,
	WarningMssg=5,
}RmDv_WkUp_CurrentState;

/* Liste des code de télécommande */
typedef enum {
	_Chaud_18_VanBas_FanAuto = 0xC1,
	_Chaud_19_VanBas_FanAuto = 0xC2,
	_Chaud_20_VanBas_FanAuto = 0xC3,
	_Chaud_21_VanBas_FanAuto = 0xC4,
	_Chaud_22_VanBas_FanAuto = 0xC5,
	_Chaud_23_VanBas_FanAuto = 0xC6,
	_Stop = 0xC0,
}RmDv_TelecoIR_Cmde;

/* Liste des warnings, fonctionnement normal*/
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

			|MssgWarningCode|RmDv_WarningCode|

			Ack   		|Value = no value
	*/

/* Liste des des codes d'identification de trame*/
typedef enum {
	MssgWarningCode=100,
	MssgTempCode=101,
	MssgTimeClimOrderCode=102,
	MssgAckCode=103,
	MssgErrorCode=104,

}MssgCode;

MssgCode Protocole_ExtractMssgcode(char * MssgTempStr);
float Protocole_ExtractTemperature(char * MssgTempStr);
RmDv_TelecoIR_Cmde Protocole_ExtractClimOrder(char * MssgTempStr);
RmDv_WarningCode Protocole_ExtractWarningCode(char * MssgTempStr);

void Protocole_BuildMssgAck(char * MssgTempStr);
void Protocole_BuildMssgTemp(char * MssgTempStr, float Temp);
void Protocole_BuildMssgAck(char * MssgTempStr);
void Protocole_BuildMssgWarning(char * MssgTempStr, RmDv_WarningCode Warning);
void Protocole_BuildMssgError(char * MssgTempStr, RmDv_WkUp_CurrentState ErrorCode);
void Protocole_BuildMssgTelecoHeure(char * MssgStr, RmDv_TelecoIR_Cmde Cmde);

#endif /* INC_PROTOCOLEFCTS_H_ */
