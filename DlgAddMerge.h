#pragma once


// CDlgAddMerge dialog

class CDlgAddMerge : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddMerge)

public:
	CDlgAddMerge(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddMerge();

// Dialog Data
	enum { IDD = IDD_ADD_MERGE };
	enum {	M_MERGES = 1,
			M_REPLACE_PATT,
			M_REPLACE_POLY,
			M_ADDPAGE,
			M_RENAMEPAGE,
			M_READPAGE,
			M_ADD_REF,
			M_REF_LIST,
			M_REM_LIST,
			M_REPLACE_FAV,
			M_COPY_FOOTPRINT,
			M_SET_POLY_LENGTH,
			M_URL
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_merge_name; //
	CArray<CString> m_combo_box;
	CStatic m_stat;
	CComboBox m_edit;
	CFreePcbDoc * Doc;
	int m_mode;
	int m_box_index;
};