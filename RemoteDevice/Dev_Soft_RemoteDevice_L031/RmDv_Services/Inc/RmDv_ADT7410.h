/* =================================================================================
* ==================   RmDvADT7410.c	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *   Dépendance : I2C_L031.c/.h, FctDiverses.c/.h
 *  ------------------------------------------------------------------------------
 *
 *  Gère la capteur de température ADT7410
 *  Permet de lire la température via I2C
 *
 *  NB : ATTENTION, la fonction d'initialisation ne gère pas les IO du bus I2C
 *  S'assurer que SDA et SCL sont en OD avant d'utiliser cette lib
 *
 *  INFORMATIONS COMPOSANT ADT7410


		Puissance consommée, suivant modes de fonctionnement

		700µW (200µA / 3.3V) en Mode Normal
		2µA à 15µA / 3.3V et 5.2 à 25µA / 5V en shutdown mode
		150µW (45µA / 3.3V) en Mode 1 SPS

	->  Normal mode 	: rafale, la mesure se lance en rafale (240ms par mesure)
						/ Conf Reg=0x0,
	->  One Shot Mode 	: une demandé, la conversion est faite (240ms)  puis le circuit
						repasse en shutdown mode / ConfReg =0x01<<5,
	->  1 SPS 			: One Sample per Second (60ms de tps de conversion), le reste
 	 	 	 	 	 	 en idle mode /  ConfReg = 0x2<<5,
	-> Shutdown mode	: ConfReg = 0x3<<5

 ADRESSES :
 Adresses I2C 0x48
 Adresses de registres internes utiles :
  	ConRegAdr 3
    TempHigh Adress 0
    TempLow Adress 1


* =================================================================================*/

#ifndef INC_RMDV_ADT7410_H_
#define INC_RMDV_ADT7410_H_

#include <I2C_L031.h>



/* Adresse I2C du ADT7410 */
#define ADT7410_Slave8bitsAdr (0x48)

/* Adresse des registres internes */
#define ConfRegAdr 3
#define TempHighAdr 0
#define TempLowAdr 1

/*Normal mode : rafale, la mesure se lance en rafale (240ms par mesure) */
#define ConfReg_Mode_Normal (0x0)
/* One Shot Mode : une fois demandé, la conversion est faite (240ms)
 *  puis le circuit repasse en shutdown mode*/
#define ConfReg_Mode_OneShot (0x01<<5)
/* 1 SPS - One Sample per Second (60ms de tps de conversion), le reste en idle mode*/
#define ConfReg_Mode_1SPS (0x2<<5)
/* Shutdown mode :*/
#define ConfReg_Mode_Shutdown (0x3<<5)


/*Résolution température*/
#define ConfReg_Reso_13 (0x0) /*13 bits résolution = 3 LSB à jeter, bits 15..3 / 0.0625°C*/
#define ConfReg_Reso_16 (1<<7) /* 16 bits résolution =  bits 15..0 / 7.8125m°C */



/*______________________________________________________________________________
*_______________________  ADT7410_Init     _____________________________________
 *
 *   Rôle: Initialise le composant ADT7410 en mode One Shot, format data 16 bits
 *   		(LSB = 7.8125m°C, format 9.7)
*   		Initialise au préalable le module I2C1 à 100kHz standard
 *   Param in : _
 *   Exemple : ADT7410_Init();
 *_______________________________________________________________________________*/
void ADT7410_Init(void);

/*______________________________________________________________________________
*_______________________  ADT7410_GetTemp_fract_9_7   __________________________
 *
 *   Rôle: Récupère la température en deux octets 8 bits, donc 16bits.
 *   		Durée 250ms
 *   		Format 9.7
 *   		Valeurs signées classiques
 *
 *   Param in : _
 *   Param out : short int 16 bits
 *   Exemple : ADT7410_GetTemp_fract_9_7();
 *_______________________________________________________________________________*/
short int ADT7410_GetTemp_fract_9_7(void);


#endif /* INC_RMDV_ADT7410_H_ */
