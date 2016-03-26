#include "pp.h"

void dia1P_drawInitialSetup(FILE *pf, int L, int N, double scale, char *text, int pad)
{
  fprintf(pf,"%%!\n");

  fprintf(pf,"%%%% %s\n",text);

  fprintf(pf,"%%%%BoundingBox: %d %d %d %d\n",72-pad,72-pad,(L+1)*72+pad,(L+1)*72+pad);

  fprintf(pf,"72\t72 translate\n");

  fprintf(pf,"%g\t%g scale\n",scale,scale);
  fprintf(pf,"0 setlinewidth\n");

}

