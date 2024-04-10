

/*=======================================================================================
 	Auteur T.R.
  Janvier 2024, 
  Processeur : STM32F103RB � 72MHz
  Logiciel dev : �Vision 5.34

 Principe :
 Le module utilise un timer, TIMER_TimeStamp, qui est par d�faut TIM4 (voir define ci-apr�s)
 Le type prinipal utilis� dans la lib est 
		TimeStampTypedef (champs sec, min, heure, jour, mois, ann�e)
		
 Une variable globale unique qui r�git l'horloge du syst�me est cr�e : 
		TimeStampClock de type TimeStampTypedef
 elle avance � chaque seconde dans une interuuption  TIMER_TimeStamp
 
 Elle doit �tre mise � l'heure (par d�faut 1 Jan 2024 00:00:00. 
 Cela se fait gr�ce � la fonction : 
		void TimeStamp_SetClock(TimeStampTypedef * Stamp);
 
 NB: le timer d�marre avec la fonction et autorise les IT, prio 0
	void TimerStamp_Start(void);
 NB: la fonction de mise � l'heure, lance automatiquement le timer..;

 Les fonctions 
	int TimeStamp_2_Sec(TimeStampTypedef * Ptr);
	int TimeStamp_substract(TimeStampTypedef * PtrA,TimeStampTypedef * PtrB );
 permettent de faire des diff�rences de temps.
 

 La fonction 
		TimeStampTypedef * TimeStamp_GetClock(void);
	permet d'acc�der � la variable TimeStampClock pour y lire le stamp
 
 =============
 Une autre s�rie de fonction s'appuie sur un type plus petit :
	 HourStampTypedef (champs sec, Min Heure). 
 Cela permet de g�rer le calendrier de transaction des RmDv dans une journ�e
 
 On trouve les fonctions associ�es :	
	int HourStamp_2_Sec(HourStampTypedef * Ptr);
	int HourStamp_substract(HourStampTypedef * PtrA,HourStampTypedef * PtrB );
	void HourStampAdd(HourStampTypedef * PtrA, HourStampTypedef * PtrB );
	
NB : L'interruption seconde TIMER_TimeStamp peut prendre en charge un callback.
Ce dernier sera lanc� toutes les h15 et h45. Il s'agit de pouvoir incr�menter un index tps r�el
toutes les 30mn pour pouvoir suivre simplement les transactions. Cela se fait avec la fonction :
	void HourStamp_30mnCallbackAssociation(void (*IT_function) (void));
	 

//=======================================================================================*/


#ifndef _TIMESTAMPMANAGEMENT_H__
#define _TIMESTAMPMANAGEMENT_H__


#include "Timer_F103.h"

/* =============================================
				TimeStampManagement

 Module qui g�re les horodatages.
 Associ� � un timer programm� � 1 seconde
 
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
  * @brief  Initialise TIMER_TimeStamp � une seconde et le lance.
	*					associe le callback
  **/
void TimerStamp_Start(void);


/**
  * @brief  Renvoie l'horodatage r�el
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
TimeStampTypedef * TimeStamp_GetClock(void);


/**
  * @brief  Met � l'heure l'horloge 
  * @param pointeur sur le stamp contenant l'heure voulue
  **/
void TimeStamp_SetClock(TimeStampTypedef * Stamp);


/**
* @brief  remet � 0 une structure de type TimeStampTypedef
  * @retval _
  **/
void TimeStamp_ResetStamp(TimeStampTypedef * Ptr);



/**
* @brief  convertit le timestamp en seconde � partir du 1 Jan 2024, 00:00:00
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int TimeStamp_2_Sec(TimeStampTypedef * Ptr);

/**
* @brief  D�termine la diff�rence en secondes entre le TimeStamp A et le B
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
* @brief  associe un callback qui claque toutes les 30mn avec un d�calage de 15mn.
* @paral @ fonction
**/
void HourStamp_30mnCallbackAssociation(void (*IT_function) (void));


/**
* @brief  convertit le hourstamp en seconde � partir de, 00h:00mn:00s
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int HourStamp_2_Sec(HourStampTypedef * Ptr);

/**
* @brief  D�termine la diff�rence en secondes entre le TimeStamp A et le B
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
