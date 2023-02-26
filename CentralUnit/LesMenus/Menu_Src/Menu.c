/*
 * MyAnalog.c
 *
 *  Created on: 15 Avril. 2021
 *      Author: trocache
 *
 *  CIBLE : stm32L476RG (Nucléo board)
 *  IDE : Cube IDE vers 1.5.1
 *  CONTEXTE : gestion soft système PV
 *  DEPENDENCES : ComUART
 *
 *
 */


#include "Menu.h"

/******************************************************************************
*  PRIVATE Variables
*
******************************************************************************/

/*------------------------------------------------
*  MENU NODES, ITEMS
*  -> user : ajoutez autant de Menu_Node que d'items
*  dans les menus
*  -> user : remplir les menu nodes en fin de fichier
-------------------------------------------------*/
enum {First, Middle, Last};
void Menu_PassivePrint(void);
void Menu_InputStateMachine(void);


// Param nul
Menu_ParamValTypedef ParamNull;







/*==========================================================================================
 *******************************************************************************************
 ******** 		USER ZONE BEGIN      *******************************************************
 *******************************************************************************************
 ==========================================================================================*/
/*............................................
 *
 * 		Définitions des Menu_Nodes utiles
 * 		à déclarer dans le menu.h
 *
.............................................*/

Menu_Node MenuNodePassive_1, MenuNodePassive_2, MenuNodePassive_3, MenuNodePassive_4,MenuNodePassive_5,MenuNodePassive_6;
Menu_Node MenuNodePassive_1_1,MenuNodePassive_1_2;
Menu_Node MenuNodePassive_2_1;
Menu_Node MenuNodePassive_3_1,MenuNodePassive_3_2,MenuNodePassive_3_3,MenuNodePassive_3_4,MenuNodePassive_3_5,MenuNodePassive_3_6,MenuNodePassive_3_7;
Menu_Node MenuNodeInput_VoltParam;

Menu_Node MenuNodeInput_10;
Menu_Node MenuNodeInput_20,MenuNodeInput_21;
Menu_Node MenuNodeInput_30,MenuNodeInput_31,MenuNodeInput_32;

Menu_Node MenuNodePassive_4_0,MenuNodePassive_4_1,MenuNodePassive_4_2,MenuNodePassive_4_3;
Menu_Node MenuNodeInput_40,MenuNodeInput_41,MenuNodeInput_42,MenuNodeInput_43;

Menu_Node MenuNodePassive_5_0,MenuNodePassive_5_1,MenuNodePassive_5_2,MenuNodePassive_5_3;
Menu_Node MenuNodeInput_50,MenuNodeInput_51,MenuNodeInput_52,MenuNodeInput_53;

Menu_Node MenuNodePassive_6_0,MenuNodePassive_6_1,MenuNodePassive_6_2,MenuNodePassive_6_3;
Menu_Node MenuNodeInput_60,MenuNodeInput_61,MenuNodeInput_62,MenuNodeInput_63;

/*............................................
 *
 * 		Définitions des structures param
 * 		privée au module
 *
.............................................*/

// Voltage
Menu_ParamValTypedef VoltageParam;
Menu_ParamValTypedef Param_10;
Menu_ParamValTypedef Param_20,Param_21;
Menu_ParamValTypedef Param_30,Param_31,Param_32;
Menu_ParamValTypedef Param_40,Param_41,Param_42,Param_43;
Menu_ParamValTypedef Param_50,Param_51,Param_52,Param_53;
Menu_ParamValTypedef Param_60,Param_61,Param_62,Param_63;

/*............................................
 *
 * 		Chargement des maillons MenuNodes
 *      = CONSTRUCTION DES DIVERS MENUS
 *
 *		chargement des param
 *		pour les menus saisie
.............................................*/




