#ifndef _UART_STACK_H__
#define _UART_STACK_H__


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


#include "UART_F103.h"
#include "Timer_F103.h"
#include "RessourcesPeriph.h"

/* les valeurs possibles d'ereur */
typedef enum {
	_NoError,
	_CheckSumError,
	_TimeOut,
}UARTStack_ErrorType;



#define TimeOut_x100ms 5

/**
  * @brief  Initialise la stack (UART et TimeOut. Voir .h pour
	* 				identifier UART et Timeout 
	* 
**/
void UARTStack_Init(void);


/**
  * @brief  Permet de savoir si un message est re�u.
	* @ret   1 si un message est re�u, 0 sinon
**/
char UARTStack_IsHMIMssg(void);  /* ATTENTION, un nouveau mssg peut �tre faux (erreur checksum par ex )*/
															   /* il faut donc lancer un geterrorstatus pour savoir si pas d'erreur...
																	avant de lire le r�sultat ! */



/**
  * @brief  Getter error type
	* @ret   UARTStack_ErrorType voir .h pour les valeurs
**/
UARTStack_ErrorType UARTStack_GetErrorStatus(void);


/**
  * @brief  Renvoie la longueur de la cha�ne r�cup�r�e (Payload uniquement)
	* @ret   la longueur
**/
char UARTStack_GetLen(void);


/**
  * @brief  Getter mssg 
	* @ret    @ de string de r�ception (256 max).
	* @Note   la longueur est inscrite dans le premier byte
**/
char * UARTStack_GetHMIMssg(void);


/**
  * @brief  Envoie par scrutation une cha�ne de caract�re
	*         respectant la trame : |NbOctets| Data Bytes|CheckSum|
  * @param  adresse de la cha�ne � envoyer
	* @param  Nmbre de caract�re de la payload
**/
void UARTStack_SendNewMssg (char * AdrString, int Len);

#endif
