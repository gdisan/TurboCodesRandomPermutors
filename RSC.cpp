
#include "stdafx.h"
#include "Diploma.h"
#include "RSC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



RSC::RSC()
{
  m_State = 0;
  m_v = 0;
  m_NextStates = NULL;
  m_OutPuts = NULL;
  m_PrevStates = NULL;
  m_LFR = NULL;
}

int RSC::Next( int state, int input )
{
  return m_NextStates[state][input];
}

int RSC::Out( int state, int input )
{
  return m_OutPuts[state][input];
}

int RSC::Prev( int state, int input )
{
  return m_PrevStates[state][input];
}

void RSC::Set( int Mem, int CodeGen[] )
{
    m_State = 0;

    if( m_NextStates ) delete[] m_NextStates;
    if( m_OutPuts ) delete[] m_OutPuts;
    if( m_PrevStates ) delete[] m_PrevStates;
	if( m_LFR ) delete[] m_LFR;

    int NumOfStates,state,input,st,exit,nextstate,code;

    m_v = Mem; 
    NumOfStates = (int)pow( 2.0, m_v );

	m_NextStates = new int[NumOfStates][2];
	m_OutPuts = new int[NumOfStates][2];
	m_PrevStates = new int[NumOfStates][2];
	m_LFR = new int[NumOfStates];
  
    for( state = 0; state < NumOfStates; state++ )
	{
	    for( input = 0; input < 2; input++ )
		{
          st = SetBit( state, m_v, input ); // [ input state ]
	      exit = (HemWeight( st&CodeGen[0] ))%2 ; // recursive input
		  nextstate = SetBit( (state>>1), m_v-1, exit );
		  st = SetBit( state, m_v, exit ); // [ recursive_input state ]
		  exit = (HemWeight( st&CodeGen[1] ))%2; // parity bit

		  code = SetBit( exit, 1, input ); // [ input code ]

          m_NextStates[state][input] = nextstate;
		  m_PrevStates[nextstate][input] = state;
		  m_OutPuts[state][input] = code;
		}

		m_LFR[state] = (HemWeight( state&CodeGen[0] ))%2; 
	}
}

void RSC::Encode( double data[], int DataSize, double code[], double tail[] )
{
   int out, i, input;

   for( i = 0; i < m_v; i++ )
	   tail[i] = 0;

   for( i = 0; i < DataSize; i++ )
   {
	 input = (int)data[i];

     out = m_OutPuts[m_State][input];

     out = out&1; // 0-bit = parity bit

     code[i] = out;

     m_State = m_NextStates[m_State][input];
   }

   /* Terminate Coder */
   for( i = 0; i < m_v; i++ )
   {
	   input = m_LFR[m_State];
 
       out = m_OutPuts[m_State][input];

       out = out&1; // 0-bit = parity bit

       code[DataSize+i] = out;
	   tail[i] = input;

       m_State = m_NextStates[m_State][input];	   
	}
}

RSC::~RSC()
{
     if( m_NextStates ) delete[] m_NextStates;
     if( m_OutPuts ) delete[] m_OutPuts;
     if( m_PrevStates ) delete[] m_PrevStates;
	 if( m_LFR ) delete[] m_LFR;
}

int RSC::SetBit( int number, int bitnum, int bit )
{
  int ret;

  if( bit )
    ret = number|(1<<bitnum);
  else
	ret = number&(~(1<<bitnum));

  return ret;
}

int RSC::HemWeight( int number )
{
  int i,count = 0;

  for( i = 0; i < ( (sizeof(int))*8 ); i++ )
   count += ( (number>>i) & 1 );

  return count;
}
