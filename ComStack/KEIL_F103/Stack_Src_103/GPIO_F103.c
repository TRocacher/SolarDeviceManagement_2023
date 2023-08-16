//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//

//=======================================================================================
// 	Auteur T.R.
//  12 Mai 2010, 
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 4.53 
//  Module qui configure les I/O, plus d'info ds le Port_IO.h
//  Release : Juin 2011 conf des ports D et E pour le STM32 107
//  modif Jan 2013 pour compatibilité 4_53 : stm32f10x.h et Clock.c
//  Clé compil : Pas besoin, configurée pour le 107, donc GPIOD et E accessibles
//=======================================================================================


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "GPIO_F103.h"




char GPIO_Configure(GPIO_TypeDef * Port, int Broche, int Sens, int Techno)


// Doc tech : STM32F103RM.pdf
// Port : GPIOA à GPIOC
// Broche : de 0 à 15 pour GPIOA, GPIOB, 

// Sens   : I pour entree, O pour sortie

// Techno : 0 Analog						  / Output push-pull
//			1 Input floating				  / Output open-drain
//			2 Input-pull-down / up			  / Alternate function push-pull
//			3 reserved						  / Alternate function open-drain
// 
// Retourne 0 si tout est OK, 1 sinon
{
		
	
char BrocheH;
char Erreur;
char Sens_1_0;

	// Activer horloges A, B et C
	if (Port==GPIOA) (RCC->APB2ENR)=(RCC->APB2ENR)|RCC_APB2ENR_IOPAEN;
	if (Port==GPIOB) (RCC->APB2ENR)=(RCC->APB2ENR)|RCC_APB2ENR_IOPBEN;
	if (Port==GPIOC) (RCC->APB2ENR)=(RCC->APB2ENR)|RCC_APB2ENR_IOPCEN;

	if (Port==GPIOD) (RCC->APB2ENR)=(RCC->APB2ENR)|RCC_APB2ENR_IOPDEN;
	if (Port==GPIOE) (RCC->APB2ENR)=(RCC->APB2ENR)|RCC_APB2ENR_IOPEEN;

	
	// Verifications	 générales
	// vérification  nbre de broche
	Erreur=0; //pas de pb à priori
	if (Broche>15)	Erreur = 1;
	
	//verification que la "combinaison reservée" n'est pas demandée
	if ((Sens == 'i' || Sens =='I') && Techno == 3) Erreur=1;
	
	// vérification depassement valeur de techno de 0 à 3
	if (Techno>3) Erreur=1;
	
	// verification valeur de Sens
	
	if (Sens != 'i' && Sens != 'I' && Sens != 'o' && Sens != 'O' ) Erreur=1;
	
	//------------------------------------------
	// Configuration proprement dite :
	//------------------------------------------
	// Techno, pull up, pushpull, analog...
	
	// Sens de la broche (Input ou Output 10MHz, on se prive de la sortie 50MHz...)
	if (Sens == 'i' || Sens == 'I') Sens_1_0=0;
	if (Sens == 'o' || Sens == 'O') Sens_1_0=1;
	
	// But: fixer la techno de la broche en question 
	if (Broche <8)
	{
		// Exemple Broche = 5, Techno = 2
		// il faut arriver à CRL = xxxx xxxx | 10xx xxxx | xxxx xxxx | xxxx xxxx
		// mise à 0 du champ
		Port->CRL=(Port->CRL)&~(0x3<<(4*Broche+2));
		// ecriture de la valeur
		Port->CRL=(Port->CRL) | Techno<<(4*Broche+2); // les bits à 1 sont en place
	
	    // Mise à 0 de la zone de 2 bits
		Port->CRL=(Port->CRL) & ~(0x3<<(4*Broche)); 
		// Ecriture du sens par un OU
		Port->CRL=(Port->CRL) | Sens_1_0<<(4*Broche);  
 	}
	else
    // idem mais sur le registre CRH
 	{
		BrocheH=Broche-8;
		// mise à 0 du champ
		Port->CRH=(Port->CRH)&~(0x3<<(4*BrocheH+2));
		// ecriture de la valeur
		Port->CRH=(Port->CRH) | Techno<<(4*BrocheH+2); // les bits à 1 sont en place
		// Mise à 0 de la zone de 2 bits
		Port->CRH=(Port->CRH) & ~(0x3<<(4*BrocheH)); 
		// Ecriture du sens par un OU
		Port->CRH=(Port->CRH) | Sens_1_0<<(4*BrocheH); 
   	}

	return Erreur;
}



