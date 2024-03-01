
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




int Stop; 							/* Flag qui stoppe l'enchaînement des tâches */
RmDv_WkUp_CurrentState StandByWkUpPgm_CurrentState;	/* Etat courant pour debug*/
RmDv_WarningCode StandByWkUpPgm_WCode;				/* Statut à émettre fin de process */


/***************************************************************
		Définitions des variables messages pour la stack
***************************************************************/

float Temperature;						/* Température mesurée */
char ReceivedMssg[30];					/* Chaîne de réception */
char Long;								/* Longueur de la chaîne */
char ReceivedCodeClim,ReceivedTempSet;	/* Code clim, Consigne de température reçue */
unsigned short int Interval_sec;		/* Intervalle de prochain Wkup */
char Index;	/* pour le test...*/


RmDv_SGw_FSKP_ReqInfoTypedef Req_Data;		/* Structure de donnée requête info*/
RmDv_SGw_FSKP_ReqStatusTypedef Req_Status;	/* Structure de donnée requête Status*/

/* Getter pour exploitation dans callback watchdog (non utilisé encore */
RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}



/* Programme principal */
void DevPgmWup(void)
{

	Stop=0; /* Par défaut progression OK */
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
		/* Initialisation du système de gestion des timeout via systick */
		TimeManag_TimeOutInit();
		/* Initialisation de la pile FSK */
		FSKStack_Init(My_);
		/* Chargement variable Requête info*/
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
		/* Lancement de la requête... n fois ...*/
		RmDv_SGw_FSKP_ReqInfo(&Req_Data);
		/* pour le test...*/
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
			/* récupération des données de la requête info (température et Intervalle)*/
			ReceivedTempSet = Req_Data.NewSet;
			Interval_sec = RmDv_SGw_FSKP_ExtractNextWupInterval(ReceivedMssg);
			/* Initialisation télécommande IR et émission effective */
			RmDv_TelecoIR_Init();
			if (ReceivedTempSet == 18) ReceivedCodeClim = _Chaud_18_VanBas_FanAuto;
			else if  (ReceivedTempSet == 19) ReceivedCodeClim = _Chaud_19_VanBas_FanAuto;
			else if  (ReceivedTempSet == 20) ReceivedCodeClim = _Chaud_20_VanBas_FanAuto;
			else if  (ReceivedTempSet == 21) ReceivedCodeClim = _Chaud_21_VanBas_FanAuto;
			else if  (ReceivedTempSet == 22) ReceivedCodeClim = _Chaud_22_VanBas_FanAuto;
			else if  (ReceivedTempSet == 23) ReceivedCodeClim = _Chaud_23_VanBas_FanAuto;
			else ReceivedCodeClim = _Stop;
			RmDv_TelecoIR_SetCmde(ReceivedCodeClim);
			/* coupure interruption Timer Teleco*/
			RmDv_TelecoIR_DeInit();
		/***************************************************************
					  		Mise à jour prochain intervalle
					  		Ajustement RTC
		***************************************************************/

			/* A FAIRE !!! */
			StandByWkUpPgm_CurrentState=RTCAdjust;

		}
		else /* Requête info a échoué, on ne va pas plus loin ...*/
		{
			Stop=1;
			/* <--- Actualisation status -->    */
			StandByWkUpPgm_WCode =  Status_Error_NewTempSetNotReceived;
		}
	}


	/***************************************************************
			  		Emission requête status
	***************************************************************/
	if (Stop == 0)
	{
		StandByWkUpPgm_CurrentState=WarningMssg;
		/* Chargement variable Requête status*/
		Req_Status.DestAdr = SGw_;
		Req_Status.Status = StandByWkUpPgm_WCode;
		Req_Status.TimeOut_ms = RMDV_TimeOutReq;
		Req_Status.TrialMaxNb = RMDV_StatusReqTrialNb;
		Req_Status.TrialActualNb =0;
		Req_Status.success = 0;

		/* Emission requête status*/
		RmDv_SGw_FSKP_ReqStatus(&Req_Status);
	}

}



