#ifndef _GLOBAL_SMARTGATEWAY_H__
#define _GLOBAL_SMARTGATEWAY_H__


/* =================================================================================
* ==================    GLOBAL_SMARTGATEWAY.h	     =================================
 *
 *   Created on: 22/02/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : _
 *  ------------------------------------------------------------------------------
 *  Donne les constantes nécessaires à l'ensemble des échanges (haut niveau)
* =================================================================================*/

/* Ident des 4 climatisations NB : Les codes doivent se suivrent dans l'ordre et sans saut
	depuis ID_Clim_Salon jusqu'à ID_Ext !! */
#define	ID_Clim_Salon 0xD1
#define	ID_Clim_SaManger 0xD2
#define	ID_Clim_Entree 0xD3
#define	ID_Clim_Couloir 0xD4
#define ID_Ext 0xD5
#define  My 0xDD





#endif

