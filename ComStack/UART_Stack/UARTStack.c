/* =================================================================================
* ==================   UARTStack.c	     =================================
 *
 *   Created on: 24/01/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : TimeStampManagement (TimeOut)
 *  ------------------------------------------------------------------------------
 * Pile de comùmunication UART
 * Principe :
 * Format de trame UART |NbOctets y compris Cmde et checksum (0 à 255)| Data Bytes|CheckSum|
 
 * Machine à états qui attend la réception d'un octet : c'est le nbre à récupérer
 * La châine est alors construite, brute, en entier donc 256 bytes réservés
 * Un timeOut est géré de 2 secondes (9600 Bds , 256 bytes donne 
 *
* =================================================================================*/