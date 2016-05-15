#include <iostream>

using namespace std;

/*
	Arrays in cpp are pretty much the same as arrays in C. 
	Below are several examples.
*/

void printArr(int ar[], int len)
{
	int i = 0; 
	for(i=0; i<len; i++)
		cout << ar[i] << "\t";
	cout << '\n';
}

void print2DArr(int ar[][4], int row, int col)
{
	int i = 0, j = 0; 
	for(i=0; i<row; i++)
	{
		for(j=0; j<col; j++)
			cout << ar[i][j] << "\t";
		cout << '\n';
	}
	cout << '\n';
}

int main()
{
	// All of the below are equivalent
	int ar1[5] = {1,2,3,4,5};
	int ar2[] = {1,2,3,4,5};
	int ar3[5];
	{
		int i = 0;
		for(i=0; i<5; i++)
		{
			ar3[i] = i+1;
		}
	}

	printArr(ar1,5);
	// Multi-dimensional arrays are straight foward as well
	int ar4[2][4];
	{
		int i=0, j=0;
		for(i=0;i<2;i++)
			for(j=0;j<4;j++)
				ar4[i][j] = i+j;
	}

	print2DArr(ar4,2,4);

	// Character arrays are also the same as C. These are equivalent
	char ar5[] = "Hello";
	char ar6[] = {'H','e','l','l','o','\0'};

	cout << ar5 << endl;
	cout << ar6 << endl;
}


