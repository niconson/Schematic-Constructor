// PartList.cpp : implementation of class CPartList
//
// this is a linked-list of parts on a PCB board
//
#include "stdafx.h"
#include <math.h>
#include "DisplayList.h"
#include "DlgMyMessageBox.h"
#include "mainfrm.h"

#define PL_MAX_SIZE		10000		// default max. size 

// globals
BOOL g_bShow_header_28mil_hole_warning = TRUE;	
BOOL g_bShow_SIP_28mil_hole_warning = TRUE;	

//******** constructors and destructors *********
 
part_pin::part_pin()
{
	x = y = 0;
	utility = 0;
	selected = 0;
	net = NULL;
	dl_sel = NULL;
	dl_hole = NULL;
}

part_pin::~part_pin()
{
	dl_els.RemoveAll();
}

cpart::cpart()
{
	// zero out pointers
	dl_sel = 0;
	dl_ref_sel = 0;
	shape = 0;
	drawn = FALSE;
}

cpart::~cpart()
{
}


CPartList::CPartList( CDisplayList * dlist, SMFontUtil * fontutil ) 
{
	m_start.prev = 0;		// dummy first element in list
	m_start.next = &m_end;
	m_end.next = 0;			// dummy last element in list
	m_end.prev = &m_start;
	m_max_size = PL_MAX_SIZE-1;	// size limit
	m_size = 0;					// current size
	m_dlist = dlist;
	m_fontutil = fontutil;
	m_footprint_cache_map = NULL;
	m_swell_pad_for_solder_mask = 0;
	m_swell_pad_for_paste_mask = 0;
	m_layers = 2;
	m_annular_ring = 0;
	begin_dragging_x = 0;
	begin_dragging_y = 0;
	begin_dragging_ang = 0;
	begin_dragging_side = 0;
	m_part_line_visible = 1;
}

CPartList::~CPartList()
{
	// traverse list, removing all parts
	while( m_end.prev != &m_start )
		Remove( m_end.prev );
}

// Create new empty part and add to end of list
// return pointer to element created.
//
cpart * CPartList::Add()
{
	if(m_size >= (m_max_size-1) )
	{
		AfxMessageBox( "Maximum number of parts exceeded" );
		return 0;
	}

	// create new instance and link into list
	cpart * part = new cpart;//ok
	part->prev = m_end.prev;
	part->next = &m_end;
	part->prev->next = part;
	part->next->prev = part;
	part->m_merge = -1;
	part->sub_merge = -1;
	part->selected = 0;
	return part;
}

// Create new part, add to end of list, set part data 
// return pointer to element created.
//
cpart * CPartList::Add( CShape * shape, CString * ref_des, 
							int x, int y, int side, int angle, int visible, int glued )
{
	if(m_size >= m_max_size )
	{
		AfxMessageBox( "Maximum number of parts exceeded" );
		return 0;
	}

	// create new instance and link into list
	cpart * part = Add();
	// set data
	SetPartData( part, shape, ref_des, x, y, side, angle, visible, glued, -1);
	return part;
}

// Set part data, draw part if m_dlist != NULL
//
int CPartList::SetPartData( cpart * part, CShape * shape, CString * ref_des, 
							int x, int y, int side, int angle, int visible, int glued, int merge )
{
	UndrawPart( part );
	CDisplayList * old_dlist = m_dlist;
	m_dlist = NULL;		// cancel further drawing
	// now copy data into part
	id id( ID_PART );
	part->visible = visible;
	part->ref_des = *ref_des;
	part->m_id = id;
	part->x = x;
	part->y = y;
	part->side = side;
	part->angle = angle;
	part->glued = glued;
	part->m_merge = merge;
	if( !shape )
	{
		part->shape = NULL;
		part->pin.SetSize(0);
		part->m_ref_xi = 0;
		part->m_ref_yi = 0;
		part->m_ref_angle = 0;
		part->m_ref_size = 0;
		part->m_ref_w = 0;
		part->m_value_xi = 0;
		part->m_value_yi = 0;
		part->m_value_angle = 0;
		part->m_value_size = 0;
		part->m_value_w = 0;		
	}
	else
	{
		part->shape = shape;
		part->pin.SetSize( shape->m_padstack.GetSize() );
		Move( part, x, y, angle, side );	// force setting pin positions
		part->m_ref_xi = shape->m_ref_xi;
		part->m_ref_yi = shape->m_ref_yi;
		part->m_ref_angle = shape->m_ref_angle;
		part->m_ref_size = shape->m_ref_size;
		part->m_ref_w = shape->m_ref_w;
		part->m_value_xi = shape->m_value_xi;
		part->m_value_yi = shape->m_value_yi;
		part->m_value_angle = shape->m_value_angle;
		part->m_value_size = shape->m_value_size;
		part->m_value_w = shape->m_value_w;
	}
	part->m_outline_stroke.SetSize(0);
	part->dl_ref_el = NULL;
	part->dl_value_el = NULL;
	m_size++;

	// now draw part into display list
	m_dlist = old_dlist;
	if( part->shape )
		DrawPart( part );

	return 0;
}

// Highlight part
//
int CPartList::HighlightPart( cpart * part, BOOL bX )
{
	// highlight it by making its selection rectangle visible
	int EL_W = 50;
	if (part->shape)
	{
		for( int i=part->m_outline_stroke.GetSize()-1; i>=0; i-- )
		{
			if( part->m_outline_stroke[i] )
				m_dlist->HighLight( part->m_outline_stroke[i] );
		}
		if( bX )
		{
			dl_element * n_el = m_dlist->Cpy( part->dl_sel );
			n_el->gtype = DL_RECT_X;
			n_el->layers_bitmap = 0;
			n_el->el_w = EL_W;
			m_dlist->HighLight( n_el );
		}
		else
		{
			m_dlist->HighLight( part->dl_sel );
			part->dl_sel->el_w = EL_W;
			part->dl_sel->transparent = LAY_BACKGND;
		}
	}
	return 0;
}

void CPartList::SetLinesVis( cpart * p, int vis )
{
	if( p->shape )
	{
		for( int lns=p->shape->m_outline_poly.GetSize()-1; lns>=0; lns-- )
		{
			if( p->shape->m_outline_poly[lns].GetLayer() == LAY_FP_SILK_TOP ||
				p->shape->m_outline_poly[lns].GetLayer() == LAY_FP_SILK_BOTTOM )
				p->shape->m_outline_poly[lns].SetVisible(vis);
		}
	}
}



// Highlight part ref text
//
int CPartList::SelectRefText( cpart * part )
{
	// highlight it by making its selection rectangle visible
	if( part->dl_ref_el )
		m_dlist->HighLight( part->dl_ref_el );
	return 0;
}

// Highlight part value
//
int CPartList::SelectValueText( cpart * part )
{
	// highlight it by making its selection rectangle visible
	if( part->dl_value_el )
		m_dlist->HighLight( part->dl_value_el );
	return 0;
}

void CPartList:: HighlightAllPadsOnNet( cnet * net, int swell, int layer, int bTRANSPARENT, int excl, cpart * ex_p )
{
	for( int ip=0; ip<net->npins; ip++ )
		if( net->pin[ip].part )
			if( net->pin[ip].part->shape )
			{
				CString pname = net->pin[ip].pin_name;
				for( int ii=net->pin[ip].part->shape->GetPinIndexByName(pname,-1); ii>=0; ii=net->pin[ip].part->shape->GetPinIndexByName(pname,ii))
					if( ii>=0 && (ii != excl || net->pin[ip].part != ex_p ))
					{
						
						if(!SelectPad( net->pin[ip].part, ii, swell, layer, bTRANSPARENT ))
						{
							int ll = GetPinLayer( net->pin[ip].part, ii );
							if( ll == LAY_NET_NAME )
								ll = layer;
							if(!SelectPad( net->pin[ip].part, ii, swell, ll, bTRANSPARENT ))
								SelectPad( net->pin[ip].part, ii, swell, 0, bTRANSPARENT );
						}
					}
			}
}

// Select all pads
int CPartList::SelectPads( cpart * part, int drc, int layer, int bTRANSPARENT )
{
	// select it by making its selection rectangle visible
	if ( !part->shape )
		return 0;
	for ( int np=part->shape->GetNumPins()-1; np>=0; np-- )
		SelectPad( part, np, drc, layer, bTRANSPARENT );
	return 1;
}


// Select part pad
//
dl_element * CPartList::SelectPad( cpart * part, int i, int swell, int layer, int bTRANSPARENT )
{
	// select it by making its selection rectangle visible
	if ( !part->shape )
		return NULL;
	if( i < 0 || i >= part->shape->GetNumPins() )
		return NULL;
	
	dl_element * pad_el = NULL;
	if( !layer && part->pin[i].dl_sel )
	{
		pad_el = part->pin[i].dl_sel;
	}
	int pad_layers = 0;
	for( int nl=part->pin[i].dl_els.GetSize()-1; nl>=0; nl-- )
	{
		if( part->pin[i].dl_els[nl] )
		{
			pad_layers |= part->pin[i].dl_els[nl]->layers_bitmap;
			if( getbit( part->pin[i].dl_els[nl]->layers_bitmap, layer ) )
			{
				if( !pad_el )
				{
					pad_el = part->pin[i].dl_els[nl];
					break;
				}
			}
		}
	}
	if( !pad_layers )
	{
		int gl = m_dlist->GetTopLayer();
		if( gl > 0 )
			setbit( pad_layers, gl );
		if( part->pin[i].dl_hole )
		{
			if( !pad_el )
			{
				pad_el = part->pin[i].dl_hole;
				swell = 0;
			}
		}
	}
	if( !pad_el )
		return NULL;
	//
	// Draw Highlighted Pad..
	//
	dl_element * el=NULL;
	int EL_W = 10*m_pcbu_per_wu;
	//
	//-----------> DL_RECT_X , FILL TYPE
	if( swell < 2 )
	{
		m_dlist->HighLight( pad_el );
		el = pad_el;
	}
	//
	//-----------> HOLLOW TYPE
	else 
	{	
		CArray<CPoint> * pts = m_dlist->Get_Points( pad_el, NULL, 0 );
		int np = pts->GetSize();
		CPoint * P=NULL;
		if( np )
		{		
			P = new CPoint[np];//ok
			m_dlist->Get_Points( pad_el, P, &np );
		}
		if( pad_el->gtype == DL_CIRC )
		{
			RECT rct;
			m_dlist->Get_Rect( pad_el, &rct );
			SwellRect( &rct, swell);
			el = m_dlist->Add( pad_el->id, pad_el->ptr, 0, DL_HOLLOW_CIRC, 1, &rct, EL_W, P, np );
			m_dlist->HighLight( el );
			el->map_orig_layer = pad_layers;
		}
		else if( pad_el->gtype == DL_LINE )
		{
			CArray<CPoint> * pnts = m_dlist->Get_Points( pad_el, NULL, NULL );
			if( pnts->GetSize() > 2 )
			{
				CPoint hlP[32];
				hlP[0] = (*pnts)[0];
				hlP[1] = (*pnts)[1];
				RECT * rct = m_dlist->Get_Rect( pad_el, NULL );
				int npt = Gen_HollowLinePoly( (*pnts)[0].x, (*pnts)[0].y, (*pnts)[1].x, (*pnts)[1].y, pad_el->el_w, &hlP[2], 30 );
				dl_element * el = m_dlist->Add( pad_el->id, pad_el->ptr, 0, DL_HOLLOW_LINE, 1, 
												rct, 0, hlP, npt+2, FALSE );
				m_dlist->HighLight( el );
				el->map_orig_layer = pad_layers;
			}
		}
		else if( pad_el->gtype == DL_RECT || pad_el->gtype == DL_RRECT )
		{
			RECT rct;
			m_dlist->Get_Rect( pad_el, &rct );
			SwellRect( &rct, swell);
			int w = pad_el->el_w*m_pcbu_per_wu;
			el = m_dlist->Add( pad_el->id, pad_el->ptr, 0, DL_HOLLOW_RRECT, 1, &rct, w+swell, P, np );
			m_dlist->HighLight( el );
			el->map_orig_layer = pad_layers;
		}
		else if( pad_el->gtype == DL_POLYGON || pad_el->gtype == DL_RECT_X )
		{
			RECT rct;
			m_dlist->Get_Rect( pad_el, &rct );
			if( !np )
			{
				P[0].x = rct.left;
				P[0].y = rct.bottom;
				P[1].x = rct.left;
				P[1].y = rct.top;
				P[2].x = rct.right;
				P[2].y = rct.top;
				P[3].x = rct.right;
				P[3].y = rct.bottom;
				np = 4;
			}
			int r = m_dlist->Get_el_w( pad_el )/2.0;
			int new_np = SwellPolygon( P, np, NULL, swell+r );
			if( new_np )
			{
				CPoint * Pnew = new CPoint[new_np+1];//ok						
				new_np = SwellPolygon( P, np, Pnew, swell+r );
				Pnew[new_np] = Pnew[0];
				el = m_dlist->Add( pad_el->id, pad_el->ptr, 0, DL_POLYLINE, 1, &rct, 1, Pnew, new_np+1 );
				m_dlist->HighLight( el );
				el->map_orig_layer = pad_layers;
				delete Pnew;
			}
		}
		if(P)
			delete P;
	}
	if( el )
		el->transparent = bTRANSPARENT;
	return el;	
}

