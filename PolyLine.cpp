// PolyLine.cpp ... implementation of CPolyLine class

#include "stdafx.h"
#include "math.h"
#include "PolyLine.h"
#include "FreeSch.h"
#include "utility.h"
#include "layers.h"
#include "php_polygon.h"
#include "php_polygon_vertex.h"


#define pi  3.14159265359
#define DENOM 25400	// to use mils for php clipping
//#define DENOM 1			// to use nm for php clipping
extern CFreeasyApp theApp;
// constructor:
//   dl is a pointer to CDisplayList for drawing graphic elements
//   if dl = NULL, doesn't draw anything but can still hold data
//
CPolyLine::CPolyLine( CDisplayList * dl )
{
	m_dlist = dl;
	Ini();
}

CPolyLine::CPolyLine()
{ 
	m_dlist = NULL;
	Ini();
}

void CPolyLine::Ini()
{
	group = rect(0,0,0,0);
	m_ncorners = 0;
	m_ptr = 0;
	m_hatch = 0;
	m_sel_box = 0;
	m_gpc_poly = new gpc_polygon;//ok
	m_gpc_poly->num_contours = 0;
	m_php_poly = new polygon;//ok
	cont_data.SetSize( 1 );
	cont_data[0].start_c = 0;
	cont_data[0].end_c = -1;
	cont_data[0].rect_cont.left =    INT_MAX;
	cont_data[0].rect_cont.bottom =  INT_MAX;
	cont_data[0].rect_cont.right =	INT_MIN;
	cont_data[0].rect_cont.top =	INT_MIN;
	for( int ind=0; ind<num_attributes; ind++ )
	{
		pAttr[ind] = NULL;
		Next[ind] = -1;
	}
	m_visible = 1;
	m_source = -1;
	bDrawn = 0;
	dl_node1 = NULL;
	dl_node2 = NULL;
}

// destructor, removes display elements
//
CPolyLine::~CPolyLine()
{
	Undraw();
	FreeGpcPoly();
	delete m_gpc_poly;
	delete m_php_poly;
}

// Use the General Polygon Clipping Library to clip contours
// If this results in new polygons, return them as CArray p
// If bRetainArcs == TRUE, try to retain arcs in polys
// Returns number of external contours, or -1 if error
//
int CPolyLine::NormalizeWithGpc( CArray<CPolyLine*> * pa, BOOL bRetainArcs )
{
	CArray<CArc> arc_array;

	if( bRetainArcs )
		MakeGpcPoly( -1, &arc_array );
	else
		MakeGpcPoly( -1, NULL );

	Undraw();
	int src = m_source;
	// now, recreate poly
	// first, find outside contours and create new CPolyLines if necessary
	int n_ext_cont = 0;
	for( int ic=0; ic<m_gpc_poly->num_contours; ic++ )
	{
		if( !(m_gpc_poly->hole)[ic] )
		{
			if( n_ext_cont == 0 )
			{
				// first external contour, replace this poly
				corner.RemoveAll();
				side_style.RemoveAll();
				cont_data.RemoveAll();
				m_ncorners = 0;
				for( int i=0; i<m_gpc_poly->contour[ic].num_vertices; i++ )
				{
					int x = ((m_gpc_poly->contour)[ic].vertex)[i].x;
					int y = ((m_gpc_poly->contour)[ic].vertex)[i].y;
					if( i==0 )
					{
						Start( m_layer, m_w, m_sel_box, x, y, m_hatch, &m_id, m_ptr );
						// restore picture
						m_source = src;
					}
					else
						AppendCorner( x, y, STRAIGHT, FALSE );
				}
				if (m_gpc_poly->num_contours == 1)
					Close();
				else
					Close( STRAIGHT, FALSE, FALSE );
				if( m_source >= 0 )
					m_dlist->MoveTemplate( &GetCornerBounds(), m_source, m_layer );
				n_ext_cont++;
			}
			else if( pa )
			{
				// next external contour, create new poly
				CPolyLine * poly = new CPolyLine;//ok
				pa->SetSize(n_ext_cont);	// put in array
				(*pa)[n_ext_cont-1] = poly;
				for( int i=0; i<m_gpc_poly->contour[ic].num_vertices; i++ )
				{
					int x = ((m_gpc_poly->contour)[ic].vertex)[i].x;
					int y = ((m_gpc_poly->contour)[ic].vertex)[i].y;
					if( i==0 )
					{
						poly->Start( m_layer, m_w, m_sel_box, x, y, m_hatch, &m_id, m_ptr );
						// restore picture
						//poly->m_source = src;
					}
					else
						poly->AppendCorner( x, y, STRAIGHT, FALSE );
				}
				if (ic == (m_gpc_poly->num_contours-1) )
					poly->Close();
				else
					poly->Close( STRAIGHT, FALSE, FALSE );
				//if( poly->m_source )
				//	m_dlist->MoveTemplate( &poly->GetCornerBounds(), poly->m_source );
				n_ext_cont++;
			}
		}
	}


	// now add cutouts to the CPolyLine(s)
	for( int ic=0; ic<m_gpc_poly->num_contours; ic++ ) 
	{
		if( (m_gpc_poly->hole)[ic] )
		{
			CPolyLine * ext_poly = NULL;
			if( n_ext_cont == 1 )
			{
				ext_poly = this;
			}
			else
			{
				// find the polygon that contains this hole
				for( int i=0; i<m_gpc_poly->contour[ic].num_vertices; i++ )
				{
					int x = ((m_gpc_poly->contour)[ic].vertex)[i].x;
					int y = ((m_gpc_poly->contour)[ic].vertex)[i].y;
					if( TestPointInside( x, y ) )
						ext_poly = this;
					else
					{
						for( int ext_ic=0; ext_ic<n_ext_cont-1; ext_ic++ )
						{
							if( (*pa)[ext_ic]->TestPointInside( x, y ) )
							{
								ext_poly = (*pa)[ext_ic];
								break;
							}
						}
					}
					if( ext_poly )
						break;
				}
			}
			if( !ext_poly )
				ASSERT(0);
			for( int i=0; i<m_gpc_poly->contour[ic].num_vertices; i++ )
			{
				int x = ((m_gpc_poly->contour)[ic].vertex)[i].x;
				int y = ((m_gpc_poly->contour)[ic].vertex)[i].y;
				ext_poly->AppendCorner( x, y, STRAIGHT, FALSE );
			}
			if (ic == (m_gpc_poly->num_contours-1) )
				ext_poly->Close();
			else
				ext_poly->Close( STRAIGHT, FALSE, FALSE );
		}
	}
	if( bRetainArcs )
		RestoreArcs( &arc_array, pa );
	FreeGpcPoly();

	return n_ext_cont;
}

// make a php_polygon from first contour
int CPolyLine::MakePhpPoly()
{
	FreePhpPoly();
	polygon test_poly;
	int nv = GetContourEnd(0);
	for( int iv=0; iv<=nv; iv++ )
	{
		int x = GetX(iv)/DENOM;
		int y = GetY(iv)/DENOM;
		m_php_poly->addv( x, y );
	}
	return 0;
}

void CPolyLine::FreePhpPoly()
{
	// delete all vertices
	while( m_php_poly->m_cnt > 1 )
	{
		vertex * fv = m_php_poly->getFirst();
		m_php_poly->del( fv->m_nextV );
	}
	delete m_php_poly->m_first;
	m_php_poly->m_first = NULL;
	m_php_poly->m_cnt = 0;
}

// Use the php clipping lib to clip this poly against poly
//
void CPolyLine::ClipPhpPolygon( int php_op, CPolyLine * poly )
{
	Undraw();
	poly->MakePhpPoly();
	MakePhpPoly();
	polygon * p = m_php_poly->boolean( poly->m_php_poly, php_op );
	poly->FreePhpPoly();
	FreePhpPoly();

	if( p )
	{
		// now screw with the PolyLine
		corner.RemoveAll();
		side_style.RemoveAll();
		cont_data.RemoveAll();
		do
		{
			vertex * v = p->getFirst();
			Start( m_layer, m_w, m_sel_box, v->X()*DENOM, v->Y()*DENOM, m_hatch, &m_id, m_ptr );
			do
			{
				vertex * n = v->Next();
				AppendCorner( v->X()*DENOM, v->Y()*DENOM );
				v = n;
			}
			while( v->id() != p->getFirst()->id() );
			Close();
			delete p;
			p = NULL;
		}
		while( p );
	}
	Draw();
}

