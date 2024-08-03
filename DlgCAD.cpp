// DlgCAD.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgCAD.h"
#include "Gerber.h"
#include "DlgLog.h"
#include "DlgMyMessageBox2.h"
#include "PathDialog.h"
#include "RTcall.h"
#include ".\dlgcad.h"

// CDlgCAD dialog

IMPLEMENT_DYNAMIC(CDlgCAD, CDialog)
CDlgCAD::CDlgCAD(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCAD::IDD, pParent)
{
	m_dlg_log = NULL;
	m_folder = "";
}

CDlgCAD::~CDlgCAD()
{
}
 
void CDlgCAD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//
	DDX_Control(pDX, IDC_BMP_VIEWER, m_viewer);
	DDX_Control(pDX, IDC_RADIO_VM, m_radio_VM );
	DDX_Control(pDX, IDC_RADIO_GV, m_radio_GV );
	DDX_Control(pDX, IDC_RADIO_GL, m_radio_GL );
	DDX_Control(pDX, IDC_RADIO_PNG, m_check_render_all);
	//
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_edit_folder);
	DDX_Control(pDX, IDC_CHECK_FREE_LINE, m_check_free_line);
	DDX_Control(pDX, IDC_CHECK_PART_LINE, m_check_part_line);
	DDX_Control(pDX, IDC_CHECK_PART_NAME, m_check_part_name);
	DDX_Control(pDX, IDC_CHECK_PIN_LINE, m_check_pin_line);
	DDX_Control(pDX, IDC_CHECK_PIN_NAME, m_check_pin_name);
	DDX_Control(pDX, IDC_CHECK_NET_LINE, m_check_net_line);
	DDX_Control(pDX, IDC_CHECK_NET_NAME, m_check_net_name);
	DDX_Control(pDX, IDC_CHECK_INNER1, m_check_add1);
	DDX_Control(pDX, IDC_CHECK_INNER2, m_check_add2);
	DDX_Control(pDX, IDC_CHECK_INNER3, m_check_add3);
	DDX_Control(pDX, IDC_CHECK_INNER4, m_check_add4);
	DDX_Control(pDX, IDC_CHECK_INNER5, m_check_add5);
	DDX_Control(pDX, IDC_CHECK_INNER6, m_check_add6);
	DDX_Control(pDX, IDC_CHECK_INNER7, m_check_add7);
	DDX_Control(pDX, IDC_CHECK_INNER8, m_check_add8);
	DDX_Control(pDX, IDC_CHECK_INNER9, m_check_add9);
	DDX_Control(pDX, IDC_CHECK_INNER10, m_check_add10);
	DDX_Control(pDX, IDC_CHECK_INNER11, m_check_add11);
	DDX_Control(pDX, IDC_CHECK_INNER12, m_check_add12);
	DDX_Control(pDX, IDC_CHECK_INNER13, m_check_add13);
	DDX_Control(pDX, IDC_CHECK_INNER14, m_check_add14);
	DDX_Control(pDX, IDC_CHECK_FOOTPRINT, m_check_footprint);
	DDX_Control(pDX, IDC_CHECK_PART_VALUE, m_check_part_value);
	DDX_Control(pDX, IDC_CHECK_DESCRIPTION, m_check_description);
	DDX_Control(pDX, IDC_COMBO_CAD_UNITS, m_combo_units);
	DDX_Control(pDX, IDC_EDIT_CAD_MIN_SSW, m_edit_text_line_w);
	DDX_Control(pDX, IDC_EDIT_CAD_HIGH_W, m_edit_high_w);
	DDX_Control(pDX, IDC_EDIT_CAD_POLYLINE_WID, m_edit_polyline_w);
	DDX_Control(pDX, IDC_EDIT_CAD_TEXT_H, m_edit_text_h);
	DDX_Text( pDX, IDC_EDIT_FOLDER, m_folder );
	if( !pDX->m_bSaveAndValidate )
	{
		// entry
		m_combo_units.InsertString( 0, "MIL" );
		m_combo_units.InsertString( 1, "MM" );
		if( m_units == MIL )
			m_combo_units.SetCurSel( 0 );
		else
			m_combo_units.SetCurSel( 1 );
		SetFields();
		// enable checkboxes for valid gerber layers
		if( m_num_addit_layers < 2 )
			m_check_add2.EnableWindow( FALSE );
		if( m_num_addit_layers < 3 )
			m_check_add3.EnableWindow( FALSE );
		if( m_num_addit_layers < 4 )
			m_check_add4.EnableWindow( FALSE );
		if( m_num_addit_layers < 5 )
			m_check_add5.EnableWindow( FALSE );
		if( m_num_addit_layers < 6 )
			m_check_add6.EnableWindow( FALSE );
		if( m_num_addit_layers < 7 )
			m_check_add7.EnableWindow( FALSE );
		if( m_num_addit_layers < 8 )
			m_check_add8.EnableWindow( FALSE );
		if( m_num_addit_layers < 9 )
			m_check_add9.EnableWindow( FALSE );
		if( m_num_addit_layers < 10 )
			m_check_add10.EnableWindow( FALSE );
		if( m_num_addit_layers < 11 )
			m_check_add11.EnableWindow( FALSE );
		if( m_num_addit_layers < 12 )
			m_check_add12.EnableWindow( FALSE );
		if( m_num_addit_layers < 13 )
			m_check_add13.EnableWindow( FALSE );
		if( m_num_addit_layers < 14 )
			m_check_add14.EnableWindow( FALSE );

		// load saved settings
		SetFields();
		OnChangeViewer();
	}
	else
		GetFields();
}


