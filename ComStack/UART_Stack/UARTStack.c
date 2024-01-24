/* =================================================================================
* ==================   UARTStack.c	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   D�pendance : TimeStampManagement (TimeOut)
 *  ------------------------------------------------------------------------------
 * Pile de com�munication UART
 * Principe :
 * Format de trame UART |NbOctets y compris Cmde et checksum (0 � 255)| Data Bytes|CheckSum|
 
 * Machine � �tats qui attend la r�ception d'un octet : c'est le nbre � r�cup�rer
 * La ch�ine est alors construite, brute, en entier donc 256 bytes r�serv�s
 * Un timeOut est g�r� de 2 secondes (9600 Bds , 256 bytes donne 
 *
* =================================================================================*/