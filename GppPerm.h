// GppPerm.h: interface for the GppPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPPPERM_H__6033F938_644C_449F_84D4_364B205B2FA6__INCLUDED_)
#define AFX_GPPPERM_H__6033F938_644C_449F_84D4_364B205B2FA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* ----------------- 3GPP(3rd Generation PartnerShip Project) Permutor ---------------- */

#include "Perm.h"

const int R = 20; // Number of Rows
const int p = 53; // Prime Number
const int C = p -1; // Number of Columns
const int v = 2;

class GppPerm : public Perm  
{
    int (*m_U)[C]; // Intra-row permutation pattern
    
    int *m_T; // Inter-row permutation pattern

	double (*m_data_in)[C];
	double (*m_data_out)[C];
	double (*m_data_out2)[C];

	/* Positions of dummy bits */
	int m_IR;
	int *m_JC;

public:

	GppPerm();

    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~GppPerm();

};

#endif // !defined(AFX_GPPPERM_H__6033F938_644C_449F_84D4_364B205B2FA6__INCLUDED_)