void Menu_Node_InitDefinition(void)
{


	/*............................................
	 *
	 * 		Chargement des maillons MenuNodes
	 *      = CONSTRUCTION DES DIVERS MENUS
	.............................................*/



	/*      **********************
	 * 		*Menu principal      *
	 * 		* Nenu 1             *
	 * 		* Nenu 2             *
	 * 		* Nenu 3             *
	 * 		* Nenu 4             *
	 * 		**********************
	 */
	MenuNodePassive_1.DownNode=&MenuNodePassive_2;
	MenuNodePassive_1.NextNode=&MenuNodePassive_1_1;
	MenuNodePassive_1.UpNode=&MenuNodePassive_1;  // lui même
	MenuNodePassive_1.PreviousNode=&MenuNodePassive_1;  // lui même
	MenuNodePassive_1.MenuPosition=First;
	MenuNodePassive_1.Title="Menu principal      ";
	MenuNodePassive_1.ItemToPrint =" Nenu 1             ";
	MenuNodePassive_1.NodeExecFct=Menu_PassivePrint;
	MenuNodePassive_1.Param=&ParamNull; //------------------------------------------------------------------

	MenuNodePassive_2.DownNode=&MenuNodePassive_3;
	MenuNodePassive_2.NextNode=&MenuNodePassive_2_1;
	MenuNodePassive_2.UpNode=&MenuNodePassive_1;
	MenuNodePassive_2.PreviousNode=&MenuNodePassive_2;  // lui même
	MenuNodePassive_2.MenuPosition=Middle;
	MenuNodePassive_2.Title="Menu principal      ";
	MenuNodePassive_2.ItemToPrint =" Nenu 2             ";
	MenuNodePassive_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3.DownNode=&MenuNodePassive_4;
	MenuNodePassive_3.NextNode=&MenuNodePassive_3_1;
	MenuNodePassive_3.UpNode=&MenuNodePassive_2;
	MenuNodePassive_3.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3.MenuPosition=Middle;
	MenuNodePassive_3.Title="Menu principal      ";
	MenuNodePassive_3.ItemToPrint =" Nenu 3             ";
	MenuNodePassive_3.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_4.DownNode=&MenuNodePassive_5;
	MenuNodePassive_4.NextNode=&MenuNodePassive_4_0;
	MenuNodePassive_4.UpNode=&MenuNodePassive_3;
	MenuNodePassive_4.PreviousNode=&MenuNodePassive_4;
	MenuNodePassive_4.MenuPosition=Middle;
	MenuNodePassive_4.Title="Menu principal      ";
	MenuNodePassive_4.ItemToPrint =" Nenu 4             ";
	MenuNodePassive_4.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_5.UpNode=&MenuNodePassive_4;
	MenuNodePassive_5.NextNode=&MenuNodePassive_5_0;
	MenuNodePassive_5.DownNode=&MenuNodePassive_6;
	MenuNodePassive_5.PreviousNode=&MenuNodePassive_5;
	MenuNodePassive_5.MenuPosition=Middle;
	MenuNodePassive_5.Title="Menu principal      ";
	MenuNodePassive_5.ItemToPrint =" Nenu 5             ";
	MenuNodePassive_5.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_6.UpNode=&MenuNodePassive_5;
	MenuNodePassive_6.NextNode=&MenuNodePassive_6_0;
	MenuNodePassive_6.DownNode=&MenuNodePassive_6;
	MenuNodePassive_6.PreviousNode=&MenuNodePassive_6;
	MenuNodePassive_6.MenuPosition=Last;
	MenuNodePassive_6.Title="Menu principal      ";
	MenuNodePassive_6.ItemToPrint =" Nenu 6             ";
	MenuNodePassive_6.NodeExecFct=Menu_PassivePrint;

	/*      **********************
	 * 		*Menu 1...           *
	 * 		* Nenu 1_1           *
	 * 		* Nenu 1_2           *
	 * 		**********************
	 */

	MenuNodePassive_1_1.DownNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_1.NextNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_1.UpNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_1.PreviousNode=&MenuNodePassive_1;
	MenuNodePassive_1_1.MenuPosition=First;
	MenuNodePassive_1_1.Title="Menu 1...           ";
	MenuNodePassive_1_1.ItemToPrint =" Nenu 1_1           ";
	MenuNodePassive_1_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_1_2.DownNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_2.NextNode=&MenuNodePassive_1_2;
	MenuNodePassive_1_2.UpNode=&MenuNodePassive_1_1;
	MenuNodePassive_1_2.PreviousNode=&MenuNodePassive_1;
	MenuNodePassive_1_2.MenuPosition=Last;
	MenuNodePassive_1_2.Title="Menu 1...           ";
	MenuNodePassive_1_2.ItemToPrint =" Nenu 1_2           ";
	MenuNodePassive_1_2.NodeExecFct=Menu_PassivePrint;



	/*      **********************
	 * 		*Menu 2...           *
	 * 		* Nenu 2_1           *
	 * 		**********************
	 */
	MenuNodePassive_2_1.DownNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.NextNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.UpNode=&MenuNodePassive_2_1;
	MenuNodePassive_2_1.PreviousNode=&MenuNodePassive_2;
	MenuNodePassive_2_1.MenuPosition=First;
	MenuNodePassive_2_1.Title="Menu 2...           ";
	MenuNodePassive_2_1.ItemToPrint =" Nenu 2_1           ";
	MenuNodePassive_2_1.NodeExecFct=Menu_PassivePrint;



	/*      **********************
	 * 		*Menu 3...           *
	 * 		* Voltage Param ...  *
	 * 		* Nenu 3_2           *
	 * 		* Nenu 3_3           *
	 * 		* Nenu 3_4           *
	 * 		* Nenu 3_5           *
	 * 		* Nenu 3_6           *
	 * 		* Nenu 3_7           *
	 * 		**********************
	 */
	MenuNodePassive_3_1.DownNode=&MenuNodePassive_3_2;
	MenuNodePassive_3_1.NextNode=&MenuNodeInput_VoltParam;
	MenuNodePassive_3_1.UpNode=&MenuNodePassive_3_1;
	MenuNodePassive_3_1.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_1.MenuPosition=First;
	MenuNodePassive_3_1.Title="Menu 3...           ";
	MenuNodePassive_3_1.ItemToPrint =" Nenu 3_1           ";
	MenuNodePassive_3_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_2.DownNode=&MenuNodePassive_3_3;
	MenuNodePassive_3_2.NextNode=&MenuNodeInput_10;
	MenuNodePassive_3_2.UpNode=&MenuNodePassive_3_1;
	MenuNodePassive_3_2.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_2.MenuPosition=Middle;
	MenuNodePassive_3_2.Title="Menu 3...           ";
	MenuNodePassive_3_2.ItemToPrint =" Nenu 3_2           ";
	MenuNodePassive_3_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_3.DownNode=&MenuNodePassive_3_4;
	MenuNodePassive_3_3.NextNode=&MenuNodeInput_20;
	MenuNodePassive_3_3.UpNode=&MenuNodePassive_3_2;
	MenuNodePassive_3_3.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_3.MenuPosition=Middle;
	MenuNodePassive_3_3.Title="Menu 3...           ";
	MenuNodePassive_3_3.ItemToPrint =" Nenu 3_3           ";
	MenuNodePassive_3_3.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_4.DownNode=&MenuNodePassive_3_5;
	MenuNodePassive_3_4.NextNode=&MenuNodeInput_21;
	MenuNodePassive_3_4.UpNode=&MenuNodePassive_3_3;
	MenuNodePassive_3_4.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_4.MenuPosition=Middle;
	MenuNodePassive_3_4.Title="Menu 3...           ";
	MenuNodePassive_3_4.ItemToPrint =" Nenu 3_4           ";
	MenuNodePassive_3_4.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_5.DownNode=&MenuNodePassive_3_6;
	MenuNodePassive_3_5.NextNode=&MenuNodeInput_30;
	MenuNodePassive_3_5.UpNode=&MenuNodePassive_3_4;
	MenuNodePassive_3_5.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_5.MenuPosition=Middle;
	MenuNodePassive_3_5.Title="Menu 3...           ";
	MenuNodePassive_3_5.ItemToPrint =" Nenu 3_5           ";
	MenuNodePassive_3_5.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_3_6.DownNode=&MenuNodePassive_3_7;
	MenuNodePassive_3_6.NextNode=&MenuNodeInput_31;
	MenuNodePassive_3_6.UpNode=&MenuNodePassive_3_5;
	MenuNodePassive_3_6.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_6.MenuPosition=Middle;
	MenuNodePassive_3_6.Title="Menu 3...           ";
	MenuNodePassive_3_6.ItemToPrint =" Nenu 3_6           ";
	MenuNodePassive_3_6.NodeExecFct=Menu_PassivePrint;

	// procéder dans cet ordre c'est plus logique :
	MenuNodePassive_3_7.UpNode=&MenuNodePassive_3_6;
	MenuNodePassive_3_7.NextNode=&MenuNodeInput_32;
	MenuNodePassive_3_7.DownNode=&MenuNodePassive_3_7;
	MenuNodePassive_3_7.PreviousNode=&MenuNodePassive_3;
	MenuNodePassive_3_7.MenuPosition=Last;
	MenuNodePassive_3_7.Title="Menu 3...           ";
	MenuNodePassive_3_7.ItemToPrint =" Nenu 3_7           ";
	MenuNodePassive_3_7.NodeExecFct=Menu_PassivePrint;


	/*      **********************
	 * 		*Volt Param [V]...   *
	 * 		* Min :				 *
	 * 		* Max :	             *
	 * 		* Actual :           *
	 * 		**********************
	 */

	// procéder dans cet ordre c'est plus logique :
	MenuNodeInput_VoltParam.UpNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.NextNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.DownNode=&MenuNodeInput_VoltParam;
	MenuNodeInput_VoltParam.PreviousNode=&MenuNodePassive_3_1;
	MenuNodeInput_VoltParam.MenuPosition=Last;
	MenuNodeInput_VoltParam.Title="Volt Param [V]...   ";
	MenuNodeInput_VoltParam.ItemToPrint =" Actual :           ";
	MenuNodeInput_VoltParam.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_VoltParam.Param=&VoltageParam;


	/*............................................
	 *		chargement des param
	 *		pour les menus saisie
	.............................................*/
	VoltageParam.Val=12.5;
	VoltageParam.MinVal=12.0;
	VoltageParam.MaxVal=13.0;
	VoltageParam.NewUserInputFlag=0;
	VoltageParam.DigitNb=4;
	VoltageParam.DecimalNb=2;

//===================================================================================================
// ========================== TESTS FORMATS FRACT ===================================================
//===================================================================================================

	/*      **********************
	 * 		*Volt Param 10...    *
	 * 		* Min :				 *
	 * 		* Max :	             *
	 * 		* Actual :           *
	 * 		**********************
	 */

	MenuNodeInput_10.UpNode=&MenuNodeInput_10;
	MenuNodeInput_10.NextNode=&MenuNodeInput_10;
	MenuNodeInput_10.DownNode=&MenuNodeInput_10;
	MenuNodeInput_10.PreviousNode=&MenuNodePassive_3_2;
	MenuNodeInput_10.MenuPosition=Last;
	MenuNodeInput_10.Title="Volt Param 10...    ";
	MenuNodeInput_10.ItemToPrint =" Actual :           ";
	MenuNodeInput_10.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_10.Param=&Param_10;

	Param_10.Val=1.5;
	Param_10.MinVal=1.0;
	Param_10.MaxVal=3.0;
	Param_10.NewUserInputFlag=0;
	Param_10.DigitNb=1;
	Param_10.DecimalNb=0;

	//===================20=====================================
	MenuNodeInput_20.UpNode=&MenuNodeInput_20;
	MenuNodeInput_20.NextNode=&MenuNodeInput_20;
	MenuNodeInput_20.DownNode=&MenuNodeInput_20;
	MenuNodeInput_20.PreviousNode=&MenuNodePassive_3_3;
	MenuNodeInput_20.MenuPosition=Last;
	MenuNodeInput_20.Title="Volt Param 20...    ";
	MenuNodeInput_20.ItemToPrint =" Actual :           ";
	MenuNodeInput_20.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_20.Param=&Param_20;

	Param_20.Val=10.0;
	Param_20.MinVal=5.0;
	Param_20.MaxVal=99.0;
	Param_20.NewUserInputFlag=0;
	Param_20.DigitNb=2;
	Param_20.DecimalNb=0;

	//=====================21===================================
	MenuNodeInput_21.UpNode=&MenuNodeInput_21;
	MenuNodeInput_21.NextNode=&MenuNodeInput_21;
	MenuNodeInput_21.DownNode=&MenuNodeInput_21;
	MenuNodeInput_21.PreviousNode=&MenuNodePassive_3_4;
	MenuNodeInput_21.MenuPosition=Last;
	MenuNodeInput_21.Title="Volt Param 21...    ";
	MenuNodeInput_21.ItemToPrint =" Actual :           ";
	MenuNodeInput_21.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_21.Param=&Param_21;

	Param_21.Val=1.5;
	Param_21.MinVal=0;
	Param_21.MaxVal=9.0;
	Param_21.NewUserInputFlag=0;
	Param_21.DigitNb=2;
	Param_21.DecimalNb=1;

	//=====================30===================================
	MenuNodeInput_30.UpNode=&MenuNodeInput_30;
	MenuNodeInput_30.NextNode=&MenuNodeInput_30;
	MenuNodeInput_30.DownNode=&MenuNodeInput_30;
	MenuNodeInput_30.PreviousNode=&MenuNodePassive_3_5;
	MenuNodeInput_30.MenuPosition=Last;
	MenuNodeInput_30.Title="Volt Param 30...    ";
	MenuNodeInput_30.ItemToPrint =" Actual :           ";
	MenuNodeInput_30.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_30.Param=&Param_30;

	Param_30.Val=150;
	Param_30.MinVal=-100;
	Param_30.MaxVal=500;
	Param_30.NewUserInputFlag=0;
	Param_30.DigitNb=3;
	Param_30.DecimalNb=0;

	//=====================31===================================
	MenuNodeInput_31.UpNode=&MenuNodeInput_31;
	MenuNodeInput_31.NextNode=&MenuNodeInput_31;
	MenuNodeInput_31.DownNode=&MenuNodeInput_31;
	MenuNodeInput_31.PreviousNode=&MenuNodePassive_3_6;
	MenuNodeInput_31.MenuPosition=Last;
	MenuNodeInput_31.Title="Volt Param 31...    ";
	MenuNodeInput_31.ItemToPrint =" Actual :           ";
	MenuNodeInput_31.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_31.Param=&Param_31;

	Param_31.Val=-20.0;
	Param_31.MinVal=-50.0;
	Param_31.MaxVal=99.0;
	Param_31.NewUserInputFlag=0;
	Param_31.DigitNb=3;
	Param_31.DecimalNb=1;

	//=====================32===================================
	MenuNodeInput_32.UpNode=&MenuNodeInput_32;
	MenuNodeInput_32.NextNode=&MenuNodeInput_32;
	MenuNodeInput_32.DownNode=&MenuNodeInput_32;
	MenuNodeInput_32.PreviousNode=&MenuNodePassive_3_7;
	MenuNodeInput_32.MenuPosition=Last;
	MenuNodeInput_32.Title="Volt Param 32...    ";
	MenuNodeInput_32.ItemToPrint =" Actual :           ";
	MenuNodeInput_32.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_32.Param=&Param_32;

	Param_32.Val=-9.0;
	Param_32.MinVal=0.0;
	Param_32.MaxVal=5.55;
	Param_32.NewUserInputFlag=0;
	Param_32.DigitNb=3;
	Param_32.DecimalNb=2;



	/*      **********************
	 * 		*Menu 4...           *
	 * 		* Param 4.0          *
	 * 		* Param 4.1          *
	 * 		* Param 4.2          *
	 * 		* Param 4.3          *
	 * 		**********************
	 */
	MenuNodePassive_4_0.UpNode=&MenuNodePassive_4_0;
	MenuNodePassive_4_0.NextNode=&MenuNodeInput_40;
	MenuNodePassive_4_0.DownNode=&MenuNodePassive_4_1;
	MenuNodePassive_4_0.PreviousNode=&MenuNodePassive_4;
	MenuNodePassive_4_0.MenuPosition=First;
	MenuNodePassive_4_0.Title="Menu 4...           ";
	MenuNodePassive_4_0.ItemToPrint =" Param 4.0          ";
	MenuNodePassive_4_0.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_4_1.UpNode=&MenuNodePassive_4_0;
	MenuNodePassive_4_1.NextNode=&MenuNodeInput_41;
	MenuNodePassive_4_1.DownNode=&MenuNodePassive_4_2;
	MenuNodePassive_4_1.PreviousNode=&MenuNodePassive_4;
	MenuNodePassive_4_1.MenuPosition=Middle;
	MenuNodePassive_4_1.Title="Menu 4...           ";
	MenuNodePassive_4_1.ItemToPrint =" Param 4.1          ";
	MenuNodePassive_4_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_4_2.UpNode=&MenuNodePassive_4_1;
	MenuNodePassive_4_2.NextNode=&MenuNodeInput_42;
	MenuNodePassive_4_2.DownNode=&MenuNodePassive_4_3;
	MenuNodePassive_4_2.PreviousNode=&MenuNodePassive_4;
	MenuNodePassive_4_2.MenuPosition=Middle;
	MenuNodePassive_4_2.Title="Menu 4...           ";
	MenuNodePassive_4_2.ItemToPrint =" Param 4.2          ";
	MenuNodePassive_4_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_4_3.UpNode=&MenuNodePassive_4_2;
	MenuNodePassive_4_3.NextNode=&MenuNodeInput_43;
	MenuNodePassive_4_3.DownNode=&MenuNodePassive_4_3;
	MenuNodePassive_4_3.PreviousNode=&MenuNodePassive_4;
	MenuNodePassive_4_3.MenuPosition=Last;
	MenuNodePassive_4_3.Title="Menu 4...           ";
	MenuNodePassive_4_3.ItemToPrint =" Param 4.3          ";
	MenuNodePassive_4_3.NodeExecFct=Menu_PassivePrint;



	//=====================40===================================
	MenuNodeInput_40.UpNode=&MenuNodeInput_40;
	MenuNodeInput_40.NextNode=&MenuNodeInput_40;
	MenuNodeInput_40.DownNode=&MenuNodeInput_40;
	MenuNodeInput_40.PreviousNode=&MenuNodePassive_4_0;
	MenuNodeInput_40.MenuPosition=Last;
	MenuNodeInput_40.Title="Volt Param 40...    ";
	MenuNodeInput_40.ItemToPrint =" Actual :           ";
	MenuNodeInput_40.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_40.Param=&Param_40;

	Param_40.Val=1000;
	Param_40.MinVal=-2000;
	Param_40.MaxVal=9000;
	Param_40.NewUserInputFlag=0;
	Param_40.DigitNb=4;
	Param_40.DecimalNb=0;

	//=====================41===================================
	MenuNodeInput_41.UpNode=&MenuNodeInput_41;
	MenuNodeInput_41.NextNode=&MenuNodeInput_41;
	MenuNodeInput_41.DownNode=&MenuNodeInput_41;
	MenuNodeInput_41.PreviousNode=&MenuNodePassive_4_1;
	MenuNodeInput_41.MenuPosition=Last;
	MenuNodeInput_41.Title="Volt Param 41...    ";
	MenuNodeInput_41.ItemToPrint =" Actual :           ";
	MenuNodeInput_41.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_41.Param=&Param_41;

	Param_41.Val=100.0;
	Param_41.MinVal=-200.0;
	Param_41.MaxVal=900.0;
	Param_41.NewUserInputFlag=0;
	Param_41.DigitNb=4;
	Param_41.DecimalNb=1;

	//=====================42===================================
	MenuNodeInput_42.UpNode=&MenuNodeInput_42;
	MenuNodeInput_42.NextNode=&MenuNodeInput_42;
	MenuNodeInput_42.DownNode=&MenuNodeInput_42;
	MenuNodeInput_42.PreviousNode=&MenuNodePassive_4_2;
	MenuNodeInput_42.MenuPosition=Last;
	MenuNodeInput_42.Title="Volt Param 42...    ";
	MenuNodeInput_42.ItemToPrint =" Actual :           ";
	MenuNodeInput_42.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_42.Param=&Param_42;

	Param_42.Val=10;
	Param_42.MinVal=-20;
	Param_42.MaxVal=90.00;
	Param_42.NewUserInputFlag=0;
	Param_42.DigitNb=4;
	Param_42.DecimalNb=2;

	//=====================43===================================
	MenuNodeInput_43.UpNode=&MenuNodeInput_43;
	MenuNodeInput_43.NextNode=&MenuNodeInput_43;
	MenuNodeInput_43.DownNode=&MenuNodeInput_43;
	MenuNodeInput_43.PreviousNode=&MenuNodePassive_4_3;
	MenuNodeInput_43.MenuPosition=Last;
	MenuNodeInput_43.Title="Volt Param 43...    ";
	MenuNodeInput_43.ItemToPrint =" Actual :           ";
	MenuNodeInput_43.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_43.Param=&Param_43;

	Param_43.Val=1.000;
	Param_43.MinVal=-9;
	Param_43.MaxVal=9.0;
	Param_43.NewUserInputFlag=0;
	Param_43.DigitNb=4;
	Param_43.DecimalNb=3;

	/*      **********************
	 * 		*Menu 5...           *
	 * 		* Param 5.0          *
	 * 		* Param 5.1          *
	 * 		* Param 5.2          *
	 * 		* Param 5.3          *
	 * 		**********************
	 */
	MenuNodePassive_5_0.UpNode=&MenuNodePassive_5_0;
	MenuNodePassive_5_0.NextNode=&MenuNodeInput_50;
	MenuNodePassive_5_0.DownNode=&MenuNodePassive_5_1;
	MenuNodePassive_5_0.PreviousNode=&MenuNodePassive_5;
	MenuNodePassive_5_0.MenuPosition=First;
	MenuNodePassive_5_0.Title="Menu 5...           ";
	MenuNodePassive_5_0.ItemToPrint =" Param 5.0          ";
	MenuNodePassive_5_0.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_5_1.UpNode=&MenuNodePassive_5_0;
	MenuNodePassive_5_1.NextNode=&MenuNodeInput_51;
	MenuNodePassive_5_1.DownNode=&MenuNodePassive_5_2;
	MenuNodePassive_5_1.PreviousNode=&MenuNodePassive_5;
	MenuNodePassive_5_1.MenuPosition=Middle;
	MenuNodePassive_5_1.Title="Menu 5...           ";
	MenuNodePassive_5_1.ItemToPrint =" Param 5.1          ";
	MenuNodePassive_5_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_5_2.UpNode=&MenuNodePassive_5_1;
	MenuNodePassive_5_2.NextNode=&MenuNodeInput_52;
	MenuNodePassive_5_2.DownNode=&MenuNodePassive_5_3;
	MenuNodePassive_5_2.PreviousNode=&MenuNodePassive_5;
	MenuNodePassive_5_2.MenuPosition=Middle;
	MenuNodePassive_5_2.Title="Menu 5...           ";
	MenuNodePassive_5_2.ItemToPrint =" Param 5.2          ";
	MenuNodePassive_5_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_5_3.UpNode=&MenuNodePassive_5_2;
	MenuNodePassive_5_3.NextNode=&MenuNodeInput_53;
	MenuNodePassive_5_3.DownNode=&MenuNodePassive_5_3;
	MenuNodePassive_5_3.PreviousNode=&MenuNodePassive_5;
	MenuNodePassive_5_3.MenuPosition=Last;
	MenuNodePassive_5_3.Title="Menu 5...           ";
	MenuNodePassive_5_3.ItemToPrint =" Param 5.3          ";
	MenuNodePassive_5_3.NodeExecFct=Menu_PassivePrint;

	//=====================50===================================
	MenuNodeInput_50.UpNode=&MenuNodeInput_50;
	MenuNodeInput_50.NextNode=&MenuNodeInput_50;
	MenuNodeInput_50.DownNode=&MenuNodeInput_50;
	MenuNodeInput_50.PreviousNode=&MenuNodePassive_5_0;
	MenuNodeInput_50.MenuPosition=Last;
	MenuNodeInput_50.Title="Volt Param 50...    ";
	MenuNodeInput_50.ItemToPrint =" Actual :           ";
	MenuNodeInput_50.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_50.Param=&Param_50;

	Param_50.Val=1000;
	Param_50.MinVal=-2000;
	Param_50.MaxVal=9000;
	Param_50.NewUserInputFlag=0;
	Param_50.DigitNb=5;
	Param_50.DecimalNb=0;

	//=====================51===================================
	MenuNodeInput_51.UpNode=&MenuNodeInput_51;
	MenuNodeInput_51.NextNode=&MenuNodeInput_51;
	MenuNodeInput_51.DownNode=&MenuNodeInput_51;
	MenuNodeInput_51.PreviousNode=&MenuNodePassive_5_1;
	MenuNodeInput_51.MenuPosition=Last;
	MenuNodeInput_51.Title="Volt Param 51...    ";
	MenuNodeInput_51.ItemToPrint =" Actual :           ";
	MenuNodeInput_51.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_51.Param=&Param_51;

	Param_51.Val=100.0;
	Param_51.MinVal=-200.0;
	Param_51.MaxVal=900.0;
	Param_51.NewUserInputFlag=0;
	Param_51.DigitNb=5;
	Param_51.DecimalNb=1;
	//=====================52===================================
	MenuNodeInput_52.UpNode=&MenuNodeInput_52;
	MenuNodeInput_52.NextNode=&MenuNodeInput_52;
	MenuNodeInput_52.DownNode=&MenuNodeInput_52;
	MenuNodeInput_52.PreviousNode=&MenuNodePassive_5_2;
	MenuNodeInput_52.MenuPosition=Last;
	MenuNodeInput_52.Title="Volt Param 52...    ";
	MenuNodeInput_52.ItemToPrint =" Actual :           ";
	MenuNodeInput_52.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_52.Param=&Param_52;

	Param_52.Val=10;
	Param_52.MinVal=-20;
	Param_52.MaxVal=90.00;
	Param_52.NewUserInputFlag=0;
	Param_52.DigitNb=5;
	Param_52.DecimalNb=2;
	//=====================53===================================
	MenuNodeInput_53.UpNode=&MenuNodeInput_53;
	MenuNodeInput_53.NextNode=&MenuNodeInput_53;
	MenuNodeInput_53.DownNode=&MenuNodeInput_53;
	MenuNodeInput_53.PreviousNode=&MenuNodePassive_5_3;
	MenuNodeInput_53.MenuPosition=Last;
	MenuNodeInput_53.Title="Volt Param 53...    ";
	MenuNodeInput_53.ItemToPrint =" Actual :           ";
	MenuNodeInput_53.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_53.Param=&Param_53;
	Param_53.Val=1.000;
	Param_53.MinVal=-9;
	Param_53.MaxVal=9.0;
	Param_53.NewUserInputFlag=0;
	Param_53.DigitNb=5;
	Param_53.DecimalNb=3;


	/*      **********************
	 * 		*Menu 6...           *
	 * 		* Param 6.0          *
	 * 		* Param 6.1          *
	 * 		* Param 6.2          *
	 * 		* Param 6.3          *
	 * 		**********************
	 */
	MenuNodePassive_6_0.UpNode=&MenuNodePassive_6_0;
	MenuNodePassive_6_0.NextNode=&MenuNodeInput_60;
	MenuNodePassive_6_0.DownNode=&MenuNodePassive_6_1;
	MenuNodePassive_6_0.PreviousNode=&MenuNodePassive_6;
	MenuNodePassive_6_0.MenuPosition=First;
	MenuNodePassive_6_0.Title="Menu 6...           ";
	MenuNodePassive_6_0.ItemToPrint =" Param 6.0          ";
	MenuNodePassive_6_0.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_6_1.UpNode=&MenuNodePassive_6_0;
	MenuNodePassive_6_1.NextNode=&MenuNodeInput_61;
	MenuNodePassive_6_1.DownNode=&MenuNodePassive_6_2;
	MenuNodePassive_6_1.PreviousNode=&MenuNodePassive_6;
	MenuNodePassive_6_1.MenuPosition=Middle;
	MenuNodePassive_6_1.Title="Menu 6...           ";
	MenuNodePassive_6_1.ItemToPrint =" Param 6.1          ";
	MenuNodePassive_6_1.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_6_2.UpNode=&MenuNodePassive_6_1;
	MenuNodePassive_6_2.NextNode=&MenuNodeInput_62;
	MenuNodePassive_6_2.DownNode=&MenuNodePassive_6_3;
	MenuNodePassive_6_2.PreviousNode=&MenuNodePassive_6;
	MenuNodePassive_6_2.MenuPosition=Middle;
	MenuNodePassive_6_2.Title="Menu 6...           ";
	MenuNodePassive_6_2.ItemToPrint =" Param 6.2          ";
	MenuNodePassive_6_2.NodeExecFct=Menu_PassivePrint;

	MenuNodePassive_6_3.UpNode=&MenuNodePassive_6_2;
	MenuNodePassive_6_3.NextNode=&MenuNodeInput_63;
	MenuNodePassive_6_3.DownNode=&MenuNodePassive_6_3;
	MenuNodePassive_6_3.PreviousNode=&MenuNodePassive_6;
	MenuNodePassive_6_3.MenuPosition=Last;
	MenuNodePassive_6_3.Title="Menu 6...           ";
	MenuNodePassive_6_3.ItemToPrint =" Param 6.3          ";
	MenuNodePassive_6_3.NodeExecFct=Menu_PassivePrint;

	//=====================60===================================
	MenuNodeInput_60.UpNode=&MenuNodeInput_60;
	MenuNodeInput_60.NextNode=&MenuNodeInput_60;
	MenuNodeInput_60.DownNode=&MenuNodeInput_60;
	MenuNodeInput_60.PreviousNode=&MenuNodePassive_6_0;
	MenuNodeInput_60.MenuPosition=Last;
	MenuNodeInput_60.Title="Volt Param 60...    ";
	MenuNodeInput_60.ItemToPrint =" Actual :           ";
	MenuNodeInput_60.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_60.Param=&Param_60;

	Param_60.Val=100000;
	Param_60.MinVal=-900000;
	Param_60.MaxVal=999999;
	Param_60.NewUserInputFlag=0;
	Param_60.DigitNb=6;
	Param_60.DecimalNb=0;

	//=====================61===================================
	MenuNodeInput_61.UpNode=&MenuNodeInput_61;
	MenuNodeInput_61.NextNode=&MenuNodeInput_61;
	MenuNodeInput_61.DownNode=&MenuNodeInput_61;
	MenuNodeInput_61.PreviousNode=&MenuNodePassive_6_1;
	MenuNodeInput_61.MenuPosition=Last;
	MenuNodeInput_61.Title="Volt Param 61...    ";
	MenuNodeInput_61.ItemToPrint =" Actual :           ";
	MenuNodeInput_61.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_61.Param=&Param_61;

	Param_61.Val=50000;
	Param_61.MinVal=-80000;
	Param_61.MaxVal=60000;
	Param_61.NewUserInputFlag=0;
	Param_61.DigitNb=6;
	Param_61.DecimalNb=1;
	//=====================62===================================
	MenuNodeInput_62.UpNode=&MenuNodeInput_62;
	MenuNodeInput_62.NextNode=&MenuNodeInput_62;
	MenuNodeInput_62.DownNode=&MenuNodeInput_62;
	MenuNodeInput_62.PreviousNode=&MenuNodePassive_6_2;
	MenuNodeInput_62.MenuPosition=Last;
	MenuNodeInput_62.Title="Volt Param 62...    ";
	MenuNodeInput_62.ItemToPrint =" Actual :           ";
	MenuNodeInput_62.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_62.Param=&Param_62;

	Param_62.Val=-2000;
	Param_62.MinVal=-5000;
	Param_62.MaxVal=-1000;
	Param_62.NewUserInputFlag=0;
	Param_62.DigitNb=6;
	Param_62.DecimalNb=2;
	//=====================63===================================
	MenuNodeInput_63.UpNode=&MenuNodeInput_63;
	MenuNodeInput_63.NextNode=&MenuNodeInput_63;
	MenuNodeInput_63.DownNode=&MenuNodeInput_63;
	MenuNodeInput_63.PreviousNode=&MenuNodePassive_6_3;
	MenuNodeInput_63.MenuPosition=Last;
	MenuNodeInput_63.Title="Volt Param 63...    ";
	MenuNodeInput_63.ItemToPrint =" Actual :           ";
	MenuNodeInput_63.NodeExecFct=Menu_InputStateMachine;
	MenuNodeInput_63.Param=&Param_63;
	Param_63.Val=200;
	Param_63.MinVal=-900;
	Param_63.MaxVal=900;
	Param_63.NewUserInputFlag=0;
	Param_63.DigitNb=6;
	Param_63.DecimalNb=3;


}




