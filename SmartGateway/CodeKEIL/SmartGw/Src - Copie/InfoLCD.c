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
#include "DataFromHMI.h"

/*==================================================================================
						Attributs du module
===================================================================================*/
					
						
		


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
	MyLCD_Print("SGw : ");
	MyLCD_Print(RevisionSoft);
	MyLCD_Set_cursor(0, 1);
	MyLCD_Print("Push BP to scroll...");
	

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
	/* Init Tableau Stamp string : Pour débuggage*/
	InfoLCD_MemStampStrInit();
}

	


/*  ========= LES FONCTIONS D'AFFICHAGE ===========*/ 
/**
	* @brief  Affiche l'écran de démarrage
	* @Note		Exemple : 
											SGw : Rev x.yz
											
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenStart(void)
{
	/* Clear first line */
	MyLCD_ClearLineUp();
	MyLCD_Print("SGw :");
	MyLCD_Print(RevisionSoft);
	/* Clear second line */
	MyLCD_ClearLineDown();
}

/**
	* @brief  Affiche l'écran d'attente connexion HMI
	* @Note		Exemple : 
											Waiting for HMI
											...
											
  * @param  
  * @retval 
  **/
void InfoLCD_Screen_WaitForHMI(void)
{
	/* Clear first line */
	MyLCD_ClearLineUp();
	MyLCD_Print("Waiting for HMI");
	/* Clear second line */
	MyLCD_ClearLineDown();
	MyLCD_Print("...");
}


/**
	* @brief  Affiche l'écran how to use
	* @Note		Exemple : 
											Push V main menu
											Push > sub-menu
											
  * @param  
  * @retval 
  **/
void InfoLCD_Screen_HowToUse(void)
{
	/* Clear first line */
	MyLCD_ClearLineUp();
	MyLCD_Print("Push V main menu");
	/* Clear second line */
	MyLCD_ClearLineDown();
	MyLCD_Print("Push > sub-menu");
}

/**
	* @brief  Affiche heure système format xx:xx:xx
	* @Note		Exemple : 
											| System Clock :
											10:34:56
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenSystem(void)
{
	char LCD_Mssg[20];
	char * PtrChar;	
	TimeStampTypedef* TimeStamp;
	
	/* Clear first line */
	MyLCD_ClearLineUp();
	MyLCD_Print("|>System clock :");
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	// Conversion exploitable pour LCD
	TimeStamp=TimeStamp_GetClockStampAdr();
	PtrChar=LCD_Mssg;
	StringFct_Int2Str_99(TimeStamp->Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(TimeStamp->Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(TimeStamp->Sec,PtrChar);
	PtrChar=PtrChar+2;
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Mssg,8); 
}
	

/**
* @brief  : Affichage des 5 températures
						Récupère les 5 températures via les 5 variables RmDv_Data
						du module DataFromRmDv.c
* @Note : Exemple :
					| XX.X XX.X XX.X
					XX.X XX.X
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenAllTemp(void)
{
	RmDvDataTypedef* PtrStamp;
	float LocalTemp;
	char FloatString[5][10];
	int i;
	
	/* Conversion String des 5 temperatures*/
	for (i=0;i<5;i++)
	{
		PtrStamp=RmDvData_GetObjectAdress(ID_Clim_Salon+i); /* 0xD1 à 0xD5*/
		LocalTemp=PtrStamp->Temperature;
		StringFct_Float2Str(LocalTemp,&FloatString[i][0], 3, 1);
		FloatString[i][5]=' '; /* insertion espace */
	}
	MyLCD_ClearLineUp();
	MyLCD_Print("|");
	MyLCD_Print_n(&FloatString[0][0],5); /* +xx.x (avec espace à la fin)*/
	MyLCD_Print_n(&FloatString[1][0],5); /* xx.x (avec espace à la fin)*/
	MyLCD_Print_n(&FloatString[2][0],5); /* xx.x (avec espace à la fin)*/
	MyLCD_ClearLineDown();
	MyLCD_Print_n(&FloatString[3][0],5); /* xx.x (avec espace à la fin)*/
	MyLCD_Print_n(&FloatString[4][0],5); /* xx.x (avec espace à la fin)*/
}


