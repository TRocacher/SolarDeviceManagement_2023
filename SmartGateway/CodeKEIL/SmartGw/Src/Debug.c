/* =================================================================================
* ==================   DEBUG.c	     =================================
 *
 *   Created on: 07/04/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/

#include "Debug.h"



void HardFault_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("HardFault...");
	while(1)
	{
	}
}

void MemManage_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("MemManage...");
	while(1)
	{
	}
}

void BusFault_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("BusFault...");
	while(1)
	{
	}
}


void UsageFault_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("UsageFault...");
	while(1)
	{
	}
}


void SVC_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("SVC_Handler...");
	while(1)
	{
	}
}

void DebugMon_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("DebugMon_Handler");
	while(1)
	{
	}
}

void PendSV_Handler(void)
{
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("PendSV_Handler");
	while(1)
	{
	}
}


