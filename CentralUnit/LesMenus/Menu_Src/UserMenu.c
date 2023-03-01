/*
 *
 *
 */


#include "UserMenu.h"

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

