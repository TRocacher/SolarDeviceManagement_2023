/*
 * StringFct.c
 *
 *  Created on: 26 avr. 2021
 *      Author: trocache
 */


#include "StringFct.h"

char ConvQuartetToAscii(char Quartet)
{
	char Val;
	if (Quartet<=9) Val=Quartet+0x30;
	else  Val=Quartet-10+65; /* à partir de A (Ascii de A =65 )
		Exemple Quartet=0xB cad 11. Val = 11-10 +65 = 1+65 =66 = code ASCII de B*/
	return Val;
}



void StringFct_Int2Str_99(int Value,char * DigitTab)
{
	int Q,R;
	Q=Value/10;
	R=Value-Q*10;
	*DigitTab=Q+0x30;
	*(DigitTab+1)=R+0x30;
	*(DigitTab+2)=0; /* Null */
}


void StringFct_Int2Str(int Value,char * DigitTab)
{
	int Q,R;
	Q=Value/10000;
	R=Value-Q*10000;
	*DigitTab=Q+0x30;
	
	Q=R/1000;
	R=R-Q*1000;
	*(DigitTab+1)=Q+0x30;
	
	Q=R/100;
	R=R-Q*100;
	*(DigitTab+2)=Q+0x30;
	
	Q=R/10;
	R=R-Q*10;
	*(DigitTab+3)=Q+0x30;
	

	*(DigitTab+4)=R+0x30;
	*(DigitTab+5)=0; /* Null */
}

char StringFct_Float2Str(float Value,char * DigitTab, int DigitNb, int Frac)
{
  // exemple 1342.54, Fract=2, DigitNb=6
	// pos       0    1   2  2   4   5   6   7  8
	// 		| 'Sign' '1' '3 '4' '2' '.' '5' '4' 0 |  9 char
	// min  | 'Sign' '1'  '.' 0 .... |


	int i;
	char PosVirg;
	int PdsMax;
	int Val;


    if ((Frac>=0)&&(Frac<=3)&&(DigitNb>0)&&(DigitNb<=6)&&(DigitNb>Frac))
    {
    	// gestion virgule
    	PosVirg=DigitNb-Frac+1; // exemple 4.2 donne PosVirg = 5
    	* DigitTab=' '; // + par défaut
    	if (Value<0)
    	{
    		Value=-Value;
    		* DigitTab='-';
    	}
    	// -> | ' ' 'x' 'x 'x' 'x' 'x' 'x' 'x' x |

    	// réglage du poids max
    	PdsMax=1;
    	for (i=1; i<DigitNb;i++)
    	{
    		PdsMax=PdsMax*10; // ex : DigitNb = 2 -> PdsMax=10, DigitNb = 1 -> PdsMax=1
    	}
    	// passage entier provisoire :
    	switch (Frac)
    	{
    		case 0:Val=Value;break;
    		case 1:Val=10.0*Value;break;
    		case 2:Val=100.0*Value;break;
    		case 3:Val=1000.0*Value;break;
    	}
    	// 1342.54 donne Val (int) =  134254


    	// détermination des digit
    	for (i=1; i<=DigitNb;i++)
    	{
    		*(DigitTab+i)=Val/PdsMax+0x30;Val=Val%PdsMax;
    		PdsMax=PdsMax/10;
    	}
    	// -> | ' ' '1' '3 '4' '2' '5' '4' 'x' x |

    	// insertion virgule
    	for (i=DigitNb; i>=PosVirg;i--)
    	{
    		*(DigitTab+i+1)=*(DigitTab+i);
    	}
    	// -> | ' ' '1' '3 '4' '2' '5' '5' '4' x |
    	*(DigitTab+PosVirg)='.';
    	// -> | ' ' '1' '3 '4' '2' '.' '5' '4' x |

    	// insertion Null
    	*(DigitTab+DigitNb+2)=0;

    	return 1;

    }

    else return 0;

}

float StringFct_Str2Float(char * DigitTab, int DigitNb, int Frac)
{
	//|sxx.xxx00| DigitNb=5, DecimalNb=2
	//|sx.xx0000| DigitNb=3, DecimalNb=2
	//|sx.000000| DigitNb=1, DecimalNb=0
	//|sxxx.xxx0|
    //|sxxxxxx.0|

	float Val;
	int i;
	char Digit;
	float Fact;

    if ((Frac>=0)&&(Frac<=3)&&(DigitNb>0)&&(DigitNb<=6)&&(DigitNb>Frac))
    {
	// calcul entier
	Val=0.0;
	Fact=1.0;
	for (i=DigitNb+1;i>=1;i--)
	{
		Digit=*(DigitTab+i);
		if (Digit !=0)
		{
			if (Digit !='.')
			{
				Digit = Digit-0x30;
				Val=Val+(float)Digit*Fact;
				Fact=Fact*10.0;
			}
		}
	}

	// fractionnaire
	switch(Frac)
	{
	case 1:Val=Val/10.0;break;
	case 2:Val=Val/100.0;break;
	case 3:Val=Val/1000.0;break;
	}

	if (*DigitTab == '-') Val=-Val;
    }
    else Val=1100000.0; // valeur hors gabarit, testable en comparant à 1000000.0

	return (Val); // test erreur en faisant si fct <1000000.0

}

