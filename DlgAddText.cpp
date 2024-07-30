// DlgAddText.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAddText.h"
#include "layers.h"

BOOL gUseDefaultWidth = TRUE;
CString TEXT_DIS = "Disabled";
CString TEXT_SEV = "Several";

// CDlgAddText dialog

IMPLEMENT_DYNAMIC(CDlgAddText, CDialog)
CDlgAddText::CDlgAddText(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddText::IDD, pParent)
{
	m_x = 0;
	m_y = 0;
	m_text_sizes = NULL;
	m_text_arr = NULL;
}

CDlgAddText::~CDlgAddText()
{
}

void CDlgAddText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edit_height);
	DDX_Control(pDX, IDC_SET_WIDTH, m_button_set_width);
	DDX_Control(pDX, IDC_DEF_WIDTH, m_button_def_width);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edit_width);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_text);
	DDX_Control(pDX, IDC_Y, m_edit_y);
	DDX_Control(pDX, IDC_X, m_edit_x);
	DDX_Text( pDX, IDC_EDIT_TEXT, m_str );
	DDX_Text( pDX, IDC_EDIT_TEXT2, m_special );
	DDX_Control(pDX, IDC_RADIO1, m_button_drag);
	DDX_Control(pDX, IDC_RADIO2, m_button_set_position );
	DDX_Control(pDX, IDC_COMBO_ADD_TEXT_UNITS, m_combo_units);
	DDX_Control(pDX, IDC_TEXT_FONT, m_combo_font);
	DDX_Control(pDX, IDC_CHECK_RT, m_check_pdf_rt_align);
	DDX_Control(pDX, IDC_CHECK_COMPRESSED, m_inter_letter_compress);
	DDX_Control(pDX, IDC_CHECK_UNCLENCHED, m_inter_letter_unclench);
	DDX_Control(pDX, IDC_CHECK_ITALIC, m_italic);
	DDX_Control(pDX, IDC_PDF_DEF, m_def );
	DDX_Control(pDX, IDC_PDF_USE_FONT, m_use_f );
	DDX_Control(pDX, IDC_PDF_GRAPHIC, m_graphic );
	if( pDX->m_bSaveAndValidate )
	{
		if( m_str.Compare( TEXT_DIS ) == 0 || m_str.Compare( TEXT_SEV ) == 0 )
			m_str = "";
		else if( m_str.GetLength() == 0 )
			m_str = m_old_str;
		//
		m_str = m_str.Trim();
		BOOL itsRef = FALSE;
		if( m_text_ptr )
			if( m_text_ptr->m_tl == m_pl->GetCurrentAttrPtr()->m_Reflist )
				itsRef = TRUE;
		if( itsRef && m_str.GetLength() )
		{
			if( m_str.FindOneOf( ILLEGAL_REF ) != -1 )
			{
				CString mess;
				mess.Format(G_LANGUAGE == 0 ? "Illegal reference designator \"%s\"":"Недопустимое обозначение детали \"%s\"", m_str);
				AfxMessageBox( mess );
				pDX->Fail();
			} 
			if( m_old_str.Compare( m_str ) )
			{
				CText * f = NULL;
				CText * oldf = NULL;
				int pg = -1;
				if( m_pl->IsThePageIncludedInNetlist( m_pl->GetActiveNumber() ) )
				{
					CString f_str = m_str;
					int dot = m_str.Find(".");
					if( dot <= 0 )
						dot = m_str.Find("-");
					if( dot > 0 )
						f_str = m_str.Left(dot) + "&";// check for multi-parts
					pg = m_pl->FindPart( &f_str, &f, TRUE, TRUE );
					if( m_text_ptr )
						if( m_text_ptr == f )
							pg = m_pl->FindPart( &f_str, &f, FALSE, TRUE );
				}
				else
				{
					int it2 = -1;
					f = m_pl->GetCurrentAttrPtr()->m_Reflist->GetText( &m_str, &it2 );
					if( m_text_ptr )
						if( m_text_ptr == f )
							f = m_pl->GetCurrentAttrPtr()->m_Reflist->GetText( &m_str, &it2 );
				}
				if( pg != -1 )
				{
					CString s,nm;
					m_pl->GetPageName( pg, &nm );
					s.Format(G_LANGUAGE == 0 ? 
						"Part present on page %d - %s.\n\n(Edit reference designators through the polyline attributes dialog window to automatically find a free part number)":
						"Деталь присутствует на странице %d - %s.\n\n(Измените позиционные обозначения через диалоговое окно атрибутов полилинии, чтобы автоматически определить свободный номер для детали)", pg + 1, nm);
					AfxMessageBox( s, MB_ICONWARNING );
					pDX->Fail();
				}
				else if( f )
				{
					AfxMessageBox(G_LANGUAGE == 0 ? 
						"Part already exists on this page.\n\n(Edit reference designators through the polyline attributes dialog window to automatically find a free part number)":
						"Деталь уже существует на этой странице.\n\n(Измените позиционные обозначения через диалоговое окно атрибутов полилинии, чтобы автоматически определить свободный номер для детали)", MB_ICONWARNING);
					pDX->Fail();
				}
			}
		}
		else 
		{
			if( getbit( m_mask_att, index_value_attr ) || getbit( m_mask_att, index_foot_attr ) )
			{
				if( m_str.FindOneOf( "\"@" ) != -1 )
				{
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Illegal string \"%s\"":"Недопустимая строка \"%s\"", m_str);
					AfxMessageBox( mess );
					pDX->Fail();
				} 
				if( m_str.FindOneOf( "|'" ) != -1 && getbit( m_mask_att, index_foot_attr ) )
				{
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Illegal footprint name \"%s\"":"Недопустимое имя футпринта \"%s\"", m_str);
					AfxMessageBox( mess );
					pDX->Fail();
				} 
			}
			if( getbit( m_mask_att, index_desc_attr ) )
			{
				if( m_str.FindOneOf( "\"" ) != -1 )
				{
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Illegal description text \"%s\"":"Недопустимый текст описания \"%s\"", m_str);
					AfxMessageBox( mess );
					pDX->Fail();
				} 
			}
			if( getbit( m_mask_att, index_net_attr ) )
			{
				if( m_str.FindOneOf( ILLEGAL_NET ) != -1 )
				{
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Illegal net name \"%s\"":"Недопустимое имя эл.цепи \"%s\"", m_str);
					AfxMessageBox( mess );
					pDX->Fail();
				} 
			}
			if( getbit( m_mask_att, index_pin_attr ) )
			{
				if( m_str.FindOneOf( ILLEGAL_PIN ) != -1 )
				{
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Illegal pin name \"%s\"":"Недопустимое имя пина \"%s\"", m_str);
					AfxMessageBox( mess );
					pDX->Fail();
				} 
			}
		}
		// leaving the dialog
		m_str = m_str.Trim();
		if( m_str.Find( '\"' ) != -1 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "Text string can't contain \"":"Текстовая строка не может содержать \"");
			pDX->Fail();
		}
		GetFields();
	}
	else
	{
		if( m_str.Compare( TEXT_DIS ) == 0 || m_str.Compare( TEXT_SEV ) == 0 || m_str.GetLength() == 0 )
		{
			if( m_str.GetLength() )
				m_text.EnableWindow(0);
			if( m_str.Compare( TEXT_SEV ) == 0 || m_str.GetLength() == 0 )
			{
				m_edit_x.EnableWindow(0);
				m_edit_y.EnableWindow(0);
				m_height = 1;
				m_width = 1;
			}
		}
		if( m_text_sizes )
		{
			CArray<CString> arr;
			CString key;
			int np = ParseKeyString( m_text_sizes, &key, &arr );
			for(int i=0; i<np-1; i++)
			{
				CString s = arr.GetAt(i);
				s.Replace("/"," / ");
				m_edit_height.AddString( s );
			}
		}
		if( m_text_arr )
		{
			for(int i=0; i<m_text_arr->GetSize(); i++)
			{
				m_text.AddString( m_text_arr->GetAt(i) );		
			}
		}
		m_text.SetFocus();
	}
}


