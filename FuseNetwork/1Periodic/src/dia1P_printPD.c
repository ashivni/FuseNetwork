/**************************************************** 
 *		filename:   dia1P_printPD.c	            *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.2 (1.1 Created 14th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
	1.2		| Updated for better diagn-	|17th June 09 
			| ostic and output handling |
			| and to accomodate the 	|
			| changes made to			| 
			| dia1P_probDef data 	|
			| structure					|
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_printPD. The purpose of this function
	is to print the global variable pD.
*/

#include "dia1P.h"

void dia1P_printPD(char *caller)
{
	char *name = "dia1P_printPD";
	FILE *pf;
	// Check input
	if(caller==NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,"NULL pointer in arguments");
	}

	if(pD.diagMode!=0)
	{

		// Make sure diagFile is not null
		if(pD.diagFile==NULL)
		{
			dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,"pD.diagFile is NULL");
		}
		else
		{
			pf = pD.diagFile;
		}

		fprintf(pf,"***********************************************************\n");
		fprintf(pf,"* Solving Fuse Network Problem With Grid Size %d\n",pD.gridSize);
		fprintf(pf,"* %d simulations will be run\n",pD.NSim);
		switch (pD.density)
		{
			case 0:	fprintf(pf,"* Density = %d (fuse strengths power law with exponent %.2lf)\n",pD.density,pD.densityParam1); break;
			default: dia1P_errHandler(errCode_BadInitialization,name,caller,"unknown density");
		}
		switch (pD.diagMode)
		{
			case 0:	fprintf(pf,"* diagmode = %d (silent, no diagnostic output)\t diagFileName = %s\n",pD.diagMode,pD.diagFileName); break;
			case 1:	fprintf(pf,"* diagmode = %d (minimal diagnostic output)\t diagFileName = %s\n",pD.diagMode,pD.diagFileName); break;
			case 2:	fprintf(pf,"* diagmode = %d (normal diagnostic output)\t diagFileName = %s\n",pD.diagMode,pD.diagFileName); break;
			case 3:	fprintf(pf,"* diagmode = %d (verbose, much diagnostic output)\t diagFileName = %s\n",pD.diagMode,pD.diagFileName); break;
			default: dia1P_errHandler(errCode_BadInitialization,name,caller,"unknown diagMode");
		}
		fprintf(pf,"* outFileName = %s\n",pD.outFileName);
		fprintf(pf,"***********************************************************\n");
	}
}