/*==========================================================================================
 *******************************************************************************************
 ******** 		USER ZONE END        *******************************************************
 *******************************************************************************************
 ==========================================================================================*/









/*==========================================================================================
 *******************************************************************************************
 ******** 		GESTION MENU PARTIE UART         *********************************************
 *******************************************************************************************
 ==========================================================================================*/

/******************************************************************************
*  Les commandes (code ASCII touches clavier)
*
******************************************************************************/

// pour les droitiers
#define Uart_Up_R 'o'
#define Uart_Down_R 'l'
#define Uart_Left_R 'k'
#define Uart_Right_R 'm'
// pour les gauchers
#define Uart_Up_L 'z'
#define Uart_Down_L 's'
#define Uart_Left_L 'q'
#define Uart_Right_L 'd'

/**
  * @brief Variable d'état du module
  * Cmde, RecepFlag, TransmFlag accessibles
  **/
struct {
	char Cmde;      // le caractère filtré
	char CurrentByte; // caractère effectivement reçu non filtré
	char RecepFlag;  // indique qu'une commande est arrivée
}MenuIncoming_Status;


/******************************************************************************
*  Callback UART, Filtrage des commandes
*
******************************************************************************/
void Menu_NodeUpdate(void);
void Callback_UART(void)
{
	char Byte;
	Byte=USART_GetByte(Menu_UART);
	MenuIncoming_Status.CurrentByte=Byte;
	switch (Byte)
	{
	case Uart_Up_R:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Up_L:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Down_R:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Down_L:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Left_R:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Left_L:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Right_R:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	case Uart_Right_L:MenuIncoming_Status.Cmde=Byte;MenuIncoming_Status.RecepFlag=1;break;
	default : MenuIncoming_Status.Cmde=0;

	}

	if (MenuIncoming_Status.RecepFlag==1)
		{
		MenuIncoming_Status.RecepFlag=0;
		Menu_NodeUpdate();
		}
}






