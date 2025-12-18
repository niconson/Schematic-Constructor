// DisplayList.cpp : implementation of class CDisplayList
//
// this is a linked-list of display elements
// each element is a primitive graphics object such as a line-segment,
// circle, annulus, etc.
//
#include "stdafx.h" 
#include <math.h>

// dimensions passed to DisplayList from the application are in PCBU (i.e. nm)
// since the Win95/98 GDI uses 16-bit arithmetic, PCBU must be scaled to DU (i.e. mils) 
//
//#define PCBU_PER_DU		PCBU_PER_WU
//#define MIL_PER_DU		NM_PER_MIL/PCBU_PER_DU	// conversion from mils to display units
//#define DU_PER_MIL		PCBU_PER_DU/NM_PER_MIL	// conversion from display units to mils

#define DL_MAX_LAYERS	32
#define HILITE_POINT_W	10	// size/2 of selection box for points (mils)  

extern CFreeasyApp theApp;
// constructor
//
CDisplayList::CDisplayList()
{
	bDrawBreak = -1;
	// create lists for all layers
	for( int layer=0; layer<MAX_LAYERS; layer++ )
	{
		// linked list for layer
		// default colors, these should be overwritten with actual colors
		m_rgb[layer][0] = layer*63;			// R 
		m_rgb[layer][1] = (layer/4)*127;	// G
		m_rgb[layer][2] = (layer/8)*63;		// B
		// default order
		m_layer_in_order[layer] = layer;	// will be drawn from highest to lowest
		m_order_for_layer[layer] = layer;
		//
		m_start[layer].prev = 0;		// first element
		m_start[layer].next = &m_end[layer];
		m_start[layer].next_HLT = &m_end[LAY_HILITE];
		m_end[layer].next = 0;			// last element
		m_end[layer].prev = &m_start[layer];
		m_end[layer].next_HLT = NULL;
	}
	// miscellaneous
	m_drag_flag = 0;
	m_drag_num_lines = 0;
	m_drag_line_pt = 0;
	m_drag_num_ratlines = 0;
	m_drag_ratline_start_pt = 0;
	m_drag_ratline_end_pt = 0;
	m_drag_ratline_width = 0;
	m_cross_hairs = 0;
	m_visual_grid_on = 0;
	m_visual_grid_spacing = 0;
	m_inflection_mode = IM_NONE;
	m_last_inflection_mode = IM_NONE;
	m_highlight_mode = EL_NON_STATIC;
	m_grid_style = 0;
	m_drag_layer = 0;
	m_drag_start = 0;
	m_show_pictures = 1;
	m_tmp.SetSize(0);
	m_src.SetSize(0);
	m_client_r = rect(0,0,0,0);
}

// destructor
//
CDisplayList::~CDisplayList()
{
	RemoveAll();
}

void CDisplayList::RemoveAll()
{
	// remove bmp
	for( int i=0; i<m_tmp.GetSize(); i++ )
	{
		m_tmp.GetAt(i)->bm.DeleteObject();
		if( m_tmp.GetAt(i)->lp )
			m_tmp.GetAt(i)->lp->Release();
		delete m_tmp.GetAt(i);
	}
	m_tmp.RemoveAll();

	// traverse lists for all layers, removing all elements
	for( int lay=0; lay<MAX_LAYERS; lay++ )
		while( m_end[lay].prev != &m_start[lay] )
		{
			dl_element *  el = m_end[lay].prev;
			Remove( el );
		}
	if( m_drag_line_pt )
	{
		free( m_drag_line_pt );
		m_drag_line_pt = 0;
	}
	if( m_drag_ratline_start_pt )
	{
		free( m_drag_ratline_start_pt );
		m_drag_ratline_start_pt = 0;
	}
	if( m_drag_ratline_end_pt )
	{
		free( m_drag_ratline_end_pt );
		m_drag_ratline_end_pt = 0;
	}
}

// Set conversion parameters between world coords (used by elements in
// display list) and window coords (pixels)
//
// enter with:
//	client_r = window client rectangle (pixels)
//	screen_r = window client rectangle in screen coords (pixels)
//	pane_org_x = starting x-coord of PCB drawing area in client rect (pixels)
//  pane_bottom_h = height of bottom pane (pixels)
//	pcb units per pixel = nm per pixel
//	org_x = x-coord of left edge of drawing area (pcb units)
//	org_y = y-coord of bottom edge of drawing area (pcb units)
//
// note that the actual scale factor is set by the arguments to 
// SetWindowExt and SetViewportExt, and may be slightly different for x and y
//
void CDisplayList::SetMapping( CRect *client_r, CRect *screen_r, int pane_org_x, int pane_bottom_h, 
							double pcbu_per_pixel, int org_x, int org_y )
{
	if( bDrawBreak != -1 )
		return;
	m_client_r = client_r;				// pixels
	m_screen_r = screen_r;				// pixels
	m_pane_org_x = pane_org_x;			// pixels
	m_bottom_pane_h = pane_bottom_h;	// pixels
	m_pane_org_y = client_r->bottom - pane_bottom_h;	// pixels
	m_scale = pcbu_per_pixel/(double)m_pcbu_per_wu;	// world units per pixel
	m_org_x = org_x/m_pcbu_per_wu;		// world units
	m_org_y = org_y/m_pcbu_per_wu;		// world units

	//now set extents
	double rw = m_client_r.Width();	// width of client area (pixels)
	double rh = m_client_r.Height();	// height of client area (pixels)
	double ext_x = rw*pcbu_per_pixel/m_pcbu_per_wu;	// width in WU
	double ext_y = rh*pcbu_per_pixel/m_pcbu_per_wu;	// height in WU
	int div = 1, 
		mult = m_pcbu_per_wu;
	
	//if( ext_x*mult/div > INT_MAX )
	//	ASSERT(0);
	//if( ext_y*mult/div > INT_MAX )
	//	ASSERT(0);
	w_ext_x = ext_x*mult/div;
	v_ext_x = rw*mult/div;
	w_ext_y = ext_y*mult/div;
	v_ext_y = -rh*mult/div;
	m_wu_per_pixel_x = (double)w_ext_x/v_ext_x;		// actual ratios
	m_wu_per_pixel_y = (double)w_ext_y/v_ext_y;
	m_pcbu_per_pixel_x = m_wu_per_pixel_x * m_pcbu_per_wu;		
	m_pcbu_per_pixel_y = m_wu_per_pixel_y * m_pcbu_per_wu;
	m_max_x = m_org_x + m_wu_per_pixel_x*(client_r->right-pane_org_x) + 2;	// world units
	m_max_y = m_org_y - m_wu_per_pixel_y*client_r->bottom + 2;				// world units
}
int CDisplayList::GetMapping(	CRect *client_r, int *pane_org_x, 
								int *pane_bottom_h, double *pcbu_per_pixel, 
								double *org_x, double *org_y )
{
	if( m_client_r.Width() && m_client_r.Height() )
	{
		*client_r = m_client_r;				// pixels
		*pane_org_x = m_pane_org_x;			// pixels
		*pane_bottom_h = m_bottom_pane_h;	// pixels
		*pcbu_per_pixel = m_scale*(double)m_pcbu_per_wu;	// world units per pixel
		*org_x = m_org_x*(double)m_pcbu_per_wu;		// world units
		*org_y = m_org_y*(double)m_pcbu_per_wu;		// world units
		return 1;
	}
	return 0;
}

// add graphics element used for selection
//
dl_element * CDisplayList::AddSelector( void * ptr, id _id, int gtype, int visible, 
										RECT * el_rect, int el_w, CPoint * p, int np, int orig_layer )
{
	dl_element * test = Add( ptr, _id, LAY_SELECTION, gtype, visible, el_rect, el_w, p, np );
	test->layer = orig_layer;
	return test;
}

dl_element * CDisplayList::AddSelector( dl_element * el )
{
	dl_element * test = NULL;
	if( el->pts.GetSize() )
		test = Add( el->ptr, el->id, LAY_SELECTION, el->gtype, 
		el->visible, &el->rect, el->el_w, &el->pts[0], el->pts.GetSize(), FALSE );
	else
		test = Add( el->ptr, el->id, LAY_SELECTION, el->gtype, 
		el->visible, &el->rect, el->el_w, NULL, 0, FALSE );
	test->layer = el->layer;
	return test;
}

dl_element * CDisplayList::GetElById( int layer, int id_i )
{
	dl_element * out_el = &m_end[layer];
	if( m_end[layer].prev != &m_start[layer] )
	{
		int max_i = m_end[layer].prev->id.i;
		if( id_i < (max_i-id_i) )
		{
			for( dl_element * el=m_start[layer].next; el!=&m_end[layer]; el=el->next )
			{
				if( el->id.i > id_i )
				{
					out_el = el;
					break;
				}
			}
		}
		else
		{
			for( dl_element * el=m_end[layer].prev; el!=&m_start[layer]; el=el->prev )
			{
				if( el->id.i <= id_i )
				{
					out_el = el->next;
					break;
				}
			}
		}
	}
	return out_el;
}

// Add entry to end of list, returns pointer to element created.
//
// Dimensional units for input parameters are PCBU
//
dl_element * CDisplayList::Add( void * ptr, id _id, int layer, int gtype, int visible,
								RECT * el_rect, int el_w, CPoint * p, int np, BOOL convert, dl_element * end_element )
{
	// create new element and link into list
	dl_element * new_element = new dl_element;// add
	if( end_element == NULL )
		end_element = &m_end[layer];
	new_element->prev = end_element->prev;
	new_element->next = end_element;
	new_element->prev->next = new_element;
	new_element->next->prev = new_element;
	new_element->next_HLT = NULL;

	// now copy data from entry into element	
	new_element->id = _id;
	new_element->ptr = ptr;
	new_element->gtype = gtype;
	new_element->visible = visible;

	int per = m_pcbu_per_wu;
	if( !convert )
		per = 1;
	if(p&&np)
	{
		new_element->pts.SetSize( np );
		for( int i=np-1; i>=0; i-- )
		{
			new_element->pts[i].x = p[i].x/per;
			new_element->pts[i].y = p[i].y/per;
		}
	}
	else
		new_element->pts.SetSize( 0 );

	new_element->el_w =	el_w/per;
	if( el_rect )
	{
		new_element->rect.left =	el_rect->left	/per;
		new_element->rect.right =	el_rect->right	/per;
		new_element->rect.bottom =	el_rect->bottom	/per;
		new_element->rect.top =		el_rect->top	/per;
	}
	else if( p&&np >= 2 )
		new_element->rect = rect(new_element->pts[0].x,new_element->pts[0].y,new_element->pts[1].x,new_element->pts[1].y);
	else
		new_element->rect = rect(0,0,0,0);
	new_element->layer = layer;
	new_element->el_static = EL_NON_STATIC;
	new_element->half_tone = 0;
	new_element->transparent = 0;
	new_element->dlist = this;
	return new_element;
}

void CDisplayList::HighLight( dl_element * el )
{
	if( el->next_HLT == NULL ) // !!!
	{
		el->next_HLT = m_start[LAY_HILITE].next_HLT;
		m_start[LAY_HILITE].next_HLT = el;
	}
}

dl_element * CDisplayList::CopyToLayer( dl_element * src_el, int layer )
{
	dl_element * test = NULL;
	if( src_el->pts.GetSize() )
		test = Add(	src_el->ptr, 
					src_el->id, 
					layer, 
					src_el->gtype, 
					src_el->visible, 
					&src_el->rect, 
					src_el->el_w, 
					&src_el->pts[0], 
					src_el->pts.GetSize(),0 );
	else
		test = Add(	src_el->ptr, 
					src_el->id, 
					layer, 
					src_el->gtype, 
					src_el->visible, 
					&src_el->rect, 
					src_el->el_w, 
					NULL, 0,0 );
	test->layer = src_el->layer;
	test->transparent = src_el->transparent;
	test->next_HLT = NULL;
	return test;
}

dl_element * CDisplayList::HighLight( int gtype, RECT * el_rect, int el_w, CPoint * p, int np, int orig_layer )
{
	id _id;
	dl_element * test = Add( NULL, _id, LAY_HILITE, gtype, 1, el_rect, el_w, p, np );
	test->layer = orig_layer;
	return test;
}

int CDisplayList::HighlightAll()
{
	for( int lay=0;lay<MAX_LAYERS; lay++ )
	{
		if( lay == LAY_SELECTION || lay == LAY_HILITE )
			continue;
		for( dl_element * el=m_start[lay].next; el->next; el=el->next )
			HighLight( el );
	}
	HighlightSelRect( TRANSPARENT_HILITE );
	return 0;
}
dl_element * CDisplayList::HighlightRect( RECT * R, int orig_layer, int Transparent, int DL_SHAPE )
{
	if( R->right > R->left )
	{
		dl_element * el_bounds = HighLight( DL_SHAPE, R, 0, NULL, 0, orig_layer );
		el_bounds->transparent = Transparent;
		if( Transparent == TRANSPARENT_BLACK_GROUND )
			el_bounds->visible = 0;
		return el_bounds;
	}
	return NULL;
}
RECT CDisplayList::HighlightSelRect( int Transparent )
{
	RECT rct = GetHighlightedBounds(NULL);
	HighlightRect( &rct, m_top_layer, Transparent, DL_HOLLOW_RECT );
	return rct;
}

RECT CDisplayList::GetHighlightedBounds( int * map_orig_layer, int convert )
{
	RECT r = rect( 0,0,0,0 );
	int start = 0;
	for( dl_element * el=m_start[LAY_HILITE].next; el->next; el=el->next )
	{
		if( el->el_static == 0 /*|| el->next == &m_end[LAY_HILITE]*/ )
		{
			if( start == 0 )
			{
				r.left = r.bottom = INT_MAX;
				r.right = r.top = INT_MIN;
				start = 1;
			}
			SwellRect(&r, el->rect );
			//if( el->el_static )
			//	SwellRect(&r, el->el_w*3 );
			if( map_orig_layer )
				setbit( *map_orig_layer, el->layer );
			if(el->pts.GetSize() == 4)
			{
				SwellRect(&r, el->pts[0] );
				SwellRect(&r, el->pts[1] );
				SwellRect(&r, el->pts[2] );
				SwellRect(&r, el->pts[3] );
			}
		}
	}
	for( dl_element * el=m_start[LAY_HILITE].next_HLT; el!=&m_end[LAY_HILITE]; el=el->next_HLT )
	{
		if( el->el_static == 0 )
		{
			if( start == 0 )
			{
				r.left = r.bottom = INT_MAX;
				r.right = r.top = INT_MIN;
				start = 1;
			}
			SwellRect(&r, el->rect );
			if( map_orig_layer )
				setbit( *map_orig_layer, el->layer );
			if(el->pts.GetSize() == 4)
			{
				SwellRect(&r, el->pts[0] );
				SwellRect(&r, el->pts[1] );
				SwellRect(&r, el->pts[2] );
				SwellRect(&r, el->pts[3] );
			}
		}
	}
	if( convert )
	{
		r.top *= m_pcbu_per_wu;
		r.left *= m_pcbu_per_wu;
		r.right *= m_pcbu_per_wu;		
		r.bottom *= m_pcbu_per_wu;
	}
	return r;
}

