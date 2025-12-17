#pragma once
#include "afxwin.h"


// CDlgAddMaskCutout dialog

class CDlgAddMaskCutout : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddMaskCutout)

public:
	CDlgAddMaskCutout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddMaskCutout();
	void Initialize( int l, int h, int num_layers, int units, int polyline_w, int bCl );
// Dialog Data
	enum { IDD = IDD_ADD_MASK_CUTOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_radio_open;
	CButton m_radio_closed;
	CButton m_radio_type_off;
	CButton m_range;
	CEdit m_edit_width;
	CEdit m_edit_width_min;
	CEdit m_edit_width_max;
	CComboBox m_combo_units;
	int m_units;
	int m_width;
	int m_width_min;
	int m_width_max;
	int m_closed_flag;
	int m_type_off_flag;
	int m_hatch_off_flag;
	int m_layer_off_flag;
	int m_range_flag;
	int m_no_contours;
	//
	CComboBox m_combo_layer;
	CComboBox m_combo_tone;
	CButton m_radio_none;
	CButton m_radio_edge;
	CButton m_radio_full;
	CButton m_radio_dotted;
	CButton m_radio_hatch_off;
	CButton m_monochrom;
	//
	CButton m_def_layer;
	CButton m_layer_off;
	int m_layer;
	int m_hatch;
	int m_num_layers;
	afx_msg void OnDef();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedDotted();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnCbnSelchangeComboAddPolyUnits();
	void GetFields();
	void SetFields();
	//
	BOOL bNEW;
	afx_msg void OnBnClickedApplyInRange();
	afx_msg void OnBnClickedNotChange3();
	afx_msg void OnBnClickedNotChange2();
};
