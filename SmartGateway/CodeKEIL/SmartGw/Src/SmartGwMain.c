/* =================================================================================
* ==================    SmartGwMain_xx.c	     =================================
 *
 *   Created on: 04/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *   Revision : voir GLOBAL_SMARTGATEWAY.h
 *  ------------------------------------------------------------------------------
 *  Donne les constantes nécessaires à l'ensemble des échanges (haut niveau)
* =================================================================================*/

#include "FSKStack.h"
#include "GLOBAL_SMARTGATEWAY.h"
//#include "RmDv_SGw_Protocol.h"
#include "TimeManagement.h"

#include "InfoLCD.h"

#include "UARTStack.h"
#include "DataFromHMI.h"
#include "DataFromRmDv.h"

#include "MainFcts.h"
#include "HMI_RmDv_Algo.h"

/* Gestion LCD */
char Using_BP; /* indique que le user utilise les boutons, bloquera le screen system (heure) 
							qui s'active par défaut toutes les secondes */
#define TimeOut_BPUnused (30*1000) /* 30 secondes */
Screen Scr_Idx;	/* Indexe identifiant de manière univoque un affichage LCD*/
int Scr_ID;			/* ID du RmDv dans un subsreenLCD associé au RmDv*/


void Transaction_RmDv(char ID);
void ReceiveReset_RmDv(char ID);
void Transaction_HMI(void);

void Init_RmDvDataPtrTab(void);

TimeStampTypedef TestStamp; /////!!!!!!!!!!


/* les données clim (RmDDv)*/
int RmDv_ID; 										/* l'@ source du signal FSK en cours de réception, qui est l'ID de la clim */
//RmDvDataTypedef* pClimSalon;		/* Pointeur de données du RmDv Salon */
//RmDvDataTypedef* pClimSaM;			/* Pointeur de données du RmDv Salle à manger */
//RmDvDataTypedef* pClimEntree;		/* Pointeur de données du RmDv Entrée */
//RmDvDataTypedef* pClimCouloir;	/* Pointeur de données du RmDv Couloir */
//RmDvDataTypedef* pRmDvExt;			/* Pointeur de données du RmDv situé à l'extérieur */

//RmDvDataTypedef* Tab_RmDvData[5];	/* tableau de Pointeurs de données des divers RmDv */




/******************************************************************************************************************

	MAIN PROGRAM
		
*****************************************************************************************************************/
void UserBP(void);

void UserBP_Dwn(void);
void UserBP_Right(void);

void OneSec_Callback(void);
char OnSecITEnalble;				/* Flag qui autorise l'affichage périodique 1 sec 
														utile en phase de reset RmDv lorsque la SGw indique la version*/

int main (void)
{
	Using_BP=0; /* Le user n'utilise pas les boutons*/
	/* Lancement du système (pile FSK, UART, Timeout) */
	MainFcts_SystemStart();
	/* Mise à l'heure du système (set time et init fuseaux horaire et IdxTps réel */
	MainFcts_SetArbitraryTime();
	/* init DFH_CentralData en attendant Mssg HMI*/
	DFH_Init_CentralData();
	/*Initialisation des pointeurs sur les RmDv et
	  Initialisation de la table de pointeurs Tab_RmDvData*/
	Init_RmDvDataPtrTab();
	/* Initialisation de l'affichage, cad des Idx*/
	Scr_Idx = Screen_Start;
	Scr_ID=ID_Clim_Salon;
	/* Initialisation du module d'affichage LCD*/
	InfoLCD_Init();
	/* Config PC13 Tamp */
	NVIC_Ext_IT (GPIOC, 13, FALLING_EGDE, INPUT_FLOATING, 14, UserBP_Dwn);
	/* Config PA0 WakeUp */
	NVIC_Ext_IT (GPIOA, 0, FALLING_EGDE, INPUT_FLOATING, 14, UserBP_Right);
	HourStamp_1sec_CallbackAssociation(OneSec_Callback);

	OnSecITEnalble=1; /* validation affichage périodique 1 sec*/
  


while(1)
	{
		/* Un message HMI est arrivé ? */
		if (UARTStack_IsHMIMssg()==1)
		{
			Transaction_HMI();	/* Lancement routine associée ...*/		
		}
		/* Un message venant d'un RmDv est arrivé ? */
		if (FSKStack_IsNewMssg()==1) /* arrivée d'un ordre de l'un des 5 RmDv*/
		{
			RmDv_ID=FSKStack_GetSrcAdress();
			if (FSKStack_IsBroadcast()==1)	ReceiveReset_RmDv(RmDv_ID);
			else 	Transaction_RmDv(RmDv_ID);	    /* Lancement routine associée ...*/			
		}
		
	}
}


