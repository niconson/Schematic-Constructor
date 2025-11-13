// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "mainfrm.h"
#include "locale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFreeasyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_USER_VISIBLE_GRID, OnChangeVisibleGrid )
	ON_MESSAGE( WM_USER_PLACEMENT_GRID, OnChangePlacementGrid )
	ON_MESSAGE( WM_USER_TEXT_GRID, OnChangeTextGrid )
	ON_MESSAGE( WM_USER_SNAP_ANGLE, OnChangeSnapAngle )
	ON_MESSAGE( WM_USER_UNITS, OnChangeUnits )
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_DROPFILES, Wm__DropFiles)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	0,
	0,
	0,
	0,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bAutoMenuEnable = FALSE;
	m_bHideCursor = FALSE;
	m_bCursorHidden = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndMyToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// status bar stuff
	if (!m_wndStatusBar.Create(this ) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	// initialize pane 0 of status bar
	UINT uID, uStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo( 0, uID, uStyle, nWidth );
	m_wndStatusBar.SetPaneInfo( 0, uID, uStyle | SBARS_SIZEGRIP, 200 );

	// initialize pane 1 of status bar
	m_wndStatusBar.GetPaneInfo( 1, uID, uStyle, nWidth );
	CDC * pDC = m_wndStatusBar.GetDC();
	pDC->SelectObject( m_wndStatusBar.GetFont() );
	CRect rectArea;
	pDC->DrawText( _T("X: -9999999"), 	-1, rectArea, DT_SINGLELINE | DT_CALCRECT );
	m_wndStatusBar.ReleaseDC( pDC );
	int pane_width = rectArea.Width();
	m_wndStatusBar.SetPaneInfo( 1, uID, uStyle, pane_width );
	CString test;
	test.Format( "X: 0" );
	m_wndStatusBar.SetPaneText( 1, test );

	// initialize pane 2 of status bar
	m_wndStatusBar.GetPaneInfo( 2, uID, uStyle, nWidth );
	pDC = m_wndStatusBar.GetDC();
	pDC->SelectObject( m_wndStatusBar.GetFont() );
	pDC->DrawText( _T("Y: -9999999"), -1, rectArea, DT_SINGLELINE | DT_CALCRECT );
	m_wndStatusBar.ReleaseDC( pDC );
	pane_width = rectArea.Width();
	m_wndStatusBar.SetPaneInfo( 2, uID, uStyle, pane_width );
	test.Format( "Y: 0" );
	m_wndStatusBar.SetPaneText( 2, test );

	// initialize pane 3 of status bar
	m_wndStatusBar.GetPaneInfo( 3, uID, uStyle, nWidth );
	pDC = m_wndStatusBar.GetDC();
	pDC->SelectObject( m_wndStatusBar.GetFont() );
	pDC->DrawText( _T("********************************************STATUS STRING**************************************************"), 
						-1, rectArea, DT_SINGLELINE | DT_CALCRECT );
	m_wndStatusBar.ReleaseDC( pDC );
	pane_width = rectArea.Width();
	m_wndStatusBar.SetPaneInfo( 3, uID, uStyle, pane_width );
	test.Format( "hello" );
	m_wndStatusBar.SetPaneText( 3, test );

	// initialize pane 4 of status bar
	m_wndStatusBar.GetPaneInfo( 4, uID, uStyle, nWidth );
	pDC = m_wndStatusBar.GetDC();
	pDC->SelectObject( m_wndStatusBar.GetFont() );
	pDC->DrawText( _T("BottomXXX"), -1, rectArea, DT_SINGLELINE | DT_CALCRECT );
	m_wndStatusBar.ReleaseDC( pDC );
	pane_width = rectArea.Width();
	m_wndStatusBar.SetPaneInfo( 4, uID, uStyle, pane_width );
	test.Format( "hello" );
	m_wndStatusBar.SetPaneText( 4, test );

	
	// create timer event every TIMER_PERIOD seconds
	m_timer = SetTimer( TMR_AUTOSAVE, TIMER_PERIOD*1000, 0 );
	m_timer = SetTimer( TMR_FK_PRESS, INT_MAX, 0 );
	m_timer = SetTimer( TMR_SW_PCB, INT_MAX, 0 );
	m_timer = SetTimer( TMR_SW_CDS, INT_MAX, 0 );
	m_timer = SetTimer( TMR_DRC, 500, 0 );

	// separator
	// DecimalSeparator;

	// cursor
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	DragAcceptFiles();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Read Registry settings for window placement.
	cs.x = (int)theApp.GetProfileInt(_T("Settings"),_T("LeftWinCoord"),cs.x);
	cs.y = (int)theApp.GetProfileInt(_T("Settings"),_T("TopWinCoord"),cs.y);
	cs.cx = (int)theApp.GetProfileInt(_T("Settings"),_T("WinWidth"),cs.cx);
	cs.cy = (int)theApp.GetProfileInt(_T("Settings"),_T("WinHeight"),cs.cy);

	return CFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::DrawStatus( int pane, CString * str )
{
	m_wndStatusBar.SetPaneText( pane, *str );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LONG CMainFrame::OnChangeVisibleGrid( UINT wp, LONG lp )
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		((CFreePcbView*)m_view)->OnChangeVisibleGrid( wp, lp );
	return 0;
}

LONG CMainFrame::OnChangePlacementGrid( UINT wp, LONG lp )
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		((CFreePcbView*)m_view)->OnChangePlacementGrid( wp, lp );
	return 0;
}

