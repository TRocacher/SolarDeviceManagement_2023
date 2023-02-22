#ifndef _EXCHCTRL_H__
#define _EXCHCTRL_H__




//**************************************************************************************************************
//**************************************************************************************************************
// 							COUCHE ExchangeCtrl
//**************************************************************************************************************
//**************************************************************************************************************

#define ExchangeCtrlLayer_Prio (PhyUART_FSM_Prio+1)  // priorit� FSM imm�diatement en sous de la couche PhyUART (moins prioritaire)
#define TIM_ExchangeCtrlLayer TIM3





void ExchCtrl_Init(void);
void ExchCtrlArrival_StartFSM(void);

// IO fct
int Exch_SendNewMssg (char * AdrString, int Len);

#endif

