#pragma once
#include "afxwin.h"


// CDlgAttr dialog

class CDlgAttr : public CDialog
{
	DECLARE_DYNAMIC(CDlgAttr)

public:
	CDlgAttr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAttr();
	void Initialize( CArray<CString> * F,
					 CFreePcbDoc * doc,
					 CString * ref, 
					 CString * pin, 
					 CString * pindesc, 
					 CString * net, 
					 CString * fname, 
					 CString * partv,
					 CString * folder,
					 CString * footprint_ext,
					 CString * lib_dir,
					 CString * prefixes,
					 attr_size size,
					 int units, 
					 int mask );

// Dialog Data
	enum { IDD = IDD_ATTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_check_apply_ref;
	CEdit m_edit_height_ref;
	CEdit m_edit_width_ref;
	CButton m_check_apply_pin;
	CEdit m_edit_height_pin;
	CEdit m_edit_width_pin;
	CButton m_check_apply_net;
	CEdit m_edit_height_net;
	CEdit m_edit_width_net;
	CButton m_check_apply_pdesc;
	CEdit m_edit_height_pdesc;
	CEdit m_edit_width_pdesc;
	CButton m_check_apply_foot;
	CEdit m_edit_height_foot;
	CEdit m_edit_width_foot;
	CButton m_check_apply_value;
	CEdit m_edit_height_value;
	CEdit m_edit_width_value;
	CButton q1;
	CButton q2;
	CButton q3;
	CButton q4;
	CButton m_open_lib;
	CButton Bcmd[7];
	attr_size m_Asize;
	int m_Units;
	int m_mask;
	CAttrib * m_att;
	CPageList * m_pl;
	CFreePcbDoc * m_doc;
	CComboBox m_edit_ref;
	CComboBox m_edit_pin;
	CEdit  m_edit_pindesc;
	CComboBox m_edit_netname;
	CComboBox m_edit_fname;
	CComboBox m_edit_value;
	CString m_ref;
	CString m_pin;
	CString m_pindesc;
	CString m_net;
	CString m_fname;
	CString m_partv;
	CString m_folder;
	CString m_footprint_ext;
	CString m_lib_dir;
	CString m_prefixes;

	CArray<CString> * m_F;
	afx_msg void OnChange();
	afx_msg void OnOpenLib();
	afx_msg void OnQ1();
	afx_msg void OnQ2();
	afx_msg void OnQ3();
	afx_msg void OnQ4();
	afx_msg void OnQ5();
	afx_msg void OnQ6();
	afx_msg void OnQ7();
	afx_msg void OnQ8();
	afx_msg void OnQ9();
	afx_msg void OnQ10();
	//
	afx_msg void OnCmd1();
	afx_msg void OnCmd2();
	afx_msg void OnCmd3();
	afx_msg void OnCmd4();
	afx_msg void OnCmd5();
	afx_msg void OnCmd6();
	afx_msg void OnCmd7();
	//
	afx_msg int OnCopyData(CWnd* pWnd, COPYDATASTRUCT* Msg);
	// Show Hint
	afx_msg void OnCbnSelchangePartvalue();
	afx_msg void OnCbnSelchangeFname();
	afx_msg void OnCbnSelchangePinname();
	afx_msg void OnCbnSelchangeNetname();
	void ShowHint( int item );
	afx_msg void OnCbnSelchangeRefdes();
	afx_msg void OnCbnSelendokRefdes();
	afx_msg void OnCbnCloseupRefdes();
	afx_msg void OnCbnEditupdateRefdes();
};
