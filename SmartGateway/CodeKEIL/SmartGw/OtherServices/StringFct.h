/*
 * StrinFct.h
 *
 *  Created on: 26 avr. 2021
 *      Author: trocache
 */

#ifndef MENU_STRINGFCT_H_
#define MENU_STRINGFCT_H_

/**
  * @brief  Transforme un quartet en son code ASCII.
  * @Note 1 : Exemple Quartet=0xB cad 11. Val = 11-10 +65 = 1+65 =66 = code ASCII de B
  * @param  Quarter sur 4 bits... donc entre 0 et 15, 0x et 0xF
  * @retval valeur ASCII correspondante
  **/
char ConvQuartetToAscii(char Quartet);


/**
  * @brief  Transforme un int 16 bits en un string  ASCII.
  * @Note 1 : Max 65535 donc il faut 5 caractères ASCII
  * @Note 2 : fonction mal nommée, puisqu'il s'agit d'un short int et non un int !!
  * @Note 3 :  ATTENTION DigitTab doit être de 6 (5 + null) !!!!!! si en dessous effet de bord garanti
  * @param  Valeur, celle du int
	* @param  pointeur sur la chaine de caractère
  **/
	
/* ATTENTION DigitTab doit être de 5 !!!!!! si en dessous effet de bord garanti*/
void StringFct_Int2Str(int Value,char * DigitTab);

/**
  * @brief  Transforme un int 16 bits de 0 à 99 !!  en un string  ASCII.
  * @Note 1 : Max 99 donc il faut 2 caractères ASCII + null
  * @Note 2 : fonction mal nommée, puisqu'il s'agit d'un short int et non un int !!
  * @Note 3 :  ATTENTION DigitTab doit être de 2 !!!!!! si en dessous effet de bord garanti
  * @param  Valeur, celle du int
	* @param  pointeur sur la chaine de caractère qui se termine par null
  **/
void StringFct_Int2Str_99(int Value,char * DigitTab);

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
char StringFct_Float2Str(float Value,char * DigitTab, int DigitNb, int Frac);



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
float StringFct_Str2Float(char * DigitTab, int DigitNb, int Frac);


#endif /* MENU_STRINGFCT_H_ */