/* ==============================================
			Call back 1 seconde  
=================================================*/
void OneSec_Callback(void)
{
	if( OnSecITEnalble==1)
	{
		/* Affichage LCD toutes les secondes*/
		if (Scr_Idx==Screen_Start)	
		{
			InfoLCD_ScreenStart();
			TimeManag_Delay_x_ms(2000);	/* attente 2 sec*/
			Scr_Idx=Screen_WaitForHMI;
		}
		else if (Scr_Idx==Screen_WaitForHMI)	
		{
			InfoLCD_Screen_WaitForHMI();
			if (TimeStamp_GetClockUpdated_Flag()==1)	Scr_Idx=Screen_HowToUse;	
		}
		else if (Scr_Idx==Screen_HowToUse)	
		{
			InfoLCD_Screen_HowToUse();
			TimeManag_Delay_x_ms(2000);	/* attente 2 sec*/	
			Scr_Idx=Screen_System;
		}
		else 
		{
			/* Affichage du screen système si BP non utilisé*/
			if ((Using_BP==0)|| (Scr_Idx==Screen_System))
			{
				InfoLCD_ScreenSystem();
			}
		}
		/* Remise à 0 flag Using_BP si timeout avéré*/
		if (TimeManag_GetTimeOutStatus(Chrono_TimeOutMenuLCD)==1)
		{
			Using_BP=0;
		}
	}
}



/******************************************************************************************************************
		TRANSACTIONS RmDv 
*****************************************************************************************************************/

