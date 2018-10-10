// NonRandomPerm.h: interface for the NonRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONRANDOMPERM_H__C92370EC_CBFC_41EE_8EDD_10A54682AE19__INCLUDED_)
#define AFX_NONRANDOMPERM_H__C92370EC_CBFC_41EE_8EDD_10A54682AE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* ------------------ Cvetkoff&Ziablov Nonrandom Permutor ------------------- */

#include "Perm.h"
#include <cmath>


class NonRandomPerm : public Perm  
{

    int *m_pattern;

public:
	
	NonRandomPerm();
    int GetSpread();
	/* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~NonRandomPerm();

};

#endif // !defined(AFX_NONRANDOMPERM_H__C92370EC_CBFC_41EE_8EDD_10A54682AE19__INCLUDED_)
