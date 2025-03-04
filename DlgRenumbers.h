#pragma once

// DlgRenumbers dialog

class DlgRenumbers : public CDialog
{
	DECLARE_DYNAMIC(DlgRenumbers)

	enum{
		RENUMB=1,
		RAISE,
		CLEAR
	};
public:
	DlgRenumbers(CWnd* pParent = NULL);   // standard constructor
	void Ini( CString * pp, int mode, int LtoR, int num );
	virtual ~DlgRenumbers();
	//
	CComboBox m_combo_prefix;
	CEdit m_edit_shift;
	CEdit m_edit_start;
	CButton m_b_left;
	CButton m_b_top;
	CButton m_b_groups;

	// outputs
	CString m_prefix_str;
	CString m_suffix_str;
	int m_shift;
	int m_start_num;
	int m_left;
	int m_groups;

	//
	BOOL m_mode;
	CStatic m_Static1;
	CStatic m_Static2;
	CStatic m_Static3;
	CString * m_pr;

// Dialog Data
	enum { IDD = IDD_RENUMBERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadioTb();
};
