
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021.h"
#include "GPIO.h"
#include "FctDiverses_v2.h"
#include "API_Xbee_ModeAT_2022.h"
#include "LCD.h"
#include "USART_rev2021.h"
#include <string.h>
#include "stdio.h"

char CptByte;
void ByteRec(void);


int main (void)
{
	CLOCK_Configure();
  Init_USART(USART2,9600, 0); 
	Init_IT_Serial_Receive_Byte(USART2, ByteRec);
	lcd_init ();
	lcd_clear();
	set_cursor(0, 0);
	lcd_print(" LCD OK...");
	set_cursor(0, 1);
	CptByte=0;
	
	while(1)	
	{
	}
	
}

char Byte;
void ByteRec()
{
	CptByte++;
	if (CptByte==16)
	{
		CptByte=0;
		lcd_clear();
		set_cursor(0, 0);
		lcd_print(" LCD OK...");
		set_cursor(0, 1);
	}
	Byte=Read_Received_Current_Byte(USART2);
	lcd_putchar(Byte);
	Put_Char(USART2,Byte+1); 
	
}


