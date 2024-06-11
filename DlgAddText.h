#pragma once
#include "afxwin.h"


// CDlgAddText dialog

class CDlgAddText : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddText)

public:
	CDlgAddText(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddText();
	void Initialize( CPageList * pl,
					 CString * str, 
					 CString * special_chars,
					 int units,
					 int height, 
					 int width, 
					 int x, 
					 int y, 
					 int font_n, 
					 int pdf_opt,
					 int mask_att,
					 double scale );

// Dialog Data
	enum { IDD = IDD_ADD_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetFields();
	void GetFields();
	CPageList * m_pl;
	int m_x, m_y;			// set on entry if editing
	int m_width;
	int m_height;
	int m_mask_att;
	BOOL m_bDrag;
	BOOL m_bUseDefaultWidth;
	int m_units;
	int m_unit_mult;
	int m_pdf_options;
	int m_font_number;
	int m_remain;
	int m_remain_italic;
	double m_gl_scale;
	CButton m_def;
	CButton m_use_f;
	CButton m_graphic;
	CString m_str;
	CString m_special;
	CString m_old_str;
	CText * m_text_ptr;
	virtual BOOL OnInitDialog();
	CString * m_text_sizes;
	CArray<CString> * m_text_arr;
	CComboBox m_edit_height;
	CButton m_button_set_width;
	CButton m_button_def_width;
	CEdit m_edit_width;
	CComboBox m_text;
	CEdit m_edit_y;
	CEdit m_edit_x;
	CButton m_button_set_position;
	CButton m_button_drag;
	afx_msg void OnBnClickedSetPosition();
	afx_msg void OnBnClickedDrag();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSetWidth();
	afx_msg void OnBnClickedDefWidth();
	CComboBox m_combo_units;
	CComboBox m_combo_font;
	afx_msg void OnCbnSelchangeComboAddTextUnits();
	CButton m_check_pdf_rt_align;
	CButton m_inter_letter_compress;
	CButton m_inter_letter_unclench;
	CButton m_italic;
	afx_msg void OnLbnSelchangeListLayer();
	afx_msg void OnBnClickedCheckCompressed();
	afx_msg void OnBnClickedCheckUnclenched();
	afx_msg void OnEnSetfocusEditText();
	afx_msg void OnCbnSelchangeEditHeight();
//	afx_msg void OnCbnSelendokEditHeight();
};
