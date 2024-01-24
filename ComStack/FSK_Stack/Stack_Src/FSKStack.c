/* =================================================================================
* ==================   MACPhyUART.c	     =================================
 *
 *   Created on: 15/08/23
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *  ------------------------------------------------------------------------------
 *  Pile de communication Xbee Like (@My, @Dest).
 *  Fonctionne en UART Half duplex avec un module FSK (RT606), 38400 Bd max
 *  "réseau" identifié par une préampbule ##### démarrant toute trame.
 *  
 *  Lire pdf associé "LaPile_FSK_COM_STACK_UG_Light.pdf"
 *
* =================================================================================*/


#include "FSKStack.h"







/*
Architecture du module :
-> Variables globales du module
-> Gestion du temps par Systick (TimeOut)
-> API COUCHE MAC
-> USART Callback
-> FSM PhyUART (réception/émission)
-> API COUCHE PHY UART (utilisation déconseillée, préférer API couche MAC)
*/


/*======================================================================================================
========================================================================================================
		Variables globales du module			
========================================================================================================		
=======================================================================================================*/




//#define MyDebug
//#define Log



#ifdef SpyUART
char SpyUART_ByteRec;
#endif

/*---------------------------------
 états possibles de la FSM
----------------------------------*/

typedef enum {
	Init,
	WaitForHeader,
	ReadingFrame,
	CheckSum,
	UpdateMssgForMAC,
	Framing,
	SendMssg
}PhyUART_FSM_StateType;



/*---------------------------------
   VARIABLE D'INTERFACE AVEC MAC
----------------------------------*/


// la structure d'échange MACPhyUART
struct PhyUART_Mssg_type
{
	char StrReceived[StringLenMax-2];				// |Org@|Dest@|Data|  : On enlève la longueur Len (premier octet) et Checksum (dernier octet), 28 octets typiquement
	char LenStrReceived;
	char NewStrReceived;
	// ---< Ajout MAC >-----//
	char MACMatch;													// flag indiquant que l'adresse destination du message est celle de My, utile à la mise à jour
	char My;																// My : Adresse MAC propre du module
	char MACStrReceived[StringLenMax-4];    // |Data| : on enlève les adresses d'origines et de destination, 26 octets typiquement
																					// NB : StrReceived : string en sortie couche Phy incluant les deux
																					// adresses + datas. Sont enlevées la longeur du message initial 
																					// reçu par l'UART (LEN) et le checksum (d'où le -2 dans la longueur 
																					// et le -4 dans la longueur MAC String)
	char MACLenStrReceived;
	char MACNewStrReceived;									// passe à 1 	près remplissage de MACStrReceived
	char MACSrcAdress;
	// ---< Fin Ajout MAC >-----//
	char StrToSend[StringLenMax];
	char LenStrToSend;
	char NewStrToSend;
	PhyUART_StatusType Status;
	PhyUART_ErrorType Error;
}PhyUART_Mssg;


#ifdef Log
PhyUART_FSM_StateType OldState;
PhyUART_FSM_StateType LogTab[20];
//PhyUART_FSM_StateType LogTab[100];
int j=0;
#endif

/*---------------------------------
Gestion détection header
----------------------------------*/
// flag indiquant que lee header est reçu en phase scrutation
char UART_HeaderDetected;
// flag indiquant l'arrivée d'un octer
char UART_Receiv;
// cpteur de caractère Header
char PhyUART_HeaderCarCpt;

#define HeaderCar '#'
#define HeaderCarLenMax 5





/*---------------------------------
String de réception UART
----------------------------------*/
// longueur chaîne lue de la frame
char Phy_UART_Len;
char InComingMssg[StringLenMax];

/*---------------------------------
String de transmmission UART
----------------------------------*/
char Phy_UART_TransmFrameLen;
char Phy_UART_TransmFrame[50];





#ifdef SpyUART
char SpyUART_Is_ByteRec(void)
{
	return SpyUART_ByteRec;
}

void SpyUART_Clear_ByteRecFlag(void)
{
	SpyUART_ByteRec=0;
}

char SpyUART_Get_ByteRec(void)
{
	return USART_FSK_GetByte();
}
#endif