BEGIN_MESSAGE_MAP(CDlgAddText, CDialog)
	ON_BN_CLICKED(IDC_SET_WIDTH, OnBnClickedSetWidth)
	ON_BN_CLICKED(IDC_DEF_WIDTH, OnBnClickedDefWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnEnChangeEditHeight)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedSetPosition)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedDrag)
	ON_CBN_SELCHANGE(IDC_COMBO_ADD_TEXT_UNITS, OnCbnSelchangeComboAddTextUnits)
	ON_BN_CLICKED(IDC_CHECK_COMPRESSED, &CDlgAddText::OnBnClickedCheckCompressed)
	ON_BN_CLICKED(IDC_CHECK_UNCLENCHED, &CDlgAddText::OnBnClickedCheckUnclenched)
	ON_CBN_SETFOCUS(IDC_EDIT_TEXT, &CDlgAddText::OnEnSetfocusEditText)
	ON_CBN_SELCHANGE(IDC_EDIT_HEIGHT, &CDlgAddText::OnCbnSelchangeEditHeight)
//	ON_CBN_SELENDOK(IDC_EDIT_HEIGHT, &CDlgAddText::OnCbnSelendokEditHeight)
END_MESSAGE_MAP()

// Initialize dialog
//
void CDlgAddText::Initialize(	CPageList * pl, 
								CString * str, 
								CString * special_chars,
								int units, 
								int height, 
								int width, 
								int x, 
								int y, 
								int font_n, 
								int pdf_opt,
								int mask_att,
								double scale )

