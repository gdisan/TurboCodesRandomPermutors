// NonRandomPerm.cpp: implementation of the NonRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Diploma.h"
#include "NonRandomPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NonRandomPerm::NonRandomPerm()
{
   m_pattern = NULL;
   m_N = 996;
}

void NonRandomPerm::GetPermPattern( int NBit )
{
   if( m_pattern ) delete[] m_pattern;
   m_pattern = new int[m_N];

   int p = 89;

   for( int i = 0; i < m_N; i++ ) m_pattern[i] = ((p*(i+1))%(m_N+1))-1;
}

void NonRandomPerm::Interleave( double data_in[], double data_out[] )
{

  for( int i = 0; i < m_N; i++ )
      data_out[i] = data_in[m_pattern[i]];
  
}

void NonRandomPerm::DeInterleave( double data_in[], double data_out[] )
{
  for( int i = 0; i < m_N; i++ )
	data_out[m_pattern[i]] = data_in[i];
}

int NonRandomPerm::GetSpread()
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

NonRandomPerm::~NonRandomPerm()
{
   if( m_pattern ) delete[] m_pattern;
}