BEGIN_MESSAGE_MAP(CDlgCAD, CDialog)
	ON_BN_CLICKED(ID_GO, OnBnClickedGo)
	ON_BN_CLICKED(IDC_RADIO_VM, OnChangeViewer)
	ON_BN_CLICKED(IDC_RADIO_GV, OnChangeViewer)
	ON_BN_CLICKED(IDC_RADIO_GL, OnChangeViewer)
	ON_BN_CLICKED(IDC_RADIO_PNG, OnChangeViewer)
	ON_CBN_SELCHANGE(IDC_COMBO_CAD_UNITS, OnCbnSelchangeComboCadUnits)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DEF, OnBnClickedButtonDef)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER, OnBnClickedButtonFolder)
	ON_BN_CLICKED(IDC_BUTTON_DONE, OnBnClickedButtonDone)
END_MESSAGE_MAP()

void CDlgCAD::Initialize( double version, 
						 CString * folder, 
						 CString * project_folder,
						 CString * app_folder,
						 CString * file_name,
						 int num_addit_layers, 
						 int units,
						 int min_poly_width,
						 int min_text_wid, 
						 int highlight_width, 
						 int min_text_height,
						 int flags, 
						 int layers, 
						 CArray<CPolyLine> * op,
						 CAttrib * Attr, CDisplayList * dl, StrList * ml,
						 CDlgLog * log )
{
	m_version = version;
	m_folder = *folder;
	m_project_folder = *project_folder;
	m_app_folder = *app_folder;
	m_f_name = (*file_name).Left((*file_name).GetLength()-4);
	m_f_name.MakeLower();
	m_units = units;
	m_min_polyline_width = min_poly_width;
	m_min_text_height = min_text_height;
	m_min_text_line_width = min_text_wid;
	m_highlight_width = highlight_width;
	m_num_addit_layers = num_addit_layers;
	m_flags = flags;
	m_layers = layers;
	m_op = op;
	m_Attr = Attr;
	m_dl = dl;
	m_ml = ml;
	m_dlg_log = log;
}

// CDlgCAD message handlers

