#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int  compare_doubles (const void *a, const void *b)
{
	const double *da = (const double *) a;
   	const double *db = (const double *) b;
     
       	return (*da > *db) - (*da < *db);
}


void  printData(FILE *pf, double *ar, int n_read)
{

	int i = 0;
	for(i=0; i<n_read; i++)
	{
		fprintf(pf,"%.10lf\t",ar[i]);
	}
		fprintf(pf,"\n");
}

int getData(FILE *pf, double *ar, int n_read)
{
	// Change this function if you want to incorporate 
	// new kinds of data file

	int read = 1;
	int i = 0;
	for(i=0; i<n_read-1; i++)
	{
		read = read && (fscanf(pf,"%lf\t",&(ar[i])) == 1);
		if (read == 0) break;
	}
	if (read == 1)
	{
		// Read the last data point
		read = read && (fscanf(pf,"%lf\n",&(ar[n_read-1])) == 1);
	}

	return read;
}

FILE * initializeInputFile(char *s)
{
	// This function initializes a file stream that will be querried 
	// by the function getData to read the data.
	// Initialization etc. should be done here.

	FILE *pf = fopen(s,"r");
	char dummyStr[1000];

	// Ignore the first 3 lines of the input file
	int count = 0;
	for(count=0;count<3;count++) 
	{
		fscanf(pf,"%[^\n]\n",dummyStr);
	}

	return pf;
}

int isEndOfRun(double *data_ar)
{
	// This function tests if the current line of data is the 
	// last in a particular run

	return (data_ar[0] == data_ar[1]);
}

int main(int argv, char *argc[])
{
	
	if(argv != 5)
	{
		printf("Usage: AFS InputFile OutputFile NF AMax\n");
		printf("InputFile: File to read simulation data from\n");
		printf("OutputFile: File to write average strength of the nth bond to fail\n");
		printf("NF: Number of columns in the input file\n");
		printf("L: Linear System Size\n");
		exit(1);
	}
	
	FILE *pfIn = initializeInputFile(argc[1]);	// File to read data from
	FILE *pfOut = fopen(argc[2],"a+");					// File to write the results to 

	int length_input_data = atoi(argc[3]);			// Number of fields in the data file, all assumed floats
	int sys_size = atoi(argc[4]);			// Number of fields in the data file, all assumed floats

	double *data_ar = (double *)malloc(length_input_data*sizeof(double));					// array to hold the scanned data


	double sum_fail_vol = 0.f;		
	double sum_fail_vol_sq = 0.f;		
	double fail_vol = 0.f;
	double sum_fail_cur = 0.f;		
	double sum_fail_cur_sq = 0.f;		
	double fail_cur = 0.f;
	
	int n_sim = 0;	


	while(getData(pfIn,data_ar,length_input_data))
	{
		if(isEndOfRun(data_ar))
		{
			n_sim = n_sim + 1;
			
			sum_fail_vol = sum_fail_vol + fail_vol;
			sum_fail_vol_sq = sum_fail_vol_sq + fail_vol*fail_vol;
			fail_vol = 0;
			
			sum_fail_cur = sum_fail_cur + fail_cur;
			sum_fail_cur_sq = sum_fail_cur_sq + fail_cur*fail_cur;
			fail_cur = 0;
		}
		else
		{
			fail_vol = data_ar[2] > fail_vol ? data_ar[2] : fail_vol;
			fail_cur = data_ar[3]+data_ar[4] > fail_cur ? data_ar[3] + data_ar[4] : fail_cur;
		}
	}		

	// write output to file
	{
		double mean_vol = (sum_fail_vol/n_sim)/sys_size;
		double mean_sq_vol = (sum_fail_vol_sq/n_sim)/(sys_size*sys_size);
		double stdev_vol = sqrt(mean_sq_vol - mean_vol*mean_vol);

		double mean_cur = (sum_fail_cur/n_sim)/sys_size;
		double mean_sq_cur = (sum_fail_cur_sq/n_sim)/(sys_size*sys_size);
		double stdev_cur = sqrt(mean_sq_cur - mean_cur*mean_cur);

		fprintf(pfOut,"%d\t%d\t%.10lf\t%.10f\t%.10lf\t%.10lf\n",sys_size,n_sim,mean_vol,stdev_vol,mean_cur,stdev_cur);
	}

	fclose(pfIn);
	fclose(pfOut);

	return 1;
}
