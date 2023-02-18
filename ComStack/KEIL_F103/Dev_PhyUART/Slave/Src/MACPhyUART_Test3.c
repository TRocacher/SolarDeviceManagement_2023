#include "MACPhyUART.h"
#include "MyTimer.h"

#include "Timer_1234.h"
#include "MyLCD.h"
#include "FctDiverses.h"

/******************************************************************************************************************
	Slave, test PhyUART
*****************************************************************************************************************/



char Rec[30];
int i;



int main (void)
{
  
	SystickStart(); // obligatoire pour la gestion des TimeOut � tous les �tages...
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
				MyLCD_Set_cursor(11, 0);
				MyLCD_Print(Rec);
				
				Delay_x_ms(200);
								
				MACPhyUART_SendNewMssg(0xAA,Rec,5);
				MyLCD_Set_cursor(11, 1);
				MyLCD_Print(Rec);

				
			}
		}
		
	}
}
