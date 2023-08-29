
#include "StandByWkupPgm.h"

/* =================================================================================
* ==================   Main_StandByWkUpPgm	     ===================================
 *
 *   Created on: Jul 18, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1,
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 *

* =================================================================================*/

float TempSensor[4],TempSensorOrdered[4];
float MinTemp,Temperature;
int i,j,k; /* Indice boucle*/
int Error;

void Main_StandByWkUpPgm(void)
{
	int Temp;

	Error=0;


	/***************************************************************
	  		Activation Boost 3V -> 5V -> 3,3V
	  		-> Alimentation 3,3V pour ADT7410, OPAmp µphone,
	  		OpAmp LED IR Xbee (si câblé)
	  		-> Alimentation 5V pour RT606 (FSK, si câblé)
	***************************************************************/
	RmDv_EnableBoost;
	Delay_x_ms(50); /* attendre 50ms pour que le ADT7410 se réveille*/

	/***************************************************************
	  		Mesure température
	***************************************************************/
	ADT7410_Init();
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
			 TempSensor[k]=50.0;
		 }
	 }
	 /* Calcul sur la moyenne des éléments, min et max exclus*/
	 Temperature = (TempSensorOrdered[1]+TempSensorOrdered[2])/2.0;


/***************************************************************
			Emission Info UC, attente réception UC
			Trame :
			|Code | Value |

			Error |Value = code erreur string
			Temp  |Value = float brut
			Time  |Value = String formaté HH:Mn:Sec
			ClimOrder |Value = enum|
			Ack   |Value = no value
***************************************************************/





/***************************************************************
			Mise à jour Climatiseur
***************************************************************/
	  /***************************************************************
	  		 test LED IR Clim
	  ***************************************************************/
	  USART_FSK_RT606_OFF();
	  RmDv_TelecoIR_Init();

	  RmDv_TelecoIR_SetCmde(_Stop);

	  RmDv_TelecoIR_DeInit();
/***************************************************************
			Ajustement RTC
***************************************************************/



}



