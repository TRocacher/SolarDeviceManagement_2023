#ifndef USERMENU_H_
#define USERMENU_H_

/*
 * UserMenu.h
 *
 *
 *
 *  ===========================    GENERALITE   =====================



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
 */







#include "MenuType.h"

void Menu_Node_InitDefinition(void);


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




#endif
