#include "HMI_RmDv_Algo.h"


/* =================================================================================
* ==================    HMI_RmDv_Algo.h	     =================================
 *
 *   Created on: 19/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : _
 *   Revision : 
 *  ------------------------------------------------------------------------------
 *  Rassemble les algorithmes permettant de r�pondre aux requ�tes des RmDv, c'est � dire :
 *	Elaborer puis fournir la nouvelle consigne de temp�rature �  partir de la derni�re
 *  mise � jour de la variable HMI_CentralData
* =================================================================================*/
#include "DataFromRmDv.h"
#include "DataFromHMI.h"
#include "TimeStampManagement.h"
#include "HMI_RmDv_Algo.h"
#include "MyLCD.h" /* Pour debug ...*/


RmDv_TelecoIR_Cmde HMIRmDvAlgo_ProgramMode(char ID);
RmDv_TelecoIR_Cmde HMIRmDvAlgo_AutoMode(char ID);
RmDv_TelecoIR_Cmde HMIRmDvAlgo_Temp2CmdeIR(char Temp);
char HMIRmDvAlgo_CmdeIR2Temp(RmDv_TelecoIR_Cmde Cmde);

			
struct 
{
	char ProgTemp[4];				/* correspond aux temp�ratures programm�es sur le slot donn�*/
	char CurrentTemp[4];	 	/* correspond aux temp�ratures effectives sur les clim*/
	char CurrentBoost[4];		/* indique les qt� de Boost correspondant � chaque clim, 0, 1 ou 2*/
	char TempSet[4];				/* les temp�ratures calcul�es, � r�cup�rer lors des prochaines transactions RmDv*/
	char Prio[4];						/* contient la priorit� pour chaque clim*/
	
	int CurrentBoostTokenNb;	/*Somme de tous les jetons Boost mis en jeu en ce moment*/
	int DeltaBoostTokenNb;		/*la qt� de boost disponible en fonction de l'ensoleillement*/
	int DesiredBoostTokenNb;	/*la qt� totale voulue pour la nouvelle programmation des clims*/
	int BoostTokenNbMax;			/*la qt� maximale de Boost possible dans la slot de prog actuel*/

}HMIRmDvAlgo_AutoData;	
	



/**
  * @brief  Fonction qui remplit tous les champs
	de la grosse variable HMIRmDvAlgo_AutoData
	notamment les temperatures voulues (TempSet)
	Cette fonction est appel�e normalement par l'IT
	HMI.
  * @Note   
  * @param  _
  * @retval _
  **/