// set element parameters in PCBU
//

void CDisplayList::Move( dl_element * el, int dx, int dy )
{
	for( int i=el->pts.GetSize()-1; i>=0; i-- )
	{
		el->pts[i].x += dx;
		el->pts[i].y += dy;
	}
	MoveRect( &el->rect, dx, dy );
}


// get element parameters in PCBU
//
dl_element * CDisplayList::Get_Start( int lay )
{
	return &m_start[lay];
}
dl_element * CDisplayList::Get_End( int lay )
{
	return &m_end[lay];
}
RECT * CDisplayList::Get_Rect( dl_element * el, RECT * Get )
{
	if( Get )
	{
		(*Get).left = el->rect.left		*m_pcbu_per_wu;
		(*Get).bottom = el->rect.bottom	*m_pcbu_per_wu;
		(*Get).right = el->rect.right	*m_pcbu_per_wu;
		(*Get).top = el->rect.top		*m_pcbu_per_wu;
	}
	return &el->rect;
}

CArray<CPoint> * CDisplayList::Get_Points( dl_element * el, CPoint * Get, int * npoints, int dx, int dy )
{
	if( el )
	{
		if( npoints )
		{
			*npoints = min( *npoints, el->pts.GetSize() );
			if( Get ) for( int ii=(*npoints)-1; ii>=0; ii-- )
			{
				Get[ii].x = el->pts[ii].x * m_pcbu_per_wu + dx;
				Get[ii].y = el->pts[ii].y * m_pcbu_per_wu + dy;
			}
		}
		return &el->pts;
	}
	if( npoints )
		*npoints = 0;
	return NULL;
}

int CDisplayList::Get_el_w( dl_element * el )
{
	return el->el_w * m_pcbu_per_wu;
}

int CDisplayList::Get_Selected ()
{ 
	dl_element * el = &m_end[LAY_HILITE];
	if( el->prev != &m_start[LAY_HILITE] )
		return 1;
	else
		return 0;
}

void CDisplayList::Get_Endpoints(CPoint *cpi, CPoint *cpf) 
{ 
	cpi->x = m_drag_xi*m_pcbu_per_wu; cpi->y = m_drag_yi*m_pcbu_per_wu;
	cpf->x = m_drag_xf*m_pcbu_per_wu; cpf->y = m_drag_yf*m_pcbu_per_wu;
}

void CDisplayList::HLTClear()
{
	dl_element * el = m_start[LAY_HILITE].next_HLT;
	while ( el )
	{
		dl_element * This = el;
		el = This->next_HLT;
		This->next_HLT = NULL;
	}
	m_start[LAY_HILITE].next_HLT = &m_end[LAY_HILITE];
}

// Remove element from list, return id
//
id CDisplayList::Remove( dl_element * element )
{
	if( !element )
	{
		id no_id;
		return no_id;
	}
	// remove links to this element
	element->next->prev = element->prev;
	element->prev->next = element->next;
	
	// cancel highlight
	if( m_start[LAY_HILITE].next_HLT != &m_end[LAY_HILITE] )
		HLTClear();

	// destroy element
	id el_id = element->id;
	element->pts.SetSize(0);
	delete( element );
	return el_id;
}

BOOL CDisplayList::ProcessMessages()
{
	MSG msg;
	PeekMessage(&msg, NULL, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_NOREMOVE);
	if (msg.message == WM_MOUSEWHEEL)
	{
		///TranslateMessage(&msg);
		///DispatchMessage(&msg);
		return 1;
	}
	return 0;
}

