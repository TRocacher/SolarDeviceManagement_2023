

//=======================================================================================
// 	Auteur  : T.R. / G.A.
//  Date denri�re r�vision : Sept 2018
//	Description : API permettant de piloter l'ADC tr�s simplement par scrutation
//  D�pendance logicielle : 
//	- ADC_DMA.h, GPIO.h (Drivers_STM32F103_107_Juin2018a.lib)
//	Ressources mat�rielle utilis�es :
//	- ADC1
//	- GPIO (une IO au choix)
//=======================================================================================



#ifndef _API_ADC_H__
#define _API_ADC_H__




/* =============================================
				L'ADC
 ============================================= */
 /*
 D�finition des diverses IO possibles pour l'ADC
 */
 
#define		PA0 0
#define		PA1 1
#define		PA2 2
#define		PA3 3
#define		PA4 4
#define		PA5 5
#define		PA6 6
#define		PA7 7
#define		PB0 8
#define		PB1 9
#define		PC0 10
#define		PC1 11
#define		PC2 12
#define		PC3 13
#define		PC4 14
#define		PC5 15



/**
 * Fonction qui configure l'ADC sur la Pin souhait�e (16 possibles, voir ci-dessus).
 * La fonction est � lancer qu'une seule fois. Elle configure automatiquement la Pin
 * choisie en INPUT, ANALOG.
 * Si l'utilisateur veut pouvoir traiter une autre entr�e, il suffit simplement de configurer
 * la Pin voulue en INPUT, ANALOG (voir GPIO). Pas besoin de relancer la fonction de config g�n�rale.
 * NB : le temps de pr�l�vement de la tension est fix�e � 10�s.
 * @param Pin : pin d'entr�e de l'ADC  voir d�finitions ci-dessus
 */ 
void ADC_Global_Conf(int Pin);





/**
 * C'est une fonction permettant :
 * de lancer une conversion sur la Pin voulue
 * d'attendre la fin de conversion
 * de r�cup�rer le r�sultat de conversion (entier de 0 � 4095) puisque l'ADC est un 12 bits.
 * @param Pin : pin d'entr�e de l'ADC  voir d�finitions ci-dessus
 */ 
int ADC_Get_Value(int Pin);



#endif