void HMIRmDvAlgo_AutoModeDataUpdateFromHMI(void)
{
	int i,j, IndiceDuMin; 															/* indice � tout  faire */
	char Min;																						/*utilis� pour recherche de min*/
	int	Prog, Current; 																	/* temp�rature de programmation et courrante */
	int	TokenNb; 																				/* nombre de jeton*/
	int	ClimOffNb, ClimOnNb; 														/*  */
	TimeStampTypedef  Local; 														/* Stamp RmDv (Local) et Stamp vis� pour next �change*/
	HourStampTypedef LocalHour, MinuteOffset; 					/*Stamp heure pour ajuster au mieux l'intervalle
																											de temps correspondant en mode programm�*/
	short int Hour;																			/* variable heure locale pour le if else*/
	char * Temperature_ProgIntervalPtr; 								/* Pointeur pour pouvoir lire l'intervalle horaire
																											voulu contenant les consignes programm�es des 4 clim*/
	DFH_ProgramModeDataTypedef* AdressCentralData_Prog; /* pointeur sur la sous structure Prog de
																											AdressCentralData*/
	DFH_AutoModeDataTypedef* AdressCentralAutoModData; 	/* pointeur sur les donn�es centrale du mode Auto*/
	RmDvDataTypedef* RmDvPtr;														/*Ptr pour acc�der aux data RmDv*/
	DFH_PowDataOptTypedef* PowOptDataPtr;								/*Ptr pour acc�der aux donn�es de puissances et opt*/
	float Power,PowerStart,PowerStop;															/* Puissance disponible, de d�marrage, d'arr�t*/
	
	
	/*=============================================================================
		INITIALISATION de la structure HMIRmDvAlgo_AutoData
	==============================================================================*/
	
	/*--------------------------------
	R�cup�ration des temp�rature prog
	du slot en cous
	HMIRmDvAlgo_AutoData.ProgTemp[4]
	et
	char Prio[4];
	--------------------------------*/
	
	/*Stamp now */
	TimeStamp_GetClock(&Local);
	/*ajout de 7mn pour �tre certain de tomber dans le bon intervalle
	Il faut �viter un stamp HMI � 5h55mn qui en fait correspond � 6h et plus.
	On ajoute donc 7mn pour �tre tranquille. On se trouve alors dans l'intervalle � venir*/
	LocalHour.Sec =Local.Sec;
	LocalHour.Min =Local.Min;
	LocalHour.Hour =Local.Hour;
	MinuteOffset.Sec=0;
	MinuteOffset.Min =7;
	MinuteOffset.Hour=0;
	HourStampAdd(&LocalHour, &MinuteOffset ); /* Stamp + 7mn*/
	Hour = LocalHour.Hour;										/*heure stamp qui cadre bien dans un intervalle*/
	/* R�cup�ration de l'@ du champ de type DFH_ProgramModeDataTypedef*/
	AdressCentralData_Prog=DFH_GetCentralData_ProgParam();
	
	/* D�termination de l'intervalle horaire dans lequel le stamp appartient*/
	if (Hour>=23) Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_23h;
	else if (Hour>=18) Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_18h;
	else if (Hour>=15) Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_15h;
	else if (Hour>=10) Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_10h;
	else if (Hour>=8) Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_8h;
	else if (Hour>=6)  Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_6h;
	else /* stamp 00h et 5h0*/
	{
		Temperature_ProgIntervalPtr=AdressCentralData_Prog->Temperature_23h;
	}
	AdressCentralAutoModData= DFH_GetCentralData_AutoParam();
	for (i=0;i<4;i++)
	{
		/*MAJ ProgTemp*/
		HMIRmDvAlgo_AutoData.ProgTemp[i]=Temperature_ProgIntervalPtr[i];
		/*MAJ ClimPrio*/
		HMIRmDvAlgo_AutoData.Prio[i]=AdressCentralAutoModData->ClimPrio[i];
	}
	

	/*--------------------------------
	R�cup�ration des temp�ratures 
	actuellement sur les clim
	-> lecture RmDv_Data, remplissage 
	HMIRmDvAlgo_AutoData.CurrentTemp[4]
	--------------------------------*/
	for (i=0;i<4;i++)
	{
		RmDvPtr=RmDvData_GetObjectAdress(ID_Clim_Salon+i);
		if (RmDvPtr->LastTempSet == _NoCommandToSend) /*on r�cup�re la temp d'avant...*/
		{
			/*MAJ CurrentTemp*/
			HMIRmDvAlgo_AutoData.CurrentTemp[i]= \
			HMIRmDvAlgo_CmdeIR2Temp(RmDvPtr->LastTempSetBeforeNoCommand);
		}
		else
		{
			/*MAJ CurrentTemp*/
			HMIRmDvAlgo_AutoData.CurrentTemp[i]= \
			HMIRmDvAlgo_CmdeIR2Temp(RmDvPtr->LastTempSet);
		}
	}

	
	/*--------------------------------
	*D�termination des qt� de jeton boost
	par clim
	-> HMIRmDvAlgo_AutoData.CurrentBoost[4]
	*D�termination du CurrentBoostTokentNb (total)
	--------------------------------*/
	HMIRmDvAlgo_AutoData.CurrentBoostTokenNb=0;
	
	for (i=0;i<4;i++)
	{
		Prog=HMIRmDvAlgo_AutoData.ProgTemp[i];
		Current=HMIRmDvAlgo_AutoData.CurrentTemp[i];
		TokenNb=0;
		/* deux if =clim qui �tait �teintes*/
		if ((Prog==0) && (Current == TemperatureOn)) TokenNb=TokenNb+1;
		if ((Prog==0) && (Current == (TemperatureOn+TemperatureIncBoost))) TokenNb=TokenNb+2;
		/* if suivant = clim qui �tait en service en mode prog*/
		if ((Prog!=0) && ((Current - Prog)==TemperatureIncBoost)) TokenNb=TokenNb+1;
		
		HMIRmDvAlgo_AutoData.CurrentBoost[i]=TokenNb;
		HMIRmDvAlgo_AutoData.CurrentBoostTokenNb+=TokenNb;
	}
	
	/*--------------------------------
	*D�termination du nombre max de boost
	-> HMIRmDvAlgo_AutoData.BoostTokenNbMax
	--------------------------------*/
	ClimOnNb=0;
	for (i=0;i<4;i++)
	{
		if (HMIRmDvAlgo_AutoData.ProgTemp[i]==0) ClimOnNb++;
	}
	/*MAJ BoostTokenNbMax*/
	HMIRmDvAlgo_AutoData.BoostTokenNbMax=2*ClimOnNb+(4-ClimOnNb);
	
	
	/*=============================================================================
		CALCULS de la structure HMIRmDvAlgo_AutoData
	==============================================================================*/
	/*--------------------------------
	*D�termination du DeltaBoostNb, DesiredBoostTokenNb en fonction
	de la puissance re�ue
	--------------------------------*/
	PowOptDataPtr=DFH_GetCentralData_OptPowerData();
	Power=PowOptDataPtr->PowExcess;
	PowerStop=AdressCentralAutoModData->PowExcessStop;
	PowerStart= AdressCentralAutoModData->PowExcessStart;
	/*NB : BoostNb est en r�at� un Delta ! la var �tait disponible...*/
	if (Power>(PowerStart+5*PowerBoostQuantum)) TokenNb=6;				/*[ > Pstart + 5PQ]*/
	else if (Power>(PowerStart+4*PowerBoostQuantum)) TokenNb=5;		/*]Pstart + 4PQ ; Pstart + 5PQ]*/
	else if (Power>(PowerStart+3*PowerBoostQuantum)) TokenNb=4;		/*]Pstart + 3PQ ; Pstart + 4PQ]*/
	else if (Power>(PowerStart+2*PowerBoostQuantum)) TokenNb=3;   /*]Pstart + 2PQ ; Pstart + 3PQ]*/
	else if (Power>(PowerStart+PowerBoostQuantum)) TokenNb=2;			/*]Pstart + PQ ; Pstart + 2PQ]*/
	else if (Power>PowerStart) TokenNb=1;													/*]Pstart  ; Pstart + 1PQ]*/
	else if (Power>PowerStop) TokenNb=0;													/*]Pstop  ; Pstart ]*/	
	else if (Power>PowerStop-PowerBoostQuantum) TokenNb=-1;				/*]Pstop-PQ  ; Pstop ]*/
	else if (Power>PowerStop-2*PowerBoostQuantum) TokenNb=-2;			/*]Pstop-2PQ  ; Pstop-PQ ]*/
	else if (Power>PowerStop-3*PowerBoostQuantum) TokenNb=-3;			/*]Pstop-3PQ  ; Pstop-2PQ ]*/
	else if (Power>PowerStop-4*PowerBoostQuantum) TokenNb=-4;			/*]Pstop-4PQ  ; Pstop-3PQ ]*/
	else if (Power>PowerStop-5*PowerBoostQuantum) TokenNb=-5;			/*]Pstop-5PQ  ; Pstop-4PQ ]*/
	else TokenNb=-6;																							/*] <= Pstop-5PQ ] */
	/*MAJ DeltaBoostTokenNb*/
	HMIRmDvAlgo_AutoData.DeltaBoostTokenNb=TokenNb;
	i= TokenNb+HMIRmDvAlgo_AutoData.CurrentBoostTokenNb;
	/*test satu*/
	if (i>=HMIRmDvAlgo_AutoData.BoostTokenNbMax) i=HMIRmDvAlgo_AutoData.BoostTokenNbMax;
	else if (i<0) i=0;
	/*MAJ DesiredBoostTokenNb*/
	HMIRmDvAlgo_AutoData.DesiredBoostTokenNb=i;
	
	
	/*--------------------------------
	R�partition des Boost sur chaque 
	climatisation
	--------------------------------*/
	/*recopie des temp�ratures en mode programmation
	vers le nouveau set de temp. Situation initiale... */
	for (i=0;i<4;i++)
	{
		HMIRmDvAlgo_AutoData.TempSet[i]= HMIRmDvAlgo_AutoData.ProgTemp[i];
	}
	/* Init de Token au nbre voulu, avant le while*/
	TokenNb=HMIRmDvAlgo_AutoData.DesiredBoostTokenNb;
	ClimOffNb=4-ClimOnNb;
	while(TokenNb>0)
	{
		/*D�terminer le nbre de clim off*/
		if (ClimOffNb>0) /* il existe des clims � l'arr�t*/
		{
			/*Trouver la clim de plus grande prio (valeur min !)*/
			Min=5;
			for (j=0;j<4;j++)
			{
				if (HMIRmDvAlgo_AutoData.TempSet[j]==0)
				{
					if (HMIRmDvAlgo_AutoData.Prio[j]<Min) 
					{
						Min=HMIRmDvAlgo_AutoData.Prio[j];
						IndiceDuMin=j; /*on a trouv� un min provisoire*/
					}
				}
			}
			if (Min!=5) /* une clim � l'arr�t est trouv�e, 
									celle de plus haute prio*/
			{
				HMIRmDvAlgo_AutoData.TempSet[IndiceDuMin]=19; /*la clim passe de 0 � 19�C*/
				TokenNb--;
				ClimOffNb--;
			}
			
		}
		else /* on donne les token aux clims d�j� on*/
		{
			/*trouver la clim de plus haute prio*/
			/*Trouver la clim de plus grande prio (valeur min !)*/
			Min=5;
			for (j=0;j<4;j++)
			{
				if (HMIRmDvAlgo_AutoData.Prio[j]<Min) 
				{
					Min=HMIRmDvAlgo_AutoData.Prio[j];
					HMIRmDvAlgo_AutoData.Prio[j]=10; /* pour sortir cet �l�ment du jeu
																						lors de la prochaine boucle*/
					IndiceDuMin=j; /*on a trouv� un min provisoire*/
				}
			}
			if (Min!=5) /* une clim  est trouv�e, 
									celle de plus haute prio*/
			{
				HMIRmDvAlgo_AutoData.TempSet[IndiceDuMin]+= TemperatureIncBoost;
				TokenNb--;
			}
		}
	}
}



