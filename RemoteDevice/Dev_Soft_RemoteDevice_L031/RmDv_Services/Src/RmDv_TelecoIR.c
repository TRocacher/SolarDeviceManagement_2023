

/* =================================================================================
* ==================    RmDv_TelecoIR.c	         ===================================
 *
 *   Created on: 24/08/2023
 *   Author: T.Rocacher
 *   Tool : Cube IDE 1.12.1
 *   Target : STM32L31F6P7
 *  ------------------------------------------------------------------------------
 *
 *
* =================================================================================*/

#include "RmDv_TelecoIR.h"

/***************************************************************
		 Les codes de la climatisation
***************************************************************/
#define RmDv_TelecoIR_TabOffLen 36
#define RmDv_TelecoIR_TabCommonCodeLen 19
#define RmDv_TelecoIR_CodeNumber 6
#define RmDv_TelecoIR_TabCodeLen 18

const char RmDv_TelecoIR_TabOFF[RmDv_TelecoIR_TabOffLen]={0xFF, 0x08, 0x8A, 0xA2, 0xA2, 0x28, 0xA8, 0x8A, 0x22, 0xA2, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A, 0x22, 0xAA, 0xA8, 0x8A, 0xAA, 0x28, 0x8A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x8A, 0x2A, 0x28};
const char RmDv_TelecoIR_Tab_CommonCode[RmDv_TelecoIR_TabCommonCodeLen]={0xFF, 0x08, 0x8A, 0xA2, 0xA2, 0x28, 0xA8, 0x8A, 0x22, 0xA2, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0x8A, 0x8A, 0x8A, 0xAA};
const char RmDv_TelecoIR_TabCode[RmDv_TelecoIR_CodeNumber][RmDv_TelecoIR_TabCodeLen]={
					 {0xA2, 0x88, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x88, 0x8A, 0x22, 0x80},
		             {0xAA, 0x22, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x88, 0xA2, 0x28, 0x00},
					 {0xA2, 0x28, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0xA2, 0x28, 0x8A, 0x00},
					 {0xA8, 0xA2, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x22, 0x88, 0xA0, 0x00},
					 {0xA2, 0xA2, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA2, 0x28, 0xA2, 0x28, 0x00},
					 {0xAA, 0x8A, 0xAA, 0xA2, 0x8A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA2, 0x8A, 0x22, 0x80, 0x00}
};

/***************************************************************
		 Variables de lecture des codes
***************************************************************/
char RmDv_TelecoIR_BitStart;
char RmDv_TelecoIR_CurrentByte;
char RmDv_TelecoIR_IndexBit;
int RmDv_TelecoIR_IndexOctet;

void RmDv_TelecoIR_Bit_Callback(void);

void RmDv_TelecoIR_Init(void)
{
	/* Configuration du timer en PWM */
	Timer_CkEnable(RmDv_TelecoIR_Timer_PWM);
	Timer_Set_Period(RmDv_TelecoIR_Timer_PWM, 632-1, 1-1 ); // 38kHz à 24MHz (en fait 24M/632=37.97kHz)
	Timer_ConfPWM_Ch2(RmDv_TelecoIR_Timer_PWM);
	Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,Clear);
	Timer_Set_Duty(RmDv_TelecoIR_Timer_PWM,2,50);
	Timer_EnablePWMChan(RmDv_TelecoIR_Timer_PWM,2);
	/* Configuration du timer bit*/
	Timer_CkEnable(RmDv_TelecoIR_Timer_Bit) ;
	Timer_Set_Period(RmDv_TelecoIR_Timer_Bit, 430*24-1, 1-1 ); // 430µs à 24MHz
	Timer_IT_Enable( RmDv_TelecoIR_Timer_Bit, RmDv_TelecoIR_Prio_IT_Bit, RmDv_TelecoIR_Bit_Callback);
	Timer_Disable_IT_Callback(RmDv_TelecoIR_Timer_Bit);
}

void RmDv_TelecoIR_DeInit(void)
{
	/* Arrêt PWM*/
	Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,Clear);
	Timer_CkDisable(RmDv_TelecoIR_Timer_PWM);

	/* Configuration du timer bit*/
	Timer_Disable_IT_Callback(RmDv_TelecoIR_Timer_Bit);
	Timer_CkDisable(RmDv_TelecoIR_Timer_Bit);

}


void RmDv_TelecoIR_BitByteIncOverflow(void);