void Transaction_RmDv(char ID)
{
	char FSKMssgRec[30];				/* String de réception de la pile StackFSK */
	MssgCode Code;							/* Code qui seea extrait du string */
	int L;											/* Longueur du string */
	float TemperatureMesuree;		/* Température mesurée au niveau du RmDv*/
	RmDv_TelecoIR_Cmde lastTempSet;						/* dernière consigne reçu par le RmDv lors de la précédente requête */
	RmDv_TelecoIR_Cmde NewTempSet = _Stop;

	char Success;								/* indicateur de succès de l'échange global */
	RmDv_WarningCode Status;		/* statut final de l'échange */
	RmDv_WkUp_CurrentState PrevState; /* Etat lors du précédent échange */
	int CorrInterval_ToSend;		/* l'intervalle à renvoyer au RmDv, corrigé.*/
	
	RmDvDataTypedef* PtrRmDvData;



	/* Recopie locale du message reçu par la pile FSK*/
	L=FSKStack_GetLen();
	FSKStack_GetNewMssg(FSKMssgRec,L); 
	
	/* récupération code request*/
	Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
	
	if (Code == MssgReq_SendInfo) /* si le code n'est pas celui de la première requête, alors on
																	ne va pas plus loin */
	{
 		/* Démarrage timeout ...*/
		TimeManag_TimeOutStart(Chrono_WaitTransactionEnd , TimeOutTransaction );
		/*Accès à la donnée RmDvData*/
		PtrRmDvData=RmDvData_GetObjectAdress(ID);
		
		/* Stamp data dans tableau pour debug*/
		TimeStamp_GetClock(&TestStamp);
		InfoLCD_AddTimeStampToMem(&TestStamp,ID);
		
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = (RmDv_TelecoIR_Cmde)RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		/* Réponse vers le RmDv */
		
		/*-----------------------------------------------------------
		Calcul nouvelle consigne et nouveau délai, corrigé (stamp transaction inclu)
		------------------------------------------------------------*/
		if (TimeStamp_GetClockUpdated_Flag()==1) /* L'horloge a été intialisée par l'HMI*/
		{
			CorrInterval_ToSend=RmDvData_GenerateNextTimeInterval(PtrRmDvData);
			NewTempSet=HMIRmDvAlgo_ComputeTempCmde(ID); /* se fait en fonction du mode*/
		}
		else /* L'horloge n'est pas encore à jour, on demande un arrêt clim 
			et un nouveau réveil dans 5mn */
		{
			CorrInterval_ToSend = 5*60; /* NB dans ce cas, le délai n'est pas corrigé*/ 
			NewTempSet = _Stop;
		}
		
		/* New , Gestion du beep : 
		- si le beep n'est pas demandé :
				- si l'ancienne consigne est égale à la nouvelle 
						la nouvelle consigne devient "_NoCommandToSend" 
						le champ LastTempSetBeforeNoCommand reçoit l'ancienne commande*/
		/* avant tout, on recale LastTempSet à sa valeur d'origine si NoCommand
		ainsi on se retrouve dans une situation "normale"*/
		if (lastTempSet==_NoCommandToSend) lastTempSet=PtrRmDvData->LastTempSetBeforeNoCommand;
			
		if (DFH_GetCentralData_OptPowerData()->ReapeatBeep==0)
		{
			if (NewTempSet==lastTempSet) /*il ne faut pas répéter la commande*/
			{
				NewTempSet=_NoCommandToSend;
				PtrRmDvData->LastTempSetBeforeNoCommand=lastTempSet;				
			}
		}
		
				
		/*-----------------------------------------------------------
		Envoie nouvelle consigne et nouveau délai
		------------------------------------------------------------*/
	
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,NewTempSet, CorrInterval_ToSend);
		
		/* Bloquage dans un timout avec polling info/status
		ici on peut encore recevoir soit une info (redondance), ou un status ce qui est attendu ! */
		
		//TEST
		if (TimeManag_GetTimeOutStatus(Chrono_WaitTransactionEnd)==0)
		{
			Success=1;
		}
		//
		
		PrevState=WarningMssg; /*valeur par défaut en cas d'échec réception
		Rem : la non réception est confirmée par le chps Status qui
		vaut alors Status_NoStatusReceived 
		Cette init de PrevState est faite pour ne pas rester non affectée
		dans l'algo qui suit*/
		Success=0; /*Echec par défaut...*/
		while(TimeManag_GetTimeOutStatus(Chrono_WaitTransactionEnd)==0)
		{
			/* attente non bloquante d'un message RmDV correspondant à ID */
			if (FSKStack_IsNewMssg()==1) /* arrivée d'un ordre de l'un des 5 RmDv*/
			{
				RmDv_ID=FSKStack_GetSrcAdress(); /* Récup ID*/
				if (FSKStack_GetSrcAdress()==ID) /* compare ID*/
				{
					/* Recopie locale du message*/
					L=FSKStack_GetLen();
					FSKStack_GetNewMssg(FSKMssgRec,L);
					/* récupération code */
					Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
					/* si req info, renvoie la valeur */
					if (Code == MssgReq_SendInfo)
					{
						RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,NewTempSet, CorrInterval_ToSend);
					}
					/* si req Status récupération status + renvoie ack*/
					if (Code == MssgReq_SendStatus)
					{
						Status=RmDv_SGw_FSKP_ExtracStatus(FSKMssgRec);
						PrevState=RmDv_SGw_FSKP_ExtractPreviousState(FSKMssgRec);
						RmDv_SGw_FSKP_SendMssgAns_Ack(ID);
						Success=1;
						break; /* on arrête là puisqu'on a bien reçu l'état.*/
					}
				}
			}
	
		}
				
		/*  mettre à jour la variable de donnée RmDv en f*/
		if (Success==0) /* si échec (le RmDv n'a pas pu donner son statut*/
		{
			Status=Status_NoStatusReceived;
		}
		
		/* Mise à jour de la variable RmDv */
		RmDvData_Update(PtrRmDvData, TemperatureMesuree,lastTempSet,NewTempSet,Status,PrevState);
		
	}
	
}


/******************************************************************************************************************
	 RmDv en phase de reset
*****************************************************************************************************************/

void ReceiveReset_RmDv(char ID)
{
	char Revision[10];
	RmDvData_Reset(ID); /* reinit RmDvData associé*/
	OnSecITEnalble=0;	/* blocage affichage courant 1sec*/
	/*récupération du message*/
	FSKStack_GetNewMssg (Revision, 8); 
	/*Affichage info */
	InfoLCD_PrintRevision(Revision,8,ID);
	/* attente 2 sec*/
	TimeManag_Delay_x_ms(2000);
	OnSecITEnalble=1;	/* libération affichage courant 1sec*/
}


/******************************************************************************************************************
		TRANSACTIONS HMI
      - Mise à jour de la variable Central data (DFH_CentralData)
			- Mise à jour du flag de mise à l'heure système
*****************************************************************************************************************/


char * PtrOnString;							/* pointeur pour récupérer l'adresse de string depuis UART HMI */
UARTStack_ErrorType MyError;		/* Variable indiquant une erreur de la pile UARTStack */

