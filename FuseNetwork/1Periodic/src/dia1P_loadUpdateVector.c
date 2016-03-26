/**************************************************** 
 *		filename:   dia1P_loadUpdateVector.c	*
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.3 (1.1 Created 14th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
	1.2		| Updated for better diagn-	|17th June 09 
			| ostic and output handling |
 ----------------------------------------------------
	1.3		| Updated for the N vs N-1 	|26th June 09 
			| issue in periodic BCs		|
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_loadUpdateVector. The purpose of this
	function is to return a vector v that will update the
	load vector as b_new = b_old + v.
*/

#include "dia1P.h"

cholmod_sparse *dia1P_loadUpdateVector(dia1P_failureSite *FD, cholmod_common *Common, char *caller)
{
	char *name = "dia1P_loadUpdateVector";

	int N = pD.gridSize;
	// Number of unknown voltages
	int NVar = N*(N-1)/2;	
	int nUpdate = 0;
	int *Ti, *Tj;
	double *Tx;

	// Check for NULL pointers in inputs
	if( FD == NULL || Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}

	// Allocate a triplet to hold the update vector
	cholmod_triplet *T;
	T = cholmod_allocate_triplet(NVar,1,1,0,CHOLMOD_REAL,Common);
	
	if( T == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_CholmodReturnedNullPointer);
	}

	Ti = (int *)(T->i);
	Tj = (int *)(T->j);
	Tx = (double *)(T->x);
	
	if( Ti == NULL || Tj == NULL || Tx == NULL)
	{
		dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,errMesg_CholmodReturnedNullPointer);
	}

	// Make sure that FD has sorted nodes
	if(FD->node1<FD->node2)
	{
		dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_UnsortedNodesInFD);
	}

	// Load vector changes only if a fuse connected to the top edge blows
	// check and update accordingly.
	if(FD->node1-N/2 >= NVar)
	{
		Ti[nUpdate] = FD->node2-N/2;
		Tj[nUpdate] = 0;
		Tx[nUpdate] = -1;
		nUpdate++;
	}

	T->nnz = nUpdate;

	// Convert triplet to sparse 
	cholmod_sparse *V;
	V = cholmod_triplet_to_sparse(T,1,Common);

	// free triplet
	cholmod_free_triplet(&T,Common);
	
	// return sparse
	return V;
}
