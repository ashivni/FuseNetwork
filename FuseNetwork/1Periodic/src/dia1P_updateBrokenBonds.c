/**************************************************************** 
 *		filename:   dia1P_updateBrokenBonds.c 	*
 *      Author:     Ashivni Shekhawat               			*
 *      Version:    1.3 (Created 14th June 09)       			*
 ================================================================
 *                      Revision History                        *
 ================================================================
 *  Version |           Comment                     |   Date    *
 ================================================================
    1.1     | First version                         | 14th June 09
 -----------------------------------------------------------------
    1.2     | Updated to handle the avalanche 	   	| 21st June 09
            | related arrays.						|
 --------------------------------------------------------------------
    1.3     | Updated to handle the current  	   	| 06th July 09
            | avalanche related arrays.				|
 --------------------------------------------------------------------
 ********************************************************************/

/*
	This file contains the code for the function
	dia1P_updateBrokenBonds. The purpose of this function
	update the structure dia1P_brokenBonds when a bond
	snaps. If the total number of snapped bonds exceed
	the storage capacity of the structure then the 
	memory is reallocated. Appropriate error codes
	are generated if a failure in memory allocation 
	occurs.
*/

#include "dia1P.h"

void dia1P_updateBrokenBonds(dia1P_brokenBonds *BB, dia1P_failureSite *FS, char *caller)
{
	char *name = "dia1P_updateBrokenBonds";
	
	if(BB == NULL || FS == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}
	
	/* START CODE FOR UPDATING INFO ON BOND BREAKING*/


	int nF = BB->nFail;
	
	// Update the number of failed bonds
	BB->nFail = nF + 1;

	// Append info to the arrays
	BB->i[nF] = FS->node1;
	BB->j[nF] = FS->node2;
	BB->v[nF] = FS->fVol;
	BB->c[nF] = FS->fCur;
	BB->bondStrength[nF] = FS->str;
		
	BB->BondArr[dia1P_getBondNumber(FS->node1,FS->node2,name)] = dia1P_Broken;
}

