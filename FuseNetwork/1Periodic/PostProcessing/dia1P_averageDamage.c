#include "pp.h"


int main(int argv, char *argc[])
{
	
	if(argv != 6)
	{
		printf("Usage: AVGDMG InputFile OutputFile NF CN SysSize\n");
		printf("InputFile: File to read simulation data from\n");
		printf("OutputFile: File to write avalanche size distribution to\n");
		printf("NF: Number of columns in the input file\n");
		printf("CN: Column number of load\n");
		printf("SysSize: Linear system size\n");
		exit(1);
	}
	
	FILE *pfIn = dia1P_initializeInputFile(argc[1],3);	// File to read data from
	FILE *pfOut = fopen(argc[2],"a+");					// File to write the results to 

	int length_input_data = atoi(argc[3]);			// Number of fields in the data file, all assumed floats
	int field_pos = atoi(argc[4]) - 1;          // Column number of load
	int sys_size = atoi(argc[5]);								// linear system size

	double *data_ar = (double *)malloc(length_input_data*sizeof(double));					// array to hold the scanned data
	double *data_ar_prev = (double *)malloc(length_input_data*sizeof(double));		// array to hold the data scanned one read ago

	double sum_damage = 0, sum_damage_sq = 0, damage = 0;	// Damage till end
	double sum_DBF = 0, sum_DBF_sq = 0, DBF = 0;					// Damage Before Fracture 
	int n_sim = 0;
	int ava_size;

	// Read one line of the data into "previous data"
	dia1P_getData(pfIn,data_ar_prev,length_input_data);
	
	while(dia1P_getData(pfIn,data_ar,length_input_data))
	{
		// Increment the size of the avalanche
		ava_size = ava_size + 1;
		damage = damage+1;

		if((dia1P_isEndOfRun(data_ar) || dia1P_isEndOfAva(data_ar,data_ar_prev,field_pos)))
		{
			// Copy the present data to previous data
			if(dia1P_isEndOfAva(data_ar,data_ar_prev,field_pos))
			{
				int count = 0;
				for(count = 0; count < length_input_data; count++)	data_ar_prev[count] = data_ar[count];
				ava_size = 0;
			}
			else
			{
				sum_damage		= sum_damage 		+ damage/(2*sys_size*sys_size);	// Add the average damage fraction
				sum_damage_sq	= sum_damage_sq + pow(damage/(2*sys_size*sys_size),2.f);	
		
				damage = damage - ava_size;		// Remove the last avalanche to get the damage before peak stress
				sum_DBF		= sum_DBF	+ damage/(2*sys_size*sys_size);	// Add the average damage fraction at peak stress
				sum_DBF_sq	= sum_DBF_sq + pow(damage/(2*sys_size*sys_size),2.f);	

				n_sim = n_sim + 1;
				damage = 0;

				if(!dia1P_getData(pfIn,data_ar_prev,length_input_data))
				{
					break;
				}
			}
			ava_size = 0;
		}
			
	}		

	// write output to file
	{
		double mean_damage = sum_damage/n_sim;
		double mean_damage_sq = sum_damage_sq/n_sim;
		double stdev_damage = sqrt(mean_damage_sq - mean_damage*mean_damage);

		double mean_DBF = sum_DBF/n_sim;
		double mean_DBF_sq = sum_DBF_sq/n_sim;
		double stdev_DBF = sqrt(mean_DBF_sq - mean_DBF*mean_DBF);

		fprintf(pfOut,"%d\t%d\t%.10lf\t%.10lf\t%.10lf\t%.10lf\n",sys_size,n_sim,mean_damage,stdev_damage,mean_DBF,stdev_DBF);

	}

	fclose(pfIn);
	fclose(pfOut);

	return 1;
}