/**
	* @brief  : Affichage le ID du RmDv en question
	* @Note		Exemple : 
											|> Salon ID :
											0xD1
  * @param  
  * @retval 
  **/
void InfoLCD_ScreenRmDvID(int ID)
{
	char MyStringInt[10];
	StringFct_Int2Str(ID,MyStringInt);
	
	
	/* Clear first line */
	MyLCD_ClearLineUp();
	switch(ID)
	{
		case ID_Clim_Salon:MyLCD_Print("|>Clim Salon :");break;
		case ID_Clim_SaManger:MyLCD_Print("|>Clim SaM:");break;
		case ID_Clim_Entree:MyLCD_Print("|>Clim Entree :");break;
		case ID_Clim_Couloir:MyLCD_Print("|>Clim Couloir :");break;
		case ID_Ext:MyLCD_Print("|>Capteur Ext :");break;
		default: while(1);/////// bug
	}
	/* Clear second line */
	MyLCD_ClearLineDown();	
	MyLCD_Print("ID :");
	MyLCD_Print(MyStringInt);
}


/**
	* @brief  : Affichage le Mode du RmDv en question
	* @Note		Exemple : 
											> Mode :
											Programmation
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysMode(void)
{
	DFH_HMIMode* ModePtr;
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Mode :");
	/* second line */
	MyLCD_ClearLineDown();
	ModePtr= DFH_GetCentralData_Mode();
	if (*ModePtr == HMI_Mode_Off) MyLCD_Print(" Off");
	else if (*ModePtr == HMI_Mode_Auto) MyLCD_Print(" Auto");	
	else if (*ModePtr == HMI_Mode_Program) MyLCD_Print(" Program");	
	else MyLCD_Print(" Holliday");	
}



/**
	* @brief  : Affichage de l'heure de la dernière requête HMI
	* @Note		Exemple : 
											> Last HMI stamp
											10:34:00
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysHMIStamp(void)
{
	TimeStampTypedef* HMIStampPtr;
	char LCD_Mssg[20];
	char * PtrChar;	
	
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Last HMI stamp");
	/* Clear second line */
	MyLCD_ClearLineDown();
	HMIStampPtr=DFH_GetCentralData_Stamp();
	
	// Conversion exploitable pour LCD
	PtrChar=LCD_Mssg;
	StringFct_Int2Str_99(HMIStampPtr->Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(HMIStampPtr->Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(HMIStampPtr->Sec,PtrChar);
	PtrChar=PtrChar+2;
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Mssg,8); // +1 pour ne pas afficher le nbre de bytes
}



/**
	* @brief  : Affichage de la puissance onduleur
	* @Note		Exemple : 
											> Power Inverter
											1234.3 W
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysPowerInverter(void)
{
	DFH_PowDataOptTypedef* PwOptPtr;
	float Power;
	char LCD_Mssg[20];
	
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Power Inverter");
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	PwOptPtr= DFH_GetCentralData_OptPowerData();
	Power=PwOptPtr->PowInverter;
	if ( Power<0) MyLCD_Print("Inverter StdBy...");
	else
	{
		StringFct_Float2Str(Power,LCD_Mssg, 5, 1);
		MyLCD_Print(LCD_Mssg);
		MyLCD_Print(" W");
	}
	
}



/**
	* @brief  : Affichage de la puissance en excès, disponible
	* @Note		Exemple : 
											> Power Excess :
											1234.3 W
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysPowerExcess(void)
{
	DFH_PowDataOptTypedef* PwOptPtr;
	float Power;
	char LCD_Mssg[20];
	
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Power Excess :");
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	PwOptPtr= DFH_GetCentralData_OptPowerData();
	Power=PwOptPtr->PowExcess;
	StringFct_Float2Str(Power,LCD_Mssg, 5, 1);
	MyLCD_Print(LCD_Mssg);
	MyLCD_Print(" W");

}


/**
	* @brief  : Affichage de l'écart en secondes entre
							le stamp système et le stamp HMI
	* @Note		Exemple : 
											> Delta Stamp 
											HMI/SGw : -5 s
  * @param  
  * @retval 
  **/
