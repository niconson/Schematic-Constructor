#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CDlgPartlist dialog

class CDlgPartlist : public CDialog
{
	DECLARE_DYNAMIC(CDlgPartlist)

public:
	CDlgPartlist(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPartlist();

// Dialog Data
	enum { IDD = IDD_PARTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void Size( int cx, int cy, int save );
	void DrawListCtrl();
	void Initialize( CFreePcbView * view );
	void SaveSelections();
	void RestoreSelections();
	void ReDraw();
	void OnFilter();
	void GetColWidths();
	void ImportPartlist(BOOL bReplaceSel=0);
	int GetPCBButtonCheck();
	int Find( CString * s, CString * page );
	//
	CFreePcbView * m_view;
	CPageList * m_pl;
	int m_sort_type;
	int m_colw[4];
	CListCtrl m_list_ctrl;
	CEdit m_flt;
	CButton Butt[7];
	CArray<BOOL> bSelected;
	CArray<BOOL> bVisibled;
	CString m_current_pcb_name;
	//
	afx_msg void OnLvnColumnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnJumpTo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChange();
	afx_msg void OnPCBCheck();
	afx_msg void JumpTo();
	afx_msg void OnCancel();
	afx_msg void OnHighlight();
	afx_msg void OnTXT();
	afx_msg void InvertSelection();
	afx_msg void OnBnClickedSall();
	afx_msg void OnBnClickedHpcb();
	afx_msg void OnEnChangeFilter();
};