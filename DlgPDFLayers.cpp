// DlgLayers.cpp : implementation file
//
// uses global arrays in layers.h
//
#include "stdafx.h"
#include "FreeSch.h"
#include "DlgPDFLayers.h"
#include "layers.h"
#include ".\dlgPDFlayers.h"


// CDlgPDFLayers dialog

IMPLEMENT_DYNAMIC(CDlgPDFLayers, CDialog)
CDlgPDFLayers::CDlgPDFLayers(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPDFLayers::IDD, pParent)
{
}

CDlgPDFLayers::~CDlgPDFLayers()
{
}

void CDlgPDFLayers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if( !pDX->m_bSaveAndValidate )
	{
		// on entry
		for( int i=m_nlayers; i<NUM_DLG_LAYERS; i++ )
			GetDlgItem( IDC_CHECK_LAYER_3+i-2 )->EnableWindow( 0 );
	}
	for( int i=0; i<NUM_DLG_LAYERS; i++ )
	{
		if( i>1 )
			DDX_Check( pDX, IDC_CHECK_LAYER_3+i-2, m_vis[i] );
	}
	DDX_Check( pDX, IDC_CHECK_USE_FONT, m_use_font );
	DDX_Check( pDX, IDC_CHECK_PRINT_1, m_print_1 );
	DDX_Check( pDX, IDC_CHECK_PRINT_2, m_print_2 );
	DDX_Check( pDX, IDC_CHECK_PRINT_3, m_print_3 );
	DDX_Check( pDX, IDC_CHECK_PRINT_4, m_print_4 );
	DDX_Check( pDX, IDC_CHECK_PRINT_5, m_print_5 );
	DDX_Check( pDX, IDC_CHECK_PRINT_6, m_print_6 );
	DDX_Check( pDX, IDC_CHECK_PRINT_7, m_print_7 );
	DDX_Check( pDX, IDC_CHECK_PRINT_8, m_print_8 );
	DDX_Check( pDX, IDC_APPLY_INV, m_apply_invert );
	DDX_Check( pDX, IDC_CHECK_LAYER_SYS_CLRS, m_sys_colors );
	DDX_Check( pDX, IDC_CHECK_PRINT_SEPARATE, m_print_separate);
	///DDX_Radio( pDX, IDC_CHECK_PRINT_SEPARATE, m_en_separate);
	DDX_Control( pDX, IDC_TEXT_FONTS, m_font_box );
	DDX_Control( pDX, IDC_EDIT_MARGIN, m_margin_edit );
	if( pDX->m_bSaveAndValidate )
	{
		// on OK
		if( m_print_1 == 0 &&
			m_print_2 == 0 &&
			m_print_3 == 0 &&
			m_print_4 == 0 &&
			m_print_5 == 0 &&
			m_print_6 == 0 &&
			m_print_7 == 0 &&
			m_print_8 == 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "Select print pages !":"Выберите страницы для печати!");
			pDX->Fail();
		}

		// save options
		//0
		if( m_use_font )
			setbit( m_pdf_checking, 0 );
		else
			clrbit( m_pdf_checking, 0 );
		//1
		if( m_print_1 )
			setbit( m_pdf_checking, 1 );
		else
			clrbit( m_pdf_checking, 1 );
		//2
		if( m_print_2 )
			setbit( m_pdf_checking, 2 );
		else
			clrbit( m_pdf_checking, 2 );
		//3
		if( m_print_3 )
			setbit( m_pdf_checking, 3 );
		else
			clrbit( m_pdf_checking, 3 );
		//4
		if( m_print_4 )
			setbit( m_pdf_checking, 4 );
		else
			clrbit( m_pdf_checking, 4 );
		//5
		if( m_print_5 )
			setbit( m_pdf_checking, 5 );
		else
			clrbit( m_pdf_checking, 5 );
		//6
		if( m_print_6 )
			setbit( m_pdf_checking, 6 );
		else
			clrbit( m_pdf_checking, 6 );
		//7
		if( m_print_7 )
			setbit( m_pdf_checking, 7 );
		else
			clrbit( m_pdf_checking, 7 );
		//8
		if( m_print_8 )
			setbit( m_pdf_checking, 8 );
		else
			clrbit( m_pdf_checking, 8 );
		//9
		if( m_apply_invert )
			setbit( m_pdf_checking, 9 );
		else
			clrbit( m_pdf_checking, 9 );
		//10
		if( m_sys_colors )
			setbit( m_pdf_checking, 10 );
		else
			clrbit( m_pdf_checking, 10 );
		//11
		if( m_print_separate )
			setbit( m_pdf_checking, 11 );
		else
			clrbit( m_pdf_checking, 11 );
		//
		for( int i=0; i<NUM_DLG_LAYERS; i++ )
		{
			m_rgb_ptr[i*3] = m_rgb[i][0];
			m_rgb_ptr[i*3+1] = m_rgb[i][1];
			m_rgb_ptr[i*3+2] = m_rgb[i][2];
		}
		
		CString mrg;
		m_margin_edit.GetWindowTextA( mrg );
		m_margin = my_atof(&mrg);
		i_font = m_font_box.GetCurSel();
		//
	}
	else
	{
		for( int i=0; i<MAX_PAGES; i++ )
		{
			if( i >= m_num_pages )
			{
				CWnd * box = GetDlgItem( IDC_CHECK_PRINT_1 + i );
				if( box )
					box->EnableWindow(0);
			}
		}
		for( int i=0; i<n_fonts; i++ )
			m_font_box.AddString( m_fonts[i] );
		if( i_font >= 0 && i_font < n_fonts )
			m_font_box.SetCurSel(i_font);
		else if( n_fonts > 0 )
			m_font_box.SetCurSel(0);
		CString mrg;
		::MakeCStringFromDouble( &mrg, m_margin );
		m_margin_edit.SetWindowTextA( mrg );
		GetDlgItem( IDC_CHECK_PRINT_SEPARATE )->EnableWindow( m_en_separate );
		if( m_en_separate == 0 )
		{
			CButton * B = (CButton*)GetDlgItem( IDC_CHECK_PRINT_SEPARATE );
			if( B )
				B->SetCheck(0);
		}
	}
}

