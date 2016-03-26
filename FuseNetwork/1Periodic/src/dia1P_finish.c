/**************************************************** 
 *		filename:  	dia1P_finish.c	            *
 *      Author:    	Ashivni Shekhawat               *
 *      Version:   	1.1 (1.1 Created 1st June 09)   *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|17th June 09 
 ----------------------------------------------------
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_finish. The purpose of this function
	close the file pointers in the global object pD.
*/

#include "dia1P.h"

void dia1P_finish(char *caller)
{
	char *name = "dia1P_finish";

		switch (pD.diagOut)
		{
			case 0:	break;

			case 1:
				if(pD.diagFile == NULL)
				{
					dia1P_errHandler(errCode_NullPointerInFuseNetDS, name, caller,errMesg_NullPointerInFuseNetDS);
				}
				else
				{
					fclose(pD.diagFile);
				}
    		if(pD.diagFileName == NULL)
		    {   
    		  dia1P_errHandler(errCode_NullPointer, name, caller,errMesg_NullPointerInFuseNetDS);
		    }   
    		else
		    {   
    		  free(pD.diagFileName);
    		}	   
				break;

			default:
				dia1P_errHandler(errCode_UnknownDiagMode,name,caller,errMesg_UnknownDiagMode);
				break;
		}


    if(pD.outFile == NULL)
    {   
      dia1P_errHandler(errCode_NullPointer, name, caller,errMesg_NullPointerInFuseNetDS);
    }   
    else
    {   
      fclose(pD.outFile);
    }   
    if(pD.outFileName == NULL)
    {   
      dia1P_errHandler(errCode_NullPointer, name, caller,errMesg_NullPointerInFuseNetDS);
    }   
    else
    {   
      free(pD.outFileName);
    }   


    RngStream_DeleteStream(&(pD.gen));

}
