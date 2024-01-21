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

Menu_ParamValTypedef ParamNull;

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







/*==========================================================================================
 *******************************************************************************************
 **************** 	    fct : Menu_Init         ********************************************
 *******************************************************************************************
 ==========================================================================================*/

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




/*==========================================================================================
 *******************************************************************************************
 **************** 	    fct : MenuNodeUpdate        ****************************************
 *******************************************************************************************
 ==========================================================================================*/

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




/*==========================================================================================
 *******************************************************************************************
 **************** 	    fct : Menu_PassivePrint        *************************************
 *******************************************************************************************
 ==========================================================================================*/


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

/*==========================================================================================
 *******************************************************************************************
 **************** 	    fct : Menu_InputNumber         *************************************
 *******************************************************************************************
 ==========================================================================================*/

void MenuInput_PrintPlantageConv(void);
void MenuInput_PrintDiscard(void);
void MenuInput_PrintConfirm(void);
void MenuInput_PrintRetry(void);
void MenuInput_PrintParam(void);


void Menu_InputNumber(void)
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

/*==========================================================================================
 *******************************************************************************************
 **************** 	    fct : Menu_InputYesNo          *************************************
 *******************************************************************************************
 ==========================================================================================*/

void Menu_InputYesNo (void) 
{
}




/**
  * @brief
  */
