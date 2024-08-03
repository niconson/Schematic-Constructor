// DlgProjectOptions.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgProjectOptions.h"
#include "DlgAddWidth.h"
#include "PathDialog.h"
#include ".\dlgprojectoptions.h"

// global callback function for sorting
//		
int CALLBACK WidthCompare( LPARAM lp1, LPARAM lp2, LPARAM type )
{
	if( lp1 == lp2 )
		return 0;
	else if( lp1 > lp2 )
		return 1;
	else
		return -1;
}

// CDlgProjectOptions dialog

IMPLEMENT_DYNAMIC(CDlgProjectOptions, CDialog)
CDlgProjectOptions::CDlgProjectOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProjectOptions::IDD, pParent)
	, m_trace_w(0)
	, m_font_nmb(0)
	, m_layers(0)
{
	m_folder_changed = FALSE;
	m_folder_has_focus = FALSE;
}

CDlgProjectOptions::~CDlgProjectOptions()
{
}

void CDlgProjectOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit_name);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name );
	//
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_edit_folder);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_path_to_folder );
	//
	DDX_Control(pDX, IDC_EDIT_LIBRARY_FOLDER, m_edit_lib_folder);
	DDX_Text(pDX, IDC_EDIT_LIBRARY_FOLDER, m_lib_folder );
	//
	DDX_Control(pDX, IDC_EDIT_NUM_LAYERS, m_edit_layers );
	DDX_Text(pDX, IDC_EDIT_NUM_LAYERS, m_layers );
	DDV_MinMaxInt(pDX, m_layers, 4, 16 );
	DDX_Text(pDX, IDC_EDIT_SCALE, m_scale );
	DDV_MinMaxDouble(pDX, m_scale, 0.01, 100.0 );
	//
	DDX_Text(pDX, IDC_EDIT_PARENT_I, m_parent );
	DDV_MinMaxInt(pDX, m_parent, 1, 8 );
	//
	DDX_Control(pDX, IDC_DEFAULT_TEXT_FONT, m_font );
	//
	DDX_Control(pDX, IDC_DEFAULT_FOLDER, mb_default );
	if( pDX->m_bSaveAndValidate )
	{
		// leaving dialog
		if( m_name.GetLength() == 0 || m_name.Trim().GetLength() == 0 || m_name.FindOneOf( ILLEGAL_FILENAME ) >= 0 )
		{
			pDX->PrepareEditCtrl( IDC_EDIT_NAME );
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Please enter name for project":
				"Введите название проекта");
			pDX->Fail();
		}
		else if( m_path_to_folder.GetLength() == 0 )
		{
			pDX->PrepareEditCtrl( IDC_EDIT_FOLDER );
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Please enter project folder":
				"Пожалуйста, введите папку проекта");
			pDX->Fail();
		}
		else if( m_lib_folder.GetLength() == 0 )
		{
			pDX->PrepareEditCtrl( IDC_EDIT_LIBRARY_FOLDER );
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Please enter library folder":
				"Пожалуйста, введите папку библиотеки");
			pDX->Fail();
		}
		else
		{

			CString s = "";
			DDX_Text(pDX,IDC_EDIT_DEF_TRACE_W,s);
			if( s.Right(2) == "MM" || s.Right(2) == "mm" )
				m_trace_w = -my_atof(&s);
			else
				m_trace_w = my_atof(&s);
			m_trace_w = (double)m_trace_w/m_scale;

			s = "";
			DDX_Text(pDX,IDC_EDIT_NODE_W,s);
			if( s.Right(2) == "MM" || s.Right(2) == "mm" )
				m_node_w = -my_atof(&s);
			else
				m_node_w = my_atof(&s);
			m_node_w = (double)m_node_w/m_scale;

			m_font_nmb = m_font.GetCurSel();
			if( m_font_nmb < 0 )
				m_font_nmb = SIMPLEX;	
			DDX_Text(pDX,IDC_A_CLEARANCE,s);
			m_alignment = my_atof(&s);
			if( m_alignment > 2.0 )
				m_alignment = 2.0;
			if( m_alignment < 0.5 )
				m_alignment = 0.5;
			//
			m_default = mb_default.GetCheck();
			m_path_to_folder = m_path_to_folder.Left( m_path_to_folder.GetLength() - 1 );
		}
	}
	else
	{
		m_font.InsertString( 0, "GOTHIC" );
		m_font.InsertString( 0, "FANCY" );
		m_font.InsertString( 0, "ITALLIC_TRIPLEX" );
		m_font.InsertString( 0, "ITALLIC_DUPLEX" );
		m_font.InsertString( 0, "SCRIPT_DUPLEX" );
		m_font.InsertString( 0, "SCRIPT_SIMPLEX" );
		m_font.InsertString( 0, "MODERN" );
		m_font.InsertString( 0, "TRIPLEX" );
		m_font.InsertString( 0, "DUPLEX" );
		m_font.InsertString( 0, "SIMPLEX" );
		m_font.InsertString( 0, "SMALL_DUPLEX" );
		m_font.InsertString( 0, "SMALL_SIMPLEX" );
		m_font.SetCurSel( m_font_nmb );
		CString s;
		::MakeCStringFromDimension( m_scale, &s, abs(m_node_w), (m_node_w>=0?MIL:MM), TRUE, TRUE, FALSE, (m_node_w>=0?0:2) );
		DDX_Text(pDX,IDC_EDIT_NODE_W,s);
		::MakeCStringFromDimension( m_scale, &s, abs(m_trace_w), (m_trace_w>=0?MIL:MM), TRUE, TRUE, FALSE, (m_trace_w>=0?0:2) );
		DDX_Text(pDX,IDC_EDIT_DEF_TRACE_W,s);
		::MakeCStringFromDimension( 1.0, &s, m_parent, 0, 0, 0 );
		DDX_Text(pDX,IDC_EDIT_PARENT_I,s);
		s.Format( "%3.2f", m_alignment );
		DDX_Text(pDX,IDC_A_CLEARANCE,s);
		s.Format( "%2.5f", m_scale );
		DDX_Text(pDX,IDC_EDIT_SCALE,s);
		//
		mb_default.SetCheck(0);
		mb_default.EnableWindow( m_new_project );
	}
}


