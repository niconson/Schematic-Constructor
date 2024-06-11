#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CDlgPartlistRep dialog

class CDlgPartlistRep : public CDialog
{
	DECLARE_DYNAMIC(CDlgPartlistRep)

public:
	CDlgPartlistRep(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPartlistRep();

// Dialog Data
	enum { IDD = IDD_PARTLISTREPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void DrawListCtrl();   
	void Initialize( CPageList * plist );
	void SaveSelections();
	void RestoreSelections();
	void ReloadList();
	void SaveBomOptions();

	CPageList * m_plist;
	CComboBox m_ref_list;
	CEdit m_det_max;
	int m_n_sel;
	int m_sort_type;
	int m_d_len;
	int m_csv;
	int m_entire;
	int main_page;
	CListCtrl m_list_ctrl;
	CArray<BOOL> bSelected;
	CButton m_bom;
	CButton m_incl;
	CButton m_excl;
	CButton m_ignore;
	afx_msg void OnLvnColumnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTXT();
	afx_msg void OnChange();
	afx_msg void OnClick1();
	afx_msg void OnClick2();
	afx_msg void OnBnClickedBom2();
	afx_msg void OnBnClickedEntirePr();
	afx_msg void OnBnClickedExcl();
	afx_msg void OnBnClickedIncl();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnDropdownRefLists();
	afx_msg void OnBnClickedCancel();
};
