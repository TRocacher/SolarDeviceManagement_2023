#include "ExchangeCtrl.h"
#include "MACPhyUART.h"
#include "MyTimer.h"
#include "FctDiverses.h"

#define My 0xAA

/*------------------------------------------------------------------------
   Valeurs possibles pour les états des 4 FSM possibles dans les échanges
-------------------------------------------------------------------------*/
typedef enum {
	Req_Init,
	Req_ExchCreation,
	Req_FailToCreate,
	Req_SendToMAC,
	Req_IncTrial,
	Req_TimeOut,
	Req_FrameReceived,
	
	AsyMssg_Init,
	AsyMssg_Creation,
	AsyMssg_FailToCreate,
	AsyMssg_SendToMAC,
	AsyMssg_IncTrial,
	AsyMssg_TimeOut,
	AsyMssg_FrameAck,
	
}ExchCtrl_FSM_StateEnum;



/*------------------------------------------------------------------------
   Les divers status possibles pour un échange
-------------------------------------------------------------------------*/
typedef enum {
	ExchStatus_Free,
	ExchStatus_Ready,
	ExchStatus_Error, 	//erreur Arrivée
	ExchStatus_WaitOnDataAckRecept,
	ExchStatus_FailToReceive,
	ExchStatus_DataAckJustReceived,
	ExchStatus_WaitForAppData,
	ExchStatus_WaitForAppAck, 
}ExchCtrl_Status;



/*------------------------------------------------------------------------
   Les transitions possibles pour les FSM associées aux échanges
-------------------------------------------------------------------------*/
typedef struct 
{
	char Start;
	char TimeOut;
	char ReqAsy_CreateExchOK;
	char Req_DataFromMAC;
	char Req_FrameReadByAppli;
	char Asy_Ack;	
	char Data_DataFromAppli;
	char Ack_AckFromAppli;
}ExchCtrl_FSM_Transition;



/*------------------------------------------------------------------------
   La structure échange
-------------------------------------------------------------------------*/
typedef enum {
	Exch_Req,
	Exch_Asy,
	Exch_Data,
	Exch_Ack,
	Exch_Undefine,
}Exch_Type;

#define LenAppliDataMax (StringLenMax-4-3)
// la structure d'échange 
typedef struct 
{
	ExchCtrl_Status Status;
	char OrgAdr;
	char DestAdr;
	char ID;
	Exch_Type Type;
	char LenData;
	char Data[LenAppliDataMax];
	char Trial;
	int TimeOutDate;
	ExchCtrl_FSM_StateEnum FSMState;
	ExchCtrl_FSM_Transition FSMTrans;
}Exchange;


/*------------------------------------------------------------------------
   Les états de la FSM Arrival
-------------------------------------------------------------------------*/

typedef enum {
	Exch_Init,
	Exch_WaitForMACMssg,
	Exch_ArrivalPreProcessing,
	Exch_ExchangeCreationUpdate,
	Exch_BlankingTimeForMssgReading,
}ExchCtrlArrival_FSM_State;




/*---------------------------------
Gestion du time out
----------------------------------*/
int Arrival_TimeOutDate;  // la date d'échéance
#define ArrivalBlankTime_ms 1000

void Arrival_TimeOut_Start(int ms)
{
	Arrival_TimeOutDate=10*ms+SystickGet();
}

char Arrival_GetTimeOut_Status(void)
{
	if ((SystickGet()-Arrival_TimeOutDate)>=0) return 1;
	else return 0;
}



//**************************************************************************************************************
//**************************************************************************************************************
// 							VARIABLES IMPORTANTES DE LA COUCHES EXCHANGE 
//							- Table d'échange
//							- ExchCtrlArrival (objet qui gère la couche basse, ARRIVAL)
//**************************************************************************************************************
//**************************************************************************************************************

// Top of Exchange Layer
#define ExchTabSize 10
Exchange ExchTab[ExchTabSize];
int ExchArrivalIndex;

// Bottom of Exchange Layer
typedef enum {
	Arrival_NoError,
	Arrival_PhyUARTError,
	Arrival_DataTooLong,
	Arrival_ExchMismatch,
}ExchArrival_ErrorType;


