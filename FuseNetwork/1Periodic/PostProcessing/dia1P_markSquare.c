#include "pp.h"

void dia1P_markSquare(FILE *pf, const square sq, const rgb color)
{
    fprintf(pf,"\n%lf\t%lf translate\n",sq.x1,sq.y1);
    fprintf(pf,"\t newpath\n");
		fprintf(pf,"\t 0 0 moveto\n");
		fprintf(pf,"\t %lf 0 lineto\n",sq.x2-sq.x1);
		fprintf(pf,"\t %lf %lf lineto\n",sq.x2-sq.x1, sq.y2-sq.y1);
		fprintf(pf,"\t 0 %lf lineto\n",sq.y2-sq.y1);
		fprintf(pf,"\tclosepath\n\t %lf\t%lf\t%lf setrgbcolor\n\t fill\n",color.r,color.g,color.b);
    fprintf(pf,"%lf\t%lf translate\n",-sq.x1,-sq.y1);
}

