#include "FctDiverses_v2.h"

/* =============================================
				DIVERS
 ============================================= */
/*
Fonction permettant d'attendre environ x ms
Procède par boucle while , donc peu précis.
Testé pour CPU à 72MHz
*/
void Delay_x_ms(int x)
{
	int i;
	int j;
	i=0;
	while (i<x)
	{
		j=0;
		while (j<11990)
		{
			j++;
		}
		i++;
	}		
}


/*
Fonction qui renvoie l'adresse d'une chaine de caractère (4 caractère).
Format d'entrée floattant signé
Format de sortie +xx.x ou -xx.x
*/

char FctDiverse_Tab[10];
char * Float2Tring(float i)
{
	// i au format xx.x
	// Tab donnera au final
	// 0 1 2 3 4 ...
	// x x . x
	int Temp;
	if (i<0)
	{
		i=-i;
		FctDiverse_Tab[0]='-';
	}
	else FctDiverse_Tab[0]=' ';
	
	Temp=(int)(i*10); // Temp est entier xxx
	FctDiverse_Tab[1]=Temp/100;
	Temp=Temp-100*FctDiverse_Tab[1];
	FctDiverse_Tab[2]= Temp/10;
	FctDiverse_Tab[3]='.';
	FctDiverse_Tab[4]=Temp-10*FctDiverse_Tab[2]+0x30;
	FctDiverse_Tab[2]+=0x30;
	FctDiverse_Tab[1]+=0x30;
	FctDiverse_Tab[5]=0;
	return FctDiverse_Tab;
	
}
