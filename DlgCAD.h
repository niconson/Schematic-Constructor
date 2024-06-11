#pragma once
#include "afxwin.h"
#include "DlgLog.h"


// CDlgCAD dialog

class CDlgCAD : public CDialog
{
	DECLARE_DYNAMIC(CDlgCAD)

public:
	CDlgCAD(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCAD();
	void Initialize(	double version, 
						 CString * folder, 
						 CString * project_folder,
						 CString * app_folder,
						 CString * file_name,
						 int num_addit_layers, 
						 int units,
						 int min_poly_width,
						 int min_text_wid, 
						 int highlight_width, 
						 int min_text_height,
						 int flags, 
						 int layers, 
						 CArray<CPolyLine> * op,
						 CAttrib * Attr, CDisplayList * dl, StrList * ml,
						 CDlgLog * log );
	void SetFields();
	void GetFields();
// Dialog Data
	enum { IDD = IDD_CAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_version;
	double m_file_version;
	CEdit m_edit_folder;
	CButton m_check_free_line;
	CButton m_check_part_line;
	CButton m_check_part_name;
	CButton m_check_pin_line;
	CButton m_check_pin_name;
	CButton m_check_net_line;
	CButton m_check_net_name;
	CButton m_check_add1;
	CButton m_check_add2;
	CButton m_check_add3;
	CButton m_check_add4;
	CButton m_check_add5;
	CButton m_check_add6;
	CButton m_check_add7;
	CButton m_check_add8;
	CButton m_check_add9;
	CButton m_check_add10;
	CButton m_check_add11;
	CButton m_check_add12;
	CButton m_check_add13;
	CButton m_check_add14;
	CButton m_check_footprint;
	CButton m_check_part_value;
	CButton m_check_description;
	CButton m_check_render_all;
	int m_num_addit_layers;
	int m_min_polyline_width;
	int m_min_text_height;
	int m_min_text_line_width;
	int m_highlight_width;
	int m_units;
	int m_flags;
	int m_layers;
	CArray<CPolyLine> * m_op;
	CAttrib * m_Attr; 
	CDisplayList * m_dl;
	StrList * m_ml;
	CDlgLog * m_dlg_log;
	CString m_folder;
	CString m_project_folder;
	CString m_app_folder;
	CString m_f_name;
	afx_msg void OnBnClickedGo();
	CComboBox m_combo_units;
	afx_msg void OnCbnSelchangeComboCadUnits();
	afx_msg void OnBnClickedCancel();
	CEdit m_edit_text_line_w;
	CEdit m_edit_high_w;
	CEdit m_edit_polyline_w;
	CEdit m_edit_text_h;
	afx_msg void OnBnClickedButtonDef();
	afx_msg void OnBnClickedButtonFolder();
	afx_msg void OnBnClickedButtonDone();
	CStatic m_viewer;
	CBitmap m_bmp;
	CButton m_radio_VM;
	CButton m_radio_GV;
	CButton m_radio_GL;
	afx_msg void OnChangeViewer();
};
