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
		  NbOctets = Data byte nb + 1 (checksum). Le byte NbOctets n'est pas compt�
			
			La cha�ne r�cup�r� ressemble � ceci :
			|5|'T'|'O'|'T'|'O'| checkS|
        0  1  2   3   4    5
				
 * L'UART fonctionne en IT sur r�ception byte.
   � la premi�re r�ception, on capte la longueur
	 au bytes suivant la cha�ne se construit
	 si elle est incompl�te alors un timout stoppe le processus
	 
	 des getter permettent d'acc�der au erreurs et au tableau de 256 bytes
 *
* =================================================================================*/


#include "UARTStack.h"

char HMISting[256];
int char_pos;
int charNbToread;
char UARTStack_Error;
char HMIStringComplete;
char HMIStringComplete_NoTimOut;

UARTStack_ErrorType UARTStackErrorStatus;
int UARTStack_TimeOutCpt;



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
	if (UARTStack_TimeOutCpt==5)
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
	char poubelle;
	if (char_pos==0) // Premi�re lecture
	{
		HMIStringComplete=0;
		HMIStringComplete_NoTimOut=0;
		charNbToread=USART_GetByte(UART_HMI);
	  HMISting[char_pos]=charNbToread;
		UARTStack_TimeOutCpt=0; /* r�armement timeout � 500ms*/
		TimerOn(TIM_UARTStack);
		
	}
	else 
	{
		 HMISting[char_pos]=USART_GetByte(UART_HMI);
		 if (char_pos==charNbToread)
		 {
			 HMIStringComplete=1;					// flag user
			 HMIStringComplete_NoTimOut=1; // sp�cifique gestion timeout
			 UARTStackErrorStatus=_NoError;
			 USART_ReceivDisable(UART_HMI); // blocage UART
		 }
	}
	char_pos++;
	
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