BEGIN_MESSAGE_MAP(CDlgPDFLayers, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LAYER_1, OnBnClickedButtonLayer1)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_2, OnBnClickedButtonLayer2)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_3, OnBnClickedButtonLayer3)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_4, OnBnClickedButtonLayer4)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_5, OnBnClickedButtonLayer5)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_6, OnBnClickedButtonLayer6)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_7, OnBnClickedButtonLayer7)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_8, OnBnClickedButtonLayer8)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_9, OnBnClickedButtonLayer9)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_10, OnBnClickedButtonLayer10)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_11, OnBnClickedButtonLayer11)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_12, OnBnClickedButtonLayer12)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_13, OnBnClickedButtonLayer13)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_14, OnBnClickedButtonLayer14)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_15, OnBnClickedButtonLayer15)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_16, OnBnClickedButtonLayer16)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_17, OnBnClickedButtonLayer17)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_18, OnBnClickedButtonLayer18)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_19, OnBnClickedButtonLayer19)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_20, OnBnClickedButtonLayer20)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_21, OnBnClickedButtonLayer21)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_22, OnBnClickedButtonLayer22)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_23, OnBnClickedButtonLayer23)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_24, OnBnClickedButtonLayer24)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_25, OnBnClickedButtonLayer25)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_26, OnBnClickedButtonLayer26)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_27, OnBnClickedButtonLayer27)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_28, OnBnClickedButtonLayer28)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_29, OnBnClickedButtonLayer29)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_30, OnBnClickedButtonLayer30)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_31, OnBnClickedButtonLayer31)
	ON_BN_CLICKED(IDCANCEL, OnBnCancel)
	ON_CBN_SELCHANGE(IDC_TEXT_FONTS, &CDlgPDFLayers::OnCbnSelchangeTextFonts)
END_MESSAGE_MAP()

