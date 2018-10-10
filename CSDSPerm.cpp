// CSDSPerm.cpp: implementation of the CCSDSPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "CSDSPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCSDSPerm::CCSDSPerm()
{
  m_pattern = NULL;
}

void CCSDSPerm::GetPermPattern( int NBit )
{
  	m_N = NBit;
	
    if( m_pattern ) delete[] m_pattern;
    m_pattern = new int[NBit];

    int m,i,j,t,q,c;

	int k1 = 8;
	int k2 = 223;

	int p[8] = { 31, 37, 43, 47, 53, 59, 61, 67 };

    for( int s = 1; s <= m_N; s++ )
	{
	    m = (s-1)%2;
		i = (int)((s-1)/(2*k2));
		j = (int)((s-1)/2) - i*k2;
        t = (19*i+1)%(k1/2);
		q = t%8 + 1;
		c = (p[q-1]*j + 21*m)%k2;

		m_pattern[s-1] = 2*(t+c*(k1/2)+1) - m;
	}

   
}

void CCSDSPerm::Interleave( double data_in[], double data_out[] )
{
  
  for( int i = 0; i < m_N; i++ )
	data_out[i] = data_in[m_pattern[i]];
}

void CCSDSPerm::DeInterleave( double data_in[], double data_out[] )
{
  for( int i = 0; i < m_N; i++ )
	data_out[m_pattern[i]] = data_in[i];
}

int CCSDSPerm::GetSpread()
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


CCSDSPerm::~CCSDSPerm()
{
   if( m_pattern ) delete[] m_pattern;
}
