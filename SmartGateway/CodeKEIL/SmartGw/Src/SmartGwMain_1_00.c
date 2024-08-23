/* =================================================================================
* ==================    SmartGwMain_xx.c	     =================================
 *
 *   Created on: 04/08/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : _
 *   Revision : 1.00
 *  ------------------------------------------------------------------------------
 *  Donne les constantes n�cessaires � l'ensemble des �changes (haut niveau)
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

int Mode;

void Transaction_RmDv(char ID);
void ReceiveReset_RmDv(char ID);
void Transaction_HMI(void);

void Init_RmDvDataPtrTab(void);

TimeStampTypedef TestStamp; /////!!!!!!!!!!


/* les donn�es clim (RmDDv)*/
int RmDv_ID; 										/* l'@ source du signal FSK en cours de r�ception, qui est l'ID de la clim */
RmDvDataTypedef* pClimSalon;		/* Pointeur de donn�es du RmDv Salon */
RmDvDataTypedef* pClimSaM;			/* Pointeur de donn�es du RmDv Salle � manger */
RmDvDataTypedef* pClimEntree;		/* Pointeur de donn�es du RmDv Entr�e */
RmDvDataTypedef* pClimCouloir;	/* Pointeur de donn�es du RmDv Couloir */
RmDvDataTypedef* pRmDvExt;			/* Pointeur de donn�es du RmDv situ� � l'ext�rieur */

RmDvDataTypedef* Tab_RmDvData[5];	/* tableau de Pointeurs de donn�es des divers RmDv */




/******************************************************************************************************************

	MAIN PROGRAM
		
*****************************************************************************************************************/
void UserBP(void);
void OneSec_Callback(void);
char OnSecITEnalble;				/* Flag qui autorise l'affichage p�riodique 1 sec 
														utile en phase de reset RmDv lorsque la SGw indique la version*/

int main (void)
{

	/* Lancement du syst�me (pile FSK, UART, Timeout) */
	MainFcts_SystemStart();
	/* Mise � l'heure du syst�me (set time et init fuseaux horaire et IdxTps r�el */
	MainFcts_SetArbitraryTime();
	/* init DFH_CentralData en attendant Mssg HMI*/
	DFH_Init_CentralData();
	/*Initialisation des pointeurs sur les RmDv et
	  Initialisation de la table de pointeurs Tab_RmDvData*/
	Init_RmDvDataPtrTab();
	/* Initialisation du module d'affichage LCD*/
	InfoLCD_Init();
	/* Config PC13 Tamp */
	NVIC_Ext_IT (GPIOC, 13, FALLING_EGDE, INPUT_FLOATING, 14, UserBP);
	/* Config PA0 WakeUp */
	NVIC_Ext_IT (GPIOA, 0, FALLING_EGDE, INPUT_FLOATING, 14, UserBP);
	HourStamp_1sec_CallbackAssociation(OneSec_Callback);
	OnSecITEnalble=1; /* validation affichage p�riodique 1 sec*/
while(1)
	{
		/* Un message HMI est arriv� ? */
		if (UARTStack_IsHMIMssg()==1)
		{
			Transaction_HMI();	/* Lancement routine associ�e ...*/		
		}
		/* Un message venant d'un RmDv est arriv� ? */
		if (FSKStack_IsNewMssg()==1) /* arriv�e d'un ordre de l'un des 5 RmDv*/
		{
			RmDv_ID=FSKStack_GetSrcAdress();
			if (FSKStack_IsBroadcast()==1)	ReceiveReset_RmDv(RmDv_ID);
			else 	Transaction_RmDv(RmDv_ID);	    /* Lancement routine associ�e ...*/			
		}
		
	}
}


