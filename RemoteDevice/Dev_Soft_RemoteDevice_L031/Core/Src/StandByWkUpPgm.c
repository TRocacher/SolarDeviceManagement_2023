
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
float Temperature;

RmDv_WkUp_CurrentState StandByWkUpPgm_CurrentState;
RmDv_WarningCode StandByWkUpPgm_WCode;


/***************************************************************
		Définitions des variables messages pour la stack
***************************************************************/
char TransmitMssg[30];
char ReceivedMssg[30];
char Long;
char ReceivedCodeClim;

RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
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
		StandByWkUpPgm_WCode=Temp_Error;
	}

	Protocole_BuildMssgTemp(TransmitMssg, Temperature);



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
		 MACPhyUART_Init(My);
		 MACPhyUART_StartFSM();


		 Stop=1; /* On stoppe par défaut*/
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
		 if (Stop==1) StandByWkUpPgm_WCode=Transm_Error_NoTimeClimCodeReceived;
	 }

	/***************************************************************
			Mise à jour Climatiseur
	***************************************************************/

//////// TRUCAGE ///////////
	Stop=0;
//////// FIN TRUCAGE ///////////
	if (Stop==0) /* on poursuit la transaction*/
	{
		//////// TRUCAGE ///////////	 	if (Protocole_ExtractMssgcode(ReceivedMssg)==MssgTimeClimOrderCode)
	 	{
			StandByWkUpPgm_CurrentState=ClimUpdate;
		 	ReceivedCodeClim=Protocole_ExtractClimOrder(ReceivedMssg);
		 	RmDv_TelecoIR_Init();
		 	//RmDv_TelecoIR_SetCmde(ReceivedCodeClim);
		 	RmDv_TelecoIR_SetCmde(_Stop);
		 	RmDv_TelecoIR_DeInit();
	 	}
	 	//////// TRUCAGE ///////////	 	else
	 	//////// TRUCAGE ///////////	 	{
	 	//////// TRUCAGE ///////////	 		StandByWkUpPgm_WCode=WrongCmdeWhenReceivingTimeClimCode;
	 	//////// TRUCAGE ///////////	 		Stop=1;
	 	//////// TRUCAGE ///////////	 	}

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
	Protocole_BuildMssgWarning(TransmitMssg, StandByWkUpPgm_WCode);

	for (i=0;i<3;i++)
	{
		MACPhyUART_SendNewMssg (UC_Adress,TransmitMssg, 2);
		TimeManag_TimeOutStart(Chrono_3 , 100);
		while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
		{
			if (MACPhyUART_IsNewMssg()==1)
			{
				Long=MACPhyUART_GetLen();
				MACPhyUART_GetNewMssg(ReceivedMssg, Long);
				break;
			}
		}
	}
	/***************************************************************
				go sleep standby (ds le main)
	***************************************************************/





}