// Test for hit on pad
//
int CPartList::TestHitOnPad( cpart * part, CString * pin_name, int x, int y, int layer )
{
	if( !part )
		return -1;
	if( !part->shape )
		return -1;
	for( int pin_index=part->shape->GetPinIndexByName(*pin_name,-1); 
			 pin_index>=0; 
			 pin_index=part->shape->GetPinIndexByName(*pin_name,pin_index))
	{

		CPoint pts[4];
		int np = 4;
		dl_element * el=0;
		for(int ip=part->pin[pin_index].dl_els.GetSize()-1; ip>=0; ip-- )
		{
			if( part->pin[pin_index].dl_els[ip] )
				if( getbit( part->pin[pin_index].dl_els[ip]->layers_bitmap, layer ) )
				{
					el = part->pin[pin_index].dl_els[ip];
					break;
				}
		}
		if( el )
		{
			m_dlist->Get_Points( el, pts, &np );
			if( np )
			{
				if( TestPolygon( x, y, pts, np ))
					return pin_index;
			}
			else
			{
				RECT r = rect(0,0,0,0);
				m_dlist->Get_Rect( el, &r );
				if( InRange( x, r.left, r.right ) )
					if( InRange( y, r.top, r.bottom ) )
						return pin_index;
			}
		}
	}
	return -1;
}


// Move part to new position, angle and side
// x and y are in world coords
// Does not adjust connections to pins
//
int CPartList::Move( cpart * part, int x, int y, int angle, int side, BOOL bDraw )
{
	// remove all display list elements
	if( bDraw )
		UndrawPart( part );
	// move part
	part->x = x;
	part->y = y;
	part->angle = angle % 360;
	part->side = side;
	// calculate new pin positions
	if( part->shape )
	{
		for( int ip=0; ip<part->pin.GetSize(); ip++ )
		{
			CPoint pt = GetPinPoint( part, ip, part->side, part->angle );
			part->pin[ip].x = pt.x;
			part->pin[ip].y = pt.y;
		}
	}
	// now redraw it
	if( bDraw )
		DrawPart( part );
	return PL_NOERR;
}

// Move ref text with given id to new position and angle
// x and y are in absolute world coords
// angle is relative to part angle
//
int CPartList::MoveRefText( cpart * part, int x, int y, int angle, int size, int w )
{
	// remove all display list elements
	UndrawPart( part );
	
	// get position of new text box origin relative to part
	CPoint part_org, tb_org;
	tb_org.x = x - part->x;
	tb_org.y = y - part->y;
	
	// correct for rotation of part
	if( part->angle )
		RotatePoint( &tb_org, part->angle, zero );
	
	// correct for part on bottom of board (reverse relative x-axis)
	if( part->side == 1 )
		tb_org.x = -tb_org.x;
	
	// reset ref text position
	part->m_ref_xi = tb_org.x;
	part->m_ref_yi = tb_org.y;
	part->m_ref_angle = angle % 360;
	if (part->m_ref_angle < 0)
		part->m_ref_angle += 360;
	part->m_ref_size = size;
	part->m_ref_w = w;
	
	// now redraw part
	DrawPart( part );
	return PL_NOERR;
}

// Resize ref text for part
//
void CPartList::ResizeRefText( cpart * part, int size, int width, BOOL vis, BOOL bDraw )
{
	if( part->shape )
	{
		// remove all display list elements
		if( bDraw )
			UndrawPart( part );
		// change ref text size
		part->m_ref_size = size;
		part->m_ref_w = width;	
		part->m_ref_vis = vis;
		// now redraw part
		if( bDraw )
			DrawPart( part );
	}
}

// Move value text to new position and angle
// x and y are in absolute world coords
// angle is relative to part angle
//
int CPartList::MoveValueText( cpart * part, int x, int y, int angle, int size, int w )
{
	// remove all display list elements
	UndrawPart( part );
	
	// get position of new text box origin relative to part
	CPoint part_org, tb_org;
	tb_org.x = x - part->x;
	tb_org.y = y - part->y;
	
	// correct for rotation of part
	if( part->angle )
		RotatePoint( &tb_org, part->angle, zero );
	
	// correct for part on bottom of board (reverse relative x-axis)
	if( part->side == 1 )
		tb_org.x = -tb_org.x;
	
	// reset value text position
	part->m_value_xi = tb_org.x;
	part->m_value_yi = tb_org.y;
	part->m_value_angle = angle % 360;
	if (part->m_value_angle < 0)
		part->m_value_angle += 360;
	part->m_value_size = size;
	part->m_value_w = w;
	
	// now redraw part
	DrawPart( part );
	return PL_NOERR;
}

// Resize value text for part
//
void CPartList::ResizeValueText( cpart * part, int size, int width, BOOL vis, BOOL bDraw )
{
	if( part->shape )
	{
		// remove all display list elements
		if( bDraw )
			UndrawPart( part );
		// change ref text size
		part->m_value_size = size;
		part->m_value_w = width;
		part->m_value_vis = vis;
		// now redraw part
		if( bDraw )
			DrawPart( part );
	}
}

// Set part value
//
void CPartList::SetValue( cpart * part, CString * value, 
						 int x, int y, int angle, int size, int w, BOOL vis )
{
	part->value = *value;
	part->m_value_xi = x;
	part->m_value_yi = y;
	part->m_value_angle = angle;
	part->m_value_size = size;
	part->m_value_w = w;
	part->m_value_vis = vis;
	if( part->shape && m_dlist )
	{
		UndrawPart( part );
		DrawPart( part );
	}
}

// Set part value
//
void CPartList::SetValue( cpart * part, CString * value )
{
	part->value = *value;
	if( part->shape && m_dlist )
	{
		UndrawPart( part );
		DrawPart( part );
	}
}


// Get side of part
//
int CPartList::GetSide( cpart * part )
{
	return part->side;
}

// Get angle of part
//
int CPartList::GetAngle( cpart * part )
{
	return part->angle;
}

// Get angle of ref text for part
//
int CPartList::GetRefAngle( cpart * part )
{
	return part->m_ref_angle;
}

// Get angle of value for part
//
int CPartList::GetValueAngle( cpart * part )
{
	return part->m_value_angle;
}

// get actual position of ref text
//
CPoint CPartList::GetRefPoint( cpart * part )
{
	CPoint ref_pt;

	// move origin of text box to position relative to part
	ref_pt.x = part->m_ref_xi;
	ref_pt.y = part->m_ref_yi;
	// flip if part on bottom
	if( part->side )
		ref_pt.x = -ref_pt.x;
	// rotate with part about part origin
	if( part->angle )	
		RotatePoint( &ref_pt, -part->angle, zero );
	ref_pt.x += part->x;
	ref_pt.y += part->y;
	return ref_pt;
}

// get actual position of value text
//
CPoint CPartList::GetValuePoint( cpart * part )
{
	CPoint value_pt;

	// move origin of text box to position relative to part
	value_pt.x = part->m_value_xi;
	value_pt.y = part->m_value_yi;
	// flip if part on bottom
	if( part->side )
		value_pt.x = -value_pt.x;
	// rotate with part about part origin
	if( part->angle )
		RotatePoint( &value_pt, -part->angle, zero );
	value_pt.x += part->x;
	value_pt.y += part->y;
	return value_pt;
}

// Get pin info from part
//
CPoint CPartList::GetPinPoint( cpart * part, int pin_index, int side, int angle )
{
	if( !part->shape )
		ASSERT(0);

	// get pin coords relative to part origin
	CPoint pp;
	if( pin_index == -1 )
		ASSERT(0);
	pp.x = part->shape->m_padstack[pin_index].x_rel;
	pp.y = part->shape->m_padstack[pin_index].y_rel;
	// flip if part on bottom
	if( side )
		pp.x = -pp.x;
	// rotate if necess.
	if( angle )
		RotatePoint( &pp, -angle, zero );

	// add coords of part origin
	pp.x = part->x + pp.x;
	pp.y = part->y + pp.y;
	return pp;
}

// Get centroid info from part
//
CPoint CPartList::GetCentroidPoint( cpart * part )
{
	if( part->shape == NULL )
		ASSERT(0);
	// get coords relative to part origin
	CPoint pp;
	pp.x = part->shape->m_centroid_x;
	pp.y = part->shape->m_centroid_y;
	// flip if part on bottom
	if( part->side )
		pp.x = -pp.x;
	// rotate if necess.
	if( part->angle )
		RotatePoint( &pp, -part->angle, zero );
	// add coords of part origin
	pp.x = part->x + pp.x;
	pp.y = part->y + pp.y;
	return pp;
}

// Get glue spot info from part
//
CPoint CPartList::GetGluePoint( cpart * part, int iglue )
{
	if( part->shape == NULL )
		ASSERT(0);
	if( iglue >= part->shape->m_glue.GetSize() )
		ASSERT(0);
	// get coords relative to part origin
	CPoint pp;
	pp.x = part->shape->m_glue[iglue].x_rel;
	pp.y = part->shape->m_glue[iglue].x_rel;
	// flip if part on bottom
	if( part->side )
		pp.x = -pp.x;
	// rotate if necess.
	if( part->angle )
		RotatePoint( &pp, -part->angle, zero );
	// add coords of part origin
	pp.x = part->x + pp.x;
	pp.y = part->y + pp.y;
	return pp;
}

