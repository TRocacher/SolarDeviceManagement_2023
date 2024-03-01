
#include "StandByWkupPgm.h"




/* =================================================================================
* ==================   Main_StandByWkUpPgm	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 *

* =================================================================================*/



int i,j,k; /* Indice boucle*/
int Error;
int Stop; /* Si 1 on arrête là le processus */


RmDv_WkUp_CurrentState StandByWkUpPgm_CurrentState;
RmDv_WarningCode StandByWkUpPgm_WCode;


/***************************************************************
		Définitions des variables messages pour la stack
***************************************************************/
//char TransmitMssg[30];
float Temperature;
char ReceivedMssg[30];
char Long;
char ReceivedCodeClim,ReceivedTempSet;
unsigned short int Interval_sec;
char Index;


RmDv_SGw_FSKP_ReqInfoTypedef Req_Data;
RmDv_SGw_FSKP_ReqStatusTypedef Req_Status;

RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}


void DevPgmWup(void)
{

	Stop=0;
	Error=0;
	StandByWkUpPgm_CurrentState=BoostActivation;
	/* <--- Actualisation status -->    */
	StandByWkUpPgm_WCode=Status_NoWarning;

	/***************************************************************
	  		Activation Boost 3V -> 5V -> 3,3V
	  		-> Alimentation 3,3V pour ADT7410, OPAmp µphone,
	  		OpAmp LED IR Xbee (si câblé)
	  		-> Alimentation 5V pour RT606 (FSK, si câblé)
	***************************************************************/
	RmDv_EnableBoost;
	Delay_x_ms(50); /* attendre 50ms pour que le ADT7410 se réveille*/

	/***************************************************************
	  		Mesure température
	***************************************************************/
	StandByWkUpPgm_CurrentState=TemperatureMeasure;
	ADT7410_Init();
	Temperature=ADT7410_GetTemp_float();
	if (Temperature>-100.0) Stop=0;/* pas d'erreur*/
	else
	{
		Stop=1;
		/* <--- Actualisation status -->    */
		StandByWkUpPgm_WCode = Status_Error_TempI2C;
	}

	/***************************************************************
		  		Emission requête information
	 ***************************************************************/
	if (Stop == 0)
	{
		StandByWkUpPgm_CurrentState=WakeUpMssgToUC;
		TimeManag_TimeOutInit();
		FSKStack_Init(My_);
		/* Chargement variable Requête, ici requête info*/
		Req_Data.DestAdr = SGw_;
		Req_Data.Temp = Temperature;
		Req_Data.LastSet = 19;
		Req_Data.TimeOut_ms = RMDV_TimeOutReq;
		Req_Data.TrialMaxNb = RMDV_InfoReqTrialNb;
		Req_Data.TrialActualNb =0;
		Req_Data.success = 0;
		Req_Data.NewSet = 0;
		Req_Data.NextInterval = 0;

		Req_Data.LastSet = Index;
		RmDv_SGw_FSKP_ReqInfo(&Req_Data);
		Index++;
		/***************************************************************
				  		Mise à jour clim
		***************************************************************/
		if (Req_Data.success == 1)
		{
			/* <--- Actualisation status -->    */
			StandByWkUpPgm_WCode = Req_Data.TrialActualNb;
			Stop=0;
			StandByWkUpPgm_CurrentState=ClimUpdate;
			ReceivedTempSet = Req_Data.NewSet;
			Interval_sec = RmDv_SGw_FSKP_ExtractNextWupInterval(ReceivedMssg);
			RmDv_TelecoIR_Init();
			if (ReceivedTempSet == 18) ReceivedCodeClim = _Chaud_18_VanBas_FanAuto;
			else if  (ReceivedTempSet == 19) ReceivedCodeClim = _Chaud_19_VanBas_FanAuto;
			else if  (ReceivedTempSet == 20) ReceivedCodeClim = _Chaud_20_VanBas_FanAuto;
			else if  (ReceivedTempSet == 21) ReceivedCodeClim = _Chaud_21_VanBas_FanAuto;
			else if  (ReceivedTempSet == 22) ReceivedCodeClim = _Chaud_22_VanBas_FanAuto;
			else if  (ReceivedTempSet == 23) ReceivedCodeClim = _Chaud_23_VanBas_FanAuto;
			else ReceivedCodeClim = _Stop;
			RmDv_TelecoIR_SetCmde(ReceivedCodeClim);
			RmDv_TelecoIR_DeInit();
		/***************************************************************
					  		Mise à jour prochain intervalle
					  		Ajustement RTC
		***************************************************************/

			StandByWkUpPgm_CurrentState=RTCAdjust;

		}
		else
		{
			Stop=1;
			/* <--- Actualisation status -->    */
			StandByWkUpPgm_WCode =  Status_Error_NewTempSetNotReceived;
		}
	}

	/***************************************************************
			  		Emission requête status
	***************************************************************/
	StandByWkUpPgm_CurrentState=WarningMssg;
	/* Chargement variable Requête, ici requête status*/
	Req_Status.DestAdr = SGw_;
	Req_Status.Status = StandByWkUpPgm_WCode;
	Req_Status.TimeOut_ms = RMDV_TimeOutReq;
	Req_Status.TrialMaxNb = RMDV_StatusReqTrialNb;
	Req_Status.TrialActualNb =0;
	Req_Status.success = 0;

	RmDv_SGw_FSKP_ReqStatus(&Req_Status);

}



