
#include "FSKStack.h"
#include "GLOBAL_SMARTGATEWAY.h"
#include "RmDv_SGw_Protocol.h"
#include "DelayMngt.h"

#include "InfoLCD.h"

#include "UARTStack.h"
#include "DataFromHMI.h"
#include "DataFromRmDv.h"

#include "MainFcts.h"

void Transaction_RmDv(char ID);
void Transaction_HMI(void);

void Init_RmDvDataPtrTab(void);



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

int main (void)
{
	/* Lancement du syst�me (pile FSK, UART, Timeout) */
	MainFcts_SystemStart();
	/* Mise � l'heure du syst�me (set time et init fuseaux horaire et IdxTps r�el */
	MainFcts_SetTime();
	/*Initialisation des pointeurs sur les RmDv et
	  Initialisation de la table de pointeurs Tab_RmDvData*/
	Init_RmDvDataPtrTab();
	/* Initialisation du module d'affichage LCD*/
	InfoLCD_Init();
	
	
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
			Transaction_RmDv(RmDv_ID);	    /* Lancement routine associ�e ...*/			
		}
		
	}
}










/******************************************************************************************************************
		TRANSACTIONS RmDv 
*****************************************************************************************************************/

void Transaction_RmDv(char ID)
{
	char FSKMssgRec[30];				/* String de r�ception de la pile StackFSK */
	MssgCode Code;							/* Code qui sera extrait du string */
	int L;											/* Longueur du string */
	float TemperatureMesuree;		/* Temp�rature mesur�e au niveau du RmDv*/
	char lastTempSet;						/* derni�re consigne re�u par le RmDv lors de la pr�c�dente requ�te */
	
	char RealTimeIndex;					/* Index temps r�el que l'on fige durant toute une transaction*/
	char TransIdx;							/* index de transaction re�u*/
	char NextTransIdx;					/* Index de transition suivant � calculer */
	
	char Success;								/* indicateur de succ�s de l'�change global */
	RmDv_WarningCode Status;		/* statut final de l'�change */
  RmDvDataTypedef* PtrRmDvData;	/* Pointeur sur RmDvData pour extraire l'ID */
	int NextDelay_Sec;					/* Calcul du d�lai pour la transaction future*/
	int RealValueLastDelay_Sec;	/* d�lai entre deux stamp successif, le pr�sent - le pr�c�dent */


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
		/* M�morise RealTimeIndex pour le figer, durant l'op�ration*/
		RealTimeIndex=DelayMngt_GetRealTimeIndex();
		/*		Stamp Data dans RmDvData  */
		RmDvData_StampReceivedData(PtrRmDvData);
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		TransIdx=RmDv_SGw_FSKP_ExtracTransIdx(FSKMssgRec);
		
		/* R�ponse vers le RmDv */
		
		/*-----------------------------------------------------------
		Calcul nouvelle consigne nouveau d�lai
		------------------------------------------------------------*/
//!!		PtrRmDvData->Delay.NextDesiredWkupDelay_sec=30;  /*  TEST 30 sec*/
		NextDelay_Sec=DelayMngt_CalculateNextDelay(RealTimeIndex,TransIdx, PtrRmDvData);
		NextTransIdx = DelayMngt_CalculateNextTransactionIdx(RealTimeIndex);
		RealValueLastDelay_Sec = DelayMngt_Calculate_RealPreviousInterval(RealTimeIndex,PtrRmDvData);
		
		/*-----------------------------------------------------------
		Envoie nouvelle consigne et nouveau d�lai
		------------------------------------------------------------*/
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID, NextTransIdx, lastTempSet, \
																			NextDelay_Sec, RealValueLastDelay_Sec);
		
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
							RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID, NextTransIdx, lastTempSet, \
																			NextDelay_Sec, RealValueLastDelay_Sec);
					}
					/* si req Status r�cup�ration status + renvoie ack*/
					if (Code == MssgReq_SendStatus)
					{
						Status=RmDv_SGw_FSKP_ExtracStatus(FSKMssgRec);
						RmDv_SGw_FSKP_SendMssgAns_Ack(ID,00);
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
		RmDvData_Update(PtrRmDvData, TemperatureMesuree,lastTempSet,Status,TransIdx);
		/* mise � jour statut dans la table ScheduleTab*/
		DelayMngt_UpdateStatus(RealTimeIndex,PtrRmDvData);
		
		/* Affichage LCD*/
		InfoLCD_Status_LastTempSet(Status,lastTempSet);
	}
	
}

/******************************************************************************************************************
		TRANSACTIONS HMI
*****************************************************************************************************************/


char * PtrOnString;							/* pointeur pour r�cup�rer l'adresse de string depuis UART HMI */
UARTStack_ErrorType MyError;		/* Variable indiquant une erreur de la pile UARTStack */

void Transaction_HMI(void)
{
	int L;
	TimeStampTypedef * PtrTimeStamp;
	
	MyError=UARTStack_GetErrorStatus();
	if( MyError == _NoError) 
	{
		PtrOnString=UARTStack_GetHMIMssg();
		L=UARTStack_GetLen();
		/* Mise � jour Central Data*/
		DFH_Update_All(PtrOnString,L);
		/* lecture du pointeur Stamp de Central Data */
		PtrTimeStamp=DFH_ReadStampFromCentralData();
		/* Affichage LCD de l'heure*/
		InfoLCD_PrintHMIHour(PtrTimeStamp);	
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

