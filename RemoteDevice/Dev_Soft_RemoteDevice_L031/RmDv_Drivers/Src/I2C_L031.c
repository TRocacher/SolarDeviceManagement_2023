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

#include <I2C_L031.h>

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
void I2C_L031_Init(I2C_TypeDef * I2Cx)
{
	/*************************************************
	**  	cong I2C							   	**
	*************************************************/
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; /* Activation Clk I2C1*/
	I2Cx->TIMINGR=0x00505D8D; /* Réglage Timing, 100kHz avec CkI2C = 24MHz (valeur Cube)*/
	I2Cx->CR1=0; /* Clear CR1 par défaut*/
	/*I2C_CR1_ANFOFF=0 Validation filtre analogique */
	/*I2C_CR1_DNF=0000b Invalidation filtre numérique */
	I2Cx->CR1 =I2C_CR1_RXDMAEN|I2C_CR1_TXDMAEN|I2C_CR1_PE; /*Rx/Tx DMA enable, Enable I2C */

	/*************************************************
	**  	Préparation DMA :DMA1 channel 6         **
	**  	en émission, channel 7 en réception 	**
	*************************************************/
	/* Préparation DMA :DMA1 channel 6 en émission, channel 7 en réception */
	RCC->AHBENR|= RCC_AHBENR_DMA1EN; /* Activation Clk DMA1*/
	DMA1_Channel6->CCR=0; /* init à 0 par défaut */
	//DMA1_Channel6->CCR &=~DMA_CCR_PSIZE;
	//DMA1_Channel6->CCR &=~DMA_CCR_MSIZE;
	//DMA1_Channel6->CCR&=~DMA_CCR_PINC;
	//DMA1_Channel6->CCR&=~DMA_CCR_CIRC;/* Réglage du mode circulaire : non circulaire */
	DMA1_Channel7->CCR=0; /* init à 0 par défaut */
	//DMA1_Channel7->CCR &=~DMA_CCR_PSIZE;
	//DMA1_Channel7->CCR &=~DMA_CCR_MSIZE;
	//DMA1_Channel7->CCR&=~DMA_CCR_PINC;
	//DMA1_Channel7->CCR&=~DMA_CCR_CIRC;
	//DMA1_Channel7->CCR&=~DMA_CCR_DIR;		/* 	 I2C vers Buffer (lecture) */
	DMA1_Channel6->CPAR=(uint32_t)&I2Cx->TXDR; /* Affectation @ périphérique : celle de TxDR */
	DMA1_Channel7->CPAR=(uint32_t)&I2Cx->RXDR; /* Affectation @ périphérique : celle de RxDR */
	DMA1_Channel6->CCR|=DMA_CCR_MINC;			/* Réglage de l'incrémentation côté buffer RAM : +1 */
	DMA1_Channel7->CCR|=DMA_CCR_MINC;
	DMA1_Channel6->CCR|=DMA_CCR_DIR;		/* Réglage du sens DMA 	: Buffer vers I2C (écriture) */
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
void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2Cx->CR2=0; /* Clear CR2 par défaut*/
	/*I2C_CR2_RELOAD=0  Nécessaire si nbre de bytes < 255 */
	/*I2C_CR2_RD_WRN=0 Write acces */
	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2|=DataToSend->SlaveAdress7bits<<1; /* @ I2C ADT7410 ds le champ SADD de CR2
	 	 	 	 	 	 	 	 	 	 	  avec un déclage de 1 (SADD[0] = don't care*/
	I2Cx->CR2|=(DataToSend->Nb_Data)<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop */



	/*************************************************
	**  	PHASE 1 :  I2C Adressing			   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_START; /* Start cond. après avoir testé l'acces
	 	 	 	 	 	 	 	 + adressing*/


	/*************************************************
	**  	PHASE 2 : ECRITURE DMA 					**
	*************************************************/

	DMA1_Channel6->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel6->CMAR=(uint32_t)DataToSend->Ptr_Data;	/* Affectation adresse buffer */
	DMA1_Channel6->CNDTR=DataToSend->Nb_Data;			/* réglage nombre d'octets à lire */
	DMA1_Channel6->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
	while ((DMA1->ISR&DMA_ISR_TCIF6)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF6;						/* Effacement Flag DMA full */



	/*************************************************
	**  	PHASE 3 : Fermeture communication		**
	*************************************************/
	/* Génération condition stop automatique AUTOEND = 1  */
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
void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2Cx->CR2=0; /* Clear CR2 par défaut*/
	/*I2C_CR2_RELOAD=0  Nécessaire si nbre de bytes < 255 */
	/*I2C_CR2_RD_WRN=0 Write acces */
	/*I2C_CR2_AUTOEND=0;  pas de stop automatique pour pouvoir faire un restart */
	I2Cx->CR2|=DataToReceive->SlaveAdress7bits<<1; /* @ I2C ADT7410 ds le champ SADD de CR2
		 	 	 	 	 	 	 	 	 	 	  avec un déclage de 1 (SADD[0] = don't care*/
	I2Cx->CR2|=(1<<I2C_CR2_NBYTES_Pos); 		 /*un seul byte à transmettre, le Word Adress */

	/*************************************************
	**  	PHASE 1 :  I2C Adressing			   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_START; /* Start cond. après avoir testé l'acces
	 	 	 	 	 	 	 	 + adressing*/
	/***************************************************************************
	**  	PHASE 2 : ECRITURE word adress + restart en mode read de Nb_Data  **
	****************************************************************************/
	while ((I2Cx->ISR&I2C_ISR_TXIS)==0); /* attendre la levée de flag pour émettre*/
	I2Cx->TXDR=*(DataToReceive->Ptr_Data);
	while ((I2Cx->ISR&I2C_ISR_TC)==0); /* attendre la fin d'émission, Nbyte, (=1 ici)*/

	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2|=(DataToReceive->Nb_Data)<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop */
	I2Cx->CR2|=I2C_CR2_RD_WRN; /*read acces */

	I2Cx->CR2|=I2C_CR2_START; /* ReStart cond. après avoir testé l'acces*/

	/****************************************
	**  	PHASE 3 : LECTURE  de Nb_Data  **
	*****************************************/
	DMA1_Channel7->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel7->CMAR=(uint32_t)((DataToReceive->Ptr_Data)+1);	/* Affectation adresse buffer
	 	 	 	 	 	 	 	 	 	 	 	 	 	 décalé de 1 pour laisser WordAdr*/
	DMA1_Channel7->CNDTR=DataToReceive->Nb_Data;		/* réglage nombre d'octets à lire */
	DMA1_Channel7->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
	while ((DMA1->ISR&DMA_ISR_TCIF7)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF7;						/* Effacement Flag DMA full */

	/*************************************************
	**  	PHASE 3 : Fermeture communication		**
	*************************************************/
	/* Génération condition stop automatique AUTOEND = 1  */

}