void CDlgPDFLayers::Initialize( int nlayers, 
								int vis[], 
								int rgb[][3], 
								char * Fonts[], 
								int n_f, 
								int i_f,
								float f_mrg,
								int options,
								int en_separate,
								int num_gps )
{
	m_en_separate = en_separate;
	m_pdf_checking = options;
	m_num_pages = num_gps;

	// get options
	m_use_font =		getbit(	m_pdf_checking, 0 );
	m_print_1 =		getbit(	m_pdf_checking, 1 );
	m_print_2 =		getbit(	m_pdf_checking, 2 );
	m_print_3 =		getbit(	m_pdf_checking, 3 );
	m_print_4 =		getbit(	m_pdf_checking, 4 );
	m_print_5 =		getbit(	m_pdf_checking, 5 );
	m_print_6 =		getbit(	m_pdf_checking, 6 );
	m_print_7 =		getbit(	m_pdf_checking, 7 );
	m_print_8 =		getbit(	m_pdf_checking, 8 );
	m_apply_invert =	getbit(	m_pdf_checking, 9 );
	m_sys_colors =		getbit(	m_pdf_checking,	10 );
	m_print_separate =	getbit(	m_pdf_checking, 11 );
	//
	m_fonts = Fonts;
	n_fonts = n_f;
	i_font = i_f;
	m_margin = f_mrg;
	m_nlayers = nlayers;
	m_vis = vis;
	m_rgb_ptr = (int*)rgb;
	for( int i=0; i<NUM_DLG_LAYERS; i++ )
	{
		m_rgb[i][0] = m_rgb_ptr[i*3];
		m_rgb[i][1] = m_rgb_ptr[i*3+1];
		m_rgb[i][2] = m_rgb_ptr[i*3+2];
	}
}


// CDlgPDFLayers message handlers
//

// set color boxes
//
HBRUSH CDlgPDFLayers::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  Change any attributes of the DC here
	// TODO:  Return a different brush if the default is not desired
	if ( pWnd->GetDlgCtrlID() >= IDC_STATIC_LAYER_1 && pWnd->GetDlgCtrlID() <= (IDC_STATIC_LAYER_1+NUM_DLG_LAYERS-1) )
	{
		int i = pWnd->GetDlgCtrlID() - IDC_STATIC_LAYER_1;
		// Set layer color
		pDC->SetTextColor(RGB(m_rgb[i][0], m_rgb[i][1], m_rgb[i][2]));
		pDC->SetBkMode(TRANSPARENT);
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush( RGB(m_rgb[i][0], m_rgb[i][1], m_rgb[i][2]) );
		hbr = m_brush;
	}
	return hbr;
}

// handle edit button clicks
//
void CDlgPDFLayers::OnBnClickedButtonLayer1() { EditColor( 0 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer2() { EditColor( 1 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer3() { EditColor( 2 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer4() { EditColor( 3 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer5() { EditColor( 4 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer6() { EditColor( 5 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer7() { EditColor( 6 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer8() { EditColor( 7 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer9() { EditColor( 8 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer10() { EditColor( 9 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer11() { EditColor( 10 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer12() { EditColor( 11 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer13() { EditColor( 12 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer14() { EditColor( 13 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer15() { EditColor( 14 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer16() { EditColor( 15 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer17() { EditColor( 16 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer18() { EditColor( 17 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer19() { EditColor( 18 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer20() { EditColor( 19 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer21() { EditColor( 20 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer22() { EditColor( 21 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer23() { EditColor( 22 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer24() { EditColor( 23 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer25() { EditColor( 24 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer26() { EditColor( 25 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer27() { EditColor( 26 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer28() { EditColor( 27 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer29() { EditColor( 28 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer30() { EditColor( 29 ); }
void CDlgPDFLayers::OnBnClickedButtonLayer31() { EditColor( 30 ); }

// edit color for selected layer
//
void CDlgPDFLayers::EditColor( int layer )
{
	CColorDialog dlg( RGB(m_rgb[layer][0], m_rgb[layer][1], m_rgb[layer][2]), 
		CC_FULLOPEN | CC_ANYCOLOR );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		COLORREF color = dlg.GetColor();
		m_rgb[layer][0] = GetRValue( color );
		m_rgb[layer][1] = GetGValue( color );
		m_rgb[layer][2] = GetBValue( color );
		Invalidate( FALSE );
	}
}

void CDlgPDFLayers::OnBnCancel()
{
	OnOK();
	OnCancel();
}



void CDlgPDFLayers::OnCbnSelchangeTextFonts()
{
	// TODO: добавьте свой код обработчика уведомлений
}