LONG CMainFrame::OnChangeTextGrid( UINT wp, LONG lp )
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		((CFreePcbView*)m_view)->OnChangeTextGrid( wp, lp );
	return 0;
}

LONG CMainFrame::OnChangeSnapAngle( UINT wp, LONG lp )
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		((CFreePcbView*)m_view)->OnChangeSnapAngle( wp, lp );
	return 0;
}

LONG CMainFrame::OnChangeUnits( UINT wp, LONG lp )
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		((CFreePcbView*)m_view)->OnChangeUnits( wp, lp );
	return 0;
}

LRESULT CMainFrame::Wm__DropFiles( WPARAM Message, LPARAM qwerty )
{
	CString FName ;
	char buff[MAX_PATH] ;
	HDROP hDrop = (HDROP)Message ;
	int nFiles = DragQueryFile(hDrop, -1, NULL, NULL) ;
	for(int i=0; i<nFiles; i++) 
	{ 
		DragQueryFile(hDrop, i, buff, sizeof(buff)) ; 
		FName = buff ; 
		m_view = (CView*)GetActiveView();
		if( (CFreePcbView*)m_view )
		{
			((CFreePcbView*)m_view)->m_Doc->OnFileAutoOpen( FName );
			break;
		}
	}
	DragFinish(hDrop) ;
	return 0;
}