void InfoLCD_SubScreenSysDeltaStamp(void)
{
	char LCD_Mssg[20];
	int Delta;
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Delta Stamp");
	/* Clear second line */
	MyLCD_ClearLineDown();
	MyLCD_Print("HMI/SGw : ");
	Delta=TimeStamp_GetTimeStampDeltaStamp();
	
	if (Delta>=0) StringFct_Int2Str_99(Delta,LCD_Mssg);
	else StringFct_Int2Str_99(-Delta,LCD_Mssg);MyLCD_Print("-");
	
	MyLCD_Print_n(LCD_Mssg,2);
	MyLCD_Print(" s");
}



/**
	* @brief  : Affichage de l'heure de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Last stamp
											10:34:00
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvLastStamp(int ID)
{

	char LCD_Mssg[20];
	char * PtrChar;	
	RmDvDataTypedef* RmDvDataAdr; 
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Last stamp");
	/* Clear second line */
	MyLCD_ClearLineDown();
	RmDvDataAdr=RmDvData_GetObjectAdress(ID);
	// Conversion exploitable pour LCD
	PtrChar=LCD_Mssg;
	StringFct_Int2Str_99(RmDvDataAdr->Delay.StampNow.Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(RmDvDataAdr->Delay.StampNow.Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(RmDvDataAdr->Delay.StampNow.Sec,PtrChar);
	PtrChar=PtrChar+2;
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Mssg,8); 
}



/**
	* @brief  : Affichage de la température de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Temperature :
											21°C
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvTemp(int ID)
{
	RmDvDataTypedef* PtrStamp;
	float LocalTemp;
	char FloatString[10];
	char deg;
	
	deg=0xDF;

	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Temperature :");
	/* Clear second line */
	MyLCD_ClearLineDown();
	PtrStamp=RmDvData_GetObjectAdress(ID); 
	LocalTemp=PtrStamp->Temperature;
	StringFct_Float2Str(LocalTemp,&FloatString[0], 3, 1);
	MyLCD_Print_n(&FloatString[0],5); /* +xx.x (avec espace à la fin)*/
	//MyLCD_Print(" °C"); 
	MyLCD_Print(" ");
	MyLCD_Print(&deg);
	MyLCD_Print("C");
}



/**
	* @brief  : Affichage du statut de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Status :
											No Warning
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvStatus(int ID)
{
	RmDv_WarningCode LocalStatus;
	RmDvDataTypedef* PtrRmDvData;
	PtrRmDvData = RmDvData_GetObjectAdress(ID);
	LocalStatus = PtrRmDvData->Status;
	
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Status :");
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	/* Print Status */		
	MyLCD_Print(TabPtrStr_StatusMssg[LocalStatus]);
	}


/**
	* @brief  : Affichage du prochain intervalle programmé lors
						de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Next interval:
											00:34:03
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvNextInterval(int ID)
{

	char LCD_Mssg[20];
	char * PtrChar;	
	RmDvDataTypedef* RmDvDataAdr; 
	int DelaySec;
	int Hour,Min,Sec,Rest;
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Next interval:");
	/* Clear second line */
	MyLCD_ClearLineDown();
	RmDvDataAdr=RmDvData_GetObjectAdress(ID);
	// Conversion exploitable pour LCD
	PtrChar=LCD_Mssg;
	
	DelaySec=RmDvDataAdr->Delay.TimeIntervalTheoNow;
	/*conversion heure, min sec*/
	Hour=DelaySec/3600;
	Rest=DelaySec-Hour*3600; 
	Min=Rest/60;
	Sec=Rest-Min*60;
	
	StringFct_Int2Str_99(Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(Sec,PtrChar);
	PtrChar=PtrChar+2;
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Mssg,8); 
}


/**
	* @brief  : Affichage facteur de correction de délai lors
						de la dernière requête du RmDv considéré
	* @Note		Exemple : 
											> Delay Factor :
											1.24
  * @param  ID du RmDv
  * @retval 
  **/
