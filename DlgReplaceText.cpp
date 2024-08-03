// DlgReplaceText.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgReplaceText.h"

// CDlgAddMergeName dialog

IMPLEMENT_DYNAMIC(DlgReplaceText, CDialog)
DlgReplaceText::DlgReplaceText(CWnd* pParent /*=NULL*/)
	: CDialog(DlgReplaceText::IDD, pParent)
{
	m_str1 = "";
	m_str2 = "";
	m_mode = M_REPLACE_TEXT;
}  

DlgReplaceText::~DlgReplaceText()
{
}

void DlgReplaceText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STR1, m_edit1);
	DDX_Control(pDX, IDC_STR2, m_edit2);
	if( pDX->m_bSaveAndValidate )
	{
		m_edit1.GetWindowText(m_str1);
		m_edit2.GetWindowText(m_str2);
		if( m_str1.GetLength() == 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "Empty string?":"Пустая строка?");
			pDX->Fail();
		}
		if ( m_str1.FindOneOf( "\"" ) >= 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"The string must not contain a special character!":
				"Строка не должна содержать специальные символы!");
			pDX->Fail();
		}
		if( m_mode == M_REPLACE_TEXT )
		{

		}
	}
	else
	{
		m_edit1.SetWindowText(m_str1);
		m_edit2.SetWindowText(m_str2);
	}
}


BEGIN_MESSAGE_MAP(DlgReplaceText, CDialog)
END_MESSAGE_MAP()


// CDlgAddWidth message handlers
