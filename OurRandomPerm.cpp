// OurRandomPerm.cpp: implementation of the OurRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "OurRandomPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OurRandomPerm::OurRandomPerm()
{
   m_pattern = NULL;
   m_N = 0;
}

void OurRandomPerm::GetPermPattern( int NBit )
{
  	m_N = NBit;

    if( m_pattern ) delete[] m_pattern;
    m_pattern = new int[NBit];

	int i,j,spread = 0;

    for( i = 0; i < NP; i++ )
	{
		while( spread < 4 )
		{
          GetRndPattern();
		  spread = GetSpread();
		}
        m_patset[i] = new int[m_N];
        for( j = 0; j < m_N; j++ )
		  m_patset[i][j] = m_pattern[j];
	}
}

void OurRandomPerm::GetRndPattern()
{
   
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

int OurRandomPerm::GetSpread()
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

  return (int)Spread; //( (MaxMinSpread-1)*(m_N-1) + 1 ) - всего значений;
}

void OurRandomPerm::Interleave( double data_in[], double data_out[] )
{
    for( int i = 0; i < m_N; i++ )
	    data_out[i] = data_in[m_pattern[i]];
}

void OurRandomPerm::DeInterleave( double data_in[], double data_out[] )
{
  for( int i = 0; i < m_N; i++ )
	  data_out[m_pattern[i]] = data_in[i];
}

void OurRandomPerm::Delete( int mas[], int index, int limit )
{
  for( int i = index; i < (limit-1); i++ ) mas[i] = mas[i+1];
}

void OurRandomPerm::SetCurPattern( int i )
{
   for( int j = 0; j < m_N; j++ )
       m_pattern[j] = m_patset[i][j];
}

void OurRandomPerm::SetCurPattern( int mas[], int len )
{
   m_N = len;
   if( m_pattern ) delete[] m_pattern;
   m_pattern = new int[m_N];

   for( int j = 0; j < m_N; j++ )
   	   m_pattern[j] = mas[j];
}

void OurRandomPerm::GetCurPattern( int mas[] )
{
    for( int j = 0; j < m_N; j++ )
   	    mas[j] = m_pattern[j];
}

void OurRandomPerm::Clear()
{
  for( int i = 0; i < NP; i++ )
    delete[] m_patset[i];
}

OurRandomPerm::~OurRandomPerm()
{
  if( m_pattern ) delete[] m_pattern;
}