struct
{
	ExchCtrlArrival_FSM_State FSM_State;
	char ExchCtrlArrival_Start;
	char CurrentExchQte;
	ExchArrival_ErrorType Error;
	//char NewReqAsyMACExchange;
	Exchange MACExchange;
}ExchCtrlSituation;

char Exch_IncomingMssg[StringLenMax-4];











//**************************************************************************************************************
//**************************************************************************************************************
// 							Fonctions de configurations, utilisations
//**************************************************************************************************************
//**************************************************************************************************************
void ExchCtrl_ResetExchange(Exchange ThisExchange);
void ExchCtrlArrival_FSM_Progress(void);

void ExchCtrl_Init(void)
{
	int i;
	// reset complet de la table d'échange
	for (i=0;i<10;i++)
	{
		ExchCtrl_ResetExchange(ExchTab[i]);
	}
	
	// reset complet de l'échange de ExchCtrlSituation
	ExchCtrl_ResetExchange(ExchCtrlSituation.MACExchange);
	
	ExchCtrlSituation.FSM_State=Exch_Init;
	ExchCtrlSituation.CurrentExchQte=0;	
	ExchCtrlSituation.Error=Arrival_NoError;
	
	
	
	// Activation et démarrage de la FSM couche MACPhy
	PhyUART_Init();
	PhyUART_StartFSM();
	
	// Configuration du Timer pilotant la FSM ExchCtrlArrival
	MyTimer_CkEnable(TIM_ExchangeCtrlLayer);
	MyTimer_IT_Enable( TIM_ExchangeCtrlLayer, ExchangeCtrlLayer_Prio, ExchCtrlArrival_FSM_Progress);
  MyTimer_Set_Period(TIM_ExchangeCtrlLayer, 10000-1, 360-1 ); // 50ms

}


void ExchCtrlArrival_StartFSM(void)
{
	ExchCtrlSituation.ExchCtrlArrival_Start=1;
}


void ExchCtrl_ResetFSMTransition(ExchCtrl_FSM_Transition ThisFSMTrans)
{
	ThisFSMTrans.Ack_AckFromAppli=0;
	ThisFSMTrans.Asy_Ack=0;
	ThisFSMTrans.Data_DataFromAppli=0;
	ThisFSMTrans.ReqAsy_CreateExchOK=0;
	ThisFSMTrans.Req_DataFromMAC=0;
	ThisFSMTrans.Req_FrameReadByAppli=0;
	ThisFSMTrans.Start=0;
	ThisFSMTrans.TimeOut=0;
}


void ExchCtrl_ResetExchange(Exchange ThisExchange)
{
	int i;
	ThisExchange.Status=ExchStatus_Free;
	ThisExchange.OrgAdr=0;
	ThisExchange.DestAdr=0;
	ThisExchange.ID=0;	
	ThisExchange.Type=Exch_Undefine;
	ThisExchange.LenData=0;
	for (i=0;i<LenAppliDataMax;i++) ThisExchange.Data[i]=0;
	ThisExchange.Trial=0;
	ThisExchange.TimeOutDate=0;
	ThisExchange.FSMState=Req_Init; 
	ExchCtrl_ResetFSMTransition(ThisExchange.FSMTrans);
}
	
// renvoie l'index, 0xFF si non trouvé
int ExchCtrl_CheckExchExistance(char ThisID, Exch_Type ThisType, char ThisOrgAdress)
{
	int i;
	for (i=0;i<ExchTabSize;i++)
	{
		if (ExchTab[i].ID ==ThisID)
		{
			if (ExchTab[i].Type ==ThisType)
			{
				if (ExchTab[i].DestAdr ==ThisOrgAdress)
				{
					return i;
				}
			}
		}
	}
	// pas de match
	return -1;
}




