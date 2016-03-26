#include "pp.h"

int dia1P_isEndOfRun(double *data_ar)
{
  // This function tests if the current line of data is the 
  // last in a particular run

  return (data_ar[0] == data_ar[1]);
}