// ---< Ajout MAC >-----//
/*======================================================================================================
========================================================================================================
		API Couche MAC
========================================================================================================		
=======================================================================================================*/

void PhyUART_StartFSM(void);
void PhyUART_Init(void);
/*______________________________________________________________________________
_______________________ void FSKStack_Init(char My)	__________________________
 
    Rôle : Initialise la pile, tout est prêt pour la lancement de la FSM à suivre
					 macro FSKStack_StartFSM(), voir .h
    Param : My, l'adresse propre du module 

* __________________________________________________________________________________*/
void FSKStack_Init(char My)
{
	PhyUART_Mssg.My=My;
	PhyUART_Init();
	PhyUART_StartFSM(); 
}
	


char FSKStack_IsNewMssg(void)
{
	return (PhyUART_Mssg.MACNewStrReceived);
}


/*------------------------------------------------------
   int FSKStack_GetNewMssg (char * AdrString, int Len)
-------------------------------------------------------*/
int FSKStack_GetNewMssg (char * AdrString, int Len)
{
		
// recopie le string stocké dans la chaîne MAC
// mets à 0 les autres octets jusqu'à la fin de la chaîne recevant (Len)
// retourne -1 si la longueur donnée est inférieure à la longueur
// effective de la châine MAC

	int i;
	
	// remise à 0 du flag de réception
	PhyUART_Mssg.MACNewStrReceived=0;
	PhyUART_Mssg.NewStrReceived=0;
	if (Len< PhyUART_Mssg.MACLenStrReceived) 
	{
		return -1;
	}
	else
	{
		for (i=0;i<Len;i++)
		{
			if (i<PhyUART_Mssg.MACLenStrReceived)
			{
				*AdrString=PhyUART_Mssg.MACStrReceived[i];
			}
			else 
			{
				*AdrString=0;
			}
			AdrString++;
		}
		return 0;		
	}
}


/*---------------------------------------------------------------------
   char FSKStack_GetSrcAdress(void)
-----------------------------------------------------------------------*/

char FSKStack_GetSrcAdress(void)
{
	return PhyUART_Mssg.MACSrcAdress;
}


/*---------------------------------------------------------------------
   char FSKStack_GetLen(void);
-----------------------------------------------------------------------*/

char FSKStack_GetLen(void)
{
	return PhyUART_Mssg.MACLenStrReceived;
}




/*---------------------------------------------------------------------
   int FSKStack_SendNewMssg (char DestAdr, char * AdrString, int Len)
-----------------------------------------------------------------------*/
int FSKStack_SendNewMssg (char DestAdr, char * AdrString, int Len)
{
	int i;
	
	
	if (Len>StringLenMax-4) // longueur trop grande, l'envoie ne se fait pas.
	{
		return -1;
	}
	else
	{
		// insertion src@ Dest@
		PhyUART_Mssg.StrToSend[0]=PhyUART_Mssg.My;
		PhyUART_Mssg.StrToSend[1]=DestAdr;
		for (i=2;i<(Len+2);i++)
		{
			PhyUART_Mssg.StrToSend[i]=*AdrString;
			AdrString++;
		}
		PhyUART_Mssg.LenStrToSend=Len+2;
		PhyUART_Mssg.NewStrToSend=1;
		return 0;
	}
}

// ---< Fin Ajout MAC >-----//





















/*======================================================================================================
========================================================================================================
		UART Callback
========================================================================================================		
=======================================================================================================*/


/*---------------------------------------------------------------------
   void UART_Callback(void)
-----------------------------------------------------------------------*/
void UART_Callback(void)
{
	#ifdef MyDebug	
	int i;
// pulse up 
GPIOC->ODR|=GPIO_ODR_ODR3;
for (i=0;i<300;i++);
#endif		
	
	#ifdef SpyUART
	SpyUART_ByteRec=1;
	#endif
	
	
	// indication arrivée d'un octet
	UART_Receiv=1;

	// détection d'un HeaderCar ('#')
	if (USART_FSK_GetByte()==HeaderCar)
	{
		PhyUART_HeaderCarCpt++;
	}
	else PhyUART_HeaderCarCpt=0;
	
	// indication d'une détection du Header
	if (PhyUART_HeaderCarCpt==HeaderCarLenMax) 
	{
		// réglage Echantillonnage à 100µs pour ne pas manquer de caractères à 38400Bds (10*1/38400 = 260µs) 
		TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,100);
		UART_HeaderDetected=1;
	}
	
	#ifdef MyDebug			
