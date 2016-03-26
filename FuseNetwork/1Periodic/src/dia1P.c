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
	char *name = "main";
	char *seperator = "**********************************************************";

	// Setup the data structure with parameters of the problem
	switch (argc)
	{
		case 1:
				printf("No input file specified. Using dia1P.inp\n");
				dia1P_initialize("dia1P.inp",name);
				break;
		case 2:
				dia1P_initialize(argv[1],name);
				break;
		default: 
				dia1P_errHandler(errCode_TooManyArguments,name,name,errMesg_TooManyArguments);
	}

	// Print the problem to make sure
	dia1P_printPD(name);

	/* The prefix M_ is used for components that can be reused in several 
		failure simulations. For example, it is not necessary to compute 
		the first stiffness matrix M_M or its decomposition M_L for each 
		failure simulation. On the other hand, the matrix of fuse strengths, 
		S, needs to be repopulated every time.
	*/
	
	/* START REUSABLE COMPONENTS DECLARATIONS */

	// Stiffness matrix M
	cholmod_sparse *M_M;

	// J = M_V2C*V; where J = current flowing into the bottom nodes,
	// and V = vector of voltages of all nodes
	cholmod_sparse	*M_V2C;

	// Voltages at top and bottom nodes
	cholmod_sparse *M_vTop, *M_vBot;	

	// Cholesky factor of the stiffness matrix M
	cholmod_factor *M_L;				

	// Cholmod Common object
	cholmod_common Common;	
	
	// Basic scalars, one and minus one
	double one [2] = {1,0}, m1 [2] = {-1,0} ;

	/* END REUSABLE COMPONENTS DECLARATIONS */


	/* START REUSABLE COMPONENTS INITIALIZATIONS */
	
	// Start cholmod, and the cholmod_common object
	cholmod_start(&Common);	

	// Populated the top and bottom node voltages

	// Bottom row is "grounded", thus has zero 
	// voltage by convention. 
	// cholmod_spzeros(NRow,NCol,stype,xtype,*common)	
	M_vBot = cholmod_spzeros(pD.gridSize/2,1,0,CHOLMOD_REAL,&Common);

	// The top row has voltage = 1. Since cholmod has no inbuild
	// function to return a sparse vector of all ones (makes sense)
	// so we first create a dense vector of ones and then 
	// convert it to a sparse vector

	{ // limit the scope of temporary variables
		cholmod_dense *temp;
		temp = cholmod_ones(pD.gridSize/2,1,CHOLMOD_REAL,&Common);
		M_vTop = cholmod_dense_to_sparse(temp,1,&Common);
		cholmod_free_dense(&temp,&Common);
	}

	// Polulate voltage to current matrix and check it for
	// consistency 
	M_V2C = dia1P_voltageToCurrentMatrix(&Common,name);
	cholmod_check_sparse(M_V2C,&Common);

	// Populate stiffness matrix
	M_M = dia1P_stiffnessMatrix(&Common,name);
	// Check it for consistency
	cholmod_check_sparse(M_M,&Common);

	// Analyze and factorise the stiffness matrix
	M_L = cholmod_analyze(M_M,&Common);
	cholmod_factorize(M_M,M_L,&Common);
	// Check the factor for consistency
	cholmod_check_factor(M_L,&Common);
	
	/* END REUSABLE COMPONENTS INITIALIZATIONS */

	// Depending on the mode in which the program is run 
	// various levels of output are given to the user.
	// The three modes implemented so far are:
	// 0: Silent,
	// 1: minimal,
	// 2: normal
	// 3: verbose
	switch (pD.diagMode)
	{
		case 0:	
			break;
		case 1:	
			fprintf(pD.diagFile,"NSim\tnF\t\tnAv\t\tV\t\tC\n");
			fflush(pD.diagFile);
			break;
		case 2:	
			break;
		case 3:	
			fprintf(pD.diagFile,"Initial Stiffness Matrix\n");
			cholmod_write_sparse(pD.diagFile,M_M,NULL,NULL,&Common);
			fflush(pD.diagFile);	
			break;
		default: 
			dia1P_errHandler(errCode_UnknownDiagMode,name,name,errMesg_UnknownDiagMode);
	}

	/* START MAIN SIMULATIONS LOOP */

	// Number of simulations performed
	int countSims = 0;	

	while (countSims < pD.NSim)
	{	
		/* START LOOP COMPONENTS DECLARATIONS */

		// The sampleFailed flag remains zeros as long as 
		// the sample is not broken (a spanning crack is 
		// not encountered; it becomes 1 otherwise.
		int sampleFailed = 0;
	
		// nFail counts the number of bonds snapped till 
		// sample failure
		int nFail = 0;
					
		// Cholesky factor L will hold the cholesky factor that will be updated after each bond snapping
		cholmod_factor *L;

		// Vector of random fuse strengths
		double *S;
		
		// Matrix that maps the node voltages to the vector of
		// currents flowing into the bottom nodes.
		// This matrix is update after every bond breaking
		cholmod_sparse *V2C;

		// Load vector b. This vector is to be updated after 
		// every bond breaking
		cholmod_sparse *b;

		// A data structure that will store information about the 
		// most recently failed bond
		dia1P_failureSite FD;

		// A data structure that will store information about the 
		// sequence of failures in a particular simulation
		dia1P_brokenBonds *BB;	

		/* END LOOP COMPONENTS DECLARATIONS */


		/* START LOOP COMPONENTS INITIALIZATIONS */
	
		// Copy the pre-calculated cholesky factor into the local 
		// cholesky factor
		L = cholmod_copy_factor(M_L,&Common);

		// Populate fuse strength vector
		S = dia1P_strengthVector(name);	
		//FILE *pf = fopen("16.S","r");  S = cholmod_read_sparse(pf,&Common); fclose(pf);
	
		// Copy the initial voltage to current matrix
		V2C = cholmod_copy_sparse(M_V2C,&Common);

		// Initialize the structure for keeping records of broken bonds
		BB = dia1P_initializeBrokenBonds(name);
		
		// Polulate the load vector b
		b = dia1P_loadVector(&Common,name);
		// Check to ensure consistency... 
		cholmod_check_sparse(b,&Common);

		/* END LOOP COMPONENTS INITIALIZATIONS */
		// Write diagonistic output as requested
		switch (pD.diagMode)
		{
			case 0:	break;
			case 1:	break;
			case 2:	
					fprintf(pD.diagFile,"%s\n",seperator);
					fprintf(pD.diagFile,"Starting Simulation Number %d\n",countSims+1);
					fprintf(pD.diagFile,"I\t\tJ\t\tV\t\tC\n");
					fflush(pD.diagFile);
					break;
			case 3:	
					fprintf(pD.diagFile,"%s\n",seperator);
					fprintf(pD.diagFile,"Starting Simulation Number %d\n",countSims+1);
					fprintf(pD.diagFile,"Matrix of Random Fuse Strengths:\n");
					{
						int count = 0;
						for(count = 0; count < (pD.gridSize)*(pD.gridSize); count++)
						{
							int n1, n2;
							dia1P_getNodeNumbers(&n1,&n2,count,name);
							fprintf(pD.diagFile,"%d\t%d\t%G\n",n1,n2,S[count]);
						}
						fprintf(pD.diagFile,"\n");
					}
					//cholmod_write_sparse(pD.diagFile,S,NULL,NULL,&Common);
					fflush(pD.diagFile);
					break;
			default: dia1P_errHandler(errCode_UnknownDiagMode,name,name,errMesg_UnknownDiagMode);
		}

		while(sampleFailed == 0)
		{
			/* START INNER LOOP COMPONENTS INITIALIZATIONS */
			// Vector x will hold the unknown voltages
			cholmod_sparse *x;

			// Vectors VNode_s and VNode_d hold the full set
			// of node voltages (knowns appended to the calculated unknowns)
			cholmod_sparse *VNode_s;
			cholmod_dense *VNode_d;

			// This vector will be used to update the stiffness matrix M
			// as M_new = M_old - stiffUpdate*transpose(stiffUpdate)
			// Ofcouse, M is not update, rather its cholesky factor L is
			cholmod_sparse *stiffUpdate;

			// This vector updates the load vector as 
			// b_new = b_old + loadUpdate
			cholmod_sparse *loadUpdate;

			// This vector is needed for internal cholmod use.
			// We L = PMP^T, where P is the permuation matrix.
			// Thus, if U updates M, then PU will update L. 
			// uper = PU.
			cholmod_sparse *uper;

			/* END INNER LOOP COMPONENTS INITIALIZATIONS */

			// Solve for the unknown voltages
			x = cholmod_spsolve(CHOLMOD_A,L,b,&Common);

			// Append the known vectors top and the bottom 
			// row voltages to x to construct the complete 
			// vector of voltages.
			{	// Limit the score of temporary variables
				cholmod_sparse *temp1;
				temp1 = cholmod_vertcat(M_vBot,x,1,&Common);
				VNode_s = cholmod_vertcat(temp1,M_vTop,1,&Common);
				cholmod_free_sparse(&temp1,&Common);
			}

			// Check if the sample is broken, if it is then
			// we are done
			if(dia1P_isBroken(VNode_s,V2C,&Common,name))
			{
				sampleFailed = 1;
				{
					int count = 0;
					for(count = 0; count < BB->nFail; count++)
					{
						fprintf(pD.outFile,"%d\t%d\t%G\t%G\t%G\n",BB->i[count]+1,BB->j[count]+1,BB->v[count],BB->c[count],BB->bondStrength[count]);
					}
					fprintf(pD.outFile,"%d\t%d\t%G\t%G\t%G\n",0,0,0.f,0.f,0.f);
				}
			}
			else
			{	// If the sample is not broken yet, then we need to 
				// to find which bond will be snapped next.
				
				// Increment the number of failed bonds, since we know
				// that one is going to snap
				nFail++;

				// Make a dense vector of voltages
				VNode_d = cholmod_sparse_to_dense(VNode_s,&Common);
	
				// Find which bond to break and store the information 
				// in the data structure FD.
				dia1P_bondToSnap(S,VNode_d,VNode_s,V2C,BB,&FD,&Common,name);

				// Update the data structure BB, which stores the entire
				// sequence of broken bonds
				dia1P_updateBrokenBonds(BB,&FD,name);

				// Update the voltage to current matrix.
				// This matrix will change only if a fuse connected to the 
				// bottom edge is blown.
				dia1P_updateVoltageToCurrentMatrix(V2C,&FD,&Common,name);

				// Find the vector to update the stiffness matrix. 
				// This vector is never empty, it has either 1 or 2 nonzero components
				// depending on weather a boundary node is involved in the snapping or not
				stiffUpdate = dia1P_stiffnessUpdateVector(&FD,&Common,name);

				// Find the vector to update the load vector.
				// This vector is non-zero only if a fuse connected to the 
				// top edge is blown.
				loadUpdate = dia1P_loadUpdateVector(&FD,&Common,name);
						

				// Update the load vector
				{ // Limit the score of temporary variables
					cholmod_sparse *temp;
					temp = cholmod_copy_sparse(b,&Common);
					// Free the current memory occupied by b before reallocating
					cholmod_free_sparse(&b,&Common);								
					// Reallocate b
					b = cholmod_add(temp,loadUpdate,one,one,1,0,&Common);
					// Free temp
					cholmod_free_sparse(&temp,&Common);
				}

				// Calculate the permuted update vector for updating the cholesky factor
				uper = cholmod_submatrix(stiffUpdate,L->Perm,L->n,NULL,-1,1,1,&Common);
				
				// update (downdate) the cholesky factor
				cholmod_updown(0,uper,L,&Common);

				// Write appropriate diagnostic output
				switch (pD.diagMode)
				{
					case 0:	break;
					case 1:	break;
					case 2:
						fprintf(pD.diagFile,"%d\t\t%d\t\t%.3f\t%.3f\n",FD.node1+1,FD.node2+1,FD.fVol,FD.fCur);
						break;
					case 3:
						fprintf(pD.diagFile,"\nPass No. %d\nUnknown Node Voltages:\n",nFail);
						cholmod_write_sparse(pD.diagFile,x,NULL,NULL,&Common);
						fprintf(pD.diagFile,"\nSnapped Bond: \nI\t\tJ\t\tV\t\tC\n");
						fprintf(pD.diagFile,"%d\t\t%d\t\t%.3f\t%.3f\n\n",FD.node1+1,FD.node2+1,FD.fVol,FD.fCur);
						fprintf(pD.diagFile,"\nStiffNess Update Vector\n");
						cholmod_write_sparse(pD.diagFile,stiffUpdate,NULL,NULL,&Common);
						fprintf(pD.diagFile,"\nLoad Update Vector\n");
						cholmod_write_sparse(pD.diagFile,loadUpdate,NULL,NULL,&Common);
						break;
					default: dia1P_errHandler(errCode_UnknownDiagMode,name,name,errMesg_UnknownDiagMode);
				}

				//Free memory
				cholmod_free_dense(&VNode_d,&Common);
				cholmod_free_sparse(&stiffUpdate,&Common);
				cholmod_free_sparse(&loadUpdate,&Common);
				cholmod_free_sparse(&uper,&Common);
			}//ESLE
			cholmod_free_sparse(&x,&Common);
			cholmod_free_sparse(&VNode_s,&Common);
		}//ELIHW, loop for nth simulation
		
		// Free memory
		free(S);
		cholmod_free_sparse(&b,&Common);
		cholmod_free_sparse(&V2C,&Common);
		cholmod_free_factor(&L,&Common);
		dia1P_freeBrokenBonds(&BB,name);
		countSims++;
	}//ELIHW, main loop for NSim simulations

	// This completes the requested set of NSim simulations. 
	// Free memory
	cholmod_free_sparse(&M_M,&Common);
	cholmod_free_sparse(&M_V2C,&Common);
	cholmod_free_sparse(&M_vBot,&Common);
	cholmod_free_sparse(&M_vTop,&Common);
	cholmod_free_factor(&M_L,&Common);

	// Close dia1P and cholmod
	dia1P_finish(name);
//	cholmod_print_common("FuseNet Statistics",&Common);
	cholmod_finish(&Common);
	return(0);	
}
