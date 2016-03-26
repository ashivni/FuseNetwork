/****************************************************
 *		filename:	dia1P.h						*
 *      Author: 	Ashivni Shekhawat				*
 *		Version:	1.1 (Created 1st June 09)		*
 ****************************************************/
// This file is update too often, and I have not kept
// a good version history.
/* 	
	This file contains all function prototypes,
	error codes, data structure prototypes and 
	other includes. Each build must be updated 
    when this file is updated.
*/

#ifndef _Includes
#define _Includes

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cholmod.h>
#include <RngStream.h>

#endif

#ifndef _ProbDef
#define _ProbDef

struct dia1P_probDef
{
	int gridSize;	// grid size of the problem

	int NSim;		// Number of simulations

	int density;	// type of distribution of failure strenghts
								// 0 for uniform (nothing else supported currently)
	double densityParam1;
	double densityParam2;

	int diagMode;	// Level of diagnostic output
					// 0 for silent, 1 for minimal, 2 for verbose 

	int diagOut;	// Destination of diagnostic output. 
					// 0 defaults to stdout; 1 to user specified file

	char *diagFileName; // File name to be specified if diagOut == 1 
	FILE *diagFile; // File stream to be specified if diagOut == 1

	char *outFileName;	// Name of the output file
	FILE *outFile;	// File stream

	int seedMode; 
	
	RngStream gen;	// Stream of random numbers for the program
					// Usage x = RngStream_RandU01(gen)
};

#endif

#ifndef _FailureSite
#define _FailureSite
typedef struct dia1P_failureSite_struct
{
	int node1, node2;	// Failure between node1 and node2
	double fVol;		// Faliure voltage
	double fCur;		// Faliure current
	double str;			// Strength of the failed bond
}dia1P_failureSite;
#endif

#ifndef _BrokenBonds
#define _BrokenBonds
typedef struct dia1P_brokenBonds_struct
{
	/* Data structure stores the sequence of failed bonds.
		For k < nFail, i[k] and j[k] are the pair of sites
		the bonds between which snapped in the k+1 th 
		iteration and v[k] is the corresponding voltage.
	*/
	int nFail;			// Number of failures
	int *i;				// Index of failure sites, size maxFail
	int *j;				// Index of failure sites, size maxFail
	int *BondArr;   // Keeps track of broken bonds (flips parity at the broken bond number)
	double *v;			// Array of voltages at each step
	double *c;			// Array of currents at each step
	double *bondStrength;  // Array of strengths of broken bonds
}dia1P_brokenBonds;
#endif


#ifndef _ErrCodes
#define _ErrCodes

#define errCode_FileNotFound 				100
#define errCode_OutOfMemory					110
#define errCode_BadInitialization			200
#define errCode_TooManyArguments			210
#define errCode_UnknownDensityInInput		220
#define errCode_UnknownDiagModeInInput		230
#define errCode_UnknownStatModeInInput		240
#define errCode_UnknownAvalModeInInput		250
#define errCode_InconsistentDataStructure	300
#define errCode_UnknownDiagMode				310
#define errCode_UnknownStatMode				320
#define errCode_UnknownAvalMode				330
#define errCode_NullPointer					400
#define errCode_CholmodReturnedNullPointer	410
#define errCode_NullPointerInFuseNetDS		411
#define errCode_NullPointerInCholmodDS		420
#define errCode_BadErrCode					500
#define errCode_WrongSizeAllocation			600
#define errCode_RandTooSmall				700
#endif

#ifndef _ErrMesgs
#define _ErrMesgs

#define errMesg_NullPointerInArguments      "NULL Pointer in Input Arguments"
#define errMesg_FileOpenWPMode              "Cannot Open File in w+ Mode"
#define errMesg_FileOpenWMode               "Cannot Open File in w Mode"
#define errMesg_FileOpenAPMode              "Cannot Open File in a+ Mode"
#define errMesg_FileOpenAMode               "Cannot Open File in a Mode"
#define errMesg_FileOpenRMode               "Cannot Open File in r Mode"
#define errMesg_CholmodReturnedNullPointer  "Cholmod Returned NULL Pointer"
#define errMesg_NullPointerInCholmodDS		"NULL Pointer Member in Cholmod Data Structure"
#define errMesg_NullPointerInFuseNetDS		"NULL Pointer Member in FuseNet Data Structure"
#define errMesg_CallerNull                  "Null Caller"
#define errMesg_MallocNullPointer           "Malloc Returned NULL Pointer"
#define errMesg_UnsortedNodesInFD           "Unsorted Nodes in FuseNet DataStructure fuseNet2P_failureSite"
#define errMesg_WrongSizeAllocation			"Allocated Size Of Data Structure Different From Number Of Polulated Sites"
#define errMesg_TooManyArguments			"Too Many Arguments"
#define errMesg_UnknownErrorCode			"Unknown Error Code"
#define errMesg_UnknownDiagMode				"Unknown Diagnostic Mode Set"
#define errMesg_UnknownStatMode				"Unknown Statistical Output Mode Set"
#define errMesg_UnknownAvalMode				"Unknown Avalanche Output Mode Set"
#define errMesg_UnknownDensityInInput		"Unknown Density Of Random Fuse Strengths (Value on Line 2 of Input File Can be 0 only"
#define errMesg_UnknownDiagModeInInput		"Unknown DiagMode. (Value on Line 3 of Input File Can be 0, 1, 2 or 3 only"
#define errMesg_UnknownStatModeInInput		"Unknown DiagMode. (Value on Line 3 of Input File Can be 0, 1, 2 or 3 only"
#define errMesg_RandTooSmall							"Random Number Too Small. Increase Simulation BT value"
#define errMesg_InconsistentDataStructure	 "Datastructure logically inconsistent"
#endif