// pulse Down 
GPIOC->ODR&=~GPIO_ODR_ODR3;	
#endif
}






















/*======================================================================================================
========================================================================================================
		FSM PhyUART (réception/émission)			
========================================================================================================		
=======================================================================================================*/




PhyUART_FSM_StateType PhyUART_FSM_State;


/*---------------------------------
 Variables et Fct de démarrage FSM
----------------------------------*/
// user fct pour démarrer la FSM
char PhyUART_Start;

/*---------------------------------------------------------------------
   void PhyUART_StartFSM(void)
-----------------------------------------------------------------------*/
void PhyUART_StartFSM(void)
{
	PhyUART_Start=1;
}


void PhyUART_Framing (void);


/*---------------------------------
 PhyUART_FSM_Progress
----------------------------------*/

// index indiquant le numéro d'octet dans la frame entrante
char PhyUART_FrameIndex; // |Len|My@|Dest@|ID|Type_LenData|Data|Trial|CheckSum| 
//                      0   1    2    3    4          ... 

/*---------------------------------------------------------------------
   void PhyUART_FSM_Progress(void)
-----------------------------------------------------------------------*/
void PhyUART_FSM_Progress(void)
{
int Sum,i;
char CRC_Val;
	
#ifdef Log
if (OldState!=PhyUART_FSM_State)
{
	//LogTab[j].Time_100us=SystickGet();
	LogTab[j]=PhyUART_FSM_State;
	//LogTab[j]=PhyUART_FSM_State;
	j++;
	if (j==20) j=0;
	OldState=PhyUART_FSM_State;
}
#endif


	
switch (PhyUART_FSM_State)
	{
		case Init:
		{	
			// *****************************
			//  Etape Init
			// *****************************
			PhyUART_Mssg.Status=Ready;
			PhyUART_Mssg.Error=NoError;
			PhyUART_HeaderCarCpt=0;
			// positionnement module en réception
			USART_FSK_SetReceiveAntenna(); 
			if (PhyUART_Start==1) 
			{
				// Remise réglage Echantillonnage à 1ms
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);
				// ---< Evolution next State >-----//
				PhyUART_FSM_State=WaitForHeader;
			}
			break;
		}
		
				
		case WaitForHeader:
		{	
			// *****************************
			//  Etape Wait for Header 
			// *****************************
			
			// positionnement module en réception
			USART_FSK_SetReceiveAntenna(); 
			PhyUART_Mssg.Status=Listening;
			if (UART_HeaderDetected==1)
			{
				UART_HeaderDetected=0;
				UART_Receiv=0;
				PhyUART_HeaderCarCpt=0;
				PhyUART_FrameIndex=0; // pour préparer le sampling frame
				TimeManag_TimeOutStart(Chrono_1 ,PhyUART_TimeOut); // lancement TimeOut
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,100);
				// ---< Evolution next State >-----//
				PhyUART_FSM_State=ReadingFrame; 
			}
			else if (PhyUART_Mssg.NewStrToSend==1) 
			{
				PhyUART_Mssg.NewStrToSend=0;
				
				// réglage Echantillonnage 100us pour accélérer la FSM)
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000); 
				// ---< Evolution next State >-----//
				PhyUART_FSM_State=Framing;					
			}
			break;
		}
		
		case Framing:
		{
			PhyUART_Mssg.Status=SendingMssg;
			PhyUART_Framing();
			
			// ---< Evolution next State >-----//
			PhyUART_FSM_State=SendMssg;
			break;
		}
		
		case SendMssg:  // ! Maintien ds l'IT pdt toute l'émission...
		{
			PhyUART_Mssg.Status=SendingMssg;
			USART_FSK_SetTransmAntenna();
			Delay_x_ms(4);
			USART_FSK_Print("123456789",9);      // envoie de quelques caractères car le premier byte est souvent dégradé.
																		// Voir avec l'expérience si on peut diminuer le nbre.
			USART_FSK_Print(Phy_UART_TransmFrame,(Phy_UART_TransmFrameLen+5)); // envoie le corps
			USART_FSK_SetReceiveAntenna();  // remise du module en réception

			// ---< Evolution next State >-----//
			PhyUART_FSM_State=WaitForHeader;
			break;
		}
		
		case ReadingFrame:
		{	
			// *****************************
			//  Etape Reading Frame
			// *****************************


			
			
			PhyUART_Mssg.Error=NoError;
			if (TimeManag_GetTimeOutStatus(Chrono_1)==0) // Traitement si on n'est pas en time out !
			{
			
				PhyUART_Mssg.Status=ReceivingMssg;
				// Frame : Len||My@|Dest@|ID|Type_LenData|Data|Trial|CheckSum| 
				if (UART_Receiv==1) // une data UART vient d'arriver
				{
				
					UART_Receiv=0;
					// c'est la LEN qu'on est en train de traiter
					// Index = 0
					if (PhyUART_FrameIndex==0) 
					{
						PhyUART_FrameIndex++;
						Phy_UART_Len=USART_FSK_GetByte();
						if ((Phy_UART_Len>StringLenMax)||(Phy_UART_Len<StringLenMin))  // si vrai on a une frame trop longue,
																												// ou trop courte : il faut avorter et revenir à WaitForHeader
						{
							PhyUART_Mssg.Error=LenError;
							// Remise réglage Echantillonnage à 1ms
							TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);			
							// ---< Evolution next State >-----//
							PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
						}
					}
					// Ici, la longueur a été jugée bonne.
					else // tous les autres bytes de la frame sont samplé jusqu'au dernier PhyUART_FrameIndex>0
					// c'est le reste de la chaîne qu'on traîte
					// Index = 1 à LEN-1
					{
					// exemple : Frame = Len et 5 octets. Soit LEN = 6
					//       LEN a b c d e 
					//  idx   0  1 2 3 4 5 
					//           1 ..... LEN-1
					//   On veut obtenir
					//  InCommingMssg ="a b c d e" soit une table [LEN-1] qui va de 0 à LEN-2
					//                  0 1 2 3 4    
					//  après la dernière passe, donc après inc de Idx, Idx vaut 6 C'est la condition d'arrêt
						InComingMssg[PhyUART_FrameIndex-1]=USART_FSK_GetByte();
						PhyUART_FrameIndex++; 
						if (PhyUART_FrameIndex==Phy_UART_Len) 
						{
							PhyUART_FrameIndex=0;
							// Remise réglage Echantillonnage à 1ms
							TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);
							// ---< Evolution next State >-----//
							PhyUART_FSM_State=CheckSum;
						}
					}
				}	
			}
			else // timeout
			{
				PhyUART_Mssg.Error=TimeOutError;
				// Remise réglage Echantillonnage à 1ms
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);
 				// ---< Evolution next State >-----//				
				PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
			}
			

			break;
			
			
			
		}
		case CheckSum:
		{	

			// *****************************
			//  Etape CheckSum
			// *****************************			
			PhyUART_Mssg.Error=NoError;
			PhyUART_Mssg.Status=ReceivingMssg;
			// reconstruction de la valeur du checksum = dernier octet lu
			CRC_Val=InComingMssg[Phy_UART_Len-2];
			
			Sum=0;
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				Sum=Sum+InComingMssg[i];
			}
			// ajoutons l'octet LEN pour être complet
			Sum=Sum+Phy_UART_Len;
			
			if (CRC_Val==(char)Sum) // Checksum OK
			{
				// ---< Evolution next State >-----//	
				PhyUART_FSM_State=UpdateMssgForMAC;
			}
			else
			{
				PhyUART_Mssg.Error=CheckSumError;		
				
				// ---< Evolution next State >-----//				
				PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
			}
			break;
		}
		case UpdateMssgForMAC:
		{	
			PhyUART_Mssg.Status=ReceivingMssg;
			// ---< Ajout MAC >-----//
			PhyUART_Mssg.MACMatch=0;
			if (PhyUART_Mssg.MACNewStrReceived==1) PhyUART_Mssg.Error=MACOverRunError;
			// ---< Fin Ajout MAC >-----//
			if (PhyUART_Mssg.NewStrReceived==1) PhyUART_Mssg.Error=OverRunError;
			
			// recopie de la chaîne 
			// de |Dest@|Org@|ID|Type_LenData|Data|Trial|CheckSum|
			// vers |Dest@|Org@|ID|Type_LenData|Data|Trial|
			// donc on enlève le checksum = dernier octet de IncomminMssg
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				PhyUART_Mssg.StrReceived[i]=InComingMssg[i];
				// ---< Ajout MAC >-----//
				if (i==0)  PhyUART_Mssg.MACSrcAdress=InComingMssg[i];
				else if (i==1) // filtrage @Destination
				{
					if ((InComingMssg[i]==PhyUART_Mssg.My)||(InComingMssg[i]==0xFF)) PhyUART_Mssg.MACMatch=1;
					// NB -1 = 0xFF, broadcast
				}
				else if (PhyUART_Mssg.MACMatch==1) // sampling de MACstring
				{
					PhyUART_Mssg.MACStrReceived[i-2]=PhyUART_Mssg.StrReceived[i]; // Longueur MAC = Longeur Phy -2 puisqu'on sort les deux @
				}
				// ---< Fin Ajout MAC >-----//
			}	
			PhyUART_Mssg.LenStrReceived=Phy_UART_Len-2;
			PhyUART_Mssg.NewStrReceived=1;
			
			// ---< Ajout MAC >-----//
			if (PhyUART_Mssg.MACMatch==1)
			{
				PhyUART_Mssg.MACLenStrReceived=Phy_UART_Len-4;
				PhyUART_Mssg.MACNewStrReceived=1;  
			}
			PhyUART_Mssg.MACMatch=0;
			// ---< Fin Ajout MAC >-----//
			
			// ---< Evolution next State >-----//
			PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
			

			break;
		}

	}
}