/*==========================================================================================
 *******************************************************************************************
 ******** 		GESTION MENU         *******************************************************
 *******************************************************************************************
 ==========================================================================================*/

/*------------------------------------------------
*  LA VARIABLE D ETAT DU MODULE
*   c'est elle qui permet de savoir à quel Item (node)
*   nous en sommes
-------------------------------------------------*/

typedef enum  {Menu_Up, Menu_Down, Menu_Left, Menu_Right } Menu_Cmd;
typedef enum  {Entry, SetValue, Discard, Confirm,Retry} InputNodeSt;
struct {
	Menu_Node *  ActualNode;
	Menu_Cmd Cmde;

	InputNodeSt State;
	Menu_Node * LinkPreviousNode;
	char StringInputValue[9]; // sign + max 6 digit + '.' + Null
	int CursorPos; // 0 sur le premier digit , 6 maxi.
} Menu_Status;







/******************************************************************************
*  PUBLIC  INIT FUNCTION

******************************************************************************/

void Menu_NodeUpdate(void);
void Menu_Init(void)
{
	/*************************************
	 * init attribut MenuStatus
	 *
	 *************************************/
	Menu_Status.ActualNode=&MenuNodePassive_1;


	//Initialisation des maillons de la châine
	Menu_Node_InitDefinition();
	// Initialisation ComUART en IT callback
	
	USART_Init(Menu_UART, Menu_UART_BdRate, Menu_UART_Prio_Callback,Callback_UART);
	// Init du ParamNull
	ParamNull.MaxVal=0.0;
	ParamNull.MinVal=0.0;
	ParamNull.Val=0.0;
	ParamNull.NewUserInputFlag=0;
	ParamNull.DigitNb=0;

	Menu_Status.State=Entry;
	Menu_NodeUpdate(); // tout premier écran sans que le user ait à appuyer sur une touche...
}


