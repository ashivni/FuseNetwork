#include "pp.h"

FILE * dia1P_initializeInputFile(char *s, int nSkip) 
{
  // This function initializes a file stream that will be querried 
  // by the function getData to read the data.
  // Initialization etc. should be done here.

	// nSkip: Number of lines to skip

  FILE *pf = fopen(s,"r");
  char dummyStr[1000];

  // Ignore the first 3 lines of the input file
  int count = 0;
  for(count=0;count<nSkip;count++)
  {
    fscanf(pf,"%[^\n]\n",dummyStr);
  }

  return pf;
}