// Get pin layer
// returns LAY_ADD_1, LAY_ADD_2 or LAY_NET_NAME
//

// Get pin layer
// returns LAY_ADD_1, LAY_ADD_2 or LAY_NET_NAME
//
int CPartList::GetPinLayer( cpart * part, int pin_index )
{
	if( part->shape->m_padstack[pin_index].hole_size )
		return LAY_NET_NAME;
	else if( part->shape->m_padstack[pin_index].top.shape != PAD_NONE && part->shape->m_padstack[pin_index].bottom.shape != PAD_NONE )
		return LAY_NET_NAME;
	else if( part->side == 0 && part->shape->m_padstack[pin_index].top.shape != PAD_NONE 
		|| part->side == 1 && part->shape->m_padstack[pin_index].bottom.shape != PAD_NONE )
		return LAY_ADD_1;
	else
		return LAY_ADD_2;
}

// Get pin net
//
cnet * CPartList::GetPinNet( cpart * part, CString * pin_name )
{
	int pin_index = part->shape->GetPinIndexByName( *pin_name, -1 );
	if( pin_index == -1 )
		return NULL;
	return part->pin[pin_index].net;
}

// Get pin net
//
cnet * CPartList::GetPinNet( cpart * part, int pin_index )
{
	return part->pin[pin_index].net;
}

// Get max pin width, for drawing thermal symbol
// enter with pin_num = pin # (1-based)


// Get bounding rect for all pins
// Currently, just uses selection rect
// returns 1 if successful
//
int CPartList::GetPartBoundingRect( cpart * part, RECT * part_r )
{
	if( !part )
		return 0;
	if( !part->shape )
		return 0;
	if( part->dl_sel )
	{
		m_dlist->Get_Rect( part->dl_sel, part_r );
		return 1;
	}  
	return 0;
}

int CPartList::GetPinsBoundingRect	( cpart * part, RECT * pins_r )
{
	if( !part )
		return 0;
	if( !part->shape )
		return 0;
	int np = part->shape->GetNumPins();
	pins_r->left = pins_r->bottom = INT_MAX;
	pins_r->right = pins_r->top = INT_MIN;
	RECT r;
	for( int ip=0; ip<np; ip++ )
	{
		m_dlist->Get_Rect( part->pin[ip].dl_sel, &r );
		SwellRect( pins_r, r );
	}  
	return 1;
}

int CPartList::GetPartThruPadsRect	( cpart * part, RECT * ThruPads )
{
	ThruPads->left = ThruPads->bottom = INT_MAX;
	ThruPads->right = ThruPads->top = INT_MIN;
	int np = 0;
	if( part->shape )
		for( int ip=part->shape->GetNumPins()-1; ip>=0; ip-- )
		{
			if( part->pin[ip].dl_hole )
			{
				RECT Get;
				m_dlist->Get_Rect( part->pin[ip].dl_hole, &Get );
				SwellRect( ThruPads, Get );
				np++;
			}
		}
	return np;
}

// GetRefBoundingRect
// return 1 if ok
//
int CPartList::GetRefBoundingRect	( cpart * part, RECT * ref_r )
{
	if( !part )
		return 0;
	if( !part->shape )
		return 0;
	if( part->dl_ref_sel )
	{
		m_dlist->Get_Rect( part->dl_ref_sel, ref_r );
		return 1;
	}
	return 0;
}

// GetValueBoundingRect
// return 1 if ok
//
int CPartList::GetValueBoundingRect( cpart * part, RECT * value_r )
{
	if( !part )
		return 0;
	if( !part->shape )
		return 0;
	if( part->dl_value_sel )
	{
		m_dlist->Get_Rect( part->dl_value_sel, value_r );
		return 1;
	}
	return 0;
}

// GetPadBounds
// return 1 if ok
//
int CPartList::GetPadBounds( cpart * part, int pad, RECT * pr )
{
	if( !part )
		return 0;
	if( !part->shape )
		return 0;
	if( part->drawn )
		*pr = part->pin[pad].bounds;
	else
	{
		pr->left =		0;
		pr->right =		10*NM_PER_MIL;
		pr->bottom =	0;
		pr->top =		10*NM_PER_MIL;
		return 0;
	}
	return 1;
}

// get bounding rectangle of parts
// return parts found
//
int CPartList::GetPartBoundaries( RECT * part_r )
{
	int parts_found = 0;
	// iterate
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		RECT gr;
		if( GetPartBoundingRect(part,&gr) )
		{
			if( parts_found == 0 )
				*part_r = gr;
			else
				SwellRect(part_r, gr);
			parts_found++;
		}
		part = part->next;
	}
	return parts_found;
}

// Get pointer to part in part_list with given ref
//
cpart * CPartList::GetPart( LPCTSTR ref_des )
{
	// find element with given ref_des, return pointer to element
	cpart * part = m_start.next;
	if( part ) 
		while( part->next != 0 )
		{
			if(  part->ref_des.Compare( ref_des ) == 0 )
				return part;
			part = part->next;
		}
	return NULL;	// if unable to find part
}

// Iterate through parts
//
cpart * CPartList::GetFirstPart()
{
	cpart * p = m_start.next;
	if( p->next )
		return p;
	else
		return NULL;
}

cpart * CPartList::GetEndPart()
{
	cpart * p = m_end.prev;
	if( p->prev )
		return p;
	else
		return NULL;
}

cpart * CPartList::GetNextPart( cpart * part )
{
	cpart * p = part->next;
	if( !p )
		return NULL;
	if( !p->next )
		return NULL;
	else
		return p;
}

cpart * CPartList::GetPrevPart( cpart * part )
{
	cpart * p = part->prev;
	if( !p )
		return NULL;
	if( !p->prev )
		return NULL;
	else
		return p;
}

// Get selected count
int CPartList::GetSelCount()
{
	int cnt=0;
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		if( part->shape )
			if( part->selected )
				cnt++;
		part = part->next;
	}
	return cnt;
}

int CPartList::GetSelParts(CString * AllSelected)
{
	if( AllSelected )
	{
		*AllSelected = "";
		for(cpart* gp=GetFirstPart(); gp; gp=GetNextPart(gp))
			if( gp->selected )
				*AllSelected += (gp->ref_des + " ");
		return 1;
	}
	return 0;
}

// get number of times a particular shape is used
//
int CPartList::GetNumFootprintInstances( CShape * shape )
{
	int n = 0;

	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		if(  part->shape == shape  )
			n++;
		part = part->next;
	}
	return n;
}

// Remove part from list and delete it
//
int CPartList::Remove( cpart * part )
{
	// delete all entries in display list
	UndrawPart( part );

	// remove links to this element
	part->next->prev = part->prev;
	part->prev->next = part->next;
	// destroy part
	m_size--;
	delete( part );

	return 0;
}

// Remove all parts from list
//
void CPartList::RemoveAllParts()
{
	// traverse list, removing all parts
	while( m_end.prev != &m_start )
		Remove( m_end.prev );
}

// Set utility flag for all parts
//
void CPartList::MarkAllParts( int mark )
{
	cpart * part = GetFirstPart();
	while( part )
	{
		part->utility = mark;
		part = GetNextPart( part );
	}
}

// generate an array of strokes for a string that is attached to a part
// enter with:
//  str = pointer to text string
//	size = height of characters
//	w = stroke width
//	rel_angle = angle of string relative to part
//	rel_x, rel_y = position of string relative to part
//	angle = angle of part
//	x, y = postion of part
//	side = side of PCB that part is on
//	strokes = pointer to CArray of strokes to receive data
//	br = pointer to RECT to receive bounding rectangle
//	dlist = pointer to display list to use for drawing (not used)
//	sm = pointer to SMFontUtil for character data	
// returns number of strokes generated
//
int GenerateStrokesForPartString( CString * str, 
								  int size, int mirror, int rel_angle, int rel_xi, int rel_yi, int w, 
								  int x, int y, int angle, int side,
								  CArray<CPoint> * strokes, RECT * br, CPoint * pts,
								  CDisplayList * dlist, SMFontUtil * sm )
{
	strokes->SetSize( 0 );
	double x_scale = (double)size/22.0;
	double y_scale = (double)size/22.0;
	double y_offset = 9.0*y_scale;
	double xc = 0.0;
	CPoint si, sf;
	br->left = br->bottom = pts[0].x = pts[0].y = INT_MAX;
	br->right = br->top = pts[2].x = pts[2].y =   INT_MIN;
	for( int ic=0; ic<str->GetLength(); ic++ )
	{
		// get stroke info for character
		//int xi, yi, xf, yf;
		double coord[64][4];
		double min_x, min_y, max_x, max_y;
		int nstrokes = sm->GetCharStrokes( str->GetAt(ic), SIMPLEX, 
			&min_x, &min_y, &max_x, &max_y, coord, 64 );
		br->left = min( br->left, xc );
		br->bottom = min( br->bottom, (min_y*y_scale + y_offset ) );
		br->right = max( br->right, (max_x - min_x)*x_scale + xc );
		br->top = max( br->top, (max_y*y_scale + y_offset) );
		int sz = strokes->GetSize();
		if( nstrokes > 0 )
			strokes->SetSize( sz + nstrokes*2 );
		for( int is=0; is<nstrokes; is++ )
		{
			si.x = (coord[is][0] - min_x)*x_scale + xc;
			si.y = coord[is][1]*y_scale + y_offset;
			sf.x = (coord[is][2] - min_x)*x_scale + xc;
			sf.y = coord[is][3]*y_scale + y_offset;

			// mirror
			if( mirror )
			{
				si.x = -si.x;
				sf.x = -sf.x;
			}
			// get stroke relative to text box
			// rotate about text box origin
			if( rel_angle )
			{
				RotatePoint( &si, -rel_angle, zero );
				RotatePoint( &sf, -rel_angle, zero );
			}
			// move origin of text box to position relative to part
			si.x += rel_xi;
			sf.x += rel_xi;
			si.y += rel_yi;
			sf.y += rel_yi;
			// flip if part on bottom
			if( side )
			{
				si.x = -si.x;
				sf.x = -sf.x;
			}

			// rotate with part about part origin
			if( angle )
			{
				RotatePoint( &si, -angle, zero );
				RotatePoint( &sf, -angle, zero );
			}

			// add x, y to part origin and draw
			(*strokes)[sz+is*2].x = x + si.x;
			(*strokes)[sz+is*2].y = y + si.y;
			(*strokes)[sz+is*2+1].x = x + sf.x;
			(*strokes)[sz+is*2+1].y = y + sf.y;
		}
		xc += (max_x - min_x + 8.0)*x_scale;
	}
	SwellRect( br, w/2 );
	pts[0].x = pts[1].x = br->left;
	pts[0].y = pts[3].y = br->bottom;
	pts[2].x = pts[3].x = br->right;
	pts[2].y = pts[1].y = br->top;
	if( rel_angle )
		RotatePOINTS( pts, 4, -rel_angle, zero );
	// move origin of text box to position relative to part
	pts[0].x += rel_xi;
	pts[0].y += rel_yi;
	pts[1].x += rel_xi;
	pts[1].y += rel_yi;
	pts[2].x += rel_xi;
	pts[2].y += rel_yi;
	pts[3].x += rel_xi;
	pts[3].y += rel_yi;
	// flip if part on bottom
	if( side )
	{
		pts[0].x = -pts[0].x;
		pts[1].x = -pts[1].x;
		pts[2].x = -pts[2].x;
		pts[3].x = -pts[3].x;
	}
	if(angle)
		RotatePOINTS( pts, 4, -angle, zero );
	pts[0].x += x;
	pts[0].y += y;
	pts[1].x += x;
	pts[1].y += y;
	pts[2].x += x;
	pts[2].y += y;
	pts[3].x += x;
	pts[3].y += y;
	*br = rect( pts[0].x, pts[0].y, pts[1].x, pts[1].y );
	SwellRect(br,pts[2]);
	SwellRect(br,pts[3]);
	return strokes->GetSize();
}


