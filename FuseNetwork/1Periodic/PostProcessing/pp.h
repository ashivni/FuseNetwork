#ifndef _INCLUDES
#define _INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#endif

#ifndef _RGB_STRUCT
#define _RGB_STRUCT
typedef struct rgb_struct
{
  double r, g, b;
}rgb;
#endif

#ifndef _SQUARE_STRUCT
#define _SQUARE_STRUCT
typedef struct square_struct
{
  double x1, x2; 
  double y1, y2; 
}square;
#endif

#ifndef _Dia_MarkSquare
#define _Dia_MarkSquare
void dia1P_markSquare(FILE *pf, const square sq, const rgb color);
#endif


#ifndef _Dia1P_DrawInitialSetup
#define _Dia1P_DrawInitialSetup
void dia1P_drawInitialSetup(FILE *pf, int L, int N, double scale, char *text, int pad);
#endif

#ifndef _Dia1P_MarkBond
#define _Dia1P_MarkBond
int dia1P_markBond(int n1, int n2, int N, rgb color, FILE *pf);
#endif


#ifndef	_Dia1P_CompareDoubles
#define	_Dia1P_CompareDoubles
int dia1P_compareDoubles (const void *a, const void *b);
#endif


#ifndef	_Dia1P_InitializeInputFile
#define	_Dia1P_InitializeInputFile
FILE * dia1P_initializeInputFile(char *s, int nSkip);
#endif


#ifndef	_Dia1P_IsEndOfRun
#define	_Dia1P_IsEndOfRun
int dia1P_isEndOfRun(double *data_ar);
#endif

#ifndef	_Dia1P_PrintData
#define	_Dia1P_PrintData
void  dia1P_printData(FILE *pf, double *ar, int n_read);
#endif

#ifndef	_Dia1P_GetData
#define	_Dia1P_GetData
int dia1P_getData(FILE *pf, double *ar, int n_read);
#endif

#ifndef	_Dia1P_IsEndOfAva
#define	_Dia1P_IsEndOfAva
int dia1P_isEndOfAva(double *data_ar, double *data_ar_prev, int ind);
#endif
