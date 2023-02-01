#include "stm32f10x.h"
#include "UART_FSK_103.h"
#include "PhyUART.h"
#include "TimeOut.h"


//#define MyDebug

/******************************************************************************************************************
	VARIABLES GLOBALES
*****************************************************************************************************************/

//******************************************
// GESTION FSM PhyUART (réception)
//******************************************


// Baudrate
#define PhyUART_BdRate 9600

// flag indiquant qu'un octet est arrivé sur l'UART
char UART_Receiv;

char PhyUART_HeaderCarCpt;

// user fct pour démarrer la FSM
char PhyUART_Start;

// index indiquant le numéro d'octet dans la frame 
char PhyUART_FrameIndex; // |Len|My@|Dest@|ID|Type_LenData|Data|Trial|CheckSum| 
//                      0   1    2    3    4          ...  

// longueur chaîne lue de la frame
char Phy_UART_Len;

// états possibles de la FSM
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

#define HeaderCar '#'
#define HeaderCarLenMax 5


char InComingMssg[StringLenMax];


int PhyUART_TimeOutDate;  // la date d'échéance
int PhyUART_TimeOut;  	  // la durée à partir du start











//******************************************
// VARIABLE D'INTERFACE AVEC MAC
//******************************************



// la structure d'échange PhyUART-MAC
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




/******************************************************************************************************************
	GESTION TIMEOUT

Rôle :
Param : 
*****************************************************************************************************************/

void PhyUART_TimeOut_Start(int ms)
{
	PhyUART_TimeOutDate=10*ms+SystickGet();
}

char PhyUART_GetTimeOut_Status(void)
{
	if ((SystickGet()-PhyUART_TimeOutDate)>=0) return 1;
	else return 0;
}



/******************************************************************************************************************
	UART_Callback

Rôle :
Param : 
*****************************************************************************************************************/

void UART_Callback(void)
{
	UART_Receiv=1;
}









/******************************************************************************************************************
	PhyUART_StartFSM

Rôle :
Param : 
*****************************************************************************************************************/
void PhyUART_StartFSM(void)
{
	PhyUART_Start=1;
}








/******************************************************************************************************************
	PhyUART_Init

Rôle :
Param : 
*****************************************************************************************************************/
void PhyUART_Init(void)
{
	USART_FSK_Init(PhyUART_BdRate,0,UART_Callback);
	USART_FSK_SetReceiveAntenna(); // place le module FSK en réception
	PhyUART_FSM_State=Init;
	PhyUART_Mssg.Status=Ready;
	// calcul time Out on prévoit la durée d'une chaîne maximale +10% 
	// calcul en ms : T = NbBit*NbMaxOctet*1.1*Tbit = NbBit*NbMaxOctet*1.1/R 
	//			            = 1000*10*NbMaxOctet*1.1/R = (1100*NbBit*NbMaxOctet1)/R
	
	PhyUART_TimeOut=(11000*StringLenMax)/PhyUART_BdRate;
	
	
#ifdef MyDebug	
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_IOPCEN;
	GPIOC->CRH&=~(0xF<<(10%8)*4); // output ppull 2MHz
	GPIOC->CRH|=(0x1<<(10%8)*4);
#endif
	
}







char FrameLen;
char Frame[50];

void PhyUART_Framing (void);

/******************************************************************************************************************
	PhyUART_FSM_Progress

Rôle :
Param : 
*****************************************************************************************************************/

void PhyUART_FSM_Progress(void)
{
int Sum,i;
char CRC_Val;
	
if (PhyUART_Mssg.Status!=SendingMssg) // si la couche est en train d'émettre, on bloque le FSM de réception
{


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
			if (UART_Receiv==1)
			{
				UART_Receiv=0;
				if (USART_FSK_GetByte()==HeaderCar)PhyUART_HeaderCarCpt++;
				else PhyUART_HeaderCarCpt=0;
				if (PhyUART_HeaderCarCpt==HeaderCarLenMax) 
				{
					PhyUART_FSM_State=ReadingFrame; 
					PhyUART_HeaderCarCpt=0;
					PhyUART_FrameIndex=0; // pour préparer le sampling frame
					PhyUART_TimeOut_Start(PhyUART_TimeOut); // lancement TimeOut
				}
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
		
		case SendMssg:  // ! Maintien ds l'IT pdt toute l'émission...
		{
			PhyUART_Mssg.Status=SendingMssg;
			USART_FSK_SetTransmAntenna();
			USART_FSK_Print("1234",4);      // envoie de quelques caractères car le premier byte est souvent dégradé.
																		// Voir avec l'expérience si on peut diminuer le nbre.
			USART_FSK_Print(Frame,(FrameLen+5)); // envoie le corps
			USART_FSK_SetReceiveAntenna();  // remise du module en réception
			PhyUART_FSM_State=WaitForHeader;
		}
		
		case ReadingFrame:
		{	
			// *****************************
			//  Etape Reading Frame
			// *****************************
			// positionnement module en réception
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
																												// ou trop courte : il faut avorter et revenir à WaitForHeader
						{
							PhyUART_Mssg.Error=LenError;			
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
							PhyUART_FSM_State=CheckSum;
						}
					}
				}	
			}
			else // timeout
			{
				PhyUART_Mssg.Error=TimeOutError;			
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
				PhyUART_FSM_State=UpdateMssgForMAC;
			}
			else
			{
				PhyUART_Mssg.Error=CheckSumError;			
				PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
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
			
			// recopie de la chaîne 
			// de |Dest@|Org@|ID|Type_LenData|Data|Trial|CheckSum|
			// vers |Dest@|Org@|ID|Type_LenData|Data|Trial|
			// donc on enlève le checksum = dernier octet de IncomminMssg
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				PhyUART_Mssg.StrReceived[i]=InComingMssg[i];
			}	
			PhyUART_Mssg.LenStrReceived=Phy_UART_Len-2;
			PhyUART_Mssg.NewStrReceived=1;
			PhyUART_Mssg.Status=Listening;
			PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
			
#ifdef MyDebug			
// pulse Down 
GPIOC->ODR&=~GPIO_ODR_ODR10;	
#endif
			break;
		}

	}
}

}




/******************************************************************************************************************
	IO Fcts

Rôle :
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


void PhyUART_Framing (void)
{

	int Sum,i;

	
	FrameLen=PhyUART_Mssg.LenStrToSend+2; // ajout de l'octet qui est la longueur + octet checksum
	// Encapsulation et calcul Checksum
	for (i=0;i<5;i++)
	{
		Frame[i]='#';
	}
	Frame[5]=FrameLen;
	Sum=FrameLen; // on l'ajoute dès le départ avant d'ajouter les char de ArdString
	for (i=0;i<PhyUART_Mssg.LenStrToSend;i++)
	{
		Frame[i+6]= PhyUART_Mssg.StrToSend[i];  
		Sum=Sum+Frame[i+6];
	}
	Frame[FrameLen+4]=(char)Sum; // insertion du checksum
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


