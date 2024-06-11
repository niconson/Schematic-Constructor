#pragma once


// CDlgAddMerge dialog

class DlgReplaceText : public CDialog
{
	DECLARE_DYNAMIC(DlgReplaceText)

public:
	DlgReplaceText(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgReplaceText();

// Dialog Data
	enum { IDD = IDD_REPLACE_TEXT };
	enum {	
			M_REPLACE_TEXT = 1
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_str1; //
	CString m_str2; //
	CEdit m_edit1;
	CEdit m_edit2;
	int m_mode;
};