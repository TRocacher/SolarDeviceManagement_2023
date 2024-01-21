#include "FSKStack.h"
#include "Timer_F103.h"
#include "MyLCD.h"

#include <ProtocoleFcts.h>
#include "StringFct.h"
#include "TimeManagement.h"
#include "Driver_DS1307.h"


#include "TimeStampManagement.h"

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



typedef struct 
{
	short int Mode; 
	short int deux;  // 1973
	float trois; // 3.14
}HMIData2SGw_Typedef;

typedef enum {
	Off,
	Auto,
	Tempo,
	Hollidays,

}Mode ;


typedef struct 
{
	short int un; // 2024
	short int deux;  // 1973
	float trois; // 3.14
}MyStructDef;

//MyStructDef MyStruct={40000,1973,3.14};
char Phrase[8]={0x40,0x9C, 0xB5,0x07,0xC3,0xF5,0x48,0x40};

MyStructDef * PtrOnMyStrut;
char * PtrOnString;

float Val3;
short int val2;
short int val1;

int Secondes;
TimeStampTypedef TimeStampTest={0,0,0,1,1,2025}; 
int main (void)
{


	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	FSKStack_Init(My);
	
	TimerStamp_Start();
	Secondes=TimeStamp_2_Sec(&TimeStampTest);
	PtrOnMyStrut=(MyStructDef *)Phrase; // PtrOnMyStruct contient l'adresse de début de la structure.
	val1=PtrOnMyStrut->un;
	val2=PtrOnMyStrut->deux;
	Val3=PtrOnMyStrut->trois;
	
	
	while(1)
	{
	
	}
}