/*---------------------------------------------------------------------
   void PhyUART_Framing (void)
-----------------------------------------------------------------------*/
void PhyUART_Framing (void)
{

	int Sum,i;

	
	Phy_UART_TransmFrameLen=PhyUART_Mssg.LenStrToSend+2; // ajout de l'octet qui est la longueur + octet checksum
	// Encapsulation et calcul Checksum
	for (i=0;i<5;i++)
	{
		Phy_UART_TransmFrame[i]='#';
	}
	Phy_UART_TransmFrame[5]=Phy_UART_TransmFrameLen;
	Sum=Phy_UART_TransmFrameLen; // on l'ajoute dès le départ avant d'ajouter les char de ArdString
	for (i=0;i<PhyUART_Mssg.LenStrToSend;i++)
	{
		Phy_UART_TransmFrame[i+6]= PhyUART_Mssg.StrToSend[i];  
		Sum=Sum+Phy_UART_TransmFrame[i+6];
	}
	Phy_UART_TransmFrame[Phy_UART_TransmFrameLen+4]=(char)Sum; // insertion du checksum
	// La frame mesure donc  5 (les #)  +1 (FrameLen) + Len (les data du param) + 1 (checksum) = Len+7 = FrameLen+5	
}


/*---------------------------------------------------------------------
   int PhyUART_SendNewMssg (char * AdrString, int Len)
-----------------------------------------------------------------------*/
int PhyUART_SendNewMssg (char * AdrString, int Len)
{
	int i;
	if (Len>StringLenMax-2) // longueur trop grande, l'envoie ne se fait pas.
	{
		return -1;
	}
	else
	{
		for (i=0;i<Len;i++)
		{
			PhyUART_Mssg.StrToSend[i]=*AdrString;
			AdrString++;
		}
		PhyUART_Mssg.LenStrToSend=Len;
		PhyUART_Mssg.NewStrToSend=1;
		return 0;
	}
}


















