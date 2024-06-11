// CDlgImportMerge.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
//#include "DlgFindPart.h"
#include "DlgImportMerge.h"
//#include ".\dlgimportpart.h"

extern CFreeasyApp theApp;

// CDlgImportPart dialog

IMPLEMENT_DYNAMIC(CDlgImportMerge, CDialog)
CDlgImportMerge::CDlgImportMerge(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportMerge::IDD, pParent)
{
}

CDlgImportMerge::~CDlgImportMerge()
{
}

void CDlgImportMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_part_list);
	DDX_Control(pDX, IDC_LIST2, m_path_list);
	DDX_Control(pDX, IDC_PREVIEW, m_preview);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		HICON hIcon;
		hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		HICON prevIcon = SetIcon( hIcon, FALSE );
		if( prevIcon )
			DestroyIcon( prevIcon );
		Change();
		OnClickList();
	}
	else
	{
		// outgoing
		int cur_sel = m_part_list.GetCurSel();
		if( cur_sel >= 0 )
		{
			m_part_list.GetText( cur_sel, m_merger );
		}
	}
}


BEGIN_MESSAGE_MAP( CDlgImportMerge, CDialog)
	ON_LBN_SELCHANGE( IDC_LIST, OnClickList )
	ON_LBN_SELCHANGE( IDC_LIST2, OnClickList2 )
	ON_LBN_DBLCLK( IDC_LIST, OnDblClickList )
	ON_LBN_DBLCLK( IDC_LIST2, OnDblClickList2 )
	ON_CBN_EDITCHANGE( IDC_COMBO1, OnChange)
END_MESSAGE_MAP()


// CDlgImportPart message handlers

void CDlgImportMerge::Initialize( CLibPointer * library )
{
	m_library = library;
	m_merges = m_library->GetLibrary();
	m_current_path = -1;
}


void CDlgImportMerge::OnClickList()
{
	CString str;
	int i = m_part_list.GetCurSel();
	if( i >= 0 )
	{
		m_part_list.GetText(i,str);
		//m_combo_attr.SetWindowTextA(str);
		for( int i=m_path_list.GetCount()-1; i>=0; i-- )
			m_path_list.DeleteString(i);
		CArray<CString> * Pathes = m_library->GetPathes( &str );
		for( int i=0; i<Pathes->GetSize(); i++ )
		{
			m_path_list.AddString( Pathes->GetAt(i).Right( Pathes->GetAt(i).GetLength() - theApp.m_Doc->m_parent_folder.GetLength() ) );
		}
		m_current_path = 0;
		DrawPattern( &str );
		m_current_path = -1;
		m_path_list.SetCurSel( m_current_path );
	}
}

void CDlgImportMerge::OnClickList2()
{
	m_current_path = m_path_list.GetCurSel();
	CString str;
	int i = m_part_list.GetCurSel();
	if( i >= 0 )
	{
		m_part_list.GetText(i,str);
		DrawPattern( &str );
	}
}

void CDlgImportMerge::OnDblClickList()
{
	CString s = theApp.m_Doc->m_pcb_full_path;
	int L = theApp.m_Doc->m_parent_folder.GetLength();
	s = s.Right( s.GetLength() - L );
	m_current_path = m_path_list.FindStringExact(-1,s);
	if( m_current_path >= 0 )
	{
		m_path_list.SetCurSel( m_current_path );
		OnDblClickList2();
	}
	else
		AfxMessageBox("This part is not used in the current project");
}

void CDlgImportMerge::OnDblClickList2()
{
	int cur = m_part_list.GetCurSel();
	CString ref,path;
	m_part_list.GetText( cur, ref );
	m_path_list.GetText( m_current_path, path );
	path = theApp.m_Doc->m_parent_folder + path;
	theApp.m_Doc->SwitchToCDS( &path, &ref, TRUE );
}


void CDlgImportMerge::OnChange()
{
	Change();
}