{
	m_pl = pl;
	m_font_number = font_n;
	m_pdf_options = max( pdf_opt, 1 );
	if( getbit( m_pdf_options, PDF_DEF ) == 0 )
		if( getbit( m_pdf_options, PDF_GRAPHIC ) == 0 )
			if( getbit( m_pdf_options, PDF_USE_FONT ) == 0 )
				setbit( m_pdf_options, PDF_DEF );
	m_bDrag = 0;
	m_units = units;
	if( str )
		m_str = *str;
	else
		m_str = "";
	m_old_str = m_str.Trim();
	m_special = *special_chars;
	m_bUseDefaultWidth = FALSE;
	m_height = height;
	m_width = width;
	m_x = x;
	m_y = y;
	m_mask_att = mask_att;
	m_text_ptr = NULL;
	if( m_pl && m_str.GetLength() )
	{
		if( m_text_ptr = m_pl->GetText( &m_str ) )
		{
			if( m_text_ptr->m_tl != m_pl->GetCurrentAttrPtr()->m_Reflist &&
				m_text_ptr->m_tl != m_pl->GetCurrentAttrPtr()->m_Pinlist &&
				m_text_ptr->m_tl != m_pl->GetCurrentAttrPtr()->m_pDesclist )
				m_text_ptr = NULL;
		}
	}
	m_remain = 0;
	m_gl_scale = scale;
}

// CDlgAddText message handlers

