/* =================================================================================
* ==================   UARTStack.c	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : TimeStampManagement (TimeOut)
 *  ------------------------------------------------------------------------------
 * Pile de com�munication UART
 * Principe :
 * Format de trame UART : 
 
     |NbOctets| Data Bytes|CheckSum|
		  NbOctets = Data byte nb + 1 (checksum). Le byte NbOctets est compt�
			dans le checksum, 
			
			La cha�ne r�cup�r� ressemble � ceci :
			|5|'T'|'O'|'T'|'O'| checkS|
        0  1  2   3   4    5
				
 * L'UART fonctionne en IT sur r�ception byte.
   � la premi�re r�ception, on capte la longueur
	 au bytes suivant la cha�ne se construit
	 
	 Syst�matiquement un timer est lanc� au d�but du processus.
	 Arriv� � �ch�ance, si la cha�ne a �t� acquise, pas d'erreur
	 Sinon, le statut d'erreur passe � Timeout.
	 
	 NB : si le premier octet de longueur est n et si le nbre d'octets
	 envoy�s est sup�rieur � n, aucune erreur se produira. La r�ception sera
	 un succ�s
	 
	 
	 des getter permettent d'acc�der au erreurs et au tableau de 256 bytes
	 
********* GESTION DE LA RECEPTION ********
	 Se fait par la fonction char UARTStack_IsHMIMssg(void);
	 La fonction renvoie le flag HMIStringComplete
	 
	 HMIStringComplete est mis � 1 :
	 - lorsque le nombre de caract�res est atteint.
			NB : l'UART est d�s lors bloqu�e.

	 HMIStringComplete est mis � 0 :
	 - Lors d'nue lecture du string de r�ception
	 - au d�but du processus de r�ception (lecture du nb d'octet � saisir)
	 
	 L'UART est valid�e :
	 - � l'init
	 - apr�s un timeout (d�fini en multiple de 100ms)
	 
	 L'UART est invalid�e :
	 - Lors de la r�ception de n octets.
 

********* GESTION DEs ERREURS ********
_NoError, : a l'init et uniquement � r�ception de la cha�ne, si pas d'erreur Checksum
_CheckSumError, : � la r�ception compl�te de la cha�ne, si erreur Checksum
_TimeOut,  : si le timer est arriv� � �ch�ance et sur la cha�ne est incompl�te




 *
* =================================================================================*/


#include "UARTStack.h"

char HMISting[25]; ////////////passer � 256 ?
int char_pos;
int charNbToread;
char UARTStack_Error;
char HMIStringComplete;
char HMIStringComplete_NoTimOut;

UARTStack_ErrorType UARTStackErrorStatus;
int UARTStack_TimeOutCpt;


char Checksum(char * Ptr);

void UARTStack_Restart(void)
{
		char_pos=0;
		charNbToread=0;
		TimerOff(TIM_UARTStack);
	  USART_ReceivEnable(UART_HMI); // revalidation r�ception
}


void UARTStack_TimeOut(void) /* Syst�matique m�me si bonne r�ception*/
{
	UARTStack_TimeOutCpt++;
	if (UARTStack_TimeOutCpt==TimeOut_x100ms)
	{
		UARTStack_TimeOutCpt=0;
		if (HMIStringComplete_NoTimOut==0)
		{
			UARTStackErrorStatus=_TimeOut;
		}
		/* Reinit UART */
		UARTStack_Restart();
	}
}

void HMI_UART_Callback(void)
{
	char Character;
	Character=USART_GetByte(UART_HMI); /* Lecture syst�matique pour �viter boucle IT*/
	
	if (char_pos==0) // Premi�re lecture
	{
		HMIStringComplete=0;
		HMIStringComplete_NoTimOut=0;
		charNbToread=Character;
	  HMISting[char_pos]=charNbToread;
		UARTStack_TimeOutCpt=0; /* r�armement timeout */
		TimerOn(TIM_UARTStack);
		char_pos++;
	}
	else if (char_pos<=charNbToread)
	{
		 HMISting[char_pos]=Character; 
		 if (char_pos==charNbToread) /*  NB octets lu completed*/
		 {
			 if (Checksum(HMISting)==1) UARTStackErrorStatus=_NoError;
			 else UARTStackErrorStatus=_CheckSumError;
			 
			 HMIStringComplete=1;					// flag user
			 HMIStringComplete_NoTimOut=1; // sp�cifique gestion timeout
			 USART_ReceivDisable(UART_HMI); // blocage UART
		 }
		 char_pos++;
	}
  else
	{
		while(1); /* PLANTAGE */
	}
		

	
	
}


/**
  * @brief  Initialise la stack (UART et TimeOut. Voir .h pour
	* 				identifier UART et Timeout 
	* 
**/
void UARTStack_Init(void)
{
	char_pos=0;
	HMIStringComplete=0;
	UARTStackErrorStatus=_NoError;
	charNbToread=0;
	USART_Init(UART_HMI, 9600, 0, HMI_UART_Callback); 
	USART_ReceivEnable(UART_HMI);
	
	Timer_CkEnable(TIM_UARTStack);
	Timer_Set_Period(TIM_UARTStack, 10000-1, 720-1 ); // p�riode 100 msec
	Timer_IT_Enable( TIM_UARTStack,1, UARTStack_TimeOut);
	TimerOff(TIM_UARTStack);	// Arr�t pour l'instant
	
	
}


/**
  * @brief  Permet de savoir si un message est re�u.
	* @ret   1 si un message est re�u, 0 sinon
**/
char UARTStack_IsHMIMssg(void)
{
	return HMIStringComplete;
}

/**
  * @brief  Getter error type
	* @ret   UARTStack_ErrorType voir .h pour les valeurs
**/
UARTStack_ErrorType UARTStack_GetErrorStatus(void)
{
	return UARTStackErrorStatus;
}

/**
  * @brief  Getter mssg 
	* @ret    @ de string de r�ception (256 max).
	* @Note   la longueur est inscrite dans le premier byte
**/
char * UARTStack_GetHMIMssg(void)
{
	HMIStringComplete=0; 
	return  HMISting;
}



/**
  * @brief  PRIVATE Calcul du checksum (Nb octets + payload)
	*  Principe : on somme tous les bytes (sauf checksum evidemment)
	*  cad nbre bytes + payload. On ne garde que l'octet de pds faible.
	* @ret   1 si OK 0 sinon
**/

char Checksum(char * Ptr)
{
	char UART_Stack_CRC_Val;
	char i;
	char UART_Stack_CRCResult;
	int UART_Stack_Sum;
	
	UART_Stack_CRCResult=0;
	// reconstruction de la valeur du checksum = dernier octet lu
	UART_Stack_CRC_Val=*(Ptr +charNbToread);
	// |5|'T'|'O'|'T'|'O'| checkS|
  //   0  1  2   3   4    5
	// cha�ne la plus courte
	// |1|checkS|
   //  0  1  
	UART_Stack_Sum=0;
	for (i=0;i<charNbToread;i++) // on ne compte pas le CheckSum ! 
	{
		UART_Stack_Sum=UART_Stack_Sum+*(Ptr +i);
	}
		
	if (UART_Stack_CRC_Val==(char)UART_Stack_Sum) // Checksum OK
	{
		UART_Stack_CRCResult=1;
	}
	else
	{
		UART_Stack_CRCResult=0;	
	}
	return 	UART_Stack_CRCResult;	
}