// Draw part into display list
//
int CPartList::DrawPart( cpart * part )
{
	return PL_NOERR;
}

// Undraw part from display list
//
int CPartList::UndrawPart( cpart * part )
{
	int i;

	if( !m_dlist )
		return 0;
	if( !part )
		return 0;
	if( part->drawn == FALSE )
		return 0;

	CShape * shape = part->shape;
	if( shape )
	{
		// undraw selection rectangle
		m_dlist->Remove( part->dl_sel );
		part->dl_sel = 0;
		m_dlist->Remove( part->dl_ref_el );
		part->dl_ref_el = 0;
		// undraw selection rectangle for ref text
		m_dlist->Remove( part->dl_ref_sel );
		part->dl_ref_sel = 0;
		m_dlist->Remove( part->dl_value_el );
		part->dl_value_el = 0;
		// undraw selection rectangle for value
		m_dlist->Remove( part->dl_value_sel );
		part->dl_value_sel = 0;

		// undraw part outline (this also includes footprint free text)
		for( i=part->m_outline_stroke.GetSize()-1; i>=0; i-- )
		{
			m_dlist->Remove( (dl_element*)part->m_outline_stroke[i] );
			part->m_outline_stroke[i] = 0;
		}

		// undraw padstacks
		for( i=part->pin.GetSize()-1; i>=0; i-- )
		{
			part_pin * pin = &part->pin[i];
			for( int il=pin->dl_els.GetSize()-1; il>=0; il-- )
				m_dlist->Remove( pin->dl_els[il] );
			pin->dl_els.RemoveAll();
			m_dlist->Remove( pin->dl_hole );
			m_dlist->Remove( pin->dl_sel );
			pin->dl_hole = NULL;
			pin->dl_sel = NULL;
		}
	}
	part->drawn = FALSE;
	return PL_NOERR;
}

// the footprint was changed for a particular part
// note that this function also updates the netlist
//
void CPartList::PartFootprintChanged( cpart * part, CShape * new_shape, CString * PINS )
{
	UndrawPart( part );
	// new footprint
	part->shape = new_shape;
	part->pin.SetSize( new_shape->GetNumPins() );
	// calculate new pin positions
	if( part->shape && new_shape )
	{
		int dx=0, dy=0;
		if( part->shape->origin_moved_X || part->shape->origin_moved_Y )
		{
			dx = part->shape->origin_moved_X;
			dy = part->shape->origin_moved_Y;
		}
		else if( new_shape->origin_moved_X || new_shape->origin_moved_Y )
		{
			dx = new_shape->origin_moved_X;
			dy = new_shape->origin_moved_Y;
		}
		if( dx || dy )
		{
			part->m_ref_xi += dx;
			part->m_ref_yi += dy;
			part->m_value_xi += dx;
			part->m_value_yi += dy;
			if( part->side )
				dx = -dx;
			if( 180-part->angle )
				Rotate_i_Vertex( &dx, &dy, 180-part->angle, 0, 0 );
			Move( part, part->x+dx, part->y+dy,
				part->angle, part->side, 1 );

		}
		for( int ip=0; ip<part->pin.GetSize(); ip++ )
		{
			CPoint pt = GetPinPoint( part, ip, part->side, part->angle );
			part->pin[ip].x = pt.x;
			part->pin[ip].y = pt.y;
		}
	}
	DrawPart( part );
	m_nlist->PartFootprintChanged( part, PINS );
}

// the footprint was modified, apply to all parts using it
//
void CPartList::FootprintChanged( CShape * shape )
{
	// find all parts with given footprint and update them
	CString PINS="_";// initial value
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		if( part->shape )
		{
			if(  part->shape->m_name == shape->m_name  )
			{
				PartFootprintChanged( part, shape, &PINS );
			}
		}
		part = part->next;
	}
}

// the ref text height and width were modified, apply to all parts using it
//
void CPartList::RefTextSizeChanged( CShape * shape )
{
	// find all parts with given shape and update them
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		if(  part->shape->m_name == shape->m_name  )
		{
			ResizeRefText( part, shape->m_ref_size, shape->m_ref_w );
		}
		part = part->next;
	}
}

// Make part visible or invisible, including thermal reliefs
//
void CPartList::MakePartVisible( cpart * part, BOOL bVisible )
{
	// make part elements invisible, including copper area connections
	// outline strokes
	for( int i=0; i<part->m_outline_stroke.GetSize(); i++ )
	{
		dl_element * el = part->m_outline_stroke[i];
		if( el )
			el->visible = bVisible;
	}
	// pins
	if( part->shape )
	{
		for( int ip=0; ip<part->shape->m_padstack.GetSize(); ip++ )
		{
			// pin pads
			dl_element * el = part->pin[ip].dl_hole;
			if( el )
				el->visible = bVisible;
			for( int i=part->pin[ip].dl_els.GetSize()-1; i>=0; i-- )
			{
				if( part->pin[ip].dl_els[i] )
					part->pin[ip].dl_els[i]->visible = bVisible;
			}
			// pin copper area connections
			cnet * net = (cnet*)part->pin[ip].net;
			if( net )
			{
				for( int ia=0; ia<net->nareas; ia++ )
				{
					for( int i=0; i<net->area[ia].npins; i++ )
					{
						if( net->pin[net->area[ia].pin[i]].part == part )
						{
							net->area[ia].dl_thermal[i]->visible = bVisible;
						}
					}
				}
			}
		}
	}
	// ref text strokes
	if( part->dl_ref_el )
		part->dl_ref_el->visible = bVisible;
	// value strokes
	if( part->dl_value_el )
		part->dl_value_el->visible = bVisible;
}

// Start dragging part by setting up display list
// if bRatlines == FALSE, no rubber-band ratlines
// else if bBelowPinCount == TRUE, only use ratlines for nets with less than pin_count pins
//
int CPartList::StartDraggingPart( CDC * pDC, cpart * part, BOOL bRatlines, 
								 BOOL bBelowPinCount, int pin_count, BOOL bSaveStartPos )
{
	if( !part->shape )
		return 0;
	if( !part->drawn )
		DrawPart( part );

	// save position
	if( bSaveStartPos )
	{
		begin_dragging_x = part->x;
		begin_dragging_y = part->y;
		begin_dragging_ang = part->angle;
		begin_dragging_side = part->side;
	}

	// make part invisible
	MakePartVisible( part, FALSE );
	m_dlist->CancelHighLight();

	// create drag lines
	int psz = part->shape->m_padstack.GetSize();
	CArray<CPoint> pin_points;
	pin_points.SetSize( part->shape->m_padstack.GetSize() );
	if( psz == 0 )
	{
		m_dlist->MakeDragLineArray( 4 );
		CPoint P[4];
		int np = 4;
		m_dlist->Get_Points( part->dl_sel, P, &np, -part->x, -part->y );
		if( np == 4 )
		{
			for( int ii=0; ii<np-1; ii++ )
				m_dlist->AddDragLine( P[ii], P[ii+1] );
			m_dlist->AddDragLine( P[3], P[0] );
		} 
		else // rectangle
		{
			RECT Get;
			m_dlist->Get_Rect( part->dl_sel, &Get );
			CPoint pt[4];
			pt[0].x = Get.left		- part->x;
			pt[0].y = Get.bottom	- part->y;
			pt[1].x = Get.left		- part->x;
			pt[1].y = Get.top		- part->y;
			pt[2].x = Get.right		- part->x;
			pt[2].y = Get.top		- part->y;
			pt[3].x = Get.right		- part->x;
			pt[3].y = Get.bottom	- part->y;
			m_dlist->AddDragLine( P[0], P[3] );
			m_dlist->AddDragLine( P[0], P[1] );
			m_dlist->AddDragLine( P[1], P[2] );
			m_dlist->AddDragLine( P[2], P[3] );
		}
	}
	else
	{
		m_dlist->MakeDragLineArray( 4*part->shape->m_padstack.GetSize() );
		for( int ip=0; ip<part->shape->m_padstack.GetSize(); ip++ )
		{
			if( part->pin[ip].dl_sel == 0 )
				continue;
			CPoint P[4];
			int np = 4;
			m_dlist->Get_Points( part->pin[ip].dl_sel, P, &np, -part->x, -part->y );
			if( np == 4 )
			{
				for( int ii=0; ii<np-1; ii++ )
					m_dlist->AddDragLine( P[ii], P[ii+1] );
				m_dlist->AddDragLine( P[3], P[0] );
			} 
			else // rectangle
			{
				RECT Get;
				m_dlist->Get_Rect( part->pin[ip].dl_sel, &Get );
				CPoint pt[4];
				pt[0].x = Get.left    - part->x;
				pt[0].y = Get.bottom  - part->y;
				pt[1].x = Get.left	  - part->x;
				pt[1].y = Get.top	  - part->y;
				pt[2].x = Get.right   - part->x;
				pt[2].y = Get.top	  - part->y;
				pt[3].x = Get.right   - part->x;
				pt[3].y = Get.bottom  - part->y;
				m_dlist->AddDragLine( P[0], P[3] );
				m_dlist->AddDragLine( P[0], P[1] );
				m_dlist->AddDragLine( P[1], P[2] );
				m_dlist->AddDragLine( P[2], P[3] );
			}
			// save pin position
			pin_points[ip].x = part->pin[ip].x - part->x;
			pin_points[ip].y = part->pin[ip].y - part->y;
		}
	}
	// create drag lines for ratlines connected to pins
	if( bRatlines ) 
	{
		m_dlist->MakeDragRatlineArray( 2*part->shape->m_padstack.GetSize(), 1 );
		// zero utility flags for all nets
		cnet * n = m_nlist->GetFirstNet();
		while( n )
		{
			n->utility = 0;
			n = m_nlist->GetNextNet(/*LABEL*/);
		}

		// now loop through all pins in part to find nets that connect
		for( int ipp=0; ipp<part->shape->m_padstack.GetSize(); ipp++ )
		{
			n = (cnet*)part->pin[ipp].net;
			if( n )
			{
				// only look at visible nets, only look at each net once
				if( /*n->visible &&*/ n->utility == 0 )
				{
					// zero utility flags for all connections
					for( int ic=0; ic<n->nconnects; ic++ )
					{
						n->connect[ic].utility = 0;
					}
					for( int ic=0; ic<n->nconnects; ic++ )
					{
						cconnect * c = &n->connect[ic];
						if( c->utility )
							continue;	// skip this connection

						// check for connection to part
						int pin1 = n->connect[ic].start_pin;
						int pin2 = n->connect[ic].end_pin;
						cpart * pin1_part = n->pin[pin1].part;
						cpart * pin2_part = NULL;
						if( pin2 != cconnect::NO_END )
							pin2_part = n->pin[pin2].part;
						if( pin1_part != part && pin2_part != part )
							continue;	// no

						// OK, this connection is attached to our part 
						if( pin1_part == part )
						{
							int ip = n->connect[ic].start_pin_shape;
							if( ip != -1 )
							{
								// ip is the start pin for the connection
								// hide segment
								c->seg[0].dl_el->visible = 0;
								// add ratline
//**								if( !bBelowPinCount || n->npins <= pin_count )
								{
									BOOL bDraw = FALSE;
									if( pin2_part == part )
									{
										// connection starts and ends on this part,
										// only drag if 3 or more segments
										if( c->nsegs > 2 )
											bDraw = TRUE;
									}
									else 
										bDraw = TRUE;
									if( bDraw )
									{
										// add dragline from pin to second vertex
										CPoint vx( c->vtx[1].x, c->vtx[1].y );
										m_dlist->AddDragRatline( vx, pin_points[ip] );
									}
								}
							}
						}
						if( pin2_part == part )
						{
							int ip = -1;
							if( pin2 != cconnect::NO_END )
								ip = n->connect[ic].end_pin_shape;
							if( ip != -1 )
							{
								// ip is the end pin for the connection
								c->seg[c->nsegs-1].dl_el->visible = 0;
								// OK, get prev vertex, add ratline and hide segment
//**								if( !bBelowPinCount || n->npins <= pin_count )
								{
									BOOL bDraw = FALSE;
									if( pin1_part == part )
									{
										// starts and ends on part
										if( c->nsegs > 2 )
											bDraw = TRUE;
									}
									else
										bDraw = TRUE;
									if( bDraw )
									{
										CPoint vx( n->connect[ic].vtx[c->nsegs-1].x, n->connect[ic].vtx[c->nsegs-1].y );
										m_dlist->AddDragRatline( vx, pin_points[ip] );
									}
								}
							}
						}
						c->utility = 1;	// this connection has been checked
					}
				}
				n->utility = 1;	// all connections for this net have been checked
			}
		}
	}
	m_dlist->StartDraggingArray( pDC, part->x, part->y );
	return 0;
}

