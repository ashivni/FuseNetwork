#include "pp.h"

int dia1P_getData(FILE *pf, double *ar, int n_read)
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

