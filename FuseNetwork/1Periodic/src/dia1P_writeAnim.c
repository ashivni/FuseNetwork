/******************************************************************** 
 *		filename:   dia1P_writeAnim.c		 	    			*
 *      Author:     Ashivni Shekhawat               				*
 *      Version:    1.2 (1.1 Created 18th June 09)      			*
 ====================================================================
 *                      Revision History                            *
 ====================================================================
 *  Version |           Comment                         |   Date    *
 ====================================================================
    1.1     | First version                             | 18th June 09
 --------------------------------------------------------------------
    1.2     | Updated to handle the  N vs N-1 issues    | 28th June 09
			| for periodic BCs							|
 --------------------------------------------------------------------
 ********************************************************************/

/*
	This file contains the code for the function
	dia1P_writeAnim. The purpose of this function
	write files that can be used my gnuplot to make 
	animations of the bond snapping process
*/

#include "dia1P.h"

void dia1P_writeAnim(dia1P_brokenBonds *BB, cholmod_common *Common, char *caller)
{
	char *name = "dia1P_writeAnim";
	int i, N, *f1, *f2;
	double x1, y1, x2, y2, del;

	// Check for null pointers
	if(BB == NULL || Common == NULL || caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,caller,errMesg_NullPointerInArguments);
	}

	// Open file for writing the animation data
	FILE *pf = fopen("dia1P_Anim.dat","w+");

	// Check for null pointers
	if(pf == NULL)
	{
		dia1P_errHandler(errCode_FileNotFound,name,caller,errMesg_FileOpenWPMode);
	}

	// Get pointers to arrays of positions of broken bonds
	f1 = (int *)(BB->i);
	f2 = (int *)(BB->j);

	if(f1 == NULL || f2 == NULL)
	{
		dia1P_errHandler(errCode_NullPointerInFuseNetDS,name,caller,errMesg_NullPointerInArguments);
	}

	N = pD.gridSize;

	// Grid step size
	del = 1.f/(1.f*N );

	int count = BB->nFail;

	// This loop write the basic data for plotting the sequence of broken bonds. 
	// Remember that the BCs are periodic in the y direction, so a broken bond
	// on the left edge also means a broken bond on the right edge
	for(i = 0; i < BB->nFail; i++)
	{
		// Check to make sure that the pairs are sorted
		if(f2[i]>f1[i])
		{
			dia1P_errHandler(errCode_InconsistentDataStructure,name,caller,errMesg_UnsortedNodesInFD);
		}

		if( (f1[i]+1)%N == 0 && f2[i]%N == 0)
		{	// points on opposite periodic boundary
			x1 = (f1[i] % N)*del;
			y1 = (floor( ((double)f1[i]) / N ))*del;
			x2 = 1;
			y2 = y1;
		}
		else
		{
			if (f1[i] % N == 0 && f2[i]%N == 0)
			{	// points on same periodic boundary
				x1 = (f1[i] % N)*del;
				y1 = (floor( ((double)f1[i]) / N ))*del;
				x2 = (f2[i] % N)*del;
				y2 = (floor( ((double)f2[i]) / N ))*del;

				fprintf(pf,"%g\t%g\n",x1+1,y1);
				fprintf(pf,"%g\t%g\n",x2+1,y2);
				fprintf(pf,"\n\n");
				count++;

			}
			else
			{	// Interior points
				x1 = (f1[i] % N)*del;
				y1 = (floor( ((double)f1[i]) / N ))*del;
				x2 = (f2[i] % N)*del;
				y2 = (floor( ((double)f2[i]) / N ))*del;
			}
		}
		fprintf(pf,"%g\t%g\n",x1,y1);
		fprintf(pf,"%g\t%g\n",x2,y2);
		fprintf(pf,"\n\n");
	}

	// This loop write the data for plotting all grid points (not edges)
	for(i = 0; i < (N+1)*(N+1); i++)
	{
		x1 = (i % (N+1))*del;
		y1 = (floor( ((double)i) / (N+1) ))*del;
		
		fprintf(pf,"%g\t%g\n",x1,y1);
	}
	fprintf(pf,"\n\n");

	// This loop write the data for plotting all edges (unbroken) 
	for(i = 0; i <= N; i++)
	{
		x1 = 0.0;
		y1 = i*del;
		x2 = 1.0;
		y2 = i*del;
		
		fprintf(pf,"%g\t%g\n",x1,y1);
		fprintf(pf,"%g\t%g\n",x2,y2);
		fprintf(pf,"\n\n");

		fprintf(pf,"%g\t%g\n",y1,x1);
		fprintf(pf,"%g\t%g\n",y2,x2);
		fprintf(pf,"\n\n");
	}
	fclose(pf);

	// The file dia1P_AnimLoop.gpi has code for generating the 
	// loop that gnuplot will use to create subsiquent frames of the 
	// animation of snapping bonds. 
	pf=fopen("dia1P_AnimLoop.gpi","w+");
	
	if(pf == NULL)
	{
		dia1P_errHandler(errCode_FileNotFound,name,caller,"Could not open file dia1P_AnimLoop.gpi in w+ mode");
	}

	fprintf(pf,"count=count+1\n");
	fprintf(pf,"if (count<%d) \\\n",count);
	fprintf(pf,"\t plot [-0.1:1.1] [-0.1:1.1] \"dia1P_Anim.dat\" index 0:count w l lt 1; \\\n");
	fprintf(pf,"\t reread" );
	fclose(pf);

	// The file dia1P_AnimDriver.gpi is the main driver file for gnuplot. 
	// When run with gnuplot it will create a animation (.gif) of the breaking bonds and a snapshot of the 
	// broken sample (.png)

	pf=fopen("dia1P_AnimDriver.gpi","w+");

	if(pf == NULL)
	{
		dia1P_errHandler(errCode_FileNotFound,name,caller,"Could not open file dia1P_AnimDriver.gpi in w+ mode");
	}

	// Frame delay for the animation, measured in 1/100 of a second
	int delay = 0;

	// Pointsize for the lattice points
	double pointsize = 1;

	// Different system sizes need different frame rates and lattice point sizes 
	// for the animations to look reasonable. Values found by experimenting.
	if(pD.gridSize <= 8)
	{
		delay = 50;
		pointsize = 1.0;
	}
	else
	{
		if(pD.gridSize <=16)
		{
			delay = 20;
			pointsize = 0.75;
		}
		else
		{
			if(pD.gridSize <=32)
			{
				delay = 5;
				pointsize = 0.5;
			}
			else
			{
				delay = 2;
				pointsize = 0.25;
			}
		}
	}

	fprintf(pf,"set term gif large animate notransparent optimize delay %d size 800,800 x000000 xffffff xff0000 xff0000 xff0000 xff0000\n",delay);
	fprintf(pf,"set output '1P_%d.gif'\n",pD.gridSize);
	fprintf(pf,"set title '%d-by-%d Sample. 1-D Periodic BCs.'\n",pD.gridSize,pD.gridSize);
	fprintf(pf,"unset key\nunset tics\n");
	fprintf(pf,"count=-1\n");
	fprintf(pf,"load \"dia1P_AnimLoop.gpi\"\n");
	fprintf(pf,"reset\n\n");
	
	fprintf(pf,"set title '%d-by-%d Broken Sample. 1-D Periodic BCs.'\n",pD.gridSize,pD.gridSize);
	fprintf(pf,"unset key\nunset tics\n");
	fprintf(pf,"set term png large transparent size 800,800 x000000\n");
	fprintf(pf,"set out '1P_%d_Broken.png'\n",pD.gridSize);
	fprintf(pf,"plot [-0.1:1.1][-0.1:1.1]  'dia1P_Anim.dat' index %d:%d w l lt 1 lc rgb 'yellow'",count+1,count+1+2*(N+1));
	fprintf(pf,", 'dia1P_Anim.dat' index 0:%d w l lt 1 lw 3 lc rgb 'red', 'dia1P_Anim.dat' index %d w p pt 7 pointsize %f",count-1,count,pointsize);
	fclose(pf);
}
