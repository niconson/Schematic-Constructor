// DlgAddMaskCutout.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAddMaskCutout.h"


// CDlgAddMaskCutout dialog

IMPLEMENT_DYNAMIC(CDlgAddMaskCutout, CDialog)
CDlgAddMaskCutout::CDlgAddMaskCutout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddMaskCutout::IDD, pParent)
{
	m_width = 10*NM_PER_MIL;	// default
	m_width_min = 0;			// default
	m_width_max = 0;		// default
	m_range_flag = 0;
	bNEW = TRUE;
}

CDlgAddMaskCutout::~CDlgAddMaskCutout()
{
}

void CDlgAddMaskCutout::Initialize( int l, int h, int num_layers, int units, int polyline_w, int bCl )
{
	m_layer = l;
	m_hatch = h;
	m_num_layers = num_layers;
	m_units = units;
	m_width = max( NM_PER_MIL, polyline_w );
	m_closed_flag = bCl;
	if( bCl == 2 )
	{
		bNEW = 0;
		m_range_flag = 1;
	}
}
void CDlgAddMaskCutout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//
	DDX_Control(pDX, IDC_RADIO_OPEN, m_radio_open);
	DDX_Control(pDX, IDC_RADIO_CLOSED, m_radio_closed);
	DDX_Control(pDX, IDC_COMBO_ADD_POLY_UNITS, m_combo_units);
	DDX_Control( pDX, IDC_EDIT_WIDTH, m_edit_width );
	DDX_Control( pDX, IDC_WIDTH_MIN, m_edit_width_min );
	DDX_Control( pDX, IDC_WIDTH_MAX, m_edit_width_max );
	DDX_Control( pDX, IDC_APPLY_IN_RANGE, m_range );
	//
	DDX_Control(pDX, IDC_NOT_CHANGE1, m_radio_type_off);
	DDX_Control(pDX, IDC_NOT_CHANGE2, m_radio_hatch_off);
	DDX_Control(pDX, IDC_NOT_CHANGE3, m_layer_off);
	//
	DDX_Control(pDX, IDC_COMBO1, m_combo_layer);
	DDX_Control(pDX, IDC_COMBO3, m_combo_tone);
	DDX_Control(pDX, IDC_RADIO1, m_radio_none);
	DDX_Control(pDX, IDC_RADIO2, m_radio_edge);
	DDX_Control(pDX, IDC_RADIO3, m_radio_full);
	DDX_Control(pDX, IDC_DOTTED, m_radio_dotted);
	DDX_Control(pDX, IDC_DEF_LAYER, m_def_layer);
	DDX_Control(pDX, IDC_NOT_CONT, m_monochrom);
	//
	DDX_Check(pDX, IDC_NOT_CONT, m_no_contours);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		for(int i=0; i<m_num_layers; i++)
			m_combo_layer.InsertString( i, layer_str[i+LAY_ADD_1] );
		if( m_layer < LAY_ADD_1 )
			m_combo_layer.InsertString( m_num_layers, layer_str[m_layer] );
		m_combo_layer.SetCurSel(m_combo_layer.GetCount()-1);
		// layer
		if( m_layer >= LAY_ADD_1 )
			if( m_layer-LAY_ADD_1 < m_num_layers )
				m_combo_layer.SetCurSel(m_layer-LAY_ADD_1);
		m_radio_none.SetCheck(0);
		m_radio_edge.SetCheck(0);
		m_radio_full.SetCheck(0);
		m_radio_dotted.SetCheck(0);
		m_combo_tone.EnableWindow(0);
		while( m_combo_tone.GetCount() )
			m_combo_tone.DeleteString(0);
		m_combo_tone.AddString("5%");
		m_combo_tone.AddString("10%");
		m_combo_tone.AddString("15%");
		m_combo_tone.AddString("20%");
		m_combo_tone.AddString("25%");
		m_combo_tone.AddString("30%");
		m_combo_tone.AddString("40%");
		m_combo_tone.AddString("50%");
		m_combo_tone.AddString("60%");
		m_combo_tone.AddString("70%");
		m_combo_tone.SetCurSel(8);
		//
		if (m_hatch > 5 && (m_hatch % 5))
		{
			m_hatch -= (m_hatch % 5);
			m_monochrom.SetCheck(1);
		}
		if( m_hatch == 5 )
			m_combo_tone.SetCurSel(0);
		else if( m_hatch == 10 )
			m_combo_tone.SetCurSel(1);
		else if( m_hatch == 15 )
			m_combo_tone.SetCurSel(2);
		else if( m_hatch == 20 )
			m_combo_tone.SetCurSel(3);
		else if( m_hatch == 25 )
			m_combo_tone.SetCurSel(4);
		else if( m_hatch == 30 )
			m_combo_tone.SetCurSel(5);
		else if( m_hatch == 40 )
			m_combo_tone.SetCurSel(6);
		else if( m_hatch == 50 )
			m_combo_tone.SetCurSel(7);
		else if (m_hatch == 60)
			m_combo_tone.SetCurSel(8);
		else if( m_hatch == 70 )
			m_combo_tone.SetCurSel(9);
		//
		if( m_closed_flag == 2 )
			m_radio_hatch_off.SetCheck(1);
		else if( m_hatch == CPolyLine::NO_HATCH )
			m_radio_none.SetCheck(1);
		else if( m_hatch >= CPolyLine::HALF_TONE )
		{
			m_radio_edge.SetCheck(1);
			m_combo_tone.EnableWindow(1);
		}
		else if( m_hatch == CPolyLine::DIAGONAL_FULL )
			m_radio_full.SetCheck(1);
		else
			m_radio_dotted.SetCheck(1);
		//
		//
		m_range.SetCheck( m_range_flag );
		m_range.EnableWindow( m_range_flag );
		//
		m_combo_units.InsertString( 0, "MIL" );
		m_combo_units.InsertString( 1, "MM" );
		if( m_units == MIL )
			m_combo_units.SetCurSel( 0 );
		else
			m_combo_units.SetCurSel( 1 );
		//
		//
		m_radio_open.SetCheck(0);
		m_radio_closed.SetCheck(0);
		m_radio_type_off.SetCheck(0);
		if( m_closed_flag == 0 )
			m_radio_open.SetCheck(1);
		else if( m_closed_flag == 1 )
			m_radio_closed.SetCheck(1);
		else 
			m_radio_type_off.SetCheck(1);
		//
		//
		//
		if( bNEW )
		{
			m_range.EnableWindow(0);
			m_radio_type_off.EnableWindow(0);
			m_radio_hatch_off.EnableWindow(0);
			m_layer_off.EnableWindow(0);
			m_def_layer.EnableWindow(0);
		}
		else
		{
			m_layer_off.SetCheck(1);
		}
		SetFields();
		OnDef();
	}
	else
	{	
		// outgoing
		int cs = m_combo_layer.GetCurSel();
		if( cs >= 0 && cs < m_num_layers )
			m_layer = LAY_ADD_1 + cs;
		if( m_def_layer.GetCheck() )
			m_layer = 0;
		//
		if( m_radio_none.GetCheck() )
			m_hatch = CPolyLine::NO_HATCH;
		else if( m_radio_edge.GetCheck() )
		{
			int itone = m_combo_tone.GetCurSel();
			if( itone <= 0 )
				m_hatch = 5;
			else if( itone == 1 )
				m_hatch = 10;
			else if( itone == 2 )
				m_hatch = 15;
			else if( itone == 3 )
				m_hatch = 20;
			else if( itone == 4 )
				m_hatch = 25;
			else if( itone == 5 )
				m_hatch = 30;
			else if( itone == 6 )
				m_hatch = 40;
			else if( itone == 7 )
				m_hatch = 50;
			else if (itone == 8)
				m_hatch = 60;
			else if( itone == 9 )
				m_hatch = 70;
			if (m_no_contours)
				m_hatch++;
		}
		else if( m_radio_full.GetCheck() )
			m_hatch = CPolyLine::DIAGONAL_FULL;
		else if( m_radio_dotted.GetCheck() )
			m_hatch = CPolyLine::DOTTED;
		else
			m_hatch = CPolyLine::NO_HATCH;
		//
		//
		GetFields();
		m_closed_flag =		m_radio_closed.GetCheck();
		m_range_flag =		m_range.GetCheck();
		m_type_off_flag =	m_radio_type_off.GetCheck();
		m_hatch_off_flag =	m_radio_hatch_off.GetCheck();
		m_layer_off_flag =	m_layer_off.GetCheck();
		if( m_width < 0 || m_width > 999*NM_PER_MIL )
		{
			pDX->PrepareEditCtrl( IDC_EDIT_WIDTH );
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Width out of range (1 to 999 mils)":
				"Ўирина вне диапазона (от 1 до 999 мил)");
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgAddMaskCutout, CDialog)
	ON_BN_CLICKED( IDC_DEF_LAYER, OnDef )
	ON_BN_CLICKED( IDC_NOT_CHANGE3, OnDef )
	ON_BN_CLICKED(IDC_RADIO2, &CDlgAddMaskCutout::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_DOTTED, &CDlgAddMaskCutout::OnBnClickedDotted)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgAddMaskCutout::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgAddMaskCutout::OnBnClickedRadio1)
	ON_CBN_SELCHANGE(IDC_COMBO_ADD_POLY_UNITS, OnCbnSelchangeComboAddPolyUnits)
	ON_BN_CLICKED(IDC_APPLY_IN_RANGE, &CDlgAddMaskCutout::OnBnClickedApplyInRange)
	ON_BN_CLICKED(IDC_NOT_CHANGE3, &CDlgAddMaskCutout::OnBnClickedNotChange3)
	ON_BN_CLICKED(IDC_NOT_CHANGE2, &CDlgAddMaskCutout::OnBnClickedNotChange2)
