// ModNonRandomPerm.h: interface for the ModNonRandomPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODNONRANDOMPERM_H__2FBA9668_CF61_40F3_9326_80D7B6798B6E__INCLUDED_)
#define AFX_MODNONRANDOMPERM_H__2FBA9668_CF61_40F3_9326_80D7B6798B6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* ------------------ Cvetkoff&Ziablov Modified Nonrandom Permutor ---------------- */

#include "Perm.h"

const int K = 5;
const int n = 198; 

class ModNonRandomPerm : public Perm  
{

    int (*m_pattern)[n];

	double (*m_data_in)[n];
	double (*m_data_out)[n];

public:

	ModNonRandomPerm();
    
		/* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~ModNonRandomPerm();

};

#endif // !defined(AFX_MODNONRANDOMPERM_H__2FBA9668_CF61_40F3_9326_80D7B6798B6E__INCLUDED_)
