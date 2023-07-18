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

#ifndef INC_MYL031_I2C_H_
#define INC_MYL031_I2C_H_

#include "RmDv_IO.h"

// Structure de données pour l'émission ou la réception I2C :
typedef struct
{
	char SlaveAdress7bits;  /* Adresse du composant I2C */
	char WordAdress;  			/* Adresse interne du composant où lire-écrire */
	char * Ptr_Data;				/* Adresse du tableau de données à transmettre ou  recevoir */
	char Nb_Data;						/* Nbre de données à transmettre-recevoir */
}
I2C_RecSendData_Typedef;


void MyL031_I2C_Init(I2C_TypeDef * I2Cx);
void MyL031_I2C_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend);

#endif /* INC_MYL031_I2C_H_ */