// make a gpc_polygon for a closed polyline contour
// approximates arcs with multiple straight-line segments
// if icontour = -1, make polygon with all contours,
// combining intersecting contours if possible
// returns data on arcs in arc_array
//
int CPolyLine::MakeGpcPoly( int icontour, CArray<CArc> * arc_array )
{
	FreeGpcPoly();
	if( !GetClosed() && (icontour == (GetNumContours()-1) || icontour == -1))
		return 1;	// error

	int n_arcs = 0;
	int first_contour = icontour;
	int last_contour = icontour;
	if( icontour == -1 )
	{
		first_contour = 0;
		last_contour = GetNumContours() - 1;
	}
	if( arc_array )
		arc_array->SetSize(0);
	int iarc = 0;
	for( int icont=first_contour; icont<=last_contour; icont++ )
	{
		// make gpc_polygon for this contour
		gpc_polygon * gpc = new gpc_polygon;//ok
		gpc->num_contours = 0;
		gpc->hole = NULL;
		gpc->contour = NULL;

		// first, calculate number of vertices in contour
		int n_vertices = 0;
		int ic_st = GetContourStart(icont);
		int ic_end = GetContourEnd(icont);
		for( int ic=ic_st; ic<=ic_end; ic++ )
		{
			int style = side_style[ic];
			int x1 = corner[ic].x;
			int y1 = corner[ic].y;
			int x2, y2;
			if( ic < ic_end )
			{
				x2 = corner[ic+1].x;
				y2 = corner[ic+1].y;
			}
			else
			{
				x2 = corner[ic_st].x;
				y2 = corner[ic_st].y;
			}
			if( style == STRAIGHT )
				n_vertices++;
			else
			{
				// style is ARC_CW or ARC_CCW
				int n;	// number of steps for arcs
				n = (abs(x2-x1)+abs(y2-y1))/(CArc::MAX_STEP);
				n = max( n, CArc::MIN_STEPS );	// or at most 5 degrees of arc
				n_vertices += n;
				n_arcs++;
			}
		}
		// now create gcp_vertex_list for this contour
		gpc_vertex_list * g_v_list = new gpc_vertex_list;//ok
		g_v_list->num_vertices = n_vertices;
		g_v_list->vertex = (gpc_vertex*)calloc( n_vertices, 2*sizeof(double) );		
		int ivtx = 0;
		for( int ic=ic_st; ic<=ic_end; ic++ )
		{
			int style = side_style[ic];
			int x1 = corner[ic].x;
			int y1 = corner[ic].y;
			int x2, y2;
			if( ic < ic_end )
			{
				x2 = corner[ic+1].x;
				y2 = corner[ic+1].y;
			}
			else
			{
				x2 = corner[ic_st].x;
				y2 = corner[ic_st].y;
			}
			if( style == STRAIGHT )
			{
				g_v_list->vertex[ivtx].x = x1;
				g_v_list->vertex[ivtx].y = y1;
				ivtx++;
			}
			else
			{
				// style is arc_cw or arc_ccw
				int n;	// number of steps for arcs
				n = (abs(x2-x1)+abs(y2-y1))/(CArc::MAX_STEP);
				n = max( n, CArc::MIN_STEPS );	// or at most 5 degrees of arc
				double xo, yo, theta1, theta2, a, b;
				a = fabs( (double)(x1 - x2) );
				b = fabs( (double)(y1 - y2) );
				if( style == CPolyLine::ARC_CW )
				{
					// clockwise arc (ie.quadrant of ellipse)
					int i=0, j=0;
					if( x2 > x1 && y2 > y1 )
					{
						// first quadrant, draw second quadrant of ellipse
						xo = x2;	
						yo = y1;
						theta1 = pi;
						theta2 = pi/2.0;
					}
					else if( x2 < x1 && y2 > y1 )
					{
						// second quadrant, draw third quadrant of ellipse
						xo = x1;	
						yo = y2;
						theta1 = 3.0*pi/2.0;
						theta2 = pi;
					}
					else if( x2 < x1 && y2 < y1 )	
					{
						// third quadrant, draw fourth quadrant of ellipse
						xo = x2;	
						yo = y1;
						theta1 = 2.0*pi;
						theta2 = 3.0*pi/2.0;
					}
					else
					{
						xo = x1;	// fourth quadrant, draw first quadrant of ellipse
						yo = y2;
						theta1 = pi/2.0;
						theta2 = 0.0;
					}
				}
				else
				{
					// counter-clockwise arc
					int i=0, j=0;
					if( x2 > x1 && y2 > y1 )
					{
						xo = x1;	// first quadrant, draw fourth quadrant of ellipse
						yo = y2;
						theta1 = 3.0*pi/2.0;
						theta2 = 2.0*pi;
					}
					else if( x2 < x1 && y2 > y1 )
					{
						xo = x2;	// second quadrant
						yo = y1;
						theta1 = 0.0;
						theta2 = pi/2.0;
					}
					else if( x2 < x1 && y2 < y1 )	
					{
						xo = x1;	// third quadrant
						yo = y2;
						theta1 = pi/2.0;
						theta2 = pi;
					}
					else
					{
						xo = x2;	// fourth quadrant
						yo = y1;
						theta1 = pi;
						theta2 = 3.0*pi/2.0;
					}
				}
				// now write steps for arc
				if( arc_array )
				{
					arc_array->SetSize(iarc+1);
					(*arc_array)[iarc].style = style;
					(*arc_array)[iarc].n_steps = n;
					(*arc_array)[iarc].xi = x1;
					(*arc_array)[iarc].yi = y1;
					(*arc_array)[iarc].xf = x2;
					(*arc_array)[iarc].yf = y2;
					iarc++;
				}
				for( int is=0; is<n; is++ )
				{
					double theta = theta1 + ((theta2-theta1)*(double)is)/n;
					double x = xo + a*cos(theta);
					double y = yo + b*sin(theta);
					if( is == 0 )
					{
						x = x1;
						y = y1;
					}
					g_v_list->vertex[ivtx].x = x;
					g_v_list->vertex[ivtx].y = y;
					ivtx++;
				}
			}
		}
		if( n_vertices != ivtx )
			ASSERT(0);
		// add vertex_list to gpc
		gpc_add_contour( gpc, g_v_list, 0 );
		// now clip m_gpc_poly with gpc, put new poly into result
		gpc_polygon * result = new gpc_polygon;//ok
		if( icontour == -1 && icont != 0 )
			gpc_polygon_clip( GPC_DIFF, m_gpc_poly, gpc, result );	// hole
		else
			gpc_polygon_clip( GPC_UNION, m_gpc_poly, gpc, result );	// outside
		// now copy result to m_gpc_poly
		//
		gpc_free_polygon( m_gpc_poly );
		FreeGpcPoly();
		delete m_gpc_poly;
		m_gpc_poly = result;
		delete g_v_list->vertex;
		delete g_v_list;
		gpc_free_polygon( gpc );
		delete gpc;
	}
	return 0;
}

int CPolyLine::FreeGpcPoly()
{
	if( m_gpc_poly )
		if( m_gpc_poly->num_contours )
			gpc_free_polygon( m_gpc_poly );
	// initialize m_gpc_poly
	m_gpc_poly->num_contours = 0;
	m_gpc_poly->hole = NULL;
	m_gpc_poly->contour = NULL;
	return 0;
}


// Restore arcs to a polygon where they were replaced with steps
// If pa != NULL, also use polygons in pa array
//
int CPolyLine::RestoreArcs( CArray<CArc> * arc_array, CArray<CPolyLine*> * pa )
{
	// get poly info
	int n_polys = 1;
	if( pa )
		n_polys += pa->GetSize();
	CPolyLine * poly;

	// undraw polys and clear utility flag for all corners
	for( int ip=0; ip<n_polys; ip++ )
	{
		if( ip == 0 )
			poly = this;
		else
			poly = (*pa)[ip-1];
		poly->Undraw();
		for( int ic=0; ic<poly->GetNumCorners(); ic++ )
			poly->SetUtility( ic, 0 );	// clear utility flag
	}

	// find arcs and replace them
	BOOL bFound;
	int arc_start;
	int arc_end;
	for( int iarc=0; iarc<arc_array->GetSize(); iarc++ )
	{
		int arc_xi = (*arc_array)[iarc].xi;
		int arc_yi = (*arc_array)[iarc].yi;
		int arc_xf = (*arc_array)[iarc].xf;
		int arc_yf = (*arc_array)[iarc].yf;
		int n_steps = (*arc_array)[iarc].n_steps;
		int style = (*arc_array)[iarc].style;
		bFound = FALSE;
		// loop through polys
		for( int ip=0; ip<n_polys; ip++ )
		{
			if( ip == 0 )
				poly = this;
			else
				poly = (*pa)[ip-1];
			for( int icont=0; icont<poly->GetNumContours(); icont++ )
			{
				int ic_start = poly->GetContourStart(icont);
				int ic_end = poly->GetContourEnd(icont);
				if( (ic_end-ic_start) > n_steps )
				{
					for( int ic=ic_start; ic<=ic_end; ic++ )
					{
						int ic_next = ic+1;
						if( ic_next > ic_end )
							ic_next = ic_start;
						int xi = poly->GetX(ic);
						int yi = poly->GetY(ic);
						if( xi == arc_xi && yi == arc_yi )
						{
							// test for forward arc
							int ic2 = ic + n_steps;
							if( ic2 > ic_end )
								ic2 = ic2 - ic_end + ic_start - 1;
							int xf = poly->GetX(ic2);
							int yf = poly->GetY(ic2);
							if( xf == arc_xf && yf == arc_yf )
							{
								// arc from ic to ic2
								bFound = TRUE;
								arc_start = ic;
								arc_end = ic2;
							}
							else
							{
								// try reverse arc
								ic2 = ic - n_steps;
								if( ic2 < ic_start )
									ic2 = ic2 - ic_start + ic_end + 1;
								xf = poly->GetX(ic2);
								yf = poly->GetY(ic2);
								if( xf == arc_xf && yf == arc_yf )
								{
									// arc from ic2 to ic
									bFound = TRUE; 
									arc_start = ic2;
									arc_end = ic;
									style = 3 - style;
								}
							}
							if( bFound )
							{
								poly->side_style[arc_start] = style;
								// mark corners for deletion from arc_start+1 to arc_end-1
								for( int i=arc_start+1; i!=arc_end; )
								{
									if( i > ic_end )
										i = ic_start;
									poly->SetUtility( i, 1 );
									if( i == ic_end )
										i = ic_start;
									else
										i++;
								}
								break;
							}
						}
						if( bFound )
							break;
					}
				}
				if( bFound )
					break;
			}
		}
		if( bFound )
			(*arc_array)[iarc].bFound = TRUE;
	}

	// now delete all marked corners
	for( int ip=0; ip<n_polys; ip++ )
	{
		if( ip == 0 )
			poly = this;
		else
			poly = (*pa)[ip-1];
		for( int ic=poly->GetNumCorners()-1; ic>=0; ic-- )
		{
			if( poly->GetUtility(ic) )
				poly->DeleteCorner( ic,1, FALSE, FALSE );
		}
		for( int ico=poly->GetNumContours()-1; ico>=0; ico-- )
			poly->RecalcRectC(ico);
		poly->Draw();
	}
	return 0;
}

// initialize new polyline
// set layer, width, selection box size, starting point, id and pointer
//
// if sel_box = 0, don't create selection elements at all
//
void CPolyLine::Start( int layer, int w, int sel_box, int x, int y, 
					  int hatch, id * id, void * ptr, int SIZE )
{
	m_merge = -1;
	m_sub_merge = -1;
	m_layer = layer;
	m_w = max(1,w);
	m_source = -1;
	m_sel_box = max(NM_PER_MIL*3,sel_box/2);
	if( id )
		m_id = *id;
	else
		m_id.Clear();
	m_ptr = ptr;
	m_ncorners = 1;
	m_hatch = hatch;
	if (SIZE<DEF_SIZE)
		SIZE = DEF_SIZE;
	Node.SetSize( SIZE );
	corner.SetSize( SIZE );
	side_style.SetSize( SIZE );
	select_c.SetSize( SIZE );
	select_s.SetSize( SIZE );
	cont_data.SetSize( SIZE/3+1 );
	corner[0].x = x;
	corner[0].y = y;
	side_style[0] = STRAIGHT;
	select_c[0] = 0;
	select_s[0] = 0;
	Node[0] = 0;
	corner[0].num_contour = 0;
	cont_data[0].start_c = 0;
	cont_data[0].end_c = -1;
	cont_data[0].rect_cont.left =	x;
	cont_data[0].rect_cont.bottom = y;
	cont_data[0].rect_cont.right =	x;
	cont_data[0].rect_cont.top =	y;
	//
	for( int ind=0; ind<num_attributes; ind++ )
		pAttr[ind] = NULL;
}

// add a corner to unclosed polyline
//
void CPolyLine::AppendCorner( int x, int y, int style, BOOL bDraw )
{
	if( bDraw )
		Undraw();
	if (!m_ncorners)
		return;
	// increase size of arrays
	if( corner.GetSize() < m_ncorners+1 )
	{
		Node.SetSize( m_ncorners+DEF_ADD );
		corner.SetSize( m_ncorners+DEF_ADD );
		select_c.SetSize( m_ncorners+DEF_ADD );
		select_s.SetSize( m_ncorners+DEF_ADD );
		side_style.SetSize( m_ncorners+DEF_ADD );
	}
	if( cont_data.GetSize() < ((m_ncorners+1)/3+1) )
	{
		cont_data.SetSize( (m_ncorners+DEF_ADD)/3+1 );
	}
	// add entries for new corner and side
	corner[m_ncorners].x = x;
	corner[m_ncorners].y = y;
	side_style[m_ncorners] = style;
	select_c[m_ncorners] = 0;
	select_s[m_ncorners] = 0;
	Node[m_ncorners] = 0;
	corner[m_ncorners].num_contour = corner[m_ncorners-1].num_contour;
	if (cont_data[corner[m_ncorners-1].num_contour].end_c == (m_ncorners-1) )
	{
		(corner[m_ncorners].num_contour)++;
		int icont = corner[m_ncorners].num_contour;
		cont_data[icont].start_c = m_ncorners;
		cont_data[icont].end_c = -1;
		cont_data[icont].rect_cont.left =	x;
		cont_data[icont].rect_cont.right =	x;
		cont_data[icont].rect_cont.bottom = y;
		cont_data[icont].rect_cont.top =	y;	
	}
	else
	{
		side_style[m_ncorners-1] = style;
		int icont = corner[m_ncorners].num_contour;
		cont_data[icont].rect_cont.left =	min( x, cont_data[icont].rect_cont.left );
		cont_data[icont].rect_cont.right =	max( x, cont_data[icont].rect_cont.right );
		cont_data[icont].rect_cont.top =	max( y, cont_data[icont].rect_cont.top );
		cont_data[icont].rect_cont.bottom = min( y, cont_data[icont].rect_cont.bottom );
	}
	m_ncorners++;
	if( bDraw )
		Draw();
}