void OneSec_Callback(void)
{
	if( OnSecITEnalble==1)
	{
	
	/* Affichage LCD toutes les secondes*/
	switch(Mode)
	{
		case Temperature:InfoLCD_Print5Temp();break;
		case HeureCourante:InfoLCD_PrintHour("Heure systeme:",TimeStamp_GetClockStampAdr());break;
		case Salon_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Salon);break;
		case Salon_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Salon);break;
		case SaM_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_SaManger);break;
		case SaM_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_SaManger);break;
		case Entree_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Entree);break;
		case Entree_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Entree);break;
		case Couloir_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Couloir);break;
		case Couloir_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Couloir);break;
		case Ext_1:InfoLCD_PrintRmDv_Stamp(ID_Ext);break;
		default:InfoLCD_PrintRmDv_StatFactNewSet(ID_Ext);break;	
		//default : NewTempSet = InfoLCD_PrintNewSet((TerminalMode)Mode);break;  /*les autres cas sont des temp set*/
		//default:break;
	}
	}
}



/******************************************************************************************************************
		TRANSACTIONS RmDv 
*****************************************************************************************************************/

void Transaction_RmDv(char ID)
{
	char FSKMssgRec[30];				/* String de r�ception de la pile StackFSK */
	MssgCode Code;							/* Code qui seea extrait du string */
	int L;											/* Longueur du string */
	float TemperatureMesuree;		/* Temp�rature mesur�e au niveau du RmDv*/
	RmDv_TelecoIR_Cmde lastTempSet;						/* derni�re consigne re�u par le RmDv lors de la pr�c�dente requ�te */
	RmDv_TelecoIR_Cmde NewTempSet = _Stop;

	char Success;								/* indicateur de succ�s de l'�change global */
	RmDv_WarningCode Status;		/* statut final de l'�change */
	RmDv_WkUp_CurrentState PrevState; /* Etat lors du pr�c�dent �change */
	int CorrInterval_ToSend;		/* l'intervalle � renvoyer au RmDv, corrig�.*/
	
	RmDvDataTypedef* PtrRmDvData;



	/* Recopie locale du message re�u par la pile FSK*/
	L=FSKStack_GetLen();
	FSKStack_GetNewMssg(FSKMssgRec,L); 
	
	/* r�cup�ration code request*/
	Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
	
	if (Code == MssgReq_SendInfo) /* si le code n'est pas celui de la premi�re requ�te, alors on
																	ne va pas plus loin */
	{
 		/* D�marrage timeout ...*/
		TimeManag_TimeOutStart(Chrono_WaitTransactionEnd , TimeOutTransaction );
		/*Acc�s � la donn�e RmDvData*/
		PtrRmDvData=RmDvData_GetObjectAdress(ID);
		
		/* Stamp data dans tableau pour debug*/
		TimeStamp_GetClock(&TestStamp);
		InfoLCD_AddTimeStampToMem(&TestStamp,ID);
		
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = (RmDv_TelecoIR_Cmde)RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		/* R�ponse vers le RmDv */
		
		/*-----------------------------------------------------------
		Calcul nouvelle consigle et nouveau d�lai, corrig� (stamp transaction inclu)
		------------------------------------------------------------*/
		if (TimeStamp_GetClockUpdated_Flag()==1) /* L'horloge a �t� intialis�e par l'HMI*/
		{
			CorrInterval_ToSend=RmDvData_GenerateNextTimeInterval(PtrRmDvData);
			NewTempSet=HMIRmDvAlgo_ComputeTempCmde(ID); /* se fait en fonction du mode*/
		}
		else /* L'horloge n'est pas encore � jour, on demande un arr�t clim 
			et un nouveau r�veil dans 5mn */
		{
			CorrInterval_ToSend = 5*60; /* NB dans ce cas, le d�lai n'est pas corrig�*/ 
			NewTempSet = _Stop;
		}
		


				
		/*-----------------------------------------------------------
		Envoie nouvelle consigne et nouveau d�lai
		------------------------------------------------------------*/
	
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,NewTempSet, CorrInterval_ToSend);
		
		/* Bloquage dans un timout avec polling info/status
		ici on peut encore recevoir soit une info (redondance), ou un status ce qui est attendu ! */
		
		
		Success=0; /*Echec par d�faut...*/
		while(TimeManag_GetTimeOutStatus(Chrono_WaitTransactionEnd)==0)
		{
			/* attente non bloquante d'un message RmDV correspondant � ID */
			if (FSKStack_IsNewMssg()==1) /* arriv�e d'un ordre de l'un des 5 RmDv*/
			{
				RmDv_ID=FSKStack_GetSrcAdress(); /* R�cup ID*/
				if (FSKStack_GetSrcAdress()==ID) /* compare ID*/
				{
					/* Recopie locale du message*/
					L=FSKStack_GetLen();
					FSKStack_GetNewMssg(FSKMssgRec,L);
					/* r�cup�ration code */
					Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
					/* si req info, renvoie la valeur */
					if (Code == MssgReq_SendInfo)
					{
						RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,lastTempSet, CorrInterval_ToSend);
					}
					/* si req Status r�cup�ration status + renvoie ack*/
					if (Code == MssgReq_SendStatus)
					{
						Status=RmDv_SGw_FSKP_ExtracStatus(FSKMssgRec);
						PrevState=RmDv_SGw_FSKP_ExtractPreviousState(FSKMssgRec);
						RmDv_SGw_FSKP_SendMssgAns_Ack(ID);
						Success=1;
						break; /* on arr�te l� puisqu'on a bien re�u l'�tat.*/
					}
				}
			}
	
		}
		

		
		/*  mettre � jour la variable de donn�e RmDv en f*/
		if (Success==0) /* si �chec (le RmDv n'a pas pu donner son statut*/
		{
			Status=Status_NoStatusReceived;
		}
		
		/* Mise � jour de la variable RmDv */
		RmDvData_Update(PtrRmDvData, TemperatureMesuree,lastTempSet,NewTempSet,Status,PrevState);
		
	
		/* Affichage LCD*/
		//InfoLCD_Status_LastTempSet(Status,lastTempSet);

		
	}
	
}


