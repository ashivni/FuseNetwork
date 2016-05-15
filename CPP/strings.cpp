#include <iostream>
using namespace std;

/*
	CPP has a dedicated string class. 
*/

void reverse(string *s)
{
	// Reverse a string
	int i = 0, L = (*s).length();
	char temp;
	for(i=0; i < L/2; i++)
		{
			temp = (*s)[i];
			(*s)[i] = (*s)[L-1-i];
			(*s)[L-1-i] = temp;
		}

}

int main()
{

	string a = "Word";
	cout << a << a.length() << endl;
	reverse(&a);
	cout << a << endl;
	a = "Wording";
	cout << a << endl;
	reverse(&a);
	cout << a << endl;
	return 1;
}


