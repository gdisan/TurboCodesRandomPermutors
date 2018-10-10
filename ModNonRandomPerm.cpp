// ModNonRandomPerm.cpp: implementation of the ModNonRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "ModNonRandomPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModNonRandomPerm::ModNonRandomPerm()
{
   m_pattern = NULL;
   m_data_in = NULL;
   m_data_out = NULL;

   m_N = 990;
}

void ModNonRandomPerm::GetPermPattern( int NBit )
{
  if( m_pattern ) delete[] m_pattern;
  m_pattern = new int[K][n];

  int p[K] = { 178, 164, 84, 14, 54 };

  for ( int k = 0; k < K; k++ )
     for( int i = 0; i < n; i++ ) 
		 m_pattern[k][i] = ((p[k]*(i+1))%(n+1))-1;
}

void ModNonRandomPerm::Interleave( double data_in[], double data_out[] )
{
   m_data_in = new double[K][n];
   m_data_out = new double[K][n];
  
   int i,k;


   for( k = 0; k < K; k++ )
	   for( i = 0; i < n; i++ ) 
		   m_data_in[k][i] = data_in[n*k+i];

   for( k = 0; k < K; k++ )
	   for(  i = 0; i < n; i++ ) 
		   m_data_out[k][i] = m_data_in[k][m_pattern[k][i]];

    for(  i = 0; i < n; i++ )
		for( k = 0; k < K; k++ ) 
		   data_out[K*i + k] = m_data_out[k][i];
		

    delete[] m_data_in;
    delete[] m_data_out;

} 

void ModNonRandomPerm::DeInterleave( double data_in[], double data_out[] )
{
   m_data_in = new double[K][n];
   m_data_out = new double[K][n];
  
   int i,k;

   for( i = 0; i < n; i++ )
		for( k = 0; k < K; k++ ) 
			m_data_in[k][i] = data_in[K*i + k];


   for( k = 0; k < K; k++ )
	   for(  i = 0; i < n; i++ ) 
		   m_data_out[k][m_pattern[k][i]] = m_data_in[k][i];
		  

   for( k = 0; k < K; k++ )
	   for( i = 0; i < n; i++ ) 
		   data_out[n*k+i] = m_data_out[k][i];


   delete[] m_data_in;
   delete[] m_data_out;
}



ModNonRandomPerm::~ModNonRandomPerm()
{
   if( m_pattern ) delete[] m_pattern;
}