void Menu_LoadParam(int Param,  Menu_ParamValTypedef * ParamValues)
{
	//Menu_ParamTable[Param]=ParamValues;
}




/******************************************************************************
*  PRIVATE functions : FONCTION DE MISE A JOUR DES ITEM SUR RECEPTION CMDE
*  appelée en IT ou timer
******************************************************************************/

void Menu_NodeUpdate(void)
{
	char Touch;
	Touch=MenuIncoming_Status.Cmde;
	switch(Touch)
	{
	case Uart_Up_R:
		{
		Menu_Status.Cmde=Menu_Up;
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		break;
		}
	case Uart_Up_L:
		{
		Menu_Status.Cmde=Menu_Up;
		Menu_Status.ActualNode=Menu_Status.ActualNode->UpNode;
		break;
		}
	case Uart_Down_R:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Down_L:
		{
		Menu_Status.Cmde=Menu_Down;
		Menu_Status.ActualNode=Menu_Status.ActualNode->DownNode;
		break;
		}
	case Uart_Left_R:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Left_L:
		{
		Menu_Status.Cmde=Menu_Left;
		Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
		break;
		}
	case Uart_Right_R:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	case Uart_Right_L:
		{
		Menu_Status.Cmde=Menu_Right;
		Menu_Status.ActualNode=Menu_Status.ActualNode->NextNode;
		break;
		}
	}

	Menu_Status.ActualNode->NodeExecFct();
}




