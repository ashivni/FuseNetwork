#include "pp.h"

int dia1P_compareDoubles (const void *a, const void *b) 
{
  const double *da = (const double *) a;
    const double *db = (const double *) b;
    
        return (*da > *db) - (*da < *db);
}

