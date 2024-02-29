
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


RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}


void DevPgmWup(void)
{

	Stop=0;
	Error=0;
	StandByWkUpPgm_CurrentState=BoostActivation;
	StandByWkUpPgm_WCode=NoWarning;

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
		StandByWkUpPgm_WCode = Error_TempI2C;
	}

	/***************************************************************
		  		Emission régulière
		***************************************************************/
	StandByWkUpPgm_CurrentState=WakeUpMssgToUC;
	TimeManag_TimeOutInit();
	FSKStack_Init(My_);
	/* Chargement variable Requête, ici requête info*/
	Req_Data.DestAdr = SGw_;
	Req_Data.Temp = Temperature;
	Req_Data.LastSet = 19;
	Req_Data.TimeOut_ms = 200;
	Req_Data.TrialMaxNb = 10;
	Req_Data.TrialActualNb =0;
	Req_Data.success = 0;
	Req_Data.NewSet = 0;
	Req_Data.NextInterval = 0;



	Req_Data.LastSet = Index;
	RmDv_SGw_FSKP_ReqInfo(&Req_Data);
	Index++;
	if (Req_Data.success == 1)
	{
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
	}


}






void Main_StandByWkUpPgm(void)
{
	int i; /* Boucle*/
	Stop=0;
	Error=0;
	StandByWkUpPgm_CurrentState=BoostActivation;
	StandByWkUpPgm_WCode=NoWarning;

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
		StandByWkUpPgm_WCode = Error_TempI2C;
	}


	 /***************************************************************
			Wakeup Emission Info UC Temperature. 3 tentatives.
			Wait for  Nouvel Ordre Clim, Heure

			Trame :
			|Code 		| Value |

			Error 		|Value = code erreur string
			Temp 		|Value = float brut
			Time  		|Value = String formaté HH:Mn:Sec
			ClimOrder 	|Value = enum|
			Ack   		|Value = no value
	***************************************************************/
	 if (Stop==0) /* on poursuit la transaction*/
	 {
		 StandByWkUpPgm_CurrentState=WakeUpMssgToUC;
		 TimeManag_TimeOutInit();
		 FSKStack_Init(My_);
		 //FSKStack_StartFSM(); devenu inutile car inclu ds Init (ligne dessus)

		 Stop=1; /* On stoppe par défaut*/
		 for (i=0;i<3;i++)
		 {
			 RmDv_SGw_FSKP_SendMssgReq_SendInfo(SGw_, Temperature, 19);
			 //TimeManag_TimeOutStart(Chrono_3 , TimeOutProtocole_ms);
			 while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
			 {
				 if (FSKStack_IsNewMssg()==1)
				 {
					 Long=FSKStack_GetLen();
					 FSKStack_GetNewMssg(ReceivedMssg, Long);
					 if (RmDv_SGw_FSKP_ExtractMssgcode(ReceivedMssg)==MssgAns_SendInfo)
					 {
						 Stop=0;
						 break;
					 }
				 }
			 }
			 if (Stop==0) break;
			 else StandByWkUpPgm_WCode=Transm_1_Attempt+i;
		 }
		 if (Stop==1) StandByWkUpPgm_WCode=Error_NewTempSetNotReceived;
	 }

	/***************************************************************
			Mise à jour Climatiseur
	***************************************************************/

	if (Stop==0) /* on poursuit la transaction, la chaine est une ans info*/
	{

		StandByWkUpPgm_CurrentState=ClimUpdate;
		ReceivedTempSet = RmDv_SGw_FSKP_ExtracNewTempSet(ReceivedMssg);
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

	}

	/***************************************************************
				Ajustement RTC
	***************************************************************/
	 StandByWkUpPgm_CurrentState=RTCAdjust;


	/***************************************************************
				Statut final Wkup phase
				Emission Warning
				Attente retour quelqu'il soit
	***************************************************************/
	StandByWkUpPgm_CurrentState=WarningMssg;

	Stop=1; /* On stoppe par défaut*/
	for (i=0;i<3;i++)
	{
		RmDv_SGw_FSKP_SenddMssgReq_SendStatus(SGw_,  StandByWkUpPgm_WCode);
		//TimeManag_TimeOutStart(Chrono_3 , TimeOutProtocole_ms);
		while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
		{
			if (FSKStack_IsNewMssg()==1)
			{
				Long=FSKStack_GetLen();
				FSKStack_GetNewMssg(ReceivedMssg, Long);
				if (RmDv_SGw_FSKP_ExtractMssgcode(ReceivedMssg)==MssgAns_Ack)
				{
					Stop=0;
					break;
				}
			}
		}
		if (Stop==0) break;
	}
	/***************************************************************
				go sleep standby (ds le main)
	***************************************************************/





}



