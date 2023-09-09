#include "MACPhyUART.h"
#include "Timer_F103.h"
#include "MyLCD.h"

#include <ProtocoleFcts.h>
#include "StringFct.h"
#include "TimeManagement.h"
#include "Driver_DS1307.h"

/******************************************************************************************************************
	Pgm de test du Rm Device.
	Le RmDV :
	- se réveille 
	- mesure la temp
	- l'envoie au UC
	- il attend l'ordre de clim
	- met à jour l'ordre de clim
	- envoie statut warning
	- se rendord
	
Si plabntage, -> go to sleep, pour l'instant l'UC ne peut pas le savoir clairement

Fct normal = mise à jour toutes les 30mn. Durée 2ans.
Tourne la journée de 9h00 à 23h = 14h = 28 réveils par jour.
Doit donc tenir 28*365*2 = 20440 réveils.
On réduit à 1 mois = 1*31*24*60=44640 mn.
Periode = 44640 mn / 20440 = 2,18mn.
On arrondi à 2mn.
Durant 30sec on affiche le compteur.

-> en réalité le timing est d'environ 1mn45s.
Le tps d'affichage de la temp de l'UC est réglé à 30 seconde.

Pour l'UC algo :
- boucle d'attente réception (sans test de quel RmDv a émis (y a que 0xA0)
- réception température (non bloquant, si n'arrive pas rien ne se passe, rien ne s'affiche)
- réception warning (idem). C'est dans cet état que l'affichage du compteur se fait. 
  dans cet état une tempo d 30sec est lancée qui bloque toute réception pour que l'utilisateur
	ait le tps de lire temp et warning. Ensuite on affiche le comptage et surtout on remet à 0
	la pile pour éviter une nouvelle boucle après la tempo dû à une seconde émission du RmDv.



	
	
	
*****************************************************************************************************************/

#define Adr_RmDv_1 0xA0
#define My 0xBB

float Temperature;
char Temp_String[9];
char Rec[30]; // Ex 0x41 0xAB
char Send[30];
char StrincAscii[60];
char IntString[5];
char D,U;
int i;
char ActiveLine;
char FinReception;

int COMPTEUR_REVEIL;


char SrcAddr,Longueur;
MssgCode CodeMessage;
RmDv_WarningCode WarningCode;
char ConvQuartetToAscii(char Quartet)
{
	char Val;
	if (Quartet<=9) Val=Quartet+0x30;
	else  Val=Quartet-10+65; /* à partir de A (Ascii de A =65 )
		Exemple Quartet=0xB cad 11. Val = 11-10 +65 = 1+65 =66 = code ASCII de B*/
	return Val;
}

/*  RTC DS1307 déclaration*/
DS1307_Time_Typedef UserTimeSet;

/* Test émission protocole*/
Protocole_MssgTypedef Protocole_Mssg;
char trial;

int main (void)
{
  COMPTEUR_REVEIL=0;

	ActiveLine=0;
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	MACPhyUART_Init(My);
	MACPhyUART_StartFSM();
	
	MyLCD_Init();
	MyLCD_Clear();
	
	/*
	// Init RTC DS1307
	DS1307_Init(I2C1);
	// set date
	UserTimeSet.Year=22;
	UserTimeSet.Month=10;
	UserTimeSet.Date=13;
	UserTimeSet.Day=4;
	UserTimeSet.Hour=20;
	UserTimeSet.Min=54;
	UserTimeSet.Sec=0;
	*/
	
	//DS1307_SetTime(&UserTimeSet);
	//DS1307_GetTime(&UserTimeSet);
		
  MyLCD_Set_cursor(0, 0);
  MyLCD_Print("Welcom !      ");
  MyLCD_Set_cursor(0, 1);
  MyLCD_Print("Test RmDv Prog   ");
	Delay_x_ms(2000);
	MyLCD_Set_cursor(0, 0);
  MyLCD_Print("Waiting for      ");
	MyLCD_Set_cursor(0, 1);
  MyLCD_Print("RmDv Messages ...       ");
	

  /* TEST envoie multiple*/
	Protocole_Mssg.BaseName=Chrono_5;
	Protocole_Mssg.TimeOut_ms=500;
	Protocole_Mssg.DestAdr=0xA0;
	Protocole_Mssg.Len=6;
	Protocole_Mssg.Mssg="Coucou";
	Protocole_Mssg.TrialNb=250;

	trial=Protocole_SendMACMssg(Protocole_Mssg);
	
	while(1)
	{
	}
	
	
	
	while(1)
	{
		
		
		
		if (MACPhyUART_IsNewMssg()==1) 
		{
				
			Longueur=MACPhyUART_GetLen();
			MACPhyUART_GetNewMssg(Rec,Longueur); 
			CodeMessage=Protocole_ExtractMssgcode(Rec);
			
			/* Réception température */
			if ((CodeMessage==MssgTempCode))
			{
			
				/* Reception actions...*/
				Temperature=Protocole_ExtractTemperature(Rec);
				StringFct_Float2Str(Temperature,Temp_String, 4, 2);
				MyLCD_Set_cursor(0, ActiveLine);	
				MyLCD_Print("Temp °C :");
				MyLCD_Print(Temp_String);
				MyLCD_Print("     ");
			  ActiveLine=(ActiveLine+1)%2;
				/*  Write back Cmde clim..*/
				Protocole_BuildMssgTelecoHeure(Send, _Stop);
				MACPhyUART_SendNewMssg(Adr_RmDv_1,Send,8);
				
			}
			
			/* Réception warning Mssg */
			else if ((CodeMessage==MssgWarningCode)) //&& (WarningDejaRecu==0))
			{
				
				/* Reception actions...*/				
				WarningCode=Protocole_ExtractWarningCode(Rec); 
				MyLCD_Set_cursor(0, ActiveLine);
				switch(WarningCode)
				{
					case NoWarning: MyLCD_Print("No Warning          ");break;
					case Transm_1_Attempt: MyLCD_Print("Fail try 1        ");break;
					case Transm_2_Attempt: MyLCD_Print("Fail try 2        ");break;
					case Transm_3_Attempt: MyLCD_Print("Fail3 No Clim Update   ");break;
					case Temp_Error: MyLCD_Print("Temperature Error       ");break;
					case Transm_Error_NoTimeClimCodeReceived : MyLCD_Print("No clim cmde Rec      ");break;
					case WrongCmdeWhenReceivingTimeClimCode : MyLCD_Print("Wrong Clim Code       ");break;
					
					default:MyLCD_Print("Pb Soft sw case...        ");
				}
				ActiveLine=(ActiveLine+1)%2;
				/* Send back Ack*/
				Protocole_BuildMssgAck(Send);
				MACPhyUART_SendNewMssg(Adr_RmDv_1,Send,1);
					
				
				/* Affichage Cmpteur)*/
				/* attente 30 sec environ*/
				Delay_x_ms(30000);
				
				MyLCD_Set_cursor(0, ActiveLine);
				MyLCD_Print("COMPTEUR: ");
				StringFct_Int2Str(COMPTEUR_REVEIL,IntString);
				MyLCD_Print(IntString);
				MyLCD_Print("      ");
				ActiveLine=(ActiveLine+1)%2;
				/* gestion compteur*/
				COMPTEUR_REVEIL++;
				MACPhyUART_Reset_Restart_KeepMy();				
			}
			

		}
		
	}
}