int ExchCtrlCreateExch(void)
{
int i;
	if (ExchCtrlSituation.CurrentExchQte>=ExchTabSize) // la table est pleine
	{
		return -1; // la table est pleine	
	}
	else
	{
		for (i=0;i<ExchTabSize;i++)
		{
			if (ExchTab[i].Status==ExchStatus_Free)
			{
				ExchTab[i].Status=ExchStatus_Ready;
				ExchCtrlSituation.CurrentExchQte++;
				if (ExchCtrlSituation.CurrentExchQte>ExchTabSize) while(1); // bug de gestion nbre d'échange JAMAIS !
				return i;
			
			}
		}
	}	
}

void ExchCtrlDestroyExch(int Index)
{
	ExchTab[Index].Status=ExchStatus_Free;
	ExchCtrlSituation.CurrentExchQte--;
	if (ExchCtrlSituation.CurrentExchQte<0) while(1); // bug de gestion nbre d'échange JAMAIS !
}




//**************************************************************************************************************
//**************************************************************************************************************
// 							La FSM Request
//**************************************************************************************************************
//**************************************************************************************************************
void ExchCtrl_Req_FSM_Progress(int Index)
{
	switch (ExchTab[Index].FSMState)
	{
		case Req_Init:
		{
			if (ExchTab[Index].FSMTrans.Start==1)
			{
				ExchTab[Index].FSMTrans.Start=0;
				// ---< Evolution next State >-----//		
				ExchTab[Index].FSMState=Req_ExchCreation;
			}
			break;
		}
		
		case Req_ExchCreation:
		{
			// Création de l'échange
			
			if (ExchCtrlCreateExch()==-1) ExchTab[Index].FSMState=Req_FailToCreate; // ---< Evolution next State >-----//	
			
					??	ExchTab[Index].Trial=1;
			break;
		}

		case Req_FailToCreate:
		{
			break;
		}
		
		case Req_SendToMAC:
		{
			break;
		}

		case Req_IncTrial:
		{
			break;
		}
		
		case Req_TimeOut:
		{
			break;
		}	
		
		case Req_FrameReceived:
		{
			break;
		}			
		
	}
	
}






