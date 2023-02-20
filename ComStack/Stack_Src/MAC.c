#include "stm32f10x.h"
#include "MAC.h"
#include "FctDiverses.h"




/*---------------------------------
// VARIABLE D'INTERFACE AVEC ExchangeLayer
----------------------------------*/


// la structure d'échange PhyUART-MAC
struct MAC_Mssg_type
{
	char StrReceived[StringLenMax]; // |Org@|Data|
	char LenStrReceived;
	char NewStrReceived;
}MAC_Mssg;



int MAC_SendNewMssg (char * AdrString, int Len)
{
	
}