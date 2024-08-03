// FreePcb.cpp : Defines the class behaviors for the application. 
//

#include "stdafx.h"
#include "freeSch.h"
#include "resource.h"
#include "DlgShortcuts.h"
#include "afxwin.h"

#include ".\freeSch.h"
//#include "QAFDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;  
#endif

/////////////////////////////////////////////////////////////////////////////
// CFreeasyApp

BEGIN_MESSAGE_MAP(CFreeasyApp, CWinApp)
	//{{AFX_MSG_MAP(CFreeasyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_HELP_GOTOWEBSITE, OnHelpGotoWebsite)
	ON_COMMAND(ID_FILE_MRU_FILE1, OnFileMruFile1)
	ON_COMMAND(ID_FILE_MRU_FILE2, OnFileMruFile2)
	ON_COMMAND(ID_FILE_MRU_FILE3, OnFileMruFile3)
	ON_COMMAND(ID_FILE_MRU_FILE4, OnFileMruFile4)
	ON_COMMAND(ID_FILE_MRU_FILE5, OnFileMruFile5)
	ON_COMMAND(ID_FILE_MRU_FILE6, OnFileMruFile6)
	ON_COMMAND(ID_FILE_MRU_FILE7, OnFileMruFile7)
	ON_COMMAND(ID_HELP_KEYBOARDSHORTCUTS, OnHelpKeyboardshortcuts)
	ON_COMMAND(ID_HELP_USERGUIDE_PDF, OnHelpUserGuidePdf)
	ON_COMMAND(ID_HELP_USERGUIDE_PDF2, OnHelpUserGuidePdf2)
	ON_COMMAND(ID_HELP_USERGUIDE_PDF3, OnHelpUserGuidePdf3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreeasyApp construction

CFreeasyApp::CFreeasyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFreeasyApp object

CFreeasyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFreeasyApp initialization

BOOL CFreeasyApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("eebit3"));

	CWinApp::LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	if( CWinApp::m_pRecentFileList == NULL)
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"NOTE: The recent file list is disabled on your system.\nUse the system policy editor to re-enable.":
			"ПРИМЕЧАНИЕ. Список последних файлов отключен в вашей системе.\nИспользуйте редактор системной политики для повторного включения.");
	}
	//else
	//	CWinApp::m_pRecentFileList->m_nSize = _AFX_MRU_COUNT;
	EnableShellOpen();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	m_pDocTemplate = new CSingleDocTemplate(//the app
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFreePcbDoc),
		RUNTIME_CLASS(CMainFrame),    
		RUNTIME_CLASS(CFreePcbView));
	AddDocTemplate(m_pDocTemplate);

	// load menus
	VERIFY( m_main.LoadMenu( IDR_MAINFRAME ) );
	VERIFY( m_main_drag.LoadMenu( IDR_MAINFRAME_DRAG ) );
	VERIFY( m_foot.LoadMenu( IDR_FOOTPRINT ) );
	VERIFY( m_foot_drag.LoadMenu( IDR_FOOTPRINT_DRAG ) );

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The window has been initialized, so show and update it.
	UINT iShowCmd = GetProfileInt(_T("Settings"),_T("ShowCmd"),SW_SHOW);
	m_pMainWnd->ShowWindow(iShowCmd);
	m_pMainWnd->UpdateWindow();

	// set pointers to document and view
	CMainFrame * pMainWnd = (CMainFrame*)AfxGetMainWnd();
	m_Doc = (CFreePcbDoc*)pMainWnd->GetActiveDocument();
	m_View = (CFreePcbView*)pMainWnd->GetActiveView();
	m_View->InitInstance();
	//
	// set initial view mode
	m_view_mode = PCB;
	//
	m_Doc->InitializeNewProject();
	//
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen )
	{
		CString fn = cmdInfo.m_strFileName;
		m_Doc->OnFileAutoOpen( fn );
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if( !pDX->m_bSaveAndValidate )
	{

	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFreeasyApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



// switch to a new CView
//
BOOL CFreeasyApp::SwitchToView( CRuntimeClass * pNewViewClass )
{
	// I have no idea what most of this code actually does, but it seems to work
	CFrameWnd * pMainWnd = (CFrameWnd*)AfxGetMainWnd();
	CView * pOldActiveView = pMainWnd->GetActiveView();
	if( pOldActiveView->IsKindOf( pNewViewClass ) )
		return TRUE;
	CView * pNewView;
	if( pNewViewClass != RUNTIME_CLASS( CFreePcbView ) )
	{
		// switch to footprint view
		CCreateContext context;
		context.m_pNewViewClass = pNewViewClass;
		context.m_pCurrentDoc = m_Doc;
		pNewView = STATIC_DOWNCAST(CView, pMainWnd->CreateView(&context));
	}
	else
	{
		// switch to pcb view
		pNewView = m_View;
	}
	if( pNewView )
	{
#if 0
		CMenu m_NewMenu;
		if( pNewViewClass == RUNTIME_CLASS( CFreePcbView ) )
			m_NewMenu.LoadMenu(IDR_MAINFRAME);
		else
			m_NewMenu.LoadMenu(IDR_FOOTPRINT);
		ASSERT(m_NewMenu);
		// Add the new menu
		pMainWnd->SetMenu(&m_NewMenu);
		m_NewMenu.Detach();
#endif
		if( pNewViewClass == RUNTIME_CLASS( CFreePcbView ) )
			pMainWnd->SetMenu(&m_main);
		else
			pMainWnd->SetMenu(&m_foot);

		// Exchange view window IDs so RecalcLayout() works.
		UINT temp = ::GetWindowLong(pOldActiveView->m_hWnd, GWL_ID);
		::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
		::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
		// SetActiveView
		pMainWnd->SetActiveView( pNewView );
		pOldActiveView->ShowWindow( SW_HIDE );
		pNewView->ShowWindow( SW_SHOW );	
		pNewView->OnInitialUpdate();
		pMainWnd->RecalcLayout();
		if( pNewViewClass == RUNTIME_CLASS( CFreePcbView ) )
		{
			// destroy old footprint view
			pOldActiveView->DestroyWindow();
			if( !m_Doc->m_project_open )
			{
				m_Doc->m_project_modified = FALSE;
				m_Doc->m_project_modified_since_autosave = FALSE;
				m_Doc->OnFileClose();	
			}
			// restore toolbar stuff
			CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
			frm->m_wndMyToolBar.SetLists( &m_Doc->m_visible_grid, &m_Doc->m_part_grid, &m_Doc->m_routing_grid,
				m_Doc->m_visual_grid_spacing, m_Doc->m_part_grid_spacing, m_Doc->m_routing_grid_spacing, 
				m_Doc->m_snap_angle, m_Doc->m_units );
			m_View->m_Doc->m_dlist->SetVisibleGrid( 1, m_Doc->m_visual_grid_spacing/m_View->m_user_scale );
			frm->SetWindowText( m_Doc->m_window_title ); 
			m_View->ShowSelectStatus();
			m_View->ShowActiveLayer(m_Doc->m_num_additional_layers);
			m_Doc->m_edit_footprint = FALSE;	// clear this flag for next time
		}
		// resize window in case it changed
		CRect client_rect;
		pMainWnd->GetClientRect( client_rect );
		// TODO: replace these constants
		client_rect.top += 24;		// leave room for toolbar
		client_rect.bottom -= 18;	// leave room for status bar
		pNewView->MoveWindow( client_rect, 1 );
		//
		return TRUE;
	}
	return FALSE;
}

int CFreeasyApp::ExitInstance()
{
	return( CWinApp::ExitInstance() );
}

void CFreeasyApp::OnHelpGotoWebsite()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = "https://github.com/niconson";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreeasyApp::OnFileMruFile1() 
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[0];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile2()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[1];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile3()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[2];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile4()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[3];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile5()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[4];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile6()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[5];
	m_Doc->OnFileAutoOpen( str );
	return;
}

void CFreeasyApp::OnFileMruFile7()
{
	ASSERT( CWinApp::m_pRecentFileList );
	CString str = (*CWinApp::m_pRecentFileList)[6];
	m_Doc->OnFileAutoOpen( str );
	return;
}

BOOL CFreeasyApp::OnOpenRecentFile(UINT nID)
{
	return( CWinApp::OnOpenRecentFile( nID ) );
}

CString CFreeasyApp::GetMRUFile()
{
	CRecentFileList * pRecentFileList = CWinApp::m_pRecentFileList;
	if( pRecentFileList == NULL )
		return "";
	if( pRecentFileList->GetSize() == 0 )
		return "";
	CString str = (*CWinApp::m_pRecentFileList)[0];
	return str;
}

void CFreeasyApp::AddMRUFile( CString * str )
{
	CRecentFileList * pRecentFileList = CWinApp::m_pRecentFileList;
	if( m_pRecentFileList == NULL )
		return;
	(*CWinApp::m_pRecentFileList).Add( *str );
}

void CFreeasyApp::OnHelpKeyboardshortcuts()
{
	CDlgShortcuts dlg = new CDlgShortcuts;//new019
	dlg.DoModal();
	delete dlg;//new019
}


void CFreeasyApp::OnHelpUserGuidePdf()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_Doc->m_app_dir + "\\doc\\Schematic_Constructor.pdf";
	if(G_LANGUAGE)
		fn = m_Doc->m_app_dir + "\\doc\\СхемАтор.pdf";
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreeasyApp::OnHelpUserGuidePdf2()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_Doc->m_app_dir + "\\doc\\Schematic_Constructor.ru.en.pdf";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

void CFreeasyApp::OnHelpUserGuidePdf3()
{
    SHELLEXECUTEINFO ShExecInfo;
	CString fn = m_Doc->m_app_dir + "\\doc\\Schematic_Constructor.ru.sp.pdf";

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = NULL;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fn;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_MAXIMIZE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);
}

int CFreeasyApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	// show cursor
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	::ShowCursor( TRUE );
	int ret = CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
	::ShowCursor( FALSE );
	return ret;
}

