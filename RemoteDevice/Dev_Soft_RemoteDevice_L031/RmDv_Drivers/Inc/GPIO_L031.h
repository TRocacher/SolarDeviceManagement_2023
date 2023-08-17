

#ifndef _GPIO_L031_H__
#define _GPIO_L031_H__
#include "stm32l0xx.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"

/* ===============================================================================
 * ==================    GPIO_L031.c   ===========================================
 *
 *   Created on: Aout 16, 2023
 *   Author: trocache
 *   Tool : CubeIDE 1.12.1, from CubeMx for initialization
 *   Target : STM32L031
 *  ------------------------------------------------------------------------------
 *
 * La fonction de configuration des IO n'est pas codée puisque déjà fait ds le code
 * de RmDv via LL
 * Les fonctions set/reset et lecture sont strictement les mêmes que LL et que les
 * lib 103. Elles sont conservées en tant que macro dans le .h
 *
 * !!! ATTENTION : pour les IT externe, on implémente QUE la voie
 * 5 pour le BP user
 * 15 pour la ligne CD
 * Le reste n'est pas impléménté pour ne pas gaspiller de place inutilement.
 *
 * !!! ATTENTION : ds cette configuration IT ext le champ Techno est "pipo" puisque
 * les IO sont paramétées ailleurs. C'est conservé pour garder la compatibilité avec
 * la version 103.
 *
 * ===============================================================================*/





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

/* Doc tech : STM32F103RM.pdf
 Port : GPIOA � GPIOC (extension D et E pour STM107 , pr�ciser la cl� de compil STM32F107 !!!)
 Broche : de 0 � 15 

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




//______________________________________________________________________________
// IT externes...
//______________________________________________________________________________





void NVIC_Ext_IT (GPIO_TypeDef * Port, char Broche, char Front, char Techno, char Prio, void (*IT_function) (void));
// Definitions :
// Front
#define FALLING_EGDE 2
#define RISING_EDGE 1
#define FALLING_RISING_EDGE 3







#endif /* _GPIO_H__ */