BOOL CDlgAddText::OnInitDialog()
{
	CDialog::OnInitDialog();

	// units
	m_combo_units.InsertString(	0, "MIL" );
	m_combo_units.InsertString(	1, "MM"	);
	if(	m_units	== MIL )
	{
		m_combo_units.SetCurSel(0);
		m_unit_mult = NM_PER_MIL;
	}
	else
	{
		m_combo_units.SetCurSel(1);
		m_unit_mult = 1000000;
	}
	m_combo_font.InsertString( 0, "GOTHIC" );
	m_combo_font.InsertString( 0, "FANCY" );
	m_combo_font.InsertString( 0, "ITALLIC_TRIPLEX" );
	m_combo_font.InsertString( 0, "ITALLIC_DUPLEX" );
	m_combo_font.InsertString( 0, "SCRIPT_DUPLEX" );
	m_combo_font.InsertString( 0, "SCRIPT_SIMPLEX" );
	m_combo_font.InsertString( 0, "MODERN" );
	m_combo_font.InsertString( 0, "TRIPLEX" );
	m_combo_font.InsertString( 0, "DUPLEX" );
	m_combo_font.InsertString( 0, "SIMPLEX" );
	m_combo_font.InsertString( 0, "SMALL_DUPLEX" );
	m_combo_font.InsertString( 0, "SMALL_SIMPLEX" );
	SetFields();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgAddText::OnBnClickedSetWidth()
{
	GetFields();
	SetFields();
}

void CDlgAddText::OnBnClickedDefWidth()
{
	GetFields();
	SetFields();
}

void CDlgAddText::OnEnChangeEditHeight()
{
	if( m_button_def_width.GetCheck() )
	{
		CString str;
		m_edit_height.GetWindowText( str );
		m_height = (double)atof(str)*(double)m_unit_mult/m_gl_scale;
		m_width = m_height/10;
		///m_width = max( m_width, 1*NM_PER_MIL );
		if( m_units == MIL )
		{
			MakeCStringFromDouble( &str, (double)m_width/(double)NM_PER_MIL );
			m_edit_width.SetWindowText( str );
		}
		else
		{
			MakeCStringFromDouble( &str, (double)m_width/(double)NM_PER_MM );
			m_edit_width.SetWindowText( str );
		}
	}
}

void CDlgAddText::OnBnClickedOk()
{
	GetFields();
	OnOK();
}

void CDlgAddText::OnBnClickedSetPosition()
{
	GetFields();
	SetFields();
}

void CDlgAddText::OnBnClickedDrag()
{
	GetFields();
	SetFields();
}

void CDlgAddText::OnCbnSelchangeComboAddTextUnits()
{
	GetFields();
	if( m_combo_units.GetCurSel() == 0 )
	{
		m_units = MIL;
		m_unit_mult = NM_PER_MIL;
	}
	else
	{
		m_units = MM;
		m_unit_mult = 1000000;
	}
	SetFields();
}

void CDlgAddText::GetFields()
{
	CString str;
	double mult;
	if( m_units == MIL )
		mult = NM_PER_MIL;
	else
		mult = 1000000.0;
	//
	if( m_def.GetCheck() )
		setbit( m_pdf_options, PDF_DEF );
	else
		clrbit( m_pdf_options, PDF_DEF );
	if( m_graphic.GetCheck() )
		setbit( m_pdf_options, PDF_GRAPHIC );
	else
		clrbit( m_pdf_options, PDF_GRAPHIC );
	if( m_use_f.GetCheck() )
		setbit( m_pdf_options, PDF_USE_FONT );
	else
		clrbit( m_pdf_options, PDF_USE_FONT );
	int ch = m_check_pdf_rt_align.GetCheck();
	if( ch != 0 && ch != 1 )
		setbit( m_pdf_options, PDF_NO_ALIGN );
	else if( ch )
	{
		clrbit( m_pdf_options, PDF_NO_ALIGN );
		setbit( m_pdf_options, PDF_ALIGN );
	}
	else
	{
		clrbit( m_pdf_options, PDF_NO_ALIGN );
		clrbit( m_pdf_options, PDF_ALIGN );
	}
	//
	m_remain = FALSE;
	ch = m_inter_letter_compress.GetCheck();
	if( ch == 1 )
		setbit( m_pdf_options, COMPRESSED );
	else if( ch == 0 )
		clrbit( m_pdf_options, COMPRESSED );
	else
		m_remain = TRUE;
	ch = m_inter_letter_unclench.GetCheck();
	if( ch == 1 )
		setbit( m_pdf_options, UNCLENCHED );
	else if( ch == 0 )
		clrbit( m_pdf_options, UNCLENCHED );
	else
		m_remain = TRUE;
	m_remain_italic = FALSE;
	ch = m_italic.GetCheck();
	if( ch == 1 )
		setbit( m_pdf_options, ITALIC );
	else if( ch == 0 )
		clrbit( m_pdf_options, ITALIC );
	else
		m_remain_italic = TRUE;
	//
	m_font_number = m_combo_font.GetCurSel();
	//
	m_edit_height.GetWindowText( str );
	if( str.Compare("*") == 0 )
		m_height = 1;
	else
		m_height = (double)atof( str ) * mult/m_gl_scale;
	if( m_height > 1 )
		m_height += 1;
	m_edit_width.GetWindowText( str );
	if( str.Compare("*") == 0 )
		m_width = 1;
	else
		m_width = (double)atof( str ) * mult/m_gl_scale;
	if( m_width > 1 )
		m_width += 1;
	m_edit_x.GetWindowText( str );
	m_x = atof( str ) * mult/m_gl_scale;
	m_edit_y.GetWindowText( str );
	m_y = atof( str ) * mult/m_gl_scale;
	m_bUseDefaultWidth = m_button_def_width.GetCheck();
	m_bDrag = m_button_drag.GetCheck();
}

void CDlgAddText::SetFields()
{
	CString str;
	double mult;
	if( m_units == MIL )
		mult = NM_PER_MIL;
	else
		mult = 1000000.0;
	mult -= 0.00001;
	m_def.SetCheck( getbit( m_pdf_options, PDF_DEF ));
	m_graphic.SetCheck( getbit( m_pdf_options, PDF_GRAPHIC ));
	m_use_f.SetCheck( getbit( m_pdf_options, PDF_USE_FONT )); 
	//
	if( getbit( m_pdf_options, PDF_NO_ALIGN ) )
		m_check_pdf_rt_align.SetCheck(2);
	else
		m_check_pdf_rt_align.SetCheck( getbit( m_pdf_options, PDF_ALIGN ));
	//
	m_inter_letter_compress.SetCheck(2);
	m_inter_letter_unclench.SetCheck(2);
	if( getbit( m_pdf_options, COMPRESSED ) )
		m_inter_letter_unclench.SetCheck(0);
	else if( getbit( m_pdf_options, UNCLENCHED ) )
		m_inter_letter_compress.SetCheck(0);
	m_italic.SetCheck(2);
	//
	m_combo_font.SetCurSel( m_font_number );
	//
	if( m_height == 1 )
		str = "*";
	else if( m_units == MIL )
		str.Format( "%.3d", (int)((double)m_height/(double)NM_PER_MIL*(double)m_gl_scale) );
	else
		str.Format( "%.2f", (double)m_height/(double)NM_PER_MM*(double)m_gl_scale );
		//MakeCStringFromDimension( m_gl_scale, &str, m_height, m_units, 0, 0, 0, m_units==MIL?0:2 );
	//if( str.GetLength() > 4 )
	//	str.Truncate(4);
	m_edit_height.SetWindowText( str );
	CString lstr;
	m_edit_height.GetLBText( m_edit_height.GetCount()-1, lstr );
	if( lstr.Find("/") != -1 )
		m_edit_height.InsertString( m_edit_height.GetCount(), str );
	if( m_width == 1 )
		str = "*";
	else if( m_units == MIL )
		str.Format( "%.3d", (int)((double)m_width/(double)NM_PER_MIL*(double)m_gl_scale) );
	else
		str.Format( "%.2f", (double)m_width/(double)NM_PER_MM*(double)m_gl_scale );
		//MakeCStringFromDimension( m_gl_scale, &str, m_width, m_units, 0, 0, 0, m_units==MIL?0:2 );
	//if( str.GetLength() > 4 )
	//	str.Truncate(4);
	m_edit_width.SetWindowText( str );
	MakeCStringFromDimension( m_gl_scale, &str, m_x, m_units, 0, 0, 0, m_units==MIL?1:3 );
	//if( str.GetLength() > 7 )
	//	str.Truncate(7);
	m_edit_x.SetWindowText( str );
	MakeCStringFromDimension( m_gl_scale, &str, m_y, m_units, 0, 0, 0, m_units==MIL?1:3 );
	//if( str.GetLength() > 7 )
	//	str.Truncate(7);
	m_edit_y.SetWindowText( str );
	//
	m_button_def_width.SetCheck( m_bUseDefaultWidth );
	m_button_set_width.SetCheck( !m_bUseDefaultWidth );
	m_button_def_width.SetCheck( m_bUseDefaultWidth );
	m_edit_width.EnableWindow( !m_bUseDefaultWidth );
	m_button_drag.SetCheck( m_bDrag );
	m_button_set_position.SetCheck( !m_bDrag );
	if( m_str.Compare( TEXT_SEV ) && m_str.GetLength() )
	{
		m_edit_x.EnableWindow( !m_bDrag );
		m_edit_y.EnableWindow( !m_bDrag );
	}
	else
		m_button_drag.EnableWindow( 0 );
}


void CDlgAddText::OnLbnSelchangeListLayer()
{
	GetFields();
	SetFields();
}


void CDlgAddText::OnBnClickedCheckCompressed()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( m_inter_letter_compress.GetCheck() == 2 )
		m_inter_letter_unclench.SetCheck(2);
	else //if( m_inter_letter_compress.GetCheck() )
		m_inter_letter_unclench.SetCheck(0);
}


