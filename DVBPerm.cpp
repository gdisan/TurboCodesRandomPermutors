// DVBPerm.cpp: implementation of the DVBPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "DVBPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DVBPerm::DVBPerm()
{
   m_pattern = NULL;

}

void DVBPerm::GetPermPattern( int NBit )
{
  	m_N = NBit;

    m_n = NBit/2;
	
    if( m_pattern ) delete[] m_pattern;
    m_pattern = new int[m_n];
    
    int P[4] = { 13, 10, 4, 2 };
	int p;

	for( int i = 0; i < m_n; i++ )
	{
	    if( i%4 == 0 ) p = 0;
		if( i%4 == 1 ) p = m_n/2 + P[1];
		if( i%4 == 2 ) p = P[2];
		if( i%4 == 3 ) p = m_n/2 + P[3];

		m_pattern[i] = (P[0]*i + p + 1)%m_n; 
	}
    
}

void DVBPerm::Interleave( double data_in[], double data_out[] )
{
  int i;

  m_data_out = new double[m_N];

  /* Permutation inside in couples */
  for( i = 0; i < m_n; i++ )
  {
	if( i%2 == 0 )
	{
      m_data_out[i*2] = data_in[i*2+1];
	  m_data_out[i*2+1] = data_in[i*2];
	}
	else
	{
	  m_data_out[i*2] = data_in[i*2];
	  m_data_out[i*2+1] = data_in[i*2+1];
	}
  }
  
  /* Permutation beetween couples */
  for( i = 0; i < m_n; i++ )
  {
	data_out[i*2] = m_data_out[m_pattern[i]*2];
	data_out[i*2+1] = m_data_out[m_pattern[i]*2+1];
  }

 

  delete[] m_data_out;
}

void DVBPerm::DeInterleave( double data_in[], double data_out[] )
{
  int i;

  m_data_out = new double[m_N];

  /* Permutation beetween couples */
  for( i = 0; i < m_n; i++ )
  {
	m_data_out[m_pattern[i]*2] = data_in[2*i];
	m_data_out[m_pattern[i]*2+1] = data_in[2*i+1];
  }

   /* Permutation inside in couples */
  for( i = 0; i < m_n; i++ )
  {
	if( i%2 == 0 )
	{
      data_out[i*2] = m_data_out[i*2+1];
	  data_out[i*2+1] = m_data_out[i*2];
	}
	else
	{
	  data_out[i*2] = m_data_out[i*2];
	  data_out[i*2+1] = m_data_out[i*2+1];
	}
  }

  delete[] m_data_out;
}

int DVBPerm::GetSpread()
{
  int Spread = m_N;

  int count =0;
  
  int MaxMinSpread = (int)(sqrt(2.0*m_N));

  int StopPoint = 0,tmp;

  for( int i=0; i < m_N; i++ )
  {
	  tmp = i + MaxMinSpread + 1;
      if( tmp > m_N )
          StopPoint = m_N;
      else
          StopPoint = tmp;
      
      for( int j = i+1; j < StopPoint; j++ )
	  {
		  tmp = abs(m_pattern[i]-m_pattern[j]) + abs(i-j);

          if( tmp < MaxMinSpread/2 ) count++; // Number of bad pair
 
		  if( Spread > tmp )
             Spread = tmp;
	  }
  }

  return (int)Spread;
}

DVBPerm::~DVBPerm()
{
  if( m_pattern ) delete[] m_pattern;
}