void CDlgCAD::OnBnClickedGo()
{
	GetFields();
	// get CAM folder and create it if necessary
	struct _stat buf;
	m_folder.Format( "%s\\related_files\\print_layers", m_project_folder );
	m_edit_folder.SetWindowText( m_folder );
	int err = _stat( m_folder, &buf );//ok
	if( err )
	{
		err = _mkdir( m_folder );
		if( err )
		{
			CString str;
			str.Format(G_LANGUAGE == 0 ? 
				"Unable to create folder \"%s\". Try to save your project and retry again":
				"Невозможно создать папку \"%s\". Попробуйте сохранить проект и повторить попытку", m_folder);
			AfxMessageBox( str, MB_OK );
			return;
		}
	}

	// show log
	m_dlg_log->Clear();
	m_dlg_log->OnShowMe();
	//
	// delete old files
	HANDLE search_file;
    WIN32_FIND_DATA aa;
    search_file = FindFirstFile(m_folder+"\\*", &aa);
    while (FindNextFile(search_file, &aa) != NULL)
    {
		CString name = aa.cFileName;
		int pos = name.ReverseFind('.');
		if( pos > 0 )
		{
			CString name2 = name.Left(pos);
			if( name2.Compare(m_f_name) == 0 ||
				name.Right(name.GetLength()-pos) == ".gbr" ||
				name.Right(name.GetLength()-pos) == ".png" ||
				name.Right(name.GetLength()-pos) == ".drl" )
				DeleteFile( m_folder + "\\" + name );
		}
    }

	BOOL errors = FALSE;	// if errors occur

	// gerber name style
	int gCASE = 1;

	// create Gerber files for selected layers
	for( int i=LAY_FREE_LINE; i<MAX_LAYERS; i++ )
		clrbit( m_flags, i );
	for( int i=0; i<MAX_LAYERS; i++ )
	{
		CString gerber_name = "";
		int layer = 0;
		if( getbit( m_layers,i ) == 0 && i < MAX_LAYERS-1 )
			continue;
		switch(i)
		{
		case 0: 
				gerber_name = "FREE_LINE.gbr";
				layer = LAY_FREE_LINE;
				setbit( m_flags, LAY_FREE_LINE );
				break;
		case 1: 
				gerber_name = "PART_OUTLINE.gbr";
				layer = LAY_PART_OUTLINE;
				setbit( m_flags, LAY_PART_OUTLINE );
				break;
		case 2: 
				gerber_name = "REF_DES.gbr";
				layer = LAY_PART_NAME;
				setbit( m_flags, LAY_PART_NAME );
				break;
		case 3: 
				gerber_name = "PIN_LINE.gbr";
				layer = LAY_PIN_LINE;
				setbit( m_flags, LAY_PIN_LINE );
				break;
		case 4: 
				gerber_name = "PIN_NAME.gbr";
				layer = LAY_PIN_NAME;
				setbit( m_flags, LAY_PIN_NAME );
				break;
		case 5: 
				gerber_name = "CONNECTION.gbr";
				layer = LAY_CONNECTION;
				setbit( m_flags, LAY_CONNECTION );
				break;
		case 6: 
				gerber_name = "NET_NAME.gbr";
				layer = LAY_NET_NAME;
				setbit( m_flags, LAY_NET_NAME );
				break;
		case 7: 
				gerber_name = "FOOTPRINT.gbr"; 
				layer = LAY_FOOTPRINT;
				setbit( m_flags, LAY_FOOTPRINT );
				break;
		case 8: 
				gerber_name = "VALUE.gbr";
				layer = LAY_PART_VALUE;
				setbit( m_flags, LAY_PART_VALUE );
				break;
		case 9: 
				gerber_name = "DESCRIPTION.gbr";
				layer = LAY_PIN_DESC;
				setbit( m_flags, LAY_PIN_DESC );
				break;
		case 10: 
				gerber_name = "FRONT_LAYER_1.gbr";
				layer = LAY_ADD_1;
				setbit( m_flags, LAY_ADD_1 );
				break;
		case 11: 
				gerber_name = "FRONT_LAYER_2.gbr";
				layer = LAY_ADD_2;
				setbit( m_flags, LAY_ADD_2 );
				break;
		case 12: 
				gerber_name = "BACK_LAYER_1.gbr";
				layer = LAY_ADD_2+1;
				setbit( m_flags, LAY_ADD_2+1 );
				break;
		case 13: 
				gerber_name = "BACK_LAYER_2.gbr";
				layer = LAY_ADD_2+2;
				setbit( m_flags, LAY_ADD_2+2 );
				break;
		case 14: 
				gerber_name = "BACK_LAYER_3.gbr";
				layer = LAY_ADD_2+3;
				setbit( m_flags, LAY_ADD_2+3 );
				break;
		case 15: 
				gerber_name = "BACK_LAYER_4.gbr";
				layer = LAY_ADD_2+4;
				setbit( m_flags, LAY_ADD_2+4 );
				break;
		case 16: 
				gerber_name = "BACK_LAYER_5.gbr";
				layer = LAY_ADD_2+5;
				setbit( m_flags, LAY_ADD_2+5 );
				break;
		case 17: 
				gerber_name = "BACK_LAYER_6.gbr";
				layer = LAY_ADD_2+6;
				setbit( m_flags, LAY_ADD_2+6 );
				break;
		case 18: 
				gerber_name = "BACK_LAYER_7.gbr";
				layer = LAY_ADD_2+7;
				setbit( m_flags, LAY_ADD_2+7 );
				break;
		case 19: 
				gerber_name = "BACK_LAYER_8.gbr";
				layer = LAY_ADD_2+8;
				setbit( m_flags, LAY_ADD_2+8 );
				break;
		case 20: 
				gerber_name = "BACK_LAYER_9.gbr";
				layer = LAY_ADD_2+9;
				setbit( m_flags, LAY_ADD_2+9 );
				break;
		case 21: 
				gerber_name = "BACK_LAYER_10.gbr";
				layer = LAY_ADD_2+10;
				setbit( m_flags, LAY_ADD_2+10 );
				break;
		case 22: 
				gerber_name = "BACK_LAYER_11.gbr";
				layer = LAY_ADD_2+11;
				setbit( m_flags, LAY_ADD_2+11 );
				break;
		case 23: 
				gerber_name = "BACK_LAYER_12.gbr";
				layer = LAY_ADD_2+12;
				setbit( m_flags, LAY_ADD_2+12 );
				break;
		default: 
				gerber_name = "RENDER_ALL.gbr";
				layer = MAX_LAYERS;
				break;
		}
		if( ( getbit( m_flags, GERBER_RENDER_ALL ) == 0 && layer < MAX_LAYERS ) ||
			( getbit( m_flags, GERBER_RENDER_ALL ) && layer == MAX_LAYERS ) )
		{
			// write the gerber file
			CString f_str = m_folder + "\\" + gerber_name;
			CStdioFile f;
			int ok = f.Open( f_str, CFile::modeCreate | CFile::modeWrite );
			if( !ok )
			{
				CString log_message;
				log_message.Format( "ERROR: Unable to open file \"%s\"\r\n", f_str );
				m_dlg_log->AddLine( log_message );
				errors = TRUE;
			}
			else
			{
				CString log_message;
				log_message.Format( "Writing file: \"%s\"\r\n", f_str );
				m_dlg_log->AddLine( log_message );
				CString line;
				line.Format( "G04 %sMFC Application*\n", CDS_HEADER );
				f.WriteString( line );
				line.Format( "G04 Version: %s v1.1*\n", PROGRAM_NAME );
				f.WriteString( line );
				line.Format( "G04 WebSite: https://github.com/niconson*\n" );
				f.WriteString( line );
				line.Format( "G04 %s*\n", f_str );
				f.WriteString( line );
				::WriteGerberFile( &f, m_flags, layer, 
									m_dlg_log, 
									m_min_text_line_width, 
									m_highlight_width, 
									m_min_polyline_width,
									m_min_text_height,
									m_op, m_Attr, m_dl, m_ml );
				f.WriteString( "M02*\n" );	// end of job
				f.Close();
			}
		}
	}
	if( errors )
		m_dlg_log->AddLine( "****** ERRORS OCCURRED DURING CREATION OF GERBERS ******\r\n" );
	else
		m_dlg_log->AddLine( "************ ALL GERBERS CREATED SUCCESSFULLY **********\r\n" );
	//
	if( getbit( m_flags, GERBER_RENDER_ALL ) )
	{
		// create command-line to render layer
		CString command_str = m_app_folder + "\\GerberRender.exe -r 400 -a 0.04 RENDER_ALL.gbr RENDER_ALL.png";

		_chdir( m_folder );		// change current working directory to CAM folder
		m_dlg_log->AddLine( "****************** RENDERING .PNG FILE *****************\r\n" );
		m_dlg_log->AddLine( "Run: " + command_str + "\r\n" ); 
		::RunConsoleProcess( command_str, m_dlg_log );
		m_dlg_log->AddLine( "\r\n" ); 
		
		m_dlg_log->AddLine( "************************* DONE *************************\r\n" ); 
		_chdir( m_app_folder );	// change back
		ShellExecute(	NULL,"open",m_folder,
									NULL,m_folder,SW_SHOWNORMAL);
	}
	clrbit( m_flags, FL_LATER );
	OnOK();
}

