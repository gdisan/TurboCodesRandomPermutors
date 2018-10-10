#include "Perm.h"


typedef struct 
{
    int v1;
	int G1[2];

	int PermType;
	int N;

	int v2;
	int G2[2];

	bool bPunctured;

	double SNRInit;
	double SNRStep;
	double SNREnd;

	int MaxNumOfIteration;

	bool bFixed;

}TURBO_CODEC;

const int NPERM = 9;


const int RND = 0;
const int SRND = 1;
const int OURRND = 2;
const int BERROU = 3;
const int CVETKOFF1 = 4; // Nonrandom
const int CVETKOFF2 = 5; // Modified Nonrandom
const int GPP = 6;
const int DVBRCS = 7;
const int CCSDS = 8;





