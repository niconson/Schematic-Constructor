#pragma once
#include "afxwin.h"
#include "afxbutton.h"

// CDlgImportPart dialog

class CDlgImportPart : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportPart)
	enum{ MAX_ICONS=5 };

public:
	CDlgImportPart(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportPart();

// Dialog Data
	enum { IDD = IDD_IMPORT_PART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL bDrawn;
	BOOL bOpenTXTPreixes;
	BOOL m_doc_icon_dir;
	CComboBox m_combo_attr;
	CListBox m_part_list;
	CListBox m_path_list;
	CLibPointer * m_library;
	CArray<CString> * m_parts;
	CArray<CString> * m_attr_str;
	CString m_prefixes;
	CString m_app_dir;
	CString m_str;
	CString m_str2;
	CString * m_wt;
	CString m_doc_path[MAX_ICONS];
	
	int m_current_path;
	int m_search_flags;
	int m_search_mask;
	int m_pin_range[2];

	CComboBox m_description;
	CComboBox m_range1;
	CComboBox m_range2;
	CButton m_b[NUM_F_FLAGS];
	CButton mb_Ignore;
	CButton mb_PcbKit1;
	CButton mb_PcbKit2;
	CButton mb_PcbKit3;
	CButton mb_Ok;
	CButton mb_runlib;
	CStatic m_preview;
	CStatic m_preview2;
	CButton m_doc1, m_doc2, m_doc3, m_doc4;
	CStatic m_static[6];
	void Initialize( CArray<CString> * str, int flags, int mask, int pin_min, int pin_max, CString * caption, CLibPointer * library, CString * app_dir );
	void Change();
	void DrawPattern( CString * VdP );
	void OpenTXTPreixes();
	void UploadPartlist();
	void ReadPrefixes();
	void UpdatebRunLibText();
	void RemoveDescription( CString * str );
	afx_msg void OnChange();
	afx_msg void OnCheck();
	afx_msg void OnPinDef();
	afx_msg void OnClickList();
	afx_msg void OnClickList2();
	afx_msg void OnDblClickList();
	afx_msg void OnDblClickList2();
	afx_msg void OnClickListButton();
	afx_msg void OnUniqueParts();
	afx_msg void OnAboutUniqueParts();
	afx_msg void OnPartsCurrentProject();
	afx_msg void OnOnceAnyProject();
	int PcbAssemblyKit( int MODE );
	afx_msg void OnBnClickedOk();
	//STDMETHODIMP OnDrawItem ( DRAWITEMSTRUCT * lParam, LRESULT * pResult );
	afx_msg void OnCbnDropdownCombo1();
	//afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRunl();
	void CheckDocuments( CString * path, CString * file=NULL );
	afx_msg void OnBnClickedDoc1();
	afx_msg void OnBnClickedDoc2();
	afx_msg void OnBnClickedDoc3();
	afx_msg void OnBnClickedDoc4();
	afx_msg void OnBnClickedDoc5();
	afx_msg void OnBnClickedRadioSearchAllPages();
	afx_msg void OnEnChangeDescription();
	afx_msg void OnSelChangeDescription();
	afx_msg void OnBnClickedQ8();
	
	afx_msg void OnBnClickedQu();
};
