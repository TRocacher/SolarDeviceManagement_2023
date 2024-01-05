#include "MACPhyUART.h"
#include "Timer_F103.h"
#include "MyLCD.h"

#include <ProtocoleFcts.h>
#include "StringFct.h"
#include "TimeManagement.h"
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

typedef enum {
	EachByte=0,
	EachRzoMssg=1,
	EachMACMssg=2,
}TerminalMode;

TerminalMode Mode;
char TerminalString[50];
int Date_ms; /* affichage ms modulo 10 000 devant chaque trame*/
char Date_Str[5]; /* tableau d'ascii pour affichage date*/

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

void toto(void)
{
}
void UserBP(void)
{
	Clear_Flag_IT_Ext_5_15(13);
	Mode=(Mode+1)%3;
	switch(Mode)
	{
		case EachByte: 
		{
			  MyLCD_Set_cursor(0, 0);
				MyLCD_Print("                   ");
				MyLCD_Set_cursor(0, 0);
				MyLCD_Print("Echo each byte ");
				break;
		}
		case EachRzoMssg:
		{
			  MyLCD_Set_cursor(0, 0);
				MyLCD_Print("                   ");
				MyLCD_Set_cursor(0, 0);
				MyLCD_Print("Rzo Trame ... ");
				break;
		}
		case EachMACMssg :
		{
			  MyLCD_Set_cursor(0, 0);
				MyLCD_Print("                   ");
				MyLCD_Set_cursor(0, 0);
				MyLCD_Print("MAC Trame ... ");
				break;
		}
	}
	
	
}
int main (void)
{
  COMPTEUR_REVEIL=0;
	Mode=EachByte;
	
	ActiveLine=0;
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	MACPhyUART_Init(My);
	MACPhyUART_StartFSM();
	
	MyLCD_Init();
	MyLCD_Clear();
	
	USART_Init(USART1,  115200, 15, toto);

	/* Config UserBP*/
	//GPIO_Configure(GPIOC,13, INPUT, INPUT_FLOATING);
	NVIC_Ext_IT (GPIOC, 13, FALLING_EGDE, INPUT_FLOATING, 14, UserBP);
	
		
  MyLCD_Set_cursor(0, 0);
  MyLCD_Print("HF38400/PC115200 ");
  MyLCD_Set_cursor(0, 1);
  MyLCD_Print("Push User But...     ");

	
	
	while(1)
	{
		
		switch(Mode)
		{
			case EachByte:
			{
				if (SpyUART_Is_ByteRec()==1)
				{
					SpyUART_Clear_ByteRecFlag();
					USART1->DR=SpyUART_Get_ByteRec();
				}
				break;
			}
			case EachRzoMssg:
			{
				if (PhyUART_IsNewMssg()==1)
				{
					Date_ms=(SystickGet()/10)%10000;
  				PhyUART_GetNewMssg(TerminalString, 30);
					/* affichage date*/
					StringFct_Int2Str(Date_ms,Date_Str);
					USART_Print(USART1,Date_Str,5);
					USART_Print(USART1," : ",3);
					/* affichage Trame*/
					USART_Print(USART1,TerminalString,20);
					USART1->DR=0x0D;
					//USART_Print(USART1,"\r",2);
				}
				break;
			}
			default:break;
		}
	}	

}