void Transaction_HMI(void)
{
	int L, DeltaStamp;
	TimeStampTypedef * PtrTimeStampHMI, *PtrTimeStampLocal;
	
	OnSecITEnalble=0;	/* blocage affichage courant 1sec*/
	MyError=UARTStack_GetErrorStatus();
	if( MyError == _NoError) 
	{
	/*--------------------------------
	Ecriture de HMI_CenralData...
	--------------------------------*/
		PtrOnString=UARTStack_GetHMIMssg();
		L=UARTStack_GetLen();
		/* Mise à jour Central Data*/
		DFH_Update_All(PtrOnString,L);
	/*--------------------------------
	Mise à l'heure si l'écart entre 
	le stamp local et le stamp HMI sont 
	éloignées de plus de 5 secondes
	--------------------------------*/		
		PtrTimeStampHMI=DFH_GetCentralData_Stamp(); 		/* obtention du pointeur Stamp de Central Data */
		PtrTimeStampLocal=TimeStamp_GetClockStampAdr();	/* Obtention du pointeur clock */
		DeltaStamp =TimeStamp_substract(PtrTimeStampLocal,PtrTimeStampHMI ); /* Local - HMI stamps*/
		TimeStamp_SetTimeStampDeltaStamp(DeltaStamp);
		/* mise à jour conditionnel du flag de mise à l'heure système local*/
		if (DeltaStamp>=5 || DeltaStamp<=-5) /* condition de mise à l'heure*/
		{
			TimeStamp_SetClock(PtrTimeStampHMI);
			TimeStamp_SetClockUpdated_Flag();
		}
	/*--------------------------------
	Ecriture de HMIRmDvAlgo_AutoData...
	pour anticiper les demandes de température
	en mode automatique
	--------------------------------*/			
		HMIRmDvAlgo_AutoModeDataUpdateFromHMI();
		
	}
	OnSecITEnalble=1;	/* libération affichage courant 1sec*/
}


/******************************************************************************************************************
		Choix Menu à afficher
*****************************************************************************************************************/
#define DebounceLaps_ms 200
int DebounceFstOccu_BP_Dwn;

void UserBP_Dwn(void)
{
	Clear_Flag_IT_Ext_5_15(13);
	/* Principe de l'antirebond
		Le timeout est indépendant de l'interruption BP.
		Lors de la toute première IT, il y a timeout. Ainsi, la variable
		DebounceFstOccu passe à 1. Mais également, le timeout est relancé.
	  
		Cela permet alors d'enchaîner sur l'algo voulu. 
		A la fin de cet Algo, la variable DebounceFstOccu passe à 0 empêchant
		ainsi une nouvelle exécution de l'algo si une IT se déclenche trop vite (rebond).
	
		Un nouvel algo ne pourra s'exécuter qu'à échéance du timeout. La constante
		DebounceLaps_ms est fixée à 200ms
	*/
	
	/* Blocage menu système par défaut (qui donne l'heure, pendant TimeOut_BPUnused */
	TimeManag_TimeOutStart(Chrono_TimeOutMenuLCD , TimeOut_BPUnused);
	Using_BP=1;
	
	/*démarrage tempo pour antirebond si pas déjà lancé*/
	if (TimeManag_GetTimeOutStatus(Chrono_Debounce_BpDwn)==1)
	{
		TimeManag_TimeOutStart(Chrono_Debounce_BpDwn ,DebounceLaps_ms); // lancement TimeOut
		DebounceFstOccu_BP_Dwn=1;
	}
	
		
	if (DebounceFstOccu_BP_Dwn==1)  /* uniquement lorsque le timout a été lancé ...*/
	{
		if (Scr_Idx == Screen_System) Scr_Idx=Screen_AllTemperatures;
		else if (Scr_Idx == Screen_AllTemperatures)  {Scr_Idx=Screen_RmDvSalon;Scr_ID=ID_Clim_Salon;}
		else if (Scr_Idx == Screen_RmDvSalon)  {Scr_Idx=Screen_RmDvSaM;Scr_ID=ID_Clim_SaManger;}
		else if (Scr_Idx == Screen_RmDvSaM)  {Scr_Idx=Screen_RmDvEntree;Scr_ID=ID_Clim_Entree;}
		else if (Scr_Idx == Screen_RmDvEntree)  {Scr_Idx=Screen_RmDvCouloir;Scr_ID=ID_Clim_Couloir;}
		else if (Scr_Idx == Screen_RmDvCouloir)  {Scr_Idx=Screen_RmDvExt;Scr_ID=ID_Ext;}
		else if (Scr_Idx == Screen_RmDvExt)  Scr_Idx=Screen_System;
		/* Affichage par anticipation sur le callback 1 sec pour plus de réactivité*/
		/* Affichage LCD toutes les secondes*/
		
		
		switch(Scr_Idx) 
		{
			case Screen_System:InfoLCD_ScreenSystem();break;
			case Screen_AllTemperatures:InfoLCD_ScreenAllTemp();break;
			case Screen_RmDvSalon:InfoLCD_ScreenRmDvID(Scr_ID);break;
			case Screen_RmDvSaM:InfoLCD_ScreenRmDvID(Scr_ID);break;
			case Screen_RmDvEntree:InfoLCD_ScreenRmDvID(Scr_ID);break;
			case Screen_RmDvCouloir:InfoLCD_ScreenRmDvID(Scr_ID);break;
			case Screen_RmDvExt:InfoLCD_ScreenRmDvID(Scr_ID);break;
			
			default: Scr_Idx=Screen_System;break;
//		case HeureCourante:InfoLCD_PrintHour("Heure systeme:",TimeStamp_GetClockStampAdr());break;
//		case Salon_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Salon);break;
//		case Salon_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Salon);break;
//		case SaM_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_SaManger);break;
//		case SaM_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_SaManger);break;
//		case Entree_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Entree);break;
//		case Entree_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Entree);break;
//		case Couloir_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Couloir);break;
//		case Couloir_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Couloir);break;
//		case Ext_1:InfoLCD_PrintRmDv_Stamp(ID_Ext);break;
//		case Ext_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Ext);break;	
  
		}
		DebounceFstOccu_BP_Dwn=0; /* ce n'est plus la première fois, le bloc ne se relance pas
		sauf si un timeout survient ...*/
	}

}



