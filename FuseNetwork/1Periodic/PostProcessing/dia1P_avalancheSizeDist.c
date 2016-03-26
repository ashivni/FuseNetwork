#include "pp.h"


int main(int argv, char *argc[])
{
	
	if(argv != 7)
	{
		printf("Usage: ASD InputFile OutputFile NF CN AMax LAV\n");
		printf("InputFile: File to read simulation data from\n");
		printf("OutputFile: File to write avalanche size distribution to\n");
		printf("NF: Number of columns in the input file\n");
		printf("CN: Column number of the data on which to count avalanches\n");
		printf("AMax: Maximum possible avalanche size\n");
		printf("LAV: Include the largest avalanche in statistics? (0: No, 1: Yes)\n");
		exit(1);
	}
	
	FILE *pfIn = dia1P_initializeInputFile(argc[1],3);	// File to read data from
	FILE *pfOut = fopen(argc[2],"w+");					// File to write the results to 

	int length_input_data = atoi(argc[3]);			// Number of fields in the data file, all assumed floats
	int field_pos = atoi(argc[4]) - 1;					// Column number on which avalanches will be calculated
	int max_ava_size = atoi(argc[5]);						// max possible avalanche size
	int largest_ava_inc = atoi(argc[6]);				// Include the largest avalanche in statistics? 0 : No, 1: Yes

	//printf("\ninp file = %s, out_file = %s, length = %d, max_size = %d\n",argc[1],argc[2],length_input_data, max_ava_size);
	
	double *data_ar = (double *)malloc(length_input_data*sizeof(double));					// array to hold the scanned data
	double *data_ar_prev = (double *)malloc(length_input_data*sizeof(double));		// array to hold the data scanned one read ago


	// array to hold the number of avalanches of a particular size.
	// for example, number of avalanches of size 2 = num_ava[2];
	double *num_ava = (double *)malloc((1+max_ava_size)*sizeof(double));		
	// Initialization
	{
		int count = 0;
		for(count = 0; count <= max_ava_size; count++)	num_ava[count] = 0.f;
	}

	double total_ava = 0;	// total number of avalanches
	int ava_size = 0;
	int largest_ava = 0;	


	// Read one line of the data into "previous data"
	dia1P_getData(pfIn,data_ar_prev,length_input_data);
	//printData(stdout,data_ar_prev,length_input_data);
	
	while(dia1P_getData(pfIn,data_ar,length_input_data))
	{
		//printData(stdout,data_ar,length_input_data);
		// Increment the size of the avalanche
		ava_size = ava_size+ (dia1P_isEndOfRun(data_ar)==1 ? 1 : 1);

		if((dia1P_isEndOfRun(data_ar) || dia1P_isEndOfAva(data_ar,data_ar_prev,field_pos)))
		{
			//printf("End of Run or Ava. Adding Avalanche of size %d\n",ava_size);
			num_ava[ava_size] = num_ava[ava_size] + 1;
			total_ava = total_ava + 1;
			//printf("Total_ava = %f, ava_size = %d\n",total_ava,ava_size);
			largest_ava = ava_size > largest_ava ? ava_size : largest_ava;
			ava_size = 0;

			// Copy the present data to previous data
			if(dia1P_isEndOfAva(data_ar,data_ar_prev,field_pos))
			{
				//printf("End of Ava\n");
				int count = 0;
				for(count = 0; count < length_input_data; count++)	data_ar_prev[count] = data_ar[count];
				ava_size = 0;
			}
			else
			{
				if(largest_ava_inc == 0)
				{
					//printf("End of Run, removing largest of size %d\n",largest_ava);
					num_ava[largest_ava] = num_ava[largest_ava] - 1;
					total_ava = total_ava - 1;
					//printf("Total_ava = %f\n",total_ava);
				}
				ava_size = 0;
				largest_ava = 0;

				if(!dia1P_getData(pfIn,data_ar_prev,length_input_data))
				{
					break;
				}
			}
		}
			
	}		

	// write output to file
	{
		int count = 0;
		for(count = 1; count <= max_ava_size; count++)
		{
			if(num_ava[count] > 0)
			{
				fprintf(pfOut,"%d\t%.10lf\n",count,num_ava[count]);
			}
		}
		//printf("Total Ava = %f\n",total_ava);

	}

	fclose(pfIn);
	fclose(pfOut);

	return 1;
}
