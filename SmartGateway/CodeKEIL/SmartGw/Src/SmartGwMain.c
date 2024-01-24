#include "FSKStack.h"
#include "Timer_F103.h"
#include "MyLCD.h"

#include <ProtocoleFcts.h>
#include "StringFct.h"
#include "TimeManagement.h"
#include "Driver_DS1307.h"


#include "TimeStampManagement.h"
#include "UARTStack.h"

/******************************************************************************************************************
	

	
	
	
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
TimeStampTypedef TimeStampTest={0,0,0,1,1,2024}; 
TimeStampTypedef TimeStampTestB={0,0,0,24,1,2024}; 

UARTStack_ErrorType MyError;

int main (void)
{


	SystickStart(); // obligatoire pour la gestion des TimeOut � tous les �tages...
	FSKStack_Init(My);
	
	TimerStamp_Start();
	Secondes=TimeStamp_substract(&TimeStampTestB,&TimeStampTest);
	PtrOnMyStrut=(MyStructDef *)Phrase; // PtrOnMyStruct contient l'adresse de d�but de la structure.
	val1=PtrOnMyStrut->un;
	val2=PtrOnMyStrut->deux;
	Val3=PtrOnMyStrut->trois;
	
	MyLCD_Init ();
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print(" LCD Now OK...");
	MyLCD_Set_cursor(0, 1);
	
	UARTStack_Init();
	
while(1)
	{
		if (UARTStack_IsHMIMssg()==1)
		{
			PtrOnString=UARTStack_GetHMIMssg();
			MyError=UARTStack_GetErrorStatus();
		  MyLCD_Set_cursor(0, 1);	
			MyLCD_Print("                ");
		  MyLCD_Set_cursor(0, 1);	
			MyLCD_Print_n (PtrOnString+1,*PtrOnString); // +1 pour ne pas afficher le nbre de bytes
			
		}
		
	}
}
