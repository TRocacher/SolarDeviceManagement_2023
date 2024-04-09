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
#define hmax 22
#define ScheduleTabLen ((((hmax-hmin)+1)*2)) /* Taille tableau si étendue 6h00 à 22h30 inclu  = 36 élts*/
ScheduleStep_typedef ScheduleTab[ScheduleTabLen]; /* contient toutes les heures de hmin à hmax y compris les 1/2h*/

/*
Exemple hmin =1, hmax=2
((((hmax-hmin)+1)*2)) = ((((2-1)+1)*2)) = 4
ce qui correspond à 1h00 1h30 et 2h00 2h30 
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
	Shift.Min=0;
	Shift.Sec=5;
	
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

/**
* @brief  Renvoie l'indx tps réel
* @note : 
  * @retval index tps reel
  **/
char DelayMngt_GetRealTimeIndex(void)
{
	return DelayMngt_RealTimeIdx;
}


/**
* @brief  Met à jour le champ status de la table ScheduleTable 
* @note : 
  * @retval 
  **/
int DelayMngt_UpdateStatus(char RealTimeIdx, RmDvDataTypedef* RmDvData)
{
	char ID;
	int ID_Idx; /* index du tableau des RmDv*/
	ID=RmDvData->ID;
	ID_Idx=ID-ID_Clim_Salon;
	/* Mémoriser le statut de l'échange*/
	ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].TransactionStatus=RmDvData->Status;
}


/**
* @brief  Met à jour la le prochain délai pour un RmDv donné. 
* @note : 
  * @retval renvoi le délai corrigé en seconde. Si -1 c'est qu'une erreur s'est produite (ID inconnue)
  **/
int DelayMngt_CalculateNextDelay(char RealTimeIdx, char TransactionIdx, RmDvDataTypedef* RmDvData)
{
	/* Le délai est calculé sur la base de l'index tps réel et sur le champ DesiredStamp
	de chacune des ligne du tableau ScheduleTab 
	Si l'index reçu du RmDv ne correspond pas le champ MssgOnTime = 0xFF, sinon 1
	L'erreur de gigue est calculée, puis ajoutée à l'intervalle avec le prochain step du 
	scheduler (ScheduleTab ) 
	Quoiqu'il en soit, le statut de l'échange est mémorisé.
	
	NB : Lorsqu'on est sur le dernier créneau, il faut remettre à 0 la table.
	*/
	
	char ID;
	int ID_Idx; /* index du tableau des RmDv*/
	int Delay;  /* intervalle théorique calculé.*/
	int ArrivalTimeGap;
	char NextTransacIdx;
	
	ID=RmDvData->ID;
	ID_Idx=ID-ID_Clim_Salon;
	if ((ID_Idx)>=0 && (ID_Idx)<=5) /* ID valide ?*/
	{
		/* Stamper la transaction (recopie du stamp RmDvData vers le tableau*/
		ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp.Hour=RmDvData->RmDvTimeStamp.Hour; 
		ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp.Min=RmDvData->RmDvTimeStamp.Min; 
		ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp.Sec=RmDvData->RmDvTimeStamp.Sec; 
		/* Calculer l'écart d'arrivée*/
		ArrivalTimeGap = HourStamp_substract(&ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp, \
																					&ScheduleTab[RealTimeIdx].DesiredStamp);
		ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].StampGap=ArrivalTimeGap;
		
		/* Préciser que la transaction est "à l'heure" au gap près ... ou pas*/
		if (TransactionIdx==RealTimeIdx)
		{
			ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].MssgOnTime=1;
		}
		else
		{
			/* Préciser que la transaction n'est pas "à l'heure" */
			ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].MssgOnTime=0xFF;
		}
		

		/* Calculer le prochain intervalle  : calculer NextStepDesiredStamp-StampActuel */
    NextTransacIdx=(RealTimeIdx+1)%ScheduleTabLen;
		Delay=HourStamp_substract(&ScheduleTab[NextTransacIdx].DesiredStamp,\
																&ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp);
		/* dernier élt 22h30... on peut avoir 6h00 - 22h30 ! ce qui donne un nombre de seconde négatif.
			Si c'est le cas, il faut l'inverser.
		Ou bien on détecte que NextTransacIdx = 0 et on inverse la soustracton calcul...*/
		if (Delay<0) Delay = -Delay; /* cas pour le saut entre 22h30 et 6h00*/
		
		/* Remise à 0 du scheduler lors du dernier fuseau , cad NextIdx = 0*/
		if (NextTransacIdx == 0) 
		{
			Init_ScheduleTab();
		}
		
	}
	else
	{
		Delay=-1;		
	}
	

	
		
	return Delay;
}


/**
* @brief  Détermine l'index de transaction suivant pour émission vers RmDv
* @note : l'index futur est calculé à partir de l'index temps réel et non à partir
					de l'index extrait de la requête. Cela permet de recaler l'ensemble si
					il existe un mismatch entre la réalité et l'index arrivant
  * @retval renvoi l'index futur
  **/
char DelayMngt_CalculateNextTransactionIdx(char RealTimeIdx)
{
	char NextTransacIdx;
	NextTransacIdx=(RealTimeIdx+1)%ScheduleTabLen;
	return NextTransacIdx;
}


/**
* @brief :  calcule le délai réel constaté lors de la dernière transaction
* @note : Renvoie -1 si le calcul ne peut pas être fait (valeur précédente erronée ou non remplie)
* @retval : l'écart en seconde entre le stamp actuel et le stamp passé.
**/
int DelayMngt_Calculate_RealPreviousInterval(char RealTimeIdx,RmDvDataTypedef* RmDvData)
{
	char ID, ID_Idx;
	char LastTransIdx;
	int RealDelay;
	
	
	ID=RmDvData->ID;
	ID_Idx=ID-ID_Clim_Salon;
		/* calcul Ancien */
	LastTransIdx = (RealTimeIdx-1)%ScheduleTabLen;
	if (ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].MssgOnTime==1)
	{
		RealDelay = HourStamp_substract(&ScheduleTab[RealTimeIdx].RmDvInfoTab[ID_Idx].Stamp,\
																&ScheduleTab[RealTimeIdx].RmDvInfoTab[LastTransIdx].Stamp);
	}
	else 
	{
		RealDelay = -1;
	}
	return RealDelay;
}