// close last polyline contour
//
void CPolyLine::Close( int style, BOOL bDraw, BOOL bSetSize )
{
	if( GetClosed() )
		return;
	if( !m_ncorners )
		return;
	Undraw();
	if (bSetSize)
	{
		corner.SetSize( m_ncorners );
		select_c.SetSize( m_ncorners );
		select_s.SetSize( m_ncorners );
		side_style.SetSize( m_ncorners );
		cont_data.SetSize( m_ncorners/3+1 );
	}
	side_style[m_ncorners-1] = style;
	cont_data[corner[m_ncorners-1].num_contour].end_c = m_ncorners-1;
	if( bDraw )
		Draw();
}
void CPolyLine::UnClose()
{
	if( !m_ncorners )
		return;
	Undraw();
	cont_data[corner[m_ncorners-1].num_contour].end_c = -1;
	Draw();
}

void CPolyLine::ModifyGroupRect( RECT * dl )
{
	if( dl )
		group = *dl;
}

void CPolyLine::ModifyGroupRect( int dx, int dy )
{
	group.left += dx;
	group.right += dx;
	group.top += dy;
	group.bottom += dy;
}

void CPolyLine::RecalcRectC( int icont )
{
	cont_data[icont].rect_cont.left =	INT_MAX;
	cont_data[icont].rect_cont.bottom = INT_MAX;
	cont_data[icont].rect_cont.top =	INT_MIN;
	cont_data[icont].rect_cont.right =	INT_MIN;
	int i_max = cont_data[icont].end_c;
	if( i_max == -1 )
		i_max = m_ncorners-1;
	for ( int i=cont_data[icont].start_c; i<=i_max; i++ )
	{
		cont_data[icont].rect_cont.left =	min( corner[i].x, cont_data[icont].rect_cont.left );
		cont_data[icont].rect_cont.bottom = min( corner[i].y, cont_data[icont].rect_cont.bottom );
		cont_data[icont].rect_cont.top =	max( corner[i].y, cont_data[icont].rect_cont.top );
		cont_data[icont].rect_cont.right =	max( corner[i].x, cont_data[icont].rect_cont.right );
	}
}

// move corner of polyline
//
void CPolyLine::MoveCorner( int ic, int x, int y, BOOL DRAW)
{
	if ( DRAW )
		Undraw();
	corner[ic].x = x;
	corner[ic].y = y;
	RecalcRectC(GetNumContour(ic));
	if ( DRAW )
		Draw();
}

// delete corner and adjust arrays
//
void CPolyLine::DeleteCorner( int ic, int nc, BOOL Rec, BOOL bDraw )
{
	if( !nc )
		return;
	if( bDraw )
		Undraw();
	if (ic < 0)
		return;
	int icont = GetContour( ic );
	int istart = GetContourStart( icont );
	int iend = GetContourEnd( icont );
	int num_cntrs = GetNumContours();
	int cl = GetClosed();
	BOOL bClosed = (icont < num_cntrs-1 || cl);
	// if num del corner < iend
	int Switch = ic+nc-iend-1;
	if( Switch <= 0 )
	{
		corner.RemoveAt( ic, nc );
		select_c.RemoveAt( ic, nc ); 
		if( ic < select_s.GetSize() )
			select_s.RemoveAt( ic, nc ); 
		if( !bClosed )
		{
			// open contour, must be last contour		 
			side_style.RemoveAt( max(istart,ic-1), nc );		
			if( cl )
				cont_data[icont].end_c -= nc;	
		}
		else
		{
			// closed contour
			side_style.RemoveAt( ic, nc );
			cont_data[icont].end_c -= nc;
		}
		m_ncorners -= nc;
		for (int c=(icont+1); c<num_cntrs; c++)
		{
			cont_data[c].start_c -= nc;
			cont_data[c].end_c -= nc;
		}
	}
	else 
	// if num del corner > iend
	{
		DeleteCorner( ic, iend-ic+1, 0,0 );
		DeleteCorner( istart, nc-iend+ic-1, 0,0 );
	}

	// delete the entire contour
	if( bClosed && GetContourSize(icont) < 3 && num_cntrs > 1 && icont > 0 )
		RemoveContour( icont );
	else if( Rec )
		RecalcRectC( icont );

	// Draw
	if( bDraw )
		Draw();
}

void CPolyLine::RemoveContour( int icont, BOOL bDraw )
{
	Undraw();
	if (icont < 0)
		return;
	//Get parameters
	int istart = GetContourStart( icont );
	int iend = GetContourEnd( icont );
	int c_c = GetNumContours();
	int size_c = GetContourSize( icont );

	if( icont == 0 && c_c == 1 )
	{
		// remove the only contour
		ASSERT(0);
	}
	else if( icont == (c_c-1) )
	{
		// remove last contour
		int sz = GetContourStart(icont);
		corner.SetSize( sz );
		select_c.SetSize( sz );
		select_s.SetSize( sz );
		side_style.SetSize( sz );
		m_ncorners -= size_c;
	}
	else
	{
		// remove closed contour
		if( iend >= istart )
		{
			corner.RemoveAt( istart, (iend-istart+1) );
			select_c.RemoveAt( istart, (iend-istart+1) );
			select_s.RemoveAt( istart, (iend-istart+1) );
			side_style.RemoveAt( istart, (iend-istart+1) );
			m_ncorners -= (iend-istart+1);
		}
		for( int ic=istart; ic<m_ncorners; ic++ )
		{
			corner[ic].num_contour--;
		}
	}	
	c_c--;
	for ( int ic=icont; ic<c_c; ic++ )
	{
		cont_data[ic].start_c = cont_data[ic+1].start_c - size_c;
		cont_data[ic].end_c = cont_data[ic+1].end_c - size_c;
		cont_data[ic].rect_cont = cont_data[ic+1].rect_cont;
	}
	if( bDraw )
		Draw();
}

// insert a new corner between two existing corners
//
void CPolyLine::InsertCorner( int ic, int x, int y, BOOL bDraw )
{
	if (!m_ncorners)
		return;
	if ( ic >= m_ncorners )
		return;
	int icont;
	icont = corner[ic].num_contour;
	int u = select_s.GetSize();
	int ncont = corner[m_ncorners-1].num_contour + 1;
	//
	Node.InsertAt( ic, 0 );
	corner.InsertAt( ic, CPolyPt(x,y) );
	BOOL b = FALSE;
	side_style.InsertAt( ic, b );
	select_c.InsertAt( ic, b );
	select_s.InsertAt( ic, b );
	m_ncorners++;
	corner[ic].num_contour = icont;
	cont_data[icont].rect_cont.left =	min( x, cont_data[icont].rect_cont.left );
	cont_data[icont].rect_cont.right =	max( x, cont_data[icont].rect_cont.right );
	cont_data[icont].rect_cont.top =	max( y, cont_data[icont].rect_cont.top );
	cont_data[icont].rect_cont.bottom = min( y, cont_data[icont].rect_cont.bottom );
	if( cont_data[icont].end_c >= 0 )
		cont_data[icont].end_c++;
	for (int c=(icont+1); c<ncont; c++)
	{
		cont_data[c].start_c++;
		if( cont_data[c].end_c >= 0 )
			cont_data[c].end_c++;
	}
	if( bDraw )
		Draw();
}

// undraw polyline by removing all graphic elements from display list
//
void CPolyLine::Undraw()
{
	if( m_dlist && bDrawn )
	{
		// remove display elements, if present
		for( int i=dl_side.GetSize()-1; i>=0; i-- )
			m_dlist->Remove( dl_side[i] );
		for( int i=dl_side_sel.GetSize()-1; i>=0; i-- )
			m_dlist->Remove( dl_side_sel[i] );
		for( int i=dl_corner_sel.GetSize()-1; i>=0; i-- )
			m_dlist->Remove( dl_corner_sel[i] );
		for( int i=dl_hatch.GetSize()-1; i>=0; i-- )
			m_dlist->Remove( dl_hatch[i] );
		// remove nodes
		if( dl_node1 )
			m_dlist->Remove( dl_node1 );
		if( dl_node2 )
			m_dlist->Remove( dl_node2 );
		// remove pointers
		dl_side.RemoveAll();
		dl_side_sel.RemoveAll();
		dl_corner_sel.RemoveAll();
		dl_hatch.RemoveAll();
		dl_node1 = NULL;
		dl_node2 = NULL;
		m_nhatch = 0;
	}
	bDrawn = FALSE;
}