void InfoLCD_SubScreenRmDvDelayFactor(int ID)
{
	float LocalFactor;
	char LCD_Mssg[20];
		
	RmDvDataTypedef* PtrRmDvData;
	PtrRmDvData = RmDvData_GetObjectAdress(ID);
	LocalFactor = PtrRmDvData->Delay.TimeExpansionFactor;
	/* First line */
	MyLCD_ClearLineUp();
	MyLCD_Print("> Delay Factor :");
	/* Clear second line */
	MyLCD_ClearLineDown();
	StringFct_Float2Str(LocalFactor,LCD_Mssg, 3, 2);
	MyLCD_Print(LCD_Mssg);
}



/// a trier




#define MemStampStrSize 50

/**
  * @brief  
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintHour(char * PtrLineUpStr,TimeStampTypedef* TimeStamp)
{
	char LCD_Sentence[20];
	char * PtrChar;	
	
	/* Clear first line */
	MyLCD_ClearLineUp();
	MyLCD_Print(PtrLineUpStr); //("Heure systeme:");
	/* Clear second line */
	MyLCD_ClearLineDown();
	
	// Conversion exploitable pour LCD
	PtrChar=LCD_Sentence;
	StringFct_Int2Str_99(TimeStamp->Hour,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';
	PtrChar++;
	StringFct_Int2Str_99(TimeStamp->Min,PtrChar);
	PtrChar=PtrChar+2;
	*PtrChar=':';			
	PtrChar++;
	StringFct_Int2Str_99(TimeStamp->Sec,PtrChar);
	PtrChar=PtrChar+2;
			
	/* Affichage LCD*/		
	MyLCD_Print_n (LCD_Sentence,8); // +1 pour ne pas afficher le nbre de bytes
}
	




/**
* @brief  Affiche sur le LCD l'ID du RmDv ainsi que sa vession logicielle
  * @Note
  * @param  
  * @retval 
  **/
void InfoLCD_PrintRevision(char * RevisionStr,int Len, char ID)
{
	char IDStr[2];
	
	IDStr[0] =ConvQuartetToAscii(ID>>4); /* dizaine*/
	IDStr[1] =ConvQuartetToAscii(ID&0x0F); /* unité*/	
	
	MyLCD_Clear();
	MyLCD_ClearLineUp();
	MyLCD_Print("RmDv ID:");
	MyLCD_Print_n(IDStr,2);
	MyLCD_ClearLineDown();
	MyLCD_Print("Soft.: ");
	MyLCD_Print_n(RevisionStr,Len);
}

/*----------------------------------------------
	MAINTENANCE , DEPANNAGE TRASMISSION
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



/* Test IR */
RmDv_TelecoIR_Cmde InfoLCD_PrintNewSet(TerminalMode Mode)
{
	RmDv_TelecoIR_Cmde CmdeClim = _Stop;
	MyLCD_Clear();
	MyLCD_ClearLineUp();
	MyLCD_Print("Temp. Set :");
	MyLCD_ClearLineDown();
	
	switch(Mode)
	{
		case Temp_18: CmdeClim = _Chaud_18_VanBas_FanAuto ;MyLCD_Print(" 18 deg");break;	
		case Temp_19: CmdeClim = _Chaud_19_VanBas_FanAuto ;MyLCD_Print(" 19 deg");break;	
		case Temp_20: CmdeClim = _Chaud_20_VanBas_FanAuto ;MyLCD_Print(" 20 deg");break;	
		case Temp_21: CmdeClim = _Chaud_21_VanBas_FanAuto ;MyLCD_Print(" 21 deg");break;	
		case Temp_22: CmdeClim = _Chaud_22_VanBas_FanAuto ;MyLCD_Print(" 22 deg");break;	
		case Temp_23: CmdeClim = _Chaud_23_VanBas_FanAuto ;MyLCD_Print(" 23 deg");break;	
		default: CmdeClim = _Stop ;MyLCD_Print(" Stop ");
	}
	return CmdeClim;
}
