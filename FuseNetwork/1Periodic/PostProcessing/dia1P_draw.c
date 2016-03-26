#include "pp.h"

int main(int argc, char *argv[])
{

	// argv
	// argv[1] input file
	// argv[2] output file 
	// argv[3] mode, 0 for full, 1 for till failure
	// argv[4] Size of the system

	// SOME COLOR DEFINITIONS
	rgb rgb_black, rgb_red, rgb_green;

	rgb_black.r = 0;	rgb_black.g 	= 0;	rgb_black.b = 0;
	rgb_red.r 	= 1;	rgb_red.g 		= 0;	rgb_red.b = 0;
	rgb_green.r = 0;	rgb_green.g 	= 1;	rgb_green.b = 0;

	// High level variables (Remain unchanged)
	int L = 4;	// Inch, side of picture
	int length_inp_dat = 5;	// Number of columns in the input file
	int F = atoi(argv[3]);	// Flag, 0 for full simulation, 1 for till critical event
	int N = atoi(argv[4]);	// Size of the system
	double scale = (72.f*L/N);
	int pad = 20;

	// Initialize Input and Output Files
	FILE *in = dia1P_initializeInputFile(argv[1],3);
	FILE *out = fopen(argv[2],"w+");
	
	double *data_ar = (double *)malloc(length_inp_dat*sizeof(double));         // array to hold the scanned data

	// Setup the PS file	
	char *text = "Fuse Network In Diamond Topology By Ashivni Shekhawat";
	dia1P_drawInitialSetup(out,L,N,scale,text,pad);


	// Arrays for storing failed bonds
	int *n1 = (int *)malloc(N*N*sizeof(int));	
	int *n2 = (int *)malloc(N*N*sizeof(int));	
	int nFail = 0, pos_max_cur = 0;	// Number of bonds failed, position of the largest current
	double max_cur = 0;

	// Read data
	while(dia1P_getData(in,data_ar,length_inp_dat))
	{
		if(! dia1P_isEndOfRun(data_ar))
		{
			n1[nFail] = (int)data_ar[0] - 1;
			n2[nFail] = (int)data_ar[1] - 1;
			if (data_ar[3] > max_cur)
			{
				max_cur = data_ar[3];
				pos_max_cur = nFail;
			}
			nFail = nFail + 1;
		}
		else
		{
			break;
		}
	}

	// Write output
	{
		int count = 0;
		for(count = 0; count < nFail; count++)
		{
			if(count < pos_max_cur)
			{
				dia1P_markBond(n1[count],n2[count],N,rgb_black,out);
			}
			else
			{
				dia1P_markBond(n1[count],n2[count],N,rgb_red,out);
			}
		}
	}

	fprintf(out,"showpage\n");
	fclose(out);
	fclose(in);
}

