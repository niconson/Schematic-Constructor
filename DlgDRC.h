#pragma once
#include "stdafx.h"
#include "DesignRules.h"
#include "afxwin.h"
#include "PolyLine.h"
#include "DlgLog.h"

// DlgDRC dialog

class DlgDRC : public CDialog
{
	DECLARE_DYNAMIC(DlgDRC)

public:
	DlgDRC(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgDRC(); 
	void Initialize(int units, 
					int part_aa,
					int part_ap,
					int pin_aa,
					int pin_ap );
	void GetFields();
	void SetFields();

// Dialog Data
	enum { IDD = IDD_DRC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_units; 
	int m_repeat_drc;
	double m_part_aa;
	double m_part_ap;
	double m_pin_aa;
	double m_pin_ap;
	CComboBox m_combo_units;
	CEdit m_edit_part_aa;
	CEdit m_edit_part_ap;
	CEdit m_edit_pin_aa;
	CEdit m_edit_pin_ap;
	afx_msg void OnCbnChangeUnits();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck();
	CButton m_check_show_unrouted;
};
