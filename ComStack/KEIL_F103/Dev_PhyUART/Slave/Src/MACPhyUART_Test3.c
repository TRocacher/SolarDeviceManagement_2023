#include "MACPhyUART.h"
#include "Timer_F103.h"
#include "MyLCD.h"
#include "FctDiverses.h"

/******************************************************************************************************************
	Slave, test PhyUART
*****************************************************************************************************************/



char Rec[30];
int i;
char SrcAddr,Longueur;


int main (void)
{
  
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	MACPhyUART_Init(0xBB);
	MACPhyUART_StartFSM();
	
	MyLCD_Init();
	MyLCD_Clear();
		
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("Slave Rec:        ");
	MyLCD_Set_cursor(0, 1);
	MyLCD_Print("Sending :         ");
	
	
	while(1)
	{
		if (MACPhyUART_IsNewMssg()==1)
		{
			if (MACPhyUART_GetNewMssg(Rec,30)!=-1)
			{
				SrcAddr=MACPhyUART_GetSrcAdress();
				Longueur=MACPhyUART_GetLen();
				MyLCD_Set_cursor(11, 0);
				MyLCD_Print(Rec);
				
				Delay_x_ms(200);
								
				MACPhyUART_SendNewMssg(SrcAddr,Rec,Longueur);
				MyLCD_Set_cursor(11, 1);
				MyLCD_Print(Rec);

				
			}
		}
		
	}
}