END_MESSAGE_MAP()


void CDlgAddMaskCutout::OnDef()
{
	GetFields();
	if( m_def_layer.GetCheck() || m_layer_off.GetCheck() )
		m_combo_layer.EnableWindow(0);
	else
		m_combo_layer.EnableWindow(1);
	SetFields();
}


void CDlgAddMaskCutout::OnBnClickedRadio2()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_combo_tone.EnableWindow(1);
}


void CDlgAddMaskCutout::OnBnClickedDotted()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_combo_tone.EnableWindow(0);
}


void CDlgAddMaskCutout::OnBnClickedRadio3()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_combo_tone.EnableWindow(0);
}


void CDlgAddMaskCutout::OnBnClickedRadio1()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_combo_tone.EnableWindow(0);
}

void CDlgAddMaskCutout::SetFields()
{
	CString str;
	{
		MakeCStringFromDimension( 1.0, &str, m_width, m_units, 0, 0, 0, m_units==MIL?1:3 );
		m_edit_width.SetWindowText( str );
		MakeCStringFromDimension( 1.0, &str, m_width_min, m_units, 0, 0, 0, m_units==MIL?1:3 );
		m_edit_width_min.SetWindowText( str );
		MakeCStringFromDimension( 1.0, &str, m_width_max, m_units, 0, 0, 0, m_units==MIL?1:3 );
		m_edit_width_max.SetWindowText( str );
	}

	if( m_range.GetCheck() )
	{
		m_edit_width_min.EnableWindow(1);
		m_edit_width_max.EnableWindow(1);
	}
	else
	{
		m_edit_width_min.EnableWindow(0);
		m_edit_width_max.EnableWindow(0);
	}
	if( m_layer_off.GetCheck() )
	{
		m_def_layer.SetCheck(0);
		m_def_layer.EnableWindow(0);
	}
	else
		m_def_layer.EnableWindow(1);
	if( m_def_layer.GetCheck() )
	{
		m_layer_off.SetCheck(0);
		m_layer_off.EnableWindow(0);
	}
	else
		m_layer_off.EnableWindow(1);
	if( bNEW )
	{
		m_layer_off.EnableWindow(0);
		m_def_layer.EnableWindow(0);
	}
}
void CDlgAddMaskCutout::OnCbnSelchangeComboAddPolyUnits()
{
	GetFields();
	if( m_combo_units.GetCurSel() == 0 )
		m_units = MIL;
	else
		m_units = MM;
	SetFields();
}

