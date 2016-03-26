/**************************************************** 
 *		filename:   dia1P_freeBrokenBonds.c     *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.2 (1.1 Created 17th June 09) 	*
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|17st June 09
 ----------------------------------------------------
	1.2		| Updated to handle 		|21st June 09
			| avalanche related arrays  | 
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_freeBrokenBonds. The purpose of this function
	is to free the memory allocated to the pointers 
	in the datastructure dia1P_brokenBonds
*/

#include "dia1P.h"

void dia1P_freeBrokenBonds(dia1P_brokenBonds **BB, char *caller)
{
	char *name = "dia1P_freeBrokenBonds";
	if(BB == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,"NULL pointer in arguments");
	}
	
	// Free memory
	free((*BB)->i);
	free((*BB)->j);
	free((*BB)->BondArr);
	free((*BB)->bondStrength);
	free((*BB)->v);
	free((*BB)->c);
	
	free(*BB);
}
