#include "pp.h"

int dia1P_markBond(int n1, int n2, int N, rgb color, FILE *pf)
{

  // Sort to make n1 > n2
  {
    int tmp = n2;
    n2 = n2 > n1 ? n1 : n2;
    n1 = tmp > n1 ? tmp : n1;
  }

	// Find the x, y coordinates of the opposite corners of the square
	// x1, y1 is the bottom left corner, while x2, y2 is th upper right
	int x1, y1, x2, y2;
	
	y1 = (n2/(N/2));	y2 = y1 + 1;

	if( (n1-n2) == N/2 )
	{
		if( n2%N < N/2 )
		{
			x1 = (n2%N)*2;		x2 = x1 + 1;
		}
		else
		{
			x1 = ((n2-N/2)%N)*2;		x2 = x1 + 1;
		}
	}
	else if( n2%N == 0 && (n1+1)%N == 0)
	{
		x1 = N - 1;			x2 = x1 + 1;
	}
	else if( n1 - n2 == 1)
	{
		x1 = N - 1;			x2 = x1 + 1;
	}
	else if( n1 - n2 == N/2+1 )
	{
		x1 = ((n2-N/2)%N)*2+1;		x2 = x1 + 1;
	}
	else if (n1 - n2 == N/2 - 1)
	{
		x1 = (n2%N)*2 - 1;		x2 = x1 + 1;
	}
	else
	{
		printf("Bond Not marked\n");	
		printf("n1 = %d, n2 = %d\n",n1,n2);
		return -1;
	}

	square sq;
	sq.x1 = x1;		sq.x2 = x2;		sq.y1 = y1;		sq.y2 = y2;	
	//printf("n1 = %d, n2 = %d, x1 = %d, y1 = %d\n",n1,n2,x1,y1);
	dia1P_markSquare(pf, sq, color);
	return 0;
}

