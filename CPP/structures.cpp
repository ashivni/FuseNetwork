#include <iostream>
#include <stdlib.h>
using namespace std;

/*
	Structures in cpp are the same as C.
	Below are several examples
*/

struct train
{
	string name;
	string origin;
	string destination;
	int fare;
};

void printTrain(train T)
{
	cout << T.name << " operates between " << T.origin << " and " << T.destination << " and costs " << T.fare << " dollars ." << endl;
}

int main()
{
	train theOrientExpress;
	train palaceOnWheels;

	theOrientExpress.name = "The Orient Express";
	theOrientExpress.origin = "Istanbul";
	theOrientExpress.destination = "Paris";
	theOrientExpress.fare = 5000;
	printTrain(theOrientExpress);

	palaceOnWheels.name = "Palace On Wheels";
	palaceOnWheels.origin = "New Delhi";
	palaceOnWheels.destination = "Agra";
	palaceOnWheels.fare = 10000;
	printTrain(palaceOnWheels);
	return 1;
}


