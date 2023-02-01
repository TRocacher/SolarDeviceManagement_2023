#include "PhyUART.h"
#include "TimeOut.h"

#include "Timer_1234.h"
#include "MyLCD.h"
#include "clock.h"

/******************************************************************************************************************
	Slave, test PhyUART
*****************************************************************************************************************/



char Rec[30];
int i;

void ProcessFSM(void)
{
	PhyUART_FSM_Progress();
}

int main (void)
{
  CLOCK_Configure();
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	PhyUART_Init();
	PhyUART_StartFSM();
	
	// Timer pour activer FSM
	Timer_1234_Init(TIM2,50.0);
	Active_IT_Debordement_Timer( TIM2, 3, ProcessFSM);
	
	MyLCD_Init();
	MyLCD_Clear();
	
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("Waiting Master... ");
	MyLCD_Set_cursor(0, 1);
	
	
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
				
				MyLCD_Set_cursor(0, 0);
				MyLCD_Print("                ");
				MyLCD_Set_cursor(0, 0);
				MyLCD_Print("Hello Master ! ");
				
				PhyUART_SendNewMssg("Hello Master ! ",15);
				
			}
		}
		
	}
}
