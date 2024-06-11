#pragma once
#include "afxwin.h"

// CDlgImportPart dialog

class CDlgImportMerge : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportMerge)

public:
	CDlgImportMerge(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportMerge();

// Dialog Data
	enum { IDD = IDD_IMPORT_MERGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_merger;
	CListBox m_part_list;
	CListBox m_path_list;
	CLibPointer * m_library;
	CArray<CString> * m_merges;
	int m_current_path;
	CStatic m_preview;
	void Initialize( CLibPointer * library );
	void Change();
	void DrawPattern( CString * VdP );
	afx_msg void OnChange();
	afx_msg void OnClickList();
	afx_msg void OnClickList2();
	afx_msg void OnDblClickList();
	afx_msg void OnDblClickList2();
};
