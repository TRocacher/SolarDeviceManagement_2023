#include "FSKStack.h"
#include "MyLCD.h"
#include "GLOBAL_SMARTGATEWAY.h"
#include "RmDv_SGw_Protocol.h"
#include "StringFct.h"
#include "TimeManagement.h"
#include "Driver_DS1307.h"


#include "TimeStampManagement.h"
#include "UARTStack.h"
#include "DataFromHMI.h"
#include "DataFromRmDv.h"

/******************************************************************************************************************
	

	MAIN PROGRAM
	
	Les chronomètres utilisés pour les timers :

	
*****************************************************************************************************************/
void Transaction_RmDv(char ID);
void Transaction_HMI(void);

//#define AfficheHeureLCD
#define AfficheRmDvInfo

#ifdef AfficheRmDvInfo
char LCD_RmDv_Str[20];

#endif


#ifdef AfficheHeureLCD
char Phrase[8]={0x40,0x9C, 0xB5,0x07,0xC3,0xF5,0x48,0x40};
char LCD_Sentence[20];
char * PtrChar;
int Secondes;
TimeStampTypedef TimeStampIHM;
TimeStampTypedef * PtrTimeStamp;
#endif


char * PtrOnString;							/* pointeur pour récupérer l'adresse de string depuis UART HMI */
UARTStack_ErrorType MyError;		/* Variable indiquant une erreur de la pile UARTStack */




/* les données clim (RmDDv)*/
int RmDv_ID; 										/* l'@ source du signal FSK en cours de réception, qui est l'ID de la clim */
RmDvDataTypedef* pClimSalon;		/* Pointeur de données du RmDv Salon */
RmDvDataTypedef* pClimSaM;			/* Pointeur de données du RmDv Salle à manger */
RmDvDataTypedef* pClimEntree;		/* Pointeur de données du RmDv Entrée */
RmDvDataTypedef* pClimCouloir;	/* Pointeur de données du RmDv Couloir */
RmDvDataTypedef* pRmDvExt;			/* Pointeur de données du RmDv situé à l'extérieur */

RmDvDataTypedef* Tab_RmDvData[5];	/* tableau de Pointeurs de données des divers RmDv */





int main (void)
{

	TimeManag_TimeOutInit(); 	/* obligatoire pour la gestion des TimeOut à tous les étages...*/
	FSKStack_Init(My_);				/* init de la stack wireless*/
	UARTStack_Init();  				/* init de la stack UART pour HMI*/
	TimerStamp_Start(); 			/* obligatoire pour pouvoir gérer les horodatage*/
	
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
	

#if defined (AfficheHeureLCD) || defined (AfficheRmDvInfo) 	
	MyLCD_Init ();
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print(" LCD Now OK...");
	MyLCD_Set_cursor(0, 1);
#endif	
	
	
while(1)
	{
		if (UARTStack_IsHMIMssg()==1)
		{
			Transaction_HMI();			
		}
		if (FSKStack_IsNewMssg()==1) /* arrivée d'un ordre de l'un des 5 RmDv*/
		{
			RmDv_ID=FSKStack_GetSrcAdress();
			Transaction_RmDv(RmDv_ID);			
		}
		
	}
}


/******************************************************************************************************************
	
	TRANSACTIONS ...
	
*****************************************************************************************************************/
#ifdef AfficheRmDvInfo
/* Message en dur pour test LCD*/
char NoWarn[]="NoWarning";
char Trial_2[]= "Trial 2...";
char Trial_3[]= "Trial 3...";
char Trial_4[]= "Trial 4...";
char Trial_5[]= "Trial 5...";
char Trial_6[]= "Trial 6...";
char Trial_7[]= "Trial 7...";
char Trial_8[]= "Trial 8...";
char Trial_9[]= "Trial 9...";
char Trial_10[]= "Trial 10...";
char Error_TempI2C[]= "Erreur Temp I2C...";
char Error_NewSetNotRec[]= "RmDv : No New order ";
char Error_StatusNotRec[]= "SGw : No status ";
#endif


