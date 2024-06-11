#pragma once

class CDlgCompare : public CDialog
{
	DECLARE_DYNAMIC(CDlgCompare)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_COMPARE };
	BOOL BY_COORDINATES;
	//
	BOOL CNETLIST;
	CString m_netlist_name;
	CString m_netlist_path;
	//
	BOOL DLG_CREATED;
	CDlgCompare(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCompare();
	int IP1;
	int IP2;
	CStatic Group;
	CFreePcbDoc * m_doc;
	CButton mb_by_ref;
	CButton mb_by_pos;
	CButton mb_swap_all_pins;
	CComboBox mc_page1;
	CComboBox mc_page2;
	CListBox report1;
	CListBox report2;
	CListBox report3;
	CListBox report4;
	CListBox report5;
	CListBox report6;
	CArray<CString> DATA1;
	CArray<CString> DATA2;
	CArray<CString> DATA3;
	CArray<CString> DATA4;
	CArray<CString> DATA5;
	CArray<CString> DATA6;
	//
	BOOL SelectNetlist();
	void ReloadPages();
	BOOL DATA5_FindNet( CString * net );
	int Conformity( CString * s1, CString * s2 );
	int ComparePartlist( int Pg1, int Pg2 );
	int CompareNetlist( int Pg1, int Pg2 );
	CPolyLine * FindPin( CText * part, CString * pin, CArray<CPolyLine> * po );
	CText * FindPartByCoordinates( CArray<CPolyLine> * po, int x, int y, CString * compRef, int numCorners );
	afx_msg void OnLbnSelchangeListDiff1();
	afx_msg void OnLbnSelchangeListDiff2();
	afx_msg void OnLbnSelchangeListDiff3();
	afx_msg void OnLbnSelchangeListDiff4();
	afx_msg void OnLbnSelchangeListDiff5();
	afx_msg void OnLbnSelchangeListDiff6();
	afx_msg void OnAddedDblClick();
	afx_msg void OnValueDblClick();
	afx_msg void OnFootprintDblClick();
	afx_msg void OnDeletedDblClick();
	afx_msg void OnPinsNetsDblClick();
	afx_msg void OnSwapPinsDblClick();
	afx_msg void OnBnClickedCompare();
	afx_msg void OnBnClickedChParts();
	afx_msg void OnBnClickedChNets();
	afx_msg void OnBnClickedSelect1();
	afx_msg void OnBnClickedSelect2();
	afx_msg void OnBnClickedSelect3();
	afx_msg void OnBnClickedSelect4();
	afx_msg void OnBnClickedRepairAll();
	afx_msg void OnBnClickedSelNl2();
};

class CCompare
{
public:
	CCompare();
	~CCompare();
	void Compare( CFreePcbDoc * doc, BOOL bNetlist );
};

