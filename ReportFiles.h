#pragma once

#include "cpdflib.h"
// CDlgRFiles dialog

class CDlgRFiles : public CDialog
{
	DECLARE_DYNAMIC(CDlgRFiles)

public:
	CDlgRFiles(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRFiles();

// Dialog Data
	enum { IDD = IDD_RFILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int GetNextRow( CArray<CString> ** ARRAY, CString * title );
	float PageInit( CPDFdoc * pdf );
	float DrawTextBox(	CPDFdoc * pdf, 
						float x, 
						float y, 
						float w,  
						float str_cnt,
						float textW,
						float R,
						float G, 
						float B,
						CString * text,
						BOOL bDRAW = 0);
	CText * ParseString( CText * T, float W );
	CBrush m_brush;
	CColorDialog * m_cdlg;
	DECLARE_MESSAGE_MAP()
public:
	void Ini( CFreePcbDoc * doc );

	CComboBox m_pic_widths;
	CButton m_include_reflist;
	CButton m_exclude_reflist;
	CButton m_entire;
	CComboBox m_ref_list;
	CFreePcbDoc * m_doc;

private:
	enum{
		eHeader=0, eName, eText, eStroke, eFill, eURL, eSource
	};
	double PageWidth ;
	double PageHeight ;
	double StringHeight;
	double m_pdf_margin;
	double wLink ;
	double wCount ;
	double wPicture ;
	double wValue ;
	double stPicture ;
	double stValue ;
	double stCount ;
	double stLink ;
	double TextHeightPts;
	double LineW;
	double TextW;
	int pagenum;

public:
	void EditColor( int layer );
	afx_msg void OnBnClickedIncl();
	afx_msg void OnBnClickedExcl();
	afx_msg void SaveBomOptions();
	afx_msg void OnBnClickedBomEntireFile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonLayer1();
	afx_msg void OnBnClickedButtonLayer2();
	afx_msg void OnBnClickedButtonLayer3();
	afx_msg void OnBnClickedButtonLayer4();
	afx_msg void OnBnClickedButtonLayer5();
	afx_msg void OnBnClickedButtonLayer6();
	afx_msg void OnBnClickedButtonLayer7();
};