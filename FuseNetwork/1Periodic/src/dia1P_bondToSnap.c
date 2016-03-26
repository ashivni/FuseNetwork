/**************************************************** 
 *		filename:   dia1P_bondToSnap.c	 	    *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.1 (1.1 Created 17th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				| 17th June 09 
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_bondToSnap. The purpose of this function
	is to determine which bond to snap. The function
	also writes the appropriate diagnostic output
	if required. 
*/

#include "dia1P.h"

void dia1P_bondToSnap(double *S, cholmod_dense *VNode_d, cholmod_sparse *VNode_s, cholmod_sparse *V2C, dia1P_brokenBonds *BB, dia1P_failureSite *FD, cholmod_common *Common, char *caller)
{
	char *name = "dia1P_bondToSnap";

	// n1 and n2 are the nodal positions of the fuses and the bond number
	int n1 = 0, n2 = 0, bondNumber = 0;;
	int N = pD.gridSize;	// System size

	// fN1 and fN2 store the nodal position of the fuse that will be snapped in this iteration.
	// These variables evolve through the for loops that follow
	int fN1 = 0,fN2 = 0;
	double fStr;	// Strength of the failed bond

	// curRatio holds the failure current to strength ratio (ratio of current to fuse strength for the fuse that fails). 
	// This variable evolves through the for loops that follow
	double curRatio = 0;

	// Make sure none of the input pointers are NULL
	if(S==NULL || VNode_d==NULL || VNode_s == NULL || V2C == NULL || BB==NULL || FD==NULL || Common==NULL || caller==NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}

	double *v = (double *)(VNode_d->x);

  // Make sure that the type cast did not return any NULL pointers
  if(v==NULL)
  {
    dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_NullPointerInArguments);
  }


	// First loop over all columns because S is stored in compressed column format
	for(bondNumber = 0; bondNumber < N*N; bondNumber++)
	{
		// Find the nodal positions of the fuse
		dia1P_getNodeNumbers(&n1,&n2,bondNumber,name);

		// Update curRatio, fN1, fN2, if the current to strength ratio for this fuse is higher
		// than the previous ones, and if it is not already snapped
		if(fabs(v[n1] - v[n2]) > dia1P_Tiny && curRatio < fabs(  (v[n1]-v[n2])/S[bondNumber]) && dia1P_isSnapped(BB,n1,n2,name) == 0 )
		{
			curRatio =	fabs((v[n1]-v[n2])/S[bondNumber]);
			fN1 = n1;
			fN2 = n2;
			fStr = S[bondNumber];
		}
	}

	// Next find the current flowing into the bottom nodes
	double netCurrent = 0;

	// Vector of currents flowing into the bottom nodes
	cholmod_sparse *current = NULL;

	// Find the current vector as currnet = V2C * VNode_s
	current = cholmod_ssmult(V2C,VNode_s,0,1,0,Common);

	// check for null pointers
	if(current == NULL)
	{
		dia1P_errHandler(errCode_CholmodReturnedNullPointer,name,caller,errMesg_CholmodReturnedNullPointer);
	}

	{ // Limit the scope of irrelevant variables
		int count = 0, countMax = 0;
		int nCol = 0, *p = NULL;
		double *x = NULL;

		// p is the cummulative sum of number of nonzero elements in the sparse matrix	
		p = (int *)(current->p);

		// x is the vector of actual values of elements of the sparse matrix
		x = (double *)(current->x);
		// check for null pointer
		if(x == NULL || p == NULL)
		{
			dia1P_errHandler(errCode_NullPointerInCholmodDS,name,caller,errMesg_NullPointerInCholmodDS);
		}

		// Number of columns
		nCol = current->ncol;

		// Total number of nonzero elements in the matrix
		countMax = p[nCol];

		// sum over all elements to get the net current
		for(count = 0; count < countMax; count++)
		{
			netCurrent+=x[count];
		}
	}
	// Update FD
	FD->fVol = (1.0/curRatio)/N;
	// The current calculated earlier will always be negative since it is the
	// net current flowing into the bottom edge, which is at a lower potential
	// than the upper edge. Also, this is the current that flows into the
	// bottom edge with unit voltage. To get the current at the failure
	// voltage we have to scale apropriately.
	FD->fCur = (-netCurrent/curRatio)/N;
	FD->node1 = fN1>fN2 ? fN1 : fN2;
	FD->node2 = fN1>fN2 ? fN2 : fN1;
	FD->str = fStr;

	if(fN1 ==0 && fN2 == 0)
	{
		dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_InconsistentDataStructure);
	}
	// Free memory
	cholmod_free_sparse(&current,Common);
}