// draw polyline by adding all graphics to display list
// if side style is ARC_CW or ARC_CCW but endpoints are not angled,
// convert to STRAIGHT
//
void CPolyLine::Draw(  CDisplayList * dl )
{
	// first, undraw if necessary
	if( bDrawn )
		Undraw(); 

	// the polyline can be hidden
	if( !m_visible )
		return;

	// use new display list if provided
	if( dl )
		m_dlist = dl;
	
	if( m_dlist && m_sel_box )
	{
		dl_element * end_el = m_dlist->GetElById( m_layer, m_id.i );
		if( m_hatch && m_hatch != DOTTED )
			Hatch( end_el );
		// set up CArrays
		if( cont_data[corner[m_ncorners-1].num_contour].end_c < 1 ) // open line
		{
			dl_side.SetSize( m_ncorners - 1 );
			dl_side_sel.SetSize( m_ncorners - 1 );
		}
		else
		{
			dl_side.SetSize( m_ncorners );
			dl_side_sel.SetSize( m_ncorners );
		}
		dl_corner_sel.SetSize( m_ncorners );

		// now draw elements
		int i_start_contour = 0;
		for( int ic=0; ic<m_ncorners; ic++ )
		{
			m_id.ii = ic;
			CPoint P[2];
			P[0].x = corner[ic].x;
			P[0].y = corner[ic].y;
			int ds = 0;
			if((cont_data[corner[ic].num_contour].end_c == ic ) ||
			   (cont_data[corner[ic].num_contour].end_c == -1 && ic == m_ncorners-1 ))
			{
				P[1].x = corner[i_start_contour].x;
				P[1].y = corner[i_start_contour].y;
				i_start_contour = ic+1;
				if( cont_data[corner[ic].num_contour].end_c != -1 || ic < m_ncorners-1 )
					ds = 1;
			}
			else 
			{
				P[1].x = corner[ic+1].x;
				P[1].y = corner[ic+1].y;
				ds = 1;
			}

			if( ds )
			{
				int wid = m_w;
				int g_type = side_style[ic] + 1;
				m_id.sst = ID_SIDE;
				RECT r = rect( P[0].x, P[0].y, P[1].x, P[1].y );
				SwellRect( &r, max(abs(m_w),m_pcbu_per_wu*100) );
				if( m_hatch == DOTTED )
				{
					int sel_type = g_type;
					g_type = DL_LINES_ARRAY;
					float len = Distance( P[0].x, P[0].y, P[1].x, P[1].y );
					float k = len/(float)m_w;
					int npts = k/3 + 2;
					npts -= npts%2;
					CPoint * PT_ARR = new CPoint[npts];//new013
					if( side_style[ic] )
						npts = Generate_Arc( P[0].x, P[0].y, P[1].x, P[1].y, side_style[ic], PT_ARR, npts-1 );
					else
						npts = Generate_Dotted( P[0].x, P[0].y, P[1].x, P[1].y, PT_ARR, npts );
					dl_side[ic] = m_dlist->Add( m_ptr, m_id, m_layer, g_type, 1, &r, max(m_pcbu_per_wu,abs(wid)), PT_ARR, npts, 1, end_el );
					dl_side_sel[ic] = m_dlist->AddSelector( m_ptr, m_id, sel_type, 1, &r, max(m_pcbu_per_wu,abs(wid)), P, 2, m_layer );
					delete PT_ARR;//new013
				}
				else
				{
					dl_side[ic] = m_dlist->Add( m_ptr, m_id, m_layer, g_type, 1, &r, max(m_pcbu_per_wu,abs(wid)), P, 2, 1, end_el );
					dl_side_sel[ic] = m_dlist->AddSelector( m_ptr, m_id, g_type, 1, &r, max(m_pcbu_per_wu,abs(wid)), P, 2, m_layer );
				}
			}
			// draw corner
			m_id.sst = ID_CORNER;
			RECT r;
			if( Node[ic] > NM_PER_MIL )
				r = rect( P[0].x-Node[ic]/2, P[0].y-Node[ic]/2, P[0].x+Node[ic]/2, P[0].y+Node[ic]/2 );
			else
			{
				int p_w = abs(m_w);
				int d = p_w/2 + min(p_w,abs(theApp.m_Doc->m_polyline_w));
				int d_st = d;
				{
					int icp = GetPrevCornerIndex(ic);
					if( icp != ic )
					{
						int dd = Distance( corner[icp].x, corner[icp].y, P[0].x, P[0].y ); 
						if( dd < abs(m_w)*4 && p_w < d )
							d /= 2;
						if( dd < abs(m_w)*2 )
							d /= 2;
					}
				}
				{
					int icn = GetNextCornerIndex(ic);
					if( icn != ic )
					{
						int dd = Distance( corner[icn].x, corner[icn].y, P[0].x, P[0].y ); 
						if( dd < abs(m_w)*4 && d == d_st && p_w < d )
							d /= 2;
						if( dd < abs(m_w)*2 && d >= d_st/2 )
							d /= 2;
					}
				}
				r.left =   P[0].x - d;
				r.right =  P[0].x + d;
				r.top =    P[0].y + d;
				r.bottom = P[0].y - d;
			}
			//
			if( Node[ic] > NM_PER_MIL )
			{
				dl_element * el = m_dlist->Add( m_ptr, m_id, m_layer, DL_CIRC, 1, &r, 0, NULL, 0, 1, end_el );
				if( ic == 0 )
					dl_node1 = el;
				else 
					dl_node2 = el;
				dl_corner_sel[ic] = m_dlist->AddSelector( el );
			}
			else
				dl_corner_sel[ic] = m_dlist->AddSelector( m_ptr, m_id, DL_CIRC, 1, &r, 0, NULL, 0, m_layer );
		}
		bDrawn = TRUE;
	}
	
}

void CPolyLine::SetSideVisible( int is, int visible )
{
	if( m_dlist && dl_side.GetSize() > is )
	{
		dl_side[is]->visible = visible;
	}
}

// start dragging new corner to be inserted into side, make side and hatching invisible
//
void CPolyLine::StartDraggingToInsertCorner( CDC * pDC, int ic, int x, int y, int crosshair )
{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn || !m_visible ) 
		return;
	int icont = GetContour( ic );
	int istart = GetContourStart( icont );
	int iend = GetContourEnd( icont );
	int post_c;

	if( ic == iend )
		post_c = istart;
	else
		post_c = ic + 1;
	int xi = corner[ic].x;
	int yi = corner[ic].y;
	int xf = corner[post_c].x;
	int yf = corner[post_c].y;
	m_dlist->StartDraggingLineVertex( pDC, x, y, xi, yi, xf, yf, 
		LAY_SELECTION, LAY_SELECTION, m_w, m_w, DSS_STRAIGHT, DSS_STRAIGHT,
		0, 0, 0, 0, crosshair );
	m_dlist->HighLight( dl_side[ic] );
	dl_side[ic]->visible = 0;
	for( int ih=0; ih<m_nhatch; ih++ )
		dl_hatch[ih]->visible = 0;
	// make texts, parts and segments invisible
	m_dlist->SetLayerVisible( LAY_HILITE, FALSE );
}

// cancel dragging inserted corner, make side and hatching visible again
//
void CPolyLine::CancelDraggingToInsertCorner( int ic )
{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn || !m_visible ) 
		return;
	int post_c;
	if( ic == (m_ncorners-1) )
		post_c = 0;
	else
		post_c = ic + 1;
	m_dlist->StopDragging();
	dl_side[ic]->visible = 1;
	for( int ih=0; ih<m_nhatch; ih++ )
		dl_hatch[ih]->visible = 1;
	if( theApp.m_Doc->m_vis[LAY_HILITE] )
		m_dlist->SetLayerVisible( LAY_HILITE, TRUE );
}

// start dragging corner to new position, make adjacent sides and hatching invisible
//
void CPolyLine::StartDraggingToMoveCorner( CDC * pDC, int ic, int x, int y, int crosshair )
{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn || !m_visible ) 
		return;
	// see if corner is the first or last corner of an open contour
	int icont = GetContour( ic );
	int istart = GetContourStart( icont );
	int iend = GetContourEnd( icont );
	if( !GetClosed()
		&& icont == GetNumContours() - 1
		&& (ic == istart || ic == iend) )
	{
		// yes
		int style, xi, yi, iside;
		if( ic == istart )
		{
			// first corner
			iside = ic;
			xi = GetX( ic+1 );
			yi = GetY( ic+1 );
			style = GetSideStyle( iside );
			// reverse arc since we are drawing from corner 1 to 0
			if( style == CPolyLine::ARC_CW )
				style = CPolyLine::ARC_CCW;
			else if( style == CPolyLine::ARC_CCW )
				style = CPolyLine::ARC_CW;
			if( dl_node1 )
				dl_node1->visible = 0;
		}
		else
		{
			// last corner
			iside = ic - 1;
			xi = GetX( ic-1 );
			yi = GetY( ic-1);
			style = GetSideStyle( iside );
			if( dl_node2 )
				dl_node2->visible = 0;
		}		
		m_dlist->StartDraggingArc( pDC, style, GetX(ic), GetY(ic), xi, yi, LAY_SELECTION, 1, crosshair );
		m_dlist->HighLight( dl_side[iside] );
		dl_side[iside]->visible = 0;
		for( int ih=0; ih<m_nhatch; ih++ )
			dl_hatch[ih]->visible = 0;
	}
	else
	{
		// no
		// get indexes for preceding and following corners
		int pre_c, post_c;
		int poly_side_style1, poly_side_style2;
		int style1, style2;
		if( ic == istart )
		{
			pre_c = iend;
			post_c = istart+1;
			poly_side_style1 = side_style[iend];
			poly_side_style2 = side_style[istart];
		}
		else if( ic == iend )
		{
			// last side
			pre_c = ic-1;
			post_c = istart;
			poly_side_style1 = side_style[ic-1];
			poly_side_style2 = side_style[ic];
		}
		else
		{
			pre_c = ic-1;
			post_c = ic+1;
			poly_side_style1 = side_style[ic-1];
			poly_side_style2 = side_style[ic];
		}
		if( poly_side_style1 == STRAIGHT )
			style1 = DSS_STRAIGHT;
		else if( poly_side_style1 == ARC_CW )
			style1 = DSS_ARC_CW;
		else if( poly_side_style1 == ARC_CCW )
			style1 = DSS_ARC_CCW;
		if( poly_side_style2 == STRAIGHT )
			style2 = DSS_STRAIGHT;
		else if( poly_side_style2 == ARC_CW )
			style2 = DSS_ARC_CW;
		else if( poly_side_style2 == ARC_CCW )
			style2 = DSS_ARC_CCW;
		int xi = corner[pre_c].x;
		int yi = corner[pre_c].y;
		int xf = corner[post_c].x;
		int yf = corner[post_c].y;
		m_dlist->StartDraggingLineVertex( pDC, x, y, xi, yi, xf, yf, 
			LAY_SELECTION, LAY_SELECTION, m_w, m_w, style1, style2, 
			0, 0, 0, 0, crosshair );
		dl_side[pre_c]->visible = 0;
		dl_side[ic]->visible = 0;
		m_dlist->HighLight( dl_side[pre_c] );
		m_dlist->HighLight( dl_side[ic] );
		for( int ih=0; ih<m_nhatch; ih++ )
			dl_hatch[ih]->visible = 0;
	}
	// make texts, parts and segments invisible
	m_dlist->SetLayerVisible( LAY_HILITE, FALSE );
}

// cancel dragging corner to new position, make sides and hatching visible again
//
void CPolyLine::CancelDraggingToMoveCorner( int ic )
{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn || !m_visible ) 
		return;
	// get indexes for preceding and following sides
	int pre_c;
	if( ic == 0 )
	{
		pre_c = m_ncorners-1;
	}
	else
	{
		pre_c = ic-1;
	}
	m_dlist->StopDragging();
	int gns = GetNumSides();
	if( pre_c < gns )
		dl_side[pre_c]->visible = 1;
	if( ic < gns )
		dl_side[ic]->visible = 1;
	if( dl_node1 )
		dl_node1->visible = 1;
	if( dl_node2 )
		dl_node2->visible = 1;
	for( int ih=0; ih<m_nhatch; ih++ )
		dl_hatch[ih]->visible = 1;
	if( theApp.m_Doc->m_vis[LAY_HILITE] )
		m_dlist->SetLayerVisible( LAY_HILITE, TRUE );
}