// start dragging ref text
//
int CPartList::StartDraggingRefText( CDC * pDC, cpart * part )
{
	// make ref text elements invisible
	if( part->dl_ref_el )
		part->dl_ref_el->visible = 0;

	// cancel selection 
	m_dlist->CancelHighLight();
	CPoint P[4];
	int np = 4;
	// create drag lines
	m_dlist->MakeDragLineArray( np );
	CPoint ref_p = GetRefPoint( part );
	m_dlist->Get_Points( part->dl_ref_sel, P, &np, -ref_p.x, -ref_p.y );
	if( np == 4 )
	{
		for( int ii=0; ii<np-1; ii++ )
			m_dlist->AddDragLine( P[ii], P[ii+1] );
		m_dlist->AddDragLine( P[3], P[0] );
	} 
	else //rectangle
	{
		RECT Get;
		m_dlist->Get_Rect( part->dl_ref_sel, &Get );
		CPoint pt[4];
		pt[0].x = Get.left;
		pt[0].y = Get.bottom;
		pt[1].x = Get.left;
		pt[1].y = Get.top;
		pt[2].x = Get.right;
		pt[2].y = Get.top;
		pt[3].x = Get.right;
		pt[3].y = Get.bottom;
		m_dlist->AddDragLine( pt[0], pt[3] );
		m_dlist->AddDragLine( pt[0], pt[1] );
		m_dlist->AddDragLine( pt[1], pt[2] );
		m_dlist->AddDragLine( pt[2], pt[3] );
	}
	m_dlist->StartDraggingArray( pDC, ref_p.x, ref_p.y );
	return 0;
}

// start dragging value
//
int CPartList::StartDraggingValue( CDC * pDC, cpart * part )
{
	// make value text elements invisible
	if( part->dl_value_el )
		part->dl_value_el->visible = 0;
	int rx,ry;
	// cancel selection 
	m_dlist->CancelHighLight();
	CPoint P[4];
	int np = 4;
	// create drag lines
	m_dlist->MakeDragLineArray( np );
	CPoint value_p = GetValuePoint( part );
	m_dlist->Get_Points( part->dl_value_sel, P, &np, -value_p.x, -value_p.y );
	if( np == 4 )
	{
		for( int ii=0; ii<np-1; ii++ )
			m_dlist->AddDragLine( P[ii], P[ii+1] );
		m_dlist->AddDragLine( P[3], P[0] );
	} 
	else //rectangle
	{
		RECT Get;
		m_dlist->Get_Rect( part->dl_value_sel, &Get );
		CPoint pt[4];
		pt[0].x = Get.left;
		pt[0].y = Get.bottom;
		pt[1].x = Get.left;
		pt[1].y = Get.top;
		pt[2].x = Get.right;
		pt[2].y = Get.top;
		pt[3].x = Get.right;
		pt[3].y = Get.bottom;
		m_dlist->AddDragLine( pt[0], pt[3] );
		m_dlist->AddDragLine( pt[0], pt[1] );
		m_dlist->AddDragLine( pt[1], pt[2] );
		m_dlist->AddDragLine( pt[2], pt[3] );
	}
	m_dlist->StartDraggingArray( pDC, value_p.x, value_p.y );
	return 0;
}

// cancel dragging, return to pre-dragging state
//
int CPartList::CancelDraggingPart( cpart * part )
{
	Move(part,begin_dragging_x,begin_dragging_y,begin_dragging_ang,begin_dragging_side);
	m_nlist->PartMoved(part,0);
	// make part visible again
	MakePartVisible( part, TRUE );

	// get any connecting segments and make visible
	for( int ip=0; ip<part->shape->m_padstack.GetSize(); ip++ )
	{
		cnet * net = (cnet*)part->pin[ip].net;
		if( net )
		{
			if( net->visible )
			{
				for( int ic=0; ic<net->nconnects; ic++ )
				{
					cconnect * c = &net->connect[ic];
					int pin1 = c->start_pin;
					int pin2 = c->end_pin;
					int nsegs = c->nsegs;
					if( net->pin[pin1].part == part )
					{
						// start pin
						c->seg[0].dl_el->visible = 1;
					}
					if( pin2 != cconnect::NO_END )
					{
						if( net->pin[pin2].part == part )
						{
							// end pin
							c->seg[nsegs-1].dl_el->visible = 1;
						}
					}
				}
			}
		}
	}
	m_dlist->StopDragging();
	return 0;
}

// cancel dragging of ref text, return to pre-dragging state
int CPartList::CancelDraggingRefText( cpart * part )
{
	// make ref text elements invisible
	if( part->dl_ref_el )
		part->dl_ref_el->visible = 1;
	m_dlist->StopDragging();
	return 0;
}

// cancel dragging value, return to pre-dragging state
int CPartList::CancelDraggingValue( cpart * part )
{
	// make ref text elements invisible
	if( part->dl_value_el )
		part->dl_value_el->visible = 0;
	m_dlist->StopDragging();
	return 0;
}

// normal completion of any dragging operation
//
int CPartList::StopDragging()
{
	m_dlist->StopDragging();
	return 0;
}

// create part from string
//
cpart * CPartList::AddFromString( CString * str )
{
	CShape * s = NULL;
	CString in_str, key_str;
	CArray<CString> p;
	int pos = 0;
	int len = str->GetLength();
	int np;
	CString ref_des = "";
	BOOL ref_vis = FALSE;
	int ref_size = 0;
	int ref_width = 0;
	int ref_angle = 0;
	int ref_xi = 0;
	int ref_yi = 0;
	CString value = "";
	BOOL value_vis = FALSE;
	int value_size = 0;
	int value_width = 0;
	int value_angle = 0;
	int value_xi = 0;
	int value_yi = 0;
	CString package = "";
	int x = 0;
	int y = 0;
	int side = 0;
	int angle = 0;
	int glued = 0;
	int merge = -1;
	int submerge = -1;
	//
	in_str = str->Tokenize( "\n", pos );
	while( in_str.GetLength() )
	{
		np = ParseKeyString( &in_str, &key_str, &p );
		if( key_str == "ref" )
		{
			ref_des = in_str.Right( in_str.GetLength()-4 );
			ref_des.Trim();
			ref_des = ref_des.Left(MAX_REF_DES_SIZE);
		}
		else if( key_str == "part" )
		{
			ref_des = in_str.Right( in_str.GetLength()-5 );
			ref_des.Trim();
			ref_des = ref_des.Left(MAX_REF_DES_SIZE);
		}
		else if( np >= 6 && key_str == "ref_text" )
		{
			ref_size = my_atoi( &p[0] );
			ref_width = my_atoi( &p[1] );
			ref_angle = my_atoi( &p[2] );
			ref_xi = my_atoi( &p[3] );
			ref_yi = my_atoi( &p[4] );
			if( np >= 7 )
				ref_vis = my_atoi( &p[5] );
			else
			{
				if( ref_size )
					ref_vis = TRUE;
				else
					ref_vis = FALSE;
			}
		}
		else if( np >= 7 && key_str == "value" )
		{
			value = p[0];
			value_size = my_atoi( &p[1] );
			value_width = my_atoi( &p[2] );
			value_angle = my_atoi( &p[3] );
			value_xi = my_atoi( &p[4] );
			value_yi = my_atoi( &p[5] );
			if( np >= 8 )
				value_vis = my_atoi( &p[6] );
			else
			{
				if( value_size )
					value_vis = TRUE;
				else
					value_vis = FALSE;
			}
		}
		else if( key_str == "package" )
		{
			if( np >= 2 )
				package = p[0];
			else
				package = "";
			package = package.Left(CShape::MAX_NAME_SIZE);
		}
		else if( key_str == "merge" )
		{
			if( np >= 2 )
				merge = my_atoi( &p[0] );
			if( np >= 3 )
				submerge = my_atoi( &p[1] );
		}
		else if( np >= 2 && key_str == "shape" )
		{
			// lookup shape in cache
			s = NULL;
			void * ptr;
			CString name = p[0];
			name = name.Left(CShape::MAX_NAME_SIZE);
			int err = m_footprint_cache_map->Lookup( name, ptr );
			if( err )
			{
				// found in cache
				s = (CShape*)ptr; 
			}
		}
		else if( key_str == "pos" )
		{
			if( np >= 6 )
			{
				x = my_atoi( &p[0] );
				y = my_atoi( &p[1] );
				side = my_atoi( &p[2] );
				angle = my_atoi( &p[3] );
				glued = my_atoi( &p[4] );
			}
			else
			{
				x = 0;
				y = 0;
				side = 0;
				angle = 0;
				glued = 0;
			}
		}
		in_str = str->Tokenize( "\n", pos );
	}
	if( ref_des.GetLength() == 0 )
		return 0;
	cpart * part = GetPart( ref_des );
	if( part )
		return part;
	// so we only draw once
	CDisplayList * old_dlist = m_dlist;
	m_dlist = NULL;
	part = Add();
	SetPartData( part, s, &ref_des, x, y, side, angle, 1, glued, merge );
	SetValue( part, &value, value_xi, value_yi, value_angle, value_size, value_width, value_vis );
	if( part->shape ) 
	{
		part->m_ref_xi = ref_xi;
		part->m_ref_yi = ref_yi;
		part->m_ref_angle = ref_angle;
		ResizeRefText( part, ref_size, ref_width, ref_vis, 0 );
	}
	// set submerge
	part->sub_merge = submerge;
	//
	m_dlist = old_dlist;
	DrawPart( part );
	return part;
}

