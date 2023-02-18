#include "MACPhyUART.h"
#include "MyTimer.h"


#include "MyLCD.h"
#include "FctDiverses.h"
#include "clock.h"

/******************************************************************************************************************
	Master, test PhyUART
*****************************************************************************************************************/


char Rec[30];
int Error,Res;
short int Cnt; // max 65535
int Reste;
char Alpha[6];


int CharToInt(char * Messg)
{
 int i;
 i=10000*(Alpha[0]-0x30)+1000*(Alpha[1]-0x30)+100*(Alpha[2]-0x30)+10*(Alpha[3]-0x30)+(Alpha[4]-0x30);
 return i;
 
}

void IntToChar(int Val)
{
	int i;
	Alpha[0]=Val/10000;
	Reste=Val%10000;
	Alpha[1]=Reste/1000;
	Reste=Reste%1000;
	Alpha[2]=Reste/100;
	Reste=Reste%100;
	Alpha[3]=Reste/10;
	Alpha[4]=Reste%10;
	Alpha[5]=0; // null
	
	for (i=0;i<5;i++)
	{
		Alpha[i]=Alpha[i]+0x30;
	}

	
}

void IT_1sec(void)
{
	Cnt++;
	IntToChar(Cnt);
	
	
	if ((Res+1)!=Cnt) Error++;
	
	MyLCD_Set_cursor(10, 0);
	MyLCD_Print(Alpha);
	
	MACPhyUART_SendNewMssg(0xBB,Alpha,5);
	
	
}


int main (void)
{
	Cnt=0;
	Error=0;
	SystickStart(); // obligatoire pour la gestion des TimeOut à tous les étages...
	MACPhyUART_Init(0xAA); // My = 0xAA
	MACPhyUART_StartFSM();
	
	MyLCD_Init();
	MyLCD_Clear();
		
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("Mast send:       ");
	MyLCD_Set_cursor(0, 1);
	MyLCD_Print("Receiving:       ");
		
	
	MyTimer_CkEnable(TIM1);
	MyTimer_Set_Period(TIM1, 7200-1, 10000-1 );
  MyTimer_IT_Enable( TIM1, 5, IT_1sec);	

	


  
	
	while(1)
	{
		if (MACPhyUART_IsNewMssg()==1)
		{
			if (MACPhyUART_GetNewMssg(Rec,30)!=-1)
			{
				MyLCD_Set_cursor(10, 1);
				MyLCD_Print(Rec);
				Res=CharToInt(Rec);
		
			}
		}
		
	}
}
