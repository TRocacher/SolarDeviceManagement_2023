/*
 * Menu.h
 *
 *  Created on: 15 Avril. 2021
 *      Author: Thierry Rocacher
 *
 *  ======= CONTEXTE DU LOGICIEL ===================
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : gestion soft système PV
 *  DEPENDENCES :
 *
 *
 *  ===========================    GENERALITE   =====================
 *  Le Module menu.c utilise le module StringFct.h et ComUART.h
 *  Le module est entièrement autonome : une fois initialisé, le USER
 *  communique avec le module via l'UART
 *
 *  USER <---UART---> Menu.c
 *
 *  Dès que le USER envoie une commande reconnaissable (up/Down/Left/Right) le module
 *  réagit de manière complètement autonome.
 *
 *  defined keys : (up/Down/Left/Right) = (o/l/k/m) ou (z/s/q/d)
 *
 *  le module est entièrement fonctionnel, mais sa programmation doit être complétée pour disposer
 *  du système de menu désiré dans l'application. Un exemple est donné.
 *
 *  Principe :
 *  Les menus sont gérés par des listes chaînées (Horizontale et verticale).
 *  Chaque maillon (MenuNode) correspond à un item d'un menu.
 *  Chaque maillon contient l'@du maillon précédent, suivant, supérieur et inférieur
 *  D'autres champs existent (voir structure Menu_Node dans ce .h) en particulier
 *  un pointeur de fonction. Celuis ci est lancé systématiquement lors d'une Cmde reçue.
 *  A chaque arrivée d'une Cmde valide, le Maillon en cours (MenuNode) est mise à jour.
 *
 *  Il existe 3 type de Maillons (non explicite) qui se différentient par la fonction particulière
 *  lancée :
 *  - Les maillons passifs (simple navigation)
 *  - les maillons input (saisie d'une float de 1 à 6 digits, de 0 à 3 décimales)
 *  - les maillons output (permettent d'afficher des données)
 *
 *
 *
 *
 *
 *  Etat d'avancement du module :
 *  Maillons passifs et input OK
 *  une seule fonction user : void Menu_Init(UART_HandleTypeDef * UsedUSART)
 *
 *
 *  PROGRAMMATION ACTUELLE A TITRE D EXEMPLE
 *
 *  	**********************              **********************
 *      *Menu principal      *              *Menu 1...           *
 *      * Nenu 1             * -----------> * Nenu 1_1           *
 *      * Nenu 2             * ----------   * Nenu 1_2           *
 *      * Nenu 3             * --------  |  *                    *
 *      * Nenu 4             * -----   | |  **********************
 *      * Nenu 5             * ----  | | |
 *      * Nenu 6             * --  | | | |  **********************
 *      **********************   | | | | |  *Menu 2...           *
 *                               | | | |  > * Nenu 2_1           *
 *                               | | | |    *                    *
 *                               | | | |    *                    *
 *                               | | | |    **********************
 *                               | | | |
 *                               | | | |    **********************
 *                               | | | |    *Menu 3...           *                  (divers menu input pour tester les formats)
 *                               | | |  --> * Nenu 3_1           *  ---------------> VoltParam input
 *                               | | |      * Nenu 3_2           *  ---------------> Param 1.0
 *                               | | |      * Nenu 3_3           *  ---------------> Param 2.0
 *                               | | |      * Nenu 3_4           *  ---------------> Param 2.1
 *                               | | |      * Nenu 3_5           *  ---------------> Param 3.0
 *                               | | |      * Nenu 3_6           *  ---------------> Param 3.1
 *                               | | |      * Nenu 3_7           *  ---------------> Param 3.2
 *                               | | |      **********************                 (NB 3.2 veux dire 3 digit dont 2 décimales)
 *                               | | |
 *                               | | |      **********************
 *                               | | |      *Menu 4...           *
 *                               | |  ----> * Menu 4_0           *  ---------------> Param 4.0
 *                               | |        * Menu 4_1           *  ---------------> Param 4.1
 *                               | |        * Menu 4_2           *  ---------------> Param 4.2
 *                               | |        * Menu 4_3           *  ---------------> Param 4.3
 *                               | |        **********************
 *                               | |
 *                               | |        **********************
 *                               | |        *Menu 5...           *
 *                               |  ------> * Menu 5_0           *  ---------------> Param 5.0
 *                               |          * Menu 5_1           *  ---------------> Param 5.1
 *                               |          * Menu 5_2           *  ---------------> Param 5.2
 *                               |          * Menu 5_3           *  ---------------> Param 5.3
 *                               |          **********************
 *                               |
 *                               |          **********************
 *                               |          *Menu 6...           *
 *                                 -------> * Menu 6_0           *  ---------------> Param 6.0
 *                                          * Menu 6_1           *  ---------------> Param 6.1
 *                                          * Menu 6_2           *  ---------------> Param 6.2
 *                                          * Menu 6_3           *  ---------------> Param 6.3
 *                                          **********************
 *
 *
 * A faire : toutes les fonctions d'interaction avec Appli / Menu
 * A faire : les menus d'affichage

 */