//=======================================================================================
// 	Auteur T.R.
//  02 Oct 2010, Modif 18 Jan 2013
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 4.53
//  Module qui configure le timer système Systick
//=======================================================================================


//========================================================================================
// Définition des pointeurs de fonction d'IT
//=======================================================================================

static void (*Ptr_EXTI0)(void);	
static void (*Ptr_EXTI1)(void);	
static void (*Ptr_EXTI2)(void);	
static void (*Ptr_EXTI3)(void);	
static void (*Ptr_EXTI4)(void);
static void (*Ptr_EXTI9_5)(void);	
static void (*Ptr_EXTI15_10)(void);		


//========================================================================================
// Validation des lignes d'interruption (hors coeur), 0 à 63
//=======================================================================================

void NVIC_Enable_IT(char NumeroIT)
// 
{
char Numero_Rang;
Numero_Rang = NumeroIT % 32; // modulo
if (NumeroIT < 32)
	{
	NVIC->ISER[0]= (1<<Numero_Rang);
	}
else if	(NumeroIT < 64)
	{
	NVIC->ISER[1]= (1<<Numero_Rang);
	}
}

//========================================================================================
// Invalidation des lignes d'interruption (hors coeur), 0 à 63
//=======================================================================================

void NVIC_Disable_IT(char NumeroIT)
{
char Numero_Rang;	////////////////////////////////////////////////////BORDEL A VOIR SEB
Numero_Rang = NumeroIT % 32; // modulo
if (NumeroIT < 32)
	{
	NVIC->ICER[0]= (1<<Numero_Rang);
	}
else if	(NumeroIT < 64)
	{
	NVIC->ICER[1]=(1<<Numero_Rang);
	}
}

//========================================================================================
// Fixer la priorité des lignes d'interruption (hors coeur), 0 à 63
//=======================================================================================

void NVIC_Prio_IT(char NumeroIT, char Prio)
{	
NVIC->IP[(int)NumeroIT]=Prio<<4;	
}
	  


//========================================================================================
// place la broche du port indiquée en entree IT sur front up/down/les deux
//=======================================================================================