// Draw the display list using device DC or memory DC
//
void CDisplayList::Draw( CDC * dDC, int draw_layer )
{
	static int mem_org_x = 0;
	static int mem_max_x = 0;
	static int mem_org_y = 0;
	static int mem_max_y = 0;
	static BOOL bScaleChanged = 0;
	int clk_start = clock(); //get time

	// start drawing
	bDrawBreak = 0;

	int start_org_x = m_org_x;
	int start_max_x = m_max_x;
	int start_org_y = m_org_y;
	int start_max_y = m_max_y;

	if( draw_layer < 0 || mem_org_x != m_org_x || mem_max_x != m_max_x || mem_org_y != m_org_y || mem_max_y != m_max_y )
	{
		bScaleChanged = TRUE;
		draw_layer = 0;
	}

	// select DC
	CDC * pDC = dDC;
	if( botDC )
		pDC = botDC;

	// rectangle
	RECT SelRect = rect( start_org_x, start_org_y, start_max_x, start_max_y );
	
	// draw local rect
	if( draw_layer == LAY_BACKGND )
	{
		int map = ~0;
		RECT r = GetHighlightedBounds( &map, 0 );
		if( r.right - r.left > 0 )
		{
			int sw = m_pcbu_per_pixel_x/m_pcbu_per_wu;
			SwellRect( &r, sw );
			SelRect = r;
			draw_layer = 0;
		}
	}
	
	//
	COLORREF CLR_BLACK_GND;
	BOOL B_GND = ( (m_rgb[LAY_BACKGND][0] + m_rgb[LAY_BACKGND][1] + m_rgb[LAY_BACKGND][2]) < (128*3) );
	if( B_GND )
		CLR_BLACK_GND = RGB( 255, 255, 255 );	
	else
		CLR_BLACK_GND = RGB( 0, 0, 0 );
	COLORREF CLR_BACKGND = RGB( m_rgb[LAY_BACKGND][0], m_rgb[LAY_BACKGND][1], m_rgb[LAY_BACKGND][2] );	
	pDC->SetBkColor( CLR_BACKGND );
	//
	// create pens and brushes
	CBrush backgnd_brush( CLR_BACKGND );
	CPen backgnd_pen( PS_SOLID, 1, CLR_BACKGND );
	CPen * old_pen = pDC->SelectObject( &backgnd_pen );
	CBrush * old_brush = pDC->SelectObject( &backgnd_brush );
	RECT client_rect;
	client_rect.left =	start_org_x;
	client_rect.right = start_max_x;
	client_rect.bottom =start_org_y;
	client_rect.top =	start_max_y;
	int old_BkMode = pDC->GetBkMode();
	if( !draw_layer )
	{	
		pDC->Rectangle( &SelRect );
		int rop = pDC->SetROP2( R2_XORPEN );
		// visual grid
		if( m_visual_grid_on && (m_visual_grid_spacing/m_scale)>5 && m_vis[LAY_VISIBLE_GRID] )
		{
			CPen grid_pen( PS_SOLID, 1, RGB( m_rgb[LAY_VISIBLE_GRID][0], m_rgb[LAY_VISIBLE_GRID][1], m_rgb[LAY_VISIBLE_GRID][2] ) );
			pDC->SelectObject( &grid_pen );
			int startix = m_org_x/m_visual_grid_spacing;
			int startiy = m_org_y/m_visual_grid_spacing;
			double start_grid_x = startix*m_visual_grid_spacing;
			double start_grid_y = startiy*m_visual_grid_spacing;
			if( m_grid_style )
			{
				for( double ix=start_grid_x; ix<SelRect.right; ix+=m_visual_grid_spacing )
				{
					CPoint gp(ix,SelRect.bottom);
					pDC->MoveTo(gp);
					gp.x = ix;
					gp.y = SelRect.top;
					pDC->LineTo(gp);
				}
				for( double iy=start_grid_y; iy<SelRect.top; iy+=m_visual_grid_spacing )
				{
					CPoint gp(SelRect.left,iy);
					pDC->MoveTo(gp);
					gp.x = SelRect.right;
					gp.y = iy;
					pDC->LineTo(gp);
				}
			}
		}
		pDC->SetROP2( rop );
	}

	// now traverse the lists, starting with the layer in the last element 
	// of the m_order[] array
	
	int size_of_2_pixels = 4*m_scale;
	//int draw_order = m_order_for_layer[draw_layer];
	// COLOR FOR TRANSPARENT MODE
	int CL1,CL2,CL3,min_t=60;
	CL3 = min_t + m_rgb[TRANSPARENT_HILITE][0]/2;
	CL1 = min_t + m_rgb[TRANSPARENT_HILITE][1]/2;
	CL2 = min_t + m_rgb[TRANSPARENT_HILITE][2]/2;
	CL1 = min(150, CL1);
	CL2 = min(150, CL2);
	CL3 = min(150, CL3);
	COLORREF Transparent_hilite = RGB( CL1,CL2,CL3 );
	CL3 = min_t + m_rgb[TRANSPARENT_BACKGND][0]/2;
	CL1 = min_t + m_rgb[TRANSPARENT_BACKGND][1]/2;
	CL2 = min_t + m_rgb[TRANSPARENT_BACKGND][2]/2;
	CL1 = min(150, CL1);
	CL2 = min(150, CL2);
	CL3 = min(150, CL3);
	COLORREF Transparent_background = RGB( CL1,CL2,CL3 );
	CL3 = min_t + m_rgb[TRANSPARENT_BLACK_GROUND][0]/2;
	CL1 = min_t + m_rgb[TRANSPARENT_BLACK_GROUND][1]/2;
	CL2 = min_t + m_rgb[TRANSPARENT_BLACK_GROUND][2]/2;
	CL1 = min(150, CL1);
	CL2 = min(150, CL2);
	CL3 = min(150, CL3);
	COLORREF Transparent_black_ground = RGB( CL1,CL2,CL3 );
	int swOrder = m_order_for_layer[SWITCH_LAYER];
	for( int order=(MAX_LAYERS-1); order>=0; order-- )
	{
		int layer = m_layer_in_order[order];
		if( !m_vis[layer] || layer == LAY_SELECTION )
			continue;
		if( draw_layer )
		{
			if( order > m_order_for_layer[draw_layer] )
				continue;
		}
		if( order <= m_order_for_layer[LAY_DRC_ERROR] && pDC == botDC ) // switch on other DC
		{
			if( topDC && pDC != topDC )
			{
				topDC->BitBlt(	SelRect.left, SelRect.bottom, SelRect.right-SelRect.left, SelRect.top-SelRect.bottom,
								botDC, SelRect.left, SelRect.bottom, SRCCOPY ) ;
				if( SelRect.left != m_org_x ) // проверено
					botDC->BitBlt(	client_rect.left, 
									client_rect.bottom, 
									client_rect.right-client_rect.left, 
									client_rect.top-client_rect.bottom,
									topDC, client_rect.left, client_rect.bottom, SRCCOPY ) ;
				pDC = topDC;
			}
		}
		if( bDrawBreak == 0 )
			bDrawBreak |= ProcessMessages();
		if( order <= swOrder )
			if( bScaleChanged || bDrawBreak )
			{
			
				// accelerator
			
				continue;
			}

		if( start_org_x != m_org_x || start_max_x != m_max_x || start_org_y != m_org_y || start_max_y != m_max_y )
		{
			// coordinates were changed 
			// during drawing
			ASSERT(0);
			return;
		}


		// draw templates
		Marker();
		for( ; m_show_pictures; )
		{
			// first we draw
			// pictures of a
			// larger area
			int max_w = 0; 
			int ret_i = -1;
			for( int itmp=0; itmp<m_tmp.GetSize(); itmp++ )
			{
				if( m_tmp[itmp]->m_vis == 0 )
					continue;
				if( layer != m_tmp[itmp]->m_layer )
					continue;
				if( m_tmp[itmp]->m_rect.left == m_tmp[itmp]->m_rect.right )
					continue;
				if( m_tmp[itmp]->m_marker )
					continue;
				int max = m_tmp[itmp]->m_rect.right-m_tmp[itmp]->m_rect.left + m_tmp[itmp]->m_rect.top-m_tmp[itmp]->m_rect.bottom;
				if( max < max_w )
					continue;
				ret_i = itmp;
				max_w = max;
			}
			if( ret_i == -1 )
				break;
			int st = m_tmp[ret_i]->m_style;
			CSize sz = GetTemplateSZ(ret_i);
			if( m_tmp[ret_i]->m_format == BMP )
			{
				CDC nDC;
				nDC.CreateCompatibleDC(pDC);
				pDC->SetStretchBltMode( HALFTONE );

				nDC.SelectObject(m_tmp[ret_i]->bm);
				
				pDC->StretchBlt( m_tmp[ret_i]->m_rect.left, 
								 m_tmp[ret_i]->m_rect.top, 
								 m_tmp[ret_i]->m_rect.right - m_tmp[ret_i]->m_rect.left, 
								 m_tmp[ret_i]->m_rect.bottom - m_tmp[ret_i]->m_rect.top, 
								 &nDC,0,0,sz.cx,sz.cy, SRCCOPY );
				if( st == 0 )
					pDC->InvertRect(&m_tmp[ret_i]->m_rect );

			}
			else if( m_tmp[ret_i]->m_format == JPEG )
			{
				m_tmp[ret_i]->lp->Render( pDC->GetSafeHdc(), 
							 m_tmp[ret_i]->m_rect.left, 
							 m_tmp[ret_i]->m_rect.bottom, 
							 m_tmp[ret_i]->m_rect.right - m_tmp[ret_i]->m_rect.left, 
							 m_tmp[ret_i]->m_rect.top - m_tmp[ret_i]->m_rect.bottom, 
							 0,0,sz.cx,sz.cy, NULL );
				if( st == 0 )
					pDC->InvertRect(&m_tmp[ret_i]->m_rect );
			}

			// set mark
			m_tmp[ret_i]->m_marker = 1;
		}

		B_GND = ( (m_rgb[layer][0] + m_rgb[layer][1] + m_rgb[layer][2]) < (128*3) );
		COLORREF Layer_color = RGB( m_rgb[layer][0], m_rgb[layer][1], m_rgb[layer][2] );
		COLORREF * lay_clr = &Layer_color;
		CBrush fill_brush( *lay_clr );
		pDC->SelectObject( fill_brush );
		CPen line_pen( PS_SOLID, 0, *lay_clr );
		CPen pen     ( PS_SOLID, 0, *lay_clr );
		pDC->SelectObject( pen );
		dl_element * el = &m_start[layer];
		int nElement = 0;
		int prev_el_w = 0;
		int prev_transparent = 0;
		while( el->next->next )
		{
			el = el->next;
			if( !el->visible )
				continue;
			if( el->id.type == ID_TEXT )
				continue;

			if( bDrawBreak && el->id.sst != ID_HATCH )
			{
				if( el->prev->id.sst == ID_HATCH )
					break;
				if( el->next->next )
					el = el->next;
				if( el->next->next && el->id.sst != ID_HATCH )
					el = el->next;
			}

			if( nElement%64 == 0 && bDrawBreak == 0 )
				bDrawBreak |= ProcessMessages();
			nElement++;

			// if el hilite
			if( layer == LAY_HILITE ) // (if orig layer not vis)	
			{			
				if( !m_vis[el->layer] )
					continue;
			}
			
			// el rect
			if( el->rect.left > SelRect.right )
				continue;
			if( el->rect.right < SelRect.left )
				continue;
			if( el->rect.bottom > SelRect.top )
				continue;
			if( el->rect.top < SelRect.bottom )
				continue;

			if((el->rect.top - el->rect.bottom <= size_of_2_pixels ) && ( el->rect.right - el->rect.left <= size_of_2_pixels ))
				continue;

			
			if(( el->transparent && (layer == LAY_HILITE || el->id.sst == ID_HATCH) ) ||
				(el->transparent == 0 && prev_transparent) )
			{
				if( el->transparent == TRANSPARENT_HILITE )
					lay_clr = &Transparent_hilite;	
				else if( el->transparent == TRANSPARENT_BACKGND )
					lay_clr = &Transparent_background;	
				else if( el->transparent == TRANSPARENT_BLACK_GROUND )
					lay_clr = &Transparent_black_ground;
				else // default
					lay_clr = &Layer_color;
				fill_brush.DeleteObject();
				fill_brush.CreateSolidBrush( *lay_clr );
				pen.DeleteObject();
				pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
				pDC->SelectObject( pen );
				pDC->SelectObject( fill_brush );
				//
				line_pen.DeleteObject();
				line_pen.CreatePen( PS_SOLID, 0, *lay_clr );
			}
			else if( el->el_w != prev_el_w )
			{
				pen.DeleteObject();
				pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
				pDC->SelectObject( pen );
			}
			int old_ROP2 = NULL;
			if( el->transparent && layer == LAY_HILITE )
				old_ROP2 = pDC->SetROP2(R2_MERGEPEN);

			prev_el_w = el->el_w;
			prev_transparent = el->transparent;

//-----> first type
			if( el->gtype == DL_CIRC || el->gtype == DL_HOLLOW_CIRC )
			{
				if( el->gtype == DL_HOLLOW_CIRC )
					pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
				CPen * old_pen = pDC->SelectObject( &line_pen );
				pDC->Ellipse(	el->rect.left, 
								el->rect.bottom, 
								el->rect.right, 
								el->rect.top );	
				pDC->SelectObject( old_pen );
			}
//-----> next type
			else if( el->gtype == DL_LINE || el->gtype == DL_HOLLOW_LINE )
			{
				if( el->pts.GetSize() >= 2 )
				{
					// only draw line segments which are in viewport
					// line segment from (xi,yi) to (xf,yf)
					int draw_flag = 0;
					int xi = el->pts[0].x;
					int xf = el->pts[1].x;
					int yi = el->pts[0].y;
					int yf = el->pts[1].y;
					//
					if( el->rect.right < m_max_x && 
						el->rect.left > m_org_x )
						draw_flag = 1;
					else if( el->rect.top < m_max_y && 
							 el->rect.bottom > m_org_y )
						draw_flag = 1;
					else 
					{
						double angle = Angle( xi,yi,xf,yf );
						CPoint P[5];
						P[0].x = m_org_x, P[0].y = m_org_y ;			//bot_left( m_org_x, m_org_y );
						P[1].x = m_org_x, P[1].y = m_max_y ;			//top_left( m_org_x, m_max_y );
						P[2].x = m_max_x, P[2].y = m_org_y ;			//bot_right( m_max_x, m_org_y );
						P[3].x = m_max_x, P[3].y = m_max_y ;			//top_right( m_max_x, m_max_y );
						P[4].x = xi, P[4].y = yi ;						//p1( xi,yi );
						RotatePOINTS( P, 5, -angle, zero );
						if( ( P[0].y < P[4].y && P[1].y < P[4].y && P[2].y < P[4].y && P[3].y < P[4].y ) ||
							( P[0].y > P[4].y && P[1].y > P[4].y && P[2].y > P[4].y && P[3].y > P[4].y ) )
						{
							int test=0;
							// протестировано: работает на ура
							// tested: works with a bang
						}
						else
							draw_flag = 1;
					}
					// now draw the line segment if not clipped
					if( draw_flag )
					{
						if ( el->gtype == DL_LINE )
						{	
							if (el->half_tone)
							{
								float proc = (100.0 - (float)el->half_tone) / 100.0;
								int C1 = (float)m_rgb[layer][0] + (float)((255.0 * B_GND) - m_rgb[layer][0]) * proc;
								int C2 = (float)m_rgb[layer][1] + (float)((255.0 * B_GND) - m_rgb[layer][1]) * proc;
								int C3 = (float)m_rgb[layer][2] + (float)((255.0 * B_GND) - m_rgb[layer][2]) * proc;
								COLORREF HALF_TONE = RGB(C1, C2, C3);
								int w = el->el_w + m_scale * 2.0;
								CPen Half_Pen(PS_SOLID, w, HALF_TONE);
								CPen* old = pDC->SelectObject(&Half_Pen);
								pDC->MoveTo(xi, yi);
								pDC->LineTo(xf, yf);
								pDC->SelectObject(old);
							}
							else
							{
								pDC->MoveTo(xi, yi);
								pDC->LineTo(xf, yf);
							}
						}
						else 
						{
							CPen * old_pen = pDC->SelectObject( &line_pen );
							int sz = el->pts.GetSize();
							if( sz >= 4 )
								pDC->Polyline( &el->pts[2], sz-2 );
							pDC->SelectObject( old_pen );
						}
					}
				}
			}
//-----> next type
			else if( el->gtype == DL_LINES_ARRAY )
			{
				int sz = el->pts.GetSize();
				if( el->transparent == TRANSPARENT_BLACK_GROUND )
				{
					if( old_ROP2 )
						pDC->SetROP2(old_ROP2);
					CPen TBlack__pen( PS_SOLID, el->el_w*7, CLR_BLACK_GND );
					CPen * old = pDC->SelectObject( &TBlack__pen );
					for( int ii=0; ii<sz; ii+=2 )
					{
						pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
						pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
					}
					pDC->SelectObject( old );		
				}	
				//
				// accelerator
				// for BOM text
				if( sz > 5000 )// accelerator for BOM text
				{
					for( int nstep=0; nstep<5; nstep++ )
					{
						for( int ii=2*nstep; ii<sz; ii+=10 )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
						if( draw_layer == SWITCH_LAYER )
							dDC->BitBlt(	SelRect.left, SelRect.bottom, SelRect.right-SelRect.left, SelRect.top-SelRect.bottom,
											pDC, SelRect.left, SelRect.bottom, SRCCOPY ) ;
						bDrawBreak |= ProcessMessages();
						// accelerator
						if( bDrawBreak )
						{
							break;
						}
					}
				}
				else 
				{
					if( el->half_tone )
					{
						float proc = (100.0 - (float)el->half_tone)/100.0;
						int C1 = (float)m_rgb[layer][0] + (float)((255.0*B_GND)-m_rgb[layer][0])*proc;
						int C2 = (float)m_rgb[layer][1] + (float)((255.0*B_GND)-m_rgb[layer][1])*proc;
						int C3 = (float)m_rgb[layer][2] + (float)((255.0*B_GND)-m_rgb[layer][2])*proc;
						COLORREF HALF_TONE = RGB( C1, C2, C3 ); 
						int w = el->el_w + m_scale*2.0;
						CPen Half_Pen( PS_SOLID, w, HALF_TONE );
						CPen * old = pDC->SelectObject( &Half_Pen );
						for( int ii=0; ii<sz; ii+=2 )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
						pDC->SelectObject( old );
					}
					else if( el->id.sst == ID_HATCH )
					{
						int w = el->el_w + m_scale*2.0;
						CPen mod_pen( PS_SOLID, w, *lay_clr );
						CPen * old = pDC->SelectObject( &mod_pen );
						for( int ii=0; ii<sz; ii+=2 )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
						pDC->SelectObject( old );
					}
					else for( int ii=0; ii<sz; ii+=2 )
					{
						pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
						pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
					}
				}
				if( el->transparent == TRANSPARENT_BLACK_GROUND )
					pDC->SetROP2(R2_MERGEPEN);
			}
//-----> next type
			else if( el->gtype == DL_RECT )
			{
				CPen * old_pen = pDC->SelectObject( &line_pen );
				pDC->Rectangle( el->rect.left, 
								el->rect.bottom, 
								el->rect.right, 
								el->rect.top );
				pDC->SelectObject( old_pen );
			}
//-----> next type
			else if( el->gtype == DL_HOLLOW_RECT )
			{
				pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
				pDC->SelectObject( line_pen );
				pDC->MoveTo( el->rect.left, el->rect.bottom );
				pDC->LineTo( el->rect.left, el->rect.top );
				pDC->LineTo( el->rect.right, el->rect.top );
				pDC->LineTo( el->rect.right, el->rect.bottom );
				pDC->LineTo( el->rect.left, el->rect.bottom );			
			}
//-----> next type
			/*else if( el->gtype == DL_RRECT || el->gtype == DL_HOLLOW_RRECT )
			{
				if( el->gtype == DL_HOLLOW_RRECT )
					pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
				pDC->SelectObject( line_pen );
				pDC->RoundRect( el->rect.left, 
								el->rect.bottom, 
								el->rect.right, 
								el->rect.top, 
								2*el->el_w, 
								2*el->el_w );	
			}*/
//-----> next type
			/*else if( el->gtype == DL_POLYGON || el->gtype == DL_POLYLINE )
			{
				int np = el->pts.GetSize();
				if( el->el_w || el->gtype == DL_POLYLINE )
				{
					pDC->MoveTo( el->pts[0].x, el->pts[0].y );
					for( int n=1; n<np; n++ )
						pDC->LineTo( el->pts[n].x, el->pts[n].y );
					if( el->gtype == DL_POLYGON )
						pDC->LineTo( el->pts[0].x, el->pts[0].y );
				}
				if( el->gtype == DL_POLYGON )
				{
					pDC->SelectObject( fill_brush );
					pDC->Polygon( &el->pts[0], np );
				}
			}*/
			else if(el->gtype == DL_ARC_CW ||
					el->gtype == DL_ARC_CCW ||
					el->gtype == DL_CURVE_CW ||
					el->gtype == DL_CURVE_CCW)
			{
				if (el->half_tone)
				{
					float proc = (100.0 - (float)el->half_tone) / 100.0;
					int C1 = (float)m_rgb[layer][0] + (float)((255.0 * B_GND) - m_rgb[layer][0]) * proc;
					int C2 = (float)m_rgb[layer][1] + (float)((255.0 * B_GND) - m_rgb[layer][1]) * proc;
					int C3 = (float)m_rgb[layer][2] + (float)((255.0 * B_GND) - m_rgb[layer][2]) * proc;
					COLORREF HALF_TONE = RGB(C1, C2, C3);
					int w = el->el_w + m_scale * 2.0;
					CPen Half_Pen(PS_SOLID, w, HALF_TONE);
					CPen* old = pDC->SelectObject(&Half_Pen);
					if (el->gtype == DL_ARC_CW)
						DrawArc(pDC, DL_ARC_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
					else if (el->gtype == DL_ARC_CCW)
						DrawArc(pDC, DL_ARC_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
					else if (el->gtype == DL_CURVE_CW)
						DrawCurve(pDC, DL_CURVE_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
					else if (el->gtype == DL_CURVE_CCW)
						DrawCurve(pDC, DL_CURVE_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
					pDC->SelectObject(old);
				}
				else if (el->gtype == DL_ARC_CW)
					DrawArc(pDC, DL_ARC_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
				else if (el->gtype == DL_ARC_CCW)
					DrawArc(pDC, DL_ARC_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
				else if (el->gtype == DL_CURVE_CW)
					DrawCurve(pDC, DL_CURVE_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
				else if (el->gtype == DL_CURVE_CCW)
					DrawCurve(pDC, DL_CURVE_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y);
			}
			if( old_ROP2 )
				pDC->SetROP2(old_ROP2);
		}
		
		// restore original objects
		pDC->SelectObject( old_pen );
		pDC->SelectObject( old_brush );
	}

	// ~~~~~~~~~~~~~~~~~~
	// draw texts
	// ~~~~~~~~~~~~~~~~~~
	if( bDrawBreak == 0 && bScaleChanged == 0 ) 
		for( int order=(MAX_LAYERS-1); order>=0; order-- )
		{
			int layer = m_layer_in_order[order];
			if( !m_vis[layer] || layer == LAY_SELECTION )
				continue;
			
			if( bDrawBreak == 0 )
				bDrawBreak |= ProcessMessages();
			if( order <= swOrder )
				if( bScaleChanged || bDrawBreak )
				{
				
					// accelerator
				
					continue;
				}

			if( start_org_x != m_org_x || start_max_x != m_max_x || start_org_y != m_org_y || start_max_y != m_max_y )
			{
				// coordinates were changed 
				// during drawing
				ASSERT(0);
				return;
			}

			B_GND = ( (m_rgb[layer][0] + m_rgb[layer][1] + m_rgb[layer][2]) < (128*3) );
			COLORREF Layer_color = RGB( m_rgb[layer][0], m_rgb[layer][1], m_rgb[layer][2] );
			COLORREF * lay_clr = &Layer_color;
			CBrush fill_brush( *lay_clr );
			pDC->SelectObject( fill_brush );
			CPen line_pen( PS_SOLID, 0, *lay_clr );
			CPen pen     ( PS_SOLID, 0, *lay_clr );
			pDC->SelectObject( pen );
			dl_element * el = &m_start[layer];
			int nElement = 0;
			int prev_el_w = 0;
			int prev_transparent = 0;

			if( layer == LAY_HILITE && bDrawBreak == 0 )
			{
				nElement = 0;
				el = &m_start[LAY_HILITE];
				while( el->next_HLT != &m_end[LAY_HILITE] )
				{
					el = el->next_HLT;
					if( !el->visible )
						continue;
					if( !m_vis[el->layer] )
							continue;
					if( nElement%64 == 0 )
					{
						if( bDrawBreak == 0 )
							bDrawBreak |= ProcessMessages();
						// accelerator
						if( bDrawBreak )
						{
							break;
						}
					}

					// el rect
					if( el->rect.left > SelRect.right )
						continue;
					if( el->rect.right < SelRect.left )
						continue;
					if( el->rect.bottom > SelRect.top )
						continue;
					if( el->rect.top < SelRect.bottom )
						continue;

					if( el->transparent != prev_transparent || nElement == 0 )
					{
						if( el->transparent == TRANSPARENT_HILITE )
							lay_clr = &Transparent_hilite;	
						else if( el->transparent == TRANSPARENT_BACKGND )
							lay_clr = &Transparent_background;	
						else if( el->transparent == TRANSPARENT_BLACK_GROUND )
							lay_clr = &Transparent_black_ground;
						else // default
							lay_clr = &Layer_color;
						fill_brush.DeleteObject();
						fill_brush.CreateSolidBrush( *lay_clr );
						pen.DeleteObject();
						pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
						pDC->SelectObject( pen );
						pDC->SelectObject( fill_brush );
						//
						line_pen.DeleteObject();
						line_pen.CreatePen( PS_SOLID, 0, *lay_clr );
					}
					else if( el->el_w != prev_el_w )
					{
						pen.DeleteObject();
						pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
						pDC->SelectObject( pen );
					}
					int old_ROP2 = NULL;
					if( el->transparent )
						old_ROP2 = pDC->SetROP2(R2_MERGEPEN);

					prev_el_w = el->el_w;
					prev_transparent = el->transparent;
					nElement++;

					//-----> first type
					if( el->gtype == DL_CIRC || el->gtype == DL_HOLLOW_CIRC )
					{
						if( el->gtype == DL_HOLLOW_CIRC )
							pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
						CPen * old_pen = pDC->SelectObject( &line_pen );
						pDC->Ellipse(	el->rect.left, 
										el->rect.bottom, 
										el->rect.right, 
										el->rect.top );	
						pDC->SelectObject( old_pen );
					}
					//-----> next type
					else if( el->gtype == DL_LINE || el->gtype == DL_HOLLOW_LINE )
					{
						if( el->pts.GetSize() >= 2 )
						{
							// only draw line segments which are in viewport
							// line segment from (xi,yi) to (xf,yf)
							int draw_flag = 0;
							int xi = el->pts[0].x;
							int xf = el->pts[1].x;
							int yi = el->pts[0].y;
							int yf = el->pts[1].y;
							//
							if( el->rect.right < m_max_x && 
								el->rect.left > m_org_x )
								draw_flag = 1;
							else if( el->rect.top < m_max_y && 
									 el->rect.bottom > m_org_y )
								draw_flag = 1;
							else 
							{
								double angle = Angle( xi,yi,xf,yf );
								CPoint P[5];
								P[0].x = m_org_x, P[0].y = m_org_y ;			//bot_left( m_org_x, m_org_y );
								P[1].x = m_org_x, P[1].y = m_max_y ;			//top_left( m_org_x, m_max_y );
								P[2].x = m_max_x, P[2].y = m_org_y ;			//bot_right( m_max_x, m_org_y );
								P[3].x = m_max_x, P[3].y = m_max_y ;			//top_right( m_max_x, m_max_y );
								P[4].x = xi, P[4].y = yi ;						//p1( xi,yi );
								RotatePOINTS( P, 5, -angle, zero );
								if( ( P[0].y < P[4].y && P[1].y < P[4].y && P[2].y < P[4].y && P[3].y < P[4].y ) ||
									( P[0].y > P[4].y && P[1].y > P[4].y && P[2].y > P[4].y && P[3].y > P[4].y ) )
								{
									int test=0;
									// протестировано: работает на ура
									// tested: works with a bang
								}
								else
									draw_flag = 1;
							}
							// now draw the line segment if not clipped
							if( draw_flag )
							{
								if ( el->gtype == DL_LINE )
								{	
									pDC->MoveTo( xi, yi );
									pDC->LineTo( xf, yf );
								}
								else 
								{
									CPen * old_pen = pDC->SelectObject( &line_pen );
									int sz = el->pts.GetSize();
									if( sz >= 4 )
										pDC->Polyline( &el->pts[2], sz-2 );
									pDC->SelectObject( old_pen );
								}
							}
						}
					}
					//-----> next type
					else if( el->gtype == DL_LINES_ARRAY )
					{
						int sz = el->pts.GetSize();
						if( el->transparent == TRANSPARENT_BLACK_GROUND )
						{
							if( old_ROP2 )
								pDC->SetROP2(old_ROP2);
							CPen TBlack__pen( PS_SOLID, el->el_w*7, CLR_BLACK_GND );
							CPen * old = pDC->SelectObject( &TBlack__pen );
							for( int ii=0; ii<sz; ii+=2 )
							{
								pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
								pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
							}
							pDC->SelectObject( old );		
						}			
						for( int ii=0; ii<sz; ii+=2 )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
						if( el->transparent == TRANSPARENT_BLACK_GROUND )
							pDC->SetROP2(R2_MERGEPEN);
					}
					//-----> next type
					else if( el->gtype == DL_RECT )
					{
						CPen * old_pen = pDC->SelectObject( &line_pen );
						pDC->Rectangle( el->rect.left, 
										el->rect.bottom, 
										el->rect.right, 
										el->rect.top );
						pDC->SelectObject( old_pen );
					}
					//-----> next type
					else if( el->gtype == DL_HOLLOW_RECT )
					{
						pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
						pDC->SelectObject( line_pen );
						pDC->MoveTo( el->rect.left, el->rect.bottom );
						pDC->LineTo( el->rect.left, el->rect.top );
						pDC->LineTo( el->rect.right, el->rect.top );
						pDC->LineTo( el->rect.right, el->rect.bottom );
						pDC->LineTo( el->rect.left, el->rect.bottom );		
					}
					//-----> next type
					/*else if( el->gtype == DL_RRECT || el->gtype == DL_HOLLOW_RRECT )
					{
						if( el->gtype == DL_HOLLOW_RRECT )
							pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
						pDC->SelectObject( line_pen );
						pDC->RoundRect( el->rect.left, 
										el->rect.bottom, 
										el->rect.right, 
										el->rect.top, 
										2*el->el_w, 
										2*el->el_w );	
					}*/
					//-----> next type
					/*else if( el->gtype == DL_POLYGON || el->gtype == DL_POLYLINE )
					{
						int np = el->pts.GetSize();
						if( el->el_w || el->gtype == DL_POLYLINE )
						{
							pDC->MoveTo( el->pts[0].x, el->pts[0].y );
							for( int n=1; n<np; n++ )
								pDC->LineTo( el->pts[n].x, el->pts[n].y );
							if( el->gtype == DL_POLYGON )
								pDC->LineTo( el->pts[0].x, el->pts[0].y );
						}
						if( el->gtype == DL_POLYGON )
						{
							pDC->SelectObject( fill_brush );
							pDC->Polygon( &el->pts[0], np );
						}
					}*/
					//-----> next type
					else if( el->gtype == DL_ARC_CW )
					{
						DrawArc( pDC, DL_ARC_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y );
					}
					//-----> next type
					else if( el->gtype == DL_ARC_CCW )
					{
						DrawArc( pDC, DL_ARC_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y );
					}
					//-----> next type
					else if( el->gtype == DL_CURVE_CW )
					{
						DrawCurve( pDC, DL_CURVE_CW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y );
					}
					//-----> next type
					else if( el->gtype == DL_CURVE_CCW )
					{
						DrawCurve( pDC, DL_CURVE_CCW, el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y );
					}
					if( old_ROP2 )
						pDC->SetROP2(old_ROP2);
				}
			}
			else while( el->next->next )
			{
				el = el->next;
				if( !el->visible )
					continue;
				if( el->id.type != ID_TEXT )
					continue;

				if( bDrawBreak )
				{
					break;
				}

				if( nElement%64 == 0 && bDrawBreak == 0 )
					bDrawBreak |= ProcessMessages();
				nElement++;

				// if el hilite
				if( layer == LAY_HILITE ) // (if orig layer not vis)	
				{			
					if( !m_vis[el->layer] )
						continue;
				}
				
				// el rect
				if( el->rect.left > SelRect.right )
					continue;
				if( el->rect.right < SelRect.left )
					continue;
				if( el->rect.bottom > SelRect.top )
					continue;
				if( el->rect.top < SelRect.bottom )
					continue;

				if((el->rect.top - el->rect.bottom <= size_of_2_pixels ) && ( el->rect.right - el->rect.left <= size_of_2_pixels ))
					continue;

				
				if( (el->transparent && layer == LAY_HILITE ) ||
					(el->transparent == 0 && prev_transparent) )
				{
					if( el->transparent == TRANSPARENT_HILITE )
						lay_clr = &Transparent_hilite;	
					else if( el->transparent == TRANSPARENT_BACKGND )
						lay_clr = &Transparent_background;	
					else if( el->transparent == TRANSPARENT_BLACK_GROUND )
						lay_clr = &Transparent_black_ground;
					else // default
						lay_clr = &Layer_color;
					fill_brush.DeleteObject();
					fill_brush.CreateSolidBrush( *lay_clr );
					pen.DeleteObject();
					pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
					pDC->SelectObject( pen );
					pDC->SelectObject( fill_brush );
					//
					line_pen.DeleteObject();
					line_pen.CreatePen( PS_SOLID, 0, *lay_clr );
				}
				else if( el->el_w != prev_el_w )
				{
					pen.DeleteObject();
					pen.CreatePen( PS_SOLID, el->el_w, *lay_clr );
					pDC->SelectObject( pen );
				}
				int old_ROP2 = NULL;
				if( el->transparent && layer == LAY_HILITE )
					old_ROP2 = pDC->SetROP2(R2_MERGEPEN);

				prev_el_w = el->el_w;
				prev_transparent = el->transparent;

				//-----> txt type
				if( el->gtype == DL_LINES_ARRAY )
				{
					int sz = el->pts.GetSize();
					if( el->transparent == TRANSPARENT_BLACK_GROUND )
					{
						if( old_ROP2 )
							pDC->SetROP2(old_ROP2);
						CPen TBlack__pen( PS_SOLID, el->el_w*7, CLR_BLACK_GND );
						CPen * old = pDC->SelectObject( &TBlack__pen );
						for( int ii=0; ii<sz; ii+=2 )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
						pDC->SelectObject( old );		
					}	
					//
					// accelerator
					// for BOM text
					if( sz > 5000 )// accelerator for BOM text
					{
						for( int nstep=0; nstep<5; nstep++ )
						{
							for( int ii=2*nstep; ii<sz; ii+=10 )
							{
								pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
								pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
							}
							if( draw_layer == SWITCH_LAYER )
								dDC->BitBlt(	SelRect.left, SelRect.bottom, SelRect.right-SelRect.left, SelRect.top-SelRect.bottom,
												pDC, SelRect.left, SelRect.bottom, SRCCOPY ) ;
							bDrawBreak |= ProcessMessages();
							// accelerator
							if( bDrawBreak )
							{
								break;
							}
						}
					}
					else 
					{
						if( el->half_tone )
						{
							float proc = (100.0 - (float)el->half_tone)/100.0;
							int C1 = (float)m_rgb[layer][0] + (float)((255.0*B_GND)-m_rgb[layer][0])*proc;
							int C2 = (float)m_rgb[layer][1] + (float)((255.0*B_GND)-m_rgb[layer][1])*proc;
							int C3 = (float)m_rgb[layer][2] + (float)((255.0*B_GND)-m_rgb[layer][2])*proc;
							COLORREF HALF_TONE = RGB( C1, C2, C3 ); 
							int w = el->el_w + m_scale;
							CPen Half_Pen( PS_SOLID, w, HALF_TONE );
							CPen * old = pDC->SelectObject( &Half_Pen );
							for( int ii=0; ii<sz; ii+=(bDrawBreak?4:2) )
							{
								pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
								pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
							}
							pDC->SelectObject( old );
						}
						else if( el->id.sst == ID_HATCH )
						{
							int w = el->el_w + m_scale;
							CPen mod_pen( PS_SOLID, w, *lay_clr );
							CPen * old = pDC->SelectObject( &mod_pen );
							for( int ii=0; ii<sz; ii+=(bDrawBreak?4:2) )
							{
								pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
								pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
							}
							pDC->SelectObject( old );
						}
						else for( int ii=0; ii<sz; ii+=(bDrawBreak?4:2) )
						{
							pDC->MoveTo( el->pts[ii].x, el->pts[ii].y );
							pDC->LineTo( el->pts[ii+1].x, el->pts[ii+1].y );
						}
					}
					if( el->transparent == TRANSPARENT_BLACK_GROUND )
						pDC->SetROP2(R2_MERGEPEN);
				}
			}
		}
	// visual grid
	if( m_visual_grid_on && (m_visual_grid_spacing/m_scale)>5 && m_vis[LAY_VISIBLE_GRID] )
	{
		if( !m_grid_style )
		{
			int startix = m_org_x/m_visual_grid_spacing;
			int startiy = m_org_y/m_visual_grid_spacing;
			double start_grid_x = startix*m_visual_grid_spacing;
			double start_grid_y = startiy*m_visual_grid_spacing;
			COLORREF GRD = RGB(	m_rgb[LAY_VISIBLE_GRID][0], 
							m_rgb[LAY_VISIBLE_GRID][1], 
							m_rgb[LAY_VISIBLE_GRID][2] );
			for( double ix=start_grid_x; ix<m_max_x; ix+=m_visual_grid_spacing )
			{
				for( double iy=start_grid_y; iy<m_max_y; iy+=m_visual_grid_spacing )
			 	{
					pDC->SetPixel(ix,iy,GRD);
				}
			}
		}
	}
	
	// draw origin
	CRect r;
	r.top = m_scale*15;
	r.left = -m_scale*15;
	r.right = m_scale*15;
	r.bottom = -m_scale*15;
	CPen pen;
	if( B_GND )
		pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
	else
		pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	pDC->SelectObject( &pen );
	pDC->SelectObject( GetStockObject( GRAY_BRUSH ) );
	pDC->Ellipse( r );
	pDC->SelectObject( GetStockObject( LTGRAY_BRUSH ) );
	r.top = m_scale*10;
	r.left = -m_scale*10;
	r.right = m_scale*10;
	r.bottom = -m_scale*10;
	pDC->Ellipse( r );
	pDC->MoveTo( -m_scale*30, 0 );
	pDC->LineTo( -m_scale*15, 0 );
	pDC->MoveTo( m_scale*30, 0 );
	pDC->LineTo( m_scale*15, 0 );
	pDC->MoveTo( 0, -m_scale*30 );
	pDC->LineTo( 0, -m_scale*15 );
	pDC->MoveTo( 0, m_scale*30 );
	pDC->LineTo( 0, m_scale*15 );

	//
	// Draw Text Info
	RECT tr;
	tr.bottom = m_bottom_pane_h;
	tr.top = tr.bottom - 20;
	tr.left = 110;
	tr.right = tr.left + 200;
	pDC->DrawText("restore original objects", -1, &tr, DT_TOP );

	// restore original objects
	pDC->SelectObject( old_pen );
	pDC->SelectObject( old_brush );
	
	// double-buffer to screen
	if( topDC )
		dDC->BitBlt(	client_rect.left, client_rect.bottom, client_rect.right-client_rect.left, client_rect.top-client_rect.bottom,
					topDC, client_rect.left, client_rect.bottom, SRCCOPY ) ;
	else if( botDC )
		dDC->BitBlt(	client_rect.left, client_rect.bottom, client_rect.right-client_rect.left, client_rect.top-client_rect.bottom,
								botDC, client_rect.left, client_rect.bottom, SRCCOPY ) ;
	// finalize
	m_drag_start = 0;
	if( bDrawBreak == 0 )
	{	
		bDrawBreak |= ProcessMessages();
		if( bDrawBreak == 0 )
		{	
			if( theApp.m_Doc->m_view && draw_layer == SWITCH_LAYER )
				theApp.m_Doc->m_view->CheckMessages();
			else if( bScaleChanged )
			{	
				bScaleChanged = 0;
				mem_org_x = client_rect.left;
				mem_max_x = client_rect.right;
				mem_org_y = client_rect.bottom;
				mem_max_y = client_rect.top;
				Draw( dDC, SWITCH_LAYER );
			}
		}
	}
	bDrawBreak = -1; // end drawing
}

// set the display color for a layer
//
void CDisplayList::SetLayerRGB( int layer, int r, int g, int b )
{
	m_rgb[layer][0] = r;
	m_rgb[layer][1] = g;
	m_rgb[layer][2] = b;
}

void CDisplayList::SetLayerVisible( int layer, BOOL vis )
{
	m_vis[layer] = vis;
}

// test x,y for a hit on an item in the selection layer
// creates arrays with layer and id of each hit item
// assigns priority based on layer and id
// then returns pointer to item with highest priority
// If exclude_id != NULL, excludes item with 
// id == exclude_id and ptr == exclude_ptr 
// If include_id != NULL, only include items that match include_id[]
// where n_include_ids is size of array, and
// where 0's in include_id[] fields are treated as wildcards
//
void * CDisplayList::TestSelect( int x, int y, id * sel_id, int * sel_layer, 
								id * exclude_id, void * exclude_ptr, 
								id * include_id, int n_include_ids, int MAX_DIST, CArray<CPolyLine> * polyarr )
{
	enum {
		MAX_HITS = 200
	};
	int  nhits = 0;
	int hit_order[MAX_HITS];
	int hit_layer_map[MAX_HITS];
	id hit_id[MAX_HITS];
	void * hit_ptr[MAX_HITS];
	long long S[MAX_HITS];

	int xx = x/m_pcbu_per_wu;
	int yy = y/m_pcbu_per_wu;
	MAX_DIST /= m_pcbu_per_wu;

	// traverse the list, looking for selection shapes
	dl_element * el = &m_start[LAY_SELECTION];
	RECT rgr;
	BOOL bCONNECTWASMADE = 0;
	int min_distance = MAX_DIST;
	int prev_distance = 1;
	while( el->next != &m_end[LAY_SELECTION] )
	{
		el = el->next;	
		if( el->visible == 0 )
			continue;
		// don't select anything on an invisible layer
		if( m_vis[el->layer] == 0 )
			continue;
		// don't select anything invisible
		if( xx+MAX_DIST < el->rect.left )
			continue;
		if( xx-MAX_DIST > el->rect.right )
			continue;
		if( yy+MAX_DIST < el->rect.bottom )
			continue;
		if( yy-MAX_DIST > el->rect.top )
			continue;
		
		// Ok 
		BOOL Ok = FALSE;
		BOOL excluded_hit = FALSE;
		BOOL included_hit = TRUE;
		if( exclude_id )
		{
			if( el->ptr == exclude_ptr )
				if( exclude_id->type == el->id.type
					&& ( exclude_id->st == exclude_id->def || exclude_id->st == el->id.st )
					&& ( exclude_id->i == exclude_id->def || exclude_id->i == el->id.i )
					&& ( exclude_id->sst == exclude_id->def || exclude_id->sst == el->id.sst )
					&& ( exclude_id->ii == exclude_id->def || exclude_id->ii == el->id.ii ) )
					excluded_hit = TRUE;
		}
		if( include_id )
		{
			included_hit = FALSE;
			for( int inc=0; inc<n_include_ids; inc++ )
			{
				id * inc_id = &include_id[inc];
				if( inc_id->type == el->id.type
					&& ( inc_id->st == inc_id->def || inc_id->st == el->id.st )
					&& ( inc_id->i == inc_id->def || inc_id->i == el->id.i )
					&& ( inc_id->sst == inc_id->def || inc_id->sst == el->id.sst )
					&& ( inc_id->ii == inc_id->def || inc_id->ii == el->id.ii ) )
				{
					included_hit = TRUE;
					break;
				}
			}
		}
		if( excluded_hit || !included_hit )
			continue;
		if( CText * el_t = (CText*)el->ptr )
		{
			if( el_t->m_font_size == 0 )
				continue;
		}
		// Ok 
		Ok = FALSE;
		BOOL isCONNECT = (	el->layer == LAY_PIN_LINE || 
							el->layer == LAY_CONNECTION || 
							el->layer == LAY_FREE_LINE );
		int min_d = min_distance;
		if( bCONNECTWASMADE && isCONNECT == 0 )
			min_d = 0;
		if( el->gtype == DL_RECT || el->gtype == DL_HOLLOW_RECT )
		{
			// OK, hit
			int b1 = 0, b2 = 0;
			if( xx < el->rect.left )
			{
				b1 = 1;
				if( el->rect.left - xx < min_d )
				{
					Ok = TRUE;
					bCONNECTWASMADE |= isCONNECT;
					min_distance = el->rect.left - xx;
				}
			}
			else if( xx > el->rect.right )
			{
				b1 = 1;
				if( xx - el->rect.right < min_d )
				{
					Ok = TRUE;
					bCONNECTWASMADE |= isCONNECT;
					min_distance = xx - el->rect.right;
				}
			}
			// vertical test
			if( yy < el->rect.bottom )
			{
				b2 = 1;
				if( el->rect.bottom - yy < min_d )
				{
					Ok = TRUE;
					bCONNECTWASMADE |= isCONNECT;
					min_distance = el->rect.bottom - yy;
				}
			}
			else if( yy > el->rect.top )
			{
				b2 = 1;
				if( yy - el->rect.top < min_d )
				{
					Ok = TRUE;
					bCONNECTWASMADE |= isCONNECT;
					min_distance = yy - el->rect.top;
				}
			}
			if( b1 == 0 && b2 == 0 )
			{
				Ok = TRUE;
				min_distance = 0;
			}
		}
		else if(	el->gtype == DL_ARC_CW || el->gtype == DL_ARC_CCW || 
					el->gtype == DL_CURVE_CW || el->gtype == DL_CURVE_CCW )
		{
			int tp;
			if( el->gtype == DL_CURVE_CW || el->gtype == DL_ARC_CW )
				tp = CPolyLine::ARC_CW;
			else 
				tp = CPolyLine::ARC_CCW;
			CArray<CPoint> * pts = Get_Points( el, NULL, 0 );
			if( pts->GetSize() >= 2 )
			{
				int w = max( abs(el->el_w)/2, 4.0*m_scale );
				w = max( w, 2 );
				int px=0,py=0;
				int dd = ::GetClearanceBetweenSegments(	xx-10, yy-10, xx+10, yy+10, CPolyLine::STRAIGHT, 1, 
														(*pts)[0].x, (*pts)[0].y, (*pts)[1].x, (*pts)[1].y, tp, w, (abs(el->el_w)+1)*10, NULL, NULL );
				if( dd <= min_d )
				{
					Ok = TRUE;
					bCONNECTWASMADE |= isCONNECT;
					min_distance = dd;
				}
			}
		}
		else if( el->gtype == DL_LINE || el->gtype == DL_HOLLOW_LINE )
		{
			int w = abs(el->el_w);
			w = max( w, 2 );
			int gx=0, gy=0;
			int test = ::GetClearanceBetweenSegments( el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y, CPolyLine::STRAIGHT, w, 
														xx, yy, xx+10, yy+10, CPolyLine::STRAIGHT, 1, MAX_DIST*2, &gx, &gy );	
			if( test <= min_distance )
			{
				// OK, hit
				Ok = TRUE;
				min_distance = test;
			}
		}
		else if( el->gtype == DL_HOLLOW_CIRC || el->gtype == DL_CIRC || el->gtype == DL_HOLE )
		{
			int w = (el->rect.right - el->rect.left)/2;
			int x = (el->rect.right + el->rect.left)/2;
			int y = (el->rect.top + el->rect.bottom)/2;
			int d = Distance( x, y, xx, yy );
			if( d-w <= min_d )
			{
				// OK, hit
				Ok = TRUE;
				bCONNECTWASMADE |= isCONNECT;

				if( polyarr )
				{
					if( el->id.type == ID_POLYLINE )
					{
						CPolyLine * p = &polyarr->GetAt( el->id.i );
						if( CText * t = p->Check( index_part_attr ) )
							Ok = FALSE;
					}
				}
				if( Ok )
					min_distance = d-w;//max(0,d-w);
			}
		}
		else if( el->gtype == DL_POLYGON || el->gtype == DL_POLYLINE )
		{
			int d_test = MAX_DIST;
			if( TestPolygon( xx, yy, &el->pts[0], el->pts.GetSize(), &d_test ) )
			{
				///if( d_test <= min_d )
				{
					Ok = TRUE;
					///min_distance = d_test;
				}
			}
		}

		if( !Ok )
			continue;
		
		hit_order[nhits] = MAX_LAYERS-1;
		///for( int layer=0; layer<MAX_LAYERS; layer++ )
		///{ // set priority
			///int layer = m_layer_in_order[order];
			if( el->layer > LAY_HILITE )
			{
				if( el->layer == LAY_CONNECTION )
					hit_order[nhits] = 0;
				else if( el->layer == LAY_PIN_LINE && m_drag_flag )
					hit_order[nhits] = 0;
				else if( el->layer == LAY_ADD_1 && m_drag_flag )
					hit_order[nhits] = 0;
				else if( el->layer == LAY_PART_OUTLINE )
					hit_order[nhits] = MAX_LAYERS-1;
				else 
					hit_order[nhits] = el->layer;
			}
		///}
		hit_layer_map[nhits] = el->layer;
		hit_id[nhits] = el->id;
		hit_ptr[nhits] = el->ptr;
		if( el->gtype == DL_LINE || el->gtype == DL_HOLLOW_LINE )
		{
			long long d = Distance( el->pts[0].x, el->pts[0].y, el->pts[1].x, el->pts[1].y );
			long long w = el->el_w;
			S[nhits] = w*(d+w);
		}
		else
			S[nhits] = (long long)(el->rect.right - el->rect.left)*(long long)(el->rect.top - el->rect.bottom);
		//
		if( nhits )
			if( prev_distance >= min_distance )
				S[nhits-1] = INT_MAX;
		if( nhits < MAX_HITS-1 )
			nhits++;
		prev_distance = min_distance;
	}
	// now return highest priority hit
	if( nhits == 0 )
	{
		// no hit
		id no_id;
		*sel_id = no_id;
		*sel_layer = 0;
		return NULL;
	}
	else
	{
		// assign priority to each hit, track maximum, exclude exclude_id item
		int best_hit = nhits;	
		S[nhits] = INT_MAX;
		hit_order[nhits] = MAX_LAYERS-1;
		for( int i=0; i<nhits; i++ )
		{
			if( hit_order[i] < hit_order[best_hit] )
				best_hit = i;
			else if( hit_order[i] == hit_order[best_hit] && S[i] < S[best_hit] )
				best_hit = i;
		}
		if( best_hit == nhits )
		{
			id no_id;
			*sel_id = no_id;
			*sel_layer = 0;
			return NULL;
		}
		*sel_id = hit_id[best_hit];

		// sel layer
		*sel_layer = m_layer_in_order[hit_order[best_hit]];
		return hit_ptr[best_hit];
	}
}

// Start dragging arrays of drag lines and ratlines
// Assumes that arrays have already been set up using MakeLineArray, etc.
// If no arrays, just drags point
//
int CDisplayList::StartDraggingArray( CDC * pDC, int xx, int yy, int crosshair )
{
	// convert to display units
	int x = xx/m_pcbu_per_wu;
	int y = yy/m_pcbu_per_wu;

	// cancel dragging non-array shape
	m_drag_flag = 0;
	m_drag_shape = 0;
	m_drag_start = 0;

	// set up for dragging array
	m_drag_x = x;			// "grab point"
	m_drag_y = y;
	m_drag_layer = LAY_SELECTION;
	m_drag_angle = 0;
	m_drag_side = 0;
	SetUpCrosshairs( crosshair, x, y );
	
	// done
	return 0;
}

// Start dragging rectangle 
//
int CDisplayList::StartDraggingRectangle( CDC * pDC, int x, int y, int xi, int yi,
										 int xf, int yf, int layer )
{
	// create drag lines
	CPoint p1(xi,yi);
	CPoint p2(xf,yi);
	CPoint p3(xf,yf);
	CPoint p4(xi,yf);
	MakeDragLineArray( 4 );
	AddDragLine( p1, p2 ); 
	AddDragLine( p2, p3 ); 
	AddDragLine( p3, p4 ); 
	AddDragLine( p4, p1 ); 

	StartDraggingArray( pDC, x, y );
	
	// done
	return 0;
}


// Start dragging line 
//
int CDisplayList::StartDraggingRatLine( CDC * pDC, int x, int y, int xi, int yi, 
									   int layer, int w, int crosshair )
{
	// create drag line
	CPoint p1(xi,yi);
	CPoint p2(x,y);
	MakeDragRatlineArray( 1, w );
	AddDragRatline( p1, p2 ); 

	StartDraggingArray( pDC, xi, yi, crosshair );
	
	// done
	return 0;
}

// set style of arc being dragged, using CPolyLine styles
//
void CDisplayList::SetDragArcStyle( int style )
{
	if( style == CPolyLine::STRAIGHT )
		m_drag_shape = DS_ARC_STRAIGHT;
	else if( style == CPolyLine::ARC_CW )
		m_drag_shape = DS_ARC_CW;
	else if( style == CPolyLine::ARC_CCW )
		m_drag_shape = DS_ARC_CCW;
}

// Start dragging arc endpoint, using style from CPolyLine
// Use the layer color and width w
//
int CDisplayList::StartDraggingArc( CDC * pDC, int style, int xx, int yy, int xi, int yi, 
								   int layer, int w, int crosshair )
{
	int x = xx/m_pcbu_per_wu;
	int y = yy/m_pcbu_per_wu;

	// set up for dragging
	m_drag_flag = 1;
	m_drag_start = 0;

	if( style == CPolyLine::STRAIGHT )
		m_drag_shape = DS_ARC_STRAIGHT;
	else if( style == CPolyLine::ARC_CW )
		m_drag_shape = DS_ARC_CW;
	else if( style == CPolyLine::ARC_CCW )
		m_drag_shape = DS_ARC_CCW;
	m_drag_x = x;	// position of endpoint (at cursor)
	m_drag_y = y;
	m_drag_xi = xi/m_pcbu_per_wu;	// start point
	m_drag_yi = yi/m_pcbu_per_wu;
	m_drag_side = 0;
	m_drag_layer_1 = layer;
	m_drag_w1 = w/m_pcbu_per_wu;

	// set up cross hairs
	SetUpCrosshairs( crosshair, x, y );

	// done
	return 0;
}

// Start dragging the selected line endpoint
// Use the layer1 color and width w
//
int CDisplayList::StartDraggingLine( CDC * pDC, int x, int y, int xi, int yi, int layer1, int w,
									int layer_no_via, int via_w, int via_holew,
									int crosshair, int style, int inflection_mode )
{
	// set up for dragging
	m_drag_flag = 1;
	m_drag_shape = DS_LINE;
	m_drag_start = 0;
	//
	m_drag_x = x/m_pcbu_per_wu;	// position of endpoint (at cursor)
	m_drag_y = y/m_pcbu_per_wu;
	m_drag_xi = xi/m_pcbu_per_wu;	// initial vertex
	m_drag_yi = yi/m_pcbu_per_wu;
	m_drag_side = 0;
	m_drag_layer_1 = layer1;
	m_drag_w1 = w/m_pcbu_per_wu;
	m_drag_style1 = style;
	m_drag_layer_no_via = layer_no_via;
	m_drag_via_w = via_w/m_pcbu_per_wu;
	m_drag_via_holew = via_holew/m_pcbu_per_wu;
	m_drag_via_drawn = 0;
	m_inflection_mode = inflection_mode;
	m_last_inflection_mode = IM_NONE;

	// set up cross hairs
	SetUpCrosshairs( crosshair, x, y );

	//Redraw
//	Draw( pDC );

	// done
	return 0;
}

// Start dragging line vertex (i.e. vertex between 2 line segments)
// Use the layer1 color and width w1 for the first segment from (xi,yi) to the vertex,
// Use the layer2 color and width w2 for the second segment from the vertex to (xf,yf)
// While dragging, insert via at start point of first segment if layer1 != layer_no_via
// using via parameters via_w and via_holew
// Note that layer1 may be changed while dragging by ChangeRouting Layer()
// If dir = 1, swap start and end points
//
int CDisplayList::StartDraggingLineVertex( CDC * pDC, int x, int y, 
									int xi, int yi, int xf, int yf,
									int layer1, int layer2, int w1, int w2, int style1, int style2,
									int layer_no_via, int via_w, int via_holew, int dir,
									int crosshair )
{
	// set up for dragging
	m_drag_flag = 1;
	m_drag_shape = DS_LINE_VERTEX;
	m_drag_start = 0;
	m_drag_x = x/m_pcbu_per_wu;	// position of central vertex (at cursor)
	m_drag_y = y/m_pcbu_per_wu;
	if( dir == 0 )
	{
		// routing forward
		m_drag_xi = xi/m_pcbu_per_wu;	// initial vertex
		m_drag_yi = yi/m_pcbu_per_wu;
		m_drag_xf = xf/m_pcbu_per_wu;	// final vertex
		m_drag_yf = yf/m_pcbu_per_wu;
	}
	else
	{
		// routing backward
		m_drag_xi = xf/m_pcbu_per_wu;	// initial vertex
		m_drag_yi = yf/m_pcbu_per_wu;
		m_drag_xf = xi/m_pcbu_per_wu;	// final vertex
		m_drag_yf = yi/m_pcbu_per_wu;
	}
	m_drag_side = 0;
	m_drag_layer_1 = layer1;
	m_drag_layer_2 = layer2;
	m_drag_w1 = w1/m_pcbu_per_wu;
	m_drag_w2 = w2/m_pcbu_per_wu;
	m_drag_style1 = style1;
	m_drag_style2 = style2;
	m_drag_layer_no_via = layer_no_via;
	m_drag_via_w = via_w/m_pcbu_per_wu;
	m_drag_via_holew = via_holew/m_pcbu_per_wu;
	m_drag_via_drawn = 0;

	// set up cross hairs
	SetUpCrosshairs( crosshair, x, y );

	//Redraw
	//Draw( pDC );

	// done
	return 0;
}


// Start dragging line segment (i.e. line segment between 2 vertices)
// Use the layer0 color and width w0 for the "before" segment from (xb,yb) to (xi, yi),
// Use the layer1 color and width w1 for the moving segment from (xi,yi) to (xf, yf),
// Use the layer2 color and width w2 for the ending segment from (xf, yf) to (xe,ye)
// While dragging, insert via at (xi, yi) if layer1 != layer_no_via1, insert via
// at (xf, yf) if layer2 != layer_no_via.
// using via parameters via_w and via_holew
// Note that layer1 may be changed while dragging by ChangeRouting Layer()
// If dir = 1, swap start and end points
//
int CDisplayList::StartDraggingLineSegment( CDC * pDC, int x, int y,
									int xb, int yb,
									int xi, int yi, 
									int xf, int yf,
									int xe, int ye,
									int layer0, int layer1, int layer2,
									int w0,		int w1,		int w2,
									int style0, int style1, int style2,
									
									int layer_no_via, int via_w, int via_holew, 
									int crosshair )
{
	// set up for dragging
	m_drag_flag = 1;
	m_drag_shape = DS_SEGMENT;
	m_drag_start = 0;
	m_drag_x = x/m_pcbu_per_wu;	// position of reference point (at cursor)
	m_drag_y = y/m_pcbu_per_wu;

	m_drag_xb = xb/m_pcbu_per_wu;	// vertex before
	m_drag_yb = yb/m_pcbu_per_wu;
	m_drag_xi = xi/m_pcbu_per_wu;	// initial vertex
	m_drag_yi = yi/m_pcbu_per_wu;
	m_drag_xf = xf/m_pcbu_per_wu;	// final vertex
	m_drag_yf = yf/m_pcbu_per_wu;
	m_drag_xe = xe/m_pcbu_per_wu;	// End vertex
	m_drag_ye = ye/m_pcbu_per_wu;

	m_drag_side = 0;
	m_drag_layer_0 = layer0;
	m_drag_layer_1 = layer1;
	m_drag_layer_2 = layer2;

	m_drag_w0 = w0/m_pcbu_per_wu;
	m_drag_w1 = w1/m_pcbu_per_wu;
	m_drag_w2 = w2/m_pcbu_per_wu;

	m_drag_style0 = style0;
	m_drag_style1 = style1;
	m_drag_style2 = style2;

	m_drag_layer_no_via = layer_no_via;
	m_drag_via_w = via_w/m_pcbu_per_wu;
	m_drag_via_holew = via_holew/m_pcbu_per_wu;
	m_drag_via_drawn = 0;

	// set up cross hairs
	SetUpCrosshairs( crosshair, x, y );

	//Redraw
	Draw( pDC, NULL );

	// done
	return 0;
}

// Drag graphics with cursor 
//
void CDisplayList::Drag( CDC * pDC, int x, int y )
{	
	// convert from PCB to display coords
	int xx = x/m_pcbu_per_wu;
	int yy = y/m_pcbu_per_wu;
	
	// set XOR pen mode for dragging
	int old_ROP2 = pDC->SetROP2( R2_XORPEN );

	//**** there are three dragging modes, which may be used simultaneously ****//
	COLORREF drag_color = RGB( m_rgb[m_drag_layer][0], m_rgb[m_drag_layer][1], m_rgb[m_drag_layer][2] );
	// drag array of lines, used to make complex graphics like a part
	if( m_drag_num_lines )
	{
		int wL = 1;
		if( m_drag_num_lines < 20 )// for pin
			wL = 4*m_scale;
		CPen drag_pen( PS_SOLID, wL, drag_color );
		CPen * old_pen = pDC->SelectObject( &drag_pen );
		for( int il=0; il<m_drag_num_lines; il++ )
		{
			// undraw
			if( m_drag_start )
			{
				pDC->MoveTo( m_drag_x+m_drag_line_pt[2*il].x, m_drag_y+m_drag_line_pt[2*il].y );
				pDC->LineTo( m_drag_x+m_drag_line_pt[2*il+1].x, m_drag_y+m_drag_line_pt[2*il+1].y );
			}
			// redraw
			pDC->MoveTo( xx+m_drag_line_pt[2*il].x, yy+m_drag_line_pt[2*il].y );
			pDC->LineTo( xx+m_drag_line_pt[2*il+1].x, yy+m_drag_line_pt[2*il+1].y );
		}
		pDC->SelectObject( old_pen );
	}

	// drag array of rubberband lines, used for ratlines to dragged part
	if( m_drag_num_ratlines )
	{
		CPen drag_pen( PS_SOLID, 1, drag_color );
		CPen * old_pen = pDC->SelectObject( &drag_pen );
		for( int il=0; il<m_drag_num_ratlines; il++ )
		{
			// undraw
			if( m_drag_start )
			{
				pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
				pDC->LineTo( m_drag_x+m_drag_ratline_end_pt[il].x, m_drag_y+m_drag_ratline_end_pt[il].y );
			}
			// draw
			pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
			pDC->LineTo( xx+m_drag_ratline_end_pt[il].x, yy+m_drag_ratline_end_pt[il].y );
		}
		pDC->SelectObject( old_pen );
	}

	// draw special shapes, used for polyline sides and trace segments
	if( m_drag_flag )
	{
		if( m_drag_shape == DS_LINE_VERTEX || m_drag_shape == DS_LINE )
		{
			// drag rubberband trace segment, or vertex between two rubberband segments
			// used for routing traces
			CPen pen_w( PS_SOLID, m_drag_w1, drag_color );

			// undraw first segment
			CPen * old_pen = pDC->SelectObject( &pen_w );
			if( m_drag_start )
			{
				if( m_drag_style1 == DSS_STRAIGHT )
				{
					if( m_last_inflection_mode == IM_NONE )
					{
						pDC->MoveTo( m_drag_xi, m_drag_yi );
						pDC->LineTo( m_drag_x, m_drag_y );
					}
					else
					{
						CPoint pi( m_drag_xi, m_drag_yi );
						CPoint pf( m_drag_x, m_drag_y );
						CPoint p = GetInflectionPoint( pi, pf, m_last_inflection_mode );
						pDC->MoveTo( m_drag_xi, m_drag_yi );
						if( p != pi )
							pDC->LineTo( p.x, p.y );
						pDC->LineTo( m_drag_x, m_drag_y );
					}
				}
				else if( m_drag_style1 == DSS_ARC_CW )
					DrawArc( pDC, DL_ARC_CW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_drag_style1 == DSS_ARC_CCW )
					DrawArc( pDC, DL_ARC_CCW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_drag_style1 == DSS_CURVE_CW )
					DrawCurve( pDC, DL_CURVE_CW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_drag_style1 == DSS_CURVE_CCW )
					DrawCurve( pDC, DL_CURVE_CCW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else 
					ASSERT(0);
				if( m_drag_shape == DS_LINE_VERTEX )
				{
					// undraw second segment
					CPen pen( PS_SOLID, m_drag_w2, drag_color );
					CPen * old_pen = pDC->SelectObject( &pen );
					if( m_drag_style2 == DSS_STRAIGHT )
						pDC->LineTo( m_drag_xf, m_drag_yf );
					else if( m_drag_style2 == DSS_ARC_CW )
						DrawArc( pDC, DL_ARC_CW, m_drag_x, m_drag_y, m_drag_xf, m_drag_yf );
					else if( m_drag_style2 == DSS_ARC_CCW )
						DrawArc( pDC, DL_ARC_CCW, m_drag_x, m_drag_y, m_drag_xf, m_drag_yf );
					else if( m_drag_style2 == DSS_CURVE_CW )
						DrawCurve( pDC, DL_CURVE_CW, m_drag_x, m_drag_y, m_drag_xf, m_drag_yf );
					else if( m_drag_style2 == DSS_CURVE_CCW )
						DrawCurve( pDC, DL_CURVE_CCW, m_drag_x, m_drag_y, m_drag_xf, m_drag_yf );
					else
						ASSERT(0);
					pDC->SelectObject( old_pen );
				}
			}
			// draw first segment
			if( m_drag_style1 == DSS_STRAIGHT ) 
			{
				if( m_inflection_mode == IM_NONE )
				{
					pDC->MoveTo( m_drag_xi, m_drag_yi );
					pDC->LineTo( xx, yy );
				}
				else
				{
					CPoint pi( m_drag_xi, m_drag_yi );
					CPoint pf( xx, yy );
					CPoint p = GetInflectionPoint( pi, pf, m_inflection_mode );
					pDC->MoveTo( m_drag_xi, m_drag_yi );
					if( p != pi )
						pDC->LineTo( p.x, p.y );
					pDC->LineTo( xx, yy );
				}
				m_last_inflection_mode = m_inflection_mode;
			}
			else if( m_drag_style1 == DSS_ARC_CW )
				DrawArc( pDC, DL_ARC_CW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_style1 == DSS_ARC_CCW )
				DrawArc( pDC, DL_ARC_CCW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_style1 == DSS_CURVE_CW )
				DrawCurve( pDC, DL_CURVE_CW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_style1 == DSS_CURVE_CCW )
				DrawCurve( pDC, DL_CURVE_CCW, m_drag_xi, m_drag_yi, xx, yy );
			else
				ASSERT(0);
			if( m_drag_shape == DS_LINE_VERTEX )
			{
				// draw second segment
				CPen pen( PS_SOLID, m_drag_w2, drag_color );
				CPen * old_pen = pDC->SelectObject( &pen );
				if( m_drag_style2 == DSS_STRAIGHT )
					pDC->LineTo( m_drag_xf, m_drag_yf );
				else if( m_drag_style2 == DSS_ARC_CW )
					DrawArc( pDC, DL_ARC_CW, xx, yy, m_drag_xf, m_drag_yf );
				else if( m_drag_style2 == DSS_ARC_CCW )
					DrawArc( pDC, DL_ARC_CCW, xx, yy, m_drag_xf, m_drag_yf );
				else if( m_drag_style2 == DSS_CURVE_CW )
					DrawArc( pDC, DL_ARC_CW, xx, yy, m_drag_xf, m_drag_yf );
				else if( m_drag_style2 == DSS_CURVE_CCW )
					DrawArc( pDC, DL_ARC_CCW, xx, yy, m_drag_xf, m_drag_yf );
				else
					ASSERT(0);
				pDC->SelectObject( old_pen );
			}
			pDC->SelectObject( old_pen );
		}
		// move a trace segment
		else if( m_drag_shape == DS_SEGMENT )
		{
			ASSERT(m_drag_style0 == DSS_STRAIGHT);
			if( m_drag_start )
			{
				pDC->MoveTo( m_drag_xb, m_drag_yb );

				// undraw first segment
				CPen pen0( PS_SOLID, m_drag_w0, drag_color );
				CPen * old_pen = pDC->SelectObject( &pen0 );
				pDC->LineTo( m_drag_xi, m_drag_yi );

				// undraw second segment
				ASSERT(m_drag_style1 == DSS_STRAIGHT);
				CPen pen1( PS_SOLID, m_drag_w1, drag_color );
				pDC->SelectObject( &pen1 );
				pDC->LineTo( m_drag_xf, m_drag_yf );

				// undraw third segment
				if(m_drag_style2 == DSS_STRAIGHT)		// Could also be DSS_NONE (this segment only)
				{
					CPen pen2( PS_SOLID, m_drag_w2, drag_color );
					pDC->SelectObject( &pen2 );
					pDC->LineTo( m_drag_xe, m_drag_ye );
				}
				pDC->SelectObject( old_pen );
			}	

			// Adjust the two vertices, (xi, yi) and (xf, yf) based on the movement of xx and yy
			//  relative to m_drag_x and m_drag_y:

			// 1. Move the endpoints of (xi, yi), (xf, yf) of the line by the mouse movement. This
			//		is just temporary, since the final ending position is determined by the intercept
			//		points with the leading and trailing segments:
			int new_xi = m_drag_xi + xx - m_drag_x;			// Check sign here.
			int new_yi = m_drag_yi + yy - m_drag_y;

			int new_xf = m_drag_xf + xx - m_drag_x;
			int new_yf = m_drag_yf + yy - m_drag_y;

			int old_xb_dir = sign(m_drag_xi - m_drag_xb);
			int old_yb_dir = sign(m_drag_yi - m_drag_yb);

			int old_xi_dir = sign(m_drag_xf - m_drag_xi);
			int old_yi_dir = sign(m_drag_yf - m_drag_yi);

			int old_xe_dir = sign(m_drag_xe - m_drag_xf);
			int old_ye_dir = sign(m_drag_ye - m_drag_yf);

			// 2. Find the intercept between the extended segment in motion and the leading segment.
			double d_new_xi;
			double d_new_yi;
			FindLineIntersection(m_drag_xb, m_drag_yb, m_drag_xi, m_drag_yi,
									new_xi,    new_yi,	   new_xf,    new_yf,
									&d_new_xi, &d_new_yi);
			int i_drag_xi = floor(d_new_xi + .5);
			int i_drag_yi = floor(d_new_yi + .5);

			// 3. Find the intercept between the extended segment in motion and the trailing segment:
			int i_drag_xf, i_drag_yf;
			if(m_drag_style2 == DSS_STRAIGHT)
			{
				double d_new_xf;
				double d_new_yf;
				FindLineIntersection(new_xi,    new_yi,	   new_xf,    new_yf,
										m_drag_xf, m_drag_yf, m_drag_xe, m_drag_ye,
										&d_new_xf, &d_new_yf);

				i_drag_xf = floor(d_new_xf + .5);
				i_drag_yf = floor(d_new_yf + .5);
			} 
			else 
			{
				i_drag_xf = new_xf;
				i_drag_yf = new_yf;
			}
			
			// If we drag too far, the line segment can reverse itself causing a little triangle to form.
			//   That's a bad thing.
			if(sign(i_drag_xf - i_drag_xi) == old_xi_dir && sign(i_drag_yf - i_drag_yi) == old_yi_dir &&
			   sign(i_drag_xi - m_drag_xb) == old_xb_dir && sign(i_drag_yi - m_drag_yb) == old_yb_dir &&
			   sign(m_drag_xe - i_drag_xf) == old_xe_dir && sign(m_drag_ye - i_drag_yf) == old_ye_dir   )
			{
				m_drag_xi = i_drag_xi;
				m_drag_yi = i_drag_yi;
				m_drag_xf = i_drag_xf;
				m_drag_yf = i_drag_yf;
			}
			else
			{
				xx = m_drag_x;
				yy = m_drag_y;
			}

			// 4. Redraw the three segments:
			{
				pDC->MoveTo( m_drag_xb, m_drag_yb );

				// draw first segment
				CPen pen0( PS_SOLID, m_drag_w0, drag_color );
				CPen * old_pen = pDC->SelectObject( &pen0 );
				pDC->LineTo( m_drag_xi, m_drag_yi );

				// draw second segment
				CPen pen1( PS_SOLID, m_drag_w1, drag_color );
				pDC->SelectObject( &pen1 );
				pDC->LineTo( m_drag_xf, m_drag_yf );

				if(m_drag_style2 == DSS_STRAIGHT)
				{
					// draw third segment
					CPen pen2( PS_SOLID, m_drag_w2, drag_color );
					pDC->SelectObject( &pen2 );
					pDC->LineTo( m_drag_xe, m_drag_ye );
				}
				pDC->SelectObject( old_pen );
			}
		}
		else if( (m_drag_shape == DS_ARC_STRAIGHT 
		|| m_drag_shape == DS_ARC_CW || m_drag_shape == DS_ARC_CCW
		|| m_drag_shape == DS_CURVE_CW || m_drag_shape == DS_CURVE_CCW) )
		{
			CPen pen_w( PS_SOLID, m_drag_w1, drag_color );
	
			// undraw old arc
			CPen * old_pen = pDC->SelectObject( &pen_w );
			if( m_drag_start )
			{
				if( m_last_drag_shape == DS_ARC_STRAIGHT )
					DrawArc( pDC, DL_LINE, m_drag_x, m_drag_y, m_drag_xi, m_drag_yi );
				else if( m_last_drag_shape == DS_ARC_CW )
					DrawArc( pDC, DL_ARC_CW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_last_drag_shape == DS_ARC_CCW )
					DrawArc( pDC, DL_ARC_CCW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_last_drag_shape == DS_CURVE_CW )
					DrawCurve( pDC, DL_CURVE_CW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
				else if( m_last_drag_shape == DS_CURVE_CCW )
					DrawCurve( pDC, DL_CURVE_CCW, m_drag_xi, m_drag_yi, m_drag_x, m_drag_y );
			}
			// draw new arc
			if( m_drag_shape == DS_ARC_STRAIGHT )
				DrawArc( pDC, DL_LINE, xx, yy, m_drag_xi, m_drag_yi );
			else if( m_drag_shape == DS_ARC_CW )
				DrawArc( pDC, DL_ARC_CW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_shape == DS_ARC_CCW )
				DrawArc( pDC, DL_ARC_CCW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_shape == DS_CURVE_CW )
				DrawCurve( pDC, DL_CURVE_CW, m_drag_xi, m_drag_yi, xx, yy );
			else if( m_drag_shape == DS_CURVE_CCW )
				DrawCurve( pDC, DL_CURVE_CCW, m_drag_xi, m_drag_yi, xx, yy );
			m_last_drag_shape = m_drag_shape;	// used only for polylines
			//
			pDC->SelectObject( old_pen );
		}
	}
	
	// now undraw and redraw cross-hairs, if necessary
	if( m_cross_hairs )
	{
		int x = xx;
		int y = yy;
		CPen _pen( PS_DASH, 0, drag_color );
		CPen * old_pen = pDC->SelectObject( &_pen );
		if( m_drag_start )
		{
			pDC->MoveTo( m_cross_left );
			pDC->LineTo( m_cross_right );
			pDC->MoveTo( m_cross_bottom );
			pDC->LineTo( m_cross_top );
			if( m_cross_hairs == 2 )
			{
				pDC->MoveTo( m_cross_topleft );
				pDC->LineTo( m_cross_botright );
				pDC->MoveTo( m_cross_botleft );
				pDC->LineTo( m_cross_topright );
			}
		}
		m_cross_left.x = m_org_x;
		m_cross_left.y = y;
		m_cross_right.x = m_max_x;
		m_cross_right.y = y;
		m_cross_bottom.x = x;
		m_cross_bottom.y = m_org_y;
		m_cross_top.x = x;
		m_cross_top.y = m_max_y;
		if( x-m_org_x > y-m_org_y )
		{
			// bottom-left cursor line intersects m_org_y
			m_cross_botleft.x = x - (y - m_org_y);
			m_cross_botleft.y = m_org_y;
		}
		else
		{
			// bottom-left cursor line intersects m_org_x
			m_cross_botleft.x = m_org_x;
			m_cross_botleft.y = y - (x - m_org_x);
		}
		if( m_max_x-x > y-m_org_y )
		{
			// bottom-right cursor line intersects m_org_y
			m_cross_botright.x = x + (y - m_org_y);
			m_cross_botright.y = m_org_y;
		}
		else
		{
			// bottom-right cursor line intersects m_max_x
			m_cross_botright.x = m_max_x;
			m_cross_botright.y = y - (m_max_x - x);
		}

		if( x-m_org_x > m_max_y-y )
		{
			// top-left cursor line intersects m_max_y
			m_cross_topleft.x = x - (m_max_y - y);
			m_cross_topleft.y = m_max_y;
		}
		else
		{
			// top-left cursor line intersects m_org_x
			m_cross_topleft.x = m_org_x;
			m_cross_topleft.y = y + (x - m_org_x);
		}
		if( m_max_x-x > m_max_y-y )
		{
			// top-right cursor line intersects m_max_y
			m_cross_topright.x = x + (m_max_y - y);
			m_cross_topright.y = m_max_y;
		}
		else
		{
			// top-right cursor line intersects m_max_x
			m_cross_topright.x = m_max_x;
			m_cross_topright.y = y + (m_max_x - x);
		}
		pDC->MoveTo( m_cross_left );
		pDC->LineTo( m_cross_right );
		pDC->MoveTo( m_cross_bottom );
		pDC->LineTo( m_cross_top );
		if( m_cross_hairs == 2 )
		{
			pDC->MoveTo( m_cross_topleft );
			pDC->LineTo( m_cross_botright );
			pDC->MoveTo( m_cross_botleft );
			pDC->LineTo( m_cross_topright );
		}
		//pDC->SelectObject( old_pen );
		COLORREF bk = pDC->SetBkColor( RGB(0,0,0) );
		pDC->SetBkColor( bk );
	}

	// remember shape data
	m_drag_x = xx;
	m_drag_y = yy;
	m_drag_start = 1;

	// restore drawing mode
	pDC->SetROP2( old_ROP2 );

	return;
}

// Stop dragging 
//
int CDisplayList::StopDragging()
{
	m_drag_flag = 0;
	m_drag_num_lines = 0;
	m_drag_num_ratlines = 0;
	m_cross_hairs = 0;
	m_last_drag_shape = DS_NONE;
	return 0;
}

// change angle by adding 90 degrees clockwise
//
void CDisplayList::IncrementDragAngle( CDC * pDC )
{
	m_drag_angle = (m_drag_angle + 90) % 360;

	CPoint zero(0,0);

	CPen drag_pen( PS_SOLID, 1, RGB( m_rgb[m_drag_layer][0], 
					m_rgb[m_drag_layer][1], m_rgb[m_drag_layer][2] ) );
	CPen *old_pen = pDC->SelectObject( &drag_pen );

	int old_ROP2 = pDC->GetROP2();
	pDC->SetROP2( R2_XORPEN );
	
	// erase lines
	for( int il=0; il<m_drag_num_lines; il++ )
	{
		pDC->MoveTo( m_drag_x+m_drag_line_pt[2*il].x, m_drag_y+m_drag_line_pt[2*il].y );
		pDC->LineTo( m_drag_x+m_drag_line_pt[2*il+1].x, m_drag_y+m_drag_line_pt[2*il+1].y );
	}
	for( int il=0; il<m_drag_num_ratlines; il++ )
	{
		pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
		pDC->LineTo( m_drag_x+m_drag_ratline_end_pt[il].x, m_drag_y+m_drag_ratline_end_pt[il].y );
	}
		
	// rotate points, redraw lines
	for( int il=0; il<m_drag_num_lines; il++ )
	{
		RotatePoint( &m_drag_line_pt[2*il], -90, zero );
		RotatePoint( &m_drag_line_pt[2*il+1], -90, zero );
		pDC->MoveTo( m_drag_x+m_drag_line_pt[2*il].x, m_drag_y+m_drag_line_pt[2*il].y );
		pDC->LineTo( m_drag_x+m_drag_line_pt[2*il+1].x, m_drag_y+m_drag_line_pt[2*il+1].y );
	}
	for( int il=0; il<m_drag_num_ratlines; il++ )
	{
		RotatePoint( &m_drag_ratline_end_pt[il], -90, zero );
		pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
		pDC->LineTo( m_drag_x+m_drag_ratline_end_pt[il].x, m_drag_y+m_drag_ratline_end_pt[il].y );
	}

	pDC->SelectObject( old_pen );
	pDC->SetROP2( old_ROP2 );
}

// flip to opposite side of board
//
void CDisplayList::FlipDragSide( CDC * pDC )
{
	m_drag_side = 1 - m_drag_side;

	CPen drag_pen( PS_SOLID, 1, RGB( m_rgb[m_drag_layer][0], 
					m_rgb[m_drag_layer][1], m_rgb[m_drag_layer][2] ) );
	CPen *old_pen = pDC->SelectObject( &drag_pen );

	int old_ROP2 = pDC->GetROP2();
	pDC->SetROP2( R2_XORPEN );

	// erase lines
	for( int il=0; il<m_drag_num_lines; il++ )
	{
		pDC->MoveTo( m_drag_x+m_drag_line_pt[2*il].x, m_drag_y+m_drag_line_pt[2*il].y );
		pDC->LineTo( m_drag_x+m_drag_line_pt[2*il+1].x, m_drag_y+m_drag_line_pt[2*il+1].y );
	}
	for( int il=0; il<m_drag_num_ratlines; il++ )
	{
		pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
		pDC->LineTo( m_drag_x+m_drag_ratline_end_pt[il].x, m_drag_y+m_drag_ratline_end_pt[il].y );
	}

	// modify drag lines
	for( int il=0; il<m_drag_num_lines; il++ )
	{
		m_drag_line_pt[2*il].x = -m_drag_line_pt[2*il].x;
		m_drag_line_pt[2*il+1].x = -m_drag_line_pt[2*il+1].x;
	}
	for( int il=0; il<m_drag_num_ratlines; il++ )
	{
		m_drag_ratline_end_pt[il].x = -m_drag_ratline_end_pt[il].x;
	}

	// redraw lines
	for( int il=0; il<m_drag_num_lines; il++ )
	{
		pDC->MoveTo( m_drag_x+m_drag_line_pt[2*il].x, m_drag_y+m_drag_line_pt[2*il].y );
		pDC->LineTo( m_drag_x+m_drag_line_pt[2*il+1].x, m_drag_y+m_drag_line_pt[2*il+1].y );
	}
	for( int il=0; il<m_drag_num_ratlines; il++ )
	{
		pDC->MoveTo( m_drag_ratline_start_pt[il].x, m_drag_ratline_start_pt[il].y );
		pDC->LineTo( m_drag_x+m_drag_ratline_end_pt[il].x, m_drag_y+m_drag_ratline_end_pt[il].y );
	}

	pDC->SelectObject( old_pen );
	pDC->SetROP2( old_ROP2 );
}

// get any changes in side which occurred while dragging
//
int CDisplayList::GetDragSide()
{
	return m_drag_side;
}

// get any changes in angle which occurred while dragging
//
int CDisplayList::GetDragAngle()
{
	return m_drag_angle;
}

int CDisplayList::MakeDragLineArray( int num_lines )
{
	if( m_drag_line_pt )
		free( m_drag_line_pt );
	m_drag_line_pt = (CPoint*)calloc( 2*num_lines, sizeof(CPoint) );
	if( !m_drag_line_pt )
		return 1;

	m_drag_max_lines = num_lines;
	m_drag_num_lines = 0;
	return 0;
}

int CDisplayList::MakeDragRatlineArray( int num_ratlines, int width )
{
	if( m_drag_ratline_start_pt )
		free(m_drag_ratline_start_pt );
	m_drag_ratline_start_pt = (CPoint*)calloc( num_ratlines, sizeof(CPoint) );
	if( !m_drag_ratline_start_pt )
		return 1;

	if( m_drag_ratline_end_pt )
		free(m_drag_ratline_end_pt );
	m_drag_ratline_end_pt = (CPoint*)calloc( num_ratlines, sizeof(CPoint) );
	if( !m_drag_ratline_end_pt )
		return 1;

	m_drag_ratline_width = width;
	m_drag_max_ratlines = num_ratlines;
	m_drag_num_ratlines = 0;
	return 0;
}

int CDisplayList::AddDragLine( CPoint pi, CPoint pf )
{
	if( m_drag_num_lines >= m_drag_max_lines )
		return  1;

	m_drag_line_pt[2*m_drag_num_lines].x = pi.x/m_pcbu_per_wu;
	m_drag_line_pt[2*m_drag_num_lines].y = pi.y/m_pcbu_per_wu;
	m_drag_line_pt[2*m_drag_num_lines+1].x = pf.x/m_pcbu_per_wu;
	m_drag_line_pt[2*m_drag_num_lines+1].y = pf.y/m_pcbu_per_wu;
	m_drag_num_lines++;
	return 0;
}

int CDisplayList::AddDragRatline( CPoint pi, CPoint pf )
{
	if( m_drag_num_ratlines == m_drag_max_ratlines )
		return  1;

	m_drag_ratline_start_pt[m_drag_num_ratlines].x = pi.x/m_pcbu_per_wu;
	m_drag_ratline_start_pt[m_drag_num_ratlines].y = pi.y/m_pcbu_per_wu;
	m_drag_ratline_end_pt[m_drag_num_ratlines].x = pf.x/m_pcbu_per_wu;
	m_drag_ratline_end_pt[m_drag_num_ratlines].y = pf.y/m_pcbu_per_wu;
	m_drag_num_ratlines++;
	return 0;
}

int CDisplayList::CancelHighLight( BOOL IncludeStatic )
{
	// 1
	HLTClear();
	// 2
	dl_element * el = m_end[LAY_HILITE].prev;
	while ( el != &m_start[LAY_HILITE] )
	{
		el = el->prev;
		if ( el->next->el_static == 0 || IncludeStatic )
			Remove(el->next);
	}	
	return 0;
}

// Set the device context and memory context to world coords
//
void CDisplayList::SetDCToWorldCoords( CDC * pDC, CDC * bDC, CDC * tDC, int pcbu_org_x, int pcbu_org_y )
{
	botDC = NULL;
	topDC = NULL;
	if( pDC )
	{
		// set window scale (WU per pixel) and origin (WU)
		pDC->SetMapMode( MM_ANISOTROPIC );
		pDC->SetWindowExt( w_ext_x, w_ext_y );
		pDC->SetWindowOrg( pcbu_org_x/m_pcbu_per_wu, pcbu_org_y/m_pcbu_per_wu );

		// set viewport to client rect with origin in lower left
		// leave room for m_left_pane to the left of the PCB drawing area
		pDC->SetViewportExt( v_ext_x, v_ext_y );
		pDC->SetViewportOrg( m_pane_org_x, m_pane_org_y );
	}
	if( bDC->m_hDC )
	{
		// set window scale (units per pixel) and origin (units)
		bDC->SetMapMode( MM_ANISOTROPIC );
		bDC->SetWindowExt( w_ext_x, w_ext_y );
		bDC->SetWindowOrg( pcbu_org_x/m_pcbu_per_wu, pcbu_org_y/m_pcbu_per_wu );

		// set viewport to client rect with origin in lower left
		// leave room for m_left_pane to the left of the PCB drawing area
		bDC->SetViewportExt( v_ext_x, v_ext_y );
		bDC->SetViewportOrg( m_pane_org_x, m_pane_org_y );
		
		// update pointer
		botDC = bDC;
	}
	if( tDC )
		if( tDC->m_hDC )
		{
			// set window scale (units per pixel) and origin (units)
			tDC->SetMapMode( MM_ANISOTROPIC );
			tDC->SetWindowExt( w_ext_x, w_ext_y );
			tDC->SetWindowOrg( pcbu_org_x/m_pcbu_per_wu, pcbu_org_y/m_pcbu_per_wu );
	
			// set viewport to client rect with origin in lower left
			// leave room for m_left_pane to the left of the PCB drawing area
			tDC->SetViewportExt( v_ext_x, v_ext_y );
			tDC->SetViewportOrg( m_pane_org_x, m_pane_org_y );
			
			// update pointer
			topDC = tDC;
		}
}


void CDisplayList::SetVisibleGrid( BOOL on, double grid )
{
	m_visual_grid_on = on;
	m_visual_grid_spacing = grid/m_pcbu_per_wu;
}


void CDisplayList::SetStatic( int m )
{
	dl_element * el = m_end[LAY_HILITE].prev;
	while ( el != &m_start[LAY_HILITE] )
	{
		el = el->prev;
		el->next->el_static = m;
	}
	if( m )
	{
		el = &m_start[LAY_HILITE];
		while ( el->next_HLT != &m_end[LAY_HILITE] )
		{
			el = el->next_HLT;
			dl_element * new_el = CopyToLayer( el, LAY_HILITE );
			new_el->el_static = m;
		}
	}
}

void CDisplayList::SetUpCrosshairs( int type, int x, int y )
{
	// set up cross hairs
	m_cross_hairs = type;
	m_cross_left.x = m_org_x;
	m_cross_left.y = y;
	m_cross_right.x = m_max_x;
	m_cross_right.y = y;
	m_cross_bottom.x = x;
	m_cross_bottom.y = m_org_y;
	m_cross_top.x = x;
	m_cross_top.y = m_max_y;
	if( x-m_org_x > y-m_org_y )
	{
		// bottom-left cursor line intersects m_org_y
		m_cross_botleft.x = x - (y - m_org_y);
		m_cross_botleft.y = m_org_y;
	}
	else
	{
		// bottom-left cursor line intersects m_org_x
		m_cross_botleft.x = m_org_x;
		m_cross_botleft.y = y - (x - m_org_x);
	}
	if( m_max_x-x > y-m_org_y )
	{
		// bottom-right cursor line intersects m_org_y
		m_cross_botright.x = x + (y - m_org_y);
		m_cross_botright.y = m_org_y;
	}
	else
	{
		// bottom-right cursor line intersects m_max_x
		m_cross_botright.x = m_max_x;
		m_cross_botright.y = y - (m_max_x - x);
	}

	if( x-m_org_x > m_max_y-y )
	{
		// top-left cursor line intersects m_max_y
		m_cross_topleft.x = x - (m_max_y - y);
		m_cross_topleft.y = m_max_y;
	}
	else
	{
		// top-left cursor line intersects m_org_x
		m_cross_topleft.x = m_org_x;
		m_cross_topleft.y = y - (x - m_org_x);
	}
	if( m_max_x-x > m_max_y-y )
	{
		// top-right cursor line intersects m_max_y
		m_cross_topright.x = x + (m_max_y - y);
		m_cross_topright.y = m_max_y;
	}
	else
	{
		// top-right cursor line intersects m_max_x
		m_cross_topright.x = m_max_x;
		m_cross_topright.y = y + (m_max_x - x);
	}
}

// Convert point in window coords to PCB units (i.e. nanometers)
//
CPoint CDisplayList::WindowToPCB( CPoint point )
{
	CPoint p;     
	double test = ((point.x-m_pane_org_x)*m_wu_per_pixel_x + m_org_x)*m_pcbu_per_wu;
	p.x = test;    
	test = ((point.y-m_pane_org_y)*m_wu_per_pixel_y + m_org_y)*m_pcbu_per_wu;
	p.y = test;
	return p;
}

// Convert point in screen coords to PCB units
//
CPoint CDisplayList::ScreenToPCB( CPoint point )
{
	CPoint p;
	p.x = point.x - m_screen_r.left;
	p.y = point.y - m_screen_r.top;
	p = WindowToPCB( p );
	return p;
}

// Convert point in PCB units to screen coords
//
CPoint CDisplayList::PCBToScreen( CPoint point )
{
	CPoint p;
	p.x = (point.x - m_org_x*m_pcbu_per_wu)/m_pcbu_per_pixel_x+m_pane_org_x+m_screen_r.left;
	p.y = (point.y - m_org_y*m_pcbu_per_wu)/m_pcbu_per_pixel_y-m_bottom_pane_h+m_screen_r.bottom; 
	return p;
}


RECT CDisplayList::GetClientRect( BOOL convert )
{
	if( convert == 0 )
		return m_client_r;
	else
		return m_client_r.MulDiv( m_pcbu_per_wu, 1 );
}
// 
//
RECT CDisplayList::GetWindowRect()
{
	RECT wr;
	wr.left =	m_org_x;
	wr.right =	m_max_x; 
	wr.top =	m_max_y;
	wr.bottom = m_org_y; 
	return wr;
}

void CDisplayList::ShowImages( int vis )
{
	m_show_pictures = vis;
}

void CDisplayList::Marker( int m, int i )
{
	if( i >= 0 )
		m_tmp.GetAt(i)->m_marker = m;
	else for(int itmp=0; itmp<m_tmp.GetSize(); itmp++)
		m_tmp.GetAt(itmp)->m_marker = m;
}

int CDisplayList::FindSource( CString * bmp )
{
	for( int i=0; i<m_src.GetSize(); i++ )
	{
		if( m_src.GetAt(i).CompareNoCase(*bmp) == 0 )
			return i;
	}
	return -1;
}

int CDisplayList::AddSource( CString * bmp )
{
	int isrc = -1;
	for( int i=0; i<m_src.GetSize(); i++ )
	{
		if( m_src.GetAt(i).CompareNoCase(*bmp) == 0 )
		{
			isrc = i;
			break;
		}
		if( m_tmp.GetSize() )
		{
			BOOL found = 0;
			for( int ii=0; ii<m_tmp.GetSize(); ii++ )
			{
				if( m_tmp.GetAt(ii)->m_source == i )
				{
					found = 1;// image already used
					break;
				}
			}
			if( found == 0 ) // image not used
			{
				isrc = i;
				break;
			}
		}
	}
	if( isrc == -1 )
	{
		m_src.Add(*bmp);
		isrc = m_src.GetSize()-1;
	}
	else
	{
		m_src[isrc] = *bmp;
	}
	return isrc;
}

int CDisplayList::AddTemplate( int index_src, int layer, RECT * r, BOOL bNU )
{
	int i = -1;
	if( bNU )// find not used
		for( int ii=0; ii<m_tmp.GetSize(); ii++ )
		{
			if( m_tmp.GetAt(ii)->m_rect.left == m_tmp.GetAt(ii)->m_rect.right )// (right - left == 0)
			{
				i = ii;
				break;
			}
		}
	if( i == -1 )
	{
		i = m_tmp.GetSize();
		CTemplate * tmp = new CTemplate;
		m_tmp.SetSize(i+1);
		m_tmp[i] = tmp;
	}
	m_tmp.GetAt(i)->m_rect = rect(0,0,0,0);
	m_tmp.GetAt(i)->m_style = 0;
	m_tmp.GetAt(i)->m_vis = 0;
	m_tmp.GetAt(i)->lp = NULL;
	m_tmp.GetAt(i)->m_format = 0;
	m_tmp.GetAt(i)->m_source = index_src;
	if( index_src >= 0 && index_src < m_src.GetSize() )
		SetTemplate( &m_src.GetAt(index_src), r, i, layer );
	return i;
}

void CDisplayList::SetTemplate( CString * bmp, RECT * r, int i, int layer )
{
	if( i < 0 || i >= m_tmp.GetSize() )
		return;
	if( bmp )
		m_tmp.GetAt(i)->m_source = AddSource(bmp);
	int isrc = m_tmp.GetAt(i)->m_source;
	if( isrc == -1 )
		return;
	struct _stat buf;
	int err = _stat( m_src.GetAt( isrc ), &buf );//ok
	if( err == 0 )
	{	
		// first i try to load bmp
		HGDIOBJ Obj = LoadImage( 0, m_src.GetAt( isrc ), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_MONOCHROME );	
		if(Obj)
		{
			m_tmp.GetAt(i)->m_format = BMP;
			m_tmp.GetAt(i)->bm.DeleteObject();
			m_tmp.GetAt(i)->bm.Attach( Obj );	
			if( r )
				m_tmp.GetAt(i)->m_vis = 1;
			MoveTemplate( r, i, layer );
		}
		else
		{	// jpeg
			CFile file;
			int err = file.Open( m_src.GetAt( isrc ), CFile::modeRead, NULL );
			if( err )
			{
				m_tmp.GetAt(i)->m_format = JPEG;
				if( m_tmp.GetAt(i)->lp )
					m_tmp.GetAt(i)->lp->Release();
				CArchive ar( &file, CArchive::load | CArchive::bNoFlushOnDelete );
				CArchiveStream ars(&ar);
				OleLoadPicture( (LPSTREAM)&ars, 0, FALSE, IID_IPicture, (void**)&m_tmp.GetAt(i)->lp );
				if( m_tmp.GetAt(i)->lp )
				{
					if( r )
						m_tmp.GetAt(i)->m_vis = 1;
					MoveTemplate( r, i, layer );
				}
				else
				{
					MoveTemplate( &rect(0,0,0,0), i, layer );
					m_tmp.GetAt(i)->m_vis = 0;
				}
				ar.Close();
				file.Close();
			}
		}		
	}
}

CString CDisplayList::GetSource( int i )
{
	if( i >= 0 && i < m_src.GetSize() )
		return m_src.GetAt(i);
	return "";
}

int CDisplayList::GetSourceIndex( int tmp_index )
{
	if( tmp_index >= 0 && tmp_index < m_tmp.GetSize() )
		return m_tmp.GetAt(tmp_index)->m_source;
	return -1;
}

int CDisplayList::GetTemplate(  int i, 
								CString * bmp, 
								RECT * r, 
								int * style, 
								int * vis, 
								int * format,
								int * marker )
{
	if( i < 0 )
		return 0;
	if( bmp )
		if( m_tmp.GetAt(i)->m_source >= 0 )
			*bmp = m_src.GetAt( m_tmp.GetAt(i)->m_source );
	if( r )
	{
		r->left =	m_tmp.GetAt(i)->m_rect.left		;
		r->right =	m_tmp.GetAt(i)->m_rect.right	;
		r->top =	m_tmp.GetAt(i)->m_rect.top		;
		r->bottom = m_tmp.GetAt(i)->m_rect.bottom	;
	}
	if( style )
		*style = m_tmp.GetAt(i)->m_style;
	if( vis )
		*vis = m_tmp.GetAt(i)->m_vis;
	if( format )
		*format = m_tmp.GetAt(i)->m_format; 
	if( marker )
		*marker = m_tmp.GetAt(i)->m_marker;
	return m_tmp.GetAt(i)->m_layer;
}

void CDisplayList::MoveTemplate( RECT * r, int i, int layer )
{
	if( r )
	{
		m_tmp.GetAt(i)->m_rect.left		= r->left	/m_pcbu_per_wu;
		m_tmp.GetAt(i)->m_rect.right	= r->right	/m_pcbu_per_wu;
		m_tmp.GetAt(i)->m_rect.top		= r->top	/m_pcbu_per_wu;
		m_tmp.GetAt(i)->m_rect.bottom	= r->bottom /m_pcbu_per_wu;
	}
	m_tmp.GetAt(i)->m_layer = layer;
}

void CDisplayList::SetTemplateStyle( int style, int i )
{
	m_tmp.GetAt(i)->m_style = style;
}

void CDisplayList::SetTemplateVis( int vis, int i )
{
	m_tmp.GetAt(i)->m_vis = vis;
}

int CDisplayList::GetNumTemplates()
{
	return m_tmp.GetSize();
}

int CDisplayList::GetNumSources()
{
	return m_src.GetSize();
}

CSize CDisplayList::GetTemplateSZ( int i )
{
	CSize sz(0,0);
	if( m_tmp[i]->m_format == BMP )
	{
		BITMAP bmap;
		m_tmp[i]->bm.GetBitmap(&bmap);
		sz.cx = bmap.bmWidth;
		sz.cy = bmap.bmHeight;
	}
	else if( m_tmp[i]->lp )
	{
		m_tmp[i]->lp->get_Width( &sz.cx );
		m_tmp[i]->lp->get_Height( &sz.cy );
	}
	return sz;
}
