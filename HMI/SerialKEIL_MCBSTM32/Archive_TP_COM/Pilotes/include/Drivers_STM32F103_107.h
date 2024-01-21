

//=======================================================================================
// 	Auteur T.R.
//  
//=======================================================================================

#include "stm32f10x.h"

#ifndef _DRIVERS_STM32F103_107_H__
#define _DRIVERS_STM32F103_107_H__


/* =============================================
				GPIO
 ============================================= */
//______________________________________________________________________________
	   
char GPIO_Configure(GPIO_TypeDef * Port, int Broche, int Sens, int Techno);
//______________________________________________________________________________
/*
La fonction initialise n'importe quelle broche de port (entr�e, sortie, techno....)
Exemple :
Port_IO_Init(GPIOB, 8, OUTPUT, OUTPUT_PPULL);
Place le bit 8 du port B en sortie Push-pull
Renvoie 0 si tout est OK,  et 1 s'il y a un probl�me (plage d'entr�e non repsect�e)
*/

// Sens
#define INPUT 				'i'
#define OUTPUT 				'o'

// Techno pour pin en entr�e (INPUT)
#define ANALOG 				0
#define INPUT_FLOATING 		1
#define INPUT_PULL_DOWN_UP 	2

// Techno pour pin en sortie (OUTPUT)
#define OUTPUT_PPULL 		0
#define OUTPUT_OPDRAIN 		1
#define ALT_PPULL 			2
#define ALT_OPDRAIN 		3
/*
Fonction d'utilisation d'une PIN
Ex : GPIO_Pin_Set(Port_B,4);
La fonction GPIO_Pin_Read renvoie 0 si la PIN est au niveau logique 0
Sinon, c'est que l'entr�e est au niveau logique 1
*/

//______________________________________________________________________________
// Macro
//______________________________________________________________________________
#define GPIO_Set(GPIO,Broche) GPIO->BSRR=(0x01<<Broche)
// Sp�cifier GPIO (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15)
// exemple : Port_IO_Set(GPIOB,8);

#define GPIO_Clear(GPIO,Broche) GPIO->BRR=(0x01<<Broche)
// Sp�cifier Port (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15)
// exemple : GPIO_Clear(GPIOB,8);

#define GPIO_Read(GPIO,Broche) ((GPIO->IDR)&(0x01<<Broche))
// Sp�cifier Port (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15) 
// Port_IO_Lire (GPIO,Broche) est un entier 16 bits	 qui vaut
// 0x0000 si la broche est � 0,  0x01<<Broche sinon
// Cette macro est utilis�e lors d'un test de mise � 1 ou � 0
// d'une broche, �videmment configur�e en entr�e.







/* =============================================
				LES TIMERS
 ============================================= */

//______________________________________________________________________________

float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );
//______________________________________________________________________________

/*
Timer : de TIM1 � TIM4
Duree_us : c'est l'intervalle de temps exprim� en �s entre deux d�bordements successifs
Renvoie la dur�e v�ritable, au plus proche de la valeur souhait�e
Le prescaler est calcul� le plus petit possible pour une pr�cision la plus 
fine possible). Le Timer est lanc� en mode UP et non down (modif 2015).
NB: Lancer avant la fonction d'initialis�tion du syst�me d'hrologe. 
Elle permet de r�gler toutes les horloges par d�faut.

Exemple :
CLOCK_Configure(); // lance l'initialis�tion du syst�me d'horloge de la puce
Timer_1234_Init(TIM2, 10.0 );   // Lance le timer avec une p�riodicit� de 10.0 uS. 
*/
//______________________________________________________________________________




//______________________________________________________________________________

void Active_IT_Debordement_Timer( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
//______________________________________________________________________________

/*
La fonction initialise le p�riph�rique et le NVIC de mani�re � g�n�rer une interruption 
� chaque d�bordement du timer pr�cis�.
La fonction pass�e en param�tre, forc�ment de type void Toto(void) est lanc�e sous interruption
Prio fixe la priorit� (0 � 15, 0 �tant le plus prioritaire)

Exemple :
Active_IT_Debordement_Timer( TIM2, 1, IT_Timer2); // La fonction active une interruption 
lors du d�bordement du Timer 2. La fonction appel�e est IT_Timer2. La priorit� associ�e est	1 
*/




#endif
