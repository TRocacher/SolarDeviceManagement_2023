#include "PhyUART.h"
#include "TimeOut.h"

char Test[5];
int i;



int main (void)
{

	SystickStart();
	PhyUART_Init();
	PhyUART_StartFSM();
	
	while(1)
	{
		PhyUART_FSM_Progress();
	}
}
