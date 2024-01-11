//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//

/******************************************************************************
* File Name          : GPIO.c
* Author             : T Rocacher
* Version            : 
* Date               : 18/01/2013
* Description        : driver pour les GPIO
*                     
********************************************************************************/
#include "stm32f10x.h"
#ifndef _GPIO_H__
#define _GPIO_H__


//=======================================================================================
// Rappel sur les ressources 
// Broche : de 0 � 15 pour GPIOA, GPIOB, GPIOC (STM103)
// Revue en Juin 2011 pour �tendre au port D et E du 107
// modif Jan 2013 pour compatibilit� 4_53 : stm32f10x.h et Clock.c
//  Cl� compil : Pas besoin, configur�e pour le 107, donc GPIOD et E accessibles
//=======================================================================================


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


#define GPIO_Toggle(GPIO,Broche) GPIO->ODR=(GPIO->ODR)^(0x01<<Broche)
// Sp�cifier Port (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15)

#define GPIO_Write(GPIO,Broche,Val) GPIO->BRR=(0x01<<Broche); \
                                    GPIO->ODR|=(Val<<Broche)
// Sp�cifier Port (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15) 
// Cette macro est �quivalente � GPIO_Clear et GPIO_Set, mais 
// dans ce cas, c'est la variable val, suivant
// qu'elle est � 1 ou � 0, qui provoquera le clear ou set. Cette
// macro prend tout sons sens lors d'une test bianire et d'une affectation
// en suivant sur une broche de port (pas besoin d'op�rer un test pour ensuite
// faire explicitement un set ou un clear).

#define GPIO_Read(GPIO,Broche) ((GPIO->IDR)&(0x01<<Broche))
// Sp�cifier Port (GPIOA..GPIOC)
// Sp�cifier le num�ro de broche (0 � 15) 
// Port_IO_Lire (GPIO,Broche) est un entier 16 bits	 qui vaut
// 0x0000 si la broche est � 0,  0x01<<Broche sinon
// Cette macro est utilis�e lors d'un test de mise � 1 ou � 0
// d'une broche, �videmment configur�e en entr�e.

#define GPIO_Write_Multi_bits(GPIO,Masque32Bits) (GPIO->BSRR)=Masque32Bits
// Sp�cifier Port (GPIOA..GPIOC)
// Pr�ciser le masque comme suit :
// Masque 32 bits
// | 16 bits Reset | 16 bits Set |
// Chacun des 16 bits est dans l'ordre des broches du GPIO : Br15 ... Br0.
//
// Exemple :
// on veut mettre � 1 le bit 15 et mettre le bit 6 � 0, la valeur du masque sera :
// Masque32Bits = b 0000 0000 0010 0000   1000 0000 0000 0000 = 0x00208000



//______________________________________________________________________________
// IT externes...
//______________________________________________________________________________

void NVIC_Enable_IT(char NumeroIT);
/*
Permet de valider une ligne d'interruption, hors coeur, de 0 � 63. Le num�ro 
est pass� en param�tre.
*/
void NVIC_Disable_IT(char NumeroIT);
/*
Permet d'invalider une ligne d'interruption, hors coeur, de 0 � 63. Le num�ro 
est pass� en param�tre.
*/
void NVIC_Prio_IT(char NumeroIT, char Prio);
/*
Fixe la priorit� de la ligne d'interruption sp�cifi�e.
*/



char NVIC_Ext_IT (GPIO_TypeDef * Port, char Broche, char Front, char Techno, char Prio, void (*IT_function) (void));
// Definitions :
// Front
#define FALLING_EGDE 0
#define RISING_EDGE 1
#define FALLING_RISING_EDGE 2

/*
place la broche du port indiqu�e en entree IT sur front up/down/les deux
La fonction est "cl� en main" pour configurer l'IT sur une broche externe 
voie 0 � 4, port A, B ou C)
pour les autres voies, 5 � 15, c'est un peu plus complexe.
Retourne 1 si erreur, 0 sinon

Le user sp�cifie :
 - port
 - num�ro de broche
 - front actif qui d�clenche l'IT
 - Techno de la broche (flottant, pull down...)
 - Niveau de priorit�
 - nom de la fonction � lancer lors de l'IT
 Tout est param�tr�, jusqu'au NVIC.  Le flag d'IT est mis � 0 automatiquement

 Exemple de code :
NVIC_Ext_IT (GPIOA, 2, RISING_EDGE, INPUT_FLOATING,8,Cligno);
 La broche GPIOA.2 provoque au front montant une IT qui lance la fct Cligno(), avec
 une priorit� de 8. L'entr�e est flottante.


 Pour les broches, de 5 � 15, le STM32 n'utilise que 2 vecteurs d'IT.
 C'est pourquoi, contrairement aux broches 0 � 4, c'est au user de chercher
 le flag lev�, et de le remettre � 0.
 Ce flag, c'est le bit de m�me rang que la broche, du registre EXTI->PR

 Exemple, remise � 0 du flag correspondant � la broche 10:
 Clear_Flag_IT_Ext_5_15(10)

*/

//============== MODIF TR 09 Janvier 2015==================
#define Clear_Flag_IT_Ext_5_15(Voie) EXTI->PR=(EXTI->PR)|(0x01<<Voie)
//============== FIN MODIF TR 09 Janvier 2015==============








//___________________________FIN ___________________________________________________
// suite � ignorer.

/* Retro-compatibilit� avec les noms de fonction de Thierry */
#define Port_IO_Init	GPIO_Configure
#define Port_IO_Set 	GPIO_Set 
#define Port_IO_Clr 	GPIO_Clear
#define Port_IO_Toggle 	GPIO_Toggle
#define Port_IO_Lire 	GPIO_Read

#endif /* _GPIO_H__ */
