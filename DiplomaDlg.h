// DiplomaDlg.h : header file
//

#if !defined(AFX_DIPLOMADLG_H__41C405C5_5B77_4E70_A6FB_57AA3CA184D7__INCLUDED_)
#define AFX_DIPLOMADLG_H__41C405C5_5B77_4E70_A6FB_57AA3CA184D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CDiplomaDlg dialog

class CDiplomaDlg : public CDialog
{
// Construction
public:
	CDiplomaDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDiplomaDlg)
	enum { IDD = IDD_DIPLOMA_DIALOG };
	CProgressCtrl	m_Line;
	CComboBox	m_perm;
	double	m_pe;
	BOOL	m_punc;
	int		m_back1;
	int		m_forw1;
	int		m_v1;
	int		m_v2;
	int		m_back2;
	int		m_forw2;
	int		m_Len;
	double	m_init;
	double	m_step;
	double	m_end;
	int		m_max;
	int		m_df;
	BOOL	m_fixed;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiplomaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDiplomaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnCheck1();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnCancel2();
	afx_msg void OnOk2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CDiplomaDlg* pdlgMain;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIPLOMADLG_H__41C405C5_5B77_4E70_A6FB_57AA3CA184D7__INCLUDED_)


