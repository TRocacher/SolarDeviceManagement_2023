
#include "StandByWkupPgm.h"
#include "main.h"
#include <GLOBAL_RmDv.h>
#include "LowPower_L031.h"

/* =================================================================================
* ==================   Main_StandByWkUpPgm	     ===================================
 *
 *   Created on: August 4, 2024
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *   Revision : 1.00
 *  ------------------------------------------------------------------------------
* =================================================================================*/




int Stop; 											/* Flag qui stoppe l'enchaînement des tâches */
RmDv_WkUp_CurrentState StandByWkUpPgm_CurrentState;	/* Etat courant pour debug*/
RmDv_WarningCode StandByWkUpPgm_WCode;				/* Statut à émettre fin de process */


/***************************************************************
		Définitions des variables messages pour la stack
***************************************************************/

float Temperature;						/* Température mesurée */
char ReceivedMssg[30];					/* Chaîne de réception */
char Long;								/* Longueur de la chaîne */
RmDv_TelecoIR_Cmde ReceivedTempSet, LastSet;		/* Code clim, Consigne de température reçue */
unsigned short int Interval_sec;		/* Intervalle de prochain Wkup */
char Index;	/* pour le test...*/


RmDv_SGw_FSKP_ReqInfoTypedef Req_Data;		/* Structure de donnée requête info*/
RmDv_SGw_FSKP_ReqStatusTypedef Req_Status;	/* Structure de donnée requête Status*/

/* Getter pour exploitation dans callback watchdog (non utilisé encore */
RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}






/***************************************************************
		Programme Main_StandByWkUpPgm
***************************************************************/
void Main_StandByWkUpPgm(void)
{
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
		LowPower_L031_EnableBKP();
		/* Lecture ancienne valeur */
		LastSet=LL_RTC_ReadReg(RTC,BKPReg_TempSet);
		/* Blocage accès BKP Reg */
		LowPower_L031_DisableBKP();

		/* Initialisation du système de gestion des timeout via systick */
		TimeManag_TimeOutInit();
		/* Initialisation de la pile FSK */
		FSKStack_Init(My_);
		/* Chargement variable Requête info*/
		Req_Data.DestAdr = SGw_;
		Req_Data.Temp = Temperature;
		Req_Data.LastSet = LastSet;
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
			Interval_sec = Req_Data.NextInterval;
			/* Enregistrement BKPreg de la nouvelle température dans BKP Reg : maintenant lastTemp...*/
			LowPower_L031_EnableBKP();
			LL_RTC_WriteReg(RTC,BKPReg_TempSet,ReceivedTempSet);
			LowPower_L031_DisableBKP();

			/* Initialisation télécommande IR et émission effective */
			RmDv_TelecoIR_Init();
			RmDv_TelecoIR_SetCmde(ReceivedTempSet);
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
				 Mise à jour prochain intervalle

***************************************************************/

		StandByWkUpPgm_CurrentState=RTCAdjust;
		/* Réglage Période RTC*/
		LowPower_L031_WUTConf(Interval_sec);


/***************************************************************
		  		Warning Mssg :Emission requête status erreur ou pas !
***************************************************************/

	StandByWkUpPgm_CurrentState=WarningMssg;
	/* Chargement variable Requête status*/
	Req_Status.DestAdr = SGw_;
	Req_Status.Status = StandByWkUpPgm_WCode;
	/* Récup ancien état de la SM*/
	LowPower_L031_EnableBKP();
	Req_Status.PreviousState=LL_RTC_ReadReg(RTC,BKPReg_RmDv_State);
	LowPower_L031_DisableBKP();

	Req_Status.TimeOut_ms = RMDV_TimeOutReq;
	Req_Status.TrialMaxNb = RMDV_StatusReqTrialNb;
	Req_Status.TrialActualNb =0;
	Req_Status.success = 0;

	/* Emission requête status*/
	RmDv_SGw_FSKP_ReqStatus(&Req_Status);

	/* Mémorisation du champ Previous state*/
	LowPower_L031_EnableBKP();
	/* Ecriture "tout est OK" pour le prochain run */
	LL_RTC_WriteReg(RTC,BKPReg_RmDv_State,RmDv_SM_OK);
	/* Blocage accès BKP Reg */
	LowPower_L031_DisableBKP();

}


void FactoryReset_StandByWkUpPgm(void)
{
/* Activation électronique + init soft pile*/
	RmDv_EnableBoost;
	Delay_x_ms(50); /* attendre 50ms pour que le ADT7410 se réveille*/
	/* Initialisation du système de gestion des timeout via systick */
	TimeManag_TimeOutInit();
	/* Initialisation de la pile FSK */
	FSKStack_Init(My_);

/* Emission simple du numéro de révision*/
	FSKStack_SendNewMssg (BroadCast,Revision, 8);
	/* attente messg parti : 8 octets, disons 15.
	 * un octet 10bits à 100µs/bit donne 1ms/octets.
	 * Attente 15ms*/
	Delay_x_ms(15);
/* sleep for 10 sec */
	LowPower_L031_WUTConf(10);
}
