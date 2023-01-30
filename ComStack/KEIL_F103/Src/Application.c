#include "PhyUART.h"
#include "TimeOut.h"

char Test[30];
int i;



int main (void)
{

	SystickStart();
	PhyUART_Init();
	PhyUART_StartFSM();
	
	while(1)
	{
		PhyUART_FSM_Progress();
		if (PhyUART_IsNewMssg()==1)
		{
			if (PhyUART_GetNewMssg(Test,30)!=-1)
			{
				//PhyUART_SendNewMssg("Il était une fois un petit chaperon rouge qui se promenait",28);
				PhyUART_SendNewMssg("OK",2);
			}
		}
		
	}
}
