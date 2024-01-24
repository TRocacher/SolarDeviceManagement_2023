

//=======================================================================================
// 	Auteur T.R.
//  Janvier 2024, 
//  Processeur : STM32F103RB à 72MHz
//  Logiciel dev : µVision 5.34
//=======================================================================================


#ifndef _TIMESTAMPMANAGEMENT_H__
#define _TIMESTAMPMANAGEMENT_H__


#include "Timer_F103.h"

/* =============================================
				TimeStampManagement

 Module qui gère les horodatages.
 Associé à un timer programmé à 1 seconde
 
 ============================================= */


#define TIMER_TimeStamp TIM4

#define Janv 1
#define Fev 2
#define Mars 3
#define Avr 4
#define Mai 5
#define Juin 6
#define Jul 7
#define Aout 8
#define Sept 9
#define Oct 10
#define Nov 11
#define Dec 12

typedef struct 
{
	short int Sec; 
	short int Min;
	short int Hour;
	short int Day; 
	short int Month;
	short int Year;
}TimeStampTypedef;



/**
  * @brief  Initialise TIMER_TimeStamp à une seconde et le lance.
	*					associe le callback
  **/
void TimerStamp_Start(void);


/**
  * @brief  Renvoie l'horodatage réel
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
TimeStampTypedef * TimeStamp_GetClock(void);



/**
* @brief  convertit le timestamp en seconde à partir du 1 Jan 2024, 00:00:00
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int TimeStamp_2_Sec(TimeStampTypedef * Ptr);

/**
* @brief  Détermine la différence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int TimeStamp_substract(TimeStampTypedef * PtrA,TimeStampTypedef * PtrB );

#endif
