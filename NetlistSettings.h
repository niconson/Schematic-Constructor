#pragma once


// CDlgAddMerge dialog

class CDlgNetlistSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgNetlistSettings)

public:
	CDlgNetlistSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNetlistSettings();

// Dialog Data
	enum { IDD = IDD_NETLIST_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void Ini( CPageList * pl, BOOL rnmb_present, CString * project_folder );
	CComboBox m_edit;
	CComboBox m_edit_pcb_names;
	CPageList * m_pl;
	int m_box_index;
	int m_rnmb_present;
	int m_current_i;
	CButton m_pages[MAX_PAGES];
	CButton m_add_pcb;
	CButton m_del_pcb;
	CString m_project_folder;
	//
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnCheck9();
	afx_msg void OnCheck10();
	afx_msg void OnChange1();
	void TryConnectPage( int i_page );
	void SetMask();
	void OnAddPcb();
	void OnDeletePcb();
};