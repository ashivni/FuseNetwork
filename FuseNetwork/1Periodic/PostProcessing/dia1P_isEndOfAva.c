#include "pp.h"

int dia1P_isEndOfAva(double *data_ar, double *data_ar_prev, int ind)
{
    return (data_ar[ind] > data_ar_prev[ind]) && (data_ar_prev[0] != data_ar_prev[1]);
}

