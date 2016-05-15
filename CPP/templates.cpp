#include <iostream>

using namespace std;

/*
	A function template is the first C++ feature that I encountered
	that is different from C. Coming from a Python background, it seems 
	a very funny way to trying to do duck typing. Its almost as if 
	they wanted duck typing, but were too much grounded in C to think 
	with clarity! Any way, the following is the syntax to declare 
	a function template, for a function called func, that uses 
	an aproiri unknown type T in its call signature 

	template <typename T> void func(T a){ 	cout << a << endl; }

	So far, so good. However, the following is apparent illegal

	template <typename T> void func(int a){ 	cout << a << endl; }

	why you might ask? There is no good reason I could find, except that 
	the compiler (and the binary) cannot figure out what T means. Does it 
	need to in this situation? No. Does it still care? Yes. 
	Further more, even this is illegal

	template <typename T> void func(int a){ 	T b = a; cout << a << endl; }

	That is truly funny, since the compiler can now figure out that T is int. 

	It seems that the general rule is that the compiler should not be able to 
	ascertain the type of T at compile time, and should be able to do so at run time. 

	Below are several examples.
*/

template <typename T> void printIt(T a){ 	cout << a << endl; }

template <typename T1, typename T2> T1 addThem(T1 a, T2 b){ return a+b;}

template <typename T3, typename T4> T3 multThem(T3 a, T4 b){ return a*b;}
int main()
{

	{ 
		int a = 1;
		printIt(a);
	}

	{
		double c = 1.1, d = 2.2;
		cout << addThem(d,c) << endl;
	}

	{
		int a = 1;
		double b = 1.1;
		// These two are different
		cout << addThem(a,b) << endl;
		cout << addThem(b,a) << endl;
	}

	{
		int a = 1;
		double b = 2.1;
		// These will confuse the compiler
		// cout << multThem(a,b) << endl;
		cout << multThem(a,b) << endl;
		
	}

}
