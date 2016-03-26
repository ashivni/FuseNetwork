/**************************************************** 
 *		filename:   dia1P_strengthMatrix.c 	    *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.3 (1.1 Created 14th June 09)  *
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
    1.3     | Update to handle the N	|06th July 09
            | versus N-1 issues in the  |
            | non-periodic direction    |
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_strengthMatrix. The purpose of this function
	return a sparse symmtric strict lower triangle matrix
	of random fuse strengths.
*/

#include "dia1P.h"

double *dia1P_strengthVector(char *caller)
{
	char *name = "dia1P_strengthMatrix";
  if(caller == NULL)
  {
    dia1P_errHandler(errCode_NullPointer,name,caller,"NULL pointer in arguments");
  }

	int n_bonds = (pD.gridSize)*(pD.gridSize);
	
	double * M = (double *) malloc(n_bonds*sizeof(double));

  // Check for memory allocation
  if(M == NULL)
  {
    dia1P_errHandler(errCode_OutOfMemory,name,caller,errMesg_MallocNullPointer);
  }


	int count = 0;
	for(count = 0; count < n_bonds; count++)
	{
		if(pD.density == 0)		// Power law distributed
			M[count] = pow(RngStream_RandU01(pD.gen),1.f/pD.densityParam1);
		else if(pD.density == 1)		// Weibull distributed
			M[count] = pow(-log(1.0 - RngStream_RandU01(pD.gen)),1.f/pD.densityParam1);
	}	
	
	return M;
}