int CMainFrame::OnCopyData(CWnd * pWnd, COPYDATASTRUCT * Msg) 
{
	m_view = (CView*)GetActiveView();
	CFreePcbView * view = (CFreePcbView*)m_view;
	if( view )
		if( view->CurDragging() )
			return 0;

	enum{C_JUMP=0, C_PARTS, C_PIN, C_REOPEN, C_FILE, C_VDP, C_SEL_M};
	int np, mode=-1;
	int bSaveScale = 0;
	CArray<CString> p;
	p.SetSize( 0 );
	CString Mess, sz;
	Mess = (char*)Msg->lpData;
	if( Mess.Left(1) == "*" )
	{
		Mess = Mess.Right( Mess.GetLength() - 1 );
		bSaveScale = TRUE;
	}
	CString key_str;
	if (Mess.Left(4) == "jump")
	{
		key_str = "jump";
		mode = C_JUMP;
	}
	else if (Mess.Left(5) == "parts")
	{
		key_str = "parts";
		mode = C_PARTS;
	}
	else if (Mess.Left(3) == "pin")
	{
		key_str = "pin";
		mode = C_PIN;
	}
	else if (Mess.Left(6) == "reopen")
	{
		key_str = "reopen";
		mode = C_REOPEN;
	}
	else if(Mess.Left(8) == "pcb_file")
	{
		key_str = "file";
		mode = C_FILE;
	}
	else if (Mess.Left(15) == "valuedogpackage")
	{
		key_str = "valuedogpackage";
		mode = C_VDP;
	}
	else if (Mess.Left(12) == "select_merge")
	{
		key_str = "select_merge";
		mode = C_SEL_M;
	}
	//
	if (mode >= 0)
	{
		np = ParseKeyString( &Mess, &key_str, &p );
		if( (CFreePcbView*)m_view )
		{
			if( mode == C_SEL_M )
			{
				int imer = theApp.m_Doc->m_mlist->GetIndex( p.GetAt(0) );
				int ipage = theApp.m_Doc->Pages.FindMerge( imer );
				if( ipage >= 0 )
				{
					theApp.m_Doc->SwitchToPage( ipage, TRUE );
				}
			}
			((CFreePcbView*)m_view)->OnMess(mode,np,&p,bSaveScale);
		}
	}
	return 0;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	m_view = (CView*)GetActiveView();
	if( nID == SC_CLOSE )
	{
		CFreePcbDoc * doc = (CFreePcbDoc*)GetActiveDocument();
		if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		{
			if( doc )
			{
				if( doc->FileClose() == IDCANCEL )
					return;
			}
		}
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnEditUndo()
{
	m_view = (CView*)GetActiveView();
	CFreePcbDoc * doc = (CFreePcbDoc*)GetActiveDocument();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
		doc->OnEditUndo();
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	m_view = (CView*)GetActiveView();
	if( m_view->IsKindOf( RUNTIME_CLASS(CFreePcbView) ) )
	{
		CFreePcbDoc * doc = (CFreePcbDoc*)GetActiveDocument();
		if( doc )
		if( doc->m_project_open )
		{
			if( nIDEvent == TMR_AUTOSAVE )
			{
				if( doc->m_project_modified_since_autosave && 
					doc->m_view->m_cursor_mode == CUR_NONE_SELECTED )
				{
					doc->m_auto_elapsed += TIMER_PERIOD;
					if( m_view && doc->m_auto_interval && doc->m_auto_elapsed > doc->m_auto_interval )
					{
						if( !doc->m_view->CurDragging() )
							doc->AutoSave();
					}
				}
			}
			else if( nIDEvent == TMR_FK_PRESS )
			{
				doc->m_view->m_fk_button_index = -1;
				doc->m_view->m_left_pane_invalid = -1;
				doc->m_view->DrawBottomPane();
				SetTimer( TMR_FK_PRESS, INT_MAX, 0 );
			}
			else if( nIDEvent == TMR_SW_PCB )
			{
				doc->SwitchToPCB(TRUE,FALSE);
			}
			else if( nIDEvent == TMR_SW_CDS )
			{
				doc->SwitchToCDS();
			}
			else if( nIDEvent == TMR_SW_CDSM )
			{
				doc->SwitchToCDS( NULL,NULL,TRUE );
			}
			else if( nIDEvent == TMR_SW_STATIC )
			{
				static int counter = 0;
				if( doc->last_el )
				{
					counter++;
					if( counter%2 )
						doc->last_el->visible = FALSE;
					else
						doc->last_el->visible = TRUE;
					if( counter > 8 )
					{
						counter = 0;
						doc->last_el->dlist->Remove( doc->last_el );
						doc->last_el = NULL;
					}
					///doc->m_view->m_draw_layer = LAY_BACKGND;
					doc->m_view->Invalidate( FALSE );
					///doc->OnRangeCmds(NULL);
				}
				else
					SetTimer( TMR_SW_STATIC, INT_MAX, 0 );
			}
			else if (nIDEvent == TMR_DRC)
			{
				static bool state = 0;
				if (doc->m_drelist->list.GetSize())
				{
					if (state)
						doc->m_drelist->MakeSolidCircles();
					else
						doc->m_drelist->MakeHollowCircles();
					state = !state;
					doc->m_view->m_draw_layer = LAY_DRC_ERROR;
					doc->m_view->Invalidate(FALSE);
				}
			}
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if( !m_bHideCursor )
	{
		// cursor hiding disabled
		if(!m_bCursorHidden )
		{
			return( CFrameWnd::OnSetCursor(pWnd, nHitTest, message) );
		}
		else
		{
			::ShowCursor( TRUE );
			m_bCursorHidden = FALSE;
		}
	}
	else  
	{
		// cursor hiding enabled
		CPoint cur_pt;
		GetCursorPos( &cur_pt );
		if( m_hide_cursor_rect.PtInRect(cur_pt) )
		{
			if( !m_bCursorHidden )
			{
				::ShowCursor( FALSE );
				m_bCursorHidden = TRUE;
				return TRUE;
			}
		}
		else
		{
			if( m_bCursorHidden )
			{
				::ShowCursor( TRUE );
				m_bCursorHidden = FALSE;
				return TRUE;
			}
		}
	}
	return TRUE;
}

// on moving window, get client area in screen coords
//
void CMainFrame::OnMove(int x, int y)
{
	CPoint new_client_topleft_pt(x,y+24);
	m_hide_cursor_rect.OffsetRect( new_client_topleft_pt - m_client_rect.TopLeft() );
	m_client_rect.MoveToXY( new_client_topleft_pt );
	CFrameWnd::OnMove(x, y);
}

// on resizing window, get client area in screen coords
//
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	int new_client_width = cx;
	int new_client_height = cy - 48;
	int dx = new_client_width - m_client_rect.Width();
	int dy = new_client_height - m_client_rect.Height();
	m_client_rect.right += dx;
	m_client_rect.bottom += dy;
	m_hide_cursor_rect.right += dx;
	m_hide_cursor_rect.bottom += dy;
	/*CRect wr;
	GetWindowRect(&wr);
	int pane_width = wr.right - wr.left - 150;
	UINT uID, uStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo( 0, uID, uStyle, nWidth );
	m_wndStatusBar.SetPaneInfo( 3, uID, uStyle, pane_width );*/
	CFrameWnd::OnSize(nType, cx, cy);
}

// enable and set a screen rectangle for hiding the cursor, or disable
//
void CMainFrame::SetHideCursor( BOOL bHideEnable, CRect * screen_rect )
{
	if( bHideEnable && screen_rect )
		m_hide_cursor_rect = screen_rect;
	m_bHideCursor = bHideEnable;
}

BOOL CMainFrame::DestroyWindow()
{
	// Read current WINDOWPLACEMENT value
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);
	// write settings to registry
	theApp.WriteProfileInt(_T("Settings"),_T("LeftWinCoord"),(int)wpl.rcNormalPosition.left);
	theApp.WriteProfileInt(_T("Settings"),_T("TopWinCoord"),(int)wpl.rcNormalPosition.top);
	theApp.WriteProfileInt(_T("Settings"),_T("WinWidth"),((int)wpl.rcNormalPosition.right-(int)wpl.rcNormalPosition.left));
	theApp.WriteProfileInt(_T("Settings"),_T("WinHeight"),((int)wpl.rcNormalPosition.bottom-(int)wpl.rcNormalPosition.top));
	theApp.WriteProfileInt(_T("Settings"),_T("ShowCmd"),wpl.showCmd);

	return CWnd::DestroyWindow();
}