void CDlgAddText::OnBnClickedCheckUnclenched()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( m_inter_letter_unclench.GetCheck() == 2 )
		m_inter_letter_compress.SetCheck(2);
	else //if( m_inter_letter_unclench.GetCheck() )
		m_inter_letter_compress.SetCheck(0);
}


void CDlgAddText::OnEnSetfocusEditText()
{
	int io1 = m_str.Find("|");
	int io2 = m_str.Find("'");
	if( io1 > 0 || io2 > 0 )
	{
		if( io1 < io2 && io1 > 0 )
			m_text.SetEditSel(0,io1);
		else if( io2 > 0 )
			m_text.SetEditSel(0,io2);
		else
			m_text.SetEditSel(0,io1);
	}
	else if( m_text_ptr )
	{
		CString pref, suff;
		int num = ParseRef( &m_str, &pref, &suff );
		if( num > 0 && suff.GetLength() == 0 )
			m_text.SetEditSel( pref.GetLength(), m_str.GetLength()-suff.GetLength() );
		else
			m_text.SetEditSel( 0, m_str.GetLength() );
	}
	else
		m_text.SetEditSel( 0, m_str.GetLength() );
}


void CDlgAddText::OnCbnSelchangeEditHeight()
{
	// TODO: добавьте свой код обработчика уведомлений
	int cur = m_edit_height.GetCurSel();
	if( cur < 0 )
		return;
	CString str;
	m_edit_height.GetLBText( cur, str );
	int io = str.Find("/");
	if( io > 0 )
	{
		CString left = str.Left(io).Trim();
		CString right = str.Right( str.GetLength()-io-1 ).Trim();
		m_edit_height.GetLBText( m_edit_height.GetCount()-1, str );
		if( str.Find("/") == -1 )
			m_edit_height.DeleteString( m_edit_height.GetCount()-1 );
		m_edit_height.InsertString( m_edit_height.GetCount(), left );
		m_edit_height.SetCurSel( m_edit_height.GetCount()-1 );
		m_edit_width.SetWindowTextA( right );
	}
}


//void CDlgAddText::OnCbnSelendokEditHeight()
//{
//	// TODO: добавьте свой код обработчика уведомлений
//	
//}