/**
  * @brief  G�n�re la consigne de temp�rature en fonction du mode sp�cifi� dans la CentralData
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
	else if (*AdressCentralData_Mode == HMI_Mode_Auto) Localcmde=HMIRmDvAlgo_AutoMode(ID);
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

/*---------------------------------
 FONCTIONS PRIVEE
----------------------------------*/
/**
  * @brief  Convertit une temp�rature en son code commande IR correspondant
  * @Note		
  * @param  Temp�rature : 18 � 23, 0 si arr�t
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_Temp2CmdeIR(char Temp)
{
	RmDv_TelecoIR_Cmde LocalCmde;
	switch(Temp)
	{
		case 0: LocalCmde=_Stop;break;
		case 17:LocalCmde=_Chaud_18_VanBas_FanAuto;break; /*!! � virer provisoire pour compat HMI*/
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
  * @brief  Convertit un code commande IR en sa temp�rature correspondante
	* @Note  Rev 15/10/24 ajout de _NoCommandToSend = 0xC7,
					on renvoie 100 comme temp�rature, c'est idiot mais �a plante pas.
  * @Note   Si la commande est stop, la temp�rature vaut 0
  * @param  Code commande IR
  * @retval  Temp�rature (0 si stop)
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
	else if (Cmde==_NoCommandToSend ) LocalTemp=100;
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
  * @brief  G�n�re la consigne de temp�rature en mode Programmation
  * @Note   
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_ProgramMode(char ID)
{
	TimeStampTypedef Local;
	HourStampTypedef LocalHour, MinuteOffset;
	short int Hour;
	DFH_ProgramModeDataTypedef* AdressCentralData_Prog;
	RmDv_TelecoIR_Cmde LocalCmde;
	int ID_Idx;
	
	/* construction idx de 0 � 4*/
	ID_Idx=(int)ID-(int)ID_Clim_Salon;
	
	/*r�cup�rer le stamp cible*/
	TimeStamp_GetClock(&Local);

	/*r�cup�ration de l'heure lors du stamp de r�ception RmDv
	en ajoutant 5mn pour �tre certain de tomber dans le bon intervalle
	Il faut �viter un stamp � 5h59mn59sec qui en fait correspond � 6h et plus.
	On ajoute donc 5mn pour �tre tranquille.
	Ce stamp d�termine l'intervalle et donc la temp�rature voulue*/
	LocalHour.Sec =Local.Sec;
	LocalHour.Min =Local.Min;
	LocalHour.Hour =Local.Hour;
	MinuteOffset.Sec=0;
	MinuteOffset.Min =5;
	MinuteOffset.Hour=0;
	HourStampAdd(&LocalHour, &MinuteOffset ); /* Stamp + 5mn*/
	Hour = LocalHour.Hour;										/*heure stamp qui cadre bien dans un intervalle*/
	/* R�cup�ration de l'@ du champ de type DFH_ProgramModeDataTypedef*/
	AdressCentralData_Prog=DFH_GetCentralData_ProgParam();
	
	
	/* Principe :
	la donn�e est constitu�e comme suit :
	typedef struct 
{
	char Temperature_6h[4];					
	char Temperature_8h[4];
	char Temperature_10h[4];	
	char Temperature_15h[4];	
	char Temperature_18h[4];
	char Temperature_23h[4];		
}DFH_ProgramModeDataTypedef;
	
	On va donc chercher l'information temp�rature en fonction de l'heure en commen�ant par 
	un if heure >= 23 h exclusif. Chaque clim est filtr�e par son ID.
	Rem : La fonction HMIRmDvAlgo_Temp2CmdeIR permet d'obtenir le code IR en fonction de la 
	temp�rature en �C (sauf 0 qui veut dire stop)
	
	
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
  * @brief  G�n�re la consigne de temp�rature en mode automatique
  * @Note   
  * @param  ID : l'ID de la clim
  * @retval  consigne clim de type RmDv_TelecoIR_Cmde
  **/
RmDv_TelecoIR_Cmde HMIRmDvAlgo_AutoMode(char ID)
{
	RmDv_TelecoIR_Cmde LocalCmde;
	int temp;
	int ID_Idx;
	
	/* construction idx de 0 � 4*/
	ID_Idx=(int)ID-(int)ID_Clim_Salon;
	temp=HMIRmDvAlgo_AutoData.TempSet[ID_Idx];
	LocalCmde=HMIRmDvAlgo_Temp2CmdeIR(temp);
	return LocalCmde;
}
