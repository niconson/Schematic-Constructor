#pragma once
#include "afxwin.h"


// CDlgProtection dialog

class CDlgProtection : public CDialog
{
	DECLARE_DYNAMIC(CDlgProtection)

public:
	CDlgProtection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProtection();

// Dialog Data
	enum { IDD = IDD_MAKE_ARCHIVED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	enum{
		UNPROTECT=-1,
		P_ENABLE,
		P_DISABLE,
		P_DIS_ALL
	};
	CFreePcbDoc * m_doc;
	void Initialize( CFreePcbDoc * doc );
	BOOL FindArchive( int ipcb );
	CButton mb_en;
	CButton mb_dis;
	CButton mb_dis_all;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