void CDlgCAD::GetFields()
{
	CString str;
	double mult;
	if( m_units == MIL )
		mult = NM_PER_MIL;
	else
		mult = 1000000.0;
	m_edit_text_line_w.GetWindowText( str );
	m_min_text_line_width = atof( str ) * mult;
	m_edit_high_w.GetWindowText( str );
	m_highlight_width = atof( str ) * mult;
	m_edit_polyline_w.GetWindowText( str );
	m_min_polyline_width = atof( str ) * mult;
	m_edit_text_h.GetWindowText( str );
	m_min_text_height = atof( str ) * mult;

	// get flags
	if( m_check_render_all.GetCheck() )
		setbit( m_flags, GERBER_RENDER_ALL );
	else
		clrbit( m_flags, GERBER_RENDER_ALL );
	//
	if( m_radio_VM.GetCheck() )
		setbit( m_flags, FL_VIEWMATE );
	else
		clrbit( m_flags, FL_VIEWMATE );
	//
	if( m_radio_GV.GetCheck() )
		setbit( m_flags, FL_GERBV );
	else
		clrbit( m_flags, FL_GERBV );
	//
	if( m_radio_GL.GetCheck() )
		setbit( m_flags, FL_GERBERLOGIX );
	else
		clrbit( m_flags, FL_GERBERLOGIX );

	// get layers
	m_layers = 0x0;
	for( int i=0; i<26; i++ )
	{
		switch(i)
		{
		case 0: if( m_check_free_line.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 1: if( m_check_part_line.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 2: if( m_check_part_name.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 3: if( m_check_pin_line.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 4: if( m_check_pin_name.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 5: if( m_check_net_line.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 6: if( m_check_net_name.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 7: if( m_check_footprint.GetCheck() )
					m_layers |= 1<<i; 
				 break;
		case 8: if( m_check_part_value.GetCheck() )
					m_layers |= 1<<i; 
				 break;
		case 9: if( m_check_description.GetCheck() )
					m_layers |= 1<<i; 
				 break;
		case 10: if( m_check_add1.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 11: if( m_check_add2.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 12: if( m_check_add3.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 13: if( m_check_add4.GetCheck() )
					m_layers |= 1<<i;  
				break;
		case 14: if( m_check_add5.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 15: if( m_check_add6.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 16: if( m_check_add7.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 17: if( m_check_add8.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 18: if( m_check_add9.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 19: if( m_check_add10.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 20: if( m_check_add11.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 21: if( m_check_add12.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 22: if( m_check_add13.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		case 23: if( m_check_add14.GetCheck() )
					m_layers |= 1<<i;  
				 break;
		default: continue;
		}
	}
}

void CDlgCAD::SetFields()
{
	CString str;
	double mult;
	if( m_units == MIL )
		mult = NM_PER_MIL;
	else
		mult = 1000000.0;
	MakeCStringFromDouble( &str, m_min_text_line_width/mult );
	m_edit_text_line_w.SetWindowText( str );
	MakeCStringFromDouble( &str, m_highlight_width/mult );
	m_edit_high_w.SetWindowText( str );
	MakeCStringFromDouble( &str, m_min_polyline_width/mult );
	m_edit_polyline_w.SetWindowText( str );
	MakeCStringFromDouble( &str, m_min_text_height/mult );
	m_edit_text_h.SetWindowText( str );

	//
	m_check_render_all.SetCheck(getbit( m_flags, GERBER_RENDER_ALL ));
	m_radio_VM.SetCheck(getbit( m_flags, FL_VIEWMATE ));
	m_radio_GV.SetCheck(getbit( m_flags, FL_GERBV ));
	m_radio_GL.SetCheck(getbit( m_flags, FL_GERBERLOGIX ));

	//
	for( int i=0; i<26; i++ )
	{
		switch(i)
		{
		case 0: m_check_free_line.SetCheck(m_layers & (1<<i)); break;
		case 1: m_check_part_line.SetCheck(m_layers & (1<<i)); break;
		case 2: m_check_part_name.SetCheck(m_layers & (1<<i)); break;
		case 3: m_check_pin_line.SetCheck(m_layers & (1<<i)); break;
		case 4: m_check_pin_name.SetCheck(m_layers & (1<<i)); break;
		case 5: m_check_net_line.SetCheck(m_layers & (1<<i)); break;
		case 6: m_check_net_name.SetCheck(m_layers & (1<<i)); break;
		case 7: m_check_footprint.SetCheck(m_layers & (1<<i)); break;
		case 8: m_check_part_value.SetCheck(m_layers & (1<<i)); break;
		case 9: m_check_description.SetCheck(m_layers & (1<<i)); break;
		case 10: m_check_add1.SetCheck(m_layers & (1<<i)); break;
		case 11: m_check_add2.SetCheck(m_layers & (1<<i)); break; 
		case 12: m_check_add3.SetCheck(m_layers & (1<<i)); break;
		case 13: m_check_add4.SetCheck(m_layers & (1<<i)); break;
		case 14: m_check_add5.SetCheck(m_layers & (1<<i)); break;
		case 15: m_check_add6.SetCheck(m_layers & (1<<i)); break;
		case 16: m_check_add7.SetCheck(m_layers & (1<<i)); break;
		case 17: m_check_add8.SetCheck(m_layers & (1<<i)); break;
		case 18: m_check_add9.SetCheck(m_layers & (1<<i)); break;
		case 19: m_check_add10.SetCheck(m_layers & (1<<i)); break;
		case 20: m_check_add11.SetCheck(m_layers & (1<<i)); break;
		case 21: m_check_add12.SetCheck(m_layers & (1<<i)); break;
		case 22: m_check_add13.SetCheck(m_layers & (1<<i)); break;
		case 23: m_check_add14.SetCheck(m_layers & (1<<i)); break;
		default: continue;
		}
	}
}
void CDlgCAD::OnCbnSelchangeComboCadUnits()
{
	GetFields();
	if( m_combo_units.GetCurSel() == 0 )
		m_units = MIL;
	else
		m_units = MM;
	SetFields();
}

void CDlgCAD::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgCAD::OnBnClickedButtonDef()
{
	//CString win_t;
	m_edit_folder.GetWindowText( m_folder );
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	CString s[5];
	s[0].Format( "%s\\CAM(%s)", m_project_folder, m_f_name );
	s[1].Format( "%s\\Print(%s)", m_project_folder, m_f_name );
	s[2].Format( "%s\\View1(%s)", m_project_folder, m_f_name );
	s[3].Format( "%s\\View2(%s)", m_project_folder, m_f_name );
	s[4].Format( "%s\\%s(%d-%d-%d)", m_project_folder, m_f_name, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year+1900 );
	int i=0;
	for(; i<5; i++)
		if( m_folder.CompareNoCase(s[i]) == 0 )
		{
			i++;
			if(i>4)
				i=0;
			m_folder = s[i];
			break;
		}
	if( i == 5 )
	{
		CString s2[5], s6;
		for(i=0; i<5; i++)
			s2[i] = s[i].Left(s[i].Find("("));
		int f = m_folder.Find("(");
		if( f > 0 )
		{
			s6 = m_folder.Left(f);
			for(i=0; i<5; i++)
				if( s6.CompareNoCase(s2[i]) == 0 )
				{
					m_folder = s[i];
					break;
				}
			if( i == 5 )
				m_folder = s[0];
		}
		else
		{
			m_folder = s[0];
		}
	}
	m_edit_folder.SetWindowText( m_folder );
}

void CDlgCAD::OnBnClickedButtonFolder()
{
	if( m_folder.GetLength() == 0 )
	{
		m_folder.Format( "%s\\CAM(%s)", m_project_folder, m_f_name );
		m_edit_folder.SetWindowText( m_folder );
	}
	CPathDialog dlg(G_LANGUAGE==0?"Select Folder":"Папка для герберов", 
					G_LANGUAGE == 0 ? "Set CAM output folder":"Выберите папку", m_folder);
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		m_folder = dlg.GetPathName();
		m_edit_folder.SetWindowText( m_folder );
	}
}

void CDlgCAD::OnBnClickedButtonDone()
{
	GetFields();
	setbit( m_flags, FL_LATER );
	OnOK();
}

void CDlgCAD::OnChangeViewer()
{
	GetFields();
	m_bmp.DeleteObject();
	if( m_radio_VM.GetCheck() )
	{
		m_bmp.LoadBitmap( IDB_BITMAP_VIEWMATE );
		m_viewer.SetBitmap( m_bmp );
	}
	else if( m_radio_GV.GetCheck() )
	{
		m_bmp.LoadBitmap( IDB_BITMAP_GERBV );
		m_viewer.SetBitmap( m_bmp );
	}
	else if( m_radio_GL.GetCheck() )
	{
		m_bmp.LoadBitmap( IDB_BITMAP_GERBERLOGIX );
		m_viewer.SetBitmap( m_bmp );
	}
	else
	{
		m_bmp.LoadBitmap( IDB_BITMAP_PNG );
		m_viewer.SetBitmap( m_bmp );
	}
}
