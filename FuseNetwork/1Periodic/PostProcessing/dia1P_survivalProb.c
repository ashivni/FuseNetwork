#include "pp.h"


int main(int argv, char *argc[])
{
	
	if(argv != 6)
	{
		printf("Usage: SP InputFile OutputFile NF NSim L\n");
		printf("InputFile: File to read simulation data from\n");
		printf("OutputFile: File to write the survival probability\n");
		printf("NF: Number of columns in the input file\n");
		printf("NSim: Number of simulations\n");
		printf("L: Linear system size\n");
		exit(1);
	}
	
	FILE *pfIn = dia1P_initializeInputFile(argc[1],3);	// File to read data from
	FILE *pfOut = fopen(argc[2],"w+");					// File to write the results to 

	int length_input_data = atoi(argc[3]);			// Number of fields in the data file, all assumed floats
	int n_cdf = atoi(argc[4]);				// Number of readings for the cdf or sdf
	int sys_size = atoi(argc[5]);			// Linear system size

	double *data_ar = (double *)malloc(length_input_data*sizeof(double));					// array to hold the scanned data

	double *vol = (double *)malloc(n_cdf*sizeof(double));		// array to hold the failure voltages
	double *cur = (double *)malloc(n_cdf*sizeof(double));		// array to hold the failure current

	double fail_vol = 0.f;
	double fail_cur = 0.f;
	
	int n_sim = 0;	

	while(dia1P_getData(pfIn,data_ar,length_input_data))
	{
		if(dia1P_isEndOfRun(data_ar))
		{
			if(n_sim == n_cdf)
			{
				printf("Too many simulations.\n");
				exit(1);
			}

			vol[n_sim] = fail_vol;
			fail_vol = 0;
			
			cur[n_sim] = fail_cur;
			fail_cur = 0;

			n_sim = n_sim + 1;
		}
		else
		{
			fail_vol = data_ar[2] > fail_vol ? data_ar[2] : fail_vol;
			fail_cur = data_ar[3] > fail_cur ? data_ar[3] : fail_cur;
		}
	}		

	// sort to get SDF
	qsort(vol,n_sim,sizeof(double),dia1P_compareDoubles);	
	qsort(cur,n_sim,sizeof(double),dia1P_compareDoubles);	

	// write output to file
	{
		int count = 0;
		for(count = 0; count < n_sim; count++)
		{
			fprintf(pfOut,"%G\t%G\t%G\n",(1.f*n_sim -count)/(n_sim +1),vol[count],cur[count]);
		}
	}

	fclose(pfIn);
	fclose(pfOut);

	return 1;
}
