#pragma once
#include "stdafx.h"
#include "afxwin.h"


// DlgDRC dialog

class DlgPinMap : public CDialog
{
	DECLARE_DYNAMIC(DlgPinMap)

public:
	DlgPinMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgPinMap(); 
	void Initialize( CText * part, CPageList * PGS );

// Dialog Data
	enum { IDD = IDD_PIN_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_BGA;
	CArray<CString> m_list_arr;
	CListBox m_list;
	CEdit m_edit;
	CButton m_butt;
	CText * m_part;
	CPageList * m_pages;
	CArray<CPolyLine> * m_poly;
	void Filter();
	afx_msg void OnChange();
	afx_msg void OnLbnSelchangePinlist();
	afx_msg void OnBnClickedPinbutt();
};