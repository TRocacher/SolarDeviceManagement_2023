
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

#define Test_Boost
//#define Test_LED // attention ds ce cas on configure ppull et non alt
				 // Commenter pour test final !!!!
#define Test_Temp
#define Test_FSK_Stack


char i;

float Temperature;

RmDv_WkUp_CurrentState StandByWkUpPgm_CurrentState;
RmDv_WarningCode StandByWkUpPgm_WCode;


/***************************************************************
		Définitions des variables messages pour la stack
***************************************************************/
char TransmitMssg[30];
char ReceivedMssg[30];
char Long;
int longueur;
char ReceivedCodeClim;

RmDv_WkUp_CurrentState StandByWkUpPgm_GetCurrentState(void)
{
	return StandByWkUpPgm_CurrentState;
}

void Main_StandByWkUpPgm(void)
{
#ifdef Test_LED
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
#endif
#ifdef Test_FSK_Stack
	#ifndef Test_LED
		LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	#endif
#endif


	/***************************************************************
	  		Activation Boost 3V -> 5V -> 3,3V
	  		-> Alimentation 3,3V pour ADT7410, OPAmp µphone,
	  		OpAmp LED IR Xbee (si câblé)
	  		-> Alimentation 5V pour RT606 (FSK, si câblé)
	***************************************************************/
#ifdef Test_Boost
	RmDv_EnableBoost;
	Delay_x_ms(50); /* attendre 50ms pour que le ADT7410 se réveille*/
#endif

#ifdef Test_LED
	  /*LED_IR  Output PushPull (FT 5V)*/
	  GPIO_InitStruct.Pin = LED_IR_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
	  LL_GPIO_Init(LED_IR_GPIO_Port, &GPIO_InitStruct);



	while(1)
	{
		RmDv_LED_On;
		Delay_x_ms(200);
		RmDv_LED_Off;
		Delay_x_ms(200);
	}
#endif

	/***************************************************************
	  		Mesure température
	***************************************************************/
#ifdef Test_Temp

	ADT7410_Init();
	Temperature=ADT7410_GetTemp_float();

#endif



#ifdef Test_FSK_Stack
	i=0x30;
	FSKStack_Init(My);
	 /*LED_IR  Output PushPull (FT 5V)*/
	GPIO_InitStruct.Pin = LED_IR_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
	LL_GPIO_Init(LED_IR_GPIO_Port, &GPIO_InitStruct);

	while(1)
	{
		FSKStack_SendNewMssg (UC_Adress,&i, 1);
		i++;
		if (i==0x3A) i=0x30;
		Delay_x_ms(200);
		if (FSKStack_IsNewMssg()==1)
		{
			RmDv_LED_On;
			longueur=FSKStack_GetLen();
			FSKStack_GetNewMssg (ReceivedMssg, longueur);
		}
		Delay_x_ms(200);
		RmDv_LED_Off;
		Delay_x_ms(600);
	}



#endif



}



