#include "TimeStampManagement.h"

/* =============================================
				TimeStampManagement

 Module qui gère les horodatages.
 Associé à un timer programmé à 1 seconde
 
 ============================================= */
void (*PtrFct)(void)=0;

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
  * @brief  Initialise TIMER_TimeStamp à une seconde et le lance.
	*					associe le callback
  **/
void TimerStamp_Start(void)
{
	Timer_CkEnable(TIMER_TimeStamp);
	Timer_Set_Period(TIMER_TimeStamp, 10000-1,7200-1 ); // période 1 sec 
	//Timer_Set_Period(TIMER_TimeStamp, 100-1, 7200-1 ); // période 10m sec, cad 30mn = 18 secondes réel
	Timer_IT_Enable( TIMER_TimeStamp, 0, TimeStamp_Update);
}

/**
* @brief  associe un callback qui claque toutes les 30mn avec un décalage de 15mn.
* @paral @ fonction
**/
void HourStamp_30mnCallbackAssociation(void (*IT_function) (void))
{
	PtrFct=IT_function;
}


/**
  * @brief  Renvoie l'horodatage réel
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
TimeStampTypedef * TimeStamp_GetClock(void)
{
	return &TimeStampClock;
}


/**
  * @brief  Met à l'heure l'horloge 
  * @param pointeur sur le stamp contenant l'heure voulue
  **/
void TimeStamp_SetClock(TimeStampTypedef * Stamp)
{
	TimerOff(TIMER_TimeStamp);
	TimeStampClock.Year=Stamp->Year;
	TimeStampClock.Month=Stamp->Month;
	TimeStampClock.Day=Stamp->Day;
	TimeStampClock.Hour=Stamp->Hour;
	TimeStampClock.Min=Stamp->Min;
	TimeStampClock.Sec=Stamp->Sec;
	TimerStamp_Start();
}



/**
* @brief  remet à 0 une structure de type TimeStampTypedef
  * @retval _
  **/
void TimeStamp_ResetStamp(TimeStampTypedef * Ptr)
{
	Ptr->Year=0;
	Ptr->Month=0;
	Ptr->Day=0;
	Ptr->Hour=0;
	Ptr->Min=0;
	Ptr->Sec=0;
}


/**
* @brief  convertit le timestamp en seconde à partir du 1 Jan 2024, 00:00:00
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
* @brief  Détermine la différence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int TimeStamp_substract(TimeStampTypedef * PtrA,TimeStampTypedef * PtrB )
{
	return (TimeStamp_2_Sec(PtrA)-TimeStamp_2_Sec(PtrB));
}


/* ===============================
        Fonction heure/min/sec
==================================*/

/**
* @brief  convertit le hourstamp en seconde à partir de, 00h:00mn:00s
  * @retval pointeur sur la variable TimeStamp, TimeStampClock
  **/
int HourStamp_2_Sec(HourStampTypedef * Ptr)
{
	int Duree_Sec;
	Duree_Sec=(Ptr->Sec)+(Ptr->Min)*60+(Ptr->Hour)*3600;
	return Duree_Sec;
}

/**
* @brief  A=A+B
  * @retval none
  **/
void HourStampAdd(HourStampTypedef * PtrA, HourStampTypedef * PtrB )
{
	HourStampTypedef  res;
	res.Sec = PtrA->Sec + PtrB->Sec;
	res.Min = res.Sec/60;
	res.Sec = res.Sec%60;
	
	res.Min = res.Min + PtrA->Min + PtrB->Min;
	res.Hour = res.Min/60;
	res.Min = res.Min%60;
	
	res.Hour = res.Hour + PtrA->Hour + PtrB->Hour;
	
	PtrA->Sec=res.Sec;
	PtrA->Min=res.Min;
	PtrA->Hour=res.Hour;
	
}



/**
* @brief  Détermine la différence en secondes entre le TimeStamp A et le B
	*	Resultat = Sec A - Sec B
  * @retval le nombre de seconde
  **/
int HourStamp_substract(HourStampTypedef * PtrA,HourStampTypedef * PtrB )
{
	return (HourStamp_2_Sec(PtrA)-HourStamp_2_Sec(PtrB));
}





/*---------------------------------
 FONCTIONS PRIVEES
----------------------------------*/


/**
  * @brief  Callback privé. Incrémente le TimeStamp en temps réel
	*					appelé par TIMER_TimeStamp réglé à 1 sec de période
  **/
void TimeStamp_Update(void)
{
	TimeStamp_SecInc(&TimeStampClock);

	if ((TimeStampClock.Sec==0)&&((TimeStampClock.Min==15)||(TimeStampClock.Min==45)))
	{
		if (PtrFct!=0) PtrFct(); 
	}
}

/**
  * @brief  Private Incrémente de 1 seconde le timestamp donné en argument
	* @param  pointeur sur la variable timestamp à incrémenter
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
	else DayMax=NonBissextilDayPerMonth[Month-1]+1;  // Month de 1 à 12 !
		
			
	// incrémentation locale
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
	
  // Affectation structure pointée	
	Ptr->Sec=Sec; 
	Ptr->Min=Min;
	Ptr->Hour=Hour;
	Ptr->Day=Day;
	Ptr->Month=Month;
	Ptr->Year=Year;
}



/**
  * @brief  Private Renvoie le nombre de jours écoulés de puis 1/Jan/2024 0h00s.
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
	
	BissextilCumulatePastDayPerMonth[0]=0; // cumulé mois de janvier
	NonBissextilCumulatePastDayPerMonth[0]=0; // cumulé mois de janvier
	for (i=1;i<12;i++)
	{
		BissextilCumulatePastDayPerMonth[i]=BissextilCumulatePastDayPerMonth[i-1]\
																				+BissextilDayPerMonth[i];
		NonBissextilCumulatePastDayPerMonth[i]=NonBissextilCumulatePastDayPerMonth[i-1]\
																				+NonBissextilDayPerMonth[i];
	}
	// détermination du nbre d'années bissextiles passées
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
	if (((Ptr->Year)%4)==0) // année en cours bissextile
	{
		DayAbs+=BissextilCumulatePastDayPerMonth[Ptr->Month];
	}	
	else // année en cours non bissextile
	{
		DayAbs+=NonBissextilCumulatePastDayPerMonth[Ptr->Month-1]; // Month de 1 à 12, à passer de 0 à 11
	}		
	DayAbs+=(Ptr->Day)-1; // Les jours démarrent à 1. Donc il faut enlever 1.
	
	return (DayAbs);
}




