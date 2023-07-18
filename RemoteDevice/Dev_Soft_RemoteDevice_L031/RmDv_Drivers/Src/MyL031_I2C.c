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

#include "MyL031_I2C.h"

/*______________________________________________________________________________
*_______________________ Nom fonction	________________________________________
 *
 *   Rôle:
 *   Appelant :
 *   Appelés :
 *   Param :
 *
 *
 *
* __________________________________________________________________________________*/
void MyL031_I2C_Init(I2C_TypeDef * I2Cx)
{
	/* Configuration de l'I2C1*/
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; /* Activation Clk I2C1*/
	I2Cx->TIMINGR=0x00505D8D; /* Réglage Timing, 100kHz avec CkI2C = 24MHz (valeur Cube)*/
	I2Cx->CR1&=~I2C_CR1_ANFOFF; /* Validation filtre analogique */
	I2Cx->CR1&=~I2C_CR1_DNF; /* Invalidation filtre numérique */

	I2Cx->CR1 |= I2C_CR1_PE; /* Enable I2C */
}


/*______________________________________________________________________________
*_______________________ Nom fonction	________________________________________
 *
 *   Rôle:
 *   Appelant :
 *   Appelés :
 *   Param :
 *
 *
 *
* __________________________________________________________________________________*/
void MyL031_I2C_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2&=~I2C_CR2_RELOAD; /* Nécessaire si nbre de bytes < 255 */
	I2Cx->CR2&=~I2C_CR2_SADD_Msk;
	I2Cx->CR2|=DataToSend->SlaveAdress7bits; /* @ I2C ADT7410 */
	I2Cx->CR2&=~I2C_CR2_RD_WRN;    /* Write acces */
	I2Cx->CR2&=~I2C_CR2_NBYTES_Msk;
	I2Cx->CR2|=DataToSend->Nb_Data;  /*réglage du nbre de bytes à envoyer avant stop */

	/*************************************************
	**  	PHASE 1 :  I2C Adressing			   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_START; /* Start cond. après avoir testé l'acces*/

	Etc..


	/* ACCES AU BUS */
	I2Cx->CR1 &= ~I2C_CR1_ACK ; 										/* Pas d'acquittement */
	while((I2Cx->SR1&I2C_SR2_BUSY)==I2C_SR2_BUSY); 	/* Attente accès bus */
	/* DEMARRAGE COMMUNICATION */
	I2Cx->CR1 |= I2C_CR1_START; 										/* Emission Condition Start */
	while(!(I2Cx->SR1 & I2C_SR1_SB )){};						/* Attente SB */
	buffer = I2Cx->SR1; 														/* lecture bidon */
	/* EMISSION I2C Slave @ */
	I2Cx->DR=DataToSend->SlaveAdress7bits<<1; 		 	/* émission I2C @ avec insertion bit Write */
	while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {};					/* Attente ADDR OK */
	buffer = I2Cx->SR1;  														/* lecture bidon */
	buffer = I2Cx->SR2;  														/* lecture bidon */
	/* EMISSION Word @ interne au slave*/
	while (!(I2Cx->SR1 & I2C_SR1_TXE)) {}; 					/* Attendre TX vide */
	I2Cx->DR = DataToSend->WordAdress;												/* Emission Word @ */

	/*************************************************
	**  	PHASE 2 : ECRITURE DMA										**
	*************************************************/
	DMA1_Channel6->CCR &=~DMA_CCR6_EN;									/* Blocage DMA pour configuration */
  DMA1_Channel6->CMAR=(uint32_t)DataToSend->Ptr_Data;	/* Affectation adresse buffer */
	DMA1_Channel6->CNDTR=DataToSend->Nb_Data;						/* réglage nombre d'octets à lire */
	DMA1_Channel6->CCR|=DMA_CCR6_EN;										/* Démarrage DMA */
  while ((DMA1->ISR&DMA_ISR_TCIF6)==0);								/* Attente DMA full */
  DMA1->IFCR|=DMA_IFCR_CTCIF6;												/* Effacement Flag DMA full */

	/*************************************************
	**  	PHASE 3 : Fermeture communication					**
	*************************************************/
  while (!(I2Cx->SR1 & I2C_SR1_BTF)) {};						/* Attente transmission */
	I2Cx->CR1 |= I2C_CR1_STOP;												/* Génération condition stop */
}