// highlight side by drawing line over it
//
void CPolyLine::HighlightSide( int is, int w, int bTRANSPARENT )
{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn ) 
		return;
	if( GetClosed() && is >= m_ncorners )
		return;
	if( !GetClosed() && is >= (m_ncorners-1) )
		return;
	if( !w )
		w = m_w;
	dl_element * el = NULL;
	if( w == m_w )
	{
		m_dlist->HighLight( dl_side[is] );
		el = dl_side[is];
	}
	else
	{
		int n = GetNextCornerIndex(is);
		RECT R = rect( corner[is].x, corner[is].y, corner[n].x, corner[n].y );
		SwellRect( &R, w/2 );
		CPoint P[2];
		P[0].x = corner[is].x;
		P[0].y = corner[is].y;
		P[1].x = corner[n].x;
		P[1].y = corner[n].y;
		el = m_dlist->HighLight( side_style[is]+1, &R, w, P, 2, m_layer );
	}
	if( bTRANSPARENT == 0 )
	{
		el->transparent = 0;
		if( is == 0 )
			if( Node[0] > 1 )
				HighlightCorner(0,1);
		if( is == m_ncorners-2 )
			if( Node[m_ncorners-1] > 1 )
				HighlightCorner( m_ncorners-1, 1 );
	}
	if( bTRANSPARENT )
		el->transparent = bTRANSPARENT;
	else if( w < 0 )
		el->transparent = TRANSPARENT_HILITE;
}

// highlight corner by drawing box around it
//
dl_element * CPolyLine::HighlightCorner( int ic, int w )

{
	if( !m_dlist )
		ASSERT(0);
	if( !bDrawn ) 
		return NULL;
	m_dlist->HighLight( dl_corner_sel[ic] );
	dl_corner_sel[ic]->gtype = DL_CIRC;
	if( w == 0 || w == m_w )
		dl_corner_sel[ic]->transparent = 0;
	else
		dl_corner_sel[ic]->transparent = TRANSPARENT_HILITE;
	return dl_corner_sel[ic];
}

int CPolyLine::GetX( int ic ) 
{	
	return corner[ic].x; 
}

int CPolyLine::GetY( int ic ) 
{	
	return corner[ic].y; 
}

RECT CPolyLine::GetHatchLoc( int index )
{
	if( !bDrawn ) 
		return rect(0,0,0,0);
	RECT line;
	CPoint P[2];
	int np = 2;
	m_dlist->Get_Points( dl_hatch[index], P, &np );
	if( np == 2 )
	{
		line.left =		P[0].x;
		line.bottom =	P[0].y;
		line.right =	P[1].x;
		line.top =		P[1].y;
	}
	return line;
}

int CPolyLine::GetHatchSize ()
{
	return m_nhatch;
}

int CPolyLine::GetMerge( BOOL bSUB )
{
	if( bSUB )
		return m_sub_merge;
	else
		return m_merge;
}

CText * CPolyLine::Check( int Attr )
{
	if( pAttr[Attr] )
		if( pAttr[Attr]->isVISIBLE )
			return pAttr[Attr];
	return NULL;
}

int CPolyLine::GetLength()
{
	int d=0;
	for( int i=0; i<m_ncorners; i++ )
	{
		int n = GetNextCornerIndex(i);
		d += Distance(corner[i].x, corner[i].y, corner[n].x, corner[n].y );
	}
	if( GetClosed() )
		d += Distance(corner[0].x, corner[0].y, corner[m_ncorners-1].x, corner[m_ncorners-1].y );
	return d;
}

int CPolyLine::GetSel()
{
	for( int i=0; i<m_ncorners; i++ )
		if( select_c[i] )
			return 1;
	int nsides = m_ncorners;
	if( GetClosed() == 0 )
		nsides--;
	for( int i=0; i<nsides; i++ )
		if( select_s[i] )
			return 1;
	return 0;
}

int CPolyLine::GetSelCount()
{
	int Count = 0;
	int nsides = select_s.GetSize();
	for( int i=0; i<nsides; i++ )
		if( select_s[i] )
			Count++;
	return Count;
}

int CPolyLine::GetSideSel()
{
	int nsides = m_ncorners;
	if( GetClosed() == 0 )
		nsides--;
	for( int i=0; i<nsides; i++ )
		if( select_s[i] )
			return 1;
	return 0;
}

int CPolyLine::GetSel(int i)
{
	if( i >= select_c.GetSize() || i < 0 )
	{
		ASSERT(0);
	}
	return select_c[i];
}

int CPolyLine::GetSideSel(int i)
{
	if( i >= select_s.GetSize() || i < 0 )
	{
		ASSERT(0);
		return select_s[0];
	}
	return select_s[i];
}

int CPolyLine::UnSelVertices()
{
	int COUNT = 0;
	for( int i=0; i<select_c.GetSize(); i++ )
	{
		if( select_c[i] )
		{
			COUNT++;
			select_c[i] = 0;
		}
	}
	return COUNT;
}

void CPolyLine::SetMerge( int merge, BOOL bSUB )
{
	if( bSUB )
		m_sub_merge = merge;
	else
		m_merge = merge;
}

void CPolyLine::SetSel( int i, BOOL sel )
{
	select_c[i] = sel;
}

void CPolyLine::SetSideSel( int i, BOOL sel )
{
	select_s[i] = sel;
}

int CPolyLine::GetNumContour( int ic ) 
{	
	return corner[ic].num_contour; 
}

int CPolyLine::GetVisible ()
{
	return m_visible;
}

void CPolyLine::MakeVisible( BOOL visible ) 
{	
	if( bDrawn )
	{
		int ns = m_ncorners-1;
		if( GetClosed() )
			ns = m_ncorners;
		for( int is=0; is<ns; is++ )
			dl_side[is]->visible = visible; 
		for( int iv=0; iv<m_ncorners; iv++ )
			dl_corner_sel[iv]->visible = visible; 
		for( int ih=0; ih<m_nhatch; ih++ )
			dl_hatch[ih]->visible = visible; 
		if( dl_node1 )
			dl_node1->visible = 1;
		if( dl_node2 )
			dl_node2->visible = 1;
	}
} 

void CPolyLine::MakeVisible( int is,  BOOL visible ) 
{	
	if( bDrawn )
	{
		int ns = m_ncorners-1;
		int cl = GetClosed();
		if( cl )
			ns = m_ncorners;
		dl_side[min(ns-1,is)]->visible = visible; 
		if( cl == 0 )
		{
			if( is == 0 )
				if( dl_node1 )
					dl_node1->visible = 0;
			if( is == ns-1 )
				if( dl_node2 )
					dl_node2->visible = 0;
		}
		for( int ih=0; ih<m_nhatch; ih++ )
			dl_hatch[ih]->visible = visible; 
	}
} 

RECT CPolyLine::GetBounds()
{
	RECT r = GetCornerBounds(0);
	SwellRect( &r, (abs(m_w)/2)+NM_PER_MIL );
	return r;
}

RECT CPolyLine::GetCornerBounds( int icont )
{
	return cont_data[icont].rect_cont;
}

int CPolyLine::GetNumCorners() 
{	
	return m_ncorners;	
}

int CPolyLine::GetNumArcs()
{
	int n_a = 0;
	for( int i=0; i<GetNumSides(); i++ )
	{
		if(GetSideStyle(i) != STRAIGHT)
			n_a++;
	}
	return n_a;
}

int CPolyLine::GetNumSides() 
{	
	if( GetClosed() )
		return m_ncorners;	
	else
		return max(0,m_ncorners-1);	
}

int CPolyLine::GetLayer() 
{	
	return m_layer;	
}

int CPolyLine::GetW() 
{	
	return m_w;	
}

int CPolyLine::GetPicture()
{
	return m_source;
}

int CPolyLine::GetSelBoxSize() 
{	
	return m_sel_box;	
}

int CPolyLine::GetNumContours()
{
	if( !m_ncorners )
		return 0;
	return corner[m_ncorners-1].num_contour + 1;
}

int CPolyLine::GetContour( int ic )
{
	return corner[ic].num_contour;
}

int CPolyLine::GetContourStart( int icont )
{
	return cont_data[icont].start_c;
}

int CPolyLine::GetContourEnd( int icont )
{
	if ( cont_data[icont].end_c == -1 )
		return (m_ncorners-1);
	else
		return cont_data[icont].end_c;
}

int CPolyLine::GetPrevCornerIndex( int crnr )
{
	if (crnr == cont_data[corner[crnr].num_contour].start_c || crnr == 0 )
	{
		if ( cont_data[corner[m_ncorners-1].num_contour].end_c != (m_ncorners-1) )
			return crnr;
		else if( cont_data[corner[crnr].num_contour].end_c >= 0 )
			return cont_data[corner[crnr].num_contour].end_c;
		else 
			return m_ncorners-1;
	}
	else
		return crnr-1;
}

int CPolyLine::GetNextCornerIndex( int crnr )
{
	if (crnr == cont_data[corner[crnr].num_contour].end_c || crnr == (m_ncorners-1) )
	{
		if ( cont_data[corner[m_ncorners-1].num_contour].end_c != (m_ncorners-1) )
			return crnr;
		else
			return cont_data[corner[crnr].num_contour].start_c;
	}
	else
		return crnr+1;
}

int CPolyLine::GetContourSize( int icont )
{
	return (GetContourEnd(icont) - GetContourStart(icont) + 1);
}


void CPolyLine::SetSideStyle( int is, int style, BOOL DRAW ) 
{	
	if ( DRAW )
		Undraw();
	CPoint p1, p2;
	int icont = GetContour( is );
	int istart = GetContourStart( icont );
	int iend = GetContourEnd( icont );
	p1.x = corner[is].x;
	p1.y = corner[is].y;
	if( is == iend )
	{
		p2.x = corner[istart].x;
		p2.y = corner[istart].y;
	}
	else
	{
		p2.x = corner[is+1].x;
		p2.y = corner[is+1].y;
	}
	if( p1.x == p2.x || p1.y == p2.y )
		side_style[is] = STRAIGHT;
	else
		side_style[is] = style;	
	if ( DRAW )
		Draw();
}

int CPolyLine::GetSideStyle( int is ) 
{	
	if( is >= GetNumSides() )
		return STRAIGHT;
	return side_style[is];	
}

// renumber ids
//
void CPolyLine::SetId( id * id )
{
	m_id = *id;
	if( m_dlist )
	{
		m_id.sst = ID_SIDE;
		int sz = dl_side.GetSize();
		for( int i=0; i<sz-1; i++ )
		{
			m_id.ii = i;
			dl_side[i]->id = m_id;
		}
		if( sz )
			if( dl_side[sz-1] )
				dl_side[sz-1]->id = m_id;
		m_id.sst = ID_CORNER;
		for( int i=0; i<dl_corner_sel.GetSize(); i++ )
		{
			m_id.ii = i;
			dl_corner_sel[i]->id = m_id;
		}
		m_id.sst = ID_HATCH;
		for( int ih=0; ih<dl_hatch.GetSize(); ih++ )
		{
			m_id.ii = ih;
			dl_hatch[ih]->id = m_id; 
		}
	}
}

// get root id
//
id CPolyLine::GetId()
{
	return m_id;
}

int CPolyLine::GetClosed() 
{	
	if( m_ncorners == 0 )
		return 0;
	else
	{
		if ( cont_data[corner[m_ncorners-1].num_contour].end_c == (m_ncorners-1) )
			return 1;
		else
			return 0;
	}
}
void CPolyLine::TransparentHatch( int bTRANSPARENT )
{
	for( int h=dl_hatch.GetSize()-1; h>=0; h-- )
	{
		dl_hatch[h]->transparent = bTRANSPARENT;
		if( m_dlist && bTRANSPARENT )
			m_dlist->HighLight( dl_hatch[h] );
	}
}

