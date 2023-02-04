#include "stm32f10x.h"
#include "UART_FSK_103.h"
#include "PhyUART.h"
#include "MyTimer.h"
#include "FctDiverses.h"

//#define MyDebug





/*---------------------------------
// VARIABLE D'INTERFACE AVEC MAC
----------------------------------*/


// la structure d'�change PhyUART-MAC
struct PhyUART_Mssg_type
{
	char StrReceived[StringLenMax];
	char LenStrReceived;
	char NewStrReceived;
	char StrToSend[StringLenMax];
	char LenStrToSend;
	char NewStrToSend;
	PhyUART_StatusType Status;
	PhyUART_ErrorType Error;
}PhyUART_Mssg;




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


/*---------------------------------
Geston du time out
----------------------------------*/
int PhyUART_TimeOutDate;  // la date d'�ch�ance
int PhyUART_TimeOut;  	  // la dur�e � partir du start

void PhyUART_TimeOut_Start(int ms)
{
	PhyUART_TimeOutDate=10*ms+SystickGet();
}

char PhyUART_GetTimeOut_Status(void)
{
	if ((SystickGet()-PhyUART_TimeOutDate)>=0) return 1;
	else return 0;
}




//**************************************************************************************************************
//**************************************************************************************************************
// 							USART Callback
//**************************************************************************************************************
//**************************************************************************************************************

void UART_Callback(void)
{
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
		UART_HeaderDetected=1;
	}
	
}






//**************************************************************************************************************
//**************************************************************************************************************
// 							FSM PhyUART (r�ception/�mission)
//**************************************************************************************************************
//**************************************************************************************************************

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

PhyUART_FSM_StateType PhyUART_FSM_State;


/*---------------------------------
 Variables et Fct de d�marrage FSM
----------------------------------*/
// user fct pour d�marrer la FSM
char PhyUART_Start;
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
void PhyUART_FSM_Progress(void)
{
int Sum,i;
char CRC_Val;
	

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
			if (PhyUART_Start==1) PhyUART_FSM_State=WaitForHeader;
			break;
		}
		
				
		case WaitForHeader:
		{	
			// *****************************
			//  Etape Wait for Header 
			// *****************************
			
			PhyUART_Mssg.Status=Listening;
			if (UART_HeaderDetected==1)
			{
				UART_HeaderDetected=0;
				UART_Receiv=0;
				PhyUART_FSM_State=ReadingFrame; 
				PhyUART_HeaderCarCpt=0;
				PhyUART_FrameIndex=0; // pour pr�parer le sampling frame
				PhyUART_TimeOut_Start(PhyUART_TimeOut); // lancement TimeOut
				
			}
			else if (PhyUART_Mssg.NewStrToSend==1) 
			{
				PhyUART_Mssg.NewStrToSend=0;
				PhyUART_FSM_State=Framing;					
			}
			break;
		}
		
		case Framing:
		{
			PhyUART_Mssg.Status=SendingMssg;
			PhyUART_Framing();
			PhyUART_FSM_State=SendMssg;
		}
		
		case SendMssg:  // ! Maintien ds l'IT pdt toute l'�mission...
		{
			PhyUART_Mssg.Status=SendingMssg;
			USART_FSK_SetTransmAntenna();
			Delay_x_ms(4);
			USART_FSK_Print("1234",4);      // envoie de quelques caract�res car le premier byte est souvent d�grad�.
																		// Voir avec l'exp�rience si on peut diminuer le nbre.
			USART_FSK_Print(Phy_UART_TransmFrame,(Phy_UART_TransmFrameLen+5)); // envoie le corps
			USART_FSK_SetReceiveAntenna();  // remise du module en r�ception
			PhyUART_FSM_State=WaitForHeader;
		}
		
		case ReadingFrame:
		{	
			// *****************************
			//  Etape Reading Frame
			// *****************************
			// positionnement module en r�ception
			USART_FSK_SetReceiveAntenna(); 
			PhyUART_Mssg.Error=NoError;
			if (PhyUART_GetTimeOut_Status()==0) // Traitement si on n'est pas en time out !
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
							PhyUART_FSM_State=CheckSum;
						}
					}
				}	
			}
			else // timeout
			{
				PhyUART_Mssg.Error=TimeOutError;			
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
				PhyUART_FSM_State=UpdateMssgForMAC;
			}
			else
			{
				PhyUART_Mssg.Error=CheckSumError;			
				PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
			}
			break;
		}
		case UpdateMssgForMAC:
		{	
#ifdef MyDebug	
// pulse up 
GPIOC->ODR|=GPIO_ODR_ODR10;
#endif			
			PhyUART_Mssg.Status=ReceivingMssg;
			if (PhyUART_Mssg.NewStrReceived==1) PhyUART_Mssg.Error=OverRunError;
			
			// recopie de la cha�ne 
			// de |Dest@|Org@|ID|Type_LenData|Data|Trial|CheckSum|
			// vers |Dest@|Org@|ID|Type_LenData|Data|Trial|
			// donc on enl�ve le checksum = dernier octet de IncomminMssg
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				PhyUART_Mssg.StrReceived[i]=InComingMssg[i];
			}	
			PhyUART_Mssg.LenStrReceived=Phy_UART_Len-2;
			PhyUART_Mssg.NewStrReceived=1;
			PhyUART_Mssg.Status=Listening;
			PhyUART_FSM_State=WaitForHeader; // retour � l'�tape d'attente	
			
#ifdef MyDebug			
// pulse Down 
GPIOC->ODR&=~GPIO_ODR_ODR10;	
#endif
			break;
		}

	}
}




/******************************************************************************************************************
	PhyUART_Init

R�le :
Param : 
*****************************************************************************************************************/
void PhyUART_Init(void)
{
	USART_FSK_Init(PhyUART_BdRate,0,UART_Callback);
	USART_FSK_SetReceiveAntenna(); // place le module FSK en r�ception
	PhyUART_FSM_State=Init;
	PhyUART_Mssg.Status=Ready;
	// calcul time Out on pr�voit la dur�e d'une cha�ne maximale +10% 
	// calcul en ms : T = NbBit*NbMaxOctet*1.1*Tbit = NbBit*NbMaxOctet*1.1/R 
	//			            = 1000*10*NbMaxOctet*1.1/R = (1100*NbBit*NbMaxOctet1)/R
	
	PhyUART_TimeOut=(11000*StringLenMax)/PhyUART_BdRate;
	
	// mise en place interruption
	MyTimer_CkEnable(TIM2);
	MyTimer_Set_Period(TIM2, 100*72-1, 1-1 ); // p�riode par d�faut 100�s
	MyTimer_IT_Enable( TIM2, 3, PhyUART_FSM_Progress);
	
	
#ifdef MyDebug	
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPCEN;
	GPIOC->CRH&=~(0xF<<(10%8)*4); // output ppull 2MHz
	GPIOC->CRH|=(0x1<<(10%8)*4);
#endif
	
}



/******************************************************************************************************************
	IO Fcts

R�le :
Param : 
*****************************************************************************************************************/
char PhyUART_IsNewMssg(void)
{
	return (PhyUART_Mssg.NewStrReceived);
}

PhyUART_StatusType PhyUART_Get_Status(void)
{
	return (PhyUART_Mssg.Status);
}

PhyUART_ErrorType PhyUART_Get_Error(void)
{
	return (PhyUART_Mssg.Error);
}

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
			PhyUART_Mssg.LenStrToSend=Len;
			AdrString++;
		}
		PhyUART_Mssg.NewStrToSend=1;
		return 0;
	}
}