void CDlgImportMerge::Change()
{
	if( m_merges->GetSize() )
	{
		m_part_list.EnableWindow(0);
		m_part_list.ShowWindow(SW_HIDE);
		for( int i=m_part_list.GetCount()-1; i>=0; i-- )
			m_part_list.DeleteString(i);
		for( int i=m_merges->GetSize()-1; i>=0; i-- )
			if( m_merges->GetAt(i).Left(6) != "Merge-" )
			m_part_list.AddString( m_merges->GetAt(i) );
		m_part_list.ShowWindow(SW_SHOW);
		m_part_list.EnableWindow(1);
	}
}




void CDlgImportMerge::DrawPattern( CString * VdP )
{
	int idx = -1;
	CString SpeedFile = theApp.m_Doc->m_merge_library.GetPath( VdP, m_current_path, &idx );
	int insertMode = 1;
	int singleMode = 1;
	int f_flags = 0;
	//
	int mINDEX = idx;
	int cX=0, cY=0;
	CArray<CPolyLine> * p_arr = &theApp.m_Doc->clip_outline_poly;
	p_arr->RemoveAll();
	CString s1="", s2="";
	theApp.m_Doc->m_view->FindStrInFile( &SpeedFile, &s1, &s2, f_flags, insertMode, singleMode, &cX, &cY, -1, -1, mINDEX );
	//
	CRect wr;
	m_preview.GetClientRect(&wr);
	int win_W = wr.Width();
	int win_H = wr.Height();
	RECT TotalR;
	TotalR.left = TotalR.bottom = INT_MAX;
	TotalR.top = TotalR.right = INT_MIN;
	if( p_arr->GetSize() == 0 )
		TotalR = rect(0,0,NM_PER_MM,NM_PER_MM);
	else for( int i=0; i<p_arr->GetSize(); i++ )
	{
		CPolyLine * p = &p_arr->GetAt(i);
		RECT r = p->GetBounds();
		SwellRect( &TotalR, r );
		if( CText * pin=p->Check( index_pin_attr ) )
		{
			int ch = pin->m_tl->GetTextRectOnPCB( pin, &r );
			if( ch )
				SwellRect( &TotalR, r );
		}
		if( CText * desc=p->Check( index_desc_attr ) )
		{
			int ch = desc->m_tl->GetTextRectOnPCB( desc, &r );
			if( ch )
				SwellRect( &TotalR, r );
		}
	}
	SwellRect( &TotalR, NM_PER_MM );
	//
	int k1 = (TotalR.right-TotalR.left)/win_W;
	int k2 = (TotalR.top-TotalR.bottom)/win_H;
	k1 = max( k1, k2 );
	k2 = TotalR.left/k1;
	int k3 = TotalR.bottom/k1;
	CDC * gDC = m_preview.GetDC();
	COLORREF gnd = RGB(255,255,255);
	CBrush brush( gnd );
	CPen pengnd( PS_SOLID, 1, gnd );
	gDC->SelectObject( &brush );
	COLORREF rgb = RGB(100,200,150);
	CPen pen( PS_SOLID, 1, rgb );
	gDC->SetBkColor( rgb );
	gDC->SelectObject( pengnd );
	gDC->Rectangle( 0, 0, win_W, win_H );
	
	if( p_arr->GetSize() == 0 )
	{
		ReleaseDC( gDC );
		return; // no elements
	}
	gDC->SelectObject( pen );
	int w = abs(TotalR.right-TotalR.left)/k1;
	int h = abs(TotalR.top-TotalR.bottom)/k1;
	int kx = (win_W - w)/2;
	int ky = (win_H - h)/2;
	for( int i=0; i<p_arr->GetSize(); i++ )
	{
		CPolyLine * p = &p_arr->GetAt(i);
		//
		for( int ii=0; ii<p->GetNumSides(); ii++ )
		{
			if( p->GetContourStart( p->GetNumContour(ii) ) == ii )
			{
				int x = p->GetX(ii)/k1 - k2 + kx;
				int y = p->GetY(ii)/k1 - k3 + ky;
				y = win_H - y;
				gDC->MoveTo(x,y);
			}
			int inx = p->GetNextCornerIndex(ii);
			int x = p->GetX(inx)/k1 - k2 + kx;
			int y = p->GetY(inx)/k1 - k3 + ky;
			y = win_H - y;
			if( p->GetSideStyle(ii) )
			{
				int xp = p->GetX(ii)/k1 - k2 + kx;
				int yp = p->GetY(ii)/k1 - k3 + ky;
				yp = win_H - yp;
				CPoint P[10];
				int np = Generate_Arc( xp, yp, x, y, 3-p->GetSideStyle(ii), P, 9 );
				for(int ip=0; ip<np; ip++ )
				{
					gDC->LineTo( P[ip].x, P[ip].y );
				}
			}
			else
				gDC->LineTo(x,y);
		}
		if( CText * pin=p->Check( index_pin_attr ) )
		{
			CArray<CPoint> * pt_arr = pin->dl_el->dlist->Get_Points( pin->dl_el, NULL, 0 );
			int np = pt_arr->GetSize();
			CPoint * GET = new CPoint[np];//new002
			pin->dl_el->dlist->Get_Points( pin->dl_el, GET, &np ); 
			for( int iv=0; iv<np; iv++ )
			{
				int x = GET[iv].x/k1 - k2 + kx;
				int y = GET[iv].y/k1 - k3 + ky;
				y = win_H - y;
				if( x > 1 && x < win_W-1 && y > 1 && y < win_H-1 )
				{
					if( iv%2 == 0 )
						gDC->MoveTo(x,y);
					else
						gDC->LineTo(x,y);
				}
				else if( iv%2 == 0 )
					iv++;
			}
			delete GET;//new002
		}
		if( CText * desc=p->Check( index_desc_attr ) )
		{
			CArray<CPoint> * pt_arr = desc->dl_el->dlist->Get_Points( desc->dl_el, NULL, 0 );
			int np = pt_arr->GetSize();
			CPoint * GET = new CPoint[np];//new003
			desc->dl_el->dlist->Get_Points( desc->dl_el, GET, &np ); 
			for( int iv=0; iv<np; iv++ )
			{
				int x = GET[iv].x/k1 - k2 + kx;
				int y = GET[iv].y/k1 - k3 + ky;
				y = win_H - y;
				if( x > 1 && x < win_W-1 && y > 1 && y < win_H-1 )
				{
					if( iv%2 == 0 )
						gDC->MoveTo(x,y);
					else
						gDC->LineTo(x,y);
				}
				else if( iv%2 == 0 )
					iv++;
			}
			delete GET;//new003
		}
		if( CText * cnet=p->Check( index_net_attr ) )
		{
			CArray<CPoint> * pt_arr = cnet->dl_el->dlist->Get_Points( cnet->dl_el, NULL, 0 );
			int np = pt_arr->GetSize();
			CPoint * GET = new CPoint[np];//new543
			cnet->dl_el->dlist->Get_Points( cnet->dl_el, GET, &np ); 
			for( int iv=0; iv<np; iv++ )
			{
				int x = GET[iv].x/k1 - k2 + kx;
				int y = GET[iv].y/k1 - k3 + ky;
				y = win_H - y;
				if( x > 1 && x < win_W-1 && y > 1 && y < win_H-1 )
				{
					if( iv%2 == 0 )
						gDC->MoveTo(x,y);
					else
						gDC->LineTo(x,y);
				}
				else if( iv%2 == 0 )
					iv++;
			}
			delete GET;//new543
		}
	}
	for( int i=0; i<p_arr->GetSize(); i++ )
	{
		CPolyLine * p = &p_arr->GetAt(i);
		int iatt = -1;
		while( CText * t=theApp.m_Doc->ScanAttr( p, &iatt ) )
		{
			t->InVisible();
			p->pAttr[iatt] = NULL;
		}
		int src = p->GetPicture();
		if( src >= 0 )
			theApp.m_Doc->m_dlist->SetTemplateVis( 0, src );
	}
	p_arr->RemoveAll();
	ReleaseDC( gDC );
}