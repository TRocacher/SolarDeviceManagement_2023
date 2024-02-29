/* =================================================================================
* ==================   FSK_Stack.c	     =================================
 *
 *   Created on: 15/08/23
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *  ------------------------------------------------------------------------------
 *  Pile de communication Xbee Like (@My, @Dest).
 *  Fonctionne en UART Half duplex avec un module FSK (RT606), 38400 Bd max
 *  "r�seau" identifi� par une pr�ampbule ##### d�marrant toute trame.
 *  
 *  Lire pdf associ� "FSK_Stack_UG_Light.pdf"
 *
* =================================================================================*/


#include "FSKStack.h"







/*
Architecture du module :
-> Variables globales du module
-> Gestion du temps par Systick (TimeOut)
-> API COUCHE MAC
-> USART Callback
-> FSM PhyUART (r�ception/�mission)
-> API COUCHE PHY UART (utilisation d�conseill�e, pr�f�rer API couche MAC)
*/


/*======================================================================================================
========================================================================================================
		Variables globales du module			
========================================================================================================		
=======================================================================================================*/




//#define MyDebug
#define Log
const char Preambule[4]={0xFF,0xFF,0xFF,0xFF};


/*---------------------------------
 �tats possibles de la FSM
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


// la structure d'�change MACPhyUART
struct PhyUART_Mssg_type
{
	char StrReceived[StringLenMax-2];				// |Org@|Dest@|Data|  : On enl�ve la longueur Len (premier octet) et Checksum (dernier octet), 28 octets typiquement
	char LenStrReceived;
	char NewStrReceived;
	// ---< Ajout MAC >-----//
	char MACMatch;													// flag indiquant que l'adresse destination du message est celle de My, utile � la mise � jour
	char My;																// My�: Adresse MAC propre du module
	char MACStrReceived[StringLenMax-4];    // |Data| : on enl�ve les adresses d'origines et de destination, 26 octets typiquement
																					// NB�: StrReceived�: string en sortie couche Phy incluant les deux
																					// adresses + datas. Sont enlev�es la longeur du message initial 
																					// re�u par l'UART (LEN) et le checksum (d'o� le -2 dans la longueur 
																					// et le -4 dans la longueur MAC String)
	char MACLenStrReceived;
	char MACNewStrReceived;									// passe � 1 	pr�s remplissage de MACStrReceived
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
int j_=0;
#endif

/*---------------------------------
Gestion d�tection header
----------------------------------*/
// flag indiquant que lee header est re�u en phase scrutation
char UART_HeaderDetected;
// flag indiquant l'arriv�e d'un octer
char UART_Receiv;
// cpteur de caract�re Header
char PhyUART_HeaderCarCpt;

#define HeaderCar '#'
#define HeaderCarLenMax 5





/*---------------------------------
String de r�ception UART
----------------------------------*/
// longueur cha�ne lue de la frame
char Phy_UART_Len;
char InComingMssg[StringLenMax];

/*---------------------------------
String de transmmission UART
----------------------------------*/
char Phy_UART_TransmFrameLen;
char Phy_UART_TransmFrame[50];










// ---< Ajout MAC >-----//
/*======================================================================================================
========================================================================================================
		API Couche MAC
========================================================================================================		
=======================================================================================================*/


