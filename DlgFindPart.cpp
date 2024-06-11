// DlgFindPart.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgFindPart.h"
#include ".\dlgfindpart.h"


// CDlgFindPart dialog

IMPLEMENT_DYNAMIC(CDlgFindPart, CDialog)
CDlgFindPart::CDlgFindPart(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindPart::IDD, pParent)
{
}

CDlgFindPart::~CDlgFindPart()
{
}

void CDlgFindPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo_attr);
	DDX_Control(pDX, IDC_RADIO_STR_BEGIN, m_b[F_BEGIN]);
	DDX_Control(pDX, IDC_RADIO_STR_END, m_b[F_END]);
	DDX_Control(pDX, IDC_RADIO_STR_FIND, m_b[F_FIND]);
	DDX_Control(pDX, IDC_RADIO_WORD, m_b[F_WORD]);
	DDX_Control(pDX, IDC_RADIO_CASE_SENS, m_b[F_CASE]);
	DDX_Control(pDX, IDC_RADIO_PREFIX, m_b[F_PREFIX]);
	DDX_Control(pDX, IDC_RADIO_STR_REF, m_b[F_REF]);
	DDX_Control(pDX, IDC_RADIO_STR_VALUE, m_b[F_VALUE]);
	DDX_Control(pDX, IDC_RADIO_STR_FOOT, m_b[F_FOOT]);
	DDX_Control(pDX, IDC_RADIO_PIN_N, m_b[F_PIN]);
	DDX_Control(pDX, IDC_RADIO_NET_N, m_b[F_NET]);
	DDX_Control(pDX, IDC_RADIO_DESCR, m_b[F_DESC]);
	DDX_Control(pDX, IDC_RADIO_SEARCH_EVERYWHERE, m_b[F_OTHER]);
	DDX_Control(pDX, IDC_RADIO_SEARCH_ALL_PAGES, m_b[F_ALLPAGES]);
	DDX_Control(pDX, IDC_EDIT_RNG1, m_range1 );
	DDX_Control(pDX, IDC_EDIT_RNG2, m_range2 );
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		CString str;
		str.Format("%d", m_pin_range[0] );
		m_range1.SetWindowTextA( str );
		str.Format("%d", m_pin_range[1] );
		m_range2.SetWindowTextA( str );
		if( sel_attr_str )
		{
			if( sel_attr_str->GetSize() )
			{
				for( int i=sel_attr_str->GetSize()-1; i>=0; i-- )
					m_combo_attr.AddString( sel_attr_str->GetAt(i) );
				m_combo_attr.SetWindowText( sel_attr_str->GetAt(sel_attr_str->GetSize()-1) );
			}
		}
		if( m_wt )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, *m_wt );
		}
		for( int f=0; f<F_USE_LIST; f++ )
		{
			if( getbit( m_search_flags, f ) )
				m_b[f].SetCheck(1);
			else
				m_b[f].SetCheck(0);
			if( getbit( m_search_mask, f ) )
				m_b[f].EnableWindow(1);
			else
				m_b[f].EnableWindow(0);
		}
		OnCheck();
	}
	else
	{
		// outgoing
		m_combo_attr.GetWindowText( m_str );
		m_search_flags = 0;
		for( int f=0; f<F_USE_LIST; f++ )
		{
			if( m_b[f].GetCheck() )
				setbit( m_search_flags, f );
		}
		CString str;
		m_range1.GetWindowTextA( str );
		m_pin_range[0] = my_atoi(&str);
		m_range2.GetWindowTextA( str );
		m_pin_range[1] = my_atoi(&str);
	}
}


BEGIN_MESSAGE_MAP(CDlgFindPart, CDialog)
	ON_BN_CLICKED( IDC_RADIO_SEARCH_EVERYWHERE, OnCheck )
	ON_BN_CLICKED( IDSETDEF, OnPinDef )
END_MESSAGE_MAP()


// CDlgFindPart message handlers

void CDlgFindPart::Initialize( CArray<CString> * str, int flags, int mask, int pin_min, int pin_max, CString * caption )
{
	sel_attr_str = str;
	m_search_flags = flags;
	m_search_mask = mask;
	m_wt = caption;
	m_pin_range[0] = pin_min;
	m_pin_range[1] = pin_max;
}

void CDlgFindPart::OnCheck()
{
	if( m_b[F_OTHER].GetCheck() )
	{
		m_b[F_ALLPAGES].SetCheck(1);
		m_b[F_ALLPAGES].EnableWindow(0);
	}
	else
		m_b[F_ALLPAGES].EnableWindow(1);
}

void CDlgFindPart::OnPinDef()
{
	m_range1.SetWindowTextA("-1");
	m_range2.SetWindowTextA("-1");
}