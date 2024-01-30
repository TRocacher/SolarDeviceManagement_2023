
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

#define Test_BoostEtLED
//#define Test_LED // attention ds ce cas on configure ppull et non alt
				 // Commenter pour test final !!!!
#define Test_Temp
//#define Test_FSK_Stack



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
#ifdef Test_LED
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
#endif


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
#ifdef Test_BoostEtLED
	RmDv_EnableBoost;
	Delay_x_ms(50); /* attendre 50ms pour que le ADT7410 se réveille*/
#endif

#ifdef Test_LED
	  /*LED_IR  Output PushPull (FT 5V)*/
	  GPIO_InitStruct.Pin = LED_IR_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
	  LL_GPIO_Init(LED_IR_GPIO_Port, &GPIO_InitStruct);



	while(1)
	{
		RmDv_LED_On;
		Delay_x_ms(200);
		RmDv_LED_Off;
		Delay_x_ms(200);
	}
#endif

	/***************************************************************
	  		Mesure température
	***************************************************************/
#ifdef Test_Temp
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
#endif



#ifdef Test_FSK_Stack

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
		 FSKStack_Init(My);
		 //FSKStack_StartFSM(); devenu inutile car inclu ds Init (ligne dessus)


		 Stop=1; /* On stoppe par défaut*/
		 for (i=0;i<3;i++)
		 {
			 FSKStack_SendNewMssg (UC_Adress,TransmitMssg, 5);
			 TimeManag_TimeOutStart(Chrono_3 , 100);
			 while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
			 {
				 if (FSKStack_IsNewMssg()==1)
				 {
					 Long=FSKStack_GetLen();
					 FSKStack_GetNewMssg(ReceivedMssg, Long);
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

	if (Stop==0) /* on poursuit la transaction*/
	{
		if (Protocole_ExtractMssgcode(ReceivedMssg)==MssgTimeClimOrderCode)
	 	{
			StandByWkUpPgm_CurrentState=ClimUpdate;
		 	ReceivedCodeClim=Protocole_ExtractClimOrder(ReceivedMssg);

		 	// shunt réponse :
		 	ReceivedCodeClim=_Chaud_18_VanBas_FanAuto;

		 	RmDv_TelecoIR_Init();
		 	RmDv_TelecoIR_SetCmde(ReceivedCodeClim);
		 	RmDv_TelecoIR_DeInit();
	 	}
	 	else
	 	{
	 		StandByWkUpPgm_WCode=WrongCmdeWhenReceivingTimeClimCode;
	 	}

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

	Stop=1; /* On stoppe par défaut*/
	for (i=0;i<3;i++)
	{
		FSKStack_SendNewMssg (UC_Adress,TransmitMssg, 2);
		TimeManag_TimeOutStart(Chrono_3 , 100);
		while(TimeManag_GetTimeOutStatus(Chrono_3)==0)
		{
			if (FSKStack_IsNewMssg()==1)
			{
				Long=FSKStack_GetLen();
				FSKStack_GetNewMssg(ReceivedMssg, Long);
				Stop=0;
				break;
			}
		}
		if (Stop==0) break;
	}
	/***************************************************************
				go sleep standby (ds le main)
	***************************************************************/

#endif



}



