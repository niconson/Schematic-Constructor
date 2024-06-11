#pragma once
#include "afxwin.h"


// CComponentNoteExt dialog

class CComponentNoteExt : public CDialog
{
	DECLARE_DYNAMIC(CComponentNoteExt)

public:
	CComponentNoteExt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CComponentNoteExt();

// Dialog Data
	enum { IDD = IDD_COMPONENT_NOTE_EXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int ext_num;     
	CString ext;
	void Initialize( int num );
	void Sync( CFreePcbDoc * doc );
	CButton m_1;
	CButton m_2;
	CButton m_3;
	CButton m_external_file;
	BOOL m_extern;
	afx_msg void OnBnClickedExfile();
};