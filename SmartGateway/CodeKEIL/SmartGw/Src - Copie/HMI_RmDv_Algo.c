#include "HMI_RmDv_Algo.h"


/* =================================================================================
* ==================    HMI_RmDv_Algo.h	     =================================
 *
 *   Created on: 19/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *   Revision : 
 *  ------------------------------------------------------------------------------
 *  Rassemble les algorithmes permettant de répondre aux requêtes des RmDv, c'est à dire :
 *	Elaborer puis fournir la nouvelle consigne de température à  partir de la dernière
 *  mise à jour de la variable HMI_CentralData
* =================================================================================*/
#include "DataFromRmDv.h"
#include "DataFromHMI.h"
#include "TimeStampManagement.h"
#include "HMI_RmDv_Algo.h"
#include "MyLCD.h" /* Pour debug ...*/

/**
  * @brief  Convertit une température en son code commande IR correspondant
  * @Note
  * @param  Température : 18 à 23, 0 si arrêt
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_Temp2CmdeIR(char Temp)
{
	RmDv_TelecoIR_Cmde LocalCmde;
	switch(Temp)
	{
		case 0: LocalCmde=_Stop;break;
		case 17:LocalCmde=_Chaud_18_VanBas_FanAuto;break; /*!! à virer provisoire pour compat HMI*/
		case 18: LocalCmde=_Chaud_18_VanBas_FanAuto;break;
		case 19: LocalCmde=_Chaud_19_VanBas_FanAuto;break;
		case 20: LocalCmde=_Chaud_20_VanBas_FanAuto;break;
		case 21: LocalCmde=_Chaud_21_VanBas_FanAuto;break;
		case 22: LocalCmde=_Chaud_22_VanBas_FanAuto;break;
		case 23: LocalCmde=_Chaud_23_VanBas_FanAuto;break;
		default:
		{
			MyLCD_Set_cursor(0, 0);
			MyLCD_Print("Plantage Fct :");
			MyLCD_Set_cursor(0, 1);
			MyLCD_Print("HMIRmDvAlgo_Temp2CmdeIR");
			while(1); 
		}	
	}
	return LocalCmde;
}

/**
  * @brief  Convertit un code commande IR en sa température correspondante
  * @Note   Si la commande est stop, la température vaut 0
  * @param  Code commande IR
  * @retval  Température (0 si stop)
  **/
char HMIRmDvAlgo_CmdeIR2Temp(RmDv_TelecoIR_Cmde Cmde)
{
	char LocalTemp;
	if (Cmde==_Stop) LocalTemp=0;
	else if (Cmde==_Chaud_18_VanBas_FanAuto) LocalTemp=18;
	else if (Cmde==_Chaud_19_VanBas_FanAuto) LocalTemp=19;
	else if (Cmde==_Chaud_20_VanBas_FanAuto) LocalTemp=20;
	else if (Cmde==_Chaud_21_VanBas_FanAuto) LocalTemp=21;
	else if (Cmde==_Chaud_22_VanBas_FanAuto) LocalTemp=22;
	else if (Cmde==_Chaud_23_VanBas_FanAuto) LocalTemp=23;
	else 
	{
		MyLCD_Set_cursor(0, 0);
		MyLCD_Print("Plantage Fct :");
		MyLCD_Set_cursor(0, 1);
		MyLCD_Print("HMIRmDvAlgo_CmdeIR2Temp");
		while(1); // provisoire...
	}	
	return LocalTemp;
}






/**
  * @brief  Génère la consigne de température en mode Programmation
  * @Note   
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_ProgramMode(char ID)
{
	TimeStampTypedef TargetStamp, Local;
	short int Hour;
	DFH_ProgramModeDataTypedef* AdressCentralData_Prog;
	RmDv_TelecoIR_Cmde LocalCmde;
	int ID_Idx;
	
	/* construction idx de 0 à 4*/
	ID_Idx=(int)ID-(int)ID_Clim_Salon;
	
	/*récupérer le stamp cible*/
	TimeStamp_GetClock(&Local);
	RmDvData_CalculateStampTarget(ID,&Local,&TargetStamp);
	/*récupération de l'heure cible */
	Hour = TargetStamp.Hour;
	/* Récupération de l'@ du champ de type DFH_ProgramModeDataTypedef*/
	AdressCentralData_Prog=DFH_GetCentralData_ProgParam();
	
	
	/* Principe :
	la donnée est constituée comme suit :
	typedef struct 
{
	char Temperature_6h[4];					
	char Temperature_8h[4];
	char Temperature_10h[4];	
	char Temperature_15h[4];	
	char Temperature_18h[4];
	char Temperature_23h[4];		
}DFH_ProgramModeDataTypedef;
	
	On va donc chercher l'information température en fonction de l'heure en commençant par 
	un if heure >= 23 h exclusif. Chaque clim est filtrée par son ID.
	Rem : La fonction HMIRmDvAlgo_Temp2CmdeIR permet d'obtenir le code IR en fonction de la 
	température en °C (sauf 0 qui veut dire stop)
	
	
*/
	
	if (Hour>=23) 
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_23h[ID_Idx]);
	}
	else if (Hour>=18)
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_18h[ID_Idx]);
	}
	else if (Hour>=15)
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_15h[ID_Idx]);
	}
	else if (Hour>=10)
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_10h[ID_Idx]);
	}
	else if (Hour>=8)
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_8h[ID_Idx]);
	}
	else if (Hour>=6)
	{
		LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(AdressCentralData_Prog->Temperature_6h[ID_Idx]);
	}
	else 
	{
		MyLCD_Set_cursor(0, 0);
		MyLCD_Print("Plantage Fct :");
		MyLCD_Set_cursor(0, 1);
		MyLCD_Print("HMIRmDvAlgo_ProgramMode");
		while(1); // provisoire...
	}	
		
	return LocalCmde;
}



/**
  * @brief  Génère la consigne de température en fonction du mode spécifié dans la CentralData
  * @Note
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_ComputeTempCmde(char ID)
{
	DFH_HMIMode* AdressCentralData_Mode;
	RmDv_TelecoIR_Cmde Localcmde;
	
	/*Lecture du Mode dans CentralData*/
	AdressCentralData_Mode=DFH_GetCentralData_Mode();
	if (*AdressCentralData_Mode == HMI_Mode_Program) Localcmde=HMIRmDvAlgo_ProgramMode(ID);
	else 
	{
		MyLCD_Set_cursor(0, 0);
		MyLCD_Print("Plantage Fct :");
		MyLCD_Set_cursor(0, 1);
		MyLCD_Print("HMIRmDvAlgo_ComputeTempCmde");
		while(1); // provisoire...
	}		
	
	
	return Localcmde;
}


