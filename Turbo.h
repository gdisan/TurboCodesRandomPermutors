
// Standart C++
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <stdlib.h>

#include "RSC.h"
#include "Perm.h"

using namespace std;

const int MAXV = 4;
const int MAXSTATES = 16;

extern int *global_data;


void turbocoder( RSC*, Perm*, RSC*, double*, int, double*, double* );

void turbo_decoder( RSC*, Perm*, RSC*, double*, int, double*, double*,
				   double, int*, double*, bool, int );

void Gen_CodeSeq1(double*,double*,double*,double*,int, bool, int );
void Gen_CodeSeq2(double*,double*,double*,int, bool, int );
void Gen_deCodeSeq(double*, double*,double*, double (*Sigma0)[MAXSTATES], 
				   double (*Sigma1)[MAXSTATES],int, int, double, double, int );

void Gen_deCodeSeq_on2decoder(double*, double*, double*, double (*Sigma0)[MAXSTATES], 
							  double (*Sigma1)[MAXSTATES],int, int,double, int );

void VichGamma( double (*Gamma0)[MAXSTATES], double (*Gamma1)[MAXSTATES], double*, int, RSC*, 
			   int, int, int, double, double, double );

void VichBetta( double (*Alpha)[MAXSTATES], double (*Betta)[MAXSTATES], double (*Gamma0)[MAXSTATES],
			   double (*Gamma1)[MAXSTATES], int, int,RSC* );

void VichAlpha( double (*Alpha)[MAXSTATES], double (*Gamma0)[MAXSTATES],double (*Gamma1)[MAXSTATES], int, int, RSC* );
void VichSigma( double (*Sigma0)[MAXSTATES], double (*Sigma1)[MAXSTATES], double (*Alpha)[MAXSTATES],
			   double (*Betta)[MAXSTATES], double (*Gamma0)[MAXSTATES], double (*Gamma1)[MAXSTATES], int , int, RSC* );

void toGauss(int,double* );
void Sort(int *, int );
double randn( double , double );
double sigma( double*, int );
double GetPerr( int*, int );
double GetNerr( int*, int );

