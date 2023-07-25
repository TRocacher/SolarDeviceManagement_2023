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
 *  Le concept I2C sur STML031 (prérequis SDA et SCL en OD)
 *  Il existe un compteur qu'il faut charger (il ne se décrémente pas en cours de transmission)
 *  Il correspond au nombre de bytes à transmettre.
 *  Le bit AUTOEND permet de générer un stop automatiquement une fois le nombre de bytes
 *  écoulé.
 *
 *  Emission du start : l'adresse slave aura été programmée. Dès que l'émission a lieu
 *  					l'adresse est posée sur le bus avec attente bus libre au préalable
 *  Dès que l'adresse est acquittée, le flag TXIS passe à 1.
 *  En mode scrutation, dès que TXIS pass à 1, on envoie un octet.
 *  Dès que les octets sont passés, un stop est généré.

* =================================================================================*/

//#define DMA
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
	I2Cx->CR1 &=~I2C_CR1_PE; /* Reset I2C */
	I2Cx->TIMINGR=0x00505D8D; /* Réglage Timing, 100kHz avec CkI2C = 24MHz (valeur Cube)*/
	I2Cx->CR1=0; /* Clear CR1 par défaut*/
	/*I2C_CR1_ANFOFF=0 Validation filtre analogique */
	/*I2C_CR1_DNF=0000b Invalidation filtre numérique */
#ifdef DMA
	I2Cx->CR1 =I2C_CR1_RXDMAEN|I2C_CR1_TXDMAEN; /*Rx/Tx DMA enable */
#endif
	I2Cx->CR1 |=I2C_CR1_PE; /* Enable I2C */


#ifdef DMA
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
#endif
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
char I2C_L031_NByte;
uint8_t * I2C_L031_Ptr_Data;
void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2C_L031_Ptr_Data=DataToSend->Ptr_Data;
	I2C_L031_NByte=(DataToSend->Nb_Data)+1;  /* on passe tout y compris WordAdress donc +1

	I2Cx->ICR=0xFF; /* Clear all flags */////////////////////////////// check
	I2Cx->CR2=0; /* Clear CR2 par défaut*/
	/*I2C_CR2_RELOAD=0  Nécessaire si nbre de bytes < 255 */
	/*I2C_CR2_RD_WRN=0 Write acces */
	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2|=DataToSend->SlaveAdress7bits<<1; /* @ I2C ADT7410 ds le champ SADD de CR2
	 	 	 	 	 	 	 	 	 	 	  avec un déclage de 1 (SADD[0] = don't care*/
	I2Cx->CR2|=I2C_L031_NByte<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop
	 	 	 	 	 	 	 	 	 	 	 	 	 Word Adress compris*/



	/*************************************************
	**  	PHASE 1 :  I2C Adressing			   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_START; /* Start cond. après avoir testé l'acces
	 	 	 	 	 	 	 	 + adressing*/

	/*************************************************
	**  	PHASE 2 : ECRITURE Data				**
	*************************************************/
	while (I2C_L031_NByte!=0)
	{
		while ((I2Cx->ISR&I2C_ISR_TXIS)==0); /* attendre la levée de flag pour émettre*/
		I2Cx->TXDR=*(I2C_L031_Ptr_Data);
		(I2C_L031_Ptr_Data)++;
		I2C_L031_NByte--;
	}

#ifdef DMA
	DMA1_Channel6->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel6->CMAR=(uint32_t)DataToSend->Ptr_Data;	/* Affectation adresse buffer */
	DMA1_Channel6->CNDTR=DataToSend->Nb_Data;			/* réglage nombre d'octets à lire */
	DMA1_Channel6->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
	while ((DMA1->ISR&DMA_ISR_TCIF6)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF6;						/* Effacement Flag DMA full */
#endif


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
	I2C_L031_Ptr_Data=DataToReceive->Ptr_Data;
	I2C_L031_NByte=(DataToReceive->Nb_Data);  /* on ne passera en boucle que les data */

	I2Cx->ICR|=I2C_ICR_STOPCF; /* Clear all flags *//////////////
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
	I2Cx->TXDR=*I2C_L031_Ptr_Data;
	while ((I2Cx->ISR&I2C_ISR_TC)==0); /* attendre la fin d'émission, Nbyte, (=1 ici)*/
	/* préparation lecture*/
	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2|=I2C_L031_NByte<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop */
	I2Cx->CR2|=I2C_CR2_RD_WRN; /*read acces */
	I2C_L031_Ptr_Data++; /* positionnement pointeur sur les data (on saute WordAdress)*/

	I2Cx->CR2|=I2C_CR2_START; /* ReStart cond. après avoir testé l'acces*/

	/****************************************
	**  	PHASE 3 : LECTURE  de Nb_Data  **
	*****************************************/

	while (I2C_L031_NByte!=0)
	{
		while ((I2Cx->ISR&I2C_ISR_RXNE)==0); /* attendre la levée de flag pour émettre*/
		*I2C_L031_Ptr_Data=I2Cx->RXDR;
		I2C_L031_Ptr_Data++;
		I2C_L031_NByte--;
	}



#ifdef DMA
	DMA1_Channel7->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel7->CMAR=(uint32_t)((DataToReceive->Ptr_Data)+1);	/* Affectation adresse buffer
	 	 	 	 	 	 	 	 	 	 	 	 	 	 décalé de 1 pour laisser WordAdr*/
	DMA1_Channel7->CNDTR=DataToReceive->Nb_Data;		/* réglage nombre d'octets à lire */
	DMA1_Channel7->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
	while ((DMA1->ISR&DMA_ISR_TCIF7)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF7;						/* Effacement Flag DMA full */
#endif


	/*************************************************
	**  	PHASE 3 : Fermeture communication		**
	*************************************************/
	/* Génération condition stop automatique AUTOEND = 1  */

}