/******************************************************************************
*  FONCTION SPECIFIQUE MENU PASSIF
******************************************************************************/


void Menu_PassivePrint(void)
{

	USART_Print(Menu_UART,Menu_Status.ActualNode->Title, 20);
	USART_Print(Menu_UART,"\r\n", 2);
	switch (Menu_Status.ActualNode->MenuPosition)
	{
		case First:
		{
			//Ligne 2 Actual
			USART_Print(Menu_UART,">", 1);
			USART_Print(Menu_UART,Menu_Status.ActualNode->ItemToPrint,19);
			USART_Print(Menu_UART,"\r\n", 2);
			//Ligne 3 si...
			if (Menu_Status.ActualNode->DownNode!=Menu_Status.ActualNode) // existe un item en dessous
			{
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->DownNode->ItemToPrint,20);
				USART_Print(Menu_UART,"\r\n", 2);
			}
			else
			{
				USART_Print(Menu_UART,"                    ",20); //clr
				USART_Print(Menu_UART,"\r\n", 2);
			}
			//Ligne 4 si...
			if (Menu_Status.ActualNode->DownNode->DownNode!=Menu_Status.ActualNode->DownNode) // existe un item en dessous
			{
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->DownNode->DownNode->ItemToPrint,20);
				USART_Print(Menu_UART,"\r\n", 2);
			}
			else
			{
				USART_Print(Menu_UART,"                    ",20); //clr
				USART_Print(Menu_UART,"\r\n", 2);
			}
			break;
		}

		case Middle:
		{
			// il existe forcément un item au dessus et au dessous sinon il
			// serait First ou Last
			// ligne 2
			USART_Print(Menu_UART," ", 1);
			USART_Print(Menu_UART,Menu_Status.ActualNode->UpNode->ItemToPrint,20);
			USART_Print(Menu_UART,"\r\n", 2);
			// ligne 3 Acutal
			USART_Print(Menu_UART,">", 1);
			USART_Print(Menu_UART,Menu_Status.ActualNode->ItemToPrint,19);
			USART_Print(Menu_UART,"\r\n", 2);
			// ligne 4
			USART_Print(Menu_UART," ", 1);
			USART_Print(Menu_UART,Menu_Status.ActualNode->DownNode->ItemToPrint,20);
			USART_Print(Menu_UART,"\r\n", 2);
			break;
		}
		case Last:
		{
			// l'item précédent est soit Middle soit First.
			// La ligne 2 peut être écrite par le node n-2; la ligne 3 par le node n-1, la ligne 3 atuel
			// ou la ligne 2 est écrite par le node n-1, la ligne 2 par l'actuel, ligne 3 vide

			if (Menu_Status.ActualNode->UpNode->UpNode!=Menu_Status.ActualNode->UpNode)
			// l'item n-2 n'est pas un first
			{
				//ligne 2
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->UpNode->UpNode->ItemToPrint,20);
				USART_Print(Menu_UART,"\r\n", 2);
				// ligne 3
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				USART_Print(Menu_UART,"\r\n", 2);
				// ligne 4
				USART_Print(Menu_UART,">", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->ItemToPrint,19);
				USART_Print(Menu_UART,"\r\n", 2);

			}
			else // l'item n-2 n'existe pas
			{
				//ligne 2
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->UpNode->ItemToPrint,20);
				USART_Print(Menu_UART,"\r\n", 2);
				// ligne 3
				USART_Print(Menu_UART,">", 1);
				USART_Print(Menu_UART,Menu_Status.ActualNode->ItemToPrint,19);
				USART_Print(Menu_UART,"\r\n", 2);
				// ligne 4
				USART_Print(Menu_UART," ", 1);
				USART_Print(Menu_UART,"                    ",20); //clr
				USART_Print(Menu_UART,"\r\n", 2);
			}
			break;
		}

	}
}

