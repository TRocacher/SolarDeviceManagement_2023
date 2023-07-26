/* =================================================================================
* ==================   MyL031_I2C.h     	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *   Dépendances : main.h (reg def)
 *  ------------------------------------------------------------------------------
 *  Directement aux registres
 *
 * void I2C_L031_Init(I2C_TypeDef * I2Cx);
 * void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend);
 * void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive);
 *
 * Voir commentaires fonctions pour plus de détail.
 * La lib fonctionne en scrutation ou en DMA avec scrutation fin DMA à la fin
 *
 * Utiliser #define DMA ou pas selon votre choix

* =================================================================================*/

#ifndef INC_I2C_L031_H_
#define INC_I2C_L031_H_

#include "main.h"

/* Structure de données pour les fonctions */
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


/*______________________________________________________________________________
*_______________________ void I2C_L031_Init_____________________________________
 *
 *   Rôle: Initialise le module I2C
 *   Param in : I2C_TypeDef * I2Cx
 *   Exemple : I2C_L031_Init(I2C1);
 *_______________________________________________________________________________*/
void I2C_L031_Init(I2C_TypeDef * I2Cx);

/*______________________________________________________________________________
*_______________________ I2C_L031_PutString_____________________________________
 *
 *   Rôle: Envoie une série d'octets vers un composant I2C slave d'@ donnée,
 *   et à partir d'une adresse interne (Word Adress) au composant.
 *   L'adresse d'un tableau contenant la châine est donnée. Ce tableau contient au
 *   minimum deux octets : la Word Adress et la donnée à y mettre (au moins une).
 *
 *   Param in : I2C_TypeDef * I2Cx
 *   			I2C_RecSendData_Typedef * DataToSend (voir définition .h)
 *   				* Adresse du composant I2C
 *   				* Ptr sur la table de données (contenant la word Adress au début)
 *   				* Nbre de donnée de la table (incluant la word adress), min 2.

 *   Exemple : 	I2C_Data_Struct.Nb_Data=2;
				I2C_Data_Struct.Ptr_Data=data;
				I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;

				I2C_L031_PutString(I2C1,&I2C_Data_Struct);
* __________________________________________________________________________________*/
void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend);

/*______________________________________________________________________________
*_______________________ I2C_L031_GetString_____________________________________
 *
 *   Rôle: Lit une série d'octets à partir d'un composant I2C slave d'@ donnée,
 *   et à partir d'une adresse interne (Word Adress) au composant.
 *   L'adresse d'un tableau contenant la chaîne est donnée. Ce tableau contient au
 *   minimum deux octets : la Word Adress et la donnée à lire (au moins une).
 *   Le procédé se déroule en deux étapes :
 *   - une écriture de la WordAdress
 *   - une lecture ensuivant via un restart
 *
 *   Param in : I2C_TypeDef * I2Cx
 *   			I2C_RecSendData_Typedef * DataToReceive (voir définition .h)
 *   				* Adresse du composant I2C
 *   				* Ptr sur la table de données (contenant la word Adress au début)
 *   				* Nbre de donnée de la table (incluant la word adress), min 2.

 *   Exemple : 		data[0]=TempHighAdr;
					I2C_Data_Struct.Nb_Data=3;
					I2C_Data_Struct.Ptr_Data=data;
					I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
					I2C_L031_GetString(I2C1, &I2C_Data_Struct);
* __________________________________________________________________________________*/
void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive);

#endif /* INC_I2C_L031_H_ */
