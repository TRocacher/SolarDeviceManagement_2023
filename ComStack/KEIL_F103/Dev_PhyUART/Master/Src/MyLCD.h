/*----------------------------------------------------------------------------
 * Name:    LCD.H
 * Purpose: LCD function prototypes
 * Version: V1.10
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *---------------------------------------------------------------------------*/

extern void MyLCD_Init       (void);
extern void MyLCD_Set_cursor     (int column, int line);
extern void lcd_bargraph   (int value, int size);
extern void lcd_bargraphXY (int pos_x, int pos_y, int value);
extern void lcd_putchar    (char c);
/******************************************************************************/
extern void MyLCD_Clear (void);
extern void MyLCD_ClearLineUp (void);
extern void MyLCD_ClearLineDown (void);
extern void MyLCD_Print (char *string);
