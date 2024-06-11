#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <afxcoll.h>
#include <afxtempl.h>


// CDlgProjectOptions dialog

class CDlgProjectOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgProjectOptions)

public:
	CDlgProjectOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProjectOptions();
	void Init( BOOL new_flag,
		CString * name,
		CString * path_to_folder,
		CString * lib_folder,
		CString * app_dir,
		int num_layers,
		BOOL bSMT_connect_copper,
		int node_w,
		int trace_w,
		int font_n,
		int parent,
		float a_spacing,
		double scale);
	CString GetName(){ return m_name; };
	CString GetPathToFolder(){ return m_path_to_folder; };
	CString GetLibFolder(){ return m_lib_folder; };
	int GetNumALayers(){ return m_layers; };
	int GetPolyWidth(){ return m_trace_w; };
	int GetFont(){ return m_font_nmb; }
	int GetNodeWidth(){ return m_node_w; };
	double GetScale(){ return m_scale; }

// Dialog Data
	enum { IDD = IDD_PROJECT_OPTIONS };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	BOOL m_new_project;
	BOOL m_folder_changed;
	BOOL m_folder_has_focus;
	CString m_name;
	CString m_path_to_folder;
	CString m_lib_folder;
	int m_node_w;
	int m_trace_w;
	int m_font_nmb;
	CButton mb_default;
	CComboBox m_font;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnChangeEditName();
	afx_msg void OnEnChangeEditFolder();
	int m_layers;
	double m_scale;
	CEdit m_edit_name;
	CEdit m_edit_folder;
	afx_msg void OnEnSetfocusEditFolder();
	afx_msg void OnEnKillfocusEditFolder();
	CEdit m_edit_lib_folder;
	CEdit m_edit_layers;
public:
	int m_parent;
	int m_default;
	float m_alignment;
	afx_msg void OnBnClickedButtonLib();
	afx_msg void OnBnClickedButtonProjectFolder();
	afx_msg void OnBnClickedQ11();
	afx_msg void OnBnClickedDefaultFolder();
};