/*======================================================================================================
========================================================================================================
		API COUCHE PHY UART			
========================================================================================================		
=======================================================================================================*/


/******************************************************************************************************************
	PhyUART_Init

Rôle :
Param : 
*****************************************************************************************************************/


/*---------------------------------------------------------------------
   void PhyUART_Init(void)
-----------------------------------------------------------------------*/
void PhyUART_Init(void)
{
		
#ifdef Log
	OldState=Init;
#endif
	
	USART_FSK_Init(PhyUART_BdRate,UART_Prio_CD,UART_Prio,UART_Callback);
	USART_FSK_SetReceiveAntenna(); // place le module FSK en réception
	PhyUART_FSM_State=Init;
	PhyUART_Mssg.Status=Ready;

	
	// mise en place interruption, TIM FSM à 1ms
	TimeManag_FSMTimerInit(TIM_PhyUART_FSM, PhyUART_FSM_Prio, PhyUART_FSM_Progress,1000);
	
	// Initialisation du gestionnaire de TimeOut (Systick)
	TimeManag_TimeOutInit();
	
	
#ifdef MyDebug	
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPCEN;
	GPIOC->CRL&=~(0xF<<(3%8)*4); // output ppull 2MHz
	GPIOC->CRL|=(0x1<<(3%8)*4);
#endif
	
}



