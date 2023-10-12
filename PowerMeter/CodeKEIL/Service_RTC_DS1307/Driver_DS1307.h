#ifndef _DRIVER_DS1307_H
#define _DRIVER_DS1307_H







#include "stm32f10x.h"
/* ====================================================================
	// Voir datasheet DS1307 et moodle, aide I2C "I2C mode d emploi pdf" page p6
	// @ 7bits = 0x68 1101000
	// ECRITURE : Adressage 0x68 (Write cad bit 0 = 0), puis �criture du pointeur d'adresse
	// puis �criture s�quentielle.
	
	// LECTURE : en deux temps. D'abord une �criture I2C @0x68 (Write, cad bit 0 = 0) puis �cr du pointeur
	// du composant. Puis nouveau start avec @I2C = 0x68 (read , cad bit 0 =1). Puis lecture s�quentielle.
	

==========================================================================================*/

// Adresse I2C de la RTC DS1307
#define DS1307_Slave7bitsAdr 0x68

typedef struct
{
	char Sec;
	char Min;
	char Hour;
	char Day;
	char Date;
	char Month;
	char Year;
}
DS1307_Time_Typedef;


/**
* @brief initialise l'I2C pour piloter le DS1307.

  * @param  le p�riph I2C � utiliser (100 kHz, Prio IT erreur =1, Own adress =0x55 -inutile-)
  * @retval None
*/
void DS1307_Init(I2C_TypeDef * I2Cx);



/**
* @brief  Mets � l'heure la RTC : doit envoyer 8 octets � partir de l'adresse 00h qui sont
*  1-Sec ;2-min ;3-Hours (format 12/24..) ; 4- Jour (Lundi - Dim) ; 5- date (1-31) ; 6- Mois; 
*  7-Ann�e (diz et unit); 8-ctrl ( voir doc, 0 dans notre cas)
*  La trame I2C est donc : 0x68 (Write) puis l'adresse pointeur interne puis 8 data

  * @param  usertime (voir ci-dessus)
  * @retval None
*/
void DS1307_SetTime(DS1307_Time_Typedef * UserTime);




/**
* @brief  R�cup�re les 7 valeurs du RTC
* en deux temps , d'abord �criture pteur RTC
* puis lancement lecture s�quentielle avec un restart

  * @param  usertime (voir ci-dessus)
  * @retval None
*/
void DS1307_GetTime(DS1307_Time_Typedef * UserTime);



#endif
