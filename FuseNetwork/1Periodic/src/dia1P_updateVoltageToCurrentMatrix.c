/******************************************************************** 
 *		filename:   dia1P_updateVoltageToCurrentMatrix.c		*
 *      Author:     Ashivni Shekhawat               				*
 *      Version:    1.2 (1.1 Created 17th June 09)					*
 ====================================================================
 *                      Revision History                            *
 ====================================================================
 *  Version |           	Comment                     |   Date    *
 ====================================================================
    1.1     | First version                             |17th June 09
 --------------------------------------------------------------------
    1.2     | Updated to handle the N vs N-1 issues		|26th June 09
			| in periodic boundaries					|
 --------------------------------------------------------------------
 ********************************************************************/

/*
	This file contains the code for the function
	dia1P_updateVoltageToCurrentMatrix.c. The purpose of 
	this function is to update the voltageToCurrent matrix
	in case a bond connected to the lower most row breaks.
*/

#include "dia1P.h"

int dia1P_updateVoltageToCurrentMatrix(cholmod_sparse *A, dia1P_failureSite *FD, cholmod_common *Common, char *caller)
{
	char *name = "dia1P_updateVoltageToCurrentMatrix";
	int N = pD.gridSize;
	int *i, *p;
	int nupdate = 0;
	double *x;

    // Check for NULL pointers in inputs
    if( A == NULL || FD == NULL || Common == NULL || caller == NULL)
    {   
        dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
    }   
	

    // Make sure that FD has sorted nodes
    if(FD->node1<FD->node2)
    {   
        dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_UnsortedNodesInFD);
    }

	// The matrix needs to be updated only if a bond connected to the lower most edge is broken.
	if(FD->node2 < N/2)
	{   
		i = (int *)(A->i);	p = (int *)(A->p);	x = (double *)(A->x);

    if( i == NULL || p == NULL || x == NULL)
    {   
    	dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_CholmodReturnedNullPointer);
    }

		int low, up;
		int count = 0;

		/* START UPDATE DIAGONAL ELEMENT */

		// V2C is stored in a compressed column format. We have to find the index range in which the 
		// information pertaining to the lower node is stored. This information is stored in indices
		//	i[low]  through i[up-1].
		low = p[FD->node2];	
		up = p[FD->node2+1];

		// Look through all nodes connected to the lower node to find the index of its connection
		// to itself. This is kind of cryptic.. its equivalent to finding M[i,i] in a 
		// compressed column matrix
		for(count=low; count < up; count ++)
		{
			if(i[count] == FD->node2)
			{	// Diagonal element found, update it 
				--x[count];
				nupdate++;
					break;
			}
		}
		
		/* END UPDATE DIAGONAL ELEMENT */
		
		/* START UPDATE OFF-DIAGONAL ELEMENT */

		// same as above...
		low = p[FD->node1];	
		up = p[FD->node1+1];
		for(count=low; count < up; count ++)
		{
			if(i[count] == FD->node2)
			{
				++x[count];
				nupdate++;
					break;
			}
		}
		/* END UPDATE OFF-DIAGONAL ELEMENT */
	}
	else 
	{ 
		// Do nothing
	}   
	
	return nupdate;
}
