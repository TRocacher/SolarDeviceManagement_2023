#include "stm32f10x.h"
#include "USART_User_Conf_2021.h"


// 13/10/2021 : ajout de la fonction de lecture du flag 
// Is_StringReceived(USART_TypeDef *USART)
// pour permettre une lecture non bloquante

// 08/2018 : Documentation_USART_Driver.pdf pour une information pr�cise



//=======================================================================================
// 	Auteur T.R.
//  Juin 2017 , 28/06
//  Processeur : STM32F103RB
//  Logiciel dev : �Vision 5.23
// 
//=======================================================================================


#ifndef _USART_rev3_H__
#define _USART_rev3_H__


/* 
 ======================================================================================
|
|								GENERALITE
|   Compl�ter le fichier USART_User_Conf.h pour param�trer la COM
|   taille des buffer, caract�res sp�ciaux...
|
 ======================================================================================
 
La lib fournit les fonctionnalit�s de bas niveau de l'UART mais aussi des fonctions
de plus haut niveau qui permettent la manipulation de cha�ne de caract�re.

* les 3 USART sont programmables
* les IO sont configur�es dans la fonction d'initialisation de l'USART

=> USART_1_TX = TIM1_CH2 = PA9
=> USART_1_RX = TIM1_CH3 = PA10
=> USART_2_TX = TIM2_CH3 = PA2
=> USART_2_RX = TIM2_CH4 = PA3
=> USART_3_TX = PB10
=> USART_3_RX = PB11
 
Le d�bit de chacune des transmission est param�trable.
Par contre les autres configurations sont fig�es :
- pas de parit�
- 8 bits
- pas de contr�le de flux mat�riel
- 1 bit de stop
 
 
*/


//=======================================================================================
//                      Configuration  
//=======================================================================================

void Init_USART(USART_TypeDef *USART,int Baud_Rate_bits_par_Sec, char Prio_USART); // OK
/*___________________________________________________________________________________________

PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
Baud_Rate_bits_par_Sec: D�bit de symboles en bits / s (normalement en Bauds)
Prio_USART: Priorit� de l'interruption USART lors de la r�ception. On conseille de choisir 0 pour avoir la priorit� maximale.

DESCRIPTION�:
Initialise l'USART voulue au d�bit souhait�.

EXEMPLE�: 
Init_USART(USART2,115200,0);	
___________________________________________________________________________________________*/


void Init_IT_Serial_Receive_Byte(USART_TypeDef *USART, void (*IT_function) (void));
void Init_IT_Serial_Receive_Str(USART_TypeDef *USART, void (*IT_function) (void));
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
IT_function: Adresse (nom) de la fonction callback � lancer

DESCRIPTION�:
M�morise le nom de la fonction callback � lancer (� la r�ception d'un octet ou s'un string). Initialise 
le m�canisme d'appel � la fonction callback.

EXEMPLE�: 
Init_IT_Serial_Receive_Str(USART2, MaFonction);
___________________________________________________________________________________________*/




//=======================================================================================
//                      R�ception
//=======================================================================================


char * Get_String(USART_TypeDef *USART); 
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION�:
Attend la r�ception compl�te d'un string sur l'USART consid�r�e

EXEMPLE�: 
char* Reponse�;  						// d�claration d'un pointeur de caract�re 
Reponse=Get_String(USART2); // r�ponse contient l'adresse du buffer qui contient
														// la cha�ne de caract�res
___________________________________________________________________________________________*/


char Is_Str_Received(USART_TypeDef *USART);
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION�:
Renvoie l'�tat (0 ou 1) du flag qui signale qu'un String est arriv�.

EXEMPLE�: 
		if (Is_Str_Received(USART1)==1)
		{
			Reponse=Get_String(USART1); // jamais bloquant puisque un string est re�u !
		}
___________________________________________________________________________________________*/


void Flush(USART_TypeDef *USART);
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION�:
vide le buffer (�crit Null sur toute la longueur du buffer)

EXEMPLE�: 
Flush(USART3)�;
___________________________________________________________________________________________*/





//=======================================================================================
//                      Emission
//=======================================================================================


void Put_Char(USART_TypeDef *USART,char Car); 
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
Car�: le caract�re � transmette

DESCRIPTION�:
Emet juste un caract�re sur l'USART donn�e

EXEMPLE�: 
Put_Char(USART1,'c');
___________________________________________________________________________________________*/


int Put_String(USART_TypeDef *USART,char * pCar);
/*___________________________________________________________________________________________
PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
* pCar�: adresse du premier caract�re de la cha�ne � transmettre

DESCRIPTION�:
La fonction envoie toute une cha�ne de caract�res dont l'adresse de d�but est donn�e.
S'arr�te quand le caract�re Null est trouv�, ou si la longueur maximale est atteinte.
(cf "USART_User_Conf_2018.h")

EXEMPLE�: 
char* Reponse�; 
sprintf(Reponse,� ��coucou��);
Put_String(USART2,Reponse);
//ou directement�:
Put_String(USART2,� ��coucou�� );
___________________________________________________________________________________________*/


int Put_String_FixedLen(USART_TypeDef *USART,char * pCar,int Len);

/*___________________________________________________________________________________________

PARAM:
USART�: adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
* pCar�: adresse du premier caract�re de la cha�ne � transmettre
Len�: longueur de la cha�ne � passer

DESCRIPTION�:
Cette fonction permet d'envoyer une suite d'octets quelconque, y compris le caract�re Null.
Elle ne peut donc pas fonctionner sur le principe de la d�tection de Null.
C'est pourquoi la longueur est pass�e en param�tre. La fonction va envoyer un nombre de
caract�re �gal � Len, � partir de l'adresse donn�e dans pcar.

EXEMPLE�: 
char* Reponse�; 
Put_String(USART2,Reponse,15); // �met 15 caract�res � partir de l'adresse contenue dans Reponse
__________________________________________________________________________________________*/








//=======================================================================================
//                      Getter et Setters
//=======================================================================================

int Read_Received_StrLen(USART_TypeDef *USART);
char Read_Received_Current_Byte(USART_TypeDef *USART);
char *  Read_Received_Current_String(USART_TypeDef *USART);
void Write_EndCar1(USART_TypeDef *USART, char EndCar); // permet de modifier par pgm les endCar
void Write_EndCar2(USART_TypeDef *USART, char EndCar);
int Read_Transmitted_StrLen(USART_TypeDef *USART);
/*___________________________________________________________________________________________
Fonction de lecture / �criture  � la vol�e des donn�es structur�e
@ USART_TypeDef *USART = USART, USART2 ou USART3
Les fonctions qui suivent permettent des acc�s en lecture et �criture de certains champs 
des variables structur�es d�finies dans le .c. 
Ces fonctions sont � utiliser avec pr�caution.
___________________________________________________________________________________________*/









#endif