// set CString to description of part
//
int CPartList::SetPartString( cpart * part, CString * str )
{
	CString line;
	if( part->ref_des.GetLength() == 0 )
	{
		*str = "";
		return 0;
	}
	line.Format( "part: %s\n", part->ref_des );  
	*str = line;
	if( part->shape )
	{
		line.Format( "  ref_text: %d %d %d %d %d %d\n", 
						part->m_ref_size, part->m_ref_w, 
						part->m_ref_angle%360,
						part->m_ref_xi, part->m_ref_yi, part->m_ref_vis );
		str->Append( line );
		line.Format( "  shape: \"%s\"\n", part->shape->m_name );
		str->Append( line );
		if ( part->m_merge >= 0 )
		{
			line.Format( "  merge: %d %d\n", part->m_merge, part->sub_merge );
			str->Append( line );
		}
		if ( part->value.GetLength() )
		{
			line.Format( "  value: \"%s\" %d %d %d %d %d %d\n", 
							part->value, part->m_value_size, 
							part->m_value_w, part->m_value_angle%360,
							part->m_value_xi, part->m_value_yi,
							part->m_value_vis );
			str->Append( line );
		}
		line.Format( "  pos: %d %d %d %d %d\n", part->x, part->y, part->side, part->angle%360, part->glued );
		str->Append( line );
	}
	else if ( part->value.GetLength() )
	{
		line.Format( "  value: \"%s\" %d %d %d %d %d %d\n", 
						part->value, part->m_value_size, 
						part->m_value_w, part->m_value_angle%360,
						part->m_value_xi, part->m_value_yi,
						part->m_value_vis );
		str->Append( line );
	}
	line.Format( "\n" );
	str->Append( line );
	return 0;
}

// create record describing part for use by CUndoList
// if part == NULL, just set m_plist and new_ref_des
//
undo_part * CPartList::CreatePartUndoRecord( cpart * part, CString * new_ref_des )
{
	int NumPins = 0;	
	int size = sizeof( undo_part );
	if( part )
	{
		if( part->shape )
			NumPins = part->shape->GetNumPins();
		size = sizeof( undo_part ) + NumPins*(CShape::MAX_PIN_NAME_SIZE+1);
	}
	undo_part * upart = (undo_part*)malloc( size );
	upart->size = size;
	upart->m_plist = this;
	if( part )
	{
		char * chptr = (char*)upart;
		chptr += sizeof(undo_part);
		upart->m_id = part->m_id;
		upart->visible = part->visible;
		upart->x = part->x;
		upart->y = part->y;
		upart->side = part->side;
		upart->angle = part->angle;
		upart->glued = part->glued;
		upart->m_ref_vis = part->m_ref_vis;
		upart->m_ref_xi = part->m_ref_xi;
		upart->m_ref_yi = part->m_ref_yi;
		upart->m_ref_angle = part->m_ref_angle;
		upart->m_ref_size = part->m_ref_size;
		upart->m_ref_w = part->m_ref_w;
		upart->m_value_vis = part->m_value_vis;
		upart->m_value_xi = part->m_value_xi;
		upart->m_value_yi = part->m_value_yi;
		upart->m_value_angle = part->m_value_angle;
		upart->m_value_size = part->m_value_size;
		upart->m_value_w = part->m_value_w;
		strcpy( upart->ref_des, part->ref_des );				
		strcpy( upart->value, part->value );
		upart->merge_p = part->m_merge;
		upart->submerge_p = part->sub_merge;
		upart->shape = part->shape;
		if( part->shape )
		{
			strcpy( upart->shape_name, part->shape->m_name );
			// save names of nets attached to each pin
			for( int ip=0; ip<NumPins; ip++ )
			{
				if( cnet * net = part->pin[ip].net )
					strcpy( chptr, net->name );
				else
					*chptr = 0;
				chptr += CShape::MAX_PIN_NAME_SIZE + 1;
			}
		}
	}
	if( new_ref_des )
		strcpy( upart->new_ref_des, *new_ref_des );
	else
		strcpy( upart->new_ref_des, part->ref_des );
	return upart;
}

#if 0
// create special record for use by CUndoList
//
void * CPartList::CreatePartUndoRecordForRename( cpart * part, CString * old_ref_des )
{
	int size = sizeof( undo_part );
	undo_part * upart = (undo_part*)malloc( size );
	upart->m_plist = this;
	strcpy( upart->ref_des, part->ref_des );
	upart->merge_name = part->m_merge;
	strcpy( upart->package, *old_ref_des );
	return (void*)upart;
}
#endif


// export part list data into partlist_info structure for editing in dialog
// if test_part != NULL, returns index of test_part in partlist_info
//
int CPartList::ExportPartListInfo( partlist_info * pl, cpart * test_part )
{
	// traverse part list to find number of parts
	int ipart = -1;
	int nparts = 0;
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		nparts++;
		part = part->next;
	}
	// now make struct
	pl->SetSize( nparts );
	int i = 0;
	part = m_start.next;
	while( part->next != 0 )
	{
		if( part == test_part )
			ipart = i;
		(*pl)[i].part = part;
		(*pl)[i].shape = part->shape;
		(*pl)[i].bShapeChanged = FALSE;
		(*pl)[i].ref_des = part->ref_des;
		if( part->shape )
		{
			(*pl)[i].ref_size = part->m_ref_size;
			(*pl)[i].ref_width = part->m_ref_w;
		}
		else
		{
			(*pl)[i].ref_size = 0;
			(*pl)[i].ref_width = 0;
		}	
		(*pl)[i].value = part->value;
		(*pl)[i].value_vis = part->m_value_vis;
		(*pl)[i].x = part->x;
		(*pl)[i].y = part->y;
		(*pl)[i].angle = part->angle;
		(*pl)[i].side = part->side;
		(*pl)[i].deleted = FALSE;
		(*pl)[i].selected = part->selected;
		(*pl)[i].bOffBoard = FALSE;
		(*pl)[i].mrgs = part->m_merge;
		i++;
		part = part->next;
	}
	return ipart;
}

