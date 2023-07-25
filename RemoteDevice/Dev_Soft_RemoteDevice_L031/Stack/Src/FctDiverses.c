#include "FctDiverses.h"
// vérifier les timing !!
#define SysClk_MHz 24
#define N ((11990*SysClk_MHz)/72)

/* =============================================
				DIVERS
 ============================================= */
/*
Fonction permettant d'attendre environ x ms
Proc�de par boucle while , donc peu pr�cis.
Test� pour CPU � 72MHz
*/
void Delay_x_ms(int x)
{
	int i;
	int j;
	i=0;
	while (i<x)
	{
		j=0;
		while (j<N)
		{
			j++;
		}
		i++;
	}		
}

