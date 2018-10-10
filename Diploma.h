// Diploma.h : main header file for the DIPLOMA application
//

#if !defined(AFX_DIPLOMA_H__35B68979_4EA1_44C0_9EEB_64A601B8403E__INCLUDED_)
#define AFX_DIPLOMA_H__35B68979_4EA1_44C0_9EEB_64A601B8403E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDiplomaApp:
// See Diploma.cpp for the implementation of this class
//

class CDiplomaApp : public CWinApp
{
public:
	CDiplomaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiplomaApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDiplomaApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIPLOMA_H__35B68979_4EA1_44C0_9EEB_64A601B8403E__INCLUDED_)
