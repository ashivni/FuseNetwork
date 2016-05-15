#include <iostream>
#include <stdlib.h>
using namespace std;

/*
	Pointers in cpp are also the same as pointers in C. 
	The big difference is that malloc is not the best way to allocated memory in cpp, and must be avoided. 
	new is the way to go. 
	Below are several examples
*/

void setVal(int *pt, int val)
{
	*pt = val;
}

int main()
{
	int a = 1;
	int *b = NULL;

	// b now points to a
	b = &a;

	// Value of a, address of a, value pointed to by b (=a), address of b
	cout << a << ", " << b << ", " << *b << ", " << &b << endl;

	// Value of a, address of a, value pointed to by b (=a), address of b
	setVal(b,12);
	cout << a << ", " << b << ", " << *b << ", " << &b << endl;


	// An example of malloc. Should be avoided.
	// Pointers to pointers
	int **c;

	c = (int **)malloc(10*sizeof(int *));

	{
		 int i = 0, j = 0;
		 for (i=0; i< 10; i++)
		 {
				c[i] = (int *)malloc(4*sizeof(int));
				for(j=0; j<4; j++)
					c[i][j] = i;
		 }
		 
		 
		 for (i=0; i< 10; i++)
		 {
				for(j=0; j<4; j++)
					cout << c[i][j] << "\t";
				cout << endl;
		 }

		 for(i=0; i<10; i++)
		 	free(c[i]);
	}
	
	free(c);

	// An example of new/delete

	int *d = new int [4] ;
	// This is okay
	delete [] d;
	// This would not be okay
	// delete d;

	int *e = new int;
	// This is not okay
	// delete [] e;
	
	// This is okay
	delete e;
}