void Transaction_RmDv(char ID)
{
	char FSKMssgRec[30];				/* String de réception de la pile StackFSK */
	MssgCode Code;							/* Code qui seea extrait du string */
	int L;											/* Longueur du string */
	float TemperatureMesuree;		/* Température mesurée au niveau du RmDv*/
	char lastTempSet;						/* dernière consigne reçu par le RmDv lors de la précédente requête */

	char Success;								/* indicateur de succès de l'échange global */
	RmDv_WarningCode Status;		/* statut final de l'échange */
	
		
#ifdef AfficheRmDvInfo	
	char* TabPtrString[25];		/* Tableau de phrases à afficher */
	/* Prépa pointeur de chaine*/
	TabPtrString[Status_NoWarning]=NoWarn;
	TabPtrString[Status_Trial_2]=Trial_2;
	TabPtrString[Status_Trial_3]=Trial_3;
	TabPtrString[Status_Trial_4]=Trial_4;
	TabPtrString[Status_Trial_5]=Trial_5;
	TabPtrString[Status_Trial_6]=Trial_6;
	TabPtrString[Status_Trial_7]=Trial_7;
	TabPtrString[Status_Trial_8]=Trial_8;
	TabPtrString[Status_Trial_9]=Trial_9;
	TabPtrString[Status_Error_TempI2C]=Error_TempI2C;
	TabPtrString[Status_Error_NewTempSetNotReceived]=Error_NewSetNotRec;
	TabPtrString[Status_NoStatusReceived]=Error_StatusNotRec;
#endif	
	
	/* Recopie locale du message*/
	L=FSKStack_GetLen();
	FSKStack_GetNewMssg(FSKMssgRec,L); 
	
	/* récupération code request*/
	Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
	
	if (Code == MssgReq_SendInfo) /* si le code n'est pas celui de la première requête, alors on
																	ne va pas plus loin */
	{
 		/* Démarrage timeout ...*/
		TimeManag_TimeOutStart(Chrono_WaitTransactionEnd , TimeOutTransaction );
		
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		/* Réponse vers le RmDv */
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,lastTempSet, 65535);
		
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
						RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,lastTempSet, 65535);
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
		
		/* Mise à jour de la vatiable RmDv */

#ifdef AfficheRmDvInfo		
		/* Mise à jour LCD */
		MyLCD_Set_cursor(0, 0);	
		MyLCD_Print("                ");
		MyLCD_Set_cursor(0, 0);
//    StringFct_Float2Str(TemperatureMesuree,LCD_RmDv_Str, 3, 1);
//		MyLCD_Print("Temp = ");
//		MyLCD_Print_n (LCD_RmDv_Str,5);		
		
		MyLCD_Print(TabPtrString[Status]);
		MyLCD_Set_cursor(0, 1);	
		MyLCD_Print("                ");
		MyLCD_Set_cursor(0, 1);
		
		StringFct_Int2Str_99(lastTempSet,LCD_RmDv_Str);
		MyLCD_Print_n (LCD_RmDv_Str,5);
#endif
	}
	
}




void Transaction_HMI(void)
{
	#ifdef	AfficheHeureLCD
	int i;
	#endif
	
	int L;
	/* Mise à jour IHM Central Data */

	MyError=UARTStack_GetErrorStatus();
	if( MyError == _NoError) 
	{
		PtrOnString=UARTStack_GetHMIMssg();
		L=UARTStack_GetLen();
		DFH_Update_All(PtrOnString,L);
			
		#ifdef	AfficheHeureLCD	
			MyError=UARTStack_GetErrorStatus();
		  MyLCD_Set_cursor(0, 1);	
			MyLCD_Print("                ");
		  MyLCD_Set_cursor(0, 1);	
			
			/* Remplissage structure*/
			PtrTimeStamp=&TimeStampIHM;
			PtrChar=(char *)PtrTimeStamp;
			for (i=0;i<12;i++)
			{
				*PtrChar=*PtrOnString;
				PtrOnString++;
				PtrChar++;
			}
			// Conversion exploitable pour LCD
			PtrChar=LCD_Sentence;
			StringFct_Int2Str_99(TimeStampIHM.Hour,PtrChar);
			PtrChar=PtrChar+2;
			*PtrChar=':';
			PtrChar++;
			StringFct_Int2Str_99(TimeStampIHM.Min,PtrChar);
			PtrChar=PtrChar+2;
			*PtrChar=':';			
			PtrChar++;
			StringFct_Int2Str_99(TimeStampIHM.Sec,PtrChar);
			PtrChar=PtrChar+2;
			*PtrChar=':';	
			
			
			MyLCD_Print_n (LCD_Sentence,16); // +1 pour ne pas afficher le nbre de bytes
			
			#endif
		
	}
	
}
