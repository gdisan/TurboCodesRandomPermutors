// Perm.h: interface for the Perm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(AFX_PERM_H__5ECF6553_7462_4392_B288_29B431E571B7__INCLUDED_)
#define AFX_PERM_H__5ECF6553_7462_4392_B288_29B431E571B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* ----------------====== Permutor Base Class ==========----------------- */

class Perm  
{
protected:

	int m_N; // Number of bits to permute

	/* You'd better use as follows:
	   
		 private int m_N;

         protected void SetN(int);
		 protected int GetN();
	*/

public:

	Perm();

	virtual void Interleave( double*, double* ){};
	virtual void GetPermPattern( int ){};
    virtual void DeInterleave( double*, double* ){};

	virtual ~Perm();

};

#endif // !defined(AFX_PERM_H__5ECF6553_7462_4392_B288_29B431E571B7__INCLUDED_)
