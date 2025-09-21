// FreePcbDoc.cpp : implementation of the CFreePcbDoc class
//
#pragma once

#include "stdafx.h"
#include "PcbFont.h"
#include "DlgProjectOptions.h"
#include "DlgImportOptions.h" 
#include "Documents.h"
#include "DlgLayers.h"
#include "DlgPDFLayers.h"
#include "DlgIndexing.h"
#include "UndoBuffer.h"
#include "UndoList.h"
#include "DlgCAD.h"
#include "utility.h"
#include "gerber.h"
#include "dlgdrc.h"
#include "DlgReassignLayers.h"
#include "RTcall.h"
#include "DlgReport.h"
#include "DlgMyMessageBox.h"
#include "DlgRenumbers.h"
#include "DlgAddMerge.h"
#include "NetlistSettings.h"
#include "DlgAttrSync.h"
#include "cpdflib.h"
#include "CCompare.h"
#include "DlgPinMap.h"
#include "ComponentNoteExt.h"
#include "DlgProtection.h"
#include "DlgCreateBranch.h"
#include "ImportNetlist.h"
#include "PathDialog.h"
#include "SpeedFiles.h"
#include "ReadFootprintFolder.h"
#include "ReportFiles.h"
#include "BomInTable.h"
#include "TagTable.h"
#include "math.h"
//
//#include <fstream>
//using namespace std;
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFreeasyApp theApp;

CFreePcbDoc * this_Doc;		// global for callback

/////////////////////////////////////////////////////////////////////////////
// CFreePcbDoc

IMPLEMENT_DYNCREATE(CFreePcbDoc, CDocument)

BEGIN_MESSAGE_MAP(CFreePcbDoc, CDocument)
	//{{AFX_MSG_MAP(CFreePcbDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_VIEW_LAYERS, OnViewLayers)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_GENERATECADFILES, OnFileGenerateCadFiles)
	ON_COMMAND(ID_PROJECT_OPTIONS, OnProjectOptions)
	ON_COMMAND(ID_PROJECT_PARTLIST, OnProjectPartList)
	ON_COMMAND(ID_FILE_GENERATEREPORTFILE, OnProjectGenBOM)
	ON_COMMAND(ID_FILE_GENERATEREPORT2, OnProjectGenBOM2)
	ON_COMMAND(ID_FILE_GENERATEREPORT3, OnProjectGenTag)
	ON_COMMAND(ID_FILE_GENERATEREPORT4, OnProjectGenBOM3)
	ON_COMMAND(ID_PROJECT_ADAPT_GRID, OnProjectAdaptGrid)
	ON_COMMAND(ID_FILE_EXPORTNETLIST, OnFileExport1)
	ON_COMMAND(ID_FILE_SETNETLIST,	  OnFileSetNetlist)
	ON_COMMAND(ID_TOOLS_DRC, OnToolsDrc)
	ON_COMMAND(ID_TOOLS_CLEAR_DRC, OnToolsClearDrc)
	ON_COMMAND(ID_TOOLS_CLEAR_ALL_DRC, OnToolsClearAllDrc)
	ON_COMMAND(ID_TOOLS_SHOWDRCERRORLIST, OnToolsShowDRCErrorlist)
	ON_COMMAND(ID_VIEW_LOG, OnViewLog)
	ON_COMMAND(ID_EDIT_PASTEFROMFILE, OnEditPasteFromFile)
	ON_COMMAND(ID_SELECTALL, OnEditSelectAll)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_CAM1, OnFileCam1)
	ON_COMMAND(ID_FILE_CAM2, OnFileCam2)
	ON_COMMAND(ID_FILE_CAM3, OnFileCam3)
	ON_COMMAND(ID_FILE_CAM4, OnFileCam4)
	ON_COMMAND(ID_FILE_CAM5, OnFileCam5)
	ON_COMMAND(ID_FILE_CAM6, OnFileCam6)
	ON_COMMAND(ID_FILE_CAM7, OnFileCam7)
	ON_COMMAND(ID_FILE_CAM8, OnFileCam8)
	ON_COMMAND(ID_FILE_CAM9, OnFileCam9)
	ON_COMMAND(ID_FILE_CAM10, OnFileCam10)
	ON_COMMAND(ID_FILE_CAM11, OnFileCam11)
	ON_COMMAND(ID_FILE_CAM12, OnFileCam12)
	ON_COMMAND(ID_FILE_CAM13, OnFileCam13)
	ON_COMMAND(ID_FILE_CAM14, OnFileCam14)
	ON_COMMAND(ID_FILE_CAM15, OnFileCam15)
	ON_COMMAND(ID_FILE_CAM16, OnFileCam16)
	ON_COMMAND(ID_FILE_CAM17, OnFileCam17)
	ON_COMMAND(ID_FILE_CAM18, OnFileCam18)
	ON_COMMAND(ID_FILE_CAM19, OnFileCam19)
	ON_COMMAND(ID_FILE_CAM20, OnFileCam20)
	ON_COMMAND(ID_FILE_CAM21, OnFileCam21)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_NONE_REPEATDRC, OnRepeatDrc)
	ON_COMMAND(ID_TOOLS_REPEATDRC, OnRepeatDrc)
	ON_COMMAND(ID_PROJECT_COMBINEBOARD, OnProjectCombineBoard)
	ON_COMMAND(ID_CREATE_POLYLINE, OnCreatePolyline)
	ON_COMMAND(ID_PART_RENUMB, RenumberPartDesignations)
	ON_COMMAND(ID_PART_CLEAR, ClearPartDesignations)
	ON_COMMAND(ID_PART_RAISE, RaisePartNumbers)
	ON_COMMAND(ID_PART_REMOVE_SUFFIX, TryToRemoveSuffix)
	ON_COMMAND(ID_NEW_PAGE, AddPage)
	ON_COMMAND(ID_PAGE_1, SwitchToPage1)
	ON_COMMAND(ID_PAGE_2, SwitchToPage2)
	ON_COMMAND(ID_PAGE_3, SwitchToPage3)
	ON_COMMAND(ID_PAGE_4, SwitchToPage4)
	ON_COMMAND(ID_PAGE_5, SwitchToPage5)
	ON_COMMAND(ID_PAGE_6, SwitchToPage6)
	ON_COMMAND(ID_PAGE_7, SwitchToPage7)
	ON_COMMAND(ID_PAGE_8, SwitchToPage8)
	ON_COMMAND(ID_DPAGE_1, DeletePage1)
	ON_COMMAND(ID_DPAGE_2, DeletePage2)
	ON_COMMAND(ID_DPAGE_3, DeletePage3)
	ON_COMMAND(ID_DPAGE_4, DeletePage4)
	ON_COMMAND(ID_DPAGE_5, DeletePage5)
	ON_COMMAND(ID_DPAGE_6, DeletePage6)
	ON_COMMAND(ID_DPAGE_7, DeletePage7)
	ON_COMMAND(ID_DPAGE_8, DeletePage8)
	ON_COMMAND(ID_RNPAGE_1, RenamePage1)
	ON_COMMAND(ID_RNPAGE_2, RenamePage2)
	ON_COMMAND(ID_RNPAGE_3, RenamePage3)
	ON_COMMAND(ID_RNPAGE_4, RenamePage4)
	ON_COMMAND(ID_RNPAGE_5, RenamePage5)
	ON_COMMAND(ID_RNPAGE_6, RenamePage6)
	ON_COMMAND(ID_RNPAGE_7, RenamePage7)
	ON_COMMAND(ID_RNPAGE_8, RenamePage8)
	ON_COMMAND(ID_ATTR_SYNC, OnAttrSync)
	ON_COMMAND(ID_DETACH_BMP, OnDetachBMP)
	ON_COMMAND(ID_ATTACH_BMP1, OnAttachBMP1)
	ON_COMMAND(ID_ATTACH_BMP2, OnAttachBMP2)
	ON_COMMAND(ID_ATTACH_GND1, OnAttachGND1)
	ON_COMMAND(ID_ATTACH_GND2, OnAttachGND2)
	ON_COMMAND(ID_ATTACH_GND3, OnAttachGND3)
	ON_COMMAND(ID_ATTACH_GND4, OnAttachGND4)
	ON_COMMAND(ID_ATTACH_GND5, OnAttachGND5)
	ON_COMMAND(ID_PROJECT_REFLIST, AddRefList)
	ON_COMMAND(ID_PROJECT_DELREFLIST, DelRefList)
	ON_COMMAND(ID_PLAY_REFLIST1, PlayRefList1)
	ON_COMMAND(ID_PLAY_REFLIST2, PlayRefList2)
	ON_COMMAND(ID_PLAY_REFLIST3, PlayRefList3)
	ON_COMMAND(ID_PLAY_REFLIST4, PlayRefList4)
	ON_COMMAND(ID_PLAY_REFLIST5, PlayRefList5)
	ON_COMMAND(ID_PLAY_REFLIST6, PlayRefList6)
	ON_COMMAND(ID_PLAY_REFLIST7, PlayRefList7)
	ON_COMMAND(ID_PLAY_REFLIST8, PlayRefList8)
	ON_COMMAND(ID_PLAY_REFLIST9, PlayRefList9)
	ON_COMMAND(ID_PLAY_REFLIST10, PlayRefList10)
	ON_COMMAND(ID_PLAY_REFLIST11, PlayRefList11)
	ON_COMMAND(ID_PLAY_REFLIST12, PlayRefList12)
	ON_COMMAND(ID_PLAY_REFLIST13, PlayRefList13)
	ON_COMMAND(ID_PLAY_REFLIST14, PlayRefList14)
	ON_COMMAND(ID_PLAY_REFLIST15, PlayRefList15)
	ON_COMMAND(ID_PLAY_REFLIST16, PlayRefList16)
	ON_COMMAND(ID_PLAY_REFLIST17, PlayRefList17)
	ON_COMMAND(ID_PLAY_REFLIST18, PlayRefList18)
	ON_COMMAND(ID_PLAY_REFLIST19, PlayRefList19)
	ON_COMMAND(ID_PLAY_REFLIST20, PlayRefList20)
	ON_COMMAND(ID_VIEW_SWITCH_PCB, SwitchToPCB)
	ON_COMMAND(ID_SWITCH_PCB_BUTTON, SwitchToPcbOnButton)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_SEL_ADJACENT, SelectAdj)
	ON_COMMAND(ID_SEL_SIMILAR_POLY, OnSimilarPoly)
	ON_COMMAND(ID_SEL_REPLACE_POLY, OnReplacePoly)
	ON_COMMAND(ID_COMPONENT_NOTE, OnComponentNote)
	ON_COMMAND(ID_COMPONENT_URL, OnComponentURL)
	ON_COMMAND(ID_COMPONENT_IMAGE, OnComponentImage)
	ON_COMMAND(ID_OPEN_STORAGE, OnOpenComponentDatasheets)
	ON_COMMAND(ID_OPEN_STORAGE_M, OnOpenComponentDatasheetsM)
	ON_COMMAND(ID_TXT_NOTE, OnTXTNote)
	ON_COMMAND(ID_DOC_NOTE, OnDOCNote)
	ON_COMMAND(ID_PDF_NOTE, OnPDFNote)
	ON_COMMAND(ID_URL_NOTE, OnURLNote)
	ON_COMMAND(ID_ADD_MARKS, OnAddMarks)
	ON_COMMAND(ID_GET_PIN_MAP, OnGetPinMap)
	ON_COMMAND(ID_ADD_FAVORITE, OnAddFavorite)
	ON_COMMAND(ID_MAKE_ARCHIVED, OnProtection)
	ON_COMMAND(ID_CREATE_BRANCH, OnCreateBranch)
	ON_COMMAND(ID_IMPORT_NETLIST, OnImportNetlist)
	ON_COMMAND(ID_FILE_SEL_FOLDER, OnSelectProjectFolder)
	ON_COMMAND(ID_FILE_RELOAD_MENU, OnReloadMenu)
	ON_COMMAND(ID_LIB_MANAGER, OnRunLibManager)
	ON_COMMAND(ID_FONT_MANAGER, OnRunFontManager)
	ON_COMMAND(ID_IMP_DXF, OnRunImpDxf)
	ON_COMMAND(ID_IMP_PCAD, OnRunImpPcad)
	ON_COMMAND_EX_RANGE(ID_FILE_OPEN_TEMPLATE,ID_FILE_OPEN_TEMPLATE2, OnOpenTemplate)
	ON_COMMAND_EX_RANGE(ID_PROJECT_COMPARE_1,ID_PROJECT_COMPARE_2, OnCompare)
	ON_COMMAND_EX_RANGE(ID_FILE_OPEN_FROM_START,ID_FILE_OPEN_FROM_END, OnSpeedFile)
	ON_COMMAND_EX_RANGE(1, ID_MAX_NUM_COMMANDS, OnRangeCmds)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreePcbDoc construction/destruction

CFreePcbDoc::CFreePcbDoc()
{
	//
	TCHAR szFileName[MAX_PATH*4];
	GetModuleFileName(0, szFileName, MAX_PATH*4);
	m_app_dir = szFileName;
	m_app_dir.Trim();
	int pos = m_app_dir.ReverseFind( '\\' );
	if( pos == -1 )
		pos = m_app_dir.ReverseFind( ':' ); 
	if( pos == -1 )
		ASSERT(0);	// failed to find application folder
	m_app_dir = m_app_dir.Left( pos );

	int err = _chdir( m_app_dir );	// change to application folder
	if( err )
		ASSERT(0);	// failed to switch to application folder
	// new
	m_smfontutil = new SMFontUtil( &m_app_dir );// CFreePcbDoc()
	m_mlist = new StrList();// CFreePcbDoc()
	clip_mlist = new StrList();// CFreePcbDoc()
	m_ref_lists = new StrList();// CFreePcbDoc()
	m_dlg_log = new CDlgLog;// CFreePcbDoc()
	m_dlg_log->Create( IDD_LOG );
	theApp.m_Doc = this;
	this_Doc = this;
	//
	for(int i=0; i<NUM_CAM_FOLDERS; i++)
		CAM[i] = "";
	m_pcb_filename = "";
	m_pcb_full_path = "";
	m_netlist_full_path = "";
	m_find_component = "https://www.google.com/search?q=";
	for( int ii=0; ii<NUM_SHORTCUTS; ii++ )
		m_cam_shortcut[ii] = "";
	m_full_lib_dir = "";
	m_special_chars = "°±©";
	m_project_open = FALSE;
	m_project_modified = FALSE;
	m_project_modified_since_autosave = FALSE;
	m_footprint_modified = FALSE;
	m_footprint_name_changed = FALSE;
	m_auto_interval = 180;
	m_auto_elapsed = 0;
	bNoFilesOpened = TRUE;
	// VERSION (key)
	m_version = 1.427;
	m_file_version = m_version;
	m_protection = 0;
	m_current_page = 0;
	m_parent_index = 2;
	m_system_colors = TRUE;
	m_netlist_completed = 0;
	m_num_layers = 11;
	m_part_renumber_pos = 0;
	m_renumber_left_to_right = 0;
	m_alignment = 1.0;
	// pdf
	m_pdf_checking = 0;
	m_pdf_font_i = 0;
	m_pdf_margin = 0;
	m_partlist_size.cx = 800;
	m_partlist_size.cy = 200;
	m_partlist_pos.cx = 0;
	m_partlist_pos.cy = 0;
	m_partlist_col_w = ~0;
	//
	m_unconnected_pin_layer = LAY_ADD_1 + 2; // background layer
	m_magnetize_value = 100;
	m_letter_spacing = 1.0;
	ClearFootprintNames(this);
	
	m_library.Clear();
	m_merge_library.Clear();
}

CFreePcbDoc::~CFreePcbDoc()
{
	// write lib
	CArray<CString> * lib = m_library.GetLibrary();
	if(lib->GetSize() > 1) 
	{
		CString Lfile = m_app_dir + "\\components.txt";
		CStdioFile fLib;
		int ok = fLib.Open( Lfile, CFile::modeCreate | CFile::modeWrite );
		if(ok)
		{
			for(int i=0; i<lib->GetSize(); i++)
			{
				fLib.WriteString( lib->GetAt(i) );
				fLib.WriteString( "\n" );
			}
			fLib.Close();
		}
	}

	// remove all pages
	Pages.ClearAll();
	Pages.RemoveAll();
	ClearSpeedFiles( this );

	// delete group clipboard
	clip_outline_poly.RemoveAll();
	ClearFootprintNames(this);
	m_library.Clear();
	m_merge_library.Clear();
	delete clip_mlist;
	delete m_mlist;
	delete m_ref_lists;
	delete m_smfontutil;
	// delete log window
	if( m_dlg_log )
	{
		m_dlg_log->DestroyWindow();
		delete m_dlg_log;
	}
}

void CFreePcbDoc::SendInitialUpdate()
{
	CDocument::SendInitialUpdate();
}

// this is only executed once, at beginning of app
//
BOOL CFreePcbDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_window_title = (G_LANGUAGE==0?"no project open":"конструктор схем");
	m_parent_folder = ""; // is empty !
	m_lib_dir = OLD_NO_FP ;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFreePcbDoc serialization

void CFreePcbDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CFreePcbDoc diagnostics

#ifdef _DEBUG
void CFreePcbDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFreePcbDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFreePcbDoc commands


BOOL CFreePcbDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	return CDocument::OnSaveDocument(lpszPathName);
}

void CFreePcbDoc::OnFileNew()
{

	if( FileClose() == IDCANCEL )
		return;

	m_view->CancelSelection();

	// now set default project options
	InitializeNewProject();
	m_parent_index = 2;
	CDlgProjectOptions dlg;
	dlg.Init( TRUE, &m_name, &m_parent_folder, &m_full_lib_dir, &m_app_dir,
		m_num_additional_layers, 0, m_node_w,
		m_polyline_w, m_default_font, m_parent_index, m_alignment, m_view->m_user_scale );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		// set up project file name and path
		m_name = dlg.GetName();
		m_view->m_user_scale = dlg.GetScale();
		m_dlist->SetVisibleGrid( TRUE, m_visual_grid_spacing/m_view->m_user_scale );
		CString fullpath;
		char full[_MAX_PATH];
		fullpath = _fullpath( full, (LPCSTR)dlg.GetPathToFolder(), MAX_PATH );
		m_path_to_folder = (CString)fullpath;
		if( dlg.m_default )
			m_parent_folder = m_path_to_folder;

		// Check if project folder exists and create it if necessary
		struct _stat buf;
		int err = _stat( m_path_to_folder, &buf );
		if( err )
		{
			err = _mkdir( m_path_to_folder );
			if( err )
				AfxMessageBox(G_LANGUAGE == 0 ? 
					("Unable to create folder "+m_path_to_folder):
					("Невозможно создать папку " + m_path_to_folder), MB_ICONERROR);
		}
		if( err )
			return;
		m_path_to_folder = m_path_to_folder + "\\" + m_name;
		err = _stat( m_path_to_folder, &buf );
		if( err )
		{
			err = _mkdir( m_path_to_folder );
			if( err )
				AfxMessageBox(G_LANGUAGE == 0 ?
					("Unable to create folder " + m_path_to_folder) :
					("Невозможно создать папку " + m_path_to_folder), MB_ICONERROR );
		}
		if( err )
			return;
		CString separator = "-";
		if( m_name.Find(".") > 0 )
			separator = ".";
		else if( m_name.Find("_") > 0 )
			separator = "_";
		m_path_to_folder = m_path_to_folder + "\\version" + separator + "01";
		err = _stat( m_path_to_folder, &buf );
		if( err )
		{
			err = _mkdir( m_path_to_folder );
			if( err )
				AfxMessageBox(G_LANGUAGE == 0 ?
					("Unable to create folder " + m_path_to_folder) :
					("Невозможно создать папку " + m_path_to_folder), MB_ICONERROR );
		}
		if( err )
			return;
		m_parent_index = dlg.m_parent;
		m_alignment = dlg.m_alignment;
		m_netlist_completed = 0;
		m_pcb_filename = m_name + separator + "01.cds";
		m_pcb_full_path = m_path_to_folder + "\\" + m_pcb_filename;
		m_window_title = CDS_HEADER + m_pcb_filename;
		EnableMenuItems();

		// make path to library folder and index libraries
		m_lib_dir = dlg.GetLibFolder();
		fullpath = _fullpath( full, (LPCSTR)m_lib_dir, MAX_PATH );
		if( fullpath[fullpath.GetLength()-1] == '\\' )	
			fullpath = fullpath.Left(fullpath.GetLength()-1);
		m_full_lib_dir = fullpath;
		ReadFootprintFolder(this);
		// set options from dialog
		m_num_additional_layers = dlg.GetNumALayers();
		m_num_layers = m_num_additional_layers + LAY_ADD_1;
		m_node_w = dlg.GetNodeWidth();
		m_polyline_w = dlg.GetPolyWidth();
		m_default_font = dlg.GetFont();
		m_view->m_polyline_width = abs(m_polyline_w);
		//
		// Reset masks
		Pages.m_netlist_page_mask[0] = 1;
		Pages.m_page_mirror_mask = 0;
		//
		m_via_hole_w = 0;
		for( int i=0; i<m_num_layers; i++ )
		{
			m_vis[i] = 1;
			m_dlist->SetLayerRGB( i, m_rgb[i][0], m_rgb[i][1], m_rgb[i][2] );
		}		
		
		// force redraw	
		m_project_open = TRUE;
		RECT r = rect(-100000000,-100000000,100000000,100000000);
		m_view->ViewAllElements(&r);
		m_view->InvalidateLeftPane();
		HWND WND = theApp.GetMainWnd()->GetSafeHwnd();
		UpdateWindow(WND);

		//
		// fix def pcb name
		if( Pages.m_pcb_names.GetSize() == 0 )
		{
			if( m_pcb_filename.Right(4) == ".cds" )
				Pages.m_pcb_names.Add( m_pcb_filename.Left( m_pcb_filename.GetLength() - 4 ) + ".fpc" );
			else
				Pages.m_pcb_names.Add( m_pcb_filename + ".fpc" );
			Pages.m_netlist_page_mask[0] = 1;
		}
		// force redraw of function key text
		m_view->m_cursor_mode = 999;
		m_view->SetCursorMode( CUR_NONE_SELECTED );

		// force redraw of window title
		m_project_modified = FALSE;
		m_project_modified_since_autosave = FALSE;
		m_auto_elapsed = 0;
		m_view->m_sel_mask = ~0;
		// save project
		OnFileSave();
		SaveOptions();
		//
		m_view->m_draw_layer = 0;
		m_view->m_fk_button_index = 0;
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		frm->m_timer = frm->SetTimer( TMR_FK_PRESS, 150, 0 );// FileNew
		//
		m_view->InvalidateLeftPane();
		m_view->Invalidate( FALSE );
	}
}

CString CFreePcbDoc::RunFileDialog( BOOL bOPEN, CString format )
{
#define BUFSIZE	_2540/10
	format = format.MakeLower();
	const int size = 8;
	const int MAX_ITERATORS = 200;
	HANDLE cmd = GlobalAlloc(GMEM_FIXED,32);
	memcpy( cmd, "", size );
	if( bOPEN )
	{	// reading
		if( format.Find("cds") >= 0 )
			memcpy( cmd, "cds_imp", size );
		else if( format.Find("net") >= 0 )
			memcpy( cmd, "net_imp", size );
		else if( format.Find("img") >= 0 )
			memcpy( cmd, "pic_imp", size );
		else if( format.Find("pas") >= 0 )
			memcpy( cmd, "pag_imp", size );
		else if( format.Find("doc") >= 0 )
			memcpy( cmd, "doc_imp", size );
		else return "";
	}
	else
	{	// saving
		if( format.Find("cds") >= 0 )
			memcpy( cmd, "cds_exp", size );
		else if( format.Find("net") >= 0 )
			memcpy( cmd, "net_exp", size );
		else if( format.Find("sav") >= 0 )
			memcpy( cmd, "grp_exp", size );
		else return "";
	}
	HWND WND = theApp.GetMainWnd()->GetSafeHwnd();
	do Sleep(100);  
		while(!OpenClipboard(WND));
	EmptyClipboard();
	SetClipboardData(CF_TEXT,cmd);
	CloseClipboard();
	CString command_str = m_app_dir + "\\VCL_Dialog\\VCL_Dialogs.exe";
	CString Path = "";// empty (default)
	char buffer[(int)BUFSIZE];
	int ret = GetFullPathName( command_str, BUFSIZE, buffer, NULL );
	if (ret)
	{
		HWND dlg = FindWindow(NULL,"File dialog");
		if( dlg )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"File dialog already running!":
				"Диалоговое окно «Файл» уже запущено!");
			return Path;
		}
		if ( (UINT)ShellExecute(	NULL,"open",command_str, NULL,buffer,SW_SHOWNORMAL) > 32 )
		{
			for( int i=0; FindWindow(NULL,"File dialog") == NULL && i<MAX_ITERATORS; i++ ){Sleep(100);}
			dlg = FindWindow(NULL,"File dialog");
			if(dlg)
			{
				//ShowWindow(WND,SW_HIDE);
				for( int st=0; dlg; st++ )
				{
					ret=0;
					Sleep(200);
					dlg = FindWindow(NULL,"Infobox");
					if( dlg )
					{
						COPYDATASTRUCT cd;
						cd.cbData = 1;
						cd.lpData = '\0';
						SendMessage( dlg, WM_COPYDATA,0, (LPARAM)&cd );
					}
					dlg = FindWindow(NULL,"File dialog");	
					if( dlg )
					{
						COPYDATASTRUCT cd;
						cd.cbData = 1;
						cd.lpData = '\0';
						SendMessage( dlg, WM_COPYDATA,0, (LPARAM)&cd );
						BringWindowToTop(dlg);
						UpdateWindow(WND);
						{
							MSG msg;
							PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
							if (msg.message)
							{
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
						}
					}
				}
				//ShowWindow(WND,SW_SHOW);
			}
			Sleep(200);
			SetForegroundWindow(WND);
			UpdateWindow(WND);
			if( m_view )
				m_view->Invalidate(false);
			if( !ret )
			{		
				CStdioFile pth;
				for( int ITERATOR=0; pth.Open( m_app_dir+"\\VCL_Dialog\\file_dialog.pth", CFile::modeReadWrite, NULL ) == 0; ITERATOR++ )
				{
					Sleep(50);
					if( ITERATOR > 50 )
					{
						AfxMessageBox(G_LANGUAGE == 0 ? 
							"Unable to open global configuration file \"file_dialog.pth\"\nYou installed FreePcb-2 in a protected folder, so the application will not work correctly. Remove the read-only attribute to continue.":
							"Невозможно открыть глобальный файл конфигурации «file_dialog.pth»\nВы установили ПлатФорм в защищенную папку, поэтому приложение не будет работать правильно. Снимите атрибут «только для чтения», чтобы продолжить.");
						return Path;
					}
				}
				pth.ReadString( Path );
				pth.Close();
			}
			else
				AfxMessageBox(G_LANGUAGE == 0 ? "File dialog is not connected!":"Диалоговое окно файла не подключено!");
		}
	}
	#undef BUFSIZE
	return Path;
}

void CFreePcbDoc::OnFileOpen()
{
	if( FileClose() == IDCANCEL )
		return;

	m_view->CancelSelection();
	InitializeNewProject();		// set defaults
	//
	CString filename = RunFileDialog( 1, "cds" ); 
	if ( filename.GetLength() )
	{
		// read project file
		FileOpen( filename );
	}
}

void CFreePcbDoc::OnFileAutoOpen( LPCTSTR fn )
{
	CString filename = fn;
	CString ext = filename.Right(3);
	if( ext.CompareNoCase("BMP") == 0 || 
		ext.CompareNoCase("PEG") == 0 || 
		ext.CompareNoCase("JPG") == 0 )
	{
		if( m_project_open == 0 )
			return;
		int sz = m_outline_poly->GetSize();
		m_outline_poly->SetSize(sz+1);
		m_outline_poly->GetAt(sz).SetDisplayList( m_dlist );
		m_outline_poly->GetAt(sz).Hide();
		m_view->SaveUndoInfoForOutlinePoly( sz, TRUE, m_undo_list );
		m_outline_poly->GetAt(sz).m_visible = 1;
		m_view->m_sel_id.Set( ID_POLYLINE, ID_GRAPHIC, sz, ID_CORNER, 0 );
		CPoint p;
		GetCursorPos( &p );
		CPoint pt = m_dlist->ScreenToPCB( p );
		RECT r_all;
		GetPolylineBounds( &r_all );
		int pic_h = r_all.top - r_all.bottom;
		pic_h = pic_h/12;
		pic_h = max( pic_h, 12*NM_PER_MM );
		if( m_view->m_polyline_layer == 0 )
			m_view->m_polyline_layer = LAY_FREE_LINE;
		m_outline_poly->GetAt(sz).Start( LAY_FREE_LINE, m_polyline_w, NM_PER_MIL, pt.x, pt.y, 0, &m_view->m_sel_id, NULL );
		m_outline_poly->GetAt(sz).AppendCorner( pt.x, pt.y+pic_h, 0, 0 );
		m_outline_poly->GetAt(sz).AppendCorner( pt.x+pic_h, pt.y+pic_h, 0, 0 );
		m_outline_poly->GetAt(sz).AppendCorner( pt.x+pic_h, pt.y, 0, 0 );
		m_outline_poly->GetAt(sz).Close();
		AttachBMP( 1, &filename );
		
		int i = m_outline_poly->GetAt(sz).GetPicture();
		CSize psize = m_dlist->GetTemplateSZ(i);
		double k = (double)psize.cx/((double)psize.cy+0.01);
		if( k > 100.0 || k < 0.01 )
			k = 1.0;
		int pic_w = (double)pic_h*k;
		m_outline_poly->GetAt(sz).SetX( 2, pt.x+pic_w );
		m_outline_poly->GetAt(sz).SetX( 3, pt.x+pic_w );
		m_outline_poly->GetAt(sz).RecalcRectC(0);
		RECT r = m_outline_poly->GetAt(sz).GetCornerBounds();
		m_dlist->MoveTemplate( &r, i, m_outline_poly->GetAt(sz).GetLayer() );
		m_outline_poly->GetAt(sz).Draw();
		m_view->CancelSelection();
		ProjectModified(TRUE);
		OnRangeCmds(NULL);
	}
	else if( ext.CompareNoCase("CDS") == 0 )
		FileOpen( fn );	
	else
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This format is not supported. CDS JPEG or BMP formats supported":
			"Этот формат не поддерживается. Поддерживаются форматы CDS JPEG или BMP");
}

// open project file, fn = full path to file
// if bLibrary = true, open library file and read footprints
// return TRUE if success
//
BOOL CFreePcbDoc::FileOpen( LPCTSTR fn )
{
	// if another file open, offer to save before closing
	if( FileClose() == IDCANCEL )
		return FALSE;		// file close cancelled
	
	// reset before opening new project
	m_view->CancelSelection();
	InitializeNewProject();		// set defaults

	// now read part renumbering file
	CString path = fn;

	// now open cds file
	CStdioFile pcb_file;
	int ok = pcb_file.Open( fn, CFile::modeRead, NULL );
	if( !ok )
	{
		// error opening project file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? "Unable to open file %s":"Невозможно открыть файл %s", fn);
		AfxMessageBox( mess );
		return FALSE;
	}
	try
	{
		// read project from file
		CString key_str;
		CString in_str;
		CArray<CString> p;
		int u = m_units;
		CMainFrame * pMainFr = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		if( !pMainFr ) 
		{
			pcb_file.Close();
			return FALSE;
		}
		//
		m_pcb_full_path = fn;
		CString str;
		// read options
		str.Format( "Reading options");
		pMainFr->DrawStatus( 3, &str );
		int m_org_changed = ReadOptions( &pcb_file, !m_system_colors );
		if( m_file_version > m_version )
		{
			CString mess;
			mess.Format(G_LANGUAGE == 0 ? 
				("Warning: the file version is %5.3f\n\nYou are running an earlier "PROGRAM_NAME" version %5.3f"):
				("Внимание: версия файла %5.3f\n\nВы используете более раннюю версию "PROGRAM_NAME" %5.3f"),
				m_file_version, m_version );
			mess += "\n\nErrors may occur\n\nClick on OK to continue reading or CANCEL to cancel";
			int ret = AfxMessageBox( mess, MB_OKCANCEL );
			if( ret == IDCANCEL )
			{
				pcb_file.Close();
				CString * err_str = new CString( "Reading project file failed: Cancelled by user" );//throw
				throw err_str;
			}
		}
		UpdateWindow((HWND)m_view->GetActiveWindow());

		// read Footprint Folder
		str.Format( "Reading footprint folder");
		pMainFr->DrawStatus( 3, &str );
		ReadFootprintFolder(this);

		// make path to library folder and index libraries
		if (m_full_lib_dir.GetLength() == 0)
		{
			CString fullpath;
			char full[MAX_PATH];
			fullpath = _fullpath(full, (LPCSTR)m_lib_dir, MAX_PATH);
			if (fullpath[fullpath.GetLength() - 1] == '\\')
				fullpath = fullpath.Left(fullpath.GetLength() - 1);
			//m_full_lib_dir = fullpath; ??
		}
		//
		int fpl = m_pcb_full_path.GetLength();
		int isep = m_pcb_full_path.ReverseFind('\\');
		if (isep == -1)
			isep = m_pcb_full_path.ReverseFind(':');
		if (isep == -1)
			ASSERT(0);		// unable to parse filename
		m_pcb_filename = m_pcb_full_path.Right(fpl - isep - 1);
		int fnl = m_pcb_filename.GetLength();
		m_path_to_folder = m_pcb_full_path.Left(m_pcb_full_path.GetLength() - fnl - 1);

		// read graphics
		str.Format( "Reading graphics");
		pMainFr->DrawStatus( 3, &str );
		ReadGraphics( &pcb_file );
		UpdateWindow((HWND)m_view->GetActiveWindow());
		//
		str.Format( "Complete");
		pMainFr->DrawStatus( 3, &str );
		pcb_file.Close();
		//
		if( m_name.GetLength() == 0 )   
		{
			m_name = m_pcb_filename;
		}
		if( m_name.Compare( m_pcb_filename ) ) 
		{
			m_name = m_pcb_filename;
			AfxMessageBox(G_LANGUAGE == 0 ? 
				("The project file has been renamed.\n"\
				"You can rename "\
				"a project through the OPTIONS dialog box."):
				("The project file has been renamed.\n"\
				"You can rename "\
				"a project through the OPTIONS dialog box."), MB_ICONINFORMATION);
		}
		if( m_get_app_folder.Compare( m_app_dir ) &&
			m_pcb_filename.Find("bom.") == -1 &&
			m_pcb_filename.Find("tag.") == -1
			)
		{
			CString path = m_app_dir + "\\Backups";
			struct _stat buf;
			int err = _stat( path+"\\"+m_pcb_filename, &buf );
			if( err == 0 )
			{
				int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
					"This project has been changed in another version of the program. To restore a project that was edited in the current version of the program, open the BackUps folder. This folder contains the most recent versions of files. Open this folder?":
					"Этот проект был изменен в другой версии программы (другой exe-файл, запущенный из другого места). Чтобы восстановить проект, на момент редактирования его именно в текущей версии программы, откройте папку BackUps. Эта папка содержит самые последние версии файлов. Открыть эту папку?", MB_OKCANCEL);
				if( ret == IDOK )
				{
					ShellExecute(	NULL,"open",path,NULL,path,SW_SHOWNORMAL);
				}
			}
		}
		m_project_open = TRUE;
		theApp.AddMRUFile( &m_pcb_full_path );
		// now set layer visibility
		for( int i=0; i<m_num_layers; i++ )
		{
			m_dlist->SetLayerRGB( i, m_rgb[i][0], m_rgb[i][1], m_rgb[i][2] );
			m_dlist->SetLayerVisible( i, m_vis[i] );
		}
		
		// force redraw of function key text
		CheckBOM();
		m_view->m_cursor_mode = 999;
		m_view->SetCursorMode( CUR_NONE_SELECTED );
		m_view->ShowActiveLayer(m_num_additional_layers);	
		m_view->InvalidateLeftPane();
		ProjectModified( FALSE );

		// set mapping
		if( m_org_changed )
		{
			CRect screen_r;
			m_view->GetWindowRect( &screen_r );
			m_view->m_client_r.right = screen_r.right-screen_r.left;
			m_view->m_client_r.bottom = screen_r.bottom-screen_r.top;
			m_dlist->SetMapping( &m_view->m_client_r, 
								 &screen_r, 
								 m_view->m_left_pane_w, 
								 m_view->m_bottom_pane_h, 
								 m_view->m_pcbu_per_pixel, 
								 m_view->m_org_x, 
								 m_view->m_org_y );
		}
		else
		{
			m_view->OnViewAllElements();
		}
		m_auto_elapsed = 0;

		CDC * pDC = m_view->GetDC();
		m_view->OnDraw( pDC );
		m_view->ReleaseDC( pDC );

		m_view->InvalidateLeftPane();		
		GetGerberPathes(fn);
		

		// fix pcb names
		Pages.CheckPCBNames( &m_path_to_folder );
		
		// creating related files
		int rf = path.ReverseFind( '\\' );
		if( rf > 0 )
		{
			// create renumbering file
			int RENUMB = -1;
			m_part_renumber_pos = 0;
			for( int ipcb=0; ipcb<Pages.m_pcb_names.GetSize(); ipcb++ )
			{
				PartRenumber[ipcb].RemoveAll();
				int rf = m_pcb_full_path.ReverseFind( '\\' );
				CString report = m_pcb_full_path.Left(rf) + freeasy_netlist + Pages.m_pcb_names[ipcb] + ".txt";
				CString in_str="";
				if( Check_Txt_Msg( &report, &in_str ) )
				{
					if( RENUMB == -1 )
					{
						CString rnmb = m_pcb_full_path.Left(rf) + freeasy_netlist + Pages.m_pcb_names[ipcb] + ".rnmb";
						CStdioFile rnmb_file;
						ok = rnmb_file.Open( rnmb, CFile::modeRead, NULL );
						if( ok )
						{	
							CString in_str;
							rnmb_file.ReadString( in_str );
							rnmb_file.Close();
							if( in_str.GetLength() )
								RENUMB = ipcb;
						}
					}
					// get message
					if( in_str.Compare( FILE_PROTECTED ) == 0 )
					{
						setbit( m_protection, ipcb );
						int getpcb = Pages.GetPcbIndex( Pages.GetActiveNumber() );
						if( getpcb == ipcb )
						{
							m_view->m_protect = 1;
							m_view->SetFKText( m_view->m_cursor_mode );
							ProjectModified();
						}
					}
					else if( in_str.GetLength() )
					{
						int pg = Pages.ReturnPageByPCB( &Pages.m_pcb_names[ipcb] ) + 1;
						if( pg > 0 )
						{
							CString s;
							s.Format(G_LANGUAGE == 0 ? 
								"The schematic diagram on page %d for the %s PCB-project has been changed.\n(%s)":
								"Принципиальная схема на странице %d для проекта печатной платы %s была изменена.\n(%s)", pg, Pages.m_pcb_names[ipcb], in_str);
							AfxMessageBox( s );
						}
					}
					else if( getbit( m_protection, AUTOPROTECT ) )
					{
						CDlgProtection dlg;
						dlg.Initialize( this );
						if( dlg.FindArchive( ipcb ) )
							Pages.Protection( ipcb );
					}
				}
			}
			if( RENUMB >= 0 )
			{
				int pg = Pages.ReturnPageByPCB( &Pages.m_pcb_names[RENUMB] ) + 1;
				CString s;
				s.Format(G_LANGUAGE == 0 ? 
					("Important message from page %d:\n\nAfter renumbering the parts, you forgot "\
					"to download the %s.net netlist into FreePcb-2. You can continue to "\
					"work and download it later, but it is best to always update the "\
					"netlist immediately after any changes to the schematic file."):
					("Важное сообщение со страницы %d:\n\nПосле перенумерации деталей вы забыли "\
					"загрузить список соединений %s.net в редактор ПлатФорм. Вы можете продолжить "\
					"работу и загрузить его позже, но лучше всегда обновлять список соединений "\
					"сразу после любых изменений в файле схемы."), pg, Pages.m_pcb_names[RENUMB]);
				AfxMessageBox(	s, MB_ICONWARNING );
			}
		}
		
		// copy datasheets
		CComponentNoteExt dlg;
		dlg.Sync( this );

		/// At the very end we will install the title
		m_window_title = CDS_HEADER + m_pcb_filename;
		EnableMenuItems();
		bNoFilesOpened = FALSE;
		return TRUE;
	}
	catch( CString * err_str )
	{
		// parsing error
		AfxMessageBox( *err_str );
		delete err_str;
		CDC * pDC = m_view->GetDC();
		m_view->OnDraw( pDC );
		m_view->ReleaseDC( pDC );
		return FALSE;
	}
}

void CFreePcbDoc::OnFileClose()
{
	FileClose();
}

// return IDCANCEL if closing cancelled by user
//
int CFreePcbDoc::FileClose()
{
	if( m_project_open )
	{
		if( m_get_app_folder.Compare( m_app_dir ) == 0 )
		{
			CString BackUps = m_app_dir + "\\Backups";
			FileSave( &BackUps, &m_pcb_filename, NULL, NULL, FALSE );
		}
		if( m_project_modified )
		{
			int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
				"Project modified, save it?":
				"Проект изменен, сохранить его?", MB_YESNOCANCEL);
			if( ret == IDCANCEL )
				return IDCANCEL;
			else if( ret == IDYES )
			{
				OnFileSave();
				if( m_project_modified )
					return 0; // (error)
			}
		}
		// copy datasheets
		CComponentNoteExt dlg;
		dlg.Sync( this );
		bNoFilesOpened = TRUE;
	}
	m_view->CancelSelection();	

	// remove favorites
	for( int i=0; i<MAX_FAVORITES; i++ )
		m_view->m_favorite_poly[i] = -1;
	for( int i=0; i<MAX_FAVORITES; i++ )
		m_view->m_favorite_page[i] = 0;

	// destroy existing project
	ResetUndoState();

	// remove all pages
	for( int pg=Pages.GetNumPages()-1; pg>0; pg-- )
	{
		Pages.Clear(pg);
		DeletePage(pg);
	}
	Pages.DeleteVirtualPages();
	Pages.ClearAll();
	Pages.RemoveAll();
	CString PG = "Page 1";
	Pages.AddNew(&PG);
	SwitchToPage(0);

	//
	if( m_view->m_dlg_partlist )
	{
		m_view->m_dlg_partlist->DestroyWindow();
		delete m_view->m_dlg_partlist;
		m_view->m_dlg_partlist = NULL;
	}
	if( m_view->m_dlg_compare )
	{
		if( m_view->m_dlg_compare->DLG_CREATED )
			m_view->m_dlg_compare->DestroyWindow();
		delete m_view->m_dlg_compare;
		m_view->m_dlg_compare = NULL;
	}

	// remove merges, clip_outline_poly
	m_mlist->Clear();
	clip_mlist->Clear();
	m_ref_lists->Clear();
	clip_outline_poly.RemoveAll();	
	for( int i=0; i<MAX_REF_LISTS; i++ )
		OnReflistRemoveItem(3);	// 3 - number of first menu	item	
	
	m_view->Invalidate( FALSE );// (doubtful)
	m_project_open = FALSE;
	ProjectModified( FALSE );
	m_auto_elapsed = 0;
	// force redraw
	m_view->m_cursor_mode = 999;
	CDC * pDC = m_view->GetDC();
	m_view->InvalidateLeftPane();
	m_view->OnDraw( pDC );
	m_view->ReleaseDC( pDC );
	m_view->SetCursorMode( CUR_NONE_SELECTED );
	m_window_title = (CString)CDS_HEADER+(G_LANGUAGE == 0 ? "no project open":"конструктор схем");
	DisableMenuItems();

	return IDOK;
}

void CFreePcbDoc::OnFileSave() 
{
	Pages.RemoveEmptyMergers( m_mlist );
	m_view->m_seg_clearance = m_seg_clearance + _2540*2;
	int err = FileSave( &m_path_to_folder, &m_pcb_filename, &m_path_to_folder, &m_pcb_filename );
	if( !err )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Unable to save file. Write protection.":"Невозможно сохранить файл. Защита от записи.");
	}
	else if( err == 1 )
	{
		m_dlg_log->RemoveAll();
		ProjectModified( FALSE );
		// now redraw polylines
		m_view->RedrawLayers(); // OnFileSave
		CheckBOM();
		ResetUndoState();
		clip_outline_poly.RemoveAll();
		Pages.InvalidatePcbIndexes();
		bNoFilesOpened = FALSE;
	}
}

// Autosave
//
BOOL CFreePcbDoc::AutoSave()
{
	CString str;
	CString auto_folder = m_app_dir + "\\Autosave";
	struct _stat s;
	int err = _stat( auto_folder, &s );//ok
	if( err )
	{
		err = _mkdir( auto_folder );
		if( err )
		{
			m_project_modified_since_autosave = 0;
			str.Format(G_LANGUAGE == 0 ? 
				"Unable to create autosave folder \"%s\"":
				"Не удалось создать папку автосохранения \"%s\"", auto_folder);
			AfxMessageBox( str, MB_OK );
			return FALSE;
		}
	}
	CFileFind finder;
	CString list_str = "";
	CTime time;
	time_t bin_time;
	time_t max_time = 0;
	int max_suffix = 0;
	if( _chdir( auto_folder ) != 0 )
	{
		m_project_modified_since_autosave = 0;
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? 
			"Unable to open autosave folder \"%s\"":
			"Невозможно открыть папку автосохранения \"%s\"", auto_folder);
		AfxMessageBox( mess );
	}
	else
	{
		CString first_f = "\\??.cds";
		CString search_str = auto_folder + first_f;
		BOOL bWorking = finder.FindFile( search_str );
		if( !bWorking )
		{
			first_f = "01.cds";
			FileSave( &auto_folder, &first_f, NULL, NULL, FALSE );
		}
		else while (bWorking)
		{
			bWorking = finder.FindNextFile();
			CString fn = finder.GetFileName();
			if( !finder.IsDirectory() )
			{
				// found a file
				int suffix = atoi( fn.Left(2) );
				finder.GetLastWriteTime( time );
				bin_time = time.GetTime();
				if( bin_time > max_time )
				{
					max_time = bin_time;
					max_suffix = suffix;
				}
			}
		}
	}
	if( max_suffix )
	{
		CString new_file_name;
		int new_suffix = max_suffix%25 + 1;
		if( new_suffix < 10 )
			new_file_name.Format( "0%d.cds", new_suffix );
		else
			new_file_name.Format( "%d.cds", new_suffix );
		FileSave( &auto_folder, &new_file_name, NULL, NULL, FALSE );
		m_project_modified_since_autosave = FALSE;		
	}
	finder.Close();
	return TRUE;
}

// save project file
// make backup if the new file has the same path and filename as the old file
// returns TRUE if successful, FALSE if fails
//
int CFreePcbDoc::FileSave( CString * folder, CString * filename, 
						   CString * old_folder, CString * old_filename,
						   BOOL bBackup ) 
{
	if( !m_project_open )
		return FALSE;

	// write project file
	int err = 0;
	CString full_path = *folder;
	if( bBackup )
	{
		if( m_project_modified )
		{
			m_get_app_folder = m_app_dir; // (project modified, so enable backups)
			m_view->RemoveOrphanMergeNames();
			int ipg = -1;
			int itxt = -1;
			while( CText * t = Pages.GetNextAttr( index_desc_attr, &itxt, &ipg ) )
				if( t->m_tl )
					if( t->m_str.GetLength() > 12 ) // there cannot be less than 12 characters ...
					{
						t->m_tl->TimeUpdate(t);
						m_view->DrawPATH(t,ipg);
					}
		}
		CString full_path_b = *folder + "\\related_files";
		struct _stat buf;
		err = _stat( full_path_b, &buf );//ok
		if( err )
		{		
			err = _mkdir( full_path_b );
			if( err )
			{
				CString str;
				str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"":"Невозможно создать папку \"%s\"", full_path_b);
				AfxMessageBox( str, MB_OK );
				old_folder = NULL;
			}
		}
		//
		full_path_b += "\\backup";
		err = _stat( full_path_b, &buf );//ok
		if( err )
		{
			CString str;
			err = _mkdir( full_path_b );
			if( err )
			{
				str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"" : "Невозможно создать папку \"%s\"", full_path_b );
				AfxMessageBox( str, MB_OK );
				old_folder = NULL;
			}
		}
		//
		
		// create folder of netlist
		m_pcb_full_path = *folder + "\\" + *filename;
		CString nlist_folder = *folder + freeasy_netlist;
		nlist_folder = nlist_folder.Left( nlist_folder.GetLength() - 1 );
		//
		//
		err = _stat( nlist_folder, &buf );
		if( err )
		{
			CString str;
			err = _mkdir( nlist_folder );
			if( err )
			{
				str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"" : "Невозможно создать папку \"%s\"", nlist_folder );
				AfxMessageBox( str, MB_OK );
			}
		}
		// create folder for pictures
		CString pic_folder = m_pcb_full_path.Left(m_pcb_full_path.ReverseFind('\\')) + "\\related_files\\pictures";	
		err = _stat( pic_folder, &buf );//ok
		if( err )
		{
			CString str;
			err = _mkdir( pic_folder );
			if( err )
			{
				str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"" : "Невозможно создать папку \"%s\"", pic_folder );
				AfxMessageBox( str, MB_OK );
			}
		}
		int mem_pg = Pages.GetActiveNumber();
		for( int ipcb=0; ipcb<Pages.m_pcb_names.GetSize(); ipcb++ )
		{
			if( m_pcb_modified[ipcb] == 0 )
				continue;
			int new_pg = Pages.ReturnPageByPCB( &Pages.m_pcb_names[ipcb] );
			SwitchToPage( new_pg );
			// write message
			CString messg = m_netlist_full_path + ".txt";
			CString instr="";
			//
			if( Check_Txt_Msg( &messg, &instr ) )
			{	
				if( instr.Left(instr.GetLength()) == NETLIST_WARNING )
				{
					AfxMessageBox(G_LANGUAGE == 0 ? 
						(instr+"\n\nFile save rejected. First, save the PCB-file in the PCB-editor or close it without saving"):
						(instr + "\n\nСохранение файла отклонено. Сначала сохраните файл печатной платы в редакторе ПлатФорм или закройте его без сохранения"), MB_ICONERROR);
					m_view->OnRangeCmds(NULL);
					return 2;
				}
				else //if( instr.Left(instr.GetLength()) != FILE_PROTECTED )
				{
					CStdioFile m_file;
					err = m_file.Open( LPCSTR(messg), CFile::modeCreate | CFile::modeWrite, NULL );
					if( err )
					{
						m_file.WriteString("Warning! The design of the circuit was changed, but the netlist was not exported.");
						m_file.Close();
					}
				}
			}
			
			// write renumbering
			if( bBackup )
			{
				CStdioFile rnmb_file;
				CString renumb = m_netlist_full_path + ".rnmb";
				int notempty = rnmb_file.Open( LPCSTR(renumb), CFile::modeRead, NULL );
				//
				int sv_pos = PartRenumber[ipcb].GetSize();
				if( notempty )
				{
					CString in_str;
					for( int ins=0; rnmb_file.ReadString( in_str ); )
						if( in_str.GetLength() > 5 )
							PartRenumber[ipcb].InsertAt( ins++, in_str );
					rnmb_file.Close();
				}
				if( PartRenumber[ipcb].GetSize() )
				{
					err = rnmb_file.Open( LPCSTR(renumb), CFile::modeCreate | CFile::modeWrite, NULL );
					if( err )
					{
						for( int ii=0; ii<PartRenumber[ipcb].GetSize(); ii++ )
							rnmb_file.WriteString( PartRenumber[ipcb].GetAt(ii) + "\n" );
						// close
						rnmb_file.Close();
						PartRenumber[ipcb].RemoveAll();
					}
					else if( PartRenumber[ipcb].GetSize() - sv_pos )
						PartRenumber[ipcb].RemoveAt( 0, PartRenumber[ipcb].GetSize() - sv_pos );
				}	
			}
		}
		// switch to old page
		SwitchToPage( mem_pg );
		//
		// write backup file
		CString backup = full_path_b + "\\" + *filename + ".b";
		// see if we need to make a backup file
		if( old_folder != NULL && old_filename != NULL )
		{
			if( bBackup && *folder == *old_folder && *filename == *old_filename )
			{
				// rename file
				remove( backup );
				CopyFile( full_path + "\\" + *filename, backup, 1 );
			}
		}
	}
	// write cds file
	CStdioFile pcb_file;
	CString write = *folder + "\\" + *filename;
	err = pcb_file.Open( LPCSTR(write), CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening file
		return FALSE;
	}
	else
	{
		// write project to file
		try
		{
			WriteOptions( &pcb_file, 0 );
			WriteOutlinesPoly( &pcb_file, FALSE );
			pcb_file.WriteString( "[end]\n" );
			pcb_file.Close();
			theApp.AddMRUFile( &m_pcb_full_path );
			bNoFilesOpened = FALSE;
			m_auto_elapsed = 0;
		}
		catch( CString * err_str )
		{
			// error
			AfxMessageBox( *err_str );
			delete err_str;
			CDC * pDC = m_view->GetDC();
			m_view->OnDraw( pDC ) ;
			m_view->ReleaseDC( pDC );
			return FALSE;
		}
	}
	return TRUE;
}



void CFreePcbDoc::OnFileSaveAs() 
{
	// force old-style file dialog by setting size of OPENFILENAME struct
	CWnd * WND = theApp.GetMainWnd();
	CString filename = RunFileDialog( 0, "cds" ); 
	if ( filename.GetLength() )
	{
		if( filename.Right(3).MakeLower() != "cds" )
		{
			CString S = filename;
			filename.Format( "%s.cds", S );
		}
		// get new filename and folder
		CString new_pathname = filename;
		CString new_folder = filename.Left( filename.ReverseFind('\\') );
		int nfl = new_folder.GetLength();
		CString new_filename = filename.Right( filename.GetLength()-nfl-1 );
		// write project file
		BOOL ok = FileSave( &new_folder, &new_filename, &m_path_to_folder, &m_pcb_filename );
		if( ok )
		{
			// update member variables, MRU files and window title
			m_pcb_filename = new_filename;
			m_pcb_full_path = new_pathname;
			m_path_to_folder = new_folder;
			theApp.AddMRUFile( &m_pcb_full_path );
			m_window_title = CDS_HEADER + m_pcb_filename;
			CWnd* pMain = AfxGetMainWnd();
			pMain->SetWindowText( m_window_title );
			ProjectModified( FALSE );
		}
		else
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "Unable to save file. Write protection.":"Невозможно сохранить файл. Защита от записи.");
		}
	}
}

// write outline to file
//
// throws CString * exception on error
//

void CFreePcbDoc::WriteOutlinesPoly( CStdioFile * file, BOOL SEL_ONLY )
{
	int FAVORITE = 9;
	CString line;
	try
	{
		int LEGAL_MERGE = 0;
		for (int i = 0; i < m_mlist->GetSize(); i++)
		{
			if (i >= 32)
				break;
			CString s = m_mlist->GetStr(i);
			if (s.Find(PCBV) == -1)
				setbit(LEGAL_MERGE, i);
		}
		// save num current page
		int curP = Pages.GetActiveNumber();
		// outlines
		line.Format( "[graphics]\n\n" );
		file->WriteString( line );

		// parts
		for( int pg=0; pg<Pages.GetNumPages(); pg++ )
		{
			CString name;
			Pages.GetPageName(pg,&name);
			if( pg == 0 )
			{
				line.Format("rename_page: \"%s\"\r\n", name); 
				file->WriteString( line );
			}
			else
			{
				line.Format("add_new_page: \"%s\"\r\n", name); 
				file->WriteString( line );
			}
			SwitchToPage(pg);
			for( int isrc=0; isrc<m_dlist->GetNumSources(); isrc++ )
			{
				line.Format( "bmp_src: \"%s\"\r\n", m_dlist->GetSource(isrc) );
				file->WriteString( line );
			}
			CArray<CPolyLine> * smc = m_outline_poly;
			// MARK FAVORITES
			BOOL X = 0;
			for( int i=0; i<MAX_FAVORITES; i++ )
				if( m_view->m_favorite_page[i] == pg )
					if( m_view->m_favorite_poly[i] >= 0 && m_view->m_favorite_poly[i] < smc->GetSize() )
					{
						if( X == 0 )
							m_view->MarkAllOutlinePoly(0,-1);
						X = 1;
						smc->GetAt( m_view->m_favorite_poly[i] ).SetUtility( FAVORITE );
					}
			int it=-1;
			for( CText * part = Attr->m_Reflist->GetNextText(&it); part; part = Attr->m_Reflist->GetNextText(&it) )
			{
				if( part->m_polyline_start == -1 )
					AttributeIndexing( part, index_part_attr ); // m_polyline_start == -1
				if( part->m_polyline_start == -1 )
					continue;
				CPolyLine * po = &smc->GetAt(part->m_polyline_start);
				///if( po->GetNumCorners() < 2 )
				///	AfxMessageBox("Invalid number of corners");
				if( SEL_ONLY && po->GetSel() == 0 )
					continue;
				CText * ref = NULL;
				if( po->pAttr[index_part_attr] )
				{
					// num pins
					int np = GetNumAllPins( po->pAttr[index_part_attr] );
					line.Format( "n_pins: %d\n", np );
					file->WriteString( line );
					// rectangle
					RECT r = GetPartBounds( po->pAttr[index_part_attr], NULL );
					line.Format( "rectangle: %d %d %d %d\n", r.left, r.bottom, r.right, r.top );
					file->WriteString( line );
					// ref
					CText * t = po->pAttr[index_part_attr];
					ref = t;
					line.Format( "ref_des: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
						t->m_x, 
						t->m_y, 
						LAY_PART_NAME, 
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width, 
						t->m_merge, 
						t->m_submerge, 
						t->m_font_number,
						t->m_pdf_options );
					file->WriteString( line );
				}
				else
					continue;
				if( po->Check( index_foot_attr ) )
				{
					CText * t = po->pAttr[index_foot_attr];
					line.Format( "footprint_name: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
						t->m_x, 
						t->m_y, 
						LAY_FOOTPRINT,
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width,  
						t->m_merge, 
						t->m_submerge,
						t->m_font_number,
						t->m_pdf_options );
					file->WriteString( line );
				}
				if( po->Check( index_value_attr) )
				{
					CText * t = po->pAttr[index_value_attr];
					line.Format( "part_value: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
						t->m_x, 
						t->m_y, 
						LAY_PART_VALUE, 
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width, 
						t->m_merge, 
						t->m_submerge,
						t->m_font_number,
						t->m_pdf_options );
					file->WriteString( line );
				}
				for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
				{
					po = &smc->GetAt(get);
					if( po->m_visible == 0 )
					{
						// Must be indexed
						// To exclude deleted lines
						AttributeIndexing( part, index_part_attr ); // necessarily !!!
						if( part->m_polyline_start < 0 )
							break;
						get = part->m_polyline_start;
						po = &smc->GetAt(get);
					}
					int l = po->GetLayer();
					int ncr = po->GetNumCorners();
					if( ncr < 2 )
					{
						continue;
					}
					int src = m_dlist->GetSourceIndex( po->GetPicture() );// get index via GetSource
					int inv = 0;
					m_dlist->GetTemplate( po->GetPicture(), NULL, NULL, &inv );
					if( po->GetClosed() == 0 )
					{
						line.Format( "polyline: %d %d %d %d %d %d %d %d\n",  
							po->GetNumCorners(), 
							po->GetHatch(), 
							po->GetLayer(), 
							po->GetW(), 
							po->GetMerge(), 
							po->GetMerge(TRUE), 
							src,
							inv);
						file->WriteString( line );
						int end_cont = po->GetContourEnd(0);
						for( int icor=0; icor<=end_cont; icor++ )
						{
							line.Format( "  corner: %d %d %d %d %d\n", icor+1,
								po->GetX( icor ),
								po->GetY( icor ),
								po->GetSideStyle( min( icor,end_cont-1 )),
								po->Node[icor] );
							file->WriteString( line );
						}
					}
					else
					{
						line.Format( "outline: %d %d %d %d %d %d %d %d\n", 
							po->GetNumCorners(), 
							po->GetHatch(), 
							po->GetLayer(), 
							po->GetW(), 
							po->GetMerge(), 
							po->GetMerge(TRUE),
							src,
							inv );
						file->WriteString( line );
						for( int icor=0; icor<ncr; icor++ )
						{
							line.Format( "  corner: %d %d %d %d %d\n", icor+1,
								po->GetX( icor ),
								po->GetY( icor ),
								po->GetSideStyle( icor ),
								po->GetNumContour( icor )
								);
							file->WriteString( line );
						}
					}
					if( po->Check( index_pin_attr ) ) 
					{
						CText * t = po->pAttr[index_pin_attr];
						line.Format( "pin_name: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
							t->m_x, 
							t->m_y, 
							t->m_layer, 
							t->m_angle, 
							t->m_font_size, 
							t->m_stroke_width, 
							t->m_merge, 
							t->m_submerge,
							t->m_font_number,
							t->m_pdf_options );
						file->WriteString( line );
					}
					if( po->Check( index_desc_attr ) )
					{
						CText * t = po->pAttr[index_desc_attr];
						line.Format( "description: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
							t->m_x, 
							t->m_y, 
							t->m_layer, 
							t->m_angle, 
							t->m_font_size, 
							t->m_stroke_width,  
							t->m_merge, 
							t->m_submerge,
							t->m_font_number,
							t->m_pdf_options );
						file->WriteString( line );
					}
					if( po->Check( index_net_attr ) )
						if( po->pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
						{
							CText * t = po->pAttr[index_net_attr];
							line.Format( "net_name: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
								t->m_x, 
								t->m_y, 
								LAY_NET_NAME, 
								t->m_angle, 
								t->m_font_size, 
								t->m_stroke_width, 
								t->m_merge, 
								t->m_submerge,
								t->m_font_number,
								t->m_pdf_options );
							file->WriteString( line );
						}
					file->WriteString( "\n" );
				}
				if( ref )
					file->WriteString( "part_end\n" );
				file->WriteString( "\n" );
			}	
			// polylines
			for( int ip=0; ip<smc->GetSize(); ip++ )
			{
				CPolyLine * po = &smc->GetAt(ip);
				if( po->Check( index_part_attr ) )
					continue;
				if( po->m_visible == 0 )
					continue;
				if( po->GetNumCorners() < 2 )
				{
					AfxMessageBox(G_LANGUAGE == 0 ? "Invalid number of corners":"Неверное количество вершин");
					continue;
				}
				if (po->GetMerge() >= 0)
					if (getbit(LEGAL_MERGE, po->GetMerge()) == 0)
						continue;
				if( SEL_ONLY && po->GetSel() == 0 )
					continue;
				int l = po->GetLayer();
				int ncr = po->GetNumCorners();
				if( ncr < 2 )
				{
					continue;
				}
				int src = m_dlist->GetSourceIndex( po->GetPicture() );// get index via GetSource
				int inv = 0;
				m_dlist->GetTemplate( po->GetPicture(), NULL, NULL, &inv );
				if( po->GetClosed() == 0 )
				{
					line.Format( "polyline: %d %d %d %d %d %d %d %d %d\n",  
						po->GetNumCorners(), 
						po->GetHatch(), 
						po->GetLayer(), 
						po->GetW(), 
						po->GetMerge(), 
						po->GetMerge(TRUE), 
						src,
						inv, 
						po->GetUtility() );
					file->WriteString( line );
					int end_cont = po->GetContourEnd(0);
					for( int icor=0; icor<=end_cont; icor++ )
					{
						line.Format( "  corner: %d %d %d %d %d\n", icor+1,
							po->GetX( icor ),
							po->GetY( icor ),
							po->GetSideStyle( min( icor,end_cont-1 )), 
							po->Node[icor] );
						file->WriteString( line );
					}
				}
				else
				{
					line.Format( "outline: %d %d %d %d %d %d %d %d %d\n", 
						po->GetNumCorners(), 
						po->GetHatch(), 
						po->GetLayer(), 
						po->GetW(), 
						po->GetMerge(), 
						po->GetMerge(TRUE),
						src,
						inv,
						po->GetUtility() );
					file->WriteString( line );
					for( int icor=0; icor<ncr; icor++ )
					{
						line.Format( "  corner: %d %d %d %d %d\n", icor+1,
							po->GetX( icor ),
							po->GetY( icor ),
							po->GetSideStyle( icor ),
							po->GetNumContour( icor )
							);
						file->WriteString( line );
					}
				}
				if( po->Check( index_pin_attr ) ) 
				{
					CText * t = po->pAttr[index_pin_attr];
					line.Format( "pin_name: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
						t->m_x, 
						t->m_y, 
						t->m_layer, 
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width, 
						t->m_merge, 
						t->m_submerge,
						t->m_font_number,
						t->m_pdf_options );
					file->WriteString( line );
				}
				if( po->Check( index_desc_attr ) )
				{
					CText * t = po->pAttr[index_desc_attr];
					line.Format( "description: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
						t->m_x, 
						t->m_y, 
						t->m_layer, 
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width,
						t->m_merge, 
						t->m_submerge,
						t->m_font_number,
						t->m_pdf_options );
					file->WriteString( line );
				}
				if( po->Check( index_net_attr ) )
					if( po->pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
					{
						CText * t = po->pAttr[index_net_attr];
						line.Format( "net_name: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", t->m_str,
							t->m_x, 
							t->m_y, 
							LAY_NET_NAME, 
							t->m_angle, 
							t->m_font_size, 
							t->m_stroke_width, 
							t->m_merge, 
							t->m_submerge,
							t->m_font_number,
							t->m_pdf_options );
						file->WriteString( line );
					}
				file->WriteString( "\n" );
			}
		}
		SwitchToPage(curP);
	}
	catch( CFileException * e )
	{
		CString * err_str = new CString;//throw
		if( e->m_lOsError == -1 )
			err_str->Format( "File error: %d\n", e->m_cause );
		else
			err_str->Format( "File error: %d %ld (%s)\n", 
				e->m_cause, e->m_lOsError, _sys_errlist[e->m_lOsError] );
		*err_str = "CFreePcbDoc::WriteOutlinesPoly()\n" + *err_str;
		throw err_str;
	}
}




CString CFreePcbDoc::ReadGraphics( CStdioFile * pcb_file, CArray<CPolyLine> * ssm, int dx, int dy, RECT * gRECT, int i_page, long long LinkToPart, int mINDEX, BOOL ADD_SRC )
{
	int err, pos, np;
	CArray<CString> p;
	CString in_str, key_str, RETSRC="";
	int last_side_style = CPolyLine::STRAIGHT;
	CArray<CPolyLine> * op;
	if( ssm == NULL )
		op = m_outline_poly;
	else
		op = ssm;
	
	try
	{
		// find beginning of [outlines] section
		CRect sv_rect = m_view->m_client_r;
		int sv_pane1 = m_view->m_left_pane_w;
		int sv_pane2 = m_view->m_bottom_pane_h;
		double sv_pixel = m_view->m_pcbu_per_pixel;
		double sv_org1 = m_view->m_org_x;
		double sv_org2 = m_view->m_org_y;
		CText * part=NULL;
		CText * value=NULL;
		CText * footprint=NULL;
		int m_p_start = -1;
		int units = NM;
		int gl_ic = -1;
		if( ssm )
			gl_ic = ssm->GetSize()-1;
		int m_EN = 0;
		if( mINDEX == -1 )
			m_EN = 1;
		int page_num = -1;
		CArray<CString> src_new;
		src_new.SetSize(0);
		//
		BOOL bReadingM = (mINDEX >= 0 || m_mlist->GetSize());
		// get data
		while( 1 )
		{
			err = pcb_file->ReadString( in_str );
			if( !err )
			{
				op->SetSize(gl_ic+1);
				return RETSRC;
			}
			in_str.Trim();
			//------------------------------------
			//------------------------------------
			//------------------------------------
			if( in_str.GetLength() == 0 ) // ignore empty string
				continue;
			//------------------------------------
			// normal return
			//------------------------------------
			if( in_str.Left(1) == "[" )
			{				
				op->SetSize(gl_ic+1);
				part = NULL;
				value = NULL;
				footprint = NULL;
				do
				{
					if( in_str == "[graphics]" )
						break;
					if( in_str == "[footprints]" )
						break;
					if( in_str == "[end]" )
					{		
						// now redraw polylines
						if( ssm == NULL )
						{
							m_view->RedrawLayers(); // ReadGraphics
							//if( Pages.GetActiveNumber() ) n.u.
							{
								m_view->ViewAllElements(NULL);
								if( m_current_page < Pages.GetNumPages() )
								{
									SwitchToPage(m_current_page, TRUE);
									CRect screen_r;
									m_view->GetWindowRect( &screen_r );
									m_view->m_client_r = sv_rect; 
									m_view->m_left_pane_w = sv_pane1;
									m_view->m_bottom_pane_h = sv_pane2;
									m_view->m_pcbu_per_pixel = sv_pixel;
									m_view->m_org_x = sv_org1;
									m_view->m_org_y = sv_org2;
									m_dlist->SetMapping( &sv_rect, &screen_r, sv_pane1, sv_pane2, sv_pixel, sv_org1, sv_org2 );
								}
								else
									SwitchToPage(0, TRUE);
							}
						}
						return RETSRC;
					}
					err = pcb_file->ReadString( in_str );
					in_str.Trim();
				} while( err );
				if( !err )
				{
					// error reading pcb file
					CString mess;
					mess.Format(G_LANGUAGE == 0 ? "Unable to find [end] section in file":"Не удалось найти [конец] раздела в файле");
					AfxMessageBox( mess );
					return RETSRC;
				}
			}
			//------------------------------------
			//------------------------------------
			//------------------------------------
			np = ParseKeyString( &in_str, &key_str, &p );
			//
			//------------------------------------
			// picture pathes
			//------------------------------------
			if( np >= 2 && key_str == "bmp_src" )
			{
				int fi2 = p[0].ReverseFind('\\');
				//if( ssm )
				//{
					int fi = pcb_file->GetFilePath().ReverseFind('\\');		
					CString new_name = pcb_file->GetFilePath().Left(fi) + rel_f_pictures + p[0].Right( p[0].GetLength() - fi2 -1 );
					src_new.Add( new_name );
				//}
				//else
				//{
				//	int fi = m_pcb_full_path.ReverseFind('\\');
				//	CString new_name = m_pcb_full_path.Left(fi) + rel_f_pictures + p[0].Right( p[0].GetLength() - fi2 -1 );
				//	src_new.Add( new_name );
				//}
				continue;
			}
			else if( LinkToPart )
			{
				if( LinkToPart > 0 )// only if > 0
				{
					pcb_file->Seek( LinkToPart, CFile::begin );
					LinkToPart = -1; // =-1 to break after reading part
					continue;
				}
			}
			else// ( when LinkToPart == 0 check pages )
			{	//------------------------------------
				// start page
				//------------------------------------
				if( in_str.Left(11) == "rename_page" )
				{
					page_num++;
					if( ssm == NULL )
						RenamePage( Pages.GetActiveNumber(), &(p[0]) );
					src_new.RemoveAll();
					continue;
				}
				//------------------------------------
				// "rename_page" test 
				//------------------------------------
				if( page_num == -1 )// "rename_page" no present
					continue;
				//------------------------------------
				// next pages
				//------------------------------------
				if( in_str.Left(12) == "add_new_page" )
				{	
					page_num++;
					op->SetSize(gl_ic+1);
					if( ssm == NULL )
					{
						// now redraw polylines
						m_view->RedrawLayers(); // ReadGraphics
						m_view->ViewAllElements(NULL);
						gl_ic = -1;
						AddNewPage( &(p[0]) );				
						op = m_outline_poly;
						part = NULL;
						value = NULL;
						footprint = NULL;
						m_p_start = -1;
					}
					src_new.RemoveAll();
					continue;
				}
				//------------------------------------
				// read this page only
				//------------------------------------
				if( i_page != -1 )
					if( i_page != page_num )
						continue;
			}
			//------------------------------------
			// reading outline-polyline
			//------------------------------------
			if( np && (key_str == "outline" || key_str == "polyline") )
			{
				if( np < 4 ) 
				{
					CString * err_str = new CString( "error parsing [graphics] section of project file" );//throw
					throw err_str;
				}
				int ncorners = my_atoi( &p[0] );
				if( ncorners > 1 )
				{
					int hatch = my_atoi( &p[1] );
					int lay = my_atoi( &p[2] );
					int clsd = 1;
					if( key_str == "polyline" )
						clsd = 0;
					int wid = 0;
					if ( np >= 5 )
						wid = my_atoi( &p[3] );

					// r merge
					int merge = -1;
					if ( np >= 6 && bReadingM )
						merge = my_atoi( &p[4] );
					int s_merge = -1;
					if ( np >= 7 && bReadingM )
						s_merge = my_atoi( &p[5] );

					if( merge != mINDEX && s_merge != mINDEX && mINDEX >= 0 )
					{
						m_EN = 0;
						continue;
					}
					else
						m_EN = 1;

					int src = -1;
					if ( np >= 8 )
						src = my_atoi( &p[6] );
					int src_invert = 1;
					if ( np >= 9 )
						src_invert = my_atoi( &p[7] );
					int favorite = -1;
					if ( np >= 10 )
						favorite = my_atoi( &p[8] );
					if( favorite > 0 )
					{
						for( int i=0; i<MAX_FAVORITES; i++ )
							if( m_view->m_favorite_poly[i] <= -1 )
							{
								m_view->m_favorite_poly[i] = gl_ic+1;
								m_view->m_favorite_page[i] = page_num;
								break;
							}
					}
					//
					gl_ic++;
					int ic = gl_ic;
					if( op->GetSize() <= ic )
						op->SetSize(ic+100);
					int n_cont=0, last_n_cont=0, i_start=0;
					for( int icor=0; icor<ncorners; icor++ )
					{
						err = pcb_file->ReadString( in_str );
						if( !err )
						{
							CString * err_str = new CString( "unexpected EOF in project file" );//throw
							throw err_str;
						}
						np = ParseKeyString( &in_str, &key_str, &p );
						if( key_str != "corner" || np < 4 )
						{
							CString * err_str = new CString( "error parsing [graphics] section of project file" );//throw
							throw err_str;
						}
						n_cont = 0;
						if( clsd )
						{
							// end contour
							if( np >= 6 )
								n_cont = my_atoi( &p[4] );	
						}
						if( last_n_cont != n_cont && icor > i_start+2 )
						{
							(*op)[ic].Close( last_side_style, 0 );
							i_start = icor;
						}
						//
						int ncor = my_atoi( &p[0] );
						if( (ncor-1) != icor )
						{
							CString * err_str = new CString( "error parsing [graphics] section of project file" );
							throw err_str;
						}
						int x = my_atoi( &p[1] );
						int y = my_atoi( &p[2] );
						id id_sm( ID_POLYLINE, ID_GRAPHIC, ic );
						if( icor == 0 )
						{
							// make new line
							(*op)[ic].Start( lay, wid, NM_PER_MIL, x+dx, y+dy, hatch, &id_sm, NULL );
							if( ssm )
								(*op)[ic].SetDisplayList( NULL );
							else
								(*op)[ic].SetDisplayList( m_dlist );
						}
						else
							(*op)[ic].AppendCorner( x+dx, y+dy, last_side_style, FALSE );
						if( np >= 5 )
							last_side_style = my_atoi( &p[3] );
						else
							last_side_style = CPolyLine::STRAIGHT;
						// node size
						(*op)[ic].Node[icor] = 0;
						if( clsd == 0 )
							if( np >= 6 )
								(*op)[ic].Node[icor] = my_atoi( &p[4] );
						last_n_cont = n_cont;
						if( icor == (ncorners-1) )
						{
							if( clsd && ncorners > 2 )
								(*op)[ic].Close( last_side_style, NULL );
							(*op)[ic].SetMerge(merge);
							(*op)[ic].SetMerge(s_merge,TRUE);
							(*op)[ic].pAttr[index_part_attr] = part;
							(*op)[ic].pAttr[index_value_attr] = value;
							(*op)[ic].pAttr[index_foot_attr] = footprint;
							if( m_p_start == -1 )
								m_p_start = ic;
							else if( part )
								(*op)[ic-1].Next[index_part_attr] = ic;
							else 
								(*op)[ic-1].Next[index_part_attr] = -1;

							if( gRECT )
							{
								RECT P = (*op)[ic].GetBounds();
								SwellRect( gRECT, P );
							}
						}
					}
					if( ncorners >= 4 )
					{
						int tmp = -1;
						if( src >= 0 && src < src_new.GetSize() )
						{
							if( ADD_SRC )
							{
								src = AddSource( &src_new.GetAt(src) );
								if( src >= 0 )
								{
									tmp = m_dlist->AddTemplate( src, (*op)[ic].GetLayer(), &(*op)[ic].GetCornerBounds() );
									m_dlist->SetTemplateStyle( src_invert, tmp );
								}
								else if( m_dlg_log )
								{
									m_dlg_log->OnShowMe();
								}
							}
							else if( RETSRC.GetLength() == 0 )
							{
								RETSRC = src_new.GetAt(src);
								tmp = -2;
							}
						}
						(*op)[ic].SetPicture( tmp );
					}
				}
			}
			// attr
			if( np && ( in_str.Left(5) == "name:" ||
						in_str.Left(8) == "ref_des:" ||
						in_str.Left(9) == "pin_name:" ||
						in_str.Left(9) == "net_name:" ||
						in_str.Left(11) == "part_value:" ||
						in_str.Left(15) == "footprint_name:" ||
						in_str.Left(12) == "description:"))
			{
				CString str = p[0];
				if( in_str.Left(5) == "name:" )
				{
					part = Attr->m_Reflist->AddText( 0, 0, 0, LAY_PART_NAME, 1000000, 200000, m_default_font, &str );	
					m_p_start = -1;
				}
				else if( str.GetLength() )
				{
					int x = my_atoi( &p[1] );
					int y = my_atoi( &p[2] );
					int layer = my_atoi( &p[3] );
					int angle = my_atoi( &p[4] );
					int font_size = my_atoi( &p[5] );
					int stroke_width = my_atoi( &p[6] );
					int merge = -1;
					if ( np >= 9 && bReadingM )
						merge = my_atoi( &p[7] );
					int s_merge = -1;
					if ( np >= 10 && bReadingM )
						s_merge = my_atoi( &p[8] );
					int font_n = SIMPLEX;
					if ( np >= 11 )
						font_n = my_atoi( &p[9] );
					int pdf_opt = 1;
					if ( np >= 12 )
						pdf_opt = my_atoi( &p[10] );
					CText * t;
					int ic = gl_ic;
					if( in_str.Left(8) == "ref_des:" )
					{
						part = Attr->m_Reflist->AddText( x+dx, y+dy, angle, LAY_PART_NAME, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );	
						t = part;
						m_p_start = -1;
						if( LinkToPart == -1 )
						{
							RECT tr;
							if( Attr->m_Reflist->GetTextRectOnPCB( part, &tr ) )
								if( gRECT )
									SwellRect( gRECT, tr );
						}
					}
					else if( in_str.Left(11) == "part_value:" )
					{
						value = Attr->m_Valuelist->AddText( x+dx, y+dy, angle, LAY_PART_VALUE, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );
						t = value;
						if( LinkToPart == -1 )
						{
							RECT tr;
							if( Attr->m_Valuelist->GetTextRectOnPCB( value, &tr ) )
								if( gRECT )
									SwellRect( gRECT, tr );
						}
					}
					else if( in_str.Left(15) == "footprint_name:" )
					{
						if( str.Compare( OLD_NO_FP ) == 0 )
							str = NO_FOOTPRINT; // replace old marking
						footprint = Attr->m_Footlist->AddText( x+dx, y+dy, angle, LAY_FOOTPRINT, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );
						t = footprint;
						if( LinkToPart == -1 )
						{
							RECT tr;
							if( Attr->m_Footlist->GetTextRectOnPCB( footprint, &tr ) )
								if( gRECT )
									SwellRect( gRECT, tr );
						}
					}
					else if( m_EN == 0 )
						continue;
					else if( in_str.Left(9) == "pin_name:" )
					{
						t = Attr->m_Pinlist->AddText( x+dx, y+dy, angle, layer, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );
						t->m_polyline_start = ic;
						(*op)[ic].pAttr[index_pin_attr] = t;
					}
					else if( in_str.Left(9) == "net_name:" )
					{
						t = Attr->m_Netlist->AddText( x+dx, y+dy, angle, layer, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );
						t->m_polyline_start = ic;
						(*op)[ic].pAttr[index_net_attr] = t;
					}
					else if( in_str.Left(12) == "description:" )
					{
						t = Attr->m_pDesclist->AddText( x+dx, y+dy, angle, layer, font_size, stroke_width, font_n, &str, TRUE, pdf_opt );
						t->m_polyline_start = ic;
						(*op)[ic].pAttr[index_desc_attr] = t;
					}
					t->m_merge = merge;
					t->m_submerge = s_merge;
					//t->m_font_number = font_n;?
					if( getbit( t->m_pdf_options, PDF_DEF ) == 0 )
						if( getbit( t->m_pdf_options, PDF_GRAPHIC ) == 0 )
							if( getbit( t->m_pdf_options, PDF_USE_FONT ) == 0 )
							{
								setbit( t->m_pdf_options, PDF_DEF );
								clrbit( t->m_pdf_options, PDF_ALIGN );
							}
				}
			}
			else if( in_str.Left(8) == "part_end" )
			{
				int iv = gl_ic;
				if( iv >= 0 )
				{
					(*op)[iv].Next[index_part_attr] = -1;
					if( part )
					{
						part->m_polyline_start = m_p_start;
						if( value )
							value->m_polyline_start = m_p_start;
						if( footprint )
							footprint->m_polyline_start = m_p_start;
						if( ssm == NULL )
						{
							RECT P = GetPartBounds( part, NULL );
							(*op)[iv].ModifyGroupRect( &P );
						}
					}
				}
				if( part && m_p_start == -1 )// remove texts
				{
					part->InVisible();
					if( value )
						value->InVisible();
					if( footprint )
						footprint->InVisible();
				}
				part = NULL;
				value = NULL;
				footprint = NULL;
				m_p_start = -1;
				if( LinkToPart == -1 )
					break;
			}
			else if( (key_str == "outline_polygon" || key_str == "outline_polyline")
				&& np >= 4 )
			{
				int w = GetDimensionFromString( &p[0], units);
				int x = GetDimensionFromString( &p[1], units);
				int y = GetDimensionFromString( &p[2], units);
				int vis=1, hatch=0, glayer=LAY_FP_SILK_TOP;
				if( np >= 5 )
					vis = my_atoi( &p[3] );
				if( np >= 6 )
					hatch = my_atoi( &p[4] );
				glayer = LAY_PART_OUTLINE;
				// id
				gl_ic++;
				int ic = gl_ic;
				if( op->GetSize() <= ic )
					op->SetSize(ic+100);
				//
				// move ref text
				if( part )
					if( part->m_x == 0 && part->m_y == 0 )
					{
						part->m_x = x;
						part->m_y = y;
						part->Draw( m_dlist, m_smfontutil );
					}
				id id_sm( ID_POLYLINE, ID_GRAPHIC, ic );
				//
				(*op)[ic].Start( glayer, w, NM_PER_MIL, x, y, hatch, &id_sm, NULL );
				if( ssm )
					(*op)[ic].SetDisplayList( NULL );
				else
					(*op)[ic].SetDisplayList( m_dlist );
				if(key_str == "outline_polygon")
					(*op)[ic].SetHatch(CPolyLine::DIAGONAL_FULL);
				(*op)[ic].SetVisible(vis);
				(*op)[ic].pAttr[index_part_attr] = part;
				if( m_p_start == -1 )
					m_p_start = gl_ic;
				else if( part )
					(*op)[ic-1].Next[index_part_attr] = ic;
				else 
					(*op)[ic-1].Next[index_part_attr] = -1;
			}
			else if( key_str == "next_corner" && np >= 3 )
			{
				int x = GetDimensionFromString( &p[0], units);
				int y = GetDimensionFromString( &p[1], units);
				int style = CPolyLine::STRAIGHT;
				if( np >= 4 ) 
					style = my_atoi( &p[2] );
				(*op)[gl_ic].AppendCorner( x, y, style );
			}
			else if( key_str == "close_polyline" && np >= 2 )
			{
				int style = CPolyLine::STRAIGHT;
				if( np >= 2 )
					style = my_atoi( &p[0] );
				(*op)[gl_ic].Close( style );
			}
			else if( key_str == "units" && np >= 2 )
			{
				if( p[0] == "MIL" )
				{
					units = MIL;
				}
				else if( p[0] == "MM" )
				{
					units = MM;
				}
				else if( p[0] == "NM" )
				{
					units = NM;
				}
			}
			else if( key_str == "view_all" )
				m_view->ViewAllElements(NULL);
		}
		op->SetSize(gl_ic+1);	
	}
	catch( CFileException * e )
	{
		CString * err_str = new CString;//throw
		if( e->m_lOsError == -1 )
			err_str->Format( "File error: %d\n", e->m_cause );
		else
			err_str->Format( "File error: %d %ld (%s)\n", 
				e->m_cause, e->m_lOsError, _sys_errlist[e->m_lOsError] );
		*err_str = "CFreePcbDoc::ReadGraphics()\n" + *err_str;
		throw err_str;
	}
	return RETSRC;
}
// read project options from file
//
// throws CString * exception on error
//
int CFreePcbDoc::ReadOptions( CStdioFile * pcb_file, BOOL rColors )
{
	int err, pos, np;
	int layer_info_number = -1;
	int pdf_layer_info_number = -1;
	CArray<CString> p;
	CString in_str, key_str;
	BOOL m_org_changed = FALSE;
	// initalize
	CFreePcbView * view = (CFreePcbView*)m_view;
	if( rColors )
	{
		m_visible_grid.SetSize( 0 );
		m_part_grid.SetSize( 0 );
		m_routing_grid.SetSize( 0 );
		m_pdf_checking = 0;
		m_pdf_font_i = 0;
		m_pdf_margin = 0;
	}
	BOOL bLoadSysColors = 0;
	if( pcb_file->GetFileName().Compare( "default_cds.cfg" ) == 0 )
	{
		m_color_data = "";
		bLoadSysColors = 1;
	}
	m_name = "";
	m_auto_interval = 180;
	m_dr.bCheckUnrouted = FALSE;
	m_default_font = SIMPLEX;
	m_node_w = 25*NM_PER_MIL;
	m_seg_clearance = 25*NM_PER_MIL;
	m_report_flags = 0;
	m_view->m_active_layer = LAY_FREE_LINE;
	m_view->m_sel_mask = INT_MAX;
	m_view->SetSelMaskArray( m_view->m_sel_mask );
	m_dlist->SetTopLayer(m_view->m_active_layer);
	m_file_version = 1.3;
	m_snap_angle = 45;
	try
	{
		// find beginning of [options] section
		do
		{
			err = pcb_file->ReadString( in_str );
			if( !err )
			{
				// error reading pcb file
				CString mess;
				mess.Format(G_LANGUAGE == 0 ? "Unable to find [options] section in file":"Не удалось найти раздел [options] в файле");
				AfxMessageBox( mess );
				return 0;
			}
			in_str.Trim();
		}
		while( in_str != "[options]" );

		// get data
		while( 1 )
		{
			pos = pcb_file->GetPosition();
			err = pcb_file->ReadString( in_str );
			if( !err )
			{
				CString * err_str = new CString( "unexpected EOF in project file" );//throw
				throw err_str;
			}
			in_str.Trim();
			if( in_str.Left(1) == "[" && in_str != "[options]" )
			{
				// normal return
				pcb_file->Seek( pos, CFile::begin );
				break;
			}
			//
			np = ParseKeyString( &in_str, &key_str, &p );
			if( np == 2 && key_str == "project_name" )
			{
				m_name = p[0];
			}
			else if( np == 2 && key_str == "app_folder" )
			{
				m_get_app_folder = p[0];
			}
			else if( np == 2 && key_str == "version" )
			{
				m_read_version = my_atof( &p[0] );
			}
			else if( np == 2 && key_str == "file_version" )
			{
				m_file_version = my_atof( &p[0] );
			}
			else if( np && key_str == "parent_folder" )
			{
				if( m_parent_folder.GetLength() == 0 )
					m_parent_folder = p[0];
				if( m_parent_folder.Right(1) == "\\" )
					m_parent_folder = m_parent_folder.Left(m_parent_folder.GetLength() - 1);
			}
			else if( np && key_str == "library_folder" )
			{
				m_lib_dir = p[0];
				if( m_lib_dir.Right(1) == "\\" )
					m_lib_dir = m_lib_dir.Left(m_lib_dir.GetLength() - 1);
			}
			else if( np && key_str == "full_library_folder" )
			{
				m_full_lib_dir = p[0];
				if( m_full_lib_dir.Right(1) == "\\" )
					m_full_lib_dir = m_full_lib_dir.Left(m_full_lib_dir.GetLength() - 1);
				if( m_full_lib_dir.Left(1) == "?" )
					m_full_lib_dir.Replace( "?", m_parent_folder );
			}
			else if( np && key_str == "CAM_folder" )
			{
				m_cam_full_path = p[0];
				if( m_cam_full_path.Right(1) == "\\" )
					m_cam_full_path = m_cam_full_path.Left(m_cam_full_path.GetLength() - 1);
			}
			else if( np && key_str == "bom_options" )
			{
				m_bom_options =  my_atoi( &p[0] );
			}
			else if( np && key_str == "pcb_name" )
			{
				if( Pages.ReturnPageByPCB( &p[0] ) == -1 )
				{
					Pages.m_pcb_names.Add( p[0] );
					int ipcb = Pages.m_pcb_names.GetSize() - 1;
					if( ipcb >= 0 )
						Pages.m_netlist_page_mask[ipcb] = my_atoi( &p[1] );
					if( Pages.m_pcb_names.GetSize() == 1 )
						SwitchToPage(0,0,0);// to update netlist_full_path data !!!
				}
			}
			else if( np && key_str == "netlist_format" )
			{
				Pages.m_netlist_format = my_atof( &p[0] );
			}
			else if( np && key_str == "page_mirror_mask" )
			{
				Pages.m_page_mirror_mask = my_atof( &p[0] );
			}
			else if( np && key_str == "page_number" )
			{
				m_current_page = my_atoi( &p[0] );
			}
			else if( np && key_str == "parent_index" )
			{
				m_parent_index = my_atoi( &p[0] );
			}
			else if( np && key_str == "alignment" )
			{
				m_alignment = my_atof( &p[0] );
			}
			//else if( np && key_str == "part_search" )
			//{
			//	m_part_search = p[0];
			//}
			else if( np && key_str == "find_component" && rColors )
			{
				m_find_component = p[0];
				if( m_find_component.Right(1) != "=" )
					m_find_component += dSPACE;
			}
			else if( np && key_str == "special_chars" && rColors )
			{
				m_special_chars = p[0];
			}
			else if( np && key_str == "partlist_size_x" )
			{
				m_partlist_size.cx = my_atoi( &p[0] );
			}
			else if( np && key_str == "partlist_size_y" )
			{
				m_partlist_size.cy = my_atoi( &p[0] );
			}
			else if( np && key_str == "partlist_pos_x" )
			{
				m_partlist_pos.cx = my_atoi( &p[0] );
			}
			else if( np && key_str == "partlist_pos_y" )
			{
				m_partlist_pos.cy = my_atoi( &p[0] );
			}
			else if( np && key_str == "partlist_col_w" )
			{
				m_partlist_col_w = my_atoi( &p[0] );
			}
			else if( np && key_str == "default_node_width" )
			{
				if( p[0].Right(2) == "MM" || p[0].Right(2) == "mm" )
					m_node_w = -my_atof( &p[0] );
				else
					m_node_w = my_atof( &p[0] );
			}
			else if( np && key_str == "n_additional_layers" )
			{
				m_num_additional_layers = my_atoi( &p[0] );
				m_num_layers = m_num_additional_layers + LAY_ADD_1;
			}
			else if( np && key_str == "autosave_interval" )
			{
				m_auto_interval = my_atoi( &p[0] );
			}
			else if( np && key_str ==  "netlist_completed" )
			{
				m_netlist_completed = my_atoi( &p[0] );
			}
			//else if( np && key_str == "m_client_rect_top" )
			//{
			//	m_view->m_client_r.top = my_atoi( &p[0] );
			//}
			//else if( np && key_str == "m_client_rect_left" )
			//{
			//	m_view->m_client_r.left = my_atoi( &p[0] );
			//}
			//else if( np && key_str == "m_client_rect_right" )
			//{
			//	m_view->m_client_r.right = my_atoi( &p[0] );
			//}
			//else if( np && key_str == "m_client_rect_bottom" )
			//{
			//	m_view->m_client_r.bottom = my_atoi( &p[0] );
			//}
			else if( np && key_str == "m_org_x" )
			{
				m_org_changed = TRUE;
				m_view->m_org_x = my_atoi( &p[0] )*m_pcbu_per_wu;
			}
			else if( np && key_str == "m_org_y" )
			{
				m_org_changed = TRUE;
				m_view->m_org_y = my_atoi( &p[0] )*m_pcbu_per_wu;
			}
			else if( np == 13 && key_str == "m_attr_size" )
			{
				m_view->m_attr_size.H_ref = my_atoi( &p[0] );
				m_view->m_attr_size.W_ref = my_atoi( &p[1] );
				m_view->m_attr_size.H_pin = my_atoi( &p[2] );
				m_view->m_attr_size.W_pin = my_atoi( &p[3] );
				m_view->m_attr_size.H_net = my_atoi( &p[4] );
				m_view->m_attr_size.W_net = my_atoi( &p[5] );
				m_view->m_attr_size.H_pindesc = my_atoi( &p[6] );
				m_view->m_attr_size.W_pindesc = my_atoi( &p[7] );
				m_view->m_attr_size.H_foot = my_atoi( &p[8] );
				m_view->m_attr_size.W_foot = my_atoi( &p[9] );
				m_view->m_attr_size.H_value = my_atoi( &p[10] );
				m_view->m_attr_size.W_value = my_atoi( &p[11] );
			}
			else if( np && key_str == "m_scale_factor" )
			{
				int scale = my_atoi( &p[0] );
				m_view->m_pcbu_per_pixel = (double)scale;
			}
			else if( np && key_str == "m_user_scale" )
			{
				double scale = my_atof( &p[0] );
				m_view->m_user_scale = scale;
			}
			else if( np && key_str == "m_measure_scale" )
			{
				int scale = my_atoi( &p[0] );
				m_view->m_measure_scale = scale;
			}
			else if( np && key_str == "m_arrow_scale" )
			{
				int scale = my_atoi( &p[0] );
				m_view->m_arrow_scale = scale;
			}
			else if( np && key_str == "m_system_colors" )
			{
				m_system_colors = my_atoi( &p[0] );
			}
			else if( np && key_str == "units" )
			{
				if( p[0] == "MM" )
					m_units = MM;
				else
					m_units = MIL;
			}
			else if( np && key_str == "m_grid_style" )
			{
				int gs = my_atoi( &p[0] );
				m_dlist->SetGridStyle(gs);
			}
			else if( np && key_str == "m_top_layer" )
			{
				int tl = my_atoi( &p[0] );
				m_view->m_active_layer = max(tl,LAY_FREE_LINE);
				m_dlist->SetTopLayer(m_view->m_active_layer);

			}
			else if( np && key_str == "m_visible_layers" )
			{
				int vl = my_atoi( &p[0] );
				for( int ii=0; ii<MAX_LAYERS; ii++ )
					if( getbit( vl, ii ) )
						m_vis[ii] = 1;
					else
						m_vis[ii] = 0;
			}
			else if( np && key_str == "m_sel_mask" )
			{
				int msm = my_atoi( &p[0] );
				m_view->m_sel_mask = msm;
				m_view->SetSelMaskArray( msm );
			}
			else if( np && key_str == "visible_grid_spacing" )
			{
				m_visual_grid_spacing = my_atof( &p[0] );
				if( m_dlist )
					m_dlist->SetVisibleGrid( TRUE, m_visual_grid_spacing/m_view->m_user_scale );
			}
			else if( np && key_str == "visible_grid_item" && rColors )
			{
				CString str;
				double value;
				if( np == 3 )
					str = p[1];
				else
					str = p[0];
				value = my_atof( &str );
				if( str.Right(2) == "MM" || str.Right(2) == "mm" )
					m_visible_grid.Add( -value );
				else
					m_visible_grid.Add( value );
			}
			else if( np && key_str == "polyline_grid_spacing" )
			{
				m_part_grid_spacing = my_atof( &p[0] );
			}
			else if( np && key_str == "placement_grid_item" && rColors )
			{
				CString str;
				double value;
				if( np == 3 )
					str = p[1];
				else
					str = p[0];
				value = my_atof( &str );
				if( str.Right(2) == "MM" || str.Right(2) == "mm" )
					m_part_grid.Add( -value );
				else
					m_part_grid.Add( value );
			}
			else if( np && key_str == "text_grid_spacing" )
			{
				m_routing_grid_spacing = my_atof( &p[0] );
			}
			else if( np && key_str == "text_grid_item" && rColors )
			{
				CString str;
				double value;
				if( np == 3 )
					str = p[1];
				else
					str = p[0];
				value = my_atof( &str );
				if( str.Right(2) == "MM" || str.Right(2) == "mm" )
					m_routing_grid.Add( -value );
				else
					m_routing_grid.Add( value );
			}
			else if( np && key_str == "snap_angle" )
			{
				m_snap_angle = my_atof( &p[0] );
			}
			else if( np && key_str == "m_seg_clearance" )
			{
				m_view->m_seg_clearance = my_atoi( &p[0] );
			}
			else if( np && key_str == "magnetize_value" )
			{
				m_magnetize_value = my_atoi( &p[0] );
			}
			else if (np && key_str == "unconnected_pin_layer")
			{
				m_unconnected_pin_layer = my_atoi(&p[0]);
			}
			else if( np && key_str == "letter_spacing" )
			{
				m_letter_spacing = my_atof( &p[0] );
			}
			else if( np && key_str == "polyline_width" )
			{
				m_min_polyline_width = my_atoi( &p[0] );
			}
			else if( np && key_str == "text_height" )
			{
				m_min_text_height = my_atoi( &p[0] );
			}
			else if( np && key_str == "min_text_stroke_width" )
			{
				m_min_text_stroke_width = my_atoi( &p[0] );
			}
			else if( np && key_str == "highlight_width" )
			{
				m_highlight_wid = my_atoi( &p[0] );
			}
			else if( np && key_str == "cam_flags" )
			{
				m_cam_flags = my_atoi( &p[0] );
			}
			else if( np && key_str == "cam_layers" )
			{
				m_cam_layers = my_atoi( &p[0] );
			}
			else if( np && key_str == "cam_units" )
			{
				m_cam_units = my_atoi( &p[0] );
			}
			else if( np && key_str == "report_options" )
			{
				m_report_flags = my_atoi( &p[0] );
			}
			else if( np && key_str == "default_font" )
			{
				m_default_font = my_atoi( &p[0] );
			}
			// DRC stuff
			else if( np && key_str == "drc_check_unrouted" )
			{
				m_dr.bCheckUnrouted = my_atoi( &p[0] );
			}
			else if( np && key_str == "drc_part_attr_to_attr" )
			{
				m_dr.part_attr_to_attr = my_atoi( &p[0] );
			}
			else if( np && key_str == "drc_part_attr_to_poly" )
			{
				m_dr.part_attr_to_poly = my_atoi( &p[0] );
			}
			else if( np && key_str == "drc_pin_attr_to_attr" )
			{
				m_dr.pin_attr_to_attr = my_atoi( &p[0] );
			}
			else if( np && key_str == "pin_attr_to_poly" )
			{
				m_dr.pin_attr_to_poly = my_atoi( &p[0] );
			}
			else if( np && key_str == "default_polyline_width" )
			{
				if( p[0].Right(2) == "MM" || p[0].Right(2) == "mm" )
					m_polyline_w = -my_atof( &p[0] );
				else
					m_polyline_w = my_atof( &p[0] );
			}
			/*else if( np && key_str == "default_via_pad_width" )
			{
				if( p[0].Right(2) == "MM" || p[0].Right(2) == "mm" )
					m_via_w = -my_atof( &p[0] );
				else
					m_via_w = my_atof( &p[0] );
			}
			else if( np && key_str == "default_via_hole_width" )
			{
				if( p[0].Right(2) == "MM" || p[0].Right(2) == "mm" )
					m_via_hole_w = -my_atof( &p[0] );
				else
					m_via_hole_w = my_atof( &p[0] );
			}*/
			else if( np && key_str == "layer_info" )
			{
				CString file_layer_name = p[0];
				int i = my_atoi( &p[1] );
				int layer = -1;
				if( rColors )
					for( int il=0; il<=LAY_ADD_2; il++ )
					{
						CString layer_string = &layer_str[il][0];
						if( file_layer_name == layer_string )
						{
							layer = il;
							layer_info_number = -1;
							break;
						}
					}
				if( rColors && layer == -1 )
					for( int il=LAY_ADD_2+1; il<MAX_LAYERS; il++ )
					{
						CString layer_string = &layer_str[il][0];
						if( file_layer_name == layer_string )
						{
							layer = il;
							layer_info_number = -1;
							break;
						}
					}
				if (layer < 0 && layer_info_number >= 0)
				{
					layer = layer_info_number;
					layer_info_number++;
				}
				if( layer < 0 )
				{
					//AfxMessageBox( "Warning: layer \"" + file_layer_name + "\" not supported" );
				}
				else
				{
					m_rgb[layer][0] = my_atoi( &p[2] );
					m_rgb[layer][1] = my_atoi( &p[3] );
					m_rgb[layer][2] = my_atoi( &p[4] );
					m_vis[layer] = my_atoi( &p[5] );
				}
				if( bLoadSysColors )
					m_color_data += in_str + "\n";
			}
			else if( np && key_str == "pdf_layer_info" )
			{
				CString file_layer_name = p[0];
				int i = my_atoi( &p[1] );
				int layer = -1;
				if( rColors )
					for( int il=0; il<MAX_LAYERS; il++ )
					{
						CString layer_string = &layer_str[il][0];
						if( file_layer_name == layer_string )
						{
							layer = il;
							pdf_layer_info_number = -1;
							break;
						}
					}
				if (layer < 0 && pdf_layer_info_number >= 0)
				{
					layer = pdf_layer_info_number;
					pdf_layer_info_number++;
				}
				if( layer < 0 )
				{
					//AfxMessageBox( "Warning: layer \"" + file_layer_name + "\" not supported" );
				}
				else
				{
					m_pdf_rgb[layer][0] = my_atoi( &p[2] );
					m_pdf_rgb[layer][1] = my_atoi( &p[3] );
					m_pdf_rgb[layer][2] = my_atoi( &p[4] );
					m_pdf_vis[layer] = my_atoi( &p[5] );
				}
				if( bLoadSysColors )
					m_color_data += in_str + "\n";
			}
			else if( key_str == "m_bom_colors" && np >= 22 && rColors )
			{
				for( int k=0; k<21; k++ )
					m_bom_rgb[k/3][k%3] = my_atoi( &p[k] );
				if( bLoadSysColors )
					m_color_data += in_str + "\n";
			}
			else if( key_str == "pdf_checking" && rColors )
			{
				m_pdf_checking = my_atoi( &p[0] );
			}
			else if( np && key_str == "m_pdf_margin" && rColors )
			{
				m_pdf_margin = my_atof( &p[0] );
			}
			else if( np && key_str == "m_pdf_font_i" && rColors )
			{
				m_pdf_font_i = my_atoi( &p[0] );
			}
			else if( np >= 2 && key_str == "merge" )
			{
				if( m_mlist )
					m_mlist->AddNew(p[0]);
			}
			else if( np >= 2 && key_str == "ref_list" )
			{
				if( m_ref_lists )
					AddNewRefList(&p[0]);
			}
		}
		
		CRect screen_r;
		m_view->GetWindowRect( &screen_r );
		m_view->m_client_r.left = screen_r.left;
		m_view->m_client_r.top = screen_r.top;
		m_view->m_client_r.right = screen_r.right-screen_r.left;
		m_view->m_client_r.bottom = screen_r.bottom-screen_r.top;
		//
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		frm->m_wndMyToolBar.SetLists( &m_visible_grid, &m_part_grid, &m_routing_grid,
			m_visual_grid_spacing, m_part_grid_spacing, m_routing_grid_spacing, m_snap_angle, m_units );
		LoadSpeedFiles( this );
		return m_org_changed;
	}
	catch( CFileException * e )
	{
		CString * err_str = new CString;//throw
		if( e->m_lOsError == -1 )
			err_str->Format( "File error: %d\n", e->m_cause );
		else
			err_str->Format( "File error: %d %ld (%s)\n", 
				e->m_cause, e->m_lOsError, _sys_errlist[e->m_lOsError] );
		*err_str = "CFreePcbDoc::WriteOptions()\n" + *err_str;
		throw err_str;
	}
}

// write project options to file
//
// throws CString * exception on error
//
void CFreePcbDoc::WriteOptions( CStdioFile * file, BOOL bConfig, CString * pr_name )
{
	CString line;
	try
	{
		CString str;
		CFreePcbView * view = (CFreePcbView*)m_view;
		line.Format( "[options]\n\n" );
		file->WriteString( line );
		if( m_get_app_folder.Compare( m_app_dir ) == 0 )
		{
			line.Format( "app_folder: \"%s\"\n", m_app_dir );
			file->WriteString( line );
		}
		line.Format( "version: %5.3f\n", m_version );
		file->WriteString( line );
		m_file_version = m_version;
		line.Format( "file_version: %5.3f\n", m_file_version );
		file->WriteString( line );
		if( pr_name )
			line.Format( "project_name: \"%s\"\n", *pr_name );
		else
			line.Format( "project_name: \"%s\"\n", m_pcb_filename );
		m_name = m_pcb_filename;
		file->WriteString( line );
		CString lp = m_full_lib_dir.Left( m_full_lib_dir.ReverseFind( '\\' ) );
		line.Format( "library_folder: \"%s\"\n", lp );
		file->WriteString( line );

		if( bConfig )
		{
			line.Format( "parent_folder: \"%s\"\n", m_parent_folder );
			file->WriteString( line );
		}
		// lib folder
		{
			str = m_full_lib_dir;
			if( str.MakeLower().Find( m_parent_folder.MakeLower() ) == 0 )
				str.Replace( m_parent_folder, "?" );
			line.Format( "full_library_folder: \"%s\"\n", str );
			file->WriteString( line );
		}

		line.Format( "CAM_folder: \"%s\"\n", m_cam_full_path );
		file->WriteString( line );
		line.Format( "netlist_completed: \"%d\"\n", m_netlist_completed );
		file->WriteString( line );
		line.Format( "bom_options: \"%d\"\n", m_bom_options );
		file->WriteString( line );
		line.Format( "page_number: \"%d\"\n", Pages.GetActiveNumber() );
		file->WriteString( line );
		line.Format( "parent_index: \"%d\"\n", m_parent_index );
		file->WriteString( line );
		line.Format( "alignment: %3.2f\n", m_alignment );
		file->WriteString( line );
		line.Format( "partlist_size_x: \"%d\"\n", m_partlist_size.cx );
		file->WriteString( line );
		line.Format( "partlist_size_y: \"%d\"\n", m_partlist_size.cy );
		file->WriteString( line );
		line.Format( "partlist_pos_x: \"%d\"\n", m_partlist_pos.cx );
		file->WriteString( line );							
		line.Format( "partlist_pos_y: \"%d\"\n", m_partlist_pos.cy );
		file->WriteString( line );
		line.Format( "partlist_col_w: \"%d\"\n", m_partlist_col_w );
		file->WriteString( line );
		if( bConfig )
		{
			line.Format( "find_component: \"%s\"\n", m_find_component );
			file->WriteString( line );
			line.Format( "special_chars: \"%s\"\n", m_special_chars );
			file->WriteString( line );
			line = "netlist_file_path: \"\"\n";
		}
		else
			line.Format( "netlist_file_path: \"%s\"\n", m_netlist_full_path );
		file->WriteString( line );
		//
		if( bConfig == 0 )
			for( int gi=0; gi<Pages.m_pcb_names.GetSize(); gi++ )
			{
				line.Format( "pcb_name: \"%s\" %d\n", Pages.m_pcb_names[gi], Pages.m_netlist_page_mask[gi] );
				file->WriteString( line );
			}
		line.Format( "netlist_format: %d\n", Pages.m_netlist_format );
		file->WriteString( line );
		line.Format( "page_mirror_mask: %d\n", Pages.m_page_mirror_mask );
		file->WriteString( line );
		line.Format( "default_font: %d\n", m_default_font );
		file->WriteString( line );
		line.Format( "default_node_width: %d\n", m_node_w );
		file->WriteString( line );
		line.Format( "autosave_interval: %d\n", m_auto_interval );
		file->WriteString( line );
		line.Format( "m_client_rect_left: %d\n", m_view->m_client_r.left );
		file->WriteString( line );
		line.Format( "m_client_rect_right: %d\n", m_view->m_client_r.right );
		file->WriteString( line );
		line.Format( "m_client_rect_bottom: %d\n", m_view->m_client_r.bottom );
		file->WriteString( line );
		line.Format( "m_client_rect_top: %d\n", m_view->m_client_r.top );
		file->WriteString( line );
		line.Format( "m_org_x: %d\n", (int)((double)m_view->m_org_x/(double)m_pcbu_per_wu) );
		file->WriteString( line );
		line.Format( "m_org_y: %d\n", (int)((double)m_view->m_org_y/(double)m_pcbu_per_wu) );
		file->WriteString( line );
		line.Format( "m_scale_factor: %d\n", (int)m_view->m_pcbu_per_pixel );
		file->WriteString( line );
		line.Format( "m_attr_size: %d %d %d %d %d %d %d %d %d %d %d %d\n", 
			(int)m_view->m_attr_size.H_ref,
			(int)m_view->m_attr_size.W_ref,
			(int)m_view->m_attr_size.H_pin,
			(int)m_view->m_attr_size.W_pin,
			(int)m_view->m_attr_size.H_net,
			(int)m_view->m_attr_size.W_net,
			(int)m_view->m_attr_size.H_pindesc,
			(int)m_view->m_attr_size.W_pindesc,
			(int)m_view->m_attr_size.H_foot,
			(int)m_view->m_attr_size.W_foot,
			(int)m_view->m_attr_size.H_value,
			(int)m_view->m_attr_size.W_value );
		file->WriteString( line );
		//
		if( bConfig )
		{
			line.Format( "m_system_colors: %d\n", m_system_colors );
			file->WriteString( line );
			line.Format( "m_measure_scale: %d\n", m_view->m_measure_scale );
			file->WriteString( line );
			line.Format( "m_arrow_scale: %d\n", m_view->m_arrow_scale );
			file->WriteString( line );
		}
		else
		{
			line.Format( "m_user_scale: %.10f\n", m_view->m_user_scale );
			file->WriteString( line );
		}
		if( m_units == MIL )
			file->WriteString( "units: MIL\n\n" );
		else
			file->WriteString( "units: MM\n\n" );
		line.Format( "m_sel_mask: %d\n", m_view->m_sel_mask );
		file->WriteString( line );
		int m_vis_lrs = 0;
		for ( int ii=0; ii< MAX_LAYERS; ii++ )
			if( m_dlist->m_vis[ii] )
				setbit( m_vis_lrs, ii );
		line.Format( "m_visible_layers: %d\n", m_vis_lrs );
		file->WriteString( line );
		line.Format( "m_top_layer: %d\n", m_dlist->GetTopLayer() );
		file->WriteString( line );
		line.Format( "m_grid_style: %d\n", m_dlist->GetGridStyle() );
		file->WriteString( line );
		//
		line.Format( "visible_grid_spacing: %f\n", m_visual_grid_spacing );
		file->WriteString( line );
		for( int i=0; i<m_visible_grid.GetSize(); i++ )
		{
			if( m_visible_grid[i] > 0 )
				::MakeCStringFromDimension( 1.0, &str, m_visible_grid[i], MIL, TRUE, TRUE );
			else
				::MakeCStringFromDimension( 1.0, &str, -m_visible_grid[i], MM, TRUE, TRUE );
			file->WriteString( "  visible_grid_item: " + str + "\n" );
		}
		file->WriteString( "\n" );

		//
		line.Format( "polyline_grid_spacing: %f\n", m_part_grid_spacing );
		file->WriteString( line );
		for( int i=0; i<m_part_grid.GetSize(); i++ )
		{
			if( m_part_grid[i] > 0 )
				::MakeCStringFromDimension( 1.0, &str, m_part_grid[i], MIL, TRUE, TRUE );
			else
				::MakeCStringFromDimension( 1.0, &str, -m_part_grid[i], MM, TRUE, TRUE );
			file->WriteString( "  placement_grid_item: " + str + "\n" );
		}
		file->WriteString( "\n" );
		line.Format( "text_grid_spacing: %f\n", m_routing_grid_spacing );
		file->WriteString( line );
		for( int i=0; i<m_routing_grid.GetSize(); i++ )
		{
			if( m_routing_grid[i] > 0 )
				::MakeCStringFromDimension( 1.0, &str, m_routing_grid[i], MIL, TRUE, TRUE );
			else
				::MakeCStringFromDimension( 1.0, &str, -m_routing_grid[i], MM, TRUE, TRUE );
			file->WriteString( "  text_grid_item: " + str + "\n" );
		}
		file->WriteString( "\n" );
		line.Format( "snap_angle: %d\n", m_snap_angle );
		file->WriteString( line );
		file->WriteString( "\n" );
		line.Format( "m_seg_clearance: %d\n", m_view->m_seg_clearance );
		file->WriteString( line );
		if( bConfig )
		{
			line.Format( "unconnected_pin_layer: %d\n", m_unconnected_pin_layer );
			file->WriteString(line);
			line.Format( "magnetize_value: %d\n", m_magnetize_value );
			file->WriteString( line );
			line.Format( "letter_spacing: %5.3f\n", m_letter_spacing );
			file->WriteString( line );
		}
		line.Format( "polyline_width: %d\n", m_min_polyline_width );
		file->WriteString( line );
		line.Format( "text_height: %d\n", m_min_text_height );
		file->WriteString( line );
		line.Format( "min_text_stroke_width: %d\n", m_min_text_stroke_width );
		file->WriteString( line );
		line.Format( "highlight_width: %d\n", m_highlight_wid );
		file->WriteString( line );
		line.Format( "cam_flags: %d\n", m_cam_flags );
		file->WriteString( line );
		line.Format( "cam_layers: %d\n", m_cam_layers );
		file->WriteString( line );
		line.Format( "cam_units: %d\n", m_cam_units );
		file->WriteString( line );
		file->WriteString( "\n" );
		//
		line.Format( "report_options: %d\n", m_report_flags );
		file->WriteString( line );
		line.Format( "drc_check_unrouted: %d\n", m_dr.bCheckUnrouted );
		file->WriteString( line );
		line.Format( "drc_part_attr_to_attr: %d\n", m_dr.part_attr_to_attr );
		file->WriteString( line );
		line.Format( "drc_part_attr_to_poly: %d\n", m_dr.part_attr_to_poly );
		file->WriteString( line );
		line.Format( "drc_pin_attr_to_attr: %d\n", m_dr.pin_attr_to_attr );
		file->WriteString( line );
		line.Format( "drc_pin_attr_to_poly: %d\n", m_dr.pin_attr_to_poly );
		file->WriteString( line );
		file->WriteString( "\n" );
		//
		line = "default_polyline_width:";
		file->WriteString( line );
		::MakeCStringFromDimension( 1.0, &line, abs(m_polyline_w), (m_polyline_w>=0?MIL:MM), TRUE, TRUE, FALSE, (m_polyline_w>=0?0:3) );
		file->WriteString( line );
		file->WriteString( "\n" );
		//
		line.Format( "n_additional_layers: %d\n", m_num_additional_layers );
		file->WriteString( line );

		// pdf
		::MakeCStringFromDimension( 1.0, &line, m_pdf_font_i, NM, FALSE, FALSE, FALSE, 0 );
		file->WriteString( "m_pdf_font_i: " + line + "\r\n" );
		//
		::MakeCStringFromDouble( &line, m_pdf_margin );
		file->WriteString( "m_pdf_margin: " + line + "\r\n" );
		//
		line.Format( "pdf_checking: %d\n", m_pdf_checking );
		file->WriteString( line );
		//
		// write optional
		if( bConfig == 0 || m_system_colors )
		{
			for( int i=0; i<MAX_LAYERS; i++ )
			{
				line.Format( "  layer_info: \"%s\" %d %d %d %d %d\n",
					&layer_str[i][0], i,
					m_rgb[i][0], m_rgb[i][1], m_rgb[i][2], m_vis[i] );
				file->WriteString( line );
			}
			file->WriteString( "\n" );
			for( int i=0; i<MAX_LAYERS; i++ )
			{
				line.Format( "  pdf_layer_info: \"%s\" %d %d %d %d %d\n",
					&layer_str[i][0], i,
					m_pdf_rgb[i][0], m_pdf_rgb[i][1], m_pdf_rgb[i][2], m_pdf_vis[i] );
				file->WriteString( line );
			}
			file->WriteString( "\n" );
			{
				CString bom_clr;
				line = "m_bom_colors:";
				for( int k=0; k<21; k++ )
				{
					bom_clr.Format( " %d", m_bom_rgb[k/3][k%3] );
					line += bom_clr;
				}
				file->WriteString( line );
			}
		}
		else
		{
			file->WriteString( m_color_data );
		}
		file->WriteString( "\n" );
		if( bConfig == 0 )
		{
			if( m_mlist ) for( int ml=0; ml<m_mlist->GetSize(); ml++ )
			{
				CString str;
				CString test = m_mlist->GetStr(ml);
				str.Format( "  merge: \"%s\"\n", test );
				file->WriteString( str );
			}
			if( m_ref_lists ) for( int ml=0; ml<m_ref_lists->GetSize(); ml++ )
			{
				CString str;
				CString test = m_ref_lists->GetStr(ml);
				str.Format( "  ref_list: \"%s\"\n", test );
				file->WriteString( str );
			}
			file->WriteString( "\n" );
		}	
		return;
	}
	catch( CFileException * e )
	{
		CString * err_str = new CString;//throw
		if( e->m_lOsError == -1 )
			err_str->Format( "File error: %d\n", e->m_cause );
		else
			err_str->Format( "File error: %d %ld (%s)\n", 
				e->m_cause, e->m_lOsError, _sys_errlist[e->m_lOsError] );
		*err_str = "CFreePcbDoc::WriteBoardOutline()\n" + *err_str;
		throw err_str;
	}
}

// set defaults for a new project
//
void CFreePcbDoc::InitializeNewProject()
{
	// protection
	m_protection = 0;

	// remove merges, clip_outline_poly
	m_mlist->Clear();
	clip_mlist->Clear();
	m_ref_lists->Clear();
	clip_outline_poly.RemoveAll();	

	// these are the embedded defaults
	for(int i=0; i<NUM_CAM_FOLDERS; i++)
		CAM[i] = "";
	m_name = "";
	m_get_app_folder = m_app_dir;
	m_path_to_folder = "..\\projects\\";
	m_lib_dir = OLD_NO_FP ;
	m_pcb_filename = "";
	m_pcb_full_path = "";
	m_netlist_full_path = "";
	m_units = MIL;
	m_num_additional_layers = 4;
	m_num_layers = m_num_additional_layers + LAY_ADD_1;
	m_auto_interval = 180;

	// colors for layers
	for( int i=0; i<MAX_LAYERS; i++ )
	{
		m_vis[i] = 0;
		m_rgb[i][0] = 250; 
		m_rgb[i][1] = 250; 
		m_rgb[i][2] = 250;			// default white
	}
	m_rgb[LAY_BACKGND][0] = 0; 
	m_rgb[LAY_BACKGND][1] = 0; 
	m_rgb[LAY_BACKGND][2] = 0;			// background BLACK
	m_rgb[LAY_HILITE][0] = 0; 
	
	// pdf colors for layers
	for( int i=0; i<MAX_LAYERS; i++ )
	{
		m_pdf_vis[i] = 0;
		m_pdf_rgb[i][0] = m_rgb[i][0]; 
		m_pdf_rgb[i][1] = m_rgb[i][1]; 
		m_pdf_rgb[i][2] = m_rgb[i][2];			// default 
	}

	// bom colors
	for( int i=0; i<21; i++ )
	{
		m_bom_rgb[i/3][i%3] = (1050*i)%255;			// default 
	}

	// now set layer colors and visibility
	for( int i=0; i<MAX_LAYERS; i++ )
	{
		if( i<m_num_layers )
			m_vis[i] = 1;
		else
			m_vis[i] = 0;
		m_dlist->SetLayerRGB( i, m_rgb[i][0], m_rgb[i][1], m_rgb[i][2] );
		m_dlist->SetLayerVisible( i, m_vis[i] );
	}

	// default visible grid spacing menu values (in NM)
	m_visible_grid.RemoveAll();
	m_visible_grid.Add( 100*NM_PER_MIL );
	m_visible_grid.Add( 125*NM_PER_MIL );
	m_visible_grid.Add( 200*NM_PER_MIL );	// default index = 2
	m_visible_grid.Add( 250*NM_PER_MIL );
	m_visible_grid.Add( 400*NM_PER_MIL );
	m_visible_grid.Add( 500*NM_PER_MIL );
	m_visible_grid.Add( 1000*NM_PER_MIL );
	m_visual_grid_spacing = 200*NM_PER_MIL;
	m_dlist->SetVisibleGrid( TRUE, m_visual_grid_spacing/m_view->m_user_scale );

	// default placement grid spacing menu values (in NM)
	m_part_grid.RemoveAll();
	m_part_grid.Add( 10*NM_PER_MIL );
	m_part_grid.Add( 20*NM_PER_MIL );
	m_part_grid.Add( 25*NM_PER_MIL );
	m_part_grid.Add( 40*NM_PER_MIL );
	m_part_grid.Add( 50*NM_PER_MIL );		// default
	m_part_grid.Add( 100*NM_PER_MIL );
	m_part_grid.Add( 200*NM_PER_MIL );
	m_part_grid.Add( 250*NM_PER_MIL );
	m_part_grid.Add( 400*NM_PER_MIL );
	m_part_grid.Add( 500*NM_PER_MIL );
	m_part_grid.Add( 1000*NM_PER_MIL );
	m_part_grid_spacing = 50*NM_PER_MIL;

	// default routing grid spacing menu values (in NM)
	m_routing_grid.RemoveAll();
	m_routing_grid.Add( 1*NM_PER_MIL );
	m_routing_grid.Add( 2*NM_PER_MIL );
	m_routing_grid.Add( 2.5*NM_PER_MIL );
	m_routing_grid.Add( 3.333333333333*NM_PER_MIL );
	m_routing_grid.Add( 4*NM_PER_MIL );
	m_routing_grid.Add( 5*NM_PER_MIL );
	m_routing_grid.Add( 8.333333333333*NM_PER_MIL );
	m_routing_grid.Add( 10*NM_PER_MIL );	// default
	m_routing_grid.Add( 16.66666666666*NM_PER_MIL );
	m_routing_grid.Add( 20*NM_PER_MIL );
	m_routing_grid.Add( 25*NM_PER_MIL );
	m_routing_grid.Add( 40*NM_PER_MIL );
	m_routing_grid.Add( 50*NM_PER_MIL );
	m_routing_grid.Add( 100*NM_PER_MIL );
	m_routing_grid_spacing = 10*NM_PER_MIL;

	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	frm->m_wndMyToolBar.SetLists( &m_visible_grid, &m_part_grid, &m_routing_grid,
		m_visual_grid_spacing, m_part_grid_spacing, m_routing_grid_spacing, m_snap_angle, MIL );

	// default PCB parameters
	m_default_font = SIMPLEX;
	m_node_w = 25*NM_PER_MIL;
	m_polyline_w = 10*NM_PER_MIL;
	m_via_hole_w = 14*NM_PER_MIL;

	// default cam parameters
	for( int ii=0; ii<NUM_SHORTCUTS; ii++ )
		m_cam_shortcut[ii] = "";
	m_cam_full_path = "";
	m_min_text_height = 8*NM_PER_MIL; 
	m_min_polyline_width = 10*NM_PER_MIL;
	m_min_text_stroke_width = 5*NM_PER_MIL;
	m_highlight_wid = 5*NM_PER_MIL;
	m_cam_flags = 0;
	m_cam_layers = 0xf00fff;	// default layers
	m_cam_units = MIL;

	// default DRC limits
	m_dr.bCheckUnrouted = FALSE;
	m_dr.part_attr_to_attr = 30*NM_PER_MIL; 
	m_dr.part_attr_to_poly = 30*NM_PER_MIL; 
	m_dr.pin_attr_to_attr = 10*NM_PER_MIL;
	m_dr.pin_attr_to_poly = 10*NM_PER_MIL; 
	
	// clear flags
	for( int i=0; i<MAX_PAGES; i++ )
	{
		// clear netlist bitmap
		Pages.m_netlist_page_mask[i] = 0;

		// clear pcb flags
		m_pcb_modified[i] = 0;
	}
	// clear drc flags
	for( int i=0; i<MAX_PAGES+2; i++ ) // (+2 virtual pages)
	{
		// clear drc flags
		m_clearance_checked[i] = 0;
		m_footprint_checked[i] = 0;
		m_netlist_created[i] = 0;
		m_parts_checked[i] = 0;
	}
	// clear pcb names
	Pages.m_pcb_names.RemoveAll();

	CFreePcbView * view = (CFreePcbView*)m_view;
	view->InitializeView();

	// now try to find global options file
	CStdioFile file;
	CString def_file = m_app_dir + "\\default.cfg";
	if( !file.Open( def_file, CFile::modeRead | CFile::typeText ) )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Unable to open global configuration file \"default.cfg\"\nUsing application defaults":
			"Невозможно открыть глобальный файл конфигурации «default.cfg», поэтому будут использованы настройки приложения по умолчанию");
	}
	else
	{
		try
		{
			CString in_str;
			while( file.ReadString(in_str) )
			{
				in_str.Trim();
				if( in_str.Left(10) == "protected:" )
				{
					in_str = in_str.Right( in_str.GetLength() - 10 );
					in_str.Trim();
					int autoprotect = my_atoi(&in_str);
					if( getbit( autoprotect, PROTECT_ARCHIVED ) )
						setbit( m_protection, AUTOPROTECT );
					break;
				}
			}
			file.Close();
		}
		catch( CString * err_str )
		{
			*err_str = (G_LANGUAGE == 0 ?
				"Unable to open global configuration file \"default.cfg\"\n" :
				"Невозможно открыть файл глобальной конфигурации \"default.cfg\"\n");
			AfxMessageBox(*err_str,MB_ICONERROR);
			delete err_str;
		}
	}
	CString fn = m_app_dir + "\\default_cds.cfg";
	if( !file.Open( fn, CFile::modeRead | CFile::typeText ) )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Unable to open global configuration file \"default_cds.cfg\"\nUsing application defaults":
			"Невозможно открыть глобальный файл конфигурации «default_cds.cfg», поэтому будут использованы настройки приложения по умолчанию");
	}
	else
	{
		try
		{
			// read global default file options
			ReadOptions( &file, 1 );
			// make path to library folder and index libraries
			if( m_full_lib_dir.GetLength() == 0 )
			{
				char full[_MAX_PATH];
				CString fullpath = _fullpath( full, (LPCSTR)m_lib_dir, MAX_PATH );
				if( fullpath[fullpath.GetLength()-1] == '\\' )	
					fullpath = fullpath.Left(fullpath.GetLength()-1);
				//m_full_lib_dir = fullpath; ??
			}
			file.Close();
		}
		catch( CString * err_str )
		{
			*err_str = (G_LANGUAGE == 0 ?
				"Unable to open global configuration file \"default_cds.cfg\"\n" :
				"Невозможно открыть файл глобальной конфигурации \"default_cds.cfg\"\n");
			AfxMessageBox(*err_str,MB_ICONERROR);
			delete err_str;
		}
	}
	last_el = NULL;
	m_window_title = (CString)CDS_HEADER+(G_LANGUAGE == 0 ? "no project open":"конструктор схем");
	DisableMenuItems();
}

// Call this function when the project is modified,
// or to clear the modified flags
//
void CFreePcbDoc::ProjectModified( int flag, BOOL b_clear_redo )
{
	CWnd* pMain = AfxGetMainWnd();
	CMenu* pMenu = pMain->GetMenu();
	int npg = Pages.GetActiveNumber();
	if( flag > 0 )
	{
		// set modified flag
		if( b_clear_redo )
		{
			// pcb flags
			int ipcb = Pages.GetPcbIndex( npg );
			if( ipcb >= 0 )
				m_pcb_modified[ipcb] = flag;
			//
			m_clearance_checked[npg] = 0;
			m_footprint_checked[npg] = 0;
			m_netlist_created[npg] = 0;
			for( int ip=0; ip<MAX_PAGES; ip++ )
				m_parts_checked[ip] = 0;
			if( m_redo_list->m_num_items > 0 )
				// can't redo after a new operation
				m_redo_list->Clear();
		}
		if( m_project_modified )
		{
			// flag already set
			m_project_modified_since_autosave = TRUE;
		}
		else
		{
			// set flags
			m_project_modified = TRUE;
			m_project_modified_since_autosave = TRUE;
			m_window_title = m_window_title + "*";
			pMain->SetWindowText( m_window_title );
			CMenu* smenu = pMenu->GetSubMenu(3);	// "Project" submenu
			if( smenu )
				smenu->EnableMenuItem( ID_MAKE_ARCHIVED, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		}
	}
	else if( flag == 0 )
	{
		// clear flags
		for( int ipcb=0; ipcb<Pages.m_pcb_names.GetSize(); ipcb++ )
			m_pcb_modified[ipcb] = 0;
		//
		m_project_modified = FALSE;
		m_project_modified_since_autosave = FALSE;
		int len = m_window_title.GetLength();
		if( len > 1 && m_window_title[len-1] == '*' )
		{
			CWnd* pMain = AfxGetMainWnd();
			m_window_title = m_window_title.Left(len-1);
			pMain->SetWindowText( m_window_title );
			CMenu* smenu = pMenu->GetSubMenu(3);	// "Project" submenu
			if( smenu )
				smenu->EnableMenuItem( ID_MAKE_ARCHIVED, MF_BYCOMMAND | MF_ENABLED );
		}
		ResetUndoState();
	}
	
	// enable/disable menu items
	CMenu* submenu = pMenu->GetSubMenu(1);	// "Edit" submenu
	if( m_undo_list->m_num_items == 0 )
		submenu->EnableMenuItem( ID_EDIT_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
	else
		submenu->EnableMenuItem( ID_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED );	
	if( m_redo_list->m_num_items == 0 )
		submenu->EnableMenuItem( ID_EDIT_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
	else
		submenu->EnableMenuItem( ID_EDIT_REDO, MF_BYCOMMAND | MF_ENABLED );
	submenu = pMenu->GetSubMenu(0);	// "File" submenu
	if( m_view->m_protect )
	{	
		submenu->EnableMenuItem( ID_FILE_EXPORTNETLIST, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_FILE_SETNETLIST, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMenu->EnableMenuItem( 4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED );
	}
	else if( m_project_open )
	{
		submenu->EnableMenuItem( ID_FILE_EXPORTNETLIST, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_FILE_SETNETLIST, MF_BYCOMMAND | MF_ENABLED );
		pMenu->EnableMenuItem( 4, MF_BYPOSITION | MF_ENABLED );
	}
	pMain->DrawMenuBar();
}

void CFreePcbDoc::SaveOptions()
{
	// now try to save global options file
	CString fn = m_app_dir + "\\" + "default_cds.cfg";
	CStdioFile file;
	int err = file.Open( LPCSTR(fn), CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? "Unable to open file %s" : "Невозможно открыть файл %s", fn );
		AfxMessageBox( mess );
		return;
	}
	else
	{
		// write 
		try
		{
			WriteOptions( &file, 1 );
			file.WriteString("[end]");
			file.Close();
		}
		catch( CString * err_str )
		{
			*err_str = "CFreePcbDoc::SaveOptions()\n" + *err_str;
			AfxMessageBox(*err_str,MB_ICONERROR);
			delete err_str;
		}
	}
}

void CFreePcbDoc::OnViewLayers()
{
	CDlgLayers dlg;
	CFreePcbView * view = (CFreePcbView*)m_view;
	dlg.Initialize( m_num_layers, m_vis, m_rgb, m_system_colors );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		for( int i=0; i<m_num_layers; i++ )
		{
			m_dlist->SetLayerRGB( i, m_rgb[i][0], m_rgb[i][1], m_rgb[i][2] );
			m_dlist->SetLayerVisible( i, m_vis[i] );
		}
		m_system_colors = dlg.m_sys_clrs;
		view->m_left_pane_invalid = TRUE;	// force erase of left pane
		view->CancelSelection();
		view->OnRangeCmds( NULL );
		SaveOptions();
	}
}

BOOL CFreePcbDoc::Check_Txt_Msg( CString * TXT, CString * MSG )
{
	CString search_str = m_path_to_folder + "\\*.fpc";
	CFileFind finder;
	BOOL ok = 0;
	BOOL bWorking = finder.FindFile( search_str );
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if( !finder.IsDirectory() && !finder.IsDots() )
		{
			ok = 1;
			break;
		}
	}
	if( ok == 0 )
		return FALSE;

	CString ch_file;
	// test for txt
	if( TXT == NULL )
		ch_file = m_netlist_full_path + ".txt";
	else
		ch_file = *TXT;
	struct _stat buf;
	int err = _stat( ch_file, &buf );//ok
	if( err == 0 )
	{
		if( MSG )
		{
			CStdioFile fMess;
			err = fMess.Open( ch_file, CFile::modeRead, NULL );
			if( err )
			{
				fMess.ReadString( *MSG );
				fMess.Close();
			}
		}
	}
	return TRUE;
}

void CFreePcbDoc::FileExport( CString str, int type )
{
	CStdioFile file;
	if( !file.Open( str, CFile::modeWrite | CFile::modeCreate ) )
	{
		str.Format(G_LANGUAGE == 0 ? "Unable to open file %s":"Невозможно открыть файл %s", str);
		AfxMessageBox( str );
	}
	else
	{
		int flags = ~0;
		ExportPCBNetlist( &file, flags, type );
		file.Close();
		//
		// remove message
		//if( Check_Txt_Msg() )
		{
			CStdioFile m_file;
			CString messg = m_netlist_full_path + ".txt";
			int err = m_file.Open( LPCSTR(messg), CFile::modeCreate | CFile::modeWrite, NULL );
			if( err )
			{
				// update message
				m_file.WriteString( NETLIST_UPDATED );
				m_file.Close();
			}
			else
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Error! Unable to save related file. Write protection.":
					"Ошибка! Невозможно сохранить связанный файл. Защита от записи.", MB_ICONERROR);
		}
	}
	m_view->m_draw_layer = 0;
	m_view->Invalidate( FALSE );
}
void CFreePcbDoc::OnFileExport1()
{
	OnFileExport(TRUE);
}
void CFreePcbDoc::OnFileExport(BOOL DLG)
{
	if( Pages.IsThePageIncludedInNetlist( Pages.GetActiveNumber() ) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The current page is not included in the netlist. You can connect the pages in the Netlist Settings dialog box (the File menu)":
			"Текущая страница не включена в список эл.цепей. Вы можете подключить страницы в диалоговом окне Настройки списка (меню Файл)");
		return;
	}
	// 1
	OnToolsClearAllDrc();
	int mem_n_page = Pages.GetActiveNumber();
	for( int pg=0; pg<Pages.GetNumPages(); pg++ )
	{
		BOOL CH = TRUE;
		if( mem_n_page != pg ) 
		{
			int get_pcb = Pages.GetPcbIndex(pg);
			if( get_pcb != Pages.GetPcbIndex(mem_n_page) )
				CH = FALSE; // this page included in a different netlist, ignored
		}
		if( CH && m_netlist_created[pg] == 0 )
		{
			SwitchToPage(pg);
			int num_parts = Attr->m_Reflist->GetNumTexts();
			if( num_parts > 1 )
			{
				CreatePCBNets();
				if( m_netlist_created[pg] == 0 )
				{
					m_view->InvalidateLeftPane();
					AfxMessageBox(G_LANGUAGE == 0 ? 
						"Correct netlist errors on this page.":
						"Исправьте ошибки эл.цепей на этой странице.", MB_ICONINFORMATION);
					m_dlg_log->OnShowMe();
					m_view->OnRangeCmds(NULL);
					return;
				}
			}
			else if( num_parts == 1 )
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"The schematic design page must contain at least two PCB parts":
					"Страница проекта схемы должна содержать не менее двух деталей печатной платы.", MB_ICONERROR);
				return;
			}
		}
	}
	if( mem_n_page != Pages.GetActiveNumber() )
		SwitchToPage(mem_n_page);
	// 2
	OnToolsClearAllDrc();
	int error_page = -1;
	if( m_parts_checked[mem_n_page] == 0 )
	{
		m_dlg_log->Clear();
		error_page = DRC();
	}
	if( m_parts_checked[mem_n_page] == 0 )
	{
		if( error_page >= 0 )
			SwitchToPage( error_page, TRUE );
		m_view->InvalidateLeftPane();
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Correct errors related to Parts, Pins and Electronic buses":
			"Исправьте ошибки, связанные с деталями, штифтами и/или электронными шинами.", MB_ICONINFORMATION);
		m_dlg_log->OnShowMe();
		return;
	}
	// save project 
	if( mem_n_page != Pages.GetActiveNumber() )
		SwitchToPage(mem_n_page);
	int ok = FileSave( &m_path_to_folder, &m_pcb_filename, &m_path_to_folder, &m_pcb_filename );
	if( ok != 1 )
	{
		ProjectModified( TRUE );
		return;
	}
	// export netlist
	int rf = m_pcb_full_path.ReverseFind( '\\' );
	CString file_name = m_pcb_full_path.Right( m_pcb_full_path.GetLength() - rf - 1 );
	CString ch_file = m_netlist_full_path + ".net";
	FileExport( ch_file, Pages.m_netlist_format );
	if( mem_n_page != Pages.GetActiveNumber() )
		SwitchToPage(mem_n_page);
	m_dlg_log->ShowWindow( SW_HIDE );
	CString s;
	s.Format("Netlist created in the project folder. Full path to file: \n%s\n", ch_file );
	CDlgMyMessageBox Dlg;
	Dlg.Initialize( s, IDB_BITMAP_NL );
	ok = DLG ? Dlg.DoModal() : IDOK;
	if( ok == IDOK )
	{
		m_view->CancelSelection();
		SwitchToPCB(0,0,0);
	}
	else
	{
		m_dlg_log->AddLine("\r\n___________________________\r\nHint: to bring up this dialog press L\r\n");
		m_dlg_log->OnShowMe();
	}
	ProjectModified(FALSE);
}

void CFreePcbDoc::OnFileSetNetlist()
{
	int rf = m_pcb_full_path.ReverseFind( '\\' );
	CString file_name = m_pcb_full_path.Right( m_pcb_full_path.GetLength() - rf - 1 );

	// test for txt
	int m_netlist_used = Check_Txt_Msg();

	// test for rnmb
	struct _stat buf;
	CString ch_file = m_netlist_full_path + ".rnmb";
	int err = _stat( ch_file, &buf );//ok

	// check pcb names
	Pages.CheckPCBNames( &m_path_to_folder );

	// extract dlg
	int sv_page = Pages.GetActiveNumber();
	CDlgNetlistSettings dlg;
	dlg.Ini( &Pages, (BOOL)(m_netlist_used && err == 0), &m_path_to_folder );
	dlg.DoModal();
	ProjectModified( TRUE );
	//if( Pages.GetActiveNumber() != sv_page ) ! N.U.
	SwitchToPage( sv_page, TRUE );
	Pages.InvalidatePcbIndexes();
	rf = m_pcb_full_path.ReverseFind( '\\' );
	m_netlist_full_path = m_pcb_full_path.Left(rf) + freeasy_netlist + Pages.GetCurrentPCBName();
}

int GetSessionLayer( CString * ses_str )
{
	if( *ses_str == "Top" )
		return LAY_ADD_1;
	else if( *ses_str == "Bottom" )
		return LAY_ADD_2;
	else if( ses_str->Left(6) == "Inner_" )
	{
		return( LAY_ADD_2 + my_atoi( &(ses_str->Right(1)) ) );
	}
	return -1;
}

// export netlist in PADS-PCB format
// enter with file already open
// flags:
//	IMPORT_PARTS = include parts in file
//	IMPORT_NETS = include nets in file
//	IMPORT_AT = use "value@footprint" format for parts
//
int CFreePcbDoc::ExportPCBNetlist( CStdioFile * file, UINT flags, UINT format, CArray<CString> * Partlist )
{
	CString str, str2;
	if( format == NL_PADS_PCB && file )
		file->WriteString( "*PADS-PCB*\n" );
	
	if( flags & EXPORT_PARTS )
	{
		int str_len = 0;
		m_dlg_log->AddHeader("Parts included in the listing",2);
		if( format == NL_PADS_PCB && file )
			file->WriteString( "*PART*\n" );
		for( int ip=0; ip<Pages.GetNumPages(); ip++ )
		{
			CTextList * tl = Pages.GetTextList( ip, index_part_attr );
			tl->MarkAllTexts(0);
		}
		int it = -1; 
		int ip = -1;
		for( CText * part=Pages.GetNextAttr( index_part_attr, &it, &ip, TRUE ); 
					 part; 
					 part=Pages.GetNextAttr( index_part_attr, &it, &ip, TRUE ) )
		{
			if( part->utility )
				continue;
			int iof = part->m_str.Find(".");
			if( iof < 1 )
				iof = part->m_str.Find("-");
			if( iof > 0 )// multipart
			{
				CString pmod = part->m_str.Left(iof) + "&"; // include multiparts, for example DA1.1 or DA1-1
				part->utility = 1;
				CText * pmark = NULL;
				for( Pages.FindPart( &pmod, &pmark, TRUE, TRUE, it, ip ); pmark; Pages.FindPart( &pmod, &pmark, FALSE, TRUE ) )
					pmark->utility = 1;
			}
			CString str, V="", F="";
			if( part->m_polyline_start >= 0 )
			{
				CString ref = part->m_str;
				if( iof > 0 )
					ref = part->m_str.Left(iof);
				CArray<CPolyLine> * pa = Pages.GetPolyArray( ip );
				if( pa->GetAt( part->m_polyline_start ).pAttr[index_value_attr] )
					V = pa->GetAt( part->m_polyline_start ).pAttr[index_value_attr]->m_str;
				if( pa->GetAt( part->m_polyline_start ).pAttr[index_foot_attr] )
					F = pa->GetAt( part->m_polyline_start ).pAttr[index_foot_attr]->m_str;
				if( F.Compare(NO_FOOTPRINT) == 0 )
					continue;
				if( format == NL_PADS_PCB )
				{
					if( V.GetLength() )
						str.Format("%s  %s@%s", ref, V, F );
					else
						str.Format("%s  %s", ref, F );
				}
				else if( format == NL_TANGO )
				{
					str = "[\n" + ref + "\n" + F + "\n" + F + "\n" + V + "\n\n\n]";
				}
				if( file )
					file->WriteString( str + "\n" );

				// partlist info
				if( Partlist )
				{
					CString nV = V.GetLength()?V:"---";
					CString info_str;
					info_str.Format("%s  %s@%s", ref, nV, F );
					Partlist->Add( info_str );
				}

				// report
				if( str_len )
					m_dlg_log->AddLine(",   ");
				if( str_len > 50 )
				{
					str_len = 0;
					m_dlg_log->AddLine("\r\n");
				}
				m_dlg_log->AddLine(ref);
				str_len += (ref.GetLength()+4);
			}	
		}
		m_dlg_log->AddLine("\r\n");
		int CNT = 0;
		it=-1, ip=-1; 
		for( CText * ptr = Pages.FindBLK_PTR(&it,&ip); ptr; ptr=Pages.FindBLK_PTR(&it,&ip) )
		{
			CString destination="";
			Pages.GetBlockParts( &destination, ptr, format, &CNT, m_dlg_log, NULL, NULL, -1, Partlist );
			if( file )
				file->WriteString( destination );
		}
		if( format == NL_PADS_PCB && file )
			file->WriteString( "\n" );
	}
	m_dlg_log->AddLine("\r\n");
	CArray<CString> Array;
	if( flags & EXPORT_NETS )
	{
		m_dlg_log->AddHeader("Nets included in the listing",2);
		CString NetControl = dSPACE;
		if( format == NL_PADS_PCB && file )
			file->WriteString( "*NET*\n" );
		for( int ip=0; ip<Pages.GetNumPages(); ip++ )
		{
			CTextList * tl = Pages.GetTextList( ip, -1 );
			tl->MarkAllTexts(0);
		}
		
		int it = -1; 
		int ip = -1;
		for( CText * net=Pages.GetNextAttr( -1, &it, &ip, TRUE ); 
					 net; 
					 net=Pages.GetNextAttr( -1, &it, &ip, TRUE ) )
		{
			// опрашиваем сети на страницах
			// включенных в нетлист БЕЗ 
			// учета страниц комплексных деталей ! ( use_netlist_page_mask=TRUE )
			
			if( net->utility )
				continue;

			int it2 = -1; 
			int ip2 = ip+1;
			for( CText * net2=Pages.GetNextAttr( -1, &it2, &ip2, TRUE ); 
						 net2; 
						 net2=Pages.GetNextAttr( -1, &it2, &ip2, TRUE ) )
			{
				// set mark for same texts on diff pages
				// маркируем эту сеть на всех страницах
				// т к функция GetNetPins считывает пины сети со всех страниц
				// а также пины соединенные с пинами комплексной детали
				if( net->m_str.Compare(net2->m_str) == 0 )
					if( net->m_str.Left(3) == FREENET )
					{
						ASSERT(0);
						AfxMessageBox(G_LANGUAGE == 0 ? 
							("Fatal.. Invalid net name " + net->m_str + ", process stopped"):
							("Фатальная ошибка. Недопустимое имя эл.цепи " + net->m_str + ", процесс остановлен"));
						return 0;
					}
					else
						net2->utility = 1;
			}
			// Get Net Pins
			CString gPins, Rename, Asuff="";
			int err = Pages.GetNetPins( &net->m_str, &gPins, Pages.m_netlist_format, &Rename, &NetControl, -1, &Asuff );
			if( err )
			{
				// замыкание сетей или 
				// какая то другая ошибка
				m_dlg_log->AddLine("\r\nFAIL\r\n");
				return 0;
			}
			int pins_added = 0;
			if( net->m_str.Compare(Rename) )
			{
				// эта сеть соединенная с пинами комплексной детали 
				// была переименована внутри комплексной детали
				// проверяем не была ли уже записана ранее
				// и вставляем в нужное место
				for( int ia=0; ia<Array.GetSize(); ia++ )
				{
					if( format == NL_PADS_PCB )
					{
						if( Array.GetAt(ia).Find( "*SIGNAL* " + Rename + "\n" ) == 0 )
						{
							Array.InsertAt( ia+1, gPins );
							pins_added++;
						}
					}
					else if( format == NL_TANGO )
					{
						if( Array.GetAt(ia).Find( "(\n" + Rename + "\n" ) == 0 )
						{
							Array.InsertAt( ia+1, gPins );
							pins_added++;
						}
					}
				}
				// report	
				if( pins_added )
				{
					gPins.Replace("-",".");
					gPins.Replace("\n","\r\n\t");
					m_dlg_log->AddLine("Append pins for net " + Rename + ":\r\n");
					m_dlg_log->AddLine("\t" + gPins + "\r\n");
				}

			}
			if( pins_added == 0 )
			{
				// create header
				if( format == NL_PADS_PCB )
					Array.Add( "*SIGNAL* " + Rename + "\n" );
				else if( format == NL_TANGO )
					Array.Add( "(\n" + Rename + "\n" );
				// pin array
				Array.Add( gPins );

				// report	
				{
					gPins.Replace("-",".");
					gPins.Replace("\n","\r\n\t");
					m_dlg_log->AddLine("Net " + Rename + ":\r\n");
					m_dlg_log->AddLine("\t" + gPins + "\r\n");
				}

				// find this net in the blocks
				if( Rename.Left(3) != FREENET )
				{
					// если именованная сеть то сразу
					// считываем пины этой сети внутри комплексной детали ( Rename=userNet )
					// кроме пинов соединенных с пинами комплексной детали
					int CNT = 0;
					it2=-1, ip2=-1;
					for( CText * ptr = Pages.FindBLK_PTR(&it2,&ip2); ptr; ptr=Pages.FindBLK_PTR(&it2,&ip2) )
					{
						CString destination="";
						if( Pages.GetBlockNets( &destination, ptr, format, &CNT, &Rename, &NetControl, m_dlg_log ) )
							Array.Add( destination );
						else if( destination.GetLength() ) // this is fatal error
							return 0;
					}

					// добавляем сеть в список считанных только здесь, не ранее
					NetControl += (Rename + dSPACE); // !!!
				}

				// close tango-netlist
				// закрываем заголовок 
				if( format == NL_TANGO )
					Array.Add(")\n");
			}
		}
		int CNT = 0;
		it=-1, ip=-1;
		for( CText * ptr = Pages.FindBLK_PTR(&it,&ip); ptr; ptr=Pages.FindBLK_PTR(&it,&ip) )
		{
			// считываем остальные сети внутри комплексной детали ( режим nF="" )
			// кроме пинов соединенных с пинами комплексной детали
			// и кроме сетей добавленных в список NetControl (уже считаных)
			CString destination="", nF="";
			if( Pages.GetBlockNets( &destination, ptr, format, &CNT, &nF, &NetControl, m_dlg_log ) )
				Array.Add( destination );
			else if( destination.GetLength() ) // this is fatal error
				return 0;
		}
		Array.Add( "\n" );
	}
	if( format == NL_PADS_PCB )
		Array.Add( "*END*\n" ); 

	// записываем в файл
	if( file ) 
		for( int i=0; i<Array.GetSize(); i++ )
			file->WriteString( Array.GetAt(i) );
	return 0;
}
//--------------------------
//--------------------------
void CFreePcbDoc::OnAppExit()
{
	if( FileClose() != IDCANCEL )
	{
//		m_view->SetHandleCmdMsgFlag( FALSE );
		AfxGetMainWnd()->SendMessage( WM_CLOSE, 0, 0 );
	}
}
//--------------------------
//--------------------------
// create undo record for moving origin
//
undo_move_origin * CFreePcbDoc::CreateMoveOriginUndoRecord( int x_off, int y_off )
{
	// create undo record 
	undo_move_origin * undo = new undo_move_origin;//ok
	undo->x_off = x_off;
	undo->y_off = y_off;
	return undo;
}

// undo operation on move origin
//
void CFreePcbDoc::MoveOriginUndoCallback( void * ptr, BOOL undo )
{
	if( undo )
	{
		// restore previous origin
		undo_move_origin * un_mo = (undo_move_origin*)ptr;
		if( un_mo )
		{
			int x_off = un_mo->x_off;
			int y_off = un_mo->y_off;
			this_Doc->m_view->MoveOrigin( -x_off, -y_off );
			this_Doc->m_view->Invalidate( FALSE );// (doubtful)
		}
	}
	delete ptr;
}


// create undo record for SM cutout
// only include closed polys
//
undo_outline_poly * CFreePcbDoc::CreateOutlinePolyUndoRecord( CPolyLine * poly, int num )
{
	// create undo record for sm cutout
	undo_outline_poly * undo;
	int ncorners = poly->GetNumCorners();
	undo = (undo_outline_poly*)malloc( sizeof(undo_outline_poly)+ncorners*sizeof(undo_corner));
	undo->layer = poly->GetLayer();
	undo->wid = poly->GetW();
	undo->m_bmp = poly->GetPicture();
	undo->closed = poly->GetClosed();
	undo->hatch_style = poly->GetHatch();
	undo->ncorners = poly->GetNumCorners();
	undo->bShown = poly->m_visible;
	undo_corner * corner = (undo_corner*)((UINT)undo + sizeof(undo_outline_poly));
	for( int ic=0; ic<ncorners; ic++ )
	{
		corner[ic].x = poly->GetX( ic );
		corner[ic].y = poly->GetY( ic );
		corner[ic].num_contour = poly->GetNumContour( ic );
		if( undo->closed || ic<ncorners-1 )
			corner[ic].style = poly->GetSideStyle( ic );
		corner[ic].node = poly->Node[ic];
	}
	undo->merge_op = poly->GetMerge();
	undo->submerge_op = poly->GetMerge(TRUE);
	if( RECT * R = poly->GetGroupRect() )
		undo->grr = *R;
	else
		undo->grr = rect(0,0,0,0);
	for( int ind=0; ind<num_attributes; ind++ )
	{
		undo->u_Attr[ind] = poly->pAttr[ind];
		undo->next[ind] = poly->Next[ind];
	}
	undo->num = num;
	return undo;
}

// undo operation on solder mask cutout
//
void CFreePcbDoc::OutlinePolyUndoCallback( void * ptr, BOOL undo )
{
	if( undo ) 
	{
		// restore cutout from undo record
		undo_outline_poly * undo_poly = (undo_outline_poly*)ptr;	
		if( undo_poly )
		{
			CPolyLine * poly = &this_Doc->m_outline_poly->GetAt(undo_poly->num);
			poly->SetDisplayList( this_Doc->m_dlist );
			id p_id( ID_POLYLINE, ID_GRAPHIC, undo_poly->num );
			undo_corner * corner = (undo_corner*)((UINT)undo_poly + sizeof(undo_outline_poly));
			poly->Hide();// Hide() for hide picture
			poly->m_visible = undo_poly->bShown;
			poly->Start( undo_poly->layer, undo_poly->wid, 5*NM_PER_MIL, 
				corner[0].x, corner[0].y, undo_poly->hatch_style, &p_id, NULL );
			poly->Node[0] = corner[0].node;
			int ic_start = 0;
			for( int ic=1; ic<undo_poly->ncorners; ic++ )
			{			
				if( corner[ic].num_contour != corner[ic-1].num_contour )
				{
					poly->Close( corner[ic-1].style );
					ic_start = ic;
					poly->AppendCorner( corner[ic].x, corner[ic].y, NULL, 0 );
				}
				else
					poly->AppendCorner( corner[ic].x, corner[ic].y, corner[ic-1].style, 0 );
				poly->Node[ic] = corner[ic].node;
			}
			if( undo_poly->closed )
				poly->Close( corner[undo_poly->ncorners-1].style );
			//
			// set new
			// picture
			poly->SetPicture( undo_poly->m_bmp );
			if( undo_poly->m_bmp >= 0 )
			{
				RECT pr = poly->GetCornerBounds();
				this_Doc->m_dlist->MoveTemplate( &pr, undo_poly->m_bmp, undo_poly->layer );
				this_Doc->m_dlist->SetTemplateVis( poly->GetVisible(), undo_poly->m_bmp );
			}
			poly->SetMerge( undo_poly->merge_op );
			poly->SetMerge( undo_poly->submerge_op, TRUE );
			poly->ModifyGroupRect(&undo_poly->grr);
			for( int ind=0; ind<num_attributes; ind++ )
			{
				poly->pAttr[ind] = undo_poly->u_Attr[ind];
				poly->Next[ind] = undo_poly->next[ind];
			}
			if( poly->m_visible )
				poly->Show();
		}
	}
	delete ptr;
}

// call dialog to create Gerber and drill files
void CFreePcbDoc::OnFileGenerateCadFiles()
{
	if( m_outline_poly->GetSize() == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"A outline must be present for CAM file generation":
			"Для генерации этого файла должны присутствовать полилинии.");
		return;
	}
	CDlgCAD dlg;
	if( m_cam_full_path.GetLength() == 0 )
		m_cam_full_path.Format( "%s\\CAM(%s)", m_path_to_folder, m_pcb_filename.Left( m_pcb_filename.GetLength()-4 ));
	dlg.Initialize( m_version,
		&m_cam_full_path, 
		&m_path_to_folder,
		&m_app_dir,
		&m_pcb_filename,
		m_num_additional_layers, 
		m_cam_units,
		m_min_polyline_width,
		m_min_text_stroke_width,
		m_highlight_wid,
		m_min_text_height,
		m_cam_flags,
		m_cam_layers,
		m_outline_poly, 
		Attr, 
		m_dlist,
		m_mlist,
		m_dlg_log );
	int ret = dlg.DoModal();
	if( ret != IDCANCEL)
	{
		int maxw = 0;
		// update parameters
		ProjectModified( TRUE );
		///
		m_cam_full_path = dlg.m_folder;
		m_cam_units = dlg.m_units;
		m_min_polyline_width = dlg.m_min_polyline_width;
		m_min_text_height = dlg.m_min_text_height;
		m_min_text_stroke_width = dlg.m_min_text_line_width;
		m_highlight_wid = dlg.m_highlight_width;
		m_cam_flags = dlg.m_flags;
		m_cam_layers = dlg.m_layers;
		GetGerberPathes(m_pcb_full_path);
		if( getbit( m_cam_flags, FL_LATER ) == 0 )
			if( getbit( m_cam_flags, GERBER_RENDER_ALL ) == 0 )
			{
				CString shcut[NUM_SHORTCUTS];
				for( int i=0; i<NUM_SHORTCUTS; i++ )
				{
					shcut[i] = m_cam_shortcut[i];
					shcut[i].MakeLower();
				}
				if( getbit( m_cam_flags, FL_VIEWMATE ) )
				{
					for( int i=0; i<NUM_SHORTCUTS; i++ )
						if( shcut[i].Find( "viewmate" ) > 0 )
							OnFileGerbV( m_cam_full_path, m_cam_shortcut[i] );
				}
				else if( getbit( m_cam_flags, FL_GERBV ) )
				{
					for( int i=0; i<NUM_SHORTCUTS; i++ )
						if( shcut[i].Find( "gerbv" ) > 0 )
							OnFileGerbV( m_cam_full_path, m_cam_shortcut[i] );
				}
				else if( getbit( m_cam_flags, FL_GERBERLOGIX ) )
				{
					for( int i=0; i<NUM_SHORTCUTS; i++ )
						if( shcut[i].Find( "gerberlogix" ) > 0 )
							OnFileGerbV( m_cam_full_path, m_cam_shortcut[i] );
				}
			}
	}
}

void CFreePcbDoc::OnProjectAdaptGrid()
{
	if( m_view->m_protect )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "This PCB is protected.":"Эта печатная плата защищена.", MB_ICONERROR);
		return;
	}
	if( m_view->m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects.":"Сначала выберите объекты.");
		return;
	}
	int MIN_D = NM_PER_MIL/3;
	int grid = (int)abs(m_part_grid_spacing);
	// save m_cursor_mode
	int m_cursor_mode = m_view->m_cursor_mode;
	m_view->SelectContour();
	m_view->SaveUndoInfoForGroup( m_undo_list );
	// align the entire 
	// parts first
	int refs = 0;//  
	Attr->m_Reflist->MarkAllTexts(0);
	for( int i=0; i<m_outline_poly->GetSize(); i++ )
	{
		if( m_outline_poly->GetAt(i).GetSel() == 0 )
			continue;		
		int Xmatch = 0, Ymatch = 0;
		for( int ii=0; ii<m_outline_poly->GetAt(i).GetNumCorners(); ii++ )
		{
			int X = m_outline_poly->GetAt(i).GetX(ii);
			int Y = m_outline_poly->GetAt(i).GetY(ii);
			int sh1 = X%grid;
			int sh2 = Y%grid;
			if( (abs(sh1) > MIN_D && grid-abs(sh1) > MIN_D) || 
				(abs(sh2) > MIN_D && grid-abs(sh2) > MIN_D) )
			{
				if( m_outline_poly->GetAt(i).Check( index_part_attr ) )
				{
					if( m_outline_poly->GetAt(i).pAttr[index_part_attr]->utility == 0 )
					{
						refs++;
						m_outline_poly->GetAt(i).pAttr[index_part_attr]->utility = 1;
					}
				}
				break;
			}
		}
	}
	if( refs )
	{
		int it = -1;
		for( CText * part=Attr->m_Reflist->GetNextText(&it); part; part=Attr->m_Reflist->GetNextText(&it) )
		{
			if( part->utility )
			{
				CPolyLine * gp = NULL;
				int DX=0, DY=0;
				for( int get=part->m_polyline_start; get>=0; get=m_outline_poly->GetAt(get).Next[index_part_attr] )
				{
					gp = &m_outline_poly->GetAt(get);
					if( gp->Check( index_pin_attr ) )
					{
						int X = gp->GetX(0);
						int Y = gp->GetY(0);
						DX = X%grid;
						DY = Y%grid;
						break;
					}
				}
				if( DX || DY )
				{
					for( int get=part->m_polyline_start; get>=0; get=m_outline_poly->GetAt(get).Next[index_part_attr] )
					{
						gp = &m_outline_poly->GetAt(get);
						int gncor = gp->GetNumCorners();
						int svX = gp->GetX(0);
						int svY = gp->GetY(0);
						int svXn = gp->GetX(gncor-1);
						int svYn = gp->GetY(gncor-1);
						for( int icor=0; icor<gp->GetNumCorners(); icor++ )
						{
							int X = gp->GetX(icor);
							int Y = gp->GetY(icor);
							X -= DX;
							Y -= DY;
							gp->SetX(icor,X);
							gp->SetY(icor,Y);
						}
						gp->RecalcRectC(0);
						gp->ModifyGroupRect(&rect(0,0,0,0));
						gp->Draw();
						
						//
						if( gp->GetLayer() == LAY_PIN_LINE )
						{
							//pull the ends of 
							// the polylines of the 
							// connections to the pins
							for( int con=0; con<m_outline_poly->GetSize(); con++ )
							{
								CPolyLine * p = &m_outline_poly->GetAt(con);
								if( p->m_visible == 0 )
									continue;
								if( p->GetSideSel() == 0 )
									continue;
								if( p->GetLayer() == LAY_FREE_LINE || p->GetLayer() == LAY_CONNECTION )
								{
									int cX1 = p->GetX(0);
									int cY1 = p->GetY(0);
									int cX2 = p->GetX(p->GetNumCorners()-1);
									int cY2 = p->GetY(p->GetNumCorners()-1);
									if( abs(cX1-svX) < NM_PER_MIL && abs(cY1-svY) < NM_PER_MIL )
									{
										p->SetX( 0, gp->GetX(0) );
										p->SetY( 0, gp->GetY(0) );
										//if( p->GetNumCorners() > 1 )
										//	m_view->AlignSides( ID_POLYLINE, con, 1 );
										p->Draw();
										///break;
									}
									if( abs(cX2-svX) < NM_PER_MIL && abs(cY2-svY) < NM_PER_MIL )
									{
										p->SetX( p->GetNumCorners()-1, gp->GetX(0) );
										p->SetY( p->GetNumCorners()-1, gp->GetY(0) );
										//if( p->GetNumCorners() > 1 )
										//	m_view->AlignSides( ID_POLYLINE, con, p->GetNumCorners()-2 );
										p->Draw();
										///break;
									}
									if( abs(cX1-svXn) < NM_PER_MIL && abs(cY1-svYn) < NM_PER_MIL )
									{
										p->SetX( 0, gp->GetX(gncor-1) );
										p->SetY( 0, gp->GetY(gncor-1) );
										//if( p->GetNumCorners() > 1 )
										//	m_view->AlignSides( ID_POLYLINE, con, 1 );
										p->Draw();
										///break;
									}
									if( abs(cX2-svXn) < NM_PER_MIL && abs(cY2-svYn) < NM_PER_MIL )
									{
										p->SetX( p->GetNumCorners()-1, gp->GetX(gncor-1) );
										p->SetY( p->GetNumCorners()-1, gp->GetY(gncor-1) );
										//if( p->GetNumCorners() > 1 )
										//	m_view->AlignSides( ID_POLYLINE, con, p->GetNumCorners()-2 );
										p->Draw();
										///break;
									}
								}
							}
						}
					}
				}

			}
		}
	}
	// now align each 
	// polyline separately
	m_view->MarkAllOutlinePoly(0,-1);
	int cnt_poly = 0, total = 0;
	for( int i=0; i<m_outline_poly->GetSize(); i++ )
	{
		if( m_outline_poly->GetAt(i).GetSel() == 0 )
			continue;		
		int Xmatch = 0, Ymatch = 0;
		for( int ii=0; ii<m_outline_poly->GetAt(i).GetNumCorners(); ii++ )
		{
			int X = m_outline_poly->GetAt(i).GetX(ii);
			int Y = m_outline_poly->GetAt(i).GetY(ii);
			int sh1 = X%grid;
			int sh2 = Y%grid;
			if( (abs(sh1) > MIN_D && grid-abs(sh1) > MIN_D) || 
				(abs(sh2) > MIN_D && grid-abs(sh2) > MIN_D) )
			{
				if( m_outline_poly->GetAt(i).GetLayer() == LAY_PIN_LINE )//|| m_outline_poly->GetAt(i).GetLayer() == LAY_CONNECTION )
					cnt_poly++;
				else
					total++;
				m_outline_poly->GetAt(i).SetUtility(1);
				break;
			}
		}
	}
	int ok_for_other = 0;
	if( cnt_poly == 0 && refs == 0 )
	{
		if( total == 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"This polyline group has already been adapted to the current grid step.":
				"Эта группа полилиний уже адаптирована к текущему шагу сетки.");
			return;
		}
		else ///if( m_cursor_mode != CUR_GROUP_SELECTED )
			ok_for_other = 1;
		//else 
		//{
		//	AfxMessageBox("This group does not contain pins.") ;
		//	return;
		//}
	}
	else
	{
		CString s;
		if( cnt_poly == 0 )
		{
			if( total == 0 )
			{
				s.Format(G_LANGUAGE == 0 ? 
					"%d part(s) adapted without graphic loss.":
					"%d деталей адаптированы без потери графики.", refs);
				AfxMessageBox(s);
			}
			else ///if( AfxMessageBox("All pins of this group are adapted to the current grid. Adapt remaining graphic polylines to the grid?", MB_ICONQUESTION|MB_YESNO) == IDYES )
			{
				if( AfxMessageBox(G_LANGUAGE == 0 ? 
					"This polyline group has already been adapted to the current grid step. Adapt other polylines?":
					"Эта группа полилиний уже адаптирована к текущему шагу сетки. Адаптировать другие полилинии?", MB_YESNO | MB_ICONQUESTION) == IDYES)
					ok_for_other = 1;			
			}
		}
		else
		{
			s.Format(G_LANGUAGE == 0 ? 
				"%d part(s) adapted\n%d lines adapted\n%d pins adapted":
				"%d деталей адаптировано\n%d линий адаптировано\n%d пинов адаптировано", refs, total, cnt_poly);
			AfxMessageBox(s);
		}
	}
	if( cnt_poly || ok_for_other )
	{
		// Move arcs completely
		for( int i=0; i<m_outline_poly->GetSize(); i++ )
		{
			if( m_outline_poly->GetAt(i).GetUtility() == 0 )
				continue;
			// Move arcs completely
			if( m_outline_poly->GetAt(i).GetSideStyle(0) )
			{
				int X = m_outline_poly->GetAt(i).GetX(0);
				int Y = m_outline_poly->GetAt(i).GetY(0);
				int sh1 = X%grid;
				int sh2 = Y%grid;
			 	for( int ii=0; ii<m_outline_poly->GetAt(i).GetNumCorners(); ii++ )
				{
					int X = m_outline_poly->GetAt(i).GetX(ii);
					int Y = m_outline_poly->GetAt(i).GetY(ii);
					X -= sh1;
					Y -= sh2;
					if( ok_for_other )
					{
						m_outline_poly->GetAt(i).SetX(ii,X);
						m_outline_poly->GetAt(i).SetY(ii,Y);
					}
				}
				m_outline_poly->GetAt(i).SetUtility(0);
				m_outline_poly->GetAt(i).Draw(m_dlist);
			}
		}
		// Move other polylines
		for( int i=0; i<m_outline_poly->GetSize(); i++ )
		{
			if( m_outline_poly->GetAt(i).GetUtility() == 0 )
				continue;
			if( ok_for_other == 0 )
				if( m_outline_poly->GetAt(i).GetLayer() != LAY_PIN_LINE )
					continue;	

			int Xmatch = 0, Ymatch = 0;
			int svX = m_outline_poly->GetAt(i).GetX(0);
			int svY = m_outline_poly->GetAt(i).GetY(0);
			int gncor = m_outline_poly->GetAt(i).GetNumCorners();
			int svXn = m_outline_poly->GetAt(i).GetX(gncor-1);
			int svYn = m_outline_poly->GetAt(i).GetY(gncor-1);
			for( int ii=0; ii<gncor; ii++ )
			{
				if( ii < m_outline_poly->GetAt(i).GetNumSides() )
					if( m_outline_poly->GetAt(i).GetSideStyle(ii) )
						continue;
				int xDis = 0;//Xmatch;
				int yDis = 0;//Ymatch;
				int X = m_outline_poly->GetAt(i).GetX(ii);
				int Y = m_outline_poly->GetAt(i).GetY(ii);
				
				int in = ii;
				if( ii < gncor - 1 )
					in++;
				else
					in = 0;
				int Xn = m_outline_poly->GetAt(i).GetX(in);
				int Yn = m_outline_poly->GetAt(i).GetY(in);
				
				if( abs(Xn-X) < _2540 )
					Xmatch = 1;
				else
					Xmatch = 0;
				if( abs(Yn-Y) < _2540 )
					Ymatch = 1;
				else
					Ymatch = 0;
				int sh1 = X%grid;
				int sh2 = Y%grid;
				X -= sh1;
				Y -= sh2;
				int N = 0;
				if( abs(Xn-X) < NM_PER_MIL && abs(Yn-Y) < NM_PER_MIL && Xmatch == 0 )
					N = 1;
				if( abs(sh1) > grid/2 || N )
				{
					if( sh1 > 0 )
					{
						if( abs(X+grid-Xn) > NM_PER_MIL || abs(Yn-Y) > NM_PER_MIL )
							X += grid;
					}
					else
					{
						if( abs(X-grid-Xn) > NM_PER_MIL || abs(Yn-Y) > NM_PER_MIL )
							X -= grid;
					}
				}
				N = 0;
				if( abs(Xn-X) < NM_PER_MIL && abs(Yn-Y) < NM_PER_MIL && Ymatch == 0 )
					N = 1;
				if( abs(sh2) > grid/2 || N )
				{
					if( sh2 > 0 )
					{
						if( abs(Y+grid-Yn) > NM_PER_MIL || abs(Xn-X) > NM_PER_MIL )
							Y += grid;
					}
					else
					{
						if( abs(Y-grid-Yn) > NM_PER_MIL || abs(Xn-X) > NM_PER_MIL )
							Y -= grid;
					}
				}
				if( xDis == 0 )
					m_outline_poly->GetAt(i).SetX(ii,X);
				if( yDis == 0 )
					m_outline_poly->GetAt(i).SetY(ii,Y);
				if( Xmatch )
					m_outline_poly->GetAt(i).SetX(in,X);
				if( Ymatch )	
					m_outline_poly->GetAt(i).SetY(in,Y);
			}
			if( m_outline_poly->GetAt(i).Check( index_pin_attr ) )
				m_outline_poly->GetAt(i).ModifyGroupRect( &m_outline_poly->GetAt(i).GetBounds() );
			
			m_outline_poly->GetAt(i).Draw();

			//pull the ends of 
			// the polylines of the 
			// connections to the pins
			for( int con=0; con<m_outline_poly->GetSize(); con++ )
			{
				CPolyLine * p = &m_outline_poly->GetAt(con);
				if( p->m_visible == 0 )
					continue;
				if( p->GetSideSel() == 0 )
					continue;
				if( p->GetLayer() == LAY_FREE_LINE || p->GetLayer() == LAY_CONNECTION )
				{
					int cX1 = p->GetX(0);
					int cY1 = p->GetY(0);
					int cX2 = p->GetX(p->GetNumCorners()-1);
					int cY2 = p->GetY(p->GetNumCorners()-1);
					if( abs(cX1-svX) < NM_PER_MIL && abs(cY1-svY) < NM_PER_MIL )
					{
						p->SetX( 0, m_outline_poly->GetAt(i).GetX(0) );
						p->SetY( 0, m_outline_poly->GetAt(i).GetY(0) );
						//if( p->GetNumCorners() > 2 )
						//	m_view->AlignSides( ID_POLYLINE, con, 1 );
						p->Draw();
						///break;
					}
					if( abs(cX2-svX) < NM_PER_MIL && abs(cY2-svY) < NM_PER_MIL )
					{
						p->SetX( p->GetNumCorners()-1, m_outline_poly->GetAt(i).GetX(0) );
						p->SetY( p->GetNumCorners()-1, m_outline_poly->GetAt(i).GetY(0) );
						//if( p->GetNumCorners() > 2 )
						//	m_view->AlignSides( ID_POLYLINE, con, p->GetNumCorners()-2 );
						p->Draw();
						///break;
					}
					if( abs(cX1-svXn) < NM_PER_MIL && abs(cY1-svYn) < NM_PER_MIL )
					{
						p->SetX( 0, m_outline_poly->GetAt(i).GetX(gncor-1) );
						p->SetY( 0, m_outline_poly->GetAt(i).GetY(gncor-1) );
						//if( p->GetNumCorners() > 2 )
						//	m_view->AlignSides( ID_POLYLINE, con, 1 );
						p->Draw();
						///break;
					}
					if( abs(cX2-svXn) < NM_PER_MIL && abs(cY2-svYn) < NM_PER_MIL )
					{
						p->SetX( p->GetNumCorners()-1, m_outline_poly->GetAt(i).GetX(gncor-1) );
						p->SetY( p->GetNumCorners()-1, m_outline_poly->GetAt(i).GetY(gncor-1) );
						//if( p->GetNumCorners() > 2 )
						//	m_view->AlignSides( ID_POLYLINE, con, p->GetNumCorners()-2 );
						p->Draw();
						///break;
					}
				}
			}
		}
	}

	ProjectModified(TRUE);
	m_view->CancelSelection();
	m_view->m_draw_layer = 0;// adapt to grid
	m_view->Invalidate( FALSE );// (doubtful)
}

void CFreePcbDoc::OnProjectPartList()
{
	if( m_view->m_dlg_partlist == NULL )
	{
		m_view->m_dlg_partlist = new CDlgPartlist;// CFreePcbDoc()
		m_view->m_dlg_partlist->Initialize(m_view);
		m_view->m_dlg_partlist->Create( IDD_PARTLIST );
		if( m_view->m_dlg_partlist )
		{
			m_view->m_dlg_partlist->ShowWindow( SW_SHOW );
			m_view->m_dlg_partlist->BringWindowToTop();
		}
	}
	else if( m_view->m_dlg_partlist )
	{	
		m_view->m_dlg_partlist->ImportPartlist();
		m_view->m_dlg_partlist->ShowWindow( SW_SHOW );
		m_view->m_dlg_partlist->BringWindowToTop();
	}
}

void CFreePcbDoc::OnProjectGenBOM()
{
	if( Pages.IsThePageIncludedInNetlist( Pages.GetActiveNumber() ) )
	{
		CDlgPartlistRep dlg;
		dlg.Initialize( &Pages );
		dlg.DoModal();
		m_view->CancelSelection(0);
		OnRangeCmds(NULL);
	}
	else
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The current page is not included in the netlist. You can connect the pages in the Netlist Settings dialog box (the File menu)" :
			"Текущая страница не включена в список эл.цепей. Вы можете подключить страницы в диалоговом окне Настройки списка (меню Файл)");
}

void CFreePcbDoc::OnProjectGenBOM2()
{
	CDlgRFiles dlg;
	dlg.Ini( this );
	dlg.DoModal();
}

void CFreePcbDoc::OnProjectGenBOM3()
{
	CString s0 = m_path_to_folder+"\\related_files\\reports\\bom";
	struct _stat buf;
	if( _stat( s0, &buf ) )
	{
		OnOpenTemplate( ID_FILE_OPEN_TEMPLATE );
		Sleep(1000);
	}
	BomInTable T(this);
	T.MakeReport();
}

void CFreePcbDoc::OnProjectGenTag()
{
	CString s0 = m_path_to_folder+"\\related_files\\reports\\tag";
	struct _stat buf;
	if( _stat( s0, &buf ) )
	{
		OnOpenTemplate( ID_FILE_OPEN_TEMPLATE2 );
		Sleep(1000);
	}
	TagTable T;
	T.MakeReport(this);
}

void CFreePcbDoc::OnProjectOptions()
{
	CDlgProjectOptions dlg;
	if( m_name.GetLength() == 0 )
		m_name = m_pcb_filename;
	CString name;
	if( m_name.Right(4).MakeLower() == ".cds" )
		name = m_name.Left( m_name.GetLength()-4 );
	else
		name = m_name;
	dlg.Init( FALSE, &name, &m_path_to_folder, &m_full_lib_dir, &m_app_dir,
		m_num_additional_layers, 0, m_node_w,
		m_polyline_w, m_default_font, m_parent_index, m_alignment, m_view->m_user_scale );
	int ret = dlg.DoModal();
	if( ret == IDOK )  
	{
		m_library.Clear();
		m_merge_library.Clear();
		m_parent_index = dlg.m_parent;
		m_alignment = dlg.m_alignment;
		double old_user_scale = m_view->m_user_scale;
		m_view->m_user_scale = dlg.GetScale();
		double k = m_view->m_user_scale / old_user_scale;
		m_dlist->SetVisibleGrid( TRUE, m_visual_grid_spacing/m_view->m_user_scale );
		// deal with decreased number of layers
		if( m_num_additional_layers > dlg.GetNumALayers() )
		{
			// decreasing number of layers, offer to reassign them
			CDlgReassignLayers rel_dlg;
			rel_dlg.Initialize( m_num_additional_layers, dlg.GetNumALayers() );
			int ret = rel_dlg.DoModal();
			if( ret == IDOK )
			{
				// reassign copper layers
				m_num_additional_layers = dlg.GetNumALayers();
				m_num_layers = m_num_additional_layers + LAY_ADD_1;
				for( int ii=0; ii<m_outline_poly->GetSize(); ii++ )
				{
					int L = m_outline_poly->GetAt(ii).GetLayer();
					if( L < m_num_layers )
						continue;
					L = rel_dlg.new_layer[L-LAY_ADD_1];
					m_outline_poly->GetAt(ii).SetLayer(L+LAY_ADD_1);
					m_outline_poly->GetAt(ii).Draw();
				}
			}
			// clear clipboard
			clip_outline_poly.SetSize(0);
		}
		else if( m_num_additional_layers < dlg.GetNumALayers() )
		{
			// increasing number of layers, don't reassign
			for (int il = m_num_additional_layers; il<dlg.GetNumALayers(); il++)
			{
				m_dlist->SetLayerRGB(LAY_ADD_1 + il, m_rgb[LAY_ADD_1 + il][0], m_rgb[LAY_ADD_1 + il][1], m_rgb[LAY_ADD_1 + il][2] );
				m_dlist->SetLayerVisible(LAY_ADD_1+il, 1);
				m_vis[LAY_ADD_1+il] = 1;
			}
			m_num_additional_layers = dlg.GetNumALayers();
			m_num_layers = m_num_additional_layers + LAY_ADD_1;
			m_view->ShowActiveLayer(m_num_additional_layers);
		}
		//
		// rename project
		if( name != dlg.GetName() )
		{
			CString old_n = m_pcb_full_path;
			m_name = dlg.GetName();
			if( m_name.Right(4).MakeLower() != ".cds" )
				m_name = m_name + ".cds";
			m_pcb_filename = m_name;
			m_pcb_full_path = m_pcb_full_path.Left( m_pcb_full_path.ReverseFind('\\')+1 ) + m_pcb_filename;
			rename( old_n, m_pcb_full_path );
			m_window_title = CDS_HEADER + m_pcb_filename;
			CWnd* pMain = AfxGetMainWnd();
			pMain->SetWindowText( m_window_title );
			ProjectModified( FALSE );
		}
		//
		// Read Footprint Folder
		if( m_full_lib_dir != dlg.GetLibFolder() )
		{
			m_full_lib_dir = dlg.GetLibFolder();
			ReadFootprintFolder(this);
		}

		// m_polyline_w
		m_polyline_w = dlg.GetPolyWidth() * k;
		m_view->m_polyline_width = abs(m_polyline_w);
		m_default_font = dlg.GetFont();

		// m_node_w (after upd m_polyline_w)
		if( m_node_w != dlg.GetNodeWidth() )
		{
			int pgmem = Pages.GetActiveNumber();
			m_node_w = dlg.GetNodeWidth() * k;
			for( int ipg=0; ipg<Pages.GetNumPages(); ipg++ )
			{
				SwitchToPage( ipg, 0 );
				for( int ipo=0; ipo<m_outline_poly->GetSize(); ipo++ )
				{
					BOOL bD = 0;
					if( m_outline_poly->GetAt(ipo).Node[0] > 1 )
					{
						m_outline_poly->GetAt(ipo).Node[0] = GetStandartNode(this, ipo);
						bD = 1;
					}
					if( m_outline_poly->GetAt(ipo).Node[m_outline_poly->GetAt(ipo).GetNumCorners()-1] > 1 )
					{
						m_outline_poly->GetAt(ipo).Node[m_outline_poly->GetAt(ipo).GetNumCorners()-1] = GetStandartNode(this, ipo);
						bD = 1;
					}
					if( bD )
						m_outline_poly->GetAt(ipo).Draw(m_dlist);
				}
			}
			SwitchToPage( pgmem, TRUE );
		}
		//
		m_view->InvalidateLeftPane();
		m_project_open = TRUE;

		// force redraw of function key text
		m_view->m_cursor_mode = 999;
		m_view->CancelSelection();
		ProjectModified( TRUE );
		ResetUndoState();
		m_view->m_draw_layer = 0;
		m_view->Invalidate( FALSE );
	}
}

// come here from MainFrm on timer event
//
void CFreePcbDoc::OnTimer()
{
	
}


void CFreePcbDoc::OnToolsDrc()
{
	DlgDRC dlg;
	do{
		m_drelist->Clear();
		dlg.Initialize( m_units, 
			m_dr.part_attr_to_attr,
			m_dr.part_attr_to_poly,
			m_dr.pin_attr_to_attr,
			m_dr.pin_attr_to_poly );
		int ret = dlg.DoModal();
		if( ret != IDCANCEL )
		{
			ProjectModified( TRUE );
			m_view->BringWindowToTop();
			m_dr.part_attr_to_attr = dlg.m_part_aa;
			m_dr.part_attr_to_poly = dlg.m_part_ap;
			m_dr.pin_attr_to_attr = dlg.m_pin_aa;
			m_dr.pin_attr_to_poly = dlg.m_pin_ap;
			//
			if( dlg.m_repeat_drc )
			{
				// show log dialog
				m_dlg_log->Clear();
				m_dlg_log->OnShowMe();
				DRC();
				m_dlg_log->OnShowMe();
			}
		}
		break;//debug mode
	}while( dlg.m_repeat_drc );
	m_view->Invalidate( FALSE );// (doubtful)
}

void CFreePcbDoc::OnToolsClearDrc()
{
	if( m_view->m_cursor_mode == CUR_DRE_SELECTED )
	{
		m_view->CancelSelection();
		m_view->SetCursorMode( CUR_NONE_SELECTED );
	}
	m_drelist->Clear();
	m_view->m_draw_layer = LAY_DRC_ERROR;
	m_view->OnRangeCmds(NULL);
}

void CFreePcbDoc::OnToolsClearAllDrc()
{
	for( int ip=0; ip<Pages.GetNumPages(); ip++ )
	{
		if( ip == Pages.GetActiveNumber() )
		{
			continue;
		}
		DRErrorList * dr = Pages.GetDRErrorList(ip);
		dr->Clear();
	}
	OnToolsClearDrc();
}

void CFreePcbDoc::OnToolsShowDRCErrorlist()
{
	// TODO: Add your command handler code here
}


void CFreePcbDoc::OnViewLog()
{
	m_dlg_log->m_hidden = FALSE;
	m_dlg_log->OnShowMe();
}

void CFreePcbDoc::OnEditSelectAll()
{
	m_view->CancelSelection();
	id _id(ID_TEXT_DEF);
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = GetTextListbyLayer( lay );
		if(!tl)
			continue;
		int it=-1;
		for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			m_view->NewSelect( t, &_id, 0, 0 );
	}
	for( int i=0; i<m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * Po = &m_outline_poly->GetAt(i); 
		if( Po->pAttr[index_part_attr] )
			setbit( m_view->m_sel_flags, m_view->FLAG_SEL_PART );
		_id = Po->GetId();
		_id.sst = ID_SIDE;
		_id.i = i;
		int nc = Po->GetNumCorners();
		if( Po->GetClosed() == 0 )
			nc--;
		for( int ii=0; ii<nc; ii++ )
		{
			_id.ii = ii;
			m_view->NewSelect( NULL, &_id, 0, 0 );
		}
	}
	if( m_view->m_sel_count )
		m_view->SetCursorMode( CUR_GROUP_SELECTED );
	m_view->HighlightGroup();
}

void CFreePcbDoc::OnEditPasteFromFile()
{
	// force old-style file dialog by setting size of OPENFILENAME struct
	CWnd * WND = theApp.GetMainWnd();
	CString filename = RunFileDialog( 1, "pas" ); 
	if ( filename.GetLength() )
	{ 
		CStdioFile read_file;
		int err = read_file.Open( filename, CFile::modeRead, NULL );
		if( !err )
		{
			// error opening project file
			CString mess;
			mess.Format(G_LANGUAGE == 0 ? "Unable to open file %s" : "Невозможно открыть файл %s", filename );
			AfxMessageBox( mess );
			return;
		}
		else
		{
			CDlgAddMerge dlg;
			dlg.m_mode = dlg.M_READPAGE;
			dlg.m_merge_name = "pages:";
			CString str;
			while( read_file.ReadString( str ) )
			{
				if( str.Left(12) == "add_new_page" ||
					str.Left(11) == "rename_page" )
				{
					CString key;
					CArray<CString> pks;
					int np = ParseKeyString( &str, &key, &pks );
					if( np >= 2 )
						dlg.m_merge_name += " \"" + pks[0] + "\"";
				}
			}
			read_file.Close();
			if( dlg.DoModal() == IDOK )
				PasteFromFile( filename, FALSE, dlg.m_box_index );
			return;
		}
		//PasteFromFile(filename, FALSE );
	}
}

void CFreePcbDoc::PasteFromFile( CString pathname, BOOL bwDialog, int i_page )
{
	BOOL bDrag = 1;
	if (pathname.Right(8) == PCBV)
		bDrag = 0;
	else if( m_project_modified )
	{
		int ret = AfxMessageBox(G_LANGUAGE == 0 ?
			"Inserting data from external resources occurs when the file is saved, "\
			"since there is no way to cancel the action.Project modified, save it ? " :
			"Вставка данных из внешних ресурсов происходит при сохранённом файле, "\
			"т.к.не имеет возможности отмены действия. Проект изменен, сохранить его ? ", MB_OKCANCEL );
		if( ret == IDCANCEL )
			return;
		else
			OnFileSave();
		if( m_project_modified )
			return;
	}
	BOOL FRS = 1;
	if( pathname.GetLength() < 4 )
		FRS = 0;
	else if( pathname.Right(4).MakeLower() != ".cds" && pathname.Right(8).MakeLower() != ".pcbview")
		FRS = 0;
	//
	if( !FRS )
	{
		// error opening project file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? "Currently only cds files are supported.":"В настоящее время поддерживаются только файлы cds.");
		AfxMessageBox( mess );
		return;
	}
	// read project file
	CStdioFile pcb_file;
	int err = pcb_file.Open( pathname, CFile::modeRead, NULL );
	if( !err )
	{
		// error opening project file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? "Unable to open file %s" : "Невозможно открыть файл %s", pathname );
		AfxMessageBox( mess );
		return;
	}
	// clear clipboard objects to hold group
	clip_mlist->Clear();
	clip_outline_poly.RemoveAll();
	CMapStringToPtr cache_map;		// incoming footprints
	try
	{
		// get layers
		int fpos = 0;
		CString in_str = "";
		//float ver=1.000;
		int u = m_units;
		CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		if( !pMain ) 
			return;
		CString str;

		// read graphics
		str.Format( "Reading graphics");
		pMain->DrawStatus( 3, &str );
		pcb_file.Seek( fpos, CFile::begin );

		// read graphics
		if( i_page == -1 )
			ReadGraphics( &pcb_file, &clip_outline_poly );
		else
			ReadGraphics( &pcb_file, &clip_outline_poly, 0, 0, NULL, i_page );
		//
		pcb_file.Close();
		str.Format( "Complete");
		pMain->DrawStatus( 3, &str );
	}
	catch( CString * err_str )
	{
		// parsing error
		AfxMessageBox( *err_str );
		delete err_str;
		pcb_file.Close();
		return;
	}
	m_view->m_paste_flags = ~0; // paste from file
	m_view->OnGroupPaste(bwDialog, TRUE, 0, bDrag);
}

void CFreePcbDoc::OnFileGerbV(CString G, CString app)
{
	CString ALL_STR = "";
	CFileFind m_find_dir;
	BOOL bWorking = m_find_dir.FindFile(G+"\\*.*");  	
	while  (bWorking)  
	{  	
		bWorking = m_find_dir.FindNextFile();
		if( !m_find_dir.IsDots() && !m_find_dir.IsDirectory())   
		{  
			CString m_gName = m_find_dir.GetFileName();
			if( m_gName.Right(4).MakeLower() == ".gbr" || m_gName.Right(4).MakeLower() == ".drl" )
				ALL_STR += "\"" + m_find_dir.GetFilePath() + "\" ";
		}			
	}
	// TODO: Add your command handler code here
	SHELLEXECUTEINFO info = {0};
	CString fn;
	CString ps;
	fn.Format("\"%s\"", app);
	if( app.Find("ViewMate") > 0 )
		ps.Format("\"%s\"", G); 
	else
		ps = ALL_STR;		
	info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = NULL;
    info.hwnd = NULL;
    info.lpVerb = NULL;
    info.lpFile = fn;
    info.lpParameters = ps;
    info.lpDirectory = NULL;
    info.nShow = SW_SHOW;//SW_MAXIMIZE; //SW_HIDE
    info.hInstApp = NULL;
	int INF = ShellExecuteEx(&info);
	if( INF == 0 )  
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"There is a 'Shortcut' folder in the FreePcb directory. Put there a shortcut *.lnk referring to a Gerber file viewer program (for example Pentalogix's ViewMate).":
			"В каталоге СхемАтора есть папка 'Shortcut'. Поместите туда ярлык *.lnk, ссылающийся на программу просмотра файлов Gerber (например, ViewMate от Pentalogix).");
}

void CFreePcbDoc::OnEditUndo()
{
	if( m_undo_list->m_num_items > 0 )
	{
		// undo last operation unless dragging something
		if( !m_view->CurDragging() )
		{				
			while( m_undo_list->Pop() )
			{
			}
			m_view->CancelSelection( FALSE );
			m_view->m_draw_layer = 0;// OnEditUndo
			m_view->Invalidate( FALSE );
		}
		clip_outline_poly.RemoveAll();
		//m_bLastPopRedo = FALSE;
		ProjectModified( TRUE, FALSE );
	}
}

void CFreePcbDoc::OnEditRedo()
{
	if( m_redo_list->m_num_items > 0 )
	{
		// redo last operation unless dragging something
		//m_bLastPopRedo = TRUE;
		if( !m_view->CurDragging() )
		{
			int ipcb = Pages.GetCurrentPcbIndex();
			int sv_pos = m_part_renumber_pos;
			m_part_renumber_pos = PartRenumber[ipcb].GetSize();
			while( m_redo_list->Pop() )
			{
			}	
			m_view->CancelSelection( FALSE );
			m_part_renumber_pos = sv_pos;
			m_view->m_draw_layer = 0;// OnEditRedo
			m_view->Invalidate();
		}
		clip_outline_poly.RemoveAll();
		//m_bLastPopRedo = TRUE;
		ProjectModified( TRUE, FALSE );
	}
}

void CFreePcbDoc::ResetUndoState()
{
	m_undo_list->Clear();
	m_redo_list->Clear();
	RemoveAllInvisibleElements( this );
	//m_bLastPopRedo = FALSE;
}


void CFreePcbDoc::OnRepeatDrc()
{
	m_drelist->Clear();
	m_dlg_log->Clear();
	m_dlg_log->OnShowMe();
	DRC();
	m_view->Invalidate( FALSE );// (doubtful)
}

void CFreePcbDoc::OPRefreshID()
{
	//ids..
	for( int ipo=0; ipo<m_outline_poly->GetSize(); ipo++ )
	{
		CPolyLine * poly = &m_outline_poly->GetAt(ipo);
		id gid = poly->GetId();
		if( gid.i == ipo )
			continue;
		gid.i = ipo;
		poly->SetId( &gid );
		poly->Undraw();
		poly->Draw( m_dlist );
	}
}
int CFreePcbDoc::CombineBoardOutlines( int ib1, int ib2 )
{
	CPolyLine * poly1 = &m_outline_poly->GetAt(ib1);
	CPolyLine * poly2 = &m_outline_poly->GetAt(ib2);

	// polygons intersect, combine them
	int COUNT_CORNERS = poly1->GetNumCorners() + poly2->GetNumCorners();
	CArray<CArc> arc_array1;
	CArray<CArc> arc_array2;
	poly1->MakeGpcPoly( -1, &arc_array1 );
	poly2->MakeGpcPoly( -1, &arc_array2 );
	int n_ext_cont1 = 0;
	for( int ic=0; ic<poly1->GetGpcPoly()->num_contours; ic++ )
		if( !((poly1->GetGpcPoly()->hole)[ic]) )
			n_ext_cont1++;
	int n_ext_cont2 = 0;
	for( int ic=0; ic<poly2->GetGpcPoly()->num_contours; ic++ )
		if( !((poly2->GetGpcPoly()->hole)[ic]) )
			n_ext_cont2++;

	gpc_polygon * union_gpc = new gpc_polygon;//ok
	gpc_polygon_clip( GPC_UNION, poly1->GetGpcPoly(), poly2->GetGpcPoly(), union_gpc );

	// get number of outside contours
	int n_union_ext_cont = 0;
	for( int ic=0; ic<union_gpc->num_contours; ic++ )
		if( !((union_gpc->hole)[ic]) )
			n_union_ext_cont++;

	// if no intersection, free new gpc and return
	if( n_union_ext_cont == n_ext_cont1 + n_ext_cont2 )
	{
		gpc_free_polygon( union_gpc );
		delete union_gpc;
		return 0;
	}

	// intersection, replace ia1 with combined areas and remove ia2
	int hatch = poly1->GetHatch();
	int m_b_merge = poly1->GetMerge();
	id b_id = poly1->GetId();
	int layer = max( poly1->GetLayer(), poly2->GetLayer() );
	int w = poly1->GetW();
	int sel_box = poly1->GetSelBoxSize();

	// save undo info
	if( m_outline_poly->GetAt(ib1).GetNumCorners() )
	{
		id IDp = m_outline_poly->GetAt(ib1).GetId();
		IDp.sst = ID_SIDE;
		IDp.ii = 0;
		m_view->NewSelect( NULL, &IDp, 0, 0 );
	}
	if( m_outline_poly->GetAt(ib2).GetNumCorners() )
	{
		id IDp = m_outline_poly->GetAt(ib2).GetId();
		IDp.sst = ID_SIDE;
		IDp.ii = 0;
		m_view->NewSelect( NULL, &IDp, 0, 0 );
	}

	// create area with external contour
	int sz = m_outline_poly->GetSize();
	b_id.i = sz;
	int cnt = 0;
	for( int ic=0; ic<union_gpc->num_contours; ic++ )
	{
		if( !(union_gpc->hole)[ic] )
		{
			// external contour, replace this poly
			
			for( int i=0; i<union_gpc->contour[ic].num_vertices; i++ )
			{
				int x = ((union_gpc->contour)[ic].vertex)[i].x;
				int y = ((union_gpc->contour)[ic].vertex)[i].y;
				if( i == 0 )
				{
					cnt++;
					m_outline_poly->SetSize( sz+cnt );
					b_id.i = sz+cnt-1;
					m_outline_poly->GetAt(sz+cnt-1).SetDisplayList(m_dlist);
					m_outline_poly->GetAt(sz+cnt-1).Start( layer, w, 20*NM_PER_MIL, x, y, hatch, &b_id, NULL );
					m_outline_poly->GetAt(sz+cnt-1).SetMerge( m_b_merge );
					m_outline_poly->GetAt(sz+cnt-1).SetUtility(1);
					//
					// copy attrs
					for(int iatt=0; iatt<num_attributes; iatt++)
						if (m_outline_poly->GetAt(ib1).pAttr[iatt])
						{
							m_outline_poly->GetAt(sz + cnt - 1).pAttr[iatt] = m_outline_poly->GetAt(ib1).pAttr[iatt];
							m_outline_poly->GetAt(ib1).pAttr[iatt]->m_polyline_start = (sz + cnt - 1);
						}
						else if(m_outline_poly->GetAt(ib2).pAttr[iatt])
						{
							m_outline_poly->GetAt(sz + cnt - 1).pAttr[iatt] = m_outline_poly->GetAt(ib2).pAttr[iatt];
							m_outline_poly->GetAt(ib2).pAttr[iatt]->m_polyline_start = (sz + cnt - 1);
						}
				}
				else
					m_outline_poly->GetAt(sz+cnt-1).AppendCorner( x, y, CPolyLine::STRAIGHT, FALSE );
			}
			m_outline_poly->GetAt(sz+cnt-1).Close(CPolyLine::STRAIGHT);
		}
		if( (union_gpc->hole)[ic] )
		{
			// add holes
			for( int i=0; i<union_gpc->contour[ic].num_vertices; i++ )
			{
				int x = ((union_gpc->contour)[ic].vertex)[i].x;
				int y = ((union_gpc->contour)[ic].vertex)[i].y;
				m_outline_poly->GetAt(sz+cnt-1).AppendCorner( x, y, CPolyLine::STRAIGHT, FALSE );
			}
			m_outline_poly->GetAt(sz+cnt-1).Close(CPolyLine::STRAIGHT);
		}
	}
	m_outline_poly->GetAt(sz).RestoreArcs( &arc_array1 ); 
	m_outline_poly->GetAt(sz).RestoreArcs( &arc_array2 );
	for( int c=(m_outline_poly->GetAt(sz).GetNumContours()-1); c>=0; c-- )
		m_outline_poly->GetAt(sz).RecalcRectC(c);
	m_outline_poly->GetAt(sz).Hide();

	// save undo info
	for( int i=sz; i<sz+cnt; i++ )
	{
		// select new polylines
		if( m_outline_poly->GetAt(i).GetNumCorners() )
		{
			id IDp = m_outline_poly->GetAt(i).GetId();
			IDp.sst = ID_SIDE;
			IDp.ii = 0;
			m_view->NewSelect( NULL, &IDp, 0, 0 );
		}
	}
	// save U.I.
	m_view->SaveUndoInfoForGroup( m_undo_list );
	//  hide old polylines
	m_outline_poly->GetAt(ib1).Hide();
	m_outline_poly->GetAt(ib2).Hide();
	for( int i=sz; i<sz+cnt; i++ )
	{
		m_outline_poly->GetAt(i).Show();
		if( m_outline_poly->GetAt(i).pAttr[index_part_attr] )
			AttributeIndexing( m_outline_poly->GetAt(i).pAttr[index_part_attr], index_part_attr );
	}
	gpc_free_polygon( union_gpc );
	delete union_gpc;
	return 1;
}


void CFreePcbDoc::ClipOP ( int i )
{
	id IDp = m_outline_poly->GetAt(i).GetId();
	IDp.sst = ID_SIDE;
	IDp.ii = 0;
	m_view->NewSelect( NULL, &IDp, 0, 0 );
	m_view->SaveUndoInfoForOutlinePoly( i, TRUE, m_undo_list );
	m_view->CancelSelection();
	int mer = m_outline_poly->GetAt(i).GetMerge();
	int lay = m_outline_poly->GetAt(i).GetLayer();

	// save attributes
	CText* saveDesc[num_attributes];
	for (int att = 0; att < num_attributes; att++)
		saveDesc[att] = m_outline_poly->GetAt(i).pAttr[att];

	// combine
	CArray<CPolyLine*> * pa = new CArray<CPolyLine*>;//new017
	int n_poly = m_outline_poly->GetAt(i).NormalizeWithGpc( pa, 1 );
	m_outline_poly->GetAt(i).SetMerge(mer);

	// restore attributes
	for (int att = 0; att < num_attributes; att++)
		m_outline_poly->GetAt(i).pAttr[att] = saveDesc[att];

	int old_size = m_outline_poly->GetSize();
	if( n_poly > 1 )
	{
		for( int ip=1; ip<n_poly; ip++ )
		{
			// create new copper area and copy poly into it
			CPolyLine * new_p = (*pa)[ip-1];
			int szb = m_outline_poly->GetSize();
			m_outline_poly->SetSize(szb+1);
			id bid = new_p->GetId();
			bid.i = szb;
			// remove the poly that was automatically created for the new area
			// and replace it with a poly from NormalizeWithGpc
			m_outline_poly->GetAt(szb).Copy(new_p);
			m_outline_poly->GetAt(szb).SetMerge(mer);
			m_outline_poly->GetAt(szb).SetDisplayList( m_dlist );
			m_outline_poly->GetAt(szb).SetLayer(lay);
			m_outline_poly->GetAt(szb).SetId(&bid);
			for( int c=(m_outline_poly->GetAt(szb).GetNumContours()-1); c>=0; c-- )
				m_outline_poly->GetAt(szb).RecalcRectC(c);
			m_outline_poly->GetAt(szb).Hide();
			IDp.i = szb;
			m_view->NewSelect( NULL, &IDp, 0, 0 );
			delete new_p;
		}
	}
	// save undo info
	m_view->SaveUndoInfoForGroup( m_undo_list );
	for( int ipo=old_size; ipo<m_outline_poly->GetSize(); ipo++ )
	{
		// new polylines are visibled
		m_outline_poly->GetAt(ipo).Show();
	}
	delete pa;//new017
} 
void CFreePcbDoc::OnCreatePolyline()
{
	if( m_view->m_sel_count == 0 )
		return;
	
	m_view->MarkAllOutlinePoly(0,-1);
	BOOL found;
	int sel_i, sel_ii, W=0;
	int MARK = 7403;
	int sz = m_outline_poly->GetSize();
	int n_sides = 0;
	int cur_x=0, 
		cur_y=0, 
		cur_st=0, 
		first_x=0, 
		first_y=0,
		prevx=0,
		prevy=0,
		cur_x0=0,
		cur_y0=0,
		found_x=0,
		found_y=0,
		found_st=0,
		start=0;
	double min_dist=INT_MAX, max_dist = BY_ZERO;
	do
	{
		found = 0;
		sel_i = -1;
		sel_ii = -1;
		min_dist=INT_MAX;
		max_dist = BY_ZERO;
		for( int i=0; i<sz; i++ )
		{
			CPolyLine * p = &m_outline_poly->GetAt(i);
			if( p->m_visible )
				if( p->GetSideSel() )
				{
					W = p->GetW();
					for( int ii=0; ii<p->GetNumSides(); ii++ )
					{
						if( p->GetUtility(ii) == MARK )
							continue;
						if( p->GetSideSel(ii) )
						{
							int iin = p->GetNextCornerIndex(ii);
							int gx1 = p->GetX(ii);
							int gy1 = p->GetY(ii);
							int gx2 = p->GetX(iin);
							int gy2 = p->GetY(iin);
							int st = p->GetSideStyle(ii);
							//-------------------
							if( n_sides )
							{
								double dist1 = Distance( cur_x, cur_y, gx1, gy1 );
								double dist2 = Distance( cur_x, cur_y, gx2, gy2 );
								if( dist1 - min_dist < 10 || dist2 - min_dist < 10 )
								{
									if( dist2 < dist1 )
									{
										// swap
										dist1 = dist1 + dist2;
										dist2 = dist1 - dist2;
										dist1 = dist1 - dist2;
										gx1 = gx1 + gx2;
										gx2 = gx1 - gx2;
										gx1 = gx1 - gx2;
										gy1 = gy1 + gy2;
										gy2 = gy1 - gy2;
										gy1 = gy1 - gy2;
										if(st)
											st = 3 - st;
									}
									int x0=0, y0=0;
									double cor2toSeg = ::GetClearanceBetweenSegments(	prevx, prevy, cur_x, cur_y, st, _2540, 
																				gx2, gy2, gx2+10, gy2+10, CPolyLine::STRAIGHT, _2540, 2*NM_PER_MM, &x0, &y0 );
									if( //cor2toSeg > W/10 &&
										(dist1 < min_dist || cor2toSeg > max_dist) )
									{
										found = 1;
										min_dist = min(min_dist,dist1);
										if( min_dist < W/10 )
											max_dist = max(max_dist,cor2toSeg);
										found_x = gx2;
										found_y = gy2;
										found_st = st;
										cur_x0 = gx1;
										cur_y0 = gy1;
										sel_i = i;
										sel_ii = ii;
									}
									else
									{
										//ignore
										p->SetUtility( ii, MARK );
										continue;
									}
								}
								else
								{
									int x0=0, y0=0;
									int cortoSeg = ::GetClearanceBetweenSegments(	gx1, gy1, gx2, gy2, st, _2540, 
																				cur_x, cur_y, cur_x+10, cur_y+10, 
																				CPolyLine::STRAIGHT, _2540, 2*NM_PER_MM, &x0, &y0 );
									if( cortoSeg < W/10 )
									{
										int g1toPoly = INT_MAX;
										int g2toPoly = INT_MAX;
										for( int ico=0; ico<m_outline_poly->GetAt(sz).GetNumSides(); ico++ )
										{
											int inxt =  m_outline_poly->GetAt(sz).GetNextCornerIndex(ico);
											int getx1 = m_outline_poly->GetAt(sz).GetX(ico);
											int gety1 = m_outline_poly->GetAt(sz).GetY(ico);
											int getx2 = m_outline_poly->GetAt(sz).GetX(inxt);
											int gety2 = m_outline_poly->GetAt(sz).GetY(inxt);
											int getst = m_outline_poly->GetAt(sz).GetSideStyle(ico);
											int d = ::GetClearanceBetweenSegments(	getx1, gety1, getx2, gety2, getst, _2540, 
																				gx1, gy1, gx1+10, gy1+10, 
																				CPolyLine::STRAIGHT, _2540, 2*NM_PER_MM, &x0, &y0 );
											g1toPoly = min( g1toPoly, d );
											d = ::GetClearanceBetweenSegments(	getx1, gety1, getx2, gety2, getst, _2540, 
																				gx2, gy2, gx2+10, gy2+10, 
																				CPolyLine::STRAIGHT, _2540, 2*NM_PER_MM, &x0, &y0 );
											g2toPoly = min( g2toPoly, d );
										}	
										if( g1toPoly < W/10 && g2toPoly < W/10 )
										{
											//ignore
											p->SetUtility( ii, MARK );
											continue;
										}
										dist1 = Distance( first_x, first_y, gx1, gy1 );
										if( g2toPoly < g1toPoly || (int)dist1 < W/10 )
										{
											// swap
											gx1 = gx1 + gx2;
											gx2 = gx1 - gx2;
											gx1 = gx1 - gx2;
											gy1 = gy1 + gy2;
											gy2 = gy1 - gy2;
											gy1 = gy1 - gy2;
											if(st)
												st = 3 - st;
										}
										dist2 = Distance( cur_x, cur_y, gx2, gy2 );
										if( dist2 > max_dist )
										{
											found = 1;
											min_dist = min(min_dist,cortoSeg);
											if( min_dist < W/10 )
												max_dist = max(max_dist,dist2);
											found_x = gx2;
											found_y = gy2;
											found_st = st;
											cur_x0 = cur_x;
											cur_y0 = cur_y;
											sel_i = i;
											sel_ii = ii;
										}
										else
										{
											//ignore
											p->SetUtility( ii, MARK );
											continue;
										}
									}
								}
							}
							else if( sel_i == -1 || sel_ii == -1 )
							{
								sel_i = i;
								sel_ii = ii;
							}
							else if(	gy1 < m_outline_poly->GetAt(sel_i).GetY( sel_ii ) ||
										gy2 < m_outline_poly->GetAt(sel_i).GetY( sel_ii )    )
							{
								sel_i = i;
								sel_ii = ii;
							}
 						}
					}
				}
		}
		if( n_sides == 0 && sel_ii >= 0 )
		{
			int iin = m_outline_poly->GetAt(sel_i).GetNextCornerIndex(sel_ii);
			int gx1 = m_outline_poly->GetAt(sel_i).GetX(sel_ii);
			int gy1 = m_outline_poly->GetAt(sel_i).GetY(sel_ii);
			int gx2 = m_outline_poly->GetAt(sel_i).GetX(iin);
			int gy2 = m_outline_poly->GetAt(sel_i).GetY(iin);
			cur_st = m_outline_poly->GetAt(sel_i).GetSideStyle(sel_ii);
			if( gy2 < gy1 )
			{
				// swap
				gx1 = gx1 + gx2;
				gx2 = gx1 - gx2;
				gx1 = gx1 - gx2;
				gy1 = gy1 + gy2;
				gy2 = gy1 - gy2;
				gy1 = gy1 - gy2;
				if(cur_st)
					cur_st = 3 - cur_st;
			}
			m_outline_poly->SetSize(sz+1);
			m_outline_poly->GetAt(sz).SetDisplayList( m_dlist );
			m_outline_poly->GetAt(sz).m_visible = 0;
			id sel_id( ID_POLYLINE, ID_GRAPHIC, sz );
			m_outline_poly->GetAt(sz).SetId( &sel_id );
			m_view->SaveUndoInfoForOutlinePoly( sz, 1, m_undo_list );
			m_outline_poly->GetAt(sz).Start( LAY_FREE_LINE, NM_PER_MIL, NM_PER_MIL, gx1, gy1, 50, &sel_id, NULL );
			m_outline_poly->GetAt(sz).m_visible = 1;
			first_x = gx1;
			first_y = gy1;
			prevx = gx1;
			prevy = gy1;
			cur_x = gx2;
			cur_y = gy2;
			found = 1;
		}
		if( found )
		{
			if( n_sides > 0 )
			{
				int dist1 = Distance( cur_x, cur_y, cur_x0, cur_y0 );
				//int dist2 = Distance( cur_x, cur_y, first_x, first_y );
				//int dist3 = Distance( cur_x0, cur_y0, first_x, first_y );
				//if( dist2 <= dist1 || dist3 < W/10 )
				//{
				//	m_outline_poly->GetAt(sz).Close( 0 );
				//	first_x = cur_x0;
				//	first_y = cur_y0;
				//	m_outline_poly->GetAt(sz).AppendCorner( first_x, first_y, 0 );
				//}
				if( dist1 > W/2 )
					//m_outline_poly->GetAt(sz).AppendCorner( cur_x0, cur_y0, 0 );
				{
					int ncont = m_outline_poly->GetAt(sz).GetNumCorners()-start;
					if( ncont > 3 )
						m_outline_poly->GetAt(sz).Close( 0 );
					else if( start )
						m_outline_poly->GetAt(sz).DeleteCorner( start, ncont );
					start = m_outline_poly->GetAt(sz).GetNumCorners();
					first_x = cur_x0;
					first_y = cur_y0;
					m_outline_poly->GetAt(sz).AppendCorner( first_x, first_y, 0 );
				}
				prevx = cur_x0;
				prevy = cur_y0;
				cur_x = found_x;
				cur_y = found_y;
				cur_st = found_st;
			}
			m_outline_poly->GetAt(sz).AppendCorner( cur_x, cur_y, cur_st );
			m_outline_poly->GetAt(sel_i).SetUtility( sel_ii, MARK );
			n_sides++;
		}
	}while( found );
	if( n_sides > 2 )
	{
		int ncont = m_outline_poly->GetAt(sz).GetNumCorners()-start;
		if( ncont > 3 )
			m_outline_poly->GetAt(sz).Close( 0 );
		else if( start )
			m_outline_poly->GetAt(sz).DeleteCorner( start, ncont );
	}
	OnRangeCmds(NULL);
	ProjectModified(TRUE);
}
void CFreePcbDoc::OnProjectCombineBoard()
{
	if( getbit( m_view->m_sel_flags, m_view->FLAG_SEL_OP ) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "No selected polylines!":"Нет выбранных полилиний!", MB_ICONINFORMATION);
		return;
	}
	if( m_outline_poly->GetSize() > 0 )
	{
		// start by testing all polygons to set utility flags
		for( int ib=m_outline_poly->GetSize()-1; ib>=0; ib-- )
		{
			int ret = m_outline_poly->GetAt(ib).TestPolygonIntersection( -1, -1 );
			if( ret )
				ClipOP(ib);
		}
		// now loop through all combinations
		int mem_size = m_outline_poly->GetSize();
		BOOL message_shown = FALSE;
		BOOL CC, AT=TRUE, CL=TRUE, OK=FALSE;
		do
		{
			CC = FALSE;
			for( int i1=m_outline_poly->GetSize()-1; i1>=0; i1-- ) 
			{
				CPolyLine * poly1 = &m_outline_poly->GetAt(i1);
				if( poly1->m_visible == 0 )
					continue;
				if( poly1->GetSideSel() == 0 )
					continue;
				if( poly1->GetClosed() == 0 )
				{
					CL = 0;
					continue;
				}
				if( poly1->GetLayer() == 0 )
					ASSERT(0);
				// legal polygon
				RECT b1 = poly1->GetCornerBounds(0);
				BOOL mod_i1 = FALSE;
				for( int i2=m_outline_poly->GetSize()-1; i2 > i1; i2-- )
				{
					CPolyLine * poly2 = &m_outline_poly->GetAt(i2);
					if( poly2->m_visible == 0 )
						continue;
					if( poly2->GetLayer() == 0 )
						ASSERT(0);
					// check layer
					if( poly1->GetLayer() == LAY_FREE_LINE )
					{
					}
					else if( poly2->GetLayer() == LAY_FREE_LINE )
					{
					}
					else if( poly2->GetLayer() != poly1->GetLayer() )
						continue;
					//
					if( poly2->GetSideSel() == 0 )
						continue;
					if( poly2->GetClosed() == 0 )
					{
						CL = 0;
						continue;
					}
					else 
					{
						if( poly1->pAttr[index_part_attr] != poly2->pAttr[index_part_attr] ||
						    poly1->pAttr[index_value_attr] != poly2->pAttr[index_value_attr] ||
							poly1->pAttr[index_foot_attr] != poly2->pAttr[index_foot_attr] )
						{
							AT = 0;
							continue;
						}
						else if(poly1->pAttr[index_pin_attr] && poly2->pAttr[index_pin_attr] ||
								poly1->pAttr[index_net_attr] && poly2->pAttr[index_net_attr] ||
								poly1->pAttr[index_desc_attr] && poly2->pAttr[index_desc_attr] )
						{
							AT = 0;
							continue;
						}
					}
					if( m_outline_poly->GetAt(i1).GetUtility() != -1 && 
						m_outline_poly->GetAt(i2).GetUtility() != -1 )
					{
						RECT b2 = m_outline_poly->GetAt(i2).GetCornerBounds(0);
						if( !( b1.left > b2.right || b1.right < b2.left
							|| b1.bottom > b2.top || b1.top < b2.bottom ) )
						{
							int ret = m_outline_poly->GetAt(i1).TestPolygonIntersection( &m_outline_poly->GetAt(i2) );
							if( ret == 1 || ret == 2 )
							{
								OK = 1;
								ret = CombineBoardOutlines( i1, i2 );
								//m_outline_poly->GetAt(i1).SetUtility(-1); ???????????
								//m_outline_poly->GetAt(i2).SetUtility(-1); ???????????
							}
							if( ret == 1 || ret == 2 )
							{
								CC = TRUE;
								CString str;
								str.Format( "Polylines %d and %d intersect and will be combined.\n",
									i1+1, i2+1 );
								str += "If they are complex, this may take a few seconds.";
								CDlgMyMessageBox dlg;
								dlg.Initialize( str, IDB_BITMAP_INTERSECT );
								dlg.DoModal();
								mod_i1 = TRUE;
							}				
						}
					}
				}
				//if( mod_i1 ) ??????????????????????
				//	i1--;		// if modified, we need to check it again
			}
		}while(CC);
		if( CL == 0 )
			AfxMessageBox(G_LANGUAGE == 0 ? "Polyline must be closed for this option.":"Для этой опции полилиния должна быть замкнутой.", MB_ICONINFORMATION);
		else if( AT == 0 )
			AfxMessageBox(G_LANGUAGE == 0 ? "You cannot combine polylines with different attributes.":"Нельзя комбинировать полилинии с разными атрибутами.", MB_ICONERROR);
		else if( OK == 0 )
			AfxMessageBox(G_LANGUAGE == 0 ? "Selected polylines do not intersect or polyline layers are different.":"Выбранные полилинии не пересекаются или слои полилиний различны.", MB_ICONINFORMATION);
		else
			ProjectModified( TRUE );
	}
	m_view->OnRangeCmds(NULL);
}

int CFreePcbDoc::DRC()
{
	// GO !
	int cur_page = Pages.GetActiveNumber();
	int iPCB = Pages.GetPcbIndex( cur_page );
	int n_err = 0;
	int mem_err = n_err;
	int err_page = -1;
	if( m_parts_checked[cur_page] == 0 && iPCB >= 0 )
	{
		m_dlg_log->AddHeader( "Design validation across all pages", 3 );
		m_dlg_log->AddParagraph( "Verification of complex parts" );
		m_dlg_log->UpdateWindow();
		CString CParts = (CString)dSPACE + dSPACE;
		int it = -1;
		int ip = -1;
		for( CText * t=Pages.GetNextAttr( index_desc_attr, &it, &ip, TRUE ); t; t=Pages.GetNextAttr( index_desc_attr, &it, &ip, TRUE ) )
		{
			// 
			if( t->m_str.Right(3) == "BLK" )
			{
				CString cmd, lnk;
				ExtractBLK( &t->m_str, &cmd, &lnk );
				if( cmd.GetLength() )
				{
					if( CParts.Find( dSPACE + lnk + cmd + dSPACE ) >= 0 )
					{
						CString s;
						s.Format("%d: Complex parts have the same suffixes %s\r\n", ++n_err, cmd );
						id id1( ID_TEXT_DEF );
						id id2( ID_TEXT_DEF );
						// ERROR
						n_err++;
						DRErrorList * dr = Pages.GetDRErrorList(ip);
						DRError * dre = dr->Add( &s, id1, id2, 
							t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						err_page = ip;
					}
					else
						CParts += (lnk + cmd + dSPACE);
					//
					CArray<CPolyLine> * pa = Pages.GetPolyArray( ip );
					CPolyLine * poly = &pa->GetAt( t->m_polyline_start );
					CString err = m_view->SelectBlock( poly, ip );
					if( err.GetLength() )
					{
						CString s;
						s.Format("%d: %s\r\n", ++n_err, err );
						id id1( ID_TEXT_DEF );
						id id2 = poly->GetId();
						// ERROR
						n_err++;
						DRErrorList * dr = Pages.GetDRErrorList(ip);
						DRError * dre = dr->Add( &s, id1, id2, 
							t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, NULL );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						err_page = ip;
					}
				}	
			}
		}
		it = -1;
		ip = -1;
		if( n_err == 0 )// !!!
		{
			CString Checked = dSPACE;
			for( CText * t=Pages.GetNextAttr( index_desc_attr, &it, &ip, TRUE ); t; t=Pages.GetNextAttr( index_desc_attr, &it, &ip, TRUE ) )
			{
				if( t->m_str.Right(7) == "BLK_PTR" )
				{
					CString NAME, REF, PIN;
					int cpp = ExtractBLK_PTR( &t->m_str, &NAME, &REF, &PIN );
					if( CParts.Find( dSPACE + REF + NAME + dSPACE ) < 0 )
					{
						// Error
						CString s;
						s.Format("%d: A complex part is missing a contour polyline (suffix %s)\r\n", ++n_err, NAME );
						id id1( ID_TEXT_DEF );
						id id2( ID_TEXT_DEF );
						// ERROR
						n_err++;
						DRErrorList * dr = Pages.GetDRErrorList(ip);
						DRError * dre = dr->Add( &s, id1, id2, 
							t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						err_page = ip;
						break;
					}
					if( Checked.Find( dSPACE + REF + NAME + dSPACE ) < 0 )
					{
						Checked +=  (REF + NAME + dSPACE);
						int old_p = SwitchToPage( cpp );
						int it2 = -1;
						CText * MainRef = Attr->m_Reflist->GetText(&REF,&it2);
						if( MainRef )
						{
							RECT MR = GetPartBounds( MainRef, NULL );
							it2 = -1;
							for( CText * ref=Attr->m_Reflist->GetNextText(&it2); ref; ref=Attr->m_Reflist->GetNextText(&it2) )
							{
								RECT R = GetPartBounds( ref, NULL );
								if( RectsIntersection( MR, R ) > 0 )// part inside part
								{
									CString CpartName = ref->m_str + NAME;
									if( Pages.FindPart( &CpartName, NULL, TRUE, TRUE ) >= 0 )
									{
										CString s;
										s.Format("%d: Duplicate Reference Designator %s\r\n", ++n_err, CpartName );
										id id1( ID_TEXT_DEF );
										id id2( ID_TEXT_DEF );
										// ERROR
										n_err++;
										DRErrorList * dr = Pages.GetDRErrorList(ip);
										DRError * dre = dr->Add( &s, id1, id2, 
											t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t );
										m_dlg_log->AddLine(s);
										m_dlg_log->UpdateWindow();
										err_page = ip;
									}
								}
							}
						}
						else
						{
							CString s;
							s.Format("%d: Reference Designator %s not found on page %d\r\n", ++n_err, REF, cpp+1 );
							id id1( ID_TEXT_DEF );
							id id2( ID_TEXT_DEF );
							// ERROR
							n_err++;
							DRErrorList * dr = Pages.GetDRErrorList(ip);
							DRError * dre = dr->Add( &s, id1, id2, 
								t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t );
							m_dlg_log->AddLine(s);
							m_dlg_log->UpdateWindow();
							err_page = ip;
						}
						SwitchToPage( old_p );
					}
				}
			}
		}
		if( n_err == 0 )
			m_dlg_log->AddLine( "    - no errors\r\n" );
		//
		m_dlg_log->AddParagraph( "Verification of electronic buses" );
		m_dlg_log->UpdateWindow();
		CString Buses = dSPACE;
		it = -1;
		ip = -1;
		for( CText * t=Pages.GetNextAttr( index_net_attr, &it, &ip, TRUE ); t; t=Pages.GetNextAttr( index_net_attr, &it, &ip, TRUE ) )
		{
			int p = t->m_str.Find("|");
			if( p > 0 && t->m_polyline_start >= 0 )
			{
				//
				CString post_s = t->m_str.Right( t->m_str.GetLength() - p - 1 );
				CArray<CPolyLine> * op = Pages.GetPolyArray(ip);
				RECT lr = op->GetAt(t->m_polyline_start).GetBounds();
				int it2 = -1;
				CTextList * D = Pages.GetTextList( ip, index_desc_attr );
				CText * bus = D->GetText( &post_s, &it2 );
				BOOL busFound = FALSE;
				BOOL Ok = FALSE;
				if( bus )
				{
					if( bus->m_polyline_start >= 0 )
					{
						busFound = 1;
						CPolyLine * pBus = &op->GetAt(bus->m_polyline_start);
						///if( pBus->GetClosed() == 0 )???// !!!
						{
							for( int ic=0; ic<pBus->GetNumSides(); ic++ )
							{
								RECT pr = rect(	pBus->GetX(ic),
												pBus->GetY(ic),
												pBus->GetX(pBus->GetNextCornerIndex(ic)),
												pBus->GetY(pBus->GetNextCornerIndex(ic)));
								SwellRect(&pr, pBus->GetW()*2/5);
								if( RectsIntersection( lr, pr ) >= 0 )
									Ok = 1;
							}
						}
					}
				}
				if( Ok == 0 )
				{
					// ERROR
					CString s="";
					if( busFound )
					{
	
						s.Format("%d: Electronic bus %s does not intersect mark %s\r\n", ++n_err, post_s, t->m_str );
						id id1( ID_TEXT_DEF );
						id id2 = op->GetAt(bus->m_polyline_start).GetId();
						// ERROR
						DRErrorList * dr = Pages.GetDRErrorList(ip);
						DRError * dre = dr->Add( &s, id1, id2, 
							t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, NULL );
						err_page = ip;
					}
					else
					{
						int pgs = 0;
						if( Pages.GetNetPages(t,&pgs,ip) == 0 )
						{
							s.Format("%d: Not found electronic bus %s for net label %s\r\n", ++n_err, post_s, t->m_str );
							id id1( ID_TEXT_DEF );
							id id2;
							// ERROR
							DRErrorList * dr = Pages.GetDRErrorList(ip);
							DRError * dre = dr->Add( &s, id1, id2, 
								t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, NULL );
							err_page = ip;
						}
						else 
						{
							if( Buses.Find( dSPACE+post_s+dSPACE ) < 0 )
							{
								s.Format("WARNING: Electronic bus %s is multi-page\r\n", post_s );
								Buses += post_s + dSPACE;
							}
						}
					}
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
				}
				else
				{
					CTextList * N = Pages.GetTextList( ip, index_net_attr );
					it2 = -1;
					CText * t2 = N->GetText(&t->m_str, &it2);
					if( t2 == t )
						t2 = N->GetText(&t->m_str, &it2);
					if( t2 == NULL )
					{
						CString s="";
						s.Format("%d: Missing second mark %s\r\n", ++n_err, t->m_str );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						id id1( ID_TEXT_DEF );
						id id2;
						// ERROR
						DRErrorList * dr = Pages.GetDRErrorList(ip);
						DRError * dre = dr->Add( &s, id1, id2, 
							t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, NULL );
						err_page = ip;
					}
				}
			}
		}
	
		if( n_err == 0 )
			m_dlg_log->AddLine( "    - no errors\r\n" );
		//
		//
		m_dlg_log->AddParagraph( "Verification of part designations" );
		m_dlg_log->UpdateWindow();
		mem_err = n_err;
		it = -1;
		ip = -1;
		for( CText * t=Pages.GetNextAttr( index_part_attr, &it, &ip, TRUE ); t; t=Pages.GetNextAttr( index_part_attr, &it, &ip, TRUE ) )
		{
			DRErrorList * dr = Pages.GetDRErrorList(ip);
			CArray<CPolyLine> * pa = Pages.GetPolyArray(ip);
			//CString pref, suff;
			//int nmbr = ParseRef( &t->m_str, &pref, &suff );
			//if( nmbr < 1 )
			//{
//
			//}
			// check on footprint error
			CText * F = pa->GetAt( t->m_polyline_start ).pAttr[index_foot_attr];
			if( F )
			{
				if( F->m_str.Compare(NO_FOOTPRINT) == 0 )
				{
					if( GetNumPins( t, pa ) )
					{
						// ERROR
						CString s;
						s.Format("%d: Part %s without footprint has electrical pins\r\n", ++n_err, t->m_str );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						id id1( ID_TEXT_DEF );
						id id2( ID_TEXT_DEF );
						// ERROR
						DRError * dre = m_drelist->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t );
					}
				}
			}
			//
			int ERR = 0;
			int get = it;
			CText * gt = NULL;
			int ipg1 = Pages.FindPart( &t->m_str, &gt, TRUE, TRUE, it, ip );
			int ipg2 = -1;
			if( gt )
			{
				// ERROR
				CString s;
				s.Format("%d: Part name duplication on pages included in netlist %s. (page %d, page %d)\r\n", ++n_err, t->m_str, ipg1+1, ip+1 );
				m_dlg_log->AddLine(s);
				m_dlg_log->UpdateWindow();
				id id1( ID_TEXT_DEF );
				id id2( ID_TEXT_DEF );
				// ERROR
				DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, gt );
				ERR = 0;
				err_page = ip;
			}
			//
			CString str = t->m_str;
			if( str.FindOneOf( ILLEGAL_REF ) != -1 )
			{
				// ERROR
				CString s;
				s.Format("%d: Illegal reference designator %s\r\n", ++n_err, t->m_str );
				m_dlg_log->AddLine(s);
				m_dlg_log->UpdateWindow();
				id id1( ID_TEXT_DEF );
				id id2;
				// ERROR
				DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t );
				err_page = ip;
			} 
			int dot = t->m_str.Find(".");
			if( dot == 0 )
				ERR = 1;
			else if( dot > 0 )
			{
				str = t->m_str.Left(dot);
				Pages.FindPart( &str, &gt, TRUE, TRUE );
				if( gt )
					ERR = 1;
			}
			int tire = t->m_str.Find("-");
			if( tire == 0 )
				ERR = 1;
			else if( tire > 0 )
			{
				if( dot > 0 )
				{
					// ERROR
					CString s;
					s.Format("%d: Illegal reference designator %s\r\n", ++n_err, t->m_str );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1( ID_TEXT_DEF );
					id id2;
					// ERROR
					DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t );
					ERR = 0;
					err_page = ip;
				}
				else
				{
					str = t->m_str.Left(tire);
					Pages.FindPart( &str, &gt, TRUE, TRUE );
					if( gt )
						ERR = 1;
				}
			}
			if( ERR == 0 && (dot > 0 || tire > 0) )
			{
				CString str_dot = t->m_str.Left(max(dot,tire));
				CString str_fail = str_dot;
				if( dot > 0 )
				{
					str_dot += ".";
					str_fail += "-";
				}
				else
				{
					str_dot += "-";
					str_fail += ".";
				}
				//
				int i_st = t->m_polyline_start;
				if( i_st == -1 )
				{
					AttributeIndexing(t,index_part_attr); // m_polyline_start == -1
					i_st = t->m_polyline_start;
					if( i_st == -1 )
					{
						AfxMessageBox(G_LANGUAGE == 0 ? 
							"Fatal! Attribute indexing does not work":
							"Фатальная ошибка! Индексация атрибутов не работает");
						return err_page;
					}
				}
				CText * v = pa->GetAt(i_st).pAttr[index_value_attr];
				CText * f = pa->GetAt(i_st).pAttr[index_foot_attr];
				int bF = 0; // number of found
				int it2 = -1;
				int ip2 = -1;
				for( CText * t2=Pages.GetNextAttr( index_part_attr, &it2, &ip2, TRUE ); t2; t2=Pages.GetNextAttr( index_part_attr, &it2, &ip2, TRUE ) )
				{
					if( t2 == t )
						continue;
					if( str_dot.GetLength() < t2->m_str.GetLength() )
					{
						CString t3 = t2->m_str.Left( str_dot.GetLength() );
						if( str_dot.CompareNoCase( t3 ) == 0 )
						{
							bF++;
							if( ip2 > ip || (it2 > it && ip == ip2) )
							{
								CArray<CPolyLine> * pa2 = Pages.GetPolyArray(ip2);
								if( t2->m_polyline_start >= 0 )
								{
									CText * v2 = pa2->GetAt(t2->m_polyline_start).pAttr[index_value_attr];
									CText * f2 = pa2->GetAt(t2->m_polyline_start).pAttr[index_foot_attr];
									BOOL bDiff = 0;
									if( v && v2 )
										if( v->m_str.Compare(v2->m_str) )
											bDiff = TRUE;
									if( f && f2 )
										if( f->m_str.Compare(f2->m_str) )
											bDiff = TRUE;
									if( (v && v2 == NULL) || (f && f2 == NULL) || (v2 && v == NULL) || (f2 && f == NULL) || bDiff )
									{
										// ERROR
										CString s;
										s.Format("%d: Different attributes for the multiplex part (ref: %s and %s)\r\n", ++n_err, t->m_str, t2->m_str );
										m_dlg_log->AddLine(s);
										m_dlg_log->UpdateWindow();
										id id1( ID_TEXT_DEF );
										id id2( ID_TEXT_DEF );
										// ERROR
										DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, t2 );
										err_page = ip;
									}
								}
							}
						}
						if( str_fail.CompareNoCase( t3 ) == 0 )
						{
							// ERROR
							CString s;
							s.Format("%d: Different separators %s %s\r\n", ++n_err, t->m_str, t2->m_str );
							m_dlg_log->AddLine(s);
							m_dlg_log->UpdateWindow();
							id id1( ID_TEXT_DEF );
							id id2( ID_TEXT_DEF );
							// ERROR
							DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, gt );
							err_page = ip;
						}
					}
				}
				if( bF == 0 )
				{
					// ERROR
					CString s;
					if( dot > 0 )
						s.Format("%d: The part name %s has a separator '.', which indicates that this part should have several graphic symbols on the working field.\r\n", ++n_err, t->m_str );
					else
						s.Format("%d: The part name %s has a separator '-', which indicates that this part should have several graphic symbols on the working field.\r\n", ++n_err, t->m_str );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1( ID_TEXT_DEF );
					id id2( ID_TEXT_DEF );
					// ERROR
					DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, gt );
					err_page = ip;
				}
				else
				{
					//CString 
				}
			}
			if( ERR )
			{
				// ERROR
				CString s;
				s.Format("%d: Invalid combination %s and %s\r\n", ++n_err, str, t->m_str );
				m_dlg_log->AddLine(s);
				m_dlg_log->UpdateWindow();
				id id1( ID_TEXT_DEF );
				id id2( ID_TEXT_DEF );
				// ERROR
				DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, gt );
				ERR = 0;
				err_page = ip;
			}
		}
		//
		if( n_err == mem_err )
			m_dlg_log->AddLine( "    - no errors\r\n" );
		//
		//
		m_dlg_log->AddParagraph( "Verification of pin names" );
		m_dlg_log->UpdateWindow();
		CString FOOTS = dSPACE;
		mem_err = n_err;
		it = -1;
		ip = -1;
		for( CText * t=Pages.GetNextAttr( index_pin_attr, &it, &ip, TRUE ); t; t=Pages.GetNextAttr( index_pin_attr, &it, &ip, TRUE ) )
		{
			DRErrorList * dr = Pages.GetDRErrorList(ip);
			//
			//
			//
			if( t->m_polyline_start == -1 )
			{
				// ERROR
				CString s;
				s.Format("%d: Fatal error: lost pin index %s\r\n", ++n_err, t->m_str );
				m_dlg_log->AddLine(s);
				m_dlg_log->UpdateWindow();
				id id1( ID_TEXT_DEF );
				id id2;
				// ERROR
				DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t );
				err_page = ip;
			}
			else
			{
				CArray<CPolyLine> * po = Pages.GetPolyArray( ip );
				if( po->GetAt(t->m_polyline_start).GetClosed() )
				{
					// ERROR
					CString s;
					s.Format("%d: Pin must not be a closed polyline (pin %s)\r\n", ++n_err, t->m_str );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1( ID_TEXT_DEF );
					id id2;
					// ERROR
					DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t );
					err_page = ip;
				}
				CText * part = po->GetAt(t->m_polyline_start).pAttr[index_part_attr]; 
				if( part == NULL )
				{
					// ERROR
					CString s;
					s.Format("%d: Missing reference designator (pin %s)\r\n", ++n_err, t->m_str );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1( ID_TEXT_DEF );
					id id2;
					// ERROR
					DRError * dre = dr->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t );
					err_page = ip;
				}
				int it2 = it;
				int ip2 = ip;
				if( part )for( CText * t2=Pages.GetNextAttr( index_pin_attr, &it2, &ip2, TRUE ); t2; t2=Pages.GetNextAttr( index_pin_attr, &it2, &ip2, TRUE ) )
				{
					// check for multipins
					//
					if( t->m_str.FindOneOf( MULTIPIN_TEST ) > 0 || t2->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
					{
						int MultiErr = 0;
						CArray<CString> arr1;
						CArray<CString> arr2;
						int c1 = ParseMultiPin( &t->m_str, &arr1 );
						int c2 = ParseMultiPin( &t2->m_str, &arr2 );
						if( !c1 || !c2 )
						{
							CString s="";
							if( !c1 )
								s.Format(G_LANGUAGE == 0 ? "Fix pin name %s in the part %s":"Исправьте имя пина %s в детали %s", t->m_str, part->m_str);
							else if( !c2 && t2->m_polyline_start >= 0 )
							{
								CArray<CPolyLine> * po2 = Pages.GetPolyArray( ip2 );
								CText * part2 = po2->GetAt(t2->m_polyline_start).pAttr[index_part_attr];
								if( part2 )
									s.Format(G_LANGUAGE == 0 ? 
										"Fix pin name %s in the part %s":
										"Исправьте имя пина %s в детали %s", t2->m_str, part2->m_str);
							}
							if( s.GetLength() )
								AfxMessageBox(s, MB_ICONINFORMATION);
							return err_page;
						}
						for( int a1=0; a1<arr1.GetSize(); a1++ )
						{
							for( int a2=0; a2<arr2.GetSize(); a2++ )
							{
								if( arr1[a1].CompareNoCase( arr2[a2] ) == 0 )
									MultiErr++;
							}
						}
						if( MultiErr == 0 )
							continue;
					}
					else if( t->m_str.CompareNoCase( t2->m_str ) )
						continue;
					if( t2->m_polyline_start >= 0 )
					{
						CArray<CPolyLine> * po2 = Pages.GetPolyArray( ip2 );
						CText * part2 = po2->GetAt(t2->m_polyline_start).pAttr[index_part_attr]; 

						int ERR = 0;
						if( part == part2 )
							ERR = 1;
						else if( part2 ) 
						{		
							CString str = part->m_str;
							int dot = part->m_str.Find(".");
							if( dot > 0 )
								str = part->m_str.Left(dot);
							else
							{
								dot = part->m_str.Find("-");
								if( dot > 0 )
									str = part->m_str.Left(dot);
							}
							CString str2 = part2->m_str;
							int dot2 = part2->m_str.Find(".");
							if( dot2 > 0 )
								str2 = part2->m_str.Left(dot);
							else
							{
								dot2 = part2->m_str.Find("-");
								if( dot2 > 0 )
									str2 = part2->m_str.Left(dot);
							}
							if( str.Compare(str2) == 0 )
								ERR = 1;
						}
						if( ERR )
						{
							BOOL bDiff = 0;
							CText * foot1 = po->GetAt(t->m_polyline_start).pAttr[index_foot_attr];
							CText * net1 = po->GetAt(t->m_polyline_start).pAttr[index_net_attr];
							CText * net2 = po->GetAt(t2->m_polyline_start).pAttr[index_net_attr];
							if( net1 && net2 == NULL )
								bDiff = 1;
							else if( net2 && net1 == NULL )
								bDiff = 1;
							else if( net1 && net2 )
							{
								if( net1->m_str.CompareNoCase( net2->m_str ) )
									bDiff = 1;
							}
							if( bDiff )
							{
								// ERROR
								CString s;
								s.Format("%d: Different nets on the same pins %s (%s)\r\n", ++n_err, t->m_str, part->m_str );
								m_dlg_log->AddLine(s);
								m_dlg_log->UpdateWindow();
								id id1( ID_TEXT_DEF );
								id id2( ID_TEXT_DEF );
								// ERROR
								DRError * dre = dr->Add( &s, id1, id2, 
									t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer,
									t, t2 );
								err_page = ip;
							}
							else
							{
								// WARNING
								CString s;
								if( foot1 )
								{
									if( FOOTS.Find( " "+foot1->m_str+dSPACE ) > 0 )
										continue;
									FOOTS += (dSPACE + foot1->m_str + dSPACE);
									s.Format("WARNING: Duplicate pin names %s (footprint: %s)\r\n", t->m_str, foot1->m_str );
								}
								else
									s.Format("WARNING: Duplicate pin names %s (%s)\r\n", t->m_str, part->m_str );
								m_dlg_log->AddLine(s);
								m_dlg_log->UpdateWindow();
								id id1( ID_TEXT_DEF );
								id id2( ID_TEXT_DEF );
								// WARNING
								DRError * dre = dr->Add( &s, id1, id2, 
									t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer,
									t, t2 );
								err_page = ip;
							}
						}
					}
				}
			}
		}
	
		if( n_err == mem_err )
			m_dlg_log->AddLine( "    - no errors\r\n" );
		if( n_err == 0 )
		{
			m_dlg_log->AddParagraph( "Verification of connections" );
			m_dlg_log->UpdateWindow();
			mem_err = n_err;
			for( int Active=0; Active<Pages.GetNumPages(); Active++ )
			{
				CTextList * nl = Pages.GetTextList( Active, index_net_attr );
				nl->MarkAllTexts(0);
			}
			CString SingleLabel = "\r\n";
			CString MultiNet = "\r\n";
			for( int ActiveNumber=0; ActiveNumber<Pages.GetNumPages(); ActiveNumber++ )
			{
				// checking pages related to the current pcb
				int get_index = Pages.GetPcbIndex( ActiveNumber ); 
				if( get_index != iPCB )
					continue;
				DRErrorList * dr = Pages.GetDRErrorList(ActiveNumber);
				CTextList * nl = Pages.GetTextList( ActiveNumber, index_net_attr );
				int it = -1;
				for( CText * net=nl->GetNextText(&it); net; net=nl->GetNextText(&it) )
				{
					if( net->utility == 0 )
					{
						int mask = 0;
						int CheckNet = Pages.GetNetPages( net, &mask, ActiveNumber );		
						if( CheckNet == 0 )
						{	
							int it_2 = it;
							if( nl->GetText( &net->m_str, &it_2 ) == NULL )
							{	
								SingleLabel += "        " + net->m_str + dSPACE + "\r\n";
								continue;
							}
							net->utility = 1;
							it_2 = it;
							for( CText * mn = nl->GetText( &net->m_str, &it_2 ); mn; mn = nl->GetText( &net->m_str, &it_2 ) )
								mn->utility = 1;
						}
						else if( net->m_str.Find("|") <= 0 )
							MultiNet += "        " + net->m_str + dSPACE + "\r\n";
						else
						{
							int BusI = net->m_str.Find("|");
							CString busName = net->m_str.Right(net->m_str.GetLength() - BusI - 1);
							if( MultiNet.Find(" " + busName + dSPACE) <= 0 )
								MultiNet += "        nets of electronic bus " + busName + dSPACE + "\r\n";
						}
						for( int markP=0; markP<Pages.GetNumPages(); markP++ )
						{
							CTextList * mark_nl = Pages.GetTextList( markP, index_net_attr );
							int it_2 = -1;
							for( CText * mn = mark_nl->GetText( &net->m_str, &it_2 ); mn; mn = mark_nl->GetText( &net->m_str, &it_2 ) )
								mn->utility = 1;
						}
					}
				}
				if( Pages.IsThePageIncludedInNetlist( ActiveNumber, TRUE ) )
				{
					int old_page = Pages.GetActiveNumber();
					Pages.SwitchToPage( ActiveNumber );
					it = -1;
					CTextList * tl = Pages.GetTextList( ActiveNumber, -1 );
					for( CText * net=tl->GetNextText(&it); net; net=tl->GetNextText(&it) )
					{
						int netX=0, netY=0;
						int Err = Pages.IncorrectElectricalCirquit( &net->m_str, &netX, &netY, ActiveNumber );
						if( Err == 2 )
						{
							CString s="";
							s.Format("%d: Pin short circuit (net %s)\r\n", ++n_err, net->m_str );
							m_dlg_log->AddLine(s);
							m_dlg_log->UpdateWindow();
							id id1;
							id id2;
							// ERROR
							DRError * dre = dr->Add( &s, id1, id2, 
								netX, netY, netX, netY, 20*NM_PER_MIL, net->m_layer, NULL, NULL );
							err_page = ActiveNumber;
						}
						if( Err == 1 )
						{
							CString s="";
							s.Format("%d: Incorrect electrical circuit (net %s)\r\n", ++n_err, net->m_str );
							m_dlg_log->AddLine(s);
							m_dlg_log->UpdateWindow();
							id id1;
							id id2;
							// ERROR
							DRError * dre = dr->Add( &s, id1, id2, 
								netX, netY, netX, netY, 20*NM_PER_MIL, net->m_layer, NULL, NULL );
							err_page = ActiveNumber;
						}
					}
					Pages.SwitchToPage( old_page );
				}
			}
			if( n_err == 0 )
			{
				if( SingleLabel.GetLength() > 3 )
				{
					// WARNING
					CString s;
					s.Format("These net labels are single: %s\r\n", SingleLabel );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
				}
				if( MultiNet.GetLength() > 3 )
				{
					// WARNING
					CString s;
					s.Format("These nets have cross-page connections: %s\r\n", MultiNet );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
				}
			}
			if( n_err == mem_err )
				m_dlg_log->AddLine( "    - no errors\r\n" );
		}
		//m_dlg_log->AddParagraph( "Verification of block names" );
		//m_dlg_log->UpdateWindow();
		//CArray<CString> * arr = Pages.GetBlockSuffixes();
		//if( arr )
		//{
		//	for( int item=0; item<arr->GetSize(); item++ )
		//	{
		//		for( int item2=item+1; item2<arr->GetSize(); item2++ )
		//		{
		//			if( arr->GetAt(item).CompareNoCase( arr->GetAt(item2) ) == 0 )
		//			{
		//				CString s="";
		//				s.Format("%d: It is not possible to duplicate block suffixes in the same PCB project.(%s)\r\n", ++n_err, arr->GetAt(item) );
		//				m_dlg_log->AddLine(s);
		//				m_dlg_log->UpdateWindow();
		//				id id1;
		//				id id2;
		//				// ERROR
		//				DRError * dre = m_drelist->Add( &s, id1, id2, 
		//					0, 0, 0, 0, 20*NM_PER_MIL, LAY_FREE_LINE, NULL, NULL );
		//			}
		//		}
		//	}
		//}
		//if( n_err == mem_err )
		//	m_dlg_log->AddLine( "    - no errors\r\n" );
	}
	if( n_err )
	{
		m_dlg_log->AddLine("\r\n\r\nCorrect fatal errors to continue...\r\nUse the D key to highlight errors in the workspace\r\n");
		m_dlg_log->UpdateWindow();
		return err_page;
	}
	else if( Pages.IsThePageIncludedInNetlist( cur_page ) )
		for( int ipg=0; ipg<MAX_PAGES; ipg++ )
			if( Pages.IsThePageIncludedInNetlist( ipg ) )
				m_parts_checked[ipg] = TRUE;
	// -----------
	// Fatal error 
	// checking completed
	// ------------------
	m_dlg_log->AddHeader( "Checking the design of the current page", 3 );
	m_dlg_log->AddParagraph( "Verification of footprint names" );
	m_dlg_log->UpdateWindow();
	mem_err = n_err;
	if( m_footprint_checked[cur_page] == 0 )
	{
		if( m_FootprintNames.GetSize() == 0 )
		{
			// WARNING
			CString s;
			s.Format("%d: FreePcb-2 footprint library path not specified (See Options on the Project menu)\r\n", ++n_err );
			m_dlg_log->AddLine(s);
			m_dlg_log->UpdateWindow();
		}	
		CString FOUND = dSPACE;
		CString HIDED = "\r\n";
		int it = -1;
		for( CText * t=Attr->m_Footlist->GetNextText( &it ); t; t=Attr->m_Footlist->GetNextText( &it ) )
		{
			if( t->m_str.Compare(NO_FOOTPRINT) == 0 )
			{
				continue;
			}
			if( t->m_font_size == 0 )
			{
				CText * RefText = m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_part_attr];
				int iof = HIDED.Find(" "+t->m_str+dSPACE+"(");
				if( iof <= 0 )
				{
					HIDED += "        " + t->m_str + dSPACE + "(" +  RefText->m_str + ")\r\n";
				}
				else
				{
					iof = HIDED.Find(")",iof);
					if( iof > 0 )
						HIDED.Insert( iof,", "+RefText->m_str );
				}
			}
			if( m_FootprintNames.GetSize() )
			{
				BOOL FIND = FindFootprint( this, &t->m_str );
				if( FIND == 0 && FOUND.Find(" "+t->m_str+" ") <= 0 )
				{
					// WARNING
					CString s;
					s.Format("%d: Footprint %s not found in the specified library\r\n", ++n_err, t->m_str );
					FOUND += t->m_str + dSPACE;
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1( ID_TEXT_DEF );
					id id2( 0,0,0,0,0 );
					// WARNING
					DRError * dre = m_drelist->Add( &s, id1, id2, 
						t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, t->m_layer, t, NULL );
				}
			}
		}
		if( HIDED.GetLength() > 3 )
		{
			// WARNING
			CString s;
			s.Format("%d: These footprints have a hidden attribute: %s\r\n", ++n_err, HIDED );
			m_dlg_log->AddLine(s);
			m_dlg_log->UpdateWindow();
		}
	}
	if( n_err == mem_err )
	{
		m_footprint_checked[cur_page] = TRUE;
		m_dlg_log->AddLine( "    - no errors\r\n" );
	}
	// Clearances
	//
	m_dlg_log->AddParagraph( "Text to text clearance check" );
	m_dlg_log->UpdateWindow();
	mem_err = n_err;
	int st_index = n_err;
	if( m_clearance_checked[cur_page] == 0 )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			if( m_vis[lay] == 0 )
				continue;
			CTextList * tl = GetTextListbyLayer( lay );
			if( !tl || tl == m_tlist )
				continue;
	
			// select clearance
			int clearance = m_dr.part_attr_to_attr;
	
			// scan texts
			int it = -1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			{
				if( t->m_font_size == 0 )
					continue;
				RECT r;
				tl->GetTextRectOnPCB( t, &r );
				SwellRect( &r, clearance );
				int it2 = it;
				for( int lay2=lay; lay2<LAY_ADD_1; lay2++ )
				{
					if( m_vis[lay2] == 0 )
						continue;
					CTextList * tl2 = GetTextListbyLayer( lay2 );
					if( !tl2 || tl2 == m_tlist )
						continue;
					for( CText * t2=tl2->GetNextText(&it2); t2; t2=tl2->GetNextText(&it2) )
					{
						if( t2->m_font_size == 0 )
							continue;
						if( t->m_x == t2->m_x && t->m_y == t2->m_y && t->m_angle == t2->m_angle )
							if( t->m_str.Compare( t2->m_str ) == 0 )
								continue;
						RECT r2;
						tl2->GetTextRectOnPCB( t2, &r2 );
						if( RectsIntersection( r, r2 ) != -1 )
						{
							// ERROR
							CString s;
							s.Format("%d: Text %s to text %s clearance error\r\n", ++n_err, t->m_str, t2->m_str );
							m_dlg_log->AddLine(s);
							m_dlg_log->UpdateWindow();
							id id1( ID_TEXT_DEF );
							id id2( ID_TEXT_DEF );
							// ERROR
							DRError * dre = m_drelist->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, lay, t, t2 );
						}
					}
					it2 = -1;
				}
			}
		}
	}
	if( n_err == mem_err )
		m_dlg_log->AddLine( "    - no errors\r\n" );
	//
	//
	m_dlg_log->AddParagraph( "Text to polyline clearance check" );
	m_dlg_log->UpdateWindow();
	mem_err = n_err;
	if( m_clearance_checked[cur_page] == 0 )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			if( m_vis[lay] == 0 )
				continue;
			CTextList * tl = GetTextListbyLayer( lay );
			if( !tl || tl == m_tlist )
				continue;
	
			// select clearance
			int clearance = m_dr.part_attr_to_poly;
	
			// scan texts
			int it = -1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			{
				if( t->m_font_size == 0 )
					continue;
				if( t->m_str.Right(3) == "BOM" )
					continue;
				//
				RECT r;
				tl->GetTextRectOnPCB( t, &r );
				SwellRect( &r, clearance );
				for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
				{
					if( m_outline_poly->GetAt(i).m_visible == 0 )
						continue;
					if( RectsIntersection( r, m_outline_poly->GetAt(i).GetBounds() ) != -1 )
					{
						int np = 4;
						CPoint P[4] = {CPoint(0,0),CPoint(0,0),CPoint(0,0),CPoint(0,0)};
						m_dlist->Get_Points( t->dl_sel, P, &np );
						int w = m_outline_poly->GetAt(i).GetW();
						for( int ii=m_outline_poly->GetAt(i).GetNumSides()-1; ii>=0; ii-- )
						{
							int next = m_outline_poly->GetAt(i).GetNextCornerIndex(ii);
							float x1 = m_outline_poly->GetAt(i).GetX(ii);
							float y1 = m_outline_poly->GetAt(i).GetY(ii);
							float x2 = m_outline_poly->GetAt(i).GetX(next);
							float y2 = m_outline_poly->GetAt(i).GetY(next);
							//
							int mind = clearance;
							int left = max(x1,x2)+w/2 - r.left;
							int right = r.right -  min(x1,x2)+w/2;
							int bottom = max(y1,y2)+w/2 - r.bottom;
							int top = r.top - min(y1,y2)+w/2;
							if( left < 0 )
								continue;
							else
								mind = min( mind, abs(clearance - left) );
							if( right < 0 )
								continue;
							else
								mind = min( mind, abs(clearance - right) );
							if( bottom < 0 )
								continue;
							else
								mind = min( mind, abs(clearance - bottom) );
							if( top < 0 )
								continue;
							else
								mind = min( mind, abs(clearance - top) ); 
							int ERR = 1;
							if( fabs(x1-x2) < _2540 )
								ERR = mind;
							else if( fabs(y1-y2) < _2540 )
								ERR = mind;
							else
							{
								int x0=0, y0=0, mind=INT_MAX;
								int d = ::GetClearanceBetweenSegments( x1,y1,x2,y2,m_outline_poly->GetAt(i).GetSideStyle(ii), w, P[0].x, P[0].y, P[1].x, P[1].y, CPolyLine::STRAIGHT, _2540, 2*clearance, &x0, &y0 );
								mind = min( mind, d );
								if( mind < clearance )
									ERR = mind;
								if( ERR == 0 )
								{
									d = ::GetClearanceBetweenSegments( x1,y1,x2,y2,m_outline_poly->GetAt(i).GetSideStyle(ii), w, P[1].x, P[1].y, P[2].x, P[2].y, CPolyLine::STRAIGHT, _2540, 2*clearance, &x0, &y0 );
									mind = min( mind, d );
									if( mind < clearance )
										ERR = mind;
									if( ERR == 0 )
									{
										d = ::GetClearanceBetweenSegments( x1,y1,x2,y2,m_outline_poly->GetAt(i).GetSideStyle(ii), w, P[2].x, P[2].y, P[3].x, P[3].y, CPolyLine::STRAIGHT, _2540, 2*clearance, &x0, &y0 );
										mind = min( mind, d );
										if( mind < clearance )
											ERR = mind;
										if( ERR == 0 )
										{
											d = ::GetClearanceBetweenSegments( x1,y1,x2,y2,m_outline_poly->GetAt(i).GetSideStyle(ii), w, P[3].x, P[3].y, P[0].x, P[0].y, CPolyLine::STRAIGHT, _2540, 2*clearance, &x0, &y0 );
											mind = min( mind, d );
											if( mind < clearance )
												ERR = mind;
										}	
									}
								}
								/*CPoint P[N_SIDES_APPROX_ARC];
								int n_arcs = Generate_Arc( x1,y1,x2,y2,m_outline_poly->GetAt(i).GetSideStyle(ii), P, N_SIDES_APPROX_ARC-1 );
								for( int ipt=0; (ipt+1)<n_arcs; ipt++ )
								{
									int np = 4;
									float angle = Angle( P[ipt].x, P[ipt].y, P[ipt+1].x, P[ipt+1].y );
									float x0 = P[ipt].x;
									float y0 = P[ipt].y;
									Rotate_Vertex( &x0, &y0, -angle );
									CPoint p[4] = { CPoint( r.left,r.bottom ), CPoint( r.left,r.top ), CPoint( r.right,r.bottom ), CPoint( r.right,r.top ) };
									if( t->m_angle%90 )
										m_dlist->Get_Points( t->dl_sel, p, &np );
									RotatePOINTS( p, 4,	-angle, CPoint(0,0) );	
									for( int ipo=0; ipo<4; ipo++ )
										if( fabs(y0-(float)p[ipo].y) < clearance )
										{
											ERR = 1;
											break;
										}
									if( ERR )
										break;
								}*/
							}
							if( ERR != 1 )
							{
								// ERROR
								CString s, cl;
								::MakeCStringFromDimension( m_view->m_user_scale, &cl, ERR, m_units, 1, 1, 1, 2 );
								s.Format("%d: Text %s to polyline clearance error (=%s)\r\n", ++n_err, t->m_str, cl );
								m_dlg_log->AddLine(s);
								m_dlg_log->UpdateWindow();
								id id1( ID_TEXT_DEF );
								id id2 = m_outline_poly->GetAt(i).GetId();
								// ERROR
								DRError * dre = m_drelist->Add( &s, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, lay, t );
							}
						}
					}
				}
			}
		}
	}
	if( n_err == mem_err )
		m_dlg_log->AddLine( "    - no errors\r\n" );
	if( st_index == n_err )
		m_clearance_checked[cur_page] = TRUE;
	// end
	m_dlg_log->AddParagraph( "DONE" );
	m_dlg_log->UpdateWindow();
	return err_page;
}


int CFreePcbDoc::GetGerberPathes(CString Path)
{
	CString shortcut[NUM_SHORTCUTS];
	CFileFind m_find_dir; 	
	CString szFavorite_path = m_app_dir + "\\Shortcut";  
	BOOL bWorking = 0; 
	int num_shortcut = 0;
	DWORD dwAttr = GetFileAttributes(szFavorite_path);
	if(dwAttr == 0xffffffff)  
	{  
		DWORD dwError = GetLastError();  
		if(dwError == ERROR_FILE_NOT_FOUND)  
		{  
			AfxMessageBox(G_LANGUAGE == 0 ? "Shortcut path not found":"Путь ярлыка не найден", MB_OK);
		}  
	}	  
	else if( Path.GetLength() )
	{  
		bWorking = m_find_dir.FindFile(szFavorite_path+"\\*.*");  	
		while  (bWorking)  
		{  	
			bWorking = m_find_dir.FindNextFile();
			if( !m_find_dir.IsDots() && !m_find_dir.IsDirectory())   
			{  
				CString m_gName = m_find_dir.GetFileName();
				if( m_gName.Right(4).MakeLower() == ".lnk" )
				{
					m_cam_shortcut[num_shortcut] = m_find_dir.GetFilePath();
					shortcut[num_shortcut] = m_gName;
					num_shortcut++;
					if( num_shortcut >= NUM_SHORTCUTS )
						break;
				}
			}			
		}
	}
	CWnd* pMain = AfxGetMainWnd();
	CMenu* pMenu = pMain->GetMenu();
	CMenu* submenu = pMenu->GetSubMenu(0);

	//out
	if( num_shortcut == 0 )
		return 0;
	// 
	struct _stat buf;
	if( _stat( m_cam_full_path, &buf ) )
		szFavorite_path = m_pcb_full_path.Left(m_pcb_full_path.ReverseFind('\\'));
	else
		szFavorite_path = m_cam_full_path.Left(m_cam_full_path.ReverseFind('\\'));
	//
	int imenu = 0; //number of menu items
	bWorking = 0; 
	BOOL b_cam_present = 0;
	dwAttr = GetFileAttributes(szFavorite_path);
	if(dwAttr == 0xffffffff)  
	{  
		DWORD dwError = GetLastError();  
		if(dwError == ERROR_FILE_NOT_FOUND)  
		{  
			//AfxMessageBox("Gerber path not found", MB_OK);  
		}  
	}	  
    else  
	{  
		bWorking = m_find_dir.FindFile(szFavorite_path+"\\*.*");  	
		while  (bWorking)  
		{  	
			bWorking = m_find_dir.FindNextFile(); 
			if ( !m_find_dir.IsDots() && m_find_dir.IsDirectory() )
			{
				CFileFind m_find_g_file; 
				CString CAM_F = m_find_dir.GetFileName();
				CString path2 = m_find_dir.GetFilePath(); 
				BOOL bWorking2 = m_find_g_file.FindFile(path2+"\\*.*");
				while  (bWorking2)  
				{
					bWorking2 = m_find_g_file.FindNextFile();
					if( !m_find_g_file.IsDots() && !m_find_g_file.IsDirectory())   
					{  
						CString m_gName = m_find_g_file.GetFileName();
						if( m_gName.Right(4).MakeLower() == ".gbr" && imenu < NUM_CAM_FOLDERS )
						{
							b_cam_present = 1;
							CAM[imenu] = path2;
							imenu++;
							break;
						}
					}
				}
			}
		}
	}
	return imenu;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
CTextList * CFreePcbDoc::GetTextListbyLayer( int L )
{
	CTextList * tl = NULL;
	if( L == LAY_PART_NAME )
		tl = Attr->m_Reflist;
	else if( L == LAY_PIN_NAME ) 
		tl = Attr->m_Pinlist;
	else if( L == LAY_PIN_DESC || L >= LAY_ADD_1 )
		tl = Attr->m_pDesclist;
	else if( L == LAY_NET_NAME ) 
		tl = Attr->m_Netlist;
	else if( L == LAY_FOOTPRINT )
		tl = Attr->m_Footlist;
	else if( L == LAY_PART_VALUE ) 
		tl = Attr->m_Valuelist;
	else if( L == LAY_INFO_TEXT ) 
		tl = m_tlist;
	return tl;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
CText * CFreePcbDoc::GetFreeNet( CString * new_net, int x, int y, int H, int W )
{
	static int it = -1;
	CString str;
	int i = Pages.GetLastFreeNetIndex( TRUE );
	do{	i++;
		str.Format( "NET%.5d", i );
	}while( m_tlist->GetText(&str) );
	if( new_net == NULL )
		new_net = &str;
	CText * net = m_tlist->GetNextInvisibleText(&it);
	if( net == NULL )
	{
		it = -1;
		return m_tlist->AddText( x, y, 0, LAY_NET_NAME, H, W, m_default_font, new_net, 0 );
	}
	else
	{
		net->Undraw();
		net->m_str = *new_net;
		net->m_nchars = new_net->GetLength();
		net->m_font_size = H;
		net->m_stroke_width = W;
		net->m_x = x;
		net->m_y = y;
		net->m_angle = 0;
		net->m_layer = LAY_NET_NAME;
		return net;
	}
}
int CFreePcbDoc::CheckFree( CPolyLine * p, int lay_mask )
{
	// ignore invis
	if( p->m_visible == 0 )
		return 0;
	// ignore polygons
	//if( p->GetClosed() )
	//	return 0;
	// filter by layer
	if( p->Check( index_pin_attr ) == 0 )
	{
		if( getbit( lay_mask, p->GetLayer() ) == 0 )
			return 0;
	}
	return 1;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
int CFreePcbDoc::FindFreePolyline( int lay_mask )
{
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		// vis, closed, layer
		if( CheckFree( &m_outline_poly->GetAt(i),lay_mask ) == 0 )
			continue;
		// polyline already used
		if( m_outline_poly->GetAt(i).Check( index_net_attr ) )
			continue;
		return i;
	}
	return -1;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
int CFreePcbDoc::CreatePCBNets( int ITERATOR )
{
	if( ITERATOR > MAX_PAGES )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			("A loop has occurred between two pages. "\
			"This is probably due to the fact that two or "\
			"more blocks refer to each other. Blocks cannot be used this way. "\
			"Place all blocks on one page and then try again."):
			("Произошёл цикл между двумя страницами. "\
			"Вероятно, это связано с тем, что два или "\
			"более блоков ссылаются друг на друга. Блоки нельзя использовать таким образом. "\
			"Поместите все блоки на одну страницу и попробуйте ещё раз."), MB_ICONERROR);
		return INT_MAX;
	}
	if( m_netlist_created[Pages.GetActiveNumber()] )
		return 0;
	RECT T = GetPolylineBounds();

	int lay_mask = 0, nerrors = 0;
	setbit( lay_mask, LAY_FREE_LINE );
	setbit( lay_mask, LAY_PIN_LINE );
	setbit( lay_mask, LAY_CONNECTION );
	//
	CString s;
	m_dlg_log->Clear();
	m_dlg_log->OnShowMe();
	// 
	m_dlg_log->AddHeader( "Generator of nets Launched", 3 );
	CString PGname;
	Pages.GetPageName( Pages.GetActiveNumber(), &PGname );
	s.Format("Current page: %s\r\n", PGname );
	m_dlg_log->AddLine(s);
	m_dlg_log->UpdateWindow();
	s.Format("Praparing.....\r\n");
	m_dlg_log->AddLine(s);
	m_dlg_log->UpdateWindow();
	
	// speed optimization
	int mid_X = (T.left+T.right)/2;
	int mid_Y = (T.top+T.bottom)/2;
	poly_ptr_array FourQuadrant;
	FourQuadrant.Rect[0] = rect( T.left, T.bottom, mid_X, mid_Y );
	FourQuadrant.Rect[1] = rect( T.left, mid_Y, mid_X, T.top );
	FourQuadrant.Rect[2] = rect( mid_X, mid_Y, T.right, T.top );
	FourQuadrant.Rect[3] = rect( mid_X, T.bottom, T.right, mid_Y );
	//
	s.Format("OK\r\nRemoving obsolete connections: ");
	m_dlg_log->AddLine(s);
	m_dlg_log->UpdateWindow();
	int n_err = 0;
	int n_war = 0;
	int it = -1;
	for( CText * t=m_tlist->GetNextText(&it); t; t=m_tlist->GetNextText(&it) )
		t->InVisible();
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{ 
		//if (m_outline_poly->GetAt(i).GetLayer() >= LAY_ADD_1) !
		//	continue; // experimental

		// delete old net pointer
		if( m_outline_poly->GetAt(i).pAttr[index_net_attr] == NULL )
		{
			if( m_outline_poly->GetAt(i).GetLayer() == LAY_CONNECTION )
				m_outline_poly->GetAt(i).SetLayer( LAY_FREE_LINE ); 
		}
		else if( m_outline_poly->GetAt(i).pAttr[index_net_attr]->m_tl != Attr->m_Netlist )
		{
			if( m_outline_poly->GetAt(i).pAttr[index_net_attr]->m_tl != m_tlist )
			{
				ASSERT(0);
			}
			//m_outline_poly->GetAt(i).pAttr[index_net_attr]->InVisible(); already done
			m_outline_poly->GetAt(i).pAttr[index_net_attr] = NULL;
			if( m_outline_poly->GetAt(i).pAttr[index_pin_attr] == NULL )
				m_outline_poly->GetAt(i).SetLayer( LAY_FREE_LINE );
		}

		// reset pin layer of complex parts
		if( m_outline_poly->GetAt(i).pAttr[index_desc_attr] )
		{
			if( m_outline_poly->GetAt(i).pAttr[index_desc_attr]->m_str.Right(7) == "BLK_PTR" )
			{
				m_outline_poly->GetAt(i).pAttr[index_net_attr] = NULL;
				m_outline_poly->GetAt(i).SetLayer( LAY_FREE_LINE );
			}
		}
		//else
		//{
		//	FindNodeLine(i); not use!!! an error appears on the nodes because this function changes the layer
		//}
		if( m_outline_poly->GetAt(i).GetLayer() == LAY_FREE_LINE )
			if( m_outline_poly->GetAt(i).Check( index_part_attr ) )
				if( m_outline_poly->GetAt(i).Check( index_pin_attr ) == 0 )
				{
					if( CText * n1 = m_outline_poly->GetAt(i).Check( index_net_attr ) )
					{
						if( n1->m_tl == m_tlist )
							m_outline_poly->GetAt(i).SetLayer( LAY_PART_OUTLINE );
					}
					else
						m_outline_poly->GetAt(i).SetLayer( LAY_PART_OUTLINE ); 
					m_outline_poly->GetAt(i).Draw();
				}
				else
				{
					m_outline_poly->GetAt(i).SetLayer( LAY_PIN_LINE ); 
					m_outline_poly->GetAt(i).Draw();
				}
		
		// vis, closed, layer
		if( CheckFree( &m_outline_poly->GetAt(i),lay_mask ) == 0 )
			continue;

		// delete old nodes
		m_outline_poly->GetAt(i).Node[0] = 0;
		m_outline_poly->GetAt(i).Node[ m_outline_poly->GetAt(i).GetNumCorners()-1 ] = 0;
		m_outline_poly->GetAt(i).SetUtility(0);

		// speed optimization
		m_outline_poly->GetAt(i).RecalcRectC(0);
		RECT pr = m_outline_poly->GetAt(i).GetBounds();
		for( int ii=0; ii<4; ii++ )
			if( RectsIntersection( pr, FourQuadrant.Rect[ii] ) != -1 )
				FourQuadrant.Array[ii].Add( &m_outline_poly->GetAt(i) );
	}
	s.Format("OK\r\nCreating User Named Nets: ");
	m_dlg_log->AddLine(s);
	m_dlg_log->UpdateWindow();
	//
	// First, create user-named networks, 
	// and then NET00000 format networks
	//
	it = -1;
	int attrS = (T.top-T.bottom)/(Attr->m_Netlist->GetNumDifferentTexts( TRUE )*2+1);
	attrS = min( attrS, 10*NM_PER_MM );
	//attrS = max( attrS, NM_PER_MM );
	int SHIFT_X = attrS;
	int SHIFT_Y = -attrS;
	// AttributeIndexing( LAY_NET_NAME );xxxxxxxxx
	CTextList * nl = Attr->m_Netlist;
	nl->MarkAllTexts(0);
	for( CText * net = nl->GetNextText(&it); net; net=nl->GetNextText(&it) )
	{
		if( net->utility ) // already completed
			continue;
		if( net->m_str.Left(3) == FREENET )
		{
			CString s;
			s.Format("\r\n        ERROR! A user net named should not begin with NET.., since this is a reserved word.( %s )", net->m_str );
			m_dlg_log->AddLine(s);
			m_dlg_log->UpdateWindow();
			id id1(ID_TEXT_DEF);
			id id2;
			int x = net->m_x;
			int y = net->m_y;
			DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );
			n_err++;
			continue;
		}
		int it2 = it;
		CText * new_net = GetFreeNet( &net->m_str, T.right+SHIFT_X, T.top+SHIFT_Y, attrS, attrS/5 );
		do{
			int ind = net->m_polyline_start;
			if( ind == -1 ) 
				break;
			// initialize rectangle
			RECT PolyRect = m_outline_poly->GetAt(ind).GetBounds();
			// search next line
			for( int repeat=0, next_i=0; next_i>=0; repeat++ )
			{
				next_i = FindNodeLine( ind, NULL, NULL, TRUE, TRUE, NULL, &PolyRect );
				if( next_i == -1 ) 
				{
					if( repeat == 0 && m_outline_poly->GetAt(ind).Check( index_pin_attr ) == NULL )
					{
						CString s;
						s.Format("\r\n        ERROR! No free electrical circuits found for net label %s ", new_net->m_str );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						id id1 = m_outline_poly->GetAt(ind).GetId();
						id id2 = id1;
						int x = m_outline_poly->GetAt(ind).GetX(0);
						int y = m_outline_poly->GetAt(ind).GetY(0);
						DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );
						n_err++;
					}
				}
				else
				{
					m_outline_poly->GetAt(next_i).pAttr[index_net_attr] = new_net;
					// scan start 
					int find = 0;
					SwellRect( &PolyRect, m_outline_poly->GetAt(next_i).GetBounds() );
					while( FindNodeLines( new_net, net, &find, lay_mask, &FourQuadrant, &PolyRect ) );
				}
			}
			net->utility = 1;
			net = nl->GetText( &net->m_str, &it2 );
		} while( net );
		//
		//set net visibility
		new_net->MakeVisible();
		s.Format("\r\n    Net %s created", new_net->m_str);
		m_dlg_log->AddLine(s);
		m_dlg_log->UpdateWindow();
		if( new_net->m_str.Find("|") <= 0 )
			SHIFT_Y -= attrS*2;
	}
	s.Format("\r\nCreating NETxxxxx format nets: ");
	m_dlg_log->AddLine(s);
	m_dlg_log->UpdateWindow();
	for(  int iFree = FindFreePolyline( lay_mask ); iFree >= 0; iFree = FindFreePolyline( lay_mask ) )
	{
		CText * net = GetFreeNet( NULL, T.right+SHIFT_X, T.top+SHIFT_Y, attrS/2, attrS/10 );
		m_outline_poly->GetAt(iFree).pAttr[index_net_attr] = net;
		int find = 0;
		RECT PolyRect = m_outline_poly->GetAt(iFree).GetBounds();
		while( FindNodeLines( net, net, &find, lay_mask, &FourQuadrant, &PolyRect ) );
		if( find == 0 )
		{
			if( m_outline_poly->GetAt(iFree).Check( index_part_attr ) == 0 || m_outline_poly->GetAt(iFree).Check( index_pin_attr ) == 0 )
			{
				s = "\r\n        WARNING! Free polyline found";
				n_war++;
				m_dlg_log->AddLine(s);
				m_dlg_log->UpdateWindow();
				//
				id id1 = m_outline_poly->GetAt(iFree).GetId();
				id id2;
				int x = m_outline_poly->GetAt(iFree).GetX(0);
				int y = m_outline_poly->GetAt(iFree).GetY(0);
				DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );
			}
			else 
			{
				m_outline_poly->GetAt(iFree).SetLayer( m_unconnected_pin_layer );
				CText * tpin = m_outline_poly->GetAt(iFree).Check(index_pin_attr);
				if (tpin)
				{
					tpin->m_layer = m_unconnected_pin_layer;
					tpin->MakeVisible();
					CText* dpin = m_outline_poly->GetAt(iFree).Check(index_desc_attr);
					if (dpin)
					{
						dpin->m_layer = m_unconnected_pin_layer;
						dpin->MakeVisible();
					}
				}
				m_outline_poly->GetAt(iFree).Draw();
			}
		}
		//
		net->MakeVisible();
		s.Format("\r\n    %s created", net->m_str);
		m_dlg_log->AddLine(s);
		m_dlg_log->UpdateWindow();
	}

	// checking
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		if( m_outline_poly->GetAt(i).m_visible )
		{
			if( m_outline_poly->GetAt(i).pAttr[index_net_attr] )
			{
				if( m_outline_poly->GetAt(i).GetLayer() == LAY_FREE_LINE )
				{
					CString net = "";
					net = m_outline_poly->GetAt(i).pAttr[index_net_attr]->m_str;
					///m_outline_poly->GetAt(i).pAttr[index_net_attr]->InVisible(); do not use !!
					s.Format("\r\n        ERROR! The polyline %s has incomplete connection", net );
					m_dlg_log->AddLine(s);
					m_dlg_log->UpdateWindow();
					id id1 = m_outline_poly->GetAt(i).GetId();
					id id2;
					int x = m_outline_poly->GetAt(i).GetX(0);
					int y = m_outline_poly->GetAt(i).GetY(0);
					DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );	
					n_err++;			
				}
				else if( m_outline_poly->GetAt(i).GetLayer() >= LAY_ADD_1 )
					m_outline_poly->GetAt(i).pAttr[index_net_attr]->InVisible();
			}
		}
	}
	it = -1;
	for( CText * t=Attr->m_Reflist->GetNextText(&it); t; t=Attr->m_Reflist->GetNextText(&it) )
	{
		for( int get = t->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_part_attr] )
		{
			if( m_outline_poly->GetAt(get).Check( index_part_attr ) )
				if( m_outline_poly->GetAt(get).Check( index_pin_attr ) )
					if( m_outline_poly->GetAt(get).Check( index_net_attr ) == NULL )
					{
						CText * part = m_outline_poly->GetAt(get).pAttr[index_part_attr];
						s.Format("\r\n        WARNING! The part %s has unconnected pins", part->m_str );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						id id1 = m_outline_poly->GetAt(get).GetId();
						id id2;
						int x = m_outline_poly->GetAt(get).GetX(0);
						int y = m_outline_poly->GetAt(get).GetY(0);
						DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );	
						n_war++;
						break;
					}
		}
	}
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		if( m_outline_poly->GetAt(i).GetVisible() == 0 )
			continue;
		if( CText * txt1 = m_outline_poly->GetAt(i).pAttr[index_net_attr] )
		{
			if( m_outline_poly->GetAt(i).pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
				FindNodeLine(i);
			for( int ii=m_outline_poly->GetSize()-1; ii>i; ii-- )
			{
				if( m_outline_poly->GetAt(ii).GetVisible() == 0 )
					continue;
				if( CText * txt2 = m_outline_poly->GetAt(ii).pAttr[index_net_attr] )
				{
					if( txt1 == txt2 )
						continue;
					if( txt1->m_str.Compare( txt2->m_str ) == 0 )
						continue;
					int ncrns = m_outline_poly->GetAt(i).GetNumCorners();
					for( int ico=1; ico<ncrns-1; ico++ )
					{
						int ncrns2 = m_outline_poly->GetAt(ii).GetNumCorners();
						for( int iico=1; iico<ncrns2-1; iico++ )
						{
							int w1 = m_outline_poly->GetAt(i).GetW();
							int w2 = m_outline_poly->GetAt(ii).GetW();
							int x1 = m_outline_poly->GetAt(i).GetX(ico);
							int x2 = m_outline_poly->GetAt(ii).GetX(iico);
							int y1 = m_outline_poly->GetAt(i).GetY(ico);
							int y2 = m_outline_poly->GetAt(ii).GetY(iico);
							if( abs(x1-x2) < (w1+w2)/2 && abs(y1-y2) < (w1+w2)/2 )
							{
								s.Format("\r\n        Invalid intersection (net %s and net %s)", txt1->m_str, txt2->m_str );
								m_dlg_log->AddLine(s);
								m_dlg_log->UpdateWindow();
								id id1 = m_outline_poly->GetAt(i).GetId();
								id id2;
								int x = m_outline_poly->GetAt(i).GetX(ico);
								int y = m_outline_poly->GetAt(i).GetY(ico);
								DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );	
								n_err++;
							}
						}
					}
				}
			}
		}
	}
	// creating nodes at the junction of three vertices
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		if( m_outline_poly->GetAt(i).m_visible == 0 )
			continue;
		int W = abs(m_outline_poly->GetAt(i).GetW());
		if( m_outline_poly->GetAt(i).Node[0] == 1 )
		{
			int cnt_connect = 0;
			int cnt_pin = 0;
			if( m_outline_poly->GetAt(i).Check( index_pin_attr ) )
				cnt_pin++;
			else
				cnt_connect++;
			CPoint pt1( m_outline_poly->GetAt(i).GetX(0), m_outline_poly->GetAt(i).GetY(0) );
			for( int ii=i-1; ii>=0; ii-- )
			{
				if( m_outline_poly->GetAt(ii).m_visible == 0 )
					continue;
				if( m_outline_poly->GetAt(ii).Node[0] == 1 )
				{
					CPoint Gpt1( m_outline_poly->GetAt(ii).GetX(0), m_outline_poly->GetAt(ii).GetY(0) );
					if( abs(Gpt1.x-pt1.x) < W/2 && abs(Gpt1.y-pt1.y) < W/2 )
						if( m_outline_poly->GetAt(ii).Check( index_pin_attr ) )
							cnt_pin++;
						else
							cnt_connect++;
				}
				if( m_outline_poly->GetAt(ii).Node[m_outline_poly->GetAt(ii).GetNumCorners()-1] == 1 )
				{
					CPoint Gpt2( m_outline_poly->GetAt(ii).GetX(m_outline_poly->GetAt(ii).GetNumCorners()-1), 
								 m_outline_poly->GetAt(ii).GetY(m_outline_poly->GetAt(ii).GetNumCorners()-1) );
					if( abs(Gpt2.x-pt1.x) < W/2 && abs(Gpt2.y-pt1.y) < W/2 )
						if( m_outline_poly->GetAt(ii).Check( index_pin_attr ) )
							cnt_pin++;
						else
							cnt_connect++;
				}
			}
			if( cnt_connect > 2 || (cnt_connect==2 && cnt_pin) || (cnt_connect && cnt_pin == 2) || cnt_pin > 2 )
			{
				m_outline_poly->GetAt(i).Node[0] = abs(m_node_w);
				m_outline_poly->GetAt(i).Draw();
			}
		}
		if( m_outline_poly->GetAt(i).Node[m_outline_poly->GetAt(i).GetNumCorners()-1] == 1 )
		{
			int cnt_connect = 0;
			int cnt_pin = 0;
			if( m_outline_poly->GetAt(i).Check( index_pin_attr ) )
				cnt_pin++;
			else
				cnt_connect++;
			CPoint pt2( m_outline_poly->GetAt(i).GetX(m_outline_poly->GetAt(i).GetNumCorners()-1), 
						m_outline_poly->GetAt(i).GetY(m_outline_poly->GetAt(i).GetNumCorners()-1) );
			for( int ii=i-1; ii>=0; ii-- )
			{
				if( m_outline_poly->GetAt(ii).m_visible == 0 )
					continue;
				if( m_outline_poly->GetAt(ii).Node[0] == 1 )
				{
					CPoint Gpt1( m_outline_poly->GetAt(ii).GetX(0), m_outline_poly->GetAt(ii).GetY(0) );
					if( abs(Gpt1.x-pt2.x) < W/2 && abs(Gpt1.y-pt2.y) < W/2 )
						if( m_outline_poly->GetAt(ii).Check( index_pin_attr ) )
							cnt_pin++;
						else
							cnt_connect++;
				}
				if( m_outline_poly->GetAt(ii).Node[m_outline_poly->GetAt(ii).GetNumCorners()-1] == 1 )
				{
					CPoint Gpt2( m_outline_poly->GetAt(ii).GetX(m_outline_poly->GetAt(ii).GetNumCorners()-1), 
								 m_outline_poly->GetAt(ii).GetY(m_outline_poly->GetAt(ii).GetNumCorners()-1) );
					if( abs(Gpt2.x-pt2.x) < W/2 && abs(Gpt2.y-pt2.y) < W/2 )
						if( m_outline_poly->GetAt(ii).Check( index_pin_attr ) )
							cnt_pin++;
						else
							cnt_connect++;
				}
			}
			if( cnt_connect > 2 || (cnt_connect==2 && cnt_pin) || (cnt_connect && cnt_pin == 2) || cnt_pin > 2 )
			{
				m_outline_poly->GetAt(i).Node[m_outline_poly->GetAt(i).GetNumCorners()-1] = abs(m_node_w);
				m_outline_poly->GetAt(i).Draw();
			}
		}
	}

	m_dlg_log->AddHeader( "DONE", 4 );  
	m_dlg_log->AddLine( "Use the D key to highlight errors in the workspace\r\n" );
	m_dlg_log->UpdateWindow();
	ProjectModified( TRUE );
	if( n_war == m_drelist->list.GetSize() )
		m_netlist_created[Pages.GetActiveNumber()] = TRUE;
	//m_view->OnViewAllElements();
	return n_err;
}
//-----------------------------------------------------------------------------------
// total reset all connections and generation again
// without assignment net pointer
//-----------------------------------------------------------------------------------
int CFreePcbDoc::FindNodeLines( CText * net_ptr, CText * net_label_ptr, int * Nodes, int lay_mask, poly_ptr_array * FQ, RECT * polyR  )
{
	int isect = 0;
	for( ; isect<4; isect++ )
		if( RectsIntersection( *polyR, FQ->Rect[isect] ) == 1 )
			break;
	int count = 0;
	for( int i=(isect<4?(FQ->Array[isect].GetSize()-1):(m_outline_poly->GetSize()-1)); i>=0; i-- )
	{
		CPolyLine * gp = NULL;
		if( isect < 4 )
			gp = FQ->Array[isect].GetAt(i);
		else
			gp = &m_outline_poly->GetAt(i);

		// vis, closed, layer
		if( CheckFree( gp, lay_mask ) == 0 )
			continue;
		if( gp->Check(index_net_attr) )
			continue;

		// test intersect
		RECT r = gp->GetBounds();
		if( RectsIntersection( r, *polyR ) == -1 )
			continue;
		int found = 0;
		for( ; found<4; found++ )
			if( RectsIntersection( r, FQ->Rect[found] ) == 1 )
				break;

		if( found < 4 )
			count += FindNodeLine( gp->GetId().i, net_ptr, net_label_ptr, TRUE, FALSE, &(FQ->Array[found]), polyR, &FQ->Rect[found] ) + 1;
		else
			count += FindNodeLine( gp->GetId().i, net_ptr, net_label_ptr, TRUE, FALSE, NULL, polyR ) + 1;
	}
	(*Nodes) += count;
	return count;
}
//-----------------------------------------------------------------------------------
// finding nodes at the ends of a polyline
// assigns the net pointer to a polyline
//-----------------------------------------------------------------------------------
int CFreePcbDoc::FindNodeLine(	int ic, 
								CText * net_ptr, 
								CText * net_label_ptr, 
								BOOL StartOver, 
								BOOL foundAndReturned, 
								CArray<CPolyLine*>*Q,
								RECT * PolyR,
								RECT * ArrayR )
{
	CPoint pti;

	// set layer mask
	int lay_mask = 0;
	{
		setbit( lay_mask, LAY_FREE_LINE );
		setbit( lay_mask, LAY_PIN_LINE );
		setbit( lay_mask, LAY_CONNECTION );
	}
	//
	int L = m_outline_poly->GetAt(ic).GetLayer();
	if( CheckFree( &m_outline_poly->GetAt(ic), lay_mask ) == 0 )
		return -1;
	if( m_outline_poly->GetAt(ic).GetClosed() && m_outline_poly->GetAt(ic).Check( index_net_attr ) == NULL )
		return -1;
	if( net_ptr )
		if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] )
			if( m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
				return -1;
	//
	int Wpti = m_outline_poly->GetAt(ic).GetW();
	int gnc1 = m_outline_poly->GetAt(ic).GetNumCorners();
	//
	//  ignore short line !
	if( gnc1 )
		if( abs( m_outline_poly->GetAt(ic).GetX(0) - m_outline_poly->GetAt(ic).GetX(gnc1-1) ) < Wpti )
			if( abs( m_outline_poly->GetAt(ic).GetY(0) - m_outline_poly->GetAt(ic).GetY(gnc1-1) ) < Wpti )
				return -1;
	//
	// mark 0
	if( StartOver )
		m_outline_poly->GetAt(ic).SetUtility( 0 );
	//
	int MAX_CL = abs(Wpti);
	// nodes count
	int NodeFound = 0;
	// return index
	int RET = -1;
	if( gnc1 > 1 )for( int iv=0; iv<gnc1; iv+=(gnc1-1) )
	{
		// delete node 
		if( net_ptr == NULL )
			m_outline_poly->GetAt(ic).Node[iv] = 0;
		// get pt
		pti.x = m_outline_poly->GetAt(ic).GetX(iv);
		pti.y = m_outline_poly->GetAt(ic).GetY(iv);
		RECT rpt = rect( pti.x-Wpti/2, pti.y-Wpti/2, pti.x+Wpti/2, pti.y+Wpti/2 );
		for( int i=(Q ? Q->GetSize()-1 : m_outline_poly->GetSize()-1); i>=0; i-- )
		{
			CPolyLine * pLL = (Q ? (*Q)[i] : &m_outline_poly->GetAt(i));
			// checking different nets only
			if( net_ptr )
			{
				if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] == NULL &&
					pLL->pAttr[index_net_attr] == NULL )
					continue;
				if( net_ptr !=		 m_outline_poly->GetAt(ic).pAttr[index_net_attr] &&
					net_label_ptr != m_outline_poly->GetAt(ic).pAttr[index_net_attr] &&
					net_ptr !=		 pLL->pAttr[index_net_attr] &&
					net_label_ptr != pLL->pAttr[index_net_attr] )
					continue;
				if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] == pLL->pAttr[index_net_attr] )
					continue;
				if( pLL->pAttr[index_net_attr] )
					if( pLL->pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
						continue;
				if( net_label_ptr )
				{
					if( net_ptr ==		 m_outline_poly->GetAt(ic).pAttr[index_net_attr] &&
						net_label_ptr == pLL->pAttr[index_net_attr] )
						continue;
					if( net_ptr ==		 pLL->pAttr[index_net_attr] &&
						net_label_ptr == m_outline_poly->GetAt(ic).pAttr[index_net_attr] )
						continue;
				}
			}
			// vis, closed, layer
			if( CheckFree( pLL,lay_mask ) == 0 )
				continue;
			// mark 0
			if( StartOver )
				pLL->SetUtility( 0 );
			// check rects
			RECT gr = pLL->GetBounds();
			if( RectsIntersection( rpt, gr ) == -1 )
				continue;
			if( i == ic )
				continue;
			int w = pLL->GetW();
			int gnc2 = pLL->GetNumCorners();
			CPoint p,pp,pp2;
			BOOL ends1 = 0, ends2 = 0;
			pp.x = pLL->GetX(0);
			pp.y = pLL->GetY(0);
			pp2.x = pLL->GetX(gnc2-1);
			pp2.y = pLL->GetY(gnc2-1);
			//
			//if( abs( pp2.x-pp.x ) < w && abs( pp2.y-pp.y ) < w )
			//	continue;// ignore short line
			//
			int x = pti.x,	y = pti.y;
			if( abs( pti.x-pp.x ) < w && abs( pti.y-pp.y ) < w )
				ends1 = 1;
			else if( abs( pti.x-pp2.x ) < w && abs( pti.y-pp2.y ) < w )
				ends2 = 1;
			int d = INT_MAX;
			if( !ends1 && !ends2 )
				for( int ii=1; (d>0 && ii<gnc2); ii++ )
				{
					//if( pLL->GetSideStyle(ii-1) )?
					//	continue;
					p.x = pLL->GetX(ii);
					p.y = pLL->GetY(ii);
					MAX_CL = max(MAX_CL,w);
					d = ::GetClearanceBetweenSegments(	p.x,p.y,pp.x,pp.y, pLL->GetSideStyle(ii), w,
						pti.x, pti.y, pti.x+10, pti.y+10, 0, Wpti, MAX_CL, &x, &y );
					pp = p;
				}
			if( d <= 0 || ends1 || ends2 )
			{
				if( ends1 )
				{
					// connection of ends
					// mark that node as found
					if( m_outline_poly->GetAt(ic).Node[iv] == 0 )
						m_outline_poly->GetAt(ic).Node[iv] = 1;
					if( pLL->Node[0] == 0 )
						pLL->Node[0] = 1;
				}
				else if( ends2 )
				{
					// connection of ends
					// mark that node as found
					if( m_outline_poly->GetAt(ic).Node[iv] == 0 )
						m_outline_poly->GetAt(ic).Node[iv] = 1;
					if( pLL->Node[gnc2-1] == 0 )
						pLL->Node[gnc2-1] = 1;
				}
				else
				{
					// point node
					m_outline_poly->GetAt(ic).Node[iv] = GetStandartNode(this, ic);
				}

				// find And Abort
				if( foundAndReturned )
					if( pLL->pAttr[index_net_attr] )
					{
						if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] == 0 )
							continue;
						if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] )
							if( pLL->pAttr[index_net_attr]->m_str.Compare( m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_str ) == 0 )
								continue;
						CString s="";
						s.Format("\r\n        ERROR! Net Label connects two different nets (%s)", m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_str );
						m_dlg_log->AddLine(s);
						m_dlg_log->UpdateWindow();
						// add error
						id id1 = m_outline_poly->GetAt(ic).GetId();
						id id2 = pLL->GetId();
						DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );
						continue;
					}
					else
						return i;

				// mark 1
				m_outline_poly->GetAt(ic).SetUtility( TRUE );

				// swell rect
				if( PolyR )
				{
					SwellRect( PolyR, gr );
					SwellRect( PolyR, m_outline_poly->GetAt(ic).GetBounds() );
				}

				// remark ptr
				if( StartOver || pLL->GetUtility() == 0 )
				{	
					if( net_ptr )
					{
						int err = 0;
						if( m_outline_poly->GetAt(ic).pAttr[index_net_attr] == net_ptr || m_outline_poly->GetAt(ic).pAttr[index_net_attr] == net_label_ptr )
						{
							if( pLL->Check(index_net_attr) == 0 )
							{
								// return index
								RET = i;
								pLL->pAttr[index_net_attr] = net_ptr;
							}
							else 
								err = 1;
						}
						else if( pLL->pAttr[index_net_attr] == net_ptr /*|| pLL->pAttr[index_net_attr] == net_label_ptr*/ )
						{
							if( m_outline_poly->GetAt(ic).Check(index_net_attr) == 0 )
							{
								// return index
								RET = i;
								m_outline_poly->GetAt(ic).pAttr[index_net_attr] = net_ptr;
							}
							else 
								err = 1;
						}
						if( err )
						{
							CString s="";
							if( pLL->pAttr[index_net_attr]->m_str.Compare( m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_str ) )
								s.Format("\r\n        ERROR! Two different nets are interconnected.( %s and %s )", 
								pLL->pAttr[index_net_attr]->m_str, m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_str );
							else 
								s.Format("\r\n        ERROR! Duplicate net label.( %s )", pLL->pAttr[index_net_attr]->m_str );

							// add line
							if( s.GetLength() )
							{
								m_dlg_log->AddLine(s);
								m_dlg_log->UpdateWindow();
								// add error
								id id1 = m_outline_poly->GetAt(ic).GetId();
								id id2 = pLL->GetId();
								DRError * dre = m_drelist->Add( &s, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_CONNECTION );
							}
						}
					}
					else
						RET = i;
					//
					//
					int mRET = 0;
					if( Q && ArrayR )
						if( RectsIntersection( *PolyR, *ArrayR ) == 1 )
							mRET |= ( FindNodeLine(i, net_ptr, net_label_ptr, FALSE, 0, Q, PolyR, ArrayR) + 1 );
					if( mRET == 0 )
						RET |= ( FindNodeLine(i, net_ptr, net_label_ptr, FALSE, 0, NULL, PolyR) + 1 );
					else
						RET |= mRET;
				}
			}
		}
		if( m_outline_poly->GetAt(ic).Node[iv] )
		{
			NodeFound++;
		}
	}

	// check node count
	CText* pintext = m_outline_poly->GetAt(ic).Check(index_pin_attr);
	CText* dtext = m_outline_poly->GetAt(ic).Check(index_desc_attr);
	if( NodeFound == 0  )
	{// empty
	}// empty
	else if( NodeFound == 1 )
	{
		if( m_outline_poly->GetAt(ic).Check( index_pin_attr ) == NULL )
		{
			if( m_outline_poly->GetAt(ic).Check( index_net_attr ) )
				if( m_outline_poly->GetAt(ic).pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
					m_outline_poly->GetAt(ic).SetLayer( LAY_CONNECTION );
			if( m_outline_poly->GetAt(ic).Check( index_desc_attr ) )
				if( m_outline_poly->GetAt(ic).pAttr[index_desc_attr]->m_str.Right(7) == "BLK_PTR" )
					m_outline_poly->GetAt(ic).SetLayer( LAY_CONNECTION );
		}
		else
		{
			m_outline_poly->GetAt(ic).SetLayer(LAY_PIN_LINE); // set pin
			if (pintext)
			{
				pintext->m_layer = LAY_PIN_NAME;
				pintext->MakeVisible();
				if (dtext)
				{
					dtext->m_layer = LAY_PIN_DESC;
					dtext->MakeVisible();
				}
			}
		}
	}
	else if( L == LAY_FREE_LINE )
		m_outline_poly->GetAt(ic).SetLayer( LAY_CONNECTION ); // set connect
	else if (m_outline_poly->GetAt(ic).Check(index_pin_attr))
	{
		m_outline_poly->GetAt(ic).SetLayer(LAY_PIN_LINE); // set pin
		if (pintext)
		{
			pintext->m_layer = LAY_PIN_NAME;
			pintext->MakeVisible();
			if (dtext)
			{
				dtext->m_layer = LAY_PIN_DESC;
				dtext->MakeVisible();
			}
		}
	}
	// now draw
	m_outline_poly->GetAt(ic).Draw( m_dlist );
	return RET;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
CText * CFreePcbDoc::ScanAttr( CPolyLine * p, int * i_attr )
{
	static int num = 0;
	static CPolyLine * prev = NULL;
	if( p != prev )
		num = 0;
	if( i_attr )
		if( *i_attr < 0 )
			num = 0;
	prev = p;
	CText * RET = NULL;
	for( ; RET == NULL; num++ )
	{
		if( num >= num_attributes )
		{
			num = 0; 
			return NULL;
		}
		else
			RET = p->pAttr[num];
	}
	if( i_attr )
		*i_attr = num-1;
	return RET;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::AttributeIndexing( CText * t, int n_a )
{
	t->m_polyline_start = -1;
	CPolyLine * prev = NULL;
	for( int i=0; i<m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * p = &m_outline_poly->GetAt(i);
		if( p->m_visible == 0 )
			continue;
		if( t == p->pAttr[n_a] )
		{
			if( t->m_polyline_start == -1 )
			{
				t->m_polyline_start = i;
				if( n_a == index_part_attr )
				{
					if( p->pAttr[index_value_attr] )
						p->pAttr[index_value_attr]->m_polyline_start = i;
					if( p->pAttr[index_foot_attr] )
						p->pAttr[index_foot_attr]->m_polyline_start = i;
				}
			}
			else
			{
				prev->Next[n_a] = i;
				if( n_a == index_part_attr )
				{
					prev->Next[index_value_attr] = i;
					prev->Next[index_foot_attr] = i;
				}
			}
			if( p->pAttr[index_pin_attr] )
				if( p->pAttr[index_pin_attr]->isVISIBLE == 0 ) 
					p->pAttr[index_pin_attr] = NULL;
				else 
					p->pAttr[index_pin_attr]->m_polyline_start = i;

			prev = p;
		}
		if( p->pAttr[index_desc_attr] )
			if( p->pAttr[index_desc_attr]->isVISIBLE == 0 )
				p->pAttr[index_desc_attr] = NULL;
			else
				p->pAttr[index_desc_attr]->m_polyline_start = i;
	}
	if( prev )
	{
		prev->Next[n_a] = -1;
		if( n_a == index_part_attr )
		{
			prev->Next[index_value_attr] = -1;
			prev->Next[index_foot_attr] = -1;
		}
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::AttributeIndexing( int layer )
{
	int n_a;
	switch( layer )
		{
		case LAY_PART_NAME:		
			n_a = index_part_attr;	
			break;
		case LAY_NET_NAME:		
		case LAY_INFO_TEXT:		
			n_a = index_net_attr;	
			break;
		default: 
			return; //fail
		}
	CTextList * tl = GetTextListbyLayer( layer );
	int it = -1;
	for( CText * t=tl->GetNextAnyText(&it); t; t=tl->GetNextAnyText(&it) )
	{
		AttributeIndexing( t, n_a );
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
RECT CFreePcbDoc::GetPartBounds( CText * part, CText * pin )
{
	RECT R;
	R.left = R.bottom = INT_MAX;
	R.right = R.top = INT_MIN;
	if( part->m_polyline_start < 0 )
		AttributeIndexing( part, index_part_attr ); // m_polyline_start == -1
	CPolyLine * po = NULL;
	for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
	{
		po = &m_outline_poly->GetAt(get);
		if( part == po->pAttr[index_part_attr] )
			if( pin == NULL || pin == po->pAttr[index_pin_attr] )
			{
				RECT r = po->GetBounds();
				SwellRect( &R, r );
			}
	}
	if( R.left == INT_MAX )
		return rect(0,0,0,0);
	else
		return R;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
RECT CFreePcbDoc::GetPolylineBounds( RECT * maxPolyRect )
{
	RECT R;
	R.left = R.bottom = INT_MAX;
	R.right = R.top = INT_MIN;
	if( maxPolyRect )
		*maxPolyRect = rect(0,0,0,0);
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		if( m_outline_poly->GetAt(i).m_visible )
			if( m_outline_poly->GetAt(i).GetClosed() )
			{
				RECT pr = m_outline_poly->GetAt(i).GetBounds();
				SwellRect( &R, pr );
				if( maxPolyRect )
					if( maxPolyRect->top + maxPolyRect->right - maxPolyRect->bottom - maxPolyRect->left < pr.top + pr.right - pr.bottom - pr.left )
						*maxPolyRect = pr;
			}
	}
	if( R.left == INT_MAX )
		return rect(0,0,0,0);
	else
		return R;
}
//===============================================================================================
// SelectPart...this is called from FreePcbDoc when a new part is added
// selects the new part as long as the cursor is not dragging something
//
void CFreePcbDoc::SelectPart( CText * part, CText * pin, BOOL SCM, int Swell )
{
	if( part == NULL )
		return;
	id mem_id = m_view->m_sel_id;
	CText * mem_t = m_view->m_sel_text;
	if( part->m_polyline_start < 0 )
		AttributeIndexing( part, index_part_attr ); // m_polyline_start == -1
	if( pin )
		setbit( m_view->m_sel_flags, m_view->FLAG_SEL_PIN );
	else 
		setbit( m_view->m_sel_flags, m_view->FLAG_SEL_PART );
	//
	CPolyLine * po = NULL;
	CPolyLine * sel_po = NULL;
	RECT pr;
	pr.left = pr.bottom = INT_MAX;
	pr.right = pr.top = INT_MIN;
	int en_un_sel = SCM;
	if( pin == NULL )
	{
		int prev_s = -1;
		for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
		{
			po = &m_outline_poly->GetAt(get);
			int sel = po->GetSel();
			if( prev_s != -1 )
				if( sel != prev_s )
					en_un_sel = 0;
			prev_s = sel;
		}
	}
	for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
	{
		po = &m_outline_poly->GetAt(get);
		if( po->m_visible == 0 )
		{
			// Must be indexed
			// To exclude deleted lines
			AttributeIndexing( part, index_part_attr ); // necessarily !!!
			if( part->m_polyline_start < 0 )
				break;
			get = part->m_polyline_start;
			po = &m_outline_poly->GetAt(get);
		}
		if( po->pAttr[index_pin_attr] == pin || pin == NULL )
		{
			SwellRect( &pr, po->GetBounds() );
			if( po->Check( index_pin_attr ) )
			{
				RECT pinr;
				if( Attr->m_Pinlist->GetTextRectOnPCB( po->pAttr[index_pin_attr], &pinr ) )
					SwellRect( &pr, pinr );
			}
			if( po->Check( index_net_attr ) )
				if( po->pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
				{
					RECT netr;
					if( Attr->m_Netlist->GetTextRectOnPCB( po->pAttr[index_net_attr], &netr ) )
						SwellRect( &pr, netr );
				}
			if( po->Check( index_desc_attr ) )
			{
				RECT descr;
				if( Attr->m_pDesclist->GetTextRectOnPCB( po->pAttr[index_desc_attr], &descr ) )
					SwellRect( &pr, descr );
			}
			id Id = po->GetId();
			Id.sst = ID_SIDE;
			sel_po = po;
			for( int ii=0; ii<po->GetNumSides(); ii++ )
			{
				Id.ii = ii;
				m_view->NewSelect( NULL, & Id, 0, en_un_sel );
				if( Swell )
					po->HighlightSide( ii, po->GetW()*Swell, TRANSPARENT_HILITE );
			}
		}
	}
	if( sel_po )
		sel_po->ModifyGroupRect( &pr );
	if( SCM )
	{
		m_view->SetCursorMode( CUR_GROUP_SELECTED );
		CText * FP = m_outline_poly->GetAt( part->m_polyline_start ).pAttr[index_foot_attr];
		if( FP )
		{
			CString modF = FP->m_str;
			ExtractComponentName( &modF, NULL );
			m_view->m_draw_pic = m_app_dir + main_component + "\\" + modF;
		}
	}
	if( mem_id.type != ID_NONE )
	{
		m_view->m_sel_id = mem_id;
		m_view->m_sel_text = mem_t;
	}
	return;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::HighlightPartAttributes( CText * part, CText * pin )
{
	int TR = 0;
	id tid( ID_TEXT_DEF );
	if( pin )
	{
		if( pin->m_polyline_start >= 0 )
		{
			CPolyLine * po = &m_outline_poly->GetAt(pin->m_polyline_start);
			if( po->Check( index_pin_attr ) )
				po->pAttr[index_pin_attr]->m_tl->HighlightText( po->pAttr[index_pin_attr], TR );
			if( po->Check( index_net_attr ) )
				if( po->pAttr[index_net_attr]->m_tl == Attr->m_Netlist )
					po->pAttr[index_net_attr]->m_tl->HighlightText( po->pAttr[index_net_attr], TR );
			if( po->Check( index_desc_attr ) )
				po->pAttr[index_desc_attr]->m_tl->HighlightText( po->pAttr[index_desc_attr], TR );
		}
	}
	if( part )
	{
		if( part->m_polyline_start >= 0 )
		{
			CPolyLine * po = &m_outline_poly->GetAt(part->m_polyline_start);
			if( po->Check( index_part_attr ) )
				po->pAttr[index_part_attr]->m_tl->HighlightText( po->pAttr[index_part_attr], TR );
			if( po->Check( index_value_attr ) )
				po->pAttr[index_value_attr]->m_tl->HighlightText( po->pAttr[index_value_attr], TR );
			if( po->Check( index_foot_attr ) )
				po->pAttr[index_foot_attr]->m_tl->HighlightText( po->pAttr[index_foot_attr], TR );
		}
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::SelectPartAttributes( CText * part, BOOL incInvisible )
{
	int get = part->m_polyline_start; 
	if( get >= 0 )
	if( incInvisible || m_outline_poly->GetAt(get).m_visible )
	{
		id mem_id( m_view->m_sel_id );
		CText * mem_t = m_view->m_sel_text;
		CPolyLine * po = &m_outline_poly->GetAt(get);
		id tid( ID_TEXT_DEF );
		if( po->pAttr[index_part_attr] )
			if( po->pAttr[index_part_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_part_attr]->isVISIBLE )
					m_view->NewSelect( po->pAttr[index_part_attr], &tid, 0, 0 );
		if( po->pAttr[index_foot_attr] )
			if( po->pAttr[index_foot_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_foot_attr]->isVISIBLE )
					m_view->NewSelect( po->pAttr[index_foot_attr], &tid, 0, 0 );
		if( po->pAttr[index_value_attr] )
			if( po->pAttr[index_value_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_value_attr]->isVISIBLE )
					m_view->NewSelect( po->pAttr[index_value_attr], &tid, 0, 0 );
		if( mem_id.type != ID_NONE )
		{
			m_view->m_sel_id = mem_id;
			m_view->m_sel_text = mem_t;
		}
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::SelectLineAttributes( CPolyLine * po, BOOL incInvisible )
{
	if( incInvisible || po->m_visible )
	{
		id mem_id( m_view->m_sel_id );
		CText * mem_t = m_view->m_sel_text;
		id tid( ID_TEXT_DEF );
		//if( po->pAttr[index_part_attr] )
		//	if( po->pAttr[index_part_attr]->m_selected == 0 )
		//		if( incInvisible || po->pAttr[index_part_attr]->isVISIBLE )
		//			m_view->NewSelect( po->pAttr[index_part_attr], &tid, 0, 0 );
		//if( po->pAttr[index_value_attr] )
		//	if( po->pAttr[index_value_attr]->m_selected == 0 )
		//		if( incInvisible || po->pAttr[index_value_attr]->isVISIBLE )
		//			m_view->NewSelect( po->pAttr[index_value_attr], &tid, 0, 0 );
		//if( po->pAttr[index_foot_attr] )
		//	if( po->pAttr[index_foot_attr]->m_selected == 0 )
		//		if( incInvisible || po->pAttr[index_foot_attr]->isVISIBLE )
		//			m_view->NewSelect( po->pAttr[index_foot_attr], &tid, 0, 0 );
		if( po->pAttr[index_pin_attr] )
			if( po->pAttr[index_pin_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_pin_attr]->isVISIBLE )
					m_view->NewSelect( po->pAttr[index_pin_attr], &tid, 0, 0 );
		if( po->pAttr[index_net_attr] )
			if( po->pAttr[index_net_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_net_attr]->isVISIBLE )
				{
					if( po->pAttr[index_net_attr]->m_tl == m_tlist )
						m_tlist->HighlightText( po->pAttr[index_net_attr] );
					else
						m_view->NewSelect( po->pAttr[index_net_attr], &tid, 0, 0 );
				}
		if( po->pAttr[index_desc_attr] )
			if( po->pAttr[index_desc_attr]->m_selected == 0 )
				if( incInvisible || po->pAttr[index_desc_attr]->isVISIBLE )
					m_view->NewSelect( po->pAttr[index_desc_attr], &tid, 0, 0 );
		if( mem_id.type != ID_NONE )
		{
			m_view->m_sel_id = mem_id;
			m_view->m_sel_text = mem_t;
		}
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::SelectGroupAttributes()
{
	if( m_view->m_sel_flags == VTX_ONLY )
		return;
	//
	id mem_id( m_view->m_sel_id );
	CText * mem_t = m_view->m_sel_text;
	id tid( ID_TEXT_DEF );
	CTextList * tl = Attr->m_Pinlist;
	int it = -1;
	for( CText * txt = tl->GetNextText(&it); txt; txt=tl->GetNextText(&it) )
	{
		if( txt->m_polyline_start >= 0 )
			if( m_outline_poly->GetAt(txt->m_polyline_start).GetSelCount() == m_outline_poly->GetAt(txt->m_polyline_start).GetNumSides() )
				m_view->NewSelect( txt, &tid, 0, 0 );
	}
	tl = Attr->m_Netlist;
	it = -1;
	for( CText * txt = tl->GetNextText(&it); txt; txt=tl->GetNextText(&it) )
	{
		if( txt->m_polyline_start >= 0 )
			if( m_outline_poly->GetAt(txt->m_polyline_start).GetSelCount() == m_outline_poly->GetAt(txt->m_polyline_start).GetNumSides() )
				m_view->NewSelect( txt, &tid, 0, 0 );
	}
	tl = Attr->m_pDesclist;
	it = -1;
	for( CText * txt = tl->GetNextText(&it); txt; txt=tl->GetNextText(&it) )
	{
		if( txt->m_polyline_start >= 0 )
			if( m_outline_poly->GetAt(txt->m_polyline_start).GetSelCount() == m_outline_poly->GetAt(txt->m_polyline_start).GetNumSides() )
				m_view->NewSelect( txt, &tid, 0, 0 );
	}
	tl = Attr->m_Reflist;
	it = -1;
	for( CText * part = tl->GetNextText(&it); part; part=tl->GetNextText(&it) )
	{
		if( part->m_polyline_start >= 0 )
		{
			int SEL_ALL = 1;
			if( m_outline_poly->GetAt(part->m_polyline_start).Next[index_part_attr] == -1 && 
				m_outline_poly->GetAt(part->m_polyline_start).GetSel() )
			{// SEL_ALL
			}// SEL_ALL
			else for( int ip=part->m_polyline_start; ip>=0; ip=m_outline_poly->GetAt(ip).Next[index_part_attr] )
			{
				if( m_outline_poly->GetAt(ip).GetSel() == 0 )
				{
					SEL_ALL = 0;
					break;
				}
			}
			if( SEL_ALL )
			{
				m_view->NewSelect( part, &tid, 0, 0 );
				CText * v = m_outline_poly->GetAt(part->m_polyline_start).Check( index_value_attr );
				CText * f = m_outline_poly->GetAt(part->m_polyline_start).Check( index_foot_attr );
				if( v )
					m_view->NewSelect( v, &tid, 0, 0 );
				if( f )
					m_view->NewSelect( f, &tid, 0, 0 );
			}
		}
	}
	if( mem_id.type != ID_NONE )
	{
		m_view->m_sel_id = mem_id;
		m_view->m_sel_text = mem_t;
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
void CFreePcbDoc::HighlightLineAttributes( CPolyLine * po )
{
	int TR = 0;
	id tid( ID_TEXT_DEF );
	if( po->Check( index_part_attr ) )
		po->pAttr[index_part_attr]->m_tl->HighlightText( po->pAttr[index_part_attr], TR );
	if( po->Check( index_value_attr ) )
		po->pAttr[index_value_attr]->m_tl->HighlightText( po->pAttr[index_value_attr], TR );
	if( po->Check( index_foot_attr ) )
		po->pAttr[index_foot_attr]->m_tl->HighlightText( po->pAttr[index_foot_attr], TR );
	if( po->Check( index_pin_attr ) )
		po->pAttr[index_pin_attr]->m_tl->HighlightText( po->pAttr[index_pin_attr], TR );
	if( po->Check( index_net_attr ) )
		po->pAttr[index_net_attr]->m_tl->HighlightText( po->pAttr[index_net_attr], TR );
	if( po->Check( index_desc_attr ) )
		po->pAttr[index_desc_attr]->m_tl->HighlightText( po->pAttr[index_desc_attr], TR );
}

void CFreePcbDoc::TryToRemoveSuffix()
{
	if( m_view->m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return;
	}
	// reset block indexes
	Pages.m_get_suffixes = 0;
	//
	int current_page = Pages.GetActiveNumber();
	BOOL INCLUDED_ON_NETLIST = Pages.IsThePageIncludedInNetlist( current_page );
	SelectGroupAttributes();
	CString suff = "";
	int it = -1;
	int EN = 0;
	for( CText * ref=Attr->m_Reflist->GetNextText(&it); ref; ref=Attr->m_Reflist->GetNextText(&it) )
	{
		if( ref->m_selected == 0 )
			continue;
		if( suff.GetLength() == 0 )
			suff = ref->m_str.Right(1);
		if( suff.Compare( ref->m_str.Right(1) ) )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"The selected group of parts contains different suffixes! Operation canceled.":
				"Выбранная группа деталей содержит разные суффиксы! Операция отменена.", MB_ICONERROR);
			return;
		}
		CString newR = ref->m_str.Left( ref->m_str.GetLength()-1 );
		if( INCLUDED_ON_NETLIST )
		{
			if( Pages.FindPart( &newR, NULL, TRUE, TRUE ) == -1 )
				EN = 1;
		}
		else
		{
			int it2 = -1;
			if( Attr->m_Reflist->GetText( &newR, &it2 ) == NULL )
				EN = 1;
		}
	}
	if( EN == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Cannot rename these parts.":"Невозможно переименовать эти детали.", MB_ICONERROR);
		return;
	}
	m_dlg_log->Clear();
	m_dlg_log->AddLine( "*** REMOVING REF SUFFIXES ***\r\n" );
	m_dlg_log->OnShowMe();
	m_view->SaveUndoInfoForGroup( m_undo_list );
	CString newRef;
	it = -1;
	for( CText * ref=Attr->m_Reflist->GetNextText(&it); ref; ref=Attr->m_Reflist->GetNextText(&it) )
	{
		if( ref->m_selected == 0 )
			continue;
		newRef = ref->m_str.Left( ref->m_str.GetLength()-1 );
		int ipg = -1;
		if( INCLUDED_ON_NETLIST )
		{
			ipg = Pages.FindPart( &newRef, NULL, TRUE, TRUE );
		}
		else
		{
			int it2 = -1;
			if( Attr->m_Reflist->GetText( &newRef, &it2 ) )
				ipg = current_page;
		}
		if( ipg == -1 )
		{
			Renumber__Add( &ref->m_str, &newRef, current_page );
			ref->Undraw();
			ref->m_str = newRef;
			ref->m_nchars = newRef.GetLength();
			ref->Draw( m_dlist, m_smfontutil );
		}
		else
		{
			CString log, nm;
			Pages.GetPageName( ipg, &nm );
			log.Format( "Cannot rename part %s to part %s. This ref already present on page %d - %s\r\n", ref->m_str, newRef, ipg+1, nm );
			m_dlg_log->AddLine( log );
			m_dlg_log->UpdateWindow();
		}
	}
	ProjectModified( TRUE );
	m_view->m_draw_layer = 0;
	m_view->Invalidate( FALSE );// (doubtful)
}

void CFreePcbDoc::Renumber__Add( CString * old_ref, CString * new_ref, int current_page, int RemoveAt, BOOL CP_RENUMBERING )
{
	if( old_ref->Compare(*new_ref) == 0 )
		return;
	int i1 = old_ref->Find(".");
	if( i1 <= 0 )
		i1 = old_ref->Find("-");
	int i2 = new_ref->Find(".");
	if( i2 <= 0 )
		i2 = new_ref->Find("-");
	if( i1 > 0 || i2 > 0 )
	{
		CString Pref1="", Pref2="", Suff="";
		int index_1 = ParseRef( old_ref, &Pref1, &Suff );
		int index_2 = ParseRef( new_ref, &Pref2, &Suff );
		if( Pref1.Compare(Pref2) == 0 && index_1 == index_2 )
			return;
	}
	CString oldr = *old_ref, newr = *new_ref;
	if( i1 > 0 )
	{
		oldr = old_ref->Left(i1);
	}
	if( i2 > 0 )
	{
		newr = new_ref->Left(i2);
	}
	if( getbit( Pages.m_page_mirror_mask, current_page ) && CP_RENUMBERING == 0 ) // sync ref des on pages
	{
		int it = -1;
		////CText * main_part = Attr->m_Reflist->GetText( old_ref, &it );
		////if( main_part )
		for( int cur_p=0; cur_p<Pages.GetNumPages(); cur_p++ )
		{
			if( getbit( Pages.m_page_mirror_mask, cur_p+MAX_PAGES ) == 0 )
				continue;
			CTextList * part_list = Pages.GetTextList( cur_p, index_part_attr );
			it = -1;
			CText * part = part_list->GetText( old_ref, &it );
			it = -1;
			CText * npart = part_list->GetText( new_ref, &it );
			if( part )
			{
				if( npart )
				{
					CString s, pn;
					Pages.GetPageName( cur_p, &pn );
					s.Format(G_LANGUAGE == 0 ? 
						"Cannot synchronize ref %s on page %s. Part %s already exists on this page!":
						"Невозможно синхронизировать %s на странице %s. Деталь %s уже существует на этой странице!", *old_ref, pn, *new_ref);
					AfxMessageBox( s, MB_ICONERROR );
				}
				else //if( getbit( main_part->m_pdf_options, SYNC_EXCLUDE ) == 0 )
				{
					CDisplayList * dl = part->m_dlist; 
					part->Undraw();
					part->m_str = *new_ref;
					part->m_nchars = new_ref->GetLength();
					part->Draw( dl, m_smfontutil );
				}
			}
		}
	}
	
	if( Pages.IsThePageIncludedInNetlist( current_page ) || CP_RENUMBERING )
	{
		int ipcb = Pages.GetPcbIndex( current_page );
		if( ipcb == -1 )
			return;
		CString log;
		log.Format( RENUMBERING_PATTERN, oldr, newr );
		if( RemoveAt == -1 )
			PartRenumber[ipcb].Add( log );
		else
			PartRenumber[ipcb].RemoveAt(RemoveAt);
		CString Page;
		Pages.GetPageName( current_page, &Page );
		if( m_dlg_log->IsWindowVisible() == 0 )
			m_dlg_log->OnShowMe();
		for( int i=log.GetLength(); i<32; i++ )
			log += " ";
		m_dlg_log->AddLine( log+"\t(page: \""+Page+"\")\r\n" );
		m_dlg_log->UpdateWindow();
	}
	else 
	{	
		CString BASE = dSPACE;
		int it = -1;
		int ip = -1;
		int ipback = ip;
		while( CText * BLK = Pages.FindBLK_PTR( &it, &ip, TRUE ) )
		{
			if( ip != ipback )
				BASE = dSPACE;
			int ipcb = Pages.GetPcbIndex( ip );
			if( ipcb == -1 )
				continue;
			CString Suff;
			int blk_pg = ExtractBLK_PTR( &BLK->m_str, &Suff );
			if( blk_pg != current_page )
				continue;
			if( BASE.Find( dSPACE+Suff+dSPACE ) == -1 )
			{
				BASE += (Suff+dSPACE);
				if( Pages.GetPcbIndex( ip, FALSE ) >= 0 )
				{
					
					CString log;
					log.Format( RENUMBERING_PATTERN, (oldr + Suff), (newr + Suff) );
					if( RemoveAt == -1 )
						PartRenumber[ipcb].Add( log );
					else
						PartRenumber[ipcb].RemoveAt(RemoveAt);
					CString Page;
					Pages.GetPageName( ip, &Page );
					if( m_dlg_log->IsWindowVisible() == 0 )
						m_dlg_log->OnShowMe();
					for( int i=log.GetLength(); i<32; i++ )
						log += " ";
					m_dlg_log->AddLine( log+"\t(page: \""+Page+"\")\r\n" );
					m_dlg_log->UpdateWindow();
					// pcb modified
					m_pcb_modified[ipcb] = TRUE;
				}
				else
				{
					CString oref = *old_ref + Suff;
					CString nref = *new_ref + Suff;
					Renumber__Add( &oref, &nref, ip, RemoveAt, CP_RENUMBERING );
				}
			}
			ipback = ip;
		}
		//else
		//	m_dlg_log->ShowWindow( SW_HIDE );
	}
}


void CFreePcbDoc::RenumberPartDesignations()
{
	if( m_view->m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return;
	}
	SelectGroupAttributes();
	m_view->SaveUndoInfoForGroup( m_undo_list );
	DlgRenumbers dlg;
	CString prefixes = "prefixes: ";
	int it = -1;
	for( CText * ref=Attr->m_Reflist->GetNextText(&it); ref; ref=Attr->m_Reflist->GetNextText(&it) )
	{
		if( ref->m_selected == 0 )
			continue;
		CString pr, sf;
		ParseRef( &ref->m_str, &pr, &sf );
		if( prefixes.Find(" "+pr+" ") <= 0 )
			prefixes += pr + " ";
	}
	dlg.Ini( &prefixes, dlg.RENUMB, m_renumber_left_to_right, 1 );
	if( dlg.DoModal() == IDOK )
	{
		m_renumber_left_to_right = dlg.m_left ;
		PartRenumbering( &dlg.m_prefix_str, &dlg.m_suffix_str, dlg.m_start_num, 0, m_renumber_left_to_right, dlg.m_groups );
	}
}

void CFreePcbDoc::ClearPartDesignations()
{
	if( m_view->m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return;
	}
	SelectGroupAttributes();
	m_view->SaveUndoInfoForGroup( m_undo_list );
	DlgRenumbers dlg;
	CString prefixes = "prefixes: ";
	int it = -1;
	int max_i = 0;
	for( CText * ref=Attr->m_Reflist->GetNextText(&it); ref; ref=Attr->m_Reflist->GetNextText(&it) )
	{
		CString pr, sf;
		int i = ParseRef( &ref->m_str, &pr, &sf );
		//max_i = max( i, max_i );
		if( ref->m_selected == 0 )
			continue;
		if( prefixes.Find(" "+pr+" ") <= 0 )
			prefixes += pr + " ";
	}
	max_i = CLR_REF;
	dlg.Ini( &prefixes, dlg.CLEAR, m_renumber_left_to_right, max_i );
	if( dlg.DoModal() == IDOK )
	{
		PartRenumbering( &dlg.m_prefix_str, &dlg.m_suffix_str, dlg.m_start_num, TRUE, m_renumber_left_to_right, 0 );
	}
}

void CFreePcbDoc::RaisePartNumbers()
{
	if( m_project_modified )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Raise part numbers will be applied to all pages included in the netlist. This tool does not have the ability to undo an action, so to complete this, you must first save the project.":
			"Смещение номеров деталей будет применено ко всем страницам, включенным в список эл.цепей. Этот инструмент не имеет возможности отменить действие, поэтому для его выполнения необходимо сначала сохранить проект.", MB_ICONWARNING);
		return;
	}
	CString instr="";
	if( Check_Txt_Msg( NULL, &instr ) )
	{
		if( instr.GetLength() )
		{
			AfxMessageBox( instr, MB_ICONERROR );
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"This option only works when the PCB and Schematic editor netlists are synchronized. Press the \"File --> Reload Netlist\" menu button in the PCB editor, then save the changes. And then try again":
				"Эта опция работает только тогда, когда списки эл.цепей редактора печатных плат и схемы синхронизированы. Нажмите кнопку меню «Файл --> Перезагрузить список эл.цепей» в редакторе печатных плат ПлатФорм и сохраните изменения. А затем попробуйте еще раз", MB_ICONINFORMATION);
			return;
		}
	}
	// reset block indexes
	Pages.m_get_suffixes = 0;
	//
	int current_page = Pages.GetActiveNumber();
	BOOL INCLUDED_ON_NETLIST = Pages.IsThePageIncludedInNetlist( current_page );
	m_view->CancelSelection();
	m_view->Invalidate( FALSE );// (doubtful)
	DlgRenumbers dlg;
	CString prefixes = "prefixes: ";
	int it = -1;
	for( CText * ref=Attr->m_Reflist->GetNextText(&it); ref; ref=Attr->m_Reflist->GetNextText(&it) )
	{
		CString pr, sf;
		ParseRef( &ref->m_str, &pr, &sf );
		if( prefixes.Find(" "+pr+" ") <= 0 )
			prefixes += pr + " ";
	}
	dlg.Ini( &prefixes, dlg.RAISE, m_renumber_left_to_right, 1 );
	if( dlg.DoModal() == IDOK )
	{
		m_dlg_log->Clear();
		m_dlg_log->AddLine( "*** RAISE PART NUMBER ***\r\n" );
		m_dlg_log->OnShowMe();
		CString prefix = dlg.m_prefix_str;
		int start = dlg.m_start_num;
		int raise_by = dlg.m_shift;
		// mark
		for( int i=0; i<Pages.GetNumPages(); i++ )
		{
			CTextList * tl = Pages.GetTextList( i, index_part_attr );
			tl->MarkAllTexts(0);
		}		
		it = -1;
		int ip = -1;
		if( INCLUDED_ON_NETLIST == 0 )
			ip = current_page;
		for( CText * ref=Pages.GetNextAttr( index_part_attr, &it, &ip, INCLUDED_ON_NETLIST ); ref; ref=Pages.GetNextAttr( index_part_attr, &it, &ip, INCLUDED_ON_NETLIST ) )
		{
			if( INCLUDED_ON_NETLIST == 0 && ip != current_page )
				break;
			CString prx, sfx;
			int i = ParseRef( &ref->m_str, &prx, &sfx );
			if( prefix.Compare( prx ) == 0 )
				if( i >= start )
				{
					ref->m_selected = TRUE;
				}
		}
		it = -1;
		ip = -1;
		if( INCLUDED_ON_NETLIST == 0 )
			ip = current_page;
		for( CText * ref=Pages.GetNextAttr( index_part_attr, &it, &ip, INCLUDED_ON_NETLIST ); ref; ref=Pages.GetNextAttr( index_part_attr, &it, &ip, INCLUDED_ON_NETLIST ) )
		{
			if( INCLUDED_ON_NETLIST == 0 && ip != current_page )
				break;
			if( ref->m_selected )
			{
				CDisplayList * dl = ref->m_dlist;
				ref->Undraw();
				CString new_ref, prx, sfx, log;
				int i = ParseRef( &ref->m_str, &prx, &sfx );
				new_ref.Format( "%s%d%s", prx, i+raise_by, sfx );
				int its = -1;
				CText * gT = NULL;
				while(1)
				{
					if( INCLUDED_ON_NETLIST )
					{
						if( gT )
							Pages.FindPart( &new_ref, &gT, FALSE, INCLUDED_ON_NETLIST );
						else
							Pages.FindPart( &new_ref, &gT, TRUE, INCLUDED_ON_NETLIST );
					}
					else
						gT = Attr->m_Reflist->GetText( &new_ref, &its );
					if( !gT )
						break;
					if( gT->utility )
					{
						CString s;
						s.Format(G_LANGUAGE == 0 ? 
							"It is not possible to renumber part %s. Name %s already exists.\n\nFree number assigned":
							"Невозможно перенумеровать деталь %s. Имя %s уже существует.\n\nНазначен свободный номер", ref->m_str, new_ref);
						AfxMessageBox( s, MB_ICONERROR );
						new_ref = "";
						CText * found = NULL;
						if( INCLUDED_ON_NETLIST )
						{
							Pages.FindPart( &ref->m_str, &found, TRUE, INCLUDED_ON_NETLIST );
							if( found == NULL )
								Pages.FindPart( &ref->m_str, &found, FALSE, INCLUDED_ON_NETLIST ); // scan 2 times to make sure the part is duplicated
						}
						else
						{
							int itf = -1;
							found = Attr->m_Reflist->GetText( &ref->m_str, &itf );
							if( found == NULL )
								found = Attr->m_Reflist->GetText( &ref->m_str, &itf ); // scan 2 times to make sure the part is duplicated
						}
						if( found )
						{
							int new_num = 0;
							for( CText * t = ref; t; )
							{
								new_num++;
								new_ref.Format( "%s%.5d%s", prx, new_num, sfx );
								if( INCLUDED_ON_NETLIST )
									Pages.FindPart( &new_ref, &t, TRUE, INCLUDED_ON_NETLIST );
								else
								{
									int ift = -1;
									t = Attr->m_Reflist->GetText( &new_ref, &ift );
								}
							}
						}
						break;
					}
					else if( gT->m_selected == 0 )
					{
						CString s;
						s.Format(G_LANGUAGE == 0 ? 
							"Error renumbering %s to %s \nPart %s is already used in this project \nIt is recommended to reload the project file without saving":
							"Ошибка перенумерации %s на %s \nДеталь %s уже используется в этом проекте \nРекомендуется перезагрузить файл проекта без сохранения", ref->m_str, new_ref, new_ref);
						AfxMessageBox( s, MB_ICONERROR );
						new_ref += "?";
						break;
					}
				}
				if( (i+raise_by) < 0 )
				{
					CString s;
					s.Format(G_LANGUAGE == 0 ? 
						"As a result of the offset, the part number is negative! \n\n\tOld ref: %s \n\tNew ref: %s \n\nIt is recommended to reload the project file without saving":
						"В результате смещения номер детали отрицательный! \n\n\tСтарая ссылка: %s \n\tНовая ссылка: %s \n\nРекомендуется перезагрузить файл проекта без сохранения", ref->m_str, new_ref);
					AfxMessageBox( s, MB_ICONERROR );
				}
				if( new_ref.GetLength() )
				{
					Renumber__Add( &ref->m_str, &new_ref, ip );
					ref->m_str = new_ref;
					ref->m_nchars = new_ref.GetLength();
				}
				ref->Draw( dl, m_smfontutil );
				ref->utility = 1;
			}
		}
		ProjectModified( TRUE );
	}	
	m_view->m_draw_layer = 0;// RaisePartNumbers
	m_view->Invalidate( FALSE );// (doubtful)
}

void CFreePcbDoc::PartRenumbering( CString * prefix, CString * suffix, int start, BOOL bClear, BOOL LR, BOOL bGroups)
{
	// reset block indexes
	Pages.m_get_suffixes = 0;
	//
	int current_page = Pages.GetActiveNumber();
	BOOL INCLUDED_ON_NETLIST = Pages.IsThePageIncludedInNetlist( current_page );
	int number = 0;
	
	for( int i=0; i<Pages.GetNumPages(); i++ )
	{
		CTextList * tl = Pages.GetTextList( i, index_part_attr );
		tl->MarkAllTexts(0);
	}
	BOOL WAR_1 = 0, WAR_2 = 0;
	CText * Found, * Found2;
	CString prevValue = "";
	do{
		Found = NULL;
		Found2 = NULL;
		int min_x = INT_MAX;
		int max_y = INT_MIN;
		int min_x2 = INT_MAX;
		int max_y2 = INT_MIN;
		int it = -1;
		for( CText * part=Attr->m_Reflist->GetNextText(&it); part; part=Attr->m_Reflist->GetNextText(&it) )
		{
			if( part->isVISIBLE )
				if( part->m_selected )
					if( part->utility == 0 )	
					{
						CString prf, sfx;
						int o_num = ParseRef( &part->m_str, &prf, &sfx );
						BOOL EN = 0;
						if( o_num >= CLR_REF && bClear )
							WAR_1 = TRUE;
						else if( o_num < CLR_REF && bClear == 0 )
							WAR_2 = TRUE;
						else
							EN = 1;
						if( EN )
							if( prefix )
								if( prefix->GetLength() )
									if( prefix->Compare( prf ) )
										EN = 0;
						if( EN )
						{
							if( LR )
							{
								if( part->m_x < min_x )
								{
									Found = part;
									min_x = part->m_x;
									max_y = part->m_y;
								}
								else if( part->m_x == min_x && part->m_y > max_y )
								{
									Found = part;
									max_y = part->m_y;	
								}
								if (bGroups)
								{
									CText* gVal = m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_value_attr];
									BOOL OK = 0;
									if (gVal)
									{
										if (prevValue.Compare(gVal->m_str) == 0)
											OK = 1;
									}
									else if (prevValue.GetLength() == 0)
										OK = 1;
									if (OK)
									{
										if (part->m_x < min_x2)
										{
											Found2 = part;
											min_x2 = part->m_x;
											max_y2 = part->m_y;
										}
										else if (part->m_x == min_x2 && part->m_y > max_y2)
										{
											Found2 = part;
											max_y2 = part->m_y;
										}
									}
								}
							}
							else
							{
								if( part->m_y > max_y )
								{
									Found = part;
									min_x = part->m_x;
									max_y = part->m_y;
								}
								else if( part->m_y == max_y && part->m_x < min_x )
								{
									Found = part;
									min_x = part->m_x;
								}
								if (bGroups)
								{
									CText* gVal = m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_value_attr];
									BOOL OK = 0;
									if (gVal)
									{
										if (prevValue.Compare(gVal->m_str) == 0)
											OK = 1;
									}
									else if (prevValue.GetLength() == 0)
										OK = 1;
									if (OK)
									{
										if (part->m_y > max_y2)
										{
											Found2 = part;
											min_x2 = part->m_x;
											max_y2 = part->m_y;
										}
										else if (part->m_y == max_y2 && part->m_x < min_x2)
										{
											Found2 = part;
											min_x2 = part->m_x;
										}
									}
								}
							}
						}
					}
		}
		number++;
		if (Found2)
		{
			Found2->utility = number;
		}
		else if (Found)
		{
			Found->utility = number;
			CText * gVal = m_outline_poly->GetAt(Found->m_polyline_start).pAttr[index_value_attr];
			if (gVal)
				prevValue = gVal->m_str;
			else
				prevValue = "";
		}
	}while( Found );
	// 
	if( WAR_1 )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Some parts already have a cleaned "\
			"reference designator index. "\
			"The option will not be applied to these parts, "\
			"because repeated cleansing does not make sense":
			"Некоторые детали уже имеют сброшенный "\
			"индекс условного обозначения. "\
			"Опция не будет применена к этим деталям, "\
			"поскольку повторный сброс не имеет смысла", MB_ICONINFORMATION );
	else if( WAR_2 )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This option only applies to parts "\
			"for which the ref number has been cleared. "\
			"Therefore, first clear the symbols "\
			"using the Clear Part Numbers option found on the Tools menu.":
			"Эта опция применима только к деталям, "\
			"для которых номер в обозначении был очищен. "\
			"Поэтому сначала очистите символы, "\
			"используя опцию «Очистить номера деталей» в меню Инструменты.", MB_ICONINFORMATION );
	// 
	if( number == 1 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"No selected items for renumbering":
			"Нет выбранных элементов для перенумерации", MB_ICONINFORMATION);
	}
	else
	{
		m_dlg_log->Clear();
		m_dlg_log->AddLine( "***** RENUMBERING *****\r\n" );
		m_dlg_log->OnShowMe();
	}
	for( int scan=1; scan<number; scan++ )
	{
		int it = -1;
		for( CText * part=Attr->m_Reflist->GetNextText(&it); part; part=Attr->m_Reflist->GetNextText(&it) )
		{
			if( part->utility == scan )
			{
				CString prefix, new_ref, sf;
				int num = ParseRef( &part->m_str, &prefix, &sf );
				
				if( bClear )// remove zero character
					for( int pos=prefix.Find("0"); pos>0; pos=prefix.Find("0") )
						prefix.Delete(pos);
				
				// if no prefix
				if( prefix.GetLength() == 0 )
					prefix = "Unknown";
				
				// add global suffix
				if( suffix->GetLength() )
					sf += *suffix;

				int free_num = start - 1;
				for( CText * t = part; t; )
				{
					free_num++;
					if( bClear )
						new_ref.Format( "%s%.5d%s", prefix, free_num, sf );	
					else
						new_ref.Format( "%s%d%s", prefix, free_num, sf );
					CString mod_ref;
					mod_ref.Format( "%s%d&", prefix, free_num ); // check for multi-parts, for example DA1.1 or DA1-1
					if( INCLUDED_ON_NETLIST )
						Pages.FindPart( &mod_ref, &t, TRUE, INCLUDED_ON_NETLIST );
					else
					{
						int it2 = -1;
						t = Attr->m_Reflist->GetText( &mod_ref, &it2 );
					}
					//
					// when there is a suffix, check on single parts
					if( t == NULL && sf != "" )
					{
						if( INCLUDED_ON_NETLIST )
							Pages.FindPart( &new_ref, &t, TRUE, INCLUDED_ON_NETLIST );
						else
						{
							int it2 = -1;
							t = Attr->m_Reflist->GetText( &new_ref, &it2 );
						}
					}
				}
				//-------------------------------------------------------
				//-------------------Parts with suffix-------------------
				//-------------------------------------------------------
				CString log;
				if( sf.GetLength() )
				{
					CString ref1;
					ref1.Format( "%s%d", prefix, num );
					int it2 = -1;
					int ip = -1;
					if( INCLUDED_ON_NETLIST == 0 )
						ip = current_page;
					for( CText * Scan=Pages.GetNextAttr( index_part_attr, &it2, &ip, INCLUDED_ON_NETLIST ); Scan; Scan=Pages.GetNextAttr( index_part_attr, &it2, &ip, INCLUDED_ON_NETLIST ) )
					{
						if( INCLUDED_ON_NETLIST == 0 && ip != current_page )
							break;
						if( Scan == part )
							continue;
						CString prefix2, ref2, sf2;
						int num2 = ParseRef( &Scan->m_str, &prefix2, &sf2 );
						
						if( bClear )// remove zero character
							for( int pos2=prefix2.Find("0"); pos2>0; pos2=prefix2.Find("0") )
								prefix2.Delete(pos2);
						
						// if no prefix
						if( prefix2.GetLength() == 0 )
							prefix2 = "Unknown";

						// add global suffix
						if( suffix->GetLength() )
							sf2 += *suffix;

						ref2.Format( "%s%d", prefix2, num2 );
						if( ref1.Compare( ref2 ) == 0 )
						{
							while(1)
							{
								if( bClear )
									ref2.Format( "%s%.5d%s", prefix, free_num, sf2 );
								else
									ref2.Format( "%s%d%s", prefix, free_num, sf2 );
								CText * found = NULL;
								if( INCLUDED_ON_NETLIST )
								{
									Pages.FindPart( &ref2, &found, TRUE, INCLUDED_ON_NETLIST );
									if( found == NULL )
										Pages.FindPart( &new_ref, &found, TRUE, INCLUDED_ON_NETLIST );
								}
								else
								{
									int it3 = -1;
									found = Attr->m_Reflist->GetText( &ref2, &it3 );
									if( found == NULL )
									{
										it3 = -1;
										found = Attr->m_Reflist->GetText( &new_ref, &it3 );
									}
								}
								//if( found == NULL ) the use of found==NULL leads to an error in the renumbering of multi-parts
								{
									if( Scan->m_selected == 0 || ip != current_page )
										m_view->SaveUndoInfoForText( Scan, TRUE, m_undo_list );
									Renumber__Add( &Scan->m_str, &ref2, ip );
									CDisplayList * dl = Scan->m_dlist;
									Scan->Undraw();
									Scan->m_str = ref2; 
									Scan->m_nchars = ref2.GetLength();
									Scan->Draw( dl, m_smfontutil );
									Scan->utility = 0;
									break;
								}
								free_num++;
								if( bClear )
									new_ref.Format( "%s%.5d%s", prefix, free_num, sf );	
								else
									new_ref.Format( "%s%d%s", prefix, free_num, sf );	
							}
						}
					}
				}
				//-------------------------------------------------------
				//-------------------------------------------------------
				//-------------------------------------------------------
				Renumber__Add( &part->m_str, &new_ref, current_page );
				CDisplayList * dl = part->m_dlist;
				part->Undraw();
				part->m_str = new_ref; 
				part->m_nchars = new_ref.GetLength();
				part->Draw( dl, m_smfontutil );
			}
		}
	}
	ProjectModified( TRUE );
	m_view->m_draw_layer = 0;// PartRenumbers
	m_view->Invalidate( FALSE );// (doubtful)

	if( m_view->m_dlg_partlist )
		if( m_view->m_dlg_partlist->IsWindowVisible() )
			m_view->m_dlg_partlist->ImportPartlist();
}


void CFreePcbDoc::ReplacePartPattern( CText * newPattern )
{
	int pCOUNT = 0;
	m_dlg_log->Clear();
	m_dlg_log->AddLine( "***** Part Graphics Correction *****\r\n" );
	m_dlg_log->OnShowMe();
	CString Header;
	Attr->m_Reflist->MarkAllTexts(0);
	for( int n_try=0; n_try<4; n_try++ )
	{
		Header.Format("\r\nAttempt number %d, angle %d\r\n",n_try,n_try*90);
		m_dlg_log->AddLine( Header );
		RECT pinRECT;
		pinRECT.left = pinRECT.bottom = INT_MAX/2; // /2 else overflow
		pinRECT.top = pinRECT.right = INT_MIN/2;// //2 else overflow
		// text ptr on 
		// left-bottom pin
		CText * pinText = NULL;
		// find
		// left-bottom pin
		int gMIN = INT_MAX;
		for( int get = newPattern->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_part_attr] )
		{
			CPolyLine * poly = &m_outline_poly->GetAt(get);
			// find
			// left-bottom pin corner
			if( poly->Check( index_pin_attr ) )
			{
				for( int ic=poly->GetNumCorners()-1; ic>=0; ic-- )
				{
					int pinX = poly->GetX(ic);
					int pinY = poly->GetY(ic);
					if( gMIN > pinX+pinY )
					{
						pinText = poly->pAttr[index_pin_attr];
						gMIN = pinX+pinY;
					}
					SwellRect( &pinRECT, pinX, pinY );
				}
			}
		}
		if( gMIN == INT_MAX )
			pinRECT = GetPartBounds( newPattern, NULL );
		int it = -1;
		for( CText * part=Attr->m_Reflist->GetNextText(&it);  part; part=Attr->m_Reflist->GetNextText(&it) )
		{
			if( part->m_selected == 0 )
				continue;
			if( part == newPattern )
				continue;
			if( part->utility )
				continue; // part already updated
			int gpolyC = 0;
			int gpinC = 0;
			RECT gpRECT;
			gpRECT.left = gpRECT.bottom = INT_MAX;
			gpRECT.top = gpRECT.right = INT_MIN;
			// text ptr on 
			// left-bottom pin
			CText * gpText = NULL;
			// find
			// left-bottom pin
			int pMIN = INT_MAX;
			for( int get = part->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_part_attr] )
			{
				gpolyC++;
				if( m_outline_poly->GetAt(get).Check( index_pin_attr ) )
				{
					gpinC++;
					for( int ic=m_outline_poly->GetAt(get).GetNumCorners()-1; ic>=0; ic-- )
					{
						int pinX = m_outline_poly->GetAt(get).GetX(ic);
						int pinY = m_outline_poly->GetAt(get).GetY(ic);
						if( pMIN > pinX+pinY )
						{
							gpText = m_outline_poly->GetAt(get).pAttr[index_pin_attr];
							pMIN = pinX+pinY;
						}
						SwellRect( &gpRECT, pinX, pinY );
					}
				}
			}
			if( pMIN == INT_MAX )
			{
				gpText = pinText;
				gpRECT = GetPartBounds( part, NULL );
			}
			CString s1 = "";
			if( pinText )
				s1 = pinText->m_str;
			CString s2 = "";
			if( gpText )
				s2 = gpText->m_str;
			if( (gpRECT.top-gpRECT.bottom > gpRECT.right-gpRECT.left && pinRECT.top-pinRECT.bottom < pinRECT.right-pinRECT.left) ||
				(gpRECT.top-gpRECT.bottom < gpRECT.right-gpRECT.left && pinRECT.top-pinRECT.bottom > pinRECT.right-pinRECT.left) )
			{
				CString s;
				s.Format("An attempt to correct the graphics of part %s failed. (angels of parts are different)\r\n", part->m_str );
				m_dlg_log->AddLine( s );
				m_dlg_log->UpdateWindow();
			}
			else if( s1.Compare(s2) )
			{
				CString s;
				s.Format("An attempt to correct the graphics of part %s failed. (pin names are different)\r\n", part->m_str );
				m_dlg_log->AddLine( s );
				m_dlg_log->UpdateWindow();
			}
			else
			{
				int dx = gpRECT.left - pinRECT.left;
				int dy = gpRECT.bottom - pinRECT.bottom;
				int start_sz = m_outline_poly->GetSize();
				// copy
				for( int getPatt = newPattern->m_polyline_start; getPatt >= 0; getPatt = m_outline_poly->GetAt(getPatt).Next[index_part_attr] )
				{
					int sz = m_outline_poly->GetSize();
					m_outline_poly->SetSize(sz+1);
					// copy poly
					m_outline_poly->GetAt(sz).Copy( &m_outline_poly->GetAt(getPatt), TRUE );
					id ID = m_outline_poly->GetAt(sz).GetId();
					ID.i = sz;
					m_outline_poly->GetAt(sz).SetId(&ID);
					// clear attr ptr
					for( int i=0; i<num_attributes; i++ )
						m_outline_poly->GetAt(sz).pAttr[i] = NULL;
					// correct poly pos
					for( int ic=m_outline_poly->GetAt(sz).GetNumCorners()-1; ic>=0; ic-- )
					{
						int X = m_outline_poly->GetAt(sz).GetX( ic );
						int Y = m_outline_poly->GetAt(sz).GetY( ic );
						m_outline_poly->GetAt(sz).SetX( ic, X + dx );
						m_outline_poly->GetAt(sz).SetY( ic, Y + dy );
					}
					// update rect
					m_outline_poly->GetAt(sz).RecalcRectC(0);
					int isrc = m_outline_poly->GetAt(sz).GetPicture();
					if( isrc >= 0 )
					{
						m_dlist->MoveTemplate( &m_outline_poly->GetAt(sz).GetCornerBounds(), isrc, m_outline_poly->GetAt(sz).GetLayer() );
					}
					// copy attr
					if( getPatt == newPattern->m_polyline_start )
					{
						int i_att = -1;
						for( CText * attr=ScanAttr( &m_outline_poly->GetAt(part->m_polyline_start), &i_att ); attr; attr=ScanAttr( &m_outline_poly->GetAt(part->m_polyline_start), &i_att ) )
						{
							if( i_att != index_part_attr && i_att != index_value_attr && i_att != index_foot_attr )
								continue;
							int iit = -1;
							CText * newT = attr->m_tl->GetNextInvisibleText(&iit);
							if( newT == NULL )
								newT = attr->m_tl->AddText( attr->m_x + dx,
															attr->m_y + dy,
															attr->m_angle,
															attr->m_layer,
															attr->m_font_size,
															attr->m_stroke_width,
															attr->m_font_number,
															&attr->m_str, FALSE);
							//
							//
							newT->m_x =				attr->m_x;
							newT->m_y =				attr->m_y;
							newT->m_angle =			attr->m_angle;
							newT->m_layer =			attr->m_layer;
							newT->m_font_size =		attr->m_font_size;
							newT->m_stroke_width =	attr->m_stroke_width;
							newT->m_font_number =	attr->m_font_number;
							newT->m_pdf_options =	attr->m_pdf_options;
							newT->m_merge =			attr->m_merge;
							newT->m_submerge =		attr->m_submerge;
							newT->m_str = 			attr->m_str;
							newT->m_nchars =		attr->m_nchars;
							newT->m_polyline_start = sz;
							newT->MakeVisible();
							m_outline_poly->GetAt(sz).pAttr[i_att] = newT;
						}
					}
					else
					{
						//if( m_outline_poly->GetAt(start_sz).pAttr[index_part_attr] && m_outline_poly->GetAt(sz).pAttr[index_part_attr] )
						//{
						//	m_outline_poly->GetAt(start_sz).pAttr[index_part_attr]->m_font_size = m_outline_poly->GetAt(sz).pAttr[index_part_attr]->m_font_size;
						//	m_outline_poly->GetAt(start_sz).pAttr[index_part_attr]->m_stroke_width = m_outline_poly->GetAt(sz).pAttr[index_part_attr]->m_stroke_width;
						//}
						m_outline_poly->GetAt(sz).pAttr[index_part_attr] = m_outline_poly->GetAt(start_sz).pAttr[index_part_attr];
						//
						//if( m_outline_poly->GetAt(start_sz).pAttr[index_value_attr] && m_outline_poly->GetAt(sz).pAttr[index_value_attr] )
						//{
						//	m_outline_poly->GetAt(start_sz).pAttr[index_value_attr]->m_font_size = m_outline_poly->GetAt(sz).pAttr[index_value_attr]->m_font_size;
						//	m_outline_poly->GetAt(start_sz).pAttr[index_value_attr]->m_stroke_width = m_outline_poly->GetAt(sz).pAttr[index_value_attr]->m_stroke_width;
						//}
						m_outline_poly->GetAt(sz).pAttr[index_value_attr] = m_outline_poly->GetAt(start_sz).pAttr[index_value_attr];
						//
						//if( m_outline_poly->GetAt(start_sz).pAttr[index_foot_attr] && m_outline_poly->GetAt(sz).pAttr[index_foot_attr] )
						//{
						//	m_outline_poly->GetAt(start_sz).pAttr[index_foot_attr]->m_font_size = m_outline_poly->GetAt(sz).pAttr[index_foot_attr]->m_font_size;
						//	m_outline_poly->GetAt(start_sz).pAttr[index_foot_attr]->m_stroke_width = m_outline_poly->GetAt(sz).pAttr[index_foot_attr]->m_stroke_width;
						//}
						m_outline_poly->GetAt(sz).pAttr[index_foot_attr] = m_outline_poly->GetAt(start_sz).pAttr[index_foot_attr];
					}
					int i_att = -1;
					for( CText * attr=ScanAttr( &m_outline_poly->GetAt(getPatt), &i_att ); attr; attr=ScanAttr( &m_outline_poly->GetAt(getPatt), &i_att ) )
					{
						if( i_att == index_part_attr || i_att == index_value_attr || i_att == index_foot_attr )
							continue;
						if( attr->m_tl == m_tlist )
							continue;
						int iit = -1;
						CText * newT = attr->m_tl->GetNextInvisibleText(&iit);
						if( newT == NULL )
							newT = attr->m_tl->AddText( attr->m_x + dx,
														attr->m_y + dy,
														attr->m_angle,
														attr->m_layer,
														attr->m_font_size,
														attr->m_stroke_width,
														attr->m_font_number,
														&attr->m_str, FALSE);
						//
						//
						newT->m_x =				attr->m_x + dx;
						newT->m_y =				attr->m_y + dy;
						newT->m_angle =			attr->m_angle;
						newT->m_layer =			attr->m_layer;
						newT->m_font_size =		attr->m_font_size;
						newT->m_stroke_width =	attr->m_stroke_width;
						newT->m_font_number =	attr->m_font_number;
						newT->m_pdf_options =	attr->m_pdf_options;
						newT->m_merge =			attr->m_merge;
						newT->m_submerge =		attr->m_submerge;
						newT->m_str = 			attr->m_str;
						newT->m_nchars =		attr->m_nchars;
						newT->m_polyline_start = sz;
						newT->MakeVisible();
						m_outline_poly->GetAt(sz).pAttr[i_att] = newT;
					}
					// DRAW
					m_outline_poly->GetAt(sz).Draw();
				}
				// hide old part
				for( int del = part->m_polyline_start; del >= 0; )
				{
					int i_att = -1;
					for( CText * attr=ScanAttr( &m_outline_poly->GetAt(del), &i_att ); attr; attr=ScanAttr( &m_outline_poly->GetAt(del), &i_att ) )
						attr->InVisible();
					for( int i=0; i<num_attributes; i++ )
						m_outline_poly->GetAt(del).pAttr[i] = NULL;
					m_outline_poly->GetAt(del).Hide();
					//
					//get next
					int del_prev = del;
					del = m_outline_poly->GetAt(del_prev).Next[index_part_attr];
					//now remove part indexes
					for( int u=0; u<num_attributes; u++ )
						m_outline_poly->GetAt(del_prev).Next[u] = -1; 
				}
				// indexing new part
				if( m_outline_poly->GetAt(start_sz).pAttr[index_part_attr] )
				{
					// indexing new part
					AttributeIndexing( m_outline_poly->GetAt(start_sz).pAttr[index_part_attr], index_part_attr );
					// mark new part
					m_outline_poly->GetAt(start_sz).pAttr[index_part_attr]->utility = 1;
					CString s;
					s.Format("Graphics correction of part %s was successful\r\n", part->m_str );
					m_dlg_log->AddLine( s );
					m_dlg_log->UpdateWindow();
					pCOUNT++;
				}
			}
		}
		// rotate part
		for( int get = newPattern->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_part_attr] )
		{
			CPolyLine * poly = &m_outline_poly->GetAt(get);
			// rotate part 90 degrees
			for( int ic=poly->GetNumCorners()-1; ic>=0; ic-- )
			{
				int pinX = poly->GetX(ic);
				int pinY = poly->GetY(ic);
				int buf = pinY;
				pinY = pinX;
				pinX = -buf;
				poly->SetX(ic,pinX);
				poly->SetY(ic,pinY);
			}
			poly->RecalcRectC(0);
			poly->ModifyGroupRect(&rect(0,0,0,0));
			poly->Draw();
			int num_att = -1;
			for( CText * t=ScanAttr( poly, &num_att ); t; t=ScanAttr( poly, &num_att ) )
			{
				if( get == newPattern->m_polyline_start || 
					num_att == index_pin_attr ||
					num_att == index_net_attr ||
					num_att == index_desc_attr )
				{
					CDisplayList * dl = t->m_dlist;
					t->Undraw();
					int buf = t->m_y;
					t->m_y = t->m_x;
					t->m_x = -buf;
					t->m_angle -= 90;
					if( t->m_angle < 0 )
						t->m_angle += 360;
					t->Draw( dl, m_smfontutil );
				}
			}
		}
	}
	{
		CString s;
		s.Format( "%d parts fixed", pCOUNT );
		m_dlg_log->AddLine( s );
		m_dlg_log->UpdateWindow();
	}
	ResetUndoState();
}


void CFreePcbDoc::UpdatePartPattern( CText * oldPattern, CText * newPattern )
{
	if( oldPattern->m_polyline_start < 0 )
		return;
	if( newPattern->m_polyline_start < 0 )
		return;
	if( m_outline_poly->GetAt( oldPattern->m_polyline_start ).GetNumCorners() == 0 )
		return;
	if( m_outline_poly->GetAt( newPattern->m_polyline_start ).GetNumCorners() == 0 )
		return;
	RECT oldBounds = GetPartBounds( oldPattern, NULL );
	RECT newBounds = GetPartBounds( newPattern, NULL );
	//
	//
	int MARK = 5;
	for( int get2 = newPattern->m_polyline_start; get2 >= 0; get2 = m_outline_poly->GetAt(get2).Next[index_part_attr] )
		m_outline_poly->GetAt(get2).SetUtility(0);
	//
	int dx = oldBounds.left - newBounds.left;
	int dy = oldBounds.bottom - newBounds.bottom;
	dx -= (dx%(int)m_part_grid_spacing);
	dy -= (dy%(int)m_part_grid_spacing);
	for( int get = oldPattern->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_part_attr] )
	{
		id ID = m_outline_poly->GetAt(get).GetId();
		int Sel = m_outline_poly->GetAt(get).GetSideSel();
		// comparing
		int NumS = m_outline_poly->GetAt(get).GetNumSides();
		//int Hatch = m_outline_poly->GetAt(get).GetHatch();
		int Cl = m_outline_poly->GetAt(get).GetClosed();
		//int Layer = m_outline_poly->GetAt(get).GetLayer();
		//
		int W1 = m_outline_poly->GetAt(get).GetW();
		CText * Pin = m_outline_poly->GetAt(get).Check( index_pin_attr );
		CText * Net = m_outline_poly->GetAt(get).Check( index_net_attr );
		if( Net )
			if( Net->m_tl != Attr->m_Netlist )
				Net = NULL;
		CText * Dsc = m_outline_poly->GetAt(get).Check( index_desc_attr );
		CText * Pin2 = NULL;
		CText * Net2 = NULL;
		CText * Dsc2 = NULL;
		int F = -1;
		for( int get2 = newPattern->m_polyline_start; get2 >= 0; get2 = m_outline_poly->GetAt(get2).Next[index_part_attr] )
		{
			CPolyLine * p2 = &m_outline_poly->GetAt(get2);
			// marked
			if( p2->GetUtility() == MARK )
				continue;
			// 
			if( NumS != p2->GetNumSides() )
				continue;
			if( Cl != p2->GetClosed() )
				continue;
			int sstyle = 0;
			for( ; sstyle<NumS; sstyle++ )
			{
				int ss1 = m_outline_poly->GetAt(get).GetSideStyle( sstyle );
				int ss2 = p2->GetSideStyle( sstyle );
				if( ss1 != ss2 )
					break;
			}
			if( sstyle < NumS )
				continue;
			//if( Hatch != p2->GetHatch() )?
			//	continue;
			//if( Layer != p2->GetLayer() )?
			//	continue;
			//
			Pin2 = p2->Check( index_pin_attr );
			if( Pin && Pin2 )
			{

				if( Pin->m_str.Compare( Pin2->m_str ) )
					continue;
			}
			else if( Pin == NULL && Pin2 == NULL )
			{
			}
			else
				continue;
			//
			Net2 = p2->Check( index_net_attr );
			if( Net2 )
				if( Net2->m_tl != Attr->m_Netlist )
					Net2 = NULL;
			if( Net && Net2 )
			{
				if( Net->m_str.Compare( Net2->m_str ) )
					continue;
			}
			else if( Net == NULL && Net2 == NULL )
			{
			}
			else
				continue;
			//
			Dsc2 = p2->Check( index_desc_attr );
			if( Dsc && Dsc2 )
			{
				if( Dsc->m_str.Compare( Dsc2->m_str ) )
					continue;
			}
			else if( Dsc == NULL && Dsc2 == NULL )
			{
			}
			else
				continue;
			F = get2;
			p2->SetUtility( MARK );
			break;
		}

		if( F == -1 )
			ASSERT(0);
		if( Pin && Pin2 )
		{
			Pin->m_x = Pin2->m_x+dx;
			Pin->m_y = Pin2->m_y+dy;
			Pin->m_angle = Pin2->m_angle;
			Pin->m_font_size = Pin2->m_font_size;
			Pin->m_stroke_width = Pin2->m_stroke_width;
			clrbit( Pin->m_pdf_options, COMPRESSED );
			clrbit( Pin->m_pdf_options, UNCLENCHED );
			clrbit( Pin->m_pdf_options, ITALIC );
			Pin->m_pdf_options |= ( Pin2->m_pdf_options & (1<<COMPRESSED) );
			Pin->m_pdf_options |= ( Pin2->m_pdf_options & (1<<UNCLENCHED) );
			Pin->m_pdf_options |= ( Pin2->m_pdf_options & (1<<ITALIC) );
			Pin->Undraw();
			Pin->MakeVisible();
		}
		if( Net && Net2 )
		{
			Net->m_x = Net2->m_x+dx;
			Net->m_y = Net2->m_y+dy;
			Net->m_angle = Net2->m_angle;
			Net->m_font_size = Net2->m_font_size;
			Net->m_stroke_width = Net2->m_stroke_width;
			clrbit( Net->m_pdf_options, COMPRESSED );
			clrbit( Net->m_pdf_options, UNCLENCHED );
			clrbit( Net->m_pdf_options, ITALIC );
			Net->m_pdf_options |= ( Net2->m_pdf_options & (1<<COMPRESSED) );
			Net->m_pdf_options |= ( Net2->m_pdf_options & (1<<UNCLENCHED) );
			Net->m_pdf_options |= ( Net2->m_pdf_options & (1<<ITALIC) );
			Net->Undraw();
			Net->MakeVisible();
		}
		if( Dsc && Dsc2 )
		{
			Dsc->m_x = Dsc2->m_x+dx;
			Dsc->m_y = Dsc2->m_y+dy;
			Dsc->m_angle = Dsc2->m_angle;
			Dsc->m_font_size = Dsc2->m_font_size;
			Dsc->m_stroke_width = Dsc2->m_stroke_width;
			clrbit( Dsc->m_pdf_options, COMPRESSED );
			clrbit( Dsc->m_pdf_options, UNCLENCHED );
			clrbit( Dsc->m_pdf_options, ITALIC );
			Dsc->m_pdf_options |= ( Dsc2->m_pdf_options & (1<<COMPRESSED) );
			Dsc->m_pdf_options |= ( Dsc2->m_pdf_options & (1<<UNCLENCHED) );
			Dsc->m_pdf_options |= ( Dsc2->m_pdf_options & (1<<ITALIC) );
			Dsc->Undraw();
			Dsc->MakeVisible();
		}
		
		if( CText * FootprintText = m_outline_poly->GetAt(get).Check( index_foot_attr ) )
		{
			FootprintText->m_angle = 0;
			FootprintText->m_x = newBounds.right + dx;
			FootprintText->m_y = newBounds.top + dy;
			if( CText * F2 = m_outline_poly->GetAt( newPattern->m_polyline_start ).Check( index_foot_attr ) )
			{
				clrbit( FootprintText->m_pdf_options, COMPRESSED );
				clrbit( FootprintText->m_pdf_options, UNCLENCHED );
				clrbit( FootprintText->m_pdf_options, ITALIC );
				FootprintText->m_pdf_options |= ( F2->m_pdf_options & (1<<COMPRESSED) );
				FootprintText->m_pdf_options |= ( F2->m_pdf_options & (1<<UNCLENCHED) );
				FootprintText->m_pdf_options |= ( F2->m_pdf_options & (1<<ITALIC) );
			}
			FootprintText->Undraw();
			FootprintText->MakeVisible();
		}
		if( CText * ValueText = m_outline_poly->GetAt(get).Check( index_value_attr ) )
		{
			ValueText->m_angle = 0;
			ValueText->m_x = newBounds.right + dx;
			ValueText->m_y = newBounds.top + dy + 2*oldPattern->m_font_size;
			if( CText * V2 = m_outline_poly->GetAt( newPattern->m_polyline_start ).Check( index_value_attr ) )
			{
				clrbit( ValueText->m_pdf_options, COMPRESSED );
				clrbit( ValueText->m_pdf_options, UNCLENCHED );
				clrbit( ValueText->m_pdf_options, ITALIC );
				ValueText->m_pdf_options |= ( V2->m_pdf_options & (1<<COMPRESSED) );
				ValueText->m_pdf_options |= ( V2->m_pdf_options & (1<<UNCLENCHED) );
				ValueText->m_pdf_options |= ( V2->m_pdf_options & (1<<ITALIC) );
			}
			ValueText->Undraw();
			ValueText->MakeVisible();
		}	
		{
			oldPattern->m_angle = 0;
			oldPattern->m_x = newBounds.right + dx;
			oldPattern->m_y = newBounds.top + dy + 4*oldPattern->m_font_size;
			{
				clrbit( oldPattern->m_pdf_options, COMPRESSED );
				clrbit( oldPattern->m_pdf_options, UNCLENCHED );
				clrbit( oldPattern->m_pdf_options, ITALIC );
				oldPattern->m_pdf_options |= ( newPattern->m_pdf_options & (1<<COMPRESSED) );
				oldPattern->m_pdf_options |= ( newPattern->m_pdf_options & (1<<UNCLENCHED) );
				oldPattern->m_pdf_options |= ( newPattern->m_pdf_options & (1<<ITALIC) );
			}
			oldPattern->Undraw();
			oldPattern->MakeVisible();
		}
		int mer = m_outline_poly->GetAt(get).GetMerge();
		int sub_mer = m_outline_poly->GetAt(get).GetMerge( TRUE );
		//
		m_outline_poly->GetAt(get).Copy( &m_outline_poly->GetAt(F), 1, 0 );
		m_outline_poly->GetAt(get).SetId( &ID );
		m_outline_poly->GetAt(get).SetW(W1);
		for( int n=0; n<m_outline_poly->GetAt(get).GetNumCorners(); n++ )
		{
			int x = m_outline_poly->GetAt(get).GetX(n);
			int y = m_outline_poly->GetAt(get).GetY(n);
			m_outline_poly->GetAt(get).SetX(n,x+dx); 
			m_outline_poly->GetAt(get).SetY(n,y+dy); 
		}
		m_outline_poly->GetAt(get).SetMerge( mer );
		m_outline_poly->GetAt(get).SetMerge( sub_mer, TRUE );
		m_outline_poly->GetAt(get).Draw();

		if( Sel )
		{
			for( int n=0; n<m_outline_poly->GetAt(get).GetNumSides(); n++ )
				m_outline_poly->GetAt(get).SetSideSel(n,TRUE);
			//m_view->m_sel_count += m_outline_poly->GetAt(get).GetSelCount();
			setbit( m_view->m_sel_flags, m_view->FLAG_SEL_OP );
			setbit( m_view->m_sel_flags, m_view->FLAG_SEL_SIDE );
		}
	}

	AttributeIndexing( oldPattern, index_part_attr );
	if( m_dlg_log )
		m_dlg_log->AddLine("(part graphic correction applied: "+oldPattern->m_str+")\r\n");
}

void CFreePcbDoc::CheckComponentInLibrary( CText * attr, CString * old_value, CString * old_footprint )
{
	static CString Fprints = "";
	static CString Fcomps = "";
	if( attr == NULL )
	{
		Fprints = "";
		Fcomps = "";
		return;
	}
	if( attr->m_polyline_start < 0 )
		return;

	CPolyLine * p = &m_outline_poly->GetAt( attr->m_polyline_start );
	CText * part =  p->Check( index_part_attr );
	CText * foot =  p->Check( index_foot_attr );
	CText * value = p->Check( index_value_attr );
	if( part && value && foot )
	{
		if( old_value && old_footprint )
		{
			ExtractComponentName( old_value, old_footprint );
			if( old_value->GetLength() && old_footprint->GetLength() )
			{
				CString newV = value->m_str;
				CString newF = foot->m_str;
				ExtractComponentName( &newV, &newF );
				if( newV.GetLength() && 
					newF.GetLength() )
				{
					CArray<CText*> V_Array;
					V_Array.RemoveAll();
					V_Array.Add( value );
					if( Pages.FindPart( &newV, &newF, TRUE, &V_Array ) == 0 )
					{
						CString CDir, CDir2="";
						int rf = m_pcb_full_path.ReverseFind('\\');
						if( rf > 0 )
							CDir2 = m_pcb_full_path.Left(rf) + rel_f_component;
						CDir = m_app_dir + main_component;
					
						CString CNote = CDir + "\\" + *old_value + "@" + *old_footprint;
						CString NewNote = CDir + "\\" + newV + "@" + newF;
						CString CNoteLoc = CDir2 + "\\" + *old_value + "@" + *old_footprint;
						struct _stat buf;
						//
						if( CNote.CompareNoCase( NewNote ) )
						{
							CString str_docx = (dSPACE + CNote+".docx" + NewNote+".docx");
							if( _stat( CNote+".docx", &buf ) == 0 &&
								_stat( NewNote+".docx", &buf ) &&
								Fcomps.Find( str_docx ) == -1 )
							{
								Fcomps += str_docx;
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									("You have renamed the part to which the datasheet file was attached with the name \n\n"+CNote+".DOCX. \n\nThis file will no longer be called from the <Component Datasheet> right-click menu because the new part name does not match the file name. To save the binding of the datasheet file to the part, you need to rename file. Do it automatically?"):
									("Вы переименовали деталь, к которой был прикреплен файл таблицы данных, на имя \n\n" + CNote + ".DOCX. \n\nЭтот файл больше не будет вызываться из контекстного меню «Вложения компонента», поскольку новое имя детали не соответствует имени файла. Чтобы сохранить привязку файла таблицы данных к детали, необходимо переименовать файл. Сделать это автоматически?"), MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( CopyFile( CNote+".docx", NewNote+".docx", 1 ) )
									{
										ret = AfxMessageBox(G_LANGUAGE == 0 ? "Delete old docx file?":"Удалить старый файл docx?", MB_ICONQUESTION | MB_YESNO);
										if( ret == IDYES )
										{
											SetFileAttributes( CNote+".docx", 0 );
											DeleteFile( CNote+".docx" );
											SetFileAttributes( CNoteLoc+".docx", 0 );
											DeleteFile( CNoteLoc+".docx" );
										}
									}
								}
							}
							CString str_doc = (dSPACE + CNote+".doc" + NewNote+".doc");
							if( _stat( CNote+".doc", &buf ) == 0 &&
								_stat( NewNote+".doc", &buf ) &&
								Fcomps.Find( str_doc ) == -1 )
							{
								Fcomps += str_doc;
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									("You have renamed the part to which the datasheet file was attached with the name \n\n"+CNote+".DOC. \n\nThis file will no longer be called from the <Component Datasheet> right-click menu because the new part name does not match the file name. To save the binding of the datasheet file to the part, you need to rename file. Do it automatically?"):
									("Вы переименовали деталь, к которой был прикреплен файл таблицы данных, на имя \n\n" + CNote + ".DOC. \n\nЭтот файл больше не будет вызываться из контекстного меню «Вложения компонента», поскольку новое имя детали не соответствует имени файла. Чтобы сохранить привязку файла таблицы данных к детали, необходимо переименовать файл. Сделать это автоматически?"), MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( CopyFile( CNote+".doc", NewNote+".doc", 1 ) )
									{
										ret = AfxMessageBox(G_LANGUAGE == 0 ? "Delete old doc file?":"Удалить старый файл doc?", MB_ICONQUESTION | MB_YESNO);
										if( ret == IDYES )
										{
											SetFileAttributes( CNote+".doc", 0 );
											DeleteFile( CNote+".doc" );
											SetFileAttributes( CNoteLoc+".doc", 0 );
											DeleteFile( CNoteLoc+".doc" );
										}
									}
								}
							}
							CString str_txt = (dSPACE + CNote+".txt" + NewNote+".txt");
							if( _stat( CNote+".txt", &buf ) == 0 &&
								_stat( NewNote+".txt", &buf ) &&
								Fcomps.Find( str_txt ) == -1 )
							{
								Fcomps += str_txt;
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									("You have renamed the part to which the datasheet file was attached with the name \n\n"+CNote+".TXT. \n\nThis file will no longer be called from the <Component Datasheet> right-click menu because the new part name does not match the file name. To save the binding of the datasheet file to the part, you need to rename file. Do it automatically?"):
									("Вы переименовали деталь, к которой был прикреплен файл таблицы данных, на имя \n\n" + CNote + ".TXT. \n\nЭтот файл больше не будет вызываться из контекстного меню «Вложения компонента», поскольку новое имя детали не соответствует имени файла. Чтобы сохранить привязку файла таблицы данных к детали, необходимо переименовать файл. Сделать это автоматически?"), MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( CopyFile( CNote+".txt", NewNote+".txt", 1 ) )
									{
										ret = AfxMessageBox(G_LANGUAGE == 0 ? 
											"Delete old txt file?":
											"Удалить старый текстовый файл?", MB_ICONQUESTION | MB_YESNO);
										if( ret == IDYES )
										{
											SetFileAttributes( CNote+".txt", 0 );
											DeleteFile( CNote+".txt" );
											SetFileAttributes( CNoteLoc+".txt", 0 );
											DeleteFile( CNoteLoc+".txt" );
										}
									}
								}
							}
							CString str_pdf = (dSPACE + CNote+".pdf" + NewNote+".pdf");
							if( _stat( CNote+".pdf", &buf ) == 0 &&
								_stat( NewNote+".pdf", &buf ) &&
								Fcomps.Find( str_pdf ) == -1 )
							{
								Fcomps += str_pdf;
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									("You have renamed the part to which the datasheet file was attached with the name \n\n"+CNote+".PDF. \n\nThis file will no longer be called from the <Component Datasheet> right-click menu because the new part name does not match the file name. To save the binding of the datasheet file to the part, you need to rename file. Do it automatically?"):
									("Вы переименовали деталь, к которой был прикреплен файл таблицы данных, на имя \n\n" + CNote + ".PDF. \n\nЭтот файл больше не будет вызываться из контекстного меню «Вложения компонента», поскольку новое имя детали не соответствует имени файла. Чтобы сохранить привязку файла таблицы данных к детали, необходимо переименовать файл. Сделать это автоматически?"), MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( CopyFile( CNote+".pdf", NewNote+".pdf", 1 ) )
									{
										ret = AfxMessageBox(G_LANGUAGE == 0 ? 
											"Delete old pdf file?":
											"Удалить старый PDF-файл?", MB_ICONQUESTION | MB_YESNO);
										if( ret == IDYES )
										{
											SetFileAttributes( CNote+".pdf", 0 );
											DeleteFile( CNote+".pdf" );
											SetFileAttributes( CNoteLoc+".pdf", 0 );
											DeleteFile( CNoteLoc+".pdf" );
										}
									}
								}
							}
							CString str_url = (dSPACE + CNote+".url" + NewNote+".url");
							if( _stat( CNote+".url", &buf ) == 0 &&
								_stat( NewNote+".url", &buf ) &&
								Fcomps.Find( str_url ) == -1 )
							{
								Fcomps += str_url;
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									"The URL may be lost if the link filename is not corrected now. Do you want to keep this URL?":
									"URL может быть утерян, если имя файла ссылки не будет исправлено сейчас. Вы хотите сохранить этот URL?", MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( CopyFile( CNote+".url", NewNote+".url", 1 ) )
									{
										ret = AfxMessageBox(G_LANGUAGE == 0 ? 
											"Delete old url file?":
											"Удалить старый файл URL?", MB_ICONQUESTION | MB_YESNO);
										if( ret == IDYES )
										{
											SetFileAttributes( CNote+".url", 0 );
											DeleteFile( CNote+".url" );
											SetFileAttributes( CNoteLoc+".url", 0 );
											DeleteFile( CNoteLoc+".url" );
										}
									}
								}
							}
						}
						if( old_footprint->CompareNoCase( newF ) &&
							Fprints.Find( dSPACE + *old_footprint + newF ) == -1 )
						{
							Fprints += (dSPACE + *old_footprint + newF);

							CString Pic1 = CDir + "\\" + *old_footprint;
							CString Pic2 = CDir + "\\" + newF;
							CString Pic3 = CDir2 + "\\" + *old_footprint;

							BOOL b1 =	(_stat( Pic1+".bmp", &buf ) == 0 |
										_stat( Pic1+".jpg", &buf ) == 0 |
										_stat( Pic1+".jpeg", &buf ) == 0);
							BOOL b2 =	(_stat( Pic2+".bmp", &buf ) == 0 |
										_stat( Pic2+".jpg", &buf ) == 0 |
										_stat( Pic2+".jpeg", &buf ) == 0);
							if( b1 && b2 == 0 )
							{
								int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
									"The part package image may be lost because you changed the name of the footprint. The previous footprint name is associated with the image of the part body. Do you want to keep linking this image to the new footprint? (if the new footprint is associated with the same part package, then click YES)":
									"Изображение корпуса детали может быть утеряно, поскольку вы изменили имя футпринта. Предыдущее имя футпринта было связано с изображением корпуса детали. Хотите сохранить привязку этого изображения к новому футпринту? (если новый футпринт должен быть связан с тем же корпусом детали, нажмите ДА)", MB_ICONQUESTION | MB_YESNO);
								if( ret == IDYES )
								{
									if( _stat( Pic1+".bmp", &buf ) == 0 &&
										_stat( Pic2+".bmp", &buf ) )
									{
										if( CopyFile( Pic1+".bmp", Pic2+".bmp", 1 ) )
										{
											ret = AfxMessageBox(G_LANGUAGE == 0 ? 
												("Delete old image file?\n\n"+Pic1+".bmp"):
												("Удалить старый файл изображения?\n\n" + Pic1 + ".bmp"), MB_ICONQUESTION | MB_YESNO);
											if( ret == IDYES )
											{
												SetFileAttributes( Pic1+".bmp", 0 );
												DeleteFile( Pic1+".bmp" );
												SetFileAttributes( Pic3+".bmp", 0 );
												DeleteFile( Pic3+".bmp" );
											}
										}
									}
									else if( _stat( Pic1+".jpg", &buf ) == 0 &&
										_stat( Pic2+".jpg", &buf ) )
									{
										if( CopyFile( Pic1+".jpg", Pic2+".jpg", 1 ) )
										{
											ret = AfxMessageBox(G_LANGUAGE == 0 ? 
												("Delete old image file?\n\n"+Pic1+".jpg"):
												("Удалить старый файл изображения?\n\n" + Pic1 + ".jpg"), MB_ICONQUESTION | MB_YESNO);
											if( ret == IDYES )
											{
												SetFileAttributes( Pic1+".jpg", 0 );
												DeleteFile( Pic1+".jpg" );
												SetFileAttributes( Pic3+".jpg", 0 );
												DeleteFile( Pic3+".jpg" );
											}
										}
									}
									else if( _stat( Pic1+".jpeg", &buf ) == 0 &&
										_stat( Pic2+".jpeg", &buf ) )
									{
										if( CopyFile( Pic1+".jpeg", Pic2+".jpeg", 1 ) )
										{
											ret = AfxMessageBox(G_LANGUAGE == 0 ? 
												("Delete old image file?\n\n"+Pic1+".jpeg"):
												("Удалить старый файл изображения?\n\n" + Pic1 + ".jpeg"), MB_ICONQUESTION | MB_YESNO);
											if( ret == IDYES )
											{
												SetFileAttributes( Pic1+".jpeg", 0 );
												DeleteFile( Pic1+".jpeg" );
												SetFileAttributes( Pic3+".jpeg", 0 );
												DeleteFile( Pic3+".jpeg" );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if( m_library.IsEmpty() )
			return;
		CString R = part->m_str;
		CString V = value->m_str;
		CString F = foot->m_str;
		CString Pref, Suff;
		ParseRef( &R, &Pref, &Suff );
		CString VdogF = "[" + Pref + "] " + V + "@" + F;
		m_library.SetPath( &m_pcb_full_path );
		if( m_library.isPresent( &VdogF ) == 0 )
			m_library.AddItem( &VdogF );
	}
}

void CFreePcbDoc::HighLightNet( CText * Net )
{
	if( Net )
	{
		/*int it = -1;
		for( CText * n = m_tlist->GetNextText(&it); n; n = m_tlist->GetNextText(&it) )
		{
			if( Net->m_str.Compare( n->m_str ) == 0 )
				for( int get = n->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_net_attr] )
					for( int ico=m_outline_poly->GetAt(get).GetNumCorners()-1; ico>=0; ico-- )
						m_outline_poly->GetAt(get).HighlightSide( ico, m_outline_poly->GetAt(get).GetW()*8, 0 );
		}
		it = -1;
		for( CText * n = Attr->m_Netlist->GetNextText(&it); n; n = Attr->m_Netlist->GetNextText(&it) )
		{
			if( Net->m_str.Compare( n->m_str ) == 0 )
				for( int get = n->m_polyline_start; get >= 0; get = m_outline_poly->GetAt(get).Next[index_net_attr] )
					for( int ico=m_outline_poly->GetAt(get).GetNumCorners()-1; ico>=0; ico-- )
						m_outline_poly->GetAt(get).HighlightSide( ico, m_outline_poly->GetAt(get).GetW()*8, 0 );
		}*/
		CRect T = GetPolylineBounds();
		for( int i=0; i<m_outline_poly->GetSize(); i++ )
		{
			if( m_outline_poly->GetAt(i).m_visible == 0 )
				continue;
			if( m_outline_poly->GetAt(i).Check( index_net_attr ) == 0 )
				continue;
			//
			if( m_outline_poly->GetAt(i).pAttr[index_net_attr] == Net )
			{
			}
			else if( Net->m_str.Compare( m_outline_poly->GetAt(i).pAttr[index_net_attr]->m_str ) )
				continue;
			for( int ico=m_outline_poly->GetAt(i).GetNumCorners()-1; ico>=0; ico-- )
				m_outline_poly->GetAt(i).HighlightSide( ico, m_outline_poly->GetAt(i).GetW()*2+(T.Width()/200), 0 );
		}
	}
}

void CFreePcbDoc::OnFilePrint()
{
	id print_area_check;
	print_area_check = Pages.FindPrintableArea( &print_area_check );
	if( m_view->m_sel_count )
	{
		print_area_check = -1;
		clrbit( m_pdf_checking, 11 );
	}
	if(m_system_colors)
		setbit(m_pdf_checking, 10);
	else
		clrbit(m_pdf_checking, 10);
	CDlgPDFLayers dlg;
	dlg.Initialize( m_num_layers, 
					m_pdf_vis, 
					m_pdf_rgb, 
					cpdf_fontnamelist, 
					48, 
					m_pdf_font_i, 
					m_pdf_margin, 
					m_pdf_checking,
					print_area_check.i+1,
					Pages.GetNumPages() );

	int ret = dlg.DoModal();
	m_pdf_font_i = dlg.i_font;
	m_pdf_margin = dlg.m_margin;
	m_pdf_checking = dlg.m_pdf_checking;

	//
	int m_pdf_use_font =		getbit( dlg.m_pdf_checking, 0 );
	int m_apply_invert_for_pdf =getbit( dlg.m_pdf_checking, 9 );
		m_system_colors =		getbit( dlg.m_pdf_checking, 10 );
	int m_print_separate =		getbit( dlg.m_pdf_checking, 11 );
	SaveOptions();

	//
	CheckBOM();
	if( ret == IDCANCEL )
		return;
	// pin text alignment
	int it = -1;
	for( CText * iT=Attr->m_Pinlist->GetNextText(&it); iT; iT=Attr->m_Pinlist->GetNextText(&it) )
		PinTextAlignment( iT );
	it = -1;
	for( CText * iT=Attr->m_pDesclist->GetNextText(&it); iT; iT=Attr->m_pDesclist->GetNextText(&it) )
		PinTextAlignment( iT );
	// formatting constants
	// set page size
	double PageWidth = 210.0/25.4;	
	double PageHeight = 297.0/25.4;
	//
	int mode;
	int pagenum = 1;
	CPDFdoc * pdf;
	pdf = cpdf_open(0, NULL);
	cpdf_init(pdf);
	cpdf_setTitle(pdf,m_name.GetBuffer());

	int cP = 0;
	int mem_n_page = Pages.GetActiveNumber();
	BOOL bPrintWasMade = 0;
	id ipoly( ID_POLYLINE, ID_GRAPHIC, -1, ID_SIDE, 0 );
	for( ; cP<Pages.GetNumPages(); ipoly.i==-1?(cP=cP+1, bPrintWasMade=0, ipoly.i=-1):cP=cP )
	{
		if( cP != Pages.GetActiveNumber() )
			SwitchToPage(cP,TRUE,FALSE);
		if( getbit( m_pdf_checking, cP+1 ) == 0 )
			continue;
		if( !m_view->m_sel_count || m_print_separate )
		{
			ipoly = Pages.FindPrintableArea( &ipoly );
			if( ipoly.i >= 0 )
			{
				m_view->NewSelect( NULL, &ipoly, 0, 0 );
				m_view->SelectContour();
			}
			else if( m_print_separate )
				continue;
			else if( bPrintWasMade )
				continue;
		}
		bPrintWasMade = 1;
		RECT r_dsp = GetPolylineBounds();
		CString format = A4;
		PageHeight = 297.0/25.4;
		double W = 595.0;
		double H = 842.0;
		if( m_view->m_sel_count && m_view->m_sel_flags != PART_ONLY )    //getbit( m_view->m_sel_flags, FLAG_SEL_TEXT ) )
		{
			r_dsp = m_dlist->GetHighlightedBounds( NULL );
			mode = PORTRAIT;
			double kw = (double)(r_dsp.top-r_dsp.bottom)/(double)(r_dsp.right-r_dsp.left);
			PageHeight = PageWidth*kw;
			H = W*kw;
			format.Format("0 0 %d %d", (int)W, (int)H );
		}
		else if( r_dsp.right  - r_dsp.left > r_dsp.top - r_dsp.bottom )
			mode = LANDSCAPE;
		else
			mode = PORTRAIT;
		if(pagenum > 1)
			cpdf_finalizePage(pdf, pagenum - 1);
		cpdf_pageInit(pdf, pagenum, mode, format, format);		/* page orientation */
		pagenum++;
		cpdf_setlinecap( pdf, 1 );	// round end-caps
		const double clr_pdf = 1.0/255.0;
		if( m_pdf_rgb[LAY_BACKGND][0] + m_pdf_rgb[LAY_BACKGND][1] + m_pdf_rgb[LAY_BACKGND][2] < 760 )
		{
			cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][0], 
										clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][1], 
										clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][2] );
			cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][0], 
										clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][1], 
										clr_pdf*(float)m_pdf_rgb[LAY_BACKGND][2] );
			cpdf_setlinewidth( pdf, 1.0 );
			cpdf_newpath( pdf );
			cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin );
			cpdf_lineto( pdf, m_pdf_margin, PageHeight - m_pdf_margin );
			cpdf_lineto( pdf, PageWidth - m_pdf_margin, PageHeight - m_pdf_margin );
			cpdf_lineto( pdf, PageWidth - m_pdf_margin, m_pdf_margin );
			cpdf_lineto( pdf, m_pdf_margin, m_pdf_margin );
			cpdf_fill( pdf );
			cpdf_closepath( pdf );	
		}
		double pdf_w = PageWidth - 2*m_pdf_margin;
		double pdf_h = PageHeight - 2*m_pdf_margin;
		const double k_pdf = 1.0/2555.0;
		
		// rectangles
		float cx_pdf = pdf_w/2.0 + m_pdf_margin;
		float cy_pdf = pdf_h/2.0 + m_pdf_margin;
		float cx_dsp = ((float)r_dsp.left + (float)r_dsp.right)/2.0/(float)m_pcbu_per_wu;
		float cy_dsp = ((float)r_dsp.top + (float)r_dsp.bottom)/2.0/(float)m_pcbu_per_wu;
		float max_x_pdf = PageWidth - m_pdf_margin;
		float max_y_pdf = PageHeight - m_pdf_margin;
		float max_x_dsp = (float)r_dsp.right/(float)m_pcbu_per_wu;
		float max_y_dsp = (float)r_dsp.top/(float)m_pcbu_per_wu;
		double k1, k2;
		if( mode == LANDSCAPE )
		{	
			k1 = (max_y_dsp - cy_dsp)/(max_x_pdf - cx_pdf);
			k2 = (max_x_dsp - cx_dsp)/(max_y_pdf - cy_pdf);
		}
		else
		{
			k1 = (max_x_dsp - cx_dsp)/(max_x_pdf - cx_pdf);
			k2 = (max_y_dsp - cy_dsp)/(max_y_pdf - cy_pdf);
		}
		k1 = max( k1, k2 );

		CString Comp = m_app_dir + main_component;

		// draw polylines
		for( int order=(MAX_LAYERS-1); order>=0; order-- )
		{
			int layer = m_dlist->GetLayer(order);
			if( !m_pdf_vis[layer] || layer == LAY_SELECTION )
				continue;

			// print bmp pictures
			for( int itmp=0; itmp<m_dlist->GetNumTemplates(); itmp++ )
				m_dlist->Marker();
			for( ; m_dlist->m_show_pictures; )
			{
				// first we draw
				// pictures of a
				// larger area
				CString gBMP;
				RECT rt;
				int vis=0, st=0, format=0, marker=0;
				int max_w = 0; 
				int ret_i = -1;
				for( int itmp=0; itmp<m_dlist->GetNumTemplates(); itmp++ )
				{
					int lay = m_dlist->GetTemplate( itmp, &gBMP, &rt, &st, &vis, &format, &marker );
					if( lay != layer )
						continue;
					if( vis == 0 )
						continue;
					if( format != JPEG )// ( because BMP is not supported )
						continue;
					if( rt.left == rt.right )
						continue;
					if( marker )
						continue;
					if( rt.right-rt.left < max_w )
						continue;
					ret_i = itmp;
					max_w = rt.right-rt.left;
				}
				if( ret_i == -1 )
					break;
				//
				m_dlist->GetTemplate( ret_i, &gBMP, &rt, &st, &vis );
				CSize sz = m_dlist->GetTemplateSZ(ret_i);
				float picx = rt.right - cx_dsp;
				float picy = rt.bottom - cy_dsp;
				float maxx = rt.left - cx_dsp;
				float maxy = rt.top - cy_dsp;
				picx /= k1;
				picy /= k1;
				maxx /= k1;
				maxy /= k1;
				float ang = 0;
				if( mode == LANDSCAPE )
				{
					picx = -picx;
					maxx = -maxx;
					picx += cy_pdf;
					picy += cx_pdf;
					maxx += cy_pdf;
					maxy += cx_pdf;
					ang = 270.0;
					// swap
					picx += picy;
					picy = picx - picy;
					picx = picx - picy;
					maxx += maxy;
					maxy = maxx - maxy;
					maxx = maxx - maxy;
				}
				else
				{
					picx += cx_pdf;
					picy += cy_pdf;
					maxx += cx_pdf;
					maxy += cy_pdf;
				}	
				float szx = 0.0;
				float szy = 0.0;
				float picW = abs(maxx - picx);
				float picH = abs(maxy - picy);
				if( mode==LANDSCAPE )
				{
					picy += picH;
					picW += picH;
					picH = picW - picH;
					picW = picW - picH;
				}
				else
					picx -= picW;
				float OpicW = picW/(PageWidth);
				float OpicH = picH/(PageHeight);
				picW = sz.cx/26.46/W;
				picH = sz.cy/26.46/H;
				float scx = OpicW/(picW);
				float scy = OpicH/(picH);
				int INV = 0;
				if( st == 0 && m_apply_invert_for_pdf == 0 )
					INV = IM_INVERT;
				//Insert
				cpdf_importImage( pdf, gBMP, JPEG_IMG, picx, picy, ang, &szx, &szy, &scx, &scy, IM_GSAVE|INV );
				
				// set mark
				m_dlist->Marker( 1, ret_i );
			}
			if( m_pdf_rgb[layer][0] == 255 &&
				m_pdf_rgb[layer][1] == 255 &&
				m_pdf_rgb[layer][2] == 255 )
				continue;
			cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[layer][0], 
										clr_pdf*(float)m_pdf_rgb[layer][1], 
										clr_pdf*(float)m_pdf_rgb[layer][2] );
			cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_pdf_rgb[layer][0], 
										clr_pdf*(float)m_pdf_rgb[layer][1], 
										clr_pdf*(float)m_pdf_rgb[layer][2] );
			for( dl_element * el=(layer==LAY_HILITE?m_dlist->Get_Start(layer)->next_HLT:m_dlist->Get_Start(layer)->next); 
				( el->next && el != m_dlist->Get_End(LAY_HILITE) ); 
				el=(layer==LAY_HILITE?el->next_HLT:el->next) )
			{
				if( el->visible == 0 )
					continue;
				if( el == m_dlist->Get_End(layer) )
					continue;
				if( el->id.type == ID_DRC )
					continue;	

				float w = (float)el->el_w*(70.0)/k1;
				cpdf_setlinewidth( pdf, w );
				CArray<CPoint> * PTS = m_dlist->Get_Points( el, NULL, 0 );
				if( el->gtype == DL_LINE || (el->gtype == DL_LINES_ARRAY && el->id.type != ID_TEXT ) )
				{
					if( el->half_tone )
					{
						float proc = (100.0 - (float)el->half_tone)/100.0;
						int c1 = (float)m_pdf_rgb[layer][0] + (float)(255.0-m_pdf_rgb[layer][0])*proc;
						int c2 = (float)m_pdf_rgb[layer][1] + (float)(255.0-m_pdf_rgb[layer][1])*proc;
						int c3 = (float)m_pdf_rgb[layer][2] + (float)(255.0-m_pdf_rgb[layer][2])*proc;
						cpdf_setrgbcolorStroke(pdf, clr_pdf*c1, clr_pdf*c2, clr_pdf*c3 );
					}
					int sz = PTS->GetSize();			
					for( int ii=0; ii+1<sz; ii+=2 )
					{
						double xi = PTS->GetAt(ii).x - cx_dsp;
						double xf = PTS->GetAt(ii+1).x - cx_dsp;
						double yi = PTS->GetAt(ii).y - cy_dsp;
						double yf = PTS->GetAt(ii+1).y - cy_dsp;
						if( PTS->GetAt(ii).x == PTS->GetAt(ii+1).x && PTS->GetAt(ii).y == PTS->GetAt(ii+1).y )
							continue;
						xi /= k1;
						xf /= k1;
						yi /= k1;
						yf /= k1;
						if( mode == LANDSCAPE )
						{
							xi = -xi;
							xf = -xf;
							xi += cy_pdf;
							xf += cy_pdf;
							yi += cx_pdf;
							yf += cx_pdf;
						}
						else
						{
							xi += cx_pdf;
							xf += cx_pdf;
							yi += cy_pdf;
							yf += cy_pdf;
						}	
						cpdf_newpath( pdf );
						if( mode == LANDSCAPE )
						{	
							cpdf_moveto( pdf, yi, xi );
							cpdf_lineto( pdf, yf, xf );
						}
						else
						{
							cpdf_moveto( pdf, xi, yi );
							cpdf_lineto( pdf, xf, yf );
						}
						cpdf_stroke( pdf );
						cpdf_closepath( pdf );	
					}
					if( el->half_tone )
					{
						float c1 = m_pdf_rgb[layer][0];
						float c2 = m_pdf_rgb[layer][1];
						float c3 = m_pdf_rgb[layer][2];
						cpdf_setrgbcolorStroke(pdf, clr_pdf*c1, clr_pdf*c2, clr_pdf*c3 );
					}
				}
				if( el->gtype == DL_CIRC || el->gtype == DL_HOLLOW_CIRC )
				{
					RECT el_r;
					m_dlist->Get_Rect( el, &el_r );
					float r = (el_r.right - el_r.left)/2.0/(float)m_pcbu_per_wu;
					double x = (el_r.right + el_r.left)/2.0/(float)m_pcbu_per_wu - cx_dsp;
					double y = (el_r.top + el_r.bottom)/2.0/(float)m_pcbu_per_wu - cy_dsp;
					x /= k1;
					y /= k1;
					r /= k1;
					if( mode == LANDSCAPE )
					{
						x = -x;
						x += cy_pdf;
						y += cx_pdf;
					}
					else
					{
						x += cx_pdf;
						y += cy_pdf;
					}	
					//cpdf_newpath( pdf );
					if( mode == LANDSCAPE )
						cpdf_circle( pdf, y, x, r );
					else
						cpdf_circle( pdf, x, y, r );
					if( el->gtype == DL_CIRC )
						cpdf_fill( pdf );
					else
						cpdf_stroke( pdf );
					//cpdf_closepath( pdf );	
				}
				else if( el->gtype == DL_ARC_CW || el->gtype == DL_ARC_CCW )
				{
					const double MagicBezier = 0.2761423749154;
					int sz = PTS->GetSize();			
					for( int ii=0; ii+1<sz; ii+=2 )
					{
						double xi = PTS->GetAt(ii).x - cx_dsp;
						double xf = PTS->GetAt(ii+1).x - cx_dsp;
						double yi = PTS->GetAt(ii).y - cy_dsp;
						double yf = PTS->GetAt(ii+1).y - cy_dsp;
						if( PTS->GetAt(ii).x == PTS->GetAt(ii+1).x && PTS->GetAt(ii).y == PTS->GetAt(ii+1).y )
							continue;
						xi /= k1;
						xf /= k1;
						yi /= k1;
						yf /= k1;
						if( mode == LANDSCAPE )
						{
							xi = -xi;
							xf = -xf;
							xi += cy_pdf;
							xf += cy_pdf;
							yi += cx_pdf;
							yf += cx_pdf;
						}
						else
						{
							xi += cx_pdf;
							xf += cx_pdf;
							yi += cy_pdf;
							yf += cy_pdf;
						}	
						// ellipse quadrant, clockwise, start vertical
						double x1 = xi;
						double y1 = yi + (yf - yi)*2.0*MagicBezier;
						double x2 = xf - (xf - xi)*2.0*MagicBezier;
						double y2 = yf;
						if	(
							(((xf>xi && yf<yi) || (xf<xi && yf>yi) ) && el->gtype-mode == DL_ARC_CW) ||
							(((xf>xi && yf>yi) || (xf<xi && yf<yi) ) && el->gtype+mode == DL_ARC_CCW)
							)
						{
							// ellipse quadrant, clockwise, start horizontal
							x1 = xi + (xf - xi)*2.0*MagicBezier;
							y1 = yi;
							x2 = xf;
							y2 = yf - (yf - yi)*2.0*MagicBezier;
						}
						//cpdf_newpath( pdf );
						if( mode == LANDSCAPE )
						{	
							cpdf_moveto( pdf, yi, xi );
							cpdf_curveto( pdf, y1, x1, y2, x2, yf, xf );
						}
						else
						{
							cpdf_moveto( pdf, xi, yi );
							cpdf_curveto( pdf, x1, y1, x2, y2, xf, yf );
						}
						cpdf_stroke( pdf );
						//cpdf_closepath( pdf );	
					}
				}
			}
		}

		// draw texts
		for( int order=(MAX_LAYERS-1); order>=0; order-- )
		{
			int layer = m_dlist->GetLayer(order);
			if( !m_pdf_vis[layer] || layer == LAY_SELECTION )
				continue;
			cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[layer][0], 
										clr_pdf*(float)m_pdf_rgb[layer][1], 
										clr_pdf*(float)m_pdf_rgb[layer][2] );
			cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_pdf_rgb[layer][0], 
										clr_pdf*(float)m_pdf_rgb[layer][1], 
										clr_pdf*(float)m_pdf_rgb[layer][2] );
			for( dl_element * el=(layer==LAY_HILITE?m_dlist->Get_Start(layer)->next_HLT:m_dlist->Get_Start(layer)->next); 
				( el->next && el != m_dlist->Get_End(LAY_HILITE) ); 
				el=(layer==LAY_HILITE?el->next_HLT:el->next) )
			{
				if( el->visible == 0 )
					continue;
				if( el == m_dlist->Get_End(layer) )
					continue;
				if( el->id.type == ID_DRC )
					continue;
				BOOL USE_FONT = 0;
				if( el->id.type == ID_TEXT )
				{
					CText * ss = (CText*)el->ptr;
					if( ss )
					{
						if( getbit( ss->m_pdf_options, PDF_USE_FONT ) )
							USE_FONT = TRUE;
						else if( m_pdf_use_font )
						{
							int L = ss->m_str.GetLength();
							int io = ss->m_str.Find("|");
							if( io > 0 )
								L = io;
							if( getbit( ss->m_pdf_options, PDF_GRAPHIC ) )
							{
							}
							else
								USE_FONT = TRUE;
						}


						CString str="";
						int iofl = ss->m_str.Find(CP_LINK);
						if( iofl > 0 )
						{
							str = ss->m_str.Right( ss->m_str.GetLength() - iofl - 6 );
							iofl = str.Find(CP_PAGE_NUM);
							if( iofl > 0 )
								str = str.Left( iofl );
							str.Replace("'"," ");
							str = str.Trim();
							if( str.Find("http") == -1 &&
								str.Find("www") == -1 )
								str = "";
						}
						else if( ss->m_tl == Attr->m_Valuelist && ss->m_polyline_start >= 0 )
						{
							if( m_outline_poly->GetSize() <= ss->m_polyline_start )
								return; //ASSERT(0);
							CPolyLine * tpo = &m_outline_poly->GetAt( ss->m_polyline_start );
							if( tpo )
							{
								if( CText * pac = tpo->Check( index_foot_attr ) )
								{
									CString Vv = ss->m_str;
									CString Pp = pac->m_str;
									ExtractComponentName( &Vv, &Pp );
									CString fNote = Comp + "\\" + Vv + "@" + Pp;
									struct _stat buf;
									if( _stat( (fNote+".pdf"), &buf ) == 0 )
										str = "./Component/" + Vv + "@" + Pp + ".pdf"; // file://
									else if( _stat( (fNote+".docx"), &buf ) == 0 )
										str = "./Component/" + Vv + "@" + Pp + ".docx";
									else if( _stat( (fNote+".doc"), &buf ) == 0 )
										str = "./Component/" + Vv + "@" + Pp + ".doc";
									else if( _stat( (fNote+".txt"), &buf ) == 0 )
										str = "./Component/" + Vv + "@" + Pp + ".txt";
								}
							}
						}
						
						if( str.GetLength() )
						{
							double x2, y2, x3, y3;
							RECT tr;
							ss->m_tl->GetTextRectOnPCB( ss, &tr );
							SwellRect( &tr, ss->m_stroke_width );
							x2 = (float)tr.left/(float)m_pcbu_per_wu - cx_dsp;
							y2 = (float)tr.bottom/(float)m_pcbu_per_wu - cy_dsp;
							x3 = (float)tr.right/(float)m_pcbu_per_wu - cx_dsp;
							y3 = (float)tr.top/(float)m_pcbu_per_wu - cy_dsp;
							x2 /= k1;
							y2 /= k1;
							x3 /= k1;
							y3 /= k1;
							if( mode == LANDSCAPE )
							{
								x2 = -x2;
								x2 += cy_pdf;
								y2 += cx_pdf;
								x3 = -x3;
								x3 += cy_pdf;
								y3 += cx_pdf;
							}
							else
							{
								x2 += cx_pdf;
								y2 += cy_pdf;
								x3 += cx_pdf;
								y3 += cy_pdf;
							}
							cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[LAY_HILITE][0], 
														clr_pdf*(float)m_pdf_rgb[LAY_HILITE][1], 
														clr_pdf*(float)m_pdf_rgb[LAY_HILITE][2] );
							if( mode == LANDSCAPE )
							{
								cpdf_setActionURL(pdf, min(y2,y3), min(x2,x3), max(y3,y2), max(x3,x2), str.GetBuffer(), NULL );
								cpdf_rect(pdf, min(y2,y3), min(x2,x3), abs(y3-y2), abs(x3-x2));
								cpdf_fill(pdf);
							}
							else
							{
								cpdf_setActionURL(pdf, min(x2,x3), min(y2,y3), max(x3,x2), max(y3,y2), str.GetBuffer(), NULL );
								cpdf_rect(pdf, min(x2,x3), min(y2,y3), abs(x3-x2), abs(y3-y2));
								cpdf_fill(pdf);
							}
							cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[layer][0], 
														clr_pdf*(float)m_pdf_rgb[layer][1], 
														clr_pdf*(float)m_pdf_rgb[layer][2] );
						}
					}
					float w = (float)el->el_w*(70.0)/k1;
					cpdf_setlinewidth( pdf, w );
					CArray<CPoint> * PTS = m_dlist->Get_Points( el, NULL, 0 );
					if( el->gtype == DL_LINES_ARRAY && USE_FONT == 0 )
					{
						if( el->half_tone )
						{
							float proc = (100.0 - (float)el->half_tone)/100.0;
							int c1 = (float)m_pdf_rgb[layer][0] + (float)(255.0-m_pdf_rgb[layer][0])*proc;
							int c2 = (float)m_pdf_rgb[layer][1] + (float)(255.0-m_pdf_rgb[layer][1])*proc;
							int c3 = (float)m_pdf_rgb[layer][2] + (float)(255.0-m_pdf_rgb[layer][2])*proc;
							cpdf_setrgbcolorStroke(pdf, clr_pdf*c1, clr_pdf*c2, clr_pdf*c3 );
						}
						int sz = PTS->GetSize();			
						for( int ii=0; ii+1<sz; ii+=2 )
						{
							double xi = PTS->GetAt(ii).x - cx_dsp;
							double xf = PTS->GetAt(ii+1).x - cx_dsp;
							double yi = PTS->GetAt(ii).y - cy_dsp;
							double yf = PTS->GetAt(ii+1).y - cy_dsp;
							if( PTS->GetAt(ii).x == PTS->GetAt(ii+1).x && PTS->GetAt(ii).y == PTS->GetAt(ii+1).y )
								continue;
							xi /= k1;
							xf /= k1;
							yi /= k1;
							yf /= k1;
							if( mode == LANDSCAPE )
							{
								xi = -xi;
								xf = -xf;
								xi += cy_pdf;
								xf += cy_pdf;
								yi += cx_pdf;
								yf += cx_pdf;
							}
							else
							{
								xi += cx_pdf;
								xf += cx_pdf;
								yi += cy_pdf;
								yf += cy_pdf;
							}	
							cpdf_newpath( pdf );
							if( mode == LANDSCAPE )
							{	
								cpdf_moveto( pdf, yi, xi );
								cpdf_lineto( pdf, yf, xf );
							}
							else
							{
								cpdf_moveto( pdf, xi, yi );
								cpdf_lineto( pdf, xf, yf );
							}
							cpdf_stroke( pdf );
							cpdf_closepath( pdf );	
						}
						if( el->half_tone )
						{
							float c1 = m_pdf_rgb[layer][0];
							float c2 = m_pdf_rgb[layer][1];
							float c3 = m_pdf_rgb[layer][2];
							cpdf_setrgbcolorStroke(pdf, clr_pdf*c1, clr_pdf*c2, clr_pdf*c3 );
						}
					}
				}
			}
			if( layer >= LAY_ADD_1 )
				continue;
			// draw texts
			{
				CTextList * tl = GetTextListbyLayer( layer );
				if( tl )
				{
					int it = -1;
					for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
					{
						if( t->m_font_size < m_min_text_height )
							continue;
						BOOL USE_FONT = 0;
						if( getbit( t->m_pdf_options, PDF_USE_FONT ) )
							USE_FONT = TRUE;
						else if( m_pdf_use_font )
						{
							int L = t->m_str.GetLength();
							int io = t->m_str.Find("|");
							if( io > 0 )
								L = io;
							if( getbit( t->m_pdf_options, PDF_GRAPHIC ) )
							{
							}
							else
								USE_FONT = TRUE;
						}

						// rendering mode
						if( USE_FONT == 0 )
							cpdf_setTextRenderingMode(pdf, TEXT_INVISIBLE);
						else
							cpdf_setTextRenderingMode(pdf, TEXT_FILL);

						// text color
						cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_pdf_rgb[t->m_layer][0], 
													clr_pdf*(float)m_pdf_rgb[t->m_layer][1], 
													clr_pdf*(float)m_pdf_rgb[t->m_layer][2] );
						cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_pdf_rgb[t->m_layer][0], 
													clr_pdf*(float)m_pdf_rgb[t->m_layer][1], 
													clr_pdf*(float)m_pdf_rgb[t->m_layer][2] );

						CString str = t->m_str;
						int ip = str.Find("|");
						if( ip > 0 )
							str = t->m_str.Left(ip);
						//
						double x, y;
						if( getbit( t->m_pdf_options, PDF_ALIGN ) )
						{
							RECT tr;
							tl->GetTextRectOnPCB( t, &tr );
							SwellRect( &tr, -t->m_stroke_width );
							x = (float)tr.left + (float)tr.right - (float)t->m_x;
							y = (float)tr.top + (float)tr.bottom - (float)t->m_y;
							x = x/(float)m_pcbu_per_wu - cx_dsp;
							y = y/(float)m_pcbu_per_wu - cy_dsp;
						}
						else
						{
							x = (float)t->m_x/(float)m_pcbu_per_wu - cx_dsp;
							y = (float)t->m_y/(float)m_pcbu_per_wu - cy_dsp;
						}
						x /= k1;
						y /= k1;
						if( mode == LANDSCAPE )
						{
							x = -x;
							x += cy_pdf;
							y += cx_pdf;
						}
						else
						{
							x += cx_pdf;
							y += cy_pdf;
						}

						float TextHeightPts = (float)t->m_font_size/(float)m_pcbu_per_wu/k1*100.0;
						cpdf_setFont(pdf, cpdf_fontnamelist[m_pdf_font_i], "WinAnsiEncoding", TextHeightPts);
						
						cpdf_beginText(pdf, 0);
						if( mode == LANDSCAPE )
						{
							if( getbit( t->m_pdf_options, PDF_ALIGN ) )
								cpdf_textAligned(pdf, y, x, 270.0-t->m_angle, TEXTPOS_UR, str.GetBuffer() );
							else
								cpdf_text(pdf, y, x, 270.0-t->m_angle, str.GetBuffer() );
						}
						else
						{
							if( getbit( t->m_pdf_options, PDF_ALIGN ) )
								cpdf_textAligned(pdf, x, y, 360.0-t->m_angle, TEXTPOS_UR, str.GetBuffer() );
							else
								cpdf_text(pdf, x, y, 360.0-t->m_angle, str.GetBuffer() );
						}
						cpdf_endText(pdf);
					}
				}
			}
		}
		m_view->CancelSelection();
		if( m_print_separate )
		{
			CPolyLine * p = &m_outline_poly->GetAt(ipoly.i);
			if( CText * cmd = p->Check( index_desc_attr ) )
			{
				CString pdf_file;
				int i1 = cmd->m_str.Find("|name:");
				int i2 = cmd->m_str.Find( COMMAND );
				if( i1>0 && i2>0 )
				{
					BOOL bFinalize = TRUE;
					pdf_file = cmd->m_str.Mid( i1, i2-i1-1 );
					pdf_file = pdf_file.Right( pdf_file.GetLength()-6 );
					pdf_file = pdf_file.Trim();
					::ExtractComponentName( &pdf_file, NULL );
					pdf_file = m_path_to_folder+"\\related_files\\" + pdf_file;
					id next_pg = Pages.FindPrintableArea(&ipoly);
					if (next_pg.i >= 0)
					{
						CPolyLine* p2 = &m_outline_poly->GetAt(next_pg.i);
						if (CText* cmd2 = p2->Check(index_desc_attr))
						{
							CString next_pdf_file;;
							int ni1 = cmd2->m_str.Find("|name:");
							int ni2 = cmd2->m_str.Find(COMMAND);
							if (ni1 > 0 && ni2 > 0)
							{
								next_pdf_file = cmd2->m_str.Mid(ni1, ni2 - ni1 - 1);
								next_pdf_file = next_pdf_file.Right(next_pdf_file.GetLength() - 6);
								next_pdf_file = next_pdf_file.Trim();
								::ExtractComponentName(&next_pdf_file, NULL);
								next_pdf_file = m_path_to_folder + "\\related_files\\" + next_pdf_file;
								if (next_pdf_file.CompareNoCase(pdf_file) == 0)
									bFinalize = 0;
							}
						}
					}
					if (bFinalize)
					{
						cpdf_finalizeAll(pdf);
						cpdf_savePDFmemoryStreamToFile(pdf, pdf_file);
						cpdf_close(pdf);
						pdf = cpdf_open(0, NULL);
						cpdf_init(pdf);
						cpdf_setTitle(pdf, m_name.GetBuffer());
						pagenum = 1;
					}
					else
						cpdf_finalizePage(pdf, pagenum - 1);
				}
			}
		}
	}
	cpdf_finalizeAll(pdf);
	CString name;
	Pages.GetPageName( mem_n_page, &name ); 
	::ExtractComponentName( &name, NULL );
	CString pdf_file = m_path_to_folder+"\\related_files\\" + name + ".pdf";
	if( m_print_separate )
		cpdf_close(pdf);
	else
	{
		int err = cpdf_savePDFmemoryStreamToFile(pdf, pdf_file );
		if( err == -1 )
		{
			if( !m_dlg_log )
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Error: Unable to write file\nIt may be read-only or open in another application":
					"Ошибка: Невозможно записать файл. Возможно, он доступен только для чтения или открыт в другом приложении", MB_OK);
			}
			else
			{
				CString log_message = "*** Error: unable to write file ***\r\n";
				m_dlg_log->AddLine( log_message );
			}
		}
		cpdf_close(pdf);
		ShellExecute(	NULL,"open", pdf_file, NULL, m_path_to_folder+"\\related_files", SW_SHOWNORMAL );
		//ShellExecute(	NULL,"open", m_path_to_folder, NULL, m_path_to_folder, SW_SHOWNORMAL );
	}
	if( mem_n_page != Pages.GetActiveNumber() )
		SwitchToPage(mem_n_page,TRUE);
}

void CFreePcbDoc::AddPage()
{
	CDlgAddMerge dlg;
	dlg.Doc = this;
	dlg.m_mode = dlg.M_ADDPAGE;
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
			"Do you want to insert a new page in place of the current page?  Indexes of subsequent pages will be shifted by one.":
			"Хотите вставить новую страницу вместо текущей? (В этом случае номера последующих страниц будут смещены на единицу)", MB_ICONQUESTION | MB_YESNO);
		AddNewPage( &dlg.m_merge_name, ret==IDYES?1:0 );
	}
}
//
void CFreePcbDoc::AddNewPage( CString * Name, BOOL insert )
{
	if( Pages.GetNumPages() >= MAX_PAGES )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Maximum number of pages is 8":"Максимальное количество страниц - 8", MB_ICONWARNING);
		return;
	}
	int gac = Pages.GetActiveNumber();
	CWnd* pMain = AfxGetMainWnd();
	CMenu* pMenu = &theApp.m_main;
	CMenu* sMenu = pMenu->GetSubMenu(2);// view
	CMenu* ssMenu = sMenu->GetSubMenu(6);// delete page
	if( !ssMenu )
	{
		AfxMessageBox("Error 239" );
		return;
	}
	int c;
	if( insert )
		c = gac;
	else
		c = ssMenu->GetMenuItemCount();
	ssMenu->InsertMenu(c, MF_BYPOSITION, ID_DPAGE_1+c, *Name );
	for( int m=c+1; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_DPAGE_1+m, str );
	}
	ssMenu = sMenu->GetSubMenu(7);// rename page
	if( !ssMenu )
	{
		AfxMessageBox("Error 241" );
		return;
	}
	if( insert )
		c = gac;
	else
		c = ssMenu->GetMenuItemCount();
	ssMenu->InsertMenu(c, MF_BYPOSITION, ID_RNPAGE_1+c, *Name );
	for( int m=c+1; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_RNPAGE_1+m, str );
	}
	ssMenu = sMenu->GetSubMenu(8);// switch to page
	if( !ssMenu )
	{
		AfxMessageBox("Error 244" );
		return;
	}
	if( insert )
		c = gac;
	else
		c = ssMenu->GetMenuItemCount();
	ssMenu->InsertMenu(c, MF_BYPOSITION, ID_PAGE_1+c, *Name );
	for( int m=c+1; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_PAGE_1+m, str );
	}
	pMain->SetMenu(&theApp.m_main);
	Pages.AddNew( Name, insert );



	// CORRECT LINKS
	int it=-1; int ip = -1;
	if( insert ) 
	{
		m_view->CancelSelection();// !!!
		for( CText * gT=Pages.GetNextAttr( index_desc_attr, &it, &ip ); 
					 gT; 
					 gT=Pages.GetNextAttr( index_desc_attr, &it, &ip ) )
		{
			int iofs = gT->m_str.Find( CP_PAGE_NUM );
			int iofpg = iofs;
			if( iofs > 0 )
			{
				CString pgstr = gT->m_str.Right( gT->m_str.GetLength() - iofs );
				iofs = pgstr.Find( ":" );
				if( iofs > 0 )
				{
					pgstr = pgstr.Right( pgstr.GetLength() - iofs - 1 );
					iofs = my_atoi( &pgstr );
					if ( iofs > gac )
					{
						CString endstr = "";
						int iofpg2 = pgstr.Find( "'" );
						if( iofpg2 > 0 )
							endstr = pgstr.Right( pgstr.GetLength() - iofpg2 );
						iofs++;
						pgstr.Format( CP_PAGE_NUM" %d%s", iofs, endstr );
						gT->m_str.Truncate( iofpg );
						gT->m_str += pgstr;
						gT->MakeVisible();
					}
				}
			}
		}
		// CORRECT FAV NET LINKS
		for( int i=0; i<MAX_FAVORITES; i++ )
			if( m_view->m_favorite_page[i] >= gac )
				m_view->m_favorite_page[i]++;
	
		// CORRECT NL PAGE MASK
		for( int i=0; i<Pages.m_pcb_names.GetSize(); i++ )
		{
			for( int ipage=MAX_PAGES-2; ipage>=gac; ipage-- )
				if( getbit( Pages.m_netlist_page_mask[i], ipage ) )
					setbit( Pages.m_netlist_page_mask[i], ipage+1 );
				else
					clrbit( Pages.m_netlist_page_mask[i], ipage+1 );
			clrbit( Pages.m_netlist_page_mask[i], gac );
		}
	
		// CORRECT PAGE MIRROR MASK
		for( int ipage=MAX_PAGES-2; ipage>=gac; ipage-- )
		{
			if( getbit( Pages.m_page_mirror_mask, ipage ) )
				setbit( Pages.m_page_mirror_mask, ipage+1 );
			else
				clrbit( Pages.m_page_mirror_mask, ipage+1 );
			if( getbit( Pages.m_page_mirror_mask, ipage+MAX_PAGES ) )
				setbit( Pages.m_page_mirror_mask, ipage+1+MAX_PAGES );
			else
				clrbit( Pages.m_page_mirror_mask, ipage+1+MAX_PAGES );
		}
		clrbit( Pages.m_page_mirror_mask, gac );
		clrbit( Pages.m_page_mirror_mask, gac+MAX_PAGES );
		//
		SwitchToPage( gac, TRUE );
	}
	else
		SwitchToPage( Pages.GetNumPages() - 1, TRUE );
		
	if( m_view->m_dlg_compare )
		m_view->m_dlg_compare->ReloadPages();
	m_view->OnRangeCmds( NULL );
	m_view->m_left_pane_invalid = -1;
}
//
int CFreePcbDoc::SwitchToPage( int number, BOOL Invalidate, BOOL UpdPartlist )
{
	if( m_view->m_sel_count )
	{
		if( m_view->m_cursor_mode == CUR_NONE_SELECTED )
			m_view->m_cursor_mode = CUR_GROUP_SELECTED;
		else if( m_view->m_cursor_mode == CUR_DRE_SELECTED )
			m_view->m_cursor_mode = CUR_NONE_SELECTED; // because m_sel_dre did not save !
	}
	else
		m_view->m_cursor_mode = CUR_NONE_SELECTED;
	int old_page = Pages.GetActiveNumber();
	if( Pages.SwitchToPage( number ) )
	{
		int ipcb = Pages.GetPcbIndex( number );
		if( ipcb >= 0 )
		{
			m_view->m_protect = getbit( m_protection, ipcb );
			int rf = m_pcb_full_path.ReverseFind( '\\' );
			m_netlist_full_path = m_pcb_full_path.Left(rf) + freeasy_netlist + Pages.GetCurrentPCBName();
		}
		else
			m_view->m_protect = 0;
		m_view->ShowActiveLayer(m_num_additional_layers);
		//------------------------------------------
		m_dlist->SetVisibleGrid( TRUE, m_visual_grid_spacing/m_view->m_user_scale );
		//------------------------------------------
		for(int i=0; i<m_num_layers; i++ )
		{
			m_dlist->SetLayerRGB( i, m_rgb[i][0], m_rgb[i][1], m_rgb[i][2] );
			m_dlist->SetLayerVisible( i, m_vis[i] );
		}
		//-------------------------------------------------------------------
		int gMap = m_dlist->GetMapping( &m_view->m_client_r, &m_view->m_left_pane_w, &m_view->m_bottom_pane_h,
			&m_view->m_pcbu_per_pixel, &m_view->m_org_x, &m_view->m_org_y );
		if( gMap )
		{
			CRect screen_r;
			m_view->GetWindowRect( &screen_r );
			m_view->GetClientRect( &m_view->m_client_r );
			m_dlist->SetMapping( &m_view->m_client_r, &screen_r, m_view->m_left_pane_w, m_view->m_bottom_pane_h,
								 m_view->m_pcbu_per_pixel, m_view->m_org_x, m_view->m_org_y );
		}
		else 
			m_view->ViewAllElements(NULL);

		if( Invalidate )
		{
			if( UpdPartlist )
				if( m_view->m_dlg_partlist )
					if( m_view->m_dlg_partlist->IsWindowVisible() )
						if( m_view->m_dlg_partlist->GetPCBButtonCheck() )
							m_view->m_dlg_partlist->OnChange();
			m_view->m_sel_count = Pages.GetSelCount(number, &m_view->m_sel_flags );
			if( m_view->m_sel_count )
			{
				if( m_view->m_cursor_mode == CUR_NONE_SELECTED )
					m_view->m_cursor_mode = CUR_GROUP_SELECTED;
				m_view->HighlightGroup();
			}
			else 
			{
				int it = -1;
				for (CText* t = Attr->m_pDesclist->GetNextText(&it); t; t = Attr->m_pDesclist->GetNextText(&it))
				{
					if (t->m_str.Right(7) == "PCBVIEW")
					{
						OnPolylineUpdatePcbView(this, t->m_polyline_start, &t->m_str, TRUE);
						m_view->CancelSelection(0);
					}
				}
			}
			int curm = m_view->m_cursor_mode;
			m_view->m_cursor_mode = INT_MAX;
			m_view->SetCursorMode( curm );
			m_view->m_draw_layer = 0;
			m_view->Invalidate( FALSE );
			m_view->InvalidateLeftPane();
			ProjectModified(); // upd menu
		}
	}
	return old_page;
}
void CFreePcbDoc::RenamePage( int number, CString * NewName )
{
	CString newCaption;
	CDlgAddMerge dlg;
	dlg.Doc = this;
	dlg.m_mode = dlg.M_RENAMEPAGE;
	int ret;
	if( NewName )
	{
		dlg.m_merge_name = *NewName;
		ret = IDOK;
	}
	else
		ret = dlg.DoModal();
	if( ret == IDOK )
	{
		newCaption = dlg.m_merge_name;
		CMenu* pMenu = &theApp.m_main;
		CMenu* sMenu = pMenu->GetSubMenu(2);// view
		//
		CMenu* ssMenu = sMenu->GetSubMenu(6);// delete page
		if( !ssMenu )
		{
			AfxMessageBox("Error 255" );
			return;
		}
		if( number >= 0 && number < ssMenu->GetMenuItemCount() )
			ssMenu->ModifyMenu(number, MF_BYPOSITION, ID_DPAGE_1+number, newCaption );
		//
		ssMenu = sMenu->GetSubMenu(7);// rename page
		if( !ssMenu )
		{
			AfxMessageBox("Error 257" );
			return;
		}
		if( number >= 0 && number < ssMenu->GetMenuItemCount() )
			ssMenu->ModifyMenu(number, MF_BYPOSITION, ID_RNPAGE_1+number, newCaption );
		//
		ssMenu = sMenu->GetSubMenu(8);// switch to page
		if( !ssMenu )
		{
			AfxMessageBox("Error 266" );
			return;
		}
		if( number >= 0 && number < ssMenu->GetMenuItemCount() )
			ssMenu->ModifyMenu(number, MF_BYPOSITION, ID_PAGE_1+number, newCaption );
		//
		Pages.Rename( number, &newCaption );
		if( m_view->m_dlg_compare )
			m_view->m_dlg_compare->ReloadPages();
		//
		CWnd* pMain = AfxGetMainWnd();
		pMain->SetMenu(&theApp.m_main);
		m_view->OnRangeCmds( NULL );
		m_view->m_left_pane_invalid = -1;	
	}
}
void CFreePcbDoc::DeletePage( int number )
{
	if( Pages.GetNumPages() <= 1 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Unable to delete first page":"Невозможно удалить первую страницу", MB_ICONWARNING);
		return;
	}
	CTextList * ptl = Pages.GetTextList( number, index_part_attr );
	int it = -1;
	if( ptl )
		if( ptl->GetNextText(&it) )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "Page contains parts, cannot be deleted.":"Страница содержит детали поэтому не может быть удалена", MB_ICONWARNING);
			return;
		}
	CWnd* pMain = AfxGetMainWnd();
	CMenu* pMenu = &theApp.m_main;
	CMenu* sMenu = pMenu->GetSubMenu(2);// view
	//
	CMenu* ssMenu = sMenu->GetSubMenu(6);// delete page
	if( !ssMenu )
	{
		AfxMessageBox("Error 246" );
		return;
	}
	if( number >= 0 && number < ssMenu->GetMenuItemCount() )
		ssMenu->RemoveMenu(number, MF_BYPOSITION );
	for( int m=number; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_DPAGE_1+m, str );
	}
	//
	ssMenu = sMenu->GetSubMenu(7);// rename page
	if( !ssMenu )
	{
		AfxMessageBox("Error 247" );
		return;
	}
	if( number >= 0 && number < ssMenu->GetMenuItemCount() )
		ssMenu->RemoveMenu(number, MF_BYPOSITION );
	for( int m=number; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_RNPAGE_1+m, str );
	}
	//
	ssMenu = sMenu->GetSubMenu(8);// switch to page
	if( !ssMenu )
	{
		AfxMessageBox("Error 249" );
		return;
	}
	if( number >= 0 && number < ssMenu->GetMenuItemCount() )
		ssMenu->RemoveMenu(number, MF_BYPOSITION );
	for( int m=number; m<ssMenu->GetMenuItemCount(); m++ )
	{
		CString str;
		ssMenu->GetMenuStringA(m,str,MF_BYPOSITION);
		ssMenu->ModifyMenu(m, MF_BYPOSITION, ID_PAGE_1+m, str );
	}
	//
	pMain->SetMenu(&theApp.m_main);
	//
	// CORRECT LINKS
	it=-1; 
	int ip = -1;
	for( CText * gT=Pages.GetNextAttr( index_desc_attr, &it, &ip ); 
				 gT; 
				 gT=Pages.GetNextAttr( index_desc_attr, &it, &ip ) )
	{
		BOOL bFoundLinks = 0;
		int iofs = gT->m_str.Find( CP_PAGE_NUM );
		int iofpg = iofs;
		if( iofs > 0 )
		{
			CString pgstr = gT->m_str.Right( gT->m_str.GetLength() - iofs );
			iofs = pgstr.Find( ":" );
			if( iofs > 0 )
			{
				pgstr = pgstr.Right( pgstr.GetLength() - iofs - 1 );
				iofs = my_atoi( &pgstr );
				if ( iofs == (number+1) )
					bFoundLinks = 1;
				else if ( iofs > number )
				{
					CString endstr = "";
					int iofpg2 = pgstr.Find( "'" );
					if( iofpg2 > 0 )
						endstr = pgstr.Right( pgstr.GetLength() - iofpg2 );
					iofs--;
					pgstr.Format( CP_PAGE_NUM" %d%s", iofs, endstr );
					gT->m_str.Truncate( iofpg );
					gT->m_str += pgstr;
					gT->MakeVisible();
				}
			}
		}
		if( bFoundLinks )
		{
			//CString np;
			//np.Format("Page %d contains links that will no longer work because they link to the page being removed", (ip+1));
			//AfxMessageBox( np, MB_ICONWARNING );
		}
	}
	//
	// REMOVE FAVORITE LINKS
	for( int i=0; i<MAX_FAVORITES; i++ )
		if( m_view->m_favorite_page[i] > number )
			m_view->m_favorite_page[i]--;
		else if( m_view->m_favorite_page[i] == number )
			m_view->m_favorite_poly[i] = -1;
	//
	// CORRECT NL PAGE MASK
	for( int i=0; i<Pages.m_pcb_names.GetSize(); i++ )
	{
		for( int ipage=number; ipage<MAX_PAGES-1; ipage++ )
			if( getbit( Pages.m_netlist_page_mask[i], ipage+1 ) )
				setbit( Pages.m_netlist_page_mask[i], ipage );
			else
				clrbit( Pages.m_netlist_page_mask[i], ipage );
		clrbit( Pages.m_netlist_page_mask[i], MAX_PAGES-1 );
	}
	//
	// CORRECT PAGE MIRROR MASK
	for( int ipage=number; ipage<MAX_PAGES-1; ipage++ )
	{
		if( getbit( Pages.m_page_mirror_mask, ipage+1 ) )
			setbit( Pages.m_page_mirror_mask, ipage );
		else
			clrbit( Pages.m_page_mirror_mask, ipage );
		if( getbit( Pages.m_page_mirror_mask, ipage+1+MAX_PAGES ) )
			setbit( Pages.m_page_mirror_mask, ipage+MAX_PAGES );
		else
			clrbit( Pages.m_page_mirror_mask, ipage+MAX_PAGES );
	}
	clrbit( Pages.m_page_mirror_mask, MAX_PAGES-1 );
	clrbit( Pages.m_page_mirror_mask, MAX_PAGES-1+MAX_PAGES );
	//
	//
	Pages.Clear( number );
	Pages.Remove( number );
	SwitchToPage( Pages.GetActiveNumber() );
	//
	//
	if( m_view->m_dlg_compare )
	{
		if( Pages.GetNumPages() < 2 )
		{
			Pages.DeleteVirtualPages();
			if( m_view->m_dlg_compare->DLG_CREATED )
				m_view->m_dlg_compare->DestroyWindow();
			delete m_view->m_dlg_compare;
			m_view->m_dlg_compare = NULL;
		}
		else
			m_view->m_dlg_compare->ReloadPages();
	}
	m_view->Invalidate( FALSE );// (doubtful)
	m_view->m_left_pane_invalid = -1;
}
void CFreePcbDoc::SwitchToPage1()
{
	SwitchToPage(0,TRUE);
}
void CFreePcbDoc::SwitchToPage2()
{
	SwitchToPage(1,TRUE);
}
void CFreePcbDoc::SwitchToPage3()
{
	SwitchToPage(2,TRUE);
}
void CFreePcbDoc::SwitchToPage4()
{
	SwitchToPage(3,TRUE);
}
void CFreePcbDoc::SwitchToPage5()
{
	SwitchToPage(4,TRUE);
}
void CFreePcbDoc::SwitchToPage6()
{
	SwitchToPage(5,TRUE);
}
void CFreePcbDoc::SwitchToPage7()
{
	SwitchToPage(6,TRUE);
}
void CFreePcbDoc::SwitchToPage8()
{
	SwitchToPage(7,TRUE);
}
void CFreePcbDoc::DeletePage1()
{
	DeletePage(0);
}
void CFreePcbDoc::DeletePage2()
{
	DeletePage(1);
}
void CFreePcbDoc::DeletePage3()
{
	DeletePage(2);
}
void CFreePcbDoc::DeletePage4()
{
	DeletePage(3);
}
void CFreePcbDoc::DeletePage5()
{
	DeletePage(4);
}
void CFreePcbDoc::DeletePage6()
{
	DeletePage(5);
}
void CFreePcbDoc::DeletePage7()
{
	DeletePage(6);
}
void CFreePcbDoc::DeletePage8()
{
	DeletePage(7);
}
void CFreePcbDoc::RenamePage1()
{
	RenamePage(0);
}
void CFreePcbDoc::RenamePage2()
{
	RenamePage(1);
}
void CFreePcbDoc::RenamePage3()
{
	RenamePage(2);
}
void CFreePcbDoc::RenamePage4()
{
	RenamePage(3);
}
void CFreePcbDoc::RenamePage5()
{
	RenamePage(4);
}
void CFreePcbDoc::RenamePage6()
{
	RenamePage(5);
}
void CFreePcbDoc::RenamePage7()
{
	RenamePage(6);
}
void CFreePcbDoc::RenamePage8()
{
	RenamePage(7);
}
//
void CFreePcbDoc::OnAttachBMP1()
{
	AttachBMP( 1 );
}
void CFreePcbDoc::OnAttachBMP2()
{
	if( m_view->m_sel_id.type != ID_POLYLINE )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "No selected polylines!" : "Нет выбранных полилиний!", MB_ICONINFORMATION );
		return;
	}
	int src = m_outline_poly->GetAt(m_view->m_sel_id.i).GetPicture();
	if( src >= 0 )
	{
		m_view->SaveUndoInfoForOutlinePoly( m_view->m_sel_id.i, TRUE, m_undo_list );
		int ts=0;
		CString Path="";
		m_dlist->GetTemplate( src, &Path, NULL, &ts );
		m_dlist->SetTemplateStyle( !ts, src );
		//
		CString Path2="";
		for( int it=0; it<m_dlist->GetNumTemplates(); it++ )
		{
			m_dlist->GetTemplate( it, &Path2 );
			if( Path.Compare( Path2 ) == 0 )
				m_dlist->SetTemplateStyle( !ts, it );
		}
		OnRangeCmds(NULL);
	}
}
void CFreePcbDoc::OnAttachGND1()
{
	CString path = m_app_dir + "\\Colors\\GND.bmp";
	AttachBMP(0, &path);
}
void CFreePcbDoc::OnAttachGND2()
{
	CString path = m_app_dir + "\\Colors\\Y.jpg";
	AttachBMP(1, &path);
}
void CFreePcbDoc::OnAttachGND3()
{
	CString path = m_app_dir + "\\Colors\\G.jpg";
	AttachBMP(1, &path);
}
void CFreePcbDoc::OnAttachGND4()
{
	CString path = m_app_dir + "\\Colors\\B.jpg";
	AttachBMP(1, &path);
}
void CFreePcbDoc::OnAttachGND5()
{
	CString path = m_app_dir + "\\Colors\\T.jpg";
	AttachBMP(1, &path);
}




void CFreePcbDoc::OnDetachBMP()
{
	if( m_view->m_sel_id.type != ID_POLYLINE )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "No selected polylines!" : "Нет выбранных полилиний!", MB_ICONINFORMATION );
		return;
	}
	int src = m_outline_poly->GetAt(m_view->m_sel_id.i).GetPicture();
	if( src >= 0 )
	{
		m_outline_poly->GetAt(m_view->m_sel_id.i).SetPicture(-1);
		m_dlist->MoveTemplate( &rect(0,0,0,0), src, 
			m_outline_poly->GetAt(m_view->m_sel_id.i).GetLayer() );
		m_dlist->SetTemplateVis( 0, src );
		OnRangeCmds(NULL);
	}
}
void CFreePcbDoc::AttachBMP( int Style, CString * fn )
{
	if( m_view->m_sel_id.type != ID_POLYLINE )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"First, select the polyline to which you want to attach the picture.":
			"Сначала выберите полилинию, к которой вы хотите прикрепить изображение.", MB_ICONINFORMATION);
		return;
	}
	if( m_outline_poly->GetAt(m_view->m_sel_id.i).GetNumCorners() < 4 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Polyline must have at least 4 vertices.":"Полилиния должна иметь не менее 4 вершин.", MB_ICONINFORMATION);
		return;
	}
	CString filename;
	if( fn == NULL )
		filename = RunFileDialog( 1, "img" );
	else
		filename = *fn;
	if ( filename.GetLength() )
	{
		// clear old img
		int old_i = m_outline_poly->GetAt(m_view->m_sel_id.i).GetPicture();
		int lay = m_outline_poly->GetAt(m_view->m_sel_id.i).GetLayer();
		if( old_i < m_dlist->GetNumTemplates() )
			m_dlist->SetTemplate( NULL, &rect(0,0,0,0), old_i, lay );
		//
		CString ext = filename.Right(3);
		if( ext.CompareNoCase("BMP") == 0 || 
			ext.CompareNoCase("PEG") == 0 || 
			ext.CompareNoCase("JPG") == 0 )
		{
			m_view->SaveUndoInfoForOutlinePoly( m_view->m_sel_id.i, TRUE, m_undo_list );
			m_outline_poly->GetAt(m_view->m_sel_id.i).RecalcRectC(0);
			RECT prect = m_outline_poly->GetAt(m_view->m_sel_id.i).GetCornerBounds();
			//
			// Now generate
			// Template Name
			CString new_name = filename;
			int isrc = AddSource( &new_name );
			//
			int tmp = m_dlist->AddTemplate( isrc, 
											m_outline_poly->GetAt(m_view->m_sel_id.i).GetLayer(), 
											&prect, 
											TRUE );
			m_dlist->SetTemplateStyle( Style, tmp );
			m_outline_poly->GetAt(m_view->m_sel_id.i).SetPicture( tmp );
			m_outline_poly->GetAt(m_view->m_sel_id.i).Show();
		}
	}
	OnRangeCmds(NULL);
}

int CFreePcbDoc::AddSource( CString * new_name )
{
	// find
	// any file
	CFileFind finder;	
	CString SRC = *new_name;
	CString first_f = "\\*";
	int fi = m_pcb_full_path.ReverseFind('\\');
	CString folder = m_pcb_full_path.Left(fi) + rel_f_pictures;
	CString name = *new_name;
	int fn = new_name->ReverseFind('\\');
	if( fn > 0 )
		name = new_name->Right( new_name->GetLength() - fn - 1 );
	
	// match names 
	// return 
	if( new_name->CompareNoCase( folder + name ) == 0 )
	{
		struct _stat buf;
		if( _stat( (*new_name), &buf ) )
		{
			// костыль против дураков которые добавляют расширение к названию
			if( _stat( (*new_name)+".jpg", &buf ) == 0 )
				MoveFile( (*new_name)+".jpg", (*new_name) );
			else if( _stat( (*new_name)+".bmp", &buf ) == 0 )
				MoveFile( (*new_name)+".bmp", (*new_name) );
			else if( _stat( (*new_name)+".jpeg", &buf ) == 0 )
				MoveFile( (*new_name)+".jpeg", (*new_name) );
		}
		if( new_name->Right(9).Left(4) == "\\PIC" )
		{
			int nLen = new_name->GetLength();
			CString CorrName;
			CorrName.Format( "%s%s.PIC", new_name->Left( nLen-8 ), new_name->Right(5) );
			CopyFile( (*new_name), CorrName, TRUE );
			*new_name = CorrName;
		}
		return m_dlist->AddSource( new_name );
	}
	// first check
	// is there a file
	// in cache
	CString search_str = folder + first_f;
	BOOL bWorking = finder.FindFile( search_str );
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString fn = finder.GetFilePath();
		if( !finder.IsDots() && !finder.IsDirectory() )
		{
			if ( SameFiles( &SRC, &fn ) ) // сравнение файлов 
			{
				// same
				*new_name = fn;
				return m_dlist->AddSource(&fn);
			}
		}
	}
	//
	int index = 0;
	do
	{
		index++;
		new_name->Format( "%.5d.PIC", index );
	} while( Pages.CheckTemplate( new_name ) >= 0 );
	
	// make full path
	*new_name = folder + *new_name;
	
	// replace file
	SetFileAttributes(*new_name, FILE_ATTRIBUTE_NORMAL);
	remove( *new_name );
	BOOL OK = CopyFile( SRC, *new_name, 0 );
	if( OK == 0 )
	{
		CString ps;
		ps.Format(G_LANGUAGE == 0 ? 
			"Unable to copy picture.\nsource: %s\ndestination: %s":
			"Невозможно скопировать изображение.\nисточник: %s\nназначение: %s", SRC, *new_name);
		AfxMessageBox( ps,  MB_ICONINFORMATION );
		return -1;
	}
	// return src index
	return m_dlist->AddSource( new_name );
}

void CFreePcbDoc::OnAttrSync()
{
	CDlgAttrSync dlg;
	dlg.Ini( this );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		AttributeSync();
	}
}
int CFreePcbDoc::AttributeSync( CText * REF, CString * old_att, CText * new_att, int i_att )
{
	if( old_att == NULL || new_att == NULL )
		return 0;
	if( Pages.m_page_mirror_mask>>MAX_PAGES == 0 )// no pages - no sync
		return 0;
	if( getbit( Pages.m_page_mirror_mask, Pages.GetActiveNumber() ) == 0 )
		return 0;
	int RET = 0;
	CText * FOUND = NULL;
	for( int pg=Pages.FindPart( &REF->m_str, &FOUND, TRUE, FALSE ); pg>=0; pg=Pages.FindPart( &REF->m_str, &FOUND, FALSE, FALSE ) )
	{
		if( getbit( Pages.m_page_mirror_mask, pg+MAX_PAGES ) == 0 )
			continue;
		CArray<CPolyLine> * po = Pages.GetPolyArray( pg );
		CText * OAtt = NULL;
		int match_cnt = 0;
		for( int get=FOUND->m_polyline_start; get>=0; get=po->GetAt(get).Next[index_part_attr] )
		{
			if( po->GetAt(get).pAttr[i_att] )
				if( old_att->Compare( po->GetAt(get).pAttr[i_att]->m_str ) == 0 )
				{
					OAtt = po->GetAt(get).pAttr[i_att];
					match_cnt++; 
				}
		}
		if( OAtt )
		{
			if( match_cnt == 1 || i_att == index_part_attr || i_att == index_value_attr || i_att == index_foot_attr )
			{
				OAtt->m_str = new_att->m_str;
				OAtt->m_nchars = new_att->m_str.GetLength();
				OAtt->m_font_size = new_att->m_font_size;
				OAtt->m_stroke_width = new_att->m_stroke_width;
				OAtt->Undraw();
				OAtt->MakeVisible();
				RET = 1;
				// set flags
				int ipcb = Pages.GetPcbIndex( pg );
				if( ipcb >= 0 )
					m_pcb_modified[ipcb] = TRUE;
			}
			else
			{
				CString s, pg_name;
				Pages.GetPageName(pg,&pg_name);
				s.Format(G_LANGUAGE == 0 ? 
					"Unable to sync attribute \"%s\". Several identical attributes were found for part %s on page %d (%s)":
					"Невозможно синхронизировать атрибут \"%s\". Для части %s на странице %d (%s) найдено несколько идентичных атрибутов.", *old_att, REF->m_str, pg + 1, pg_name);
				AfxMessageBox( s, MB_ICONERROR );
			}
		}
	}
	return RET;
}
void CFreePcbDoc::AttributeSync()
{
	if( Pages.m_page_mirror_mask>>MAX_PAGES == 0 )// no pages - no sync
		return;
	for( int cur_p=0; cur_p<Pages.GetNumPages(); cur_p++ )
	{
		if( getbit( Pages.m_page_mirror_mask, cur_p ) == 0 )
			continue;
		CTextList * part_list = Pages.GetTextList( cur_p, index_part_attr );
		int it = -1;
		for( CText * part=part_list->GetNextText(&it); part; part=part_list->GetNextText(&it) )
		{
			if( getbit( part->m_pdf_options, SYNC_EXCLUDE ) )
				continue;
			for( int mirr_p=0; mirr_p<Pages.GetNumPages(); mirr_p++ )
			{
				if( getbit( Pages.m_page_mirror_mask, mirr_p+MAX_PAGES ) == 0 )
					continue;
				int ipcb = Pages.GetPcbIndex( mirr_p );
				CTextList * mirr_plist = Pages.GetTextList( mirr_p, index_part_attr );
				int it2 = -1;
				CText * p2 = mirr_plist->GetText( &part->m_str, &it2 );
				if( p2 )
				{
					if( part->m_polyline_start >= 0 && p2->m_polyline_start >= 0 )
					{
						CArray<CPolyLine> * po1 = Pages.GetPolyArray( cur_p );
						CArray<CPolyLine> * po2 = Pages.GetPolyArray( mirr_p );
						// sync ref size
						if( p2->m_font_size != part->m_font_size )
						{
							p2->Undraw();
							p2->m_font_size = part->m_font_size;
							p2->m_stroke_width = part->m_stroke_width;
							p2->MakeVisible();
						}
						CText * V1 = po1->GetAt( part->m_polyline_start ).pAttr[index_value_attr];
						CText * V2 = po2->GetAt( p2->m_polyline_start ).pAttr[index_value_attr];
						CText * F1 = po1->GetAt( part->m_polyline_start ).pAttr[index_foot_attr];
						CText * F2 = po2->GetAt( p2->m_polyline_start ).pAttr[index_foot_attr];
						if( V1 && V2 )
						{
							int C = V1->m_str.Compare(V2->m_str);
							if( C || V2->m_font_size != V1->m_font_size )
							{
								if( ipcb >= 0 && C == 0 )
									m_pcb_modified[ipcb] = TRUE;
								V2->Undraw();
								V2->m_str = V1->m_str;
								V2->m_nchars = V1->m_nchars;
								V2->m_font_size = V1->m_font_size;
								V2->m_stroke_width = V1->m_stroke_width;
								V2->MakeVisible();
							}
						}
						else if( V1 == NULL && V2 )
						{
							if( ipcb >= 0 )
								m_pcb_modified[ipcb] = TRUE;
							for( int get=p2->m_polyline_start; get>=0; get=po2->GetAt(get).Next[index_part_attr] )
								po2->GetAt( get ).pAttr[index_value_attr] = NULL;
							V2->m_selected = 0;
							V2->InVisible();
						}
						else if( V2 == NULL && V1 )
						{
							if( ipcb >= 0 )
								m_pcb_modified[ipcb] = TRUE;
							CTextList * V_tl = Pages.GetTextList( mirr_p, index_value_attr );
							it2 = -1;
							CText * freeText = V_tl->GetNextInvisibleText(&it2);
							if( freeText == NULL )
								freeText = V_tl->AddText(0,0,0,V1->m_layer,0,0,V1->m_font_number, &V1->m_str,FALSE );
							freeText->m_x = V1->m_x + p2->m_x - part->m_x;
							freeText->m_y = V1->m_y + p2->m_y - part->m_y;
							freeText->m_angle = V1->m_angle;
							freeText->m_font_size = V1->m_font_size;
							freeText->m_stroke_width = V1->m_stroke_width;
							freeText->m_str = V1->m_str;
							freeText->m_nchars = V1->m_nchars;
							freeText->MakeVisible();
							freeText->m_polyline_start = p2->m_polyline_start;
							for( int get=p2->m_polyline_start; get>=0; get=po2->GetAt(get).Next[index_part_attr] )
								po2->GetAt( get ).pAttr[index_value_attr] = freeText;
						}
						if( F1 && F2 )
						{
							int C = F1->m_str.Compare(F2->m_str);
							if( C || F2->m_font_size != F1->m_font_size )
							{
								if( ipcb >= 0 && C == 0 )
									m_pcb_modified[ipcb] = TRUE;
								F2->Undraw();
								F2->m_str = F1->m_str;
								F2->m_nchars = F1->m_nchars;
								F2->m_font_size = F1->m_font_size;
								F2->m_stroke_width = F1->m_stroke_width;
								F2->MakeVisible();
							}
						}
						else if( F1 == NULL && F2 )
						{
							if( ipcb >= 0 )
								m_pcb_modified[ipcb] = TRUE;
							for( int get=p2->m_polyline_start; get>=0; get=po2->GetAt(get).Next[index_part_attr] )
								po2->GetAt( get ).pAttr[index_foot_attr] = NULL;
							F2->m_selected = 0;
							F2->InVisible();
						}
						else if( F2 == NULL && F1 )
						{
							if( ipcb >= 0 )
								m_pcb_modified[ipcb] = TRUE;
							CTextList * F_tl = Pages.GetTextList( mirr_p, index_foot_attr );
							it2 = -1;
							CText * freeText = F_tl->GetNextInvisibleText(&it2);
							if( freeText == NULL )
								freeText = F_tl->AddText(0,0,0,F1->m_layer,0,0,F1->m_font_number, &F1->m_str,FALSE );
							freeText->m_x = F1->m_x + p2->m_x - part->m_x;
							freeText->m_y = F1->m_y + p2->m_y - part->m_y;
							freeText->m_angle = F1->m_angle;
							freeText->m_font_size = F1->m_font_size;
							freeText->m_stroke_width = F1->m_stroke_width;
							freeText->m_str = F1->m_str;
							freeText->m_nchars = F1->m_nchars;
							freeText->MakeVisible();
							freeText->m_polyline_start = p2->m_polyline_start;
							for( int get=p2->m_polyline_start; get>=0; get=po2->GetAt(get).Next[index_part_attr] )
								po2->GetAt( get ).pAttr[index_foot_attr] = freeText;
						}
					}
				}
			}
		}
		break;
	}
}

int CFreePcbDoc::GetNumPins( CText * part, CArray<CPolyLine> * arr )
{
	if( arr == NULL )
		arr = m_outline_poly;
	int cnt = 0;
	CPolyLine * po = NULL;
	for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
	{
		po = &arr->GetAt(get);
		if( po->Check( index_pin_attr ) )
			if( po->GetLayer() < LAY_ADD_1 )
				cnt++;
	}
	return cnt;
}

int CFreePcbDoc::GetNumAllPins( CText * part, CArray<CPolyLine> * arr )
{
	if( arr == NULL )
		arr = m_outline_poly;
	int cnt = 0;
	CPolyLine * po = NULL;
	for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
	{
		po = &arr->GetAt(get);
		if( po->Check( index_pin_attr ) )
			cnt++;
	}
	return cnt;
}

void CFreePcbDoc::AddRefList()
{
	RefList(0);
}
void CFreePcbDoc::DelRefList()
{
	RefList(1);
}
int CFreePcbDoc::RefList( BOOL clear )
{
	if( m_view->m_sel_count == 0 && clear == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return -1;
	}
	int cnt = m_ref_lists->GetSize();
	CDlgAddMerge dlg;
	dlg.Doc = this;
	if( clear )
		dlg.m_mode = dlg.M_REM_LIST;
	else
		dlg.m_mode = dlg.M_REF_LIST;
	int ret = dlg.DoModal();
	int new_index = -1;
	if( ret == IDOK )
	{	
		if( clear == 0 )
		{
			m_view->OnSelectPartsByAttr();
			SelectGroupAttributes();
			new_index = AddNewRefList( &dlg.m_merge_name );
		}
		else
			new_index = m_ref_lists->GetIndex( dlg.m_merge_name );
		if( new_index >= 0 )
		{
			int keep_prev_parts = IDNO;
			if( new_index != cnt && clear == 0 )
			{
				keep_prev_parts = AfxMessageBox(G_LANGUAGE == 0 ? 
					"Keep previous parts in this reflist?":
					"Сохранить предыдущие детали в этом списке обозначений?", MB_ICONQUESTION | MB_YESNOCANCEL);
				if( keep_prev_parts == IDCANCEL )
					return -1;
			}
			int it = -1;
			int ip = -1;
			for( CText * t=Pages.GetNextAttr( index_part_attr, &it, &ip ); t; t=Pages.GetNextAttr( index_part_attr, &it, &ip ) )
			{
				if( clear )
				{
					clrbit( t->m_pdf_options, REF_LIST_INDEX + new_index );
					// shift of data
					for( int dt=REF_LIST_INDEX+new_index; dt<30; dt++ )
						if( getbit( t->m_pdf_options, dt+1 ) )
							setbit( t->m_pdf_options, dt );
						else
							clrbit( t->m_pdf_options, dt );
				}
				else if( t->m_selected )
					setbit( t->m_pdf_options, REF_LIST_INDEX + new_index );
				else if( t->m_polyline_start >= 0 )
				{
					CArray<CPolyLine> * po = Pages.GetPolyArray(ip);
					if( po->GetAt( t->m_polyline_start ).GetSideSel() )
						setbit( t->m_pdf_options, REF_LIST_INDEX + new_index );
					else if( keep_prev_parts == IDNO )
						clrbit( t->m_pdf_options, REF_LIST_INDEX + new_index );
				}
				else if( keep_prev_parts == IDNO )
					clrbit( t->m_pdf_options, REF_LIST_INDEX + new_index );
			}
			if( clear )
			{
				OnReflistRemoveItem(new_index+3);
				m_ref_lists->RemoveAt(new_index);
			}
			else if( new_index == cnt )
				AfxMessageBox(G_LANGUAGE == 0 ? "Ref list created!":"Список создан!", MB_ICONINFORMATION);
			else if( keep_prev_parts == IDNO )
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Ref list modified! Previous parts of this reflist have been removed!":
					"Список изменён! Предыдущие детали этого списка были удалены!");
			else
				AfxMessageBox(G_LANGUAGE == 0 ? "Ref list modified, new parts added.":"Список изменен, добавлены новые детали");
		}
	}
	return new_index;
}
//
int CFreePcbDoc::AddNewRefList( CString * Name )
{
	int cnt = m_ref_lists->GetSize();
	if( cnt >= MAX_REF_LISTS )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "Maximum number of lists is 20":"Максимальное количество списков — 20", MB_ICONWARNING);
		return -1;
	}
	//
	int inew = m_ref_lists->AddNew( *Name );
	if( inew == cnt )
	{
		CWnd* pMain = AfxGetMainWnd();
		CMenu* pMenu = &theApp.m_main;
		CMenu* sMenu = pMenu->GetSubMenu(3);// project
		CMenu* ssMenu = sMenu->GetSubMenu(8);// ref lists(key)
		if( !ssMenu )
		{
			AfxMessageBox("Error 235" );
			return -1;
		}
		int c = ssMenu->GetMenuItemCount();
		ssMenu->InsertMenu(c, MF_BYPOSITION, ID_PROJECT_REFLIST+c-2, *Name );
		pMain->SetMenu(&theApp.m_main);
	}
	return inew;
}
//
void CFreePcbDoc::OnReflistRemoveItem( int number )
{
	if( number > 1 )
	{
		CWnd* pMain = AfxGetMainWnd();
		CMenu* pMenu = &theApp.m_main;
		CMenu* sMenu = pMenu->GetSubMenu(3);// project
		CMenu* ssMenu = sMenu->GetSubMenu(8);// ref lists(key)
		if( !ssMenu )
		{
			AfxMessageBox("Error 236" );
			return;
		}
		if( number < ssMenu->GetMenuItemCount() )
		{
			ssMenu->RemoveMenu(number, MF_BYPOSITION );
			for( int im=ssMenu->GetMenuItemCount()-1; im>=number; im-- )
			{
				CString menu_name;
				ssMenu->GetMenuStringA( im, menu_name, MF_BYPOSITION );
				ssMenu->RemoveMenu(im, MF_BYPOSITION );
				ssMenu->InsertMenu(im, MF_BYPOSITION, ID_PROJECT_REFLIST+im-2, menu_name );
			}
		}
	}
}
void CFreePcbDoc::PlayRefList1(){PlayRefList(0);}
void CFreePcbDoc::PlayRefList2(){PlayRefList(1);}
void CFreePcbDoc::PlayRefList3(){PlayRefList(2);}
void CFreePcbDoc::PlayRefList4(){PlayRefList(3);}
void CFreePcbDoc::PlayRefList5(){PlayRefList(4);}
												
void CFreePcbDoc::PlayRefList6(){PlayRefList(5);}
void CFreePcbDoc::PlayRefList7(){PlayRefList(6);}
void CFreePcbDoc::PlayRefList8(){PlayRefList(7);}
void CFreePcbDoc::PlayRefList9(){PlayRefList(8);}
void CFreePcbDoc::PlayRefList10(){PlayRefList(9);}

void CFreePcbDoc::PlayRefList11(){PlayRefList(10);}
void CFreePcbDoc::PlayRefList12(){PlayRefList(11);}
void CFreePcbDoc::PlayRefList13(){PlayRefList(12);}
void CFreePcbDoc::PlayRefList14(){PlayRefList(13);}
void CFreePcbDoc::PlayRefList15(){PlayRefList(14);}
												  
void CFreePcbDoc::PlayRefList16(){PlayRefList(15);}
void CFreePcbDoc::PlayRefList17(){PlayRefList(16);}
void CFreePcbDoc::PlayRefList18(){PlayRefList(17);}
void CFreePcbDoc::PlayRefList19(){PlayRefList(18);}
void CFreePcbDoc::PlayRefList20(){PlayRefList(19);}

void CFreePcbDoc::PlayRefList( int i )
{
	//
	int thPage = Pages.GetActiveNumber();
	m_view->OnGroupCancelSel();
	id ID( ID_TEXT_DEF );
	int it = -1;
	int ip = -1;
	int ipb = -1;
	CString pgname;
	if( m_dlg_log && i < m_ref_lists->GetSize() )
	{
		m_dlg_log->AddHeader( m_ref_lists->GetStr(i)+" selected", 4 );
	}
	for( CText * t=Pages.GetNextAttr( index_part_attr, &it, &ip ); t; t=Pages.GetNextAttr( index_part_attr, &it, &ip ) )
	{
		if( getbit( t->m_pdf_options, REF_LIST_INDEX + i ) )
		{
			if( ip == Pages.GetActiveNumber() )
				m_view->NewSelect( t, &ID, 0, 0 );
			else
				t->m_selected = 1;
			CString str="";
			if( ipb != ip )
			{
				Pages.GetPageName( ip, &pgname );
				str += ("Page: " + pgname + "\r\n");
			}
			str += ("    Part: " + t->m_str + "\r\n");
			if( m_dlg_log )
				m_dlg_log->AddLine( str );
			ipb = ip;
		}
	}
	if( m_dlg_log )
	{
		m_dlg_log->OnShowMe();
	}
	//
	for( int i=0; i<Pages.GetNumPages(); i++ )
	{
		SwitchToPage(i);
		m_view->OnSelectPartsByAttr();
	}
	SwitchToPage(thPage);
	m_view->SetCursorMode( CUR_GROUP_SELECTED );
	m_view->HighlightGroup();
	m_view->JumpToPart( NULL );
	m_view->Invalidate( FALSE );// (doubtful)
	if( m_view->m_dlg_partlist )
		if( m_view->m_dlg_partlist->IsWindowVisible() )
			m_view->m_dlg_partlist->ImportPartlist();
}

void CFreePcbDoc::EnsureNotMinimized( HWND hWnd, BOOL bGRAB, BOOL bIronScale )
{
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	frm->SetTimer( TMR_SW_PCB, INT_MAX, 0 );


	WINDOWPLACEMENT placement;
	placement.length = sizeof(placement);
	//
	if( GetWindowPlacement(hWnd, &placement) )
	{
		if( placement.showCmd & SW_SHOWMINIMIZED) 
		{
			placement.showCmd = SW_SHOWNORMAL;
			SetWindowPlacement(hWnd, &placement);
		}
	}
	BOOL ThisPageIncludedInNetlist = Pages.IsThePageIncludedInNetlist( Pages.GetActiveNumber() );
	if( bIronScale == 0 )
		SetForegroundWindow(hWnd);
	//
	int itxt = -1;
	int count = 0;
	CString P = "";
	if( getbit( m_view->m_sel_flags, CFreePcbView::FLAG_SEL_PIN ) == 0 )
		for( CText * t=Attr->m_Reflist->GetNextText( &itxt ); t; t=Attr->m_Reflist->GetNextText( &itxt ) )
			if( t->m_selected )
			{
				count++;
				CString ref = t->m_str;
				int iof = ref.FindOneOf("-.");
				if( iof > 0 )
					ref = ref.Left(iof);
				if( ThisPageIncludedInNetlist == 0 )
					ref += "?";
				P += (" \"" + ref + "\"");
			}
			else if( t->m_polyline_start >= 0 && t->m_polyline_start < m_outline_poly->GetSize() )
			{
				if( m_outline_poly->GetAt( t->m_polyline_start ).GetSideSel() )
				{
					count++;
					CString ref = t->m_str;
					int iof = ref.FindOneOf("-.");
					if( iof > 0 )
						ref = ref.Left(iof);
					if( ThisPageIncludedInNetlist == 0 )
						ref += "?";
					P += (" \"" + ref + "\"");
				}
			}
			else
			{

			}
	itxt = -1;
	if( count == 0 )
		for( CText * t=Attr->m_Pinlist->GetNextText( &itxt ); t; t=Attr->m_Pinlist->GetNextText( &itxt ) )
			if( t->m_polyline_start >= 0 &&
				t->m_polyline_start < m_outline_poly->GetSize() )
			{
				CPolyLine * pline = &m_outline_poly->GetAt( t->m_polyline_start );
				if( t->m_selected || pline->GetSel() )
				{
					CText * ref = pline->Check( index_part_attr );
					if( ref )
					{
						CString ref_str = ref->m_str;
						int iof = ref_str.FindOneOf("-.");
						if( iof > 0 )
							ref_str = ref_str.Left(iof);
						CArray<CString> pin_arr;
						int num_pins = ParseMultiPin( &t->m_str, &pin_arr );
						for( int ip=0; ip<num_pins; ip++ )
						{
							count--;
							if( ThisPageIncludedInNetlist == 0 )
								P += (" " + ref_str + "? " + pin_arr[ip] + " 0 0");
							else
								P += (" " + ref_str + " " + pin_arr[ip] + " 0 0");
						}
					}
				}
			}
	BOOL BLK_SELECTED = 0;
	if( count == 0 )
	{
		CString M_BLK_NAMES = dSPACE;
		int it = -1;
		int ip = -1;
		int cur_pcb_i = Pages.GetPcbIndex( Pages.GetActiveNumber(), TRUE );
		while( CText * blk = Pages.FindBLK_PTR( &it, &ip, TRUE ) )
		{
			if( Pages.GetPcbIndex( ip, FALSE ) != cur_pcb_i )
				continue;
			int old_ip = SwitchToPage( ip, 0, 0 );
			CString Name, Link, out="", Asuff;
			ExtractBLK_PTR( &blk->m_str, &Name, &Link );
			if( M_BLK_NAMES.Find(dSPACE + Link + Name + dSPACE) == -1 )
			{
				M_BLK_NAMES += (Link + Name + dSPACE);
				int num = 0;
				CPolyLine * p = &m_outline_poly->GetAt( blk->m_polyline_start );	 
				Pages.GetBlockParts( &out, blk, NL_PADS_PCB, &num, m_dlg_log, &Asuff, NULL, p->GetSideSel()?-1:old_ip );
				Name = "";
				for(int u=out.Find("\n"); u>=0; u=out.Find("\n"))
				{
					Link = out.Left(u);
					if( Link.GetLength() )
					{
						int sp = Link.Find(" ");
						if( sp > 0 )
							Name += Link.Left(sp) + " ";
					}
					out = out.Right( out.GetLength()-u-1 );
				}
				P += Name;
			}
			SwitchToPage( old_ip, 0, 0 );
		}
		if( P.GetLength() )
		{
			count = 1;
			BLK_SELECTED = 1;
		}
	}
	if( count )
	{
		CString mess="cancel_sel";
		if( bIronScale )
			mess="*cancel_sel";
		OnSendMess( hWnd, &mess );
		if( bGRAB )
			mess = "move:" + P;
		else if( BLK_SELECTED )
			mess = "parts: " + P;
		else if( count == 1 || count == -1 )
			mess = "jump:" + P;
		else if( count > 0 )
			mess = "parts:" + P;
		else if( count < 0 )
			mess = "pins:" + P;
		if( bIronScale )
			mess = "*" + mess;
		OnSendMess( hWnd, &mess );
		if( bGRAB == 0 )
		{
			if( bIronScale )
				mess = "*show_sel";
			else
				mess = "show_sel";
			Sleep(100);// for redrawing freepcb window
			OnSendMess( hWnd, &mess );
		}
	}
	else
	{
		CString mess = "check_netlist";
		SetForegroundWindow(hWnd);
		OnSendMess( hWnd, &mess );
	}
}

HWND CFreePcbDoc::OnSendMess(HWND hWnd, CString * mess )
{
	COPYDATASTRUCT cd;
	int m_len = mess->GetLength();
    cd.cbData = m_len + 1;
	char * data;
	data = new char[m_len+1];//new016
    for (int cp=m_len; cp>0; cp--)
		data[cp-1] = mess->GetAt(cp-1);
	data[m_len] = '\0';
	cd.lpData = data;
	cd.cbData = m_len+1;
	int RES = 0;
	if( hWnd )
		RES = ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
	delete data;//new016
	if( RES )
		return hWnd;
	else
		return NULL;
}

void CFreePcbDoc::SwitchToPCB()
{
	SwitchToPCB( FALSE, FALSE );
}
void CFreePcbDoc::SwitchToPcbOnButton()
{
	if (bNoFilesOpened)
	{
		SwitchToPCB();
		return;
	}
	CString MSG;
	Check_Txt_Msg(NULL, &MSG);
	if( MSG.GetLength() > 10)
		OnFileExport( FALSE );
	else
		SwitchToPCB();
}
void CFreePcbDoc::SwitchToPCB( BOOL duty, BOOL bGRAB, BOOL bIronScale )
{
	CString PcbName = "";
	BOOL noPcbFile = 0;
	if (bNoFilesOpened)
	{
		noPcbFile = TRUE;
	}
	else if( Pages.IsThePageIncludedInNetlist(Pages.GetActiveNumber()) == 0 )
	{
		int gip = Pages.IsThePageIncludesCP();
		if( gip >= 0 )
			PcbName = Pages.GetCurrentPCBName(gip);
		if( PcbName.GetLength() == 0 )
		{
			if( bIronScale == 0 && duty == 0 )
				AfxMessageBox(G_LANGUAGE == 0 ?
					"The current page is not included in the netlist. You can connect the pages in the Netlist Settings dialog box (the File menu)" :
					"Текущая страница не включена в список эл.цепей. Вы можете подключить страницы в диалоговом окне Настройки списка (меню Файл)");
			else
				return;
			noPcbFile = TRUE;
		}
	}
	if (noPcbFile)
	{
		CString FreePcb2 = m_app_dir + "\\FreePcb.exe";
		if (G_LANGUAGE)
			FreePcb2 = m_app_dir + "\\ПлатФорм.exe";
		if ((UINT)ShellExecute(NULL, "open", "\"" + FreePcb2 + "\"", "", "\"" + m_app_dir + "\"", SW_SHOWNORMAL) > 32)
		{

		}
		return;
	}
	else if(PcbName.GetLength() == 0)
		PcbName = Pages.GetCurrentPCBName();
	if( bIronScale == 0 )
		if( m_view->m_sel_count )
			SelectGroupAttributes();
	CString name = FPC_HEADER + PcbName;
	HWND wnd = FindWindow(NULL,name);
	if( wnd )
	{
		EnsureNotMinimized(wnd,bGRAB,bIronScale);
	}
	else
	{
		name += "*";
		wnd = FindWindow(NULL,name);
		if( wnd )
		{
			EnsureNotMinimized(wnd,bGRAB,bIronScale);
		}
		else
		{
			if( duty || bIronScale )
				return;
			int rf = m_pcb_full_path.ReverseFind( '\\' );
			CString fpc_file = m_pcb_full_path.Left(rf) + "\\" + PcbName;
			struct _stat buf;
			int err = _stat( fpc_file, &buf );//ok
			if( err )
			{
				// check pcb names
				Pages.CheckPCBNames( &m_path_to_folder );
				CString NewName = Pages.GetCurrentPCBName();
				if( PcbName.CompareNoCase( NewName ) )
				{
					SwitchToPCB( duty, bGRAB, bIronScale );
					return;
				}
				CStdioFile defF;
				CString def_file = m_app_dir + "\\default.cfg";
				int op1 = defF.Open( def_file, CFile::modeRead, NULL );
				if( op1 )
				{
					CStdioFile fpcF;
					int op2 = fpcF.Open( fpc_file, CFile::modeCreate | CFile::modeWrite, NULL );
					if( op2 )
					{
						CString fpc_name = m_name;
						if( m_name.Right(4) == ".cds" )
							fpc_name = m_name.Left( m_name.GetLength()-4 );
						CString in_str;
						while( defF.ReadString(in_str) )
						{
							in_str.Trim();
							if( in_str.Left(6) == "m_org_" ||
								in_str.Left(13) == "m_client_rect" ||
								in_str.Left(14) == "m_scale_factor" )
							{// skip
								continue;
							}// skip
							else if( in_str.Left(11) == "m_sel_mask:" )
							{
								fpcF.WriteString("m_sel_mask: 2147483647\n");
								continue;
							}
							else if( in_str.Left(12) == "m_top_layer:" )
							{
								fpcF.WriteString("m_top_layer: 13\n");
								continue;
							}
							else if( in_str.Left(17) == "m_visible_layers:" )
							{
								fpcF.WriteString("m_visible_layers: -5\n");
								continue;
							}
							else if( in_str.Left(13) == "project_name:" )
							{
								fpcF.WriteString("project_name: \""+fpc_name+".fpc\"\n");
								continue;
							}
							else if( in_str.Left(20) == "netlist_import_flags" )
							{
								fpcF.WriteString("netlist_import_flags: 4039\n");
								continue;
							}
							else if( in_str.Left(19) == "full_library_folder" )
							{
								fpcF.WriteString("full_library_folder: \"" + m_full_lib_dir + "\"\n");
								continue;
							}
							else if( in_str.Compare("[end]") == 0 )
							{
								fpcF.WriteString("[footprints]\n");
								fpcF.WriteString("[board]\n");
								fpcF.WriteString("  outline: 4 0 254000 1 -1 -1\n");
								fpcF.WriteString("    corner: 1 100000000 0 0 0\n");
								fpcF.WriteString("    corner: 2 0 0 0 0\n");
								fpcF.WriteString("    corner: 3 0 50000000 0 0\n");
								fpcF.WriteString("    corner: 4 100000000 50000000 0 0\n");
								fpcF.WriteString("[solder_mask_cutouts]\n");
								fpcF.WriteString("[parts]\n");
								fpcF.WriteString("[nets]\n");
								fpcF.WriteString("[texts]\n");
								fpcF.WriteString("text: \""+PcbName.Left( PcbName.GetLength()-4 )+"\" 2000000 1600000 10 0 0 2540000 254000 0 -1 -1\n");
								fpcF.WriteString("text: \""+PcbName.Left( PcbName.GetLength()-4 )+"\" 98000000 1600000 11 0 1 2540000 254000 0 -1 -1\n");
								fpcF.WriteString("[merges]\n");
							}
							fpcF.WriteString(in_str);
							fpcF.WriteString("\n\n");
						}					
						fpcF.Close();
					}
					defF.Close();
				}
			}
			if( m_view->m_sel_count )
			{
				CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
				frm->SetTimer( TMR_SW_PCB, 1000, 0 );
			}
			CString FreePcb2 = m_app_dir + "\\FreePcb.exe";
			if(G_LANGUAGE)
				FreePcb2 = m_app_dir + "\\ПлатФорм.exe";
			if ( (UINT)ShellExecute(	NULL,"open", "\""+FreePcb2+"\"", "\""+fpc_file+"\"", "\""+m_app_dir+"\"", SW_SHOWNORMAL) > 32 )
			{
				
			}
		}
	}
}

void CFreePcbDoc::SwitchToCDS( CString * CdsFile, CString * PartRef, BOOL bMERGE )
{
	static CString ref_stat = OLD_NO_FP;
	static CString window = OLD_NO_FP;
	static int bWAIT = 0;
	if( CdsFile && bWAIT == 0 )
	{
		ref_stat = *PartRef;
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		if( bMERGE )
			frm->SetTimer( TMR_SW_CDSM, 1000, 0 );
		else
			frm->SetTimer( TMR_SW_CDS, 1000, 0 );
		CString FreePcb2 = m_app_dir + "\\FreeCds.exe";
		if(G_LANGUAGE)
			FreePcb2 = m_app_dir + "\\СхемАтор.exe";
		if( CdsFile->CompareNoCase( m_pcb_full_path ) )
			if ( (UINT)ShellExecute(	NULL,"open", "\""+FreePcb2+"\"", "\""+(*CdsFile)+"\"", "\""+m_app_dir+"\"", SW_SHOWNORMAL) > 32 )
			{
				bWAIT = 10;
			}	
		int rf = CdsFile->ReverseFind('\\');
		if( rf > 0 )
			window = CDS_HEADER + CdsFile->Right( CdsFile->GetLength() - rf - 1 );
	}
	else 
	{
		if( CdsFile == NULL )
			bWAIT--;
		bWAIT = max( bWAIT, 0 );
		CString name = window;
		HWND wnd = FindWindow(NULL,name);
		if( wnd == NULL )
		{
			name += "*";
			wnd = FindWindow(NULL,name);
		}
		if( wnd )
		{
			// TIMER off
			CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
			frm->SetTimer( TMR_SW_CDS, INT_MAX, 0 );
			frm->SetTimer( TMR_SW_CDSM, INT_MAX, 0 );
			bWAIT = 0;

			CString mess;
			if( bMERGE )
				mess = "select_merge: \"" + ref_stat + "\"";
			else
				mess = "valuedogpackage: \"" + ref_stat + "\"";
			OnSendMess( wnd, &mess );
		}
	}
}

void CFreePcbDoc::CheckBOM()
{
	for( int ip=0; ip<Pages.GetNumPages(); ip++ )
	{
		CTextList * Dsc = Pages.GetTextList( ip, index_desc_attr );
		CArray<CPolyLine> * p_arr = Pages.GetPolyArray(ip);
		int it = -1;
		for( CText * t=Dsc->GetNextText(&it); t; t=Dsc->GetNextText(&it) )
		{
			if( t->m_polyline_start < 0 )
				continue;
			m_view->DrawBOM( t, p_arr );
		}
	}
}

int CFreePcbDoc::ExtractBLK( CString * desc, CString * out, CString * link )
{
	int page_number = -1;
	int p = desc->Find( CP_SUFFIX );
	if( p >= 0 )
	{
		CString cmd = desc->Right( desc->GetLength() - p );
		*out = cmd;
		p = out->Find("'");
		if( p > 0 )
		{
			*out = out->Left(p);
			out->Replace(":"," ");
			p = out->Find(" ");
			if( p > 0 )
			{
				*out = out->Right( out->GetLength() - p );
				*out = out->Trim();
			}
		}
		if( link )
		{
			cmd.Replace(":"," ");
			cmd.Replace("'"," ");
			CString Key;
			CArray<CString> arr;
			int ns = ParseKeyString( &cmd, &Key, &arr );
			if( ns >= 8 )
			{
				*link = arr.GetAt(2);
				page_number = my_atoi(&arr.GetAt(4) );
			}
		}
	}
	return page_number-1;
}

int CFreePcbDoc::ExtractBLK_PTR( CString * desc, CString * name, CString * ref, CString * pin )
{
	int page_number = -1;
	int iof = desc->Find("|");
	if( iof > 0 )
	{
		CString cmd = desc->Right( desc->GetLength() - iof );
		cmd.Replace(":"," ");
		cmd.Replace("'"," ");
		CString Key;
		CArray<CString> arr;
		int ns = ParseKeyString( &cmd, &Key, &arr );
		if( name )
		{
			*name = "";
			if( ns >= 2 )
				*name = arr.GetAt(0);
			if( name->Left(3) == "|LI" )
				*name = "";
		}
		if( ref )
		{
			*ref = "";
			if( ns >= 4 )
				*ref = arr.GetAt(2);
			if( ref->Left(3) == "|pi" )
				*ref = "";
		}
		if( pin )
		{
			*pin = "";
			if( ns >= 6 )
				*pin = arr.GetAt(4);
			if( pin->Left(3) == "|pa" )
				*pin = "";
		}
		if( ns >= 8 )
			page_number = my_atoi(&arr.GetAt(6) );
	}
	return page_number-1;
}

int CFreePcbDoc::RenumberComplexPart( CText * description, CString * old_suff, CString * new_suff, CString * ASUFF )
{
	static BOOL MessShown = 0;
	if( MessShown == 0 )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Warning! Complex part renumbering has no undo option. ":
			"Внимание! Перенумерация иерархических деталей не имеет возможности отмены", MB_ICONWARNING);
	MessShown = TRUE;
	if( m_project_modified )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The file has been modified. First save the file and then try again":
			"Файл был изменен. Сначала сохраните файл, а затем попробуйте еще раз.", MB_ICONWARNING);
		return 0;
	}
	
	CString SFX, LINK, key;
	int BPAGE = ExtractBLK( &description->m_str, &SFX, &LINK );
	int L = new_suff->GetLength();
	int cnt = 0;
	CString Parts;
	int cur_pg = SwitchToPage( BPAGE );
	CText * pin_descriptor = Pages.FindBLK_PTR( &description->m_str, cur_pg );
	if( pin_descriptor == NULL )
		return 0;
	if( Pages.GetPcbIndex( cur_pg, FALSE ) == -1 )
	{
		int it=-1, ip=-1;
		for( CText * t=Pages.FindBLK( &it, &ip, TRUE ); t; t=Pages.FindBLK( &it, &ip, TRUE ) )
		{
			if( ip >= cur_pg )
				break;
			
			int blk_pg = ExtractBLK( &t->m_str, &SFX, &LINK );
			
			if( blk_pg == cur_pg )
			{
				CString sfx = SFX;
				if( ASUFF )
					sfx = *ASUFF + SFX;
				if( Pages.GetPcbIndex( ip, FALSE ) == -1 )
				{
					int save_p = SwitchToPage( ip );
					
					RenumberComplexPart( t, old_suff, new_suff, &sfx );
					SwitchToPage( save_p );
				}
				else
				{
					Parts = "";
					Pages.GetBlockParts( &Parts, pin_descriptor, NL_PADS_PCB, &cnt, NULL, &SFX );
					if( Parts.GetLength() )
					{
						Parts.Replace( dSPACE, "\n" );
						Parts.Replace( " ", "" );
						Parts = "parts: " + Parts;
						CArray<CString> arr;
						int np = ParseKeyString( &Parts, &key, &arr );
						for( int item=0;  item<arr.GetSize(); item+=2 )
						{
							CString ref = arr.GetAt(item);
							if( ref.Right( L+sfx.GetLength() ).Compare((*new_suff) + sfx) == 0 )
							{
								CString oldref = ref.Left( ref.GetLength()-L-sfx.GetLength() ) + (*old_suff) + sfx;
								Renumber__Add( &oldref, &ref, cur_pg, -1, TRUE );
							}
						}
					}
				}
			}
		}
		/*if( cnt == 0 )
		{
			// нет ни одного блока ссылающегося на текущую страницу
			AfxMessageBox("It is not possible to create a rename file for this "\
				"complex part because there is no block referring to the "\
				"current page.\n\nA complex part can contain other complex parts. "\
				"Renaming of a complex sub-parts is allowed, but renaming "\
				"of deeper hierarchies is currently not provided in the program.");
			return 0;
		}*/
	}
	else
	{
		Pages.GetBlockParts( &Parts, pin_descriptor, NL_PADS_PCB, &cnt, NULL );
		if( Parts.GetLength() )
		{
			Parts.Replace( dSPACE, "\n" );
			Parts.Replace( " ", "" );
			Parts = "parts: " + Parts;
			CArray<CString> arr;
			int np = ParseKeyString( &Parts, &key, &arr );
			for( int item=0;  item<arr.GetSize(); item+=2 )
			{
				CString ref = arr.GetAt(item);
				CString oldref = ref.Left( ref.GetLength()-L ) + (*old_suff);
				Renumber__Add( &oldref, &ref, cur_pg, -1, TRUE );
			}
		}
	}
	SwitchToPage( cur_pg );
	ResetUndoState();
	return 1;
}

void CFreePcbDoc::SelectAdj()
{
	int nc = SelectAdjacent(m_outline_poly);
	if (nc)
	{
		m_view->m_sel_count += nc;
		m_view->SetCursorMode(CUR_GROUP_SELECTED);
		m_dlist->CancelHighLight();
		m_view->HighlightGroup();
		OnRangeCmds(NULL);
	}
}

void CFreePcbDoc::OnSimilarPoly()
{
	id ID( ID_POLYLINE, ID_GRAPHIC, -1, ID_SIDE, -1 );
	CPolyLine * sel_poly = &m_outline_poly->GetAt( m_view->m_sel_id.i );
	for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
	{
		if( m_outline_poly->GetAt(i).m_visible == 0 )
			continue;
		if( sel_poly->Compare( &m_outline_poly->GetAt(i) ) )
		{
			ID.i = i;
			for( int ii=0; ii<m_outline_poly->GetAt(i).GetNumSides(); ii++ )
			{
				ID.ii = ii;
				m_view->NewSelect( NULL, &ID, 0, 0 );
			}
			//
			CString label;
			label.Format( "Marker\r\n" );
			DRError * dre = m_drelist->Add( &label, ID, ID,	m_outline_poly->GetAt(i).GetX(0), 
															m_outline_poly->GetAt(i).GetY(0), 
															m_outline_poly->GetAt(i).GetX(0), 
															m_outline_poly->GetAt(i).GetY(0), 20*NM_PER_MIL, LAY_DRC_ERROR, NULL );	
		}
	}
	m_view->SetCursorMode( CUR_GROUP_SELECTED );
	m_view->HighlightGroup();
	m_view->JumpToPart(NULL);
	OnRangeCmds(NULL);
}

void CFreePcbDoc::OnReplacePoly()
{
	if( m_view->m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			("First select polylines that you want to replace. "\
			"You can select the polyline side and use the "\
			"right-click menu \"Select Similar Polylines\""\
			"\n\nAll selected polylines will be replaced by a reference polyline, "\
			"the number of which you select from the drop-down list in the next dialog box."\
			"\n\nHow can I find out the reference polyline number?"\
			"\n\nWhen a segment of any polyline is selected, its number is displayed "\
			"in the status bar at the bottom of the window. "\
			"So start by remembering the reference polyline number."):
			("Сначала выберите полилинии, которые вы хотите заменить. "\
			"Вы можете выбрать сторону полилинии и использовать меню правой кнопки мыши «Выделить аналогичные полилинии»"\
			"\n\nВсе выбранные полилинии будут заменены опорной полилинией, "\
			"номер которой вы выбираете из раскрывающегося списка в следующем диалоговом окне."\
			"\n\nКак узнать номер опорной полилинии?"\
			"\n\nКогда выбран сегмент любой полилинии, его номер отображается "\
			"в строке состояния в нижней части окна. "\
			"Поэтому следует начать с запоминания номера опорной полилинии..."), MB_ICONINFORMATION);
		return;
	}
	CDlgAddMerge dlg;
	dlg.Doc = this;
	dlg.m_mode = dlg.M_REPLACE_POLY;
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		int BEGIN = AfxMessageBox(G_LANGUAGE == 0 ? 
			"By default, the coordinates of the first vertices of both polylines will be the same. Align the beginnings of polylines?":
			"По умолчанию координаты первых вершин обеих полилиний будут одинаковыми. Выровнять начала полилиний? \n\nДА - начало полилиний  \nНЕТ - конец полилиний", MB_YESNO);
		this->SelectGroupAttributes();
		m_view->SaveUndoInfoForGroup( m_undo_list );
		int nmb = my_atoi( &dlg.m_merge_name );
		if( nmb > m_outline_poly->GetSize() )
			return;
		CPolyLine * sel_poly = &m_outline_poly->GetAt(nmb-1);
		if( sel_poly->GetNumSides() < 1 )
			return;

		int hdx = sel_poly->GetX(0);
		int hdy = sel_poly->GetY(0);
		for( int i=m_outline_poly->GetSize()-1; i>=0; i-- )
		{
			if( m_outline_poly->GetAt(i).m_visible == 0 )
				continue;
			if( m_outline_poly->GetAt(i).GetNumSides() < 1 )
				continue;
			if( m_outline_poly->GetAt(i).GetSideSel() )
			{
				m_outline_poly->GetAt(i).Undraw();
				int dx = m_outline_poly->GetAt(i).GetX(0);
				int dy = m_outline_poly->GetAt(i).GetY(0);
				m_outline_poly->GetAt(i).Replace( sel_poly, (BEGIN==IDYES?0:1) );
				m_outline_poly->GetAt(i).Draw();
				for( int iatt=0; iatt<num_attributes; iatt++ )
					if( m_outline_poly->GetAt(i).Check(iatt) )
					{
						// redraw texts
						m_outline_poly->GetAt(i).pAttr[iatt]->MakeVisible();
						if( iatt == index_net_attr )
							if( m_outline_poly->GetAt(i).GetLayer() >= LAY_ADD_1 )
								m_outline_poly->GetAt(i).pAttr[iatt]->InVisible();
					}
			}
		}
		m_view->CancelSelection( FALSE );// !!!
		m_view->OnRangeCmds( NULL );
		ProjectModified(TRUE);
	}
}

BOOL CFreePcbDoc::OnOpenTemplate( UINT CMD )
{
	CString Path =	(CMD==ID_FILE_OPEN_TEMPLATE)?
					(m_path_to_folder+"\\related_files\\reports\\bom\\bom.cds"):
					(m_path_to_folder+"\\related_files\\reports\\tag\\tag.cds");
	CString Path2 = (CMD==ID_FILE_OPEN_TEMPLATE)?
					(m_app_dir+"\\reports\\bom\\bom.cds"):
					(m_app_dir+"\\reports\\tag\\tag.cds");
	
	if( m_project_open == 0 )
		Path = Path2;

	struct _stat buf;
	if( _stat( Path, &buf ) )
	{
		//now save templates
		CString p0 = m_path_to_folder+"\\related_files";
		if( _stat( p0, &buf ) )
			_mkdir( p0 );
		p0 += "\\reports";
		if( _stat( p0, &buf ) )
			_mkdir( p0 );
		p0 += (CMD==ID_FILE_OPEN_TEMPLATE)?"\\bom":"\\tag";
		if( _stat( p0, &buf ) )
			_mkdir( p0 );
		CopyFile( Path2, Path, 1 ); //fail if exists
		p0 += "\\related_files";
		if( _stat( p0, &buf ) )
			_mkdir( p0 );
		p0 += "\\pictures";
		if( _stat( p0, &buf ) )
			_mkdir( p0 );
		p0 += "\\";
		int rf = Path2.ReverseFind('\\');
		CString sourcename = Path2.Left(rf);
		sourcename += rel_f_pictures;
		CFileFind finder;
		BOOL bWorking = finder.FindFile( sourcename + "*" );
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			CString fn = finder.GetFilePath();
			CString pic_name = finder.GetFileName();
			if( !finder.IsDirectory() && !finder.IsDots() )
			{
				CopyFile( fn, p0+pic_name, 1 );
			}
		}
		EnableMenuItems();
	}
	ShellExecute(	NULL, 
					"open", 
					G_LANGUAGE == 0 ? ("\""+m_app_dir+"\\freecds.exe\""): ("\"" + m_app_dir + "\\СхемАтор.exe\""),
					("\""+Path+"\""), 
					m_app_dir, 
					SW_SHOWNORMAL );
	return 1;
}

BOOL CFreePcbDoc::OnCompare( UINT CMD )
{
	CCompare C;
	if( CMD == ID_PROJECT_COMPARE_1 )
		C.Compare( this, 0 );
	else if( CMD == ID_PROJECT_COMPARE_2 )
		C.Compare( this, 1 );
	OnRangeCmds(NULL);
	return 1;
}

void CFreePcbDoc::OnProtection()
{
	CString MSG="";
	Check_Txt_Msg( NULL, &MSG );
	if( MSG.Compare( FILE_PROTECTED ) && MSG.GetLength() )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			("Unable to protect page\n\n("+MSG+")"):
			("Невозможно защитить страницу\n\n(" + MSG + ")"), MB_ICONERROR);
		return;
	}
	if( Pages.GetCurrentPcbIndex() == -1 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The current page is not included in the netlist. You can connect the pages in the Netlist Settings dialog box (the File menu)" :
			"Текущая страница не включена в список эл.цепей. Вы можете подключить страницы в диалоговом окне Настройки списка (меню Файл)");
		return;
	}
	CDlgProtection dlg;
	dlg.Initialize( this );
	dlg.DoModal();
}

void CFreePcbDoc::OnCreateBranch()
{
	DlgCreateBranch dlg;
	dlg.Initialize( this );
	dlg.DoModal();
}

void CFreePcbDoc::OnSelectProjectFolder()
{
	CPathDialog dlg(G_LANGUAGE == 0 ? "Project Folder":"Папка проекта", 
					G_LANGUAGE == 0 ? "Select default project folder":"Выберите папку", m_parent_folder);
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		m_parent_folder = dlg.GetPathName();
		OnReloadMenu();
		SaveOptions();
	}
}

void CFreePcbDoc::OnReloadMenu()
{
	CMenu* pMenu = &theApp.m_main;
	CMenu* smenu = pMenu->GetSubMenu(0); // file menu
	CWnd * WND = theApp.GetMainWnd();
	CRect wRect;
	if( WND && smenu )
	{
		WND->GetWindowRect( wRect );
	}
	//
	m_library.Clear();
	m_merge_library.Clear();
	ClearSpeedFiles( this );
	LoadSpeedFiles( this );
	pMenu = &theApp.m_main;
	smenu = pMenu->GetSubMenu(0); // file menu
	if( smenu )
	{
		smenu->TrackPopupMenu(TPM_LEFTALIGN, wRect.left+9, wRect.top+50, WND);
		CMenu* ssmenu = smenu->GetSubMenu(2); // open from project folder menu
		if( ssmenu )
		{
			//ssmenu->TrackPopupMenu(TPM_LEFTALIGN, wRect.left+300, wRect.top+30, WND);
		}
		ReadFootprintFolder(this);
	}
}

void CFreePcbDoc::OnRunLibManager()
{
	HANDLE cmd = GlobalAlloc(GMEM_FIXED,2048);
	CString Mess;
	Mess.Format("SchConstr: \"\" \"%s\"", m_full_lib_dir );
	memcpy( cmd, Mess, Mess.GetLength()+1 );
	//
	HWND WND = theApp.GetMainWnd()->GetSafeHwnd();
	do Sleep(100);  
		while(!OpenClipboard(WND));
	EmptyClipboard();
	SetClipboardData(CF_TEXT,cmd);
	CloseClipboard();
	CString folder = m_app_dir+"\\FPC_EXE\\FreePCB_Libraries";
	ShellExecute(	NULL,"open",folder+"\\ABC_library.exe",
										NULL,folder,SW_SHOWNORMAL);
}

void CFreePcbDoc::OnRunFontManager()
{
	CString folder = m_app_dir;
	ShellExecute(	NULL,"open",folder+"\\FontManager.exe",
										NULL,folder,SW_SHOWNORMAL);
}

void CFreePcbDoc::OnRunImpDxf()
{
	CString folder = m_app_dir+"\\FPC_EXE\\FreePCB_ImportDXF";
	ShellExecute(	NULL,"open",folder+"\\DXF_2_FPC_FreePCB.exe",
										NULL,folder,SW_SHOWNORMAL);
}

void CFreePcbDoc::OnRunImpPcad()
{
	CString folder = m_app_dir+"\\FPC_EXE\\FreePcb_ImportPcadSCH";
	ShellExecute(	NULL,"open",folder+"\\import.exe",
										NULL,folder,SW_SHOWNORMAL);
}

void CFreePcbDoc::OnImportNetlist()
{
	if( m_project_modified )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The project has been modified. Save the project before using this option":
			"Проект был изменен. Сохраните проект перед использованием этой опции.", MB_ICONWARNING);
		return;
	}
	int it = -1;
	if( Pages.IsThePageIncludedInNetlist( Pages.GetActiveNumber(), TRUE ) || Attr->m_Reflist->GetNextText(&it) )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Netlist can only be imported to a blank page that is not included in any netlist":
			"Список эл.цепей можно импортировать только на пустую страницу, которая не включена ни в один список.", MB_ICONWARNING);
		return;
	}
	CString str = RunFileDialog( 1, "net" );
	if( str.GetLength() )
	{
		CStdioFile NListFile;
		int ok = NListFile.Open( str, CFile::modeRead, NULL );
		if( ok )
		{
			Import_PADSPCB_Netlist( this, &NListFile );
			NListFile.Close();
		}
	}
}

void CFreePcbDoc::OnAddFavorite()
{
	CText * Fav = m_outline_poly->GetAt(m_view->m_sel_id.i).Check( index_net_attr );
	BOOL Fail = (m_outline_poly->GetAt(m_view->m_sel_id.i).Check( index_pin_attr )||
				 m_outline_poly->GetAt(m_view->m_sel_id.i).Check( index_part_attr ));
	if( Fail )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This polyline has part name and cannot be added to favorite net labels.":
			"Эта полилиния относится к детали и не может быть добавлена в избранные метки.", MB_ICONERROR);
		return;
	}
	if( Fav == NULL )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This polyline has no net name and cannot be added to favorite net labels. Set net attribute with F1- \"Set Attributes\" button":
			"Эта полилиния не имеет уникального имени цепи и не может быть добавлена в избранные метки эл.цепей. Установите атрибут эл.цепи с помощью кнопки F1 - «Задать атрибут»", MB_ICONERROR);
		return;
	}
	else if( Fav->m_str.Left(3) == FREENET )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This polyline has no user net name and cannot be added to favorite net labels.":
			"Эта полилиния не имеет уникального имени цепи и не может быть добавлена в избранные метки эл.цепей.", MB_ICONERROR);
		return;
	}
	int mem_number_of_page = Pages.GetActiveNumber();
	int find = -1;
	for( int i=0; i<MAX_FAVORITES; i++ )
	{
		if( m_view->m_favorite_poly[i] < 0 || m_view->m_favorite_page[i] < 0 )
			continue; // because we look for similar link names first
		if( m_view->m_favorite_page[i] >= Pages.GetNumPages() )
			continue; // error
		if( m_view->m_favorite_page[i] != Pages.GetActiveNumber() )
			SwitchToPage( m_view->m_favorite_page[i] );
		//
		if( m_view->m_favorite_poly[i] >= m_outline_poly->GetSize() )
		{
			find = i;
			break;
		}
		CText * net = m_outline_poly->GetAt( m_view->m_favorite_poly[i] ).Check( index_net_attr );
		if( net )
			if( net->m_str.Compare( Fav->m_str ) == 0 )
			{
				int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
					"The favorite net labels already contain this net name. Replace this label?":
					"Избранные метки эл.цепей уже содержат это имя. Заменить этой меткой старую метку? \n\nДА - заменить, \nНЕТ - сохранить как есть, будет две с одинаковыми именами", MB_YESNO | MB_ICONQUESTION);
				if( ret == IDYES )
				{
					find = i;
					break;
				}
			}
	}
	if( find == -1 )
		for( int i=0; i<MAX_FAVORITES; i++ )
		{
			if( m_view->m_favorite_poly[i] < 0 || 
				m_view->m_favorite_page[i] < 0 || 
				m_view->m_favorite_page[i] >= Pages.GetNumPages() )
			{
				find = i;
				break;
			}
			if( m_view->m_favorite_page[i] != Pages.GetActiveNumber() )
				SwitchToPage( m_view->m_favorite_page[i] );
			if( m_view->m_favorite_poly[i] < m_outline_poly->GetSize() )
			{
				CText * net  = m_outline_poly->GetAt( m_view->m_favorite_poly[i] ).Check( index_net_attr );
				if( net == NULL )
				{
					find = i;
					break;
				}
			}
		}
	SwitchToPage( mem_number_of_page );
	if( find == -1 )
	{
		// shift index
		//for( int i=MAX_FAVORITES-1; i>0; i-- )
		//	m_view->m_favorite_poly[i-1] = m_view->m_favorite_poly[i];
		// shift page number
		//for( int i=MAX_FAVORITES-1; i>0; i-- )
		//	m_view->m_favorite_page[i-1] = m_view->m_favorite_page[i];
		CDlgAddMerge dlg;
		dlg.Doc = this;
		dlg.m_mode = dlg.M_REPLACE_FAV;
		int ret = dlg.DoModal();
		if( ret == IDOK && dlg.m_box_index >= 0 && dlg.m_box_index < MAX_FAVORITES )
		{
			m_view->m_favorite_poly[dlg.m_box_index] = m_view->m_sel_id.i;
			m_view->m_favorite_page[dlg.m_box_index] = Pages.GetActiveNumber();
		}
	}
	else
	{
		m_view->m_favorite_poly[find] = m_view->m_sel_id.i;
		m_view->m_favorite_page[find] = Pages.GetActiveNumber();
	}
	m_view->InvalidateLeftPane();
	m_view->OnRangeCmds(NULL);
}

void CFreePcbDoc::OnOpenComponentDatasheets()
{
	CString path = m_app_dir;
	path += main_component;
	ShellExecute( NULL,"open",path, NULL, path, SW_SHOWNORMAL );
}

void CFreePcbDoc::OnOpenComponentDatasheetsM()
{
	int rf = m_pcb_full_path.ReverseFind('\\');
	if( rf <= 0 )
		return;
	CString path = m_pcb_full_path.Left(rf) + rel_f_component;
	ShellExecute( NULL,"open",path, NULL, path, SW_SHOWNORMAL );
}

void CFreePcbDoc::OnComponentImage()
{
	CText * part_found = NULL;
	CString Parts="";
	CString Value="";
	CString Footprint="";
	int WARNING_P=0;
	int WARNING_V=0;
	int WARNING_F=0;
	int no_part_attribute=0;
	m_view->SelPolylineTest( &part_found, &Parts, &Value, &Footprint, &WARNING_P, &WARNING_V, &WARNING_F, &no_part_attribute, 100 );
	if( WARNING_F == 1 )
	{
		ExtractComponentName( &Value, &Footprint );
		if( Footprint.GetLength() )
		{
			ComponentImage( &Footprint );
		}
	}
}

void CFreePcbDoc::ComponentImage( CString * fp )
{
	CString filename = RunFileDialog( 1, "img" );
	if ( filename.GetLength() )
	{ 
		CString n2 = m_app_dir + main_component;
		int revf = filename.ReverseFind('.');
		if( revf > 0 )
			*fp += filename.Right( filename.GetLength()-revf );
		n2 += ("\\" + (*fp));
		int rf = n2.ReverseFind('.');
		if( rf > 0 )
		{
			CString cs = n2.Left(rf);
			DeleteFile( cs+".jpeg" );
			DeleteFile( cs+".jpg" );
			DeleteFile( cs+".bmp" );
		}
		CopyFile( filename, n2, 0 );
	}
}

void CFreePcbDoc::OnComponentURL()
{
	CText * part_found = NULL;
	CString Parts="";
	CString Value="";
	CString Footprint="";
	int WARNING_P=0;
	int WARNING_V=0;
	int WARNING_F=0;
	int no_part_attribute=0;
	m_view->SelPolylineTest( &part_found, &Parts, &Value, &Footprint, &WARNING_P, &WARNING_V, &WARNING_F, &no_part_attribute, 100 );
	if( WARNING_V == 1 && WARNING_F == 1 )
	{
		ExtractComponentName( &Value, &Footprint );
	}
	else
		return;
	if( Footprint.GetLength() == 0 ||
		Value.GetLength() == 0 )
		return;
	CString Link = "";
	CString path = m_app_dir + main_component + "\\" + Value + "@" + Footprint +".url";
	CString path2 = m_path_to_folder + rel_f_component + "\\" + Value + "@" + Footprint +".url";
	CStdioFile ff;
	if( ff.Open( path, CFile::modeRead, NULL ) )
	{
		ff.ReadString(Link);
		ff.ReadString(Link);
		ff.Close();
		if( Link.GetLength() > 4 )
			Link = Link.Right( Link.GetLength()-4 );
	}
	else if( ff.Open( path2, CFile::modeRead, NULL ) )
	{
		ff.ReadString(Link);
		ff.ReadString(Link);
		ff.Close();
		if( Link.GetLength() > 4 )
			Link = Link.Right( Link.GetLength()-4 );
	}
	CDlgAddMerge dlg;
	dlg.m_mode = dlg.M_URL;
	dlg.Doc = this;
	dlg.m_merge_name = Link;
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		if( dlg.m_merge_name.GetLength() )
		{
			Link = dlg.m_merge_name;
			if( ff.Open( path, CFile::modeCreate | CFile::modeWrite ) )
			{
				ff.WriteString("[InternetShortcut]\nURL=");
				ff.WriteString(Link);
				ff.Close();
			}
			SetFileAttributes( path2, 0 );
			if( ff.Open( path2, CFile::modeCreate | CFile::modeWrite ) )
			{
				ff.WriteString("[InternetShortcut]\nURL=");
				ff.WriteString(Link);
				ff.Close();
			}
		}
		else
		{
			DeleteFile( path );
			DeleteFile( path2 );
		}
	}
}

void CFreePcbDoc::OnComponentNote()
{
	ComponentNote();
}

void CFreePcbDoc::OnTXTNote()
{
	ComponentNote(2);
}

void CFreePcbDoc::OnDOCNote()
{
	ComponentNote(1);
}

void CFreePcbDoc::OnPDFNote()
{
	ComponentNote(3);
}

void CFreePcbDoc::OnURLNote()
{
	ComponentNote(4);
}

void CFreePcbDoc::ComponentNote( int MODE )
{
	CText * part_found = NULL;
	CString Parts="";
	CString Value="";
	CString Footprint="";
	int WARNING_P=0;
	int WARNING_V=0;
	int WARNING_F=0;
	int no_part_attribute=0;
	m_view->SelPolylineTest( &part_found, &Parts, &Value, &Footprint, &WARNING_P, &WARNING_V, &WARNING_F, &no_part_attribute, 100 );
	if( WARNING_V == 1 && WARNING_F == 1 )
	{
		ExtractComponentName( &Value, &Footprint );
		if( Value.GetLength() && Footprint.GetLength() )
		{
			for( int STEP=0; STEP<2; STEP++ )
			{
				CString CDir;
				if( STEP )
				{
					int rf = m_pcb_full_path.ReverseFind('\\');
					if( rf <= 0 )
						continue;
					CDir = m_pcb_full_path.Left(rf) + rel_f_component;

					// test 
					//struct _stat buf;
					//if( _stat( (m_pcb_full_path.Left(rf)+"\\related_files"), &buf ) )
					//	break;
				}
				else
					CDir = m_app_dir + main_component;
				CString CNote = CDir + "\\" + Value + "@" + Footprint;
				int err = _chdir( CDir );
				if( err )
				{
					int err = _mkdir( CDir );
					if( err )
					{
						CString str;
						str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"" : "Невозможно создать папку \"%s\"", CDir );
						AfxMessageBox( str, MB_OK );
						return;
					}
				}
				struct _stat buf;
				if( _stat( CNote+".doc", &buf ) == 0 && MODE == 1 )
				{
					ShellExecute(	NULL, "open", CNote+".doc", NULL, CDir, SW_SHOWNORMAL );
					break;
				}
				else if( _stat( CNote+".docx", &buf ) == 0 && MODE == 1 )
				{
					ShellExecute(	NULL, "open", CNote+".docx", NULL, CDir, SW_SHOWNORMAL );
					break;
				}
				else if( _stat( CNote+".txt", &buf ) == 0 && MODE == 2 )
				{
					ShellExecute(	NULL, "open", CNote+".txt", NULL, CDir, SW_SHOWNORMAL );
					break;
				}
				else if( _stat( CNote+".pdf", &buf ) == 0 && MODE == 3 )
				{
					ShellExecute(	NULL, "open", CNote+".pdf", NULL, CDir, SW_SHOWNORMAL );
					break;
				}
				else if( _stat( CNote+".url", &buf ) == 0 && MODE == 4 )
				{
					ShellExecute(	NULL, "open", CNote+".url", NULL, CDir, SW_SHOWNORMAL );
					break;
				}
				else if( MODE == 0 && STEP )
				{
					CComponentNoteExt dlg;
					int ret = dlg.DoModal();
					if( ret == IDOK )
					{
						if( dlg.m_extern )
						{
							CString fileStr = RunFileDialog( 1, "doc" );
							if( fileStr.GetLength() )
							{
								int rf = fileStr.ReverseFind('.');
								if( rf > 0 )
								{
									CString ext = fileStr.Right( fileStr.GetLength() - rf );
									dlg.ext = ext;
									if( _stat( CNote+dlg.ext, &buf ) == 0 )
									{
										if( AfxMessageBox(G_LANGUAGE == 0 ? 
											"This document already exists in the storage mirror. "\
											"Do you want to continue despite this? (If you now create a new document "\
											"with the same name in the main storage, then when you close the project, "\
											"the old document that is stored in the mirror will be replaced by "\
											"the new one) ":
											"Этот документ уже существует в зеркале хранилища. "\
											"Хотите продолжить, несмотря на это? (Если вы сейчас создадите новый документ "\
											"с тем же именем в основном хранилище, то при закрытии проекта "\
											"старый документ, хранящийся в зеркале, будет заменён "\
											"новым) ",MB_YESNO | MB_ICONWARNING) != IDYES )
											continue;
									}
									CDir = m_app_dir + main_component;
									CNote = CDir + "\\" + Value + "@" + Footprint;
									if( _stat( CNote+dlg.ext, &buf ) == 0 )
									{
										if( AfxMessageBox(G_LANGUAGE == 0 ?
											"This document already exists in the main storage. "\
											"Would you like to continue anyway and replace it "\
											"with a new file?":
											"Этот документ уже существует в основном хранилище. "\
											"Хотите ли вы продолжить в любом случае и заменить его "\
											"новым файлом?",MB_YESNO | MB_ICONWARNING) != IDYES )
											continue;
									}
									if( fileStr.Compare( CNote+ext ) )
									{
										CopyFile( fileStr, CNote+ext, 0 );
										SetFileAttributes( CNote+ext, 0 );
										dlg.ext = ext;
									}
								}
							}
						}
						else 
						{		
							if( _stat( CNote+dlg.ext, &buf ) == 0 )
							{
								if( AfxMessageBox(G_LANGUAGE == 0 ?
									"This document already exists in the storage mirror. "\
									"Do you want to continue despite this? (If you now create a new document "\
									"with the same name in the main storage, then when you close the project, "\
									"the old document that is stored in the mirror will be replaced by "\
									"the new one) ":
									"Этот документ уже существует в зеркале хранилища. "\
									"Хотите продолжить, несмотря на это? (Если вы сейчас создадите новый документ "\
									"с тем же именем в основном хранилище, то при закрытии проекта "\
									"старый документ, хранящийся в зеркале, будет заменён "\
									"новым) ",MB_YESNO | MB_ICONWARNING) != IDYES )
									continue;
							}
							CDir = m_app_dir + main_component;
							CNote = CDir + "\\" + Value + "@" + Footprint;
							if( _stat( CNote+dlg.ext, &buf ) == 0 )
							{
								if( AfxMessageBox(G_LANGUAGE == 0 ?
									"This document already exists in the main storage. "\
									"Would you like to continue anyway and replace it "\
									"with a new empty file?":
									"Этот документ уже существует в основном хранилище. "\
									"Хотите ли вы продолжить в любом случае и заменить его "\
									"новым пустым файлом?",MB_YESNO | MB_ICONWARNING) != IDYES )
									continue;
							}
							if( dlg.ext_num < 2 )	// docx, txt
							{
								//ofstream file( CNote+dlg.ext );
								//file.close();
								CStdioFile file;
								file.Open( CNote+dlg.ext, CFile::modeCreate | CFile::modeWrite, NULL );
								if( dlg.ext_num == 1 )
								{	
									file.WriteString( Value );
									file.WriteString( "\n" );
								}
								file.Close();
							}
							else					// pdf
							{
								CPDFdoc * pdf;
								pdf = cpdf_open(0, NULL);
								cpdf_init(pdf);
								cpdf_pageInit(pdf, 1, PORTRAIT, A4, A4);		/* page orientation */
								cpdf_setlinecap( pdf, 1 );	// round end-caps
								cpdf_finalizeAll(pdf);
								int err = cpdf_savePDFmemoryStreamToFile(pdf, CNote+dlg.ext );
								cpdf_close(pdf);
							}
						}
						ShellExecute(	NULL, "open", CNote+dlg.ext, NULL, CDir, SW_SHOWNORMAL );
					}
					break;
				}
			}
		}
		else
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"First enter \"value\" text to create a note file":
				"Сначала введите атрибут детали «Название», чтобы создать файл заметки.", MB_ICONERROR);
		}
	}
}

BOOL CFreePcbDoc::OnSpeedFile( UINT CMD )
{
	UINT m_file_index = CMD - ID_FILE_OPEN_FROM_START - 1;
	if( m_file_index >= m_ProjectFiles.GetSize() )
		return 0;
	FileOpen( LPCTSTR( m_ProjectFiles.GetAt(m_file_index) ) );
	return 1;
}

BOOL CFreePcbDoc::OnRangeCmds( UINT CMD )
{
	if( m_view )
	{
		m_view->m_draw_layer = 0;
		m_view->Invalidate( FALSE );//OnRangeCmds
	}
	return !CMD;
}

void CFreePcbDoc::OnAddMarks()
{
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = GetTextListbyLayer( lay );
		if( !tl )
			continue;
		int it = -1;
		for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
		{
			if( t->isVISIBLE )
				if( t->m_selected )
				{
					CString label;
					label = "Marker\r\n";
					id id1( ID_TEXT_DEF );
					id id2;
					DRError * dre = m_drelist->Add( &label, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );	
				}
		}
	}
	for( int i=0; i<m_outline_poly->GetSize(); i++ )
	{
		if( m_outline_poly->GetAt(i).m_visible == 0 )
			continue;
		for( int ii=0; ii<m_outline_poly->GetAt(i).GetNumSides(); ii++ )
			if( m_outline_poly->GetAt(i).GetSideSel(ii) )
			{
				CString label;
				label = "Marker\r\n";
				id id1( ID_POLYLINE, ID_GRAPHIC, i, ID_SIDE, ii );
				id id2;
				m_drelist->Add( &label, id1, id2, 
								m_outline_poly->GetAt(i).GetX(ii), 
								m_outline_poly->GetAt(i).GetY(ii), 
								m_outline_poly->GetAt(i).GetX(ii), 
								m_outline_poly->GetAt(i).GetY(ii), 
								20*NM_PER_MIL, LAY_DRC_ERROR, NULL );
			}
	}
	m_view->OnRangeCmds(NULL);
}

void CFreePcbDoc::OnGetPinMap()
{
	CText * part = NULL;
	if( m_view->m_sel_id.type == ID_POLYLINE )
		part = m_outline_poly->GetAt( m_view->m_sel_id.i ).Check( index_part_attr );
	else 
		return;
	///m_view->JumpToPart( part, NULL );
	if( part == NULL )
		return;
	else if( GetNumPins( part ) > 0 )
	{
		RECT r = GetPartBounds( part, NULL );
		SwellRect( &r, (r.top-r.bottom)/3 + (10*NM_PER_MM) );
		m_view->ViewAllElements(&r);
		OnRangeCmds(NULL);
	}
	else
		return;
	DlgPinMap dlg;
	dlg.Initialize( part, &Pages );
	if( dlg.DoModal() == IDOK )
	{

	}
}


int CFreePcbDoc::PinTextAlignment( CText * txt )
{
	if( txt->m_polyline_start == -1 )
		return -1;
	int ncrs = m_outline_poly->GetAt( txt->m_polyline_start ).GetNumCorners();
	if( ncrs < 2 )
		return -1;
	int xp1 = m_outline_poly->GetAt( txt->m_polyline_start ).GetX(0);
	int yp1 = m_outline_poly->GetAt( txt->m_polyline_start ).GetY(0);
	int xp2 = m_outline_poly->GetAt( txt->m_polyline_start ).GetX(ncrs-1);
	int yp2 = m_outline_poly->GetAt( txt->m_polyline_start ).GetY(ncrs-1);
	if( abs(xp1-xp2) < NM_PER_MIL && abs(yp1-yp2) < NM_PER_MIL )
		return -1;
	//
	BOOL bP1=0, bP2=0;
	if( abs(xp1-xp2) < NM_PER_MIL ) // vertical line
	{
		if( txt->m_y < yp1 && txt->m_y < yp2 ) // (for description text)
		{
			if( txt->m_angle )
			{
				if( txt->m_x < xp1 )
				{
					clrbit( txt->m_pdf_options, PDF_ALIGN );
					return 1;
				}
				else
				{
					setbit( txt->m_pdf_options, PDF_ALIGN );
					return 0;
				}
			}
			else if( txt->m_tl )
			{
				RECT R;
				if( txt->m_tl->GetTextRectOnPCB( txt, &R ) )
				{
					if( R.right < xp1 && txt->m_x < xp1 )
					{
						setbit( txt->m_pdf_options, PDF_ALIGN );
						return 1;
					}
					else if( R.right > xp1 && txt->m_x > xp1 )
					{
						clrbit( txt->m_pdf_options, PDF_ALIGN );
						return 1;
					}
				}
			}
		}
		else if( txt->m_y > yp1 && txt->m_y > yp2 ) // (for description text)
		{
			if( txt->m_angle )
			{
				if( txt->m_x < xp1 )
				{
					setbit( txt->m_pdf_options, PDF_ALIGN );
					return 1;
				}
				else
				{
					clrbit( txt->m_pdf_options, PDF_ALIGN );
					return 0;
				}
			}
			else if( txt->m_tl )
			{
				RECT R;
				if( txt->m_tl->GetTextRectOnPCB( txt, &R ) )
				{
					if( R.right < xp1 && txt->m_x < xp1 )
					{
						setbit( txt->m_pdf_options, PDF_ALIGN );
						return 1;
					}
					else if( R.right > xp1 && txt->m_x > xp1 )
					{
						clrbit( txt->m_pdf_options, PDF_ALIGN );
						return 1;
					}
				}
			}
		}
		else if( txt->m_x < xp1 )
		{
			if( txt->m_angle == 0 )
				setbit( txt->m_pdf_options, PDF_ALIGN );
			else
				clrbit( txt->m_pdf_options, PDF_ALIGN );
			return 1;
		}
		else if( txt->m_x > xp1 )
		{
			clrbit( txt->m_pdf_options, PDF_ALIGN );
			return 0;
		}
	}
	else if( abs(yp1-yp2) < NM_PER_MIL && txt->m_angle == 0 ) // horizontal line
	{
		if( txt->m_x < xp1 && txt->m_x < xp2 )
		{
			setbit( txt->m_pdf_options, PDF_ALIGN );
			return 1;
		}
		else if( txt->m_x > xp1 && txt->m_x > xp2 )
		{
			clrbit( txt->m_pdf_options, PDF_ALIGN );
			return 0;
		}
		else for( int ii=0; ii<m_outline_poly->GetSize(); ii++ )
		{
			if( m_outline_poly->GetAt(ii).GetLayer() == LAY_CONNECTION )
			{
				int poly_x1 = m_outline_poly->GetAt(ii).GetX(0);
				int poly_y1 = m_outline_poly->GetAt(ii).GetY(0);
				int ns = m_outline_poly->GetAt(ii).GetNumCorners() - 1;
				int poly_x2 = m_outline_poly->GetAt(ii).GetX(ns);
				int poly_y2 = m_outline_poly->GetAt(ii).GetY(ns);
				if( poly_x1 == xp1 && poly_y1 == yp1 )
					bP1 = TRUE;
				if( poly_x2 == xp1 && poly_y2 == yp1 )
					bP1 = TRUE;
				if( poly_x1 == xp2 && poly_y1 == yp2 )
					bP2 = TRUE;
				if( poly_x2 == xp2 && poly_y2 == yp2 )
					bP2 = TRUE;
			}
			if( bP1 || bP2 )
				break;
		}
		if( bP1 && bP2 == 0 )
		{
			if( xp1 > xp2 )
			{
				clrbit( txt->m_pdf_options, PDF_ALIGN );
				return 0;
			}
			else
			{
				setbit( txt->m_pdf_options, PDF_ALIGN );
				return 1;
			}
		}
		else if( bP2 && bP1 == 0 )
		{
			if( xp1 > xp2 )
			{
				setbit( txt->m_pdf_options, PDF_ALIGN );
				return 1;
			}
			else
			{
				clrbit( txt->m_pdf_options, PDF_ALIGN );
				return 0;
			}
		}
	}
	return -1;
}

void CFreePcbDoc::EnableMenuItems()
{
	CWnd* pMain = AfxGetMainWnd();
	if (pMain != NULL)
	{
		pMain->SetWindowText( m_window_title );
		CMenu* pMenu = pMain->GetMenu();
		pMenu->EnableMenuItem( 1, MF_BYPOSITION | MF_ENABLED ); 
		pMenu->EnableMenuItem( 2, MF_BYPOSITION | MF_ENABLED ); 
		pMenu->EnableMenuItem( 3, MF_BYPOSITION | MF_ENABLED ); 
		pMenu->EnableMenuItem( 4, MF_BYPOSITION | MF_ENABLED ); 
		pMenu->EnableMenuItem( 5, MF_BYPOSITION | MF_ENABLED ); 
		pMenu->EnableMenuItem( 6, MF_BYPOSITION | MF_ENABLED ); 
		CMenu* submenu = pMenu->GetSubMenu(0);	// "File" submenu
		submenu->EnableMenuItem( ID_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED );	
		submenu->EnableMenuItem( ID_FILE_SAVE_AS, MF_BYCOMMAND | MF_ENABLED );	
		submenu->EnableMenuItem( ID_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED );		
		submenu->EnableMenuItem( ID_FILE_EXPORTNETLIST, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_FILE_SETNETLIST, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_MAKE_ARCHIVED, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_FILE_GENERATECADFILES, MF_BYCOMMAND | MF_ENABLED );	
		submenu->EnableMenuItem( ID_FILE_PRINT, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORTFILE, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORT2, MF_BYCOMMAND | MF_ENABLED );
		if( m_project_open )
		{
			CString s0 = m_path_to_folder+"\\related_files\\reports";
			struct _stat buf;
			if( _stat( s0+"\\bom\\bom.cds", &buf ) == 0 )
			{
				submenu->EnableMenuItem( ID_FILE_GENERATEREPORT4, MF_BYCOMMAND | MF_ENABLED );
			}
			if( _stat( s0+"\\tag\\tag.cds", &buf ) == 0 )
			{
				submenu->EnableMenuItem( ID_FILE_GENERATEREPORT3, MF_BYCOMMAND | MF_ENABLED );
			}
		}
		m_view->OnHidePictures( NULL );
		submenu = pMenu->GetSubMenu(1);	// "Edit" submenu
		submenu->EnableMenuItem( ID_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED );	
		submenu->EnableMenuItem( ID_EDIT_REDO, MF_BYCOMMAND | MF_ENABLED );
		pMain->DrawMenuBar();
	}
}

void CFreePcbDoc::DisableMenuItems()
{
	// menu stuff
	CWnd* pMain = AfxGetMainWnd();
	if (pMain != NULL)
	{
		pMain->SetWindowText( m_window_title );
		GetGerberPathes("");
		CMenu* pMenu = pMain->GetMenu();
		pMenu->EnableMenuItem( 1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED ); 
		pMenu->EnableMenuItem( 2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED ); 
		pMenu->EnableMenuItem( 3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED ); 
		pMenu->EnableMenuItem( 4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED );  
		CMenu* submenu = pMenu->GetSubMenu(0);	// "File" submenu
		submenu->EnableMenuItem( ID_FILE_SAVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
		submenu->EnableMenuItem( ID_FILE_SAVE_AS, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
		submenu->EnableMenuItem( ID_FILE_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );		
		submenu->EnableMenuItem( ID_FILE_EXPORTNETLIST, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
		submenu->EnableMenuItem( ID_FILE_SETNETLIST, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_MAKE_ARCHIVED, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_FILE_GENERATECADFILES, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
		submenu->EnableMenuItem( ID_FILE_PRINT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		{
			submenu->EnableMenuItem( ID_ATTACH_BMP1, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			submenu->EnableMenuItem( ID_ATTACH_BMP2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			submenu->EnableMenuItem( ID_DETACH_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			submenu->EnableMenuItem( ID_HIDE_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			submenu->EnableMenuItem( ID_SHOW_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		}
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORTFILE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORT2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED ); 
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORT3, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED ); 
		submenu->EnableMenuItem( ID_FILE_GENERATEREPORT4, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED ); 
		submenu = pMenu->GetSubMenu(1);	// "Edit" submenu
		submenu->EnableMenuItem( ID_EDIT_UNDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );	
		submenu->EnableMenuItem( ID_EDIT_REDO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMain->DrawMenuBar();
	}
}
