// FreePcbView.cpp : implementation of the CFreePcbView class
//

#include "stdafx.h"
#include "DlgAddText.h"
#include "DlgSetSegmentWidth.h"
#include "DlgEditBoardCorner.h"
#include "DlgRefText.h"
#include "MyToolBar.h"
#include <Mmsystem.h>
#include <sys/timeb.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "freepcbview.h"
#include "DlgAddMerge.h"
#include "DlgSetAreaHatch.h"
#include "DlgDupFootprintName.h" 
#include "DlgFindPart.h"
#include "DlgImportPart.h"
#include "DlgAddMaskCutout.h"   
#include "DlgChangeLayer.h"
#include "DlgMoveOrigin.h"
#include "DlgMyMessageBox.h"
#include "DlgAttr.h"
#include "DlgSideStyle.h"
#include "DlgAddPoly.h"
#include "DlgReplaceText.h"
#include "DlgImportMerge.h"
#include "Documents.h"
#include "SpeedFiles.h"
#include "PartTemplate.h"
#include "ReadFootprintFolder.h"
#include "RTcall.h"

// globals
extern CFreeasyApp theApp;
BOOL t_pressed = FALSE;
BOOL n_pressed = FALSE;
int gTotalArrowMoveX = 0;
int gTotalArrowMoveY = 0;
BOOL gShiftKeyDown = FALSE;
BOOL gLastKeyWasArrow = FALSE;
int prev_sel_count = 0;
long long groupAverageX=0, groupAverageY=0;
int groupNumberItems=0;

HCURSOR my_cursor = LoadCursor( NULL, IDC_CROSS );

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ZOOM_RATIO 1.2
#define k_align 0.8

// constants for function key menu
#define FKEY_OFFSET_X 116
#define FKEY_OFFSET_Y 4
#define	FKEY_R_W 70
#define FKEY_R_H 30
#define FKEY_STEP (FKEY_R_W+5)
#define FKEY_GAP 20
#define FKEY_SEP_W 16

// constants for drag stub mode
#define DISABLE_BRANCH		0
#define BRANCH_TO_VERTEX	1
#define BRANCH_TO_LINE		2

// constants for layer list
#define VSTEP 14

// macro for approximating angles to 1 degree accuracy
#define APPROX(angle,ref) ((angle > ref-M_PI/360) && (angle < ref+M_PI/360))

// these must be changed if context menu is edited
enum {
	CONTEXT_NONE = 0,
	CONTEXT_TEXT,
	CONTEXT_OP_CORNER,
	CONTEXT_OP_SIDE,
	CONTEXT_GROUP,
	CONTEXT_TXTGROUP,
	CONTEXT_PROTECT,
	CONTEXT_BOM_ATTACH
};

/////////////////////////////////////////////////////////////////////////////
// CFreePcbView

IMPLEMENT_DYNCREATE(CFreePcbView, CView)

BEGIN_MESSAGE_MAP(CFreePcbView, CView)
	//{{AFX_MSG_MAP(CFreePcbView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	
ON_COMMAND(ID_TEXT_MOVE, OnTextMove)
ON_COMMAND(ID_REMOVEMERGE, OnPartRemoveMerge)
ON_COMMAND(ID_TEXT_GEN_CSV, OnTextCSV)
ON_COMMAND(ID_TEXT_EDIT, OnTextEdit)
ON_COMMAND(ID_TEXT_ALIGN_L, OnTextAlignL)
ON_COMMAND(ID_TEXT_ALIGN_R, OnTextAlignR)
ON_COMMAND(ID_TEXT_ALIGN_T, OnTextAlignT)
ON_COMMAND(ID_TEXT_ALIGN_B, OnTextAlignB)
ON_COMMAND(ID_TEXT_ALIGN_RV, OnTextAlignRV)
ON_COMMAND(ID_TEXT_ALIGN_RF, OnTextAlignRF)
ON_COMMAND(ID_TEXT_ALIGN_RVF, OnTextAlignRVF)
ON_COMMAND(ID_TEXT_ALIGN_RFV, OnTextAlignRFV)
ON_COMMAND(ID_CHANGE_VALUE,OnTextChangeV)
ON_COMMAND(ID_CHANGE_FOOT,OnTextChangeF)
ON_COMMAND(ID_REPLACE_TEXT,OnTextReplace)
ON_COMMAND(ID_ADD_POLYLINE, OnAddOutlinePoly)
ON_COMMAND(ID_ADD_CUTOUT, OnAddCutout)
ON_COMMAND(ID_SET_PRINT_AREA, OnSetPrintArea)
ON_COMMAND(ID_OP_CORNER_MOVE, OnOPCornerMove)
ON_COMMAND(ID_OP_CORNER_EDIT, OnOPCornerEdit)
ON_COMMAND(ID_OP_CORNER_DELETECORNER, OnOPCornerDelete)
ON_COMMAND(ID_OP_SIDE_INSERTCORNER, OnOPSideAddCorner)
ON_COMMAND(ID_OP_SIDE_ATTR, OnOPSetAttributes)
ON_MESSAGE( WM_USER_VISIBLE_GRID, OnChangeVisibleGrid )
ON_WM_ERASEBKGND()
ON_COMMAND(ID_OP_SIDE_CONVERTTOSTRAIGHTLINE, OnOPSideConvertToStraightLine)
ON_COMMAND(ID_OP_SIDE_CONVERTTOARC_CW, OnOPSideConvertToArcCw)
ON_COMMAND(ID_OP_SIDE_CONVERTTOARC_CCW, OnOPSideConvertToArcCcw)
ON_COMMAND(ID_VIEW_ALLELEMENTS, OnViewAllElements)
ON_COMMAND(ID_VIEW_MERGES, OnViewMerges)
ON_WM_KEYUP()
ON_COMMAND(ID_TOOLS_PART_CORR2, OnReplacePartPattern)
ON_COMMAND(ID_VIEW_FINDPART, OnViewFindAttr)
ON_COMMAND(ID_VIEW_FINDSEL, OnViewFindAmoungSel)
ON_COMMAND(ID_VIEW_FINDHID, OnViewFindAmoungHid)
ON_COMMAND(ID_PART_PASTEFROMFILE, OnPasteSinglePart)
ON_COMMAND(ID_PARTS_PASTEFROMFILE, OnPastePartRow)
ON_COMMAND(ID_NONE_DRC, OnDrc)
ON_COMMAND(ID_NONE_CLEARDRCERRORS, OnClearDRC)
ON_COMMAND(ID_NONE_VIEWALL, OnViewAllElements)
ON_COMMAND(ID_NONE_SELECTTEXTSBYLABELS, OnViewSelTextsByLabels)
ON_COMMAND(ID_TOOLS_MOVEORIGIN, OnToolsMoveOrigin)
ON_WM_LBUTTONUP()
ON_COMMAND(ID_GROUP_MOVE, OnGroupMove)
ON_COMMAND(ID_GROUP_SAVETOFILE, OnGroupSaveToFile)
ON_COMMAND(ID_GROUP_SAVETOOPENSCAD, OnGroupSaveToOpenscadFile)
ON_COMMAND(ID_GROUP_SAVETODXF, OnGroupSaveToDXFFile)
ON_COMMAND(ID_GROUP_SAVETOFPC, OnGroupSaveToFPCFile)
ON_COMMAND(ID_ADD_G_ORIGIN, OnAddGroupOrigin)
ON_COMMAND(ID_RECT_FROM_SEL, OnAddGroupRect)
ON_COMMAND(ID_STATIC_HIGHLIGHT, OnGroupStaticHighlight)
ON_COMMAND(ID_CANCEL_HIGHLIGHT, OnGroupCancelHighlight)
ON_COMMAND(ID_GLOBAL_CANCEL_SEL, OnGroupCancelSel)
ON_COMMAND(ID_APPROXIMATION_ARC, OnApproximationArc)
ON_COMMAND(ID_GROUP_SELECT_BY_ATTR, OnSelectPartsByAttr)
ON_COMMAND(ID_GROUP_SELECT_BY_ATTR2, OnSelectPolyByAttr)
ON_COMMAND(ID_GROUP_SELECT_BY_ATTR3, OnSelectObjectByAttr)
ON_COMMAND(ID_GROUP_COPY, OnGroupCopy)
ON_COMMAND(ID_GROUP_CUT, OnGroupCut)
ON_COMMAND(ID_GROUP_DELETE, OnGroupDelete)
ON_COMMAND(ID_GROUP_LINE_UP, OnGroupLineUp)
ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
ON_COMMAND(ID_EDIT_CUT, OnEditCut)
ON_COMMAND(ID_EDIT_SAVEGROUPTOFILE, OnGroupSaveToFile)
ON_COMMAND(ID_GROUP_ROTATE, OnGroupRotate)
ON_COMMAND(ID_TEXT_SIMILAR, OnTextSimilar)
ON_COMMAND(ID_TEXT_GRAB_PART, OnTextGrabPart)
ON_WM_SETCURSOR()
ON_WM_MOVE()
ON_COMMAND(ID_TOOLS_SETORIGIN, OnSetOriginToSelectedItem)
ON_COMMAND(ID_MERGE_PASTEFROMFILE, OnImportMerges)
ON_COMMAND_EX_RANGE(ID_6_PINS,ID_256_PINS, OnCreateMultiPinPartTemplate)
ON_COMMAND_EX_RANGE(ID_SEL_POLYLINES, ID_UNSEL_BACKLAYER14, OnEditSelectLayer)
ON_COMMAND_EX_RANGE(ID_HIDE_BMP, ID_SHOW_BMP, OnHidePictures)
ON_COMMAND_EX_RANGE(ID_SCALE12,ID_SCALE54, OnApplyScale)
ON_COMMAND_EX_RANGE(ID_SLIDING_ANGLE,ID_SLIDING_ANGLE2, OnSetSlidingAngle)
END_MESSAGE_MAP()


//===============================================================================================
/////////////////////////////////////////////////////////////////////////////
// CFreePcbView construction/destruction

CFreePcbView::CFreePcbView()
{
	// GetDocument() is not available at this point
	m_small_font.CreateFont( 14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial" );
#if 0
	m_small_font.CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" );
#endif

	m_Doc = NULL;
	m_last_mouse_point.x = 0;
	m_last_mouse_point.y = 0;
	m_last_cursor_point.x = 0;
	m_last_cursor_point.y = 0;
	m_left_pane_w = FKEY_OFFSET_X;	// the left pane on screen is this wide (pixels)
	m_bottom_pane_h = 40;	// the bottom pane on screen is this high (pixels)
	m_botDC_created = FALSE;
	m_topDC_created = FALSE;
	m_dragging_new_item = FALSE;
	m_bDraggingRect = FALSE;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	CalibrateTimer();
	m_dlg_partlist = NULL;
	m_dlg_compare = NULL;
 }
//===============================================================================================
// initialize the view object
// this code can't be placed in the constructor, because it depends on document
// don't try to draw window until this function has been called
// need only be called once
//
void CFreePcbView::InitInstance()
{
	// this should be called from InitInstance function of CApp,
	// after the document is created
	m_object = 0;
	m_Doc = GetDocument();
	ASSERT_VALID(m_Doc);
	m_Doc->m_view = this;
	m_Doc->m_edit_footprint = FALSE;
	InitializeView();
	// remove all pages
	m_sel_text = NULL;
	m_sel_id.Set(0);
	m_cursor_mode = 0;
	m_Doc->Pages.Assignment(	&m_Doc->m_outline_poly, 
						&m_Doc->Attr, 
						&m_Doc->m_tlist,
						&m_Doc->m_dlist, 
						&m_Doc->m_drelist, 
						&m_Doc->m_undo_list, 
						&m_Doc->m_redo_list,
						&m_sel_text, 
						&m_sel_id, 
						&m_cursor_mode, 
						MAX_PAGES, m_Doc->m_smfontutil );
	m_Doc->Pages.ClearAll();
	m_Doc->Pages.RemoveAll();
	CString PG = "Page 1";
	m_Doc->Pages.AddNew(&PG);
	m_Doc->SwitchToPage(0);
	//
	CRect screen_r;
	GetWindowRect( &screen_r );
	m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h,
		m_pcbu_per_pixel, m_org_x, m_org_y );
	for(int i=0; i<m_Doc->m_num_layers; i++ )
		m_Doc->m_dlist->SetLayerRGB( i, m_Doc->m_rgb[i][0], m_Doc->m_rgb[i][1], m_Doc->m_rgb[i][2] );
	ShowSelectStatus();
	ShowActiveLayer(MAX_LAYERS-LAY_ADD_1);
	m_polyline_width = abs(m_Doc->m_polyline_w);
	// set up array of mask ids
	m_mask_id[SEL_MASK_TEXT].Set( ID_TEXT );
	m_mask_id[SEL_MASK_OP_CORNERS].Set( ID_POLYLINE, ID_GRAPHIC, m_mask_id[SEL_MASK_OP_CORNERS].def, ID_CORNER );
	m_mask_id[SEL_MASK_OP_SIDES].Set( ID_POLYLINE, ID_GRAPHIC, m_mask_id[SEL_MASK_OP_SIDES].def, ID_SIDE );
	m_mask_id[SEL_MASK_DRC].Set( ID_DRC );

	ClearSpeedFiles( m_Doc );
}
//===============================================================================================
// initialize view with defaults for a new project
// should be called each time a new project is created
//
void CFreePcbView::InitializeView()
{
	//if( !m_Doc->m_dlist )
	//	ASSERT(0);
	// first set mode
	m_dragging_mode = 0;
	// set defaults
	m_prev_sel_merge = -1;
	SetCursorMode( CUR_NONE_SELECTED );
	m_sel_id.Clear();
	m_sel_layer = 0;
	m_page_copy = -1;
	m_dragging_new_item = 0;
	m_active_layer = LAY_FREE_LINE;
	m_draw_layer = 0;// InitializeView
	m_bDraggingRect = FALSE;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_sel_mask = 0xffff;
	SetSelMaskArray( m_sel_mask );
	m_inflection_mode = IM_90_45;
	m_snap_mode = SM_GRID_POINTS;	
	m_magnetize = 0;
	m_page = 1;
	// default screen coords in world units (i.e. display units)
	m_pcbu_per_pixel = 5.0*PCBU_PER_MIL;	// 5 mils per pixel
	m_org_x = -100.0*PCBU_PER_MIL;			// lower left corner of window
	m_org_y = -100.0*PCBU_PER_MIL;
	// previous
	prevx = 0;
	ppx = 0;
	prev_middle_x = 0;
	// previous
	prevy = 0;
	ppy = 0;
	prev_middle_y = 0;
	prev_m_ang = -1;

	// clearance
	m_seg_clearance = 0;

	// lines
	m_polyline_style = 0;
	m_polyline_hatch = 0;
	m_polyline_layer = 0;
	m_polyline_width = abs(m_Doc->m_polyline_w);
	m_polyline_closed = 0;

	//
	m_protect = 0;
	m_fk_button_index = -1;
	m_sel_count = 0;
	m_sel_merge_name = "";
	m_left_pane_invalid = TRUE;
	m_sel_flags = 0;
	m_paste_flags = 0;
	m_Doc->m_bom_options = 0;

	for( int i=0; i<MAX_FAVORITES; i++ )
		m_favorite_poly[i] = -1;
	for( int i=0; i<MAX_FAVORITES; i++ )
		m_favorite_page[i] = 0;

	m_bitmap_txt.DeleteObject();
	m_bitmap_word.DeleteObject();
	m_bitmap_pdf.DeleteObject();
	m_bitmap_url.DeleteObject();
	m_bitmap_txt.LoadBitmap( IDB_BITMAP_TEXT );
	m_bitmap_word.LoadBitmap( IDB_BITMAP_WORD );
	m_bitmap_pdf.LoadBitmap( IDB_BITMAP_PDF );
	m_bitmap_url.LoadBitmap( IDB_BITMAP_URL );

	m_user_scale = 1.0;
	m_measure_dist = 0;
	m_measure_scale = 30;
	m_arrow_scale = 20;
	m_ang_axis = 0.0;
	m_from_pt.SetPoint(0,0);
	m_sel_mask = 0;
	m_disable_context_menu = 0;
	m_sel_rect = rect(0,0,0,0);
	m_draw_pic = "";
	Invalidate( FALSE );// (doubtful)
}
//===============================================================================================
// destructor
CFreePcbView::~CFreePcbView()
{
}
//===============================================================================================
BOOL CFreePcbView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}
//===============================================================================================
/////////////////////////////////////////////////////////////////////////////
// CFreePcbView drawing

void CFreePcbView::OnDraw(CDC* pDC)
{
	static CRect m_win_r = rect(0,0,0,0);
	static int m_n_layers = 0;
	static int m_n_pages = 0;
	static int m_num_p = 0;
	static int MAXIMUM_Y = 0;

	// get client rectangle
	GetClientRect( &m_client_r );
	CRect r = m_client_r;
	// clear screen to black if no project open
	if( !m_Doc )
	{
		pDC->FillSolidRect( m_client_r, RGB(20,20,20) );
		m_num_p = MAX_PAGES;
		return;
	}
	if( !m_Doc->m_project_open )
	{
		pDC->FillSolidRect( m_client_r, RGB(20,20,20) );
		m_num_p = MAX_PAGES;
		return;
	}
	// colors for selected page
	COLORREF bk = RGB(m_Doc->m_rgb[LAY_BACKGND][0], m_Doc->m_rgb[LAY_BACKGND][1], m_Doc->m_rgb[LAY_BACKGND][2]);
	COLORREF tc(RGB(255,255,255));
	int summC = m_Doc->m_rgb[LAY_BACKGND][0] + m_Doc->m_rgb[LAY_BACKGND][1] + m_Doc->m_rgb[LAY_BACKGND][2];
	if( summC > 384 )
		tc = RGB(0,0,0);
	CPen gnd_pen( PS_SOLID, 1, bk );
	CBrush gnd_brush( bk );
	//
	// draw stuff on left pane
	int y_off = 10;
	int x_off = 10;
	if( m_left_pane_invalid )
	{
		int num_p = m_Doc->Pages.GetActiveNumber();
		BOOL Total_DRW = (	m_left_pane_invalid == -1 ||
							m_n_layers != m_Doc->m_num_layers || 
							m_n_pages != m_Doc->Pages.GetNumPages() || 
							m_num_p != num_p || 
							m_win_r.Width() != m_client_r.Width() ||
							m_win_r.Height() != m_client_r.Height()  );
		if( m_left_pane_invalid == -1 )
			DrawBottomPane();
		m_left_pane_invalid = 0;
		m_n_layers = m_Doc->m_num_layers;
		m_n_pages = m_Doc->Pages.GetNumPages();
		m_num_p = num_p;
		m_win_r = m_client_r;

		// erase previous contents if changed
		CBrush brush( RGB(255, 255, 255) );
		CPen pen( PS_SOLID, 1, RGB(255, 255, 255) );
		CBrush * old_brush = pDC->SelectObject( &brush );
		CPen * old_pen = pDC->SelectObject( &pen );

		// erase left pane
		r.right = m_left_pane_w;
		if( Total_DRW )
			pDC->Rectangle( &r );

		// erase bottom pane
		r = m_client_r;
		r.left += m_left_pane_w;
		r.top = r.bottom - m_bottom_pane_h;
		pDC->SelectObject( old_brush );
		pDC->SelectObject( old_pen );
		//
		CFont * old_font = pDC->SelectObject( &m_small_font );
		int index_to_active_layer;
		for( int i=2; i<m_Doc->m_num_layers; i++ )
		{
			// i = position index
			r.left = x_off;
			r.right = x_off+12;
			r.top = (i-2)*VSTEP+y_off;
			r.bottom = (i-2)*VSTEP+12+y_off;
			// il = layer index, since copper layers are displayed out of order
			int il = i;
			//
			CBrush brush( RGB(m_Doc->m_rgb[il][0], m_Doc->m_rgb[il][1], m_Doc->m_rgb[il][2]) );
			if( m_Doc->m_vis[il] )
			{
				// if layer is visible, draw colored rectangle
				CBrush * old_brush = pDC->SelectObject( &brush );
				pDC->Rectangle( &r );
				pDC->SelectObject( old_brush );
			}
			else
			{
				// if layer is invisible, draw box with X
				pDC->Rectangle( &r );
				pDC->MoveTo( r.left, r.top );
				pDC->LineTo( r.right, r.bottom );
				pDC->MoveTo( r.left, r.bottom );
				pDC->LineTo( r.right, r.top );
			}
			r.left += 20;
			r.right += 120;
			r.bottom += 5;
			if( Total_DRW )
				pDC->DrawText( &layer_str[il][0], -1, &r, 0 );
		}
		r.left = x_off;
		r.bottom += VSTEP*2;
		r.top += VSTEP*2;
		pDC->DrawText( G_LANGUAGE==0? "SELECTION MASK":"МАСКА ВЫБОРА", -1, &r, DT_TOP);
		y_off = r.bottom;
		for( int i=0; i<NUM_SEL_MASKS; i++ )
		{
			// i = position index
			r.left = x_off;
			r.right = x_off+12;
			r.top = i*VSTEP+y_off;
			r.bottom = i*VSTEP+12+y_off;
			CBrush green_brush( RGB(50, 255, 50) );
			CBrush red_brush( RGB(150, 0, 0) );
			if( m_sel_mask & (1<<i) )
			{
				// if mask is selected is visible, draw green rectangle
				CBrush * old_brush = pDC->SelectObject( &green_brush );
				pDC->Rectangle( &r );
				pDC->SelectObject( old_brush );
			}
			else
			{
				// if mask not selected, draw red
				CBrush * old_brush = pDC->SelectObject( &red_brush );
				pDC->Rectangle( &r );
				pDC->SelectObject( old_brush );
			}
			r.left += 20;
			r.right += 120;
			r.bottom += 5;
			if( Total_DRW )
				pDC->DrawText( sel_mask_str[i], -1, &r, DT_TOP );
		}
		r.left = x_off;
		r.bottom += VSTEP;
		r.top += VSTEP;
		
		// Draw Text Info
		{
			pDC->SelectObject( &gnd_pen );
			pDC->SelectObject( &gnd_brush );
			int R = 8;
			RECT rb = r;
			rb.top += VSTEP*num_p;
			rb.bottom += VSTEP*num_p;
			rb.left = 0;
			rb.right = m_left_pane_w;
			rb.top += VSTEP;
			rb.bottom += VSTEP-3;
			if( Total_DRW )
				pDC->Rectangle( &rb );
			rb.left = m_left_pane_w - R;
			rb.top -= R;
			rb.bottom += R;
			if( Total_DRW )
				pDC->Rectangle( &rb );
			pDC->SelectObject( &pen );
			pDC->SelectObject( &brush );
			rb.top -= R;
			rb.left -= R;
			rb.bottom = rb.top + R*2;
			if( Total_DRW )
				pDC->Ellipse( &rb );
			rb.top = rb.bottom + VSTEP;
			rb.bottom = rb.top + R*2;
			if( Total_DRW )
				pDC->Ellipse( &rb );
			CString name="";
			int oldBkMode = pDC->SetBkMode( TRANSPARENT );
			for( int ipg=0; ipg<m_Doc->Pages.GetNumPages(); ipg++ )
			{
				int obk, otc;
				if( ipg == num_p )
				{
					obk = pDC->SetBkColor(bk);
					otc = pDC->SetTextColor(tc);
				}
				r.bottom += VSTEP;
				r.top += VSTEP;
				m_Doc->Pages.GetPageName(ipg,&name);
				if( name.GetLength() > 13 )
				{
					name.Truncate( 13 );
					name += "..";
				}
				CString fullname;
				fullname.Format("%d - %s", ipg+1, name );
				if( Total_DRW )
					pDC->DrawText( fullname, -1, &r, DT_TOP );
				if( ipg == num_p )
				{
					pDC->SetBkColor(obk);
					pDC->SetTextColor(otc);
				}
			}
			pDC->SetBkMode( oldBkMode );
		}
		r.bottom += VSTEP;
		r.top += VSTEP;
		//
		r.bottom += VSTEP;
		r.top += VSTEP;
		r.bottom += VSTEP;
		r.top += VSTEP;
		if( m_favorite_poly[0] >= 0 )
		{
			COLORREF COL = RGB(235, 235, 235);
			COLORREF COL2 = RGB(215, 215, 215);
			CBrush ye_brush( COL );
			CPen ye_pen( PS_SOLID, 1, COL2 );
			int old_bk = pDC->SetBkColor( COL );
			pDC->SelectObject( &ye_brush );
			pDC->SelectObject( &ye_pen );
			RECT rb = r;
			rb.left = 5;
			rb.right = m_left_pane_w-5;
			rb.bottom += VSTEP*4;
			rb.bottom -= (VSTEP/2);
			rb.top -= (VSTEP/2);
			CPoint Pt( 16, 16 );
			pDC->RoundRect( &rb, Pt );
			CString prev_str = "";
			for( int inn=0; inn<MAX_FAVORITES; inn++ )
			{
				if( inn%2 )
					r.left = (m_left_pane_w/2) + (x_off/2);
				else
					r.left = x_off;
				if( m_favorite_poly[inn] >= 0 )
				{
					CArray<CPolyLine> * OP = m_Doc->m_outline_poly;
					if( m_favorite_page[inn] != m_Doc->Pages.GetActiveNumber() )
						OP = m_Doc->Pages.GetPolyArray( m_favorite_page[inn] );
					if( m_favorite_poly[inn] < OP->GetSize() )
					{
						CText * net = OP->GetAt( m_favorite_poly[inn] ).Check( index_net_attr );
						if( net )
						{
							CString NET_STR = net->m_str;
							if( net->m_str.GetLength() > 6 || prev_str.GetLength() > 6 )
								NET_STR = NET_STR.Left(6) + "...";
							pDC->DrawText( NET_STR, -1, &r, DT_TOP );
							prev_str = net->m_str;
						}
						else if( inn )
							m_favorite_poly[inn] = -1; // delete this link
						else
							pDC->DrawText( "no net", -1, &r, DT_TOP );
					}
					else
						pDC->DrawText( "error", -1, &r, DT_TOP );
				}
				if( inn%2 )
				{
					r.bottom += VSTEP;
					r.top += VSTEP;
				}
			}
			pDC->SetBkColor( old_bk );
			pDC->SelectObject( old_brush );
		}
		else
		{
			pDC->DrawText( "Исп 1 - 8 кнопки", -1, &r, DT_TOP );
			r.bottom += VSTEP;
			r.top += VSTEP;
			pDC->DrawText( "для смены страниц", -1, &r, DT_TOP );
		}
		MAXIMUM_Y = r.bottom;
		// draw function keys on bottom pane
		DrawBottomPane();
	}
	
	//** this is for testing only, needs to be converted to PCB coords
#if 0
	if( b_update_rect )
	{
		// clip to update rectangle
		CRgn rgn;
		rgn.CreateRectRgn( m_left_pane_w + update_rect.left,
			update_rect.bottom,
			m_left_pane_w + update_rect.right,
			update_rect.top  );
		pDC->SelectClipRgn( &rgn );
	}
	else
#endif
	{
		// clip to pcb drawing region
		pDC->SelectClipRgn( &m_pcb_rgn );
	}

	// now draw the display list
	SetDCToWorldCoords( pDC );
	m_Doc->m_dlist->Draw( pDC, m_draw_layer );
	if( CurDragging() )
		m_Doc->m_dlist->Drag(  pDC, m_last_cursor_point.x, m_last_cursor_point.y );

	CDC * gDC = GetDC();
	// Interpage Attribute Syncronization
	if( m_Doc->Pages.m_page_mirror_mask>>8 )		
	{
		r.top = 5;
		r.left = m_left_pane_w + 5;
		r.bottom = r.top + VSTEP*2;
		r.right = r.left + 250;

		// colors
		//int C1 = (m_Doc->m_rgb[LAY_BACKGND][0]);
		//int C2 = (m_Doc->m_rgb[LAY_BACKGND][1]);
		//int C3 = (m_Doc->m_rgb[LAY_BACKGND][2]);
		//COLORREF bg(RGB(C1,C2,C3));
		//COLORREF red(RGB(255,0,0));
		gDC->SetBkColor(bk);
		gDC->SetTextColor(tc);
		//
		if( getbit( m_Doc->Pages.m_page_mirror_mask, m_Doc->Pages.GetActiveNumber()+MAX_PAGES ) )
			gDC->DrawText( "Interpage Attribute Synchronization.", -1, &r, DT_TOP );
		else if( getbit( m_Doc->Pages.m_page_mirror_mask, m_Doc->Pages.GetActiveNumber() ) )
			gDC->DrawText( "Main page.", -1, &r, DT_TOP );
	}
	r.left = 2;
	r.bottom = m_client_r.bottom - 4;
	r.top = MAXIMUM_Y;
	r.right = r.left + m_left_pane_w - 4;
	COLORREF clr1 = RGB(0,0,255);
	COLORREF wh(RGB(255,255,255));
	CBrush br1( wh );
	gDC->SelectObject( &wh );
	CPen pen1( PS_SOLID, 3, wh );
	gDC->SelectObject( &pen1 );
	gDC->Rectangle( r );

	if( m_draw_pic.GetLength() )
	{
		r.top = r.bottom - m_left_pane_w + 4;
		CFile file;
		BOOL ok_pic = 1;
		if( file.Open( m_draw_pic + ".jpeg", CFile::modeRead, NULL ) == 0 )
			if( file.Open( m_draw_pic + ".jpg", CFile::modeRead, NULL ) == 0 )
				if( file.Open( m_draw_pic + ".bmp", CFile::modeRead, NULL ) == 0 )
					ok_pic = 0;
		if( ok_pic )
		{
			CArchive ar( &file, CArchive::load | CArchive::bNoFlushOnDelete );
			CArchiveStream ars(&ar);
			LPPICTURE lp;
			OleLoadPicture( (LPSTREAM)&ars, 0, FALSE, IID_IPicture, (void**)&lp );
			ar.Close();
			file.Close();
			if( lp )
			{
				CSize sz;
				lp->get_Width( &sz.cx );
				lp->get_Height( &sz.cy );
				double k = (double)sz.cx/(double)sz.cy;
				if( abs((double)r.Height()/k) < abs(r.bottom - MAXIMUM_Y) )
				{
					r.top = r.bottom - abs((double)r.Height()/k);
					lp->Render( gDC->GetSafeHdc(), 
						r.left,
						r.bottom-1,
						r.Width(), 
						-r.Height(), 
						0,0,sz.cx,sz.cy, NULL );
					CPen pen2( PS_SOLID, 1, clr1 );
					gDC->SelectObject( &pen2 );
					gDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
					gDC->Rectangle( r );
				}
				lp->Release();
			}
		}
		m_draw_pic = "";
	}
	ReleaseDC( gDC );
	if( gShiftKeyDown == 0 )
		m_draw_layer = LAY_HILITE;// OnDraw()
	else if( m_bRButtonDown )
		m_draw_layer = LAY_HILITE; // on context menu 
}
//===============================================================================================
/////////////////////////////////////////////////////////////////////////////
// CFreePcbView printing

BOOL CFreePcbView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
//===============================================================================================
void CFreePcbView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
//===============================================================================================
void CFreePcbView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
//===============================================================================================
/////////////////////////////////////////////////////////////////////////////
// CFreePcbView diagnostics

#ifdef _DEBUG
void CFreePcbView::AssertValid() const
{
	CView::AssertValid();
}
//===============================================================================================
void CFreePcbView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
//===============================================================================================
CFreePcbDoc* CFreePcbView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFreePcbDoc)));
	return (CFreePcbDoc*)m_pDocument;
}
#endif //_DEBUG
//===============================================================================================
/////////////////////////////////////////////////////////////////////////////
// CFreePcbView message handlers

// Window was resized
//
void CFreePcbView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	// update client rect and create clipping region
	GetClientRect( &m_client_r );
	m_pcb_rgn.DeleteObject();
	m_pcb_rgn.CreateRectRgn( m_left_pane_w, m_client_r.bottom-m_bottom_pane_h,
		m_client_r.right, m_client_r.top );

	// update display mapping for display list
	if( m_Doc )
		if( m_Doc->m_dlist && m_Doc->m_project_open )
		{
			CRect screen_r;
			GetWindowRect( &screen_r );
			m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel, m_org_x, m_org_y );
			InvalidateLeftPane();		
		}

	// create memory DC and DDB
	if( !m_botDC_created && m_client_r.right != 0 )
	{
		CDC * pDC = GetDC();
		m_botDC.CreateCompatibleDC( pDC );
		m_botDC_created = TRUE;
		m_bitmap1.CreateCompatibleBitmap( pDC, m_client_r.right, m_client_r.bottom );
		m_old_bitmap1 = (HBITMAP)::SelectObject( m_botDC.m_hDC, m_bitmap1.m_hObject );
		m_bitmap_rect1 = m_client_r;
		ReleaseDC( pDC );
	}
	else if( m_botDC_created && (m_bitmap_rect1 != m_client_r) )
	{
		CDC * pDC = GetDC();
		::SelectObject(m_botDC.m_hDC, m_old_bitmap1 );
		m_bitmap1.DeleteObject();
		m_bitmap1.CreateCompatibleBitmap( pDC, m_client_r.right, m_client_r.bottom );
		m_old_bitmap1 = (HBITMAP)::SelectObject( m_botDC.m_hDC, m_bitmap1.m_hObject );	
		m_bitmap_rect1 = m_client_r;
		ReleaseDC( pDC );
	}
	//
	if( !m_topDC_created && m_client_r.right != 0 )
	{
		CDC * pDC = GetDC();
		m_topDC.CreateCompatibleDC( pDC );
		m_topDC_created = TRUE;
		m_bitmap2.CreateCompatibleBitmap( pDC, m_client_r.right, m_client_r.bottom );
		m_old_bitmap2 = (HBITMAP)::SelectObject( m_topDC.m_hDC, m_bitmap2.m_hObject );
		m_bitmap_rect2 = m_client_r;
		ReleaseDC( pDC );
	}
	else if( m_topDC_created && (m_bitmap_rect2 != m_client_r) )
	{
		CDC * pDC = GetDC();
		::SelectObject(m_topDC.m_hDC, m_old_bitmap2 );
		m_bitmap2.DeleteObject();
		m_bitmap2.CreateCompatibleBitmap( pDC, m_client_r.right, m_client_r.bottom );
		m_old_bitmap2 = (HBITMAP)::SelectObject( m_topDC.m_hDC, m_bitmap2.m_hObject );	
		m_bitmap_rect2 = m_client_r;
		ReleaseDC( pDC );
	}
}
//===============================================================================================
// Left mouse button released, we should probably do something
//
void CFreePcbView::OnLButtonUp(UINT nFlags, CPoint point)
{
	int flag_scale_factor = 0;
	CDC * pDC = NULL;
	CPoint tp = m_Doc->m_dlist->WindowToPCB( point );
	if( !m_bLButtonDown )
	{
		// this avoids problems with opening a project with the button held down
		CView::OnLButtonUp(nFlags, point);
		goto goodbye;
	}
	if( CurDragging() )
		m_draw_layer = LAY_BACKGND;// CurDragging()
	m_bLButtonDown = FALSE;
	gLastKeyWasArrow = FALSE;	// cancel series of arrow keys
	if( m_cursor_mode == CUR_SCALE_FACTOR )
		flag_scale_factor = 1;

	if( m_bDraggingRect )
	{
		// we were dragging selection rect, handle it
		m_last_drag_rect.NormalizeRect();
		CPoint tl = m_Doc->m_dlist->WindowToPCB( m_last_drag_rect.TopLeft() );
		CPoint br = m_Doc->m_dlist->WindowToPCB( m_last_drag_rect.BottomRight() );
		m_sel_rect = CRect( tl, br );
		// control key held down
		if( CurSelected() || CurNone() )
		{
			if( m_sel_count < 2 )
				CancelSelection();
			SelectItemsInRect( m_sel_rect, nFlags );
			if( m_dragging_mode && m_sel_count )
			{
				if( m_dragging_mode == FK_DRAG_GROUP )
					SelectContour();
				OnGroupMove();
			}
		}
		else if( m_cursor_mode == CUR_ADD_OP )
		{
			//
			//
			m_polyline_style = 0;
			CRect grid = m_sel_rect;
			m_sel_id.sst = ID_SIDE;
			int x1,x2,x3,x4,y1,y2,y3,y4,ss;
			int Width = grid.Width();
			int Height = abs(grid.Height());
			int side_L = min( grid.Width(), abs(grid.Height()) )/2;
			if( m_object == OBJ_RNDRECT )
				side_L /= 5;
			//
			//
			if( m_object == OBJ_RECT || m_object == OBJ_CLOUD )
			{
				ss = 0;
				x1 = grid.left;
				y1 = grid.bottom;	
				x2 = grid.left;
				y2 = grid.top;	
				x3 = grid.right;
				y3 = grid.top;	
				x4 = grid.right;
				y4 = grid.bottom;
			}
			else if( m_object == OBJ_SQUARE )
			{
				ss = 0;
				x1 = grid.CenterPoint().x - side_L;
				y1 = grid.CenterPoint().y - side_L;	
				x2 = grid.CenterPoint().x - side_L;
				y2 = grid.CenterPoint().y + side_L;	
				x3 = grid.CenterPoint().x + side_L;
				y3 = grid.CenterPoint().y + side_L;	
				x4 = grid.CenterPoint().x + side_L;
				y4 = grid.CenterPoint().y - side_L;

			}
			else if( m_object == OBJ_OVAL || m_object == OBJ_RNDRECT )
			{
				ss = 0;
				if( Width < Height )
				{
					x1 = grid.left;
					y1 = grid.bottom + side_L;	
					x2 = grid.left;
					y2 = grid.top - side_L;	
					x3 = grid.right;
					y3 = grid.top - side_L;	
					x4 = grid.right;
					y4 = grid.bottom + side_L;
				}
				else
				{
					x1 = grid.left + side_L;
					y1 = grid.bottom;	
					x2 = grid.left + side_L;
					y2 = grid.top;	
					x3 = grid.right - side_L;
					y3 = grid.top;	
					x4 = grid.right - side_L;
					y4 = grid.bottom;
				}
			}
			else if( m_object == OBJ_ROUND )
			{
				ss = 1;
				x1 = grid.CenterPoint().x;
				y1 = grid.CenterPoint().y - side_L;	
				x2 = grid.CenterPoint().x - side_L;
				y2 = grid.CenterPoint().y;	
				x3 = grid.CenterPoint().x;
				y3 = grid.CenterPoint().y + side_L;	
				x4 = grid.CenterPoint().x + side_L;
				y4 = grid.CenterPoint().y;
			}
			else if( m_object == OBJ_ELLIPSE )
			{
				ss = 2;
				x1 = grid.CenterPoint().x;
				y1 = grid.CenterPoint().y - grid.Height()/2;	
				x2 = grid.CenterPoint().x - grid.Width()/2;
				y2 = grid.CenterPoint().y;	
				x3 = grid.CenterPoint().x;
				y3 = grid.CenterPoint().y + grid.Height()/2;	
				x4 = grid.CenterPoint().x + grid.Width()/2;
				y4 = grid.CenterPoint().y;
			}
			else
				ASSERT(0);
			//
			int mem_i = m_sel_id.ii;
			if( m_sel_id.ii > 0 )
			{
				m_sel_op.AppendCorner( x1, y1, ss, 0 );
				///mem_i++;
			}
			else
			{
				m_sel_op.Start( m_polyline_layer, m_polyline_width, 20*NM_PER_MIL, x1, y1, m_polyline_hatch, &m_sel_id, NULL );
			}
			m_sel_id.ii++;
			m_sel_op.AppendCorner( x2, y2, ss, 0 );
			m_sel_id.ii++;
			m_sel_op.AppendCorner( x3, y3, ss, 0 );
			m_sel_id.ii++;
			m_sel_op.AppendCorner( x4, y4, ss, 0 );
			if( m_object == OBJ_OVAL )
			{
				ss = 1;
				if( Width < Height )
				{
					m_sel_id.ii++;
					m_sel_op.InsertCorner( m_sel_id.ii-2, grid.CenterPoint().x, grid.top, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-2, 1, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-3, 1, 0 );
					m_sel_id.ii++;
					m_sel_op.AppendCorner( grid.CenterPoint().x, grid.bottom, ss, 0 );
				}
				else
				{
					m_sel_id.ii++;
					m_sel_op.InsertCorner( m_sel_id.ii-3, grid.left, grid.CenterPoint().y, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-3, 1, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-4, 1, 0 );
					m_sel_id.ii++;
					m_sel_op.InsertCorner( m_sel_id.ii-1, grid.right, grid.CenterPoint().y, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-1, 1, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-2, 1, 0 );
					ss = 0;
				}
			}
			else if( m_object == OBJ_RNDRECT )
			{
				ss = 1;
				if( Width < Height )
				{
					m_sel_id.ii;
					m_sel_op.InsertCorner( m_sel_id.ii-1, grid.right - side_L, grid.top, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-1, 1, 0 );
					m_sel_op.InsertCorner( m_sel_id.ii-1, grid.left + side_L, grid.top, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-2, 1, 0 );
					
					m_sel_op.AppendCorner( grid.right - side_L, grid.bottom, ss, 0 );
					m_sel_op.AppendCorner( grid.left + side_L, grid.bottom, 0, 0 );
				}
				else
				{
					m_sel_op.InsertCorner( m_sel_id.ii-2, grid.left, grid.top - side_L, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-2, 1, 0 );
					m_sel_op.InsertCorner( m_sel_id.ii-2, grid.left, grid.bottom + side_L, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii-3, 1, 0 );
					m_sel_op.InsertCorner( m_sel_id.ii+2, grid.right, grid.bottom + side_L, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii+2, 1, 0 );
					m_sel_op.InsertCorner( m_sel_id.ii+2, grid.right, grid.top - side_L, 0 );
					m_sel_op.SetSideStyle( m_sel_id.ii+1, 1, 0 );
					ss = 0;
				}
			}
			else if( m_object == OBJ_CLOUD )
			{
				int n_eddies = Height/(m_sel_op.GetW()+1)/10 + 1;
				n_eddies = min(n_eddies,100);
				side_L = Height;
				side_L /= n_eddies;
				side_L /= 2;
				//
				int shift = Height%side_L;
				int nsteps1 = n_eddies;
				m_sel_op.MoveCorner(1+mem_i, m_sel_op.GetX(1+mem_i), m_sel_op.GetY(1+mem_i)-shift, 0 );
				m_sel_op.MoveCorner(2+mem_i, m_sel_op.GetX(2+mem_i), m_sel_op.GetY(2+mem_i)-shift, 0 );
				shift = Width%side_L;
				int nsteps2 = Width/side_L;
				m_sel_op.MoveCorner(2+mem_i, m_sel_op.GetX(2+mem_i)-shift, m_sel_op.GetY(2+mem_i), 0 );
				m_sel_op.MoveCorner(3+mem_i, m_sel_op.GetX(3+mem_i)-shift, m_sel_op.GetY(3+mem_i), 0 );
				if( nsteps2%2 )
				{
					nsteps2--;
					m_sel_op.MoveCorner(2+mem_i, m_sel_op.GetX(2+mem_i)-side_L, m_sel_op.GetY(2+mem_i), 0 );
					m_sel_op.MoveCorner(3+mem_i, m_sel_op.GetX(3+mem_i)-side_L, m_sel_op.GetY(3+mem_i), 0 );
				}
				grid = rect( m_sel_op.GetX(mem_i), m_sel_op.GetY(mem_i), m_sel_op.GetX(mem_i+2), m_sel_op.GetY(mem_i+2) );
				//
				for( int c=mem_i; c<(nsteps1*2-1)+mem_i; c++ )
					m_sel_op.InsertCorner( c+1, grid.left - side_L*(1-(c-mem_i)%2), grid.bottom + side_L*((c-mem_i)+1), 0 );
				if( nsteps2 ) for( int c=mem_i; c<nsteps2-1+mem_i; c++ )
					m_sel_op.InsertCorner( c+(nsteps1*2)+1, grid.left + side_L*((c-mem_i)+1), grid.top + side_L*(1-(c-mem_i)%2), 0 );
				int Flag = !nsteps2;
				for( int c=mem_i; c<(nsteps1*2-1)+mem_i; c++ )
					m_sel_op.InsertCorner( c+(nsteps1*2)+nsteps2+Flag+1, grid.right + side_L*(1-(c-mem_i)%2), grid.top - side_L*((c-mem_i)+1), 0 );
				if( nsteps2 ) for( int c=mem_i; c<nsteps2-1+mem_i; c++ )
					m_sel_op.AppendCorner( grid.right - side_L*((c-mem_i)+1), grid.bottom - side_L*(1-(c-mem_i)%2), 0, 0 );
				for( int c=mem_i; c<m_sel_op.GetNumSides(); c++ )
					m_sel_op.SetSideStyle( c, 1, 0 );
				ss = 1;
				if( nsteps1 && nsteps2 )
				{
					m_sel_op.DeleteCorner(mem_i,1,1,0);
					m_sel_op.DeleteCorner((nsteps1*2)-1+mem_i,1,1,0);
					m_sel_op.DeleteCorner((nsteps1*2)+nsteps2-2+mem_i,1,1,0);
					m_sel_op.DeleteCorner((nsteps1*4)+nsteps2-3+mem_i,1,1,0);
				}
			}
			m_sel_op.Close( ss );
			for( int cc=mem_i; cc<m_sel_op.GetNumSides(); cc++ )
			{
				m_sel_id.ii = cc;
				NewSelect( NULL, & m_sel_id, 0, 0 );
			}
			m_sel_id.ii--;
			if( m_object == OBJ_RECT )
				m_Doc->OnProjectAdaptGrid(); //OnProjectAdaptGrid before SetCursorMode!
			SetCursorMode( CUR_GROUP_SELECTED );
			HighlightGroup();
			m_object = 0;
		}
		m_bDraggingRect = FALSE;
		CView::OnLButtonUp(nFlags, point);
		goto goodbye;
	}

	if( point.y > (m_client_r.bottom-m_bottom_pane_h) )
	{
		// clicked in bottom pane, test for hit on function key rectangle
		for( int i=0; i<9; i++ )
		{
			CRect r( FKEY_OFFSET_X+i*FKEY_STEP+(i/4)*FKEY_GAP,
				m_client_r.bottom-FKEY_OFFSET_Y-FKEY_R_H,
				FKEY_OFFSET_X+i*FKEY_STEP+(i/4)*FKEY_GAP+FKEY_R_W,
				m_client_r.bottom-FKEY_OFFSET_Y );
			if( r.PtInRect( point ) )
			{
				// fake function key pressed
				int nChar = i + 112;
				HandleKeyPress( nChar, 0, 0 );
				goto goodbye;
			}
		}
	}
	else if( point.x < m_left_pane_w )
	{
		// clicked in left pane
		CRect r = m_client_r;
		int y_off = 10;
		int x_off = 10;
		for( int i=2; i<m_Doc->m_num_layers; i++ )
		{
			// i = position index
			// il = layer index, since copper layers are displayed out of order
			int il = i;
			//
			// get color square
			r.left = x_off;
			r.right = x_off+12;
			r.top = (i-2)*VSTEP+y_off;
			r.bottom = (i-2)*VSTEP+12+y_off;
			if( r.PtInRect( point ) && il > LAY_BACKGND )
			{
				// clicked in color square
				m_Doc->m_vis[il] = !m_Doc->m_vis[il];
				m_Doc->m_dlist->SetLayerVisible( il, m_Doc->m_vis[il] );
				m_draw_layer = 0;// if( point.x < m_left_pane_w )
			}
			else
			{
				// get layer name rect
				r.left += 20;
				r.right += 120;
				r.bottom += 5;
				if( r.PtInRect( point ) )
				{
					// clicked on layer name
					if( i >= LAY_FOOTPRINT && i < LAY_FOOTPRINT + 8 )
					{
						int nChar = '1' + i - LAY_FOOTPRINT;
						HandleKeyPress( nChar, 0, 0 );
					}
					else
					{
						switch( i )
						{
						case LAY_FOOTPRINT:		HandleKeyPress( '1', 0, 0 ); break;
						case LAY_PART_VALUE:		HandleKeyPress( '2', 0, 0 ); break;
						case LAY_ADD_1:	HandleKeyPress( '3', 0, 0 ); break;
						case LAY_ADD_1+1:	HandleKeyPress( '4', 0, 0 ); break;
						case LAY_ADD_1+2:	HandleKeyPress( '5', 0, 0 ); break;
						case LAY_ADD_1+3:	HandleKeyPress( '6', 0, 0 ); break;
						case LAY_ADD_1+4:	HandleKeyPress( '7', 0, 0 ); break;
						case LAY_ADD_1+5:	HandleKeyPress( '8', 0, 0 ); break;
						case LAY_ADD_1+6:	HandleKeyPress( 'Q', 0, 0 ); break;
						case LAY_ADD_1+7:	HandleKeyPress( 'W', 0, 0 ); break;
						case LAY_ADD_1+8:	HandleKeyPress( 'E', 0, 0 ); break;
						case LAY_ADD_1+9:	HandleKeyPress( 'R', 0, 0 ); break;
						case LAY_ADD_1+10: HandleKeyPress( 'T', 0, 0 ); break;
						case LAY_ADD_1+11: HandleKeyPress( 'Y', 0, 0 ); break;
						case LAY_ADD_1+12: HandleKeyPress( 'U', 0, 0 ); break;
						case LAY_ADD_1+13: HandleKeyPress( 'I', 0, 0 ); break;
						case LAY_ADD_1+14: HandleKeyPress( 'O', 0, 0 ); break;
						case LAY_ADD_1+15: HandleKeyPress( 'P', 0, 0 ); break;
						}
					}
				}
			}
		}
		y_off = r.bottom + 2*VSTEP;
		for( int i=0; i<NUM_SEL_MASKS; i++ )
		{
			// get color square
			r.left = x_off;
			r.right = x_off+12+m_left_pane_w;
			r.top = i*VSTEP+y_off;
			r.bottom = i*VSTEP+12+y_off;
			if( r.PtInRect( point ) )
			{
				// clicked in color square or name
				invbit( m_sel_mask, i );
				if( nFlags & MK_CONTROL )
					m_sel_mask = ~m_sel_mask;
				SetSelMaskArray( m_sel_mask );
			}
		}
		y_off += (NUM_SEL_MASKS*VSTEP);
		y_off += VSTEP;
		for( int i=0; i<m_Doc->Pages.GetNumPages(); i++ )
		{
			// get color square
			r.left = x_off;
			r.right = x_off+12+m_left_pane_w;
			r.top = i*VSTEP+y_off;
			r.bottom = i*VSTEP+12+y_off;
			if( r.PtInRect( point ) )
			{
				m_Doc->SwitchToPage(i,TRUE);
				break;
			}
		}
		y_off += (m_Doc->Pages.GetNumPages()*VSTEP);
		y_off += 2*VSTEP;
		for( int i=0; i<MAX_FAVORITES; i++ )
		{
			r.left = x_off;
			r.right = (m_left_pane_w/2);
			if( i%2 == 0 )
			{
				r.top = i*VSTEP/2+y_off;
				r.bottom = i*VSTEP/2+12+y_off;
			}
			else
			{
				r.left = (m_left_pane_w/2) + x_off;
				r.right = m_left_pane_w;
			}
			if( r.PtInRect( point ) )
			{
				if( m_favorite_poly[i] >= 0 )
				{
					CancelSelection(FALSE);
					int mem_p_num = m_Doc->Pages.GetActiveNumber();
					if( m_favorite_page[i] != mem_p_num )
						m_Doc->SwitchToPage( m_favorite_page[i] );
					if( m_favorite_poly[i] < m_Doc->m_outline_poly->GetSize() )
					{
						CancelSelection(FALSE);
						id ID( ID_POLYLINE, ID_GRAPHIC, m_favorite_poly[i], ID_SIDE, 0 );
						NewSelect(NULL,&ID,TRUE,0);
						OnGroupCopy();
						if( m_favorite_page[i] != mem_p_num )
							m_Doc->SwitchToPage( mem_p_num );
						OnGroupPaste(0);
					}
				}
				break;
			}
		}
		InvalidateLeftPane();
		Invalidate( FALSE );
		goto goodbye;
	}
	else
	{
		// clicked in PCB pane
		if(	CurNone() || CurSelected() )
		{
			// see if new item selected
			CPoint p = m_Doc->m_dlist->WindowToPCB( point );
			id sid;
			void * sel_ptr = NULL;
			if( m_sel_count == 1 )
			{
				if( m_sel_id.type == ID_TEXT )
					sel_ptr = m_sel_text;
				else if( m_sel_id.type == ID_DRC )
					sel_ptr = m_sel_dre;
			}
			id inf( m_sel_id );
			////m_sel_layer = -1;
			m_sel_layer = LAY_PIN_LINE;
			int max_dist = NM_PER_MIL*m_Doc->m_dlist->m_scale/3.0;
			if( nFlags )
				inf.Clear();

			void * ptr;
			if( getbit( m_sel_mask, SEL_MASK_PART ) )
				ptr = m_Doc->m_dlist->TestSelect( p.x, p.y, &sid, &m_sel_layer, &inf, sel_ptr,
				m_mask_id, NUM_SEL_MASKS, max_dist, m_Doc->m_outline_poly );
			else
				ptr = m_Doc->m_dlist->TestSelect( p.x, p.y, &sid, &m_sel_layer, &inf, sel_ptr,
				m_mask_id, NUM_SEL_MASKS, max_dist );

			// cancel sel
			if( !nFlags )
			{
				if( sid.type == ID_NONE )
					goto cancel_selection_and_goodbye;
				if( sid != m_sel_id )
					CancelSelection();
			}
			
			// select el
			m_sel_id = sid;
			prev_sel_count = 0;
			int el_merge = -1;
			if( sid.type == ID_POLYLINE )
			{
				m_polyline_width = m_Doc->m_outline_poly->GetAt(sid.i).GetW();
				m_polyline_closed = m_Doc->m_outline_poly->GetAt(sid.i).GetClosed();
				m_polyline_layer = m_Doc->m_outline_poly->GetAt(sid.i).GetLayer();
				if( m_polyline_layer < LAY_ADD_1 )
				{
					m_active_layer = m_polyline_layer;
					ShowActiveLayer( m_active_layer );
				}
				int mem_sel_c = m_sel_count;
				el_merge = m_Doc->m_outline_poly->GetAt(sid.i).GetMerge();
				if( sid.sst != ID_CORNER )
				{
					if( el_merge >= 0 && ( m_sel_mask & (1<<SEL_MASK_MERGES ) ))
					{
						if( m_cursor_mode != CUR_GROUP_SELECTED )
							CancelSelection();
						NewSelectM( el_merge );		
						HighlightGroup();
					}
					else if( CText * desc = m_Doc->m_outline_poly->GetAt(sid.i).Check( index_desc_attr ) )
					{
						if ( m_sel_mask & (1<<SEL_COMPLEX_PARTS ) )
							if( desc->m_str.Right(3) == "BLK" )
								SelectBlock( &m_Doc->m_outline_poly->GetAt(sid.i), m_Doc->Pages.GetActiveNumber(), TRUE );
					}
					if( m_sel_count == mem_sel_c )
						if( m_dragging_mode != FK_DRAG_LINE )
							if( ( m_sel_mask & (1<<SEL_MASK_PART ) ) )
								if( m_Doc->m_outline_poly->GetAt(sid.i).Check( index_part_attr ) )
								{
									NewSelectP( m_Doc->m_outline_poly->GetAt(sid.i).pAttr[index_part_attr],
												m_Doc->m_outline_poly->GetAt(sid.i).pAttr[index_pin_attr] );
									m_Doc->SwitchToPCB( TRUE, FALSE, TRUE );
								}
				}
				if( m_sel_count == mem_sel_c )
				{
					if( sid.sst == ID_SIDE && ( m_sel_mask & (1<<SEL_MASK_OP_SIDES ) ) ||
						sid.sst == ID_CORNER && ( m_sel_mask & (1<<SEL_MASK_OP_CORNERS ) ) )
						el_merge = NewSelect( ptr, &sid, 1, 1 );
				}
			}
			else if( sid.type == ID_TEXT )
			{
				if( (nFlags & MK_CONTROL) && m_sel_count == 0 )
				{
					CText * gT = (CText*)ptr;
					if( gT )
					if( gT->m_tl == m_Doc->Attr->m_pDesclist || gT->m_tl == m_Doc->Attr->m_Valuelist )
					{
						CString LINK = CP_LINK;
						int iof = gT->m_str.Find(LINK);
						if( iof > 0 )
						{
							CString ref = gT->m_str.Right( gT->m_str.GetLength() - iof - LINK.GetLength() );
							iof = ref.Find("'");
							if( iof > 0 )
								ref = ref.Left( iof );
							if( ref.Find("www") == -1 && ref.Find("\\") == -1 && ref.Find("/") == -1 && ref.Find("file:") == -1 )
								ref.Replace(":"," ");
							ref.Replace("'"," ");
							ref.Trim();
							///iof = ref.Find(" ");
							///if( iof > 0 )
							///	ref = ref.Left(iof);
							CString PAGENUM = "page_number";
							int page = 0;
							iof = gT->m_str.Find(PAGENUM);
							if( iof > 0 )
							{
								CString PGN = gT->m_str.Right( gT->m_str.GetLength() - iof - PAGENUM.GetLength() );
								PGN.Replace(":"," ");
								PGN.Replace("'"," ");
								PGN.Trim();
								iof = PGN.Find(" ");
								if( iof > 0 )
									PGN = PGN.Left(iof);
								page = my_atoi(&PGN);
							}
							if( ref.GetLength() )
							{
								CString parse_ref, parse_pin;
								int io = ref.FindOneOf(".-");
								if( io != -1 )
								{
									parse_ref = ref.Left( io );
									parse_pin = ref.Right( ref.GetLength() - io - 1 );
								}
								int maxP = m_Doc->Pages.GetNumPages();
								for( int np=max(0,page-1); np<maxP; np++ )
								{
									int it = -1;
									CText * J = m_Doc->Pages.GetTextList(np,index_part_attr)->GetText(&ref,&it);
									if( J )
									{
										if( m_Doc->Pages.GetActiveNumber() != np )
											m_Doc->SwitchToPage(np, TRUE);
										JumpToPart(J); // LINK
										OnRangeCmds( NULL );
										goto goodbye;
									}
									else if( parse_ref.GetLength() )
									{
										J = m_Doc->Pages.GetTextList(np,index_part_attr)->GetText(&parse_ref,&it);
										if( J )
										{
											CArray<CPolyLine> * p_array = m_Doc->Pages.GetPolyArray( np );
											CDlgCompare C;
											CPolyLine * pin_poly = C.FindPin( J, &parse_pin, p_array );
											if( pin_poly )
												if( CText * tpin = pin_poly->Check( index_pin_attr ) )
												{
													if( m_Doc->Pages.GetActiveNumber() != np )
														m_Doc->SwitchToPage(np, TRUE);
													JumpToPart(J, tpin, 1 ); // LINK
													OnRangeCmds( NULL );
													goto goodbye;
												}
										}
									}
									if( np == maxP-1 )
									{
										if( ref.Left(5).CompareNoCase("file:") == 0 )
											ref.Delete(0,5);
										if( ref.Left(2).CompareNoCase("./") == 0 )
											ref.Delete(0,2);
										if( ref.Find(":") < 0 && ref.Find("www") < 0 && ref.Find("http") < 0 )
											ref = m_Doc->m_path_to_folder + "\\related_files\\" + ref;
										int revF = ref.ReverseFind('\\');
										///if( revF <= 0 )
										///	revF = ref.ReverseFind('/');
										if( revF > 0 )
											ShellExecute(	NULL, "open", "\""+ref+"\"", NULL, "\""+ref.Left(revF)+"\"", SW_SHOWNORMAL);
										else
											ShellExecute(	NULL, "open", "\""+ref+"\"", NULL, NULL, SW_SHOWNORMAL);
									}
								}
							}
						}
					}
				}		
				if( sid.type == ID_TEXT && ( m_sel_mask & (1<<SEL_MASK_TEXT ) ) ||
						sid.type == ID_DRC && ( m_sel_mask & (1<<SEL_MASK_DRC ) ) )
				{
					el_merge = NewSelect( ptr, &sid, 1, 1 );
					//if( el_merge >= 0 && ( m_sel_mask & (1<<SEL_MASK_MERGES ) ))
					//{
					//	if( m_cursor_mode != CUR_GROUP_SELECTED )
					//		CancelSelection();
					//	NewSelectM( el_merge );	
					//	if( m_sel_count == 0 )
					//		el_merge = NewSelect( ptr, &sid, 1, 1 );
					//	else
					//		HighlightGroup();
					//}
				}
			}
			else
				el_merge = NewSelect( ptr, &sid, 1, 1 );

			// selected count == 0
			if( m_sel_count == 0 )
				goto cancel_selection_and_goodbye;

			// drag group
			if( m_dragging_mode )
			{
				if( m_dragging_mode == FK_DRAG_GROUP )
					SelectContour();
				OnGroupMove();
			}

			// return if group sel
			if( m_cursor_mode == CUR_GROUP_SELECTED )
			{
				if( m_dlg_partlist )
					if( m_dlg_partlist->IsWindowVisible() )
						m_dlg_partlist->ImportPartlist();
				goto goodbye;
			}
		}
		else 
		{
			// magnetize
			int abort = 0;
			if( m_cursor_mode == CUR_ADD_OP ||
				m_cursor_mode == CUR_DRAG_OP_1 ||
				m_cursor_mode == CUR_DRAG_OP ||
				m_cursor_mode == CUR_DRAG_OP_MOVE ||
				m_cursor_mode == CUR_DRAG_OP_INSERT )
				if( m_magnetize )
				{
					pDC = GetDC();
					SetDCToWorldCoords( pDC );
					pDC->SelectClipRgn( &m_pcb_rgn );
					//ReleaseDC( pDC ); in the end

					CPoint p;
					p = m_last_cursor_point;
					id incl( ID_POLYLINE, ID_GRAPHIC, -1, ID_CORNER, -1 );
					id excl( m_sel_id );
					excl.ii = -1;
					double max_d = NM_PER_MIL*m_Doc->m_dlist->m_scale*m_Doc->m_magnetize_value/100;
					int lay = m_sel_op.GetLayer();
					id sid( -1,-1,-1,-1,-1 );
					m_Doc->m_dlist->TestSelect( p.x, p.y, &sid, &lay, &excl, NULL, &incl, 1, max_d );
					if( sid.type == ID_POLYLINE )
					{
						p.x = m_Doc->m_outline_poly->GetAt(sid.i).GetX(sid.ii);
						p.y = m_Doc->m_outline_poly->GetAt(sid.i).GetY(sid.ii);
						BOOL EN = 1;
						if( m_cursor_mode != CUR_DRAG_OP_MOVE )
							for( int icor=0; icor<m_Doc->m_outline_poly->GetAt( m_sel_id.i ).GetNumCorners(); icor++ )
								if( p.x == m_Doc->m_outline_poly->GetAt(m_sel_id.i).GetX( icor ) )
									if( p.y == m_Doc->m_outline_poly->GetAt(m_sel_id.i).GetY( icor ) )
										EN = 0;
						if( EN )
						{
							abort = 1;
							m_last_cursor_point = p;
							CPoint psc = m_Doc->m_dlist->PCBToScreen( p );
							SetCursorPos( psc.x, psc.y );
							//break;
						}
					}
					else
					{
						incl.sst = ID_SIDE;
						excl.sst = ID_SIDE;
						lay = m_sel_op.GetLayer();
						sid.Set( -1,-1,-1,-1,-1 );
						m_Doc->m_dlist->TestSelect( p.x, p.y, &sid, &lay, &excl, NULL, &incl, 1, max_d );
						if( sid.type == ID_POLYLINE )
						{
							CPoint p1;
							p1.x = m_Doc->m_outline_poly->GetAt(sid.i).GetX(sid.ii);
							p1.y = m_Doc->m_outline_poly->GetAt(sid.i).GetY(sid.ii);
							if( (sid.ii) < m_Doc->m_outline_poly->GetAt(sid.i).GetNumCorners() )
							{
								CPoint p2;
								p2.x = m_Doc->m_outline_poly->GetAt(sid.i).GetX(m_Doc->m_outline_poly->GetAt(sid.i).GetNextCornerIndex(sid.ii));
								p2.y = m_Doc->m_outline_poly->GetAt(sid.i).GetY(m_Doc->m_outline_poly->GetAt(sid.i).GetNextCornerIndex(sid.ii));
								double intersectX=0, intersectY=0;
								if( abs(p1.x-p2.x) > abs(p1.y-p2.y) )
  									FindLineIntersection(	p1.x, p1.y, p2.x, p2.y, p.x, p.y, p.x, p.y+10, &intersectX, &intersectY );
								else
									FindLineIntersection(	p1.x, p1.y, p2.x, p2.y, p.x, p.y, p.x+10, p.y, &intersectX, &intersectY );
								BOOL EN = 1;
								if( m_cursor_mode != CUR_DRAG_OP_MOVE )
									for( int icor=0; icor<m_Doc->m_outline_poly->GetAt( m_sel_id.i ).GetNumCorners(); icor++ )
										if( p.x == m_Doc->m_outline_poly->GetAt(m_sel_id.i).GetX( icor ) )
											if( p.y == m_Doc->m_outline_poly->GetAt(m_sel_id.i).GetY( icor ) )
												EN = 0;
								if( EN && (intersectX || intersectY) )
								{
									abort = 1;
									p.SetPoint( intersectX, intersectY );
									m_last_cursor_point = p;
									CPoint psc = m_Doc->m_dlist->PCBToScreen( p );
									SetCursorPos( psc.x, psc.y );
									//break;
								}
							}
						}
					}
				}
			// go!
			if( m_cursor_mode == CUR_DRAG_GROUP || m_cursor_mode == CUR_DRAG_GROUP_ADD )
			{
				// complete move
				int mode = m_cursor_mode;
				int an = m_Doc->m_dlist->GetDragAngle();
				CancelDraggingGroup();
				int dx = m_last_cursor_point.x - m_from_pt.x;
				int dy = m_last_cursor_point.y - m_from_pt.y;
				//
				if (an)
				{
					setbit(m_sel_flags, NUM_SEL_FLAGS); // for enable free rotate texts
					RotateGroup(-an, m_from_pt.x, m_from_pt.y);
				}
				//
				CText * BOM_PATH = NULL;
				if( m_sel_id.type == ID_POLYLINE )
					if( m_sel_op.Check(index_desc_attr) )
						if( m_sel_op.pAttr[index_desc_attr]->m_str.Right(3) == "BOM" )
							BOM_PATH = m_sel_op.pAttr[index_desc_attr];
				MoveGroup( dx, dy, BOM_PATH );
				OnGroupGridMagnetize( m_Doc );
				m_Doc->m_dlist->CancelHighLight();
				
				//
				if( m_dragging_mode )
				{
					HighlightGroup();
					CRect r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
					SwellRect( &r, r.Width() );
					if( m_dragging_mode == FK_DRAG_GROUP && m_sel_text )
						if( m_sel_text->m_tl == m_Doc->Attr->m_Reflist )
							if( m_sel_text->m_font_size )
							{
								if( getbit( m_sel_text->m_pdf_options, RVF_FLAG ) )
									OnTextAlignRVF();
								else
									OnTextAlignRFV();
							}
					CancelSelection();
					m_Doc->m_dlist->StartDraggingArray( pDC, m_last_mouse_point.x, m_last_mouse_point.y, 1 );
					m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
				}
				else
				{
					SetCursorMode( CUR_GROUP_SELECTED );
					HighlightGroup();
				}
				m_Doc->ProjectModified( TRUE );
			}
			else if( m_cursor_mode == CUR_MOVE_ORIGIN )
			{
				// complete move
				SetCursorMode( CUR_NONE_SELECTED );
				CPoint p = m_Doc->m_dlist->WindowToPCB( point );
				m_dragging_mode = 0;
				m_Doc->m_dlist->StopDragging();
				SaveUndoInfoForMoveOrigin( -m_last_cursor_point.x, -m_last_cursor_point.y, m_Doc->m_undo_list );
				MoveOrigin( -m_last_cursor_point.x, -m_last_cursor_point.y );
				OnViewAllElements();
				m_Doc->ProjectModified( TRUE );
				goto goodbye;
			}
			else if( m_cursor_mode == CUR_ADD_OP )
			{
				// place first corner of outline
				pDC = GetDC();
				SetDCToWorldCoords( pDC );
				pDC->SelectClipRgn( &m_pcb_rgn );

				CPoint p;
				p = m_last_cursor_point;
				// ID
				m_sel_id.st = ID_GRAPHIC;
				//
				if( m_sel_id.ii )
				{
					m_sel_op.AppendCorner( p.x, p.y, m_polyline_style, TRUE );
					int cont = m_sel_op.GetContour( m_sel_id.ii );
					if( m_sel_id.ii != m_sel_op.GetContourStart( cont ) )
						m_Doc->m_dlist->HighLight( m_sel_op.dl_side[m_sel_id.ii] );
				}
				else
				{
					m_sel_op.Start( m_polyline_layer, m_polyline_width, 20*NM_PER_MIL, p.x, p.y, m_polyline_hatch, &m_sel_id, NULL );
					m_sel_id.ii = 0;
				}
				m_sel_id.sst = ID_CORNER;
				m_Doc->FindNodeLine(m_sel_id.i);
				m_Doc->m_dlist->StartDraggingArc( pDC, m_polyline_style, p.x, p.y, p.x, p.y, LAY_SELECTION, m_sel_op.GetW(), 2 );

				SetCursorMode( CUR_DRAG_OP_1 );
				m_Doc->ProjectModified( TRUE );
				m_snap_angle_ref = m_last_cursor_point;
			}
			else if( m_cursor_mode == CUR_DRAG_OP_1 )
			{
				// place second corner of outline
				pDC = GetDC();
				SetDCToWorldCoords( pDC );
				pDC->SelectClipRgn( &m_pcb_rgn );
				CPoint p;
				p = m_last_cursor_point;
				m_sel_op.AppendCorner( p.x, p.y, m_polyline_style, TRUE );
				if (abort)
				{
					m_Doc->m_dlist->StopDragging();
					m_Doc->FindNodeLine(m_sel_id.i);
					m_Doc->m_outline_poly->GetAt(m_sel_id.i).MakeVisible();
					CancelSelection();
					OnRangeCmds(NULL);
				}
				else
				{
					m_Doc->m_dlist->HighLight(m_sel_op.dl_side[m_sel_id.ii]);
					m_Doc->m_dlist->StartDraggingArc(pDC, m_polyline_style, p.x, p.y, p.x, p.y, LAY_SELECTION, m_sel_op.GetW(), 2);
					m_sel_id.ii++;
					SetCursorMode(CUR_DRAG_OP);
					m_Doc->ProjectModified(TRUE);
					m_snap_angle_ref = m_last_cursor_point;
				}
			}
			else if( m_cursor_mode == CUR_DRAG_OP )
			{
				// place subsequent corners of outline
				pDC = GetDC();
				SetDCToWorldCoords( pDC );
				pDC->SelectClipRgn( &m_pcb_rgn );

				CPoint p;
				p = m_last_cursor_point;
				if( p.x == m_sel_op.GetX(0)
					&& p.y == m_sel_op.GetY(0) )
				{
					// this point is the start point, close the polyline and quit
					m_sel_op.Close( m_polyline_style );
					m_Doc->m_dlist->HighLight( m_sel_op.dl_side[m_sel_id.ii] );
					SetCursorMode( CUR_NONE_SELECTED );
					m_Doc->m_dlist->StopDragging();
				}
				else
				{
					// add corner to polyline
					m_sel_op.AppendCorner( p.x, p.y, m_polyline_style, TRUE );
					if( m_magnetize )
						if( m_sel_id.ii > 0 )
							if( m_polyline_layer < LAY_ADD_1 )
								if( m_Doc->m_snap_angle == 45 || m_Doc->m_snap_angle == 90 )
									AlignSides( ID_POLYLINE, m_sel_id.i, m_sel_id.ii );
					if (abort)
					{
						m_Doc->m_dlist->StopDragging();
						m_Doc->FindNodeLine(m_sel_id.i);
						m_Doc->m_outline_poly->GetAt(m_sel_id.i).MakeVisible();
						CancelSelection();
						OnRangeCmds(NULL);
					}
					else
					{
						m_Doc->m_dlist->HighLight(m_sel_op.dl_side[m_sel_id.ii]);
						m_Doc->m_dlist->StartDraggingArc(pDC, m_polyline_style, p.x, p.y, p.x, p.y, LAY_SELECTION, m_sel_op.GetW(), 2);
						m_sel_id.ii++;
						m_snap_angle_ref = m_last_cursor_point;
					}
				}
				if( m_polyline_style && m_sel_op.GetNumSides() > 0 )
				{
					if( p.x == m_sel_op.GetX( m_sel_op.GetNumSides()-1 ) ||
						p.y == m_sel_op.GetY( m_sel_op.GetNumSides()-1 ) )
					{
						m_sel_op.SetSideStyle( m_sel_op.GetNumSides()-1, CPolyLine::STRAIGHT, !m_cursor_mode );
						OnRangeCmds(NULL);
					}
				}
				m_Doc->ProjectModified( TRUE );
			}
			else if( m_cursor_mode == CUR_DRAG_OP_INSERT )
			{
				pDC = GetDC();
				SetDCToWorldCoords( pDC );
				pDC->SelectClipRgn( &m_pcb_rgn );

				CPoint p;
				p = m_last_cursor_point;
				m_dragging_mode = 0;
				m_sel_op.CancelDraggingToInsertCorner(m_sel_id.ii);
				SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
				int inext = m_sel_op.GetNextCornerIndex( m_sel_id.ii );
				m_sel_op.InsertCorner( inext, p.x, p.y, FALSE );
				m_sel_op.Draw();
				id sel_id( ID_POLYLINE, m_sel_id.st, m_sel_id.i, ID_CORNER, inext );
				CancelSelection();
				NewSelect( NULL, &sel_id, 1, 0 );
				OnRangeCmds( NULL );
				//RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
				//SwellRect( &r, abs(m_Doc->m_node_w/2) );
				//m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
				m_Doc->ProjectModified( TRUE );
			}
			else if( m_cursor_mode == CUR_DRAG_OP_MOVE )
			{
				pDC = GetDC();
				SetDCToWorldCoords( pDC );
				pDC->SelectClipRgn( &m_pcb_rgn );

				CPoint p;
				p = m_last_cursor_point;
				m_sel_op.CancelDraggingToMoveCorner(m_sel_id.ii);
				SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
				m_sel_op.MoveCorner( m_sel_id.ii, p.x, p.y, 0 );
				m_Doc->m_dlist->CancelHighLight();
				m_sel_op.Draw( m_Doc->m_dlist );
				int ret =  m_Doc->FindNodeLine(m_sel_id.i);
				HighlightGroup();
				if( ret >= 0 )
				{		
					RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
					SwellRect( &r, abs(m_Doc->m_node_w/2) );
					m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
				}
				SetCursorMode( CUR_OP_CORNER_SELECTED );			
				m_Doc->ProjectModified( TRUE );
			}
			else if( m_cursor_mode == CUR_DRAG_TEXT )
			{
				CPoint p;
				p = m_last_cursor_point;
				m_sel_text->m_tl->CancelDraggingText( m_sel_text );
				if( !m_dragging_new_item )
					SaveUndoInfoForText( m_sel_text, TRUE, m_Doc->m_undo_list );
				int old_angle = m_sel_text->m_angle;
				int angle = old_angle + m_Doc->m_dlist->GetDragAngle();
				if( angle>270 )
					angle = angle - 360;
				int layer = m_sel_text->m_layer;
				CTextList * tl = m_sel_text->m_tl;
				if( tl )
				{
					tl->MoveText( m_sel_text, m_last_cursor_point.x, m_last_cursor_point.y,
					angle, layer );
					m_dragging_new_item = FALSE;
					SetCursorMode( CUR_TEXT_SELECTED );
					m_Doc->m_dlist->CancelHighLight();
					tl->HighlightText( m_sel_text );
				}
				///m_Doc->ProjectModified( TRUE ); no mod
			}
			else if( m_cursor_mode == CUR_DRAG_MEASURE_1 )
			{
				m_measure_dist = 0;
				m_from_pt = m_last_cursor_point;
				m_Doc->m_dlist->MakeDragRatlineArray( 1, 1 );
				m_Doc->m_dlist->AddDragRatline( m_from_pt, zero ); 
				SetCursorMode( CUR_DRAG_MEASURE_2 );
			}
			else if( m_cursor_mode == CUR_DRAG_MEASURE_2 )
			{
				int dst = Distance( m_from_pt.x, m_from_pt.y, m_last_cursor_point.x, m_last_cursor_point.y );
				float cur_ang = Angle( m_from_pt.x, m_from_pt.y, m_last_cursor_point.x, m_last_cursor_point.y );
				m_measure_dist += dst;
				// line
				CPoint P[2];
				P[0] = m_from_pt;
				P[1] = m_last_cursor_point;
				dl_element * el = m_Doc->m_dlist->HighLight( DL_LINE, NULL, 1, P, 2, m_active_layer );
				el->transparent = 0;
				el->el_static = TRUE;
				// text
				int font_size = min( NM_PER_MIL*10, dst/10 );
				int stroke_width = font_size/6;
				CString str;
				::MakeCStringFromDimension( m_user_scale, &str, dst, m_Doc->m_units, 0, 0, 0 );
				CText * ht = m_Doc->m_tlist->AddText( m_last_cursor_point.x, m_last_cursor_point.y, 0/*angle*/, 
															  LAY_INFO_TEXT, font_size, stroke_width, m_Doc->m_default_font, &str );
				el = m_Doc->m_dlist->CopyToLayer( ht->dl_el, LAY_INFO_TEXT );
				el->transparent = 0;
				el->id.type = 0;
				el->el_static = TRUE;
				m_Doc->m_tlist->RemoveText( ht );
				//
				::MakeCStringFromDimension( m_user_scale, &str, m_measure_dist, m_Doc->m_units, 0, 0, 0 );
				ht = m_Doc->m_tlist->AddText( m_last_cursor_point.x, m_last_cursor_point.y-(font_size*2), 0/*angle*/, 
															  LAY_INFO_TEXT, font_size, stroke_width, m_Doc->m_default_font, &str );
				el = m_Doc->m_dlist->CopyToLayer( ht->dl_el, LAY_INFO_TEXT );
				el->transparent = 0;
				el->id.type = 0;
				el->el_static = TRUE;
				m_Doc->m_tlist->RemoveText( ht );
				if( prev_m_ang >= 0 )
				{
					float diff_ang = cur_ang - prev_m_ang;
					if( diff_ang < 0 )
						diff_ang += 360;
					if( diff_ang > 180 )
						diff_ang = 360 - diff_ang;
					diff_ang *= NM_PER_MM;
					::MakeCStringFromDimension( 1.0, &str, diff_ang, MM, 0, 0, 0, 2, 0 );
					ht = m_Doc->m_tlist->AddText( m_last_cursor_point.x, m_last_cursor_point.y-(font_size*4), 0/*angle*/, 
															  LAY_INFO_TEXT, font_size, stroke_width, m_Doc->m_default_font, &str );
					el = m_Doc->m_dlist->CopyToLayer( ht->dl_el, LAY_INFO_TEXT );
					el->transparent = 0;
					el->id.type = 0;
					el->el_static = TRUE;
					m_Doc->m_tlist->RemoveText( ht );
				}
				//
				prev_m_ang = cur_ang - 180;
				if( prev_m_ang < 0 )
					prev_m_ang += 360;
				m_from_pt = m_last_cursor_point;
				m_Doc->m_dlist->MakeDragRatlineArray( 1, 1 );
				m_Doc->m_dlist->AddDragRatline( m_from_pt, zero ); 
			}
		}
		goto goodbye;

cancel_selection_and_goodbye:
		CancelSelection( FALSE );
	}

goodbye:
	//OnInfoBoxSendMess( "part_list ");?
	ShowSelectStatus();
	
	// update window
	if( flag_scale_factor )
		m_draw_layer = 0;// CUR_SCALE_FACTOR
	else if( CurDragging() )
		m_draw_layer = LAY_BACKGND;// CurDragging()
	//
	if( pDC )
		ReleaseDC( pDC );
	Invalidate( FALSE );//OnLButtonUp
	CView::OnLButtonUp(nFlags, point);
}
//===============================================================================================
// left double-click
//
void CFreePcbView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#if 0
	if( m_cursor_mode == CUR_PART_SELECTED )
	{
		SetCursorMode( CUR_DRAG_PART );
		//CDC *pDC = GetDC();
		pDC->SelectClipRgn( &m_pcb_rgn );
		SetDCToWorldCoords( pDC );
		CPoint p = m_last_mouse_point;
		m_Doc->m_dlist->StartDraggingSelection( pDC, p.x, p.y );
	}
	if( m_cursor_mode == CUR_REF_SELECTED )
	{
		SetCursorMode( CUR_DRAG_REF );
		//CDC *pDC = GetDC();
		pDC->SelectClipRgn( &m_pcb_rgn );
		SetDCToWorldCoords( pDC );
		CPoint p = m_last_mouse_point;
		m_Doc->m_dlist->StartDraggingSelection( pDC, p.x, p.y );
	}
#endif
	CView::OnLButtonDblClk(nFlags, point);
}
//===============================================================================================
// right mouse button
//
void CFreePcbView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bRButtonDown = TRUE;
	m_disable_context_menu = 1;
	if( m_dragging_mode && m_cursor_mode == CUR_NONE_SELECTED )
	{
		m_dragging_mode = 0;
		m_Doc->m_dlist->StopDragging();
		m_left_pane_invalid = -1; // upd bottom pane !
		SetFKText( m_cursor_mode );
		SetMainMenu( TRUE ); // ( enablemenuitem )
	}
	else if( m_cursor_mode == CUR_DRAG_TEXT )
	{
		CTextList * tl = m_sel_text->m_tl;
		if( tl )
			tl->CancelDraggingText( m_sel_text );
		SetCursorMode( CUR_TEXT_SELECTED );
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
	}
	else if( m_cursor_mode == CUR_ADD_OP
		  || m_cursor_mode == CUR_DRAG_OP_1 )
	{
		// dragging first, second or third corner of outline
		// just delete it (if necessary) and cancel
		m_Doc->m_dlist->StopDragging();
		if( m_sel_id.type == ID_POLYLINE )
		{
			if( m_sel_id.ii == 0 )
			{
				m_sel_op.Hide();
				///m_Doc->OPRefreshID();
			}
			else if( m_cursor_mode != CUR_ADD_OP )
				m_sel_op.RemoveContour(m_sel_op.GetNumContour(m_sel_op.GetNumCorners()-1));
		}
		CancelSelection();
	}
	else if( m_cursor_mode == CUR_DRAG_OP )
	{
		//
		int node = m_Doc->FindNodeLine(m_sel_id.i);
		// dragging fourth or higher corner of outline, close it
		m_Doc->m_dlist->StopDragging();
		if( m_sel_id.ii < 2 )
			m_polyline_closed = 0;
		int nc = m_sel_op.GetNumContours();
		if( nc > 1 )
		{
			int nvtx = m_sel_op.GetNumCorners() - m_sel_op.GetContourStart( nc-1 );
			if( nvtx < 3 )
				m_sel_op.RemoveContour(m_sel_op.GetNumContour(m_sel_op.GetNumCorners()-1));
			else
				m_sel_op.Close( m_polyline_style );
		}
		else if( node >= 0 )
			m_sel_op.UnClose();
		else if( m_sel_op.GetNumContour(m_sel_id.ii) || m_polyline_closed )
			m_sel_op.Close( m_polyline_style );
		m_sel_op.Show();
		m_Doc->ProjectModified( TRUE );
		CPolyLine * p = &m_sel_op;
		CancelSelection();	
		RECT r = p->GetBounds();
		if( nc > 1 )
		{
			RECT rq = p->GetCornerBounds( nc-1 );
			SwellRect( &r, rq );
		}
		SwellRect( &r, abs(m_Doc->m_node_w/2) );
		m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
	}
	else if( m_cursor_mode == CUR_DRAG_OP_INSERT )
	{
		m_dragging_mode = 0;
		m_sel_op.CancelDraggingToInsertCorner(m_sel_id.ii);
		m_sel_op.HighlightSide( m_sel_id.ii );
		SetCursorMode( CUR_OP_SIDE_SELECTED );
	}
	else if( m_cursor_mode == CUR_DRAG_OP_MOVE )
	{
		// get indexes for preceding and following corners
		m_sel_op.CancelDraggingToMoveCorner(m_sel_id.ii);
		SetCursorMode( CUR_OP_CORNER_SELECTED );
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
	}
	else if( m_cursor_mode == CUR_DRAG_GROUP )
	{
		CancelDraggingGroup();
		if( m_dragging_mode )
		{
			RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
			CancelSelection();
			CDC * gDC = GetDC();
			m_Doc->m_dlist->StartDraggingArray( gDC, m_last_mouse_point.x, m_last_mouse_point.y, 1 );
			m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
			ReleaseDC( gDC );
		}
		else
		{
			m_Doc->m_dlist->CancelHighLight();
			HighlightGroup();
		}
	}
	else if( m_cursor_mode == CUR_DRAG_GROUP_ADD )
	{
		CancelDraggingGroup();
		m_Doc->OnEditUndo();
		m_disable_context_menu = 1;// was canceled in OnEditUndo !!!
	}
	else if( m_cursor_mode == CUR_DRAG_MEASURE_1 || m_cursor_mode == CUR_DRAG_MEASURE_2 )
	{
		m_dragging_mode = 0;
		m_Doc->m_dlist->StopDragging();
		SetCursorMode( CUR_NONE_SELECTED );
	}
	else if( m_protect == 0 || 
		m_cursor_mode == CUR_NONE_SELECTED ||
		m_cursor_mode == CUR_GROUP_SELECTED ||
		m_cursor_mode == CUR_TEXT_SELECTED)
	{
		m_disable_context_menu = 0;
	}
	ShowSelectStatus();
	CView::OnRButtonDown(nFlags, point);
	m_draw_layer = LAY_BACKGND;// OnRButtonDown
	Invalidate( FALSE ); //OnRButtonDown
}
//===============================================================================================
// System Key on keyboard pressed down
//
void CFreePcbView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == 121 )
		OnKeyDown( nChar, nRepCnt, nFlags);
	else
		CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
//===============================================================================================
// System Key on keyboard pressed up
//
void CFreePcbView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar != 121 )
		CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
}
//===============================================================================================
// Key pressed up
//
void CFreePcbView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_SHIFT || nChar == VK_CONTROL )
	{
		gShiftKeyDown = FALSE;
	}
	if( nChar == 'D' )
	{
		m_Doc->m_drelist->MakeHollowCircles();
		m_draw_layer = LAY_DRC_ERROR;// if( nChar == 'D' )
	}
	else if( nChar == VK_SHIFT || nChar == VK_CONTROL )
	{
		
	}
	else if( nChar == ' ' )
	{
		double old_x = m_org_x;
		double old_y = m_org_y;
		//
		CPoint mpt;
		GetCursorPos( &mpt );		// cursor pos in screen coords
		CPoint p = m_Doc->m_dlist->ScreenToPCB( mpt );	// convert to PCB coords
		// space bar pressed, center window on cursor then center cursor
		m_org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel)/2;
		m_org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel)/2;
		CRect screen_r;
		GetWindowRect( &screen_r );
		m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel,
			m_org_x, m_org_y );
		//
		CPoint newP = m_Doc->m_dlist->PCBToScreen( p );
		newP.x = (newP.x+mpt.x)/2;
		newP.y = (newP.y+mpt.y)/2;
		if( abs( newP.x-mpt.x ) > 10 || abs( newP.y-mpt.y ) > 10 )
		{
			SetCursorPos( newP.x, newP.y );
			UpdateWindow();
		}
		else
		{
			m_org_x = old_x;
			m_org_y = old_y;
			m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel,
				m_org_x, m_org_y );
		}
	}
	Invalidate( FALSE );//OnKeyUp
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}
//===============================================================================================
// Key pressed down
//
void CFreePcbView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_SHIFT || nChar == VK_CONTROL )
		gShiftKeyDown = TRUE;
	if( nChar == 'D' )
	{
		m_Doc->m_drelist->MakeSolidCircles();
		m_draw_layer = LAY_DRC_ERROR;// if( nChar == 'D' )
	}
	else if( nChar == VK_SHIFT || nChar == VK_CONTROL )
	{
		
	}
	else
	{
		HandleKeyPress( nChar, nRepCnt, nFlags );
	}
	Invalidate( FALSE );

	// don't pass through SysKey F10
	if( nChar != 121 )
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
//===============================================================================================
// Key on keyboard pressed down
//
void CFreePcbView::HandleKeyPress(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDC *pDC = GetDC();
	pDC->SelectClipRgn( &m_pcb_rgn );
	SetDCToWorldCoords( pDC );
	//
	// get cursor position and convert to PCB coords
	CPoint mpt;
	GetCursorPos( &mpt );		// cursor pos in screen coords
	CPoint p = m_Doc->m_dlist->ScreenToPCB( mpt );	// convert to PCB coords
	if( m_bDraggingRect )
		goto goodbye;
	if( nChar == VK_ESCAPE )
	{
		// ESC key, if something selected, cancel it
		// otherwise, fake a right-click
		int cs = CurSelected();
		if( m_page > 1 && cs )
		{
			m_page = 1;
			SetFKText(m_cursor_mode);
		}
		else if( cs )
		{
			CancelSelection();
			//OnInfoBoxSendMess( "part_list ");?
		}
		else
			OnRButtonDown( nFlags, CPoint(0,0) );
		goto goodbye;
	}
	if( nChar == 'M' && m_Doc->m_project_open )
	{
		if( !CurDragging() )
		{
			CancelSelection();
			SetCursorMode( CUR_DRAG_MEASURE_1 );
			m_Doc->m_dlist->StartDraggingArray( pDC, m_last_mouse_point.x, m_last_mouse_point.y, 1 ); 
		}
		else if( m_cursor_mode == CUR_DRAG_MEASURE_1 || m_cursor_mode == CUR_DRAG_MEASURE_2 )
		{
			m_dragging_mode = 0;
			m_Doc->m_dlist->StopDragging();
			SetCursorMode( CUR_NONE_SELECTED );
		}
	}
	if( nChar == 'L' )
	{
		m_Doc->OnViewLog();
	}
	else if( nChar == 'H' )
	{
		m_Doc->OnEditSelectAll();
	}
	else if( nChar == 'O' )
	{
		OnSetOriginToSelectedItem();
	}
	else if( nChar == 'N' )
	{
		if( m_sel_text )
			m_Doc->HighLightNet(m_sel_text);
		else if( m_sel_id.type == ID_POLYLINE )
			if( m_sel_op.Check( index_net_attr ) )
				m_Doc->HighLightNet( m_sel_op.pAttr[index_net_attr] );
	}
	else if( nChar == 'R' )
	{
		m_Doc->OnReloadMenu();
	}
	if( nChar == VK_BACK )
	{
		goto goodbye;
	}
	int fk = FK_NONE;
	int dx = 0;
	int dy = 0;

	// test for pressing key to change layer
	char test_char = nChar;
	if( test_char >= 97 )
		test_char = '1' + nChar - 97;
	//char * ch = strchr( layer_char, nChar );

	// continue
	m_fk_button_index = -1;
	if( nChar >= 112 && nChar <= 123 )
	{
		// function key pressed
		if( m_fk_button_index == -1 )
			m_fk_button_index = nChar-112;
		fk = m_fkey_option[nChar-112];
	}
	if( nChar >= VK_LEFT && nChar <= VK_DOWN )
	{
		// arrow key
		BOOL bShift;
		SHORT kc = GetKeyState( VK_SHIFT );
		if( kc < 0 )
			bShift = TRUE;
		else
			bShift = FALSE;
		fk = FK_ARROW;
		int d;
		if( bShift && m_Doc->m_units == MM )
			d = 10000;		// 0.01 mm
		else if( bShift && m_Doc->m_units == MIL )
			d = 25400;		// 1 mil
		else if( m_sel_flags == TEXT_ONLY && m_Doc->m_routing_grid_spacing > 999 )
			d = m_Doc->m_routing_grid_spacing/m_user_scale;
		else
			d = m_Doc->m_part_grid_spacing/m_user_scale;
		if( nChar == VK_LEFT )
			dx -= d;
		else if( nChar == VK_RIGHT ) 
			dx += d;
		else if( nChar == VK_UP ) 
			dy += d;
		else if( nChar == VK_DOWN ) 
			dy -= d;

		if( (int)abs(m_ang_axis+0.5)%90 > 0 )
		if( dx )
		{
			dy = (double)dx * sin( m_ang_axis * M_PI / 180.0 ) / cos( m_ang_axis * M_PI / 180.0 );
			if( abs(dy) > abs(dx)*3 )
				dx = dy = 0;
		}
		else
		{
			dx = (double)dy * cos( m_ang_axis * M_PI / 180.0 ) / sin( m_ang_axis * M_PI / 180.0 );
			if( abs(dx) > abs(dy)*3 )
				dx = dy = 0;
		}
	}
	else
	{
		gLastKeyWasArrow = FALSE;
		if( CurDragging() == 0 )
			if( nChar >= '1' && nChar <= '8' )
				m_Doc->SwitchToPage((int)nChar-(int)'1',TRUE);
	}

	if( fk == FK_ARROW )
	{
		m_draw_layer = LAY_BACKGND;// FK_ARROW
		if( m_cursor_mode == CUR_OP_SIDE_SELECTED )
			SetCursorMode(CUR_GROUP_SELECTED);
	}
	else if( m_fk_button_index >= 0 )
	{
		DrawBottomPane();
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		frm->m_timer = frm->SetTimer( TMR_FK_PRESS, 150, 0 );
	}
	switch( m_cursor_mode )
	{
	case  CUR_NONE_SELECTED:
		if( fk == FK_IMPORT_PART )
			OnPasteSinglePart();
		else if( fk == FK_DRAG_LINE )
		{
			m_dragging_mode = FK_DRAG_LINE;
			SetFKText( CUR_NONE_SELECTED );
			m_Doc->m_dlist->StartDraggingArray( pDC, m_last_mouse_point.x, m_last_mouse_point.y, 1 );
			ReleaseDC( pDC );
			Invalidate( FALSE );// (doubtful)
		}
		if( fk == FK_DRAG_GROUP )
		{
			m_dragging_mode = FK_DRAG_GROUP;
			SetFKText( CUR_NONE_SELECTED );
			m_Doc->m_dlist->StartDraggingArray( pDC, m_last_mouse_point.x, m_last_mouse_point.y, 1 );
			ReleaseDC( pDC );
			Invalidate( FALSE );// (doubtful)
		}
		else if( fk == FK_ADD_LINE )
		{
			m_polyline_layer = 0;
			m_polyline_width = abs(m_Doc->m_polyline_w);
			AddOutlinePoly(0);
		}
		else if( fk == FK_REPEAT_LINE )
		{
			if( m_polyline_layer < LAY_ADD_1 )
				m_polyline_layer = LAY_FREE_LINE;
			AddOutlinePoly(TRUE);
		}
		else if( fk == FK_SHOW_M )
		{
			OnViewMerges();
		}
		else if( fk == FK_PREV_PAGE )
		{
			m_Doc->SwitchToPage( m_Doc->Pages.GetActiveNumber() - 1 );
		}
		else if( fk == FK_NEXT_PAGE )
		{
			m_Doc->SwitchToPage( m_Doc->Pages.GetActiveNumber() + 1 );
		}
		else if( fk == FK_GRID_STYLE )
		{
			if( m_Doc->m_dlist )
				m_Doc->m_dlist->SetGridStyle(1-m_Doc->m_dlist->GetGridStyle());
			m_draw_layer = 0;// FK_GRID_STYLE
		}
		else if( fk == FK_REDO_RATLINES )
		{
			OnClearDRC();
			m_Doc->CreatePCBNets();
			m_draw_layer = 0;// FK_REDO_RATLINES
		}
		else if( fk == FK_OPEN_FOLDER )
		{
			int ip = m_Doc->m_pcb_full_path.ReverseFind('\\');
			CString path = m_Doc->m_pcb_full_path.Left(ip);
			ShellExecute(	NULL,"open",path,NULL,path,SW_SHOWNORMAL);
		}
		else if( fk == FK_COMPONENT_NOTES )
		{
			m_Doc->OnOpenComponentDatasheets();
		}
		break;

	case CUR_TEXT_SELECTED:
		if( fk == FK_ARROW )
		{
			if( !gLastKeyWasArrow )
			{
				SaveUndoInfoForText( m_sel_text, TRUE, m_Doc->m_undo_list );
				gTotalArrowMoveX = 0;
				gTotalArrowMoveY = 0;
				gLastKeyWasArrow = TRUE;
			}
			//
			CTextList * tl = m_sel_text->m_tl;
			if( tl )
			{
				RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
				m_Doc->m_dlist->CancelHighLight();
				tl->MoveText( m_sel_text,
						m_sel_text->m_x + dx, m_sel_text->m_y + dy,
						m_sel_text->m_angle, m_sel_text->m_layer );

				gTotalArrowMoveX += dx;
				gTotalArrowMoveY += dy;
				ShowRelativeDistance( m_sel_text->m_x, m_sel_text->m_y, 
				gTotalArrowMoveX, gTotalArrowMoveY );
				tl->HighlightText( m_sel_text );
				m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
				///m_Doc->ProjectModified( TRUE ); no mod
			}
		}
		else if( fk == FK_EDIT_TEXT )
			OnTextEdit();
		else if( fk == FK_MOVE_TEXT )
			OnTextMove();
		else if( fk == FK_ROTATE_GROUP_5 )
		{
			SaveUndoInfoForText( m_sel_text, TRUE, m_Doc->m_undo_list );
			RECT r;
			m_sel_text->m_tl->GetTextRectOnPCB( m_sel_text, &r );
			m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
			m_sel_text->m_angle += 5;
			if( m_sel_text->m_angle >= 360 )
				m_sel_text->m_angle -= 360;
			m_sel_text->Undraw();
			m_sel_text->Draw( m_Doc->m_dlist, m_Doc->m_smfontutil );
			CTextList * tl = m_sel_text->m_tl;
			if( tl )
				tl->HighlightText( m_sel_text );
			m_Doc->ProjectModified( TRUE );
			m_draw_layer = LAY_BACKGND;// FK_ROTATE_GROUP_5
		}
		else if( fk == FK_ROTATE_GROUP__5 )
		{
			SaveUndoInfoForText( m_sel_text, TRUE, m_Doc->m_undo_list );
			RECT r;
			m_sel_text->m_tl->GetTextRectOnPCB( m_sel_text, &r );
			m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
			m_sel_text->m_angle -= 5;
			if( m_sel_text->m_angle < 0 )
				m_sel_text->m_angle += 360;
			m_sel_text->Undraw();
			m_sel_text->Draw( m_Doc->m_dlist, m_Doc->m_smfontutil );
			CTextList * tl = m_sel_text->m_tl;
			if( tl )
				tl->HighlightText( m_sel_text );
			m_Doc->ProjectModified( TRUE );
			m_draw_layer = LAY_BACKGND;// FK_ROTATE_GROUP__5
		}
		else if( fk == FK_ALIGN_X || fk == FK_ALIGN_Y )
		{
			if (ppx||ppy)
			{
				SaveUndoInfoForText( m_sel_text, TRUE, m_Doc->m_undo_list );
				RECT r;
				m_sel_text->m_tl->GetTextRectOnPCB( m_sel_text, &r );
				m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
				if( fk == FK_ALIGN_X )
					m_sel_text->m_tl->MoveText( m_sel_text, ppx, m_sel_text->m_y, m_sel_text->m_angle, m_sel_text->m_layer );
				else if( fk == FK_ALIGN_Y )
					m_sel_text->m_tl->MoveText( m_sel_text, m_sel_text->m_x, ppy, m_sel_text->m_angle, m_sel_text->m_layer );
				prevx = ppx;
				prevy = ppy;
				m_sel_text->m_tl->HighlightText( m_sel_text );
				///m_Doc->ProjectModified( TRUE ); no mod
				m_draw_layer = LAY_BACKGND;
			}
		}
		else if( fk == FK_SWITCH_TO_PCB )
		{
			m_Doc->SwitchToPCB();
		}
		else if( fk == FK_CHANGE_LAYER )
		{
			if( m_sel_text->m_layer == LAY_PIN_DESC )
			{
				if( AfxMessageBox(G_LANGUAGE == 0 ? 
					"Are you sure you want to change the text color layer to a last polyline layer?":
					"Вы уверены, что хотите изменить слой текста на последний слой полилинии, по которой кликнули до этого?", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
					if( m_polyline_layer >= LAY_ADD_1 )
						for( int ip=0; ip<m_Doc->m_outline_poly->GetSize(); ip++ )
							if( m_Doc->m_outline_poly->GetAt(ip).Check( index_desc_attr ) == m_sel_text )
								m_sel_text->m_layer = m_polyline_layer;
			}
			else if( AfxMessageBox(G_LANGUAGE == 0 ? 
				"Are you sure you want to reset the text color layer?":
				"Вы уверены, что хотите сбросить цветовой слой текста? (сделать слой текста по умолчанию)", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
				m_sel_text->m_layer = LAY_PIN_DESC;
			m_sel_text->MakeVisible();
			CancelSelection();
			OnRangeCmds(NULL);
		}
		break;

	case CUR_OP_CORNER_SELECTED:
		if( fk == FK_ARROW )
		{
			if( !gLastKeyWasArrow )
			{
				SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
				gTotalArrowMoveX = 0;
				gTotalArrowMoveY = 0;
				gLastKeyWasArrow = TRUE;
			}
			RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
			CPolyLine * poly = &m_sel_op;
			if( m_protect == 0 )
				poly->MoveCorner( m_sel_is,
				poly->GetX( m_sel_is ) + dx,
				poly->GetY( m_sel_is ) + dy );
			//
			m_Doc->m_dlist->CancelHighLight();
			if( m_Doc->FindNodeLine( m_sel_ic ) >= 0 )
				SwellRect( &r, abs(m_Doc->m_node_w/2) );
			gTotalArrowMoveX += dx;
			gTotalArrowMoveY += dy;
			ShowRelativeDistance( poly->GetX( m_sel_is ), poly->GetY( m_sel_is ),
				gTotalArrowMoveX, gTotalArrowMoveY );
			HighlightGroup();
			m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_SET_POSITION )
			OnOPCornerEdit();
		else if (fk == FK_MAKE_FIRST)
		{
			int answ = IDYES;
			if (m_Doc->m_outline_poly->GetAt(m_sel_ic).GetClosed() == 0)
			{
				if (G_LANGUAGE)
					answ = AfxMessageBox("Эта функция сделает выбранную вершину полилинии первой, "\
									"а для этого нужно чтобы полилиния была замкнутой.Хотите замкнуть "\
									"данную полилинию и затем выполнить эту функцию?", MB_YESNO);
				else
					answ = AfxMessageBox("This function will make the selected vertex of the polyline "\
									"the first one, and for this to happen, the polyline must be closed. "\
									"Do you want to close this polyline and then execute this function ? ", MB_YESNO);
			}
			if (answ == IDYES)
			{
				SaveUndoInfoForOutlinePoly(m_sel_ic, TRUE, m_Doc->m_undo_list);
				m_Doc->m_outline_poly->GetAt(m_sel_ic).Close();
				m_Doc->m_outline_poly->GetAt(m_sel_ic).MakeFirst(m_sel_is);
				//m_Doc->m_outline_poly->GetAt(m_sel_ic).MakeVisible();
				m_sel_is = 0;
				m_Doc->m_dlist->CancelHighLight();
				HighlightGroup();
				m_Doc->ProjectModified(TRUE);
				OnRangeCmds(NULL);
			}
		}
		else if( fk == FK_ALIGN_SIDES )
		{
			AlignSides(m_sel_id.type, m_sel_id.i, m_sel_id.ii);
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_MOVE_CORNER )
			OnOPCornerMove();
		else if(	fk == FK_ALIGN_X || 
					fk == FK_ALIGN_Y ||
					fk == FK_ALIGN_MIDDLE ||
					fk == FK_ALIGN_MIDDLE_X ||
					fk == FK_ALIGN_MIDDLE_Y )
		{
			if ( fk == FK_ALIGN_MIDDLE && m_page == 1 )
			{
				m_page = 2;
				SetFKText(m_cursor_mode);
			}
			else if ((ppx||ppy)&&(prevx||prevy))
			{
				SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
				m_Doc->m_dlist->CancelHighLight();
				int mer = m_sel_op.GetMerge();
				if( mer >= 0 )
				{
					CString ps;
					ps.Format(G_LANGUAGE == 0 ? 
						"This polyline is connected to other objects through the \"MERGE\" property. Move all objects of the group %s?":
						"Эта полилиния связана с другими объектами через свойство «СЛИЯНИЕ». Переместить все объекты группы %s?", m_Doc->m_mlist->GetStr(mer));
					if( AfxMessageBox( ps, MB_YESNO ) == IDNO )
					{
						mer = -1;
					}
				}
				int bx = m_sel_op.GetX(m_sel_id.ii);
				int by = m_sel_op.GetY(m_sel_id.ii);
				if( mer == -1 )
				{
					if( m_sel_op.Check( index_part_attr ) && getbit( m_sel_mask, SEL_MASK_PART ) )
					{
						m_Doc->SelectPart( m_sel_op.pAttr[index_part_attr], NULL, 0 );
						m_Doc->SelectGroupAttributes();
					}
					else
					{
						CString ps;
						ps.Format(G_LANGUAGE == 0 ? "Move entire contour?":"Переместить контур целиком?");
						if( AfxMessageBox( ps, MB_YESNO ) == IDYES )
						{
							//NewSelect( NULL, &m_sel_id, 0, 0 );
							SelectContour();
							m_Doc->SelectLineAttributes( &m_sel_op );
						}
					}
				}
				else 
				{
					CancelSelection();
					NewSelectM(mer);
					m_Doc->SelectGroupAttributes();
				}
				if( fk == FK_ALIGN_X)
				{
					if( m_sel_count > 1 )
						MoveGroup(ppx-bx,prevy-by);
					else
						m_sel_op.MoveCorner( m_sel_id.ii, ppx, prevy );
					prevx = ppx;
					prevy = ppy;
				}
				else if( fk == FK_ALIGN_Y)
				{
					if( m_sel_count > 1 )
						MoveGroup(prevx-bx,ppy-by);
					else
						m_sel_op.MoveCorner( m_sel_id.ii, prevx, ppy );
					prevx = ppx;
					prevy = ppy;
				}
				else if( fk == FK_ALIGN_MIDDLE_X)
				{
					if( m_sel_count > 1 )
						MoveGroup(prev_middle_x-bx,prevy-by);
					else
						m_sel_op.MoveCorner( m_sel_id.ii, prev_middle_x, prevy );
					prevx = ppx;
				}
				else if( fk == FK_ALIGN_MIDDLE_Y)
				{
					if( m_sel_count > 1 )
						MoveGroup(prevx-bx,prev_middle_y-by);
					else
						m_sel_op.MoveCorner( m_sel_id.ii, prevx, prev_middle_y );
					prevy = ppy;
				}
				else 
				{
					if( m_sel_count > 1 )
						MoveGroup(prev_middle_x-bx,prev_middle_y-by);
					else
						m_sel_op.MoveCorner( m_sel_id.ii, prev_middle_x, prev_middle_y );
					prevx = ppx;
					prevy = ppy;
				}
				m_Doc->m_dlist->CancelHighLight();
				if( m_sel_count == 1 )
					m_sel_op.HighlightCorner( m_sel_id.ii);
				else
				{
					SetCursorMode(CUR_GROUP_SELECTED);
					HighlightGroup();
				}
				m_Doc->ProjectModified( TRUE );
				m_page = 1;
				SetFKText( m_cursor_mode );
			}
			m_draw_layer = 0;// FK_ALIGN
		}
		else if( fk == FK_ALIGN_MIDLINE )
		{
			int ip = m_sel_op.GetPrevCornerIndex(m_sel_id.ii);
			int in = m_sel_op.GetNextCornerIndex(m_sel_id.ii);
			int x1 = m_sel_op.GetX(ip);
			int y1 = m_sel_op.GetY(ip);
			int x2 = m_sel_op.GetX(in);
			int y2 = m_sel_op.GetY(in);
			m_sel_op.MoveCorner( m_sel_id.ii, (x1+x2)/2, (y1+y2)/2 );
			prevx = ppx;
			prevy = ppy;
			m_Doc->m_dlist->CancelHighLight();
			m_sel_op.HighlightCorner( m_sel_id.ii);
			m_page = 1;
			SetFKText( m_cursor_mode );
			m_draw_layer = 0;// FK_ALIGN_MIDLINE
		}
		else if( fk == FK_DELETE_CORNER || nChar == VK_DELETE )
			OnOPCornerDelete();
		else if( fk == FK_DELETE_OUTLINE )
			OnGroupDelete();
		else if( fk == FK_CIRC_ALIGN )
		{
			long long bx = m_sel_op.GetX(m_sel_id.ii);
			long long by = m_sel_op.GetY(m_sel_id.ii);
			if( abs(by) < m_pcbu_per_wu )
			{
				// уже выровнено
				//AfxMessageBox( "" );
				break;
			}
			int mer = m_sel_op.GetMerge();
			if( mer >= 0 )
			{
				CString ps;
				ps.Format(G_LANGUAGE == 0 ?
					"This polyline is connected to other objects through the \"MERGE\" property. Move all objects of the group %s?" :
					"Эта полилиния связана с другими объектами через свойство «СЛИЯНИЕ». Переместить все объекты группы %s?", m_Doc->m_mlist->GetStr( mer ) );
				if( AfxMessageBox( ps, MB_YESNO ) == IDNO )
				{
					mer = -1;
				}
			}
			if( mer == -1 )
			{
				if( m_sel_op.Check( index_part_attr ) && getbit( m_sel_mask, SEL_MASK_PART ) )
				{
					m_Doc->SelectPart( m_sel_op.pAttr[index_part_attr], NULL, 0 );
					m_Doc->SelectGroupAttributes();
				}
				else
				{
					CString ps;
					ps.Format(G_LANGUAGE == 0 ? "Move entire contour?" : "Переместить контур целиком?");
					if( AfxMessageBox( ps, MB_YESNO ) == IDYES )
					{
						SelectContour();
						m_Doc->SelectLineAttributes( &m_sel_op );
					}
				}
			}
			else 
			{
				CancelSelection();
				NewSelectM(mer);
				m_Doc->SelectGroupAttributes();
			}
			int ret = AfxMessageBox(G_LANGUAGE == 0 ? "Clockwise rotate objects?":"Поворачивать объекты по часовой стрелке?", MB_ICONQUESTION | MB_YESNO);
			if( (ret == IDYES && (by*bx) > 0 ) || 
				(ret == IDNO && (by*bx) < 0 ) )
			{
				double ang = atan( (double)by/(double)bx );
				ang = ang*180.0/M_PI;
				int ian = ang;
				ang = ang - (double)ian;
				RotateGroup( -ian, 1, 0, -ang );
			}
			else
			{
				double ang = atan( (double)bx/(double)by );
				ang = ang*180.0/M_PI;
				int ian = ang;
				ang = ang - (double)ian;
				RotateGroup( ian, 1, 0, ang );
			}
			SetCursorMode(CUR_GROUP_SELECTED);
			HighlightGroup();
			m_Doc->ProjectModified( TRUE );
			m_draw_layer = 0;
		}
		else if (fk == FK_ADD_NODE)
		{
			if (m_sel_op.Node[m_sel_id.ii])
				m_sel_op.Node[m_sel_id.ii] = 0;
			else
				m_sel_op.Node[m_sel_id.ii] = GetStandartNode(m_Doc, m_sel_id.i);
			m_sel_op.Draw();
			m_draw_layer = 0;
		}
		break;

	case CUR_OP_SIDE_SELECTED:
		if( fk == FK_POLY_STRAIGHT )
			OnOPSideConvertToStraightLine();
		else if( fk == FK_POLY_ARC_CW )
			OnOPSideConvertToArcCw();
		else if( fk == FK_POLY_ARC_CCW )
			OnOPSideConvertToArcCcw();
		else if( fk == FK_ADD_CORNER )
			OnOPSideAddCorner();
		else if( fk == FK_DRAG_LINE )
			OnGroupMove();
		else if( fk == FK_SET_ATTRIB )
		{
			OnOPSetAttributes();
			if( getbit( m_Doc->Pages.m_page_mirror_mask, m_Doc->Pages.GetActiveNumber() ) )
				m_Doc->AttributeSync();
		}
		else if( fk == FK_SELECT_CONTOUR )
			SelectContour();
		else if( fk == FK_DELETE_OUTLINE || nChar == VK_DELETE || fk == FK_DELETE_CUTOUT )
		{
			int num_c = m_sel_op.GetNumContour(m_sel_id.ii);
			if( num_c == 0 || fk == FK_DELETE_OUTLINE )
			{
				SelectContour();
				OnGroupDelete();
			}
			else 
			{
				SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
				int nc = m_sel_op.GetNumContour(m_sel_id.ii);
				m_sel_op.RemoveContour(nc);
				CancelSelection();
				m_draw_layer = 0;// FK_DELETE_OUTLINE
				m_Doc->ProjectModified( TRUE );
			}
		}
		else if( fk == FK_SET_WIDTH_HATCH_LAYER )
			EditPolylineProperties();
		else if( fk == FK_INSIDE_POLYLINE )
			OnInsidePolyline();
		else if( fk == FK_COPY_CUT )
		{
			int num_c = m_sel_op.GetNumContour(m_sel_id.ii);
			int start = m_sel_op.GetContourStart(num_c);
			int end = m_sel_op.GetContourEnd(num_c);
			for( int ii=start; ii<=end; ii++ )
			{
				int c_back = m_sel_op.GetPrevCornerIndex(ii);
				m_sel_op.AppendCorner( m_sel_op.GetX(ii)+1000000,
																m_sel_op.GetY(ii)-1000000,
																m_sel_op.GetSideStyle(c_back), FALSE );
			}
			m_sel_op.Close(m_sel_op.GetSideStyle(end));
			m_sel_op.SetSideSel(m_sel_id.ii,0);
			m_sel_id.ii = m_sel_op.GetNumCorners()-1;
			m_sel_op.SetSideSel(m_sel_id.ii,1);
			m_Doc->m_dlist->CancelHighLight();
			SelectContour();
		}
		else if( fk == FK_SET_LENGTH || fk == FK_ADD_SIZE )
		{
			int n = m_sel_op.GetNextCornerIndex(m_sel_id.ii);
			double x1 = m_sel_op.GetX(m_sel_id.ii);
			double y1 = m_sel_op.GetY(m_sel_id.ii);
			double x2 = m_sel_op.GetX(n);
			double y2 = m_sel_op.GetY(n);
			double ddx = x2 - x1;
			double ddy = y2 - y1;
			//int len = Distance( x1, y1, x2, y2 );
			if( fk == FK_ADD_SIZE )
			{
				AddGraphicSize(m_Doc, "");
				SetCursorMode( CUR_GROUP_SELECTED );
				HighlightGroup();
			}
			//else if( abs(ddx) < _2540 || abs(ddy) < _2540)
			//{
			//}
			else if( fk == FK_SET_LENGTH )
			{	
				CDlgAddMerge dlg;
				dlg.Doc = m_Doc;
				dlg.m_mode = dlg.M_SET_POLY_LENGTH;
				int ret = dlg.DoModal();
				if( ret == IDOK )
				{
					SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
					double val = my_atof( &dlg.m_merge_name );
					if( dlg.m_merge_name.FindOneOf("mM") == -1 )
					{
						if( m_Doc->m_units == MIL )
							val *= NM_PER_MIL;
						else if( m_Doc->m_units == MM )
							val *= NM_PER_MM;
					}
					val /= m_user_scale;
					ret = AfxMessageBox(G_LANGUAGE == 0 ? 
						"Do you want to save the segment angle?":
						"Хотите сохранить угол сегмента?", MB_ICONQUESTION | MB_YESNOCANCEL);
					if( ret == IDYES )
					{
						// keep the seg angle
						float quad = Angle( ddx, ddy, 0, 0 );
						x2 = x1 + val*cos( quad*M_PI/180.0 );
						y2 = y1 + val*sin( quad*M_PI/180.0 );
						m_sel_op.SetX(n,x2);
						m_sel_op.SetY(n,y2);
						m_sel_op.Draw();
					}
					else if( ret == IDNO )
					{
						ret = AfxMessageBox(G_LANGUAGE == 0 ? 
							("Do you want to keep the X coordinates "\
							"of the vertices of the selected segment unchanged "\
							"when performing this procedure? \n\n(Click YES for the "\
							"program to complete this task by changing the Y "\
							"coordinates of the vertices. \nClick NO for the program to "\
							"complete the task by changing the X coordinates of the vertices)"):
							("Сохранить координаты X "\
							"вершин выбранного сегмента неизменными "\
							"при выполнении этой функции? \n\n(Нажмите ДА, чтобы программа "\
							"сделала это, изменив только координаты Y "\
							"вершин. \nНажмите НЕТ, чтобы программа "\
							"выполнила задачу, изменив только координаты X вершин)"), MB_ICONQUESTION | MB_YESNOCANCEL);
						if( ret == IDYES )
						{
							// keep the X coord
							SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
							double quad = pow(val,2) - pow(ddx,2);
							if( quad < 0.0 )
							{
								y2 = y1;
								if( ddx > 0.0 )
									x2 = x1 + val;
								else
									x2 = x1 - val;
							}
							else
							{
								if( ddy > 0.0 )
									y2 = y1 + sqrt(quad);
								else
									y2 = y1 - sqrt(quad);
							}
							m_sel_op.SetX(n,x2);
							m_sel_op.SetY(n,y2);
							m_sel_op.Draw();
						}
						else if( ret == IDNO )
						{
							// keep the Y coord
							SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
							double quad = pow(val,2) - pow(ddy,2);
							if( quad < 0.0 )
							{
								x2 = x1;
								if( ddy > 0.0 )
									y2 = y1 + val;
								else
									y2 = y1 - val;
							}
							else
							{
								if( ddx > 0.0 )
									x2 = x1 + sqrt(quad);
								else
									x2 = x1 - sqrt(quad);
							}
							m_sel_op.SetX(n,x2);
							m_sel_op.SetY(n,y2);
							m_sel_op.Draw();
						}
					}
					CancelSelection();
					OnRangeCmds(NULL);
				}
			}
		}
		break;

	case CUR_DRE_SELECTED:
		if( nChar == VK_DELETE )
		{	
			m_Doc->m_drelist->Remove( m_sel_dre );
			CancelSelection();
		}
		break;

	case CUR_GROUP_SELECTED:
		if( fk == FK_ARROW && m_protect == 0 )
		{
			CText * BOM = NULL;
			if( m_sel_count > 1 )
				m_Doc->SelectGroupAttributes();
			else if( m_sel_id.type == ID_POLYLINE )
			{
				if( m_sel_op.Check(index_desc_attr) )
					if( m_sel_op.pAttr[index_desc_attr]->m_str.Right(3) == "BOM" )
						BOM = m_sel_op.pAttr[index_desc_attr];
				if( m_sel_op.GetNumSides() == 1 )
					m_Doc->SelectGroupAttributes();
			}
			HighlightGroup();
			RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
			if( !gLastKeyWasArrow )
			{
				gTotalArrowMoveX = 0;
				gTotalArrowMoveY = 0;
				gLastKeyWasArrow = TRUE;
			}
			MoveGroup( dx, dy, BOM );
			gTotalArrowMoveX += dx;
			gTotalArrowMoveY += dy;
			m_Doc->m_dlist->CancelHighLight();
			HighlightGroup();
			m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
			ShowRelativeDistance( gTotalArrowMoveX, gTotalArrowMoveY );
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_SET_WIDTH_HATCH_LAYER )
			EditPolylineProperties();
		else if( fk == FK_EDIT_TEXT )
			OnTextEdit();
		else if( fk == FK_SELECT_CONTOUR )
			SelectContour();
		else if( fk == FK_MOVE_GROUP )
		{
			RECT wR;
			CPoint cPos;
			GetWindowRect(&wR);
			GetCursorPos(&cPos);
			if( cPos.y > (wR.top+m_client_r.bottom-m_bottom_pane_h) )
			{
				CPoint mpnt;
				RECT r;
				GetWindowRect( &r );
				mpnt.x = (r.left + r.right)/2;
				mpnt.y = (r.top + r.bottom)/2;
				SetCursorPos( mpnt.x, mpnt.y );
				m_last_cursor_point = m_Doc->m_dlist->ScreenToPCB(mpnt);
				m_last_mouse_point = m_last_cursor_point;
			}
			OnGroupMove();
		}
		else if ( fk == FK_SIDE )
			TurnGroup();
		else if ( fk == FK_TAKE_APART )
			ExplodeGroup();
		else if ( fk == FK_MERGE_GROUP )
			MergeGroup();
		else if( fk == FK_DELETE_GROUP || nChar == VK_DELETE )
			DeleteGroup(0);
		else if ( fk == FK_DEL_WITH_MERGE )
			DeleteGroup(1);
		else if(fk == FK_ROTATE_GROUP)
		{
			m_page = 3;
			SetFKText( m_cursor_mode );
		}
		else if(fk == FK_ROTATE_GROUP_1)
			RotateGroup(1);
		else if(fk == FK_ROTATE_GROUP_5)
			RotateGroup(5);
		else if(fk == FK_ROTATE_GROUP_45)
			RotateGroup(45);
		else if(fk == FK_ROTATE_GROUP_90)
			RotateGroup(90);
		else if(fk == FK_ROTATE_GROUP__1)
			RotateGroup(-1);
		else if(fk == FK_ROTATE_GROUP__5)
			RotateGroup(-5);
		else if(fk == FK_ROTATE_GROUP__45)
			RotateGroup(-45);
		else if(fk == FK_ROTATE_GROUP__90)
			RotateGroup(-90);
		else if(fk == FK_SET_CLEARANCE)
			OnSetClearance();
		else if (fk == FK_RADIUS_DOWN)
		{
			if( prev_sel_count != m_sel_count )
				SaveUndoInfoForGroup( m_Doc->m_undo_list ); // FK_RADIUS_DOWN
			TracesRadiusUpDown(FALSE);
			m_Doc->ProjectModified( TRUE );
		}
		else if (fk == FK_RADIUS_UP)
		{
			if( prev_sel_count != m_sel_count )	
				SaveUndoInfoForGroup( m_Doc->m_undo_list ); // FK_RADIUS_UP
			TracesRadiusUpDown(TRUE);
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_CLEARANCE_UP )
		{
			if( m_Doc->m_routing_grid_spacing > 999 )
				m_seg_clearance += m_Doc->m_routing_grid_spacing/m_user_scale;
			else
				m_seg_clearance += m_Doc->m_part_grid_spacing/m_user_scale;
			OnSetClearance();
		}
		else if( fk == FK_CLEARANCE_DOWN )
		{
			if( m_Doc->m_routing_grid_spacing > 999 )
				m_seg_clearance -= m_Doc->m_routing_grid_spacing/m_user_scale;
			else
				m_seg_clearance -= m_Doc->m_part_grid_spacing/m_user_scale;
			OnSetClearance();
		}
		else if( fk == FK_OK )
		{
			if ( m_sel_count )
			{
				m_page = 1;
				SetFKText(m_cursor_mode);
			}
			else
				SetCursorMode(CUR_NONE_SELECTED);
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_SET_ATTRIB )
		{
			if( m_sel_text )
				if( m_sel_text->m_polyline_start >= 0 )
				{
					id ID( ID_POLYLINE, ID_GRAPHIC, m_sel_text->m_polyline_start, ID_SIDE, 0 );
					id IDT( ID_TEXT_DEF );
					UnSelect( m_sel_text, &IDT );	
					NewSelect( NULL, &ID, 0,0 );
					SetFKText( m_cursor_mode );
				}
			UnSelVertices();//!
			if( m_sel_id.type == ID_POLYLINE )
			{
				CText * BLK_T = NULL;
				if( CText * cmd = m_sel_op.Check(index_desc_attr) )
					if( cmd->m_str.Right(3) == "BLK" )
						BLK_T = cmd;
				if( BLK_T )
					OnOPSetAttributes();
				else if( m_sel_count == m_sel_op.GetNumSides() )
					OnOPSetAttributes();
				else
					MultipleAttributes();
			}
			else
				MultipleAttributes();
			if( getbit( m_Doc->Pages.m_page_mirror_mask, m_Doc->Pages.GetActiveNumber() ) )
				m_Doc->AttributeSync();
			if( m_dlg_partlist )
				if( m_dlg_partlist->IsWindowVisible() )
					m_dlg_partlist->ImportPartlist();
			m_draw_layer = 0;
			Invalidate( FALSE );// (doubtful)
		}
		else if( fk == FK_ALIGN_SIDES )
		{
			for( int ii=0; ii<m_Doc->m_outline_poly->GetSize(); ii++ )
				for( int iii=0; iii<m_Doc->m_outline_poly->GetAt(ii).GetNumCorners(); iii++ )
					if( m_Doc->m_outline_poly->GetAt(ii).GetSel(iii) )
						AlignSides(ID_POLYLINE, ii, iii);
			HighlightGroup();
			m_Doc->ProjectModified( TRUE );
		}
		else if( fk == FK_SCALE_FACTOR )
		{
			SetCursorMode( CUR_SCALE_FACTOR );
		}
		else if( fk == FK_SWITCH_TO_PCB )
		{
			m_Doc->SwitchToPCB();
		}
		else if( fk == FK_CHANGE_LAYER && m_sel_text )
		{
			if( m_sel_text->m_layer == LAY_PIN_DESC )
			{
				if( AfxMessageBox(G_LANGUAGE == 0 ?
					"Are you sure you want to change the text color layer to a last polyline layer?" :
					"Вы уверены, что хотите изменить слой текста на последний слой полилинии, по которой кликнули до этого?", MB_ICONQUESTION | MB_OKCANCEL ) == IDOK )
					if( m_polyline_layer >= LAY_ADD_1 )
					{
						int it = -1;
						for( CText * t=m_Doc->Attr->m_pDesclist->GetNextText(&it); t; t=m_Doc->Attr->m_pDesclist->GetNextText(&it) )
							if( t->m_selected )
							{
								t->m_layer = m_polyline_layer;
								t->MakeVisible();
							}
					}
						
			}
			else if( AfxMessageBox(G_LANGUAGE == 0 ?
				"Are you sure you want to reset the text color layer?" :
				"Вы уверены, что хотите сбросить цветовой слой текста? (сделать слой текста по умолчанию)", MB_ICONQUESTION | MB_OKCANCEL ) == IDOK )
			{
				int it = -1;
				for( CText * t=m_Doc->Attr->m_pDesclist->GetNextText(&it); t; t=m_Doc->Attr->m_pDesclist->GetNextText(&it) )
					if( t->m_selected )
					{
						t->m_layer = LAY_PIN_DESC;	
						t->MakeVisible();
					}
			}
			
			CancelSelection();
			OnRangeCmds(NULL);
		}
		else if( fk == FK_SET_POSITION )
			OnOPCornerEdit();
		else if( fk == FK_MAGNETIZE )
		{
			SaveUndoInfoForGroup( m_Doc->m_undo_list );
			OnGroupVtxMagnetize( m_Doc );
			HighlightGroup();
			OnRangeCmds(NULL);
		}
		break;

	case  CUR_DRAG_TEXT:
		if( fk == FK_ROTATE_TEXT )
			m_Doc->m_dlist->IncrementDragAngle( pDC );
		break;

	case  CUR_ADD_OP:
		if( fk == FK_DRAW_SQUARE )
		{
			m_object = OBJ_SQUARE;
		}
		else if( fk == FK_DRAW_RECT )
		{
			m_object = OBJ_RECT;
		}
		else if( fk == FK_DRAW_ROUND )
		{
			m_object = OBJ_ROUND;
		}
		else if( fk == FK_DRAW_OVAL )
		{
			m_object = OBJ_OVAL;
		}
		else if( fk == FK_DRAW_ELLIPSE )
		{
			m_object = OBJ_ELLIPSE;
		}
		else if( fk == FK_DRAW_RNDRECT )
		{
			m_object = OBJ_RNDRECT;
		}
		else if( fk == FK_DRAW_CLOUD )
		{
			m_object = OBJ_CLOUD;
		}
		else if( fk == FK_MAGNETIZE )
		{
			SetMagnet(1);
		}
		else if( fk == FK_MAGNETIZE_OFF )
		{
			SetMagnet(0);
		}
		SetFKText( CUR_ADD_OP );
		goto goodbye;

	case  CUR_DRAG_OP:
	case  CUR_DRAG_OP_1:
	case  CUR_DRAG_OP_MOVE:
		if( fk == FK_POLY_STRAIGHT )
		{
			m_polyline_style = CPolyLine::STRAIGHT;
			m_Doc->m_dlist->SetDragArcStyle( m_polyline_style );
		}
		else if( fk == FK_POLY_ARC_CW )
		{
			m_polyline_style = CPolyLine::ARC_CW;
			m_Doc->m_dlist->SetDragArcStyle( m_polyline_style );
		}
		else if( fk == FK_POLY_ARC_CCW )
		{
			m_polyline_style = CPolyLine::ARC_CCW;
			m_Doc->m_dlist->SetDragArcStyle( m_polyline_style );
		}
	case  CUR_DRAG_OP_INSERT:
		if( fk == FK_MAGNETIZE )
		{
			SetMagnet(1);
		}
		else if( fk == FK_MAGNETIZE_OFF )
		{
			SetMagnet(0);
		}
		SetFKText( m_cursor_mode );
		goto goodbye;

	case CUR_DRAG_GROUP:
	case CUR_DRAG_GROUP_ADD:
		if( fk == FK_ROTATE_GROUP_90 )
		{
			m_Doc->m_dlist->IncrementDragAngle( pDC );
		}
		else if ( fk == FK_SIDE )
		{
			if( m_sel_flags == TEXT_ONLY )
			{
				CancelDraggingGroup();
				TurnGroup();
				CPoint pt;
				pt.x  = m_sel_text->m_x;
				pt.y  = m_sel_text->m_y;
				CPoint cur_p = m_Doc->m_dlist->PCBToScreen( pt );
				SetCursorPos( cur_p.x, cur_p.y );
				m_last_cursor_point = pt;
				m_last_mouse_point = pt;
				StartDraggingGroup( 0, cur_p.x, cur_p.y );//groupAverageX, groupAverageY );
			}
		}
		break;
	}	// end switch
	//
	//
	if( nChar == VK_HOME )
	{
		// home key pressed, ViewAllElements
		OnViewAllElements();
		goto goodbye;
	}
	else if( nChar == 33 )
	{
		// PgUp pressed, zoom in
		if( m_pcbu_per_pixel > 254 )
		{
			m_pcbu_per_pixel = m_pcbu_per_pixel/ZOOM_RATIO;
			m_org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel)/2;
			m_org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel)/2;
			CRect screen_r;
			GetWindowRect( &screen_r );
			m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel,
				m_org_x, m_org_y );
			p = m_Doc->m_dlist->PCBToScreen( p );
			SetCursorPos( p.x, p.y );
		}
	}
	else if( nChar == 34 )
	{
		// PgDn pressed, zoom out
		// first, make sure that window boundaries will be OK
		int org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel*ZOOM_RATIO)/2;
		int org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel*ZOOM_RATIO)/2;
		int max_x = org_x + (m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel*ZOOM_RATIO;
		int max_y = org_y + (m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel*ZOOM_RATIO;
		if( org_x > -PCB_BOUND && org_x < PCB_BOUND && max_x > -PCB_BOUND && max_x < PCB_BOUND
			&& org_y > -PCB_BOUND && org_y < PCB_BOUND && max_y > -PCB_BOUND && max_y < PCB_BOUND )
		{
			// OK, do it
			m_org_x = org_x;
			m_org_y = org_y;
			m_pcbu_per_pixel = m_pcbu_per_pixel*ZOOM_RATIO;
			CRect screen_r;
			GetWindowRect( &screen_r );
			m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel,
				m_org_x, m_org_y );
			p = m_Doc->m_dlist->PCBToScreen( p );
			SetCursorPos( p.x, p.y );
		}
	}
goodbye:
	ReleaseDC( pDC );
	if( gLastKeyWasArrow == FALSE )
		ShowSelectStatus();
	if( CurDragging() )
		m_draw_layer = LAY_BACKGND;// CurDragging()
	Invalidate( FALSE );//handlekeypress
}
//===============================================================================================
// Mouse moved
//
void CFreePcbView::OnMouseMove(UINT nFlags, CPoint point)
{
	static int cicle = 1;
	static BOOL LinkWasMade = TRUE;
	static BOOL OnMouseMoveComplete = TRUE;
	if (!OnMouseMoveComplete)
		return;
	OnMouseMoveComplete = FALSE; 
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( m_bLButtonDown )
	{
		CString str = "To Right - SELECT  To Left - UNSELECT";
		pMain->DrawStatus( 3, &str );
	}
	else if (m_cursor_mode == CUR_NONE_SELECTED )
	{ 
		id sid;
		id idT( ID_TEXT_DEF );
		CPoint p = m_Doc->m_dlist->WindowToPCB( point );
		void * ptr = m_Doc->m_dlist->TestSelect( p.x, p.y, &sid, &m_sel_layer, NULL, NULL, &idT );
		if( ptr )
		{
			m_sel_text = (CText*)ptr;
			if( m_sel_text )
			{
				int iof = m_sel_text->m_str.Find( CP_LINK );
				if( iof > 0 )
				{
					LinkWasMade = TRUE;
					if( m_sel_text->m_tl )
					{
						m_sel_text->m_tl->HighlightText( m_sel_text );
						//m_draw_layer = LAY_BACKGND;
						Invalidate(FALSE);
					}
				}
			}
		}
		else if( LinkWasMade )
		{
			LinkWasMade = 0;
			m_Doc->m_dlist->CancelHighLight();
			//m_draw_layer = LAY_BACKGND;
			Invalidate(FALSE);
		}
	}
	//
	if( (nFlags & MK_LBUTTON) && m_bLButtonDown )
	{
		double d = abs(point.x-m_start_pt.x) + abs(point.y-m_start_pt.y);
		if( m_bDraggingRect
			|| (d > 30 /*&& !CurDragging()*/ ) )
		{
			// we are dragging a selection rect
			SIZE s1;
			s1.cx = s1.cy = 1;
			m_drag_rect.TopLeft() = m_start_pt;
			m_drag_rect.BottomRight() = point;
			m_drag_rect.NormalizeRect();
			CDC * pDC = GetDC();
			if( !m_bDraggingRect )
			{
				//start dragging rect
				pDC->DrawDragRect( &m_drag_rect, s1, NULL, s1 );
			}
			else
			{
				// continue dragging rect
				pDC->DrawDragRect( &m_drag_rect, s1, &m_last_drag_rect, s1 );
			}
			m_bDraggingRect  = TRUE;
			m_last_drag_rect = m_drag_rect;
			ReleaseDC( pDC );
		}
	}
	m_last_mouse_point = m_Doc->m_dlist->WindowToPCB( point );
	SnapCursorPoint( m_last_mouse_point, nFlags );
	// check for cursor hiding
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	if( !CurDragging() )
		frm->m_bHideCursor = FALSE;		// disable cursor hiding
	else if( !frm->m_bHideCursor )
	{
		// enable cursor hiding and set rect
		CRect r = frm->m_client_rect;
		r.left += m_left_pane_w;
		r.bottom -= m_bottom_pane_h;
		frm->SetHideCursor( TRUE, &r );
	}
	OnMouseMoveComplete = TRUE;
}
//===============================================================================================
/////////////////////////////////////////////////////////////////////////
// Utility functions
//
// Set the device context to world coords
//
int CFreePcbView::SetDCToWorldCoords( CDC * pDC )
{
	m_Doc->m_dlist->SetDCToWorldCoords( pDC, &m_botDC, &m_topDC, m_org_x, m_org_y );
	return 0;
}
//===============================================================================================
// Set cursor mode, update function key menu if necessary
//
void CFreePcbView::SetCursorMode( int mode )
{	
	if( mode != m_cursor_mode )
	{	
		m_cursor_mode = mode; // ïåðåä SetFKText !!
		SetFKText( m_cursor_mode );
		ShowSelectStatus();
		if( CurDragging() )
			SetMainMenu( FALSE );
		else if( m_Doc->m_project_open )
			SetMainMenu( TRUE );
		return;
	}
}
//===============================================================================================
// Set function key shortcut text
//
void CFreePcbView::SetFKText( int mode )
{
	for( int i=0; i<12; i++ )
	{
		m_fkey_option[i] = 0;
		m_fkey_command[i] = 0;
	}
	switch( mode )
	{
	case CUR_NONE_SELECTED:
		if( m_Doc->m_project_open )
		{	
			m_fkey_option[4] = FK_OPEN_FOLDER;
			m_fkey_option[7] = FK_GRID_STYLE;
			if( m_protect == 0 )
			{
				if( m_dragging_mode == FK_DRAG_LINE )
					m_fkey_option[1] = FK_DRAG_GROUP;
				else if( m_dragging_mode == FK_DRAG_GROUP )
					m_fkey_option[1] = FK_DRAG_LINE;
				else if( m_dragging_mode == 0 )
				{
					if( m_Doc->Attr->m_Reflist->GetNumDifferentTexts(0) )
					{
						m_fkey_option[1] = FK_DRAG_GROUP;
						m_fkey_option[8] = FK_REDO_RATLINES;
					}
					m_fkey_option[0] = FK_IMPORT_PART;
					m_fkey_option[2] = FK_ADD_LINE;
					if( m_polyline_layer )
						m_fkey_option[3] = FK_REPEAT_LINE;
				}
			}
		}
		break;

	case CUR_TEXT_SELECTED:
		ppx = prevx;
		ppy = prevy;
		prevx = m_sel_text->m_x;
		prevy = m_sel_text->m_y;
		m_fkey_option[0] = FK_EDIT_TEXT;
		m_fkey_option[1] = FK_ROTATE_GROUP_5;
		m_fkey_option[2] = FK_ROTATE_GROUP__5;
		m_fkey_option[3] = FK_MOVE_TEXT;
		m_fkey_option[6] = FK_ALIGN_X;
		m_fkey_option[7] = FK_ALIGN_Y;
		if( m_sel_text->m_tl == m_Doc->Attr->m_Reflist )
			m_fkey_option[8] = FK_SWITCH_TO_PCB;
		else if( m_sel_text->m_tl == m_Doc->Attr->m_Pinlist )
			m_fkey_option[8] = FK_SWITCH_TO_PCB;
		else if( m_sel_text->m_tl == m_Doc->Attr->m_pDesclist )
		{
			if( m_Doc->m_outline_poly->GetAt( m_sel_text->m_polyline_start ).Check( index_pin_attr ) == NULL )
				m_fkey_option[4] = FK_CHANGE_LAYER;
		}
		break;

	case CUR_OP_CORNER_SELECTED:
		if( m_protect == 0 )
		{
			if ( m_page == 1 )
			{
				prev_middle_x = (ppx + prevx)/2;
				prev_middle_y = (ppy + prevy)/2;
				ppx = prevx;
				ppy = prevy;
				prevx = m_sel_op.GetX(m_sel_id.ii);
				prevy = m_sel_op.GetY(m_sel_id.ii);
				//
				m_fkey_option[0] = FK_SET_POSITION;
				m_fkey_option[1] = FK_MAKE_FIRST;
				m_fkey_option[2] = FK_ALIGN_SIDES;
				m_fkey_option[3] = FK_MOVE_CORNER;
				m_fkey_option[4] = FK_ALIGN_MIDDLE;
				m_fkey_option[5] = FK_CIRC_ALIGN;
				m_fkey_option[6] = FK_ALIGN_X;
				m_fkey_option[7] = FK_ALIGN_Y;
				if (m_sel_op.GetClosed() == 0)
					if(m_sel_op.GetLayer() >= LAY_ADD_1)
						if(m_sel_id.ii == 0 || m_sel_id.ii == m_sel_op.GetNumCorners()-1)
							m_fkey_option[8] = FK_ADD_NODE;
			}
			else if ( m_page == 2 )
			{
				m_fkey_option[4] = FK_ALIGN_MIDLINE;
				m_fkey_option[5] = FK_ALIGN_MIDDLE;
				m_fkey_option[6] = FK_ALIGN_MIDDLE_X;
				m_fkey_option[7] = FK_ALIGN_MIDDLE_Y;
			}
		}
		break;

	case CUR_OP_SIDE_SELECTED:
		m_fkey_option[0] = FK_SET_ATTRIB;
		if( m_protect == 0 )
		{
			if( m_sel_op.GetSideStyle(m_sel_id.ii) == CPolyLine::STRAIGHT )
			{
				m_fkey_option[1] = FK_POLY_ARC_CW;
				m_fkey_option[2] = FK_ADD_CORNER;
			}
			else if( m_sel_op.GetSideStyle(m_sel_id.ii) == CPolyLine::ARC_CW )
				m_fkey_option[1] = FK_POLY_ARC_CCW;
			else
				m_fkey_option[1] = FK_POLY_STRAIGHT;
			{
				int n_c = m_sel_op.GetNumContour(m_sel_id.ii);
				int gcl = m_sel_op.GetClosed();
				if( n_c && gcl )
					m_fkey_option[3] = FK_COPY_CUT;
				else
					m_fkey_option[3] = FK_DRAG_LINE;
			}
			m_fkey_option[4] = FK_SELECT_CONTOUR;
			m_fkey_option[7] = FK_ADD_SIZE;
			if( m_sel_op.GetNumCorners() > 2 && m_sel_op.GetClosed() )
				m_fkey_option[8] = FK_INSIDE_POLYLINE;
			if( m_sel_op.GetNumContour(m_sel_id.ii) )
				m_fkey_option[8] = FK_DELETE_CUTOUT;
		}
		{
			int dx = m_sel_op.GetX( m_sel_id.ii ) - m_sel_op.GetX( m_sel_op.GetNextCornerIndex(m_sel_id.ii) );
			int dy = m_sel_op.GetY( m_sel_id.ii ) - m_sel_op.GetY( m_sel_op.GetNextCornerIndex(m_sel_id.ii) );
			if( dx && dy && m_sel_op.GetSideStyle( m_sel_id.ii ) == 0 )
				m_fkey_option[5] = FK_SET_LENGTH;
		}
		m_fkey_option[6] = FK_SET_WIDTH_HATCH_LAYER;
		break;

	case CUR_GROUP_SELECTED:
		if( m_sel_flags == VTX_ONLY )
		{
			if( m_sel_id.type == ID_POLYLINE && m_sel_id.sst == ID_CORNER )
			{
				prev_middle_x = (ppx + prevx)/2;
				prev_middle_y = (ppy + prevy)/2;
				ppx = prevx;
				ppy = prevy;
				prevx = m_sel_op.GetX(m_sel_id.ii);
				prevy = m_sel_op.GetY(m_sel_id.ii);
			}
		}
		if ( m_page == 1 )
		{
			// default key F9
			m_fkey_option[8] = FK_SWITCH_TO_PCB;
			if( m_protect == 0 )
			{
				m_fkey_option[2] = FK_SIDE;
				m_fkey_option[3] = FK_MOVE_GROUP;
				m_fkey_option[5] = FK_ROTATE_GROUP;
				m_fkey_option[7] = FK_SCALE_FACTOR;
				if( m_sel_flags == OP_SIDE_ONLY )
					m_fkey_option[8] = FK_DEL_WITH_MERGE;
			}
			//
			if ( m_sel_flags == VTX_ONLY )
			{
				m_fkey_option[0] = FK_SET_POSITION;
				// key F2 F3
				if ( TestSelElements( FOR_FK_RADIUS_UP_DOWN ) && m_protect == 0 )
				{
					m_fkey_option[1] = FK_RADIUS_DOWN;
					m_fkey_option[2] = FK_RADIUS_UP;
				}
				else 
				{
					m_fkey_option[1] = FK_MAGNETIZE;
					m_fkey_option[2] = FK_ALIGN_SIDES;
				}
				m_fkey_option[4] = FK_SELECT_CONTOUR;
				// key F6
				if( TestSelElements( FOR_FK_SELECT_BETWEEN ) )
					m_fkey_option[5] = FK_SEL_BETWEEN;	
			}
			else 
			{
				if( getbit( m_sel_mask, SEL_MASK_MERGES ) && getbit( m_sel_flags, FLAG_SEL_OP ) )
				{
					if( m_sel_merge_name.GetLength() )
						m_fkey_option[1] = FK_MERGE_GROUP;
					else if( !m_Doc->m_mlist->GetSelected() )
						m_fkey_option[1] = FK_MERGE_GROUP;
				}
				if ( getbit( m_sel_flags, FLAG_SEL_OP) )
					m_fkey_option[4] = FK_SELECT_CONTOUR;
				if ( m_sel_flags == TEXT_ONLY )
				{
					m_fkey_option[6] = FK_EDIT_TEXT;
					if( m_sel_text )
						if( m_sel_text->m_str.Right(3) == "BOM" )
							m_fkey_option[0] = FK_SET_ATTRIB;
						else
							m_fkey_option[0] = FK_EDIT_TEXT;
					BOOL CH_LAY = FALSE;
					int it = -1;
					for( CText * t=m_Doc->Attr->m_pDesclist->GetNextText(&it); t; t=m_Doc->Attr->m_pDesclist->GetNextText(&it) )
						if( t->m_selected )
						{
							CH_LAY = TRUE;
							if( m_Doc->m_outline_poly->GetAt( t->m_polyline_start ).Check( index_pin_attr ) || 
								m_Doc->m_outline_poly->GetAt( t->m_polyline_start ).Check( index_net_attr ) )
							{
								CH_LAY = FALSE;
								break;
							}
						}
					if( CH_LAY )
						m_fkey_option[4] = FK_CHANGE_LAYER;
				}
				else if( m_sel_id.type == ID_POLYLINE && ///m_sel_id.sst == ID_SIDE &&
						  getbit( m_sel_flags, FLAG_SEL_DRE ) == 0 )
				{
					m_fkey_option[0] = FK_SET_ATTRIB;
					m_fkey_option[6] = FK_SET_WIDTH_HATCH_LAYER;
				}
				if( m_sel_merge_name.GetLength() )
					m_fkey_option[4] = FK_TAKE_APART;
				//
				//
				if ( TestSelElements( FOR_FK_SET_CLEARANCE ) )
					m_fkey_option[0] = FK_SET_CLEARANCE;
			}
		}
		else if ( m_page == 2 )
		{
			if( m_protect == 0 )
			{
				m_fkey_option[1] = FK_CLEARANCE_DOWN;
				m_fkey_option[2] = FK_CLEARANCE_UP;
			}
			m_fkey_option[8] = FK_OK;
		}
		else if ( m_page == 3 )
		{
			m_fkey_option[0] = FK_ROTATE_GROUP__1;
			m_fkey_option[1] = FK_ROTATE_GROUP_1;
			m_fkey_option[2] = FK_ROTATE_GROUP__5;
			m_fkey_option[3] = FK_ROTATE_GROUP_5;
			m_fkey_option[4] = FK_ROTATE_GROUP__45;
			m_fkey_option[5] = FK_ROTATE_GROUP_45;
			m_fkey_option[6] = FK_ROTATE_GROUP__90;
			m_fkey_option[7] = FK_ROTATE_GROUP_90;		
			m_fkey_option[8] = FK_OK;
		}
		break;

	case CUR_DRAG_TEXT:
		m_fkey_option[2] = FK_ROTATE_TEXT;
		break;

	case CUR_ADD_OP:
	case CUR_DRAG_OP_MOVE:
		if( m_magnetize == 0 )
			m_fkey_option[3] = FK_MAGNETIZE;
		else
			m_fkey_option[3] = FK_MAGNETIZE_OFF;
		m_fkey_option[0] = FK_DRAW_RECT;
		m_fkey_option[1] = FK_DRAW_SQUARE;	
		m_fkey_option[2] = FK_DRAW_ROUND;
		m_fkey_option[4] = FK_DRAW_OVAL;
		m_fkey_option[5] = FK_DRAW_ELLIPSE;
		m_fkey_option[6] = FK_DRAW_RNDRECT;
		m_fkey_option[7] = FK_DRAW_CLOUD;
		break;

	case CUR_DRAG_OP:
		m_fkey_option[0] = FK_POLY_STRAIGHT;
		m_fkey_option[1] = FK_POLY_ARC_CW;
		m_fkey_option[2] = FK_POLY_ARC_CCW;
		if( m_magnetize == 0 )
			m_fkey_option[3] = FK_MAGNETIZE;
		else
			m_fkey_option[3] = FK_MAGNETIZE_OFF;
		break;

	case CUR_DRAG_OP_1:
		m_fkey_option[0] = FK_POLY_STRAIGHT;
		m_fkey_option[1] = FK_POLY_ARC_CW;
		m_fkey_option[2] = FK_POLY_ARC_CCW;
	case  CUR_DRAG_OP_INSERT:
		if( m_magnetize == 0 )
			m_fkey_option[3] = FK_MAGNETIZE;
		else
			m_fkey_option[3] = FK_MAGNETIZE_OFF;
		break;

	case CUR_DRAG_GROUP:
	case CUR_DRAG_GROUP_ADD:
		if( m_sel_flags == TEXT_ONLY )
			m_fkey_option[0] = FK_SIDE;
		m_fkey_option[2] = FK_ROTATE_GROUP_90;
	}

	for( int i=0; i<12; i++ )
	{
		strcpy( m_fkey_str[2*i],   fk_str[2*m_fkey_option[i]] );
		strcpy( m_fkey_str[2*i+1], fk_str[2*m_fkey_option[i]+1] );
	}
	if( m_fk_button_index < 0 )
		DrawBottomPane();
}

//===============================================================================================
// Draw bottom pane
//
void CFreePcbView::DrawBottomPane()
{
	static CRect m_win_r = rect(0,0,0,0);
	static int n_fk_button_index = -1;
	static int n_protect = -1;
	static int n_page = -1;
	static int n_fk_page = -1;
	static int n_cursor_mode = -1;
	if( !m_Doc )
	{
		n_cursor_mode = -1;
		return;
	}
	if( !m_Doc->m_project_open )
	{
		n_cursor_mode = -1;
		return;
	}
	if( m_left_pane_invalid == -1 ||
		n_cursor_mode != m_cursor_mode ||
		m_cursor_mode == CUR_GROUP_SELECTED ||
		n_fk_page != m_page || 
		n_page != m_Doc->Pages.GetActiveNumber() ||
		m_win_r.Width() != m_client_r.Width() ||
		m_win_r.Height() != m_client_r.Height() ||
		n_fk_button_index != m_fk_button_index ||
		n_protect != m_protect  )
	{
		// save static
		n_cursor_mode = m_cursor_mode;
		n_fk_page = m_page;
		n_page = m_Doc->Pages.GetActiveNumber();
		m_win_r = m_client_r;
		n_protect = m_protect;
		n_fk_button_index = m_fk_button_index;
		//
		CDC * pDC = GetDC();
		CFont * old_font = pDC->SelectObject( &m_small_font );
	
		// sel rect
		COLORREF CLR_BLACK_GND, CLR;
		CLR = RGB( 200, 200, 0 );
		CPen White__pen( PS_SOLID, 1, RGB(255, 255, 255) );
		CPen Black__pen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
		CPen Color__pen( PS_SOLID, 1, CLR );
		
		// get client rectangle
		///GetClientRect( &m_client_r );??
		pDC->SelectObject( &White__pen );
		CBrush fill_brush( CLR );
		CBrush none_brush( RGB(255,255,255) );
		pDC->SelectObject( none_brush );
		
		// rectangle
		CRect r = m_client_r;
		r.top = r.bottom - m_bottom_pane_h;
		r.left += FKEY_OFFSET_X;
		pDC->Rectangle( &r );
		pDC->SelectObject( &Black__pen );

		// draw labels for function keys at bottom of client area
		for( int j=0; j<3; j++ )
		{
			for( int i=0; i<4; i++ )
			{
				int ifn = j*4+i;
				if( ifn < 9 )
				{
					CRect r( FKEY_OFFSET_X+ifn*FKEY_STEP+j*FKEY_GAP,
						m_client_r.bottom-FKEY_OFFSET_Y-FKEY_R_H,
						FKEY_OFFSET_X+ifn*FKEY_STEP+j*FKEY_GAP+FKEY_R_W,
						m_client_r.bottom-FKEY_OFFSET_Y );
					if( j*4+i == m_fk_button_index )
					{
						pDC->SelectObject( &Color__pen );
						pDC->SelectObject( fill_brush );
						pDC->Rectangle( &r );
						pDC->SelectObject( &Black__pen );
						pDC->SelectObject( none_brush );
						continue;
					}
					else
						pDC->Rectangle( &r );
					pDC->MoveTo( r.left+FKEY_SEP_W, r.top );
					pDC->LineTo( r.left+FKEY_SEP_W, r.top + FKEY_R_H/2 + 1 );
					pDC->MoveTo( r.left, r.top + FKEY_R_H/2 );
					pDC->LineTo( r.left+FKEY_SEP_W, r.top + FKEY_R_H/2 );
					r.top += 1;
					r.left += 2;
					char fkstr[3] = "F1";
					fkstr[1] = '1' + j*4+i;
					pDC->DrawText( fkstr, -1, &r, 0 );
					r.left += FKEY_SEP_W;
					char * str1 = &m_fkey_str[2*ifn][0];
					char * str2 = &m_fkey_str[2*ifn+1][0];
					pDC->DrawText( str1, -1, &r, 0 );
					r.top += FKEY_R_H/2 - 2;
					pDC->DrawText( str2, -1, &r, 0 );
				}
			}
		}
		pDC->SelectObject( old_font );
		ReleaseDC( pDC );
	}	
}
//===============================================================================================
void CFreePcbView::ShowRelativeDistance( int dx, int dy )
{
	CString str;
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	dx *= m_user_scale;
	dy *= m_user_scale;
	double d = sqrt( (double)dx*dx + (double)dy*dy ) + m_measure_dist;  
	if( m_Doc->m_units == MIL )
		str.Format( "dx = %.1f, dy = %.1f, d = %.2f", (double)dx/NM_PER_MIL, (double)dy/NM_PER_MIL, d/NM_PER_MIL );
	else
		str.Format( "dx = %.3f, dy = %.3f, d = %.3f", (double)dx/1000000.0, (double)dy/1000000.0, d/1000000.0 );
	pMain->DrawStatus( 3, &str );
}
//===============================================================================================
void CFreePcbView::ShowRelativeDistance( int x, int y, int dx, int dy )
{
	CString str;
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	double d = sqrt( (double)dx*dx + (double)dy*dy );  
	if( m_Doc->m_units == MIL )
		str.Format( "x = %.1f, y = %.1f, dx = %.1f, dy = %.1f, d = %.2f",
		(double)x/NM_PER_MIL, (double)y/NM_PER_MIL,
		(double)dx/NM_PER_MIL, (double)dy/NM_PER_MIL, d/NM_PER_MIL );
	else
		str.Format( "x = %.3f, y = %.3f, dx = %.3f, dy = %.3f, d = %.3f", 
		x/1000000.0, y/1000000.0,
		dx/1000000.0, dy/1000000.0, d/1000000.0 );
	pMain->DrawStatus( 3, &str );
}
//===============================================================================================
// display selected item in status bar
//
int CFreePcbView::ShowSelectStatus()
{
//#define SHOW_UIDS

	CString str="", x_str, y_str, w_str, hole_str, via_w_str, via_hole_str;
	
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pMain )
		return 1;
	
	if( !m_Doc )
		return 1;
	int u = m_Doc->m_units;

	switch( m_cursor_mode )
	{
	case CUR_NONE_SELECTED:
		str.Format( "No selection");
		break;

	case CUR_DRE_SELECTED:
		str.Format( "DRC %s", m_sel_dre->str );
		break;

	case CUR_OP_CORNER_SELECTED:
		::MakeCStringFromDimension( m_user_scale, &x_str, m_sel_op.GetX(m_sel_id.ii), u, FALSE, FALSE, FALSE, u==MIL?1:3 );
		::MakeCStringFromDimension( m_user_scale, &y_str, m_sel_op.GetY(m_sel_id.ii), u, FALSE, FALSE, FALSE, u==MIL?1:3 );
		if( m_sel_op.GetClosed() )
			str.Format( "polyline %d, corner %d, x=%s, y=%s",
						 m_sel_id.i+1, m_sel_id.ii+1, x_str, y_str );
		else
			str.Format( "outline %d, corner %d, x=%s, y=%s",
						 m_sel_id.i+1, m_sel_id.ii+1, x_str, y_str );
		break;

	case CUR_TEXT_SELECTED:
		{
			str.Format( "%s \"%s\"", layer_str[m_sel_text->m_layer], m_sel_text->m_str ); 
			break;
		}

	case CUR_GROUP_SELECTED:
		if ( m_page  == 2 )
		{
			::MakeCStringFromDimension( m_user_scale, &w_str, (double)m_seg_clearance*m_user_scale, u, FALSE, FALSE, FALSE, u==MIL?1:3 );
			str.Format( ("Clearance value: " + w_str) );
		}
		else if ( m_sel_merge_name.GetLength() )
			str = m_sel_merge_name;
		else if ( m_page == 1 )
		{
			if( m_sel_id.type == ID_POLYLINE )
				if( m_sel_count > m_sel_op.GetNumSides() )
				{
					str.Format( "Group selected (%d). You can add nodes to the selection using the mouse selection frame.", m_sel_count );
					break;
				}
			str.Format( "Group selected (%d)", m_sel_count );
		}
		else
			break;
	case CUR_OP_SIDE_SELECTED:
		if ( m_sel_id.type == ID_POLYLINE )
		{
			int lr = m_sel_op.GetLayer();
			if( str.GetLength() == 0 )
				str.Format( "(%d) %s", m_sel_id.i+1, layer_str[lr] );
			if( m_sel_op.Check( index_net_attr ) && m_sel_count == m_sel_op.GetNumSides() )
			{
				str += ", net=";
				str += m_sel_op.pAttr[index_net_attr]->m_str ;
			}
			if( m_sel_op.Check( index_pin_attr ) && m_sel_count == m_sel_op.GetNumSides() )
			{
				str += ", pin=";
				str += m_sel_op.pAttr[index_pin_attr]->m_str ;
			}
			if( m_sel_op.Check( index_part_attr ) )
			{
				str += ", part=";
				str += m_sel_op.pAttr[index_part_attr]->m_str ;
			}
			CString len, lenx, leny;
			int x = m_sel_op.GetX(m_sel_id.ii);
			int y = m_sel_op.GetY(m_sel_id.ii);
			int xn = m_sel_op.GetX(m_sel_op.GetNextCornerIndex(m_sel_id.ii));
			int yn = m_sel_op.GetY(m_sel_op.GetNextCornerIndex(m_sel_id.ii));
			double dx = xn-x;
			double dy = yn-y;
			double dlen = Distance( x,y,xn,yn, m_sel_op.GetSideStyle( m_sel_id.ii ) );
			::MakeCStringFromDimension( m_user_scale, &len, dlen, u, TRUE, TRUE, FALSE, u==MIL?1:3 );
			::MakeCStringFromDimension( m_user_scale, &lenx, dx, u, TRUE, TRUE, FALSE, u==MIL?1:3 );
			::MakeCStringFromDimension( m_user_scale, &leny, dy, u, TRUE, TRUE, FALSE, u==MIL?1:3 );
			CString str2;
			str2.Format( ", len=%s, dx=%s, dy=%s", len, lenx, leny );
			str += str2;
		}
		break;

	case CUR_ADD_OP:
		str.Format( "Placing first corner of outline" );
		break;

	case CUR_DRAG_OP_1:
		str.Format( "Placing second corner of outline" );
		break;

	case CUR_DRAG_OP:
		str.Format( "Placing corner %d of outline", m_sel_id.ii+2 );
		break;

	case CUR_DRAG_OP_INSERT:
		str.Format( "Inserting corner %d of outline", m_sel_id.ii+2 );
		break;

	case CUR_DRAG_OP_MOVE:
		str.Format( "Moving corner %d of outline", m_sel_id.ii+1 );
		break;

	case CUR_DRAG_TEXT:
		str.Format( "Moving text \"%s\"", m_sel_text->m_str );
		break;

	case CUR_DRAG_MEASURE_1:
		str = "Measurement mode: left-click to start";
		break;

	}
	pMain->DrawStatus( 3, &str );
	return 0;
}
//===============================================================================================
// display cursor coords in status bar
//
int CFreePcbView::ShowCursor()
{
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pMain )
		return 1;

	CString str;
	CPoint p;
	p = m_last_cursor_point;
	if( m_Doc->m_units == MIL )  
	{
		str.Format( "X: %8.1f", (double)m_last_cursor_point.x*m_user_scale/(double)PCBU_PER_MIL );
		pMain->DrawStatus( 1, &str );
		str.Format( "Y: %8.1f", (double)m_last_cursor_point.y*m_user_scale/(double)PCBU_PER_MIL );
		pMain->DrawStatus( 2, &str );
	}
	else
	{
		str.Format( "X: %8.3f", (double)m_last_cursor_point.x*m_user_scale/1000000.0 );
		pMain->DrawStatus( 1, &str );
		str.Format( "Y: %8.3f", (double)m_last_cursor_point.y*m_user_scale/1000000.0 );
		pMain->DrawStatus( 2, &str );
	}
	return 0;
}
//===============================================================================================
// display active layer in status bar and change layer order for DisplayList
//
int CFreePcbView::ShowActiveLayer(int n_layers, BOOL swCASE)
{
	static int CASE=0;
	if( swCASE )
		CASE = !CASE;
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pMain )
		return 1;
	// priority
	int i = 0;
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_HILITE, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_DRC_ERROR, i++ );	
	// set high priority for text layers
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_PART_NAME, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_FOOTPRINT, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_PART_VALUE, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_PIN_NAME, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_PIN_DESC, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_NET_NAME, i++ );
	// set high priority 
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_ADD_1, i++ );
	m_Doc->m_dlist->SetLayerDrawOrder( LAY_ADD_2, i++ );
	//if( m_active_layer != LAY_PIN_LINE )
		m_Doc->m_dlist->SetLayerDrawOrder( LAY_PIN_LINE, i++ );
	//m_Doc->m_dlist->SetLayerDrawOrder( m_active_layer, i++ );
	// polyline layers
	//if( m_active_layer != LAY_FREE_LINE )
		m_Doc->m_dlist->SetLayerDrawOrder( LAY_FREE_LINE, i++ );
	//if( m_active_layer != LAY_CONNECTION )
		m_Doc->m_dlist->SetLayerDrawOrder( LAY_CONNECTION, i++ );
	//if( m_active_layer != LAY_PART_OUTLINE )
		m_Doc->m_dlist->SetLayerDrawOrder( LAY_PART_OUTLINE, i++ );
	for( int order=0; order<m_Doc->m_num_layers-LAY_ADD_1; order++ )
		m_Doc->m_dlist->SetLayerDrawOrder( m_Doc->m_num_layers-order, order+i );
	for( int order=i+m_Doc->m_num_layers-LAY_ADD_1; order<MAX_LAYERS; order++ )
		m_Doc->m_dlist->SetLayerDrawOrder( LAY_SELECTION, order );
	m_Doc->m_dlist->SetTopLayer( m_active_layer );
	return 0;
}
//===============================================================================================
// handle mouse scroll wheel
//
BOOL CFreePcbView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	static  CPoint mem_p(0,0);
	//
	GetCursorPos( &pt );
	//
	if( m_cursor_mode == CUR_SCALE_FACTOR )
	{
		if( m_sel_count != prev_sel_count )
		{
			m_Doc->SelectGroupAttributes();
			SaveUndoInfoForGroup( m_Doc->m_undo_list );
		}
		ScaleFactor( zDelta>0?1.01:0.99 );
		Invalidate( FALSE );//OnMouseWheel
		return CView::OnMouseWheel(nFlags, zDelta, pt);
	}

	// ignore if cursor not in window
	CRect wr;
	GetWindowRect( wr );
	if( pt.x < wr.left || pt.x > wr.right || pt.y < wr.top || pt.y > wr.bottom )
		return CView::OnMouseWheel(nFlags, zDelta, pt);

	// ignore if we are dragging a selection rect
	if( m_bDraggingRect )
		return CView::OnMouseWheel(nFlags, zDelta, pt);

	CRect screen_r;
	GetWindowRect( &screen_r );
	if( abs(mem_p.x-pt.x) > 50 || abs(mem_p.y-pt.y) > 50 )
	{
		// first wheel movement in a while
		// center window on cursor then center cursor
		CPoint p = m_Doc->m_dlist->ScreenToPCB( pt );	// convert to PCB coords
		m_org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel)/2;
		m_org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel)/2;	
		m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel, m_org_x, m_org_y );
		p = m_Doc->m_dlist->PCBToScreen(p);
		SetCursorPos( p.x, p.y );
		mem_p = pt;
	}
	else
	{
		// serial movements, zoom in or out
		if( zDelta > 0 && m_pcbu_per_pixel > NM_PER_MIL/1000 )
		{
			// wheel pushed, zoom in then center world coords and cursor
			CPoint p = m_Doc->m_dlist->ScreenToPCB( pt );	// convert to PCB coords
			m_pcbu_per_pixel = m_pcbu_per_pixel/ZOOM_RATIO;
			m_org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel)/2;
			m_org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel)/2;
			m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, 
								 m_pcbu_per_pixel, m_org_x, m_org_y );
			p = m_Doc->m_dlist->PCBToScreen(p);
			SetCursorPos( p.x, p.y );
		}
		else if( zDelta < 0 )
		{
			// wheel pulled, zoom out then center
			// first, make sure that window boundaries will be OK
			CPoint p = m_Doc->m_dlist->ScreenToPCB( pt );
			int org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel*ZOOM_RATIO)/2;
			int org_y = p.y - ((m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel*ZOOM_RATIO)/2;
			int max_x = org_x + (m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel*ZOOM_RATIO;
			int max_y = org_y + (m_client_r.bottom-m_bottom_pane_h)*m_pcbu_per_pixel*ZOOM_RATIO;
			if( org_x > -PCB_BOUND && org_x < PCB_BOUND && max_x > -PCB_BOUND && max_x < PCB_BOUND
				&& org_y > -PCB_BOUND && org_y < PCB_BOUND && max_y > -PCB_BOUND && max_y < PCB_BOUND )
			{
				// OK, do it
				m_org_x = org_x;
				m_org_y = org_y;
				m_pcbu_per_pixel = m_pcbu_per_pixel*ZOOM_RATIO;
				m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, 
									 m_pcbu_per_pixel, m_org_x, m_org_y );
				p = m_Doc->m_dlist->PCBToScreen(p);
				SetCursorPos( p.x, p.y );
			}
			else
			{
				m_org_x *= 1.0001;
				m_org_y *= 1.0001;
				m_pcbu_per_pixel = m_pcbu_per_pixel*1.0001;
				m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, 
									 m_pcbu_per_pixel, m_org_x, m_org_y );
				p = m_Doc->m_dlist->PCBToScreen(p);
				SetCursorPos( p.x, p.y );
			}
		}
	}
	int INF = 1800000000;
	if( m_org_x < -INF || m_org_y < -INF || m_org_x > INF/2 || m_org_y > INF/2 )
		OnViewAllElements();
	GetCursorPos( &mem_p );

	Invalidate( FALSE );//OnMouseWheel
	return CView::OnMouseWheel(nFlags, zDelta, pt);

}


//===============================================================================================
// cancel selection
//
void CFreePcbView::CancelSelection( BOOL ConsiderFlags )
{
	if( !CurDragging() )
		m_disable_context_menu = 0;

	// reset m_prev_sel_merge
	if( getbit(m_sel_flags, FLAG_SEL_PART)  )
		m_prev_sel_merge = -1;

	// remark selected elements
	if( m_sel_id.type == ID_POLYLINE )
	{
		if( m_sel_id.i < m_Doc->m_outline_poly->GetSize() )
			m_prev_sel_merge = m_sel_op.GetMerge();
	}

	//
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) || ConsiderFlags == 0 )
	{
		m_Doc->Attr->m_Reflist->CancelSelection();
		m_Doc->Attr->m_Valuelist->CancelSelection();	
		m_Doc->Attr->m_Footlist->CancelSelection();
		m_Doc->Attr->m_Netlist->CancelSelection();
		m_Doc->Attr->m_Pinlist->CancelSelection();
		m_Doc->Attr->m_pDesclist->CancelSelection();
		m_Doc->m_tlist->CancelSelection();
	}
	if( getbit( m_sel_flags, FLAG_SEL_OP ) || ConsiderFlags == 0 ) 
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			int gns = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for(int ic=gns; ic>=0; ic--)
			{	
				m_Doc->m_outline_poly->GetAt(item).SetSel( ic,FALSE );
				if( cl == 0 && ic == gns )
					continue;
				m_Doc->m_outline_poly->GetAt(item).SetSideSel( ic,FALSE );
			}
		}
	// remove all info texts
	int it = -1;
	for(CText* t=m_Doc->m_tlist->GetNextText(&it); t; t=m_Doc->m_tlist->GetNextText(&it))
		if( t->m_layer != LAY_NET_NAME )
			m_Doc->m_tlist->RemoveText( it );
	//
	m_sel_text = 0;
	m_sel_dre = 0;
	m_sel_id.Clear();
	m_sel_flags = 0;
	m_sel_count = 0;
	prev_sel_count = 0;
	prev_m_ang = -1;
	m_Doc->m_dlist->CancelHighLight();
	m_draw_layer = LAY_DRC_ERROR;
	///
	m_page = 1;
	m_magnetize = 0;
	m_sel_merge_name = "";
	gLastKeyWasArrow = FALSE;
	gShiftKeyDown = FALSE;
	//MarkAllOutlinePoly(0,-1);

	// dlg log
	if( ConsiderFlags )
		m_Doc->m_dlg_log->ShowWindow( SW_HIDE );

	// mark merges
	m_Doc->m_mlist->mark0();
	// mode
	SetCursorMode( CUR_NONE_SELECTED );
}

void CFreePcbView::ProjectRunInfoBox()
{

}
void CFreePcbView::OnProjectRunInfoBox()
{

}

void CFreePcbView::OnSetOriginToSelectedItem()
{
	SetOriginToSelectedItem();
	Invalidate( FALSE );// (doubtful)
}
void CFreePcbView::SetOriginToSelectedItem()
{
	int x=0,y=0;
	if( m_sel_id.type == ID_POLYLINE && m_sel_id.sst == ID_CORNER )
	{
		x = m_sel_op.GetX(m_sel_id.ii);
		y = m_sel_op.GetY(m_sel_id.ii);		
	}
	else if( m_cursor_mode == CUR_TEXT_SELECTED )
	{
		x = m_sel_text->m_x;
		y = m_sel_text->m_y;
	}
	else if( m_cursor_mode == CUR_GROUP_SELECTED )
	{
		FindGroupCenter(0);
		x = groupAverageX;
		y = groupAverageY;
	}
	SaveUndoInfoForMoveOrigin( -x, -y, m_Doc->m_undo_list );
	MoveOrigin( -x, -y );
	m_org_x -= x;
	m_org_y -= y;
	CRect screen_r;
	GetWindowRect( &screen_r );
	m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel, m_org_x, m_org_y );
	CancelSelection();
	SetCursorMode(CUR_NONE_SELECTED);
	m_Doc->ProjectModified( TRUE );
}

//===============================================================================================
//------------------ context-sensitive menu invoked by right-click ------------------------------
//===============================================================================================
void CFreePcbView::OnContextMenu(CWnd* pWnd, CPoint point )
{
	if( m_disable_context_menu )
	{
		// right-click already handled, don't pop up menu
		return;
	}
	if( !m_Doc->m_project_open )	// no project open
		return;
	if( m_bRButtonDown == FALSE )
		return;
	
	// OK, pop-up context menu
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CONTEXT));
	CMenu* pPopup;
	int style;
	switch( m_cursor_mode )
	{
	case CUR_NONE_SELECTED:
		pPopup = menu.GetSubMenu(CONTEXT_NONE);
		ASSERT(pPopup != NULL);
		if( m_Doc->m_dlist->Get_Selected()==0 )
			pPopup->EnableMenuItem( ID_CANCEL_HIGHLIGHT, MF_GRAYED );
		if( m_Doc->m_drelist->list.GetSize() == 0 )
		{
			pPopup->EnableMenuItem( ID_NONE_CLEARDRCERRORS, MF_GRAYED );
			pPopup->EnableMenuItem( ID_NONE_SELECTTEXTSBYLABELS, MF_GRAYED );
		}
		{
			pPopup->EnableMenuItem( ID_GLOBAL_CANCEL_SEL, MF_GRAYED );
			int flg = ~0;
			for( int i=0; i<m_Doc->Pages.GetNumPages(); i++ )
				if( m_Doc->Pages.GetSelCount(i,&flg,TRUE) )
				{
					pPopup->EnableMenuItem( ID_GLOBAL_CANCEL_SEL, MF_ENABLED );
					break;
				}		
		}
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd );
		break;

	case CUR_OP_CORNER_SELECTED:
		pPopup = menu.GetSubMenu(CONTEXT_OP_CORNER);
		ASSERT(pPopup != NULL);
		if( m_sel_op.GetNumCorners() < 4 )
				pPopup->EnableMenuItem( ID_OP_CORNER_DELETECORNER, MF_GRAYED );
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd );
		break;

	case CUR_OP_SIDE_SELECTED:
		pPopup = menu.GetSubMenu(CONTEXT_OP_SIDE);
		ASSERT(pPopup != NULL);

		if( m_sel_op.GetNumCorners() < 3 || m_sel_op.Check( index_desc_attr ) )
			pPopup->EnableMenuItem( ID_SET_PRINT_AREA, MF_GRAYED );

		if( m_sel_flags != OP_SIDE_ONLY || m_sel_count != 1 )
			pPopup->EnableMenuItem( ID_ADD_CUTOUT, MF_GRAYED );
		style = m_sel_op.GetSideStyle( m_sel_id.ii );
		if( style == CPolyLine::STRAIGHT )
		{
			int xi = m_sel_op.GetX( m_sel_id.ii );
			int yi = m_sel_op.GetY( m_sel_id.ii );
			int xf, yf;
			if( m_sel_id.ii != (m_sel_op.GetNumCorners()-1) )
			{
				xf = m_sel_op.GetX( m_sel_id.ii+1 );
				yf = m_sel_op.GetY( m_sel_id.ii+1 );
			}
			else
			{
				xf = m_sel_op.GetX( 0 );
				yf = m_sel_op.GetY( 0 );
			}
			if( xi == xf || yi == yf )
			{
				pPopup->EnableMenuItem( ID_OP_SIDE_CONVERTTOARC_CW, MF_GRAYED );
				pPopup->EnableMenuItem( ID_OP_SIDE_CONVERTTOARC_CCW, MF_GRAYED );
			}
			pPopup->EnableMenuItem( ID_OP_SIDE_CONVERTTOSTRAIGHTLINE, MF_GRAYED );
		}
		else if( style == CPolyLine::ARC_CW )
		{
			pPopup->EnableMenuItem( ID_OP_SIDE_CONVERTTOARC_CW, MF_GRAYED );
			pPopup->EnableMenuItem( ID_OP_SIDE_INSERTCORNER, MF_GRAYED );
		}
		else if( style == CPolyLine::ARC_CCW )
		{
			pPopup->EnableMenuItem( ID_OP_SIDE_CONVERTTOARC_CCW, MF_GRAYED );
			pPopup->EnableMenuItem( ID_OP_SIDE_INSERTCORNER, MF_GRAYED );
		}
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd );
		break;

	case CUR_TEXT_SELECTED:
		pPopup = menu.GetSubMenu(CONTEXT_TEXT);
		ASSERT(pPopup != NULL);
		if( m_protect )
		{	
			pPopup->EnableMenuItem( ID_TEXT_ALIGN_RV, MF_GRAYED );
			pPopup->EnableMenuItem( ID_TEXT_ALIGN_RF, MF_GRAYED );
			pPopup->EnableMenuItem( ID_TEXT_ALIGN_RVF, MF_GRAYED );
			pPopup->EnableMenuItem( ID_TEXT_ALIGN_RFV, MF_GRAYED );
			pPopup->EnableMenuItem( ID_TEXT_GRAB_PART, MF_GRAYED );
			pPopup->EnableMenuItem( ID_TEXT_EDIT, MF_GRAYED );
		}
		if( m_sel_text )
			if( m_sel_text->m_tl != m_Doc->Attr->m_Reflist )
			{
				pPopup->EnableMenuItem( ID_TEXT_ALIGN_RF, MF_GRAYED );
				pPopup->EnableMenuItem( ID_TEXT_ALIGN_RV, MF_GRAYED );
				pPopup->EnableMenuItem( ID_TEXT_ALIGN_RVF, MF_GRAYED );
				pPopup->EnableMenuItem( ID_TEXT_ALIGN_RFV, MF_GRAYED );
				pPopup->EnableMenuItem( ID_TEXT_GRAB_PART, MF_GRAYED );
			}
			else 
			{
				if( m_sel_text->m_polyline_start < m_Doc->m_outline_poly->GetSize() )
				{
					CPolyLine * part_poly = &m_Doc->m_outline_poly->GetAt( m_sel_text->m_polyline_start );
					if( part_poly->Check( index_value_attr ) == NULL )
						pPopup->EnableMenuItem( ID_TEXT_ALIGN_RV, MF_GRAYED );
					if( part_poly->Check( index_foot_attr ) == NULL )
						pPopup->EnableMenuItem( ID_TEXT_ALIGN_RF, MF_GRAYED );
					if( part_poly->Check( index_value_attr ) == NULL ||
						part_poly->Check( index_foot_attr ) == NULL )
					{
						pPopup->EnableMenuItem( ID_TEXT_ALIGN_RVF, MF_GRAYED );
						pPopup->EnableMenuItem( ID_TEXT_ALIGN_RFV, MF_GRAYED );
					}
				}
			}
			if( m_Doc->Pages.IsThePageIncludedInNetlist( m_Doc->Pages.GetActiveNumber() ) == 0 )
				pPopup->EnableMenuItem( ID_TEXT_GRAB_PART, MF_GRAYED );
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd );
		break;

	case CUR_GROUP_SELECTED:
		if( m_sel_flags == TEXT_ONLY )
		{
			pPopup = menu.GetSubMenu(CONTEXT_TXTGROUP);
			///pPopup->EnableMenuItem( ID_TEXT_ALIGN_L, MF_GRAYED );
			///pPopup->EnableMenuItem( ID_TEXT_ALIGN_R, MF_GRAYED );
			///pPopup->EnableMenuItem( ID_TEXT_ALIGN_T, MF_GRAYED );
			///pPopup->EnableMenuItem( ID_TEXT_ALIGN_B, MF_GRAYED );
		}
		else
		{
			if( m_protect == 0 )
				pPopup = menu.GetSubMenu(CONTEXT_GROUP);
			else
				pPopup = menu.GetSubMenu(CONTEXT_PROTECT);
			/////////////////////////////////////////////
			{
				CText * part_found = NULL;
				CString Parts="";
				CString Value="";
				CString Footprint="";
				int WARNING_P=0;
				int WARNING_V=0;
				int WARNING_F=0;
				int no_part_attribute=0;
				SelPolylineTest( &part_found, &Parts, &Value, &Footprint, &WARNING_P, &WARNING_V, &WARNING_F, &no_part_attribute, 100 );
				if( WARNING_P == 1 )
				{
					ExtractComponentName( &Value, &Footprint );
					if( Value.GetLength() && Footprint.GetLength() )
					{
						BOOL bOK_TXT = 0;
						BOOL bOK_DOC = 0;
						BOOL bOK_PDF = 0;
						BOOL bOK_URL = 0;
						for( int STEP=0; STEP<2; STEP++  )
						{
							CString CDir;
							if(STEP)
							{
								int rf = m_Doc->m_pcb_full_path.ReverseFind('\\');
								if( rf <= 0 )
									continue;
								CDir = m_Doc->m_pcb_full_path.Left(rf) + rel_f_component;
							}
							else
								CDir = m_Doc->m_app_dir + main_component;
							CString CNote = CDir + "\\" + Value + "@" + Footprint;
							struct _stat buf;
							
							if( bOK_DOC == 0 )
							{
								bOK_DOC = !_stat( CNote+".docx", &buf );//ok
								if( bOK_DOC == FALSE )
									bOK_DOC = !_stat( CNote+".doc", &buf );//ok
							}
							else
								bOK_DOC = 0;

							if( bOK_TXT == 0 )
								bOK_TXT = !_stat( CNote+".txt", &buf );//ok
							else
								bOK_TXT = 0;

							if( bOK_PDF == 0 )
								bOK_PDF = !_stat( CNote+".pdf", &buf );//ok
							else
								bOK_PDF = 0;

							if( bOK_URL == 0 )
								bOK_URL = !_stat( CNote+".url", &buf );//ok
							else
								bOK_URL = 0;

							if( bOK_DOC || bOK_TXT || bOK_PDF || bOK_URL )
							{
								if( int toend = pPopup->GetMenuItemCount() )
								{
									// Open TXT note
									if( bOK_TXT )
										if(STEP)
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_TXT_NOTE, "TXT document (READ-ONLY)" );
										else
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_TXT_NOTE, &m_bitmap_txt );
									// Open DOC note
									if( bOK_DOC )
										if(STEP)
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_DOC_NOTE, "Microsoft Word document (READ-ONLY)" );
										else
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_DOC_NOTE, &m_bitmap_word );
									// Open PDF note
									if( bOK_PDF )
										if(STEP)
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_PDF_NOTE, "PDF document (READ-ONLY)" );
										else
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_PDF_NOTE, &m_bitmap_pdf );
									// Open URL note
									if( bOK_URL )
										if(STEP)
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_URL_NOTE, "URL document (READ-ONLY)" );
										else
											pPopup->InsertMenuA( toend, MF_BYPOSITION, ID_URL_NOTE, &m_bitmap_url );
								}
								else
									AfxMessageBox( "Error 4367" );
							}
						}
					}
					pPopup->EnableMenuItem( ID_APPROXIMATION_ARC, MF_GRAYED );
				}
				else
				{
					pPopup->EnableMenuItem( ID_COMPONENT_NOTE, MF_GRAYED );
					pPopup->EnableMenuItem( ID_COMPONENT_URL, MF_GRAYED );
					pPopup->EnableMenuItem( ID_COMPONENT_IMAGE, MF_GRAYED );
					pPopup->EnableMenuItem( ID_GET_PIN_MAP, MF_GRAYED );
				}
			}
		}
		ASSERT(pPopup != NULL);
		if( m_sel_flags != OP_SIDE_ONLY )
			pPopup->EnableMenuItem( ID_APPROXIMATION_ARC, MF_GRAYED );
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd );
		break;
	}
	m_bRButtonDown = FALSE;
	m_draw_layer = LAY_HILITE;
}

//===============================================================================================

//===============================================================================================
// move text, enter with text selected
//
void CFreePcbView::OnTextMove()
{
	CDC *pDC = GetDC();
	pDC->SelectClipRgn( &m_pcb_rgn );
	SetDCToWorldCoords( pDC );
	// move cursor to text origin
	CPoint p;
	p.x  = m_sel_text->m_x;
	p.y  = m_sel_text->m_y;
	CPoint cur_p = m_Doc->m_dlist->PCBToScreen( p );
	SetCursorPos( cur_p.x, cur_p.y );
	// start moving
	m_dragging_new_item = 0;
	CTextList * tl = m_sel_text->m_tl;
	if( tl )
		tl->StartDraggingText( pDC, m_sel_text );
	SetCursorMode( CUR_DRAG_TEXT );
	ReleaseDC( pDC );
}


//===============================================================================================
//---------------------------------- OnPartRemoveMerge ------------------------------------------
//===============================================================================================
void CFreePcbView::OnPartRemoveMerge()
{
	if( m_sel_id.type == ID_TEXT && m_sel_text )
	{
		m_sel_text->m_merge = m_sel_text->m_submerge;
		m_sel_text->m_submerge = -1;
	}
	else if( m_sel_id.type == ID_POLYLINE )
	{
		m_sel_op.SetMerge(m_sel_op.GetMerge(TRUE));
		m_sel_op.SetMerge(-1, TRUE);
	}
	CancelSelection();
}
void CFreePcbView::OnTextAlignL()
{
	OnTextAlign('L');
}
void CFreePcbView::OnTextAlignR()
{
	OnTextAlign('R');
}
void CFreePcbView::OnTextAlignT()
{
	OnTextAlign('T');
}
void CFreePcbView::OnTextAlignB()
{
	OnTextAlign('B');
}
void CFreePcbView::OnTextAlign( char edge )
{
	SaveUndoInfoForGroup( m_Doc->m_undo_list );
	RECT Tr = m_Doc->m_dlist->GetHighlightedBounds( NULL );
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if( tl == 0 )
			continue;
		int it = -1;
		for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
		{
			if( t->m_selected )
			{
				RECT tr;
				if( tl->GetTextRectOnPCB( t, &tr ) )
				{
					t->Undraw();
					if( edge == 'L' )
					{
						t->m_x += Tr.left - tr.left;
						clrbit( t->m_pdf_options, PDF_ALIGN );
					}
					else if( edge == 'R' )
					{
						t->m_x += Tr.right - tr.right;
						setbit( t->m_pdf_options, PDF_ALIGN );
					}
					else if( edge == 'T' )
					{
						t->m_y += Tr.top - tr.top;
						setbit( t->m_pdf_options, PDF_ALIGN );
					}
					else if( edge == 'B' )
					{
						t->m_y += Tr.bottom - tr.bottom;
						clrbit( t->m_pdf_options, PDF_ALIGN );
					}
					t->Draw(m_Doc->m_dlist, m_Doc->m_smfontutil);
				}
			}
		}
	}
	m_Doc->ProjectModified(TRUE); 
	HighlightGroup();
	m_draw_layer = 0;// OnTextAlign
	Invalidate( FALSE );// (doubtful)
}
void CFreePcbView::OnTextAlignRV()
{
	TextAlignRVF( 0,1,0 );
}
void CFreePcbView::OnTextAlignRF()
{
	TextAlignRVF( 0,0,1 );
}
void CFreePcbView::OnTextAlignRVF()
{
	TextAlignRVF( 0,0,0 );
}
void CFreePcbView::OnTextAlignRFV()
{
	TextAlignRVF( 1,0,0 );
}
void CFreePcbView::TextAlignRVF( BOOL RFV, BOOL RV, BOOL RF )
{
	BOOL RVF = ( RFV == 0 && RF == 0 && RV == 0 );
	OnSelectPartsByAttr();
	m_Doc->SelectGroupAttributes();
	SaveUndoInfoForGroup( m_Doc->m_undo_list );
	int it = -1;
	for( CText * part=m_Doc->Attr->m_Reflist->GetNextText(&it); part; part=m_Doc->Attr->m_Reflist->GetNextText(&it))
	{
		if( part->m_selected )
		{
			m_sel_text = part;
			if( RVF || RV )
				setbit( m_sel_text->m_pdf_options, RVF_FLAG );
			else
				clrbit( m_sel_text->m_pdf_options, RVF_FLAG );
			if( m_sel_text->m_polyline_start >= 0 )
			{
				int Y_SHIFT = 1;
				int an1 = m_sel_text->m_angle;
				m_sel_text->m_angle = 0;
				m_sel_text->MakeVisible();
				int RIGHT_ALIGN = getbit(part->m_pdf_options, PDF_ALIGN);
				RECT R;
				m_sel_text->m_tl->GetTextRectOnPCB( m_sel_text, &R );
				for( int iattr=((RFV||RF)?index_foot_attr:index_value_attr); 
						(iattr==index_foot_attr||iattr==index_value_attr); 
						iattr=((RFV||RF)?iattr+1:iattr-1) )
				{
					if( CText * V = m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start).Check( iattr ) )
					{	
						V->m_angle = 0;
						V->m_x = m_sel_text->m_x;
						V->MakeVisible();
						RECT VR;
						V->m_tl->GetTextRectOnPCB( V, &VR );
						int kY = -(2.0 * m_Doc->m_alignment * (float)m_sel_text->m_font_size) - (float)m_sel_text->m_stroke_width/2.0;
						V->m_x -= (VR.left - R.left);
						V->m_y = m_sel_text->m_y + kY*Y_SHIFT;
						if( V->m_font_size )
						{
							Y_SHIFT++;
							if( RVF )
								for( int ig=V->m_str.Find("'"); ig>=0; ig=V->m_str.Find("'",ig+1) )
									Y_SHIFT++;
						}
						//
						V->MakeVisible();
						if( V->m_tl->GetTextRectOnPCB( V, &VR ) )
						{
							if (RIGHT_ALIGN)
							{
								V->m_x -= (VR.right - VR.left - R.right + R.left);
								setbit( V->m_pdf_options, PDF_ALIGN );
							}
						}
						if( an1 )
							Rotate_i_Vertex( &V->m_x, &V->m_y, -an1, m_sel_text->m_x, m_sel_text->m_y );
						V->m_angle = an1;
						V->MakeVisible();
					}
					if( RV || RF )
						break;
				}
				m_sel_text->m_angle = an1;
				m_sel_text->MakeVisible();
			}
		}
	}
	m_Doc->ProjectModified(TRUE); 
	CancelSelection();
	m_draw_layer = 0;// OnTextAlign	
	Invalidate( FALSE );// (doubtful)
}
//===============================================================================================
//--------------------------------- edit a text string ------------------------------------------
//===============================================================================================
void CFreePcbView::OnTextEdit()
{
	if( m_sel_text == NULL )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The selected group contains not only texts":
			"Выбранная группа содержит не только тексты", MB_ICONERROR);
		return;
	}
	// clear memory
	m_Doc->CheckComponentInLibrary();

	CString txtSizes = "text_sizes: ";
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
		if( p->m_visible == 0 )
			continue;
		for( CText * t = m_Doc->ScanAttr( p ); t; t = m_Doc->ScanAttr( p ) )
		{
			CString info, H, W;
			if( m_Doc->m_units == MIL )
			{
				int v = (double)t->m_font_size/(double)NM_PER_MIL*(double)m_user_scale;
				H.Format( "%.3d", v );
				v = (double)t->m_stroke_width/(double)NM_PER_MIL*(double)m_user_scale;
				W.Format( "%.3d", v );
			}
			else
			{
				H.Format( "%.2f", (double)t->m_font_size/(double)NM_PER_MM*(double)m_user_scale );
				W.Format( "%.2f", (double)t->m_stroke_width/(double)NM_PER_MM*(double)m_user_scale );
			}
			info.Format( "%s/%s", H, W );
			if( txtSizes.Find(" "+info+" ") == -1 )
				txtSizes += (" "+info+" ");
		}
	}
	// Reset CP Indexes
	m_Doc->Pages.m_get_suffixes = 0;
	//
	CArray<CString> ArrTexts;
	if( m_sel_text->m_polyline_start >= 0 )
	{
		int it = -1;
		CString ATexts = dSPACE;
		CPolyLine * Apln = &m_Doc->m_outline_poly->GetAt( m_sel_text->m_polyline_start );
		CText * AREF = Apln->Check( index_part_attr );
		CTextList * ttl = m_sel_text->m_tl;
		if( ttl != m_Doc->Attr->m_Reflist )
		if( AREF || ttl == m_Doc->Attr->m_Netlist )
		{
			CString Apref, Asuff;
			if( AREF )
				ParseRef( &AREF->m_str, &Apref, &Asuff );
			for( CText * t=ttl->GetNextText(&it); t; t=ttl->GetNextText(&it) )
			{
				if( t->m_polyline_start < 0 )
					ASSERT(0);
				CPolyLine * pln = &m_Doc->m_outline_poly->GetAt( t->m_polyline_start );
				CText * REF = pln->Check( index_part_attr );
				CString pref, suff;
				if( REF )
					ParseRef( &REF->m_str, &pref, &suff );
				if( pref.Compare( Apref ) && ttl != m_Doc->Attr->m_Netlist )
					continue;
				if( ATexts.Find( " " + t->m_str +" " ) <= 0 )
				{
					ATexts += t->m_str + " ";
					ArrTexts.Add( t->m_str );
				}
			}
		}
	}
	int attrib_mask = 0;
	// create dialog and pass parameters
	CString old_str = m_sel_text->m_str;
	CDlgAddText add_text_dlg;
	add_text_dlg.m_text_sizes = &txtSizes;
	BOOL bNoAlign = 0;
	BOOL isCOMMAND = m_sel_text->m_tl->IsCommand( m_sel_text );
	if( m_sel_count > 1 || isCOMMAND || m_protect )
	{	
		ArrTexts.RemoveAll();
		int nt = 0;
		int warning = 0;
		int prev_Alignmemt = -1;
		CString s = "";
		CString Texts = dSPACE;
		if( m_protect )
			s = "Disabled";
		else for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			id _id( ID_TEXT_DEF );
			int it=-1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
				if( t->m_selected )
				{
					if( prev_Alignmemt != -1 )
						if( prev_Alignmemt != getbit( t->m_pdf_options,PDF_ALIGN ) )
							bNoAlign = TRUE;
					prev_Alignmemt = getbit( t->m_pdf_options,PDF_ALIGN );
					//
					int ia = m_Doc->Pages.IsAttr(t);
					if( ia >= 0 )
						setbit( attrib_mask, ia );
					if( Texts.Find( " " + t->m_str +" " ) <= 0 )
					{
						nt++;
						Texts += t->m_str + " ";
						ArrTexts.Add( t->m_str );
					}
					if( tl == m_Doc->Attr->m_Reflist ||  tl != m_sel_text->m_tl )
						s = "Disabled";
					else if( tl == m_Doc->Attr->m_Footlist && nt > 1 )
						warning = 1;
				}
		}
		if( s.GetLength() == 0 )
		{
			CString str;
			if( isCOMMAND )
				s = "Disabled";
			else if( warning )
			{
				if( getbit( m_sel_flags, FLAG_SEL_OP ) == 0 )
					s = "Disabled";
				else
				{
					str.Format(G_LANGUAGE == 0 ? 
						"Error!\nThe selected group contains different footprints:\n%s\n\nAbort this process?":
						"Ошибка!\nВыбранная группа содержит разные футпринты:\n%s\n\nПрервать этот процесс?", Texts);
					if( AfxMessageBox( str, MB_ICONERROR | MB_YESNO ) == IDYES )
						return;
				}
			}
			else
			{
				str.Format(G_LANGUAGE == 0 ? 
					"The selected group contains texts:\n %s":
					"Выбранная группа содержит тексты:\n %s", Texts);
				//if( nt == 1 )
				//	s = Texts;
				s = s.Trim();
				if( AfxMessageBox( str, MB_ICONWARNING | MB_OKCANCEL ) == IDCANCEL )
					return;
			}
		}
		int pdf_opt = m_sel_text->m_pdf_options;
		if( bNoAlign )
			setbit( pdf_opt, PDF_NO_ALIGN );
		if( m_sel_count > 1 && s.Compare("Disabled") == 0 && isCOMMAND == 0 )
			s = "Several";
		add_text_dlg.Initialize( &m_Doc->Pages, &s, &m_Doc->m_special_chars,
			m_Doc->m_units, m_sel_text->m_font_size,
			m_sel_text->m_stroke_width, m_sel_text->m_x, m_sel_text->m_y, m_sel_text->m_font_number, pdf_opt, attrib_mask, m_user_scale );
	}
	else if( m_sel_text->m_tl == m_Doc->Attr->m_Reflist )
	{
		setbit( attrib_mask, index_part_attr );
		add_text_dlg.Initialize( &m_Doc->Pages, &m_sel_text->m_str, &m_Doc->m_special_chars,
			m_Doc->m_units, m_sel_text->m_font_size,
			m_sel_text->m_stroke_width, m_sel_text->m_x, m_sel_text->m_y, m_sel_text->m_font_number, m_sel_text->m_pdf_options, attrib_mask, m_user_scale );
	}
	else 
	{
		int ia = m_Doc->Pages.IsAttr( m_sel_text );
		if( ia >= 0 )
			setbit( attrib_mask, ia );
		add_text_dlg.Initialize( &m_Doc->Pages, &m_sel_text->m_str, &m_Doc->m_special_chars,
			m_Doc->m_units, m_sel_text->m_font_size,
			m_sel_text->m_stroke_width, m_sel_text->m_x, m_sel_text->m_y, m_sel_text->m_font_number, m_sel_text->m_pdf_options, attrib_mask, m_user_scale );
	}
	add_text_dlg.m_text_arr = &ArrTexts;
	int ret = add_text_dlg.DoModal();
	if( ret == IDCANCEL )
		return;

	// save interval of autosave
	int mem_auto_interval = m_Doc->m_auto_interval;
	m_Doc->m_auto_interval = 0;

	if( m_Doc->m_special_chars.Compare( add_text_dlg.m_special ) )
	{
		m_Doc->m_special_chars = add_text_dlg.m_special;
		m_Doc->SaveOptions();
	}
	int mem_sel_count = m_sel_count;
	SaveUndoInfoForGroup( m_Doc->m_undo_list ); // on text edit
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if( !tl )
			continue;
		int it = -1;
		for(CText* text=tl->GetNextText(&it); text; text=tl->GetNextText(&it))
		{
			if( text->m_selected )
			{
				m_sel_text = text;
				if( mem_sel_count == 1 )
				{
					text->m_x = add_text_dlg.m_x;
					text->m_y = add_text_dlg.m_y;
				}
				text->m_font_number = add_text_dlg.m_font_number;
				int old_opt = text->m_pdf_options;
				text->m_pdf_options = add_text_dlg.m_pdf_options;
				if( getbit( old_opt, RVF_FLAG ) )
					setbit( text->m_pdf_options, RVF_FLAG );
				else
					clrbit( text->m_pdf_options, RVF_FLAG );
				if( add_text_dlg.m_remain )
				{
					clrbit( text->m_pdf_options, COMPRESSED );
					clrbit( text->m_pdf_options, UNCLENCHED );
					if( getbit( old_opt, COMPRESSED ) )
						setbit( text->m_pdf_options, COMPRESSED );
					if( getbit( old_opt, UNCLENCHED ) )
						setbit( text->m_pdf_options, UNCLENCHED );
				}
				if( add_text_dlg.m_remain_italic )
				{
					if( getbit( old_opt, ITALIC ) )
						setbit( text->m_pdf_options, ITALIC );
					else
						clrbit( text->m_pdf_options, ITALIC );
				}
				if( getbit( add_text_dlg.m_pdf_options, PDF_NO_ALIGN ) )
					if( getbit( old_opt, PDF_ALIGN ) )
						setbit( text->m_pdf_options, PDF_ALIGN );
					else
						clrbit( text->m_pdf_options, PDF_ALIGN );
				clrbit( text->m_pdf_options, PDF_NO_ALIGN );//!!!
				//
				if( add_text_dlg.m_height != 1 )//  1==disabled
					text->m_font_size = add_text_dlg.m_height;
				if( add_text_dlg.m_width != 1 )//  1==disabled
					text->m_stroke_width = add_text_dlg.m_width;
				text->Undraw();
				if( add_text_dlg.m_str.GetLength() )
				{
					if( text->m_tl == m_Doc->Attr->m_Reflist )
						if( text->m_str.Compare( add_text_dlg.m_str ) )
						{
							m_Doc->Renumber__Add( &text->m_str, &add_text_dlg.m_str, m_Doc->Pages.GetActiveNumber() );
						}

					// upd library
					CString oldv="", oldf="";
					if( text->m_tl == m_Doc->Attr->m_Valuelist || text->m_tl == m_Doc->Attr->m_Footlist )
					{
						if( text->m_polyline_start >= 0 )
						{
							CText * old_V = m_Doc->m_outline_poly->GetAt( text->m_polyline_start ).Check( index_value_attr );
							CText * old_F = m_Doc->m_outline_poly->GetAt( text->m_polyline_start ).Check( index_foot_attr );
							if( old_V && old_F )
							{
								oldv = old_V->m_str;
								oldf = old_F->m_str;
							}
						}
					}
					text->m_str = add_text_dlg.m_str;
					text->m_nchars = add_text_dlg.m_str.GetLength();
					if( oldv.GetLength() && oldf.GetLength() )
						m_Doc->CheckComponentInLibrary( text, &oldv, &oldf );
				}
				m_Doc->m_dlist->CancelHighLight();
				text->Draw(m_Doc->m_dlist, m_Doc->m_smfontutil );
			}
		}
	}
	HighlightGroup();

	//  INTERPAGE SYNC
	if( getbit( m_Doc->Pages.m_page_mirror_mask, m_Doc->Pages.GetActiveNumber() ))
		m_Doc->AttributeSync();
	if( mem_sel_count == 1 && m_sel_text->m_polyline_start >= 0 )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start);
		if( CText * ref = p->Check( index_part_attr ) )
		{
			int i_att = -1;
			if( m_sel_text->m_tl == m_Doc->Attr->m_pDesclist )
				i_att = index_desc_attr;
			else if( m_sel_text->m_tl == m_Doc->Attr->m_Netlist )
				i_att = index_net_attr;
			else if( m_sel_text->m_tl == m_Doc->Attr->m_Pinlist )
				i_att = index_pin_attr;
			if( i_att >= 0 )
				m_Doc->AttributeSync( ref, &old_str, m_sel_text, i_att );
		}
	}
	// TimeUpdate
	if( m_sel_text->m_tl == m_Doc->Attr->m_pDesclist )
	{
		m_sel_text->m_tl->TimeUpdate( m_sel_text );
		DrawPATH( m_sel_text, m_Doc->Pages.GetActiveNumber() );
		if( m_sel_text->m_str.Right(3) == "BOM" )
		{
			DrawBOM( m_sel_text );
			CancelSelection();
		}
	}

	// update part list
	if( m_dlg_partlist )
		if( m_dlg_partlist->IsWindowVisible() )		
			m_dlg_partlist->ImportPartlist();

	// start dragging if requested in dialog
	if( add_text_dlg.m_bDrag )
		OnTextMove();

	// project modified
	if( m_sel_count != 1 || m_sel_text->m_tl != m_Doc->Attr->m_pDesclist )
		m_Doc->ProjectModified( TRUE );
	
	// restore autosave
	m_Doc->m_auto_interval = mem_auto_interval;

	// redraw
	m_draw_layer = 0;// OnTextEdit
	Invalidate( FALSE );// (doubtful)
}

//===============================================================================================
// move a outline corner
//
void CFreePcbView::OnOPCornerMove()
{
	CDC *pDC = GetDC();
	pDC->SelectClipRgn( &m_pcb_rgn );
	SetDCToWorldCoords( pDC );
	CPoint p = m_last_mouse_point;
	m_from_pt.x = m_sel_op.GetX( m_sel_id.ii );
	m_from_pt.y = m_sel_op.GetY( m_sel_id.ii );
	SetCursorMode( CUR_DRAG_OP_MOVE );
	SetMagnet(0); // includes StartDraggingToMoveCorner
	SetFKText( m_cursor_mode );//!
	ReleaseDC( pDC );
}
//===============================================================================================
// edit a outline corner
//
void CFreePcbView::OnOPCornerEdit()
{
	DlgEditBoardCorner dlg;
	CString str = "Corner Position";
	double x = m_sel_op.GetX(m_sel_id.ii);
	double y = m_sel_op.GetY(m_sel_id.ii);
	dlg.Init( &str, m_Doc->m_units, x*m_user_scale, y*m_user_scale );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		double x2 = dlg.GetX()/m_user_scale;
		double y2 = dlg.GetY()/m_user_scale;
		int gm = m_sel_op.GetMerge();
		if( gm >= 0 )
		{
			CString ps;
			ps.Format(G_LANGUAGE == 0 ?
				"This polyline is connected to other objects through the \"MERGE\" property. Move all objects of the group %s?" :
				"Эта полилиния связана с другими объектами через свойство «СЛИЯНИЕ». Переместить все объекты группы %s?", m_Doc->m_mlist->GetStr( gm ) );
			if( AfxMessageBox( ps, MB_YESNO ) == IDNO )
				gm = -1;
		}
		if( gm == -1 )
		{
			if( m_sel_op.Check( index_part_attr ) && getbit( m_sel_mask, SEL_MASK_PART ) )
			{
				m_Doc->SelectPart( m_sel_op.pAttr[index_part_attr], NULL, 0 );
				m_Doc->SelectGroupAttributes();
			}
			else
			{
				CString ps;
				ps.Format(G_LANGUAGE == 0 ? "Move entire contour?" : "Переместить контур целиком?");
				if( AfxMessageBox( ps, MB_YESNO ) == IDYES )
				{
					SelectContour();
					m_Doc->SelectLineAttributes( &m_sel_op );
				}
			}
		}
		else 
		{
			CancelSelection();
			NewSelectM( gm );
			m_Doc->SelectGroupAttributes();
		}
		if( m_sel_count == 1 )
		{
			SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
			m_sel_op.MoveCorner( m_sel_id.ii,
				x2, y2 );
		}
		else
		{
			SaveUndoInfoForGroup( m_Doc->m_undo_list ); // op edit corners (multiple)
			int dx = x2 - x;
			int dy = y2 - y;
			MoveGroup( dx, dy, 0 );
			SetCursorMode( CUR_GROUP_SELECTED );
			m_Doc->m_dlist->CancelHighLight();
			HighlightGroup();
		}
		m_Doc->ProjectModified( TRUE );
		OnRangeCmds( NULL );
	}
}
//===============================================================================================
// delete a corner
//
void CFreePcbView::OnOPCornerDelete()
{
	SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
	int cc = 4;
	if( m_sel_op.GetClosed() == 0 )
		cc--;
	int gnc = m_sel_op.GetNumCorners();
	if( gnc < cc )
	{
		if( gnc < 3 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Outline has too few corners":
				"У контура слишком мало вершин");
			return;
		}
		else if( m_sel_op.GetClosed() )
			m_sel_op.UnClose();
	}
	//
	m_sel_op.DeleteCorner( m_sel_id.ii );
	if( m_sel_id.ii == gnc-1 )
		m_sel_id.ii--;
	m_sel_count = 0;// 
	NewSelect( NULL, &m_sel_id, TRUE, FALSE );
	m_Doc->FindNodeLine(m_sel_id.i);
	HighlightGroup();
	//CancelSelection();
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = 0;// OnOPCornerDelete
}

void CFreePcbView::SelPolylineTest( CText ** part_found, CString * P, CString * V, CString * F, 
									int * nparts, int * nvalues, int * nfootprints, 
									int * without_attr, int max_attr )
{
	*nparts = 0;
	*nvalues = 0;
	*nfootprints = 0;
	(*P) += ":"dSPACE;
	(*V) += dSPACE;
	(*F) += dSPACE;
	//
	int no_part_attribute = 0;
	for( int pg=0; pg<m_Doc->Pages.GetNumPages(); pg++ )
	{
		m_Doc->Pages.GetTextList(pg,index_part_attr)->MarkAllTexts(0);
		m_Doc->Pages.GetTextList(pg,index_value_attr)->MarkAllTexts(0);
		m_Doc->Pages.GetTextList(pg,index_foot_attr)->MarkAllTexts(0);
	}
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * op = &m_Doc->m_outline_poly->GetAt(i);
		if( op->GetSel() )
		{
			int WA = 1;
			int attrName = -1;
			for( CText * t = m_Doc->ScanAttr( op, &attrName ); t; t = m_Doc->ScanAttr( op, &attrName ) )
			{
				if( t->isVISIBLE == 0 )
					continue;
				CString add_str = t->m_str + dSPACE;
				switch( attrName )
				{
				case index_part_attr:
					// mark ref text
					t->utility = 1;
					if( t->isVISIBLE )
					{
						*part_found = t;
						WA = 0;
						if( P->Find( add_str ) <= 0 )
						{
							(*nparts)++;
							if( (*nparts) <= max_attr )
								(*P) += add_str;
							else if( P->Right(1) != "." )
								(*P) += ".....";
						}
					}
					else if( *part_found == NULL )
						*part_found = t;
					break;
				case index_value_attr:
					// mark value text
					t->utility = 1;
					if( t->isVISIBLE )
					{
						if( V->Find( add_str ) <= 0 )
						{
							(*nvalues)++;
							if( (*nvalues) <= max_attr )
								(*V) += add_str;
							else if( V->Right(1) != "." )
								(*V) += ".....";
						}
					}
					break;
				case index_foot_attr:
					// mark foot text
					t->utility = 1;
					if( t->isVISIBLE )
					{
						if( F->Find( add_str ) <= 0 )
						{
							(*nfootprints)++;
							if( (*nfootprints) <= max_attr )
								(*F) += add_str;
							else if( F->Right(1) != "." )
								(*F) += ".....";
						}
					}
				}
			}
			no_part_attribute |= WA;
		}
	}
	if( without_attr )
		*without_attr = no_part_attribute;
}
//--------------------------------------------
//============================================
//--------------------------------------------
void CFreePcbView::MultipleAttributes( int MODE )
{
	if( m_sel_count == 0 )
		return;
	int E_MASK = ~0;
	if( m_protect )
		E_MASK = 0;
	else
	{
		clrbit( E_MASK, index_pin_attr );
		clrbit( E_MASK, index_net_attr );
		clrbit( E_MASK, index_desc_attr );
	}
	CString Parts = "";
	CString Values = G_LANGUAGE == 0 ? "different values:\n":"разные названия:";
	CString Footprints = G_LANGUAGE == 0 ? "different footprints:\n":"разные футпринты:";
	int WARNING_P = 0;
	int WARNING_V = 0;
	int WARNING_F = 0;
	int no_part_attribute = 0;
	CText * part_found = NULL;
	SelPolylineTest( &part_found, &Parts, &Values, &Footprints, &WARNING_P, &WARNING_V, &WARNING_F, &no_part_attribute, 50 );
	CString Mess;
	//============================
	if( MODE == index_value_attr )
	{
		clrbit( E_MASK, index_part_attr );
		clrbit( E_MASK, index_foot_attr );
	}
	else if( MODE == index_foot_attr )
	{
		clrbit( E_MASK, index_part_attr );
		clrbit( E_MASK, index_value_attr );
	}
	else if( WARNING_P == 1 && no_part_attribute )
	{
		Mess.Format(G_LANGUAGE == 0 ? 
			"You want to dock adjacent polylines to part%s\n\n":
			"Вы хотите прикрепить соседние полилинии к детали%s\n\n", Parts);
	}
	else if( WARNING_P > 1 )
	{
		clrbit( E_MASK, index_part_attr );
		if( no_part_attribute ) 
			Mess.Format(G_LANGUAGE == 0 ? 
				("You want to edit polylines with completely different text attributes. "\
				"The selected group contains both free polylines and parts. "\
				"By pressing the \"OK\" button you can enter the \"value\" and "\
				"\"footprint\" attributes for these parts. Free polylines will be "\
				"ignored.( %d parts found%s )\n\n"):
				("Вы хотите редактировать полилинии с совершенно разными текстовыми атрибутами. "\
				"Выбранная группа содержит как свободные полилинии, так и детали. "\
				"Нажав кнопку «Готово», вы можете ввести атрибуты «название» и "\
				"«футпринт» для этих деталей. Свободные полилинии будут "\
				"игнорированы.( Найдено %d деталей%s )\n\n"), WARNING_P, Parts);
		else
			Mess.Format("This group contains the %d parts"\
						"%s\n\n", WARNING_P, Parts );
	}
	//==============================================================
	if( (WARNING_V > 1 && (MODE == 0 || MODE == index_value_attr)) || 
		(WARNING_F > 1 && (MODE == 0 || MODE == index_foot_attr)))
	{	
		CString str;
		str.Format(G_LANGUAGE == 0 ? 
			"Warning! You want to set the same name for different attributes:":
			"Внимание! Вы хотите задать одно и то же имя для разных атрибутов:");
		Mess += str;
		if( WARNING_V > 1 && (MODE == 0 || MODE == index_value_attr) )
		{
			CString m;
			m.Format("\n\n%s", Values);
			Mess += m;
		}
		if( WARNING_F > 1 && (MODE == 0 || MODE == index_foot_attr) )
		{
			CString m;
			m.Format("\n\n%s", Footprints );
			Mess += m;
		}
	}
	int ret = IDOK;
	if( Mess.GetLength() )
	{
		int MB = MB_OKCANCEL;
		int ICON = MB_ICONINFORMATION;
		if( WARNING_P > 1 && WARNING_F > 1 )
		{
			ICON = MB_ICONERROR;		
			if( WARNING_F < 3 )
			{
				MB = MB_YESNO;
				Mess += G_LANGUAGE == 0 ? "\n\ncontinue anyway?":"\n\nпродолжить всё равно?";
			}
			else
			{
				Mess += (G_LANGUAGE == 0 ? 
					"\n\nThe attributes in this group include a large number of different footprints and cannot be changed.":
					"\n\nАтрибуты в этой группе включают в себя большое количество различных футпринтов и не могут быть одновременно изменены.");
				AfxMessageBox( Mess, ICON );
				return;
			}
		}
		else if( WARNING_V > 1 )
			ICON = MB_ICONWARNING;
		ret = AfxMessageBox( Mess, MB | ICON );
	}
	if( ret == IDOK || ret == IDYES )
	{
		// clear memory
		m_Doc->CheckComponentInLibrary(); 

		// unselect bad polylines
		if( no_part_attribute )
		{

		}
		CString cF = dSPACE;
		int f = Parts.Find( cF );
		if( f > 0 )
			Parts = Parts.Right( Parts.GetLength()-1 );
		Parts = Parts.Trim();
		//
		f = Values.Find( cF );
		if( f > 0 )
			Values = Values.Right( Values.GetLength()-f );
		Values = Values.Trim();
		//
		f = Footprints.Find( cF );
		if( f > 0 )
			Footprints = Footprints.Right( Footprints.GetLength()-f );
		Footprints = Footprints.Trim();
		//
		f = Parts.Find( cF );
		if( f > 0 && WARNING_P == 1 )
			Parts = Parts.Left( f );
		else if( WARNING_P > 1 )
			Parts = "Multiple";
		f = Values.Find( cF );
		if( f > 0 )
			Values = Values.Left( f );
		f = Footprints.Find( cF );
		if( f > 0 )
			Footprints = Footprints.Left( f );
		if( Parts.GetLength() == 0 && m_sel_id.type == ID_POLYLINE )
		{
			if( Values.GetLength() == 0 )
				if( m_sel_op.pAttr[index_value_attr] )
					Values = m_sel_op.pAttr[index_value_attr]->m_str;
			if( Footprints.GetLength() == 0 )
				if( m_sel_op.pAttr[index_foot_attr] )
					Footprints = m_sel_op.pAttr[index_foot_attr]->m_str;
		}

		// get all prefixes
		if( m_Doc->Pages.m_get_prefixes.GetLength() == 0 )
		{
			CDlgImportPart getPref;
			getPref.m_app_dir = m_Doc->m_app_dir;
			getPref.ReadPrefixes();
			m_Doc->Pages.m_get_prefixes = getPref.m_prefixes;
		}
		m_attr_size.Multiply( m_user_scale );
		CString E = "";
		CDlgAttr dlg;
		dlg.Initialize( &m_Doc->m_FootprintNames, m_Doc, &Parts, &E, &E, &E, &Footprints, &Values, 
			(m_Doc->m_footprint_extention.GetLength() ? &m_Doc->m_full_lib_dir : &m_Doc->m_app_dir), 
			&m_Doc->m_footprint_extention, &m_Doc->m_full_lib_dir, &m_Doc->Pages.m_get_prefixes, m_attr_size, m_Doc->m_units, E_MASK );
		int ret = dlg.DoModal();
		m_attr_size.Multiply( 1.0/m_user_scale );
		if (ret == IDOK )
		{
			m_draw_layer = 0;// MultipleAttributes
			m_attr_size = dlg.m_Asize;
			m_attr_size.Multiply( 1.0/m_user_scale );
			int map = 0;
			RECT R = m_Doc->m_dlist->GetHighlightedBounds( &map );
			int shift = 0;
			CText * new_part = NULL;
			CString ref_text = "";
			if( dlg.m_ref.GetLength() ) 
				ref_text = dlg.m_ref;

			// footprint name test in the current footprint library folder
			FootprintNameTest( &dlg.m_fname, &Footprints );

			//
			CString old_part = "";
			if( WARNING_P == 0 )
			{
				// create new part
				if( part_found )
					new_part = part_found;
				else
				{
					if( ref_text.GetLength() == 0 )
						return;
					new_part = m_Doc->Attr->m_Reflist->AddText( R.right, R.top+shift, 0, LAY_PART_NAME, 
																	m_attr_size.H_ref, m_attr_size.W_ref, m_Doc->m_default_font, &ref_text, 0 );
				}
				shift -= m_attr_size.H_ref*3/2 + 500000;
				
				// set ref pointers for new part
				for( int ip=0; ip<m_Doc->m_outline_poly->GetSize(); ip++ )
				{
					if( m_Doc->m_outline_poly->GetAt(ip).m_visible )
						if( m_Doc->m_outline_poly->GetAt(ip).GetSel() )
							m_Doc->m_outline_poly->GetAt(ip).pAttr[index_part_attr] = new_part;
				}
				// mark ref text
				new_part->utility = 1;
				new_part->MakeVisible();
			}
			else if( WARNING_P == 1 )
			{
				// part already used
				new_part = part_found; 
				// save old name
				old_part = part_found->m_str;
				// mark ref text
				new_part->utility = 1;
			}

			// step 1: save undo info
			int it = -1;
			CText * value = NULL;
			CText * footprint = NULL;
			for( CText * part = m_Doc->Attr->m_Reflist->GetNextText(&it); 
						 part; 
						 part = m_Doc->Attr->m_Reflist->GetNextText(&it) )
			{
				if( part->utility )
				{
					// get indexes
					m_Doc->AttributeIndexing( part, index_part_attr );
					if( part->m_polyline_start < 0 )
						continue;
					R = m_Doc->GetPartBounds( part, NULL );
					value = NULL;
					footprint = NULL;
					// find value, footprint
					CPolyLine * po = NULL;
					for( int get = part->m_polyline_start; get >= 0; get = po->Next[index_part_attr] )
					{
						po=&m_Doc->m_outline_poly->GetAt(get);
						if( po->pAttr[index_value_attr] && value == NULL )
							value = po->pAttr[index_value_attr];
						if( po->pAttr[index_foot_attr] && footprint == NULL )
							footprint = po->pAttr[index_foot_attr];
						if ( value && footprint )
							break;
					}
					if( dlg.m_partv.GetLength() )
					{
						if( value == NULL )
						{
							// create new value
							value = m_Doc->Attr->m_Valuelist->AddText( R.right, R.top+shift, 0, LAY_PART_VALUE, 
																m_attr_size.H_value, m_attr_size.W_value, m_Doc->m_default_font, &dlg.m_partv, 0 );
							shift -= m_attr_size.H_value*3/2 + 500000;
						}
					}
					if( m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_value_attr] == NULL )
						m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_value_attr] = value;
					if( dlg.m_fname.GetLength() )
					{
						if( footprint == NULL )
						{
							// create new foot
							footprint = m_Doc->Attr->m_Footlist->AddText( R.right, R.top+shift, 0, LAY_FOOTPRINT, 
																m_attr_size.H_foot, m_attr_size.W_foot, m_Doc->m_default_font, &dlg.m_fname, 0 );
							shift -= m_attr_size.H_foot*3/2 + 500000;
						}
					}
					if( m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_foot_attr] == NULL )
						m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_foot_attr] = footprint;
					
					// select
					m_Doc->SelectPartAttributes( part, TRUE );
				}
			}
			if( WARNING_P == 0 )
				new_part->isVISIBLE = 0;
			SaveUndoInfoForGroup( m_Doc->m_undo_list ); // MultipleAttributes
			if( WARNING_P == 0 )
				new_part->isVISIBLE = 1;
			if( WARNING_P == 0 || WARNING_P == 1 )
			{
				// attachment other polylines
				// set ref pointers AFTER saving undo info for group
				for( int ip=0; ip<m_Doc->m_outline_poly->GetSize(); ip++ )
				{
					if( m_Doc->m_outline_poly->GetAt(ip).GetSel() )
					{
						m_Doc->m_outline_poly->GetAt(ip).pAttr[index_part_attr] = new_part;
						m_Doc->m_outline_poly->GetAt(ip).pAttr[index_value_attr] = value;
						m_Doc->m_outline_poly->GetAt(ip).pAttr[index_foot_attr] = footprint;
						// delete nodes
						m_Doc->m_outline_poly->GetAt(ip).Node[0] = 0;
						m_Doc->m_outline_poly->GetAt(ip).Node[m_Doc->m_outline_poly->GetAt(ip).GetNumCorners()-1] = 0;
					}
				}
				// reset indexes
				m_Doc->AttributeIndexing( new_part, index_part_attr );
			}
			// step 2: apply changes
			it = -1;
			int part_renamed = 0;
			for( CText * part = m_Doc->Attr->m_Reflist->GetNextText(&it); 
						 part; 
						 part = m_Doc->Attr->m_Reflist->GetNextText(&it) )
			{
				if( part->utility && part->m_polyline_start >= 0 )
				{
					// apply changes
					if( getbit( E_MASK, index_value_attr ) )
					{
						CText * txt = m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_value_attr];
						if( txt )
						{
							Values = txt->m_str;
							if( dlg.m_partv.GetLength() && (dlg.m_ref.GetLength() || WARNING_P > 1) )
							{
								txt->Undraw();
								txt->m_str = dlg.m_partv;
								txt->m_nchars = dlg.m_partv.GetLength();
								if( m_attr_size.app_value )
								{
									txt->m_font_size = m_attr_size.H_value;
									txt->m_stroke_width = m_attr_size.W_value;
									txt->m_font_number = m_Doc->m_default_font;
								}
								txt->MakeVisible();
							}
							else
							{
								BOOL fDel = 1;
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() == 0 )
										if( m_Doc->m_outline_poly->GetAt(get).pAttr[index_value_attr] == txt )
										{
											fDel = 0;
											break;
										}
								}
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() )
										m_Doc->m_outline_poly->GetAt(get).pAttr[index_value_attr] = NULL;
								}
								if( fDel )
									txt->InVisible();
							}
						}
						else
							Values = "";
					}
					if( getbit( E_MASK, index_foot_attr ) )
					{
						CText * txt = m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_foot_attr];
						if( txt )
						{
							Footprints = txt->m_str;
							if( dlg.m_ref.GetLength() || WARNING_P > 1 )
							{
								if( dlg.m_fname.GetLength() == 0 )
									dlg.m_fname = NO_FOOTPRINT;
								txt->Undraw();
								txt->m_str = dlg.m_fname;
								txt->m_nchars = dlg.m_fname.GetLength();
								if( dlg.m_fname.Compare(NO_FOOTPRINT) == 0 )
								{
									txt->m_font_size = 0;
									txt->m_stroke_width = 0;
								}
								else if( m_attr_size.app_foot )
								{
									txt->m_font_size = m_attr_size.H_foot;
									txt->m_stroke_width = m_attr_size.W_foot;
									txt->m_font_number = m_Doc->m_default_font;
								}
								txt->MakeVisible();
							}
							else
							{
								BOOL fDel = 1;
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() == 0 )
										if( m_Doc->m_outline_poly->GetAt(get).pAttr[index_foot_attr] == txt )
										{
											fDel = 0;
											break;
										}
								}		
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() )
										m_Doc->m_outline_poly->GetAt(get).pAttr[index_foot_attr] = NULL;
								}
								if( fDel )
									txt->InVisible();
							}
						}
						else
							Footprints = "";
					}
					// ref modify
					BOOL I = FALSE;
					if( getbit( E_MASK, index_part_attr ) )
					{
						CText * txt = m_Doc->m_outline_poly->GetAt(part->m_polyline_start).pAttr[index_part_attr];
						if( txt )
						{
							if( dlg.m_ref.GetLength() )
							{
								txt->Undraw();
								txt->m_str = dlg.m_ref;
								txt->m_nchars = dlg.m_ref.GetLength();
								if( m_attr_size.app_ref )
								{
									txt->m_font_size = m_attr_size.H_ref;
									txt->m_stroke_width = m_attr_size.W_ref;
									txt->m_font_number = m_Doc->m_default_font;
								}
								txt->MakeVisible();
								part_renamed = TRUE;
							}
							else
							{
								BOOL fDel = 1;
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() == 0 )
										if( m_Doc->m_outline_poly->GetAt(get).pAttr[index_part_attr] == txt )
										{
											fDel = 0;
											break;
										}
								}	
								for( int get = txt->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() )
										m_Doc->m_outline_poly->GetAt(get).pAttr[index_part_attr] = NULL;
								}
								if( fDel )
									txt->InVisible();
								else
									I = TRUE;
							}
						}
					}
					if( dlg.m_ref.GetLength() || I || WARNING_P > 1 )
					{
						m_Doc->AttributeIndexing( part, index_part_attr );
						// upd library
						if( part )
							m_Doc->CheckComponentInLibrary( part, &Values, &Footprints );
					}
				}
			}
			if( part_renamed && old_part.GetLength() )
			{
				m_Doc->Renumber__Add( &old_part, &part_found->m_str, m_Doc->Pages.GetActiveNumber() );
			}
			// change polylines layer
			for( int ip=0; ip<m_Doc->m_outline_poly->GetSize(); ip++ )
			{
				if( m_Doc->m_outline_poly->GetAt(ip).GetSel() )
				{
					int nc = m_Doc->m_outline_poly->GetAt(ip).GetNumCorners();
					if( m_Doc->m_outline_poly->GetAt(ip).GetLayer() < LAY_ADD_1 )
					{
						int L_info = LAY_FREE_LINE;
						if( m_Doc->m_outline_poly->GetAt(ip).Check( index_part_attr ) )
							L_info = LAY_PART_OUTLINE;
						if( m_Doc->m_outline_poly->GetAt(ip).Check( index_net_attr ) )
							if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_net_attr]->m_tl == m_Doc->Attr->m_Netlist )
								L_info = LAY_CONNECTION;
						if( m_Doc->m_outline_poly->GetAt(ip).Check( index_pin_attr ) )
							L_info = LAY_PIN_LINE;
						m_Doc->m_outline_poly->GetAt(ip).SetLayer( L_info );
					}
					else if( nc )
					{
						m_Doc->m_outline_poly->GetAt(ip).Node[0] = 0;
						m_Doc->m_outline_poly->GetAt(ip).Node[nc-1] = 0;
					}
					id pid = m_Doc->m_outline_poly->GetAt(ip).GetId();
					int gnc = m_Doc->m_outline_poly->GetAt(ip).GetNumCorners();
					for( int ic=0; ic<gnc; ic++ )
					{
						pid.sst = ID_CORNER;
						pid.ii = ic;
						UnSelect( NULL, &pid );
					}
					if( m_Doc->m_outline_poly->GetAt(ip).GetClosed() == 0 )
						gnc--;
					for( int ic=0; ic<gnc; ic++ )
					{
						pid.sst = ID_SIDE;
						pid.ii = ic;
						UnSelect( NULL, &pid );
					}
					if( dlg.m_ref.GetLength() == 0 )
						m_Doc->m_outline_poly->GetAt(ip).ModifyGroupRect(&rect(0,0,0,0));

					// Draw, 
					m_Doc->m_outline_poly->GetAt(ip).Draw( m_Doc->m_dlist );

					// correct picture
					int src = m_Doc->m_outline_poly->GetAt(ip).GetPicture();
					if( src >= 0 )
					{
						RECT r = m_Doc->m_outline_poly->GetAt(ip).GetCornerBounds();
						m_Doc->m_dlist->MoveTemplate( &r, src, m_Doc->m_outline_poly->GetAt(ip).GetLayer() );
					}
				}
			}
			// unsel texts (pin attr, net attr, description)
			id tid( ID_TEXT_DEF );
			it = -1;
			for( CText * att = m_Doc->Attr->m_Netlist->GetNextText(&it); 
						 att; 
						 att = m_Doc->Attr->m_Netlist->GetNextText(&it) )
				if( att->m_selected )
					UnSelect( att, &tid );
			it = -1;
			for( CText * att = m_Doc->Attr->m_pDesclist->GetNextText(&it); 
						 att; 
						 att = m_Doc->Attr->m_pDesclist->GetNextText(&it) )
				if( att->m_selected )
					UnSelect( att, &tid );
			it = -1;
			for( CText * att = m_Doc->Attr->m_Pinlist->GetNextText(&it); 
						 att; 
						 att = m_Doc->Attr->m_Pinlist->GetNextText(&it) )
				if( att->m_selected )
					UnSelect( att, &tid );
			// clear flags
			m_sel_flags = 0;
			setbit( m_sel_flags, FLAG_SEL_TEXT );		
			if( m_sel_count <= 0 )
				CancelSelection();
			else
			{
				SetFKText( CUR_GROUP_SELECTED );
				m_Doc->m_dlist->CancelHighLight();
				HighlightGroup();
			}
			m_Doc->ProjectModified( TRUE );
		}	
		Invalidate( FALSE );// (doubtful)
	}
}

void CFreePcbView::OnOPSetAttributes()
{
	OPSetAttributes();
}

CString CFreePcbView::OPSetAttributes( CString * bDialog )
{
	// Reset CP Indexes
	m_Doc->Pages.m_get_suffixes = 0;
	CString complex_part_err = "";

	// for sync function save old str
	int MASK = ~0;
	if( m_protect )
		MASK = 0;
	else
	{
		if( CText * cmd = m_sel_op.Check(index_desc_attr) )
			if( cmd->m_str.Right(3) == "BLK" || cmd->m_str.Right(3) == "BOM" || cmd->m_str.Right(3) == "PTR" )
			{
				MASK = 0;
				setbit( MASK, index_desc_attr );
			}
		if( m_sel_op.GetLayer() >= LAY_ADD_1 )
			clrbit( MASK, index_net_attr );
	}
	CDlgAttr dlg;
	CString REF = "";
	if( m_sel_op.Check( index_part_attr ) )
			REF = m_sel_op.pAttr[index_part_attr]->m_str;
	CString PIN = "";
	if( m_sel_op.Check( index_pin_attr ) )
			PIN = m_sel_op.pAttr[index_pin_attr]->m_str;
	CString pDESC = "";
	if( m_sel_op.Check( index_desc_attr ) )
			pDESC = m_sel_op.pAttr[index_desc_attr]->m_str;
	CString NET = "";
	if( m_sel_op.Check( index_net_attr ) )
			NET = m_sel_op.pAttr[index_net_attr]->m_str;
	CString fNAME = "";
	if( m_sel_op.pAttr[index_foot_attr] )
		if( m_sel_op.pAttr[index_foot_attr]->isVISIBLE || REF.GetLength() == 0 )
			fNAME = m_sel_op.pAttr[index_foot_attr] ->m_str;
	CString VALUE = "";
	if( m_sel_op.pAttr[index_value_attr] )
		if( m_sel_op.pAttr[index_value_attr]->isVISIBLE || REF.GetLength() == 0 )
			VALUE = m_sel_op.pAttr[index_value_attr]->m_str;
	// mark 0
	m_Doc->Attr->m_Reflist->MarkAllTexts(0);
	// mark 1
	if( REF.GetLength() )
		m_sel_op.pAttr[index_part_attr]->utility = 1;
	m_attr_size.Multiply( m_user_scale );
	dlg.Initialize( &m_Doc->m_FootprintNames, m_Doc, &REF, &PIN, &pDESC, &NET, &fNAME, &VALUE, 
		(m_Doc->m_footprint_extention.GetLength() ? &m_Doc->m_full_lib_dir : &m_Doc->m_app_dir), 
		&m_Doc->m_footprint_extention, &m_Doc->m_full_lib_dir, &m_Doc->Pages.m_get_prefixes, m_attr_size, m_Doc->m_units, MASK );
	int ret;
	if( bDialog )
	{
		dlg.m_pindesc = *bDialog;
		ret = IDOK;
	}
	else
		ret = dlg.DoModal();
	m_attr_size.Multiply( 1.0/m_user_scale );
	if( ret == IDOK )
	{
		// clear memory
		m_Doc->CheckComponentInLibrary(); 

		// footprint name test
		FootprintNameTest( &dlg.m_fname, &fNAME );

		m_draw_layer = 0;// OnOPSetAttributes
		// copy attribute sizes
		m_attr_size = dlg.m_Asize;
		m_attr_size.Multiply( 1.0/m_user_scale );

		// save undo info for attributes
		id mem_id(m_sel_id);

		// if this polyline is a block contour
		// need renumber all components of block
		// find link to complex part
		// CText * ComplexPartLink = NULL;
		CString OldSuffix = "";
		CString OldLink = "";
		int OldPage = -1;
		CString OLD_STR = "";
		if( CText * blk=m_sel_op.Check( index_desc_attr ) )
		{
			OLD_STR = blk->m_str; // save str
			// Get Suffix
			if( blk->m_str.Right(3) == "BLK" )
			{
				OldPage = m_Doc->ExtractBLK( &blk->m_str, &OldSuffix, &OldLink );
			}
		}

		// apply changes
		CText ** attr;
		int text_layer;
		CString dlg_str;
		CString old_part = "";
		if( m_sel_op.Check( index_part_attr ) )
			old_part = m_sel_op.pAttr[index_part_attr]->m_str;
		int shift = 0;
		int part_renamed = 0;
		CText * part_delete = NULL;
		for( int iAtt=0; iAtt<num_attributes*2; iAtt++ )
		{
			attr = &m_sel_op.pAttr[iAtt%num_attributes];
			int H = m_attr_size.H_ref;
			int W = m_attr_size.W_ref;
			int app = 0;
			switch(iAtt)
			{
			case index_part_attr+num_attributes:
				SaveUndoInfoForGroup( m_Doc->m_undo_list ); // MultipleAttributes
			case index_part_attr:
				text_layer = LAY_PART_NAME;
				dlg_str = dlg.m_ref;
				if( m_attr_size.app_ref )
					app = 1;
				H = m_attr_size.H_ref;
				W = m_attr_size.W_ref;
				break;
			case index_pin_attr:
			case index_pin_attr+num_attributes:
				text_layer = LAY_PIN_NAME;
				dlg_str = dlg.m_pin;
				if( m_attr_size.app_pin )
					app = 1;
				H = m_attr_size.H_pin;
				W = m_attr_size.W_pin;
				break;
			case index_net_attr:
			case index_net_attr+num_attributes:
				text_layer = LAY_NET_NAME;
				dlg_str = dlg.m_net;
				if( m_attr_size.app_net )
					app = 1;
				H = m_attr_size.H_net;
				W = m_attr_size.W_net;
				break;
			case index_desc_attr:
			case index_desc_attr+num_attributes:
				text_layer = LAY_PIN_DESC;
				dlg_str = dlg.m_pindesc;
				if( m_attr_size.app_pindesc )
					app = 1;
				H = m_attr_size.H_pindesc;
				W = m_attr_size.W_pindesc;
				break;
			case index_foot_attr:
			case index_foot_attr+num_attributes:
				text_layer = LAY_FOOTPRINT;
				dlg_str = dlg.m_fname;
				if( m_attr_size.app_foot )
					app = 1;
				H = m_attr_size.H_foot;
				W = m_attr_size.W_foot;
				if( dlg_str.Compare(NO_FOOTPRINT) == 0 )
				{
					H = 0;
					W = 0;
				}
				break;
			case index_value_attr:
			case index_value_attr+num_attributes:
				text_layer = LAY_PART_VALUE;
				dlg_str = dlg.m_partv;
				if( m_attr_size.app_value )
					app = 1;
				H = m_attr_size.H_value;
				W = m_attr_size.W_value;
				break;
			default:
				ASSERT(0);
			}
			if( iAtt < num_attributes )
			{
				// for undo info
				id idt( ID_TEXT_DEF );
				if( dlg_str.GetLength() )
				{
					BOOL ADD = 0;	
					if( (*attr) )
						if( (*attr)->m_tl == m_Doc->m_tlist )
							if( dlg_str.Compare((*attr)->m_str) )
								ADD = 1;
					if( (*attr) == NULL || ADD )
					{
						shift -= (H*2 + W/2);
						CTextList * tl = m_Doc->GetTextListbyLayer( text_layer );
						(*attr) = tl->AddText( m_sel_op.GetX(m_sel_id.ii),
														m_sel_op.GetY(m_sel_id.ii)+shift, 
														0, text_layer, H, W, m_Doc->m_default_font, &dlg_str, 0 );			
					}
					else if( dlg_str.Compare((*attr)->m_str) && iAtt == index_part_attr )
						part_renamed = TRUE; 
					NewSelect( *attr, &idt, 0, 0 );
				}
				else if( (*attr) )
				{
					if( (*attr)->isVISIBLE )
						NewSelect( *attr, &idt, 0, 0 );
				}
				// restore id
				m_sel_id = mem_id;
			}
			else
			{
				// apply changes
				int m_att = iAtt - num_attributes;
				if( m_att == index_pin_attr || m_att == index_net_attr || m_att == index_desc_attr )
					m_Doc->m_outline_poly->GetAt(m_sel_id.i).Next[m_att] = -1;// remove chain
				// string present
				if( dlg_str.GetLength() )
				{
					if( m_att == index_pin_attr )
						m_Doc->m_outline_poly->GetAt(m_sel_id.i).UnClose();
					(*attr)->Undraw();
					(*attr)->m_str = dlg_str;
					(*attr)->m_nchars = dlg_str.GetLength();
					if( app )
					{
						(*attr)->m_font_size = H;
						(*attr)->m_stroke_width = W;
					}
					(*attr)->m_polyline_start = m_sel_id.i;
					(*attr)->MakeVisible();
				}
				else if( (*attr) )
				{
					if( (*attr)->isVISIBLE )
					{
						BOOL fDel = TRUE;
						if( m_att == index_part_attr || m_att == index_value_attr || m_att == index_foot_attr )
						{
							for( int get = (*attr)->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
							{
								if( m_Doc->m_outline_poly->GetAt(get).GetSel() == 0 )
									if( m_Doc->m_outline_poly->GetAt(get).pAttr[m_att] == (*attr) )
									{
										fDel = 0;
										break;
									}
							}
						}
						if( fDel )
						{
							(*attr)->InVisible();
							(*attr) = NULL;
							// delete group rect
							m_sel_op.ModifyGroupRect( &rect(0,0,0,0) );
						}
						else if( m_att == index_part_attr )
							part_delete = (*attr);
					}
				}
			}
		}
		// part modified
		if( part_delete )
		{
			m_Doc->SelectPart( part_delete, NULL, 0 );
			m_Doc->SelectGroupAttributes();
			SaveUndoInfoForGroup( m_Doc->m_undo_list ); // MultipleAttributes
			// restore id
			m_sel_id = mem_id;
			m_sel_op.pAttr[index_part_attr] = NULL;
			m_sel_op.pAttr[index_value_attr] = NULL;
			m_sel_op.pAttr[index_foot_attr] = NULL;
			if( m_sel_op.GetLayer() < LAY_ADD_1 )
			{
				if( m_sel_op.Check( index_pin_attr ))
					m_sel_op.SetLayer( LAY_PIN_LINE );
				else if( m_sel_op.Check( index_net_attr ))
					m_sel_op.SetLayer( LAY_CONNECTION );
				else
					m_sel_op.SetLayer( LAY_FREE_LINE );
			}
			RECT R = rect(0,0,0,0);
			m_sel_op.ModifyGroupRect( &R );
			m_Doc->AttributeIndexing( part_delete, index_part_attr );
			m_sel_op.Draw( m_Doc->m_dlist );
			m_Doc->ProjectModified( TRUE );
			CancelSelection();
			return complex_part_err;
		}
		else if( part_renamed && old_part.GetLength() )
		{
			m_Doc->Renumber__Add( &old_part, &m_sel_op.pAttr[index_part_attr]->m_str, m_Doc->Pages.GetActiveNumber() );
		}
		// Indexing part
		if( !part_delete )
			if( m_sel_op.Check( index_part_attr ) )
			{
				m_Doc->AttributeIndexing( m_sel_op.pAttr[index_part_attr], index_part_attr );
				// upd library
				m_Doc->CheckComponentInLibrary( m_sel_op.Check( index_part_attr ), &VALUE, &fNAME );
			}
		// set layer
		if( m_sel_op.GetLayer() < LAY_ADD_1 )
		{
			if( m_sel_op.Check( index_pin_attr ))
				m_sel_op.SetLayer( LAY_PIN_LINE );
			else if( m_sel_op.Check( index_net_attr ))
				m_sel_op.SetLayer( LAY_CONNECTION );
			else if( m_sel_op.Check( index_part_attr ))
				m_sel_op.SetLayer( LAY_PART_OUTLINE );
			else
				m_sel_op.SetLayer( LAY_FREE_LINE );
		}
		// checking printable area index
		if( CText * desc = m_sel_op.Check( index_desc_attr ) )
		{
			if( desc->m_str.Find("PrintableArea") > 0 )
			{
				id fa;
				fa.i = m_sel_id.i;
				fa = m_Doc->Pages.FindPrintableArea( &fa );
				if( fa.st > 0 )// same index found
				{
					int ok = AfxMessageBox(G_LANGUAGE == 0 ? 
						"This page number already used, so the printable area indexes of other pages will be offset by one":
						"Этот номер страницы уже используется, поэтому индексы печатной области других страниц будут смещены на единицу", MB_ICONINFORMATION | MB_OKCANCEL);
					if( ok == IDCANCEL )
					{
						desc->m_str = OLD_STR;
						desc->m_nchars = desc->m_str.GetLength();
						desc->MakeVisible();
					}
					else
					{
						fa = m_sel_id;
						m_Doc->Pages.ShiftPrintableAreaIndexes( &fa, +1 );
					}
				}
				else
				{
					int current_index = fa.type;
					int next_index = fa.sst;
					if( next_index < INT_MAX )
						if( next_index - current_index > 1 )
						{
							fa = m_sel_id;
							m_Doc->Pages.ShiftPrintableAreaIndexes( &fa, current_index - next_index + 1 );
						}
				}
			}
		}
		// sync
		if( m_sel_op.pAttr[index_part_attr] )
		{
			CString * REF = &m_sel_op.pAttr[index_part_attr]->m_str;
			if( PIN.GetLength() && m_sel_op.pAttr[index_pin_attr] )
				m_Doc->AttributeSync( m_sel_op.pAttr[index_part_attr], &PIN, m_sel_op.pAttr[index_pin_attr], index_pin_attr );
			if( NET.GetLength() && m_sel_op.pAttr[index_net_attr] )
				m_Doc->AttributeSync( m_sel_op.pAttr[index_part_attr], &NET, m_sel_op.pAttr[index_net_attr], index_net_attr );
			if( pDESC.GetLength() && m_sel_op.pAttr[index_desc_attr] )
				m_Doc->AttributeSync( m_sel_op.pAttr[index_part_attr], &pDESC, m_sel_op.pAttr[index_desc_attr], index_desc_attr );
		}
		// draw, correct picture
		m_sel_op.Draw( m_Doc->m_dlist );
		int src = m_sel_op.GetPicture();
		if( src >= 0 )
		{
			RECT r = m_sel_op.GetCornerBounds();
			m_Doc->m_dlist->MoveTemplate( &r, src, m_sel_op.GetLayer() );
		}
		if( m_sel_count > 1 )
		{
			// if there are these 
			// attributes then we 
			// leave selected only them
			if( m_sel_op.Check( index_pin_attr ) || m_sel_op.Check( index_net_attr ) || m_sel_op.Check( index_desc_attr ) )
			{
				if( CText * desc = m_sel_op.Check( index_desc_attr ) )
					if( desc->m_tl )
					{
						if( desc->m_str.Right(7) == "BLK_PTR" )
						{
							CString NAME, REF, PIN;
							int pg = m_Doc->ExtractBLK_PTR( &desc->m_str, &NAME, &REF, &PIN );
							int iof_cmd = desc->m_str.Find("|");
							if( iof_cmd > 0 && PIN.GetLength() )
							{
								CString CMD = desc->m_str.Right( desc->m_str.GetLength() - iof_cmd );
								desc->Undraw();
								desc->m_str = PIN + "'" + CMD;
								desc->m_nchars = desc->m_str.GetLength();
								desc->MakeVisible();
							}
						}
						else if( desc->m_str.Right(3) == "BLK" )
						{
							m_sel_id.i = m_Doc->Pages.FindBlkPtrs( m_sel_id.i, TRUE );
							SaveUndoInfoForGroup( m_Doc->m_undo_list );
							m_sel_id.i = m_Doc->Pages.FindBlkPtrs( m_sel_id.i );
							// Renumber Complex Part
							if( OldPage >= 0 )
							{
								CString NewSuffix = "";
								CString NewLink = "";
								int NewPage = -1;
								NewPage = m_Doc->ExtractBLK( &desc->m_str, &NewSuffix, &NewLink );
								if( NewSuffix.Compare( OldSuffix ) || NewLink.Compare( OldLink ) || NewPage != OldPage )
								{
									complex_part_err = SelectBlock( &m_Doc->m_outline_poly->GetAt( desc->m_polyline_start ), m_Doc->Pages.GetActiveNumber(), TRUE );
									if( desc->m_polyline_start >= 0 )
									{	
										if( complex_part_err.GetLength() == 0 )// !!! проверка на ошибки обязательна
										{
											if( NewSuffix.Compare( OldSuffix ) )
											{
												int itt = -1;
												CText * ComplexPartLink = m_Doc->Pages.GetTextList( NewPage, index_part_attr )->GetText( &NewLink, &itt );
												if( ComplexPartLink )
													if( m_Doc->RenumberComplexPart( desc, &OldSuffix, &NewSuffix ) == 0 )
														complex_part_err = "ERROR";
											}
										}
										if( complex_part_err.GetLength() )
										{
											desc->m_str = OLD_STR;
											desc->m_nchars = desc->m_str.GetLength();
											desc->MakeVisible();
											SelectBlock( &m_Doc->m_outline_poly->GetAt( desc->m_polyline_start ), m_Doc->Pages.GetActiveNumber(), 1 );
										}
									}
								}
							}
						}
						else if (desc->m_str.Right(7) == "PCBVIEW")
						{
							if (m_Doc->m_num_additional_layers < 12)
							{
								int ind = m_Doc->m_num_layers;
								m_Doc->m_num_additional_layers = 12;
								m_Doc->m_num_layers = m_Doc->m_num_additional_layers + LAY_ADD_1;
								for (int iL = ind; iL < m_Doc->m_num_layers; iL++)
								{
									m_Doc->m_vis[iL] = TRUE;
									m_Doc->m_dlist->SetLayerVisible(iL, 1);
									m_Doc->m_dlist->SetLayerRGB(iL, m_Doc->m_rgb[iL][0], m_Doc->m_rgb[iL][1], m_Doc->m_rgb[iL][2]);
								}
								ShowActiveLayer(m_Doc->m_num_additional_layers);
							}
							OnPolylineUpdatePcbView(m_Doc, m_sel_id.i, &OLD_STR);
							return complex_part_err;
						}
						else
						{
							desc->m_tl->TimeUpdate( desc );
							DrawPATH( desc, m_Doc->Pages.GetActiveNumber() );
							DrawBOM( desc );
						}
					}
				if( m_sel_id.type == ID_POLYLINE )
				{
					CPolyLine * save_p = &m_sel_op;
					CancelSelection(0);
					id ID(ID_TEXT_DEF);
					// first select 
					// new texts
					if( PIN.GetLength() == 0 )
						if( save_p->pAttr[index_pin_attr] )
							NewSelect( save_p->pAttr[index_pin_attr], &ID, 0, 0 );
					if( NET.GetLength() == 0 )
						if( save_p->pAttr[index_net_attr] )
							NewSelect( save_p->pAttr[index_net_attr], &ID, 0, 0 );
					if( pDESC.GetLength() == 0 )
						if( save_p->pAttr[index_desc_attr] )
							NewSelect( save_p->pAttr[index_desc_attr], &ID, 0, 0 );
					// but if not new 
					// then changed
					if( m_sel_count == 0 )
					{
						if( PIN.GetLength() && save_p->pAttr[index_pin_attr] )
							if( PIN.Compare( save_p->pAttr[index_pin_attr]->m_str ) )
								NewSelect( save_p->pAttr[index_pin_attr], &ID, 0, 0 );
						if( NET.GetLength() && save_p->pAttr[index_net_attr] )
							if( NET.Compare( save_p->pAttr[index_net_attr]->m_str ) )
								NewSelect( save_p->pAttr[index_net_attr], &ID, 0, 0 );
						if( pDESC.GetLength() && save_p->pAttr[index_desc_attr] )
							if( pDESC.Compare( save_p->pAttr[index_desc_attr]->m_str ) )
								NewSelect( save_p->pAttr[index_desc_attr], &ID, 0, 0 );
					}
					// part attr
					if( m_sel_count == 0 )
					{
						if( save_p->pAttr[index_part_attr] )
							NewSelect( save_p->pAttr[index_part_attr], &ID, 0, 0 );
						if( save_p->pAttr[index_value_attr] )
							NewSelect( save_p->pAttr[index_value_attr], &ID, 0, 0 );
						if( save_p->pAttr[index_foot_attr] )
							NewSelect( save_p->pAttr[index_foot_attr], &ID, 0, 0 );
					}
				}
			}
			else
			{
				// leave selected all part attributes
				UnSelect( NULL, &mem_id );
				clrbit( m_sel_flags, FLAG_SEL_OP );	
				clrbit( m_sel_flags, FLAG_SEL_SIDE );	
				clrbit( m_sel_flags, FLAG_SEL_VTX );
			}
			SetCursorMode( CUR_GROUP_SELECTED );
			m_Doc->m_dlist->CancelHighLight();
			HighlightGroup();
		}
		m_Doc->ProjectModified( TRUE );
		m_draw_layer = 0;
	}
	return complex_part_err;
}
//===============================================================================================
// insert a new corner in a side of a outline
//
void CFreePcbView::OnOPSideAddCorner()
{
	CDC *pDC = GetDC();
	pDC->SelectClipRgn( &m_pcb_rgn );
	SetDCToWorldCoords( pDC );
	CPoint p = m_last_mouse_point;
	m_sel_op.StartDraggingToInsertCorner( pDC, m_sel_id.ii, p.x, p.y, 2 );
	SetCursorMode( CUR_DRAG_OP_INSERT );
	ReleaseDC( pDC );
}


//===============================================================================================
// detect state where nothing is selected or being dragged
//
BOOL CFreePcbView::CurNone()
{
	return( m_cursor_mode == CUR_NONE_SELECTED );
}
//===============================================================================================
// detect any selected state
//
BOOL CFreePcbView::CurSelected()
{
	return( m_cursor_mode > CUR_NONE_SELECTED && m_cursor_mode < CUR_NUM_SELECTED_MODES );
}
//===============================================================================================
// detect any dragging state
//
BOOL CFreePcbView::CurDragging()
{
	if( m_dragging_mode )
		return 1;
	return( m_cursor_mode > CUR_NUM_SELECTED_MODES && m_cursor_mode < CUR_NUM_MODES );
}
//===============================================================================================
//===============================================================================================
// detect states using placement grid
//
BOOL CFreePcbView::CurDraggingPlacement()
{
	return ( getbit( m_sel_flags, FLAG_SEL_TEXT ) == 0 || getbit( m_sel_flags, FLAG_SEL_OP ) ) ;
}
//===============================================================================================
// snap cursor if required and set m_last_cursor_point
//
void CFreePcbView::SnapCursorPoint( CPoint wp, UINT nFlags )
{
	static int mem_scale_factor = 1;
	static int mem_1 = 1;
	static int mem_2 = 2;

	// see if we need to snap at all
	if( CurDragging() )
	{
		// yes, set snap modes based on cursor mode and SHIFT and CTRL keys
		// for other dragging modes, always use grid points with no inflection
		m_snap_mode = SM_GRID_POINTS;
		m_inflection_mode = IM_NONE;
		m_Doc->m_dlist->SetInflectionMode( m_inflection_mode );

		// set grid spacing
		int grid_spacing;
		if( (m_dragging_mode && m_sel_count == 0) || 
			m_cursor_mode == CUR_DRAG_MEASURE_1 ||
			m_cursor_mode == CUR_DRAG_MEASURE_2 )
		{
			grid_spacing = 1000;
		}
		else if( CurDraggingPlacement() )
		{
			grid_spacing = m_Doc->m_part_grid_spacing/m_user_scale;
		}
		else 
		{
			grid_spacing = m_Doc->m_routing_grid_spacing/m_user_scale;
		}
		// see if we need to snap to angle
		if( m_Doc->m_snap_angle && (wp != m_snap_angle_ref)
			&& ( m_cursor_mode == CUR_DRAG_OP_1 || m_cursor_mode == CUR_DRAG_OP ) )
		{
			// yes, check snap mode
			if( m_snap_mode == SM_GRID_LINES )
			{
				// patch to snap to grid lines, contributed by ???
				// modified by AMW to work when cursor x,y are < 0
				// offset cursor and ref positions by integral number of grid spaces
				// to make all values positive
				double offset_grid_spaces;
				modf( (double)INT_MAX/grid_spacing, &offset_grid_spaces );
				double offset = offset_grid_spaces*grid_spacing;
				double off_x = wp.x + offset;
				double off_y = wp.y + offset;
				double ref_x = m_snap_angle_ref.x + offset;
				double ref_y = m_snap_angle_ref.y + offset;
				//find nearest snap angle to an integer division of 90
				int snap_angle = m_Doc->m_snap_angle;
				if(90 % snap_angle != 0)
				{
					int snap_pos = snap_angle;
					int snap_neg = snap_angle;
					while(90 % snap_angle != 0)
					{
						snap_pos++;
						snap_neg--;
						if(snap_pos >= 90)
							snap_pos = 90;
						if(snap_neg <= 1)
							snap_neg = 1;
						if(90 % snap_pos == 0)
							snap_angle = snap_pos;
						if(90 % snap_neg == 0)
							snap_angle = snap_neg;
					}
				}

				//snap the x and y coordinates to the appropriate angle
				double angle_grid = snap_angle*M_PI/180.0;
				double dx = off_x - ref_x;
				double dy = off_y - ref_y;
				double angle = atan2(dy,dx) + 2*M_PI; //make it a positive angle
				if(angle > 2*M_PI)
					angle -= 2*M_PI;
				double angle_angle_grid = angle/angle_grid;
				int sel_snap_angle = (int)angle_angle_grid + ((angle_angle_grid - (double)((int)angle_angle_grid)) > 0.5 ? 1 : 0);
				double point_angle = angle_grid*sel_snap_angle; //result of angle calculation
				CString test, test_grid;
				test.Format("point_angle: %f\r\n",point_angle);
				test_grid.Format("grid_spacing: %d\r\n",grid_spacing);


				//find the distance along that angle
				//match the distance the actual point is from the start point
				//double dist = sqrt(dx*dx + dy*dy);
				double dist = dx*cos(point_angle)+dy*sin(point_angle);

				double distx = dist*cos(point_angle);
				double disty = dist*sin(point_angle);

				double xpos = ref_x + distx;
				double ypos = ref_y + disty;


				//special case horizontal lines and vertical lines
				// just to make sure floating point error doesn't cause any problems
				if(APPROX(point_angle,0) || APPROX(point_angle,2*M_PI) || APPROX(point_angle,M_PI))
				{
					//horizontal line
					//snap x component to nearest grid
					off_y = (int)(ypos + 0.5);
					double modval = fmod(xpos,(double)grid_spacing);
					if(modval > grid_spacing/2.0)
					{
						//round up to nearest grid space
						off_x = xpos + ((double)grid_spacing - modval);
					}
					else
					{
						//round down to nearest grid space
						off_x = xpos - modval;
					}
				}
				else if(APPROX(point_angle,M_PI/2) || APPROX(point_angle,3*M_PI/2))
				{
					//vertical line
					//snap y component to nearest grid
					off_x = (int)(xpos + 0.5);
					double modval = fabs(fmod(ypos,(double)grid_spacing));
					int test = modval * grid_spacing - offset;
					if(modval > grid_spacing/2.0)
					{
						off_y = ypos + ((double)grid_spacing - modval);
					}
					else
					{
						off_y = ypos - modval;
					}
				}
				else
				{
					//normal case
					//snap x and y components to nearest grid line along the same angle
					//calculate grid lines surrounding point
					int minx = ((int)(xpos/(double)grid_spacing))*grid_spacing - (xpos < 0);
					//int minx = (int)fmod(xpos,(double)grid_spacing);
					int maxx = minx + grid_spacing;
					int miny = ((int)(ypos/(double)grid_spacing))*grid_spacing - (ypos < 0);
					//int miny = (int)fmod(ypos,(double)grid_spacing);
					int maxy = miny + grid_spacing;

					//calculate the relative distances to each of those grid lines from the ref point
					int rminx = minx - ref_x;
					int rmaxx = maxx - ref_x;
					int rminy = miny - ref_y;
					int rmaxy = maxy - ref_y;

					//calculate the length of the hypotenuse of the triangle
					double maxxh = dist*(double)rmaxx/distx;
					double minxh = dist*(double)rminx/distx;
					double maxyh = dist*(double)rmaxy/disty;
					double minyh = dist*(double)rminy/disty;

					//some stupidly large value.  One of the results MUST be smaller than this unless the grid is this large (unlikely)
					double mindist = 1e100;
					int select = 0;

					if(fabs(maxxh - dist) < mindist)
					{
						select = 1;
						mindist = fabs(maxxh - dist);
					}
					if(fabs(minxh - dist) < mindist)
					{
						select = 2;
						mindist = fabs(minxh - dist);
					}
					if(fabs(maxyh - dist) < mindist)
					{
						select = 3;
						mindist = fabs(maxyh - dist);
					}
					if(fabs(minyh - dist) < mindist)
					{
						select = 4;
						mindist = fabs(minyh - dist);
					}

					switch(select)
					{
					case 1:
						//snap to line right of point
						off_x = maxx;
						off_y = (int)(disty*(double)rmaxx/distx + (double)(ref_y) + 0.5);
						break;
					case 2:
						//snap to line left of point
						off_x = minx;
						off_y = (int)(disty*(double)rminx/distx + (double)(ref_y) + 0.5);
						break;
					case 3:
						//snap to line above point
						off_x = (int)(distx*(double)rmaxy/disty + (double)(ref_x) + 0.5);
						off_y = maxy;
						break;
					case 4:
						//snap to line below point
						off_x = (int)(distx*(double)rminy/disty + (double)(ref_x) + 0.5);
						off_y = miny;
						break;
					default:
						ASSERT(0);//error
					}

				}
				// remove offset and correct for round-off
				double ttest = off_x - offset;
				if( ttest >= 0.0 )
					wp.x = ttest + 0.5;
				else
					wp.x = ttest - 0.5;
				ttest = off_y - offset;
				if( ttest >= 0.0 )
					wp.y = ttest + 0.5;
				else
					wp.y = ttest - 0.5;
			}
			else
			{
				// old code
				// snap to angle only if the starting point is on-grid
				double ddx = fmod( (double)(m_snap_angle_ref.x), grid_spacing );
				double ddy = fmod( (double)(m_snap_angle_ref.y), grid_spacing );
				if( fabs(ddx) < 0.5 && fabs(ddy) < 0.5 )
				{
					// starting point is on-grid, snap to angle
					// snap to n*45 degree angle
					const double pi = 3.14159265359;
					double dx = wp.x - m_snap_angle_ref.x;
					double dy = wp.y - m_snap_angle_ref.y;
					double dist = sqrt( dx*dx + dy*dy );
					double dist45 = dist/sqrt(2.0);
					if( grid_spacing > _2540 )
					{
						int d;
						d = (int)(dist/grid_spacing+0.5);
						dist = d*grid_spacing;
						d = (int)(dist45/grid_spacing+0.5);
						dist45 = d*grid_spacing;
					}
					if( m_Doc->m_snap_angle == 45 )
					{
						// snap angle = 45 degrees, divide circle into 8 octants
						double angle = atan2( dy, dx );
						if( angle < 0.0 )
							angle = 2.0*pi + angle;
						angle += pi/8.0;
						double d_quad = angle/(pi/4.0);
						int oct = d_quad;
						switch( oct )
						{
						case 0:
							wp.x = m_snap_angle_ref.x + dist;
							wp.y = m_snap_angle_ref.y;
							break;
						case 1:
							wp.x = m_snap_angle_ref.x + dist45;
							wp.y = m_snap_angle_ref.y + dist45;
							break;
						case 2:
							wp.x = m_snap_angle_ref.x;
							wp.y = m_snap_angle_ref.y + dist;
							break;
						case 3:
							wp.x = m_snap_angle_ref.x - dist45;
							wp.y = m_snap_angle_ref.y + dist45;
							break;
						case 4:
							wp.x = m_snap_angle_ref.x - dist;
							wp.y = m_snap_angle_ref.y;
							break;
						case 5:
							wp.x = m_snap_angle_ref.x - dist45;
							wp.y = m_snap_angle_ref.y - dist45;
							break;
						case 6:
							wp.x = m_snap_angle_ref.x;
							wp.y = m_snap_angle_ref.y - dist;
							break;
						case 7:
							wp.x = m_snap_angle_ref.x + dist45;
							wp.y = m_snap_angle_ref.y - dist45;
							break;
						case 8:
							wp.x = m_snap_angle_ref.x + dist;
							wp.y = m_snap_angle_ref.y;
							break;
						default:
							ASSERT(0);
							break;
						}
					}
					else
					{
						// snap angle is 90 degrees, divide into 4 quadrants
						double angle = atan2( dy, dx );
						if( angle < 0.0 )
							angle = 2.0*pi + angle;
						angle += pi/4.0;
						double d_quad = angle/(pi/2.0);
						int quad = d_quad;
						switch( quad )
						{
						case 0:
							wp.x = m_snap_angle_ref.x + dist;
							wp.y = m_snap_angle_ref.y;
							break;
						case 1:
							wp.x = m_snap_angle_ref.x;
							wp.y = m_snap_angle_ref.y + dist;
							break;
						case 2:
							wp.x = m_snap_angle_ref.x - dist;
							wp.y = m_snap_angle_ref.y;
							break;
						case 3:
							wp.x = m_snap_angle_ref.x;
							wp.y = m_snap_angle_ref.y - dist;
							break;
						case 4:
							wp.x = m_snap_angle_ref.x + dist;
							wp.y = m_snap_angle_ref.y;
							break;
						default:
							ASSERT(0);
							break;
						}
					}
				}
			}
		}
		else
			m_snap_mode = SM_GRID_POINTS;

		// snap to grid points if needed
		if( m_snap_mode == SM_GRID_POINTS && grid_spacing > _2540 )
		{
			// snap to grid
			// get position in integral units of grid_spacing
			if( wp.x > 0 )
				wp.x = (wp.x + grid_spacing/2)/grid_spacing;
			else
				wp.x = (wp.x - grid_spacing/2)/grid_spacing;
			if( wp.y > 0 )
				wp.y = (wp.y + grid_spacing/2)/grid_spacing;
			else
				wp.y = (wp.y - grid_spacing/2)/grid_spacing;
			// multiply by grid spacing, adding or subracting 0.5 to prevent round-off
			// when using a fractional grid
			double test = wp.x * grid_spacing;
			if( test > 0.0 )
				test += 0.5;
			else
				test -= 0.5;
			wp.x = test;
			test = wp.y * grid_spacing;
			if( test > 0.0 )
				test += 0.5;
			else
				test -= 0.5;
			wp.y = test;
		}

		// update drag operation
		if( wp != m_last_cursor_point )
		{
			CDC *pDC = GetDC();
			pDC->SelectClipRgn( &m_pcb_rgn );
			SetDCToWorldCoords( pDC );
			if( (int)m_pcbu_per_pixel != mem_scale_factor || mem_1 != (int)m_org_x || mem_2 != (int)m_org_y )
			{
				m_Doc->m_dlist->Draw( pDC, NULL );
				mem_scale_factor = (int)m_pcbu_per_pixel;
				mem_1 = (int)m_org_x;
				mem_2 = (int)m_org_y;
				return;
			}
			else
				m_Doc->m_dlist->Drag( pDC, wp.x, wp.y );
			ReleaseDC( pDC );
			// show relative distance
			if( m_cursor_mode == CUR_DRAG_GROUP
			|| m_cursor_mode == CUR_DRAG_GROUP_ADD
			|| m_cursor_mode ==  CUR_DRAG_OP_MOVE
			|| m_cursor_mode ==  CUR_DRAG_MEASURE_2 )
			{
				ShowRelativeDistance( wp.x - m_from_pt.x, wp.y - m_from_pt.y );
			}
		}
	}
	mem_scale_factor = (int)m_pcbu_per_pixel;
	mem_1 = (int)m_org_x;
	mem_2 = (int)m_org_y;

	// update cursor position
	m_last_cursor_point = wp;
	ShowCursor();
}
//===============================================================================================
LONG CFreePcbView::OnChangeVisibleGrid( UINT wp, LONG lp )
{
	m_Doc->m_visual_grid_spacing = max(0,lp);
	m_Doc->m_dlist->SetVisibleGrid( TRUE, m_Doc->m_visual_grid_spacing/m_user_scale );	
	m_Doc->ProjectModified( TRUE );
	Invalidate(FALSE);
	return 0;
}
//===============================================================================================
LONG CFreePcbView::OnChangePlacementGrid( UINT wp, LONG lp )
{
	m_Doc->m_part_grid_spacing = max(0,lp);
	m_Doc->ProjectModified( TRUE );
	return 0;
}
//===============================================================================================
LONG CFreePcbView::OnChangeTextGrid( UINT wp, LONG lp )
{
	if( wp == WM_BY_INDEX )
		m_Doc->m_routing_grid_spacing = fabs( m_Doc->m_routing_grid[lp] );
	else
		ASSERT(0);
	SetFocus();
	m_Doc->ProjectModified( TRUE );
	return 0;
}
//===============================================================================================
LONG CFreePcbView::OnChangeSnapAngle( UINT wp, LONG lp )
{
	if( wp == WM_BY_INDEX )
	{
		if( lp == 0 )
		{
			m_Doc->m_snap_angle = 45;
			m_inflection_mode = IM_90_45;
		}
		else if( lp == 1 )
		{
			m_Doc->m_snap_angle = 90;
			m_inflection_mode = IM_90;
		}
		else
		{
			m_Doc->m_snap_angle = 0;
			m_inflection_mode = IM_NONE;
		}
	}
	else
		ASSERT(0);
	m_Doc->ProjectModified( TRUE );
	SetFocus();
	return 0;
}
//===============================================================================================
LONG CFreePcbView::OnChangeUnits( UINT wp, LONG lp )
{
	if( wp == WM_BY_INDEX )
	{
		if( lp == 0 )
			m_Doc->m_units = MIL;
		else
			m_Doc->m_units = MM;
	}
	else
		ASSERT(0);
	m_Doc->ProjectModified( TRUE );
	SetFocus();
	ShowSelectStatus();
	return 0;
}

//===============================================================================================
BOOL CFreePcbView::OnEraseBkgnd(CDC* pDC)
{
	// Erase the left and bottom panes, the PCB area is always redrawn
	m_left_pane_invalid = -1; // forced !
	return FALSE;
}
//===============================================================================================
void CFreePcbView::OnOPSideConvertToStraightLine()
{
	m_Doc->m_dlist->CancelHighLight();
	m_sel_op.SetSideStyle( m_sel_id.ii, CPolyLine::STRAIGHT );
	m_sel_op.HighlightSide( m_sel_id.ii );
	ShowSelectStatus();
	SetFKText( m_cursor_mode );
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = LAY_BACKGND;// OnOPSideConvertToStraightLine
}
//===============================================================================================
void CFreePcbView::OnOPSideConvertToArcCw()
{
	m_Doc->m_dlist->CancelHighLight();
	m_sel_op.SetSideStyle( m_sel_id.ii, CPolyLine::ARC_CW );
	m_sel_op.HighlightSide( m_sel_id.ii );
	ShowSelectStatus();
	SetFKText( m_cursor_mode );
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = LAY_BACKGND;// OnOPSideConvertToArcCw
}
//===============================================================================================
void CFreePcbView::OnOPSideConvertToArcCcw()
{
	m_Doc->m_dlist->CancelHighLight();
	m_sel_op.SetSideStyle( m_sel_id.ii, CPolyLine::ARC_CCW );
	m_sel_op.HighlightSide( m_sel_id.ii );
	ShowSelectStatus();
	SetFKText( m_cursor_mode );
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = LAY_BACKGND;// OnOPSideConvertToArcCcw
}
//===============================================================================================
// save undo info for a group
//
int CFreePcbView::SaveUndoInfoForGroup( CUndoList * list, BOOL bDELETE, BOOL wMerge )
{
	list->NewEvent();
	undo_group_descriptor * undo = new undo_group_descriptor;//ok
	undo->view = this;
	undo->list = list;
	undo->type = UNDO_GROUP;
	undo->str.SetSize( m_sel_count );
	undo->m_id.SetSize( m_sel_count );

	// Save Undo Info 
	int iu = 0;
	// for texts
	CArray<CText*> RefDes;
	RefDes.SetSize(0);
	int * Links = new int[m_Doc->m_outline_poly->GetSize()]; //new018
	for( int iL=m_Doc->m_outline_poly->GetSize()-1; iL>=0; iL-- )
		Links[iL] = 0;
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if(!tl)
			continue;
		//set size
		int ntexts = tl->GetNumTexts();
		if( iu+ntexts >= undo->str.GetSize() )
		{
			undo->str.SetSize( iu+ntexts+50 );
			undo->m_id.SetSize( iu+ntexts+50 );
		}
		int it = -1;
		for( CText* t=tl->GetNextAnyText(&it); t; t=tl->GetNextAnyText(&it) ) // save undo info
		{
			BOOL bSv = 0;
			if( t->m_selected )
				if( tl == m_Doc->Attr->m_Reflist && getbit( m_sel_mask, SEL_MASK_PART ) == 0 )
					bSv = 2;
				else
					bSv = 1;
			if( t->m_polyline_start >= 0 )
				if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetSel() )
					bSv = 2;
			if( bSv )
			{
				if( iu+1 >= undo->str.GetSize() )
				{
					undo->str.SetSize( iu+50 );
					undo->m_id.SetSize( iu+50 );
				}
				undo->m_id[iu].Set(ID_TEXT_DEF);
				::GetStringFromGuid( &t->m_guid, &undo->str[iu] );
				iu++;
				SaveUndoInfoForText( t, FALSE, list );
				// Save entire part 
				// if ref or pin polyline
				// is highlighted
				if( bSv == 2 )
				{
					int sv_part = -1;
					if( tl == m_Doc->Attr->m_Reflist )
						sv_part = t->m_polyline_start;
					if( tl == m_Doc->Attr->m_Pinlist )
						if( CText * part = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) )
							if( part->m_selected == 0 )
								if( part->m_polyline_start >= 0 )
									if( m_Doc->m_outline_poly->GetAt( part->m_polyline_start ).GetSel() == 0 )
										sv_part = part->m_polyline_start;
					if( sv_part >= 0 )
					{
						// check part
						// part already saved
						if( Links[sv_part] )
							continue;
						Links[sv_part] = 1;

						// Save entire part 
						CPolyLine * po;
						for( int get = sv_part; get >= 0; get = po->Next[index_part_attr] )
						{
							po = &m_Doc->m_outline_poly->GetAt(get);
							if( po->GetSel() == 0 )
							{
							 	if( iu+1 >= undo->str.GetSize() )
								{
									undo->str.SetSize( iu+50 );
									undo->m_id.SetSize( iu+50 );
								}
								undo->m_id[iu] = po->GetId();
								undo->m_id[iu].sst = ID_SIDE;
								undo->m_id[iu].ii = 0;
								undo->str[iu] = "";
								iu++;
								SaveUndoInfoForOutlinePoly( get, FALSE, list );
							}
						}
					}
				}
			}
		}
	}
	delete Links;//new018

	// save undo info for all polylines
	if( getbit( m_sel_flags, FLAG_SEL_OP ) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			CPolyLine * po = &m_Doc->m_outline_poly->GetAt(item);

			// ignore not sel
			if( po->GetSel() == 0 )
				continue;
			
			//set size
			if( iu+1+num_attributes >= undo->str.GetSize() )
			{
				undo->str.SetSize( iu+1+num_attributes );
				undo->m_id.SetSize( iu+1+num_attributes );
			}
			undo->m_id[iu] = po->GetId();
			undo->m_id[iu].sst = ID_SIDE;
			undo->m_id[iu].ii = 0;
			undo->str[iu] = "";
			iu++;
			SaveUndoInfoForOutlinePoly( item, FALSE, list );
			// removing option
			if( bDELETE )
			{
				int nsides=0;
				int gnc = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
				int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
				for( int i=gnc; i>=0; i-- )
				{
					int gs, gss;
					gs = m_Doc->m_outline_poly->GetAt(item).GetSel(i);
					if( i == gnc && cl == 0 )
						gss = 0;
					else
						gss = m_Doc->m_outline_poly->GetAt(item).GetSideSel(i);
					if( gss )
						nsides++;
				}
				if( nsides < gnc )
				{
					// will be modified
					DeleteWithRecalculationPoint( &m_Doc->m_outline_poly->GetAt(item), wMerge );
					m_Doc->m_outline_poly->GetAt(item).Draw();
				}
				else
				{
					// polyline will be deleted
					// check attributes
					int iAttr = -1;
					for( CText * t = m_Doc->ScanAttr( &m_Doc->m_outline_poly->GetAt(item), &iAttr ); t; t = m_Doc->ScanAttr( &m_Doc->m_outline_poly->GetAt(item), &iAttr ) )
					{
						if( t->m_selected == 0 )
						{
							undo->m_id[iu].Set(ID_TEXT_DEF);
							::GetStringFromGuid( &t->m_guid, &undo->str[iu] );
							iu++;
							SaveUndoInfoForText( t, FALSE, list );
						}
						else
						{
							int test = 0; // ok
						}
						BOOL fDel = TRUE;
						if( t->m_tl == m_Doc->m_tlist )
							fDel = 0;
						if( m_Doc->m_outline_poly->GetAt(item).Check( index_part_attr ) )
						{
							if( iAttr == index_part_attr || iAttr == index_value_attr || iAttr == index_foot_attr )
							{
								CText * part = m_Doc->m_outline_poly->GetAt(item).pAttr[index_part_attr];
								for( int get = part->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
								{
									if( m_Doc->m_outline_poly->GetAt(get).GetSel() == 0 )
										if( m_Doc->m_outline_poly->GetAt(get).pAttr[iAttr] == t )
										{
											fDel = 0;
											break;
										}
								}
							}
						}
						if( fDel )
							t->InVisible();
					}
					// if this is a part, it will need to be indexed
					if( m_Doc->m_outline_poly->GetAt(item).Check( index_part_attr ) )
					{
						BOOL bAdd = 1;
						for( int i=RefDes.GetSize()-1; i>=0; i-- )
						{
							if( RefDes[i] == m_Doc->m_outline_poly->GetAt(item).pAttr[index_part_attr] )
							{
								bAdd = 0; // Part already added to index list
								break;
							}
						}
						if( bAdd )
							RefDes.Add( m_Doc->m_outline_poly->GetAt(item).pAttr[index_part_attr] );
					}
					// delete polyline
					m_Doc->m_outline_poly->GetAt(item).Hide();
				}
			}
		}
	}
	undo->str.SetSize( iu );
	undo->m_id.SetSize( iu );
	// now save undo descriptor 
	list->Push( undo, &UndoGroupCallback, iu );
	//
	m_sel_count = m_Doc->Pages.GetSelCount( m_Doc->Pages.GetActiveNumber(), NULL );
	// indexing modified parts
	if( bDELETE )
	{
		for( int i=0; i<RefDes.GetSize(); i++ )
			if( RefDes[i]->isVISIBLE )
				m_Doc->AttributeIndexing( RefDes[i], index_part_attr );
		if( GetSelectedItem() == 0 )
			CancelSelection();
	}	
	prev_sel_count = m_sel_count;
	return IDOK;
}

//===============================================================================================
void CFreePcbView::SaveUndoInfoForMoveOrigin( int x_off, int y_off, CUndoList * list )
{
	// now push onto undo list
	undo_move_origin * undo = m_Doc->CreateMoveOriginUndoRecord( x_off, y_off );
	list->NewEvent();
	list->Push( (void*)undo, &m_Doc->MoveOriginUndoCallback );
	// save top-level descriptor
	void * ptr = CreateUndoDescriptor( list, UNDO_MOVE_ORIGIN, NULL, NULL, x_off, y_off, NULL, NULL );
	list->Push( ptr, &UndoCallback );
}
//===============================================================================================
void CFreePcbView::SaveUndoInfoForOutlinePoly( int index, BOOL new_event, CUndoList * list )
{
	// push undo info onto list
	if( new_event )
		list->NewEvent();		// flag new undo event

	CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(index);
	undo_outline_poly * undo = m_Doc->CreateOutlinePolyUndoRecord( poly, index );
	list->Push( (void*)undo, &m_Doc->OutlinePolyUndoCallback );

	// now push top-level callback for redoing
	if( new_event )
	{
		void * ptr = CreateUndoDescriptor( list, UNDO_OP, NULL, NULL, 0, 0, NULL, (void*)undo );
		list->Push( ptr, &UndoCallback );
	}
}
//===============================================================================================
void CFreePcbView::SaveUndoInfoForText( CText * text, BOOL new_event, CUndoList * list )
{
	if( text->m_tl == NULL )
		return;
	// create new undo record and push onto undo list
	undo_text * undo = text->m_tl->CreateUndoRecord( text );
	if( new_event )
		list->NewEvent();		// flag new undo event
	list->Push( (void*)undo, &text->m_tl->TextUndoCallback );
	if( new_event )
	{
		void * ptr = CreateUndoDescriptor( list, UNDO_TEXT, NULL, NULL, 0, 0, NULL, (void *)undo );
		list->Push( ptr, &UndoCallback );
	}
}


//===============================================================================================
RECT CFreePcbView::ViewAllElements( RECT * r )
{
	// reset window to enclose all elements
	BOOL bOK = FALSE;
	//
	int map = 0;
	RECT rct;
	if( r == NULL )
	{
		m_Doc->m_dlist->HighlightAll();
		rct = m_Doc->m_dlist->GetHighlightedBounds(&map);
		m_Doc->m_dlist->CancelHighLight();
	}
	else
		rct = *r;
	if( rct.right - rct.left <= 0 )
		rct = rect( -100000000, -100000000, 100000000, 100000000 );
	int min_x = rct.left;
	int min_y = rct.bottom;
	int max_x = rct.right;
	int max_y = rct.top;
	if( r == NULL )
	{
		min_x -= NM_PER_MIL*4;
		min_y -= NM_PER_MIL*4;
		max_x += NM_PER_MIL*4;
		max_y += NM_PER_MIL*4;
		rct = rect(min_x,min_y,max_x,max_y);
		double k = 1.1;
		double x_pcbu_per_pixel = k * (double)(max_x - min_x)/(m_client_r.right - m_left_pane_w);
		double y_pcbu_per_pixel = k * (double)(max_y - min_y)/(m_client_r.bottom - m_bottom_pane_h);
		m_pcbu_per_pixel = max( x_pcbu_per_pixel, y_pcbu_per_pixel );
	}
	else
	{
		rct = rect(min_x,min_y,max_x,max_y);
		double k1 = (double)(m_client_r.right-m_client_r.left)/(double)(m_client_r.top-m_client_r.bottom);
		double k2 = (double)(rct.right-rct.left)/(double)(rct.top-rct.bottom);
		double k = max(abs(k2/k1),abs(k1/k2));
		double x_pcbu_per_pixel = k * (double)(max_x - min_x)/(m_client_r.right);
		double y_pcbu_per_pixel = k * (double)(max_y - min_y)/(m_client_r.bottom);
		m_pcbu_per_pixel = min( x_pcbu_per_pixel, y_pcbu_per_pixel );
	}
	{
		m_org_x = (max_x + min_x)/2 - (m_client_r.right - m_client_r.left - m_left_pane_w)/2 * m_pcbu_per_pixel;
		m_org_y = (max_y + min_y)/2 - (m_client_r.bottom - m_client_r.top)/2 * m_pcbu_per_pixel;
	}
	if( max_x - min_x >= INT_MAX || 
		max_y - min_y >= INT_MAX ||
		min_x < -DEFAULT ||
		min_y < -DEFAULT ||
		max_x > DEFAULT ||
		max_y > DEFAULT )
	{
		min_x = -DEFAULT/20;
		min_y = -DEFAULT/20;
		max_x =  DEFAULT/20;
		max_y =  DEFAULT/20;
	}
	CRect screen_r;
	GetWindowRect( &screen_r );
	m_Doc->m_dlist->SetMapping( &m_client_r, &screen_r, m_left_pane_w, m_bottom_pane_h, m_pcbu_per_pixel,
		m_org_x, m_org_y );

	return rct;
	/// m_org_x = p.x - ((m_client_r.right-m_left_pane_w)*m_pcbu_per_pixel)/2;
}
void CFreePcbView::OnViewMerges()
{
	BOOL bF = 0;
	int nm = m_Doc->m_mlist->GetSize();
	for( int im=0; im<nm; im++ )
	{
		CancelSelection();
		CString merge = NewSelectM(im);
		if( m_sel_count == 0 )
			continue;
		RECT mr;
		mr.left = mr.bottom = INT_MAX;
		mr.right = mr.top = INT_MIN;
		int map = 0;
		HighlightGroup();
		mr = m_Doc->m_dlist->GetHighlightedBounds(&map);
		if( mr.right > mr.left )
		{
			id ID(ID_TEXT_DEF);
			CString str = m_Doc->m_mlist->GetStr(im);
			int font_size = (mr.right - mr.left)/(str.GetLength()+1);
			int stroke_width = font_size/6;
			CPoint mid( mr.left+font_size/2, (mr.top+mr.bottom)/2 - font_size/2 );			
			CText * ht = m_Doc->m_tlist->AddText( mid.x, mid.y, 0/*angle*/, 
												  LAY_INFO_TEXT, font_size, stroke_width, m_Doc->m_default_font, &str );
			dl_element * el = m_Doc->m_dlist->CopyToLayer( ht->dl_el, LAY_INFO_TEXT );
			el->el_static = EL_STATIC;
			m_Doc->m_tlist->RemoveText( ht );
			// Rectangle
			ID.Clear();
			dl_element * el_bounds = m_Doc->m_dlist->HighLight( DL_RECT, &mr, 1, NULL, 0, m_active_layer );
			el_bounds->transparent = TRANSPARENT_BACKGND;
			el_bounds->el_static = EL_STATIC;
			//
			bF = 1;
		}
	}
	if( bF )
	{
		SetCursorMode(CUR_GROUP_SELECTED);
		m_Doc->m_dlist->SetStatic(0);
	}
	Invalidate( FALSE );// (doubtful)//OnViewMerges
}
void CFreePcbView::OnViewAllElements()
{
	ViewAllElements(NULL);
	Invalidate( FALSE );// (doubtful)//OnViewAllElements
}
void CFreePcbView::OnViewSelTextsByLabels()
{
	POSITION pos;
	for( pos = m_Doc->m_drelist->list.GetHeadPosition(); pos != NULL; )
	{
		DRError * dre = (DRError*)m_Doc->m_drelist->list.GetNext( pos );
		if( dre->text1 )
		{
			id ID( ID_TEXT_DEF );
			if( dre->text1->isVISIBLE )
				NewSelect( dre->text1, &ID, 0, 0 );
		}
		else
		{
			NewSelect( NULL, &dre->id1, 0, 0 );
		}
	}
	if( m_sel_count )
	{
		SetCursorMode( CUR_GROUP_SELECTED );
		HighlightGroup();
	}
	Invalidate( FALSE );// (doubtful)
}


int CFreePcbView::FindStrInFile( CString * file, 
								 CString * str, 
								 CString * str2, 
								 int f_flags, 
								 int insertMode, 
								 int singleMode, 
								 int * curX, 
								 int * curY, 
								 int npinmin, 
								 int npinmax, 
								 int mergeIndex, 
								 CString * jPEG )
{
	static int maxX = 0;
	if( file->Right(3) != "cds" )
		return 0;
	// if scan
	BOOL IS_SCAN = FALSE;
	int strL = str->GetLength();
	if( strL == 0 && npinmax < 0 && mergeIndex == -1 )
		IS_SCAN = TRUE;
	// //
	if( *curX == 0 && *curY == 0 )
	{	// reset 
		maxX = 0;
	}
	CString multiPart = dSPACE;
	CStdioFile File;
	int OK = File.Open( *file, CFile::modeRead );
	if( !OK )     
	{
		CString mess = (G_LANGUAGE == 0 ? 
			("Unable to open library file\n\n \"" + *file + "\""):
			("Невозможно открыть файл библиотеки\n\n \"" + *file + "\""));
		AfxMessageBox( mess, MB_ICONERROR );
		m_Doc->m_dlg_log->OnShowMe();
		return 0;
	}
	int count = 0;
	int m_index = -1;
	CString instr, key_str, Footprints=dSPACE, curF="", curV="", curP="";
	CArray<CString> ss;
	int last_ih = 0;
	int last_if = -1;
	ULONGLONG position = 0;
	ULONGLONG bmp_src_position = 0;
	if( OK ) 
	{
		CString fname = *file;
		if( fname.GetLength() > 80 )
			fname = "..." + fname.Right(80);
		m_Doc->m_dlg_log->AddLine(".");
		m_Doc->m_dlg_log->UpdateWindow();
		//
		//
		CString gPart = "";
		int start_clip_op = m_Doc->clip_outline_poly.GetSize();
		int cur_np = 0;
		RECT cur_rect = rect(0,0,0,0);
		BOOL add = 0, v_ok = 0, f_ok = 0;
		int iterator = 0;
		while( File.ReadString( instr ) )
		{
			iterator++;
			instr.Trim();	
			if( instr.Left(7) == "n_pins:" )
			{
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 2 )
					cur_np = my_atoi( &ss[0] );
				continue;
			}
			else if( instr.Left(8) == "bmp_src:" )
			{
				position = File.GetPosition();
			}
			else if( instr.Left(10) == "rectangle:" )
			{
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 5 )
				{
					cur_rect.left =		my_atoi( &ss[0] );
					cur_rect.bottom =	my_atoi( &ss[1] );
					cur_rect.right =	my_atoi( &ss[2] );
					cur_rect.top =		my_atoi( &ss[3] );
				}
				continue;
			}
			else if( instr.Left(8) == "ref_des:" )
			{
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 2 )
					gPart = ss[0];
			}
			else if( instr.Left(15) == "footprint_name:" )
			{
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 2 )
					curF = ss[0];
			}
			else if( instr.Left(11) == "part_value:" )
			{
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 2 )
					curV = ss[0];
			}
			else if( instr.Left(13) == "add_new_page:" || instr.Left(12) == "rename_page:" )
			{
				position = File.GetPosition();
				bmp_src_position = File.GetPosition();
				int np = ParseKeyString( &instr, &key_str, &ss );
				if( np >= 2 )
					curP = ss[0];
			}
			else if( mergeIndex >= 0 )
			{
				gPart = "";
				add = 1;
			}
			if( IS_SCAN ) 
			{
				if( instr.Left(6) == "merge:" )
				{
					int np = ParseKeyString( &instr, &key_str, &ss );
					if( np >= 2 )
					{
						m_index++;
						int ret_v = m_Doc->m_merge_library.AddItem( &ss.GetAt(0), m_index );
						if( ret_v == 1 )
						{
							m_Doc->m_dlg_log->AddLine( "added merge index: "+ss.GetAt(0)+"\r\n" );
							if(iterator%32==0)
								m_Doc->m_dlg_log->UpdateWindow();
						}
					}
				}
				else if( instr.Left(8) == "part_end" )
				{
					CString Pref, Suff;
					ParseRef( &gPart, &Pref, &Suff );
					CString add_s = "[" + Pref + "] " + curV + "@" + curF;
					if( curV.GetLength() )
						if( curF.GetLength() )
						{
							int ret_v = m_Doc->m_library.AddItem( &add_s );
							if( ret_v == 1 )
							{
								m_Doc->m_dlg_log->AddLine( add_s+"\r\n" );
								if (iterator % 32 == 0)
									m_Doc->m_dlg_log->UpdateWindow();
							}
						}
				}
				else continue;
			}
			// part end
			// add string to list
			if( instr.Left(8) == "part_end" || instr.Left(1) == "[" || (gPart.GetLength() == 0 && add) )
			{
				if( add && mergeIndex == -1 ) 
				{
					if( insertMode )
					{
						if( gPart.GetLength() )
						{
							CString multiF = "";
							int dot = gPart.Find(".");
							if( dot <= 0 )
								dot = gPart.Find("-");
							if( dot > 0 )
							{
								multiF = gPart.Left(dot+1);
								if( multiPart.Find( multiF ) <= 0 )
									dot = 0;
							}
							// footprints are repeated, 
							// ignore
							if( dot <= 0 )
							{
								if( Footprints.Find(" "+curV+"@"+curF+dSPACE) > 0 )
									add = 0;// already was found
							}
							// singleMode && multiF
							if( singleMode && add )
							{
								if( multiPart.GetLength() > 2 )
								{
									if( multiF.GetLength() == 0 )	
										add = 0;
									else if( multiPart.Find( multiF ) != 2 ) // (dSPACE Length == 2)
										add = 0;
								}
							}
							// refDes are repeated, 
							// ignore
							if( add )
							{
								if( multiPart.Find( gPart ) > 0 )
									add = 0;
								else 
									multiPart += (gPart + dSPACE);// register a new part
							}
						}
						else
							add = 0;
					}
				}
				//
				if( add )
				{
					m_Doc->m_dlg_log->AddLine("\r\n\r\n"+fname+":\r\n");
					m_Doc->m_dlg_log->UpdateWindow();
					add = 0;
					CString label, str1="    FOUND: ref = \"", str2="\"  value = \"", str3="\"  footprint = \"", tab="\"";
					if( curV.GetLength() == 0 && curF.GetLength() )
					{
						str1 = "    FOUND: footprint ";
						str2 = "";
						//str3 = "";
						tab = "";
					}
					//
					label.Format( "%s%s%s%s%s%s%s\r\n", str1, gPart, str2, curV, str3, curF, tab );
					m_Doc->m_dlg_log->AddLine(label);
					m_Doc->m_dlg_log->UpdateWindow();
					count++;
					if( insertMode && (gPart.GetLength() || mergeIndex >= 0) )
					{
						//
						Footprints += (curV + "@" + curF + dSPACE);
						m_Doc->m_dlg_log->AddLine("    Reading Data...");
						m_Doc->m_dlg_log->UpdateWindow();
						//
						File.Seek( bmp_src_position, CFile::begin );
						RECT R;
						R.left = R.bottom = INT_MAX;
						R.top = R.right = INT_MIN;
						int last_index = m_Doc->clip_outline_poly.GetSize();
						CString jpeg = m_Doc->ReadGraphics( &File, &m_Doc->clip_outline_poly, (*curX)-cur_rect.left, (*curY)-cur_rect.bottom, &R, -1, position, mergeIndex, !getbit(f_flags,NO_SRC) );
						if(  jPEG )
							*jPEG = jpeg;
						if( insertMode && singleMode == 0 && last_index )
						{
							int sz_op = m_Doc->clip_outline_poly.GetSize();
							if( sz_op > 0 )
							{
								CText * Cref = m_Doc->clip_outline_poly.GetAt( sz_op-1 ).pAttr[index_part_attr];
								if( Cref )
								{
									if( FindSimilarPart( Cref, last_index ) )
									{
										R.bottom = INT_MAX;
										for( int iop=last_index; iop<sz_op; iop++ )
										{
											for( CText * att=m_Doc->ScanAttr( &m_Doc->clip_outline_poly.GetAt(iop) ); att; att=m_Doc->ScanAttr( &m_Doc->clip_outline_poly.GetAt(iop) ) )
												att->InVisible();
											int tmp_idx = m_Doc->clip_outline_poly.GetAt(iop).GetPicture();
											if( tmp_idx >= 0 )
												m_Doc->m_dlist->SetTemplateVis( 0, tmp_idx );
										}
										m_Doc->clip_outline_poly.SetSize( last_index );
										m_Doc->m_dlg_log->AddLine("\r\n    it is similar, ignored\r\n");
										m_Doc->m_dlg_log->UpdateWindow();
									}
								}
							}
						}
						if( R.bottom < INT_MAX )
						{
							*curY += (R.top - R.bottom) + NM_PER_MM*2;
							//
							maxX = max( maxX, (cur_rect.right - cur_rect.left) );
						}
						if( *curY > 200000000 )// Y-axis limit
						{
							*curX += (float)maxX*1.2;
							maxX = 0;
							*curY = 0;
						}
						if( *curX > 300000000 )// X-axis limit
							*curX = 0;
						//
						m_Doc->m_dlg_log->AddLine("    OK\r\n");
						m_Doc->m_dlg_log->UpdateWindow();
						if( mergeIndex >= 0 )
							break;
					}
				}
				//
				if( instr.Left(8) == "part_end" || instr.Left(1) == "[" )
				{
					position = File.GetPosition();
					gPart = "";
					curF = "";
					curV = "";
					v_ok = 0;
					f_ok = 0;
					continue;
				}
			}
			//test of
			//string
			if( add == 0 )
			{
				if( instr.Left(8) == "ref_des:" && (getbit( f_flags, F_REF ) || getbit( f_flags, F_PREFIX ) || npinmin >= 0) ||
					instr.Left(9) == "pin_name:" && getbit( f_flags, F_PIN ) ||
					instr.Left(9) == "net_name:" && getbit( f_flags, F_NET ) ||
					instr.Left(11) == "part_value:" && getbit( f_flags, F_VALUE ) ||
					instr.Left(15) == "footprint_name:" && getbit( f_flags, F_FOOT ) ||
					instr.Left(12) == "description:" && getbit( f_flags, F_DESC ) )
				{
					int np = ParseKeyString( &instr, &key_str, &ss );
					if( np >= 2 )
					{
						CString fs = ss[0];
						fs.Trim();
						if( getbit( f_flags, F_CASE ) == 0 )
							fs.MakeLower();
						//
						if( strL )
						{
							if( getbit( f_flags, F_BEGIN ) )
								if( str->Compare( fs.Left( strL ) ) == 0 )
									add = 1;
							if( !add )
								if( getbit( f_flags, F_END ) )
									if( str->Compare( fs.Right( strL ) ) == 0 )
										add = 1;
							if( !add )
								if( getbit( f_flags, F_WORD ) )
								{
									if( str2 )
									{
										if( instr.Left(3) == "par" )
											if( str->Compare( fs ) == 0 )
												v_ok = 1;
										if( instr.Left(3) == "foo" )
											if( str2->Compare( fs ) == 0 )
												f_ok = 1;
										if( v_ok && f_ok )
											add = 1;
									}
									else if( str->Compare( fs ) == 0 )
										add = 1;
								}
							if( !add )
								if( getbit( f_flags, F_FIND ) )
									if( fs.Find( *str ) >= 0 )
										add = 1;
							if( !add )
								if( getbit( f_flags, F_PREFIX ) && key_str == "ref_des" )
								{
									CString prefix, sf;
									ParseRef( &fs, &prefix, &sf );
									if( str->Compare( prefix ) == 0 )
										add = 1;
								}
						}
						if( (add && strL) || (!add && !strL) )
						{
							if( npinmin >= 0 )
							{
								if( cur_np >= npinmin && cur_np <= npinmax )
									add = 1;
								else
									add = 0;
							}
						}
					}
				}
			}
		}
		if( IS_SCAN == 0 && count == 0 )
		{
			///m_Doc->m_dlg_log->AddLine("    - nothing was found\r\n");
			m_Doc->m_dlg_log->UpdateWindow();
		}
		File.Close();
	}
	return count;
}
//===============================================================================================
void CFreePcbView::OnReplacePartPattern()
{
	if( m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects.":"Сначала выберите объекты.");
		return;
	}
	if( m_Doc->m_project_modified )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This tool has no undo, so save the project first":
			"Этот инструмент не имеет функции отмены, поэтому сначала сохраните проект.");
		return;
	}
	CDlgAddMerge dlg;
	dlg.Doc = m_Doc;
	dlg.m_mode = dlg.M_REPLACE_PATT;
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		m_Doc->SelectGroupAttributes();
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
		int it = -1;
		CText * part = m_Doc->Attr->m_Reflist->GetText( &dlg.m_merge_name, &it );
		if( part )
			m_Doc->ReplacePartPattern( part );
		else
			AfxMessageBox(G_LANGUAGE == 0 ? "This ref not found":"Это обозначение не найдено", MB_ICONERROR);
		CancelSelection(0);
		m_Doc->ProjectModified( TRUE );
		m_draw_layer = 0;//OnReplacePartPattern()
		Invalidate( FALSE );// (doubtful)
	}
}
//===============================================================================================
void CFreePcbView::OnUpdatePartPattern()
{

}

//===============================================================================================
// find a part in the layout, center window on it and select it
//
void CFreePcbView::OnViewFindAttr()
{
	FindAttr( 0, 0 );
}
void CFreePcbView::OnViewFindAmoungHid()
{
	id TxtId( ID_TEXT_DEF );
	int thPage = m_Doc->Pages.GetActiveNumber();
	for( int ipg=0; ipg<m_Doc->Pages.GetNumPages(); ipg++ )
	{
		m_Doc->SwitchToPage(ipg);
		CancelSelection();
		for( int iatt=0; iatt<num_attributes; iatt++ )
		{
			CTextList * tl = m_Doc->Pages.GetTextList( ipg, iatt );
			int it = -1;
			for( CText * hidTxt = tl->GetNextText(&it); hidTxt; hidTxt = tl->GetNextText(&it) )
				if( hidTxt->m_font_size < NM_PER_MIL )
					NewSelect( hidTxt, &TxtId, 0, 0 );
		}
	}
	m_Doc->SwitchToPage(thPage, TRUE);
	OnViewFindAmoungSel();
}
//===============================================================================================
void CFreePcbView::OnViewFindAmoungSel()
{
	FindAttr( 0, 1 );
}
//===============================================================================================
void CFreePcbView::OnPasteSinglePart()
{
	FindAttr( 1, 1 );
}
//===============================================================================================
void CFreePcbView::OnPastePartRow()
{
	FindAttr( 1, 0 );
}
//===============================================================================================
void CFreePcbView::FindAttr( int insertMode, int singleMode, CString * autoRun )
{
	static int f_flags = 1<<F_WORD|1<<F_REF|1<<F_VALUE|1<<F_FOOT|1<<F_PIN|1<<F_NET|1<<F_DESC|1<<F_DEF_SIZES;
	static CArray<CString> S;
	static int pin_range[2] = {-1,-1};
	//
	//
	CDlgFindPart dlgF;
	CDlgImportPart dlgI;
	int m_mask = ~0;
	BOOL mem_flag_F_OTHER = getbit( f_flags, F_OTHER );
	BOOL bFindAmoungSelectedTexts = 0;
	CString Cap = "Find attributes";
	if( insertMode )
	{
		if( m_protect )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "This PCB is protected.":"Эта печатная плата защищена.", MB_ICONERROR);
			return;
		}
		//
		setbit( f_flags, F_OTHER );
		clrbit( m_mask, F_OTHER );
		if( singleMode == 0 )
			Cap = "Import an array of parts";
		else
			Cap = "Import part by attribute";
		// clear OP
		m_Doc->clip_outline_poly.RemoveAll();
	}
	else if( singleMode )// "singleMode" - single insert Mode or Find Amoung Selected
	{
		clrbit( f_flags, F_OTHER );
		clrbit( m_mask, F_OTHER );
		bFindAmoungSelectedTexts = TRUE;
		if( m_sel_text )
			if( m_sel_text->m_font_size )
				Cap = "Find attributes amoung selected..";
			else
				Cap = "Find attributes amoung hidden..";
	}
	//
	int ThisPage = m_Doc->Pages.GetActiveNumber();
	//
	//
	//	
	// start search
	int cnt;
	do
	{
		// clear bufer
		m_Doc->clip_outline_poly.SetSize(0);
		cnt = 1;
		BOOL VALUE_DOG_PACK_IS_NOTEMPTY = TRUE;
		int ret = 0;
		int path_index = -1;
		if( insertMode )
		{
			// now read m_library
			if( m_Doc->m_library.IsEmpty() )
			{
				ret = IDOK;
				VALUE_DOG_PACK_IS_NOTEMPTY = 0;
				f_flags |= 1<<F_USE_LIST;
				m_Doc->m_library.ClearTestString();
			}
			else
			{
				if( S.GetSize() )
					dlgI.Initialize( &S, f_flags, m_mask, pin_range[0], pin_range[1], &Cap, &m_Doc->m_library, &m_Doc->m_app_dir );
				else
					dlgI.Initialize( NULL, f_flags, m_mask, pin_range[0], pin_range[1], &Cap, &m_Doc->m_library, &m_Doc->m_app_dir );
				dlgI.m_prefixes = m_Doc->Pages.m_get_prefixes;
				ret = dlgI.DoModal();
				path_index = dlgI.m_current_path;
				m_Doc->Pages.m_get_prefixes = dlgI.m_prefixes;
			}
		}
		else
		{
			if( m_sel_text && m_sel_count == 1 )
			{
				if( S.GetSize() == 0 )
					S.Add( m_sel_text->m_str );
				else if( S.GetAt( S.GetSize()-1 ).Compare( m_sel_text->m_str ) )
					S.Add( m_sel_text->m_str );
			}
			if( S.GetSize() )
				dlgF.Initialize( &S, f_flags, m_mask, pin_range[0], pin_range[1], &Cap );
			else
				dlgF.Initialize( NULL, f_flags, m_mask, pin_range[0], pin_range[1], &Cap );
			if( autoRun )
			{
				VALUE_DOG_PACK_IS_NOTEMPTY = TRUE;
				clrbit( f_flags, F_OTHER );
				setbit( f_flags, F_ALLPAGES );
				setbit( f_flags, F_CASE );
				dlgF.m_search_flags = f_flags;
				dlgF.m_str = *autoRun;
				ret = IDOK;
			}
			else
				ret = dlgF.DoModal();
		}
		if( ret == IDOK )
		{	
			cnt = 0; 
			// update flags
			int mem_flags = f_flags; // save old flags
			CString str="";
			if( VALUE_DOG_PACK_IS_NOTEMPTY == 0 )
			{
				pin_range[0] = -1;
				pin_range[1] = -1;
			}
			else if( insertMode )
			{
				if( dlgI.m_str.GetLength() )
				{
					if( S.GetSize() <= 0 )
						S.Add( dlgI.m_str );
					else if( S[S.GetSize()-1] != dlgI.m_str )
						S.Add( dlgI.m_str );
				}
				// 
				pin_range[0] = dlgI.m_pin_range[0];
				pin_range[1] = dlgI.m_pin_range[1];
				str = dlgI.m_str;
				f_flags = dlgI.m_search_flags;
				mem_flags = f_flags; // update flags
				if( getbit( f_flags, F_USE_LIST ) )
				{
					setbit( f_flags, F_WORD );
					setbit( f_flags, F_VALUE );
					setbit( f_flags, F_FOOT );
					clrbit( f_flags, F_BEGIN );
					clrbit( f_flags, F_END );
					clrbit( f_flags, F_FIND );
					setbit( f_flags, F_CASE );
					clrbit( f_flags, F_PREFIX );
					clrbit( f_flags, F_REF );
					clrbit( f_flags, F_PIN );
					clrbit( f_flags, F_NET );
					clrbit( f_flags, F_DESC );
					pin_range[0] = -1;
					pin_range[1] = -1;
				}
			}
			else
			{
				if( dlgF.m_str.GetLength() )
				{
					if( S.GetSize() <= 0 )
						S.Add( dlgF.m_str );
					else if( S[S.GetSize()-1] != dlgF.m_str )
						S.Add( dlgF.m_str );
				}
				// 
				pin_range[0] = dlgF.m_pin_range[0];
				pin_range[1] = dlgF.m_pin_range[1];
				str = dlgF.m_str;
				BOOL USE_DEF = getbit( f_flags, F_DEF_SIZES );
				f_flags = dlgF.m_search_flags;
				if( USE_DEF )
					setbit( f_flags, F_DEF_SIZES );
				mem_flags = f_flags; // update flags
			}
			// test for developers
			// to correct "test on checks in checkboxes" (see down)
			if( NUM_F_FLAGS != 16 )
				AfxMessageBox( "NUM_F_FLAGS != 16 (developer information)" );
			// test on checks in checkboxes
			if( (f_flags>>6<<6) == f_flags )
				str = "";
			else if( (f_flags>>5) == (f_flags>>12<<7) )
				str = "";
			//
			//
			//
			int strL = str.GetLength();
			//
			//  MakeLower
			if( getbit( f_flags, F_CASE ) == 0 )
				str.MakeLower();
			//
			BOOL ALL_PAGES = ( getbit( f_flags, F_ALLPAGES ) && m_Doc->Pages.GetNumPages() > 1 );
			//
			// preparing pages, 
			// clearing tags
			for( int i=0; i<m_Doc->Pages.GetNumPages(); i++ )
			{
				if( ALL_PAGES || i == ThisPage )
				{
					m_Doc->SwitchToPage(i);
					if( bFindAmoungSelectedTexts )
						m_Doc->SelectGroupAttributes();
					else
						CancelSelection( FALSE );
				}
			}
			//
			m_Doc->SwitchToPage(ThisPage);
			//
			{	// show m_dlg_log 
				m_Doc->m_dlg_log->Clear();
				m_Doc->m_dlg_log->OnShowMe();
				m_Doc->m_dlg_log->AddHeader( "scanning", 4 );  
			}
			//***********************************
			// using part list, extract value & package from string
			//***********************************
			int idog = str.Find("@");
			CString str_value="", str_footprint="";
			if( idog > 0 )
			{
				str_value = str.Left(idog);
				str_footprint = str.Right(str.GetLength()-idog-1);
			}
			//***********************************
			// inside this project
			//***********************************
			if( VALUE_DOG_PACK_IS_NOTEMPTY == 0 )
			{//EMPTY LIST
			}//EMPTY LIST
			else if( insertMode == 0 || (singleMode && path_index == -1) )	// "singleMode" - single insert Mode or Find Amoung Selected
														// (insertMode == 0 || singleMode) = all modes except 
														// for importing an array of parts
			{
				int sel_pages = 0; // Bitmask of pages on which something is found
				if( idog > 0 )
				{
					for( int ip=(ALL_PAGES?0:ThisPage); ip<m_Doc->Pages.GetNumPages(); ip++ )
					{
						CTextList * tl = m_Doc->Pages.GetTextList(ip,index_part_attr);
						CArray<CPolyLine> * polys = m_Doc->Pages.GetPolyArray(ip);
						m_Doc->SwitchToPage(ip,TRUE);
						int it = -1;
						for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
						{
							if( t->m_polyline_start >= 0 )
							{
								if( bFindAmoungSelectedTexts && t->m_selected == 0 )
									continue;
								BOOL value_ok = 0, footprint_ok = 0;
								if( CText * Vtext = polys->GetAt( t->m_polyline_start ).Check( index_value_attr ) )
								{
									if( str_value.CompareNoCase( Vtext->m_str ) == 0 )
										value_ok = 1;
								}
								if( CText * Ftext = polys->GetAt( t->m_polyline_start ).Check( index_foot_attr ) )
								{
									if( str_footprint.CompareNoCase( Ftext->m_str ) == 0 )
										footprint_ok = 1;
								}
								if( value_ok && footprint_ok )
								{
									cnt++;
									NewSelectP( t, NULL );
									if( cnt == 1 )
									{
										if( insertMode == 0 )
											ThisPage = m_Doc->Pages.GetActiveNumber();
										else
											GroupCopy(0);
									}
									CString pN;
									m_Doc->Pages.GetPageName(ip,&pN);
									CString label;
									label.Format( "Page: \"%s\", x=%dnm, y=%dnm\r\n", pN, t->m_x, t->m_y );
									id id1( ID_TEXT_DEF );
									id id2;
									DRError * dre = m_Doc->m_drelist->Add( &label, id1, id2, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );	
									m_Doc->m_dlg_log->AddLine(label);
									m_Doc->m_dlg_log->UpdateWindow();
								}
							}
						}
						if( ALL_PAGES == 0 )
							break;
					}
					m_Doc->SwitchToPage( ThisPage, TRUE );
					if( insertMode == 0 )
						JumpToPart( NULL );
				}
				else  
				{
					CString f_arr = dSPACE;
					if( bFindAmoungSelectedTexts )
					{
						if( ALL_PAGES )
							m_Doc->SwitchToPage(0);
						while(1)
						{
							for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
							{
								CTextList * tl = m_Doc->GetTextListbyLayer( lay );
								if( !tl )
									continue;
								tl->MarkAllTexts(0);
								int it = -1;
								for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
								{
									if( t->m_selected )
									{
										t->utility = 1;
										id ID( ID_TEXT_DEF );
										UnSelect( t, &ID, 0, 0 );
									}
								}
							}
							if( ALL_PAGES == 0 )
								break;
							if( m_Doc->Pages.GetActiveNumber() == m_Doc->Pages.GetNumPages()-1 )
								break;
							m_Doc->SwitchToPage( m_Doc->Pages.GetActiveNumber()+1 );
						}
					}
					//
					if( ALL_PAGES )
						m_Doc->SwitchToPage(0);
					BOOL bSinglePartFound = 0; // Break for single Mode
					while(1)
					{
						for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
						{
							CTextList * tl = m_Doc->GetTextListbyLayer( lay );
							if( !tl )
								continue;
							if( getbit( f_flags, F_REF ) == 0 && 
								getbit( f_flags, F_PREFIX ) == 0 && 
								pin_range[0] < 0 &&
								pin_range[1] < 0 &&
								tl == m_Doc->Attr->m_Reflist )
								continue;
							if( getbit( f_flags, F_VALUE ) == 0 && tl == m_Doc->Attr->m_Valuelist )
								continue;
							if( getbit( f_flags, F_FOOT ) == 0 && tl == m_Doc->Attr->m_Footlist )
								continue;
							if( getbit( f_flags, F_PIN ) == 0 && tl == m_Doc->Attr->m_Pinlist )
								continue;
							if( getbit( f_flags, F_NET ) == 0 && (tl == m_Doc->Attr->m_Netlist || tl == m_Doc->m_tlist) )
								continue;
							if( getbit( f_flags, F_DESC ) == 0 && tl == m_Doc->Attr->m_pDesclist )
								continue;

							int it = -1;
							for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
							{
								int cur_np = INT_MAX;
								if( t->m_polyline_start >= 0 )
								{		
									int i_att = -1;
									CPolyLine * p = &m_Doc->m_outline_poly->GetAt(t->m_polyline_start);
									if( pin_range[0] >= 0 )
										if( CText * ref = p->Check( index_part_attr ) )
											cur_np = m_Doc->GetNumAllPins(ref);
								}
								if( t->utility == 0 && bFindAmoungSelectedTexts )
									continue;
								//
								CString s = t->m_str;
								if( getbit( f_flags, F_CASE ) == 0 )
									s.MakeLower();
								BOOL add = 0;
								if( strL )
								{
									if( getbit( f_flags, F_BEGIN ) )
										if( str.Compare( s.Left( strL ) ) == 0 )
											add = 1;
									if(!add)
										if( getbit( f_flags, F_END ) )
											if( str.Compare( s.Right( strL ) ) == 0 )
												add = 1;
									if(!add)
										if( getbit( f_flags, F_WORD ) )
											if( str.Compare( s ) == 0 )
												add = 1;
									if(!add)
										if( getbit( f_flags, F_FIND ) )
											if( s.Find( str ) >= 0 )
												add = 1;
									if(!add)
										if( getbit( f_flags, F_PREFIX ) && tl == m_Doc->Attr->m_Reflist )
										{
											CString prefix, sf;
											ParseRef( &s, &prefix, &sf );
											if( str.Compare( prefix ) == 0 )
												add = 1;
										}
								}
								if( (add && strL) || (!add && !strL) )
								{
									if( pin_range[0] >= 0 )
									{
										if( cur_np >= pin_range[0] && cur_np <= pin_range[1] )
											add = 1;
										else
											add = 0;
									}
								}
								// when we insert an 
								// array of details, 
								// then ignore the repetition 
								// of footprints
								CString curF = "";
								if( add )
									if( t->m_polyline_start >= 0 )
										if( CText * foot = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_foot_attr] )
										{
											if( f_arr.Find( " "+foot->m_str+dSPACE ) > 0 )
												add = 0;
											curF = foot->m_str;
										}
								if( add )
								{
									id id1( ID_TEXT_DEF ); 
									id id2;
									int x = t->m_x;
									int y = t->m_y;
									if( insertMode == 0 )
									{
										id ID( ID_TEXT_DEF );
										NewSelect( t, &ID, 0, 0 );
									}
									//
									CString label, Xs, Ys;
									::MakeCStringFromDimension( m_user_scale, &Xs, x, m_Doc->m_units, 1, 1, 0, 1 );
									::MakeCStringFromDimension( m_user_scale, &Ys, y, m_Doc->m_units, 1, 1, 0, 1 );
									int nP = m_Doc->Pages.GetActiveNumber();
									CString pN;
									m_Doc->Pages.GetPageName( nP, &pN );
									if( tl == m_Doc->Attr->m_Reflist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_part_attr], t->m_str, Xs, Ys );
									else if( tl == m_Doc->Attr->m_Pinlist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_pin_attr], t->m_str, Xs, Ys );
									else if( tl == m_Doc->Attr->m_Netlist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_net_attr], t->m_str, Xs, Ys );
									else if( tl == m_Doc->Attr->m_Valuelist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_value_attr], t->m_str, Xs, Ys );
									else if( tl == m_Doc->Attr->m_Footlist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_foot_attr], t->m_str, Xs, Ys );
									else if( tl == m_Doc->Attr->m_pDesclist )
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_desc_attr], t->m_str, Xs, Ys );
									else
										label.Format( "Page: \"%s\", %s, \"%s\", x=%s, y=%s\r\n", pN, attribute_str[index_net_attr], t->m_str, Xs, Ys );
									DRError * dre = m_Doc->m_drelist->Add( &label, id1, id2, x, y, x, y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );	
									m_Doc->m_dlg_log->AddLine(label);
									m_Doc->m_dlg_log->UpdateWindow();
									cnt++;
									if( insertMode )
									{
										f_arr += (dSPACE + curF + dSPACE);
										if( t->m_polyline_start >= 0 )
											if( CText * ch = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) )
											{
												NewSelectP( ch, NULL );
												GroupCopy(0);
												setbit( sel_pages, m_Doc->Pages.GetActiveNumber() ); 
												if( singleMode )
												{
													bSinglePartFound = TRUE;
													break;
												}
											}
									}
								}
							}
							if( bSinglePartFound )
								break;
						}
						if( bSinglePartFound )
							break;
						if( ALL_PAGES == 0 )
							break;
						if( m_Doc->Pages.GetActiveNumber() == m_Doc->Pages.GetNumPages()-1 )
							break;
						if( ALL_PAGES && insertMode == 0 )
							if( m_sel_count && m_sel_text )
							{
								HighlightGroup();
								JumpToPart( NULL );
							}
						m_Doc->SwitchToPage( m_Doc->Pages.GetActiveNumber()+1 );
					}
				}
				//
				if( cnt == 0 )
				{
					// restore selected texts
					if( bFindAmoungSelectedTexts )
					{
						if( ALL_PAGES )
							m_Doc->SwitchToPage(0);
						while(1)
						{
							for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
							{
								CTextList * tl = m_Doc->GetTextListbyLayer( lay );
								if( !tl )
									continue;
								int it = -1;
								for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
								{
									if( t->utility )
									{
										t->utility = 0;
										id ID( ID_TEXT_DEF );
										NewSelect( t, &ID, 0, 0 );
									}
								}
							}
							if( ALL_PAGES == 0 )
								break;
							if( m_Doc->Pages.GetActiveNumber() == m_Doc->Pages.GetNumPages()-1 )
								break;
							m_Doc->SwitchToPage( m_Doc->Pages.GetActiveNumber()+1 );
						}
					}
					if( ThisPage != m_Doc->Pages.GetActiveNumber() )
						m_Doc->SwitchToPage(ThisPage, TRUE);
					if( ALL_PAGES )
						AfxMessageBox(G_LANGUAGE == 0 ? "Nothing found in current project":"Ничего не найдено в текущем проекте");
					else
						AfxMessageBox(G_LANGUAGE == 0 ? "Nothing found in current page":"На текущей странице ничего не найдено");
				}
				else
				{
					m_Doc->m_dlg_log->AddHeader( "DONE", 4 );
				    if( ThisPage != m_Doc->Pages.GetActiveNumber() )
						m_Doc->SwitchToPage(ThisPage,TRUE);
					CString s;
					s.Format("The number of found objects: %d", cnt );
					m_Doc->m_dlg_log->AddLine( s+"\r\n" );
					if( insertMode )
					{
						if( m_Doc->clip_outline_poly.GetSize() )
						{
							m_paste_flags = PART_ONLY;
							OnGroupPaste( 0, 0 );
						}
						else
						{
							int PRES = 0;
							CString str = (G_LANGUAGE == 0 ? 
								"Nothing was found on this page, but found on the following pages: ":
								"На этой странице ничего не найдено, но найдено на следующих страницах: ");
							for( int pg=0; pg<m_Doc->Pages.GetNumPages(); pg++ )
								if( getbit( sel_pages, pg ) )
								{
									if( PRES )
										str += ", ";
									CString num_p;
									m_Doc->Pages.GetPageName( pg, &num_p );
									str += num_p;			
								}
							AfxMessageBox(str);
							CancelSelection();
						}
						CPoint point;
						RECT r;
						GetWindowRect( &r );
						point.x = (r.left + r.right)/2;
						point.y = (r.top + r.bottom)/2;
						SetCursorPos( point.x, point.y );
					}
					else 
					{
						m_Doc->m_dlg_log->AddLine( "Use key 'D' to enlarge marks\r\n" );
						m_sel_count = m_Doc->Pages.GetSelCount( m_Doc->Pages.GetActiveNumber(), &m_sel_flags );
						m_Doc->m_dlist->CancelHighLight();
						if( m_sel_count && m_sel_text )
						{
							SetCursorMode( CUR_GROUP_SELECTED );
							HighlightGroup();
							JumpToPart( NULL );
							m_Doc->m_dlg_log->ShowWindow( SW_HIDE );			
						}
					}
				}
			}
			//***********************************
			// outside this project
			//***********************************
			if( getbit( f_flags, F_OTHER ) && (cnt == 0 || (insertMode == 0 && singleMode == 0) ) )
			{	
				// show m_dlg_log 
				m_Doc->m_dlg_log->OnShowMe();
				CFileFind finder[10];
				CString search_str[10];
				search_str[0] = m_Doc->m_parent_folder;
				search_str[0] += "\\*";
				int ret = IDOK;
				int pos = 0;
				if( ret == IDOK )
				{		
					//
					int cX=0, cY=0;
					enum{ MAX_POS = 10 };
					BOOL bWorking[MAX_POS];
					bWorking[pos] = finder[pos].FindFile( search_str[pos] );
					//
					CString path;
					if( insertMode && VALUE_DOG_PACK_IS_NOTEMPTY && idog > 0 )
					{
						if( singleMode )
						{
							CString SpeedFile = m_Doc->m_library.GetPath( &str, path_index );
							int ok = FindStrInFile( &SpeedFile, (&str_value), (&str_footprint), f_flags, insertMode, singleMode, &cX, &cY, pin_range[0], pin_range[1] );
							cnt += ok;
							if( ok && dlgI.m_str2.GetLength() )
							{
								int gs = m_Doc->clip_outline_poly.GetSize();
								if( gs > 0 )
									if( CText * mFoot = m_Doc->clip_outline_poly.GetAt( gs-1 ).Check( index_foot_attr ) )
									{
										mFoot->Undraw();
										mFoot->m_str = dlgI.m_str2;
										mFoot->m_nchars = dlgI.m_str2.GetLength();
									}
							}
						}
						else 
						{
							CArray<CString> * Pathes = m_Doc->m_library.GetPathes( &str );
							for( int ip=0; ip<Pathes->GetSize(); ip++ )
							{
								CString SpeedFile = Pathes->GetAt(ip);
								//path.Format("Scanning folder: %s", SpeedFile );
								//m_Doc->m_dlg_log->AddLine( path+"\r\n" );
								//m_Doc->m_dlg_log->UpdateWindow();
								cnt += FindStrInFile( &SpeedFile, (&str_value), (&str_footprint), f_flags, insertMode, singleMode, &cX, &cY, pin_range[0], pin_range[1] );
							}
						}
					}
					else
					{
						//path.Format("Scanning folder: %s", search_str[pos] );
						//m_Doc->m_dlg_log->AddLine( path+"\r\n" );
						//m_Doc->m_dlg_log->UpdateWindow();
						while ( bWorking[pos] )
						{
							if( cnt && singleMode )
								break;
							bWorking[pos] = finder[pos].FindNextFile();
							if( !finder[pos].IsDots() )
							{
								CString fn = finder[pos].GetFilePath();
								if( !finder[pos].IsDirectory() )
								{
									if( fn.Right(3) == "cds" )
										if( fn != m_Doc->m_pcb_full_path || VALUE_DOG_PACK_IS_NOTEMPTY == 0 ) 
										{
											if( VALUE_DOG_PACK_IS_NOTEMPTY == 0 )
											{
												m_Doc->m_merge_library.SetPath( &fn );
												m_Doc->m_library.SetPath( &fn );
											}
											cnt += FindStrInFile( &fn, (idog>0?&str_value:&str), (idog>0?&str_footprint:NULL), f_flags, insertMode, singleMode, &cX, &cY, pin_range[0], pin_range[1] );
										}
								}
								else if( finder[pos].GetFileName().Right(13) != "related_files" )
								{
									pos++;
									if( pos >= MAX_POS )
										break;
									search_str[pos] = fn + "\\*";
									bWorking[pos] = finder[pos].FindFile( search_str[pos] );
									//
									//CString path;
									//path.Format("Scanning folder: %s", search_str[pos] );
									//m_Doc->m_dlg_log->AddLine( path+"\r\n" );
									//m_Doc->m_dlg_log->UpdateWindow();
								}
							}
							while( !bWorking[pos] )
							{
								pos--;
								if( pos < 0 )
									break;
								if( bWorking[pos] )
								{
									//CString path;
									//path.Format("Scanning folder: %s", search_str[pos] );
									//m_Doc->m_dlg_log->AddLine( path+"\r\n" );
									//m_Doc->m_dlg_log->UpdateWindow();
								}
							}
							if( pos < 0 )
								break;
						}
					}
				}
				//
				if( VALUE_DOG_PACK_IS_NOTEMPTY == 0 )
				{
					m_Doc->m_dlg_log->AddLine( "Reading completed\r\n" );
				}
				else if( cnt == 0 )
				{
					AfxMessageBox(G_LANGUAGE == 0 ? 
						"Nothing found in other projects":
						"Ничего не найдено в других проектах");
				}
				else
				{
					m_Doc->m_dlg_log->AddHeader( "DONE", 4 );
				    CString s;
					s.Format("The number of found objects: %d", cnt );
					m_Doc->m_dlg_log->AddLine( s+"\r\n" );
					if( insertMode )
					{
						m_paste_flags = ~0; // paste from file
						OnGroupPaste( 0, 0, getbit(f_flags, F_DEF_SIZES) );
						CPoint point;
						RECT r;
						GetWindowRect( &r );
						point.x = (r.left + r.right)/2;
						point.y = (r.top + r.bottom)/2;
						SetCursorPos( point.x, point.y );
					}
				}
			}
			if( VALUE_DOG_PACK_IS_NOTEMPTY == 0 )
			{
				CString adds = "Not founds";
				if( m_Doc->m_library.IsEmpty() )
				{
					m_Doc->m_library.SetPath( &adds );
					m_Doc->m_library.AddItem( &adds );
				}
				else 
				{
					setbit( mem_flags, F_USE_LIST );
					//CStdioFile wf;
					//CString fLib = m_Doc->m_app_dir + "\\ref.lib";
					//int OK = wf.Open( fLib, CFile::modeCreate | CFile::modeWrite );
					//if( OK )
					//{
					//	CArray<CString> * strArr = m_Doc->m_library.GetLibrary();
					//	for( int iarr=0; iarr<strArr->GetSize(); iarr++ )
					//	{
					//		wf.WriteString( strArr->GetAt(iarr) );
					//		wf.WriteString( "\n" );
					//	}
					//	wf.Close();
					//}
				}
				m_Doc->m_library.ShowTestString();
			}
			f_flags = mem_flags; // restore flags
		}
	}while( cnt == 0 && autoRun == NULL );
	if( getbit( m_mask, F_OTHER ) == 0 )
	{
		if( mem_flag_F_OTHER == 0 )
			clrbit( f_flags, F_OTHER );
		else
			setbit( f_flags, F_OTHER );
	}
	Invalidate( FALSE );// (doubtful)//OnViewFindpart
}
//===============================================================================================
void CFreePcbView::OnDrc()
{
	m_Doc->OnToolsDrc();
}
//===============================================================================================
void CFreePcbView::OnClearDRC()
{
	m_Doc->OnToolsClearDrc();
}
//===============================================================================================
void CFreePcbView::AddOutlinePoly( BOOL bREPEAT_OR_ADD_CUTOUT )
{
	if( bREPEAT_OR_ADD_CUTOUT )
		if( m_polyline_layer <= 0 )
			m_polyline_layer = LAY_FREE_LINE;
	if( m_polyline_hatch >= 100 || m_polyline_hatch < 0 )
		m_polyline_hatch = 0;

	BOOL NEW=TRUE;
	if( m_sel_id.type == ID_POLYLINE )
	{
		if( m_sel_id.i < m_Doc->m_outline_poly->GetSize() )
		{
			if( m_sel_op.GetClosed() || bREPEAT_OR_ADD_CUTOUT == 0 ) 
			{
				if( m_sel_op.GetNumCorners() >= 3 || bREPEAT_OR_ADD_CUTOUT == 0 )
				{
					if( m_sel_id.ii < m_sel_op.GetNumCorners() )
					{
						m_polyline_layer = m_sel_op.GetLayer();
						m_polyline_hatch = m_sel_op.GetHatch();				
						if( bREPEAT_OR_ADD_CUTOUT )
						{
							// OnAddCutout
							m_sel_id.sst = ID_CORNER;
							m_sel_id.ii = m_sel_op.GetNumCorners();
						}
						NEW = FALSE;
					}
				}
			}
		}
		else
			return;
	}
	//
	CDlgAddMaskCutout dlg;
	dlg.Initialize( m_polyline_layer==0?LAY_FREE_LINE:m_polyline_layer, 
					m_polyline_hatch, m_Doc->m_num_layers-LAY_ADD_1, 
					m_Doc->m_units, (abs((double)m_polyline_width)+1.0)*m_user_scale, NEW?m_polyline_closed:2 );
	int ret;
	if( bREPEAT_OR_ADD_CUTOUT )	// if the REPEAT button 
								// was pressed, or if this is 
								// the beginning of adding a cutout
	{
		ret = IDOK;
		dlg.m_layer = m_polyline_layer;
		dlg.m_hatch = m_polyline_hatch;
		dlg.m_width = (double)(m_polyline_width+1.0)*m_user_scale;
		dlg.m_closed_flag = m_polyline_closed;
		dlg.m_type_off_flag = 1;
		dlg.m_hatch_off_flag = 1;
		dlg.m_layer_off_flag = 1;
		dlg.m_range_flag = 0;//  !
	}
	else
		ret = dlg.DoModal(); 
	if( ret == IDOK )
	{
		// new outline
		if( NEW )
		{
			m_polyline_layer = dlg.m_layer;
			m_polyline_hatch = dlg.m_hatch;
			m_polyline_width = (double)dlg.m_width/m_user_scale;
			m_polyline_closed = dlg.m_closed_flag;
			dlg.m_type_off_flag = 1;
			dlg.m_hatch_off_flag = 1;
			dlg.m_layer_off_flag = 1;
			dlg.m_range_flag = 0;//  !
			//
			int sz = m_Doc->m_outline_poly->GetSize();
			m_Doc->m_outline_poly->SetSize(sz+1);
			m_Doc->m_outline_poly->GetAt(sz).SetDisplayList( m_Doc->m_dlist );
			m_Doc->m_outline_poly->GetAt(sz).Hide();
			SaveUndoInfoForOutlinePoly( sz, TRUE, m_Doc->m_undo_list );
			m_Doc->m_outline_poly->GetAt(sz).m_visible = 1;
			m_sel_id.Set( ID_POLYLINE, ID_GRAPHIC, sz, ID_CORNER, 0 );
		}
		else
		{
			SaveUndoInfoForGroup( m_Doc->m_undo_list );// AddOutlinePoly()	
			BOOL bSINGLE_OP = (m_sel_count == m_sel_op.GetSelCount());
			// SET LAYER
			if( dlg.m_layer_off_flag )
				m_polyline_layer = m_sel_op.GetLayer();
			else if( dlg.m_layer == 0 )
			{
				m_polyline_layer = m_sel_op.GetLayer();
				if( m_sel_op.Check( index_pin_attr ) )
					m_polyline_layer = LAY_PIN_LINE;
				else if( m_sel_op.Check( index_net_attr ) )
					m_polyline_layer = LAY_CONNECTION; 
				else if( m_sel_op.Check( index_part_attr ) )
					m_polyline_layer = LAY_PART_OUTLINE;
				else //if( m_polyline_layer < LAY_ADD_1 )
					m_polyline_layer = LAY_FREE_LINE;
			}
			else
				m_polyline_layer = dlg.m_layer;
			// SET HATCH PATTERN
			m_polyline_style = CPolyLine::STRAIGHT;
			if( dlg.m_hatch_off_flag == 0 )
				m_polyline_hatch = dlg.m_hatch;
			else
				m_polyline_hatch = m_sel_op.GetHatch();
			// SET CLOSED
			if( dlg.m_type_off_flag == 0 )
				m_polyline_closed = dlg.m_closed_flag;
			else
				m_polyline_closed = m_sel_op.GetClosed();
			// SET WIDTH
			if( dlg.m_range_flag == 0 )
				m_polyline_width = (double)dlg.m_width/m_user_scale;
			else if( InRange( (double)m_sel_op.GetW()*m_user_scale, dlg.m_width_min, dlg.m_width_max ) )
				m_polyline_width = (double)dlg.m_width/m_user_scale;
			else
				m_polyline_width = m_sel_op.GetW();
			//
			//
			BOOL cl = m_sel_op.GetClosed();
			if( bSINGLE_OP )
			{
				// SET CLOSED
				if( cl != m_polyline_closed )
				{
					int ncor = m_sel_op.GetNumCorners();
					if( ncor )
					{
						if( cl )
						{
							m_sel_op.UnClose();
							if( m_sel_id.ii >= ncor-1 )
							{
								m_sel_id.ii--;
								m_Doc->m_dlist->CancelHighLight();
								m_sel_op.SetSideSel(m_sel_id.ii,1);
								m_sel_op.HighlightSide( m_sel_id.ii );
							}
						}
						else if( ncor > 2 && m_sel_op.Check(index_pin_attr) == NULL )
							m_sel_op.Close( m_sel_op.GetSideStyle(ncor), 0 );
					}
				}
				// SET WIDTH
				m_sel_op.SetW( m_polyline_width );
				// SET LAYER
				CText * desc = m_sel_op.Check( index_desc_attr );
				if( m_polyline_layer != m_sel_op.GetLayer() )
				{
					if( CText * net = m_sel_op.Check( index_net_attr ) )
						net->InVisible();
					m_sel_op.pAttr[index_net_attr] = NULL;
					m_sel_op.SetLayer( m_polyline_layer );
					
					if( desc )
					{
						if( AfxMessageBox(G_LANGUAGE == 0 ? 
							"Want to move the description text onto the polyline layer?":
							"Хотите переместить текст описания на слой полилинии?", MB_ICONQUESTION | MB_YESNO) == IDYES)
						{
							if( m_polyline_layer >= LAY_ADD_1 )
								desc->m_layer = m_polyline_layer;
							else
								desc->m_layer = LAY_PIN_DESC;
						}
					}
				}
				// CORRECT PICTURE
				{
					int itmp = m_sel_op.GetPicture();
					if( itmp >= 0 )
					{
						RECT pr = m_sel_op.GetCornerBounds();
						m_Doc->m_dlist->MoveTemplate( &pr, itmp, m_polyline_layer );
					}
				}

				// correct NODES
				if (m_sel_op.Node[0] > 1)
					m_sel_op.Node[0] = GetStandartNode(m_Doc, m_sel_id.i);
				if (m_sel_op.Node[m_sel_op.GetNumCorners() - 1] > 1)
					m_sel_op.Node[m_sel_op.GetNumCorners() - 1] = GetStandartNode(m_Doc, m_sel_id.i);

				// SET HATCH PATTERN
				m_sel_op.SetHatch( m_polyline_hatch ); // inc draw()

				// description text layer
				if( desc )
				{
					if( desc->m_str.Right(3) == "BOM" )
						DrawBOM( desc );
					else
						desc->MakeVisible();
				}
			}
			else for( int ip=0; ip<m_Doc->m_outline_poly->GetSize(); ip++ )
			{
				if( m_Doc->m_outline_poly->GetAt(ip).m_visible == 0 )
					continue;
				if( m_Doc->m_outline_poly->GetAt(ip).GetSel() == 0 )
					continue;
				m_sel_id.i = ip;
				m_sel_id.ii = 0;

				// SET WIDTH
				if( dlg.m_range_flag == 0 )
					m_sel_op.SetW( m_polyline_width );
				else if( InRange( (double)m_sel_op.GetW()*m_user_scale, dlg.m_width_min, dlg.m_width_max ) )
				{
					m_polyline_width = (double)dlg.m_width/m_user_scale;
					m_sel_op.SetW( m_polyline_width );
				}
				// SET LAYER
				if( dlg.m_layer_off_flag )
				{// EMPTY
				}// EMPTY
				else if( dlg.m_layer == 0 )
				{
					// Get polyline status
					m_polyline_layer = m_sel_op.GetLayer();
					if( m_sel_op.Check( index_pin_attr ) )
						m_polyline_layer = LAY_PIN_LINE;
					else if( m_sel_op.Check( index_net_attr ) )
						m_polyline_layer = LAY_CONNECTION; 
					else if( m_sel_op.Check( index_part_attr ) )
						m_polyline_layer = LAY_PART_OUTLINE;
					else if( m_polyline_layer < LAY_ADD_1 )
						m_polyline_layer = LAY_FREE_LINE;
					m_sel_op.SetLayer( m_polyline_layer );
				}
				else
				{
					m_polyline_layer = dlg.m_layer;
					if( m_polyline_layer != m_sel_op.GetLayer() )
					{
						if( CText * net = m_sel_op.Check( index_net_attr ) )
							net->InVisible();
						m_sel_op.pAttr[index_net_attr] = NULL;
						m_sel_op.SetLayer( m_polyline_layer );
					}
				}
				// CORRECT PICTURE
				if( dlg.m_layer_off_flag == 0 )
				{
					int itmp = m_sel_op.GetPicture();
					if( itmp >= 0 )
					{
						RECT pr = m_sel_op.GetCornerBounds();
						m_Doc->m_dlist->MoveTemplate( &pr, itmp, m_polyline_layer );
					}
				}
				// SET CLOSED
				if( dlg.m_type_off_flag == 0 )
				{
					m_polyline_closed = dlg.m_closed_flag;
					BOOL cl = m_sel_op.GetClosed();
					if( cl != m_polyline_closed )
					{
						int ncor = m_sel_op.GetNumCorners();
						if( ncor )
						{
							if( cl )
							{
								m_sel_op.UnClose();
								if( m_sel_id.ii >= ncor-1 )
								{
									m_sel_id.ii--;
									m_Doc->m_dlist->CancelHighLight();
									m_sel_op.SetSideSel(m_sel_id.ii,1);
									m_sel_op.HighlightSide( m_sel_id.ii );
								}
							}
							else if( ncor > 2 && m_sel_op.Check(index_pin_attr) == NULL )
								m_sel_op.Close( m_sel_op.GetSideStyle(ncor), 0 );
						}
					}
				}

				// correct NODES
				if (m_sel_op.Node[0] > 1)
					m_sel_op.Node[0] = GetStandartNode(m_Doc, m_sel_id.i);
				if (m_sel_op.Node[m_sel_op.GetNumCorners() - 1] > 1)
					m_sel_op.Node[m_sel_op.GetNumCorners() - 1] = GetStandartNode(m_Doc, m_sel_id.i);

				// SET HATCH PATTERN
				if( dlg.m_hatch_off_flag == 0 )
					m_sel_op.SetHatch( m_polyline_hatch );
				m_sel_op.Draw();
			}
			
		}

		// force layer visible
		m_Doc->m_vis[m_polyline_layer] = TRUE;
		m_Doc->m_dlist->SetLayerVisible( m_polyline_layer, TRUE );

		// Start dragging Group
		if( NEW || bREPEAT_OR_ADD_CUTOUT )
		{
			// start dragging rectangle
			CDC *pDC = GetDC();
			pDC->SelectClipRgn( &m_pcb_rgn );
			SetDCToWorldCoords( pDC );
			m_Doc->m_dlist->CancelHighLight();
			SetCursorMode( CUR_ADD_OP );
			m_Doc->m_dlist->StartDraggingArray( pDC, m_last_cursor_point.x,
				m_last_cursor_point.y, 2 );
			ReleaseDC( pDC );
		}
		m_Doc->ProjectModified( TRUE );
		OnRangeCmds(NULL);
	}
}
void CFreePcbView::OnAddOutlinePoly()
{
	AddOutlinePoly(0);
	OnRangeCmds( NULL );//OnAddOutlinePoly
}

void CFreePcbView::OnAddCutout()
{
	SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
	if( m_sel_op.GetClosed() && m_sel_op.GetNumCorners() > 2 )
		AddOutlinePoly(TRUE);
	else
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The number of polyline vertices must be more than 2, and the polyline should be closed!":
			"Количество вершин полилинии должно быть больше 2, и полилиния должна быть замкнутой!", MB_ICONERROR);
	Invalidate( FALSE );// (doubtful)
}

void CFreePcbView::OnSetPrintArea()
{
	id iD;
	int num_pg = m_Doc->Pages.FindPrintableArea( &iD ).ii + 1;
	CString name;
	m_Doc->Pages.GetPageName( m_Doc->Pages.GetActiveNumber(), &name ); 
	ExtractComponentName( &name, NULL );
	CString cmd;
	cmd.Format( "Page%d'|name: %s.%d.pdf'%s PrintableArea", num_pg, name, num_pg, COMMAND );
	num_pg++;
	CText * newTxt = m_Doc->Attr->m_pDesclist->AddText(	m_sel_op.GetX(0),
														m_sel_op.GetY(0),
														0,
														LAY_PIN_DESC,
														0,0,
														m_Doc->m_default_font,
														&cmd );
				
	m_sel_op.pAttr[index_desc_attr] = newTxt;
	newTxt->m_polyline_start = m_sel_id.i;
	newTxt->isVISIBLE = 0; 
	SaveUndoInfoForOutlinePoly( m_sel_id.i, TRUE, m_Doc->m_undo_list );
	SaveUndoInfoForText( newTxt, 1, m_Doc->m_undo_list );
	newTxt->MakeVisible();
	m_Doc->ProjectModified(TRUE);
	OnOPSetAttributes();
}

//===============================================================================================
void CFreePcbView::OnToolsMoveOrigin()
{
	CDlgMoveOrigin dlg;
	dlg.Initialize( m_Doc->m_units );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		if( dlg.m_drag )
		{
			CDC *pDC = GetDC();
			pDC->SelectClipRgn( &m_pcb_rgn );
			SetDCToWorldCoords( pDC );
			m_Doc->m_dlist->CancelHighLight();
			SetCursorMode( CUR_MOVE_ORIGIN );
			m_Doc->m_dlist->StartDraggingArray( pDC, m_last_cursor_point.x,
				m_last_cursor_point.y, 2 );
			ReleaseDC( pDC );
			Invalidate( FALSE );// (doubtful)
		}
		else
		{
			SaveUndoInfoForMoveOrigin( -dlg.m_x, -dlg.m_x, m_Doc->m_undo_list );
			MoveOrigin( -dlg.m_x, -dlg.m_y );
			OnViewAllElements();
		}
	}
}
//===============================================================================================
// move origin of coord system by moving everything
// by (x_off, y_off)
//
void CFreePcbView::MoveOrigin( int x_off, int y_off )
{
	for( int ib=0; ib<m_Doc->m_outline_poly->GetSize(); ib++ )
		m_Doc->m_outline_poly->GetAt(ib).MoveOrigin( x_off, y_off );
	m_Doc->m_tlist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_Reflist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_Pinlist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_pDesclist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_Netlist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_Footlist->MoveOrigin( x_off, y_off );
	m_Doc->Attr->m_Valuelist->MoveOrigin( x_off, y_off );
	m_Doc->m_drelist->MoveOrigin( x_off, y_off );
}
//===============================================================================================
void CFreePcbView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// save starting position in pixels
	m_bLButtonDown = TRUE;
	m_bDraggingRect = FALSE;
	m_start_pt = point;
	CView::OnLButtonDown(nFlags, m_start_pt);
}
//===============================================================================================
// Select all parts and trace segments in rectangle
// Fill flag selected
// Set utility flags for selected parts and segments
//
void CFreePcbView::SelectItemsInRect( CRect r, UINT nFlags )
{
#define SET_M 0 //set cursor mode (dis)
	int EN_INV = 0, UNSEL = 0, SEL_INTERSECT = 0;
	//
	// reset page for group_selected
	m_page = 1;
	//
	if( m_last_mouse_point.x == r.left )
	{
		if( m_sel_count )
			UNSEL = 1;
		else
			SEL_INTERSECT = 1;
	}

	r.NormalizeRect();

	// mark all merges
	m_Doc->m_mlist->mark0();
	int p_sel_count = m_sel_count;
	prev_sel_count = 0;
	{
	// find outline sides in rect
		for( int im=0; im<m_Doc->m_outline_poly->GetSize(); im++ )
		{
			CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(im);
			if( poly->m_visible == 0 )
				continue;
			if( m_Doc->m_vis[poly->GetLayer()] == 0 )
				continue;
			RECT grp =poly->GetBounds();
			if (grp.left > r.right)
				continue;
			if (r.left > grp.right)
				continue;
			if (grp.bottom > r.bottom)
				continue;
			if (r.top > grp.top)
				continue;
			id smid = poly->GetId();
			int gnc = poly->GetNumContours()-1;
			int ncor = poly->GetNumCorners();
			for( int ic=0; ic<=gnc; ic++ )
			{
				int istart = poly->GetContourStart(ic);
				int iend = poly->GetContourEnd(ic);	
				for( int is=istart; is<=iend; is++ )
				{
					if( is == ncor-1 )
						if( poly->GetClosed() == 0 )
							break;
					if( ( poly->GetSideSel(is) == 0 && UNSEL == 0 ) ||
						( poly->GetSideSel(is) && UNSEL ) )
					{
						int ic1, ic2;
						ic1 = is;
						ic2 = poly->GetNextCornerIndex(ic1);
						int x1 = poly->GetX(ic1);
						int y1 = poly->GetY(ic1);
						int x2 = poly->GetX(ic2);
						int y2 = poly->GetY(ic2);
						if( InRange( x1, r.left, r.right )
							&& InRange( x2, r.left, r.right )
							&& InRange( y1, r.top, r.bottom )
							&& InRange( y2, r.top, r.bottom ) )
						{ 								
							smid.sst = ID_SIDE;
							smid.ii = is;
							if( poly->Check( index_part_attr ) && ( m_sel_mask & (1<<SEL_MASK_PART ) ) )
							{
								if( (poly->GetSideSel() && UNSEL) || (poly->GetSideSel() == 0 && UNSEL == 0) )
									m_Doc->SelectPart( poly->pAttr[index_part_attr], UNSEL?NULL:poly->pAttr[index_pin_attr], UNSEL );
								break;
							}
							else if( m_sel_mask & (1<<SEL_MASK_OP_SIDES ) )
							{
								if( UNSEL )
									UnSelect( NULL, &smid, SET_M );
								else
									NewSelect( NULL, &smid, SET_M, EN_INV );
							}
						}
						else if( SEL_INTERSECT )
						{
							smid.sst = ID_SIDE;
							smid.ii = is;
							double d1=0.0, d2=0.0;
							int test = FindSegmentIntersections( r.left, r.bottom, r.right, r.bottom, 0,
								x1, y1, x2, y2, 0, &d1, &d2 );
							if( !test )
							{
								test = FindSegmentIntersections( r.left, r.top, r.right, r.top, 0,
									x1, y1, x2, y2, 0, &d1, &d2 );
								if( !test )
								{
									test = FindSegmentIntersections( r.left, r.top, r.left, r.bottom, 0,
										x1, y1, x2, y2, 0, &d1, &d2 );
									if( !test )
									{
										test = FindSegmentIntersections( r.right, r.top, r.right, r.bottom, 0,
											x1, y1, x2, y2, 0, &d1, &d2 );
									}
								}
							}
							if( test )
							{
								if( poly->Check( index_part_attr ) && ( m_sel_mask & (1<<SEL_MASK_PART ) ) )
								{	
									m_Doc->SelectPart( poly->pAttr[index_part_attr], poly->pAttr[index_pin_attr], FALSE );
									break;
								}
								else
									NewSelect( NULL, &smid, SET_M, EN_INV );
							}
						}
					}
				}
			}
		}
	}

	// find texts in rect
	if( m_sel_mask & (1<<SEL_MASK_TEXT ) && ( m_sel_count == p_sel_count || UNSEL ) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			if( m_Doc->m_vis[lay] == 0 )
				continue;
			//
			int it = -1;
			CText * t = tl->GetNextText(&it);
			while( t )
			{
				if( t->isVISIBLE )
					if( t->m_font_size && t->dl_el )
						if( t->dl_el->visible )
							if( !t->m_selected || UNSEL )
							{
								RECT Get;
								m_Doc->m_dlist->Get_Rect( t->dl_sel, &Get );
								if( InRange( Get.left, r.left, r.right ) && 
									InRange( Get.right, r.left, r.right ) && 
									InRange( Get.top, r.top, r.bottom ) && 
									InRange( Get.bottom, r.top, r.bottom ) )
								{
									// add text to selection list and highlight it
									id sid(ID_TEXT_DEF);
									if( UNSEL )
										UnSelect( t, &sid, SET_M );
									else
										NewSelect( t, &sid, SET_M, EN_INV );
								}
							}
				t = tl->GetNextText(&it);
			}
			if( UNSEL )
			{
				it = -1;
				t = tl->GetNextText(&it);
				while( t )
				{
					if( t->m_selected )
						if( t->m_font_size == 0 )
						{
							if( InRange( t->m_x, r.left, r.right ) && 
								InRange( t->m_y, r.top, r.bottom ) )
							{
								// add text to selection list and highlight it
								id sid(ID_TEXT_DEF);
								UnSelect( t, &sid, SET_M );
							}
						}
					t = tl->GetNextText(&it);
				}
			}
		}
	}

	if( m_sel_mask & (1<<SEL_MASK_OP_CORNERS ) )
	{
	// find outline sides in rect
		for( int im=0; im<m_Doc->m_outline_poly->GetSize(); im++ )
		{
			CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(im);
			if( poly->m_visible == 0 )
				continue;
			if( m_Doc->m_vis[poly->GetLayer()] == 0 )
				continue;
			RECT grp =poly->GetBounds();
			if (grp.left > r.right)
				continue;
			if (r.left > grp.right)
				continue;
			if (grp.bottom > r.bottom)
				continue;
			if (r.top > grp.top)
				continue;
			id smid = poly->GetId();
			int gnc = poly->GetNumContours()-1;
			int ncor = poly->GetNumCorners();
			int ncl = poly->GetClosed();
			for( int ic=0; ic<=gnc; ic++ )
			{
				int istart = poly->GetContourStart(ic);
				int iend = poly->GetContourEnd(ic);	
				for( int is=istart; is<=iend; is++ )
				{
					int icb = poly->GetPrevCornerIndex(is);
					if( ( poly->GetSel(is) && UNSEL ) ||
						( poly->GetSel(is) == 0 && UNSEL == 0 ) )
					{
						int ic1, ic2;
						ic1 = is;
						int x1 = poly->GetX(ic1);
						int y1 = poly->GetY(ic1);
						if( UNSEL )
						{
							if( InRange( x1, r.left, r.right )
								&& InRange( y1, r.top, r.bottom ) )
							{
								smid.sst = ID_CORNER;
								smid.ii = ic1;
								UnSelect( NULL, &smid, SET_M );
							}
						}
						else if( is == 0 )
						{
							if( poly->GetSideSel(is) == 0 )
							{
								if( InRange( x1, r.left, r.right )
								&& InRange( y1, r.top, r.bottom ) )
								{
									smid.sst = ID_CORNER;
									smid.ii = ic1;
									NewSelect( NULL, &smid, SET_M, EN_INV );
								}
							}
						}
						else if( is == (ncor-1) && ncl == 0 )
						{
							if( poly->GetSideSel(icb) == 0 )
							{
								if( InRange( x1, r.left, r.right )
								&& InRange( y1, r.top, r.bottom ) )
								{
									smid.sst = ID_CORNER;
									smid.ii = ic1;
									NewSelect( NULL, &smid, SET_M, EN_INV );
								}
							}
						}
						else if( icb != (ncor-1) || ncl )
						{
							if( poly->GetSideSel(is) == 0 && poly->GetSideSel(icb) == 0 )
								if( InRange( x1, r.left, r.right )
									&& InRange( y1, r.top, r.bottom ) )
								{
									smid.sst = ID_CORNER;
									smid.ii = ic1;
									NewSelect( NULL, &smid, SET_M, EN_INV );
								}
						}
					}
				}
			}
		}
	}
	if( getbit( m_sel_flags, FLAG_SEL_PART ) )
		clrbit( m_sel_flags, FLAG_SEL_PIN ); // !!

	// now highlight selected items
	if( m_sel_count <= 0 )
		CancelSelection();
	else if( m_sel_count == 1 && m_sel_id.type == ID_POLYLINE && m_sel_id.sst == ID_CORNER )
	{
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		SetCursorMode( CUR_OP_CORNER_SELECTED );
	}
	else
	{
		m_page = 1;
		SetFKText(CUR_GROUP_SELECTED);
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		SetCursorMode( CUR_GROUP_SELECTED );
		if( m_dlg_partlist )
			if( m_dlg_partlist->IsWindowVisible() )
				m_dlg_partlist->ImportPartlist();
	}
	gLastKeyWasArrow = FALSE;
}


//===============================================================================================
// Start dragging group being moved or added
// If group is being added (i.e. pasted):
//	bAdd = TRUE;
//	x, y = coords for cursor point for dragging
//
void CFreePcbView::StartDraggingGroup( BOOL bAdd, int x, int y )
{ 
	m_disable_context_menu = 1;
	int MAX_LINES = 500;
	if( !bAdd )
	{
		SetCursorMode( CUR_DRAG_GROUP );
	}
	else
	{
		SetCursorMode( CUR_DRAG_GROUP_ADD );
		//m_last_mouse_point.x = x;
		//m_last_mouse_point.y = y;
		//m_last_cursor_point.x = x;
		//m_last_cursor_point.y = y;
	}

	int fx = 0;
	int fy = 0;
	int memx = m_last_cursor_point.x;
	int memy = m_last_cursor_point.y;
	int min = INT_MAX;
	for (int isel = 0; isel < m_Doc->m_outline_poly->GetSize(); isel++)
	{
		CPolyLine* gp = &m_Doc->m_outline_poly->GetAt(isel);
		if (gp->GetSideSel())
		{
			for (int sid = 0; sid < gp->GetNumSides(); sid++)
			{
				if (gp->GetSideSel(sid))
				{
					int gx = gp->GetX(sid);
					int gy = gp->GetY(sid);
					if (abs(gx - memx) + abs(gy - memy) < min)
					{
						fx = gx;
						fy = gy;
						min = abs(gx - memx) + abs(gy - memy);
					}
					if (gp->GetClosed() == 0)
						if (sid == gp->GetNumSides() - 1)
						{
							gx = gp->GetX(gp->GetNextCornerIndex(sid));
							gy = gp->GetY(gp->GetNextCornerIndex(sid));
							if (abs(gx - memx) + abs(gy - memy) < min)
							{
								fx = gx;
								fy = gy;
								min = abs(gx - memx) + abs(gy - memy);
							}
						}
				}
			}
		}
	}
	if (fx && fy)
		m_last_mouse_point = CPoint(fx, fy);

	// snap dragging point to placement grid
	// SnapCursorPoint( m_last_mouse_point, -1 );
	m_last_cursor_point = m_last_mouse_point;
	m_from_pt = m_last_cursor_point;

	int n_lines = 4, n_rat_lines = 0;
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			if( m_Doc->m_vis[lay] == 0 )
				continue;
			int it = -1;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
				if ( t->m_selected && t->dl_sel )
					n_lines +=4;
		}
	}
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			int iMax = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for(int ii=iMax; ii>=0; ii--)
			{
				int ib; 
				int in;
				if( cl == 0 )
				{
					ib = max(ii-1,0);
					in = min(ii+1,iMax);
				}
				else
				{
					ib = m_Doc->m_outline_poly->GetAt(item).GetPrevCornerIndex(ii);
					in = m_Doc->m_outline_poly->GetAt(item).GetNextCornerIndex(ii);
				}
				if( m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) )
					if( cl || (ii != iMax && ii != 0))
						n_rat_lines += 2;
					else 
						n_rat_lines++;
				if( ii == iMax && cl == 0 )
					continue;
				if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) )
				{
					n_lines++;
					if( in != iMax || cl )
						if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( in ) == 0 )
							n_rat_lines++;
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ib ) == 0 )
						n_rat_lines++;
				}
			}
		}
	}
	BOOL bLimit = 0;
	// set up dragline array
	if( n_lines > MAX_LINES )
	{
		n_lines = 4;
		bLimit = TRUE;
	}
	m_Doc->m_dlist->MakeDragLineArray( n_lines );
	m_Doc->m_dlist->MakeDragRatlineArray( n_rat_lines, 1 );
	//
	RECT LIM;
	LIM.left = LIM.bottom = INT_MAX;
	LIM.top = LIM.right = INT_MIN;
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			int n_sel = 0;
			CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(item);
			int iMax = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for(int ii=iMax; ii>=0; ii--)
			{
				int icb,icn,icbb,icnn;
				if( cl == 0 )
				{
					icb = max( 0, ii-1 );
					icn = min( iMax, ii+1 );
					icbb = max( 0, icb-1 );
					icnn = min( iMax, icn+1 );
				}
				else
				{
					icb = poly->GetPrevCornerIndex(ii);
					icn = poly->GetNextCornerIndex(ii);
					icbb = poly->GetPrevCornerIndex(icb);
					icnn = poly->GetNextCornerIndex(icn);
				}
				int i_d1 = -1, i_d2 = -1;
				//------------------------------------------------------
				if( ii == iMax && cl == 0 )
				{
					if( m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) &&
						m_Doc->m_outline_poly->GetAt(item).GetSideSel( icb ) == 0 &&
						m_Doc->m_outline_poly->GetAt(item).GetSel( icb ) == 0 &&
						m_Doc->m_outline_poly->GetAt(item).GetSideSel( icbb ) == 0 )
						i_d1 = icb;
				}
				else if( (m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) || m_Doc->m_outline_poly->GetAt(item).GetSel( ii )) &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( icb ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSel( icb ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( icbb ) == 0 )
					i_d1 = icb;
				//--------------------------------------------------------
				if( ii == iMax && cl == 0 )
				{//
				}//
				else if( ii == iMax-1 && cl == 0 )
				{
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) )
					{//
					}//
					else if( m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) &&
							 m_Doc->m_outline_poly->GetAt(item).GetSel( icn ) == 0 )
						i_d2 = ii;
				}
				else if( ii == iMax-2 && cl == 0 )
				{
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) &&
						m_Doc->m_outline_poly->GetAt(item).GetSideSel( icn ) == 0 &&
						m_Doc->m_outline_poly->GetAt(item).GetSel( icnn ) == 0 )
						i_d2 = icn;
					else if( m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) &&
						m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) == 0 &&
						m_Doc->m_outline_poly->GetAt(item).GetSideSel( icn ) == 0 &&
						m_Doc->m_outline_poly->GetAt(item).GetSel( icn ) == 0 )
						i_d2 = ii;
				}
				else if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( icn ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( icnn ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSel( icnn ) == 0 )
					i_d2 = icn;
				else if( m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSideSel( icn ) == 0 &&
					m_Doc->m_outline_poly->GetAt(item).GetSel( icn ) == 0 )
					i_d2 = ii;
				//----------------------------------------------------------
				if( ii < iMax || cl )
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) || 
						(m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) && m_Doc->m_outline_poly->GetAt(item).GetSel( icn )))
					{
						n_sel++;
						CPoint p1( poly->GetX(ii) - m_from_pt.x, poly->GetY(ii) - m_from_pt.y );
						CPoint p2( poly->GetX(icn) - m_from_pt.x, poly->GetY(icn) - m_from_pt.y );
						if( !bLimit )
							m_Doc->m_dlist->AddDragLine( p1, p2 );
						else
						{
							SwellRect( &LIM, p1 );
							SwellRect( &LIM, p2 );
						}
						m_Doc->m_outline_poly->GetAt(item).MakeVisible(ii,0);
						m_Doc->m_outline_poly->GetAt(item).MakeVisible(icb,0);
						m_Doc->m_outline_poly->GetAt(item).MakeVisible(icn,0);
						int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();
						if( isrc >= 0 )
							m_Doc->m_dlist->SetTemplateVis(0,isrc);
					}
				if( i_d1 >= 0 )
				{
					n_sel++;
					CPoint p1( poly->GetX(i_d1), poly->GetY(i_d1) );
					int i_d1n = (cl==0 ? i_d1+1 : poly->GetNextCornerIndex(i_d1));
					CPoint p2( poly->GetX(i_d1n) - m_from_pt.x, poly->GetY(i_d1n) - m_from_pt.y );
					m_Doc->m_dlist->AddDragRatline( p1, p2 );
					m_Doc->m_dlist->HighLight( poly->dl_side[i_d1] );
					m_Doc->m_outline_poly->GetAt(item).MakeVisible(i_d1,0);
					//int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();?
					//if( isrc >= 0 )?
					//	m_Doc->m_dlist->SetTemplateVis(0,isrc);?
				}
				if( i_d2 >= 0 )
				{
					n_sel++;
					CPoint p1( poly->GetX(i_d2) - m_from_pt.x, poly->GetY(i_d2) - m_from_pt.y );
					int i_d2n = (cl==0 ? i_d2+1 : poly->GetNextCornerIndex(i_d2));
					CPoint p2( poly->GetX(i_d2n), poly->GetY(i_d2n) );
					m_Doc->m_dlist->AddDragRatline( p2, p1 );
					m_Doc->m_dlist->HighLight( poly->dl_side[i_d2] );
					m_Doc->m_outline_poly->GetAt(item).MakeVisible(i_d2,0);
					//int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();?
					//if( isrc >= 0 )?
					//	m_Doc->m_dlist->SetTemplateVis(0,isrc);?
				}
			}
		}
	}
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			if( m_Doc->m_vis[lay] == 0 )
				continue;
			int it = -1;
			for(CText* text=tl->GetNextText(&it); text; text=tl->GetNextText(&it))
				if( text->m_selected && text->dl_sel && text->m_font_size )
				{
					text->dl_el->visible = 0;
					CPoint P[4];
					int np = 4;
					m_Doc->m_dlist->Get_Points( text->dl_sel, P, &np ); 
					if( np == 4 )
					{
						for( int ii=0; ii<4; ii++ )
						{
							P[ii].x -= m_from_pt.x;
							P[ii].y -= m_from_pt.y;
						}
						if( !bLimit )
						{
							m_Doc->m_dlist->AddDragLine( P[0], P[1] );
							m_Doc->m_dlist->AddDragLine( P[1], P[2] );
							m_Doc->m_dlist->AddDragLine( P[2], P[3] );
							m_Doc->m_dlist->AddDragLine( P[3], P[0] );
						}
						else
						{
							SwellRect( &LIM, P[0] );
							SwellRect( &LIM, P[1] );
							SwellRect( &LIM, P[2] );
							SwellRect( &LIM, P[3] );
						}
					}
					else
					{
						RECT Get;
						m_Doc->m_dlist->Get_Rect( text->dl_sel, &Get );
						CPoint Pt[4];
						Pt[0].x = Get.left		- m_from_pt.x;
						Pt[0].y = Get.bottom	- m_from_pt.y;
						Pt[1].x = Get.left		- m_from_pt.x;
						Pt[1].y = Get.top		- m_from_pt.y;
						Pt[2].x = Get.right		- m_from_pt.x;
						Pt[2].y = Get.top		- m_from_pt.y;
						Pt[3].x = Get.right		- m_from_pt.x;
						Pt[3].y = Get.bottom	- m_from_pt.y;
						if( !bLimit )
						{
							m_Doc->m_dlist->AddDragLine( P[0], P[1] );
							m_Doc->m_dlist->AddDragLine( P[1], P[2] );
							m_Doc->m_dlist->AddDragLine( P[2], P[3] );
							m_Doc->m_dlist->AddDragLine( P[3], P[0] );
						}
						else
						{
							SwellRect( &LIM, P[0] );
							SwellRect( &LIM, P[1] );
							SwellRect( &LIM, P[2] );
							SwellRect( &LIM, P[3] );
						}
					}						
				}
		}
	}
	if( bLimit && LIM.right > LIM.left )
	{
		CPoint p1( LIM.left, LIM.bottom );
		CPoint p2( LIM.left, LIM.top );
		CPoint p3( LIM.right, LIM.bottom );
		CPoint p4( LIM.right, LIM.top );
		m_Doc->m_dlist->AddDragLine( p1, p2 );
		m_Doc->m_dlist->AddDragLine( p1, p3 );
		m_Doc->m_dlist->AddDragLine( p4, p2 );
		m_Doc->m_dlist->AddDragLine( p4, p3 );
	}
	// make texts, parts and segments invisible
	m_Doc->m_dlist->SetLayerVisible( LAY_HILITE, FALSE );
	//
	CDC *pDC = GetDC();
	pDC->SelectClipRgn( &m_pcb_rgn );
	SetDCToWorldCoords( pDC );
	CPoint p;
	p.x  = m_from_pt.x;
	p.y  = m_from_pt.y;
	CPoint cur_p = m_Doc->m_dlist->PCBToScreen( p );
	SetCursorPos( cur_p.x, cur_p.y );
	m_Doc->m_dlist->StartDraggingArray( pDC, m_from_pt.x, m_from_pt.y, TRUE );
	ReleaseDC( pDC );
}
//===============================================================================================
void CFreePcbView::CancelDraggingGroup()
{
	m_Doc->m_dlist->StopDragging();
	//
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			if( m_Doc->m_outline_poly->GetAt(item).GetSel() )
			{
				m_Doc->m_outline_poly->GetAt(item).MakeVisible( TRUE );
				int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();
				if( isrc >= 0 )
					m_Doc->m_dlist->SetTemplateVis(1,isrc);
			}
		}
	}
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for(CText* text=tl->GetNextText(&it); text; text=tl->GetNextText(&it))
				if( text->m_selected )
				{
					text->dl_el->visible = TRUE;
				}
		}
	}
	if( m_Doc->m_vis[LAY_HILITE] )
		m_Doc->m_dlist->SetLayerVisible( LAY_HILITE, TRUE );
	if( !m_dragging_mode )
		SetCursorMode( CUR_GROUP_SELECTED );
}
//===============================================================================================
//---------------------------------------- Mirror PCB -------------------------------------------
//===============================================================================================
void CFreePcbView::TurnGroup ()
{
	m_Doc->SelectGroupAttributes();
	if( prev_sel_count != m_sel_count )
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pMain )
		return;

	FindGroupCenter();
		// outlines
	CString str = "Mirror Lines...";
	pMain->DrawStatus( 3, &str );
	Invalidate( FALSE );// (doubtful)
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item--)
		{
			if ( m_Doc->m_outline_poly->GetAt(item).GetNumCorners() < 2 )
			{
				m_Doc->m_outline_poly->GetAt(item).m_visible = 0;
				//AfxMessageBox(" ASSERT! Number of corners < 2 (FreePcbView.cpp)", MB_ICONERROR);
				continue;
			}
			if ( m_Doc->m_outline_poly->GetAt(item).GetSel() ) 
			{
				int iMax = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
				int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
				for(int im=iMax; im>=0; im--)
				{
					int bx = m_Doc->m_outline_poly->GetAt(item).GetX(im);
					int by = m_Doc->m_outline_poly->GetAt(item).GetY(im);
					m_Doc->m_outline_poly->GetAt(item).MoveCorner(im,2*groupAverageX-bx,by,FALSE);
					if( cl || im < iMax )
					{
						int style = m_Doc->m_outline_poly->GetAt(item).GetSideStyle(im);
						m_Doc->m_outline_poly->GetAt(item).SetSideStyle(im, (style==1?2:(style==2?1:0)), FALSE);
					}
				}
				m_Doc->m_outline_poly->GetAt(item).Undraw();
				RECT r = rect(0,0,0,0);
				m_Doc->m_outline_poly->GetAt(item).ModifyGroupRect(&r);
				r = m_Doc->m_outline_poly->GetAt(item).GetCornerBounds();
				int gl = m_Doc->m_outline_poly->GetAt(item).GetLayer();
				int gs = m_Doc->m_outline_poly->GetAt(item).GetPicture();
				if( gs >= 0 )
					m_Doc->m_dlist->MoveTemplate( &r, gs, gl );

				m_Doc->m_outline_poly->GetAt(item).Draw();
			}
		}
	}
	str = "Mirror Texts...";
	pMain->DrawStatus( 3, &str );
	Invalidate( FALSE );// (doubtful)
	if( getbit( m_sel_flags, FLAG_SEL_TEXT ) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for ( CText * t = tl->GetNextText(&it); t; t=tl->GetNextText(&it) )				
			{
				if( t->m_selected )
				{
					RECT TR;
					tl->GetTextRectOnPCB( t, &TR );
					int wid = TR.right - TR.left;
					int hgt = abs(TR.top - TR.bottom);
					int newX = 2*groupAverageX - t->m_x;
					int newY = t->m_y;
					t->m_angle = t->m_angle%360;
					if (t->m_str.Right(2) != "|C")
					{
						if (t->m_angle == 90)
							newY = t->m_y - hgt;
						else if (t->m_angle == 270)
							newY = t->m_y + hgt;
						else if (t->m_angle == 180)
							newX = 2 * groupAverageX - t->m_x + wid;
						else
							newX = 2 * groupAverageX - t->m_x - wid;
					}
					if( getbit( t->m_pdf_options, PDF_ALIGN ) )
						clrbit( t->m_pdf_options, PDF_ALIGN );
					else
						setbit( t->m_pdf_options, PDF_ALIGN );
					tl->MoveText(t,newX,newY,(t->m_angle==0?0:(360-t->m_angle)),t->m_layer);
					if( t->m_layer == LAY_PIN_DESC || t->m_layer >= LAY_ADD_1 )
						DrawBOM(t);
				}
			}
		}
	}
	// 
	m_Doc->m_dlist->CancelHighLight();
	HighlightGroup();
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = 0;// TurnGroup
}

//===============================================================================================
//---------------------------------------- Move Group -------------------------------------------
//===============================================================================================
void CFreePcbView::OnGroupMove()
{
	if( m_sel_count > 1 )
		m_Doc->SelectGroupAttributes();
	else if( m_sel_id.type == ID_POLYLINE )
	{
		 if( m_sel_op.GetNumSides() == 1 )
			m_Doc->SelectGroupAttributes();
	}
	else if( m_sel_id.type == ID_TEXT && m_dragging_mode != FK_DRAG_LINE )
	{
		if( m_sel_text->m_layer == LAY_PART_NAME )
			m_Doc->SelectPartAttributes( m_sel_text );
	}
	else if( m_dragging_mode == FK_DRAG_GROUP )
		m_Doc->SelectGroupAttributes();
	HighlightGroup();
	StartDraggingGroup(0,m_last_mouse_point.x,m_last_mouse_point.y);
	m_draw_layer = LAY_BACKGND;// OnGroupMove
	Invalidate( FALSE );
}

//===============================================================================================
//---------------------- Move group of parts and trace segments ---------------------------------
//===============================================================================================
void CFreePcbView::MoveGroup( int dx, int dy, CText * BOM_TXT )
{
	if( prev_sel_count != m_sel_count )
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
	
	// outlines
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			BOOL RedrawFlag = FALSE;
			int gns = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for( int ic=gns; ic>=0; ic-- )
			{
				int prev = m_Doc->m_outline_poly->GetAt(item).GetPrevCornerIndex( ic );
				BOOL sel, prev_sel;
				if( cl == 0 && ic == gns )
					sel = 0;
				else
					sel = m_Doc->m_outline_poly->GetAt(item).GetSideSel( ic );
				if( cl == 0 && prev == gns )
					prev_sel = 0;
				else
					prev_sel = m_Doc->m_outline_poly->GetAt(item).GetSideSel( prev );
				BOOL cor_sel = m_Doc->m_outline_poly->GetAt(item).GetSel( ic );
				if( sel || prev_sel || cor_sel )
				{
					RedrawFlag = TRUE;
					m_Doc->m_outline_poly->GetAt(item).SetX(ic,m_Doc->m_outline_poly->GetAt(item).GetX(ic)+dx);
					m_Doc->m_outline_poly->GetAt(item).SetY(ic,m_Doc->m_outline_poly->GetAt(item).GetY(ic)+dy);
				}	
			}
			if( RedrawFlag )
			{
				m_Doc->m_outline_poly->GetAt(item).RecalcRectC(0);
				m_Doc->m_outline_poly->GetAt(item).ModifyGroupRect( dx, dy );
				m_Doc->m_outline_poly->GetAt(item).Draw( m_Doc->m_dlist );
				int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();
				if( isrc >= 0 )
				{
					RECT pr = m_Doc->m_outline_poly->GetAt(item).GetCornerBounds();
					m_Doc->m_dlist->MoveTemplate( &pr, isrc, m_Doc->m_outline_poly->GetAt(item).GetLayer() );
				}
			}
		}
	}

	// texts after outlines !
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
			{
				if ( t->m_selected )
				{
					tl->MoveText(t,t->m_x+dx,t->m_y+dy,t->m_angle,t->m_layer);
					if( t->m_layer == LAY_PIN_DESC || t->m_layer >= LAY_ADD_1 )
						DrawBOM(t);
					else if( t->m_str.Compare( NO_FOOTPRINT ) == 0 )
					{
						if( t->m_layer == LAY_FOOTPRINT )
						{
							if( t->m_polyline_start >= 0 )
							{
								CPolyLine * poly = &m_Doc->m_outline_poly->GetAt( t->m_polyline_start );
								if( poly->Check( index_part_attr ) )
									tl->MoveText(t,poly->pAttr[index_part_attr]->m_x,poly->pAttr[index_part_attr]->m_y,t->m_angle,t->m_layer);
							}
						}
					}
				}
			}
		}
	}
	else if( BOM_TXT )
		DrawBOM( BOM_TXT );
	groupAverageX+=dx;
	groupAverageY+=dy;
}


void CFreePcbView::OnInsidePolyline()
{
	// sel texts
	id ID(ID_TEXT_DEF);
	int it = -1;
	id sv_id(m_sel_id);
	int i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_Reflist->GetNextText(&it); t; t=m_Doc->Attr->m_Reflist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	it = -1;
	m_sel_id = sv_id;
	i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_Pinlist->GetNextText(&it); t; t=m_Doc->Attr->m_Pinlist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	it = -1;
	m_sel_id = sv_id;
	i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_pDesclist->GetNextText(&it); t; t=m_Doc->Attr->m_pDesclist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	it = -1;
	m_sel_id = sv_id;
	i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_Netlist->GetNextText(&it); t; t=m_Doc->Attr->m_Netlist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	it = -1;
	m_sel_id = sv_id;
	i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_Footlist->GetNextText(&it); t; t=m_Doc->Attr->m_Footlist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	it = -1;
	m_sel_id = sv_id;
	i = m_sel_id.i;
	for( CText * t=m_Doc->Attr->m_Valuelist->GetNextText(&it); t; t=m_Doc->Attr->m_Valuelist->GetNextText(&it) )
		if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( t->m_x, t->m_y ) )
			NewSelect( t, &ID, 0, 0 );
	// sel lines
	for( int ip=m_Doc->m_outline_poly->GetSize()-1; ip>=0; ip-- )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(ip);
		if( p->GetVisible() == 0 )
			continue;
		// test all corners
		int ok = p->GetNumCorners();
		int nc = ok/2;
		for( int icor=p->GetNumCorners()-1; icor>=0; icor-- )
		{
			if( m_Doc->m_outline_poly->GetAt(i).TestPointInsideContour( p->GetX(icor), p->GetY(icor) ) == 0 )
				ok--;
		}
		if( ok > nc )
		{
			if( p->pAttr[index_part_attr] )
				setbit(m_sel_flags,FLAG_SEL_PART);
			ID = p->GetId();		
			ID.i = ip;
			ID.sst = ID_SIDE;
			for( int isd=p->GetNumSides()-1; isd>=0; isd-- )
			{
				ID.ii = isd;
				NewSelect( NULL, &ID, 0, 0 );
			}
		}
	}
	if( m_sel_count )
	{
		SetCursorMode( CUR_GROUP_SELECTED );
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
	}
}


//===============================================================================================
// Highlight group selection
// the only legal members are parts, texts, trace segments and
// copper area, solder mask cutout and outline sides
//
void CFreePcbView::HighlightGroup(BOOL bPins)
{
	if( m_sel_count )
	{
		// texts
		if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
		{
			for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
			{
				CTextList * tl = m_Doc->GetTextListbyLayer( lay );
				if(!tl)
					continue;
				if( m_Doc->m_vis[lay] == 0 )
					continue;
				int it = -1;
				for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
					if ( t->m_selected )
						tl->HighlightText(t);
			}
		}
		// outlines
		if( getbit(m_sel_flags, FLAG_SEL_OP) )
		{
			for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
			{
				int gns = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
				BOOL cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
				BOOL bH = 0;
				for( int ic=gns; ic>=0; ic-- )
				{
					int Sel =  m_Doc->m_outline_poly->GetAt(item).GetSel(ic);
					int iprev = m_Doc->m_outline_poly->GetAt(item).GetPrevCornerIndex(ic);
					int inext = m_Doc->m_outline_poly->GetAt(item).GetNextCornerIndex(ic);
					if( Sel )
					{
						m_Doc->m_outline_poly->GetAt(item).HighlightCorner(ic);
						if( m_Doc->m_outline_poly->GetAt(item).GetSel(iprev) == 0 )
							m_Doc->m_outline_poly->GetAt(item).HighlightCorner(iprev,1);
						if( m_Doc->m_outline_poly->GetAt(item).GetSel(inext) == 0 )
							m_Doc->m_outline_poly->GetAt(item).HighlightCorner(inext, 1);
					}
					if( cl == 0 && ic == gns )
						continue;
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(ic) )
					{
						bH = 1;
						m_Doc->m_outline_poly->GetAt(item).HighlightSide(ic);
						int ipp = m_Doc->m_outline_poly->GetAt(item).GetPrevCornerIndex(iprev);
						if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(iprev) == 0 )
							if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(ipp) == 0 )
								m_Doc->m_outline_poly->GetAt(item).HighlightCorner(iprev,1);
						int inn = m_Doc->m_outline_poly->GetAt(item).GetNextCornerIndex(inext);
						if( cl == 0 )
						{
							inext = min( inext, gns-1 );
							inn = min( inn, gns-1 );
						}
						if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(inext) == 0 )
							if( inn == inext )
								m_Doc->m_outline_poly->GetAt(item).HighlightCorner(inn+1,1);
							else if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(inn) == 0 )
								m_Doc->m_outline_poly->GetAt(item).HighlightCorner(inn,1);
					}
				}	
				if( bH && getbit( m_sel_mask, SEL_MASK_PART ) )
				{
					RECT * rgr = m_Doc->m_outline_poly->GetAt(item).GetGroupRect();
					if( rgr )
					{
						m_Doc->m_dlist->HighlightRect( rgr, m_Doc->m_outline_poly->GetAt(item).GetLayer(), TRANSPARENT_BACKGND );
					}
				}
			}
		}
	}
	if( m_sel_count <= 1 )
	{// none
	}// none
	else if( m_sel_id.type == ID_POLYLINE )
	{
		if( m_sel_count == m_sel_op.GetNumSides() )
		{// none
		}// none
		else
			m_Doc->m_dlist->HighlightSelRect( TRANSPARENT_HILITE );
	}
	else
		m_Doc->m_dlist->HighlightSelRect( TRANSPARENT_HILITE );
}


//===============================================================================================
// Set array of selection mask ids
//
void CFreePcbView::SetSelMaskArray( int mask )
{
	for( int i=0; i<NUM_SEL_MASKS; i++ )
	{
		if( mask & (1<<i) )
			m_mask_id[i].ii = m_mask_id[i].def;
		else
			m_mask_id[i].ii = 0xfffe;	// guaranteed not to exist
	}
}

void CFreePcbView::GroupCopy( int clearBuf )
{
	// clear clipboard
	if( clearBuf )
		m_Doc->clip_outline_poly.SetSize(0);
	m_Doc->clip_mlist->CopyFrom( m_Doc->m_mlist );
	m_Doc->clip_mlist->mark0();

	// set pointers
	CArray<CPolyLine> * g_op = &m_Doc->clip_outline_poly;
	//
	// copy to group
	// outlines
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		// copy polylines
		int it = -1;
		for( CText *ref=m_Doc->Attr->m_Reflist->GetNextText(&it);
					ref;
					ref=m_Doc->Attr->m_Reflist->GetNextText(&it) )
		{
			if( ref->m_polyline_start >= 0 )
			{
				for( int get=ref->m_polyline_start; get>=0; get=m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
				{
					CPolyLine * p = &m_Doc->m_outline_poly->GetAt( get );
					if( p->GetSel() )
					{
						if( p->GetVisible() == 0 )
							continue;
						int g_ism = g_op->GetSize();
						g_op->SetSize(g_ism+1);
						CPolyLine * g_p = &g_op->GetAt(g_ism);
						g_p->Copy( p );
						id sid = p->GetId();
						sid.i = g_ism;
						g_p->SetId( &sid );
						int pm = p->GetMerge();
						if( pm >= 0 )
							m_Doc->clip_mlist->mark1(pm);
						int psm = p->GetMerge( TRUE );
						if( psm >= 0 )
							m_Doc->clip_mlist->mark1(psm);
						int i_att = -1;
						for( CText * t = m_Doc->ScanAttr( p, &i_att ); t; t = m_Doc->ScanAttr( p, &i_att ) )
						{
							if( t->m_merge >= 0 )
								m_Doc->clip_mlist->mark1(t->m_merge);
							if( t->m_submerge >= 0 )
								m_Doc->clip_mlist->mark1(t->m_submerge);
						}
						g_p->Next[index_part_attr] = -1;
						g_p->Next[index_value_attr] = -1;
						g_p->Next[index_foot_attr] = -1;
					}
				}
			}
		}
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			if( m_Doc->m_outline_poly->GetAt(item).GetSideSel() )// use only GetSideSel !!!
			{
				// add to group
				CPolyLine * p = &m_Doc->m_outline_poly->GetAt(item);
				if( p->GetVisible() == 0 )
					continue;
				if( p->pAttr[index_part_attr] )
					continue;
				int g_ism = g_op->GetSize();
				g_op->SetSize(g_ism+1);
				CPolyLine * g_p = &g_op->GetAt(g_ism);
				g_p->Copy( p );
				id sid = p->GetId();
				sid.i = g_ism;
				g_p->SetId( &sid );
				int pm = p->GetMerge();
				if( pm >= 0 )
					m_Doc->clip_mlist->mark1(pm);
				int psm = p->GetMerge( TRUE );
				if( psm >= 0 )
					m_Doc->clip_mlist->mark1(psm);
				int i_att = -1;
				for( CText * t = m_Doc->ScanAttr( p, &i_att ); t; t = m_Doc->ScanAttr( p, &i_att ) )
				{
					if( t->m_merge >= 0 )
						m_Doc->clip_mlist->mark1(t->m_merge);
					if( t->m_submerge >= 0 )
						m_Doc->clip_mlist->mark1(t->m_submerge);
				}
				g_p->Next[index_part_attr] = -1;
				g_p->Next[index_value_attr] = -1;
				g_p->Next[index_foot_attr] = -1;
			}	
		}
	}
	for( int im=0; im<m_Doc->clip_mlist->GetSize(); im++ )
		if( m_Doc->clip_mlist->GetMark( im ) == 0 )
			m_Doc->clip_mlist->SetStr( im, "" );

	// see if anything copied
	if( !g_op->GetSize() )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "There are no objects to insert!":"Нет объектов для вставки!");
		CWnd* pMain = AfxGetMainWnd();
		if (pMain != NULL)
		{
			CMenu* pMenu = pMain->GetMenu();
			CMenu* submenu = pMenu->GetSubMenu(1);	// "Edit" submenu
			submenu->EnableMenuItem( ID_EDIT_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
			pMain->DrawMenuBar();
		}
	}
	else
	{
		CWnd* pMain = AfxGetMainWnd();
		if (pMain != NULL)
		{
			CMenu* pMenu = pMain->GetMenu();
			CMenu* submenu = pMenu->GetSubMenu(1);	// "Edit" submenu
			submenu->EnableMenuItem( ID_EDIT_PASTE, MF_BYCOMMAND | MF_ENABLED );
			pMain->DrawMenuBar();
		}
	}
}

//===============================================================================================
void CFreePcbView::OnGroupCopy()
{
	GroupCopy(TRUE);
	m_paste_flags = m_sel_flags;
	SaveToFile( &(m_Doc->m_app_dir + "\\buf.cds") );
	m_page_copy = m_Doc->Pages.GetActiveNumber();
}
//===============================================================================================
void CFreePcbView::OnGroupLineUp()
{
	if( m_sel_flags != TEXT_ONLY )
	{
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
		int min_x= INT_MAX, min_y= INT_MAX,
			max_x=INT_MIN, max_y=INT_MIN, count=0;
		m_Doc->SelectGroupAttributes();
		int it = -1;
		for(CText * p=m_Doc->Attr->m_Reflist->GetNextText(&it); p; p=m_Doc->Attr->m_Reflist->GetNextText(&it) )
		{
			if( p->m_selected )
			{
				p->utility = 1;
				RECT pb = m_Doc->GetPartBounds( p, NULL );
				min_x = min( min_x, pb.left );
				max_x = max( max_x, pb.left );
				min_y = min( min_y, pb.bottom );
				max_y = max( max_y, pb.bottom );
				count++;
			}
			else
				p->utility = 0;
		}
		BOOL MOD_POLYLINES = 0;
		if (count == 0)
		{
			count = 0;
			MOD_POLYLINES = 1;
			for (int i = 0; i < m_Doc->m_outline_poly->GetSize(); i++)
			{
				CPolyLine* p = &m_Doc->m_outline_poly->GetAt(i);
				if (p->GetSideSel())
				{
					p->SetUtility(1);
					RECT pb = p->GetCornerBounds();
					min_x = min(min_x, pb.left);
					max_x = max(max_x, pb.left);
					min_y = min(min_y, pb.bottom);
					max_y = max(max_y, pb.bottom);
					count++;
				}
				else
					p->SetUtility(0);
			}
		}
		if (count < 2)
			return;
		int pt = 0;
		CText * fp;
		int fpo;
		do
		{
			int min_d = DEFAULT;	
			fp = NULL;
			fpo = -1;
			CancelSelection();
			if (MOD_POLYLINES == 0)
			{
				it = -1;
				for (CText* p = m_Doc->Attr->m_Reflist->GetNextText(&it); p; p = m_Doc->Attr->m_Reflist->GetNextText(&it))
				{
					if (p->utility)
					{
						RECT pb = m_Doc->GetPartBounds(p, NULL);
						int d = Distance(pb.left, pb.bottom, min_x, min_y);
						if (d < min_d)
						{
							min_d = d;
							fp = p;
						}
					}
				}
				if (fp)
				{
					RECT pb = m_Doc->GetPartBounds(fp, NULL);
					NewSelectP(fp, NULL);
					m_Doc->SelectGroupAttributes();
					if ((max_x - min_x) > (max_y - min_y))
						MoveGroup(min_x + (pt * (max_x - min_x) / (count - 1)) - pb.left, min_y - pb.bottom);
					else
						MoveGroup(min_x - pb.left, min_y + (pt * (max_y - min_y) / (count - 1)) - pb.bottom);
					fp->utility = 0;
					pt++;
				}
			}
			else
			{
				for (int i = 0; i < m_Doc->m_outline_poly->GetSize(); i++)
				{
					CPolyLine* p = &m_Doc->m_outline_poly->GetAt(i);
					if (p->GetUtility())
					{
						RECT pb = p->GetCornerBounds();
						int d = Distance(pb.left, pb.bottom, min_x, min_y);
						if (d < min_d)
						{
							min_d = d;
							fpo = i;
						}
					}
				}
				if (fpo >= 0)
				{
					CPolyLine* p = &m_Doc->m_outline_poly->GetAt(fpo);
					RECT pb = p->GetCornerBounds();
					id sid(ID_POLYLINE, ID_GRAPHIC, fpo, ID_SIDE, 0);
					NewSelect(NULL, &sid, 0, 0);
					SelectContour();
					m_Doc->SelectGroupAttributes();
					if ((max_x - min_x) > (max_y - min_y))
						MoveGroup(min_x + (pt * (max_x - min_x) / (count - 1)) - pb.left, min_y - pb.bottom);
					else
						MoveGroup(min_x - pb.left, min_y + (pt * (max_y - min_y) / (count - 1)) - pb.bottom);
					p->SetUtility(0);
					pt++;
				}
			}
		}while( fp || fpo >= 0 );

		CancelSelection();
		it = -1;
		for(CText * p=m_Doc->Attr->m_Reflist->GetNextText(&it); p; p=m_Doc->Attr->m_Reflist->GetNextText(&it) )
		{
			if( p->utility )
			{
				NewSelectP( p, NULL );
			}
		}
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		m_Doc->ProjectModified( TRUE );
		OnRangeCmds(NULL);
	}
}
//===============================================================================================
void CFreePcbView::OnGroupCut()
{
	OnGroupCopy();
	OnGroupDelete();
}
//===============================================================================================
// Remove all elements in group from project
//
void CFreePcbView::OnGroupDelete()
{
	DeleteGroup(0);
	Invalidate( FALSE );// (doubtful)//OnGroupDelete
}
//===============================================================================================
void CFreePcbView::DeleteGroup( BOOL wMerge )
{
	if( m_protect )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "This PCB is protected." : "Эта печатная плата защищена.", MB_ICONERROR );
		return;
	}
	if( getbit( m_sel_mask, SEL_MASK_PART ) == 0 )
	{
		int it = -1;
		for( CText * ref=m_Doc->Attr->m_Reflist->GetNextText(&it); ref; ref=m_Doc->Attr->m_Reflist->GetNextText(&it) )
			for( int get = ref->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
			{
				CPolyLine * p = &m_Doc->m_outline_poly->GetAt(get);
				if( p->GetSel() )
				{
					m_Doc->SelectPartAttributes( ref );
					break;
				}
			}
	}
	CArray<CText*> V_Array;
	V_Array.RemoveAll();
	CString ExclusiveParts = "";
	int it = -1;
	for( CText * ref=m_Doc->Attr->m_Reflist->GetNextText(&it); ref; ref=m_Doc->Attr->m_Reflist->GetNextText(&it) )
	{
		BOOL SEL = TRUE;
		for( int get = ref->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[index_part_attr] )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt(get);
			if( p->GetSel() == 0 )
			{
				SEL = 0;
				break;
			}
		}
		if( SEL )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt( ref->m_polyline_start );
			CText * V = p->Check( index_value_attr );
			CText * F = p->Check( index_foot_attr );
			if( V && F )
			{
				V_Array.Add(V);
				if( m_Doc->Pages.FindPart( &V->m_str, &F->m_str, 0, &V_Array ) == 0 )
				{
					CString VP = V->m_str + "@" + F->m_str;
					CArray<CString> * pathes = m_Doc->m_library.GetPathes( &VP );
					if( pathes == NULL )
					{
						ExclusiveParts += (VP + " (not found)\n");
					}
					else if( pathes->GetSize() == 0 )
					{
						ExclusiveParts += (VP + "\n");
					}
					else if( pathes->GetSize() == 1 )
					{
						CString path = pathes->GetAt(0);
						int rf = path.ReverseFind('\\');
						if( rf > 0 )
							path = path.Right( path.GetLength() - rf - 1 );
						if( path.CompareNoCase( m_Doc->m_name ) == 0 )
							ExclusiveParts += (VP + "\n");
					}
				}
			}
		}
	}
	if( ExclusiveParts.GetLength() )
	{
		if( m_Doc->m_library.IsEmpty() )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"It is not recommended to delete this part when the library is uninitialized. Press F1 to initialize it":
				"Не рекомендуется удалять ту или иную деталь, пока библиотека не инициализирована. Нажмите F1, чтобы инициализировать ее.", MB_ICONINFORMATION);
		}
		ExclusiveParts += "\n\n";
		int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
			(ExclusiveParts+".. is the last part "\
			"of this type and it is not used anywhere else except here. "\
			"If you delete it, it will no longer be in your inventory. "\
			"Are you sure to remove this part?"):
			(ExclusiveParts + "..— последняя деталь "\
			"этого типа, и она больше нигде не используется, кроме как здесь. "\
			"Если вы удалите ее, ее больше не будет в вашем инвентаре. "\
			"Вы уверены, что хотите удалить эту деталь ? "), MB_YESNO);
		if( ret == IDNO )
			return;
	}
	if( SaveUndoInfoForGroup( m_Doc->m_undo_list, TRUE, wMerge ) == IDCANCEL )
		return;
	CancelSelection();
	RemoveOrphanMergeNames();
	m_draw_layer = 0;// DeleteGroup
	m_Doc->ProjectModified( TRUE );
}
//===============================================================================================
void CFreePcbView::OnGroupPaste( BOOL bwDialog, BOOL bSaveMerges, int m_apply_def_w, BOOL bDrag)
{
	if( m_dragging_mode )
		return;
	if( m_protect )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "This PCB is protected." : "Эта печатная плата защищена.", MB_ICONERROR );
		return;
	}
#define ELEMENT_SELECTED 1
#define PASTE_FROM_FILE (m_paste_flags == ~0)

	int clk_start = clock(); //get time
	static int max_num_index = CLR_REF;
	
	//void * vp;
	// pointers to group lists
	CArray<CPolyLine> * g_op = &m_Doc->clip_outline_poly;
	// pointers to project lists
	CArray<CPolyLine> * op = m_Doc->m_outline_poly;
	//
	int max_pin_num = 0; // for paste pins
	// MarkAll Poly
	for( int ip=0; ip<g_op->GetSize(); ip++ )
	{
		g_op->GetAt(ip).SetUtility(0);
	}
	// copy merge
	if( bSaveMerges )
	{
		if( m_sel_merge_name.GetLength() == 0 )
			m_sel_merge_name = "Merge-1";
		for( int im=0; im<m_Doc->clip_mlist->GetSize(); im++ )
		{
			CString gM = m_Doc->clip_mlist->GetStr( im );
			//
			// if merge not used in the group 
			// (length == 0)
			if( gM.GetLength() == 0 )
				continue;
			if( m_Doc->m_mlist->GetIndex( gM ) >= 0 )
			{
				CString prefix, new_m;
				ParsePin( &m_sel_merge_name, &prefix );
				int iof = 0;
				for( int num = 1; iof>=0; num++ )
				{
					new_m.Format("%s%d", prefix, num );
					iof = m_Doc->m_mlist->GetIndex( new_m );
					if( iof == -1 )
					{
						iof = m_Doc->clip_mlist->GetIndex( new_m );
						if( iof >= im )
							iof = -1;
					}
				}
				m_Doc->clip_mlist->SetStr( im, new_m );
			}
		}
	}
	//
	CancelSelection();
	//
	m_sel_id.type = ID_MULTI;
	m_Doc->m_undo_list->NewEvent();
	double min_d = (double)INT_MAX*(double)INT_MAX;
	int min_x = INT_MAX;	// lowest-left point for dragging group
	int min_y = INT_MAX;
	int n_free_lines = 0;

	// mark all texts
	for( int ip=0; ip<m_Doc->Pages.GetNumPages(); ip++ )
	{
		for( int iatt=0; iatt<num_attributes; iatt++ )
		{
			CTextList * tl = m_Doc->Pages.GetTextList( ip, iatt );
			tl->MarkAllTexts(0);
		}
	}
	// now mark all texts which are referenced from the polylines from the buffer
	for( int ipo=0; ipo<m_Doc->clip_outline_poly.GetSize(); ipo++ )
	{
		int i_att = -1;
		for( CText * attr=m_Doc->ScanAttr( &m_Doc->clip_outline_poly.GetAt(ipo), &i_att ); attr; attr=m_Doc->ScanAttr( &m_Doc->clip_outline_poly.GetAt(ipo), &i_att ) )
			attr->utility = 2; // mark for texts which are referenced from the polylines from the buffer
	}
	CArray <CText*> Main_Part;
	Main_Part.SetSize(0);

	// add poly
	int grp_size = g_op->GetSize();
	int old_size = op->GetSize();
	if( grp_size > 0 )
	{
		// set flags
		setbit( m_sel_flags, FLAG_SEL_OP );
		setbit( m_sel_flags, FLAG_SEL_SIDE );

		//
		CArray <CString> NewPartNames;
		NewPartNames.SetSize(0);
		//
		int ism = 0;
		int m_polyline_start = -1;
		int idx[num_attributes] = {-1,-1,-1,-1,-1,-1};
		for( int g_ism=0; g_ism<grp_size; g_ism++ )
		{
			if( ism < old_size )
			{
				for(; ism<old_size; ism++ )
					if( (*op)[ism].m_visible == 0 )
						break;
			}
			if( ism == old_size )
			{
				op->SetSize( old_size + grp_size - g_ism );
			}
			CPolyLine * g_p = &(*g_op)[g_ism];
			if( g_p->GetNumCorners() < 2 )
				continue;
			CPolyLine * p = &(*op)[ism];
			p->SetDisplayList( m_Doc->m_dlist );
			// if it's copying, then 
			// we copy the picture 
			// if this is an insert 
			// from a file, then we 
			// do not add a picture, 
			// because it has already 
			// been added to it
			p->Copy( g_p, (PASTE_FROM_FILE?FALSE:TRUE) ); // paste from file? 0:1
			if( m_apply_def_w ) 
				p->SetW( abs(m_Doc->m_polyline_w) );
			if( PASTE_FROM_FILE ) // paste from file
				if( p->pAttr[index_pin_attr] )
					if( p->GetLayer() != LAY_PIN_LINE )
						p->SetLayer( LAY_PIN_LINE );
			p->Hide();
			///
			m_sel_id = p->GetId();
			m_sel_id.i = ism;
			p->SetId( &m_sel_id );
			
			if( p->GetLayer() >= m_Doc->m_num_layers )
				p->SetLayer( m_Doc->m_num_layers-1 );

			// reset pointers
			p->pAttr[index_part_attr]  = NULL;
			p->pAttr[index_value_attr]  = NULL;
			p->pAttr[index_foot_attr]  = NULL;

			// merge
			p->SetMerge(-1);
			p->SetMerge(-1, TRUE);
			if( bSaveMerges )
			{	
				CString gM = m_Doc->clip_mlist->GetStr( g_p->GetMerge() );
				int merge = m_Doc->m_mlist->AddNew( gM );
				p->SetMerge( merge );
				gM = m_Doc->clip_mlist->GetStr( g_p->GetMerge(TRUE) );
				merge = m_Doc->m_mlist->AddNew( gM );
				p->SetMerge( merge, TRUE );
			}
			int lim = p->GetNumCorners();
			int cl = p->GetClosed();
			BOOL getCoord = (p->GetLayer() == LAY_PIN_LINE || min_x == INT_MAX);
			int L = p->GetLayer();
			for( int is=0; is<lim; is++ )
			{		
				if( getCoord )
				{
					int x = p->GetX(is);
					int y = p->GetY(is);
					// update lower-left corner
					double d = x + y;
					if( d < min_d )
					{
						if( L == LAY_PIN_LINE )
							min_d = d;
						min_x = x;
						min_y = y;
					}
				}
				if( is < lim-1 || cl )
				{
					p->SetSideSel( is, ELEMENT_SELECTED );
					m_sel_count++;
				}
			}
			// select el
			m_sel_id.sst = ID_SIDE;
			m_sel_id.ii = 0;
			
			int ADD = 0;
			if( getbit( m_paste_flags, FLAG_SEL_PART ) == 0 )
			{// ADD = 0
			}// ADD = 0
			else if( m_paste_flags == PINS_ONLY )
			{// ADD = 0
			}// ADD = 0
			else if( g_p->pAttr[index_part_attr] )
			{
				ADD = 1;
			}
			if( ADD )
			{
				setbit( m_sel_flags, FLAG_SEL_PART );
				CText * t = g_p->pAttr[index_part_attr];
				if( t->utility%2 == 0 )
				{
					t->utility++;// mark
					//
					if( PASTE_FROM_FILE ) // paste from file
						t->InVisible();
					//int ipg = m_Doc->Pages.GetActiveNumber();
					//int st = t->m_polyline_start;
					//if( st == -1 )
					//	ASSERT(0);
					//---------------------------------------------------------------------
					// create new ref
					// use next available ref
					CString new_ref, prefix, mod_ref, suff;
					CTextList * tl = m_Doc->Attr->m_Reflist;
					if( clock() > (clk_start+4000) )// max time = 4sec
					{
						ParseRef( &t->m_str, &prefix, &suff );
						max_num_index--;
						new_ref.Format( "%s%d%s", prefix, max_num_index, suff );
					}
					else
					{
						int ref_index = ParseRef( &t->m_str, &prefix, &suff );
						if( suff.GetLength() )
						{
							new_ref = t->m_str;
							mod_ref = new_ref;
						}
						else
						{
							new_ref = prefix;
							if( ref_index > 0 )
								new_ref.Format( "%s%d", prefix, ref_index );
							mod_ref = new_ref + "&";// include multiparts, for example DA1.1 or DA1-1
						}
						CText * get = NULL;
						if( m_Doc->Pages.IsThePageIncludedInNetlist( m_Doc->Pages.GetActiveNumber() ) )
						{
							m_Doc->Pages.FindPart( &mod_ref, &get, TRUE, TRUE );
						}
						else
						{
							int it2 = -1;
							get = m_Doc->Attr->m_Reflist->GetText( &mod_ref, &it2 );
						}
						if( get == NULL )
							for( int ipn=0; ipn<NewPartNames.GetSize(); ipn++ )
							{
								if( NewPartNames[ipn].CompareNoCase( new_ref ) == 0 )
								{
									get = t;
									break;
								}
								else if( NewPartNames[ipn].Left(new_ref.GetLength()).CompareNoCase( new_ref ) == 0 && 
									(NewPartNames[ipn].Find(".") == new_ref.GetLength() || NewPartNames[ipn].Find("-") == new_ref.GetLength()) )
								{
									get = t;
									break;
								}
								else if( new_ref.Left(NewPartNames[ipn].GetLength()).CompareNoCase( NewPartNames[ipn] ) == 0 && 
									(new_ref.Find(".") == NewPartNames[ipn].GetLength() || new_ref.Find("-") == NewPartNames[ipn].GetLength()) )
								{
									get = t;
									break;
								}
								else if( suff.GetLength() == 0 )
								{
									if( NewPartNames[ipn].Left( mod_ref.GetLength() ).CompareNoCase( new_ref+"." ) == 0 ||
										NewPartNames[ipn].Left( mod_ref.GetLength() ).CompareNoCase( new_ref+"-" ) == 0 )
									{
										get = t;
										break;
									}
								}
							}
						if( get )
						{
							CString psuff="";
							int getSuff = 1;
							if( suff.GetLength() )
								getSuff = ParsePin( &suff, &psuff );
							int sgL = suff.GetLength();
							//
							// multi-part naming mode
							// either increase the suffix (=1)
							// or increase the number (=0)
							int pas_mode = 0;//1;
							//
							//
							for( int x_num=((sgL&&pas_mode)?ref_index:1), isuff=(pas_mode?1:getSuff); get; ((sgL&&pas_mode)?isuff:x_num)++ )
							{
								if( sgL )
								{
									if( isuff )
										new_ref.Format( "%s%d%s%d", prefix, x_num, psuff, isuff );
									else
										new_ref.Format( "%s%d%s", prefix, x_num, psuff );
									mod_ref = new_ref;
								}
								else
								{
									new_ref.Format( "%s%d", prefix, x_num );
									mod_ref = new_ref + "&"; // include multiparts, for example DA1.1 or DA1-1
								}
								if( m_Doc->Pages.IsThePageIncludedInNetlist( m_Doc->Pages.GetActiveNumber() ) )
								{
									m_Doc->Pages.FindPart( &mod_ref, &get, TRUE, TRUE );
									if( get == NULL )
										if( sgL )
										{
											CString delSuff;
											delSuff.Format( "%s%d", prefix, x_num );
											m_Doc->Pages.FindPart( &delSuff, &get, TRUE, TRUE );
											if( get )
											{
												x_num++;
												if( pas_mode )
													isuff = 1;
											}
										}
								}
								else
								{
									int it2 = -1;
									get = m_Doc->Attr->m_Reflist->GetText( &mod_ref, &it2 );
									if( get == NULL )
										if( sgL )
										{
											CString delSuff;
											delSuff.Format( "%s%d", prefix, x_num );
											it2 = -1;
											get = m_Doc->Attr->m_Reflist->GetText( &delSuff, &it2 );
											if( get )
											{
												x_num++;
												if( pas_mode )
													isuff = 1;
											}
										}
								}
								if( get == NULL )
									for( int ipn=0; ipn<NewPartNames.GetSize(); ipn++ )
									{
										if( NewPartNames[ipn].CompareNoCase( new_ref ) == 0 )
										{
											get = t;
											break;
										}
										else if( NewPartNames[ipn].Left(new_ref.GetLength()).CompareNoCase( new_ref ) == 0 && 
											(NewPartNames[ipn].Find(".") == new_ref.GetLength() || NewPartNames[ipn].Find("-") == new_ref.GetLength()) )
										{
											get = t;
											break;
										}
										else if( new_ref.Left(NewPartNames[ipn].GetLength()).CompareNoCase( NewPartNames[ipn] ) == 0 && 
											(new_ref.Find(".") == NewPartNames[ipn].GetLength() || new_ref.Find("-") == NewPartNames[ipn].GetLength()) )
										{
											get = t;
											break;
										}
										else if( suff.GetLength() == 0 )
										{
											if( NewPartNames[ipn].Left( mod_ref.GetLength() ).CompareNoCase( new_ref+"." ) == 0 ||
												NewPartNames[ipn].Left( mod_ref.GetLength() ).CompareNoCase( new_ref+"-" ) == 0 )
											{
												get = t;
												break;
											}
										}
									}
							}	
						}
					}
					NewPartNames.Add(new_ref);

					if( PASTE_FROM_FILE ) // paste from file
						m_sel_text = t;
					else
					{
						for( m_sel_text = tl->GetNextInvisibleText( &idx[index_part_attr] ); 
							 m_sel_text;
							 m_sel_text = tl->GetNextInvisibleText( &idx[index_part_attr] ))
						{
							if( m_sel_text->utility < 2 )
								break;
						}
					}
					if( m_sel_text == NULL )
						m_sel_text = tl->AddText( t->m_x, t->m_y, t->m_angle,
										t->m_layer, t->m_font_size, t->m_stroke_width, m_Doc->m_default_font, &new_ref, 0 );
					else
					{
						m_sel_text->m_str = new_ref;
						m_sel_text->m_nchars = new_ref.GetLength();
						m_sel_text->m_x = t->m_x;
						m_sel_text->m_y = t->m_y;
						m_sel_text->m_angle = t->m_angle;
						m_sel_text->m_layer = t->m_layer;
						m_sel_text->m_font_size = t->m_font_size;
						m_sel_text->m_stroke_width = t->m_stroke_width;
					}
					m_sel_text->m_pdf_options = t->m_pdf_options;
					if( m_apply_def_w && m_sel_text->m_font_size ) 
					{
						m_sel_text->m_font_size = m_attr_size.H_ref;
						m_sel_text->m_stroke_width = m_attr_size.W_ref;
						//clrbit( m_sel_text->m_pdf_options, COMPRESSED );
						//clrbit( m_sel_text->m_pdf_options, UNCLENCHED );
						//clrbit( m_sel_text->m_pdf_options, ITALIC );
					}
					Main_Part.Add( m_sel_text );
					//
					m_sel_text->m_merge = -1;
					m_sel_text->m_submerge = -1;
					if( bSaveMerges )
					{
						CString gM = m_Doc->clip_mlist->GetStr( t->m_merge );
						int merge = m_Doc->m_mlist->AddNew( gM );
						m_sel_text->m_merge = merge;
						gM = m_Doc->clip_mlist->GetStr( t->m_submerge );
						merge = m_Doc->m_mlist->AddNew( gM );
						m_sel_text->m_submerge = merge;
					}
					

					// remove reflists
					if( new_ref.Compare(t->m_str) )
					{
						// clear ref list indexes
						for( int i=REF_LIST_INDEX; i<32; i++ ) // no need to clear indexes, this is useless
							clrbit( m_sel_text->m_pdf_options, i );
					}
					
					m_sel_text->m_font_number = t->m_font_number;
					m_sel_text->m_polyline_start = -1;
					p->pAttr[index_part_attr] = m_sel_text;
					// set flags
					setbit( m_sel_flags, FLAG_SEL_TEXT );
					// select el
					m_sel_id.Set(ID_TEXT_DEF);
					m_sel_text->m_selected = ELEMENT_SELECTED;
					m_sel_count++;
					//---------------------------------------------------------------------
					// create new value
					if( g_p->pAttr[index_value_attr] )
					{
						tl = m_Doc->Attr->m_Valuelist;
						t = g_p->pAttr[index_value_attr];
						if( PASTE_FROM_FILE ) // paste from file
						{
							m_sel_text = t;
							m_sel_text->InVisible();
						}
						else
						{
							for( m_sel_text = tl->GetNextInvisibleText( &idx[index_value_attr] ); 
								 m_sel_text;
								 m_sel_text = tl->GetNextInvisibleText( &idx[index_value_attr] ))
							{
								if( m_sel_text->utility < 2 )
									break;
							}
						}
						if( m_sel_text == NULL )
							m_sel_text = tl->AddText( t->m_x, t->m_y, t->m_angle,
											t->m_layer, t->m_font_size, t->m_stroke_width, m_Doc->m_default_font, &t->m_str, 0 );
						else
						{
							m_sel_text->m_x = t->m_x;
							m_sel_text->m_y = t->m_y;
							m_sel_text->m_angle = t->m_angle;
							m_sel_text->m_layer = t->m_layer;
							m_sel_text->m_font_size = t->m_font_size;
							m_sel_text->m_stroke_width = t->m_stroke_width;
							m_sel_text->m_str = t->m_str;
							m_sel_text->m_nchars = t->m_nchars;
						}
						m_sel_text->m_pdf_options = t->m_pdf_options;
						if( m_apply_def_w && m_sel_text->m_font_size ) 
						{
							m_sel_text->m_font_size = m_attr_size.H_value;
							m_sel_text->m_stroke_width = m_attr_size.W_value;
							//clrbit( m_sel_text->m_pdf_options, COMPRESSED );
							//clrbit( m_sel_text->m_pdf_options, UNCLENCHED );
							//clrbit( m_sel_text->m_pdf_options, ITALIC );
						}
						m_sel_text->m_merge = -1;
						m_sel_text->m_submerge = -1;
						if( bSaveMerges )
						{
							CString gM = m_Doc->clip_mlist->GetStr( t->m_merge );
							int merge = m_Doc->m_mlist->AddNew( gM );
							m_sel_text->m_merge = merge;
							gM = m_Doc->clip_mlist->GetStr( t->m_submerge );
							merge = m_Doc->m_mlist->AddNew( gM );
							m_sel_text->m_submerge = merge;
						}
						
						m_sel_text->m_font_number = t->m_font_number;
						m_sel_text->m_polyline_start = -1;
						p->pAttr[index_value_attr] = m_sel_text;
						m_sel_text->m_selected = ELEMENT_SELECTED;
						m_sel_count++;
					}
					else
						p->pAttr[index_value_attr] = NULL;
					//---------------------------------------------------------------------
					// create new footprint
					if( g_p->pAttr[index_foot_attr] )
					{
						tl = m_Doc->Attr->m_Footlist;
						t = g_p->pAttr[index_foot_attr];
						if( PASTE_FROM_FILE ) // paste from file
						{
							m_sel_text = t;
							m_sel_text->InVisible();
						}
						else
						{
							for( m_sel_text = tl->GetNextInvisibleText( &idx[index_foot_attr] ); 
								 m_sel_text;
								 m_sel_text = tl->GetNextInvisibleText( &idx[index_foot_attr] ))
							{
								if( m_sel_text->utility < 2 )
									break;
							}
						}
						if( m_sel_text == NULL )
							m_sel_text = tl->AddText( t->m_x, t->m_y, t->m_angle,
											t->m_layer, t->m_font_size, t->m_stroke_width, m_Doc->m_default_font, &t->m_str, 0 );
						else
						{
							m_sel_text->m_x = t->m_x;
							m_sel_text->m_y = t->m_y;
							m_sel_text->m_angle = t->m_angle;
							m_sel_text->m_layer = t->m_layer;
							m_sel_text->m_font_size = t->m_font_size;
							m_sel_text->m_stroke_width = t->m_stroke_width;
							m_sel_text->m_str = t->m_str;
							m_sel_text->m_nchars = t->m_nchars;
						}
						m_sel_text->m_pdf_options = t->m_pdf_options;
						if( m_apply_def_w && m_sel_text->m_font_size ) 
						{
							m_sel_text->m_font_size = m_attr_size.H_foot;
							m_sel_text->m_stroke_width = m_attr_size.W_foot;
							//clrbit( m_sel_text->m_pdf_options, COMPRESSED );
							//clrbit( m_sel_text->m_pdf_options, UNCLENCHED );
							//clrbit( m_sel_text->m_pdf_options, ITALIC );
						}
						m_sel_text->m_merge = -1;
						m_sel_text->m_submerge = -1;
						if( bSaveMerges )
						{
							CString gM = m_Doc->clip_mlist->GetStr( t->m_merge );
							int merge = m_Doc->m_mlist->AddNew( gM );
							m_sel_text->m_merge = merge;
							gM = m_Doc->clip_mlist->GetStr( t->m_submerge );
							merge = m_Doc->m_mlist->AddNew( gM );
							m_sel_text->m_submerge = merge;
						}
						
						m_sel_text->m_font_number = t->m_font_number;
						m_sel_text->m_polyline_start = -1;
						p->pAttr[index_foot_attr] = m_sel_text;
						m_sel_text->m_selected = ELEMENT_SELECTED;
						m_sel_count++;
					}
					else
						p->pAttr[index_foot_attr] = NULL;
					//---------------------------------------------------------------------
					m_polyline_start = ism;
					//---------------------------------------------------------------------
				}
				else
				{
					if( m_polyline_start > -1 )
					{
						p->pAttr[index_part_attr]  = op->GetAt(m_polyline_start).pAttr[index_part_attr] ;
						p->pAttr[index_value_attr] = op->GetAt(m_polyline_start).pAttr[index_value_attr];
						p->pAttr[index_foot_attr]  = op->GetAt(m_polyline_start).pAttr[index_foot_attr] ;
					}
					else
						AfxMessageBox(G_LANGUAGE == 0 ? 
							"Error code 238. Please report the error to freepcb-2":
							"Error code 238. Пожалуйста, сообщите об этой ошибке разработчику", MB_ICONERROR);
				}
			}
			else
			{
				n_free_lines++;
				m_polyline_start = -1;
				if( m_paste_flags == PINS_ONLY && g_p->pAttr[index_part_attr])
				{
					if( g_p->pAttr[index_part_attr]->utility%2 == 0 )
						NewPartNames.Add( g_p->pAttr[index_part_attr]->m_str );
					g_p->pAttr[index_part_attr]->utility = 3; // >2!
					p->pAttr[index_part_attr]  = g_p->pAttr[index_part_attr] ;
					p->pAttr[index_value_attr] = g_p->pAttr[index_value_attr];
					p->pAttr[index_foot_attr]  = g_p->pAttr[index_foot_attr] ;
				}
				else
				{
					p->pAttr[index_part_attr]  = NULL;
					p->pAttr[index_value_attr]  = NULL;
					p->pAttr[index_foot_attr]  = NULL;
				}
			}
			// paste attributes
			for( int ia=0; ia<num_attributes; ia++ )
			{
				if( ia == index_pin_attr || ia == index_net_attr || ia == index_desc_attr )
				{
					if( g_p->pAttr[ia] )
					{	
						CText * t = g_p->pAttr[ia];
						CTextList * tl = m_Doc->GetTextListbyLayer( t->m_layer );
						if (ia == index_pin_attr)
							tl = m_Doc->Attr->m_Pinlist;
						if( tl && m_Doc->Pages.IsAttr(t) >= 0 )
						{
							CString new_str = t->m_str;
							 
							//check printable area
							if( tl == m_Doc->Attr->m_pDesclist )
							{
								int fa = new_str.Find("PrintableArea");
								if( fa == new_str.GetLength()-13 && fa != -1 )
								{
									int pos = 0, it = -1;
									CString search_str = new_str.Tokenize("|",pos);
									int sl = search_str.GetLength();
									for( CText * dsctxt=tl->GetNextText( &it ); dsctxt; dsctxt=tl->GetNextText( &it ) )
									{
										if( dsctxt->m_str.Left(sl).Compare( search_str ) == 0 )
										{
											new_str = "";
											break;
										}
									}
									if( new_str.GetLength() == 0 )
									{
										if(PASTE_FROM_FILE)
											t->InVisible();
										p->pAttr[ia] = NULL;
										continue;
									}
								}
							}
							if( !PASTE_FROM_FILE ) // m_paste_flags != ~0
							{
								if( t->m_str.Right(3) == "BLK" )
								{
									CString Suff = "";
									m_Doc->ExtractBLK( &new_str, &Suff );
									Suff = GenNextSuffix( &Suff );
									new_str.Format( "Please click by'outline to update'this text'"CP_SUFFIX" %s'"COMMAND" BLK", Suff, Suff );
								}
								else if( m_paste_flags == PINS_ONLY )
								{
									if( ia == index_pin_attr )
									{
										CString prefix;
										//
										// max pin number of group
										int pin_ok;
										do
										{
											pin_ok = 0;
											ParsePin( &t->m_str, &prefix );
											if( prefix.Find(",") > 0 || prefix.Find("-") > 0 )
												prefix = "";
											new_str.Format( "%s%d", prefix, ++max_pin_num );
											for( int g_i=0; g_i<grp_size; g_i++ )
											{
												CPolyLine * gp = g_p;
												if( gp->pAttr[index_part_attr] )
												{
													for( int get=gp->pAttr[index_part_attr]->m_polyline_start; get>=0; get=gp->Next[index_part_attr] )
													{
														gp = &m_Doc->m_outline_poly->GetAt(get);
														if( gp->pAttr[index_pin_attr] )
															if( new_str.CompareNoCase( gp->pAttr[index_pin_attr]->m_str ) == 0 )
																pin_ok = 1;
															else if( gp->pAttr[index_pin_attr]->m_str.GetLength() >= 3 ) // for exmp 1,2 or 4-8 (pins array)
															{
																if( gp->pAttr[index_pin_attr]->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
																{
																	CArray<CString> arr;
																	ParseMultiPin( &gp->pAttr[index_pin_attr]->m_str, &arr );
																	for( int a=0; a<arr.GetSize(); a++ )
																	{
																		if( new_str.CompareNoCase( arr[a] ) == 0 )
																		{
																			pin_ok = 1;
																			break;
																		}
																	}
																}
															}
													}
													break; // OUT
												}
											}
										} while( pin_ok );
									}
									else if( p->pAttr[index_pin_attr] && ia == index_desc_attr )
									{
										new_str = OLD_NO_FP;
									}
								}
							}
							//
							if( PASTE_FROM_FILE ) // paste from file
							{
								m_sel_text = t;
								m_sel_text->InVisible();
							}
							else
							{
								for( m_sel_text = tl->GetNextInvisibleText( &idx[ia] );
									 m_sel_text;
									 m_sel_text = tl->GetNextInvisibleText( &idx[ia] ))
								{
									if( m_sel_text->utility < 2 )
										break;
								}
							}
							if( m_sel_text == NULL )
								m_sel_text = tl->AddText( t->m_x, t->m_y, t->m_angle,
								t->m_layer, t->m_font_size, t->m_stroke_width, m_Doc->m_default_font,
								&new_str, 0 );
							else
							{
								m_sel_text->m_x = t->m_x;
								m_sel_text->m_y = t->m_y;
								m_sel_text->m_angle = t->m_angle;
								m_sel_text->m_layer = t->m_layer;
								m_sel_text->m_font_size = t->m_font_size;
								m_sel_text->m_stroke_width = t->m_stroke_width;
								m_sel_text->m_str = new_str;
								m_sel_text->m_nchars = new_str.GetLength();
							}
							m_sel_text->m_pdf_options = t->m_pdf_options;
							if( m_apply_def_w && m_sel_text->m_font_size ) 
							{
								if( ia == index_pin_attr )
								{
									m_sel_text->m_font_size = m_attr_size.H_pin;
									m_sel_text->m_stroke_width = m_attr_size.W_pin;
								}
								else if( ia == index_net_attr )
								{
									m_sel_text->m_font_size = m_attr_size.H_net;
									m_sel_text->m_stroke_width = m_attr_size.W_net;
								}
								else if( ia == index_desc_attr )
								{
									m_sel_text->m_font_size = m_attr_size.H_pindesc;
									m_sel_text->m_stroke_width = m_attr_size.W_pindesc;
								}
								//clrbit( m_sel_text->m_pdf_options, COMPRESSED );
								//clrbit( m_sel_text->m_pdf_options, UNCLENCHED );
								//clrbit( m_sel_text->m_pdf_options, ITALIC );
							}
							m_sel_text->m_merge = -1;
							m_sel_text->m_submerge = -1;
							if( bSaveMerges )
							{
								CString gM = m_Doc->clip_mlist->GetStr( t->m_merge );
								int merge = m_Doc->m_mlist->AddNew( gM );
								m_sel_text->m_merge = merge;
								gM = m_Doc->clip_mlist->GetStr( t->m_submerge );
								merge = m_Doc->m_mlist->AddNew( gM );
								m_sel_text->m_submerge = merge;
							}
							
							m_sel_text->m_font_number = t->m_font_number;
							m_sel_text->m_polyline_start = ism;
							p->Next[ia] = -1;

							// set flags
							setbit( m_sel_flags, FLAG_SEL_TEXT );

							// select el
							m_sel_id.Set(ID_TEXT_DEF);
							m_sel_text->m_selected = ELEMENT_SELECTED;
							m_sel_count++;

							// assign
							p->pAttr[ia] = m_sel_text;
						}
						else
							p->pAttr[ia] = NULL;
					}
					else
						p->pAttr[ia] = NULL;
				}
			}
			ism++;
		}
	}

	// save undo info
	SaveUndoInfoForGroup( m_Doc->m_undo_list );
	//
	// 
	for( int ism=0; ism<op->GetSize(); ism++ )
	{
		(*op)[ism].RecalcRectC(0);
		if( (*op)[ism].GetSel() )
		{
			
			//
			RECT r = rect(0,0,0,0);
			(*op)[ism].ModifyGroupRect(&r);
			(*op)[ism].Show();
			//
			for( int ia=0; ia<num_attributes; ia++ )
			{
				if( (*op)[ism].pAttr[ia] )
					if( (*op)[ism].pAttr[ia]->m_selected )
						(*op)[ism].pAttr[ia]->MakeVisible();
			}
		}
	}
	// set indexes
	int it = -1;
	for( CText * t = m_Doc->Attr->m_Reflist->GetNextText(&it); t; t = m_Doc->Attr->m_Reflist->GetNextText(&it) )
		if( t->m_selected || t->utility )
			m_Doc->AttributeIndexing( t, index_part_attr );
	// 
	if( bDrag )
	{
		if( min_x == INT_MAX || min_y == INT_MAX )
			AfxMessageBox(G_LANGUAGE == 0 ? "No items to drag":"Нет элементов для перетаскивания");
		else
		{
			SetFKText( CUR_GROUP_SELECTED );
			int new_x = (m_Doc->m_dlist->m_org_x + m_Doc->m_dlist->m_max_x)*m_pcbu_per_wu/2;
			int new_y = (m_Doc->m_dlist->m_org_y + m_Doc->m_dlist->m_max_y)*m_pcbu_per_wu/2;
			new_x -= (new_x%(int)m_Doc->m_part_grid_spacing);
			new_y -= (new_y%(int)m_Doc->m_part_grid_spacing);
			int mnew_x = new_x - min_x;
			int mnew_y = new_y - min_y;
			//
			MoveGroup( mnew_x, mnew_y );
			CPoint mpnt;
			RECT r;
			GetWindowRect( &r );
			mpnt.x = (r.left + r.right)/2;
			mpnt.y = (r.top + r.bottom)/2;
			SetCursorPos( mpnt.x, mpnt.y );
			m_last_cursor_point = m_Doc->m_dlist->ScreenToPCB(mpnt);
			m_last_mouse_point = m_last_cursor_point;
			//
			// Try to find similar part
			if( PASTE_FROM_FILE && Main_Part.GetSize() && n_free_lines == 0 )
			{
				for( int mp=0; mp<Main_Part.GetSize(); mp++ )
				{
					CText * SP = FindSimilarPart( Main_Part[mp] );
					if( SP )
					{
						m_Doc->UpdatePartPattern( Main_Part[mp], SP );
					}
				}
			}
			//
			StartDraggingGroup( TRUE, new_x, new_y );
			m_draw_layer = LAY_BACKGND; // grouppaste
			Invalidate( FALSE );// (doubtful)
			//
			if( PASTE_FROM_FILE )
				m_Doc->clip_outline_poly.RemoveAll();
		}
	}
	else
	{
		if( m_sel_count )
			SetCursorMode( CUR_GROUP_SELECTED );
		HighlightGroup();
		GroupCopy(TRUE);
		Invalidate(FALSE);
	}
	m_Doc->ProjectModified( TRUE );

#undef ELEMENT_SELECTED
#undef PASTE_FROM_FILE
}
//===============================================================================================
void CFreePcbView::OnGroupStaticHighlight()
{
	m_Doc->m_dlist->SetStatic( EL_STATIC );
	CancelSelection();
	Invalidate( FALSE);
}
//===============================================================================================
void CFreePcbView::OnGroupCancelHighlight()
{
	GroupCancelHighlight();
	Invalidate( FALSE );// (doubtful)
}
void CFreePcbView::OnGroupCancelSel()
{
	int thP = m_Doc->Pages.GetActiveNumber();
	for( int i=0; i<m_Doc->Pages.GetNumPages(); i++ )
	{
		m_Doc->SwitchToPage(i);
		CancelSelection(0);
	}
	m_Doc->SwitchToPage(thP);
}
void CFreePcbView::GroupCancelHighlight()
{
	if( m_Doc->last_el )
		return; // protection
	m_Doc->m_dlist->CancelHighLight( TRUE );
	SetCursorMode(CUR_NONE_SELECTED);
}
void CFreePcbView::OnApproximationArc()
{
	ApproximArc();
	m_Doc->m_dlist->CancelHighLight();
	SetCursorMode( CUR_GROUP_SELECTED );
	HighlightGroup();
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = LAY_BACKGND;
	Invalidate( FALSE );// (doubtful)//OnApproximationArc
}
// SEL BY ATTR
void CFreePcbView::OnSelectPartsByAttr()
{
	SelectByAttr(0);
}
void CFreePcbView::OnSelectPolyByAttr()
{
	SelectByAttr(1);
}
void CFreePcbView::OnSelectObjectByAttr()
{
	if( m_sel_text )
		if( m_sel_text->m_polyline_start >= 0 )
			if( m_sel_text->m_polyline_start < m_Doc->m_outline_poly->GetSize() )
				if( m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start).Check( index_part_attr ) )
					SelectByAttr(0);
				else
					SelectByAttr(1);
}
void CFreePcbView::SelectByAttr( BOOL bPOLY )
{
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if( !tl )
			continue;
		int it = -1;
		for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
		{
			if( t->isVISIBLE && t->dl_el )
				if( t->m_selected )
				{
					if( t->m_polyline_start >= 0 )
						if( bPOLY )
						{
							if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) == 0 )
							{
								id pID( ID_POLYLINE, ID_GRAPHIC, t->m_polyline_start, ID_SIDE, -1 );
								if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_net_attr ) ||
									m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_pin_attr ) ||
									m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_desc_attr ))
								{
									id ID( ID_TEXT_DEF );
									UnSelect( t, &ID, 0, 0 );
									for( int ii=0; ii<m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetNumSides(); ii++ )
									{
										pID.ii = ii;
										NewSelect( NULL, &pID, 0, 0 );
									}
								}
							}
						}
						else if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) )
						{
							if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetSel() == 0 )
							{
								id ID( ID_TEXT_DEF );
								UnSelect( t, &ID, 0, 0 );
								m_Doc->SelectPart( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_part_attr], 0, 0 );
							}
						}
				}
		}
	}
	clrbit( m_sel_flags, FLAG_SEL_TEXT );
	m_Doc->m_dlist->CancelHighLight();
	HighlightGroup();
	if( m_cursor_mode == CUR_GROUP_SELECTED )
		SetFKText( CUR_GROUP_SELECTED );
	else
		SetCursorMode( CUR_GROUP_SELECTED );
	Invalidate( FALSE );// (doubtful)
}
//===============================================================================================
void CFreePcbView::SaveToFile( CString * fileS )
{
	CString filename = *fileS;
	if( filename.Right(3).MakeLower() != "cds" )
	{
		CString S = filename;
		filename.Format( "%s.cds", S );
	}
	CString pathname = filename;
	// write project file
	CStdioFile pcb_file;
	int err = pcb_file.Open( pathname, CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening partlist file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? "Unable to save file %s":"Не удалось сохранить файл %s", pathname);
		AfxMessageBox( mess );
	}
	else
	{
		// write clipboard to file
		try
		{
			int rf = filename.ReverseFind('\\');
			if( rf > 0 )
			{
				CString pr_name = filename.Right( filename.GetLength()-rf-1 );
				m_Doc->WriteOptions( &pcb_file, 0, &pr_name );
				m_Doc->WriteOutlinesPoly( &pcb_file, TRUE );
				pcb_file.WriteString( "[end]\n" );
				pcb_file.Close();

				pathname = filename.Left(rf);
				pathname += "\\related_files";
				struct _stat buf;
				if( _stat( pathname, &buf ) )
				{
					CString str;
					int err = _mkdir( pathname );
					if( err )
					{
						str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"":"Невозможно создать папку \"%s\"", pathname);
						AfxMessageBox( str, MB_OK );
					}
					pathname += "\\pictures";
					if( _stat( pathname, &buf ) )
					{
						CString str;
						int err = _mkdir( pathname );
						if( err )
						{
							str.Format(G_LANGUAGE == 0 ? "Unable to create folder \"%s\"" : "Невозможно создать папку \"%s\"", pathname );
							AfxMessageBox( str, MB_OK );
						}
					}
				}
				int PicFail = 0;
				pathname = filename.Left(rf);
				{
					CFileFind finder;
					BOOL bWorking = finder.FindFile( pathname + "\\*.cds" );
					while (bWorking)
					{
						bWorking = finder.FindNextFile();
						CString fn = finder.GetFilePath();
						CString pic_name = finder.GetFileName();
						if( !finder.IsDirectory() && !finder.IsDots() &&
							pic_name.Compare( filename.Right( pic_name.GetLength() ) ) )
						{
							PicFail = 1;
							AfxMessageBox(G_LANGUAGE == 0 ? 
								("The concept of the circuit editor does not "\
								"allow saving several circuit design files in one folder. "\
								"There is already a cds file in this folder. Select a "\
								"different folder to save the file so as not to lose the "\
								"images attached to the schematic files."):
								("Концепция редактора схем не "\
								"позволяет сохранять несколько файлов схем в одной папке. "\
								"В этой папке уже есть файл cds. Выберите "\
								"другую папку для сохранения файла, чтобы не потерять "\
								"изображения, прикрепленные к файлам схем."));
							break;
						}
					}
				}
				pathname += rel_f_pictures;
				rf = m_Doc->m_pcb_full_path.ReverseFind('\\');
				CString sourcename = m_Doc->m_pcb_full_path.Left(rf);
				sourcename += rel_f_pictures;
				if( sourcename.CompareNoCase(pathname) )
				{
					BOOL PICOK = 1;
					CFileFind finder;
					BOOL bWorking = finder.FindFile( sourcename + "*" );
					while (bWorking)
					{
						bWorking = finder.FindNextFile();
						CString fn = finder.GetFilePath();
						CString pic_name = finder.GetFileName();
						if( !finder.IsDirectory() && !finder.IsDots() )
						{
							if (m_Doc->m_dlist->FindSource(&fn) >= 0)
							{
								SetFileAttributes(pathname + pic_name, FILE_ATTRIBUTE_NORMAL);
								remove(pathname + pic_name);
								PICOK &= CopyFile(fn, pathname + pic_name, PicFail);
							}
						}
					}
					if (!PICOK)
					{
						CString ps;
						ps.Format(G_LANGUAGE == 0 ?
							"Unable to copy pictures.\nsource: %s\ndestination: %s" :
							"Невозможно скопировать некоторые изображения.\nисточник: %s\nназначение: %s", sourcename, pathname);
						AfxMessageBox(ps, MB_ICONINFORMATION);
					}
				}
			}
		}
		catch( CString * err_str )
		{
			// error
			AfxMessageBox( *err_str );
			delete err_str;
			CDC * pDC = GetDC();
			OnDraw( pDC );
			ReleaseDC( pDC );
			return;
		}
	}
}
//===============================================================================================
void CFreePcbView::OnGroupSaveToFile()
{
	// Copy group to pseudo-clipboard
	OnGroupCopy();
	
	// force old-style file dialog by setting size of OPENFILENAME struct
	//
	CString filename = m_Doc->RunFileDialog( 0, "sav" ); 
	if ( filename.GetLength() )
	{
		SaveToFile( &filename );
	}
}
//===============================================================================================
void CFreePcbView::OnEditCopy()
{
	static int bWar = 0;
	if( !m_Doc->m_project_open )
		return;
	if( getbit( m_sel_mask, SEL_MASK_PART ) == 0 && bWar == 0 )
	{
		bWar = 1;
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"In the disabled mode of the part mask, the copied part will be destroyed into primitives. To preserve the part on insertion, first enable the selection of the parts in the selection mask on the left of the window.":
			"В отключенном режиме маски детали скопированная деталь будет расформирована на примитивы. Чтобы сохранить деталь при вставке, сначала включите выделение деталей в маске выбора в левой части окна.");
	}
	OnGroupCopy();
}
//===============================================================================================
void CFreePcbView::OnEditPaste()
{
	if( !m_Doc->m_project_open )
		return;
	//if( m_sel_merge_name.GetLength() )
	//	OnGroupPaste(FALSE,TRUE);
	//else
	if( m_Doc->clip_outline_poly.GetSize() == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Objects will be pasted from the clipboard":
			"Объекты будут вставлены из буфера обмена.");
		m_Doc->PasteFromFile( (m_Doc->m_app_dir + "\\buf.cds"), 0 );
	}
	else
		OnGroupPaste(FALSE);
	int gan = m_Doc->Pages.GetActiveNumber();
	CTextList * tl = m_Doc->Pages.GetTextList( m_page_copy, index_part_attr );
	int it = -1;
	if( tl )
		if( tl->GetNextText(&it) )
			m_page_copy = -1;
	if( m_page_copy != gan && m_page_copy != -1 )
	{
		it=-1;
		int ip=-1;
		for( CText* gT=m_Doc->Pages.GetNextAttr( index_desc_attr, &it, &ip ); 
					gT; 
					gT=m_Doc->Pages.GetNextAttr( index_desc_attr, &it, &ip ) )
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
					if ( iofs == m_page_copy+1 )
					{
						CString endstr = "";
						int iofpg2 = pgstr.Find( "'" );
						if( iofpg2 > 0 )
							endstr = pgstr.Right( pgstr.GetLength() - iofpg2 );
						iofs = gan+1;
						pgstr.Format( CP_PAGE_NUM" %d%s", iofs, endstr );
						gT->m_str.Truncate( iofpg );
						gT->m_str += pgstr;
						gT->MakeVisible();
					}
				}
			}
		}
		m_page_copy = -1;
	}
}
//===============================================================================================
void CFreePcbView::OnEditCut()
{
	if( !m_Doc->m_project_open )
		return;
	if( getbit( m_sel_mask, SEL_MASK_PART ) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"You cannot cut out a group of objects in the disabled part mask mode, because this leads to the destruction of the parts into their component entities. First enable the selection of parts in the selection mask on the left.":
			"Вы не можете вырезать группу объектов в режиме отключенной маски деталей, поскольку это приводит к разрушению деталей на составляющие их полилинии. Сначала включите выделение деталей в маске выделения слева.");
		return;
	}
	OnGroupCopy();
	OnGroupDelete();
}
//===============================================================================================
void CFreePcbView::RotateGroup( int angle, int cx, int cy, double accurate )
{
	if( prev_sel_count != m_sel_count )	
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
	//
	if( cx || cy )
	{
		if( cx == 1 )
			cx = 0;
		groupAverageX = cx; 
		groupAverageY = cy;
	}
	else 
		FindGroupCenter();
	//
	CMainFrame * pMain = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pMain )
		return;
	
	// parts
	int COUNT = 0;
	
	BOOL ACC = (abs(accurate) > BY_ZERO);

	// convert arcs
	BOOL conv_ARC = FALSE;
	MarkAllOutlinePoly(0,-1);
	if( angle%90 && COUNT<m_sel_count )
	{
		// test on ARCs
		BOOL bTEST = FALSE;
		if( getbit(m_sel_flags, FLAG_SEL_OP) )
			for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
			{
				CPolyLine * p = &m_Doc->m_outline_poly->GetAt(item);
				int gss = p->GetNumCorners()-1;
				int cl = p->GetClosed();
				if( cl == 0 )
					gss--;
				for( int iss=gss; iss>=0; iss-- )
					if( p->GetSideSel(iss) || (p->GetSel(iss) && p->GetSel(p->GetNextCornerIndex(iss)) ) )
						if( p->GetSideStyle(iss) != CPolyLine::STRAIGHT )
							{
								bTEST = TRUE;
								p->SetUtility(1);
								break;
							}
			}
		if( bTEST && abs(accurate) > BY_ZERO)
		{
			//Google translate
			//Ýòà ãðóïïà ñîäåðæèò â ñåáå àðê ýëåìåíòû. 
			//Ïðè ïîâîðîòå íà  óãîë íå êðàòíûé 90 ãðàäóñîâ 
			//ëèíèÿ ìîæåò îòîáðàæàòüñÿ íåêîððåêòíî. 
			//Êîíâåðòèðîâàòü àðê ëèíèè ñ ïîìîùüþ àïïðîêñèìàòîðà?
			if( AfxMessageBox(G_LANGUAGE == 0 ? 
				"This group contains arc elements. When turning at an angle not a multiple of 90 degrees, the line may not be displayed correctly. Convert an arc line using an approximator?":
				"Эта группа содержит элементы эллиптической дуги. При повороте на угол, не кратный 90°, линия может отображаться некорректно. Преобразовать линию дуги с помощью аппроксиматора?", MB_YESNO) == IDYES)
				conv_ARC = TRUE;
		}
		
	}
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			BOOL RedrawFlag = FALSE;
			int gss = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();		
			if( conv_ARC && m_Doc->m_outline_poly->GetAt(item).GetUtility() )
			{
				ArcApp( &m_Doc->m_outline_poly->GetAt(item), TRUE, NULL );
				gss = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			}			
			for( int ic=gss; ic>=0; ic-- )
			{
				CPolyLine * p = &m_Doc->m_outline_poly->GetAt(item);
				int side_sel;
				if( cl == 0 && ic == gss )
					side_sel = 0;
				else
					side_sel = p->GetSideSel(ic);
				int prev_side_sel;
				int ib = p->GetPrevCornerIndex(ic);
				if( cl == 0 && ib == gss )
					prev_side_sel = 0;
				else
					prev_side_sel = p->GetSideSel(ib);
				int corner_sel = p->GetSel(ic);
				if( prev_side_sel || side_sel || corner_sel )
				{
					RedrawFlag = TRUE;
					int x = p->GetX(ic);
					int y = p->GetY(ic);
					Rotate_i_Vertex( &x,&y,angle,groupAverageX,groupAverageY );
					p->SetX(ic,x);
					p->SetY(ic,y);
					if( ACC )
					{
						double dbx = p->GetX(ic);
						double dby = p->GetY(ic);
						Rotate_Vertex( &dbx,&dby,accurate );
						p->SetX(ic,dbx);
						p->SetY(ic,dby);
					}
					COUNT++;
				}
			}
			if( RedrawFlag )
			{
				COUNT--;
				m_Doc->m_outline_poly->GetAt(item).RecalcRectC(0);
				RECT r = rect(0,0,0,0);
				m_Doc->m_outline_poly->GetAt(item).ModifyGroupRect(&r);
				//
				int isrc = m_Doc->m_outline_poly->GetAt(item).GetPicture();
				if( isrc >= 0 )
					m_Doc->m_dlist->MoveTemplate( &m_Doc->m_outline_poly->GetAt(item).GetCornerBounds(), 
												  isrc, m_Doc->m_outline_poly->GetAt(item).GetLayer() );
				m_Doc->m_outline_poly->GetAt(item).Draw();
				if( COUNT >= m_sel_count )
					break;
			}
		}
		int it = -1;
		for(CText* part=m_Doc->Attr->m_Reflist->GetNextText(&it); part; part=m_Doc->Attr->m_Reflist->GetNextText(&it))
		{
			if ( part->m_selected )
			{
				RECT pr = m_Doc->GetPartBounds( part, NULL );
				m_Doc->m_outline_poly->GetAt(part->m_polyline_start).ModifyGroupRect( &pr );
			}
		}
	}
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
			{
				if ( t->m_selected )
				{
					int x = t->m_x;
					int y = t->m_y;
					if (m_sel_flags != TEXT_ONLY || m_dragging_mode )
					{
						Rotate_i_Vertex(&x, &y, angle, groupAverageX, groupAverageY);
						if (ACC)
						{
							double dbx = x;
							double dby = y;
							Rotate_Vertex(&dbx, &dby, accurate);
							x = dbx;
							y = dby;
						}
					}
					tl->MoveText(t,x,y,t->m_angle-angle,t->m_layer);
					BOOL ROTATE = 0;
					if( t->m_layer == LAY_PIN_NAME ||
						t->m_layer == LAY_PART_NAME ||
						t->m_layer == LAY_PART_VALUE ||
						t->m_layer == LAY_FOOTPRINT )
						ROTATE = 1;
					else if( t->m_polyline_start >= 0 )
					{
						if( t->m_layer == LAY_PIN_DESC || t->m_layer >= LAY_ADD_1 )
							if( m_Doc->m_outline_poly->GetAt( t->m_polyline_start ).Check( index_pin_attr ) )
								ROTATE = 1;
					}
					//
					if( (t->m_angle+180)%360 == 0 && ROTATE)
					{
						t->MakeVisible();//(redraw to generate rect)
						RECT r;
						if( tl->GetTextRectOnPCB(t,&r) )
						{
							t->m_angle = 0;
							if (t->m_str.Right(2) != "|C")
							{
								t->m_x = r.left + t->m_stroke_width / 2;
								t->m_y = r.bottom + t->m_stroke_width / 2;
							}
							t->MakeVisible();
						}
					}
					else if( t->m_layer == LAY_PIN_DESC || t->m_layer >= LAY_ADD_1 )
						DrawBOM(t);
					COUNT++;
					if( COUNT >= m_sel_count )
						break;
				}
			}
		}
	}
	//
	if( m_sel_count )
		SetCursorMode( CUR_GROUP_SELECTED);
}


//===============================================================================================
CPoint CFreePcbView::FindGroupCenter( BOOL bFLOOR )
{
	groupAverageX=groupAverageY=0;

	RECT R;
	R.left = R.bottom = INT_MAX;
	R.top = R.right = INT_MIN;
	// texts 
	if( getbit( m_sel_flags, FLAG_SEL_TEXT ) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			//
			int it = -1;
			CText * t = tl->GetNextText(&it);
			while( t )
			{
				if( m_Doc->m_vis[t->m_layer] )
				{
					if( t->m_selected )
					{
						SwellRect( &R, t->m_x, t->m_y );
					}
				}
				t = tl->GetNextText(&it);
			}
		}
	}

	// outlines
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int im=m_Doc->m_outline_poly->GetSize()-1; im>=0; im-- )
		{
			CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(im);
			int gns = poly->GetNumCorners()-1;
			int cl = poly->GetClosed();
			for( int is=gns; is>=0; is-- )
			{
				int is2 = poly->GetNextCornerIndex(is);
				int x1 = poly->GetX(is);
				int y1 = poly->GetY(is);
				int x2 = poly->GetX(is2);
				int y2 = poly->GetY(is2);
				if( cl == 0 && is == gns )
					continue;
				if( poly->GetSideSel(is) )
				{
					SwellRect( &R, x1, y1 );
					SwellRect( &R, x2, y2 );
				}
			}
		}
	}

	if( R.left != INT_MAX )
	{
		groupAverageX = (R.left+R.right)/2;
		groupAverageY = (R.top+R.bottom)/2;

		if( bFLOOR )
		{
			double x=floor(groupAverageX/m_Doc->m_part_grid_spacing +.5);
			groupAverageX=(long long)(m_Doc->m_part_grid_spacing*x);
			x=floor(groupAverageY/m_Doc->m_part_grid_spacing +.5);
			groupAverageY=(long long)(m_Doc->m_part_grid_spacing*x);
		}
	}
	return CPoint((int)groupAverageX, (int)groupAverageY);
}

//===============================================================================================
// top-level description of undo operation
// list is the CUndoList that it will be pushed to
//
void * CFreePcbView::CreateUndoDescriptor( CUndoList * list, int type, CString * name1, CString * name2,
										  int int1, int int2, CString * str1, void * ptr )
{
	undo_descriptor * u_d = new undo_descriptor;//ok
	u_d->view = this;
	u_d->list = list;
	u_d->type = type;
	if( name1 )
		u_d->name1 = *name1;
	if( name2 )
		u_d->name2 = *name2;
	u_d->int1 = int1;
	u_d->int2 = int2;
	if( str1 )
		u_d->str1 = *str1;
	u_d->ptr = ptr;
	return (void*)u_d;
}
//===============================================================================================
// initial callback from undo/redo stack
// used to push redo/undo info onto the other stack
// note this is a static function (i.e. global)
//
void CFreePcbView::UndoCallback( void * ptr, BOOL undo )
{
	undo_descriptor * u_d = (undo_descriptor*)ptr;
	if( undo )
	{
		CFreePcbView * view = u_d->view;
		view->m_Doc->m_dlist->CancelHighLight();
		// if callback was from undo_list, push info to redo list, and vice versa
		CUndoList * redo_list;
		if( u_d->list == view->m_Doc->m_undo_list )
			redo_list = view->m_Doc->m_redo_list;
		else
			redo_list = view->m_Doc->m_undo_list;
		undo_text * u_text = (undo_text *)u_d->ptr;
		// save undo/redo info
		if( u_d->type == UNDO_TEXT )
		{
			GUID guid = u_text->m_guid;
			if( u_text->m_tlist == NULL )
				ASSERT(0);
			CText * text = u_text->m_tlist->GetText(&guid);
			if( text )
				view->SaveUndoInfoForText( text, TRUE, redo_list );	
		}
		else if( u_d->type == UNDO_MOVE_ORIGIN )
		{
			view->SaveUndoInfoForMoveOrigin( -u_d->int1, -u_d->int2, redo_list );
		}
		else if( u_d->type == UNDO_OP )
		{
			undo_outline_poly * u_p = (undo_outline_poly*)u_d->ptr;
			view->SaveUndoInfoForOutlinePoly( u_p->num, TRUE, redo_list );
		}
		else
			ASSERT(0);
	}
	delete(u_d);	// delete the undo record
}
//===============================================================================================
// callback for undoing group operations
// note this is a static function (i.e. global)
//
void CFreePcbView::UndoGroupCallback( void * ptr, BOOL undo )
{
	undo_group_descriptor * u_d = (undo_group_descriptor*)ptr;
	if( undo )
	{
		CFreePcbView * view = u_d->view;
		CFreePcbDoc * doc = view->m_Doc;
		// if callback was from undo_list, push info to redo list, and vice versa
		CUndoList * redo_list;
		if( u_d->list == view->m_Doc->m_undo_list )
		{
			// CTRL + Z
			redo_list = view->m_Doc->m_redo_list;
		}
		else
		{
			// CTRL + Y
			redo_list = view->m_Doc->m_undo_list;
		}
		
		// reconstruct pointers from names of items (since they may have changed)
		// and save the current status of the group
		view->CancelSelection();
		int n_items = u_d->m_id.GetSize();
		void * sel;
		for( int i=0; i<n_items; i++ )
		{
			CString * str_ptr = &u_d->str[i];
			id this_id = u_d->m_id[i];
			if( this_id.type == ID_TEXT )
			{
				GUID guid;
				::SetGuidFromString( str_ptr, &guid );
				CText * text = doc->Attr->m_Reflist->GetText( &guid );
				if( text == NULL )
					text = doc->Attr->m_Pinlist->GetText( &guid );
				if( text == NULL )
					text = doc->Attr->m_pDesclist->GetText( &guid );
				if( text == NULL )
					text = doc->Attr->m_Netlist->GetText( &guid );
				if( text == NULL )
					text = doc->Attr->m_Footlist->GetText( &guid );
				if( text == NULL )
					text = doc->Attr->m_Valuelist->GetText( &guid );
				sel = (void*)text;
				if( sel )
					view->NewSelect( sel, &this_id, 0, 0 );
			}
			else if( this_id.type == ID_POLYLINE )
				view->NewSelect( NULL, &this_id, 0, 0 );
		}
		//save group info
		view->SaveUndoInfoForGroup( redo_list );
	}
	delete(u_d);	// delete the undo record
}
//===============================================================================================

//===============================================================================================
void CFreePcbView::OnTextSimilar()
{
	if( m_sel_text->m_polyline_start == -1 )
		return;
	
	// show log
	m_Doc->m_dlg_log->Clear();
	m_Doc->m_dlg_log->OnShowMe();
	//===========================
	CTextList * gTl = m_sel_text->m_tl;
	CText * fAtt = NULL;
	//
	int it = -1;
	id ID( ID_TEXT_DEF );
	if( gTl == m_Doc->Attr->m_Reflist )
	{
		int foo = m_sel_text->m_str.FindOneOf("-.");
		if( foo > 0 )
		{
			fAtt = m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start).pAttr[index_foot_attr];
			CString s;
			s.Format("Text search among parts %s...:\r\n", m_sel_text->m_str.Left(foo+1) );
			m_Doc->m_dlg_log->AddLine( s );
			m_Doc->m_dlg_log->UpdateWindow();
			for( CText * t=gTl->GetNextText(&it); t; t=gTl->GetNextText(&it) )
			{
				if( t->m_str.Left(foo+1) == m_sel_text->m_str.Left(foo+1) )
				{
					NewSelect( t, &ID, 0, 0 );
					CString s;
					s.Format("    Text found: %s ( x = %d Nm, y = %d Nm )\r\n", t->m_str, t->m_x, t->m_y );
					m_Doc->m_dlg_log->AddLine( s );
					m_Doc->m_dlg_log->UpdateWindow();
					DRError * dre = m_Doc->m_drelist->Add( &s, ID, ID, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );
				}
			}
		}
		else
		{
			CString PREF, SUFF;
			ParseRef( &m_sel_text->m_str, &PREF, &SUFF );
			fAtt = m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start).pAttr[index_foot_attr];
			CString s;
			s.Format("Text search among parts with prefix %s:\r\n", PREF );
			m_Doc->m_dlg_log->AddLine( s );
			m_Doc->m_dlg_log->UpdateWindow();
			for( CText * t=gTl->GetNextText(&it); t; t=gTl->GetNextText(&it) )
			{
				CString pref, suff;
				ParseRef( &t->m_str, &pref, &suff );
				if( PREF.Compare(pref) == 0 )
				{
					NewSelect( t, &ID, 0, 0 );
					CString s;
					s.Format("    Text found: %s ( x = %d Nm, y = %d Nm )\r\n", t->m_str, t->m_x, t->m_y );
					m_Doc->m_dlg_log->AddLine( s );
					m_Doc->m_dlg_log->UpdateWindow();
					DRError * dre = m_Doc->m_drelist->Add( &s, ID, ID, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );
				}
			}
		}
	}
	else 
	{
		fAtt = m_Doc->m_outline_poly->GetAt(m_sel_text->m_polyline_start).Check( index_foot_attr );
		if( fAtt )
			if( m_sel_text->m_tl != m_Doc->Attr->m_Netlist &&
				m_sel_text->m_tl != m_Doc->Attr->m_pDesclist )
			{
				CString s;
				if( gTl == m_Doc->Attr->m_Valuelist )
					s.Format("Text search among parts with footprint %s and value %s\r\n", fAtt->m_str, m_sel_text->m_str );
				else
					s.Format("Text search among parts with footprint %s:\r\n", fAtt->m_str );
				m_Doc->m_dlg_log->AddLine( s );
				m_Doc->m_dlg_log->UpdateWindow();
			}
		for( CText * t=gTl->GetNextText(&it); t; t=gTl->GetNextText(&it) )
		{
			if( t->m_str.Compare( m_sel_text->m_str ) == 0 )
				if( t->m_polyline_start >= 0 )
					if( fAtt == NULL || 
						m_sel_text->m_tl == m_Doc->Attr->m_Netlist ||
						m_sel_text->m_tl == m_Doc->Attr->m_pDesclist )
					{
						if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_foot_attr ) == 0 ||
							t->m_tl == m_Doc->Attr->m_Netlist ||
							t->m_tl == m_Doc->Attr->m_pDesclist )
						{
							NewSelect( t, &ID, 0, 0 );
							CString s;
							if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) )
							{
								CString part = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_part_attr]->m_str;
								s.Format("    part %s\r\n", part );
							}
							else if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_net_attr ) )
							{
								CString net = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_net_attr]->m_str;
								s.Format("    net %s\r\n", net );
							}
							else
								s.Format("    Text found: %s ( x = %d Nm, y = %d Nm )\r\n", t->m_str, t->m_x, t->m_y );
							m_Doc->m_dlg_log->AddLine( s );
							m_Doc->m_dlg_log->UpdateWindow();
							DRError * dre = m_Doc->m_drelist->Add( &s, ID, ID, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );
						}
					}
					else if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_foot_attr ) )
						if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_foot_attr]->m_str.Compare( fAtt->m_str ) == 0 )
						{
							NewSelect( t, &ID, 0, 0 );
							CString s;
							if( m_Doc->m_outline_poly->GetAt(t->m_polyline_start).Check( index_part_attr ) )
							{
								CString part = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_part_attr]->m_str;
								CString foot = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).pAttr[index_foot_attr]->m_str;
								s.Format("    part %s\r\n", part );
							}
							else
								ASSERT(0);
							m_Doc->m_dlg_log->AddLine( s );
							m_Doc->m_dlg_log->UpdateWindow();
							DRError * dre = m_Doc->m_drelist->Add( &s, ID, ID, t->m_x, t->m_y, t->m_x, t->m_y, 20*NM_PER_MIL, LAY_DRC_ERROR, t );
						}
		}
	}
	m_Doc->m_dlg_log->AddLine( "***** DONE *****\r\n" );
	m_Doc->m_dlg_log->AddLine( "Use key 'D' to enlarge marks\r\n" );
	m_Doc->m_dlg_log->UpdateWindow();
	SetCursorMode( CUR_GROUP_SELECTED );
	HighlightGroup();
	JumpToPart(NULL);
	Invalidate( FALSE );// (doubtful)
}
//===============================================================================================
void CFreePcbView::OnTextGrabPart()
{
	m_Doc->SwitchToPCB( FALSE, TRUE );
}
//===============================================================================================
void CFreePcbView::OnGroupRotate()
{
	RotateGroup(90);
	Invalidate( FALSE );// (doubtful)//OnGroupRotate	
}
//===============================================================================================
void CFreePcbView::RotateGroup( int angle )
{
	m_Doc->SelectGroupAttributes();
	m_Doc->m_dlist->CancelHighLight();
	RotateGroup(angle,0,0);
	HighlightGroup();
	m_Doc->ProjectModified( TRUE );
	m_draw_layer = 0;// RotateGroup
}


//===============================================================================================
// enable/disable the main menu
// used when dragging
//
void CFreePcbView::SetMainMenu( BOOL bAll )
{
	CFrameWnd * pMainWnd = (CFrameWnd*)AfxGetMainWnd();
	if( bAll )
	{
		pMainWnd->SetMenu(&theApp.m_main);
		OnHidePictures( NULL );
		///CMenu* pMenu = pMainWnd->GetMenu();
		///CMenu* submenu = pMenu->GetSubMenu(0);
		m_Doc->ProjectModified(); // set menu
	}
	else
		pMainWnd->SetMenu(&theApp.m_main_drag);
	return;
}

//===============================================================================================
void CFreePcbView::AlignSides( int type, int ic, int iv)
{
	CPoint p, pback, pnext;
	int B,N;
	switch (type)
	{
	case ID_POLYLINE:
		B =			m_Doc->m_outline_poly->GetAt(ic).GetPrevCornerIndex(iv);
		N =			m_Doc->m_outline_poly->GetAt(ic).GetNextCornerIndex(iv);
		p.x =		m_Doc->m_outline_poly->GetAt(ic).GetX(iv);
		p.y =		m_Doc->m_outline_poly->GetAt(ic).GetY(iv);
		pback.x =	m_Doc->m_outline_poly->GetAt(ic).GetX(B);
		pback.y =	m_Doc->m_outline_poly->GetAt(ic).GetY(B);
		pnext.x =	m_Doc->m_outline_poly->GetAt(ic).GetX(N);
		pnext.y =	m_Doc->m_outline_poly->GetAt(ic).GetY(N);
		break;
	}
	CPoint P = AlignPoints(p,pback,pnext,FALSE,m_Doc->m_snap_angle);
	switch (type)
	{
	case ID_POLYLINE:
		RECT r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
		SwellRect( &r, abs(m_Doc->m_node_w/2) );
		SaveUndoInfoForOutlinePoly( ic, TRUE, m_Doc->m_undo_list );
		m_Doc->m_outline_poly->GetAt(ic).MoveCorner( iv, P.x, P.y );
		m_Doc->m_outline_poly->GetAt(ic).HighlightCorner(iv);
		m_Doc->m_dlist->HighlightRect( &r, m_active_layer, TRANSPARENT_BLACK_GROUND, DL_HOLLOW_RECT );
		m_draw_layer = LAY_BACKGND;// AlignSides
		break;
	}
}

//===============================================================================================
BOOL CFreePcbView::TestSelElements (int mode)
{
	return FALSE;
}
//===============================================================================================
void CFreePcbView::OnSetClearance ()
{
	if( !m_sel_count )
		return;
	if( prev_sel_count != m_sel_count )
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
	m_Doc->m_dlist->CancelHighLight();
	int cl, MARK_U=1;
	RECT SELRECT;
	SELRECT.left = DEFAULT;
	SELRECT.right = -DEFAULT;
	SELRECT.bottom = DEFAULT;
	SELRECT.top = -DEFAULT;
	if (m_seg_clearance < 0)
		cl = 0;
	else
		cl = m_seg_clearance;
	double	x1, y1, x1n, y1n, 
		    x_1, y_1, x_1n, y_1n,
			x2, y2, x2n, y2n, 
			f_an, f_x, f_y, 
			newx, newy, newxn, newyn; 
	id _id, idb, f_id;
	int w1, w2;
	MarkAllOutlinePoly(0,-1);
	if( m_sel_id.type == ID_POLYLINE )
	{
		f_id = _id = idb = m_sel_id;	
		x2 = f_x = m_Doc->m_outline_poly->GetAt(_id.i).GetX(_id.ii);
		y2 = f_y = m_Doc->m_outline_poly->GetAt(_id.i).GetY(_id.ii);
		int inext = m_Doc->m_outline_poly->GetAt(_id.i).GetNextCornerIndex( _id.ii );
		x2n = m_Doc->m_outline_poly->GetAt(_id.i).GetX(inext);
		y2n = m_Doc->m_outline_poly->GetAt(_id.i).GetY(inext);
		m_Doc->m_outline_poly->GetAt(_id.i).SetUtility( _id.ii, MARK_U );
		w2 = abs( m_Doc->m_outline_poly->GetAt(_id.i).GetW() );
		m_Doc->m_outline_poly->GetAt(_id.i).HighlightCorner( _id.ii, w2 );
		m_Doc->m_outline_poly->GetAt(_id.i).HighlightCorner( inext, w2 );	
	}
	else return;
	SwellRect( &SELRECT, x2, y2 );
	SwellRect( &SELRECT, x2n, y2n );
	SwellRect( &SELRECT, w2/2 );
	f_an = Angle(x2n,y2n,x2,y2);
	Rotate_Vertex( &f_x, &f_y, -f_an);
	int bMess = 0;
	while (1)
	{
		FindNEXTSegmentOfGroup ( f_an, &_id, &f_id );
		if( _id.ii == -1 )
			break;
		//ID POLYLINE
		if( _id.type == ID_POLYLINE )
		{
			CPolyLine * p;
			if( _id.type == ID_POLYLINE )
				p = &m_Doc->m_outline_poly->GetAt(_id.i);
			else
			{
				ASSERT(0);
			}
			int ib = p->GetPrevCornerIndex(_id.ii);
			int in = p->GetNextCornerIndex(_id.ii);
			int inn = p->GetNextCornerIndex(in);
			x_1 =	p->GetX(ib);
			y_1 =	p->GetY(ib);
			x1 =	p->GetX(_id.ii);
			y1 =	p->GetY(_id.ii);
			x1n =	p->GetX(in);
			y1n =	p->GetY(in);
			x_1n =  p->GetX(inn);
			y_1n = 	p->GetY(inn);
			w1 =    abs(p->GetW());
			p->SetUtility(_id.ii, MARK_U);
		}
		else
			ASSERT(0);
		if( idb.type == ID_POLYLINE )
		{
			CPolyLine * p;
			if( idb.type == ID_POLYLINE )
				p = &m_Doc->m_outline_poly->GetAt(idb.i);
			else
			{
				ASSERT(0);
			}
			x2 =	p->GetX(idb.ii);
			y2 =	p->GetY(idb.ii);
			int in = p->GetNextCornerIndex(idb.ii);
			x2n =	p->GetX(in);
			y2n =	p->GetY(in);
			w2 = abs(p->GetW());
		}
		else
			ASSERT(0);
		Rotate_Vertex( &x_1, &y_1, -f_an);
		Rotate_Vertex( &x1, &y1, -f_an);
		Rotate_Vertex( &x1n, &y1n, -f_an);
		Rotate_Vertex( &x_1n, &y_1n, -f_an);
		Rotate_Vertex( &x2, &y2, -f_an);
		Rotate_Vertex( &x2n, &y2n, -f_an);
		if( y1-f_y > 0 )//&& f_an < 180.0 ) || ( y1-f_y < 0 && f_an > 180.0 ))
			newy = y2 + cl + w1/2 + w2/2;
		else
			newy = y2 - cl - w1/2 - w2/2;
		float an_b = Angle(x1,y1,x_1,y_1);
		float an_n = Angle(x_1n,y_1n,x1n,y1n);		
		float dy = y1 - newy;
		newx = x1 - dy/tan(an_b*M_PI/180.0);
		dy = y1n - newy;
		newxn = x1n - dy/tan(an_n*M_PI/180.0);
		newyn = newy;
		Rotate_Vertex(&newx,&newy,	f_an);
		Rotate_Vertex(&newxn,&newyn,f_an);
		if( abs(newx) < DEFAULT/2 && abs(newy) < DEFAULT/2 && abs(newxn) < DEFAULT/2 && abs(newyn) < DEFAULT/2 )
		{
			if( _id.type == ID_POLYLINE )
			{
				CPolyLine * p = &m_Doc->m_outline_poly->GetAt(_id.i);
				p->SetX( _id.ii, newx );
				p->SetY( _id.ii, newy );
				int inext = p->GetNextCornerIndex(_id.ii);
				p->SetX( inext, newxn );
				p->SetY( inext, newyn );
				p->Draw( m_Doc->m_dlist );
				for( int icor=0; icor<p->GetNumCorners(); icor++ )
					if( p->GetUtility(icor) )
					{
						p->HighlightCorner( icor, w1 );
						int inxt = p->GetNextCornerIndex( icor );
						p->HighlightCorner( inxt, w1 );
					}
			}
			else
				ASSERT(0);
			RECT r1 = rect( newx, newy, newx+10, newy+10 );
			SwellRect( &r1, w1/2 );
			RECT r2 = rect( newxn, newyn, newxn+10, newyn+10 );
			SwellRect( &r2, w1/2 );
			SwellRect( &SELRECT, r1 );
			SwellRect( &SELRECT, r2 );
		}
		else
		{
			bMess = 1;
		}
		idb = _id;
	}
	if( SELRECT.left < DEFAULT )
	{
		int lay_hilite_m = 0;
		setbit( lay_hilite_m, LAY_HILITE );
		id _id;
		RECT r = SELRECT;
		dl_element * el = m_Doc->m_dlist->HighLight( DL_HOLLOW_RECT, &r, 1, NULL, 0, m_active_layer );
		el->transparent = TRANSPARENT_HILITE;
	}
	if( bMess )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Unable to align colinear segments!":
			"Невозможно выровнять коллинеарные сегменты!");
	m_seg_clearance = cl;
	m_page = 2;
	SetFKText(m_cursor_mode);
	m_Doc->ProjectModified(TRUE);
}





//===============================================================================================
void CFreePcbView::FindNEXTSegmentOfGroup ( float angle, id * _id, id * f_id )
{
	double x, y;
	if( f_id->type == ID_POLYLINE )
	{
		x = m_Doc->m_outline_poly->GetAt(f_id->i).GetX(f_id->ii);
		y = m_Doc->m_outline_poly->GetAt(f_id->i).GetY(f_id->ii);
	}
	else
		ASSERT(0);
	Rotate_Vertex( &x, &y, -angle );
	_id->ii = -1;
	int len, min_len = DEFAULT;
	if( getbit( m_sel_flags, FLAG_SEL_OP ) )
	{
		for (int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
			if( find_side_for_set_clearance( &m_Doc->m_outline_poly->GetAt(i), _id, y, angle, min_len ) )
			{
				id ID = m_Doc->m_outline_poly->GetAt(i).GetId();
				_id->type = ID.type;
				_id->st = ID.st;
				_id->sst = ID_SIDE;
				_id->i = i;
			}
	}
}



BOOL CFreePcbView::find_side_for_set_clearance( CPolyLine * pp, id * _id, int Y, float angle, int min_len )
{
	BOOL RET = NULL;
	int cl = pp->GetClosed();
	int nc = pp->GetNumCorners();
	if( cl == 0 )
		nc--;
	for(int ii=0; ii<nc; ii++)
	{
		if ( pp->GetUtility(ii) )
			continue;
		if (!pp->GetSideSel(ii) )
			continue;
		double vx =	pp->GetX(ii);
		double vy =	pp->GetY(ii);
		int inext = pp->GetNextCornerIndex(ii);
		double vxn =	pp->GetX(inext);
		double vyn =	pp->GetY(inext);
		Rotate_Vertex(&vx,&vy,-angle);
		Rotate_Vertex(&vxn,&vyn,-angle);
		float len	= min(abs(vy-Y),abs(vyn-Y));
		if ( len < min_len )
		{
			min_len = len;
			_id->ii = ii;
			RET = TRUE;
		}
	}
	return RET;
}

//===============================================================================================

//===============================================================================================
void CFreePcbView::ApproximArc ()
{
	SaveUndoInfoForGroup( m_Doc->m_undo_list );
	float p[2], pb[2], p1[2], p2[2], pn[2];
	enum {X = 0, Y};
	int ID = 0;
	MarkAllOutlinePoly(0,-1);
	for( int ibo=m_Doc->m_outline_poly->GetSize()-1; ibo>=0; ibo-- )
	{
		if( ArcApp( &m_Doc->m_outline_poly->GetAt(ibo), FALSE, NULL ) )
			m_Doc->m_outline_poly->GetAt(ibo).Draw();
	}
}


int CFreePcbView::ArcApp( CPolyLine * po, int forArcOnly, int m_i )
{
	float p[2], pb[2], p1[2], p2[2], pn[2];
	enum {X = 0, Y};
	BOOL bdraw = 0;
	int nc = po->GetNumCorners()-1;
	if( po->GetClosed() == 0 )
		nc--;
	for( int ii=nc; ii>=0; ii-- )
	{
		BOOL bE = (po->GetSideSel(ii) && forArcOnly == 0) || (po->GetSideStyle(ii) && forArcOnly);
		if( bE && po->GetUtility(ii) == 0 )
		{
			bdraw = 1;
			int ss = po->GetSideStyle(ii);
			po->SetSideStyle(ii,CPolyLine::STRAIGHT,0);
			po->SetUtility( ii, 1 );
			int ncont = po->GetNumContour(ii);
			int start = po->GetContourStart(ncont);
			int end = po->GetContourEnd(ncont); 
			int ib = po->GetPrevCornerIndex(ii);
			int in = po->GetNextCornerIndex(ii);
			int inn = po->GetNextCornerIndex(in);
			pb[X] = po->GetX(ib);
			pb[Y] = po->GetY(ib);
			p1[X] = po->GetX(ii);
			p1[Y] = po->GetY(ii);
			p2[X] = po->GetX(in);
			p2[Y] = po->GetY(in);
			pn[X] = po->GetX(inn);
			pn[Y] = po->GetY(inn);
			int N_SIDES = N_SIDES_APPROX_ARC;
			if( Distance( p1[X],p1[Y],p2[X],p2[Y] ) > 20*NM_PER_MM )
				N_SIDES *= 3;
			float * data = new float[N_SIDES*4+2];//new023
			int cnt;
			if(ss)
			{
				CPoint * PTS = new CPoint[(N_SIDES*2)+1];//new006
				cnt = Generate_Arc( p1[X], p1[Y], p2[X], p2[Y], ss, PTS, (N_SIDES*2) );
				for(int a=0;a<cnt;a++)
				{
					data[2*a] = PTS[cnt-a-1].x;
					data[2*a+1] = PTS[cnt-a-1].y;
				}
				cnt *= 2;
				delete PTS;//new006
			}
			else
				cnt = Rnd_Func(pn[X], pn[Y], p2[X], p2[Y], p1[X], p1[Y], pb[X], pb[Y], &data[0], (N_SIDES_APPROX_ARC*4));
			if( cnt )
			{
				int cor = ii+1;
				if( ii+1 > end )
					cor = start;
				for (int c=2; c<cnt-2; c=c+2)
				{
					po->InsertCorner( cor, data[c], data[c+1], 0 );
					id sel = po->GetId();
					sel.sst = ID_SIDE;
					sel.ii = cor;
					NewSelect( NULL, &sel, 0,0 );
					if( cor == start )
					{
						ii++;
						po->SetUtility( cor, 1 );
					}
				}
			}
			delete data;//new023
		}
	}
	return bdraw;
}

//===============================================================================================
int CFreePcbView::DeleteWithRecalculationPoint (CPolyLine * p, BOOL WITH_MERGE)
{
	int mv=-1;
	if( p )
	{
		p->Undraw();
		int first_cont = -1;
		int start = -1;
		int end = -1;
		int i_beg = -1;
		int cl = p->GetClosed();
		int ncrns = p->GetNumSides()-1;
		for( int is=ncrns; is>=0; is-- )
		{
			if( is > ncrns )
				continue;
			if( p->GetSideSel( is ))
			{
				if( first_cont == -1 )
					first_cont = p->GetContour(is);
				//
				if( is == ncrns || is == 0 )
				{
					if ( start == -1 )
						start = is;
				}
				else if( p->GetSideSel( p->GetNextCornerIndex(is)) == 0 )
				{
					start = is;
				}
				//
				if( is == 0 || is == 0 )
				{
					end = is;
					if ( start == -1 )
						start = is;
				}
				else if( p->GetSideSel( p->GetPrevCornerIndex(is)) == 0 )
				{
					end = is;
					if ( start == -1 )
						start = p->GetContourEnd(p->GetContour(is));
				}	
				if( first_cont > p->GetContour(is) )
				{
					p->RemoveContour(first_cont,0);
					first_cont = p->GetContour(is);
					ncrns = p->GetNumSides()-1;
					start = is;
				}
			}			
			if ( end >= 0 )
			{
				first_cont = -1;
				int numb_cont = p->GetContour(start);
				if( numb_cont == p->GetContour(end) )
				{
					int bx1 = p->GetX(p->GetPrevCornerIndex(end));
					int by1 = p->GetY(p->GetPrevCornerIndex(end));
					int x1 = p->GetX(end);
					int y1 = p->GetY(end);
					int s_n = p->GetNextCornerIndex(start);
					int x2 = p->GetX(s_n);
					int y2 = p->GetY(s_n);
					int nx2 = p->GetX(p->GetNextCornerIndex(s_n));
					int ny2 = p->GetY(p->GetNextCornerIndex(s_n));
					float a1 = Angle(	bx1,by1,x1,y1  );
					float a2 = Angle(	nx2,ny2,x2,y2  );
					int dc = start - end;
					if( dc < 0 )
					{
						int cstart = p->GetContourStart(numb_cont);
						int cend = p->GetContourEnd(numb_cont);
						dc = cend - end + start - cstart + 1;
					}
					if( p->GetNumCorners() > 3 )
						p->DeleteCorner(p->GetNextCornerIndex(end),dc,0,0);
					end = min(end,p->GetContourEnd(numb_cont));
					if (InRange((int)abs(a1-a2)%180, 10, 170) && WITH_MERGE )
					{
						double x = x1 - x2;
						double y =  y1 - y2;
						Rotate_Vertex( &x, &y, -a1 );
						double newY = y;
						double newX = -y*tan((90-a1+a2)*M_PI/180.0);
						Rotate_Vertex( &newX, &newY, a1 );
						newX += x2;
						newY += y2;
						mv = p->GetNextCornerIndex(end);
						p->SetX(mv,newX);
						p->SetY(mv,newY);
						if( p->GetNumCorners() > 3 )
							p->DeleteCorner(end,1,0,0);
					}
					is = min(is,(p->GetContourEnd(numb_cont)+1));
				}
				ncrns = p->GetNumSides()-1;
				start = -1;
				end = -1;
			}
		}
		if( first_cont > 0 )
			p->RemoveContour(first_cont,0);
		for( int i_cont=p->GetNumContours()-1; i_cont>=0; i_cont-- )
			p->RecalcRectC(i_cont);
		///p->Draw();
	}
	return mv;
}
//===============================================================================================

//===============================================================================================
void CFreePcbView::TracesRadiusUpDown(BOOL UP)
{
	
}


//===============================================================================================
//------------------------ Select Contour for Board, sm cutouts or  areas -----------------------
//===============================================================================================
void CFreePcbView::SelectContour()
{
	
	int S;
	if( m_sel_count == 0 )
		return;
	id sid;
	id mem_id = m_sel_id;
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			int gss = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for(int ii=gss; ii>=0; ii--)
			{
				BOOL H = 0;
				if( cl || ii!=gss )
					if( m_Doc->m_outline_poly->GetAt(item).GetSideSel( ii ) )
						H = TRUE;
				if( H || m_Doc->m_outline_poly->GetAt(item).GetSel( ii ) )
				{	
					sid = m_Doc->m_outline_poly->GetAt(item).GetId();
					int num_c = m_Doc->m_outline_poly->GetAt(item).GetNumContour(ii);
					int sc = m_Doc->m_outline_poly->GetAt(item).GetContourStart(num_c);
					int ec = m_Doc->m_outline_poly->GetAt(item).GetContourEnd(num_c);
					if( num_c == 0 )
						if( cl )
							ec = gss;
						else
							ec = gss-1;
					for(int sel=sc; sel<=ec; sel++ )
					{		
						sid.sst = ID_SIDE;
						sid.ii = sel;
						NewSelect( NULL, &sid, 0, 0);
						m_Doc->m_outline_poly->GetAt(item).HighlightSide(sel);
					}
					ii = m_Doc->m_outline_poly->GetAt(item).GetContourStart(num_c);
					// contour selected
					ii = sc;
				}
			}
		}
	}
	m_sel_id = mem_id;
	if( !m_dragging_mode )
	{
		SetCursorMode(CUR_GROUP_SELECTED);
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
	}
}

//===============================================================================================
//---------------------------------------- MergeGroup -------------------------------------------
//===============================================================================================
void CFreePcbView::MergeGroup( int merge0 )
{	
	// outline
	//if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			if( m_Doc->m_outline_poly->GetAt(item).GetSideSel() )
			{
			 	if( merge0 >= 0 )
				{
					m_Doc->m_outline_poly->GetAt(item).SetMerge( m_Doc->m_outline_poly->GetAt(item).GetMerge(), TRUE );
					m_Doc->m_outline_poly->GetAt(item).SetMerge( merge0 );
				}
				else
				{
					m_Doc->m_outline_poly->GetAt(item).SetMerge( m_Doc->m_outline_poly->GetAt(item).GetMerge(TRUE) );
					m_Doc->m_outline_poly->GetAt(item).SetMerge( merge0, TRUE );
				}
			}
		}
	}
	//// texts
	////if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
				if ( t->m_selected )
				{
					if( merge0 >= 0 )
					{
						t->m_submerge = t->m_merge;
						t->m_merge = merge0;
					}
					else
					{
						t->m_merge = t->m_submerge;
						t->m_submerge = merge0;
					}
				}
		}
	}
	if( m_Doc->Pages.InvalidateMerges( merge0 ) )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Other pages have canceled a merge with the exact same name because the same merge name is not allowed on different pages":
			"Другие страницы имеют слияние с точно таким же именем. Одно и то же имя слияния не допускается на разных страницах.");
	m_Doc->ProjectModified( TRUE );
}

//==================================================================================
void CFreePcbView::MergeGroup()
{
	if( !m_sel_count )
		return;
	//
	int merge0 = -1;
	if( m_sel_merge_name.GetLength() )
	{
		merge0 = m_Doc->m_mlist->GetIndex( m_sel_merge_name );
	}
	else
	{
		
	}
	if ( m_sel_id.type == ID_POLYLINE && merge0 == -1 )
		merge0 = m_sel_op.GetMerge();
	CDlgAddMerge dlg;
	dlg.Doc = m_Doc;
	int ret;
	if ( merge0 == -1 )
	{	
		dlg.m_merge_name = ""; 
		ret = dlg.DoModal();
	}
	else
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This group already has a merge name!":
			"У этой группы уже имеется название слияния!", MB_ICONERROR);
		return;
	}
	if (ret == IDOK)
	{
		if( dlg.m_merge_name.Left(4) == "None" )
		{
			ExplodeGroup();
			return;
		}
		merge0 = m_Doc->m_mlist->AddNew(dlg.m_merge_name);
		if( merge0 != m_Doc->m_mlist->GetSize()-1 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? "This name already used!":"Это имя уже используется!", MB_ICONERROR);
			return;
		}
		MergeGroup(merge0);
	}
	CancelSelection();
	NewSelectM(merge0);
	m_sel_merge_name = dlg.m_merge_name;
	SetCursorMode( CUR_GROUP_SELECTED );
	HighlightGroup();
	OnRangeCmds( NULL );
}
//===============================================================================================
//-------------------------------------- ExplodeGroup -------------------------------------------
//===============================================================================================
void CFreePcbView::ExplodeGroup()
{
	int ret = AfxMessageBox(G_LANGUAGE == 0 ? " Delete this merger?":"Расформировать это слияние?", MB_YESNO);
	if( ret == IDYES )
	{
		m_Doc->SelectGroupAttributes();
		MergeGroup(-1);
		RemoveOrphanMergeNames();
		m_sel_merge_name = "";
		SetFKText( m_cursor_mode );
	}
}
void CFreePcbView::RemoveOrphanMergeNames()
{
	// Remove orphan Merge Names
	m_Doc->m_mlist->mark0();
	for( int ipg=0; ipg<m_Doc->Pages.GetNumPages(); ipg++ )
	{
		CArray<CPolyLine> * po = m_Doc->Pages.GetPolyArray( ipg );
		for( int item=po->GetSize()-1; item>=0; item-- )
		{
			if( po->GetAt(item).m_visible == 0 )
				continue;
			int im = po->GetAt(item).GetMerge();
			if( im >= 0 )
			{
				m_Doc->m_mlist->mark1(im);
				im = po->GetAt(item).GetMerge(TRUE);
				if( im >= 0 )
					m_Doc->m_mlist->mark1(im);
			}
		}
	}
	for( int idel=m_Doc->m_mlist->GetSize()-1; idel>=0; idel-- )
	{
		if( m_Doc->m_mlist->GetMark(idel) == 0 )
		{
			m_Doc->m_mlist->RemoveAt(idel);
			for( int ipg=0; ipg<m_Doc->Pages.GetNumPages(); ipg++ )
			{
				CArray<CPolyLine> * po = m_Doc->Pages.GetPolyArray( ipg );
				for( int item=po->GetSize()-1; item>=0; item-- )
				{
					int im = po->GetAt(item).GetMerge();
					if( im == idel && po->GetAt(item).m_visible )
						ASSERT(0);
					if( im > idel )
						po->GetAt(item).SetMerge(im-1);
					im = po->GetAt(item).GetMerge(TRUE);
					if( im == idel && po->GetAt(item).m_visible )
						ASSERT(0);
					if( im > idel )
						po->GetAt(item).SetMerge(im-1,TRUE);
				}
				for( int iatt=0; iatt<num_attributes; iatt++ )
				{
					CTextList * tl = m_Doc->Pages.GetTextList( ipg, iatt );
					if(!tl)
						continue;
					int it = -1;
					for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
					{
						if( t->m_merge == idel )
							t->m_merge = -1;
						if( t->m_submerge == idel )
							t->m_submerge = -1;
						if( t->m_merge > idel )
							t->m_merge--;
						if( t->m_submerge > idel )
							t->m_submerge--;
					}
				}
			}
		}
	}
}
//===============================================================================================
//-------------------------------------- SelectPart --------------------------------------------
//===============================================================================================
void CFreePcbView::NewSelectP( CText * part, CText * pin, BOOL hAttrs, int Swell )
{
	BOOL setCursorMode = (m_dragging_mode == 0 && hAttrs);
	if( hAttrs )
		m_Doc->m_dlist->CancelHighLight();
	m_Doc->SelectPart( part, pin, setCursorMode, Swell );
	if( hAttrs )
		m_Doc->HighlightPartAttributes( part, pin );
	HighlightGroup(); 
}
//===============================================================================================
//-------------------------------------- SelectBlock --------------------------------------------
//===============================================================================================
void CFreePcbView::NewSelectB( CText * descriptor )
{
	SelectBlock( &m_Doc->m_outline_poly->GetAt(descriptor->m_polyline_start), m_Doc->Pages.GetActiveNumber(), TRUE );
}
//===============================================================================================
//-------------------------------------- SelectMerge --------------------------------------------
//===============================================================================================
CString CFreePcbView::NewSelectM( int number )
{
	BOOL En_UnSel=0;
	int num = number;
	if ( num >= m_Doc->m_mlist->GetSize() || num < 0 )
		return "";
	if( m_Doc->m_mlist->GetMark( num ) && En_UnSel )
		m_Doc->m_mlist->mark1( num, 0 );
	else
		m_Doc->m_mlist->mark1( num, 1 );
	id sid;
	
	// SEL TEXTS
	sid.Set(ID_TEXT_DEF);
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if(!tl)
			continue;
		int it = -1;
		for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
			if ( t->m_merge == num )
			{
				if( En_UnSel )
					UnSelect( t, &sid, 0, 0 );
				else
					NewSelect( t, &sid, 0, 0 );
			}
	}
	
	// SEL OUTLINE POLY
	for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
	{
		if( m_Doc->m_outline_poly->GetAt(item).m_visible == 0 )
			continue;
		if ( m_Doc->m_outline_poly->GetAt(item).GetMerge() == num )
		{
			sid = m_Doc->m_outline_poly->GetAt(item).GetId();
			sid.sst = ID_SIDE;
			int gss = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			if( cl == 0 )
				gss--;
			for( int ic=gss; ic>=0; ic-- )
			{
				sid.ii = ic;
				if( En_UnSel )
					UnSelect( NULL, &sid, 0, 0 );
				else
				{
					NewSelect( NULL, &sid, 0, 0 );
					if( m_Doc->m_outline_poly->GetAt(item).pAttr[index_part_attr] )
						setbit( m_sel_flags, FLAG_SEL_PART );
				}
			}
		}
	}

	// select prev
	if( GetSelectedItem() == 0 )
		CancelSelection();
	// END
	if( m_sel_count > 0 )
	{	
		m_sel_merge_name = m_Doc->m_mlist->GetStr(num);
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		SetCursorMode(CUR_GROUP_SELECTED);
			return m_sel_merge_name;
	}
	return "";
}

void CFreePcbView::CheckMessages()
{
	CArray<CString> str_arr;
	str_arr.SetSize(0);
	OnMess( -1, 0, &str_arr, 0 );
}
//===============================================================================================
//------------------------------------------ Info Box -------------------------------------------
//===============================================================================================
void CFreePcbView::OnMess( int command, int n_str, CArray<CString> *str, BOOL IronScale )
{
	static int m_command = -1;
	static int m_n_str = 0;
	static CArray<CString> m_str;
	static int m_ironscale = 0;
	static int bPageChanged = 0;
	if( command != -1 )
	{
		if( m_Doc->m_dlist->bDrawBreak != -1 )
		{
			// save command
			m_command = command;
			m_n_str = n_str;
			m_str.RemoveAll();
			for( int is=0; is<str->GetSize(); is++ )
				m_str.Add( str->GetAt(is) );
			m_ironscale = IronScale;
			return;
		}
	}
	else if( command == -1 && m_command != -1 && m_Doc->m_dlist->bDrawBreak == -1 )
	{
		// extract command
		command = m_command;
		n_str = m_n_str;
		str->RemoveAll();
		for( int is=0; is<m_str.GetSize(); is++ )
			str->Add( m_str.GetAt(is) );
		IronScale = m_ironscale;
		// reset
		m_command = -1;
	}
	else 
		return; // fail
	//
	enum{C_JUMP=0, C_PARTS, C_PIN, C_REOPEN, C_FILE, C_VDP, C_SEL_M};
	if( str == NULL )
		return;
	if( str->GetSize() < 1 )
		return;

	int old_page = m_Doc->Pages.GetActiveNumber();
	if( command == C_JUMP || command == C_PARTS )
	{
		int cancelsel = 0;
		int ipg = old_page;
		int PART_PAGE = ipg;
		for( int iget=0; iget<str->GetSize(); iget++ )
		{
			CString ref = str->GetAt(iget);
			CText * sel_p = NULL;
			ipg = m_Doc->Pages.FindPart( &ref, &sel_p, TRUE, TRUE );
			if( ipg >= 0 && sel_p )
			{
				PART_PAGE = ipg;
				m_Doc->SwitchToPage( ipg );
				if( getbit( cancelsel, ipg ) == 0 )
				{
					CancelSelection( FALSE );
					setbit( cancelsel, ipg );
				}
				do
				{
					if( sel_p->m_polyline_start >= 0 )
					{
						if( m_Doc->m_outline_poly->GetAt( sel_p->m_polyline_start ).GetSel() == 0 )
							m_Doc->SelectPart( sel_p, NULL, 0 );
					}
					int next_page = m_Doc->Pages.FindPart( &ref, &sel_p, FALSE, TRUE );
					if( next_page != ipg )
						break;
				}while( sel_p );
			}
			else
			{
				ipg = old_page;
				int ip_el = -1;
				sel_p = m_Doc->Pages.FindCPartElement( &ref, &ipg, &ip_el );
				if( ipg >= 0 && sel_p )
				{
					PART_PAGE = ipg;
					m_Doc->SwitchToPage( ipg );
					if( getbit( cancelsel, ipg ) == 0 )
					{
						CancelSelection( FALSE );
						setbit( cancelsel, ipg );
					}
					do
					{
						if( sel_p->m_polyline_start >= 0 )
						{
							if( m_Doc->m_outline_poly->GetAt( sel_p->m_polyline_start ).GetSel() == 0 )
								m_Doc->SelectPart( sel_p, NULL, 0 );
						}
						int next_page = old_page;
						sel_p = m_Doc->Pages.FindCPartElement( &ref, &next_page, &ip_el );
						if( next_page != ipg )
							break;
					}while( sel_p );
				}
				else
				{
					ref += "&";
					ipg = m_Doc->Pages.FindPart( &ref, &sel_p, TRUE, TRUE );
					if( ipg >= 0 && sel_p )
					{
						PART_PAGE = ipg;
						m_Doc->SwitchToPage( ipg );
						if( getbit( cancelsel, ipg ) == 0 )
						{
							CancelSelection( FALSE );
							setbit( cancelsel, ipg );
						}
						do
						{
							if( sel_p->m_polyline_start >= 0 )
							{
								if( m_Doc->m_outline_poly->GetAt( sel_p->m_polyline_start ).GetSel() == 0 )
									m_Doc->SelectPart( sel_p, NULL, 0 );
							}
							int next_page = m_Doc->Pages.FindPart( &ref, &sel_p, FALSE, TRUE );
							if( next_page != ipg )
								break;
						}while( sel_p );
					}
				}
			}
		}	
		//
		m_Doc->SwitchToPage( PART_PAGE, TRUE, 0 );
		m_sel_flags = PART_ONLY;
		if( m_sel_count )
		{
			if( m_dlg_partlist )
				if( m_dlg_partlist->IsWindowVisible() )
					if( m_dlg_partlist->GetPCBButtonCheck() )
						m_dlg_partlist->ImportPartlist();
			SetCursorMode( CUR_GROUP_SELECTED );
			HighlightGroup();
		}
		else if( bPageChanged )
			OnRangeCmds( NULL );
		JumpToPart( NULL, NULL, (IronScale?1:0) );// jump to selected rect
		bPageChanged = 0;
	}
	else if( command == C_PIN )
	{
		CString ref = str->GetAt(0);
		CString pin = str->GetAt(1);
		CText * sel_p = NULL;
		CText * mem_p = NULL;
		int sw_to_page = m_Doc->Pages.FindPart( &ref, &sel_p, TRUE, TRUE );
		if( sel_p && ((sw_to_page>=0 && IronScale==0) || (sw_to_page==old_page)) )
		{
			m_Doc->SwitchToPage( sw_to_page );
			CancelSelection( FALSE );
			do
			{	
				CText * PIN = NULL;
				for( int ip=sel_p->m_polyline_start; ip>=0; ip=m_Doc->m_outline_poly->GetAt(ip).Next[index_part_attr] )
				{
					if( m_Doc->m_outline_poly->GetAt(ip).Check( index_pin_attr ) )
						if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.Compare( pin ) == 0 )
							PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
						else if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
						{
							CArray<CString> arr;
							int npins = ParseMultiPin( &m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str, &arr );
							for( int ipin=0; ipin<npins; ipin++ )
								if( arr[ipin].Compare( pin ) == 0 )
									PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
						}
				}
				if( PIN )
					NewSelectP( sel_p, PIN, FALSE, 15 );
				else
					mem_p = sel_p;
				int next_page = m_Doc->Pages.FindPart( &ref, &sel_p, FALSE, TRUE );
				if( next_page != sw_to_page )
					break;
			}while( sel_p );
		}
		else
		{
			sw_to_page = old_page;
			int ip_el = -1;
			BOOL show_block = (IronScale && m_Doc->Pages.IsThePageIncludedInNetlist( old_page ));
			sel_p = m_Doc->Pages.FindCPartElement( &ref, &sw_to_page, &ip_el, show_block );
			if( sel_p && ((sw_to_page>=0 && IronScale==0) || (sw_to_page==old_page)) )
			{
				m_Doc->SwitchToPage( sw_to_page );
				CancelSelection( FALSE );
				if( show_block )// when mouse move in pcb editor
				{
					NewSelectB( sel_p );
				}
				else do
				{
					CText * PIN = NULL;
					for( int ip=sel_p->m_polyline_start; ip>=0; ip=m_Doc->m_outline_poly->GetAt(ip).Next[index_part_attr] )
					{
						if( m_Doc->m_outline_poly->GetAt(ip).Check( index_pin_attr ) )
							if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.Compare( pin ) == 0 )
							{
								PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
								NewSelectP( sel_p, PIN, FALSE, 15 );
							}
							else if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
							{
								CArray<CString> arr;
								int npins = ParseMultiPin( &m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str, &arr );
								for( int ipin=0; ipin<npins; ipin++ )
									if( arr[ipin].Compare( pin ) == 0 )
									{
										PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
										NewSelectP( sel_p, PIN, FALSE, 15 );
									}
							}
					}
					int next_page = old_page;
					sel_p = m_Doc->Pages.FindCPartElement( &ref, &next_page, &ip_el );
					if( next_page != sw_to_page )
						break;
				} while( sel_p );
			}
			else
			{
				ref += "&";
				sw_to_page = m_Doc->Pages.FindPart( &ref, &sel_p, TRUE, TRUE );
				if( sel_p && ((sw_to_page>=0 && IronScale==0) || (sw_to_page==old_page)) )
				{
					m_Doc->SwitchToPage( sw_to_page );
					CancelSelection( FALSE );
					do
					{	
						CText * PIN = NULL;
						for( int ip=sel_p->m_polyline_start; ip>=0; ip=m_Doc->m_outline_poly->GetAt(ip).Next[index_part_attr] )
						{
							if( m_Doc->m_outline_poly->GetAt(ip).Check( index_pin_attr ) )
								if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.Compare( pin ) == 0 )
									PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
								else if( m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
								{
									CArray<CString> arr;
									int npins = ParseMultiPin( &m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr]->m_str, &arr );
									for( int ipin=0; ipin<npins; ipin++ )
										if( arr[ipin].Compare( pin ) == 0 )
											PIN = m_Doc->m_outline_poly->GetAt(ip).pAttr[index_pin_attr];
								}
						}
						if( PIN )
							NewSelectP( sel_p, PIN, FALSE, 15 );
						else
							mem_p = sel_p;
						int next_page = m_Doc->Pages.FindPart( &ref, &sel_p, FALSE, TRUE );
						if( next_page != sw_to_page )
							break;
					}while( sel_p );
				}
			}
		}
		if( m_sel_count )
		{
			if( IronScale == 0 )
				SetCursorMode( CUR_GROUP_SELECTED );
		}
		else if( mem_p )
			NewSelectP( mem_p, NULL, FALSE );
		else if( bPageChanged )
			OnRangeCmds( NULL );
		if( IronScale )
			m_sel_count = 0;
		JumpToPart( NULL, NULL, (IronScale?2:0) );// jump to selected rect
		if( old_page != m_Doc->Pages.GetActiveNumber() )
			InvalidateLeftPane();
		bPageChanged = 0;
	}
	else if( command == C_REOPEN )
	{
		CString file = m_Doc->m_pcb_full_path;
		m_Doc->FileClose();
		m_Doc->FileOpen( file );
	}
	else if( command == C_FILE )
	{
		if( str->GetSize() )
		{
			int pg = m_Doc->Pages.ReturnPageByPCB( &str->GetAt(0) );
			if( pg >= 0 )
			{
				int pcb = m_Doc->Pages.GetPcbIndex( pg );
				if( pcb >= 0 && pcb != m_Doc->Pages.GetPcbIndex( m_Doc->Pages.GetActiveNumber() ) )
				{
					m_Doc->SwitchToPage( pg, 0, 0 );
					bPageChanged = 1;
				}
			}
		}
	}
	else if( command == C_VDP )
	{
		m_Doc->OnToolsClearDrc();
		FindAttr(0,0,&str->GetAt(0));
		m_Doc->m_drelist->MakeSolidCircles();
	}
	else if( command == C_SEL_M )
	{
		int num = m_Doc->m_mlist->GetIndex( str->GetAt(0) );
		NewSelectM( num );
		JumpToPart( NULL, NULL, 0 );
	}
}


CWnd * CFreePcbView::OnInfoBoxSendMess( CString mess )
{
	COPYDATASTRUCT cd;
	int m_len = mess.GetLength();
    cd.cbData = m_len + 1;
	char * data;
	data = new char[m_len+1];//new022
    for (int cp=m_len; cp>0; cp--)
		data[cp-1] = mess[cp-1];
	data[m_len] = '\0';
	cd.lpData = data;
	cd.cbData = m_len+1;
	CWnd * Find = FindWindow(NULL, "Infobox");
	int RES = 0;
	if( Find )
	{
		HWND InfoBox; 
		InfoBox = Find->GetSafeHwnd();
		if( InfoBox )
			RES = ::SendMessage(InfoBox, WM_COPYDATA, 0, (LPARAM)&cd);
	}
	delete data;//new022
	if( RES )
		return Find;
	else
		return NULL;
}

// new selected element
BOOL CFreePcbView::GetSelectedItem()
{
	// 1: outlines
	if( getbit(m_sel_flags, FLAG_SEL_OP) )
	{
		for( int item=m_Doc->m_outline_poly->GetSize()-1; item>=0; item-- )
		{
			int gns = m_Doc->m_outline_poly->GetAt(item).GetNumCorners()-1;
			int cl = m_Doc->m_outline_poly->GetAt(item).GetClosed();
			for(int ic=gns; ic>=0; ic--)
			{
				if( m_Doc->m_outline_poly->GetAt(item).GetSel(ic) )
				{
					m_sel_id = m_Doc->m_outline_poly->GetAt(item).GetId();
					m_sel_id.i = item;
					m_sel_id.sst = ID_CORNER;
					m_sel_id.ii = ic;
					return true;
				}
				if( cl == 0 && ic == gns )
					continue;
				if( m_Doc->m_outline_poly->GetAt(item).GetSideSel(ic) )
				{
					m_sel_id = m_Doc->m_outline_poly->GetAt(item).GetId();
					m_sel_id.i = item;
					m_sel_id.sst = ID_SIDE;
					m_sel_id.ii = ic;
					return true;
				}
			}
		}
	}
	// 2: texts
	if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
	{
		m_sel_id.Set(ID_TEXT_DEF);
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if(!tl)
				continue;
			int it = -1;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
				if ( t->m_selected )
				{
					m_sel_text = t;
					return true;
				}
		}
	}
	m_sel_text = NULL;
	m_sel_count = 0;
	m_sel_id.Clear();
	return 0;
}


int  CFreePcbView::UnSelect( void * ptr, id * sid, BOOL bSET_CURSOR_MODE, BOOL GSI )
{
	int m = -1;
	// Unselect text
	if ( sid->type == ID_TEXT )
	{
		CText * t = (CText*)ptr;
		if(t)
		{
			if( t->m_selected )
			{
				t->m_selected = 0;
				m_sel_count--;
			}
		}
	} 
	// Unselect outline
	else if ( sid->type == ID_POLYLINE )
	{
		if (sid->sst == ID_SIDE)
		{
			BOOL sel = m_Doc->m_outline_poly->GetAt(sid->i).GetSideSel( sid->ii );
			if( sel )
			{
				m_Doc->m_outline_poly->GetAt(sid->i).SetSideSel( sid->ii, 0 );
				m_sel_count--;
			}
		}
		else if (sid->sst == ID_CORNER)		
		{
			BOOL sel = m_Doc->m_outline_poly->GetAt(sid->i).GetSel( sid->ii );
			if( sel )
			{
				m_Doc->m_outline_poly->GetAt(sid->i).SetSel( sid->ii, 0 );
				m_sel_count--;
			}
		}
	}
	if( sid->type == m_sel_id.type && 
		sid->st == m_sel_id.st &&
		sid->sst == m_sel_id.sst &&
		sid->i == m_sel_id.i &&
		sid->ii == m_sel_id.ii )
		m_sel_id.Clear();
	if( m_sel_id.type == ID_NONE && GSI )
	{
		// select prev
		if( GetSelectedItem() == 0 )
			CancelSelection();
	}
	if( bSET_CURSOR_MODE && m_sel_count )
	{
		if( m_sel_id.type == ID_TEXT )
			NewSelect( m_sel_text, &m_sel_id, TRUE, FALSE );
		else
			NewSelect( NULL, &m_sel_id, TRUE, FALSE );
	}
	return 0;
}

// new selected element
int CFreePcbView::NewSelect( void * ptr, id * sid, BOOL bSet_cursor_mode, BOOL bInvert )
{	
	int m = -1;

	// select text
	if ( sid->type == ID_TEXT )
	{
		CText * t = (CText*)ptr;
		if(t)
		{
			m = t->m_merge;
			if( t->m_selected == 0 )
			{
				setbit( m_sel_flags, FLAG_SEL_TEXT );
				t->m_selected = 1;
				m_sel_count++;
			}
			else if( bInvert )
			{
				UnSelect( t, sid, bSet_cursor_mode );
				return m;
			}
			m_sel_text = t;
			m_sel_id = *sid;
			if( bSet_cursor_mode && m_sel_count == 1 )
			{
				if( t->m_polyline_start >= 0 )
				{
					int layer = m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetLayer();
					if( layer < LAY_ADD_1 )
					{
						m_active_layer = layer;
						ShowActiveLayer( m_active_layer );
					}
				}
				m_Doc->m_dlist->CancelHighLight();
				CTextList * tl = t->m_tl;
				if(tl)
					tl->HighlightText(t);
				int i_a = 0;
				switch( t->m_layer )
				{
				case LAY_PART_NAME:
				case LAY_PART_VALUE:
				case LAY_FOOTPRINT:
					i_a = index_part_attr; 
					break;
				case LAY_PIN_NAME:
					i_a = index_pin_attr; 
					break;
				case LAY_NET_NAME:
					i_a = index_net_attr; 
					break;
				case LAY_PIN_DESC:
					i_a = index_desc_attr; 
					break;
				}
				int TR = TRANSPARENT_BACKGND;
				if( tl == m_Doc->m_tlist )
				{
					TR = 0;
				}
				BOOL BOM = 0;
				if( tl == m_Doc->m_tlist )
					m_Doc->HighLightNet(t);
				else
				{
					int n_att = index_part_attr;
					if( t->m_layer == LAY_NET_NAME )
						n_att = index_net_attr;
					else if( t->m_layer == LAY_PIN_NAME )
						n_att = index_pin_attr;
					else if( t->m_layer == LAY_PIN_DESC || t->m_layer >= LAY_ADD_1 )
					{
						if( t->m_str.Right(3) == "BOM" )
						{
							BOM = 1;
							for( int ico=m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetNumSides()-1; ico>=0; ico-- )
								m_Doc->m_outline_poly->GetAt(t->m_polyline_start).SetSideSel(ico,1);
							m_sel_count += m_Doc->m_outline_poly->GetAt(t->m_polyline_start).GetNumSides();
						}
						n_att = index_desc_attr;
					}
					for( int get = t->m_polyline_start; get >= 0; get = m_Doc->m_outline_poly->GetAt(get).Next[n_att] )
						for( int ico=m_Doc->m_outline_poly->GetAt(get).GetNumCorners()-1; ico>=0; ico-- )
							m_Doc->m_outline_poly->GetAt(get).HighlightSide( ico, max(m_Doc->m_outline_poly->GetAt(get).GetW()*3,t->m_stroke_width), TRANSPARENT_HILITE );
				}
				if( BOM )
				{
					SetCursorMode( CUR_GROUP_SELECTED );
					setbit( m_sel_flags, FLAG_SEL_OP ); // set bit after SetCursorMode !
				}
				else
					SetCursorMode( CUR_TEXT_SELECTED );
			}
		}
	} 
	// select poly
	else if ( sid->type == ID_POLYLINE && sid->i < m_Doc->m_outline_poly->GetSize() )
	{	
		if( sid->ii < m_Doc->m_outline_poly->GetAt(sid->i).GetNumCorners() )
		{
			m = m_Doc->m_outline_poly->GetAt(sid->i).GetMerge();
			if (sid->sst == ID_SIDE)
			{	
				BOOL sel = m_Doc->m_outline_poly->GetAt(sid->i).GetSideSel( sid->ii );
				if( !sel )
				{
					setbit( m_sel_flags, FLAG_SEL_OP );
					setbit( m_sel_flags, FLAG_SEL_SIDE );
					m_Doc->m_outline_poly->GetAt(sid->i).SetSideSel( sid->ii, 1 );
					m_sel_count++;
				}
				else if( bInvert )
				{
					UnSelect( NULL, sid, bSet_cursor_mode );
					return m;
				}
				m_sel_id = *sid;
				if( m_Doc->m_outline_poly->GetAt(sid->i).m_visible )
				if( bSet_cursor_mode && m_sel_count == 1 )
				{		
					m_Doc->m_dlist->CancelHighLight();
					m_Doc->HighlightLineAttributes( &m_Doc->m_outline_poly->GetAt(sid->i) );
					for( int ico=m_Doc->m_outline_poly->GetAt(sid->i).GetNumSides()-1; ico>=0; ico-- )
						if( ico!=sid->ii )
							m_Doc->m_outline_poly->GetAt(sid->i).HighlightSide( ico, m_Doc->m_outline_poly->GetAt(sid->i).GetW(), TRANSPARENT_HILITE );
					m_Doc->m_outline_poly->GetAt(sid->i).HighlightSide( sid->ii );
					SetCursorMode( CUR_OP_SIDE_SELECTED );
					m_polyline_layer = m_Doc->m_outline_poly->GetAt(sid->i).GetLayer();
					m_polyline_hatch = m_Doc->m_outline_poly->GetAt(sid->i).GetHatch();
					m_polyline_width = m_Doc->m_outline_poly->GetAt(sid->i).GetW();
					m_polyline_closed = m_Doc->m_outline_poly->GetAt(sid->i).GetClosed();
				}
			}
			else if (sid->sst == ID_CORNER)		
			{
				BOOL sel = m_Doc->m_outline_poly->GetAt(sid->i).GetSel( sid->ii );
				if( !sel )
				{
					setbit( m_sel_flags, FLAG_SEL_OP );
					setbit( m_sel_flags, FLAG_SEL_VTX );
					m_Doc->m_outline_poly->GetAt(sid->i).SetSel( sid->ii, 1 );
					m_sel_count++;
				}
				else if( bInvert )
				{
					UnSelect( NULL, sid, bSet_cursor_mode );
					return m;
				}
				m_sel_id = *sid;
				if( m_Doc->m_outline_poly->GetAt(sid->i).m_visible )
				if( bSet_cursor_mode && m_sel_count == 1 )
				{
					m_Doc->m_dlist->CancelHighLight();
					HighlightGroup();
					SetCursorMode( CUR_OP_CORNER_SELECTED );
				}
			}
		}
	}
	// select dre
	if( sid->type == ID_DRC && sid->st == ID_DRE )
	{
		setbit( m_sel_flags, FLAG_SEL_DRE );
		m_sel_dre = (DRError*)ptr;
		if( m_sel_dre )
		{
			m_sel_id = *sid;
			m_sel_count++;
			if( bSet_cursor_mode && m_sel_count == 1 )
			{
				m_Doc->m_drelist->HighLight( m_sel_dre );
				SetCursorMode( CUR_DRE_SELECTED );
				RECT dre_r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
				m_Doc->m_dlist->HighlightRect(&dre_r, LAY_DRC_ERROR, TRANSPARENT_BACKGND );
			}
		}
	}
	
	// select group
	if( bSet_cursor_mode && m_sel_count > 1 )
	{
		m_Doc->m_dlist->CancelHighLight();
		SetCursorMode( CUR_GROUP_SELECTED );
		HighlightGroup();
	}
	return m;
}

// marking
void CFreePcbView::MarkAllOutlinePoly( int utility, int layer )
{
	// mark all corners of solder mask cutouts
	for( int im=0; im<m_Doc->m_outline_poly->GetSize(); im++ )
	{
		CPolyLine * poly = &m_Doc->m_outline_poly->GetAt(im);
		if( poly->GetLayer() != layer && layer >= 0 )
			continue;
		poly->SetUtility(utility);
		for( int ic=poly->GetNumCorners()-1; ic>=0; ic-- )
			poly->SetUtility( ic, utility );	
	}
}

void CFreePcbView::ScaleFactor( double scale, BOOL bTOTAL )
{
	FindGroupCenter();
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
		if( p->GetSideSel() )
		{
			for( int ii=0; ii<p->GetNumCorners(); ii++ )
			{
				if( p->GetSideSel( ii ) )
				{
					int icont = p->GetContour(ii);
					ii = p->GetContourStart(icont);
					int i2 = p->GetContourEnd(icont);
					while( ii <= i2 )
					{
						float x = p->GetX(ii);
						float y = p->GetY(ii);
						x -= (float)groupAverageX;
						y -= (float)groupAverageY;
						x *= scale;
						y *= scale;
						x += (float)groupAverageX;
						y += (float)groupAverageY;
						p->SetX(ii,x);
						p->SetY(ii,y);
						ii++;
					}
					ii = i2;
				}
			}
			p->ModifyGroupRect(&rect(0,0,0,0));
			p->RecalcRectC(0);
			int src = p->GetPicture();
			if( src >= 0 )
			{
				RECT r = p->GetCornerBounds();
				m_Doc->m_dlist->MoveTemplate( &r, src, p->GetLayer() );
			}
			if( bTOTAL )
			{
				int newW = (float)p->GetW()*scale;
				p->SetW(newW);
			}
			p->Draw( m_Doc->m_dlist );
		}
	}
	for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
	{
		CTextList * tl = m_Doc->GetTextListbyLayer( lay );
		if( !tl || tl == m_Doc->m_tlist )
			continue;
		int it = -1;
		for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
		{
			if( t->m_selected )
			{
				float x = t->m_x;
				float y = t->m_y;
				x -= (float)groupAverageX;
				y -= (float)groupAverageY;
				x *= scale;
				y *= scale;
				x += (float)groupAverageX;
				y += (float)groupAverageY;
				t->m_x = x;
				t->m_y = y;
				if( bTOTAL )
				{
					t->m_font_size = (float)t->m_font_size*scale;
					t->m_stroke_width = (float)t->m_stroke_width*scale;
				}
				if( t->m_str.Right(3) == "BOM" )
					DrawBOM( t );
				else
					t->Draw( m_Doc->m_dlist, m_Doc->m_smfontutil );
			}
		}
	}
	m_Doc->ProjectModified(TRUE);
	m_Doc->m_dlist->CancelHighLight();
	HighlightGroup();
	m_draw_layer = LAY_HILITE;// ScaleFactor
	Invalidate( FALSE );// (doubtful)
}

void CFreePcbView::OnTextReplace()
{
	if( m_sel_flags != TEXT_ONLY )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"First select only the text you want to replace!":
			"Сначала выберите только тексты, в той области доски, где хотите заменить!");
		return;
	}
	DlgReplaceText dlg;
	if( dlg.DoModal() == IDOK )
	{
		// clear memory
		m_Doc->CheckComponentInLibrary(); 

		CString Str = dlg.m_str1;
		CString StrR = dlg.m_str2;
		int cnt = 0;
		int err = 0;
		SaveUndoInfoForGroup( m_Doc->m_undo_list );
		CArray<CText*> txt_arr;
		for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
		{
			CTextList * tl = m_Doc->GetTextListbyLayer( lay );
			if( !tl || tl == m_Doc->m_tlist )
				continue;
			int it = -1;
			int bStringChecked = 0;
			for(CText* t=tl->GetNextText(&it); t; t=tl->GetNextText(&it))
			{
				if( t->m_selected )
				{
					if( bStringChecked == 0 )
					{
						if( t->m_layer == LAY_PART_NAME )
						{
							if( StrR.FindOneOf( ILLEGAL_REF ) >= 0 )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"The Reference Designator must not contain a special character!":
									"Условное обозначение не должно содержать специальных символов!");
								err++;
								continue;
							}
							bStringChecked = 1;
						}
						else if( t->m_layer == LAY_NET_NAME )
						{
							if( StrR.FindOneOf( ILLEGAL_NET ) >= 0 )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"The Net Name must not contain a special character!":
									"Имя эл.цепи не должно содержать специальных символов!");
								err++;
								continue;
							}
							bStringChecked = 1;
						}
						else if( t->m_layer == LAY_PIN_NAME )
						{
							if( StrR.FindOneOf( ILLEGAL_PIN ) >= 0 )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"The Pin Name must not contain a special character!":
									"Имя пина не должно содержать специальных символов!");
								err++;
								continue;
							}
							bStringChecked = 1;
						}
						else if( t->m_str.Right(7) == "BLK_PTR" || t->m_str.Right(3) == "BLK" )
						{
							if( StrR.FindOneOf( ILLEGAL_CP ) >= 0 )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"The Complex Part must not contain a special character!":
									"Иерархическая деталь не должна содержать специальных символов!");
								err++;
								continue;
							}
						}
					}

					// ok
					if( t->m_str.Find(Str) >= 0 )
					{
						txt_arr.Add(t);
					}
				}
			}
		}
		CancelSelection();
		for( int i=0; i<txt_arr.GetSize(); i++ )
		{
			if( CText * t = txt_arr.GetAt(i) )
			{
				CString old_str = t->m_str;
				CString oldV = "", oldF = "";
				CText * txtR = NULL;

				// BLK RENUMBERING
				if( t->m_str.Right(3) == "BLK" )
				{
					id ID(ID_TEXT_DEF);
					NewSelect( t, &ID, 0, 0 ); 
					ID.Set( ID_POLYLINE, ID_GRAPHIC, t->m_polyline_start, ID_SIDE, 0 );
					NewSelect( NULL, &ID, 0, 0 ); 
					old_str.Replace(Str,StrR);
					if( old_str.GetLength() == 0 )
						err++;
					else
					{
						CString errstr = OPSetAttributes( &old_str ); 
						if( errstr.GetLength() )
							err++;
						else
							cnt++;
					}
					CancelSelection();
					continue;
				}
				else if( t->m_tl == m_Doc->Attr->m_Reflist ||
					     t->m_tl == m_Doc->Attr->m_Footlist ||
					     t->m_tl == m_Doc->Attr->m_Valuelist )
				{
					CPolyLine * txtP = &m_Doc->m_outline_poly->GetAt( t->m_polyline_start );
					txtR = txtP->pAttr[index_part_attr];
					CText * txtV = txtP->pAttr[index_value_attr];
					CText * txtF = txtP->pAttr[index_foot_attr];
					if( txtV )
						oldV = txtV->m_str;
					if( txtF ) 
						oldF = txtF->m_str;
				}
				if( t->m_layer == LAY_PART_NAME )
				{
					CString tests = old_str;
					tests.Replace(Str,StrR);
					CText * FPart = NULL;
					m_Doc->Pages.FindPart( &tests, &FPart, TRUE, TRUE );
					if( FPart )
					{
						err++;
						t->m_str = old_str;
					}
					else
					{
						t->Undraw();
						t->m_str.Replace(Str,StrR);
						cnt++;
					}
				}
				else
				{
					int len = t->m_str.GetLength();
					t->Undraw();
					t->m_str.Replace(Str,StrR);
					if( t->m_str.GetLength() == 0 )
					{
						err++;
						t->m_str = Str;
						if( len != t->m_str.GetLength() )
							cnt++;
					}
					else
						cnt++;
				}
				t->m_nchars = t->m_str.GetLength();
				t->MakeVisible();

				// add component to lib
				if( txtR && oldV.GetLength() && oldF.GetLength() )
				{
					m_Doc->CheckComponentInLibrary( txtR, &oldV, &oldF );
				}

				// renumber part
				if( t->m_tl == m_Doc->Attr->m_Reflist )
					if( t->m_str.Compare( old_str ) )
					{
						m_Doc->Renumber__Add( &old_str, &t->m_str, m_Doc->Pages.GetActiveNumber() );
					}
			}
		}
		Str.Format(G_LANGUAGE == 0 ? "%d fragments modified, %d error(s)":"%d фрагментов изменено, %d ошибок", cnt, err);
		AfxMessageBox ( Str );
		m_Doc->ProjectModified(TRUE);
		m_draw_layer = LAY_BACKGND;
		Invalidate( FALSE );// (doubtful)
	}
}

void CFreePcbView::OnTextChangeV()
{
	OnTextChangeAttr( index_value_attr );
}

void CFreePcbView::OnTextChangeF()
{
	OnTextChangeAttr( index_foot_attr );
}

void CFreePcbView::OnTextChangeAttr( int iatt )
{
	if( m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return;
	}
	int it = -1;
	for( CText * ref=m_Doc->Attr->m_Reflist->GetNextText(&it); ref; ref=m_Doc->Attr->m_Reflist->GetNextText(&it) )
	{
		if( ref->m_polyline_start >= 0 )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt(ref->m_polyline_start);
			if( CText * V = p->Check( iatt ) )
			{
				id ID( ID_TEXT_DEF );		
				int iatt = -1;
				for( CText * Att=m_Doc->ScanAttr(p, &iatt); Att; Att=m_Doc->ScanAttr(p, &iatt) )
				{
					if( Att == V )
						continue;
					if( Att->m_selected )
					{				
						UnSelect( Att, &ID, 0, 0 );
						if( V->m_selected == 0 )
							NewSelect( V, &ID, 0, 0 );
					}
				}
				if( V->m_selected == 0 )
					if( p->GetSideSel() )
						NewSelect( V, &ID, 0, 0 );
			}
		}
	}
	if( m_sel_id.type == ID_TEXT )
		OnTextEdit();
}

void CFreePcbView::JumpToPart( CText * ref, CText * pin, int IronScale )
{
	RECT r=rect(0,0,0,0);
	if( ref )
		r = m_Doc->GetPartBounds(ref,pin);
	else
	{
		r = m_Doc->m_dlist->GetHighlightedBounds( NULL );
		if( r.left || r.bottom )
			SwellRect( &r, NM_PER_MM );
	}
	if( (r.right - r.left) == 0 )
	{
		///AfxMessageBox("No items found");  !!!
		OnRangeCmds(NULL);
		return;
	}
	
	RECT cl_r = m_Doc->m_dlist->GetWindowRect();
	cl_r.bottom *=	m_pcbu_per_wu;
	cl_r.left *=	m_pcbu_per_wu;
	cl_r.right *=	m_pcbu_per_wu;
	cl_r.top *=		m_pcbu_per_wu;

	BOOL NO = 0;
	if( IronScale > 1 )
		NO = TRUE;
	else if( RectsIntersection( r, cl_r ) == 1 )
		if( (cl_r.right - cl_r.left) > (r.right-r.left) )
			NO = TRUE;

	int dx = ((cl_r.right+cl_r.left)/2) - ((r.right+r.left)/2);
	int dy = ((cl_r.top+cl_r.bottom)/2) - ((r.top+r.bottom)/2);
	if( IronScale == 0 )
	{
		int selT = m_Doc->Attr->m_Reflist->GetSelCount( m_Doc->m_outline_poly );
		if( selT <= 1 )
		{
			int sw = (r.right-r.left+r.top-r.bottom);
			RECT pR;
			m_Doc->GetPolylineBounds( &pR );
			int lim = min( pR.right-pR.left, pR.top-pR.bottom )/4;
			if( sw < lim )
				SwellRect( &r, (pin?sw*5:sw) );
		}
		else
			SwellRect( &r, (r.right-r.left+r.top-r.bottom)/8 ); 
		if( (cl_r.right - cl_r.left) > (r.right-r.left) )
		{
			MoveRect( &cl_r, -dx, -dy );
			r = cl_r;
		}
		ViewAllElements(&r);
		OnRangeCmds(NULL);
	}
	else 
	{
		if( NO == 0 )
		{
			MoveRect( &cl_r, -dx, -dy );
			ViewAllElements(&cl_r);
		}
		m_draw_layer = LAY_HILITE;
		Invalidate( FALSE );
		InvalidateLeftPane();
	}
	if( m_Doc->last_el == NULL )
	{
		if( pin )
		{
			if( pin->dl_el )
				m_Doc->last_el = m_Doc->m_dlist->CopyToLayer( pin->dl_el, LAY_INFO_TEXT );
		}
		else if( ref )
		{
			if( ref->dl_el )
				m_Doc->last_el = m_Doc->m_dlist->CopyToLayer( ref->dl_el, LAY_INFO_TEXT );
		}
		if( m_Doc->last_el )
		{
			m_Doc->m_dlist->HighLight( m_Doc->last_el );
			m_Doc->last_el->transparent = TRANSPARENT_BLACK_GROUND;
			m_Doc->last_el->el_static = 1;
			CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
			frm->SetTimer( TMR_SW_STATIC, 150, 0 );
		}
	}
}

// update BOM list
// for current page only 
void CFreePcbView::DrawBOM( CText * t, CArray<CPolyLine> * Arr )
{
	if(  t->m_str.Right(3) == "BOM" )
	{
		if( t->m_angle%360 != 0 )
			t->m_angle = 0;
		{
			if( Arr == NULL )
				Arr = m_Doc->m_outline_poly;
			int iof = t->m_str.Find("|start_number:");
			if( iof > 0 )
			{
				CString cmd = t->m_str.Right( t->m_str.GetLength() - iof );
				cmd.Replace("'"," ");
				cmd.Replace(":"," ");
				CString Key;
				CArray<CString> arr;
				int ns = ParseKeyString( &cmd, &Key, &arr );
				if( ns >= 20 )
				{
					int is = my_atoi( &arr.GetAt(0) );
					int ie = my_atoi( &arr.GetAt(2) );
					int ss = my_atoi( &arr.GetAt(4) );
					int ex = my_atoi( &arr.GetAt(6) );
					int ig = my_atoi( &arr.GetAt(8) );
					int inclist = m_Doc->m_ref_lists->GetIndex( arr.GetAt(10) );
					int exlist = m_Doc->m_ref_lists->GetIndex( arr.GetAt(12) );
					int  old_page = m_Doc->Pages.GetActiveNumber();
					CString pcb_n=OLD_NO_FP;
					int ioff = t->m_str.Find("|pcb_name:");
					if( ioff > 0 )
					{
						pcb_n = t->m_str.Right( t->m_str.GetLength() - ioff - 10 );
						ioff = pcb_n.Find("'");
						if( ioff > 0 )
							pcb_n.Truncate( ioff );
						pcb_n.Trim();
					}
					int new_page = m_Doc->Pages.ReturnPageByPCB( &pcb_n );
					if( new_page == -1 )
					{
						CString nf;
						nf.Format( "PCB file %s not found:-(", pcb_n );
						t->m_str = nf + t->m_str.Right(t->m_str.GetLength() - iof);
						t->m_nchars = t->m_str.GetLength();
						t->Draw( t->m_dlist, t->m_smfontutil );
					}
					else
					{
						if( new_page != old_page )
							m_Doc->Pages.SwitchToPage( new_page );
						if( inclist >= 0 )
							inclist += REF_LIST_INDEX;
						if( exlist >= 0 )
							exlist += REF_LIST_INDEX;
						int CPPresent = 0;
						if( ie > is && is > 0 )
							CPPresent = DrawBOM( &Arr->GetAt( t->m_polyline_start ), is, ie, ss, ex, ig, inclist, exlist, &arr.GetAt(14), &pcb_n );
						if( CPPresent )
							m_Doc->SwitchToPage( old_page, TRUE );
						else if( new_page != old_page )
							m_Doc->Pages.SwitchToPage( old_page );
					}
				}
				else
				{
					t->m_str = "ERROR" + t->m_str.Right(t->m_str.GetLength() - iof);
					t->m_nchars = t->m_str.GetLength();
					t->Draw( t->m_dlist, t->m_smfontutil );
				}
			}
		}
	}
}

void CFreePcbView::DrawPATH( CText * t, int ipage )
{
	if(  t->m_str.Right(4) == "PATH" )
	{
		int PATH=0,
			NAME=2,
			PCBN=4,
			EXT=6,
			LEN=8, 
			LOWER=10;
		//
		int iof = t->m_str.Find("|");
		if( iof > 0 )
		{
			CString INPUT = t->m_str.Left( iof-1 );
			CString cmd = t->m_str.Right( t->m_str.GetLength() - iof );
			CString svcmd = cmd;
			cmd.Replace("'"," ");
			cmd.Replace(":"," ");
			CString Key;
			CArray<CString> arr;
			int ns = ParseKeyString( &cmd, &Key, &arr );
			if( ns >= 12 )
			{
				int slash = m_Doc->m_pcb_full_path.ReverseFind('\\');
				if( slash > 0 )
				{
					CString OUTPUT="";
					//
					CString LEFT = m_Doc->m_pcb_full_path.Left(slash+1);
					if( arr.GetAt(PATH).Left(1) != "0" )
						OUTPUT += LEFT;
					//
					if( arr.GetAt(NAME).Left(1) != "0" )
					{
						LEFT = m_Doc->m_name;
						if( arr.GetAt(PCBN).Left(1) != "0" )
						{
							LEFT = m_Doc->Pages.GetCurrentPCBName( ipage );
							if( LEFT.GetLength() == 0 )
							{
								m_Doc->Pages.GetPageName( ipage, &LEFT );
								//LEFT = "No PCB attached"; 
								//arr.GetAt(EXT).SetAt( 0, '1' );
							}
						}
						if( arr.GetAt(EXT).Left(1) == "0" )
						{
							int rf = LEFT.ReverseFind('.');
							if( rf > 0 )
								LEFT = LEFT.Left(rf);
						}
						OUTPUT += LEFT;
					}
					//
					int maxlen = my_atoi(&arr.GetAt(LEN));
					maxlen = max( maxlen, 1 );
					int gLEN = OUTPUT.GetLength();
					for( int ii=gLEN-(gLEN%maxlen); ii>0; ii-=maxlen )
						OUTPUT.Insert(ii,"'");
					if( arr.GetAt(LOWER).Left(1) != "0" )
						OUTPUT.MakeLower();
					//redraw
					CDisplayList * dl = t->m_dlist;
					SMFontUtil * sm = t->m_smfontutil;
					t->Undraw();
					if( INPUT.Right( OUTPUT.GetLength() ) == OUTPUT )
						OUTPUT = INPUT;
					t->m_str = OUTPUT + "'" + svcmd;
					t->m_nchars = t->m_str.GetLength();
					t->Draw(dl,sm);
				}
			}
		}
	}
}

BOOL CFreePcbView::DrawBOM( CPolyLine * p, 
							int iStart, 
							int iEnd, 
							int sortStyle, 
							int Extended, 
							BOOL ignore_without_value, 
							int inc_list, 
							int ex_list, 
							CString * order,
							CString * pcb )
{
	if( p->GetNumCorners() < 4 )
		return 0;
	enum{
		ByItem=1,
		ByValue,
		ByFootprint,
		ByCount,
		ByDetails
	};
	enum{
		col_Item=1,
		col_Value,
		col_Footprint,
		col_Count,
		col_Details
	};
	CString SORT = CSORT;
	RECT r = p->GetBounds();
	if( r.right - r.left < NM_PER_MM || r.top - r.bottom < NM_PER_MM )
		return 0;
	//
	CString V="", P="", D="";
	int bCPUsed = 0;
	int cnt = m_Doc->Pages.FindBOMNextLine(&V, &P, &D, Extended, ignore_without_value, inc_list, ex_list, TRUE, &bCPUsed );
	if( cnt >= 0 )
	{
		int nTabs = order->GetLength();
		int added_cols = ((Extended-10)/7);
		if( order->Find("2") >=0 ) //col_Value
			nTabs++;
		if( order->Find("3") >=0 ) //col_Footprint
			nTabs++;
		if( order->Find("5") >=0 ) //col_Details
		{
			nTabs++;
			nTabs += added_cols;
		}
		//
		CArray<CString> VALUES;
		CArray<CString> PACKAGES;
		CArray<CString> DETAILS;
		CArray<int> CNT;
		VALUES.RemoveAll();
		PACKAGES.RemoveAll();
		DETAILS.RemoveAll();
		CNT.RemoveAll();
		do
		{
			if( cnt )
			{
				CString cV=V;
				int sep = cV.Find(CP_LINK);
				if( sep > 0 )
					cV = cV.Left( sep );
				sep = cV.Find("|");
				if( sep > 0 )
					cV.Delete( sep );
				VALUES.Add(cV);
				PACKAGES.Add(P);
				DETAILS.Add(D);
				CNT.Add(cnt);
			}
			D = "";
			cnt = m_Doc->Pages.FindBOMNextLine(&V, &P, &D, Extended, ignore_without_value, inc_list, ex_list);
		}while( cnt >= 0 );
		CString TXT = "";
		int max_i;
		int prev_i = -1;
		int n_str = 1;
		int n_str_details = 0;

		// add pcb name
		TXT += "PCB:  " + *pcb;
		TXT += "'";
		n_str_details++;

		// add header
		for( int item=0; item<order->GetLength(); item++ )
		{
			CString ch = order->GetAt(item);
			int ni = my_atoi(&ch);
			//
			if( ni == col_Item )
				TXT += "ITEM`";
			else if( ni == col_Value )
				TXT += "VALUE``";
			else if( ni == col_Footprint )
				TXT += "FOOTPRINT``";
			else if( ni == col_Count )
				TXT += "CNT`";
			else if( ni == col_Details )
			{
				TXT += "DETAILS``";
				for( int icol=0; icol<added_cols; icol++ )
					TXT += "`";
			}
		}
		TXT += "'";
		for( int item=0; item<nTabs; item++ )
		{
			TXT += "`";
		}
		TXT += "#'";
		n_str_details++;
		do
		{
			max_i = -1;
			int max = 0;
			float stat_f = (float)INT_MAX/4.0;
			for( int i=0; i<CNT.GetSize(); i++ )
			{
				if( sortStyle == ByCount )
				{
					if( CNT[i] > max )
					{
						max_i = i;
						max = CNT[i];
					}
					else if( CNT[i] == max && max_i >= 0 )
					{
						if( prev_i >= 0 )
						{
							if( VALUES[max_i].CompareNoCase(VALUES[prev_i]) && VALUES[i].CompareNoCase(VALUES[prev_i]) == 0 )
								max_i = i;
							else if( VALUES[max_i].Left(2).CompareNoCase(VALUES[prev_i].Left(2)) && VALUES[i].Left(2).CompareNoCase(VALUES[prev_i].Left(2)) == 0 )
								max_i = i;
						}
						if( max_i != i )
						{
							if( SORT.Find( VALUES[max_i].Left(1) ) > SORT.Find( VALUES[i].Left(1) ) )
								max_i = i;
						}
					}
				}
				else if( sortStyle == ByValue )
				{
					if( CNT[i] != -1 )
					{
						if( max_i == -1 )
							max_i = i;
						else 
						{
							if( prev_i >= 0 )
							{
								if( VALUES[max_i].CompareNoCase(VALUES[prev_i]) && VALUES[i].CompareNoCase(VALUES[prev_i]) == 0 )
									max_i = i;
								else if( VALUES[max_i].Left(2).CompareNoCase(VALUES[prev_i].Left(2)) && VALUES[i].Left(2).CompareNoCase(VALUES[prev_i].Left(2)) == 0 )
									max_i = i;
							}
							if( max_i != i )
							{
								if( SORT.Find( VALUES[max_i].Left(1) ) > SORT.Find( VALUES[i].Left(1) ) )
									max_i = i;
							}
						}
					}
				}
				else if( sortStyle == ByFootprint )
				{
					if( CNT[i] != -1 )
					{
						if( max_i == -1 )
							max_i = i;
						else 
						{
							if( prev_i >= 0 )
							{
								if( PACKAGES[max_i].CompareNoCase(PACKAGES[prev_i]) && PACKAGES[i].CompareNoCase(PACKAGES[prev_i]) == 0 )
									max_i = i;
								else if( PACKAGES[max_i].Left(2).CompareNoCase(PACKAGES[prev_i].Left(2)) && PACKAGES[i].Left(2).CompareNoCase(PACKAGES[prev_i].Left(2)) == 0 )
									max_i = i;
							}
							if( max_i != i )
							{
								if( SORT.Find( PACKAGES[max_i].Left(1) ) > SORT.Find( PACKAGES[i].Left(1) ) )
									max_i = i;
							}		
						}
					}
				}
				else if( sortStyle == ByDetails )
				{
					if( CNT[i] != -1 )
					{
						if( max_i == -1 )
							max_i = i;
						else
						{
							int sort1 = SORT.Find( DETAILS[max_i].Left(1) );
							int sort2 = SORT.Find( DETAILS[i].Left(1) );
							if( sort1 > sort2 )
								max_i = i;
							else if( sort1 == sort2 )
							{
								CString pref1, pref2, suff;
								int in1 = ParseRef(&DETAILS[max_i], &pref1, &suff);
								int in2 = ParseRef(&DETAILS[i], &pref2, &suff);
								if(pref2.GetLength() == 1 && pref1.GetLength() > 1)
									max_i = i;
								else if (pref2.Compare(pref1) == 0 && in1 > in2)
									max_i = i;
								else if (pref2.GetLength() >= 2 && pref1.GetLength() >= 2)
								{
									sort1 = SORT.Find(pref1.GetAt(1));
									sort2 = SORT.Find(pref2.GetAt(1));
									if (sort1 > sort2)
										max_i = i;
									else if (sort1 == sort2)
										if (in1 > in2)
											max_i = i;
								}
							}
						}
					}
				}
			}
			if( max_i >= 0 )
			{
				if( n_str >= iStart && n_str <= iEnd )
				{
					if( prev_i >= 0 )
						if( sortStyle == ByDetails )
							if( DETAILS[max_i].Left(1) != DETAILS[prev_i].Left(1) )
							{
								for( int item=0; item<nTabs; item++ )
								{
									TXT += "`";
								}
								TXT += "#'";
								n_str_details++;
							}
					int TAB = 0;
					int tab = 0;
					CString PostStrDetails = "";
					for( int item=0; item<order->GetLength(); item++ )
					{
						CString ch = order->GetAt(item);
						int ni = my_atoi(&ch);
						if( ni == col_Item )
						{
							CString a;
							a.Format("%d`", n_str );
							TXT += a;
							tab++;
						}
						else if( ni == col_Value )
						{
							for( int ioff = VALUES[max_i].Find("'"); ioff > 0; ioff = VALUES[max_i].Find("'") )
								VALUES[max_i].Delete(ioff);
							TXT += VALUES[max_i] + "``";
							tab += 2;
						}
						else if( ni == col_Footprint )
						{
							TXT += PACKAGES[max_i] + "``";
							tab += 2;
						}
						else if( ni == col_Count )
						{
							CString a;
							a.Format("%d`", CNT[max_i] );
							TXT += a;
							tab++;
						}
						else if( ni == col_Details )
						{
							TAB = tab;
							tab += 2;
							CString ps = DETAILS[max_i];
							int fnd=DETAILS[max_i].Find("'");
							if( fnd > 0 )
							{
								ps = DETAILS[max_i].Left(fnd);
								PostStrDetails = DETAILS[max_i].Right( DETAILS[max_i].GetLength() - fnd - 1 );
							}
							TXT += (ps + "``");
							for( int icol=0; icol<added_cols; icol++ )
								TXT += "`";
							for( int fnd=DETAILS[max_i].Find("'"); fnd>=0; fnd=DETAILS[max_i].Find("'",fnd+1) )
								n_str_details++;
						}
					}
					if( PostStrDetails.GetLength() )
					{
						CString rplc = "'";
						for( int t=0; t<TAB; t++ )
							rplc += "`";
						PostStrDetails.Replace("'",rplc);
						TXT += (rplc + PostStrDetails);
					}
					TXT += "'";
				}
				CNT[max_i] = -1;
				prev_i = max_i;
				n_str++;
			}
		}while( max_i >= 0 );
		if( TXT.GetLength() )
		{
			// ref lists
			CString list1 = "not_used";
			CString list2 = "not_used";
			if( inc_list > 0 )
				list1 = m_Doc->m_ref_lists->GetStr( inc_list - REF_LIST_INDEX );
			else if( ex_list > 0 )
				list2 = m_Doc->m_ref_lists->GetStr( ex_list - REF_LIST_INDEX );
			CString cmd;
			// insert comment
			//if( list1.Find("not_used") == -1 )
			//{
			//	cmd.Format( "(%s)%s", list1, TXT);
			//	TXT = cmd;
			//}
			// insert comment
			//if( list2.Find("not_used") == -1 )
			//{
			//	cmd.Format("(%s)%s", list2, TXT);
			//	TXT = cmd;
			//}
			cmd.Format( "|start_number: %d'|end_number: %d'|sorting_column: %d'|detail_column_width: %d'|ignore_those_without_value: %d'|include_ref_list: %s'|exclude_ref_list: %s'|column_order: %s'|pcb_name: %s'"COMMAND" BOM", iStart, iEnd, sortStyle, Extended, ignore_without_value, list1, list2, *order, *pcb );
			TXT += cmd;
			//
			//
			double pw = p->pAttr[index_desc_attr]->m_stroke_width;
			double StepX = (r.right - r.left)/nTabs;
			int End = min( iEnd, n_str-1 );
			double StepY = (r.top - r.bottom)/(End-iStart+3+n_str_details) - pw/2;
			StepY /= (float)m_Doc->m_alignment;
			StepY *= 0.5;
			CDisplayList * dl = p->pAttr[index_desc_attr]->m_dlist;
			SMFontUtil * fu = p->pAttr[index_desc_attr]->m_smfontutil;
			p->pAttr[index_desc_attr]->Undraw();
			///if( p->pAttr[index_desc_attr]->m_font_size < StepY/2 || p->pAttr[index_desc_attr]->m_font_size > StepY*2 )
			p->pAttr[index_desc_attr]->m_font_size = StepY;
			p->pAttr[index_desc_attr]->m_x = r.left + p->pAttr[index_desc_attr]->m_font_size*2;
			p->pAttr[index_desc_attr]->m_y = r.top - p->pAttr[index_desc_attr]->m_font_size - p->pAttr[index_desc_attr]->m_font_size*(float)m_Doc->m_alignment*5/4;
			p->pAttr[index_desc_attr]->m_str = TXT;
			p->pAttr[index_desc_attr]->m_nchars = TXT.GetLength();
			p->pAttr[index_desc_attr]->Draw( dl, fu, StepX );
		}
	}
	else
	{
		int ver = p->pAttr[index_desc_attr]->m_str.Find("|");
		if( ver > 0 )
		{
			int L = p->pAttr[index_desc_attr]->m_str.GetLength();
			p->pAttr[index_desc_attr]->m_str = p->pAttr[index_desc_attr]->m_str.Right( L-ver );
			p->pAttr[index_desc_attr]->m_str = "Empty Ref List!'" + p->pAttr[index_desc_attr]->m_str;
			p->pAttr[index_desc_attr]->m_nchars = p->pAttr[index_desc_attr]->m_str.GetLength();
			p->pAttr[index_desc_attr]->MakeVisible();
		}
	}
	return bCPUsed;
}

CString CFreePcbView::SelectBlock( CPolyLine * poly, int npage, BOOL bSelect  )
{
	RECT pr = poly->GetBounds();
	CText * desc = poly->pAttr[index_desc_attr];
	if( desc == NULL )
		return "Block not found";
	CString BNAME="", BLINK="", desc_text="";
	CString ret="";
	int PAGE = -1;
	int BPAGE= -1;
	int iof = desc->m_str.Find("|");
	if( iof >= 0 )
	{
		desc_text = desc->m_str.Right( desc->m_str.GetLength() - iof );
		BPAGE = m_Doc->ExtractBLK( &desc->m_str, &BNAME, &BLINK );
	}
	if( BNAME.GetLength() == 0 )
	{
		ret.Format(G_LANGUAGE == 0 ? 
			"Complex part has no suffix":
			"Ошибка: иерархическая деталь не имеет суффикса");
	}
	else if( BNAME[0] >= '0' && BNAME[0] <= '9' )
	{
		ret.Format(G_LANGUAGE == 0 ? 
			"The suffix of a complex part must not start with a number":
			"Суффикс у иерархической детали не должен начинаться с цифры. Идеальный вариант - использовать заглавные буквы A, B, C...");
	}
	
	if( CText * ref_err = poly->Check(index_part_attr) )
	{
		ret.Format(G_LANGUAGE == 0 ? 
			"It is impossible to combine a complex part with an ordinary part %s":
			"Невозможно объединить иерархическую деталь с обычной деталью %s", ref_err->m_str);
	}
	// save m_sel_id
	id save_id = m_sel_id;
	//
	int it = -1;
	CTextList * descList = m_Doc->Pages.GetTextList( npage, index_desc_attr );
	CArray<CPolyLine> * poly_arr = m_Doc->Pages.GetPolyArray( npage );
	CPolyLine * get_error = poly;
	BOOL ptr_present = 0;
	if( ret.GetLength() == 0 ) for( CText * t=descList->GetNextText(&it); t; t=descList->GetNextText(&it) )
	{
		if( t->m_str.Right(7) == "BLK_PTR" )
		{
			if( t->m_polyline_start >= 0 )
			{
				CString get_name, get_ref, get_pin;
				int get_np = m_Doc->ExtractBLK_PTR( &t->m_str, &get_name, &get_ref, &get_pin );
				get_name = get_name.Trim();
				get_error = &poly_arr->GetAt(t->m_polyline_start);
				RECT getr = get_error->GetBounds();
				if( RectsIntersection( pr, getr ) >= 0 )
				{
					ptr_present = TRUE;
					if( CText * ref_err = get_error->Check(index_part_attr) )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"It is impossible to combine a complex part with an ordinary part %s":
							"Невозможно объединить иерархическую деталь с обычной деталью %s", ref_err->m_str);
						get_error = NULL;
						return ret;
					}
					if( get_name.GetLength() == 0 )
					{
						CString fstr = CP_SUFFIX;
						int f = t->m_str.Find( fstr );
						if( f > 0 )
						{
							CDisplayList * dl = t->m_dlist;
							SMFontUtil * sm = t->m_smfontutil;
							t->Undraw();
							t->m_str.Insert(f+fstr.GetLength()," "+BNAME);
							t->m_nchars = t->m_str.GetLength();
							t->Draw(dl,sm);
							// get data again
							get_np = m_Doc->ExtractBLK_PTR( &t->m_str, &get_name, &get_ref, &get_pin );
						}
						else
						{
							ret.Format(G_LANGUAGE == 0 ? 
								"Pin pointer must contain the suffix %s of the complex part":
								"Указатель пина должен содержать суффикс %s иерархической детали.", BNAME);
							break;
						}
					}
					if( get_name.Compare(BNAME) )
					{
						CString fstr = CP_SUFFIX + get_name;
						int f = t->m_str.Find( fstr );
						if( f < 0 )
						{
							fstr = CP_SUFFIX" " + get_name;
							f = t->m_str.Find( fstr );
						}
						if( f > 0 )
						{
							t->Undraw();
							CString newstr = CP_SUFFIX" " + BNAME;
							t->m_str.Replace( fstr, newstr );
							t->m_nchars = t->m_str.GetLength();
							t->MakeVisible();
							get_np = m_Doc->ExtractBLK_PTR( &t->m_str, &get_name, &get_ref, &get_pin );
						}
						else
						{
							ret.Format(G_LANGUAGE == 0 ? 
								"Complex part contains different suffixes %s and %s.":
								"Ошибка: составная деталь содержит разные суффиксы %s и %s.", BNAME, get_name);
							break;
						}
					}
					if( BLINK.GetLength() )
					{
						if( BLINK.FindOneOf( ILLEGAL_CP ) >= 0 )
						{
							CString illegal = ILLEGAL_CP;
							ret.Format(G_LANGUAGE == 0 ? 
								"Complex part link must not contain special characters (%s)\nLINK: %s":
								"Составная деталь не должна содержать специальные символы (%s)\nLINK: %s", illegal, BLINK);
						}
						if( get_ref.Compare( BLINK ) )
						{
							CString fstr = CP_LINK + get_ref;
							int f = t->m_str.Find( fstr );
							if( f < 0 )
							{
								fstr = CP_LINK" " + get_ref;
								f = t->m_str.Find( fstr );
							}
							if( f > 0 )
							{
								t->Undraw();
								CString newstr = CP_LINK" " + BLINK;
								t->m_str.Replace( fstr, newstr );
								t->m_nchars = t->m_str.GetLength();
								t->MakeVisible();
								get_np = m_Doc->ExtractBLK_PTR( &t->m_str, &get_name, &get_ref, &get_pin );
							}
						}
					}
					else
						BLINK = get_ref;
					//
					if( BPAGE >= 0 )
					{
						if( BPAGE != get_np )
						{
							CString fstr = CP_PAGE_NUM;
							int f1 = t->m_str.Find( fstr );
							int f2 = t->m_str.Find( "'", f1 );
							if( f1 > 0 && f2 > 0 )
							{
								t->Undraw();
								t->m_str.Delete( f1, f2-f1 );	
								CString newstr;
								newstr.Format( CP_PAGE_NUM" %d", BPAGE+1 );
								t->m_str.Insert( f1, newstr );
								t->m_nchars = t->m_str.GetLength();
								t->MakeVisible();
								get_np = m_Doc->ExtractBLK_PTR( &t->m_str, &get_name, &get_ref, &get_pin );
							}
						}
					}
					else
						BPAGE = get_np;
					if( BPAGE == m_Doc->Pages.GetActiveNumber() )
					{
						ret = (G_LANGUAGE == 0 ? 
							"Complex part must link to different page":
							"Составная деталь должна ссылаться на другую страницу");
						get_error = poly;
						break;
					}
					if( get_name.FindOneOf( ILLEGAL_CP ) >= 0 )
					{
						CString illegal = ILLEGAL_CP;
						ret.Format(G_LANGUAGE == 0 ? 
							"Complex part must not contain special characters (%s)":
							"Ссылка не должна содержать специальные символы (%s)", illegal);
						get_error = poly;
						break;
					}
					get_ref.Trim();
					if( get_ref.GetLength() == 0 )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Complex part pin is missing a pointer to a part":
							"У пина составной детали отсутствует указатель на обозначение иерархического символа");
						break;
					}
					get_pin.Trim();
					if( get_pin.GetLength() == 0 )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Complex part pin is missing a pointer to a pin":
							"У составной детали отсутствует указатель на пин иерархического символа");
						break;
					}
					CTextList * get_PL = m_Doc->Pages.GetTextList( get_np, index_part_attr );
					if( get_PL == NULL )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Complex part pin points to a non-existent page %d":
							"Иерархическая деталь ссылается на несуществующую страницу %d", get_np + 1);
						get_error = poly;
						break;
					}
					int it2 = -1;
					CText * get_part = get_PL->GetText( &get_ref, &it2 );
					if( get_part == NULL )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Pointer error inside this complex part. Part %s not found on page %d":
							"Ошибка указателя внутри этой иерархической детали. Деталь %s не найдена на странице %d", get_ref, get_np + 1);
						get_error = poly;
						break;
					}
					//if( get_ref.Compare( BNAME ) )
					//{
					//	ret.Format("Different reference designators inside a complex part (%s and %s)", get_ref, BNAME );
					//	break;
					//}
					CPolyLine * part_poly = NULL;
					CPolyLine * pin_found = NULL;
					CArray<CPolyLine> * poly_arr2 = m_Doc->Pages.GetPolyArray( get_np );
					for( int get=get_part->m_polyline_start; get>=0; get=part_poly->Next[index_part_attr] )
					{
						part_poly = &poly_arr2->GetAt(get);
						if( CText * gpin = part_poly->Check( index_pin_attr ) )
							if( gpin->m_str.Compare( get_pin ) == 0 )
							{
								if( bSelect )
								{
									id ID = get_error->GetId();
									ID.sst = ID_SIDE;
									for( int ii=0; ii<get_error->GetNumSides(); ii++ )
									{
										ID.ii = ii;
										NewSelect( get_error, &ID, FALSE, FALSE );
									}
								}
								if( gpin->m_polyline_start >= 0 )
									pin_found = &poly_arr2->GetAt( gpin->m_polyline_start );
								break;
							}
					}
					if( pin_found == NULL )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Complex part contains a pointer to a non-existent pin %s.%s":
							"Иерархическая деталь содержит указатель на несуществующий пин %s.%s", get_ref, get_pin);
						break;
					}
					if( m_Doc->Pages.IsThePageIncludedInNetlist( get_np, TRUE ) )
					{	
						ret.Format(G_LANGUAGE == 0 ? 
							"Error! Complex part schematic pages should not be included in the netlist":
							"Ошибка! Страницы схем иерархических деталей не должны быть включены в список соединений");
						get_error = poly;
						break;
					}
				}
				else if( get_name.Compare(BNAME) == 0 )
				{
					if( get_ref.Compare(BLINK) == 0 )
					{
						ret.Format(G_LANGUAGE == 0 ? 
							"Error! The pin of a complex part does not touch the outline polyline":
							"Ошибка! Пин иерархической детали не касается контурной полилинии");
						get_error = NULL;
						break;
					}
				}
			}
		}
	}
	if( bSelect && BNAME.GetLength() ) // update description text
	{
		desc->Undraw();
		CString mName = BNAME;
		if( BNAME.Left(1)=="|" )
			mName.Delete(0);
		desc->m_str.Format( "%s (%s)'"CP_SUFFIX" %s'"CP_LINK" %s'"CP_PAGE_NUM" %d'"COMMAND" BLK", BLINK, mName, BNAME, BLINK, BPAGE+1 );
		desc->m_nchars = desc->m_str.GetLength();
		desc->MakeVisible();
		OnRangeCmds(NULL);
	}
	if( ret.GetLength() )
	{
		if( bSelect )
		{
			AfxMessageBox( ret, MB_ICONERROR );
			if( get_error && npage == m_Doc->Pages.GetActiveNumber() )
			{
				for( int e=0; e<2; e++ )
				{
					CancelSelection();
					id ID = get_error->GetId();
					ID.sst = ID_SIDE;
					ID.ii = 0;
					NewSelect( get_error, &ID, TRUE, FALSE );
					if( e == 0 )
						OnOPSetAttributes();
				}
			}
		}
		return ret;
	}
	else if( ptr_present == 0 )
	{
		ret.Format(G_LANGUAGE == 0 ? 
			"Error! Complex part has no pins":
			"Ошибка! Иерархическая деталь не имеет пинов");
		if( bSelect )
			AfxMessageBox( ret, MB_ICONERROR );
		return ret;
	}
	else if( bSelect )
	{
		id ID = poly->GetId();
		ID.sst = ID_SIDE;
		for( int ii=0; ii<poly->GetNumSides(); ii++ )
		{
			ID.ii = ii;
			NewSelect( poly, &ID, FALSE, FALSE );
		}
		if( save_id.type != ID_NONE )
			m_sel_id = save_id;
		HighlightGroup();
		setbit( m_sel_flags, FLAG_SEL_PART ); //
		SetCursorMode(CUR_GROUP_SELECTED);
	}
	return "";
}

BOOL CFreePcbView::OnRangeCmds( UINT CMD )
{
	m_draw_layer = 0;
	Invalidate( FALSE );//OnRangeCmds
	return !CMD;
}

void CFreePcbView::SelectTextLayer( int Layer, BOOL SEL )
{
	CTextList * tl = m_Doc->GetTextListbyLayer( Layer );
	if( tl )
	{
		id Id( ID_TEXT_DEF );
		int it = -1;
		for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
		{
			if( t->m_font_size )
			{
				if( SEL )
					NewSelect( t, &Id, 0, 0 );
				else
					UnSelect( t, &Id );
			}
		}
	}
}

void CFreePcbView::SelectPolylineLayer( int Layer, BOOL SEL )
{
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		if (m_Doc->m_outline_poly->GetAt(i).m_visible == 0)
			continue;
		if( m_Doc->m_outline_poly->GetAt(i).GetLayer() == Layer || Layer == 0 )
			for( int ii=0; ii<m_Doc->m_outline_poly->GetAt(i).GetNumSides(); ii++ )
			{
				id Id( ID_POLYLINE, ID_GRAPHIC, i, ID_SIDE, ii );
				if( SEL )
					NewSelect( NULL, &Id, 0, 0 );
				else
					UnSelect( NULL, &Id );
			}
	}
}

BOOL CFreePcbView::OnHidePictures( UINT CMD )
{
	CWnd* pMain = AfxGetMainWnd();
	if(!pMain)
		return 0;
	CMenu* pMenu = pMain->GetMenu();
	if(!pMenu)
		return 0;
	CMenu* submenu = pMenu->GetSubMenu(0);	// "File" submenu
	if(!submenu)
		return 0;
	if( m_Doc->m_dlist == NULL )
		return 0;
	if( CMD == ID_SHOW_BMP )
	{
		m_Doc->m_dlist->ShowImages( 1 );	
	}
	else if( CMD == ID_HIDE_BMP )
	{
		m_Doc->m_dlist->ShowImages( 0 );
	}
	if( m_Doc->m_dlist->m_show_pictures )
	{
		submenu->EnableMenuItem( ID_ATTACH_BMP1, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_ATTACH_BMP2, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_DETACH_BMP, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_HIDE_BMP, MF_BYCOMMAND | MF_ENABLED );
		submenu->EnableMenuItem( ID_SHOW_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
	else
	{
		submenu->EnableMenuItem( ID_ATTACH_BMP1, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_ATTACH_BMP2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_DETACH_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_HIDE_BMP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		submenu->EnableMenuItem( ID_SHOW_BMP, MF_BYCOMMAND | MF_ENABLED );
	}
	if( CMD )
		OnRangeCmds(NULL);
	return 1;
}

BOOL CFreePcbView::OnEditSelectLayer( UINT CMD )
{
	int SEL = 1;
	if( CMD >= ID_UNSEL_ATTRIBUTES )
		SEL = 0;
	switch(CMD)
	{
	// TEXTS
	case ID_SEL_ATTRIBUTES:
	case ID_UNSEL_ATTRIBUTES:
		SelectTextLayer( LAY_PIN_DESC, SEL );
		SelectTextLayer( LAY_NET_NAME, SEL );
		SelectTextLayer( LAY_PIN_NAME, SEL );
	case ID_SEL_REFVALUEFOOTPRINT:
	case ID_UNSEL_REFVALUEFOOTPRINT:
		SelectTextLayer( LAY_PART_NAME, SEL );
		SelectTextLayer( LAY_PART_VALUE, SEL );
		SelectTextLayer( LAY_FOOTPRINT, SEL );
		break;
	case ID_SEL_PARTNAME:
	case ID_UNSEL_PARTNAME:
		SelectTextLayer( LAY_PART_NAME, SEL );
		break;
	case ID_SEL_PARTVALUE:
	case ID_UNSEL_PARTVALUE:
		SelectTextLayer( LAY_PART_VALUE, SEL );
		break;
	case ID_SEL_FOOTPRINTNAME:
	case ID_UNSEL_FOOTPRINTNAME:
		SelectTextLayer( LAY_FOOTPRINT, SEL );
		break;
	case ID_SEL_DESCRIPTION:
	case ID_UNSEL_DESCRIPTION:
		SelectTextLayer( LAY_PIN_DESC, SEL );
		break;
	case ID_SEL_NETNAME:
	case ID_UNSEL_NETNAME:
		SelectTextLayer( LAY_NET_NAME, SEL );
		break;
	case ID_SEL_PINNAME:
	case ID_UNSEL_PINNAME:
		SelectTextLayer( LAY_PIN_NAME, SEL );
		break;
	// POLYLINES
	case ID_SEL_POLYLINES:
	case ID_UNSEL_POLYLINES:
		SelectPolylineLayer( 0, SEL );
		break;
	case ID_SEL_RSVPOLYLINES:
	case ID_UNSEL_RSVPOLYLINES:
		{
			for(int ilay=LAY_FREE_LINE; ilay<LAY_ADD_1; ilay++ )
				SelectPolylineLayer( ilay, SEL );
			break;
		}
	case ID_SEL_OWNERLESS:
	case ID_UNSEL_OWNERLESS:
		SelectPolylineLayer( LAY_FREE_LINE, SEL );
		break;
	case ID_SEL_PARTOUTLINE:
	case ID_UNSEL_PARTOUTLINE:
		SelectPolylineLayer( LAY_PART_OUTLINE, SEL );
		break;
	case ID_SEL_PINLINE:
	case ID_UNSEL_PINLINE:
		SelectPolylineLayer( LAY_PIN_LINE, SEL );
		break;
	case ID_SEL_NETPOLYLINE:
	case ID_UNSEL_NETPOLYLINE:
		SelectPolylineLayer( LAY_CONNECTION, SEL );
		break;
	case ID_SEL_FRONTLAYER1:
	case ID_UNSEL_FRONTLAYER1:
		SelectPolylineLayer( LAY_ADD_1, SEL );
		break;
	case ID_SEL_FRONTLAYER2:
	case ID_UNSEL_FRONTLAYER2:
		SelectPolylineLayer( LAY_ADD_2, SEL );
		break;
	case ID_SEL_BACKLAYER1:
	case ID_UNSEL_BACKLAYER1:
		SelectPolylineLayer( LAY_ADD_2+1, SEL );
		break;
	case ID_SEL_BACKLAYER2:
	case ID_UNSEL_BACKLAYER2:
		SelectPolylineLayer( LAY_ADD_2+2, SEL );
		break;
	//
	case ID_SEL_BACKLAYER3:
	case ID_UNSEL_BACKLAYER3:
		SelectPolylineLayer( LAY_ADD_2+3, SEL );
		break;
	case ID_SEL_BACKLAYER4:
	case ID_UNSEL_BACKLAYER4:
		SelectPolylineLayer( LAY_ADD_2+4, SEL );
		break;
	case ID_SEL_BACKLAYER5:
	case ID_UNSEL_BACKLAYER5:
		SelectPolylineLayer( LAY_ADD_2+5, SEL );
		break;
	case ID_SEL_BACKLAYER6:
	case ID_UNSEL_BACKLAYER6:
		SelectPolylineLayer( LAY_ADD_2+6, SEL );
		break;
	case ID_SEL_BACKLAYER7:
	case ID_UNSEL_BACKLAYER7:
		SelectPolylineLayer( LAY_ADD_2+7, SEL );
		break;
	case ID_SEL_BACKLAYER8:
	case ID_UNSEL_BACKLAYER8:
		SelectPolylineLayer( LAY_ADD_2+8, SEL );
		break;
	case ID_SEL_BACKLAYER9:
	case ID_UNSEL_BACKLAYER9:
		SelectPolylineLayer( LAY_ADD_2+9, SEL );
		break;
	case ID_SEL_BACKLAYER10:
	case ID_UNSEL_BACKLAYER10:
		SelectPolylineLayer( LAY_ADD_2+10, SEL );
		break;
	case ID_SEL_BACKLAYER11:
	case ID_UNSEL_BACKLAYER11:
		SelectPolylineLayer( LAY_ADD_2+11, SEL );
		break;
	case ID_SEL_BACKLAYER12:
	case ID_UNSEL_BACKLAYER12:
		SelectPolylineLayer( LAY_ADD_2+12, SEL );
		break;
	case ID_SEL_BACKLAYER13:
	case ID_UNSEL_BACKLAYER13:
		SelectPolylineLayer( LAY_ADD_2+13, SEL );
		break;
	case ID_SEL_BACKLAYER14:
	case ID_UNSEL_BACKLAYER14:
		SelectPolylineLayer( LAY_ADD_2+14, SEL );
		break;
	}
	if( m_sel_count )
	{
		SetCursorMode(CUR_GROUP_SELECTED);
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		OnRangeCmds(CMD);
	}
	return !CMD;
}

void CFreePcbView::SetMagnet( int mag )
{
	int w = m_polyline_width;
	if( mag )
	{
		m_magnetize = 1;
		double dim = NM_PER_MIL*m_Doc->m_dlist->m_scale*m_Doc->m_magnetize_value/100;
		CPoint p1(0,dim);
		CPoint p2(dim,0);
		CPoint p3(0,-dim);
		CPoint p4(-dim,0);
		m_Doc->m_dlist->StopDragging();
		m_Doc->m_dlist->MakeDragLineArray( 4 );
		m_Doc->m_dlist->AddDragLine( p1, p2 );
		m_Doc->m_dlist->AddDragLine( p3, p2 );
		m_Doc->m_dlist->AddDragLine( p1, p4 );
		m_Doc->m_dlist->AddDragLine( p3, p4 );
	}
	else
	{
		m_Doc->m_dlist->StopDragging();
		m_magnetize = 0;	
	}
	CDC * pDC = GetDC();
	if( m_cursor_mode == CUR_ADD_OP )
		m_Doc->m_dlist->StartDraggingArray( pDC, m_last_mouse_point.x, m_last_mouse_point.y, 2 );
	else if( m_cursor_mode == CUR_DRAG_OP_MOVE )
	{
		CPoint p = m_last_mouse_point;
		m_sel_op.StartDraggingToMoveCorner( pDC, m_sel_id.ii, p.x, p.y, 2 );
	}
	else if( m_cursor_mode == CUR_DRAG_OP_INSERT )
	{
		CPoint p = m_last_mouse_point;
		m_sel_op.StartDraggingToInsertCorner( pDC, m_sel_id.ii, p.x, p.y, 2 );
	}
	else
	{
		CPoint p = m_snap_angle_ref;
		m_Doc->m_dlist->StartDraggingArc( pDC, m_polyline_style, p.x, p.y, p.x, p.y, LAY_SELECTION, w, 2 );
	}
	ReleaseDC( pDC );
}

void CFreePcbView::UnSelVertices()
{
	int mem_sel_c = m_sel_count;
	if( getbit( m_sel_flags, FLAG_SEL_VTX ) )
		for( int ipo=0; ipo<m_Doc->m_outline_poly->GetSize(); ipo++ )
			if( m_Doc->m_outline_poly->GetAt(ipo).m_visible )
				m_sel_count -= m_Doc->m_outline_poly->GetAt(ipo).UnSelVertices();
	// clear flag
	clrbit( m_sel_flags, FLAG_SEL_VTX );
	if( mem_sel_c != m_sel_count )
	{
		GetSelectedItem();
		m_Doc->m_dlist->CancelHighLight();
		HighlightGroup();
		Invalidate( FALSE );
	}
	ShowSelectStatus();
}


void CFreePcbView::EditPolylineProperties()
{
	UnSelVertices();//!
	if( m_sel_id.type != ID_POLYLINE )
		return;
	m_polyline_layer = m_sel_op.GetLayer();
	m_polyline_closed = m_sel_op.GetClosed();
	AddOutlinePoly(0);
	m_Doc->m_dlist->CancelHighLight();
	HighlightGroup();
}

CText * CFreePcbView::FindSimilarPart( CText * RefText, int start_i )
{
	if( start_i >= m_Doc->clip_outline_poly.GetSize() )
		return NULL;
	CString pref, suff;
	ParseRef( &RefText->m_str, &pref, &suff );
	int it = -1;
	int start_item = 0;
	int next_item = 0;
	for( CText * t=(start_i==-1?m_Doc->Attr->m_Reflist->GetNextText(&it):m_Doc->clip_outline_poly.GetAt( start_item ).pAttr[index_part_attr]); 
				 t;		
				 t=(start_i==-1?m_Doc->Attr->m_Reflist->GetNextText(&it):m_Doc->clip_outline_poly.GetAt( next_item ).pAttr[index_part_attr]) )
	{
		if( start_i >= 0 )
		{
			if( next_item >= start_i )
				return NULL;
			start_item = next_item;
			CText * ref_txt = m_Doc->clip_outline_poly.GetAt( start_item ).pAttr[index_part_attr];
			while( m_Doc->clip_outline_poly.GetAt( next_item ).pAttr[index_part_attr] == ref_txt )
			{
				next_item++;
				if( next_item >= m_Doc->clip_outline_poly.GetSize() )
					return NULL;
			}
		}
		if( t == RefText )
			continue;
		if( t->m_str.Left(1) != RefText->m_str.Left(1) )
			continue;
		CString pref2, suff2;
		ParseRef( &t->m_str, &pref2, &suff2 );
		if( pref.Compare( pref2 ) == 0 )
		{
			CPolyLine * p1 = NULL;
			CPolyLine * p2 = NULL;
			if( RefText->m_polyline_start == -1 )
				continue;
			if( t->m_polyline_start == -1 )
				continue;
			if( start_i == -1 )
			{
				p1 = &m_Doc->m_outline_poly->GetAt( RefText->m_polyline_start );
				p2 = &m_Doc->m_outline_poly->GetAt( t->m_polyline_start );
			}
			else
			{
				p1 = &m_Doc->clip_outline_poly.GetAt( start_i );
				p2 = &m_Doc->clip_outline_poly.GetAt( start_item );
			}
			if( CText * f1 = p1->pAttr[index_foot_attr] )
				if( CText * f2 = p2->pAttr[index_foot_attr] )
					if( f1->m_str.Compare( f2->m_str ) )
						continue;
					else if( f1->m_str.Compare( NO_FOOTPRINT ) == 0 )
						continue;
			int np1=0, np2=0;
			if( start_i == -1 )
			{
				if( m_Doc->GetNumAllPins( RefText ) != m_Doc->GetNumAllPins( t ) )
					continue;
				// n poly ?
				for( int get=RefText->m_polyline_start; get>=0; get=p1->Next[index_part_attr] )
				{
					p1 = &m_Doc->m_outline_poly->GetAt( get );
					np1++;
				}
				for( int get2=t->m_polyline_start; get2>=0; get2=p2->Next[index_part_attr] )
				{
					p2 = &m_Doc->m_outline_poly->GetAt( get2 );
					np2++;
				}
				// mark all polylines of part 2
				for( int get2=t->m_polyline_start; get2>=0; get2=p2->Next[index_part_attr] )
				{
					p2 = &m_Doc->m_outline_poly->GetAt( get2 );
					p2->SetUtility(0);
				}
			}
			else
			{
				int n_pins1 = 0;
				for( int get=start_i; get<m_Doc->clip_outline_poly.GetSize(); get++ )
				{
					CPolyLine * polyline = &m_Doc->clip_outline_poly.GetAt(get);
					if( polyline->pAttr[index_pin_attr] )
						n_pins1++;
				}
				int n_pins2 = 0;
				for( int get=start_item; get<next_item; get++ )
				{
					CPolyLine * polyline = &m_Doc->clip_outline_poly.GetAt(get);
					if( polyline->pAttr[index_pin_attr] )
						n_pins2++;
				}
				if( n_pins1 != n_pins2 )
					continue;
				// n poly ?
				np1 = m_Doc->clip_outline_poly.GetSize() - start_i;
				np2 = next_item - start_item;
				//
				// mark all polylines of part 2
				for( int get2=start_item; get2<next_item; get2++ )
				{
					p2 = &m_Doc->clip_outline_poly.GetAt( get2 );
					p2->SetUtility(0);
				}
			}
			if( np1 != np2 )
				continue;

			
			int nmatch = 0;
			int MARK = 10;
			for( int get=(start_i==-1?RefText->m_polyline_start:start_i); get>=0; get=(start_i==-1?p1->Next[index_part_attr]:(get+1)) )
			{
				if( start_i == -1 )
					p1 = &m_Doc->m_outline_poly->GetAt( get );
				else if( get >= m_Doc->clip_outline_poly.GetSize() )
					break;
				else
					p1 = &m_Doc->clip_outline_poly.GetAt( get );
				// compare
				for( int get2=(start_i==-1?t->m_polyline_start:start_item); get2>=0; get2=(start_i==-1?p2->Next[index_part_attr]:(get2+1)) )
				{
					if( start_i == -1 )
						p2 = &m_Doc->m_outline_poly->GetAt( get2 );
					else if( get2 >= next_item )
						break;
					else
						p2 = &m_Doc->clip_outline_poly.GetAt( get2 );
					int NumS = p2->GetNumSides();
					if( p2->GetUtility() != MARK )
						if( p1->GetClosed() == p2->GetClosed() )
							//if( p1->GetHatch() == p2->GetHatch() )?
								//if( p1->GetLayer() == p2->GetLayer() )?
									if( p1->GetNumSides() == NumS )
									{
										int sstyle = 0;
										for( ; sstyle<NumS; sstyle++ )
										{
											int ss1 = p1->GetSideStyle( sstyle );
											int ss2 = p2->GetSideStyle( sstyle );
											if( ss1 != ss2 )
												break;
										}
										if( sstyle < NumS )
											continue;
										{
											CText * pin1 = p1->pAttr[ index_pin_attr ];
											CText * pin2 = p2->pAttr[ index_pin_attr ];
											if( pin1 && pin2 )
											{
												if( pin1->m_str.Compare( pin2->m_str ) )
													continue;
											}
											else if( pin1 == NULL && pin2 )
												continue;
											else if( pin1 && pin2 == NULL )
												continue;
										}
										{
											CText * d1 = p1->pAttr[ index_desc_attr ];
											CText * d2 = p2->pAttr[ index_desc_attr ];
											if( d1 && d2 )
											{
												if( d1->m_str.Compare( d2->m_str ) )
													continue;
											}
											else if( d1 == NULL && d2 )
												continue;
											else if( d1 && d2 == NULL )
												continue;
										}
										{
											CText * n1 = p1->pAttr[ index_net_attr ];
											CText * n2 = p2->pAttr[ index_net_attr ];
											if( n1 )
												if( n1->m_tl != m_Doc->Attr->m_Netlist )
													n1 = NULL;
											if( n2 )
												if( n2->m_tl != m_Doc->Attr->m_Netlist )
													n2 = NULL;
											if( n1 && n2 )
											{
												if( n1->m_str.Compare( n2->m_str ) )
													continue;
											}
											else if( n1 == NULL && n2 )
												continue;
											else if( n1 && n2 == NULL )
												continue;
										}
										p2->SetUtility(MARK);
										nmatch++;
										break;
									}
				}
			}
			if( nmatch == np1 )
				return t;
		}
	}
	return NULL;
}


void CFreePcbView::OnImportMerges()
{
	if( m_Doc->m_merge_library.IsEmpty() )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Objects have not been loaded. Press F1 to load objects":
			"Объекты не загружены. Нажмите F1, чтобы загрузить объекты", MB_ICONERROR);
		return;
	}
	CDlgImportMerge dlg;
	dlg.Initialize( &m_Doc->m_merge_library );
	if( dlg.DoModal() == IDOK )
	{
		CString s1="", s2="";
		int index = -1;
		CString SFile = m_Doc->m_merge_library.GetPath( &dlg.m_merger, dlg.m_current_path, &index );
		int xx=0, yy=0;
		FindStrInFile( &SFile, &s1, &s2, 0, 1, 1, &xx, &yy, -1, -1, index );
		//if( insertMode )
		{
			m_paste_flags = ~0; // paste from file
			OnGroupPaste( 0, 0 );
			CPoint point;
			RECT r;
			GetWindowRect( &r );
			//point.x = (r.left + r.right)/2;
			//point.y = (r.top + r.bottom)/2;
			//SetCursorPos( point.x, point.y );
		}
	}
}

BOOL CFreePcbView::OnApplyScale( UINT CMD )
{
	if( m_sel_count == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First select objects." : "Сначала выберите объекты.");
		return 0;
	}
	double sc = 1.0;
	if( CMD == ID_SCALE12 )
		sc = 1.0/2.0;
	else if( CMD == ID_SCALE13 )
		sc = 1.0/3.0;
	else if( CMD == ID_SCALE14 )
		sc = 1.0/4.0;
	else if( CMD == ID_SCALE15 )
		sc = 1.0/5.0;
	else if( CMD == ID_SCALE21 )
		sc = 2.0/1.0;
	else if( CMD == ID_SCALE23 )
		sc = 2.0/3.0;
	else if (CMD == ID_SCALE25)
		sc = 2.0/5.0;
	else if (CMD == ID_SCALE31)
		sc = 3.0 / 1.0;
	else if (CMD == ID_SCALE32)
		sc = 3.0 / 2.0;
	else if (CMD == ID_SCALE34)
		sc = 3.0 / 4.0;
	else if (CMD == ID_SCALE35)
		sc = 3.0 / 5.0;
	else if (CMD == ID_SCALE41)
		sc = 4.0 / 1.0;
	else if (CMD == ID_SCALE43)
		sc = 4.0 / 3.0;
	else if (CMD == ID_SCALE45)
		sc = 4.0 / 5.0;
	else if (CMD == ID_SCALE51)
		sc = 5.0 / 1.0;
	else if (CMD == ID_SCALE52)
		sc = 5.0 / 2.0;
	else if (CMD == ID_SCALE53)
		sc = 5.0 / 3.0;
	else if (CMD == ID_SCALE54)
		sc = 5.0 / 4.0;

	RECT R = m_Doc->m_dlist->GetHighlightedBounds(NULL);
	if( R.right-R.left > DEFAULT/sc/2 || R.top-R.bottom > DEFAULT/sc/2 )
		return 0;
	m_Doc->SelectGroupAttributes();
	SaveUndoInfoForGroup( m_Doc->m_undo_list );
	ScaleFactor( sc, TRUE );
	OnRangeCmds( NULL );
	return 0;
}

BOOL CFreePcbView::OnCreateMultiPinPartTemplate( UINT CMD )
{
	CancelSelection();
	CreateMultiPinPartTemplate( m_Doc, CMD, CMD-ID_0_PINS, m_Doc->m_polyline_w, m_attr_size.H_pin, m_Doc->m_part_grid_spacing );
	if( m_sel_count )
	{
		//SaveUndoInfoForGroup( m_Doc->m_undo_list, TRUE );

		//SetCursorPos(... â öåíòð ýêðàíà
		RECT r;
		GetWindowRect( &r );
		CPoint pnt;
		pnt.x = (r.left + r.right)/2;
		pnt.y = (r.top + r.bottom)/2;
		SetCursorPos( pnt.x, pnt.y );

		//MoveGroup(... â öåíòð ýêðàíà
		CPoint p = m_Doc->m_dlist->ScreenToPCB( pnt );
		MoveGroup( p.x, p.y );
		m_last_mouse_point = p;
		OnGroupMove();
	}
	return 0;
}

void CFreePcbView::FootprintNameTest( CString * FP, CString * OLD_FP )
{
	// footprint name test
	if( FP->GetLength() )
	{
		BOOL FIND = FindFootprint( m_Doc, FP );
		if( FIND == 0 )
		{
			int modified = FindFootprintInFolder( m_Doc, FP, OLD_FP );
			if( modified == 0 )
			{
				m_Doc->m_dlg_log->OnShowMe(); 
				if( m_Doc->m_FootprintNames.GetSize() )
				{
					if( FP->Compare( NO_FOOTPRINT ) )
						m_Doc->m_dlg_log->AddLine(	"\r\n\r\n WARNING! The footprint was not found"\
											"\r\n     in the current footprint library folder,"\
											"\r\n     which is specified in the project"\
											"\r\n     settings (Project >> Options)\r\n" );
					else
						m_Doc->m_dlg_log->ShowWindow( SW_HIDE );
				}
				else
					m_Doc->m_dlg_log->AddLine(	"\r\n\r\n WARNING! The footprint library folder"\
											"\r\n     is not specified in the project"\
											"\r\n     settings (see 'Project >> Options' menu item)\r\n" );
			}
			else if( modified > 1 )
			{
				// correct footprint name
				*FP = m_Doc->m_FootprintNames.GetAt( m_Doc->m_FootprintNames.GetSize()-1 );
			}
		}
	}
}

void CFreePcbView::OnGroupSaveToOpenscadFile()
{
	CStdioFile f;
	if( f.Open( m_Doc->m_app_dir+"\\open.scad", CFile::modeCreate | CFile::modeWrite, NULL ) )
	{
		int sFont = 1;
		if( getbit(m_sel_flags, FLAG_SEL_TEXT) )
			if( AfxMessageBox(G_LANGUAGE == 0 ? 
				"Want to use Openscad font to reduce file size?":
				"Хотите использовать шрифт Openscad для оптимизации размера генерируемого файла?", MB_YESNO) == IDYES)
				sFont = 0;
		double h = NM_PER_MM;
		h /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
		CString s, hstr;
		hstr.Format("%.3f", h );
		s.Format( "$fn = 50;\nh_ex = %s;\n\npolylines();\ntexts();\n\nmodule polylines () {\n", hstr );
		f.WriteString( s );
		if( ::RemoveColinearSegments( m_Doc->m_outline_poly ) )
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Some segments were removed due to collinearity":
				"Некоторые сегменты были удалены из-за коллинеарности");
		for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
		{
			if( m_Doc->m_outline_poly->GetAt(i).GetSel() == 0 )
				continue;
			double w = m_Doc->m_outline_poly->GetAt(i).GetW();
			w /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
			w *= m_user_scale;
			hstr = "h_ex";
			CString data = m_Doc->m_outline_poly->GetAt(i).GetOpenscadPolyPts( m_Doc->m_units, 50, 10, &hstr, m_user_scale );
			s = "";
			if( m_Doc->m_outline_poly->GetAt(i).GetClosed() && m_Doc->m_outline_poly->GetAt(i).GetHatch() )
			{
				s.Format("w%d = %.3f;\nlinear_extrude(h_ex, center=true, convexity=2)\noffset(w%d/2)\npolygon(", i, w, i);
				f.WriteString( s );
				f.WriteString( data );
				f.WriteString(");\n");
			}
			else
			{
				s += (m_Doc->m_outline_poly->GetAt(i).GetNumCorners()>2?"":"//");
				s += ("union()\n{\n");
				f.WriteString( s );
				f.WriteString( data );
				f.WriteString("\n}\n");
			}
		}
		f.WriteString("}\n\n");
		s.Format( "module texts () {\n" );
		f.WriteString( s );
		for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
		{
			for( CText * t=m_Doc->ScanAttr( &m_Doc->m_outline_poly->GetAt(i) ); t; t=m_Doc->ScanAttr( &m_Doc->m_outline_poly->GetAt(i) ) )
			{
				if( t->m_selected )
				{
					if( sFont )
					{
						CArray<CPoint> * pts = t->m_dlist->Get_Points( t->dl_el, NULL, 0 );
						int npts = pts->GetSize();
						CPoint * PTS = new CPoint[npts];
						t->m_dlist->Get_Points( t->dl_el, PTS, &npts );
						for( int ipt=0; ipt<npts-1; ipt+=2 )
						{
							CPoint OPTS[20];
							int np2 = Gen_HollowLinePoly( PTS[ipt].x, PTS[ipt].y, PTS[ipt+1].x, PTS[ipt+1].y, abs(t->m_stroke_width), OPTS, 20 );
							s.Format("linear_extrude(%s*2, center=true, convexity=2)\n  polygon([", hstr);
							CString S = s;
							for( int ihp=0; ihp<np2; ihp++ )
							{
								double x = OPTS[ihp].x;
								double y = OPTS[ihp].y;
								x /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
								y /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
								x *= m_user_scale;
								y *= m_user_scale;
								if( ihp )
								{
									S += ", ";
									S += "\n";
									for( int j=1; j<12; j++ )
										S += " ";
								}
								s.Format( "[%.3f, %.3f]", x, y );
								S += s;
							}
							S += "]);\n";
							f.WriteString( S );
						}
					}
					else
					{
						double x = t->m_x;
						x /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
						x *= m_user_scale;
						double y = t->m_y;
						y /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
						y *= m_user_scale;
						double z = 0.0;
						double fs = t->m_font_size;
						fs /= (double)(m_Doc->m_units==MM?NM_PER_MM:NM_PER_MIL);
						fs *= m_user_scale;
						CString S = t->m_str;
						S.Replace("'","\n");
						int rf = S.Find("|");
						if( rf > 0 )
							S.Truncate(rf);
						s.Format("translate([%.3f, %.3f, %.3f])\n  rotate(%d)\n    linear_extrude(%s*2, center=true, convexity=2)\n      text(\"%s\", %.3f);\n", x, y, z, -t->m_angle, hstr, S, fs);
						f.WriteString( s );
					}
				}
			}
		}
		f.WriteString("}");
		f.Close();
	}
	ShellExecute(	NULL, "open", "open.scad", NULL, m_Doc->m_app_dir, SW_SHOWNORMAL );
}
void CFreePcbView::OnGroupSaveToFPCFile()
{
	CStdioFile f;
	if (f.Open(m_Doc->m_app_dir + "\\buf.fpc", CFile::modeCreate | CFile::modeWrite, NULL))
	{
		//int sFont = 1;
		//if (getbit(m_sel_flags, FLAG_SEL_TEXT))
		//	if (AfxMessageBox(G_LANGUAGE == 0 ?
		//		"Want to use Openscad font to reduce file size?" :
		//		"Хотите использовать шрифт Openscad для оптимизации размера генерируемого файла?", MB_YESNO) == IDYES)
		//		sFont = 0;
		CString s;
		f.WriteString("[options]\n");
		f.WriteString("file_version: 2.427\n");
		f.WriteString("[footprints]\n");
		f.WriteString("[board]\n");
		f.WriteString("[solder_mask_cutouts]\n");
		f.WriteString("[graphics]\n");
		if (::RemoveColinearSegments(m_Doc->m_outline_poly))
			AfxMessageBox(G_LANGUAGE == 0 ?
				"Some segments were removed due to collinearity" :
				"Некоторые сегменты были удалены из-за коллинеарности");
		for (int i = 0; i < m_Doc->m_outline_poly->GetSize(); i++)
		{
			if (m_Doc->m_outline_poly->GetAt(i).GetSel() == 0)
				continue;
			int numc = m_Doc->m_outline_poly->GetAt(i).GetNumCorners();
			int htch = m_Doc->m_outline_poly->GetAt(i).GetHatch();
			int L = 10;// m_Doc->m_outline_poly->GetAt(i).GetLayer();
			int W = m_Doc->m_outline_poly->GetAt(i).GetW();
			int clsd = m_Doc->m_outline_poly->GetAt(i).GetClosed();
			s.Format("%s: %d %d %d %d -1 -1\n", clsd ? "polyline" : "line", numc, htch, L, W);
			f.WriteString(s);
			for (int ii = 0; ii < m_Doc->m_outline_poly->GetAt(i).GetNumCorners(); ii++)
			{
				int gx1 = m_Doc->m_outline_poly->GetAt(i).GetX(ii);
				int gy1 = m_Doc->m_outline_poly->GetAt(i).GetY(ii);
				int stl = m_Doc->m_outline_poly->GetAt(i).GetSideStyle(ii);
				int icont = m_Doc->m_outline_poly->GetAt(i).GetContour(ii);
				int clf = m_Doc->m_outline_poly->GetAt(i).GetContourEnd(icont);
				s.Format("corner: %d %d %d %d %d\n", ii+1, gx1, gy1, stl, clf);
				f.WriteString(s);
			}
		}
		for (int iatt = 0; iatt < num_attributes; iatt++)
		{
			int itxt = -1;
			int current = m_Doc->Pages.GetActiveNumber();
			int ipag = current;
			for (CText* t = m_Doc->Pages.GetNextAttr(iatt, &itxt, &ipag); t && ipag == current; t = m_Doc->Pages.GetNextAttr(iatt, &itxt, &ipag))
			{
				if (t->m_selected)
				{
					CArray<CPoint>* pts = t->m_dlist->Get_Points(t->dl_el, NULL, 0);
					int npts = pts->GetSize();
					CPoint* PTS = new CPoint[npts];
					t->m_dlist->Get_Points(t->dl_el, PTS, &npts);
					for (int ipt = 0; ipt < npts - 1; ipt += 2)
					{
						int numc = 2;
						int htch = 0;
						int L = 7;
						int W = t->dl_el->el_w * m_pcbu_per_wu;
						int clsd = 0;
						s.Format("line: %d %d %d %d -1 -1\n", numc, htch, L, W);
						f.WriteString(s);
						for (int ii = 0; ii < numc; ii++)
						{
							int gx1 = PTS[ipt + ii].x * m_user_scale;
							int gy1 = PTS[ipt + ii].y * m_user_scale;
							int stl = 0;
							s.Format("corner: %d %d %d %d 0\n", ii + 1, gx1, gy1, stl);
							f.WriteString(s);
						}
					}
					delete PTS;
				}
			}
		}
		f.WriteString("[parts]\n");
		f.WriteString("[nets]\n");
		f.WriteString("[texts]\n");
		f.WriteString("[merges]\n");
		f.WriteString("[end]\n");
		f.Close();
	}
	if(G_LANGUAGE)
		ShellExecute(NULL, "open",
			"\"" + m_Doc->m_app_dir + "\\ПлатФорм.exe\"",
			"\"" + m_Doc->m_app_dir + "\\buf.fpc\"",
			m_Doc->m_app_dir, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, "open",
			"\"" + m_Doc->m_app_dir + "\\FreePcb.exe\"",
			"\"" + m_Doc->m_app_dir + "\\buf.fpc\"",
			m_Doc->m_app_dir, SW_SHOWNORMAL);
}
void CFreePcbView::OnGroupSaveToDXFFile()
{
	CStdioFile f;
	if (f.Open(m_Doc->m_app_dir + "\\open.dxf", CFile::modeCreate | CFile::modeWrite, NULL))
	{
		//int sFont = 1;
		//if (getbit(m_sel_flags, FLAG_SEL_TEXT))
		//	if (AfxMessageBox(G_LANGUAGE == 0 ?
		//		"Want to use Openscad font to reduce file size?" :
		//		"Хотите использовать шрифт Openscad для оптимизации размера генерируемого файла?", MB_YESNO) == IDYES)
		//		sFont = 0;
		double h = (double)(m_Doc->m_units == MM ? NM_PER_MM : NM_PER_MIL);
		CString s;
		f.WriteString("0\n");
		f.WriteString("SECTION\n");
		f.WriteString("2\n");
		f.WriteString("ENTITIES\n");
		if (::RemoveColinearSegments(m_Doc->m_outline_poly))
			AfxMessageBox(G_LANGUAGE == 0 ?
				"Some segments were removed due to collinearity" :
				"Некоторые сегменты были удалены из-за коллинеарности");
		for (int i = 0; i < m_Doc->m_outline_poly->GetSize(); i++)
		{
			if (m_Doc->m_outline_poly->GetAt(i).GetSel() == 0)
				continue;
			int L = m_Doc->m_outline_poly->GetAt(i).GetLayer();
			for (int ii = 0; ii < m_Doc->m_outline_poly->GetAt(i).GetNumSides(); ii++)
			{
				double gx1 = m_Doc->m_outline_poly->GetAt(i).GetX(ii);
				double gy1 = m_Doc->m_outline_poly->GetAt(i).GetY(ii);
				int inx = m_Doc->m_outline_poly->GetAt(i).GetNextCornerIndex(ii);
				double gx2 = m_Doc->m_outline_poly->GetAt(i).GetX(inx);
				double gy2 = m_Doc->m_outline_poly->GetAt(i).GetY(inx);
				int stl = m_Doc->m_outline_poly->GetAt(i).GetSideStyle(ii);
				if (stl == 0)
				{
					f.WriteString("0\n");
					f.WriteString("LINE\n");
					f.WriteString("8\n");
					s = layer_str[L];
					f.WriteString(s + "\n");
					f.WriteString("10\n");
					s.Format("%.3f\n", gx1 / h * m_user_scale);
					f.WriteString(s);
					f.WriteString("20\n");
					s.Format("%.3f\n", gy1 / h * m_user_scale);
					f.WriteString(s);
					f.WriteString("11\n");
					s.Format("%.3f\n", gx2 / h * m_user_scale);
					f.WriteString(s);
					f.WriteString("21\n");
					s.Format("%.3f\n", gy2 / h * m_user_scale);
					f.WriteString(s);
				}
				else
				{
					CPoint P[10];
					int np = Generate_Arc(gx1, gy1, gx2, gy2, stl, P, 9);
					for (int ip = 0; ip < np-1; ip++)
					{
						f.WriteString("0\n");
						f.WriteString("LINE\n");
						f.WriteString("8\n");
						s = layer_str[L];
						f.WriteString(s + "\n");
						f.WriteString("10\n");
						s.Format("%.3f\n", P[ip].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("20\n");
						s.Format("%.3f\n", P[ip].y / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("11\n");
						s.Format("%.3f\n", P[ip+1].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("21\n");
						s.Format("%.3f\n", P[ip+1].y / h * m_user_scale);
						f.WriteString(s);
					}
				}
			}
			if (m_Doc->m_outline_poly->GetAt(i).GetHatch() == 1 ||
				m_Doc->m_outline_poly->GetAt(i).GetHatch() > 2)
			{
				CArray <dl_element*>* GetH = m_Doc->m_outline_poly->GetAt(i).GetHatchPos();
				int nh = GetH->GetSize();
				for (int ic = 0; ic < nh; ic++)
				{
					CArray<CPoint>* pA = GetH->GetAt(ic)->dlist->Get_Points(GetH->GetAt(ic), NULL, 0);
					int np = pA->GetSize();
					CPoint* P = new CPoint[np];//new012
					GetH->GetAt(ic)->dlist->Get_Points(GetH->GetAt(ic), P, &np);
					for (int ip = 0; ip + 1 < np; ip += 2)
					{
						f.WriteString("0\n");
						f.WriteString("LINE\n");
						f.WriteString("8\n");
						s = layer_str[L];
						f.WriteString(s + "\n");
						f.WriteString("10\n");
						s.Format("%.3f\n", P[ip].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("20\n");
						s.Format("%.3f\n", P[ip].y / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("11\n");
						s.Format("%.3f\n", P[ip + 1].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("21\n");
						s.Format("%.3f\n", P[ip + 1].y / h * m_user_scale);
						f.WriteString(s);
					}
					delete P;//new012
				}
			}
		}
		for (int iatt = 0; iatt < num_attributes; iatt++)
		{
			int itxt = -1;
			int current = m_Doc->Pages.GetActiveNumber();
			int ipag = current;
			for (CText* t = m_Doc->Pages.GetNextAttr(iatt, &itxt, &ipag); t && ipag == current; t = m_Doc->Pages.GetNextAttr(iatt, &itxt, &ipag))
			{
				if (t->m_selected)
				{
					CArray<CPoint>* pts = t->m_dlist->Get_Points(t->dl_el, NULL, 0);
					int npts = pts->GetSize();
					CPoint* PTS = new CPoint[npts];
					t->m_dlist->Get_Points(t->dl_el, PTS, &npts);
					for (int ipt = 0; ipt < npts - 1; ipt += 2)
					{
						f.WriteString("0\n");
						f.WriteString("LINE\n");
						f.WriteString("8\n");
						s = layer_str[t->m_layer];
						f.WriteString(s + "\n");
						f.WriteString("10\n");
						s.Format("%.3f\n", PTS[ipt].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("20\n");
						s.Format("%.3f\n", PTS[ipt].y / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("11\n");
						s.Format("%.3f\n", PTS[ipt+1].x / h * m_user_scale);
						f.WriteString(s);
						f.WriteString("21\n");
						s.Format("%.3f\n", PTS[ipt+1].y / h * m_user_scale);
						f.WriteString(s);
					}
					delete PTS;
				}
			}
		}
		f.WriteString("0\n");
		f.WriteString("ENDSEC\n");
		f.WriteString("0\n");
		f.WriteString("EOF\n");
		f.Close();
	}
	ShellExecute(NULL, "open", "open.dxf", NULL, m_Doc->m_app_dir, SW_SHOWNORMAL);
}

void CFreePcbView::OnAddGroupOrigin()
{
	FindGroupCenter(0);
	RECT bounds;
	bounds.left = bounds.bottom = INT_MAX;
	bounds.top = bounds.right = INT_MIN;
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
		if( p->GetSel() )
		{
			for( int ii=0; ii<p->GetNumCorners(); ii++ )
				if( p->GetSel(ii) )
					SwellRect( &bounds, p->GetX(ii), p->GetY(ii) );
			for( int ii=0; ii<p->GetNumSides(); ii++ )
				if( p->GetSideSel(ii) )
				{
					SwellRect( &bounds, p->GetX(ii), p->GetY(ii) );
					SwellRect( &bounds, p->GetX(p->GetNextCornerIndex(ii)), p->GetY(p->GetNextCornerIndex(ii)) );
				}
		}
	}
	if( bounds.left == INT_MAX )
		return;
	if( bounds.left == bounds.right )
		bounds.left -= NM_PER_MIL;
	if( bounds.bottom == bounds.top )
		bounds.bottom -= NM_PER_MIL;
	int sz = m_Doc->m_outline_poly->GetSize();
	m_Doc->m_outline_poly->SetSize(sz+1);
	m_Doc->m_outline_poly->GetAt(sz).SetDisplayList( m_Doc->m_dlist );
	id sel_id( ID_POLYLINE, ID_GRAPHIC, sz );
	m_Doc->m_outline_poly->GetAt(sz).Start( LAY_ADD_1, NM_PER_MIL, NM_PER_MIL, groupAverageX, bounds.bottom, 2, &sel_id, NULL );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( groupAverageX, bounds.top );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( groupAverageX, groupAverageY );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( bounds.left, groupAverageY );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( bounds.right, groupAverageY );
	m_Doc->m_outline_poly->GetAt(sz).Show();
	OnRangeCmds(NULL);
}

void CFreePcbView::OnAddGroupRect()
{
	RECT bounds;
	bounds.left = bounds.bottom = INT_MAX;
	bounds.top = bounds.right = INT_MIN;
	for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
	{
		CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
		if( p->GetSel() )
		{
			for( int ii=0; ii<p->GetNumCorners(); ii++ )
				if( p->GetSel(ii) )
					SwellRect( &bounds, p->GetX(ii), p->GetY(ii) );
			for( int ii=0; ii<p->GetNumSides(); ii++ )
				if( p->GetSideSel(ii) )
				{
					SwellRect( &bounds, p->GetX(ii), p->GetY(ii) );
					SwellRect( &bounds, p->GetX(p->GetNextCornerIndex(ii)), p->GetY(p->GetNextCornerIndex(ii)) );
				}
		}
	}
	if( bounds.left == INT_MAX )
		return;
	if( bounds.left == bounds.right )
		bounds.left -= NM_PER_MIL;
	if( bounds.bottom == bounds.top )
		bounds.bottom -= NM_PER_MIL;
	int sz = m_Doc->m_outline_poly->GetSize();
	m_Doc->m_outline_poly->SetSize(sz+1);
	m_Doc->m_outline_poly->GetAt(sz).SetDisplayList( m_Doc->m_dlist );
	id sel_id( ID_POLYLINE, ID_GRAPHIC, sz );
	m_Doc->m_outline_poly->GetAt(sz).Start( LAY_ADD_1+2, NM_PER_MIL, NM_PER_MIL, bounds.left, bounds.bottom, 0, &sel_id, NULL );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( bounds.left, bounds.top );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( bounds.right, bounds.top );
	m_Doc->m_outline_poly->GetAt(sz).AppendCorner( bounds.right, bounds.bottom );
	m_Doc->m_outline_poly->GetAt(sz).Close();
	m_Doc->m_outline_poly->GetAt(sz).Show();
	OnRangeCmds(NULL);
}

BOOL CFreePcbView::OnSetSlidingAngle( UINT CMD )
{ 
	int in = m_sel_op.GetNextCornerIndex( m_sel_id.ii );
	double x1 = m_sel_op.GetX( m_sel_id.ii );
	double y1 = m_sel_op.GetY( m_sel_id.ii );
	double x2 = m_sel_op.GetX( in );
	double y2 = m_sel_op.GetY( in );
	m_ang_axis = Angle(x2,y2,x1,y1);
	if( (int)abs(m_ang_axis+0.5)%90 > 0 )
	{
		if( CMD == ID_SLIDING_ANGLE2 )
			m_ang_axis += 90.0;
		if( m_ang_axis > 360.0 )
			m_ang_axis -= 360.0;
		CString s;
		s.Format(G_LANGUAGE == 0 ? 
			"The axis tilt angle is %.3f degrees, now movement using the arrows will be carried out along this axis":
			"Угол наклона оси составляет %.3f градусов, теперь движение объектов с помощью стрелок будет осуществляться вдоль этой оси", m_ang_axis);
		AfxMessageBox(s);
	}
	else
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "The axis tilt angle is 0.0":"Установлен угол наклона оси равным 0,0");
		m_ang_axis = 0.0;
	}
	return 0;
}

void CFreePcbView::OnTextCSV()
{
	int th_page = m_Doc->Pages.GetActiveNumber();

	// find min left & max top coords
	int minX = INT_MAX;
	int maxY = INT_MIN;
	int minY = INT_MAX;
	int maxX = INT_MIN;
	for( int i_attr=0; i_attr<num_attributes; i_attr++ )
	{
		int it = -1;
		int ip = th_page;
		for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
		{
			if( t->m_selected == 0 )
				continue;
			if( minX > t->m_x )
				minX = t->m_x;
			if( maxY < t->m_y )
				maxY = t->m_y;
			if( minY > t->m_y )
				minY = t->m_y;
			if( maxX < t->m_x )
				maxX = t->m_x;
		}
	}

	// find left top txt (first txt)
	CText * FirstTxt = NULL;
	for( int i_attr=0; i_attr<num_attributes; i_attr++ )
	{
		int it = -1;
		int ip = th_page;
		for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
		{
			if( t->m_selected == 0 )
				continue;
			if( FirstTxt == NULL )
				FirstTxt = t;
			else if( (FirstTxt->m_x + maxY - FirstTxt->m_y) > (t->m_x + maxY - t->m_y) )
				FirstTxt = t;
		}
	}
	if( FirstTxt == NULL )
		return;

	// get num rows and row space
	int n_rows = 0;
	RECT FT_RECT;
	FirstTxt->m_tl->GetTextRectOnPCB( FirstTxt, &FT_RECT );
	for( int i_attr=0; i_attr<num_attributes; i_attr++ )
	{
		int it = -1;
		int ip = th_page;
		for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
		{
			if( t->m_selected == 0 )
				continue;
			RECT gr;
			t->m_tl->GetTextRectOnPCB( t, &gr );
			if( gr.left < FT_RECT.right )
				n_rows++;
		}
	}
	int row_space = (maxY - minY)/n_rows;
	if( row_space == 0 )
		row_space = FirstTxt->m_font_size;

	// get num cols
	m_Doc->Attr->m_Footlist->MarkAllTexts(0);
	m_Doc->Attr->m_Netlist->MarkAllTexts(0);
	m_Doc->Attr->m_pDesclist->MarkAllTexts(0);
	m_Doc->Attr->m_Pinlist->MarkAllTexts(0);
	m_Doc->Attr->m_Reflist->MarkAllTexts(0);
	m_Doc->Attr->m_Valuelist->MarkAllTexts(0);
	int n_cols = 0;
	int top_level = INT_MAX;
	CText * TXT_FOUND;
	do
	{
		TXT_FOUND = NULL;
		for( int i_attr=0; i_attr<num_attributes; i_attr++ )
		{
			int it = -1;
			int ip = th_page;
			for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
			{
				if( t->m_selected == 0 )
					continue;
				if( t->utility )
					continue;
				RECT gr;
				t->m_tl->GetTextRectOnPCB( t, &gr );
				if( gr.left < FT_RECT.right )
				{
					if( t->m_y < top_level )
					{
						if( TXT_FOUND == NULL )
							TXT_FOUND = t;
						else if( t->m_y > TXT_FOUND->m_y )
							TXT_FOUND = t;
					}
				}
			}
		}
		if( TXT_FOUND )
		{
			TXT_FOUND->utility = 1;
			top_level = TXT_FOUND->m_y;
			int max_cols_in_row = 0;
			for( int i_attr=0; i_attr<num_attributes; i_attr++ )
			{
				int it = -1;
				int ip = th_page;
				for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
				{
					if( t->m_selected == 0 )
						continue;
					if( t->utility )
						continue;
					if( abs(top_level - t->m_y) < row_space/2 )
					{
						max_cols_in_row++;
						t->utility = 1;
					}
				}
			}
			n_cols = max( n_cols, max_cols_in_row );
		}
	}while( TXT_FOUND );

	// generate...
	m_Doc->Attr->m_Footlist->MarkAllTexts(0);
	m_Doc->Attr->m_Netlist->MarkAllTexts(0);
	m_Doc->Attr->m_pDesclist->MarkAllTexts(0);
	m_Doc->Attr->m_Pinlist->MarkAllTexts(0);
	m_Doc->Attr->m_Reflist->MarkAllTexts(0);
	m_Doc->Attr->m_Valuelist->MarkAllTexts(0);
	CString OUTPUT = "";
	top_level = INT_MAX;
	CText * NEXT_ROW;
	do
	{
		NEXT_ROW = NULL;
		for( int i_attr=0; i_attr<num_attributes; i_attr++ )
		{
			int it = -1;
			int ip = th_page;
			for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
			{
				if( t->m_selected == 0 )
					continue;
				if( t->utility )
					continue;
				RECT gr;
				t->m_tl->GetTextRectOnPCB( t, &gr );
				if( gr.left < FT_RECT.right )
				{
					if( t->m_y < top_level )
					{
						if( NEXT_ROW == NULL )
							NEXT_ROW = t;
						else if( t->m_y > NEXT_ROW->m_y )
							NEXT_ROW = t;
					}
				}
			}
		}
		if( NEXT_ROW )
		{
			top_level = NEXT_ROW->m_y;
			NEXT_ROW->utility = 1;

			// write data
			if( OUTPUT.GetLength() )
				OUTPUT += "\n";
			int sep = NEXT_ROW->m_str.Find("|");
			if( sep >= 0 )
				OUTPUT += NEXT_ROW->m_str.Left(sep) + ";";
			else
				OUTPUT += NEXT_ROW->m_str + ";";

			int max_cols_in_row = 0;
			int left_level = INT_MIN;
			CText * NEXT_COL;
			do
			{
				NEXT_COL = NULL;
				for( int i_attr=0; i_attr<num_attributes; i_attr++ )
				{
					int it = -1;
					int ip = th_page;
					for(CText * t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ); (t && ip==th_page); t=m_Doc->Pages.GetNextAttr( i_attr, &it, &ip ) )
					{
						if( t->m_selected == 0 )
							continue;
						if( t->utility )
							continue;
						if( abs(top_level - t->m_y) < row_space/2 )
						{
							if( t->m_x > left_level )
							{
								if( NEXT_COL == NULL )
									NEXT_COL = t;
								else if( t->m_x < NEXT_COL->m_x )
									NEXT_COL = t;
							}
						}
					}
				}
				if( NEXT_COL )
				{
					NEXT_COL->utility = 1;
					left_level = NEXT_COL->m_x;
					max_cols_in_row++;
					int sep = NEXT_COL->m_str.Find("|");
					if( sep >= 0 )
						OUTPUT += NEXT_COL->m_str.Left(sep) + ";";
					else
						OUTPUT += NEXT_COL->m_str + ";";
				}
			}while( NEXT_COL );

			// дописываем ";"
			for(int i=max_cols_in_row; i<n_cols; i++)
			{
				OUTPUT += ";";
			}
		}
	}while( NEXT_ROW );

	// copy to clipboard
	if( OUTPUT.GetLength() )
		if( OpenClipboard() )
		{
			HANDLE cmd = GlobalAlloc(GMEM_FIXED,OUTPUT.GetLength()*2);
			memcpy( cmd, OUTPUT, OUTPUT.GetLength()+1 );

			EmptyClipboard();
			SetClipboardData(CF_TEXT,cmd);
			CloseClipboard();
		}
}

void CFreePcbView::RedrawLayers()
{
	BOOL bReDraw = 0;
	int pos = 0;
	int bFound;
	double P = 0.0;
	do
	{
		bFound = -1;
		P = 0.0;
		for( int i=pos; i<m_Doc->m_outline_poly->GetSize(); i++ )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
			if( p->m_visible == 0 )
				continue;
			if( p->GetClosed() && p->GetHatch() )
			{
				RECT gR = p->GetBounds();
				double gP = (double)(gR.right-gR.left)*(double)(gR.top-gR.bottom);
				if( gP > P )
				{
					P = gP;
					bFound = i;
				}
			}
		}
		if( bFound != -1 )
		{
			int i = pos++;
			if( bFound != i )
			{
				bReDraw = 1;
				if( m_sel_id.type ) 
				{ 
					if( m_sel_id.i == bFound )
						m_sel_id.i = i;
					else if( m_sel_id.i < bFound )
						if( m_sel_id.i >= i )
							m_sel_id.i++;
				}
				// correct favorites
				for (int ipo=0; ipo<MAX_FAVORITES; ipo++)
				{
					if (m_favorite_page[ipo] != m_Doc->Pages.GetActiveNumber())
						continue;
					if (m_favorite_poly[ipo] == bFound)
						m_favorite_poly[ipo] = i;
					else if (m_favorite_poly[ipo] < bFound)
						if (m_favorite_poly[ipo] >= i)
							m_favorite_poly[ipo]++;
				}
				CPolyLine p = m_Doc->m_outline_poly->GetAt( bFound );
				m_Doc->m_outline_poly->InsertAt( i, p );
				m_Doc->m_outline_poly->RemoveAt( ++bFound );
			}
		}
	} while( bFound != -1 );
	
	
	if( bReDraw )
	{
		for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
			id gid = p->GetId();
			gid.i = i;
			p->SetId( &gid );
			if( p->m_visible == 0 )
				continue;
			int iatt = -1;
			while( CText * t=m_Doc->ScanAttr( p, &iatt ) )
			{
				if( t->isVISIBLE )
				{
					t->m_polyline_start = i;
					t->MakeVisible();
				}
			}
		}
		for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
		{
			CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
			if( p->m_visible )
				p->Draw();
		}
		m_Doc->AttributeIndexing( LAY_PART_NAME );
		m_Doc->AttributeIndexing( LAY_NET_NAME );
	}
	else if( m_Doc->m_outline_poly->GetSize() )
	{
		if( m_Doc->m_outline_poly->GetAt(0).GetVisible() )
		{
			if( m_Doc->m_outline_poly->GetAt(0).bDrawn == 0 )
			{
				for( int i=0; i<m_Doc->m_outline_poly->GetSize(); i++ )
				{
					CPolyLine * p = &m_Doc->m_outline_poly->GetAt(i);
					if( p->m_visible )
						p->Draw();
				}
			}
		}
	}
	return;
}