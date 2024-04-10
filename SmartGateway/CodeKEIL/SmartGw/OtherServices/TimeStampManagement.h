

/*=======================================================================================
 	Auteur T.R.
  Janvier 2024, 
  Processeur : STM32F103RB à 72MHz
  Logiciel dev : µVision 5.34

 Principe :
 Le module utilise un timer, TIMER_TimeStamp, qui est par défaut TIM4 (voir define ci-après)
 Le type prinipal utilisé dans la lib est 
		TimeStampTypedef (champs sec, min, heure, jour, mois, année)
		
 Une variable globale unique qui régit l'horloge du système est crée : 
		TimeStampClock de type TimeStampTypedef
 elle avance à chaque seconde dans une interuuption  TIMER_TimeStamp
 
 Elle doit être mise à l'heure (par défaut 1 Jan 2024 00:00:00. 
 Cela se fait grâce à la fonction : 
		void TimeStamp_SetClock(TimeStampTypedef * Stamp);
 
 NB: le timer démarre avec la fonction et autorise les IT, prio 0
	void TimerStamp_Start(void);
 NB: la fonction de mise à l'heure, lance automatiquement le timer..;

 Les fonctions 
	int TimeStamp_2_Sec(TimeStampTypedef * Ptr);
	int TimeStamp_substract(TimeStampTypedef * PtrA,TimeStampTypedef * PtrB );
 permettent de faire des différences de temps.
 

 La fonction 
		TimeStampTypedef * TimeStamp_GetClock(void);
	permet d'accéder à la variable TimeStampClock pour y lire le stamp
 
 =============
 Une autre série de fonction s'appuie sur un type plus petit :
	 HourStampTypedef (champs sec, Min Heure). 
 Cela permet de gérer le calendrier de transaction des RmDv dans une journée
 
 On trouve les fonctions associées :	
	int HourStamp_2_Sec(HourStampTypedef * Ptr);
	int HourStamp_substract(HourStampTypedef * PtrA,HourStampTypedef * PtrB );
	void HourStampAdd(HourStampTypedef * PtrA, HourStampTypedef * PtrB );
	
NB : L'interruption seconde TIMER_TimeStamp peut prendre en charge un callback.
Ce dernier sera lancé toutes les h15 et h45. Il s'agit de pouvoir incrémenter un index tps réel
toutes les 30mn pour pouvoir suivre simplement les transactions. Cela se fait avec la fonction :
	void HourStamp_30mnCallbackAssociation(void (*IT_function) (void));
	 

//=======================================================================================*/


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
	short int Sec; 				/* 16 bits*/
	short int Min;				/* 16 bits*/
	short int Hour;				/* 16 bits*/
	short int Day; 				/* 16 bits*/
	short int Month;			/* 16 bits*/
	short int Year;				/* 16 bits*/
}TimeStampTypedef;			/* Total 12 bytes*/	



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
  * @brief  Met à l'heure l'horloge 
  * @param pointeur sur le stamp contenant l'heure voulue
  **/
void TimeStamp_SetClock(TimeStampTypedef * Stamp);


/**
* @brief  remet à 0 une structure de type TimeStampTypedef
  * @retval _
  **/
void TimeStamp_ResetStamp(TimeStampTypedef * Ptr);



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






/* =======================      Gestion heures     =============================
	ici on ne garde que les heures, les minutes et les secondes.
*/

typedef struct 
{
	short int Sec; 				/* 16 bits*/
	short int Min;				/* 16 bits*/
	short int Hour;				/* 16 bits*/
}HourStampTypedef;			/* Total 6 bytes*/	


/**
* @brief  associe un callback qui claque toutes les 30mn avec un décalage de 15mn.
* @paral @ fonction
**/
void HourStamp_30mnCallbackAssociation(void (*IT_function) (void));


/**
* @brief  convertit le hourstamp en seconde à partir de, 00h:00mn:00s
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int HourStamp_2_Sec(HourStampTypedef * Ptr);

/**
* @brief  Détermine la différence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int HourStamp_substract(HourStampTypedef * PtrA,HourStampTypedef * PtrB );

/**
* @brief  A=A+B
  * @retval none
  **/
void HourStampAdd(HourStampTypedef * PtrA, HourStampTypedef * PtrB );

#endif
