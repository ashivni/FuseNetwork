/**************************************************** 
 *		filename:   dia1P_isSnapped.c		 	*
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.1 (Created 14th June 09)      *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
	1.2		| Updated for better diagn-	|17th June 09 
			| ostic and output handling |
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_isSnapped. The purpose of this function
	is to determine if the bond between two nodes 
	has already been snapped. Returns 1 if the 
	bond is snapped, 0 otherwise.
*/

#include "dia1P.h"
int dia1P_isSnapped(dia1P_brokenBonds *BB, int node1, int node2, char *caller)
{
  char *name = "dia1P_isSnapped";
  if(BB == NULL || caller == NULL)
  {
    dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
  }

  {// limit the scope of the temporary variable
   // make sure node1 > node2
    int temp = node1;
    node1 = temp>node2 ? temp : node2;
    node2 = temp>node2 ? node2: temp;
  }

  return BB->BondArr[dia1P_getBondNumber(node1,node2,name)];
}