void PhyUART_StartFSM(void);
void PhyUART_Init(void);
/*______________________________________________________________________________
_______________________ void MACPhyUART_Init(char My)	__________________________
 
    R�le : Initialise la pile, tout est pr�t pour la lancement de la FSM � suivre
					 macro MACPhyUART_StartFSM(), voir .h
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
   int MACPhyUART_GetNewMssg (char * AdrString, int Len)
-------------------------------------------------------*/
int FSKStack_GetNewMssg (char * AdrString, int Len)
{
		
// recopie le string stock� dans la cha�ne MAC
// mets � 0 les autres octets jusqu'� la fin de la cha�ne recevant (Len)
// retourne -1 si la longueur donn�e est inf�rieure � la longueur
// effective de la ch�ine MAC

	int i;
	
	// remise � 0 du flag de r�ception
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
   char MACPhyUART_GetSrcAdress(void)
-----------------------------------------------------------------------*/

char FSKStack_GetSrcAdress(void)
{
	return PhyUART_Mssg.MACSrcAdress;
}


/*---------------------------------------------------------------------
   char MACPhyUART_GetLen(void);
-----------------------------------------------------------------------*/

char FSKStack_GetLen(void)
{
	return PhyUART_Mssg.MACLenStrReceived;
}




/*---------------------------------------------------------------------
   int MACPhyUART_SendNewMssg (char DestAdr, char * AdrString, int Len)
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
	
	// indication arriv�e d'un octet
	UART_Receiv=1;

	// d�tection d'un HeaderCar ('#')
	if (USART_FSK_GetByte()==HeaderCar)
	{
		PhyUART_HeaderCarCpt++;
	}
	else PhyUART_HeaderCarCpt=0;
	
	// indication d'une d�tection du Header
	if (PhyUART_HeaderCarCpt==HeaderCarLenMax) 
	{
		// r�glage Echantillonnage � 100�s pour ne pas manquer de caract�res � 38400Bds (10*1/38400 = 260�s) 
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
		FSM PhyUART (r�ception/�mission)			
========================================================================================================		
=======================================================================================================*/




PhyUART_FSM_StateType PhyUART_FSM_State;


/*---------------------------------
 Variables et Fct de d�marrage FSM
----------------------------------*/
// user fct pour d�marrer la FSM
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

// index indiquant le num�ro d'octet dans la frame entrante
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
	LogTab[j_]=PhyUART_FSM_State;
	//LogTab[j]=PhyUART_FSM_State;
	j_++;
	if (j_==20) j_=0;
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
			// positionnement module en r�ception
			USART_FSK_SetReceiveAntenna(); 
			if (PhyUART_Start==1) 
			{
				// Remise r�glage Echantillonnage � 1ms
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
			
			// positionnement module en r�ception
			USART_FSK_SetReceiveAntenna(); 
			PhyUART_Mssg.Status=Listening;
			if (UART_HeaderDetected==1)
			{
				UART_HeaderDetected=0;
				UART_Receiv=0;
				PhyUART_HeaderCarCpt=0;
				PhyUART_FrameIndex=0; // pour pr�parer le sampling frame
				TimeManag_TimeOutStart(FSKStach_ChronoName ,PhyUART_TimeOut); // lancement TimeOut
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,100);
				// ---< Evolution next State >-----//
				PhyUART_FSM_State=ReadingFrame; 
			}
			else if (PhyUART_Mssg.NewStrToSend==1) 
			{
				PhyUART_Mssg.NewStrToSend=0;
				
				// r�glage Echantillonnage 100us pour acc�l�rer la FSM)
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
		
		case SendMssg:  // ! Maintien ds l'IT pdt toute l'�mission...
		{
			PhyUART_Mssg.Status=SendingMssg;
			USART_FSK_SetTransmAntenna();
			Delay_x_ms(4);
			USART_FSK_Print((char*)Preambule,4);	// Voir avec l'expérience si on peut diminuer le nbre.
			USART_FSK_Print(Phy_UART_TransmFrame,(Phy_UART_TransmFrameLen+5)); // envoie le corps
			USART_FSK_SetReceiveAntenna();  // remise du module en r�ception

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
			if (TimeManag_GetTimeOutStatus(FSKStach_ChronoName)==0) // Traitement si on n'est pas en time out !
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
																												// ou trop courte : il faut avorter et revenir � WaitForHeader
						{
							PhyUART_Mssg.Error=LenError;
							// Remise r�glage Echantillonnage � 1ms
							TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);			
							// ---< Evolution next State >-----//
							PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
						}
					}
					// Ici, la longueur a �t� jug�e bonne.
					else // tous les autres bytes de la frame sont sampl� jusqu'au dernier PhyUART_FrameIndex>0
					// c'est le reste de la cha�ne qu'on tra�te
					// Index = 1 � LEN-1
					{
					// exemple : Frame = Len et 5 octets. Soit LEN = 6
					//       LEN a b c d e 
					//  idx   0  1 2 3 4 5 
					//           1 ..... LEN-1
					//   On veut obtenir
					//  InCommingMssg ="a b c d e" soit une table [LEN-1] qui va de 0 � LEN-2
					//                  0 1 2 3 4    
					//  apr�s la derni�re passe, donc apr�s inc de Idx, Idx vaut 6 C'est la condition d'arr�t
						InComingMssg[PhyUART_FrameIndex-1]=USART_FSK_GetByte();
						PhyUART_FrameIndex++; 
						if (PhyUART_FrameIndex==Phy_UART_Len) 
						{
							PhyUART_FrameIndex=0;
							// Remise r�glage Echantillonnage � 1ms
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
				// Remise r�glage Echantillonnage � 1ms
				TimeManag_SetFSMPeriod(TIM_PhyUART_FSM,1000);
 				// ---< Evolution next State >-----//				
				PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
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
			// ajoutons l'octet LEN pour �tre complet
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
				PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
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
			
			// recopie de la cha�ne 
			// de |Dest@|Org@|ID|Type_LenData|Data|Trial|CheckSum|
			// vers |Dest@|Org@|ID|Type_LenData|Data|Trial|
			// donc on enl�ve le checksum = dernier octet de IncomminMssg
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				PhyUART_Mssg.StrReceived[i]=InComingMssg[i];
				// ---< Ajout MAC >-----//
				if (i==0)  PhyUART_Mssg.MACSrcAdress=InComingMssg[i];
				else if (i==1) // filtrage @Destination
				{
					if (((unsigned char)InComingMssg[i]==PhyUART_Mssg.My)||((unsigned char)InComingMssg[i]==0xFF)) PhyUART_Mssg.MACMatch=1;
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
			PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
			

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
	Sum=Phy_UART_TransmFrameLen; // on l'ajoute d�s le d�part avant d'ajouter les char de ArdString
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

R�le :
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
	USART_FSK_SetReceiveAntenna(); // place le module FSK en r�ception
	PhyUART_FSM_State=Init;
	PhyUART_Mssg.Status=Ready;

	
	// mise en place interruption, TIM FSM � 1ms
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

R�le :
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
// recopie le string stock� � l'interface MAC et 
// mets � 0 les autres octets jusqu'� StringLenMax.
// retourne -1 si la longueur donn�e est inf�rieure � la longueur
// effective de la ch�ine
{
	int i;
	
	// remise � 0 du flag de r�ception
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


