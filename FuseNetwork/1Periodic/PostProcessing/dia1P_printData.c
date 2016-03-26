#include "pp.h"
void  printData(FILE *pf, double *ar, int n_read)
{

  int i = 0;
  for(i=0; i<n_read; i++)
  {
    fprintf(pf,"%.10lf\t",ar[i]);
  }
    fprintf(pf,"\n");
}

