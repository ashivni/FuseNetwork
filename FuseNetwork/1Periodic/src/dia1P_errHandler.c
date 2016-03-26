/**************************************************** 
 *		filename:   dia1P_errHandler.c            *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.1 (Created 1st June 09)       *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				| 14th June 09 
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_errHandler. The purpose of this function
	is to take appropriate action if any errors occur
	during the exection of the program dia1P or any
	of its subroutines.
*/

#include "dia1P.h"

void dia1P_errHandler(int ErrNo, char *fromFunc, char *pervFunc, char *mesg)
{
	char *name = "dia1P_errHandler";
	extern struct dia1P_probDef pD;

	if(pD.diagFile==NULL)
	{
		printf("No diagnostic file set. TERMINATING.\n");
		exit(0);
	}
	
	if(dia1P_isFatal(ErrNo,name) == 1)
	{
		fprintf(pD.diagFile,"Fatal error encountered in function %s. Program will TERMINATE.\n",fromFunc);	
		fprintf(pD.diagFile,"Details:\n Error Code: %d\n Previous function: %s\n Message: %s\n",ErrNo,pervFunc,mesg);	
		exit(0);
	}
	else
	{
		fprintf(pD.diagFile,"Non-fatal error encountered in function %s. Program will continue after printing this message.\n",fromFunc);	
		fprintf(pD.diagFile,"Details:\n Error Code: %d\n Previous function: %s\n Message: %s\n",ErrNo,pervFunc,mesg);	
	}
}
