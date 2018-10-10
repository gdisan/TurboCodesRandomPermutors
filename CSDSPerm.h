// CSDSPerm.h: interface for the CCSDSPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSDSPERM_H__7252EE06_17A8_42D4_A5E1_97ABAFE5D3B2__INCLUDED_)
#define AFX_CSDSPERM_H__7252EE06_17A8_42D4_A5E1_97ABAFE5D3B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* --------------------- CCSDS Standart for Space Telemetry ------------- */

#include "Perm.h"
#include <cmath>

class CCSDSPerm : public Perm  
{
    int *m_pattern;
  
    
public:


	CCSDSPerm();
    int GetSpread();
    /* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~CCSDSPerm();

};

#endif // !defined(AFX_CSDSPERM_H__7252EE06_17A8_42D4_A5E1_97ABAFE5D3B2__INCLUDED_)