/******************************************************************************************************************
	IO Fcts

Rôle :
Param : 
*****************************************************************************************************************/


/*---------------------------------------------------------------------
   char PhyUART_IsNewMssg(void)
-----------------------------------------------------------------------*/
char PhyUART_IsNewMssg(void)
{
	return (PhyUART_Mssg.NewStrReceived);
}


/*---------------------------------------------------------------------
   PhyUART_StatusType PhyUART_Get_Status(void)
-----------------------------------------------------------------------*/
PhyUART_StatusType PhyUART_Get_Status(void)
{
	return (PhyUART_Mssg.Status);
}

/*---------------------------------------------------------------------
   PhyUART_ErrorType PhyUART_Get_Error(void)
-----------------------------------------------------------------------*/
PhyUART_ErrorType PhyUART_Get_Error(void)
{
	return (PhyUART_Mssg.Error);
}

/*---------------------------------------------------------------------
   int  PhyUART_GetNewMssg (char * AdrString, int Len)
-----------------------------------------------------------------------*/
int  PhyUART_GetNewMssg (char * AdrString, int Len)
// recopie le string stocké à l'interface MAC et 
// mets à 0 les autres octets jusqu'à StringLenMax.
// retourne -1 si la longueur donnée est inférieure à la longueur
// effective de la châine
{
	int i;
	
	// remise à 0 du flag de réception
	PhyUART_Mssg.NewStrReceived=0;
	if (Len<Phy_UART_Len-2) 
	{
		return -1;
	}
	else
	{
		for (i=0;i<Len;i++)
		{
			if (i<Phy_UART_Len-2)
			{
				*AdrString=PhyUART_Mssg.StrReceived[i];
			}
			else 
			{
				*AdrString=0;
			}
			AdrString++;
		}
		return 0;		
	}
}


/*----------------- Ajout 02/09/23 ----------------------*/
void FSKStack_Reset_Restart_KeepMy()
{
	int i;
	for (i=0;i<StringLenMax-2;i++)
	{
		PhyUART_Mssg.StrReceived[i]=0;
	}
	PhyUART_Mssg.LenStrReceived=0;
	PhyUART_Mssg.NewStrReceived=0;
	PhyUART_Mssg.MACMatch=0;
	for (i=0;i<StringLenMax-4;i++)
	{
		PhyUART_Mssg.MACStrReceived[i]=0;
	}
	PhyUART_Mssg.MACLenStrReceived=0;
	PhyUART_Mssg.MACNewStrReceived=0;
	PhyUART_Mssg.MACSrcAdress=0;
	for (i=0;i<StringLenMax;i++)
	{
		PhyUART_Mssg.StrToSend[i]=0;
	}
	
	PhyUART_Mssg.LenStrToSend=0;
	PhyUART_Mssg.NewStrToSend=0;
	PhyUART_Mssg.Status=Ready;	
	PhyUART_Mssg.Error=NoError;
		
	USART_FSK_SetReceiveAntenna(); // place le module FSK en réception
	PhyUART_FSM_State=Init;
	PhyUART_Mssg.Status=Ready;
	PhyUART_StartFSM();
	
}

