// OurRandomPerm.h: interface for the OurRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OURRANDOMPERM_H__B2ACBE4C_EBAF_4B3C_B7C9_F5F05455C0CB__INCLUDED_)
#define AFX_OURRANDOMPERM_H__B2ACBE4C_EBAF_4B3C_B7C9_F5F05455C0CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000





/* ------------------------- New Random Permutor ---------------------- */

#include "Perm.h"
#include <time.h>
#include <cmath>

const int NP = 50;  // Power of a Set of Patterns


class OurRandomPerm : public Perm  
{
    int *m_pattern; // Current Best Pattern

	int *m_patset[NP]; // Set of Patterns

	void Delete( int*, int, int );
    void GetRndPattern();
    

public:
	OurRandomPerm();

    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

    void SetCurPattern( int );
	void SetCurPattern( int*, int );
	void GetCurPattern( int* );
	void Clear();
	bool IsSet( int N ){ return (m_N == N); };	
	
    int GetSpread();

	virtual ~OurRandomPerm();

};

#endif // !defined(AFX_OURRANDOMPERM_H__B2ACBE4C_EBAF_4B3C_B7C9_F5F05455C0CB__INCLUDED_)
