// SRandomPerm.h: interface for the SRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SRANDOMPERM_H__68FFCCEA_E246_4E26_93D7_0B97C981DC19__INCLUDED_)
#define AFX_SRANDOMPERM_H__68FFCCEA_E246_4E26_93D7_0B97C981DC19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Perm.h"
#include <cmath>

class SRandomPerm : public Perm  
{

	int *m_pattern;

	void Delete( int*, int, int );

public:

    SRandomPerm();

	int GetSpread();
    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~SRandomPerm();

};

#endif // !defined(AFX_SRANDOMPERM_H__68FFCCEA_E246_4E26_93D7_0B97C981DC19__INCLUDED_)
