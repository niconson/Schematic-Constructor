// DlgProtection.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgProtection.h"


// DlgProtection dialog

IMPLEMENT_DYNAMIC(CDlgProtection, CDialog)
CDlgProtection::CDlgProtection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProtection::IDD, pParent)
{
}

CDlgProtection::~CDlgProtection()
{
}



void CDlgProtection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_1, mb_en);
	DDX_Control(pDX, IDC_RADIO_2, mb_dis);
	DDX_Control(pDX, IDC_RADIO_3, mb_dis_all);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		mb_en.SetCheck(0);
		mb_dis.SetCheck(0);
		mb_dis_all.SetCheck(0);
		//
		BOOL PROTECT_ALL = TRUE;
		for( int ipcb=0; ipcb<m_doc->Pages.m_pcb_names.GetSize(); ipcb++ )
			if( getbit( m_doc->m_protection, ipcb ) == 0 )
				PROTECT_ALL = 0;
		if( PROTECT_ALL )
			mb_dis_all.SetCheck(1);
		else 
		{
			int ipcb = m_doc->Pages.GetPcbIndex( m_doc->Pages.GetActiveNumber() );
			if( ipcb >= 0 )
			{
				if( getbit( m_doc->m_protection, ipcb ) )
					mb_dis.SetCheck(1);
				else
					mb_en.SetCheck(1);
			}
			else
				mb_en.SetCheck(1);
		}
	}
	else
	{
		// outgoing
		if( mb_en.GetCheck() )
			m_doc->Pages.Protection( UNPROTECT );
		if( mb_dis.GetCheck() )
			m_doc->Pages.Protection();
		if( mb_dis_all.GetCheck() )
			for(int i=0; i<m_doc->Pages.m_pcb_names.GetSize(); i++ )
				m_doc->Pages.Protection(i);
	}
}

BEGIN_MESSAGE_MAP(CDlgProtection, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgProtection::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgProtection::OnBnClickedCancel)
END_MESSAGE_MAP()


void CDlgProtection::Initialize( CFreePcbDoc * doc )
{
	m_doc = doc;
}



void CDlgProtection::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialog::OnOK();
}





void CDlgProtection::OnBnClickedCancel()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialog::OnOK();
}


BOOL CDlgProtection::FindArchive( int ipcb )
{
	CFileFind finder;
	if( _chdir( m_doc->m_path_to_folder ) == 0 )
	{
		//CString EXT[10] = { "*.zip","*.7z","*.rar","*.tar","*.gz","*.nop","*.nop","*.nop","*.nop","*.nop" };
		CString EXT = "*.*";
		BOOL bWorking = 0;
		//for ( int i=0; (bWorking==0&&i<10); i++ )
		{
			//bWorking = finder.FindFile( EXT[i] );
			bWorking = finder.FindFile( EXT );
			while( bWorking )
			{
				bWorking = finder.FindNextFile();
				if( !finder.IsDirectory() && !finder.IsDots() )
				{
					CString fn = finder.GetFileName();
					CString name = m_doc->Pages.GetCurrentPCBName( ipcb );
					if( name.Right( 4 ) == ".fpc" )
						name.Truncate( name.GetLength()-4 );
					if( fn.Find( "(" + name + ")" ) > 0 )
						return 1; // ok
				}
			}
		}
	}
	return 0;
}