char NVIC_Ext_IT (GPIO_TypeDef * Port, char Broche, char Front, char Techno, char Prio, void (*IT_function) (void))
{
char Vect_IT;
char Erreur;
int Reg_Num,Position_Dans_Reg;
char Valeur_EXTI; // EXTI est le nom du champ qui fixe le choix entre les 3ports:
// EXTI = 000 port A.x
// EXTI = 001 port B.x
// EXTI = 010 port C.x
Erreur=0;
	
//============== MODIF TR 12 Jan 2015==================
// Valider horloge AFIO
(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_AFIOEN;	
//============== Fin MODIF TR 12 Jan 2015==============	
	
// Placer la broche en entrée et tester si erreur:
if (Port_IO_Init(Port, Broche, 'i', Techno)==0 )
{
// Aiguiller la broche vers EXTi avec i = broche (i ème IT externe)
	if (Port==GPIOA) Valeur_EXTI=0;
	else if (Port==GPIOB) Valeur_EXTI=1;
	else if (Port==GPIOC) Valeur_EXTI=2;

	Reg_Num=Broche/4; // broche entre 0 et 3 donne 0, entre 4 et 7 donne 1...
	Reg_Num=Reg_Num;
	Position_Dans_Reg=Broche%4; // exemple Broche = 5, Position_Dans_Reg=1
	// a ce stade, Position_Dans_Reg vaut 0,1,2 ou 3
	Position_Dans_Reg=Position_Dans_Reg*4;
	// a ce stade, Position_Dans_Reg vaut 0,4,8 ou 12 
	// ce qui correspond à la position des champ EXTIx(3:0) ds le reg

	//mise à 0 du champ EXTIx(3:0) au rang qui va bien
	AFIO->EXTICR[Reg_Num]=(AFIO->EXTICR[Reg_Num])&~(0xF<<Position_Dans_Reg);
	// Ecriture de la valeur
	AFIO->EXTICR[Reg_Num]=(AFIO->EXTICR[Reg_Num])|(Valeur_EXTI<<Position_Dans_Reg);

   	// On demasque l'IT externe requise
	EXTI->IMR=EXTI->IMR|1<<Broche;
	// configuration du front
	switch(Front)
	{
	case 0:	// front descendant
		EXTI->RTSR=(EXTI->RTSR)&~(1<<Broche);
		EXTI->FTSR=(EXTI->FTSR)|(1<<Broche);
		break;
	case 1:	// front montant
		EXTI->RTSR=(EXTI->RTSR)|(1<<Broche);
		EXTI->FTSR=(EXTI->FTSR)&~(1<<Broche);
		break;
	case 2:	// front montant & descendant
		EXTI->RTSR=(EXTI->RTSR)|(1<<Broche);
		EXTI->FTSR=(EXTI->FTSR)|(1<<Broche);
		break;		
	default	: Erreur=1;
	}

// Reste à valider l'interruption ds le NVIC. Il s'agit de l'IT externe n°broche  
	// determination du n°IT avec la broche (broche = n°IText
	if (Broche<=4)
		{
		Vect_IT=6+Broche; // fait avec la table p172
		if (Broche==0) Ptr_EXTI0= IT_function;
		if (Broche==1) Ptr_EXTI1= IT_function;
		if (Broche==2) Ptr_EXTI2= IT_function;
		if (Broche==3) Ptr_EXTI3= IT_function;	
		if (Broche==4) Ptr_EXTI4= IT_function;
		}
	if (Broche>=5 && Broche<=9)
		{
		Vect_IT=23; // fait avec la table p172
		Ptr_EXTI9_5= IT_function;
		}
	if (Broche>=10 && Broche<=15)
		{
		Vect_IT=40; // fait avec la table p172
		Ptr_EXTI15_10= IT_function;
		}
	NVIC_Enable_IT(Vect_IT);
	NVIC_Prio_IT(Vect_IT,Prio);
}
else
{
	Erreur =1;
}
// 
return Erreur;
}


//========================================================================================
// Handlers, saut aux fonctions associées
//=======================================================================================

void EXTI0_IRQHandler (void)
{
Ptr_EXTI0();// Appel de la fonction d'IT définie par le user
EXTI->PR = (EXTI->PR)|(0x01<<0);
}
void EXTI1_IRQHandler (void)
{
Ptr_EXTI1();// Appel de la fonction d'IT définie par le user
EXTI->PR = (EXTI->PR)|(0x01<<1);
}
void EXTI2_IRQHandler (void)
{
Ptr_EXTI2();// Appel de la fonction d'IT définie par le user
EXTI->PR = (EXTI->PR)|(0x01<<2);
}
void EXTI3_IRQHandler (void)
{
Ptr_EXTI3();// Appel de la fonction d'IT définie par le user
EXTI->PR = (EXTI->PR)|(0x01<<3);
}
void EXTI4_IRQHandler (void)
{
Ptr_EXTI4();// Appel de la fonction d'IT définie par le user
EXTI->PR = (EXTI->PR)|(0x01<<4);
}
void EXTI9_5_IRQHandler (void)
{
Ptr_EXTI9_5();// Appel de la fonction d'IT définie par le user
}
void EXTI15_10_IRQHandler (void)
{
Ptr_EXTI15_10();// Appel de la fonction d'IT définie par le user
}