#ifndef _Consts
#define _Consts

#define dia1P_Tiny	1e-6
#define dia1P_Broken 1
#define dia1P_UnBroken 0

#endif

#ifndef _GlobalVars
#define _GlobalVars
struct dia1P_probDef pD;
#endif

#ifndef _ErrHandler
#define _ErrHandler
void dia1P_errHandler(int ErrCode, char *caller, char *prevFunc, char *mesg);
#endif	

#ifndef _IsFatal
#define _IsFatal
int dia1P_isFatal(int ErrCode, char *caller);
#endif

#ifndef _Initialize
#define _Initialize
void dia1P_initialize(char *fName, char *caller);
#endif

#ifndef _Trim
#define _Trim
char *trim(char *);
#endif

#ifndef _PrintPD
#define _PrintPD
void dia1P_printPD(char *caller);
#endif

#ifndef _StiffnessMatrix
#define _StiffnessMatrix
cholmod_sparse *dia1P_stiffnessMatrix(cholmod_common *Common, char *caller);
#endif

#ifndef _StrengthVector
#define _StrenghtVector
double *dia1P_strengthVector(char *caller);
#endif

#ifndef _LoadVector
#define _LoadVector
cholmod_sparse *dia1P_loadVector(cholmod_common *Common, char *caller);
#endif

#ifndef _BondToSnap
#define _BondToSnap
void dia1P_bondToSnap(double *S, cholmod_dense *VNode_d, cholmod_sparse *VNode_s, cholmod_sparse *V2C, dia1P_brokenBonds *BB, dia1P_failureSite *FS, cholmod_common *Common, char *caller);
#endif

#ifndef _InitializeBrokenBonds
#define _InitializeBrokenBonds
dia1P_brokenBonds *dia1P_initializeBrokenBonds(char *caller);
#endif

#ifndef _UpdateBrokenBonds
#define _UpdateBrokenBonds
void dia1P_updateBrokenBonds(dia1P_brokenBonds *BB, dia1P_failureSite *FS, char *caller);
#endif

#ifndef _IsSnapped
#define _IsSnapped
int dia1P_isSnapped(dia1P_brokenBonds *BB, int node1, int node2, char *caller);
#endif

#ifndef _StiffnessUpdateVector
#define _StiffnessUpdateVector
cholmod_sparse *dia1P_stiffnessUpdateVector(dia1P_failureSite *FD, cholmod_common *Common, char *caller);
#endif

#ifndef _LoadUpdateVector
#define _LoadUpdateVector
cholmod_sparse *dia1P_loadUpdateVector(dia1P_failureSite *FD, cholmod_common *Common, char *caller);
#endif

#ifndef _IsBroken
#define _IsBroken
int dia1P_isBroken(cholmod_sparse *NodeV_s, cholmod_sparse *V2C, cholmod_common *Common, char *caller);
#endif

#ifndef _WriteAnim
#define _WriteAnim
void dia1P_writeAnim(dia1P_brokenBonds *BB, cholmod_common *Common, char *caller);
#endif

#ifndef _FreeBrokenBonds
#define _FreeBrokenBonds
void dia1P_freeBrokenBonds(dia1P_brokenBonds **BB, char *caller);
#endif

#ifndef _Finish
#define _Finish
void dia1P_finish(char *caller);
#endif

#ifndef _VoltageToCurrentMatrix
#define _VolgateToCurrentMatrix
cholmod_sparse *dia1P_voltageToCurrentMatrix(cholmod_common *Common, char *caller);
#endif

#ifndef _UpdateVoltageToCurrentMatrix
#define _UpdateVolgateToCurrentMatrix
int dia1P_updateVoltageToCurrentMatrix(cholmod_sparse *A, dia1P_failureSite *FD, cholmod_common *Common, char *caller);
#endif

#ifndef _GetBondNumber
#define _GetBondNumber
int dia1P_getBondNumber(int node1, int node2, char *caller);
#endif

#ifndef _GetNodeNumber
#define _GetNodeNumber
void dia1P_getNodeNumbers(int *node1, int *node2, int bondNumber, char *caller);
#endif

