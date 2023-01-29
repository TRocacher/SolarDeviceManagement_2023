#include "stm32f10x.h"
#include "UART_FSK_103.h"
#include "PhyUART.h"
#include "TimeOut.h"

/******************************************************************************************************************
	VARIABLES GLOBALES
*****************************************************************************************************************/

//******************************************
// GESTION FSM PhyUART (réception)
//******************************************

// longueur max des chaïnes
#define StringLenMax 30
#define StringLenMin 8

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
	UpdateMssgForMAC
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


// les valeurs possibles de Status
typedef enum {
	Ready,
	Listening,
	ReceivingMssg,
}PhyUART_StatusType;

// les valeurs possibles de Status
typedef enum {
	NoError,
	CheckSumError,
	LenError,
	TimeOutError,
	OverRunError
}PhyUART_ErrorType;

// la structure d'échange PhyUART-MAC
struct PhyUART_Mssg_type
{
	char StrReceived[StringLenMax];
	char NewStrReceived;
	char StrToSend[StringLenMax];
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
	PhyUART_FSM_State=Init;
	// calcul time Out on prévoit la durée d'une chaîne maximale +10% 
	// calcul en ms : T = NbBit*NbMaxOctet*1.1*Tbit = NbBit*NbMaxOctet*1.1/R 
	//			            = 1000*10*NbMaxOctet*1.1/R = (1100*NbBit*NbMaxOctet1)/R
	
	PhyUART_TimeOut=(11000*StringLenMax)/PhyUART_BdRate;
	
	
}











/******************************************************************************************************************
	PhyUART_FSM_Progress

Rôle :
Param : 
*****************************************************************************************************************/

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
			break;
		}
		
		case ReadingFrame:
		{	
			// *****************************
			//  Etape Reading Frame
			// *****************************

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
			if (PhyUART_Mssg.NewStrReceived==1) PhyUART_Mssg.Error=OverRunError;
			
			// recopie de la chaîne 
			// de |Dest@|Org@|ID|Type_LenData|Data|Trial|CheckSum|
			// vers |Dest@|Org@|ID|Type_LenData|Data|Trial|
			// donc on enlève le checksum = dernier octet de IncomminMssg
			for (i=0;i<Phy_UART_Len-2;i++) // on ne compte pas le CheckSum ! 
			{
				PhyUART_Mssg.StrReceived[i]=InComingMssg[i];
			}	
			PhyUART_Mssg.NewStrReceived=1;
			PhyUART_FSM_State=WaitForHeader; // retour à l'étape d'attente	
			break;
		}

	}
}

