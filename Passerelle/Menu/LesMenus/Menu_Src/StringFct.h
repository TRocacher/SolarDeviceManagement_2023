/*
 * StrinFct.h
 *
 *  Created on: 26 avr. 2021
 *      Author: trocache
 */

#ifndef MENU_STRINGFCT_H_
#define MENU_STRINGFCT_H_


/**
  * @brief  Transforme un nombre float dans un format supposé en un suite
  * de digits avec la virgule au bon endroit et le signe en première postion
  * Le format final est un string de 9 caractères de la forme (exemple 6 digits, 2 décimales) :
  *  	| 'Sign' '1' '3 '4' '2' '.' '5' '4' 0 |
  *
  * @Note 1 : Le user soit absulument faire attention au format 9 bytes afin de ne pas
  * produire des effets de bord.
  *
  * @Note 2: Valide si si DigitNbr dans  [1 - 6] &&  Frac dans [0 - 3] && si DigitNbr > Frac
  *
  * @param  float Value : la valeur à traiter
  * @param  char * DigitTab : l'adresse de la table de 9 bytes à écrire
  * @param  char DigitNbr : le nombre de digit à retranscrire (hors virgule)
  * @param  char Frac : le nombre de digit de la partie décimale
  * @retval 1 si OK, 0 si mal passé.
  **/
char StringFct_Float2Str(float Value,char * DigitTab, char DigitNb, char Frac);




/**
  * @brief  transforme une chaîne de caractères type :
  *   | 'Sign' '1' '3 '4' '2' '.' '5' '4' 0 |
  *   en un float, par exemple 1342.54 en 6/2 (normal)
  *   par contre donnera 134254 en 6/0 (anormal)
  *   Donc le user doit être cohérent !
  *
  * @param  char * DigitTab : l'adresse de la table de 9 bytes à traiter
  * @param  char DigitNbr : le nombre de digit à retranscrire (hors virgule)
  * @param  char Frac : le nombre de digit de la partie décimale
  * @retval valeur float ou 1100000.0 si erreur
  *
  * @Note : exemple d'utilisation :
  * Valeur=StringFct_Str2Float(MyString,6,2);
  * if (Valeur>1000000.0) MenuInput_PrintPlantageConv();
  **/
float StringFct_Str2Float(char * DigitTab, char DigitNb, char Frac);



#endif /* MENU_STRINGFCT_H_ */
