
#include "stdafx.h"
#include "Diploma.h"
#include "BerrouGlavieuxPerm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



BerrouGlavieuxPerm::BerrouGlavieuxPerm()
{
  m_pattern = NULL;
}

void BerrouGlavieuxPerm::GetPermPattern( int NBit )
{
    int P[8] = { 17, 37, 19, 29, 41, 23, 13, 7 };
    int eps,i,j,ir,jr,M;

	m_N = NBit;
	M = (int)sqrt((double)m_N);
	
	if( m_pattern ) delete[] m_pattern;
	m_pattern = new int[NBit];

    for( i = 0; i < M; i++ )
      for( j = 0; j < M; j++ )
	  {
        ir = ( ( M/2 + 1 )*( i + j ) )%M;
        eps = (i + j)%8;
        jr = ( (P[eps]*(j+1)) - 1 )%M;
		m_pattern[i*M + j] = ir*M + jr;
      }
}

void BerrouGlavieuxPerm::Interleave( double data_in[], double data_out[] )
{
  for( int i = 0; i < m_N; i++ )
	data_out[i] = data_in[m_pattern[i]];
}

void BerrouGlavieuxPerm::DeInterleave( double data_in[], double data_out[] )
{
   for( int i = 0; i < m_N; i++ )
	   data_out[m_pattern[i]] = data_in[i];
}

BerrouGlavieuxPerm::~BerrouGlavieuxPerm()
{
  if( m_pattern ) delete[] m_pattern;
}