BEGIN_MESSAGE_MAP(CDlgProjectOptions, CDialog)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_EN_CHANGE(IDC_EDIT_FOLDER, OnEnChangeEditFolder)
	ON_EN_SETFOCUS(IDC_EDIT_FOLDER, OnEnSetfocusEditFolder)
	ON_EN_KILLFOCUS(IDC_EDIT_FOLDER, OnEnKillfocusEditFolder)
	ON_BN_CLICKED(IDC_BUTTON_LIB, OnBnClickedButtonLib)
	ON_BN_CLICKED(IDC_BUTTON_PRF, OnBnClickedButtonProjectFolder)
	ON_BN_CLICKED(ID_Q11, &CDlgProjectOptions::OnBnClickedQ11)
	ON_BN_CLICKED(IDC_DEFAULT_FOLDER, &CDlgProjectOptions::OnBnClickedDefaultFolder)
END_MESSAGE_MAP()

// initialize data
//
void CDlgProjectOptions::Init( BOOL new_project,
							  CString * name,
							  CString * path_to_folder,
							  CString * lib_folder,
							  CString * app_dir,
							  int num_layers,
							  BOOL bSMT_connect_copper,
							  int node_w,
							  int trace_w,
							  int font_n,
							  int parent,
							  float a_spacing,
							  double scale)
{
	m_new_project = new_project;
	if(!m_new_project)
		m_name = *name;
	else
		m_name = "";
	//
	m_scale = scale;
	int isep = app_dir->ReverseFind( '\\' );
	if( isep == -1 )
		isep = app_dir->ReverseFind( ':' );
	if( isep == -1 )
		ASSERT(0);		// unable to parse filename
	CString app = app_dir->Left(isep+1);
	struct _stat buf;
	m_path_to_folder = *path_to_folder;
	if( m_path_to_folder.Right(1) == "\\" )
		m_path_to_folder = m_path_to_folder.Left(m_path_to_folder.GetLength() - 1);
	int err = _stat( m_path_to_folder, &buf );//ok
	if( err )
	{
		m_path_to_folder = app + "Projects";
	}
	if( m_path_to_folder.Right(1) != "\\" )
		m_path_to_folder += "\\";
	//
	m_lib_folder = *lib_folder;
	if( m_lib_folder.Right(1) == "\\" )
		m_lib_folder = m_lib_folder.Left(m_lib_folder.GetLength() - 1);
	err = _stat( m_lib_folder, &buf );//ok
	if( err )
	{
		m_lib_folder = app + "fp_lib\\lib";
	}
	//
	m_layers = num_layers;
	m_node_w = node_w;
	m_trace_w = trace_w;
	m_font_nmb = font_n;
	m_parent = parent;
	m_alignment = a_spacing;
}