void CPolyLine::Show()
{ 
	m_visible = 1; 
	Draw(); 
};
void CPolyLine::Hide()
{ 
	m_visible = 0; 
	//for( int u=0; u<num_attributes; u++ ) not use !
	//	Next[u] = -1;
	if( m_dlist )
		if( m_source >= 0 )
			m_dlist->SetTemplateVis( 0, m_source );
	Undraw(); 
};

// draw hatch lines
//
void CPolyLine::Hatch( dl_element * end_el )
{
	if( m_hatch == NO_HATCH )
	{
		m_nhatch = 0;
		return;
	}

	if( m_dlist && GetClosed() )
	{
		enum {
			MAXPTS = 299,
			MAXLINES = 999
		};
		dl_hatch.SetSize( MAXLINES, MAXLINES );
		int xx[MAXPTS], yy[MAXPTS];

		// define range for hatch lines
		RECT r = GetCornerBounds(0);
		int min_x = r.left;
		int max_x = r.right;
		int min_y = r.bottom;
		int max_y = r.top;
		int slope_flag = 1 - 2*(m_layer%2);	// 1 or -1
		double slope = 0.707106*slope_flag;
		int spacing = max( abs(m_w), NM_PER_MIL/2 );
		int max_a, min_a;
		if( slope_flag == 1 )
		{
			max_a = (int)(max_y - slope*min_x);
			min_a = (int)(min_y - slope*max_x);
		}
		else
		{
			max_a = (int)(max_y - slope*max_x);
			min_a = (int)(min_y - slope*min_x);
		}
		min_a = (min_a/spacing)*spacing;
		int offset;
		if( m_layer < (LAY_ADD_1) )
			offset = 0;
		else if( m_layer < (LAY_ADD_1+2) )
			offset = spacing/2;
		else if( m_layer < (LAY_ADD_1+4) )
			offset = spacing/4;
		else if( m_layer < (LAY_ADD_1+6) )
			offset = 3*spacing/4;
		else if( m_layer < (LAY_ADD_1+8) )
			offset = 1*spacing/8;
		else if( m_layer < (LAY_ADD_1+10) )
			offset = 3*spacing/8;
		else if( m_layer < (LAY_ADD_1+12) )
			offset = 5*spacing/8;
		else if( m_layer < (LAY_ADD_1+14) )
			offset = 7*spacing/8;
		else
		{
			m_layer = LAY_FREE_LINE;
			offset = 0;
		}
		min_a += offset;

		// now calculate and draw hatch lines
		int nc = m_ncorners;
		int nhatch = 0;
		int caret = 0;
		const int GROUPLINES = 200;
		CPoint pts[GROUPLINES];
		// loop through hatch lines
		for( int a=min_a; a<max_a; a+=spacing )
		{
			// get intersection points for this hatch line
			int nloops = 0;
			int npts = 0;
			// make this a loop in case my homebrew hatching algorithm screws up
			do
			{
				npts = 0;
				int i_start_contour = 0;
				for( int ic=0; ic<nc; ic++ )
				{
					double x, y, x2, y2;
					int ok;
					if( cont_data[corner[ic].num_contour].end_c == ic )
					{
						ok = FindLineSegmentIntersection( a, slope, 
								corner[ic].x, corner[ic].y,
								corner[i_start_contour].x, corner[i_start_contour].y, 
								side_style[ic],
								&x, &y, &x2, &y2 );
						i_start_contour = ic + 1;
					}
					else
					{
						ok = FindLineSegmentIntersection( a, slope, 
								corner[ic].x, corner[ic].y, 
								corner[ic+1].x, corner[ic+1].y,
								side_style[ic],
								&x, &y, &x2, &y2 );
					}
					if( ok )
					{
						xx[npts] = (int)x;
						yy[npts] = (int)y;
						npts++;
						if ( npts>=MAXPTS )	// overflow
							break;
					}
					if( ok == 2 )
					{
						xx[npts] = (int)x2;
						yy[npts] = (int)y2;
						npts++;
						if ( npts>=MAXPTS )	// overflow
							break;
					}
				}
				nloops++;
				a += PCBU_PER_MIL/100;
			} while( npts%2 != 0 && nloops < 3 );
			if( npts%2 ) 
				npts--;

			// sort points in order of descending x (if more than 2)
			if( npts>2 )
			{
				for( int istart=0; istart<(npts-1); istart++ )
				{
					int max_x = INT_MIN;
					int imax;
					for( int i=istart; i<npts; i++ )
					{
						if( xx[i] > max_x )
						{
							max_x = xx[i];
							imax = i;
						}
					}
					int temp = xx[istart];
					xx[istart] = xx[imax];
					xx[imax] = temp;
					temp = yy[istart];
					yy[istart] = yy[imax];
					yy[imax] = temp;
				}
			}

			// draw lines
			double wid = max(NM_PER_MIL,m_w);
			double min_dxy = wid*0.007;
			id hatch_id = m_id;
			hatch_id.sst = ID_HATCH;
			for( int ip=0; ip<npts; ip+=2 )
			{	
				if( caret >= GROUPLINES )
				{
					hatch_id.ii = nhatch;
					dl_element * dl = m_dlist->Add( NULL, hatch_id, m_layer, DL_LINES_ARRAY, 1, &r, wid, pts, GROUPLINES, 1, end_el );
					if( m_hatch >= HALF_TONE )
						dl->half_tone = m_hatch;
					dl_hatch.SetAtGrow(nhatch, dl);
					caret = 0;
					nhatch++;
				}			
				double dx = xx[ip+1] - xx[ip];
				double dy = yy[ip+1] - yy[ip];
				if( abs(dx) <= min_dxy || abs(dy) <= min_dxy )
					continue;
				if( Distance( xx[ip], yy[ip], xx[ip+1], yy[ip+1] ) <= wid )
					continue;
				if( caret == 0 )
					r = rect( xx[ip], yy[ip], xx[ip+1], yy[ip+1] );
				else
				{
					SwellRect( &r, xx[ip], yy[ip] );
					SwellRect( &r, xx[ip+1], yy[ip+1] );
				}
				pts[caret].x = xx[ip];
				pts[caret].y = yy[ip];
				pts[caret+1].x = xx[ip+1];
				pts[caret+1].y = yy[ip+1];
				caret += 2;
			}
		} // end for 
		if( caret )
		{
			double wid = max(NM_PER_MIL,m_w);
			double min_dxy = wid*0.01;
			id hatch_id = m_id;
			hatch_id.sst = ID_HATCH;
			hatch_id.ii = nhatch;
			dl_element * dl = m_dlist->Add( NULL, hatch_id, m_layer, DL_LINES_ARRAY, 1, &r, wid, pts, caret, 1, end_el );
			if( m_hatch >= HALF_TONE )
				dl->half_tone = m_hatch;
			dl_hatch.SetAtGrow(nhatch, dl);
			caret = 0;
			nhatch++;
		}	
		m_nhatch = nhatch;
		dl_hatch.SetSize( m_nhatch );
	}
}






//test to see if a point is inside polyline
//Copyright (c) Interpribor TM. All rights reserved.
//Author: Duxah
//Copying is allowed only with reference to the author.
//
BOOL CPolyLine::TestPointInside( int x, int y, int num_c )
{
	float st_angle;
	float back_angle; 
	float diff_angle;
	float angle;
	float SUM = 0.0;
	int start_cor, end_cor;
	if ( num_c >= 0 )
	{
		start_cor = num_c;
		end_cor = num_c;
	}
	else
	{
		start_cor = GetNumContours()-1;
		end_cor = 0;
	}
	for( int icont=start_cor; icont>=end_cor; icont-- )
	{
		if (icont)
		{
			RECT rct = GetCornerBounds(icont);
			if ( rct.left > x )		continue;
			if ( rct.right < x )	continue;
			if ( rct.bottom > y )	continue;
			if ( rct.top < y )		continue;
		}
		int istart = GetContourStart( icont );
		int iend = GetContourEnd( icont );
		st_angle = Angle(corner[istart].x, corner[istart].y, x, y);
		back_angle = st_angle;
		SUM = 0.0;
		for ( int ic=istart; ic<=iend; ic++ )
		{
	        if ( abs(corner[ic].x - x) < 2 && abs(corner[ic].y - y) < 2 )
				return TRUE;
			int prev_ic = GetPrevCornerIndex(ic);
			if (side_style[prev_ic] != CPolyLine::STRAIGHT)
			{
				CPoint arcs[N_SIDES_APPROX_ARC+1];
				int CNT = Generate_Arc(corner[prev_ic].x, corner[prev_ic].y,corner[ic].x, corner[ic].y,side_style[prev_ic],arcs,N_SIDES_APPROX_ARC);
				for (int c=1; c<CNT; c++)
				{
					angle = Angle(arcs[c].x, arcs[c].y, x, y);
					diff_angle = angle - back_angle;
					if      (diff_angle <= -180.0) diff_angle += 360.0;
					else if (diff_angle >= 180.0)  diff_angle -= 360.0;
					SUM += diff_angle;
					back_angle = angle;
				}
			}
			angle = Angle(corner[ic].x, corner[ic].y, x, y);
	        diff_angle = angle - back_angle;
	        if      (diff_angle <= -180.0) diff_angle += 360.0;
	        else if (diff_angle >= 180.0)  diff_angle -= 360.0;
	        SUM += diff_angle;
			back_angle = angle;
		}
		if (side_style[iend] != CPolyLine::STRAIGHT)
		{
			CPoint arcs[N_SIDES_APPROX_ARC+1];
			int CNT = Generate_Arc(corner[iend].x, corner[iend].y,corner[istart].x, corner[istart].y,side_style[iend],arcs,N_SIDES_APPROX_ARC);
			for (int c=1; c<CNT; c++)
			{
				angle = Angle(arcs[c].x, arcs[c].y, x, y);
				diff_angle = angle - back_angle;
				if      (diff_angle <= -180.0) diff_angle += 360.0;
				else if (diff_angle >= 180.0)  diff_angle -= 360.0;
				SUM += diff_angle;
				back_angle = angle;
			}
		}
		diff_angle = st_angle - back_angle;
		if      (diff_angle <= -180.0) diff_angle += 360.0;
		else if (diff_angle >= 180.0)  diff_angle -= 360.0;
		SUM += diff_angle;
		if (icont)
		{
			if (abs(SUM) > 180.0)
			{
				if ( num_c >= 0 )                
					return TRUE;
				else 
					return FALSE;
			}
		}
		else
		{
			if (abs(SUM) < 180.0)                 
				return FALSE;
		}
	}
if ( num_c > 0 )
	return FALSE;
else
	return TRUE;

//Allan version
	/*enum { MAXPTS = 100 };
	if( !GetClosed() )
		ASSERT(0);

	// define line passing through (x,y), with slope = 2/3;
	// get intersection points
	double xx[MAXPTS], yy[MAXPTS];
	double slope = (double)2.0/3.0;
	double a = y - slope*x;
	int nloops = 0;
	int npts;
	// make this a loop so if my homebrew algorithm screws up, we try it again
	do
	{
		// now find all intersection points of line with polyline sides
		npts = 0;
		for( int icont=0; icont<GetNumContours(); icont++ )
		{
			int istart = GetContourStart( icont );
			int iend = GetContourEnd( icont );
			for( int ic=istart; ic<=iend; ic++ )
			{
				double x, y, x2, y2;
				int ok;
				if( ic == istart )
					ok = FindLineSegmentIntersection( a, slope, 
					corner[iend].x, corner[iend].y,
					corner[ic].x, corner[ic].y, 
					side_style[iend],
					&x, &y, &x2, &y2 );
				else
					ok = FindLineSegmentIntersection( a, slope, 
					corner[ic-1].x, corner[ic-1].y, 
					corner[ic].x, corner[ic].y,
					side_style[ic-1],
					&x, &y, &x2, &y2 );
				if( ok )
				{
					xx[npts] = (int)x;
					yy[npts] = (int)y;
					npts++;
					ASSERT( npts<MAXPTS );	// overflow
				}
				if( ok == 2 )
				{
					xx[npts] = (int)x2;
					yy[npts] = (int)y2;
					npts++;
					ASSERT( npts<MAXPTS );	// overflow
				}
			}
		}
		nloops++;
		a += PCBU_PER_MIL/100;
	} while( npts%2 != 0 && nloops < 3 );
	ASSERT( npts%2==0 );	// odd number of intersection points, error

	// count intersection points to right of (x,y), if odd (x,y) is inside polyline
	int ncount = 0;
	for( int ip=0; ip<npts; ip++ )
	{
		if( xx[ip] == x && yy[ip] == y )
			return FALSE;	// (x,y) is on a side, call it outside
		else if( xx[ip] > x )
			ncount++;
	}
	if( ncount%2 )
		return TRUE;
	else
		return FALSE;*/
}

