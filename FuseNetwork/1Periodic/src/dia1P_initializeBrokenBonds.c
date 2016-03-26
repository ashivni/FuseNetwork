/******************************************************************** 
 *		filename:   dia1P_initializeBrokenBonds.c 	*
 *      Author:     Ashivni Shekhawat               				*
 *      Version:    1.4 (Created 1st June 09)       				*
 ====================================================================
 *			      		Revision History							*
 ====================================================================
 *	Version	|			Comment							| 	Date	*
 ====================================================================
 	1.1		| First version								| 14th June 09 
 --------------------------------------------------------------------
 	1.2		| Updated to handle the avalanche realted	| 21st June 09 
			| arrays.									|
 --------------------------------------------------------------------
 	1.3		| Updated to include resistance data at 	| 2nd July 09 
			| avalances.								|
 --------------------------------------------------------------------
 	1.4		| Updated to handle current avalanches		| 06th July 09 
 --------------------------------------------------------------------
 ********************************************************************/

/*
	This file contains the code for the function
	dia1P_initializeBrokenBonds. The purpose of this function
	is to initialize the structure dia1P_brokenBonds.
*/

#include "dia1P.h"

dia1P_brokenBonds *dia1P_initializeBrokenBonds(char *caller)
{
	char *name = "dia1P_initializeBrokenBonds";
	dia1P_brokenBonds *BB;
	BB = (dia1P_brokenBonds *)malloc(sizeof(dia1P_brokenBonds));
	if(BB==NULL)
	{
		dia1P_errHandler(errCode_OutOfMemory,name,caller,errMesg_MallocNullPointer);
	}

	// No failures yet
	BB->nFail = 0;

	int N = pD.gridSize;
	// Number of bonds
	int n_bonds = N*N;

	// Allocate memory for the arrays
	BB->i = (int *)malloc(n_bonds*sizeof(int));
	BB->j = (int *)malloc(n_bonds*sizeof(int));

	BB->v = (double *)malloc(n_bonds*sizeof(double));
	BB->c = (double *)malloc(n_bonds*sizeof(double));
	BB->bondStrength = (double *)malloc(n_bonds*sizeof(double));
	BB->BondArr = (int *)malloc(n_bonds*sizeof(int));

	

	// Check for memory allocation
	if(BB->i == NULL || BB->j == NULL || BB->v == NULL || BB->c == NULL || BB-> BondArr == NULL)
	{
		dia1P_errHandler(errCode_OutOfMemory,name,caller,errMesg_MallocNullPointer);
	}
	
	// Allocation successfull, initialize 

	int count = 0;
	for(count = 0; count < n_bonds; count++)
	{
		BB->i[count] = 0;
		BB->j[count] = 0;
		BB->BondArr[count] = 0;
		BB->v[count] = 0.f;
		BB->c[count] = 0.f;
		BB->bondStrength[count] = 0.f;
	}

	// Return
	return BB;
}

