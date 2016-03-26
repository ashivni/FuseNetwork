#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Not enough arguments\n");
		return 1;
	}
	if(argc > 3)
	{
		printf("Too many arguments\n");
		return 1;
	}

	FILE *pfIn = fopen(argv[1],"r");

	if(pfIn== NULL)
	{
		printf("File %s could not be opened.\n",argv[1]);
		return 1;
	}

	FILE *pfOut = fopen(argv[2],"w+");

	if(pfOut== NULL)
	{
		printf("File %s could not be opened.\n",argv[2]);
		return 1;
	}

	char header1[100], header2[100];
	int row = 0, col = 0, nnz = 0;
	int N, i, j , count;
	int ir, jr;
	long double s;

	fscanf(pfIn,"%[^\n]\n",header1);
	fscanf(pfIn,"%[^\n]\n",header2);
	fscanf(pfIn,"%d %d %d\n",&row,&col,&nnz);
	N = (1 + sqrt(1 + 4*row))/2;

	printf("Will rotate matrix with N = %d, %d rows, %d cols, %d nnz and store in file %s\n",N,row,col,nnz,argv[2]);

	fprintf(pfOut,"%s\n",header1);
	fprintf(pfOut,"%s\n",header2);
	fprintf(pfOut,"%d %d %d\n",row,col,nnz);
	for(count = 0; count < nnz ; count ++)
	{
		fscanf(pfIn,"%d %d %lf\n",&i,&j,&s);

		ir = i % (N -1) == 0 ? i - (N-2) : i+1;
		jr = j % (N -1) == 0 ? j - (N-2) : j+1;

		int temp = ir;
		
		ir = temp > jr ? temp : jr;
		jr = temp > jr ? jr : temp;
		
		
		fprintf(pfOut,"%d %d  %.16lf\n",ir,jr,s);
	}

	fclose(pfIn);	
	fclose(pfOut);	
	return 1;
}

