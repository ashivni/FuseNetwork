/**************************************************** 
 *		filename:   dia1P_stiffnessMatrix.c     *
 *      Author:     Ashivni Shekhawat               *
 *		Version:    1.4 (1.1 Created 14th June 09)  *
 ====================================================
 *              Revision History                    *
 ====================================================
 *  Version |           Comment         |   Date    *
 ====================================================
    1.1     | First version             |14th June 09
 ----------------------------------------------------
    1.2     | Update to add better      |21st June 09
            | comments and error        |
            | handling.                 |
 ----------------------------------------------------
    1.3     | Update to handle the N vs |26th June 09
            | N-1 issues in periodic    |
            | BCs.		                |
 ----------------------------------------------------
    1.4     | Update to handle the N vs |06th July 09
            | N-1 issues in non-		|	
			| periodic direction 	    |
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_stiffnessMatrix. The purpose of this function
	return a sparse stiffness matrix for a given grid 
	size.
*/

#include "dia1P.h"

cholmod_sparse *dia1P_stiffnessMatrix(cholmod_common *Common, char *caller)
{
	char *name = "dia1P_stiffnessMatrix";
	int N = pD.gridSize;

	// NVar is the number of unknown voltages. This is also
	// the size of the stiffness matrix.
	int NVar = N*(N-1)/2;

	// Number of non-zero elements in the (lower triangular) stiffness matrix.
	int NNZero = 3*N*(N-1)/2 - N;

	// The matrix will be created in a triplet format. 
	// Ti and Tj are the row and column index arrays
	int *Ti, *Tj;

	// and Tx is the array of actual values 
	double *Tx;

	// Check arguments
	if(Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,"Null pointer in arguments");
	}

	// Create a triplet object of appropriate size and type
	cholmod_triplet *T;
	// cholmod_allocate_triplet(Nrow,Ncol,nzmax,stype,xtype,*Common)
	// stype > 0 (=1) means square, symmetric, lower triangular stored.
	T = cholmod_allocate_triplet(NVar,NVar,NNZero,1,CHOLMOD_REAL,Common);

	if(T==NULL)
	{
		dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,"Cholmod returned null pointer");	
	}
	// type cast the void* pointers to suitable form
	Ti = (int *)(T->i);	Tj = (int *)(T->j);	Tx = (double *)(T->x);
	if(Ti==NULL || Tj == NULL || Tx == NULL)
	{
		dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,"Cholmod returned null pointer");	
	}


	// countVar loops over the variables (unknown voltages)
	// while countEnd keeps track of the number of non-zero 
	// entries in the matrix.
	int countVar = 0, countEnt = 0;

	for(countVar = 0; countVar < NVar; countVar++)
	{
		// Dominant diagonal
		// The degree of each node is 4
		//printf("Diagonal: S[%d,%d] = 4\n",countVar,countVar);
		Ti[countEnt] = countVar;	Tj[countEnt] = countVar;		Tx[countEnt] = 4;		countEnt++;
	
		if(countVar >=N/2)
		{
			int n1 = 0, n2 = 0, bondNum = 0;

			bondNum = countVar;
			dia1P_getNodeNumbers(&n1,&n2,bondNum,name);
			//printf("A: CountVar = %d, bondNum = %d, n1 = %d, n2 = %d\n",countVar,bondNum,n1-N/2,n2-N/2);
			Ti[countEnt] = n1-N/2;	Tj[countEnt] = n2-N/2;		Tx[countEnt] = -1;		countEnt++;

			bondNum = countVar + N*N/2;
			dia1P_getNodeNumbers(&n1,&n2,bondNum,name);
			//printf("B: CountVar = %d, bondNum = %d, n1 = %d, n2 = %d\n",countVar,bondNum,n1-N/2,n2-N/2);
			Ti[countEnt] = n1-N/2;	Tj[countEnt] = n2-N/2;		Tx[countEnt] = -1;		countEnt++;
		}
		/*
		if(countVar >= N/2)
		{
			//printf("Down: S[%d,%d] = -1\n",countVar,countVar-N/2);
			Ti[countEnt] = countVar;	Tj[countEnt] = countVar-N/2;			Tx[countEnt] = -1;	countEnt++;
	
			if((countVar+1)%(N) == 0)			
			{
				//printf("Down Periodic: S[%d,%d] = -1\n",countVar,countVar-N/2+1);
				Ti[countEnt] = countVar;	Tj[countEnt] = countVar-N+1;			Tx[countEnt] = -1;	countEnt++;
			}
			else if(countVar%N == 0)		
			{
				printf("Down Interior: S[%d,%d] = -1\n",countVar,countVar-N/2-1);
				Ti[countEnt] = countVar;	Tj[countEnt] = countVar-1;			Tx[countEnt] = -1;	countEnt++;
			}
			else if((countVar+1-N/2)%N == 0)
		}
		*/

	}
	T->nnz = NNZero;

	if(countEnt!=NNZero)
	{
		dia1P_errHandler(errCode_WrongSizeAllocation,name,caller,errMesg_WrongSizeAllocation);
	}
//	cholmod_print_triplet(T,"stiffness",Common);
	cholmod_sparse *M;
	// Convert triplet to sparse
	M = cholmod_triplet_to_sparse(T,NNZero,Common);
	if(M==NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_CholmodReturnedNullPointer);	
	}

	// Free memory
	cholmod_free_triplet(&T,Common);
	return M;
}
