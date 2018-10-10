// DVBPerm.h: interface for the DVBPerm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVBPERM_H__114A6A52_7FE1_4D6E_BBD0_27AA415760E3__INCLUDED_)
#define AFX_DVBPERM_H__114A6A52_7FE1_4D6E_BBD0_27AA415760E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* ------------- DVB Standart for Return Channel via Satellite (RCS)  -------- */

#include "Perm.h"
#include <cmath>

class DVBPerm : public Perm  
{

	int *m_pattern;
	
    int m_n; // Number of Couples

	double *m_data_out;
	
public:

	DVBPerm();
    int GetSpread();
	/* Overriding */
    void Interleave( double*, double* );
    void GetPermPattern( int );
    void DeInterleave( double*, double* );

	virtual ~DVBPerm();

};

#endif // !defined(AFX_DVBPERM_H__114A6A52_7FE1_4D6E_BBD0_27AA415760E3__INCLUDED_)
