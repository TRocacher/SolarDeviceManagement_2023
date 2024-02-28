#ifndef _UART_STACK_H__
#define _UART_STACK_H__


/* =================================================================================
* ==================   UARTStack.c	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : TimeStampManagement (TimeOut)
 *  ------------------------------------------------------------------------------
 * Pile de comùmunication UART
 * Principe :
 * Format de trame UART : 
 
     |NbOctets| Data Bytes|CheckSum|
		  NbOctets = Data byte nb + 1 (checksum). Le byte NbOctets est compté
			dans le checksum, 
			
			La chaîne récupéré ressemble à ceci :
			|5|'T'|'O'|'T'|'O'| checkS|
        0  1  2   3   4    5
				
 * L'UART fonctionne en IT sur réception byte.
   à la première réception, on capte la longueur
	 au bytes suivant la chaîne se construit
	 
	 Systématiquement un timer est lancé au début du processus.
	 Arrivé à échéance, si la chaîne a été acquise, pas d'erreur
	 Sinon, le statut d'erreur passe à Timeout.
	 
	 NB : si le premier octet de longueur est n et si le nbre d'octets
	 envoyés est supérieur à n, aucune erreur se produira. La réception sera
	 un succès
	 
	 
	 des getter permettent d'accéder au erreurs et au tableau de 256 bytes
	 
********* GESTION DE LA RECEPTION ********
	 Se fait par la fonction char UARTStack_IsHMIMssg(void);
	 La fonction renvoie le flag HMIStringComplete
	 
	 HMIStringComplete est mis à 1 :
	 - lorsque le nombre de caractères est atteint.
			NB : l'UART est dès lors bloquée.

	 HMIStringComplete est mis à 0 :
	 - Lors d'nue lecture du string de réception
	 - au début du processus de réception (lecture du nb d'octet à saisir)
	 
	 L'UART est validée :
	 - à l'init
	 - après un timeout (défini en multiple de 100ms)
	 
	 L'UART est invalidée :
	 - Lors de la réception de n octets.
 

********* GESTION DEs ERREURS ********
_NoError, : a l'init et uniquement à réception de la chaîne, si pas d'erreur Checksum
_CheckSumError, : à la réception complète de la chaïne, si erreur Checksum
_TimeOut,  : si le timer est arrivé à échéance et sur la chaîne est incomplète




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
  * @brief  Permet de savoir si un message est reçu.
	* @ret   1 si un message est reçu, 0 sinon
**/
char UARTStack_IsHMIMssg(void);  /* ATTENTION, un nouveau mssg peut être faux (erreur checksum par ex )*/
															   /* il faut donc lancer un geterrorstatus pour savoir si pas d'erreur...
																	avant de lire le résultat ! */



/**
  * @brief  Getter error type
	* @ret   UARTStack_ErrorType voir .h pour les valeurs
**/
UARTStack_ErrorType UARTStack_GetErrorStatus(void);


/**
  * @brief  Renvoie la longueur de la chaîne récupérée (Payload uniquement)
	* @ret   la longueur
**/
char UARTStack_GetLen(void);


/**
  * @brief  Getter mssg 
	* @ret    @ de string de réception (256 max).
	* @Note   la longueur est inscrite dans le premier byte
**/
char * UARTStack_GetHMIMssg(void);


/**
  * @brief  Envoie par scrutation une chaîne de caractère
	*         respectant la trame : |NbOctets| Data Bytes|CheckSum|
  * @param  adresse de la chaîne à envoyer
	* @param  Nmbre de caractère de la payload
**/
void UARTStack_SendNewMssg (char * AdrString, int Len);

#endif