/******************************************************************************************************************
	 RmDv en phase de reset
*****************************************************************************************************************/

void ReceiveReset_RmDv(char ID)
{
	char Revision[10];
	OnSecITEnalble=0;	/* blocage affichage courant 1sec*/
	/*r�cup�ration du message*/
	FSKStack_GetNewMssg (Revision, 8); 
	/*Affichage info */
	InfoLCD_PrintRevision(Revision,8,ID);
	/* attente 5 sec*/
	Delay_x_ms(2000);
	OnSecITEnalble=1;	/* lib�ration affichage courant 1sec*/
}


/******************************************************************************************************************
		TRANSACTIONS HMI
*****************************************************************************************************************/


char * PtrOnString;							/* pointeur pour r�cup�rer l'adresse de string depuis UART HMI */
UARTStack_ErrorType MyError;		/* Variable indiquant une erreur de la pile UARTStack */

void Transaction_HMI(void)
{
	int L, DeltaStamp;
	TimeStampTypedef * PtrTimeStampHMI, *PtrTimeStampLocal;
	
	MyError=UARTStack_GetErrorStatus();
	if( MyError == _NoError) 
	{
		PtrOnString=UARTStack_GetHMIMssg();
		L=UARTStack_GetLen();
		/* Mise � jour Central Data*/
		DFH_Update_All(PtrOnString,L);
		/* Mise � l'heure si l'�cart entre le stamp local et le stamp HMI sont �loign�es de plus de 5 secondes*/
		PtrTimeStampHMI=DFH_GetCentralData_Stamp(); 		/* obtention du pointeur Stamp de Central Data */
		PtrTimeStampLocal=TimeStamp_GetClockStampAdr();	/* Obtention du pointeur clock */
		DeltaStamp =TimeStamp_substract(PtrTimeStampLocal,PtrTimeStampHMI ); /* Local - HMI stamps*/
		TimeStamp_SetTimeStampDeltaStamp(DeltaStamp);
		/* mise � jour conditionnel du flag de mise � l'heure syst�me local*/
		if (DeltaStamp>=5 || DeltaStamp<=-5) /* condition de mise � l'heure*/
		{
			TimeStamp_SetClock(PtrTimeStampHMI);
			TimeStamp_SetClockUpdated_Flag();
		}
			
		/* Affichage LCD de l'heure*/
		InfoLCD_PrintHour("Heure HMI",PtrTimeStampHMI);	
	}
	
}