/******************************************************************************
*  FONCTION SPECIFIQUE MENU TYPE SAISIE
******************************************************************************/
void MenuInput_PrintPlantageConv(void);
void MenuInput_PrintDiscard(void);
void MenuInput_PrintConfirm(void);
void MenuInput_PrintRetry(void);
void MenuInput_PrintParam(void);


void Menu_InputStateMachine(void)
{
	//char MyString[8];
	float Valeur;
	char DigitNb, DecNb;
	char Tamp;
	int PosVirgule;
	int PosMax;

	// chargement local de la valeur à traiter
	Valeur=Menu_Status.ActualNode->Param->Val;
	DigitNb=Menu_Status.ActualNode->Param->DigitNb;
	DecNb=Menu_Status.ActualNode->Param->DecimalNb;

	switch(Menu_Status.State)
	{

		case  Entry:
		{
			// prochaine étape, SetValue forcément
			Menu_Status.State=SetValue;
			// link back
			Menu_Status.LinkPreviousNode=Menu_Status.ActualNode->PreviousNode; // pour pouvoir revenir
            // blocage dans cet item
			Menu_Status.ActualNode->PreviousNode=Menu_Status.ActualNode;
			// prépa premier digit pour réglage
			Menu_Status.CursorPos=0;
			if (StringFct_Float2Str(Valeur,Menu_Status.StringInputValue, DigitNb, DecNb)==0) MenuInput_PrintPlantageConv();
			// affichage
			MenuInput_PrintParam();
			break;
		}

		case  SetValue:
		{
			// calcule position Virgule
			//|xx.xxx00| DigitNb=5, DecimalNb=2
			//|x.xx0000| DigitNb=3, DecimalNb=2
			//|x.000000| DigitNb=1, DecimalNb=0
			PosVirgule= 1+Menu_Status.ActualNode->Param->DigitNb-Menu_Status.ActualNode->Param->DecimalNb;
			PosMax=Menu_Status.ActualNode->Param->DigitNb+1;

			if (Menu_Status.Cmde==Menu_Up)
			{
				// update digit highlihted (curseur)
				if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--; // si on tombe sur le '.' ..
				Tamp=Menu_Status.StringInputValue[Menu_Status.CursorPos];
				if (Tamp==' ') Tamp='-';
				else if  (Tamp=='-') Tamp=' ';
				else
				{
					Tamp++;
					if (Tamp==0x3A) Tamp=0x30;
				}
				Menu_Status.StringInputValue[Menu_Status.CursorPos]=Tamp;
				// affichage
				MenuInput_PrintParam();
			}
			else if (Menu_Status.Cmde==Menu_Down)
			{
				if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--; // si on tombe sur le '.' ..
				Tamp=Menu_Status.StringInputValue[Menu_Status.CursorPos];
				if (Tamp==' ') Tamp='-';
				else if  (Tamp=='-') Tamp=' ';
				else
				{
					Tamp--;
					if (Tamp==0x2F) Tamp=0x39;
				}
				Menu_Status.StringInputValue[Menu_Status.CursorPos]=Tamp;
				// affichage
				MenuInput_PrintParam();
			}

			else if (Menu_Status.Cmde==Menu_Left) // Discard ? or Cursor --
			{
				if (Menu_Status.CursorPos==0) // Discard ?
				{
					Menu_Status.State=Discard;
					MenuInput_PrintDiscard();
				}
				else // cursor --
				{
					Menu_Status.CursorPos--;
					// gestion '.' que l'on saute si nécessaire
					if (Menu_Status.CursorPos==PosVirgule) Menu_Status.CursorPos--;
					// affichage
					MenuInput_PrintParam();
				}
			}
			else if (Menu_Status.Cmde==Menu_Right) // Confirm ? or Cursor ++
			{
				if (Menu_Status.CursorPos==PosMax) // Confirm ?
				{
					Menu_Status.State=Confirm;
					MenuInput_PrintConfirm();
				}
				else // cursor ++
				{
					Menu_Status.CursorPos++;
					// gestion '.' que l'on saute si nécessaire (uniquement si fract>0)
					if ((Menu_Status.CursorPos==PosVirgule)&&(DecNb>0)) Menu_Status.CursorPos++;
					// affichage
					MenuInput_PrintParam();
				}

			}
			break;
		}
		case Discard:
		{
			if (Menu_Status.Cmde==Menu_Left) // confirmation du Discard
			{
				// restitution pointeur back du menu actuel
				Menu_Status.ActualNode->PreviousNode=Menu_Status.LinkPreviousNode;
				Menu_Status.State=Entry;
				// basculement node père
				Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
				// Affichage menu passif
				Menu_PassivePrint();
			}
			else if (Menu_Status.Cmde==Menu_Right) // abandon du Discard
			{
				Menu_Status.State=SetValue;
				// affichage
				MenuInput_PrintParam();
			}
			break;
		}
		case Confirm:
		{
			if (Menu_Status.Cmde==Menu_Right) // confirmation de la validation
			{
				// écriture valeur dans param
				Valeur=StringFct_Str2Float(Menu_Status.StringInputValue,Menu_Status.ActualNode->Param->DigitNb,Menu_Status.ActualNode->Param->DecimalNb);
				if (Valeur>1000000.0) MenuInput_PrintPlantageConv();
				if ((Valeur<=Menu_Status.ActualNode->Param->MaxVal)&&(Valeur>=Menu_Status.ActualNode->Param->MinVal))
				{
					// la valeur est OK
					Menu_Status.ActualNode->Param->Val=Valeur;
					Menu_Status.ActualNode->Param->NewUserInputFlag=1;
					// restitution pointeur back du menu actuel
					Menu_Status.ActualNode->PreviousNode=Menu_Status.LinkPreviousNode;
					Menu_Status.State=Entry;
					// basculement node père
					Menu_Status.ActualNode=Menu_Status.ActualNode->PreviousNode;
					Menu_PassivePrint();
				}
				else // valeur KO
				{
					Menu_Status.State=Retry;
					MenuInput_PrintRetry();
				}

			}
			else if (Menu_Status.Cmde==Menu_Left) // abandon du Discard
			{
				Menu_Status.State=SetValue;
				// affichage
				MenuInput_PrintParam();
			}
			break;
		}
		case Retry:
		{
			// prochaine étape, SetValue forcément
			Menu_Status.State=SetValue;
			Menu_Status.CursorPos=0;
			MenuInput_PrintParam();
			break;
		}
	}

}






