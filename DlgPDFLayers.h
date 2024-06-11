#pragma once
#include "layers.h"

#define NUM_DLG_LAYERS (LAY_ADD_1 + 16)// copper layers

// CDlgPDFLayers dialog

class CDlgPDFLayers : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDFLayers)

public:
	CDlgPDFLayers(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPDFLayers();

// Dialog Data
	enum { IDD = IDD_PDF_LAYERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	int * m_vis;
	int m_rgb[MAX_LAYERS][3];
	int * m_rgb_ptr;
	int m_nlayers;
	//
	int m_print_separate;
	int m_apply_invert;
	int m_sys_colors;
	int m_use_font; //checkbox "use sys font"
	int m_print_1; //checkbox "all pages"
	int m_print_2;
	int m_print_3;
	int m_print_4;
	int m_print_5;
	int m_print_6;
	int m_print_7;
	int m_print_8;
	int m_en_separate;
	CBrush m_brush;
	CColorDialog * m_cdlg;
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_font_box;
	CEdit m_margin_edit;
	float m_margin;
	int m_check[NUM_DLG_LAYERS];
	int m_pdf_checking;
	int m_num_pages;
	char ** m_fonts;
	int n_fonts;
	int i_font;
	void Initialize(	int nlayers, 
						int vis[], 
						int rgb[][3], 
						char * Fonts[], 
						int n_f, 
						int i_f,
						float f_mrg,
						int options,
						int en_separate,
						int num_gps );
	void EditColor( int layer );
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonLayer1();
	afx_msg void OnBnClickedButtonLayer2();
	afx_msg void OnBnClickedButtonLayer3();
	afx_msg void OnBnClickedButtonLayer4();
	afx_msg void OnBnClickedButtonLayer5();
	afx_msg void OnBnClickedButtonLayer6();
	afx_msg void OnBnClickedButtonLayer7();
	afx_msg void OnBnClickedButtonLayer8();
	afx_msg void OnBnClickedButtonLayer9();
	afx_msg void OnBnClickedButtonLayer10();
	afx_msg void OnBnClickedButtonLayer11();
	afx_msg void OnBnClickedButtonLayer12();
	afx_msg void OnBnClickedButtonLayer13();
	afx_msg void OnBnClickedButtonLayer14();
	afx_msg void OnBnClickedButtonLayer15();
	afx_msg void OnBnClickedButtonLayer16();
	afx_msg void OnBnClickedButtonLayer17();
	afx_msg void OnBnClickedButtonLayer18();
	afx_msg void OnBnClickedButtonLayer19();
	afx_msg void OnBnClickedButtonLayer20();
	afx_msg void OnBnClickedButtonLayer21();
	afx_msg void OnBnClickedButtonLayer22();
	afx_msg void OnBnClickedButtonLayer23();
	afx_msg void OnBnClickedButtonLayer24();
	afx_msg void OnBnClickedButtonLayer25();
	afx_msg void OnBnClickedButtonLayer26();
	afx_msg void OnBnClickedButtonLayer27();
	afx_msg void OnBnClickedButtonLayer28();
	afx_msg void OnBnClickedButtonLayer29();
	afx_msg void OnBnClickedButtonLayer30();
	afx_msg void OnBnClickedButtonLayer31();
	afx_msg void OnBnCancel();
	afx_msg void OnCbnSelchangeTextFonts();
};
