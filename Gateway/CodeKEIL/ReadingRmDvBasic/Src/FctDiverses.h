

//=======================================================================================
// 	Auteur T.R.
//  Juin 2018 , 
//  Processeur : STM32F103RB � 72MHz
//  Logiciel dev : �Vision 5.23
//=======================================================================================


#ifndef _FctDiverses_H__
#define _FctDiverses_H__

/* =============================================
				DIVERS
 ============================================= */
/*
Fonction permettant d'attendre environ x ms
Proc�de par boucle while , donc peu pr�cis.
Test� pour CPU � 72MHz
*/
void Delay_x_ms(int x);

#endif
