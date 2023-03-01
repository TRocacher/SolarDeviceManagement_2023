#ifndef MENUTYPE_H_
#define MENUTYPE_H_

/*
*/



enum {First, Middle, Last};



typedef struct
{
	float Val;
	float MinVal;
	float MaxVal;
	char DigitNb;
	char DecimalNb;
	char NewUserInputFlag;
} Menu_ParamValTypedef;




typedef struct Menu_Node Menu_Node;
struct Menu_Node {
	Menu_Node *  NextNode;
	Menu_Node *  PreviousNode;
	Menu_Node *  UpNode;
	Menu_Node *  DownNode;
	char * Title;
	char MenuPosition;
	char * ItemToPrint;
	void (*NodeExecFct)(void);
	Menu_ParamValTypedef * Param;
};



extern void Menu_PassivePrint(void);
extern void Menu_InputStateMachine(void);


// Param nul
extern Menu_ParamValTypedef ParamNull;

#endif
