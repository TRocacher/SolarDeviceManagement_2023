#include "PhyUART.h"
#include "MyTimer.h"

#include "Timer_1234.h"
#include "MyLCD.h"
#include "FctDiverses.h"
#include "clock.h"

/******************************************************************************************************************
	Master, test PhyUART
*****************************************************************************************************************/


char Rec[30];
int i;


int main (void)
{
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	PhyUART_Init();
	PhyUART_StartFSM();
		
	MyLCD_Init();
	MyLCD_Clear();
	
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("I am the Master ");
	MyLCD_Set_cursor(0, 1);
	
  PhyUART_SendNewMssg("Hey from Master",15);
	
	while(1)
	{
		if (PhyUART_IsNewMssg()==1)
		{
			if (PhyUART_GetNewMssg(Rec,30)!=-1)
			{
				MyLCD_Set_cursor(0, 1);
				MyLCD_Print("                ");
				MyLCD_Set_cursor(0, 1);
				MyLCD_Print(Rec);
				Delay_x_ms(1000);
				PhyUART_SendNewMssg("Still Master ! ",15);
			}
		}
		
	}
}