/*------------------------------------------------
*  Sous fonctions d'affichage
-------------------------------------------------*/


void MenuInput_PrintParam(void)  // var dynamique dans Menu_Status ...
{
	char MyString[9];
	// Line 1
	USART_Print(Menu_UART,Menu_Status.ActualNode->Title, 20);
	USART_Print(Menu_UART,"\r\n", 2);
	// Line 2
	USART_Print(Menu_UART," Min : ", 7);
	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MinVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;
//	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MinVal,MyString, 5, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;


	USART_Print(Menu_UART,MyString, Menu_Status.ActualNode->Param->DigitNb +2);
	USART_Print(Menu_UART,"\r\n", 2);
	// Line 3
	USART_Print(Menu_UART," Max : ", 7);
	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MaxVal,MyString, Menu_Status.ActualNode->Param->DigitNb, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;
//	if (StringFct_Float2Str(Menu_Status.ActualNode->Param->MaxVal,MyString, 5, Menu_Status.ActualNode->Param->DecimalNb)==0) MenuInput_PrintPlantageConv() ;

	USART_Print(Menu_UART,MyString, Menu_Status.ActualNode->Param->DigitNb +2);
	USART_Print(Menu_UART,"\r\n", 2);
	// line 4
	USART_Print(Menu_UART," Actual: ", 9);
	USART_Print(Menu_UART,Menu_Status.StringInputValue, Menu_Status.ActualNode->Param->DigitNb +2);
	USART_Print(Menu_UART,"\r\n", 2);
}

void MenuInput_PrintDiscard(void)
{
	USART_Print(Menu_UART,"Discard Changes ?   ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Back for yes...     ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Forward for no...   ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"                    ",20); //clr
	USART_Print(Menu_UART,"\r\n", 2);
}

void MenuInput_PrintConfirm(void)
{
	USART_Print(Menu_UART,"Confirm Changes ?   ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Forward for yes...  ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Back for no...      ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"                    ",20); //clr
	USART_Print(Menu_UART,"\r\n", 2);
}

void MenuInput_PrintRetry(void)
{
	USART_Print(Menu_UART,"Value out of range, ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Please retry        ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Strike any key ...  ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"                    ",20); //clr
	USART_Print(Menu_UART,"\r\n", 2);
}

void MenuInput_PrintPlantageConv(void)
{
	USART_Print(Menu_UART,"Plantage sys        ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Pb conv float / str ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"Revoir les datas des ",20);
	USART_Print(Menu_UART,"\r\n", 2);
	USART_Print(Menu_UART,"param MenuNode       ",20); //clr
	USART_Print(Menu_UART,"\r\n", 2);
	while(1);
}



/**
  * @brief
  */
