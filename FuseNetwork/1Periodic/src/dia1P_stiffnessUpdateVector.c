/********************************************************
 *		filename:   dia1P_stiffnessUpdateVector.c	*
 *      Author:     Ashivni Shekhawat               	*
 *      Version:    1.3 (1.1 Created 14th June 09)     	*
 ********************************************************/

/********************************************************
					Revision History                    *
 ========================================================
 *  Version |       	    Comment         |   Date    *
 ========================================================
    1.1     | First version     	        |14th June 09
 --------------------------------------------------------
    1.2     | Updated for better diagn-	 	|21st June 09
            | ostic and output handling 	|
 --------------------------------------------------------
    1.3     | Updated for the N vs N-1  	|26st June 09
            | issue in periodic BCs     	|
 --------------------------------------------------------
 ********************************************************/

/*
	This file contains the code for the function
	dia1P_stiffnessUpdateVector. The purpose of this
	function is to return a vector v that will update the
	stiffness matrix M as M_new = M_old - v*v'.
*/

#include "dia1P.h"

cholmod_sparse *dia1P_stiffnessUpdateVector(dia1P_failureSite *FD, cholmod_common *Common, char *caller)
{
	char *name = "dia1P_stiffnessUpdateVector";

	int N = pD.gridSize;
	int NVar = N*(N-1)/2;
	int nUpdate = 0;
	int *Ti, *Tj;
	double *Tx;

	cholmod_triplet *T;
	T = cholmod_allocate_triplet(NVar,1,2,0,CHOLMOD_REAL,Common);

	Ti = (int *)(T->i);
	Tj = (int *)(T->j);
	Tx = (double *)(T->x);
	
	if(FD->node1<FD->node2)
	{
		dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_UnsortedNodesInFD);
	}

	// If the snapped bond lies on the lower edge, do nothing
	if(FD->node2-N/2 < 0) { }
	// Else update
	else
	{
		Ti[nUpdate] = FD->node2-N/2;
		Tj[nUpdate] = 0;
		Tx[nUpdate] = -1.0;
		nUpdate++;
	}
	
	// If the snapped bond lies on the upper edge, do nothing
	if(FD->node1-N/2 >= NVar) { }
	else
	{
		Ti[nUpdate] = FD->node1-N/2;
		Tj[nUpdate] = 0;
		Tx[nUpdate] = 1.0;
		nUpdate++;
	}
	T->nnz = nUpdate;
	cholmod_sparse *V;
	V = cholmod_triplet_to_sparse(T,2,Common);
	cholmod_free_triplet(&T,Common);
	
	return V;
}
