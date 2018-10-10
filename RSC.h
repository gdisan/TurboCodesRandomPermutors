
#if !defined(AFX_RSC_H__77AD8118_9D61_4EF4_8B16_BCB1269A1830__INCLUDED_)
#define AFX_RSC_H__77AD8118_9D61_4EF4_8B16_BCB1269A1830__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/* --------------====== Recursive Systematic Convolutional Code ======-------------- */

#include <cmath>

class RSC  
{
    int m_v; // Memory = Constraint Length - 1
    
	/* Trellis */
	int (*m_NextStates)[2];
	int (*m_OutPuts)[2];
	int (*m_PrevStates)[2];

	int *m_LFR; // Linear Feedback Register Values
    int m_State; // Current State of Coder
 
	int SetBit( int, int, int );
    int HemWeight( int );

public:

	RSC();
  
	int GetV(){return m_v;};
	int Next( int, int );
	int Out( int, int );
	int Prev( int, int );
    void Set( int, int* ); // Set Coder Perfomance & Get Trellis
    void Encode( double*, int, double*, double* ); // Encode bit sequence and Terminate Coder
   
    virtual ~RSC();

};

#endif // !defined(AFX_RSC_H__77AD8118_9D61_4EF4_8B16_BCB1269A1830__INCLUDED_)
