// BerrouGlavieuxPerm.h: interface for the BerrouGlavieuxPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BERROUGLAVIEUXPERM_H__C358C58A_D1AD_4FAA_A788_CF1C0C7E16D2__INCLUDED_)
#define AFX_BERROUGLAVIEUXPERM_H__C358C58A_D1AD_4FAA_A788_CF1C0C7E16D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Perm.h"



/* ------------------======== Claude Berrou & Alain Glavieux Permutor =======-------------- */

#include <cmath>

class BerrouGlavieuxPerm : public Perm  
{
	int *m_pattern;

public:

	BerrouGlavieuxPerm();

    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

    virtual ~BerrouGlavieuxPerm();

};

#endif // !defined(AFX_BERROUGLAVIEUXPERM_H__C358C58A_D1AD_4FAA_A788_CF1C0C7E16D2__INCLUDED_)
