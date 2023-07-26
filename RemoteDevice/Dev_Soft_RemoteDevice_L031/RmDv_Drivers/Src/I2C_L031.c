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
 * void I2C_L031_Init(I2C_TypeDef * I2Cx);
 * void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend);
 * void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive);
 *
 * Voir commentaires fonctions pour plus de détail.
 * La lib fonctionne en scrutation ou en DMA avec scrutation fin DMA à la fin
 *
 * Utiliser #define DMA ou pas selon votre choix

* =================================================================================*/

//#define DMA
#include <I2C_L031.h>

/*______________________________________________________________________________
*_______________________ void I2C_L031_Init_____________________________________
 *
 *   Rôle: Initialise le module I2C
 *   Param in : I2C_TypeDef * I2Cx
 *   Exemple : I2C_L031_Init(I2C1);
 *_______________________________________________________________________________*/
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

	/*************************************************
	**  	Préparation DMA :DMA1 channel 2         **
	**  	en émission, channel 3 en réception 	**
	*************************************************/
	/* Préparation DMA :DMA1 channel 6 en émission, channel 7 en réception */
	RCC->AHBENR|= RCC_AHBENR_DMA1EN; /* Activation Clk DMA1*/
	DMA1_CSELR->CSELR|=6<<4|6<<8; /* sélection de I2C1 Tx sur channel 2 DMA
	 	 	 	 	 	 	 	 	 	 	 	 et I2C1 Rx sur channel 3 DMA */
	DMA1_Channel2->CCR=0; /* init à 0 par défaut */
	//DMA1_Channel2->CCR &=~DMA_CCR_PSIZE;
	//DMA1_Channel2->CCR &=~DMA_CCR_MSIZE;
	//DMA1_Channel2->CCR&=~DMA_CCR_PINC;
	//DMA1_Channel2->CCR&=~DMA_CCR_CIRC;/* Réglage du mode circulaire : non circulaire */
	DMA1_Channel3->CCR=0; /* init à 0 par défaut */
	//DMA1_Channel3->CCR &=~DMA_CCR_PSIZE;
	//DMA1_Channel3->CCR &=~DMA_CCR_MSIZE;
	//DMA1_Channel3->CCR&=~DMA_CCR_PINC;
	//DMA1_Channel3->CCR&=~DMA_CCR_CIRC;
	//DMA1_Channel3->CCR&=~DMA_CCR_DIR;		/* 	 I2C vers Buffer (lecture) */
	DMA1_Channel2->CPAR=(uint32_t)&I2Cx->TXDR; /* Affectation @ périphérique : celle de TxDR */
	DMA1_Channel3->CPAR=(uint32_t)&I2Cx->RXDR; /* Affectation @ périphérique : celle de RxDR */
	DMA1_Channel2->CCR|=DMA_CCR_MINC;			/* Réglage de l'incrémentation côté buffer RAM : +1 */
	DMA1_Channel3->CCR|=DMA_CCR_MINC;
	DMA1_Channel2->CCR|=DMA_CCR_DIR;		/* Réglage du sens DMA 	: Buffer vers I2C (écriture) */

#endif
	I2Cx->CR1 |=I2C_CR1_PE; /* Enable I2C */
}


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
char I2C_L031_NByte;
uint8_t * I2C_L031_Ptr_Data;
void I2C_L031_PutString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToSend)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2C_L031_Ptr_Data=DataToSend->Ptr_Data;
	I2C_L031_NByte=(DataToSend->Nb_Data);  /* on passe tout y compris WordAdress */
	I2Cx->ICR=0x3F<<8|0x7<<3; /* tous les flags sont resetés*/
	I2Cx->CR2=0; /* Clear CR2 par défaut*/
	/*I2C_CR2_RELOAD=0  Nécessaire si nbre de bytes < 255 */
	/*I2C_CR2_RD_WRN=0 Write acces */
	I2Cx->CR2|=I2C_CR2_AUTOEND; /* La condition de stop sera générée après les n bytes transmis  */
	I2Cx->CR2|=DataToSend->SlaveAdress7bits<<1; /* @ I2C ADT7410 ds le champ SADD de CR2
	 	 	 	 	 	 	 	 	 	 	  avec un déclage de 1 (SADD[0] = don't care*/
	I2Cx->CR2|=I2C_L031_NByte<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop
	 	 	 	 	 	 	 	 	 	 	 	 	 Word Adress compris*/

