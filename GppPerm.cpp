// GppPerm.cpp: implementation of the GppPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "GppPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GppPerm::GppPerm()
{
   m_T = NULL;
   m_U = NULL;
   m_JC = NULL;

   m_N = 1000;

   m_IR = 0;
}

void GppPerm::GetPermPattern( int NBit )
{
	int i,j;

	/* Inter-row permutation pattern */
    int T[R] = { 19, 9, 14, 4, 0, 2, 5, 7, 12, 18, 10, 8, 13, 17, 3, 1, 16, 6, 15, 11 };

	if( m_T ) delete[] m_T;
	m_T = new int[R];
    for( i = 0; i < R; i++ ) m_T[i] = T[i];

    /* S(j) - Base Sequence for intra-row permutation  */
	int S[C];
	S[0] = 1;
	for( j = 1; j < C; j++ )
		S[j] = (v*S[j-1])%p;

	/* q(i) - Minimum prime integers */
	int q[R] = { 1, 7, 11, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83 };

    /* r(i) - Permuted prime integers */
	int r[R];
	for( i = 0; i < R; i++ )
	    r[i] = q[m_T[i]];

	/* Ui(j) - Intra-row permutation pattern of i-th row */
	if( m_U ) delete[] m_U;
	m_U = new int[R][C];

    for( i = 0; i < R; i++ )
		for( j = 0; j < C; j++ )
			m_U[i][j] = S[(j*r[i])%(p-1)] - 1;
  
}

void GppPerm::Interleave( double data_in[], double data_out[] )
{
   int i,j,k;

   /* Bits-input to rectangular matrix with padding */
   m_data_in = new double[R][C];
   k = -1;
   for( i = 0; i < R; i++ )
   {
	   	for( j = 0; j < C; j++ ) 
		{
			k++;
			if( k >= m_N )
			   m_data_in[i][j] = -1.0; // Dummy bits
			else
			  m_data_in[i][j] = data_in[k];
		}
   }

  
   /* Intra-row permutation */
   if( m_JC ) delete[] m_JC;
   m_JC = new int[40];
   m_data_out = new double[R][C];
   k = 0;

   for( i = 0; i < R; i++ )
	   for(  j = 0; j < C; j++ ) 
	   {
		   m_data_out[i][j] = m_data_in[i][m_U[i][j]];

		   if( m_data_out[i][j] == -1.0 )
		   {
			   m_JC[k] = j;
			   k++;
		   }
	   }

    /* Inter-row permutation */
   m_data_out2 = new double[R][C];

   for( i = 0; i < R; i++ )
	   for(  j = 0; j < C; j++ ) 
	       m_data_out2[i][j] = m_data_out[m_T[i]][j];

    /* Bits-output from rectangular matrix with pruning */
    k = -1; 
    for(  j = 0; j < C; j++ ) 
		for( i = 0; i < R; i++ )
		{    
			k++;
            if( m_data_out2[i][j] == -1.0 ) // Dummy bits
				k--;
			else
			  data_out[k] = m_data_out2[i][j];
			 
		}

	delete[] m_data_in;
	delete[] m_data_out;
	delete[] m_data_out2;
 
} 

void GppPerm::DeInterleave( double data_in[], double data_out[] )
{
   int i,j,k,l;

   m_data_in = new double[R][C];
   k = -1; 
   l = 0;

   for(  j = 0; j < C; j++ ) 
	   for( i = 0; i < R; i++ )
	   {    
			k++;
            if( i == m_IR )
			{
				if( j == m_JC[l] )
				{
                  m_data_in[i][j] = -1.0; // Dummy bits
				  l++;
				  k--;
				}
				else
				  m_data_in[i][j] = data_in[k];
			}
			else
			   m_data_in[i][j] = data_in[k];
			 
	   }

   /* Inter-row permutation */
   m_data_out = new double[R][C];

   for( i = 0; i < R; i++ )
	   for(  j = 0; j < C; j++ ) 
	       m_data_out[m_T[i]][j] = m_data_in[i][j];

	/* Intra-row permutation */
   m_data_out2 = new double[R][C];
   k = 0;

   for( i = 0; i < R; i++ )
	   for(  j = 0; j < C; j++ ) 
	       m_data_out2[i][m_U[i][j]] = m_data_out[i][j];

   k = -1;
   for( i = 0; i < R; i++ )
   {
	   	for( j = 0; j < C; j++ ) 
		{
			k++;
			if( k < m_N )
			  data_out[k] = m_data_out2[i][j];
		}
   }

   delete[] m_data_in;
   delete[] m_data_out;
   delete[] m_data_out2;
	   
}

GppPerm::~GppPerm()
{
   if( m_T ) delete[] m_T;
   if( m_U ) delete[] m_U;
   if( m_JC ) delete[] m_JC;
}
