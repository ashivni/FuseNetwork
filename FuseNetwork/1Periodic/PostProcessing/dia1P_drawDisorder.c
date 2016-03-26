#include "pp.h"

int main(int argc, char *argv[])
{

	// argv
	// argv[1] input file
	// argv[2] output file 
	// argv[3] Size of the system
	// argv[4] Exponent of the Power law porbability distribution

	// SOME COLOR DEFINITIONS
	rgb rgb_black, rgb_red, rgb_green;

	rgb_black.r = 0;	rgb_black.g 	= 0;	rgb_black.b = 0;
	rgb_red.r 	= 1;	rgb_red.g 		= 0;	rgb_red.b = 0;
	rgb_green.r = 0;	rgb_green.g 	= 1;	rgb_green.b = 0;

	// High level variables (Remain unchanged)
	int L = 4;	// Inch, side of picture
	int length_inp_dat = 4;	// Number of columns in the input file
	int N = atoi(argv[3]);	// Size of the system
	double bt = atof(argv[4]);	// Size of the system
	double scale = (72.f*L/N);
	int pad = 20;

	// Initialize Input and Output Files
	FILE *in = dia1P_initializeInputFile(argv[1],0);
	FILE *out = fopen(argv[2],"w+");
	
	double *data_ar = (double *)malloc(length_inp_dat*sizeof(double));         // array to hold the scanned data

	// Setup the PS file	
	char *text = "Initial Disorder In Diamond Fuse Network By Ashivni Shekhawat";
	dia1P_drawInitialSetup(out,L,N,scale,text,pad);

	rgb col;
	
	while(dia1P_getData(in,data_ar,length_inp_dat))
	{
			int n1 = (int)data_ar[1];
			int n2 = (int)data_ar[2];
		
			double pc = pow(data_ar[3],bt);	
			if(pc < 0.5)
			{
				col.r = 1 - 2*pc;
				col.g = 0;
				col.b = 2*pc;
			}
			else
			{
				col.r = 0;
				col.g = 2*pc-1;
				col.b = 2*(1-pc);
			}

			dia1P_markBond(n1-1,n2-1,N,col,out);
	}

	fprintf(out,"showpage\n");
	fclose(out);
	fclose(in);
}