/******************************************************************************************************************
		Choix Menu � afficher
*****************************************************************************************************************/
#define DebounceLaps_ms 200
int BebounceFstOccu;
void UserBP(void)
{
	Clear_Flag_IT_Ext_5_15(13);
	/*d�marrage tempo pour antirebond si pas d�j� lanc�*/
	if (TimeManag_GetTimeOutStatus(Chrono_Debounce)==1)
	{
		TimeManag_TimeOutStart(Chrono_Debounce ,DebounceLaps_ms); // lancement TimeOut
		BebounceFstOccu=1;
	}
	
		
	if (BebounceFstOccu==1)  /* uniquement lorsque le timout a �t� lanc� ...*/
	{
		Mode=(Mode+1)%ModeNb;	
		
		/* Affichage par anticipation sur le callback 1 sec pour plus de r�activit�*/
		/* Affichage LCD toutes les secondes*/
		switch(Mode)
		{
		case Temperature:InfoLCD_Print5Temp();break;
		case HeureCourante:InfoLCD_PrintHour("Heure systeme:",TimeStamp_GetClockStampAdr());break;
		case Salon_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Salon);break;
		case Salon_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Salon);break;
		case SaM_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_SaManger);break;
		case SaM_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_SaManger);break;
		case Entree_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Entree);break;
		case Entree_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Entree);break;
		case Couloir_1:InfoLCD_PrintRmDv_Stamp(ID_Clim_Couloir);break;
		case Couloir_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Clim_Couloir);break;
		case Ext_1:InfoLCD_PrintRmDv_Stamp(ID_Ext);break;
		case Ext_2:InfoLCD_PrintRmDv_StatFactNewSet(ID_Ext);break;	
		default:break;
		}
		BebounceFstOccu=0; /* ce n'est plus la premi�re fois, le bloc ne se relance pas
		sauf si un timeout survient ...*/
	}

}


/******************************************************************************************************************
	
	Fonctions priv�es utiles ...
	
*****************************************************************************************************************/


void Init_RmDvDataPtrTab(void)
{
	
	/* initisation des "objets" data clim */
	pClimSalon = RmDvData_GetObjectAdress(ID_Clim_Salon);
	RmDvData_Reset(pClimSalon, ID_Clim_Salon);
	pClimSaM = RmDvData_GetObjectAdress(ID_Clim_SaManger);
	RmDvData_Reset(pClimSaM, ID_Clim_SaManger);
	pClimEntree = RmDvData_GetObjectAdress(ID_Clim_Entree);
	RmDvData_Reset(pClimEntree, ID_Clim_Entree);
	pClimCouloir = RmDvData_GetObjectAdress(ID_Clim_Couloir);
	RmDvData_Reset(pClimCouloir, ID_Clim_Couloir);
	pRmDvExt = RmDvData_GetObjectAdress(ID_Ext);
	RmDvData_Reset(pRmDvExt, ID_Ext);
	
	/* rangement des pointeur ds un tableau pour appel via ID...*/
	Tab_RmDvData[0]= pClimSalon;
	Tab_RmDvData[1]= pClimSaM;
	Tab_RmDvData[2]= pClimEntree;
	Tab_RmDvData[3]= pClimCouloir;
	Tab_RmDvData[4]= pRmDvExt;
}

