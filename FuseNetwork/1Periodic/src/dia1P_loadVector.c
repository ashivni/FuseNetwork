/**************************************************** 
 *		filename:   dia1P_loadVector.c		    *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.3 (1.1 Created 14th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
	1.2		| Updated for better diagn-	|21st June 09 
			| ostic and output handling |
 ----------------------------------------------------
	1.3		| Updated for the N vs N-1	|26st June 09 
			| issue in periodic BCs		|
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_loadVector. The purpose of this function
	return a dense load vector for the problem.
*/

#include "dia1P.h"

cholmod_sparse *dia1P_loadVector(cholmod_common *Common, char *caller)
{
	char *name = "dia1P_loadVector";
	int N = pD.gridSize;
	// Number of unknown voltages
	int NVar = N*(N-1)/2;
	
	if(Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}
	
	cholmod_dense *temp1;
	cholmod_sparse *D,*temp2,*temp3,*temp4;

	//Create a real column vector of N/2 ones
	temp1 = cholmod_ones(N/2,1,CHOLMOD_REAL,Common);
	//Convert it to a sparse vector
	temp2 = cholmod_dense_to_sparse(temp1,1,Common);
	//Add it to itself in order to get a vector of 2's (perils of working in C..!!!)
	{
		double one [2] = {1,0};
		temp3 = cholmod_add(temp2,temp2,one,one,1,0,Common);
	}

	//Create a real column vector of (NVar - N) zeros
	temp4 = cholmod_spzeros(NVar-N/2,1,0,CHOLMOD_REAL,Common);

	//Concatenate to create the load vector
	D = cholmod_vertcat(temp4,temp3,1,Common);

	if(D == NULL)
	{
		dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,errMesg_CholmodReturnedNullPointer);
	}
	// free temporary variables
	cholmod_free_dense(&temp1,Common);
	cholmod_free_sparse(&temp2,Common);
	cholmod_free_sparse(&temp3,Common);
	cholmod_free_sparse(&temp4,Common);
	
	return D;
}
