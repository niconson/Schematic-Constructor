#pragma once
#include "afxwin.h"


// CDlgMyMessageBox dialog

class CDlgMyMessageBox : public CDialog
{
	DECLARE_DYNAMIC(CDlgMyMessageBox)

public:
	CDlgMyMessageBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMyMessageBox();

// Dialog Data
	enum { IDD = IDD_MY_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_message;
	CStatic m_q;
	CStatic m_picture;
	CBitmap m_bmp;
	long long m_bitmap_ptr;
	void Initialize( LPCTSTR mess, long long bitmap_ptr );
	CString m_mess;
	BOOL m_bHideCursor;
};
