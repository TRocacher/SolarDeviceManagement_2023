
#include "FSKStack.h"
#include "GLOBAL_SMARTGATEWAY.h"
#include "RmDv_SGw_Protocol.h"


#include "InfoLCD.h"

#include "UARTStack.h"
#include "DataFromHMI.h"
#include "DataFromRmDv.h"

#include "MainFcts.h"

void Transaction_RmDv(char ID);
void Transaction_HMI(void);

void Init_RmDvDataPtrTab(void);



/* les données clim (RmDDv)*/
int RmDv_ID; 										/* l'@ source du signal FSK en cours de réception, qui est l'ID de la clim */
RmDvDataTypedef* pClimSalon;		/* Pointeur de données du RmDv Salon */
RmDvDataTypedef* pClimSaM;			/* Pointeur de données du RmDv Salle à manger */
RmDvDataTypedef* pClimEntree;		/* Pointeur de données du RmDv Entrée */
RmDvDataTypedef* pClimCouloir;	/* Pointeur de données du RmDv Couloir */
RmDvDataTypedef* pRmDvExt;			/* Pointeur de données du RmDv situé à l'extérieur */

RmDvDataTypedef* Tab_RmDvData[5];	/* tableau de Pointeurs de données des divers RmDv */



/******************************************************************************************************************

	MAIN PROGRAM
		
*****************************************************************************************************************/

int main (void)
{
	int CorrInterval_ToSend;
	RmDvDataTypedef* PtrRmDvData;
	PtrRmDvData=RmDvData_GetObjectAdress(0xD1);
	CorrInterval_ToSend=RmDvData_GenerateNextTimeInterval(PtrRmDvData);
	
	
	/* Lancement du système (pile FSK, UART, Timeout) */
	MainFcts_SystemStart();
	/* Mise à l'heure du système (set time et init fuseaux horaire et IdxTps réel */
	MainFcts_SetTime();
	/*Initialisation des pointeurs sur les RmDv et
	  Initialisation de la table de pointeurs Tab_RmDvData*/
	Init_RmDvDataPtrTab();
	/* Initialisation du module d'affichage LCD*/
	InfoLCD_Init();
	
	
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
			Transaction_RmDv(RmDv_ID);	    /* Lancement routine associée ...*/			
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
	char lastTempSet;						/* dernière consigne reçu par le RmDv lors de la précédente requête */

	char Success;								/* indicateur de succès de l'échange global */
	RmDv_WarningCode Status;		/* statut final de l'échange */
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
		
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		/* Réponse vers le RmDv */
		
		/*-----------------------------------------------------------
		Calcul nouvelle consigne nouveau délai, corrigé (stamp transaction inclu)
		------------------------------------------------------------*/
		CorrInterval_ToSend=RmDvData_GenerateNextTimeInterval(PtrRmDvData);
				
		/*-----------------------------------------------------------
		Envoie nouvelle consigne et nouveau délai
		------------------------------------------------------------*/
	
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,lastTempSet, CorrInterval_ToSend);
		
		/* Bloquage dans un timout avec polling info/status
		ici on peut encore recevoir soit une info (redondance), ou un status ce qui est attendu ! */
		
		
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
						RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,lastTempSet, CorrInterval_ToSend);
					}
					/* si req Status récupération status + renvoie ack*/
					if (Code == MssgReq_SendStatus)
					{
						Status=RmDv_SGw_FSKP_ExtracStatus(FSKMssgRec);
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
		RmDvData_Update(PtrRmDvData, TemperatureMesuree,lastTempSet,lastTempSet,Status);
		
	
		/* Affichage LCD*/
		InfoLCD_Status_LastTempSet(Status,lastTempSet);
	}
	
}

/******************************************************************************************************************
		TRANSACTIONS HMI
*****************************************************************************************************************/


char * PtrOnString;							/* pointeur pour récupérer l'adresse de string depuis UART HMI */
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
		/* Mise à jour Central Data*/
		DFH_Update_All(PtrOnString,L);
		/* lecture du pointeur Stamp de Central Data */
		PtrTimeStamp=DFH_ReadStampFromCentralData();
		/* Affichage LCD de l'heure*/
		InfoLCD_PrintHMIHour(PtrTimeStamp);	
	}
	
}




/******************************************************************************************************************
	
	Fonctions privées utiles ...
	
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

