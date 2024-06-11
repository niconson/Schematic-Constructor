#pragma once


// CDlgAddMerge dialog

class CDlgAttrSync : public CDialog
{
	DECLARE_DYNAMIC(CDlgAttrSync)

public:
	CDlgAttrSync(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAttrSync();

// Dialog Data
	enum { IDD = IDD_ATTR_SYNC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void Ini( CFreePcbDoc * doc );
	CPageList * m_pl;
	CFreePcbDoc * m_doc;
	int m_box_index;
	CButton m_page[MAX_PAGES];
	CButton m_mirror[MAX_PAGES];
	CListBox m_excl;
	afx_msg void OnCheck();
	afx_msg void OnCheck2();
	afx_msg void OnQ1();
	afx_msg void OnQ2();
	afx_msg void OnAddRef();
	afx_msg void OnRemove();
};