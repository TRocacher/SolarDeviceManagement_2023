#include "ExchangeCtrl.h"
#include "MACPhyUART.h"
#include "MyTimer.h"
#include "FctDiverses.h"



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
	ExchStatus_IDMismatch,
	ExchStatus_TabFull,
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


// la structure d'échange MACPhyUART
typedef struct 
{
	ExchCtrl_Status Status;
	char OrgAdr;
	char DestAdr;
	char ID;
	char Type;
	char LenData;
	char Data[StringLenMax-4];
	char Trial;
	int TimeOutDate;
	ExchCtrl_FSM_StateEnum FSMState;
	ExchCtrl_FSM_Transition FSMTrans;
}Exchange;


/*------------------------------------------------------------------------
   La structure ExchCtrlArrival
-------------------------------------------------------------------------*/

typedef enum {
	Exch_Init,
	Exch_WaitForMACMssg,
	Exch_ExchangeCreationUpdate,
	Exch_BlankingTimeForMssgReading,
}ExchCtrlArrival_FSM_State;




//**************************************************************************************************************
//**************************************************************************************************************
// 							VARIABLES IMPORTANTES DE LA COUCHES EXCHANGE
//**************************************************************************************************************
//**************************************************************************************************************

// Top of Exchange Layer
Exchange ExchTab[10];
int ExchIndex;

// Bottom of Exchange Layer
struct
{
	ExchCtrlArrival_FSM_State FSM_State;
	char ExchCtrlArrival_Start;
	char CurrentExchQte;
	char Error;
	char NewMACExchange;
	Exchange MACExchange;
}ExchCtrlArrival;



//**************************************************************************************************************
//**************************************************************************************************************
// 							Fonctions de configurations, utilisations
//**************************************************************************************************************
//**************************************************************************************************************

void ExchCtrlArrival_FSM_Progress(void);
void ExchCtrl_Init(void)
{
	// Activation et démarrage de la FSM couche MACPhy
	PhyUART_Init();
	PhyUART_StartFSM();
	
	// Configuration du Timer pilotant la FSM ExchCtrlArrival
	MyTimer_CkEnable(TIM_ExchangeCtrlLayer);
	MyTimer_IT_Enable( TIM_ExchangeCtrlLayer, ExchangeCtrlLayer_Prio, ExchCtrlArrival_FSM_Progress);
  MyTimer_Set_Period(TIM_ExchangeCtrlLayer, 10000-1, 360-1 ); // 50ms

	// Initialisation première étape FSM
	ExchCtrlArrival.FSM_State=Exch_Init;
}


void ExchCtrlArrival_StartFSM(void)
{
	ExchCtrlArrival.ExchCtrlArrival_Start=1;
}


//**************************************************************************************************************
//**************************************************************************************************************
// 							La FSM ExchangeCtrlSubLauer
//**************************************************************************************************************
//**************************************************************************************************************
void ExchCtrlArrival_FSM_Progress(void)
{
	switch (ExchCtrlArrival.FSM_State)
	// Trame translise par MAC 
	// |ID|Type_LenData|Data|Trial| avec Type_LenData = ttLL LLLL, avec tt = type (2bits) LL LLLL = longueur (6 bits)
	
	
	{
		case Exch_Init:
		{
			if (ExchCtrlArrival.ExchCtrlArrival_Start==1) ExchCtrlArrival.FSM_State=Exch_WaitForMACMssg;
			break;
		}
		case Exch_WaitForMACMssg:
		{
			if (MACPhyUART_IsNewMssg()==1)ExchCtrlArrival.FSM_State=Exch_ExchangeCreationUpdate;
			// reconstruire la chaine
			break;
		}
		case Exch_ExchangeCreationUpdate:
		{
			
			
			
			break;
		}
		case Exch_BlankingTimeForMssgReading:
		{
			break;
		}
	}
	
}