void RmDv_TelecoIR_SetCmde(RmDv_TelecoIR_Cmde Cmde)
{
	int TabIdx;
	RmDv_TelecoIR_BitStart=0;
	Timer_Enable_IT_Callback(RmDv_TelecoIR_Timer_Bit); /* permet d'activer le flag d'attente
	 	 	 	 	 	 	 	 	 	 	 	 	 	 RmDv_TelecoIR_BitStart */
	RmDv_TelecoIR_CurrentByte=0;
	RmDv_TelecoIR_IndexOctet=0;
	RmDv_TelecoIR_IndexBit=1<<7; /* préparation sur bit de poids fort */

	switch(Cmde)
	{
	case _Chaud_18_VanBas_FanAuto:TabIdx=0;break;
	case _Chaud_19_VanBas_FanAuto:TabIdx=1;break;
	case _Chaud_20_VanBas_FanAuto:TabIdx=2;break;
	case _Chaud_21_VanBas_FanAuto:TabIdx=3;break;
	case _Chaud_22_VanBas_FanAuto:TabIdx=4;break;
	case _Chaud_23_VanBas_FanAuto:TabIdx=5;break;
	default : TabIdx=0;
	}

	/***************************************************************
			 Gestion de la cmde Stop
	***************************************************************/

	if (Cmde==_Stop)
	{
		while(RmDv_TelecoIR_IndexOctet<RmDv_TelecoIR_TabOffLen)
		{
			while(RmDv_TelecoIR_BitStart==0); /* Attente 430µs, début du bit, tps réel*/
			RmDv_TelecoIR_BitStart=0;
			RmDv_TelecoIR_CurrentByte=RmDv_TelecoIR_TabOFF[RmDv_TelecoIR_IndexOctet];
			if ((RmDv_TelecoIR_CurrentByte&RmDv_TelecoIR_IndexBit)==RmDv_TelecoIR_IndexBit)
			{
				Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,PWM); // Timer_EnablePWMChan(RmDv_TelecoIR_Timer_PWM,2);
			}
			else Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,Clear); // Timer_DisablePWMChan(RmDv_TelecoIR_Timer_PWM,2);
			RmDv_TelecoIR_BitByteIncOverflow();
		}
	}

	/***************************************************************
			 Gestion des autres commandes
	***************************************************************/
	else
	{

		/* code commun */
		while(RmDv_TelecoIR_IndexOctet<RmDv_TelecoIR_TabCommonCodeLen)
		{
			while(RmDv_TelecoIR_BitStart==0); /* Attente 430µs, début du bit, tps réel*/
			RmDv_TelecoIR_BitStart=0;
			RmDv_TelecoIR_CurrentByte=RmDv_TelecoIR_Tab_CommonCode[RmDv_TelecoIR_IndexOctet];
			if ((RmDv_TelecoIR_CurrentByte&RmDv_TelecoIR_IndexBit)==RmDv_TelecoIR_IndexBit)
			{
				Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,PWM); // Timer_EnablePWMChan(RmDv_TelecoIR_Timer_PWM,2);
			}
			else Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,Clear); // Timer_DisablePWMChan(RmDv_TelecoIR_Timer_PWM,2);

			RmDv_TelecoIR_BitByteIncOverflow();
		}

		/* codes particuliers */
		RmDv_TelecoIR_CurrentByte=0; /* Remise à 0 des param pour seconde boucle*/
		RmDv_TelecoIR_IndexOctet=0;
		RmDv_TelecoIR_IndexBit=1<<7; /* préparation sur bit de poids fort */
		while(RmDv_TelecoIR_IndexOctet<RmDv_TelecoIR_TabCodeLen)
				{
					while(RmDv_TelecoIR_BitStart==0); /* Attente 430µs, début du bit, tps réel*/
					RmDv_TelecoIR_BitStart=0;
					RmDv_TelecoIR_CurrentByte= RmDv_TelecoIR_TabCode[TabIdx][RmDv_TelecoIR_IndexOctet];
					if ((RmDv_TelecoIR_CurrentByte&RmDv_TelecoIR_IndexBit)==RmDv_TelecoIR_IndexBit)
					{
						Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,PWM); // Timer_EnablePWMChan(RmDv_TelecoIR_Timer_PWM,2);
					}
					else Timer_SetOutputMode(RmDv_TelecoIR_Timer_PWM,Clear); // Timer_DisablePWMChan(RmDv_TelecoIR_Timer_PWM,2);

					RmDv_TelecoIR_BitByteIncOverflow();
				}

	}

	Timer_Disable_IT_Callback(RmDv_TelecoIR_Timer_Bit);
}

void RmDv_TelecoIR_BitByteIncOverflow(void)
{
	RmDv_TelecoIR_IndexBit=RmDv_TelecoIR_IndexBit>>1; /* déplacement mask -> */

	/* Gestion débordement */
	if (RmDv_TelecoIR_IndexBit==0)
	{
		RmDv_TelecoIR_IndexBit=1<<7;
		RmDv_TelecoIR_IndexOctet++;
	}
}



/***************************************************************
		 Levée Flag RmDv_TelecoIR_BitStart
***************************************************************/
void RmDv_TelecoIR_Bit_Callback(void)
{
	RmDv_TelecoIR_BitStart=1;
}
