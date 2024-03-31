#include "TimeStampManagement.h"

/* =============================================
				TimeStampManagement

 Module qui g�re les horodatages.
 Associ� � un timer programm� � 1 seconde
 
 ============================================= */

/*---------------------------------
 Variable pricipale TimeStampClock
----------------------------------*/
TimeStampTypedef TimeStampClock={0,0,0,1,1,2024}; 


/*---------------------------------
 Variable locale
----------------------------------*/
const int BissextilDayPerMonth[12]={31,29,31,30,31,30,31,31,30,31,30,31};
const int NonBissextilDayPerMonth[12]={31,28,31,30,31,30,31,31,30,31,30,31};


void TimeStamp_SecInc(TimeStampTypedef * Ptr);
void TimeStamp_Update(void); // Callback 1 sec
int TimeStamp_GetDayAbsolute(TimeStampTypedef * Ptr);



/*---------------------------------
 FONCTIONS PUBLIQUES
----------------------------------*/

/**
  * @brief  Initialise TIMER_TimeStamp � une seconde et le lance.
	*					associe le callback
  **/
void TimerStamp_Start(void)
{
	Timer_CkEnable(TIMER_TimeStamp);
	Timer_Set_Period(TIMER_TimeStamp, 10000-1, 7200-1 ); // p�riode 1 sec
	Timer_IT_Enable( TIMER_TimeStamp, 0, TimeStamp_Update);
}


/**
  * @brief  Renvoie l'horodatage r�el
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
TimeStampTypedef * TimeStamp_GetClock(void)
{
	return &TimeStampClock;
}


/**
* @brief  convertit le timestamp en seconde � partir du 1 Jan 2024, 00:00:00
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int TimeStamp_2_Sec(TimeStampTypedef * Ptr)
{
	int Duree_Sec;
	Duree_Sec=(Ptr->Sec)+(Ptr->Min)*60+(Ptr->Hour)*3600\
	+ 24*3600*TimeStamp_GetDayAbsolute(Ptr) ;
	return Duree_Sec;
}


/**
* @brief  D�termine la diff�rence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int TimeStamp_substract(TimeStampTypedef * PtrA,TimeStampTypedef * PtrB )
{
	return (TimeStamp_2_Sec(PtrA)-TimeStamp_2_Sec(PtrB));
}


/**
* @brief  convertit le hourstamp en seconde � partir de, 00h:00mn:00s
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int HourStamp_2_Sec(HourStampTypedef * Ptr)
{
	
}

/**
* @brief  D�termine la diff�rence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int HourStamp_substract(HourStampTypedef * PtrA,HourStampTypedef * PtrB );



/*---------------------------------
 FONCTIONS PRIVEES
----------------------------------*/


/**
  * @brief  Callback priv�. Incr�mente le TimeStamp en temps r�el
	*					appel� par TIMER_TimeStamp r�gl� � 1 sec de p�riode
  **/
void TimeStamp_Update(void)
{
	TimeStamp_SecInc(&TimeStampClock);
}

/**
  * @brief  Private Incr�mente de 1 seconde le timestamp donn� en argument
	* @param  pointeur sur la variable timestamp � incr�menter
  **/
void TimeStamp_SecInc(TimeStampTypedef * Ptr)
{
	short int Sec,Min,Hour,Day, Month, Year;
	char DayMax; 	
	// recopie locale
	Sec=Ptr->Sec;
	Min=Ptr->Min;
	Hour=Ptr->Hour;
	Day=Ptr->Day;
	Month=Ptr->Month;
	Year=Ptr->Year;
	
	if (Year%4==0) 	DayMax=BissextilDayPerMonth[Month-1]+1; 
	else DayMax=NonBissextilDayPerMonth[Month-1]+1;  // Month de 1 � 12 !
		
			
	// incr�mentation locale
	Sec++;
	if (Sec==60)
	{
		Sec=0;
		Min++;
		if (Min==60)
		{
			Min=0;
			Hour++;
			if (Hour==24)
			{
				Hour=0;
				Day++;
				if (Day==DayMax)
				{
					Day=1;
					Month++;
					if (Month==13)
					{
						Month=1;
						Year++;
					}
				}
			}
		}
	}
	
  // Affectation structure point�e	
	Ptr->Sec=Sec; 
	Ptr->Min=Min;
	Ptr->Hour=Hour;
	Ptr->Day=Day;
	Ptr->Month=Month;
	Ptr->Year=Year;
}



/**
  * @brief  Private Renvoie le nombre de jours �coul�s de puis 1/Jan/2024 0h00s.
	* @param  pointeur sur la variable timestamp dont on compte les jours
  **/
int TimeStamp_GetDayAbsolute(TimeStampTypedef * Ptr)
{
	int BissextilCumulatePastDayPerMonth[12],NonBissextilCumulatePastDayPerMonth[12];
	int Bissextil_Nb;
	int NonBissextil_Nb;
	int PassedYearNb;
	int DayAbs;
	int i;
	int year;
	
	BissextilCumulatePastDayPerMonth[0]=0; // cumul� mois de janvier
	NonBissextilCumulatePastDayPerMonth[0]=0; // cumul� mois de janvier
	for (i=1;i<12;i++)
	{
		BissextilCumulatePastDayPerMonth[i]=BissextilCumulatePastDayPerMonth[i-1]\
																				+BissextilDayPerMonth[i];
		NonBissextilCumulatePastDayPerMonth[i]=NonBissextilCumulatePastDayPerMonth[i-1]\
																				+NonBissextilDayPerMonth[i];
	}
	// d�termination du nbre d'ann�es bissextiles pass�es
	Bissextil_Nb=0;
	year=0;
	PassedYearNb=(Ptr->Year)-2024;
	for (i=0;i<=PassedYearNb;i++)
	{
		year=2024+i;
		if ((year%4)==0) Bissextil_Nb++;
	}
	NonBissextil_Nb=PassedYearNb-Bissextil_Nb;
	
  DayAbs=NonBissextil_Nb*365+Bissextil_Nb*366; 
	if (((Ptr->Year)%4)==0) // ann�e en cours bissextile
	{
		DayAbs+=BissextilCumulatePastDayPerMonth[Ptr->Month];
	}	
	else // ann�e en cours non bissextile
	{
		DayAbs+=NonBissextilCumulatePastDayPerMonth[Ptr->Month-1]; // Month de 1 � 12, � passer de 0 � 11
	}		
	DayAbs+=(Ptr->Day)-1; // Les jours d�marrent � 1. Donc il faut enlever 1.
	
	return (DayAbs);
}




