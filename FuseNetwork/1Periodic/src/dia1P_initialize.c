/**************************************************** 
 *		filename:  	dia1P_initialize.c          *
 *      Author:    	Ashivni Shekhawat               *
 *      Version:   	1.2 (1.1 Created 14th June 09)  *
 ====================================================
 *      		Revision History					*
 ====================================================
 *	Version	|			Comment			| 	Date	*
 ====================================================
 	1.1		| First version				| 14th June 09 
 ----------------------------------------------------
	1.2		| Updated for better diagn-	| 16th June 09 
			| ostic and output handling |
 ****************************************************/

/*
	This file contains the code for the function
	dia1P_initialize. The purpose of this function
	is to read the given input file and initialize
	the global variable pD accordingly.
*/

#include "dia1P.h"

void dia1P_initialize(char *fName, char *caller)
{
	char *name = "dia1P_initialize";
	FILE *pf = fopen(fName,"r");

	if(pf==NULL)
	{
		dia1P_errHandler(errCode_FileNotFound,name,caller,errMesg_FileOpenRMode);
	}
	else
	{
		char dummy[1000];
		char *fname1 = malloc(500*sizeof(char));
		char *fname2 = malloc(500*sizeof(char));
		int i;
		fscanf(pf,"%d%[^\n]",&(pD.gridSize),dummy);

		fscanf(pf,"%d%[^\n]",&(pD.NSim),dummy);

		fscanf(pf,"%d\t%lf\t%lf%[^\n]",&(pD.density),&(pD.densityParam1),&(pD.densityParam2),dummy);
		if(pD.density!=0 && pD.density!=1)
		{
			dia1P_errHandler(errCode_UnknownDensityInInput,name,caller,errMesg_UnknownDensityInInput);
		}

		fscanf(pf,"%d%[^\n]",&(pD.diagMode),dummy);
		if(pD.diagMode!=0 && pD.diagMode!=1 && pD.diagMode!=2 && pD.diagMode!=3)
		{
			dia1P_errHandler(errCode_UnknownDiagModeInInput,name,caller,errMesg_UnknownDiagModeInInput);
		}

		fscanf(pf,"%d",&(pD.diagOut));
		switch (pD.diagOut)
		{
			case 0:
				pD.diagFile = stdout;	
				pD.diagFileName = "stdout";
				fscanf(pf,"%[^\n]",dummy);
				break;

			case 1:
				fscanf(pf,"%s\t%[^\n]",fname1,dummy);
				//fname1 = trim(fname1);
				pD.diagFile = fopen(fname1,"w+");
				pD.diagFileName = fname1;			
				if(pD.diagFile == NULL)
				{
					dia1P_errHandler(errCode_FileNotFound, name, caller,errMesg_FileOpenWPMode);
				}
				break;

			default:
				dia1P_errHandler(errCode_BadInitialization,name,caller,"diagOut on line 4 of input file can be 0 or 1 only");
				break;
		}

    fscanf(pf,"%s\t%[^\n]",fname2,dummy);
    //fname2 = trim(fname2);
    pD.outFileName = fname2;    
    pD.outFile = fopen(fname2,"w+");

    if(pD.outFile == NULL)
    {   
      dia1P_errHandler(errCode_FileNotFound, name, caller,errMesg_FileOpenWPMode);
    }   


    fscanf(pf,"%d\t",&(pD.seedMode));
    unsigned long seed[6];
    // Populate seed
    if(pD.seedMode == 0) // Seed random if seedMode == 0
    {   
          srand((unsigned)(time(NULL)/2));
          for(i=0;i<6;i++){
              seed[i] = rand();
          }   
      //Write seed to file 'seed.txt'
      {
        FILE *fTmp = fopen("seed.txt","w+");
            for(i=0;i<6;i++){
                fprintf(fTmp,"%ld\t",seed[i]);
            }
        fprintf(fTmp,"\n");
        fclose(fTmp);
      }
    }

    if(pD.seedMode == 1)  //Seed read from file if seedMode == 1
    {
      char *fnameTmp = (char *)malloc(100*sizeof(char));
      fscanf(pf,"%s\t%[^\n]",fnameTmp,dummy);
      FILE *fTmp = fopen(fnameTmp,"r");
      for(i = 0; i< 6 ; i++)
      {
        fscanf(fTmp,"%ld\t",&(seed[i]));
      }
      fclose(fTmp);
      free(fnameTmp);
    }


    RngStream_SetPackageSeed(seed);
    /* get a stream */
    pD.gen = RngStream_CreateStream ("generator_1");
    fprintf(pD.outFile,"%%%% METADATA: System Size and random seeds\n%%%% %d\t",pD.gridSize);
    for(i=0;i<6;i++){
      fprintf(pD.outFile,"%ld\t",seed[i]);
    }
    fprintf(pD.outFile,"\n%%%%\n");

    if(pD.diagOut == 0) free(fname1);

	}
	
	fclose(pf);
	
}
