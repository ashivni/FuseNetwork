/**************************************************** 
 *		filename:   dia1P_isBroken.c			*
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.1 (Created 14th June 09)      *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_isBroken. The purpose of this function
	is to determine if the specimen is broken.
	returns 1 if the sample is broken, 0 otherwise
*/

#include "dia1P.h"

int dia1P_isBroken(cholmod_sparse *NodeV_s, cholmod_sparse *V2C, cholmod_common *Common, char *caller)
{

	char *name = "dia1P_isBroken";
	if(NodeV_s == NULL || V2C == NULL || Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}

	// Vector of currents flowing into the bottom row
	cholmod_sparse *J;
	
	// Find the currents
	J = cholmod_ssmult(V2C,NodeV_s,0,1,0,Common);

	int broken = 0;

	// If the norm of the current vector is small, then the 
	// sample is borken.
	if(cholmod_norm_sparse(J,0,Common) < dia1P_Tiny)
	{
		broken = 1;
	}
	else
	{
		broken = 0;
	}
	cholmod_free_sparse(&J,Common);

	return broken;
}
