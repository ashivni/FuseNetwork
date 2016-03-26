/******************************************************************** 
 *		filename:   dia1P_voltageToCurrentMatrix.c				*
 *      Author:     Ashivni Shekhawat               				*
 *      Version:    1.1 (Created 17th June 09)      				*
 ====================================================================
 *                      Revision History                            *
 ====================================================================
 *  Version |           Comment                         |   Date    *
 ====================================================================
    1.1     | First version                             |17th June 09
 --------------------------------------------------------------------
    1.2     | Updated to handle the avalanche realted   |21st June 09
            | arrays.                                   |
 --------------------------------------------------------------------
 ********************************************************************/

/*
	This file contains the code for the function
	dia1P_voltageToCurrentMatrix.c. The purpose of 
	this function is to return a sparse matrix which when
	multiplied with the voltage vector gives the 
	vector of currents flowing into the bottom nodes.
*/

#include "dia1P.h"

cholmod_sparse *dia1P_voltageToCurrentMatrix(cholmod_common *Common, char *caller)
{
	char *name = "dia1P_voltageToCurrentMatrix";
	int N = pD.gridSize;
	int NNZero = 3*N/2;
	int NVar = N*(N+1)/2;
	int *Ti, *Tj;
	double *Tx;

	// Check for null pointers
	if( Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}

	// Allocate a sparse matrix of suitable size
	cholmod_triplet *T;
	// cholmod_allocate_triplet(Nrow,Ncol,nzmax,stype,xtype,*Common)
	// stype > 0 (=1) means square, symmetric, lower triangular stored.
	T = cholmod_allocate_triplet(N,NVar,NNZero,0,CHOLMOD_REAL,Common);
	

	// Check for null pointers
	if( T == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_CholmodReturnedNullPointer);
	}

	// Typecast void * pointers to adequate type
	Ti = (int *)(T->i);	Tj = (int *)(T->j);	Tx = (double *)(T->x);

	// Check for null pointers
	if( T == NULL)
	{
		dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,errMesg_CholmodReturnedNullPointer);
	}

	// Counter for number of variables
	int countVar = 0;

	// Counter for number of non-zero entries in the sparse matrix
	int countEnt = 0;

	int n1 = 0, n2 = 0, bondNum = 0;

	for(countVar = 0; countVar < N/2; countVar++)
	{
		// Dominant diagonal
		Ti[countEnt] = countVar;	Tj[countEnt] = countVar;		Tx[countEnt] = 2;		countEnt++;

		bondNum = countVar;
		dia1P_getNodeNumbers(&n1,&n2,bondNum,name);
		Ti[countEnt] = n2;	Tj[countEnt] = n1;		Tx[countEnt] = -1;		countEnt++;

		bondNum = countVar + N*N/2;
		dia1P_getNodeNumbers(&n1,&n2,bondNum,name);
		Ti[countEnt] = n2;	Tj[countEnt] = n1;		Tx[countEnt] = -1;		countEnt++;
	}
	T->nnz = countEnt;
	// convert triplet to sparse
	cholmod_sparse *M;
	M = cholmod_triplet_to_sparse(T,NNZero,Common);

	// Free memory
	cholmod_free_triplet(&T,Common);
	
	return M;
}
