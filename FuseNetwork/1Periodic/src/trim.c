/****************************************************
 *      filename:   trim.c 		                    *
 *      Author:     Ashivni Shekhawat               *
 *      Version:    1.1 (Created 1st June 09)       *
 ****************************************************/

/*
	This file contains the code for the function trim.
	The purpose of this function is to trim the leading 
	and trailing spaces of a string and return a pointer
	to the trimmed string. Special attention is given
	to memory leaks.
*/


#include "dia1P.h"

char *trim(char *str)
{
		printf("Starting trim: %p\n",str);
    size_t len = 0;
    char *frontp = str - 1;
    char *endp = NULL;

    if( str == NULL )
            return NULL;

    if( str[0] == '\0' )
            return str;

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address
     * the first non-whitespace characters from
     * each end.
     */
    while( isspace(*(++frontp)) );
    while( isspace(*(--endp)) && endp != frontp );

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so
     * that if it's dynamically allocated, we can
     * still free it on the returned pointer.  Note
     * the reuse of endp to mean the front of the
     * string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) *endp++ = *frontp++;
            *endp = '\0';
    }


		printf("End trim: %p\n",str);
    return str;
}