// import part list data from struct partlist_info
//
void CPartList::ImportPartListInfo( partlist_info * pl, int flags, CDlgLog * log )
{
	CString mess; 
	MarkAllParts(FALSE);	

	// grid for positioning parts off-board
	int pos_x = 0;
	int pos_y = 0;
	enum { GRID_X = PL_MAX_SIZE/100, GRID_Y = 100 };
	BOOL * grid = (BOOL*)calloc( GRID_X*GRID_Y, sizeof(BOOL) );
	int grid_num = 0;

	// first, look for parts in project whose ref_des has been changed
#define REPEAT_REF 1
	int Duplicate, PartRef_Changed;
	do{
		Duplicate = 0;
		PartRef_Changed = 0;
		for( int i=0; i<pl->GetSize(); i++ )
		{
			part_info * pi = &(*pl)[i];
			if( pi->part )
			{
				if( pi->ref_des.Compare(pi->part->ref_des) )
				{
					int d=0;
					// test1 (disabled)
					for( cpart * p=GetFirstPart(); p; p=GetNextPart(p) )
					{
						if( p->ref_des.Compare( pi->ref_des ) == 0 )
						{
							d = 1;
							Duplicate |= d; //fail
							break;
						}
					}
					if( d )
						continue;

					// test2
					d = m_nlist->PartRefChanged( &pi->part->ref_des, &pi->ref_des );
					Duplicate |= d;
					if( d == 0 )
					{
						pi->part->ref_des = pi->ref_des;
						pi->part->utility = 1;
						PartRef_Changed = 1;
					}
				}
				if( pi->selected )
					pi->part->utility = 1;
			}
		}
		if( PartRef_Changed == 0 && Duplicate )
		{
			AfxMessageBox( "Sorry: Parts can't be renumbered :-(" );
			return;
		}
	}while( Duplicate );
#undef REPEAT_REF

	// undraw all parts and disable further drawing
	CDisplayList * old_dlist = m_dlist;
	if( m_dlist )
	{
		cpart * part = GetFirstPart();
		while( part )
		{
			UndrawPart( part );
			part = GetNextPart( part );
		}
	}
	m_dlist = NULL;	

	// now find parts in project that are not in partlist_info
	// loop through all parts in project
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		// loop through the partlist_info array
		BOOL bFound = FALSE;
		part->bPreserve = FALSE;
		for( int i=0; i<pl->GetSize(); i++ )
		{
			part_info * pi = &(*pl)[i];
			if( pi->ref_des == part->ref_des )
			{
				// part exists in partlist_info
				bFound = TRUE;
				break;
			}
		}
		cpart * next_part = part->next;
		if( !bFound )
		{
			// part in project but not in partlist_info
			if( flags & KEEP_PARTS_AND_CON )
			{
				// set flag to preserve this part
				part->bPreserve = TRUE;
				if( log )
				{
					mess.Format( "  Keeping part %s and connections\r\n", part->ref_des );
					log->AddLine( mess );
				}
			}
			else if( flags & KEEP_PARTS_NO_CON )
			{
				// keep part but remove connections from netlist
				if( log )
				{
					mess.Format( "  Keeping part %s but removing connections\r\n", part->ref_des );
					log->AddLine( mess );
				}
				m_nlist->PartDeleted( part );
			}
			else
			{
				// remove part
				if( log )
				{
					mess.Format( "  Removing part %s\r\n", part->ref_des );
					log->AddLine( mess );
				}
				m_nlist->PartDeleted( part );
				Remove( part );
			}
		}
		part = next_part;
	}

	// loop through partlist_info array, changing partlist as necessary
	for( int i=0; i<pl->GetSize(); i++ )
	{
		part_info * pi = &(*pl)[i];
		if( pi->part == 0 && pi->deleted )
		{
			// new part was added but then deleted, ignore it
			continue;
		}
		if( pi->part != 0 && pi->deleted )
		{
			// old part was deleted, remove it
			m_nlist->PartDisconnected( pi->part );
			Remove( pi->part );
			continue;
		}

		if( pi->part == 0 )
		{
			// the partlist_info does not include a pointer to an existing part
			// the part might not exist in the project, or we are importing a netlist file
			cpart * old_part = GetPart( pi->ref_des );
			if( old_part )
			{
				// an existing part has the same ref_des as the new part
				if( old_part->shape )
				{
					BOOL compare_sh = FALSE;
					if( pi->shape )
						if(pi->shape->m_name.Compare(old_part->shape->m_name) == 0 )
							if(pi->shape->m_package.Compare(old_part->shape->m_package) == 0 )
								compare_sh = TRUE;
					// the existing part has a footprint
					// see if the incoming package name matches the old package or footprint
					pi->part = old_part;
					pi->ref_size = old_part->m_ref_size; 
					pi->ref_width = old_part->m_ref_w;
					if( pi->value.GetLength() == 0 )
						pi->value = old_part->value;
					pi->value_vis = old_part->m_value_vis;
					pi->x = old_part->x; 
					pi->y = old_part->y;
					pi->angle = old_part->angle;
					pi->side = old_part->side;
					pi->mrgs = old_part->m_merge;
					pi->submrgs  = old_part->sub_merge;
					if( (flags & KEEP_FP) || compare_sh )
					{
						// use footprint and parameters from existing part						
						pi->shape = old_part->shape;
					}
					else 
					{
						// use new footprint, but preserve position
						if( pi->shape )
						{
							pi->bShapeChanged = TRUE;
							if( log && old_part->shape->m_name != pi->shape->m_name )
							{
								mess.Format( "  Changing footprint of part %s from \"%s\" to \"%s\"\r\n", 
									old_part->ref_des, old_part->shape->m_name, pi->shape->m_name );
								log->AddLine( mess );
							}
						}
					}
				}
				else
				{
					// remove old part (which did not have a footprint)
					if( log && pi->shape )
					{
						mess.Format( "  Adding footprint for part %s \"%s\"\r\n", 
							old_part->ref_des, pi->shape->m_package );
						log->AddLine( mess );
					}
					m_nlist->PartDisconnected( old_part );
					Remove( old_part );
				}
			}
		}

		if( pi->part )
		{
			if( pi->part->shape != pi->shape || pi->bShapeChanged == TRUE )
			{
				// old part exists, but footprint was changed
				if( pi->part->shape == NULL )
				{
					// old part did not have a footprint before, so remove it
					// and treat as new part
					m_nlist->PartDisconnected( pi->part );
					Remove( pi->part );
					pi->part = NULL;
				}
			}
		}

		if( pi->part == 0 )
		{
			// new part is being imported (with or without footprint)
			if( pi->shape && pi->bOffBoard )
			{
				// place new part offboard, using grid 
				int ix, iy;	// grid indices
				// find size of part in 100 mil units
				BOOL OK = FALSE;
				int w = abs( pi->shape->selection.right - pi->shape->selection.left )/(100*PCBU_PER_MIL)+2;
				int h = abs( pi->shape->selection.top - pi->shape->selection.bottom )/(100*PCBU_PER_MIL)+2;
				// now find space in grid for part
				for( ix=0; ix<GRID_X; ix++ )
				{
					iy = 0;
					while( iy < (GRID_Y - h) )
					{
						if( !grid[ix+GRID_X*iy] )
						{
							// see if enough space
							OK = TRUE;
							for( int iix=ix; iix<(ix+w); iix++ )
								for( int iiy=iy; iiy<(iy+h); iiy++ )
									if( grid[iix+GRID_X*iiy] )
										OK = FALSE;
							if( OK )
								break;
						}
						iy++;
					}
					if( OK )
						break;
				}
				if( OK )
				{
					// place part
					pi->side = 0;
					pi->angle = 0;
					if( grid_num == 0 )
					{
						// first grid, to left and above origin
						pi->x = -(ix+w)*100*PCBU_PER_MIL;
						pi->y = iy*100*PCBU_PER_MIL;
					}
					else if( grid_num == 1 )
					{
						// second grid, to left and below origin
						pi->x = -(ix+w)*100*PCBU_PER_MIL;
						pi->y = -(iy+h)*100*PCBU_PER_MIL;
					}
					else if( grid_num == 2 )
					{
						// third grid, to right and below origin
						pi->x = ix*100*PCBU_PER_MIL;
						pi->y = -(iy+h)*100*PCBU_PER_MIL;
					}
					// remove space in grid
					for( int iix=ix; iix<(ix+w); iix++ )
						for( int iiy=iy; iiy<(iy+h); iiy++ )
							grid[iix+GRID_X*iiy] = TRUE;
				}
				else
				{
					// fail, go to next grid
					if( grid_num == 2 )
						ASSERT(0);		// ran out of grids
					else
					{
						// zero grid
						for( int j=0; j<GRID_Y; j++ )
							for( int i=0; i<GRID_X; i++ )
								grid[j*GRID_X+i] = FALSE;
						grid_num++;
					}
				}
				// now offset for part origin
				pi->x -= pi->shape->selection.left;
				pi->y -= pi->shape->selection.bottom;
			}
			// now place part
			cpart * part = Add( pi->shape, &pi->ref_des, pi->x, pi->y,
				pi->side, pi->angle, TRUE, FALSE );
			if( part->shape )
			{
				ResizeRefText( part, pi->ref_size, pi->ref_width );
				SetValue( part, &pi->value, 
					part->shape->m_value_xi, 
					part->shape->m_value_yi,
					part->shape->m_value_angle, 
					part->shape->m_value_size, 
					part->shape->m_value_w,
					pi->value_vis );
			}
			else
				SetValue( part, &pi->value, 0, 0, 0, 0, 0 );
			m_nlist->PartAdded( part );
		}
		else
		{
			if( pi->part->value.Compare(pi->value) )
			{
				// value changed, keep size and position
				SetValue( pi->part, &pi->value );
			}
			if( pi->part->m_value_vis != pi->value_vis )
			{
				// value visibility changed
				pi->part->m_value_vis = pi->value_vis;
			}
			if( pi->part->shape != pi->shape || pi->bShapeChanged == TRUE )
			{
				// footprint was changed
				if( pi->part->shape == NULL )
				{
					ASSERT(0);	// should never get here
				}
				else if( pi->shape && !(flags & KEEP_FP) )
				{
					int prevx = pi->part->x;
					int prevy = pi->part->y;
					// change footprint to new one
					PartFootprintChanged( pi->part, pi->shape );
					//
					pi->x += (pi->part->x - prevx);
					pi->y += (pi->part->y - prevy);
				}
			}
			if( pi->x != pi->part->x 
				|| pi->y != pi->part->y
				|| pi->angle != pi->part->angle
				|| pi->side != pi->part->side )
			{
				// part was moved
				Move( pi->part, pi->x, pi->y, pi->angle, pi->side );
				m_nlist->PartMoved( pi->part , FALSE );
			}
		}
	}
	free( grid );

	// redraw partlist
	m_dlist = old_dlist;
	part = GetFirstPart();
	while( part )
	{
		DrawPart( part );
		part = GetNextPart( part );
	}
}

// undo an operation on a part
// note that this is a static function, for use as a callback
//
void CPartList::PartUndoCallback( int type, void * ptr, BOOL undo )
{
	undo_part * upart = (undo_part*)ptr;

	if( undo )
	{
		// perform undo
		CString new_ref_des = upart->new_ref_des;
		CString old_ref_des = upart->ref_des;
		CPartList * pl = upart->m_plist;
		CDisplayList * old_dlist = pl->m_dlist;	
		cpart * part = pl->GetPart( new_ref_des );
		if( part && type == UNDO_PART_ADD )
		{
			// part was added, just delete it		
			pl->m_nlist->PartDeleted( part );
			pl->Remove( part );
		}
		else if( type == UNDO_PART_DELETE )
		{
			// part was deleted, lookup shape in cache and add part
			pl->m_dlist = NULL;		// prevent drawing
			CShape * s;
			void * s_ptr;
			int err = pl->m_footprint_cache_map->Lookup( upart->shape_name, s_ptr );
			if( err )
			{
				// found in cache
				s = (CShape*)s_ptr; 
			}
			else
				ASSERT(0);	// shape not found
			CString ref_des = upart->ref_des;
			part = pl->Add( s, &ref_des, upart->x, upart->y,
				upart->side, upart->angle, upart->visible, upart->glued );
			part->m_merge = upart->merge_p;
			part->sub_merge = upart->submerge_p;
			part->m_ref_vis = upart->m_ref_vis;
			part->m_ref_xi = upart->m_ref_xi;
			part->m_ref_yi = upart->m_ref_yi;
			part->m_ref_angle = upart->m_ref_angle;
			part->m_ref_size = upart->m_ref_size;
			part->m_ref_w = upart->m_ref_w;
			part->value = upart->value;
			part->m_value_vis = upart->m_value_vis;
			part->m_value_xi = upart->m_value_xi;
			part->m_value_yi = upart->m_value_yi;
			part->m_value_angle = upart->m_value_angle;
			part->m_value_size = upart->m_value_size;
			part->m_value_w = upart->m_value_w;
			pl->m_dlist = old_dlist;	// turn drawing back on;
			pl->DrawPart( part );
			pl->m_nlist->PartAdded( part );
		}
		else if( part && type == UNDO_PART_MODIFY )
		{
			// part was moved or modified
			pl->UndrawPart( part );
			pl->m_dlist = NULL;		// prevent further drawing
			if( upart->shape != part->shape )
			{
				// footprint was changed
				CString sP = "_";
				pl->PartFootprintChanged( part, upart->shape, &sP );
			}
			if( upart->x != part->x
				|| upart->y != part->y 
				|| upart->angle != part->angle 
				|| upart->side != part->side )
			{
				pl->Move( part, upart->x, upart->y, upart->angle, upart->side );
				pl->m_nlist->PartMoved( part , TRUE );
			}
			part->m_merge = upart->merge_p;
			part->sub_merge = upart->submerge_p;
			part->glued = upart->glued; 
			part->m_ref_vis = upart->m_ref_vis;
			part->m_ref_xi = upart->m_ref_xi;
			part->m_ref_yi = upart->m_ref_yi;
			part->m_ref_angle = upart->m_ref_angle;
			part->m_ref_size = upart->m_ref_size;
			part->m_ref_w = upart->m_ref_w;
			part->value = upart->value;
			part->m_value_vis = upart->m_value_vis;
			part->m_value_xi = upart->m_value_xi;
			part->m_value_yi = upart->m_value_yi;
			part->m_value_angle = upart->m_value_angle;
			part->m_value_size = upart->m_value_size;
			part->m_value_w = upart->m_value_w;
			char * chptr = (char*)ptr + sizeof( undo_part );
			if( part->shape )
			{
				for( int ip=0; ip<part->shape->GetNumPins(); ip++ )
				{
					if( *chptr != 0 )
					{
						CString net_name = chptr;
						cnet * net = pl->m_nlist->GetNetPtrByName( &net_name );
						part->pin[ip].net = net;
					}
					else
						part->pin[ip].net = NULL;
					chptr += MAX_NET_NAME_SIZE + 1;
				}
			}
			// if part was renamed
			if( new_ref_des != old_ref_des )
			{
				int d = pl->m_nlist->PartRefChanged( &new_ref_des, &old_ref_des );
				if( d == 0 )
					part->ref_des = old_ref_des;
				else
					ASSERT(0);
			}
			pl->m_dlist = old_dlist;	// turn drawing back on
			pl->DrawPart( part );
		}
	}
	free(ptr);	// dele			}he undo record
}