// test to see if a point is inside polyline contour
//
BOOL CPolyLine::TestPointInsideContour( int x, int y )
{
	return TestPointInside(x,y,0);
}





int CPolyLine::TestPolygonIntersection( int test_contour, int test_corner )
{	
	if( GetClosed() == 0 ) // error
		return 0;
	int max_cont = GetNumContours();
	if ( test_contour >= max_cont ) // error
	{
		utility = 0;
		return utility;
	}
	if( test_corner >= 0 || test_contour >= 0 )
		max_cont = min( max_cont,(test_contour+2) );
	BOOL bOut = FALSE;
	BOOL bArcOut = FALSE;
	if( max_cont >= 2 )
	{
		// Set Utility
		for (int i=0; i<GetNumContours(); i++)
			SetUtility(GetContourStart(i),0);
		//
		// check for intersection with any other sides
		int nc = max(0,test_contour);
		int nc2;
		for( int i=GetContourStart(nc); nc<(max_cont-1); i++ )
		{
			RECT r = GetCornerBounds(nc);
			if ( test_contour >= 0 )
				nc2 = 0;
			else
				nc2 = nc + 1;
			if( test_corner >= 0 )
				i = GetPrevCornerIndex(test_corner);
			for( int i2=GetContourStart(nc2); nc2<GetNumContours(); i2++ )
			{
				if( nc != nc2 )
				{
					RECT r2 = GetCornerBounds(nc2);	
					if ( RectsIntersection(r,r2) != -1 )
					{
						BOOL bInt = FALSE;
						int iend;
						if( test_corner >= 0)
							iend = test_corner;
						else
							iend =  GetContourEnd(nc);					
						int iend2 = GetContourEnd(nc2);
						for (int ii=i; ii<=iend; ii++)
						{
							int style = GetSideStyle(ii);
							int x1i = GetX(ii);
							int y1i = GetY(ii);
							int x1f = GetX(GetNextCornerIndex(ii));
							int y1f = GetY(GetNextCornerIndex(ii));
							for (int ii2=i2; ii2<=iend2; ii2++)
							{
								int style2 = GetSideStyle(ii2);
								int x2i = GetX(ii2);
								int y2i = GetY(ii2);
								int x2f = GetX(GetNextCornerIndex(ii2));
								int y2f = GetY(GetNextCornerIndex(ii2));
								if ( min(x1i,x1f) > max(x2i,x2f) ) continue;
								if ( min(x2i,x2f) > max(x1i,x1f) ) continue;
								if ( min(y1i,y1f) > max(y2i,y2f) ) continue;
								if ( min(y2i,y2f) > max(y1i,y1f) ) continue;
								int ret = FindSegmentIntersections( x1i, y1i, x1f, y1f, style, x2i, y2i, x2f, y2f, style2 );
								if( ret )
								{
									// intersection between non-adjacent sides
									bOut = TRUE;
									bInt = TRUE;
									SetUtility(i,1);
									SetUtility(i2,1);
									if( style != CPolyLine::STRAIGHT || style2 != CPolyLine::STRAIGHT )
										bArcOut = TRUE;
									break;
								}
							}
							if ( bInt )
								break;
						}
					}
				}
				i2 = GetContourEnd(nc2);
				nc2++;		
			}
			i = GetContourEnd(nc);
			nc++;
		}
	}
	if( bArcOut )
		utility = -1;
	else if( bOut )
		utility = 1;
	else 
		utility = 0;
	return utility;
}






int CPolyLine::TestPolygonIntersection( CPolyLine * poly2 )
{
	if( !GetClosed() || !poly2->GetClosed() )
		return 0;
	if( GetLayer() != poly2->GetLayer() )
		return 0;

	// test bounding rects
	RECT b1 = GetCornerBounds(0);
	RECT b2 = poly2->GetCornerBounds(0);
	if(    b1.bottom > b2.top
		|| b1.top < b2.bottom
		|| b1.left > b2.right
		|| b1.right < b2.left )
		return 0;

	// now test for intersecting segments
	BOOL bInt = FALSE;
	BOOL bArcInt = FALSE;
	for( int icont1=0; icont1<GetNumContours(); icont1++ )
	{
		b1 = GetCornerBounds(icont1);
		int is1 = GetContourStart( icont1 );
		int ie1 = GetContourEnd( icont1 );
		for( int ic1=is1; ic1<=ie1; ic1++ )
		{
			int xi1 = GetX(ic1);
			int yi1 = GetY(ic1);
			int xf1, yf1, style1;
			if( ic1 < ie1 )
			{
				xf1 = GetX(ic1+1);
				yf1 = GetY(ic1+1);
			}
			else
			{
				xf1 = GetX(is1);
				yf1 = GetY(is1);
			}
			style1 = GetSideStyle( ic1 );
			for( int icont2=0; icont2<poly2->GetNumContours(); icont2++ )
			{
				b2 = poly2->GetCornerBounds(icont2);
				if(    b1.bottom > b2.top
					|| b1.top < b2.bottom
					|| b1.left > b2.right
					|| b1.right < b2.left )
					continue;
				int is2 = poly2->GetContourStart( icont2 );
				int ie2 = poly2->GetContourEnd( icont2 );
				for( int ic2=is2; ic2<=ie2; ic2++ )
				{
					int xi2 = poly2->GetX(ic2);
					int yi2 = poly2->GetY(ic2);
					int xf2, yf2, style2;
					if( ic2 < ie2 )
					{
						xf2 = poly2->GetX(ic2+1);
						yf2 = poly2->GetY(ic2+1);
					}
					else
					{
						xf2 = poly2->GetX(is2);
						yf2 = poly2->GetY(is2);
					}
					style2 = poly2->GetSideStyle( ic2 );
					if ( min(xi1,xf1) > max(xi2,xf2) ) continue;
					if ( min(xi2,xf2) > max(xi1,xf1) ) continue;
					if ( min(yi1,yf1) > max(yi2,yf2) ) continue;
					if ( min(yi2,yf2) > max(yi1,yf1) ) continue;
					int n_int = FindSegmentIntersections( xi1, yi1, xf1, yf1, style1,
									xi2, yi2, xf2, yf2, style2 );
					if( n_int )
					{
						bInt = TRUE;
						if( style1 != CPolyLine::STRAIGHT || style2 != CPolyLine::STRAIGHT )
							bArcInt = TRUE;
						break;
					}
				}
				if( bArcInt )
					break;
			}
			if( bArcInt )
				break;
		}
		if( bArcInt )
			break;
	}
	if( !bInt )
		return 0;
	if( bArcInt )
		return 2;
	return 1;
}

// set selection box size 
//
void CPolyLine::SetSelBoxSize( int sel_box )
{
	m_sel_box = max(NM_PER_MIL*3,sel_box/2);
}

// set pointer to display list, and draw into display list
//
void CPolyLine::SetDisplayList( CDisplayList * dl )
{
	if( dl != m_dlist )
	{
		if( m_dlist )
			Undraw();
		m_dlist = dl;
		///if( m_dlist )
		///	Draw();
	}
}

// copy data from another poly, but don't draw it
//
void CPolyLine::Copy( CPolyLine * src, BOOL AddTemplate, BOOL bAttrs )
{
	Undraw();
	m_visible = src->m_visible;
	if( m_dlist == NULL )
		m_dlist = src->m_dlist;
	m_id = src->m_id;
	m_ptr = src->m_ptr;
	m_layer = src->m_layer;
	m_w = src->m_w;
	m_sel_box = src->m_sel_box;
	m_ncorners = src->m_ncorners;
	m_hatch = src->m_hatch;
	m_nhatch = src->m_nhatch;
	m_merge = src->m_merge;
	m_sub_merge = src->m_sub_merge;
	m_source = src->m_source;

	// copy attribute pointers
	if( bAttrs )
		for( int ind=0; ind<num_attributes; ind++ )
			pAttr[ind] = src->pAttr[ind];

	// copy corners
	corner.SetSize( m_ncorners );
	for( int i=0; i<m_ncorners; i++ )
		corner[i] = src->corner[i];
	select_c.SetSize( m_ncorners );	
	for( int i=0; i<m_ncorners; i++ )
		select_c[i] = src->select_c[i];
	Node.SetSize( m_ncorners );
	for( int i=0; i<m_ncorners; i++ )
		Node[i] = src->Node[i];

	// copy side styles	
	int nsides = src->GetNumSides();
	select_s.SetSize( nsides );
	for( int i=0; i<nsides; i++ )
		select_s[i] = src->select_s[i];
	side_style.SetSize(nsides);
	for( int i=0; i<nsides; i++ )
		side_style[i] = src->side_style[i];
	int ncdata = src->cont_data.GetSize();
	cont_data.SetSize( ncdata );
	for( int i=0; i<ncdata; i++ )
		cont_data[i] = src->cont_data[i];
	//copy picture
	if( src->m_source >= 0 && AddTemplate && m_dlist && src->m_dlist )
	{
		int st = 0;
		int isrc = src->m_dlist->GetSourceIndex(src->m_source);
		if( isrc >= 0 )
		{
			if( src->m_dlist != m_dlist )
				isrc = m_dlist->AddSource( &src->m_dlist->GetSource(isrc) );
			int inew = m_dlist->AddTemplate( isrc, m_layer, &GetCornerBounds(), TRUE );
			src->m_dlist->GetTemplate( src->m_source, NULL, NULL, &st );
			m_dlist->SetTemplateStyle( st, inew );
			m_source = inew;
		}
	}
	// don't copy the Gpc_poly, just clear the old one
	FreeGpcPoly();
}

