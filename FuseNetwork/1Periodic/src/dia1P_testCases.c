/****************************************************
 *      filename:   dia1P.c			            *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.3 (1.1 Created 14th June 09)  *
 ====================================================
 *              Revision History                    *
 ====================================================
 *  Version |           Comment         |   Date    *
 ====================================================
    1.1     | First version             |14th June 09
 ----------------------------------------------------
	1.2		| Major updates looping to  |16th June 09
			| perform many simulations. |
 ----------------------------------------------------
	1.3		| Update to add better 		|21st June 09
			| comments and error 		|
			| handling.					|
 ----------------------------------------------------
	1.4		| Update to fix the N versus|26st June 09
			| N-1 issues in periodic 	|
			| grid.						|
 ----------------------------------------------------
 ****************************************************/


#include "dia1P.h"

int main(int argc, char *argv[])
{
	/* This file contains unit test cases for several 
		of the individual functions of dia1P suite. */

	char *name = "Tester";
	// Test the bond and site numbering scheme

	{
		int N; // System size
		int bondNum;	// Bond Number
		int node1, node2;	// Node numbers

		// Case A. N = 6
		N = 6;
		pD.gridSize = N;
		for(bondNum = 0; bondNum < N*N; bondNum++)
		{	// Loop over all bond numbers
			dia1P_getNodeNumbers(&node1,&node2,bondNum,name);
				printf("N = %d, Bond Number = %d, Node1 = %d, Node2 = %d, Generated Bond Number = %d\n",N,bondNum,node1,node2,dia1P_getBondNumber(node1,node2,name));
			if(bondNum != dia1P_getBondNumber(node1,node2,name))
			{
				printf("Error: N = %d, Bond Number = %d, Node1 = %d, Node2 = %d, Generated Bond Number = %d\n",N,bondNum,node1,node2,dia1P_getBondNumber(node1,node2,name));
			}
		}

		
		// Case A. N = 40
		N = 40;
		pD.gridSize = N;
		for(bondNum = 0; bondNum < N*N; bondNum++)
		{	// Loop over all bond numbers
			dia1P_getNodeNumbers(&node1,&node2,bondNum,name);
			if(bondNum != dia1P_getBondNumber(node1,node2,name))
			{
				printf("Error: N = %d, Bond Number = %d, Node1 = %d, Node2 = %d, Generated Bond Number = %d\n",N,bondNum,node1,node2,dia1P_getBondNumber(node1,node2,name));
			}
		}
		/*
		// Case A. N = 100
		N = 100;
		for(bondNum = 0; bondNum < N*N; bondNum++)
		{	// Loop over all bond numbers
			dia1P_getNodeNumbers(&node1,&node2,bondNum,name);
			if(bondNum != dia1P_getBondNumber(node1,node2,name))
			{
				printf("Error: N = %d, Bond Number = %d, Node1 = %d, Node2 = %d, Generated Bond Number = %d\n",N,bondNum,node1,node2,dia1P_getBondNumber(node1,node2,name));
			}
		}
	*/
	}
	// END test for bond and site number scheme
		
}
