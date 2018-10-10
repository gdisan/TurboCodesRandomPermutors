// RandomPerm.h: interface for the RandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOMPERM_H__EA5CAAEC_FD00_4D3E_9FD6_318BD4148632__INCLUDED_)
#define AFX_RANDOMPERM_H__EA5CAAEC_FD00_4D3E_9FD6_318BD4148632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* -------------------- Random Permutor ----------------- */

#include "Perm.h"
#include <time.h>
#include <cmath>

class RandomPerm : public Perm  
{
    int *m_pattern;

	void Delete( int*, int, int );

public:
	RandomPerm();
    int GetSpread();
    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~RandomPerm();

};

#endif // !defined(AFX_RANDOMPERM_H__EA5CAAEC_FD00_4D3E_9FD6_318BD4148632__INCLUDED_)
