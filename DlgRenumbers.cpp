// DlgAssignNet.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgRenumbers.h"
//#include ".\dlgassignnet.h"


// DlgAssignNet dialog

IMPLEMENT_DYNAMIC(DlgRenumbers, CDialog)
DlgRenumbers::DlgRenumbers(CWnd* pParent /*=NULL*/)
	: CDialog(DlgRenumbers::IDD, pParent)
{

}

DlgRenumbers::~DlgRenumbers()
{
}

void DlgRenumbers::Ini( CString * pp, int mode, int LtoR, int num )
{
	m_pr = pp;
	m_mode = mode;
	m_left = LtoR;
	m_start_num = num;
}

void DlgRenumbers::DoDataExchange(CDataExchange* pDX)
{	
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_PREFIX_TEXT, m_Static1 );
	DDX_Control( pDX, IDC_START_TEXT, m_Static2 );
	DDX_Control( pDX, IDC_RANGE_TEXT, m_Static3 );
	DDX_Control(pDX, IDC_COMBO_PREFIX, m_combo_prefix);
	DDX_Control(pDX, IDC_COMBO_STARTN, m_edit_start);
	DDX_Control(pDX, IDC_COMBO_RANGE, m_edit_shift);
	DDX_Control(pDX, IDC_RADIO_LR, m_b_left);
	DDX_Control(pDX, IDC_RADIO_TB, m_b_top);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		if( m_mode == RENUMB )
		{
			m_combo_prefix.AddString( " all prefixes" );
			m_combo_prefix.SetCurSel(0);
			m_Static1.SetWindowTextA( "Apply to prefix" );
			m_Static2.SetWindowTextA( "Start number" );
			m_Static3.SetWindowTextA( "Add suffix" );
			m_edit_start.SetWindowTextA( "1" );
			m_edit_shift.SetWindowTextA( "" );
			m_b_top.SetCheck( !m_left );
			m_b_left.SetCheck( m_left );
		}
		else if( m_mode == RAISE )
		{
			m_Static1.SetWindowTextA( "Apply to prefix" );
			m_Static2.SetWindowTextA( "Raise starting" );
			m_Static3.SetWindowTextA( "Raise by..." );
			m_edit_start.SetWindowTextA( "1" );
			m_edit_shift.SetWindowTextA( "1" );
			m_b_top.SetCheck( 0 );
			m_b_left.SetCheck( 0 );
			m_b_top.EnableWindow(0);
			m_b_left.EnableWindow(0);
		}
		else if( m_mode == CLEAR )
		{
			m_combo_prefix.AddString( " all prefixes" );
			m_combo_prefix.SetCurSel(0);
			m_Static1.SetWindowTextA( "Apply to prefix" );
			m_Static2.SetWindowTextA( "Start number" );
			m_Static3.SetWindowTextA( "Add suffix" );
			CString winT;
			winT.Format("%.5d", m_start_num );
			m_edit_start.SetWindowTextA( winT );
			m_edit_shift.SetWindowTextA( "" );
			m_edit_start.EnableWindow(0);
			m_edit_shift.EnableWindow(0);
			m_Static2.EnableWindow(0);
			m_Static3.EnableWindow(0);
			m_b_top.SetCheck( 0 );
			m_b_left.SetCheck( 0 );
			m_b_top.EnableWindow(0);
			m_b_left.EnableWindow(0);
		}
		CString key;
		CArray<CString> Array;
		int np = ParseKeyString( m_pr, &key, &Array );
		for( int i=0; i<np-1; i++ )
			m_combo_prefix.AddString( Array[i] );
		if( m_mode == RAISE )
			if( np >= 2 )
				m_combo_prefix.SetCurSel(0);
	}
	else
	{
		// outgoing
		m_shift = 0;
		m_edit_shift.GetWindowTextA( m_suffix_str );
		if( m_mode == RAISE )
			m_shift = my_atoi( &m_suffix_str );
		//
		CString m_start_str;
		m_edit_start.GetWindowTextA( m_start_str );
		m_start_num = my_atoi( &m_start_str );
		//
		m_combo_prefix.GetWindowTextA( m_prefix_str );
		if( m_prefix_str.Left(1) == " " )
			m_prefix_str = "";
		//
		m_left = m_b_left.GetCheck();
		//
		if( m_start_num < 0 )
		{
			AfxMessageBox( "Illegal start number" );
			pDX->Fail();
		}
	}
}

