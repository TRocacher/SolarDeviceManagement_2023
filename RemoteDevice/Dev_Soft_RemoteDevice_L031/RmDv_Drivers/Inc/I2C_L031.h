/* =================================================================================
* ==================   MyL031_I2C.c     	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *  Directement aux registres
 *

* =================================================================================*/

#ifndef INC_I2C_L031_H_
#define INC_I2C_L031_H_

#include "RmDv_IO.h"

// Structure de données pour l'émission ou la réception I2C :
typedef struct
{
	char SlaveAdress7bits;  /* Adresse du composant I2C */
	uint8_t * Ptr_Data;		/* Adresse du tableau de données à transmettre ou  recevoir
	 	 	 	 	 	 	 NB : le premier élément est classiquementt le Word Adress
	 	 	 	 	 	 	 (adresse d'écriture /lecture interne du slave).
	 	 	 	 	 	 	 Le reste sont les données brutes*/
	uint8_t Nb_Data;		/* Nbre de données à transmettre-recevoir tout compris*/

	/*Exemple
	 * Data[4] = 0xAA, 0x1A, 0x2A, 0x3B  : 0xAA est le word Adress, le reste les données
	 * Nb_Data doit être égal à 4*/
}
I2C_RecSendData_Typedef;


void I2C_L031_Init(I2C_TypeDef * I2Cx);
void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend);
void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive);

#endif /* INC_I2C_L031_H_ */
