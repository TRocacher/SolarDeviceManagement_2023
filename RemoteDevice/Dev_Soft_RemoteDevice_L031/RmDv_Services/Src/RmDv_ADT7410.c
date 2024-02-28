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




#include "RmDv_ADT7410.h"


I2C_RecSendData_Typedef I2C_Data_Struct;

// on shot mode, 16bits data format

/*______________________________________________________________________________
*_______________________  ADT7410_Init     _____________________________________
 *
 *   Rôle: Initialise le composant ADT7410 en mode One Shot, format data 16 bits
 *   		(LSB = 7.8125m°C, format 9.7)
*   		Initialise au préalable le module I2C1 à 100kHz standard
 *   Param in : _
 *   Exemple : ADT7410_Init();
 *_______________________________________________________________________________*/
short int ADT7410_Init(void)
{
	uint8_t data[4];
	I2C_L031_Init(I2C_ADT7410);
	uint8_t ConfRegVal;
	ConfRegVal=ConfReg_Reso_16;
	ConfRegVal|=ConfReg_Mode_Shutdown;
	data[0]=ConfRegAdr;
	data[1]=ConfRegVal;
	I2C_Data_Struct.Nb_Data=2;
	I2C_Data_Struct.Ptr_Data=data;
	I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
	if (I2C_L031_PutString(I2C1,&I2C_Data_Struct)==0) return -32768;
	return 1;
}



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
short int ADT7410_GetTemp_fract_9_7(void)
{
	uint8_t data[4];
	uint8_t ConfRegVal;
	short int ReturnValue;

	// Lancement one shot
	ConfRegVal=ConfReg_Reso_16;
	ConfRegVal|=ConfReg_Mode_OneShot;
	data[0]=ConfRegAdr;
	data[1]=ConfRegVal;
	I2C_Data_Struct.Nb_Data=2;
	I2C_Data_Struct.Ptr_Data=data;
	I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
	if (I2C_L031_PutString(I2C1,&I2C_Data_Struct)==0) return -32768;

	// Wait at least 240ms
	Delay_x_ms(250);

	// Read temperature
	data[0]=TempHighAdr;
	I2C_Data_Struct.Nb_Data=3;
	I2C_Data_Struct.Ptr_Data=data;
	I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
	if (I2C_L031_GetString(I2C1, &I2C_Data_Struct)==0) return -32768;

	ReturnValue=(data[1]<<8)+data[2];

	// go back to shutdown mode
	ConfRegVal=ConfReg_Reso_16;
	ConfRegVal|=ConfReg_Mode_Shutdown;
	data[0]=ConfRegAdr;
	data[1]=ConfRegVal;
	I2C_Data_Struct.Nb_Data=2;
	I2C_Data_Struct.Ptr_Data=data;
	I2C_Data_Struct.SlaveAdress7bits=ADT7410_Slave8bitsAdr;
	if (I2C_L031_PutString(I2C1,&I2C_Data_Struct)==0) return -32768;


	return ReturnValue;

	}


float ADT7410_GetTemp_float(void)
{
int i,j,k;
char Error;
float TempSensor[4],TempSensorOrdered[4];
float MinTemp,Temperature;
int Temp;

	Error=0;

/* Acquisition 4 valeurs */
	for (i=0;i<4;i++)
	{
		Temp=ADT7410_GetTemp_fract_9_7();
		if (Temp==-32768)
		{
			Error=1;
			break;
		}
		TempSensor[i]=((float)Temp)/128.0;
	}

/* Filtrage */
	if (Error==0)
	{
		/*Classement */
		for (j=0;j<4;j++)
		{
			MinTemp=50.0;
			for (i=0;i<4;i++)
			{
				if  (TempSensor[i]<MinTemp)
				{
					TempSensorOrdered[j]=TempSensor[i];
					MinTemp=TempSensor[i];
					k=i;
				}
			}
			/* k est le rang du min dans le tableau de départ
			 * il faut l'exclure du classement suivant, en le mettant au max*/
			TempSensor[k]=100.0;
		}
		/* Calcul sur la moyenne des éléments, min et max exclus*/
		Temperature = (TempSensorOrdered[1]+TempSensorOrdered[2])/2.0;
	}
	/* si erreur I2C on renvoie une température idiote -100°C : UC doit interprêter erreur ! */
	else
	{
		Temperature = -100.0;
	}
	return Temperature;
}

