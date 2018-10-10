// RandomPerm.cpp: implementation of the RandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "RandomPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RandomPerm::RandomPerm()
{
  m_pattern = NULL;
}

void RandomPerm::GetPermPattern( int NBit )
{
  	m_N = NBit;
	
    if( m_pattern ) delete[] m_pattern;
    m_pattern = new int[NBit];

    srand( (unsigned)time( NULL ) );

	int * A = new int[m_N];

    for( int i = 0; i < m_N; i++ )
		A[i] = i;

	int random;

	for( i = 0; i < m_N; i++ )
	{
		random = (rand())%(m_N-i);
		
		m_pattern[i] = A[random];

		Delete( A, random, m_N - i );
	}
    
	delete[] A;
}

void RandomPerm::Interleave( double data_in[], double data_out[] )
{
  
  for( int i = 0; i < m_N; i++ )
	data_out[i] = data_in[m_pattern[i]];
}

void RandomPerm::DeInterleave( double data_in[], double data_out[] )
{
  for( int i = 0; i < m_N; i++ )
	data_out[m_pattern[i]] = data_in[i];
}

void RandomPerm::Delete( int mas[], int index, int limit )
{
  for( int i = index; i < (limit-1); i++ ) mas[i] = mas[i+1];
}

int RandomPerm::GetSpread()
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

RandomPerm::~RandomPerm()
{
  if( m_pattern ) delete[] m_pattern;
}
