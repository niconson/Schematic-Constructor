#pragma once
#include "afxwin.h"


// CDlgProtection dialog

class DlgCreateBranch : public CDialog
{
	DECLARE_DYNAMIC(DlgCreateBranch)

public:
	DlgCreateBranch(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgCreateBranch();

// Dialog Data
	enum { IDD = IDD_CREATE_BRANCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	enum{
		P_INCREMENT=0,
		P_SUFFIX 
	};
	enum{
		MAX_POS=15
	};
	CFreePcbDoc * m_doc;
	void Initialize( CFreePcbDoc * doc );
	CButton mb_increment;
	CButton mb_suffix;
	CEdit m_edit;
	CString m_suffix;
	afx_msg void OnBnClickedOk();
	void OnBnClickedCheckCopy();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
};