#ifndef MENU_H_
#define MENU_H_

#include "StringFct.h"
#include "UART_F103.h"
//#include "ComUART.h"
#include "stm32f10x.h"


//**************************************************************************************************************
//**************************************************************************************************************
// 						 Specification UART pour la gestion du menu
//**************************************************************************************************************
//**************************************************************************************************************

#define Menu_UART USART2
#define Menu_UART_BdRate 115200
#define Menu_UART_Prio_Callback 1




typedef struct
{
	float Val;
	float MinVal;
	float MaxVal;
	char DigitNb;
	char DecimalNb;
	char NewUserInputFlag;
} Menu_ParamValTypedef;




typedef struct Menu_Node Menu_Node;
struct Menu_Node {
	Menu_Node *  NextNode;
	Menu_Node *  PreviousNode;
	Menu_Node *  UpNode;
	Menu_Node *  DownNode;
	char * Title;
	char MenuPosition;
	char * ItemToPrint;
	void (*NodeExecFct)(void);
	Menu_ParamValTypedef * Param;
};

extern Menu_Node MenuNodePassive_1, MenuNodePassive_2, MenuNodePassive_3, Item_Menu4;
extern Menu_Node MenuNodePassive_1_1,MenuNodePassive_1_2;
extern Menu_Node MenuNodePassive_2_1;
extern Menu_Node MenuNodePassive_3_1,MenuNodePassive_3_2,MenuNodePassive_3_3,MenuNodePassive_3_4,MenuNodePassive_3_5,MenuNodePassive_3_6,MenuNodePassive_3_7;
extern Menu_Node MenuNodeInput_VoltParam;

extern Menu_Node MenuNodeInput_10;
extern Menu_Node MenuNodeInput_20, MenuNodeInput_21;
extern Menu_Node MenuNodeInput_30,MenuNodeInput_31,MenuNodeInput_32;


extern Menu_Node MenuNodePassive_4_0,MenuNodePassive_4_1,MenuNodePassive_4_2,MenuNodePassive_4_3;
extern Menu_Node MenuNodeInput_40,MenuNodeInput_41,MenuNodeInput_42,MenuNodeInput_43;

extern Menu_Node MenuNodePassive_5_0,MenuNodePassive_5_1,MenuNodePassive_5_2,MenuNodePassive_5_3;
extern Menu_Node MenuNodeInput_50,MenuNodeInput_51,MenuNodeInput_52,MenuNodeInput_53;

extern Menu_Node MenuNodePassive_6_0,MenuNodePassive_6_1,MenuNodePassive_6_2,MenuNodePassive_6_3;
extern Menu_Node MenuNodeInput_60,MenuNodeInput_61,MenuNodeInput_62,MenuNodeInput_63;



/**
  * @brief  Initialise le module Menu
  * @param  UsedUSART = le handle de l'usart au sens de Hal
  * @retval None
  * Exempe :Menu_Init(&huart2);
  **/

 void Menu_Init(void);





#endif