int DebounceFstOccu_BP_Right;

void UserBP_Right(void)
{
	/* flag IT automatiquement mise à 0 pas besoin de le faire*/
	/*démarrage tempo pour antirebond si pas déjà lancé*/
	
	/* Blocage menu système par défaut (qui donne l'heure, pendant TimeOut_BPUnused */
	TimeManag_TimeOutStart(Chrono_TimeOutMenuLCD , TimeOut_BPUnused);
	Using_BP=1;
	
	if (TimeManag_GetTimeOutStatus(Chrono_Debounce_BpRight)==1)
	{
		TimeManag_TimeOutStart(Chrono_Debounce_BpRight ,DebounceLaps_ms); // lancement TimeOut
		DebounceFstOccu_BP_Right=1;
	}
	
		
	if (DebounceFstOccu_BP_Right==1)  /* uniquement lorsque le timout a été lancé ...*/
	{
		/* sous menu système ...
			SubScr_SysMode,	SubScr_SysHMIStamp,	SubScr_SysPowInverter,	SubScr_SysTotalPower,	SubScr_SysDeltaStamp,
		*/
		if (Scr_Idx == Screen_System) Scr_Idx=SubScr_SysMode;
		else if (Scr_Idx == SubScr_SysMode)  Scr_Idx=SubScr_SysHMIStamp;
		else if (Scr_Idx == SubScr_SysHMIStamp)  Scr_Idx=SubScr_SysPowInverter;
		else if (Scr_Idx == SubScr_SysPowInverter)  Scr_Idx=SubScr_SysPowerExcess;
		else if (Scr_Idx == SubScr_SysPowerExcess)  Scr_Idx=SubScr_SysDeltaStamp;
		else if (Scr_Idx == SubScr_SysDeltaStamp)  Scr_Idx=Screen_System;
		
		/* sous menu RmDv...
				SubScr_RmDv_LastStamp,	SubScr_RmDv_Temperature,SubScr_RmDv_Status,
				SubScr_RmDv_NextInterval,	SubScr_RmDv_DelayFactor,		*/	
		if ((Scr_Idx == Screen_RmDvSalon) ||(Scr_Idx == Screen_RmDvSaM) || \
			(Scr_Idx == Screen_RmDvEntree)||(Scr_Idx == Screen_RmDvCouloir) || \
			(Scr_Idx ==Screen_RmDvExt ))
		{
			Scr_Idx=SubScr_RmDv_LastStamp;
		}
		else if (Scr_Idx == SubScr_RmDv_LastStamp)  Scr_Idx=SubScr_RmDv_Temperature;
		else if (Scr_Idx == SubScr_RmDv_Temperature)  Scr_Idx=SubScr_RmDv_Status;
		else if (Scr_Idx == SubScr_RmDv_Status)  Scr_Idx=SubScr_RmDv_NextInterval;
		else if (Scr_Idx == SubScr_RmDv_NextInterval)  Scr_Idx=SubScr_RmDv_DelayFactor;
		else if (Scr_Idx == SubScr_RmDv_DelayFactor)
		{
			if (Scr_ID==ID_Clim_Salon) Scr_Idx=Screen_RmDvSalon;
			else if (Scr_ID==ID_Clim_SaManger) Scr_Idx=Screen_RmDvSaM;
			else if (Scr_ID==ID_Clim_Entree) Scr_Idx=Screen_RmDvEntree;
			else if (Scr_ID==ID_Clim_Couloir) Scr_Idx=Screen_RmDvCouloir;
			else if (Scr_ID==ID_Ext) Scr_Idx=Screen_RmDvExt;
		}
		
		/* Affichage en fonction de l'index d'écran défini précédemment*/
		switch(Scr_Idx) 
		{
			/* Sous menu system*/
			case Screen_System:InfoLCD_ScreenSystem();break;
			case SubScr_SysMode:InfoLCD_SubScreenSysMode();break;
			case SubScr_SysHMIStamp:InfoLCD_SubScreenSysHMIStamp();break;
			case SubScr_SysPowInverter:InfoLCD_SubScreenSysPowerInverter();break;
			case SubScr_SysPowerExcess:InfoLCD_SubScreenSysPowerExcess();break;
			case SubScr_SysDeltaStamp:InfoLCD_SubScreenSysDeltaStamp();break;
			/* RmDv*/
			case SubScr_RmDv_LastStamp:InfoLCD_SubScreenRmDvLastStamp(Scr_ID);break;
			case SubScr_RmDv_Temperature:InfoLCD_SubScreenRmDvTemp(Scr_ID);break;
			case SubScr_RmDv_Status:InfoLCD_SubScreenRmDvStatus(Scr_ID);break;
			case SubScr_RmDv_NextInterval:InfoLCD_SubScreenRmDvNextInterval(Scr_ID);break;
			case SubScr_RmDv_DelayFactor:InfoLCD_SubScreenRmDvDelayFactor(Scr_ID);break;
			default: InfoLCD_ScreenRmDvID(Scr_ID);break;
			
		}			
		
			
		DebounceFstOccu_BP_Right=0; /* ce n'est plus la première fois, le bloc ne se relance pas
		sauf si un timeout survient ...*/
	}
}