#ifdef DMA
	DMA1_Channel2->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel2->CMAR=(uint32_t)I2C_L031_Ptr_Data;	/* Affectation adresse buffer */
	DMA1_Channel2->CNDTR=I2C_L031_NByte;			/* réglage nombre d'octets à lire */
	DMA1_Channel2->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
#endif

	/*************************************************
	**  	PHASE 1 :  I2C Adressing			   	**
	*************************************************/
	I2Cx->CR2|=I2C_CR2_START; /* Start cond. après avoir testé l'acces
	 	 	 	 	 	 	 	 + adressing*/

	/*************************************************
	**  	PHASE 2 : ECRITURE Data				**
	*************************************************/
#ifndef DMA /* Scrutation */
	while (I2C_L031_NByte!=0)
	{
		while ((I2Cx->ISR&I2C_ISR_TXIS)==0); /* attendre la levée de flag pour émettre*/
		I2Cx->TXDR=*(I2C_L031_Ptr_Data);
		(I2C_L031_Ptr_Data)++;
		I2C_L031_NByte--;
	}
#endif

	/*************************************************
	**  	PHASE 3 : Fermeture communication		**
	*************************************************/
	/* Génération condition stop automatique AUTOEND = 1  */
#ifdef DMA
	while ((DMA1->ISR&DMA_ISR_TCIF2)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF2;						/* Effacement Flag DMA full */
#endif
}



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
void I2C_L031_GetString(I2C_TypeDef * I2Cx, I2C_RecSendData_Typedef * DataToReceive)
{
	/*************************************************
	**  	PHASE 0 :  PREPARATION				   	**
	*************************************************/
	I2C_L031_Ptr_Data=DataToReceive->Ptr_Data;
	I2C_L031_NByte=(DataToReceive->Nb_Data)-1;  /* on ne passera en boucle que les data */
	I2Cx->ICR=0x3F<<8|0x7<<3; /* tous les flags sont resetés*/
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
	I2Cx->CR2|=(I2C_L031_NByte)<<I2C_CR2_NBYTES_Pos;  /*réglage du nbre de bytes à envoyer avant stop */
	I2Cx->CR2|=I2C_CR2_RD_WRN; /*read acces */
	I2C_L031_Ptr_Data++; /* positionnement pointeur sur les data (on saute WordAdress)*/


#ifdef DMA
	DMA1_Channel3->CCR &=~DMA_CCR_EN;					/* Blocage DMA pour configuration */
	DMA1_Channel3->CMAR=(uint32_t)(I2C_L031_Ptr_Data);	/* Affectation adresse buffer
	 	 	 	 	 	 	 	 	 	 	 	 	 	 décalé de 1 pour laisser WordAdr*/
	DMA1_Channel3->CNDTR=I2C_L031_NByte;		/* réglage nombre d'octets à lire */
	DMA1_Channel3->CCR|=DMA_CCR_EN;						/* Démarrage DMA */
#endif


	I2Cx->CR2|=I2C_CR2_START; /* ReStart cond. après avoir testé l'acces*/

	/****************************************
	**  	PHASE 3 : LECTURE  de Nb_Data  **
	*****************************************/

#ifdef DMA
	while ((DMA1->ISR&DMA_ISR_TCIF3)==0);				/* Attente DMA full */
	DMA1->IFCR|=DMA_IFCR_CTCIF3;						/* Effacement Flag DMA full */
#endif

#ifndef DMA /* Scrutation */
	while (I2C_L031_NByte!=0)
	{
		while ((I2Cx->ISR&I2C_ISR_RXNE)==0); /* attendre la levée de flag pour émettre*/
		*I2C_L031_Ptr_Data=I2Cx->RXDR;
		I2C_L031_Ptr_Data++;
		I2C_L031_NByte--;
	}
#endif


	/*************************************************
	**  	PHASE 3 : Fermeture communication		**
	*************************************************/
	/* Génération condition stop automatique AUTOEND = 1  */

}