void CPolyLine::Replace( CPolyLine * src, int byEND )
{
	id oldID = m_id;
	int a1 = 0;
	int a2 = 0;
	if( byEND )
	{
		a1 = m_ncorners-1;
		a2 = (src->m_ncorners)-1;
	}
	int dx = corner[a1].x - src->corner[a2].x;
	int dy = corner[a1].y - src->corner[a2].y;
	CPolyLine::Copy( src, TRUE, FALSE );
	SetId( &oldID );
	for( int i=0; i<m_ncorners; i++ )
	{
		corner[i].x += dx;
		corner[i].y += dy;
	}
	for( int iatt=0; iatt<num_attributes; iatt++ )
	{
		if( pAttr[iatt] && src->pAttr[iatt] )
		{
			pAttr[iatt]->m_x = src->pAttr[iatt]->m_x + dx;
			pAttr[iatt]->m_y = src->pAttr[iatt]->m_y + dy;
			pAttr[iatt]->m_angle = src->pAttr[iatt]->m_angle;
			pAttr[iatt]->m_font_size = src->pAttr[iatt]->m_font_size;
			pAttr[iatt]->m_stroke_width = src->pAttr[iatt]->m_stroke_width;	
			pAttr[iatt]->m_str = src->pAttr[iatt]->m_str;
			pAttr[iatt]->m_nchars = src->pAttr[iatt]->m_nchars;
		}
	}
	RecalcRectC(0);
}

void CPolyLine::MoveOrigin( int x_off, int y_off )
{
	Undraw();
	for( int ic=0; ic<GetNumCorners(); ic++ )
	{
		SetX( ic, GetX(ic) + x_off );
		SetY( ic, GetY(ic) + y_off );
	}
	for( int c=0; c<GetNumContours(); c++ )
	{
		RecalcRectC(c);
	}
	RECT r = GetCornerBounds();
	if( m_source >= 0 )
		m_dlist->MoveTemplate( &r, m_source, m_layer );
	ModifyGroupRect( x_off, y_off );
	Draw();
}


// Set various parameters:
//   the calling function should Undraw() before calling them,
//   and Draw() after
//
void CPolyLine::SetX( int ic, int x ) 
{ 
	corner[ic].x = x; 
	cont_data[corner[ic].num_contour].rect_cont.left =	min( x, cont_data[corner[ic].num_contour].rect_cont.left );
	cont_data[corner[ic].num_contour].rect_cont.right =	max( x, cont_data[corner[ic].num_contour].rect_cont.right );
}
void CPolyLine::SetY( int ic, int y ) 
{ 
	corner[ic].y = y; 
	cont_data[corner[ic].num_contour].rect_cont.bottom = min( y, cont_data[corner[ic].num_contour].rect_cont.bottom );
	cont_data[corner[ic].num_contour].rect_cont.top =	max( y, cont_data[corner[ic].num_contour].rect_cont.top );
}
void CPolyLine::SetEndContour( int ic, BOOL end_contour ) 
{ 
	if ( end_contour )
		cont_data[corner[ic].num_contour].end_c = ic;
}
void CPolyLine::SetLayer( int layer )
{ 
	m_layer = layer; 
	if( m_layer != LAY_PIN_LINE && m_layer != LAY_FREE_LINE && m_layer != LAY_CONNECTION )
	{
		if( m_ncorners )
		{
			Node[0] = 0;
			Node[m_ncorners-1] = 0;
		}
	}
}
void CPolyLine::SetW( int w ) { m_w = max(1,w); }
void CPolyLine::SetVisible ( int visible ){ m_visible = visible; }
void CPolyLine::SetPicture( int src ) { m_source = src; };

void CPolyLine::AppendArc( int xi, int yi, int xf, int yf, int xc, int yc, int num )
{
	// get radius
	double r = sqrt( (double)(xi-xc)*(xi-xc) + (double)(yi-yc)*(yi-yc) );
	// get angles of start and finish
	double th_i = atan2( (double)yi-yc, (double)xi-xc );
	double th_f = atan2( (double)yf-yc, (double)xf-xc );
	double th_d = (th_f - th_i)/(num-1);
	double theta = th_i;
	// generate arc
	for( int ic=0; ic<num; ic++ )
	{
		int x = xc + r*cos(theta);
		int y = yc + r*sin(theta);
		AppendCorner( x, y, STRAIGHT, 0 );
		theta += th_d;
	}
	Close( STRAIGHT );
}


void CPolyLine::ClipGpcPolygon( gpc_op op, CPolyLine * clip_poly )
{
	gpc_polygon * result = new gpc_polygon;//ok
	gpc_polygon_clip( op, m_gpc_poly, clip_poly->GetGpcPoly(), result );
	gpc_free_polygon( m_gpc_poly );
	delete m_gpc_poly;
	m_gpc_poly = result;
}

int CPolyLine::Compare( CPolyLine * comp )
{
	if( m_ncorners != comp->m_ncorners )
		return 0;
	//
	int isSimilar = 1;
	int fDx = 0;
	int fDy = 0;
	BOOL fulltest = ( m_ncorners < 4 || GetClosed() );
	for( int ic=(fulltest?0:1); ic<(fulltest?m_ncorners:m_ncorners-1); ic++ )
	{
		int Dx = (corner[ic].x - comp->corner[ic].x);
		int Dy = (corner[ic].y - comp->corner[ic].y);
		if( fDx == 0 && fDy == 0 )
		{
			fDx = Dx;
			fDy = Dy;
		}
		if( abs(fDx-Dx) > NM_PER_MIL )
			return 0;
		if( abs(fDy-Dy) > NM_PER_MIL )
			return 0;
	}
	for( int i_att=0; i_att<num_attributes; i_att++ )
	{
		if( i_att == index_part_attr )
			continue;
		if( i_att == index_value_attr && pAttr[index_pin_attr] )
			continue;

		if( pAttr[i_att] && comp->pAttr[i_att] == NULL )
			return 0;
		else if( pAttr[i_att] == NULL && comp->pAttr[i_att] )
			return 0;
		else if( pAttr[i_att] )
		{
			if( pAttr[i_att]->m_str.Compare( comp->pAttr[i_att]->m_str ) )
				return 0;
		}
	}
	return isSimilar;
}

void CPolyLine::ApproximateAllArcSides()
{
	if( m_ncorners < 2 )
		return;
	CArray<CArc> arc_array;
	int cls = GetClosed();
	if( cls == 0 )
		Close();
	MakeGpcPoly( -1, &arc_array );
	gpc_polygon * gpcp = GetGpcPoly();
	///Undraw();
	if( gpcp->num_contours )
	{
		for( int icont=GetNumContours()-1; icont>0; icont-- )
			RemoveContour( icont, FALSE );
		Start( m_layer, m_w, m_sel_box, gpcp->contour[0].vertex[0].x, gpcp->contour[0].vertex[0].y, m_hatch, &m_id, m_ptr );
		for( int icont=0; icont<gpcp->num_contours; icont++ )
		{
			for( int iv=0; iv<gpcp->contour[icont].num_vertices; iv++ )
				AppendCorner( gpcp->contour[icont].vertex[iv].x, gpcp->contour[icont].vertex[iv].y, STRAIGHT, FALSE );
			Close( STRAIGHT, FALSE );
		}
	}
	if( cls == 0 )
		UnClose();
	Draw( m_dlist );
}

CString CPolyLine::GetOpenscadPolyPts( int units, int Fn, int tab, CString * h, double scale )
{
	CString S="";
	CPoint * PTS = new CPoint[Fn];
	int cl = (GetClosed() && GetHatch());
	if( cl )
	{
		S="[";
		CString str;
		for( int i=0; i<m_ncorners; i++ )
		{
			double x = corner[i].x;
			double y = corner[i].y;
			double x2 = corner[GetNextCornerIndex(i)].x;
			double y2 = corner[GetNextCornerIndex(i)].y;
			int st = GetSideStyle(i);
			int np = Generate_Arc( x,y,x2,y2,st, PTS, Fn-1 );
			for( int ipt=0; ipt<np; ipt+=2 )
			{
				x = PTS[ipt].x;
				x /= (double)(units==MM?NM_PER_MM:NM_PER_MIL);
				x *= scale;
				y = PTS[ipt].y;
				y /= (double)(units==MM?NM_PER_MM:NM_PER_MIL);
				y *= scale;
				if( S.GetLength() > 1 )
				{
					S += ", ";
					S += "\n";
					for( int j=1; j<tab; j++ )
						S += " ";
				}
				str.Format( "[%.3f, %.3f]", x, y );
				S += str;
			}
		}
		S += "],[[";
		int sh = 0;
		for( int i=0; i<m_ncorners; i++ )
		{
			if( i == 0 )
				str.Format( "%d", i+sh );
			else if( GetNumContour(i) != GetNumContour(i-1) )
				str.Format( "],[%d", i+sh );
			else
				str.Format( ",%d", i+sh );
			S += str;
			if( GetSideStyle(i) )
			{
				for( int ii=1; ii<Fn/2; ii ++ )
				{
					sh++;
					str.Format( ",%d",i+sh );
					S += str;
				}
			}
		}
		str.Format("]], convexity=%d", GetNumContours() );
		S += str;
	}
	else
	{
		CString str;
		for( int i=0; i<(GetClosed()?m_ncorners:m_ncorners-1); i++ )
		{
			double x = corner[i].x;
			double y = corner[i].y;
			double x2 = corner[GetNextCornerIndex(i)].x;
			double y2 = corner[GetNextCornerIndex(i)].y;
			int st = GetSideStyle(i);
			int np = Generate_Arc( x,y,x2,y2,st, PTS, Fn-1 );
			for( int ipt=0; ipt<np-1; ipt++ )
			{
				CPoint OPTS[20];
				int np2 = Gen_HollowLinePoly( PTS[ipt].x, PTS[ipt].y, PTS[ipt+1].x, PTS[ipt+1].y, abs(m_w), OPTS, 20 );
				
				str.Format("  linear_extrude(%s, center=true, convexity=2)\n  polygon([", *h);
				S += str;
				for( int ihp=0; ihp<np2; ihp++ )
				{
					x = OPTS[ihp].x;
					y = OPTS[ihp].y;
					x /= (double)(units==MM?NM_PER_MM:NM_PER_MIL);
					y /= (double)(units==MM?NM_PER_MM:NM_PER_MIL);
					x *= scale;
					y *= scale;
					if( ihp )
					{
						S += ", ";
						S += "\n";
						for( int j=1; j<tab+2; j++ )
							S += " ";
					}
					str.Format( "[%.3f, %.3f]", x, y );
					S += str;
				}
				S += "]);\n";
			}
		}
	}
	delete PTS;
	return S;
}

void CPolyLine::MakeFirst(int ic)
{
	if (ic > 0)
	{
		int cl = 0, st = 0;
		if (GetClosed())
		{
			UnClose();
			cl = 1;
			st = side_style[ic - 1];
		}
		Node[0] = 0;
		Node[m_ncorners - 1] = 0;
		for (int i = 0; i < ic; i++)
			AppendCorner(corner[i].x, corner[i].y, side_style[i ? i - 1 : m_ncorners - 1], 0);
		for (int i = ic - 1; i >= 0; i--)
			DeleteCorner(i);
		if (cl)
			Close(st);
	}
}