//**************************************************************************************************************
//**************************************************************************************************************
// 							La FSM ExchangeCtrlSubLayer
//**************************************************************************************************************
//**************************************************************************************************************
void ExchCtrlArrival_FSM_Progress(void)
{
	int i;
	char LenIncoming;
	
	switch (ExchCtrlSituation.FSM_State)
	// Trame translise par MAC 
	// |ID|Type_LenData|Data|Trial| avec Type_LenData = ttLL LLLL, avec tt = type (2bits) LL LLLL = longueur (6 bits)
	
	
	{
		case Exch_Init:
		{
			if (ExchCtrlSituation.ExchCtrlArrival_Start==1)
			{
				ExchCtrlSituation.ExchCtrlArrival_Start=0;
				ExchCtrlSituation.FSM_State=Exch_WaitForMACMssg;
				
			}
			break;
		}
		
		case Exch_WaitForMACMssg:
		{
	
			if (MACPhyUART_IsNewMssg()==1)
			{
			ExchCtrlSituation.Error=Arrival_NoError;
			// reset complet de l'échange de ExchCtrlSituation
			ExchCtrl_ResetExchange(ExchCtrlSituation.MACExchange);

			// ---< Evolution next State >-----//
			ExchCtrlSituation.FSM_State=Exch_ArrivalPreProcessing;	
			}
			
			break;
		}
		
		
		
		case Exch_ArrivalPreProcessing:
		{
			// reconstruire la chaine, 
			if (MACPhyUART_GetNewMssg(Exch_IncomingMssg,StringLenMax-4)==-1) // le flag IsNewMssg passe à 0 automatiquement
			{
				ExchCtrlSituation.Error=Arrival_PhyUARTError;
				// ---< Evolution next State >-----//		
				ExchCtrlSituation.FSM_State=Exch_WaitForMACMssg;
			}
						
			else
			{
				// samplig MACExchange : 
				// |ID|Type_LenData|Data|Trial| avec Type_LenData = ttLL LLLL, avec tt = type (2bits) LL LLLL = longueur (6 bits)
				ExchCtrlSituation.MACExchange.DestAdr=My;
				ExchCtrlSituation.MACExchange.OrgAdr=MACPhyUART_GetSrcAdress();
				ExchCtrlSituation.MACExchange.ID=Exch_IncomingMssg[0];
				ExchCtrlSituation.MACExchange.Type =(Exch_Type)(Exch_IncomingMssg[1]>>6);
				LenIncoming=Exch_IncomingMssg[1]&0x3F;
				ExchCtrlSituation.MACExchange.LenData=LenIncoming;
				
				if (LenIncoming>LenAppliDataMax) 
				{
					ExchCtrlSituation.Error=Arrival_DataTooLong;
					// ---< Evolution next State >-----//		
				  ExchCtrlSituation.FSM_State=Exch_WaitForMACMssg;
				}
				else // Tout s'est bien passé jusqu'ici..
				{
					// samplig des data, sans 0 padding (fait avant)
					for (i=0;i<LenIncoming;i++)
					{
						ExchCtrlSituation.MACExchange.Data[i]=Exch_IncomingMssg[i+2];
					}
					
					
					// ---< Evolution next State >-----//
					ExchCtrlSituation.FSM_State=Exch_ExchangeCreationUpdate;
					
				}
			}
			break;
		}
		
		
		case Exch_ExchangeCreationUpdate:
		{
			if (ExchCtrlSituation.MACExchange.Type==Exch_Data) // fais donc suite à une requête précédente
			{
				// vérification, recherche de l'échange de type requête existant ds la table
				// match ID, match dual type (Req si c'est une data qui est arrivée), Adresse dest Req = @ d'origine
				ExchArrivalIndex=ExchCtrl_CheckExchExistance(ExchCtrlSituation.MACExchange.ID, Exch_Req, ExchCtrlSituation.MACExchange.OrgAdr);
				if (ExchArrivalIndex==-1) // Erreur ExchMismatch
				{
					ExchCtrlSituation.Error=Arrival_ExchMismatch;
					// ---< Evolution next State >-----//		
					ExchCtrlSituation.FSM_State=Exch_WaitForMACMssg;
				}
				else // match trouvé
				{
					//Copy Data
					ExchTab[ExchArrivalIndex].LenData=ExchCtrlSituation.MACExchange.LenData;
					for (i=0;i<ExchCtrlSituation.MACExchange.LenData;i++)
					{
						ExchTab[ExchArrivalIndex].Data[i]=Exch_IncomingMssg[i];
					}
					// Préciser Data arrivée dans la FSM en cours correspondant à l'échange (déblocage FSM Req)
					ExchTab[ExchArrivalIndex].FSMTrans.Req_DataFromMAC=1;
					// Lancement Timeout pour laisser le tps à l'application de lire la data
					Arrival_TimeOut_Start(ArrivalBlankTime_ms);
					// ---< Evolution next State >-----//		
					ExchCtrlSituation.FSM_State=Exch_BlankingTimeForMssgReading;
				}
			}


//Si Type == Ack (Update)
//	Si Echange existe (vérification), mettre à jour (en particulier les champs Data et FSM_Trans. AckFromMAC=1)
//	Sinon Status.Erreur ExchMismatch
//Si Type == Req (Creation)
//	Création Exchange
//	Si table not full mettre à jour (en particulier les champs Data et FSM_Trans. ReqAsyMssgFromMAC=1)
//	Sinon Status.Erreur FullTable
//Si Type == AsyMssg (Creation)
//	Création Exchange
//	Si table not full mettre à jour (en particulier les champs Data et FSM_Trans. ReqAsyMssgFromMAC=1)
//	Sinon Status.Erreur FullTable
//FinSI	
			
			break;
		}
			
		case Exch_BlankingTimeForMssgReading:
		{
			if ((ExchTab[ExchArrivalIndex].FSMTrans.Req_DataFromMAC==0)||(Arrival_GetTimeOut_Status()==1))
			{
				// la donnée a été lue par la FSM supérieure ou timeout
				// ---< Evolution next State >-----//		
				ExchCtrlSituation.FSM_State=Exch_WaitForMACMssg;
			}
			break;
		}
	}
	
}