BOOL CDlgProjectOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	// initialize strings
	m_edit_folder.SetWindowText( m_path_to_folder );
	// disable some fields for existing project
	m_edit_folder.EnableWindow( FALSE );
	m_edit_lib_folder.EnableWindow( FALSE );
	//
	return TRUE;
}

void CDlgProjectOptions::OnEnChangeEditName()
{
	///CString str;
	///m_edit_name.GetWindowText( str ); 
	///if( m_new_project == TRUE && m_folder_changed == FALSE )
	///	m_edit_folder.SetWindowText( m_path_to_folder + str );
}

void CDlgProjectOptions::OnEnChangeEditFolder()
{
	if( m_folder_has_focus )
		m_folder_changed = TRUE;
}

void CDlgProjectOptions::OnEnSetfocusEditFolder()
{
	m_folder_has_focus = TRUE;
}

void CDlgProjectOptions::OnEnKillfocusEditFolder()
{
	m_folder_has_focus = FALSE;
}

void CDlgProjectOptions::OnBnClickedButtonLib()
{
	CPathDialog dlg(G_LANGUAGE == 0 ? "Library Folder":"Папка библиотек", 
					G_LANGUAGE == 0 ? "Select default library folder":"Выберите папку", m_lib_folder);
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		m_lib_folder = dlg.GetPathName().MakeLower();
		m_edit_lib_folder.SetWindowText( m_lib_folder );
	}
}

void CDlgProjectOptions::OnBnClickedButtonProjectFolder()
{
	if( m_new_project )
	{
		CPathDialog dlg(G_LANGUAGE == 0 ? "Project Folder":"Папка проекта", 
						G_LANGUAGE == 0 ? "Select project folder":"Выберите папку", m_path_to_folder);
		int ret = dlg.DoModal();
		if( ret == IDOK )
		{
			CString s = dlg.GetPathName()+"\\";
			m_edit_folder.SetWindowText( s );
		}
	}
}


void CDlgProjectOptions::OnBnClickedQ11()
{
	// TODO: добавьте свой код обработчика уведомлений
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("Specify the path to the FreePCB-2 footprint library.\n\n"\
		"If you use some other PCB editor instead of FreePCB-2, "\
		"then still specify the path to your library to load the list "\
		"of footprints. Version 1.4 is configured to read third-party "\
		"libraries so that you can always select a footprint name "\
		"from the list when editing part attributes. The library folder "\
		"must contain footprint files (<footprint name>. <extension>) "\
		"for reading to complete successfully. If your PCB editor uses "\
		"this library format, then success is guaranteed. Associate "\
		"the footprint file extension with your footprint viewer so "\
		"that "PROGRAM_NAME" and Windows know which program to "\
		"open these files. In this case, you can open and view the "\
		"footprint from the attribute setting dialog by pressing the \"L\" button. "\
		"So, to see the list of loaded names, select a part (or a group "\
		"of polylines) and press the F1 button. The \"SET ATTRIBUTES\" dialog "\
		"box will pop up, and you will find the \"FOOTPRINT NAME\" combobox "\
		"and the \"L\" button to the right of it."):
		("Укажите путь к библиотеке посадочных мест ПлатФорм.\n\n"\
		"Если вы используете какой-либо другой редактор печатных плат вместо ПлатФорм, "\
		"тогда всё равно укажите путь к вашей библиотеке для загрузки списка "\
		"посадочных мест. Версия 1.4 настроена на чтение сторонних "\
		"библиотек, чтобы вы всегда могли выбрать имя посадочного места "\
		"из списка при редактировании атрибутов детали. Папка библиотеки "\
		"должна содержать файлы посадочных мест (<имя посадочного места>.<расширение>) "\
		"для успешного завершения чтения. Если ваш редактор печатных плат использует "\
		"этот формат библиотеки, то успех гарантирован. Свяжите "\
		"расширение файла посадочного места с вашим средством просмотра посадочных мест, чтобы "\
		PROGRAM_NAME" и Windows знали, какой программой "\
		"открывать эти файлы. В этом случае вы можете открыть и просмотреть "\
		"посадочное место из диалогового окна настройки атрибутов, нажав кнопку «L».\n\n"\
		"Чтобы увидеть список загруженных имен, выберите деталь "\
		"и нажмите кнопку F1. Появится диалоговое окно атрибутов, "\
		"в котором вы найдёте выпадающий список футпринтов и "\
		"кнопку «L» справа от него."), MB_ICONINFORMATION);
}


void CDlgProjectOptions::OnBnClickedDefaultFolder()
{
	// TODO: добавьте свой код обработчика уведомлений
}
