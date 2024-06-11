// DlgLog.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgLog.h"
#include ".\dlglog.h"

extern CFreeasyApp theApp;

// CDlgLog dialog

IMPLEMENT_DYNAMIC(CDlgLog, CDialog)
CDlgLog::CDlgLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLog::IDD, pParent)
{
	m_running = TRUE;
	m_hidden = FALSE;
}

CDlgLog::~CDlgLog()
{
}

void CDlgLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edit_log);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		m_edit_log.LimitText( 1000000 );
	}
}


BEGIN_MESSAGE_MAP(CDlgLog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(ID_HIDE_ME, OnBnClickedHideMe)
	ON_BN_CLICKED(ID_HIDE_ME2, &CDlgLog::OnBnClickedTXT)
END_MESSAGE_MAP()

void CDlgLog::Move( int x, int y ) 
{
   CRect myRect;
   GetWindowRect(&myRect);
   myRect.OffsetRect( x, y );
   MoveWindow(myRect);
}

void CDlgLog::Clear()
{
	AddLine("\r\n\r\n\r\n\r\n");
}

void CDlgLog::RemoveAll()
{
	// clear edit control
	m_edit_log.SetSel( 0, 999999 );
	m_edit_log.ReplaceSel( "" );
}

void CDlgLog::AddLine( LPCTSTR str )
{
	int len = m_edit_log.GetWindowTextLength();
	m_edit_log.SetSel(len,len);
	m_edit_log.ReplaceSel( str );
}

void CDlgLog::AddParagraph( CString str ) 
{
	int MAX_SYM = 48;
	CString StrPtr="";
	CString StrHeader = "        " + str;
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "_";
	AddLine( "\r\n"+StrPtr+"\r\n\r\n"+StrHeader+"\r\n"+StrPtr+"\r\n\r\n");
}

void CDlgLog::AddHeader( CString str, int tab ) 
{
	int MAX_SYM = 60;
	/*int strl = str.GetLength();
	CString StrPtr="**";
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "*******\t";
	AddLine(StrPtr+"**\r\n");
	//
	StrPtr="**";
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "\t";
	AddLine(StrPtr+"**\r\n");
	//
	StrPtr="**";
	for( int ih=0; ih<MAX_SYM*4-strl/2-2; ih++ )
		StrPtr += "\t";
	StrPtr += str;
	for( int ih=0; ih<MAX_SYM*4-strl/2-2; ih++ )
		StrPtr += "\t";
	//if( StrPtr.GetLength()%2 )
	//	StrPtr += " ";
	StrPtr += "**\r\n";
	AddLine(StrPtr);
	//
	StrPtr="**";
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "\t";
	AddLine("**"+StrPtr+"**\r\n");
	//
	StrPtr="**";
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "*";
	AddLine(StrPtr+"\r\n");*/
	CString StrPtr="";
	CString StrHeader;
	for( int ih=0; ih<tab; ih++ )
		StrHeader += "\t";
	StrHeader += str;
	for( int ih=0; ih<MAX_SYM; ih++ )
		StrPtr += "=";
	AddLine( "\r\n"+StrPtr+"\r\n"+StrHeader+"\r\n"+StrPtr+"\r\n\r\n");
}

// CDlgLog message handlers

void CDlgLog::OnBnClickedOk()
{
	m_running = FALSE;
	CWnd * frm = ::AfxGetMainWnd();
	frm->BringWindowToTop();
	OnOK();
}

BOOL CDlgLog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	if( frm->m_bCursorHidden )
	{
		::ShowCursor( TRUE );
		frm->m_bCursorHidden = FALSE;
	}
//	return CDialog::OnSetCursor(pWnd, nHitTest, message);
	return TRUE;
}

void CDlgLog::OnBnClickedHideMe()
{
	this->ShowWindow( SW_HIDE );
	m_hidden = TRUE;
}

void CDlgLog::OnShowMe()
{
	if( !m_hidden )
	{
		this->ShowWindow( SW_SHOW );
		this->UpdateWindow();
		this->BringWindowToTop();
	}
}

void CDlgLog::OnBnClickedTXT()
{
	// TODO: добавьте свой код обработчика уведомлений
	CString path = theApp.m_Doc->m_path_to_folder + "\\related_files\\" + theApp.m_Doc->m_name + ".txt";
	CStdioFile File;
	int err = File.Open( LPCSTR(path), CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening file
		CString mess;
		mess.Format( "Unable to open file \"%s\"", path ); 
		AfxMessageBox( mess );
		return;
	}
	else
	{
		CString str;
		m_edit_log.GetWindowText( str );
		File.WriteString( str );
		//for( int ii=0; ii<this->getGetCount(); ii++ )
		//{
		//	CString gS = "";
		//	m_list.GetText(ii,gS);
		//	File.WriteString( gS );
		//	File.WriteString( "\n" );			
		//}
		File.Close();
		ShellExecute(	NULL,"open", path, NULL, NULL, SW_SHOWNORMAL );
	}
}
