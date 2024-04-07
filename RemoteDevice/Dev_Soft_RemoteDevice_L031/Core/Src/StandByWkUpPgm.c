
#include "StandByWkupPgm.h"
#include "main.h"
#include <GLOBAL_RmDv.h>

/* =================================================================================
* ==================   Main_StandByWkUpPgm	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 * BackupRegister :
 * 	BKP0R (8 bits utilisés sur les 32) = Consigne de températur
 * 		BKPOR <- Consigne du SGw
 * 		BKP0R -> Last consigne.
 *
 *
 *	Le test :
 *	 on envoie la valeur du BKReg BKP0R
 *	 on y ajoute +1 et on mémorise pour le prochain coup.
 *	 Wup toutes les 2 secondes.

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
int Interval_sec;						/* Intervalle de prochain Wkup */
char Index;	/* pour le test...*/


RmDv_SGw_FSKP_ReqInfoTypedef Req_Data;		/* Structure de donnée requête info*/
RmDv_SGw_FSKP_ReqStatusTypedef Req_Status;	/* Structure de donnée requête Status*/

/* Getter pour exploitation dans callback watchdog (non utilisé encore */
RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}



/* Programme principal */
void Main_StandByWkUpPgm(void)
{
	char Test;
	char TransactionIdx;

	Interval_sec=30*60;			/* si pas de réponse, on recommence au bout de 30mn
	 	 	 	 	 	 	 	 	 faire un truc plus malin qui dise qu'il y a eu un bug*/
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
		/* Donner accès au BKP reg */
		LL_PWR_EnableBkUpAccess();
		LL_RTC_DisableWriteProtection(RTC);
		/* Lecture ancienne valeur */
		Test=(char)LL_RTC_ReadReg(RTC,BKPReg_TempSet);
		/* pour le test...*/
		Test = (Test+1)%50; /* modulo 50 car pb affichage LCD...*/
		/* Lecture next TransIdx */
		TransactionIdx = (char)LL_RTC_ReadReg(RTC,BKPReg_NextTransIdx);
		/* Blocage accès BKP Reg */
		LL_PWR_DisableBkUpAccess();
		LL_RTC_EnableWriteProtection(RTC);

		/* Initialisation du système de gestion des timeout via systick */
		TimeManag_TimeOutInit();
		/* Initialisation de la pile FSK */
		FSKStack_Init(My_);
		/* Chargement variable Requête info*/
		Req_Data.DestAdr = SGw_;
		Req_Data.TransIdx =TransactionIdx;
		Req_Data.Temp = Temperature;
		Req_Data.LastSet = Test;
		Req_Data.TimeOut_ms = RMDV_TimeOutReq;
		Req_Data.TrialMaxNb = RMDV_InfoReqTrialNb;
		Req_Data.TrialActualNb =0;
		Req_Data.success = 0;
		Req_Data.NewSet = 0;
		Req_Data.NextInterval = 0;

		/* Lancement de la requête... n fois ...*/
		RmDv_SGw_FSKP_ReqInfo(&Req_Data);


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
			Interval_sec = Req_Data.NextInterval;	/* récupération de l'intervalle*/
			TransactionIdx = Req_Data.TransIdx;		/* récupération du prochain Idx de transac*/

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


		}
		else /* Requête info a échoué, on ne va pas plus loin ...*/
		{
			Stop=1;
			/* <--- Actualisation status -->    */
			StandByWkUpPgm_WCode =  Status_Error_NewTempSetNotReceived;
		}
	}


	/***************************************************************
			  		Emission requête status erreur ou pas !
	***************************************************************/
//	if (Stop == 0)
//	{
		StandByWkUpPgm_CurrentState=WarningMssg;
		/* Chargement variable Requête status*/
		Req_Status.DestAdr = SGw_;
		Req_Status.TransIdx =TransactionIdx;
		Req_Status.Status = StandByWkUpPgm_WCode;
		Req_Status.TimeOut_ms = RMDV_TimeOutReq;
		Req_Status.TrialMaxNb = RMDV_StatusReqTrialNb;
		Req_Status.TrialActualNb =0;
		Req_Status.success = 0;

		/* Emission requête status*/
		RmDv_SGw_FSKP_ReqStatus(&Req_Status);

//	}

		/***************************************************************
					  		Mise à jour prochain intervalle
					  		Ajustement RTC (5 sec ou 5mn selon valeurs pgmée
					  		en cas de non communication
					  		sinon valeur reçue
		***************************************************************/

		StandByWkUpPgm_CurrentState=RTCAdjust;
		/* Donner accès au BKP reg */
		LL_PWR_EnableBkUpAccess();
		LL_RTC_DisableWriteProtection(RTC);
		/* Ecriture new val */ /* pour le test*/
		LL_RTC_WriteReg(RTC,BKPReg_TempSet,Test);
		/* Mise à jour du bkp reg pour next wup delay */
		LL_RTC_WriteReg(RTC,BKPReg_NextDelay_sec,Interval_sec);
		/* Mise à jour du bkp reg pour next Idx de transaction */
		LL_RTC_WriteReg(RTC,BKPReg_NextTransIdx,TransactionIdx);


		/* Blocage accès BKP Reg */
		LL_PWR_DisableBkUpAccess();
		LL_RTC_EnableWriteProtection(RTC);


}



