/* =================================================================================
* ==================   InfoLCD.c	     =================================
 *
 *   Created on: 07/04/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/

#include "InfoLCD.h"
#include "StringFct.h"




/* Message en dur pour test LCD*/
char NoWarn[]="NoWarning";
char Trial_2[]= "Trial 2...";
char Trial_3[]= "Trial 3...";
char Trial_4[]= "Trial 4...";
char Trial_5[]= "Trial 5...";
char Trial_6[]= "Trial 6...";
char Trial_7[]= "Trial 7...";
char Trial_8[]= "Trial 8...";
char Trial_9[]= "Trial 9...";
char Trial_10[]= "Trial 10...";
char Error_TempI2C[]= "Erreur Temp I2C...";
char Error_NewSetNotRec[]= "RmDv : No New order ";
char Error_StatusNotRec[]= "SGw : No status ";




char* TabPtrStr_StatusMssg[25];		/* Tableau de status Transaction à afficher */
char LCD_Str[20]; 								/* String pour affichage ligne LCD*/



/* ===============================================
       
================================================== */

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Init(void)
{
	MyLCD_Init ();
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print(" LCD Ready...");
	MyLCD_Set_cursor(0, 1);
	/* Prépa pointeur de chaine*/
	TabPtrStr_StatusMssg[Status_NoWarning]=NoWarn;
	TabPtrStr_StatusMssg[Status_Trial_2]=Trial_2;
	TabPtrStr_StatusMssg[Status_Trial_3]=Trial_3;
	TabPtrStr_StatusMssg[Status_Trial_4]=Trial_4;
	TabPtrStr_StatusMssg[Status_Trial_5]=Trial_5;
	TabPtrStr_StatusMssg[Status_Trial_6]=Trial_6;
	TabPtrStr_StatusMssg[Status_Trial_7]=Trial_7;
	TabPtrStr_StatusMssg[Status_Trial_8]=Trial_8;
	TabPtrStr_StatusMssg[Status_Trial_9]=Trial_9;
	TabPtrStr_StatusMssg[Status_Error_TempI2C]=Error_TempI2C;
	TabPtrStr_StatusMssg[Status_Error_NewTempSetNotReceived]=Error_NewSetNotRec;
	TabPtrStr_StatusMssg[Status_NoStatusReceived]=Error_StatusNotRec;
}


/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_Status_LastTempSet( RmDv_WarningCode Status,char LastTempSet)
{
		/* Clear First line */
		MyLCD_ClearLineUp();
		/* Print Status */		
		MyLCD_Print(TabPtrStr_StatusMssg[Status]);
		/* Clear second line */
		MyLCD_ClearLineDown();
		/* Print LastTempSet*/
		StringFct_Int2Str_99(LastTempSet,LCD_Str);
		MyLCD_Print_n (LCD_Str,5);
}	




/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintHMIHour(TimeStampTypedef* TimeStampIHM)
{
	char LCD_Sentence[20];
	char * PtrChar;	
	
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	// Conversion exploitable pour LCD
	PtrChar=LCD_Sentence;
	StringFct_Int2Str_99(TimeStampIHM->Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(TimeStampIHM->Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(TimeStampIHM->Sec,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';	
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Sentence,16); // +1 pour ne pas afficher le nbre de bytes
}
	



/*----------------------------------------------
 Gestion des Tamp String pour maintenance/debug
----------------------------------------------*/
#define MemStampStrSize 50
#define ID_Idx_Size 5
char MemStampStr[MemStampStrSize][ID_Idx_Size][8];
int InfoLCD_StampTabIdx[ID_Idx_Size];



/**
  * @brief 
  * @Note
  * @param  
  * @retval 
  **/

void InfoLCD_MemStampStrInit(void)
{
	int i;
	int j;
	for (j=0;j<ID_Idx_Size;j++)
	{
		for (i=0;i<MemStampStrSize;i++)
		{
			MemStampStr[i][j][0]='0';
			MemStampStr[i][j][1]='0';
			MemStampStr[i][j][2]=':';
			MemStampStr[i][j][3]='0';
			MemStampStr[i][j][4]='0';
			MemStampStr[i][j][5]=':';
			MemStampStr[i][j][6]='0';
			MemStampStr[i][j][7]='0';	
		}
		InfoLCD_StampTabIdx[j]=0;
	}
	
}





/**
  * @brief  
  * @Note
  * @param  
  * @retval -1 si erreur, o sinon
  **/
int InfoLCD_AddTimeStampToMem(TimeStampTypedef* TimeStamp, char ID)
{
	char MyString[15];
	char * PtrChar;	
	int i;
	int ID_Idx;
	int Statut;
	
	ID_Idx=(int)ID-(int)ID_Clim_Salon;
	Statut=0;
	
	if ((ID_Idx>=0) && (ID_Idx<=5))
	{
	
		// Conversion exploitable pour LCD
		PtrChar=MyString;
		StringFct_Int2Str_99(TimeStamp->Hour,PtrChar);
		PtrChar=PtrChar+2;
		*PtrChar=':';
		PtrChar++;
		StringFct_Int2Str_99(TimeStamp->Min,PtrChar);
		PtrChar=PtrChar+2;
		*PtrChar=':';			
		PtrChar++;
		StringFct_Int2Str_99(TimeStamp->Sec,PtrChar);
	
		/*Ecriture dans la table à l'ndice courant*/
		PtrChar=MyString;
		for (i=0;i<8;i++)
		{
			MemStampStr[InfoLCD_StampTabIdx[ID_Idx]][ID_Idx][i]=*PtrChar;
			PtrChar++;
		}
		InfoLCD_StampTabIdx[ID_Idx]=(InfoLCD_StampTabIdx[ID_Idx]+1)%MemStampStrSize;
	}
	else
	{
		Statut=-1;
	}
	return Statut;
}



