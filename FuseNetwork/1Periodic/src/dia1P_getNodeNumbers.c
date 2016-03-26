/************************************************************************************ 
 *    filename:   dia1P_getNodeNumbers.c                                          *
 *      Author:     Ashivni Shekhawat                                               *
 *      Version:    1.1                                                             *
 ====================================================================================
 *                                  Revision History                                *
 ====================================================================================
 *  Version |                           Comment                           |   Date  *
 ====================================================================================
  1.1       | First version                                               |30th Sep 09 
 ------------------------------------------------------------------------------------
 ************************************************************************************/

/*
  This file contains the code for the function
  dia1P_getNodeNumbers. The purpose of this function
  is to return a unique number for the bond connecting the nodes n1 and n2
*/

#include "dia1P.h"

void dia1P_getNodeNumbers(int *node1, int *node2, int bondNumber, char *caller)
{
  char *name = "dia1P_getNodeNumbers";

  if(caller == NULL)
  {
    dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
  }

  int N = pD.gridSize;
	int M = N*N/2;

	if(bondNumber < M)
	{
		*node2 = bondNumber;
		*node1 = bondNumber + N/2;
	}
	else
	{
		*node2 = bondNumber - N*N/2;
		if((*node2)%N == 0)
		{
			*node1 = *node2 + N-1;
		}
		else if((*node2+1)%N == 0)
		{
			*node1 = *node2 + 1;
		}
		else if((*node2)%N < N/2)
		{
			*node1 = *node2 + N/2 - 1;
		}
		else
		{
			*node1 = *node2 + N/2 + 1;
		}
	}
}