/******************************************************************************************************************
	
	Fonctions privées utiles ...
	
*****************************************************************************************************************/


void Init_RmDvDataPtrTab(void)
{
	//RmDvDataTypedef* pClim;
	/* initisation des "objets" data clim */
	RmDvData_Reset(ID_Clim_Salon);	
	RmDvData_Reset(ID_Clim_SaManger);
	RmDvData_Reset(ID_Clim_Entree);
	RmDvData_Reset(ID_Clim_Couloir);
	RmDvData_Reset(ID_Ext);
	
	/* Bricolage pour raison de test !! à enlever*/
//	pClim=RmDvData_GetObjectAdress(ID_Clim_Salon);
//	pClim->LastTempSet=_Chaud_21_VanBas_FanAuto;
//	pClim=RmDvData_GetObjectAdress(ID_Clim_SaManger);
//	pClim->LastTempSet=_Chaud_19_VanBas_FanAuto;
//	pClim=RmDvData_GetObjectAdress(ID_Clim_Entree);
//	pClim->LastTempSet=_Chaud_19_VanBas_FanAuto;
//	pClim=RmDvData_GetObjectAdress(ID_Clim_Couloir);
//	pClim->LastTempSet=_Chaud_19_VanBas_FanAuto;
	/* Fin Bricolage pour raison de test !! à enlever*/

	
	/* rangement des pointeur ds un tableau pour appel via ID...*/
//	Tab_RmDvData[0]= pClimSalon;
//	Tab_RmDvData[1]= pClimSaM;
//	Tab_RmDvData[2]= pClimEntree;
//	Tab_RmDvData[3]= pClimCouloir;
//	Tab_RmDvData[4]= pRmDvExt;

}

