/* =================================================================================
* ==================   DelayMngt.c	     =================================
 *
 *   Created on: 30/03/24
 *   Author: T.Rocacher
 *   Tool : KEIL V5.34
 *   Target : STM32F103RTB6
 *   Dépendance : 
 *  ------------------------------------------------------------------------------
 

 *
* =================================================================================*/



#include "DelayMngt.h"
#include "TimeStampManagement.h"
#include "GLOBAL_SMARTGATEWAY.h"



/* ===============================================
          
================================================== */

/*---------------------------------
 DEFINITIONS DE LA VARIABLE ScheduleTab
----------------------------------*/
#define hmin 6
#define hmax 23
#define ScheduleTabLen ((((hmax-hmin)+1)*2)) /* Taille tableau si étendue 6h00 à 23h00 = 36 élts*/
ScheduleStep_typedef ScheduleTab[ScheduleTabLen]; /* contient toutes les heures de hmin à hmax y compris les 1/2h*/

/*
Exemple hmin =1, hmax=2
((((hmax-hmin))*2)+1)=((((2-1))*2)+1) = 3, ce qui correspond à 1h00 1h30 et 2h00
*/

char DelayMngt_RealTimeIdx;
void DelayMngt_UpdateRealTimeIdx(void);

/**
* @brief  Initialise la table SheduleTab en particulier :
	- fixe les heures souhaitées de 6h00 à 23h00 inclu
	- tous les champs à 0 / noWarning de chaque structure RmDvInfo_typedef
  * @retval le nombre de seconde
  **/
void Init_ScheduleTab(void)
{
	int i,j;
	HourStampTypedef LocalHStamp, Shift;
	LocalHStamp.Hour=6;
	LocalHStamp.Min=0;
	LocalHStamp.Sec=0;
	Shift.Hour=0;
	Shift.Min=30;
	Shift.Sec=0;
	
	for (i=0;i<ScheduleTabLen;i++)
	{
		ScheduleTab[i].DesiredStamp.Hour=LocalHStamp.Hour;
		ScheduleTab[i].DesiredStamp.Min=LocalHStamp.Min;
		ScheduleTab[i].DesiredStamp.Sec=LocalHStamp.Sec;
		for (j=0;j<4;j++)
		{
			ScheduleTab[i].RmDvInfoTab[j].MssgOnTime=0;
			ScheduleTab[i].RmDvInfoTab[j].Stamp.Hour=0;
			ScheduleTab[i].RmDvInfoTab[j].Stamp.Min=0;
			ScheduleTab[i].RmDvInfoTab[j].Stamp.Sec=0;
			ScheduleTab[i].RmDvInfoTab[j].StampGap=0;
			ScheduleTab[i].RmDvInfoTab[j].TransactionStatus=Status_NoWarning;
		}
		/* stamp+= 30' */
		HourStampAdd(&LocalHStamp,&Shift);
	}
	HourStamp_30mnCallbackAssociation(DelayMngt_UpdateRealTimeIdx);
}


void DelayMngt_UpdateRealTimeIdx(void)
{
	TimeStampTypedef* PtrStamp;
	int LocalIdx;
	PtrStamp=TimeStamp_GetClock();
	LocalIdx=(((PtrStamp->Hour)+1)-hmin)*2; 
	/* hyp hmin=6 hmax = 23 
	5h15 LocalIdx=0		voulu -1
	5h45 LocalIdx=0  	voulu 0  
	6h15 LocalIdx=2		voulu 1
	6h45 LocalIdx=2		voulu 2
	7h15 LocalIdx=4		voulu 3
	7h45 LocalIdx=4		voulu 4
	
	22h15 LocalIdx=34	
	22h45 LocalIdx=34
	23h15 LocalIdx=36
	23h45 LocalIdx=36
	*/
	if ((PtrStamp->Min>=15)&&(PtrStamp->Min<45)) LocalIdx=LocalIdx-1;
	/* hyp hmin=6 hmax = 23 
	5h15 LocalIdx=0 -1		voulu -1
	5h45 LocalIdx=0 0 	voulu 0  
	6h15 LocalIdx=2	1	voulu 1
	6h45 LocalIdx=2	2	voulu 2
	7h15 LocalIdx=4	3	voulu 3
	7h45 LocalIdx=4	4	voulu 4
	
	22h15 LocalIdx=34	33 
	22h45 LocalIdx=34 34
	23h15 LocalIdx=36 35  <- Dernier élement
	23h45 LocalIdx=36 36
	*/
	
	/* 0 1 .... 35 35 35 35 35 ... 0 1 2*/
	
	
	if (LocalIdx>=(ScheduleTabLen-1)) LocalIdx=ScheduleTabLen-1; /* passé 23h30 on reste bloqué*/
	if (LocalIdx<0)  LocalIdx=ScheduleTabLen-1;  /* idem, après minuit on reste bloqué au max */
	DelayMngt_RealTimeIdx=LocalIdx;
	
}
