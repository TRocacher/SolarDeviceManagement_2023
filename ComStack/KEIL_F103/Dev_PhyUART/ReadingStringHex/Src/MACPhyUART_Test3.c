#include "MACPhyUART.h"
#include "Timer_F103.h"
#include "MyLCD.h"
#include "FctDiverses.h"
#include <ProtocoleFcts.h>
/******************************************************************************************************************
	Slave, test PhyUART
*****************************************************************************************************************/


float Temperature;
char Rec[30]; // Ex 0x41 0xAB
char StrincAscii[60];
char D,U;
int i,j;
char SrcAddr,Longueur;

char ConvQuartetToAscii(char Quartet)
{
	char Val;
	if (Quartet<=9) Val=Quartet+0x30;
	else  Val=Quartet-10+65; /* à partir de A (Ascii de A =65 )
		Exemple Quartet=0xB cad 11. Val = 11-10 +65 = 1+65 =66 = code ASCII de B*/
	return Val;
}


int main (void)
{
  
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	MACPhyUART_Init(0xBB);
	MACPhyUART_StartFSM();
	
	MyLCD_Init();
	MyLCD_Clear();
		
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("                  ");
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("BaudRate :");
	MyLCD_Set_cursor(0, 1);
	MyLCD_Print("                  ");
	MyLCD_Set_cursor(0, 1);
	MyLCD_Print("  38400 bd");
	
	Delay_x_ms(1000);
	MyLCD_Clear();
	
	j=0;
	
	while(1)
	{
		if (MACPhyUART_IsNewMssg()==1)
		{
			Longueur=MACPhyUART_GetLen();
			MACPhyUART_GetNewMssg(Rec,Longueur); 
			Temperature=Protocole_ExtractTemperature(Rec);
			for(i=0;i<Longueur;i++)
			{
				D=Rec[i]/16;
				U=Rec[i]-16*D;
				StrincAscii[i*2]=ConvQuartetToAscii(D);
				StrincAscii[i*2+1]=ConvQuartetToAscii(U);
			}
			StrincAscii[Longueur*2]=0; /* caractere nul*/
			MyLCD_Set_cursor(0, j);	
			MyLCD_Print("                  ");
			MyLCD_Set_cursor(0, j);
			MyLCD_Print(StrincAscii);
			j=(j+1)%2;
			
			
//			if (MACPhyUART_GetNewMssg(Rec,30)!=-1)
//			{
//				SrcAddr=MACPhyUART_GetSrcAdress();
//				Longueur=MACPhyUART_GetLen();
//				MyLCD_Set_cursor(11, 0);
//				MyLCD_Print(Rec);
//				
//				Delay_x_ms(200);
//								
//				MACPhyUART_SendNewMssg(SrcAddr,Rec,Longueur);
//				MyLCD_Set_cursor(11, 1);
//				MyLCD_Print(Rec);

//				
//			}
		}
		
	}
}
