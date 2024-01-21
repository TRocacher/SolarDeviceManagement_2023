/*!
 * \file
 * \brief 	Bibliotheque de fonctions pour initialiser, envoyer et recevoir de donn�es vers/de un module GSM.
 * \author 	G.A. / T.R.
 * \date   	octobre 2017, revu 2018
 * \version 1
 */

//=======================================================================================
//  D�pendance logicielle : 
//	- USART_rev2018.h (USART_rev2018.c) 
//	- FctDiverses.h (FctDiverses.c) pour les d�lais soft
//	Ressources mat�rielle utilis�es :
//	- USART1
//=======================================================================================




#ifndef  __API_GSM_2018_H__
#define __API_GSM_2018_H__



/* =============================================
				Fonctions de configuration Xbee
 ==============================================*/
 
 /**
 * R�gle l'USART 1 � 115200 bds
 * Donne la priorit� 0 (la plus grande) � l'USART
 * Valide la carte SIM. N�cessite 6 secondes environ. Si une erreur s'est produite,
 * le programme plante dans une boucle sans fin. Si cela se produit, v�rifier bien
 * le code PIN (3 �ssais max !).
 * L'ex�cution prends en tout 12 secondes (le temps que le MODEM r�ponde au cmde AT
 *
 * R�glage du MODEM GSM dans le d�tail :
 * Valide la carte SIM (AT+CPIN) 																: 6 sec
 * SMS en mode texte	(AT+CMGF=1)																: 2 sec
 * SMS transmis/re�u en m�moire Modem (AT+CPMS=ME,ME,ME)				: 2 sec
 * R�ception SMS directe sans notification (AT+CNMI=2,2,0,0,0)	: 2 sec
 * @param pin : code PIN de la carte SIM
 * 
 * ex : GSM_Init("1234");
 */ 
void GSM_Init(char *pin);



/* =============================================
				Fonctions d'�mission / r�ception SMS
 ==============================================*/
 
/**
 * Envoi une chaine de caractere a un module GSM qui le
 * transmet sous forme de SMS
 * @param *Dest : Pointeur sur une chaine de caractere correspondant au num de tel � joindre
 * @param *Msg  : Pointeur sur une chaine de caractere qui constitue le SMS
 * Exemple : GSM_Send_SMS("06xxxxxxxx","Bonjour ");
 */ 
void GSM_Send_SMS(char *Dest,char *Msg);


/**
 * Attend la r�ception d'un SMS, fonction bloquante
 * @return Pointeur contenant l'adresse du premier caract�re de la ch�ine re�ue 
 * Exemple : 
 *			char * Reponse;
 * 			Reponse=GSM_Get_SMS();
 */

char * GSM_Get_SMS(void);

#endif