// check partlist for errors
//
int CPartList::CheckPartlist( CString * logstr )
{
	int nerrors = 0;
	int nwarnings = 0;
	CString str;
	CMapStringToPtr map;
	void * ptr;

	*logstr += "***** Checking Parts *****\r\n";

	// first, check for duplicate parts
	cpart * part = m_start.next;
	while( part->next != 0 )
	{
		CString ref_des = part->ref_des;
		BOOL test = map.Lookup( ref_des, ptr );
		if( test )
		{
			str.Format( "ERROR: Part \"%s\" duplicated\r\n", ref_des );
			str += "    ###   To fix this, delete one instance of the part, then save, close and re-open project\r\n";
			*logstr += str;
			nerrors++;
		}
		else
			map.SetAt( ref_des, NULL );

		// next part
		part = part->next;
	}

	// now check all parts
	part = m_start.next;
	while( part->next != 0 )
	{
		// check this part
		str = "";
		CString * ref_des = &part->ref_des;
		if( !part->shape )
		{
			// no footprint
			str.Format( "Warning: Part \"%s\" has no footprint\r\n",
				*ref_des );
			nwarnings++;
		}
		else
		{
			for( int ip=0; ip<part->pin.GetSize(); ip++ )
			{
				// check this pin
				cnet * net = part->pin[ip].net;
				CString * pin_name = &part->shape->m_padstack[ip].name;
				if( !net )
				{
					// part->pin->net is NULL, pin unconnected
					// this is not an error
					//				str.Format( "%s.%s unconnected\r\n",
					//					*ref_des, *pin_name );
				}
				else
				{
					cnet * netlist_net = m_nlist->GetNetPtrByName( &net->name );
					if( !netlist_net )
					{
						// part->pin->net->name doesn't exist in netlist
						str.Format( "ERROR: Part \"%s\" pin \"%s\" connected to net \"%s\" which doesn't exist in netlist\r\n",
							*ref_des, *pin_name, net->name );
						nerrors++;
					}
					else
					{
						if( net != netlist_net )
						{
							// part->pin->net doesn't match netlist->net
							str.Format( "ERROR: Part \"%s\" pin \"%s\" connected to net \"%s\" which doesn't match netlist\r\n",
								*ref_des, *pin_name, net->name );
							nerrors++;
						}
						else
						{
							// try to find pin in pin list for net
							int net_pin = -1;
							for( int ip=0; ip<net->npins; ip++ )
							{
								if( net->pin[ip].part == part )
								{
									if( net->pin[ip].pin_name == *pin_name )
									{
										net_pin = ip;
										break;
									}
								}
							}
							if( net_pin == -1 )
							{
								// pin not found
								str.Format( "ERROR: Part \"%s\" pin \"%s\" connected to net \"%\" but pin not in net\r\n",
									*ref_des, *pin_name, net->name );
								nerrors++;
							}
							else
							{
								// OK
							}

						}
					}
				}
			}
		}
		*logstr += str;

		// next part
		part = part->next;
	}
	str.Format( "***** %d ERROR(S), %d WARNING(S) *****\r\n", nerrors, nwarnings );
	*logstr += str;

	return nerrors;
}

void CPartList::MoveOrigin( int x_off, int y_off )
{
	cpart * part = GetFirstPart();
	while( part )
	{
		if( part->shape )
		{
			// move this part
			UndrawPart( part );
			part->x += x_off;
			part->y += y_off;
			for( int ip=0; ip<part->pin.GetSize(); ip++ )
			{
				part->pin[ip].x += x_off;
				part->pin[ip].y += y_off;
			}
			DrawPart( part );
		}
		part = GetNextPart(part);
	}
}

BOOL CPartList::CheckForProblemFootprints()
{
	BOOL bHeaders_28mil_holes = FALSE;   
	cpart * part = GetFirstPart();
	while( part )
	{
		if( part->shape)
		{
			if( part->shape->m_name.Right(7) == "HDR-100" 
				&& part->shape->m_padstack[0].hole_size == 28*NM_PER_MIL )
			{
				bHeaders_28mil_holes = TRUE;
			}
		}
		part = GetNextPart( part );
	}
	if( g_bShow_header_28mil_hole_warning && bHeaders_28mil_holes )   
	{
		CDlgMyMessageBox dlg;
		dlg.Initialize( "WARNING: You are loading footprint(s) for through-hole headers with 100 mil pin spacing and 28 mil holes.\n\nThese may be from an obsolete version of the library \"th_header.fpl\" with holes that are too small for standard parts. Please check your design." );
		dlg.DoModal();
		g_bShow_header_28mil_hole_warning = !dlg.bDontShowBoxState;
	}
	return bHeaders_28mil_holes;
}






// 
void CPartList::FindNetPointForPart( cpart * p, CPoint * pp, int * np, cpart * found_p, CPoint * found_pp, int * n_pins_group, int iMODE )
{
	(*found_pp).x = 0;
	(*found_pp).y = 0;
	*n_pins_group = 0;
	int MARK = 1;
	int m_n_pins = 0;
	int n_fp_pins = 0;
	double x_sum=0, y_sum=0, x_sum_fp=0, y_sum_fp=0, Smax=0;
	for(cpart* gp=GetFirstPart(); gp; gp=GetNextPart(gp))
		if( gp->selected && gp->utility && p != gp )
		{
			// reset flags
			for( int ip=gp->shape->GetNumPins()-1; ip>=0; ip-- )
				gp->pin[ip].utility = 0;
			for( int ip=p->shape->GetNumPins()-1; ip>=0; ip-- )
				p->pin[ip].utility = 0;
			RECT gr;
			GetPartBoundingRect(gp,&gr);
			double Sgp = (double)((double)gr.right-(double)gr.left)*(double)((double)gr.top-(double)gr.bottom);
			int sum_pins = 0;
			double sum_x=0, sum_y=0;
			for( int pins=p->shape->GetNumPins()-1; pins>=0; pins-- )
			{
				if( !p->pin[pins].net )
					continue;	
				if( !p->pin[pins].net->visible )
					continue;	
				for( int gpins=gp->shape->GetNumPins()-1; gpins>=0; gpins-- )
				{
					if( !gp->pin[gpins].net )
						continue;
					if( !gp->pin[gpins].net->visible )
						continue;
					if( p->pin[pins].net == gp->pin[gpins].net )
					{	
						if( !gp->pin[gpins].utility )
						{
							sum_x += gp->pin[gpins].x;
							sum_y += gp->pin[gpins].y;
							sum_pins++;
							gp->pin[gpins].utility = TRUE;
						}
						if( !p->pin[pins].utility )
						{			
							x_sum_fp += p->pin[pins].x;
							y_sum_fp += p->pin[pins].y;
							n_fp_pins++;
							p->pin[pins].utility = TRUE;
							
						}
					}
				}
			}
			(*n_pins_group) += sum_pins;

			if( iMODE == 0 )
			{
				m_n_pins += sum_pins;
				x_sum += sum_x;
				y_sum += sum_y;
			}
			if( sum_pins > m_n_pins || ( sum_pins == m_n_pins && Sgp > Smax ) )
			{
				found_p = gp;
				Smax = Sgp;			
				if( iMODE )
				{
					m_n_pins = sum_pins;
					x_sum = sum_x;
					y_sum = sum_y;
				}
			}
		}

	if( m_n_pins )
	{
		(*found_pp).x = x_sum/(double)m_n_pins;
		(*found_pp).y = y_sum/(double)m_n_pins;
		(*pp).x = x_sum_fp/(double)n_fp_pins;
		(*pp).y = y_sum_fp/(double)n_fp_pins;
		(*np) = n_fp_pins;
	}
	else
	{
		(*found_pp).x = 0;
		(*found_pp).y = 0;
		(*pp).x = 0;
		(*pp).y = 0;
		(*np) = 0;
	}
}









void CPartList::OptimizeRatlinesOnPin( cnet * pinnet, int innet )
{
	if( !pinnet )
		return;
	cpart * prt = pinnet->pin[innet].part;
	if( !prt->shape )
		return;
	// 1) removing old ratlines
	for( int ic=0; ic<pinnet->nconnects; ic++ )
	{
		if( pinnet->connect[ic].end_pin == innet )
		{
			if( pinnet->pin[pinnet->connect[ic].start_pin].utility )
				return;
			else if( pinnet->connect[ic].locked )
			{	
			}
			else if( pinnet->connect[ic].nsegs == 1 )
			{
				if( pinnet->connect[ic].seg[0].layer == LAY_PIN_NAME ) 
					m_nlist->RemoveNetConnect( pinnet, ic, false );
			}
			else if( pinnet->connect[ic].nsegs > 1 )
			{	
				if( pinnet->connect[ic].seg[pinnet->connect[ic].nsegs-1].layer == LAY_PIN_NAME ) 
				{
					pinnet->connect[ic].seg.SetSize( pinnet->connect[ic].nsegs-1 );
					pinnet->connect[ic].vtx.SetSize( pinnet->connect[ic].nsegs );
					pinnet->connect[ic].nsegs--;
					pinnet->connect[ic].end_pin = cconnect::NO_END;
					pinnet->connect[ic].end_pin_shape = cconnect::NO_END;
				}
			}
		}
		else if( pinnet->connect[ic].start_pin == innet )
		{
			if( pinnet->connect[ic].end_pin == cconnect::NO_END )
			{
				if( pinnet->connect[ic].locked )
				{
				}
				if( pinnet->connect[ic].nsegs == 1 )
				{
					if( pinnet->connect[ic].vtx[1].tee_ID && pinnet->connect[ic].seg[0].layer == LAY_PIN_NAME )
					{
						int tee = pinnet->connect[ic].vtx[1].tee_ID;
						pinnet->connect[ic].vtx[1].tee_ID = 0;
						// find branches
						int tc=-1, tv=-1;
						if( m_nlist->FindTeeVertexInNet( pinnet, tee, &tc, &tv ) == 0 )
						{
							tc = 0;
							tv = 0;
							// find header
							m_nlist->FindTeeVertexInNet( pinnet, tee, &tc, &tv );
							if( Colinear(	pinnet->connect[tc].vtx[tv-1].x,
											pinnet->connect[tc].vtx[tv-1].y,
											pinnet->connect[tc].vtx[tv].x,
											pinnet->connect[tc].vtx[tv].y,
											pinnet->connect[tc].vtx[tv+1].x,
											pinnet->connect[tc].vtx[tv+1].y ))
							{
								pinnet->connect[tc].seg.RemoveAt( tv-1 );
								pinnet->connect[tc].vtx.SetSize( tv );
								pinnet->connect[tc].nsegs--;
							}
						}
						m_nlist->RemoveNetConnect( pinnet, ic, false );		
					}
				}
			}
			else if( pinnet->pin[pinnet->connect[ic].end_pin].utility )
				return;
			else if( pinnet->connect[ic].locked )
			{		
			}
			else if( pinnet->connect[ic].nsegs == 1 )
			{	
				if( pinnet->connect[ic].seg[0].layer == LAY_PIN_NAME ) 
					m_nlist->RemoveNetConnect( pinnet, ic, false );
			}
		}
	}
	// 2) test on contact to pin with utility
#define MAX_BRANCHES 5000
	int con[MAX_BRANCHES];
	int vtx[MAX_BRANCHES];
	int caret=0;
	con[caret] = -1;
	vtx[caret] = -1;
	do
	{
		for(int ic=con[caret]+1; ic<pinnet->nconnects; ic++ )
		{
			if( pinnet->connect[ic].start_pin == innet || pinnet->connect[ic].end_pin == innet )
			{
				for( int iv=vtx[caret]+1; iv<=pinnet->connect[ic].nsegs; iv++ )
				{
					if( pinnet->connect[ic].vtx[iv].tee_ID )
					{
						if( iv == pinnet->connect[ic].nsegs )
						{
							// find header
							int tc=0, tv=0;
							int find = m_nlist->FindTeeVertexInNet( pinnet, pinnet->connect[ic].vtx[iv].tee_ID, &tc, &tv );
							if( find )
							{

							}
						}
						else
						{

						}
					}
				}
			}
			if( caret )
			{
				caret--;
				break;
			}
		}
	}while( con[0] >= 0 );
	// 3) test on contact to areas
	// 4) find pin with utility
}