void CDlgAddMaskCutout::GetFields()
{
	CString str;
	if( m_units == MIL )
	{
		m_edit_width.GetWindowText( str );
		m_width = atof( str ) * (float)NM_PER_MIL;
		m_edit_width_min.GetWindowText( str );
		m_width_min = atof( str ) * (float)NM_PER_MIL;
		m_edit_width_max.GetWindowText( str );
		m_width_max = atof( str ) * (float)NM_PER_MIL;
	}
	else
	{
		m_edit_width.GetWindowText( str );
		m_width = atof( str ) * (float)NM_PER_MM;
		m_edit_width_min.GetWindowText( str );
		m_width_min = atof( str ) * (float)NM_PER_MM;
		m_edit_width_max.GetWindowText( str );
		m_width_max = atof( str ) * (float)NM_PER_MM;
	}
}

void CDlgAddMaskCutout::OnBnClickedApplyInRange()
{
	// TODO: добавьте свой код обработчика уведомлений
	GetFields();
	SetFields();
}


void CDlgAddMaskCutout::OnBnClickedNotChange3()
{
	// TODO: добавьте свой код обработчика уведомлений
	GetFields();
	SetFields();
}


void CDlgAddMaskCutout::OnBnClickedNotChange2()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_combo_tone.EnableWindow(0);
}
