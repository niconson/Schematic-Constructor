#pragma once
#include "afxwin.h"

enum{
	F_BEGIN=0,
	F_END,
	F_FIND,
	F_WORD,
	F_CASE,
	F_PREFIX,
	F_REF,
	F_VALUE,
	F_FOOT,
	F_PIN,
	F_NET,
	F_DESC,
	F_OTHER,
	F_ALLPAGES,
	F_USE_LIST,
	F_DEF_SIZES,
	NUM_F_FLAGS,
	NO_SRC
};

// CDlgFindPart dialog

class CDlgFindPart : public CDialog
{
	DECLARE_DYNAMIC(CDlgFindPart)

public:
	CDlgFindPart(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFindPart();

// Dialog Data
	enum { IDD = IDD_SHOW_ATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_combo_attr;
	CArray<CString> * sel_attr_str;
	CString m_str;
	CString * m_wt;
	int m_search_flags;
	int m_search_mask;
	int m_pin_range[2];
	CComboBox m_range1;
	CComboBox m_range2;
	CButton m_b[F_USE_LIST];
	void Initialize( CArray<CString> * str, int flags, int mask, int pin_min, int pin_max, CString * caption=NULL );
	afx_msg void OnCheck();
	afx_msg void OnPinDef();
};
