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
	Chrono_1 : file FSKStack.c (timeout Wait for header, FSM)
	Chrono_2 : utilisé pour le timeout lors de l'attente de la requête de statut du RmDv en cours
	
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

char * PtrOnString;
UARTStack_ErrorType MyError;
char FSKMssgRec[100];

/* les données clim (RmDDv)*/
int RmDv_ID; /* l'@ source du signal FSK reçu, qui est l'ID de la clim */
RmDvDataTypedef* pClimSalon;
RmDvDataTypedef* pClimSaM;
RmDvDataTypedef* pClimEntree;
RmDvDataTypedef* pClimCouloir;
RmDvDataTypedef* pRmDvExt;

RmDvDataTypedef* Tab_RmDvData[5];

int main (void)
{

	TimeManag_TimeOutInit(); /* obligatoire pour la gestion des TimeOut à tous les étages...*/
	FSKStack_Init(My); /* init de la stack wireless*/
	UARTStack_Init();  /* init de la stack UART pour HMI*/
	TimerStamp_Start(); /* obligatoire pour pouvoir gérer les horodatage*/
	
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



void Transaction_RmDv(char ID)
{
	MssgCode Code;
	int L;
	float TemperatureMesuree;
	char lastTempSet, NewSet;
	unsigned short int NewInterval_sec;
	RmDv_WarningCode status;
	
	/* Recopie locale du message*/
	L=FSKStack_GetLen();
	FSKStack_GetNewMssg(FSKMssgRec,L); 
	
	/* récupération code request*/
	Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
	
	if (Code == MssgReq_SendInfo) /* si le code n'est pas celui de la première requête, alors on
																	ne va pas plus loin */
	{
		/* Lancement temporisation 6*TimeOutProtocole*/
		TimeManag_TimeOutStart(Chrono_2 , TimeOutProtocole_x6_ms);
		/* extract temp & last temp set */
		TemperatureMesuree = RmDv_SGw_FSKP_ExtractTemp(FSKMssgRec);
		lastTempSet = RmDv_SGw_FSKP_ExtracLastSet(FSKMssgRec);
		
		/* Calcul nouveau Set, nouvelle consigne */
		NewSet = 19; //// calcul bidon
		NewInterval_sec = (30*60);
		/*  fin calcul*/
	
		/* Réponse vers le RmDv */
		RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,NewSet, NewInterval_sec);
		status=Error_NoStatusReceived; /* par défaut avant polling*/
		
		
		while(TimeManag_GetTimeOutStatus(Chrono_2)==0) // tant que le time out n'est pas passé
		{
				while(FSKStack_IsNewMssg()==0) // blocage, attente message de n'importe quel RmDv ...
				{}
				/* Recopie locale du message*/
				L=FSKStack_GetLen();
				FSKStack_GetNewMssg(FSKMssgRec,L);
				RmDv_ID=FSKStack_GetSrcAdress();
				if (RmDv_ID==ID)  // C'est bien mon RmDv qui parle...
				{
					Code=RmDv_SGw_FSKP_ExtractMssgcode(FSKMssgRec);
					if (Code == MssgReq_SendInfo) /* le message est une nouvelle tentative */
					{
						/* Réponse vers le RmDv correspondant toujours à la première requête */
						RmDv_SGw_FSKP_SendMssgAns_SendInfo(ID,NewSet, NewInterval_sec);
					}
					else if (Code == MssgReq_SendStatus) /* le message est la requête de status */
					{
						/*Traitement de la seconde req*/
						status=RmDv_SGw_FSKP_ExtracStatus(FSKMssgRec);
						RmDv_SGw_FSKP_SendMssgAns_Ack(ID);
					}
				}
		
		}
		


		/* Mise à jour des données d'entrée et d'état de la RmDvData considérée  */
		RmDvData_Update(Tab_RmDvData[(ID-ID_Clim_Salon)], TemperatureMesuree, lastTempSet, NewSet, status,NewInterval_sec); 
		
		MyLCD_Set_cursor(0, 0);	
		MyLCD_Print("                ");
		MyLCD_Set_cursor(0, 0);
    StringFct_Float2Str(TemperatureMesuree,LCD_RmDv_Str, 3, 1);
		MyLCD_Print("Temp = ");
		MyLCD_Print_n (LCD_RmDv_Str,5);
		MyLCD_Set_cursor(0, 1);	
		MyLCD_Print("                ");
		MyLCD_Set_cursor(0, 1);
		switch (status)
		{
			case NoWarning:  MyLCD_Print("NoWarning");break;
			case Transm_1_Attempt:  MyLCD_Print("first trial");break;
			case Transm_2_Attempt:  MyLCD_Print("second trial");break;
			case Transm_3_Attempt:  MyLCD_Print("third trial");break;
			case Error_TempI2C:  MyLCD_Print("second trial");break;
			case Error_NewTempSetNotReceived:MyLCD_Print("RmDv:no newset");break;
			case Error_NoStatusReceived:MyLCD_Print("Gw:no status");break;
			default : MyLCD_Print("bug");
		}
		
		
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
