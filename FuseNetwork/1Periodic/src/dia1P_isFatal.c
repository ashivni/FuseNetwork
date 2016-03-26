/**************************************************** 
 *		filename:   dia1P_isFatal.c	            *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.2 (1.1 Created 14th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				|14th June 09 
 ----------------------------------------------------
	1.2		| Updated to include the 	|17th June 09 
			| NULL pointer error and    |
			| the BadErrCode error		|
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_isFatal. The purpose of this function
	is to determine if a given error code corresponds
	to a fatal error. Function returns 1 if the 
	error is fatal
*/

#include "dia1P.h"

int dia1P_isFatal(int ErrCode, char *caller)
{
	char *name = "dia1P_isFatal";
	if(caller == NULL)
	{
		dia1P_errHandler(errCode_NullPointer,name,"Caller Null","Null pointer in input arguments");
	}
	switch (ErrCode)
	{
		case errCode_FileNotFound :					return 1;
		case errCode_OutOfMemory : 					return 1;
		case errCode_BadInitialization : 			return 1;
		case errCode_InconsistentDataStructure :	return 1;
		case errCode_NullPointer :					return 1;
		case errCode_BadErrCode :					return 1;
		case errCode_NullPointerInCholmodDS :		return 1;
		case errCode_NullPointerInFuseNetDS :		return 1;
		case errCode_WrongSizeAllocation	:		return 1;
		case errCode_TooManyArguments		:		return 1;
		case errCode_RandTooSmall		:		return 1;
		default : 
			dia1P_errHandler(errCode_BadErrCode,name,caller,errMesg_UnknownErrorCode);	return 1;
	}
}
