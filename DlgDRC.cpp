// DlgDRC.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgDRC.h"

// DlgDRC dialog

IMPLEMENT_DYNAMIC(DlgDRC, CDialog)
DlgDRC::DlgDRC(CWnd* pParent /*=NULL*/)
	: CDialog(DlgDRC::IDD, pParent)
{
}

DlgDRC::~DlgDRC()
{
}

void DlgDRC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo_units);
	DDX_Control(pDX, IDC_PART_AA, m_edit_part_aa);
	DDX_Control(pDX, IDC_PART_AP, m_edit_part_ap);
	DDX_Control(pDX, IDC_PIN_AA, m_edit_pin_aa);
	DDX_Control(pDX, IDC_PIN_AP, m_edit_pin_ap);
	DDX_Control(pDX, IDC_CHECK_SHOW_UNROUTED, m_check_show_unrouted);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		m_combo_units.InsertString( 0, "MIL" );
		m_combo_units.InsertString( 1, "MM" );
		if( m_units == MIL )
			m_combo_units.SetCurSel( 0 );
		else
			m_combo_units.SetCurSel( 1 );
		SetFields();
	}
}


BEGIN_MESSAGE_MAP(DlgDRC, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnChangeUnits)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedCheck)
END_MESSAGE_MAP()


// Initialize
//
void DlgDRC::Initialize( int units, 
						 int part_aa,
						 int part_ap,
						 int pin_aa,
						 int pin_ap )
{
	m_repeat_drc = 0;
	m_units = units; 
	m_part_aa = part_aa;
	m_part_ap = part_ap;
	m_pin_aa = pin_aa;
	m_pin_ap = pin_ap;
}

void DlgDRC::GetFields()
{
	CString str;
	if( m_units == MIL )
	{
		m_edit_part_aa.GetWindowText( str );
		m_part_aa = atof( str ) * NM_PER_MIL;
		m_edit_part_ap.GetWindowText( str );
		m_part_ap = atof( str ) * NM_PER_MIL;
		m_edit_pin_aa.GetWindowText( str );
		m_pin_aa = atof( str ) * NM_PER_MIL;
		m_edit_pin_ap.GetWindowText( str );
		m_pin_ap = atof( str ) * NM_PER_MIL;
	}
	else
	{
		m_edit_part_aa.GetWindowText( str );
		m_part_aa = atof( str ) * NM_PER_MM;
		m_edit_part_ap.GetWindowText( str );
		m_part_ap = atof( str ) * NM_PER_MM;
		m_edit_pin_aa.GetWindowText( str );
		m_pin_aa = atof( str ) * NM_PER_MM;
		m_edit_pin_ap.GetWindowText( str );
		m_pin_ap = atof( str ) * NM_PER_MM;
	}
}

void DlgDRC::SetFields()
{
	CString str;
	if( m_units == MIL )
	{
		MakeCStringFromDouble( &str, m_part_aa/(double)NM_PER_MIL );
		m_edit_part_aa.SetWindowText( str );
		MakeCStringFromDouble( &str, m_part_ap/(double)NM_PER_MIL );
		m_edit_part_ap.SetWindowText( str );
		MakeCStringFromDouble( &str, m_pin_aa/(double)NM_PER_MIL );
		m_edit_pin_aa.SetWindowText( str );
		MakeCStringFromDouble( &str, m_pin_ap/(double)NM_PER_MIL );
		m_edit_pin_ap.SetWindowText( str );
	}
	else
	{
		MakeCStringFromDouble( &str, m_part_aa/(double)NM_PER_MM );
		m_edit_part_aa.SetWindowText( str );
		MakeCStringFromDouble( &str, m_part_ap/(double)NM_PER_MM );
		m_edit_part_ap.SetWindowText( str );
		MakeCStringFromDouble( &str, m_pin_aa/(double)NM_PER_MM );
		m_edit_pin_aa.SetWindowText( str );
		MakeCStringFromDouble( &str, m_pin_ap/(double)NM_PER_MM );
		m_edit_pin_ap.SetWindowText( str );
	}
}

// DlgDRC message handlers

void DlgDRC::OnCbnChangeUnits()
{
	CString str;
	m_combo_units.GetWindowText( str );
	GetFields();
	if( str.Compare("MIL") == 0 )
		m_units = MIL;
	else
		m_units = MM;
	SetFields();
}

void DlgDRC::OnBnClickedCancel()
{
	OnCancel();
}

void DlgDRC::OnBnClickedOk()
{
	GetFields();
	OnOK();
}

void DlgDRC::OnBnClickedCheck()
{
	m_repeat_drc = 1;
	OnBnClickedOk();
}
