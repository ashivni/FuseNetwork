/************************************************************************************ 
 *    filename:   dia1P_getBondNumber.c                                          *
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
  dia1P_getBondNumber. The purpose of this function
  is to return a unique number for the bond connecting the nodes n1 and n2
*/

#include "dia1P.h"

int dia1P_getBondNumber(int node1, int node2, char *caller)
{
  char *name = "dia1P_getBondNumber";

  if(caller == NULL)
  {
    dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
  }

  {// limit the scope of the temporary variable
   // make sure node1 > node2
    int temp = node1;
    node1 = temp>node2 ? temp : node2;
    node2 = temp>node2 ? node2: temp;
  }

  int N = pD.gridSize;
	return (((node1 - node2) == N/2) ? node2 : node2 + N*N/2);
}

