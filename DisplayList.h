// DisplayList.h : header file for CDisplayList class
//
#pragma once 
#include <afxcoll.h>
#include <afxtempl.h>
#include "afxwin.h"
#include "ids.h"
#include "layers.h"

//#define DL_MAX_LAYERS 32
#define DL_MAGIC		2674
#define EL_STATIC		1
#define EL_NON_STATIC	0
#define TRANSPARENT_BLACK_GROUND	LAY_VISIBLE_GRID
#define TRANSPARENT_HILITE			LAY_HILITE
#define TRANSPARENT_BACKGND			LAY_BACKGND
#define TRANSPARENT_LAYER			m_active_layer
#define HILITE_SHIFT 10
#define SWITCH_LAYER				LAY_PIN_LINE

//#define PCBU_PER_WU		25400	// conversion from PCB units to world units 
#define BMP 1
#define JPEG 2
struct CTemplate
{
	int m_source;
	LPPICTURE lp;
	CBitmap bm;
	RECT m_rect;
	int m_vis;
	int m_style;
	int m_format;
	int m_layer;
	int m_marker;
};

// graphics element types
enum 
{
	DL_NONE = 0,
	DL_LINE,			// line segment with round end-caps		| p[0], p[1]
	DL_ARC_CW,			// arc with clockwise curve				| p[0], p[1]
	DL_ARC_CCW,			// arc with counter-clockwise curve		| p[0], p[1]
	DL_HOLE,			// hole, shown as circle				| none
	DL_DONUT,			// annulus
	DL_CIRC,			// filled circle						| none
	DL_RECT,			// filled rectangle						| none	
	DL_RRECT,			// filled rounded rectangle				| none	
	DL_HOLLOW_LINE,		// line contour							| p[0], p[1], p[2]... p[n] - contour
	DL_HOLLOW_CIRC,		// circle outline						| none
	DL_HOLLOW_RECT,		// rectangle outline					| none
	DL_HOLLOW_RRECT,	// contour of rounded rectangle			| none
	DL_POLYLINE,		// outline								| p[0]... p[n] - contour
	DL_POLYGON,			// filled								| p[0]... p[n] - contour
	DL_RECT_X,			// rectangle outline with X				| none or p[0]...p[3]
	DL_CURVE_CW,		// circular clockwise curve				| p[0], p[1]
	DL_CURVE_CCW,		// circular counter-clockwise curve		| p[0], p[1]
	DL_CENTROID,		// circle and X							| p[0]==arrow position
	DL_X,				// X									| none
	DL_LINES_ARRAY		// Array lines							| p[0]... p[n] - lines
};

// dragging line shapes
enum
{
	DS_NONE = 0,
	DS_LINE_VERTEX,			// vertex between two lines
	DS_LINE,				// line
	DS_ARC_STRAIGHT,		// straight line (used when drawing polylines)
	DS_ARC_CW,				// clockwise arc (used when drawing polylines)
	DS_ARC_CCW,				// counterclockwise arc 
	DS_CURVE_CW,			// clockwise curve
	DS_CURVE_CCW,			// counterclockwise curve 
	DS_SEGMENT				// line segment (with rubber banding of linking segments)
};

// styles of line segment when dragging line or line vertex
enum
{
	DSS_STRAIGHT = 100,		// straight line
	DSS_ARC_CW,				// clockwise arc
	DSS_ARC_CCW,			// counterclockwise arc
	DSS_CURVE_CW,			// clockwise curve
	DSS_CURVE_CCW,			// counterclockwise curve
	DSS_NONE				// piece is missing (used in Move Segment for missing final segment)
};

// inflection modes for DS_LINE and DS_LINE_VERTEX
enum
{
	IM_NONE = 0,
	IM_90_45,		 
	IM_45_90,
	IM_90
};


class CDisplayList;
class CPolyLine;

// this structure contains an element of the display list
class dl_element
{
	friend class CDisplayList;
public:
	CDisplayList * dlist;
	dl_element * prev;
	dl_element * next;
	dl_element * next_HLT;
	id id;			// identifier (see ids.h)
	void * ptr;		// pointer to object drawing this element
	int gtype;		// type of primitive
	int visible;	// 0 to hide
	int el_static;	// for elements on highlight layer
	int half_tone;	// for hatch elements
	int transparent;	// transparent
	int el_w;			// width
	int layer;			// layer to draw on
						// for elements on highlight layer, 
						// the original layer, the highlight will
						// only be drawn if this layer is visible
private:
	RECT rect;			// element bounds
	CArray<CPoint> pts; // data
};

class CDisplayList
{
private:
	// display-list parameters for each layer
	dl_element m_start[MAX_LAYERS];
	dl_element m_end[MAX_LAYERS];
	int m_rgb[MAX_LAYERS][3];	// layer colors
	int m_layer_in_order[MAX_LAYERS];	// array of layers in draw order
	int m_order_for_layer[MAX_LAYERS];	// draw order for each layer 
	int m_top_layer;
	int m_grid_style; // 1-lines 0-points

	// window parameters
	CRect m_client_r;	// client rect (pixels)
	CRect m_screen_r;	// client rect (screen coords)
	int m_pane_org_x;	// left border of drawing pane (pixels)
	int m_pane_org_y;	// bottom border of drawing pane (pixels)
	int m_bottom_pane_h;	// height of bottom pane
	CDC * botDC;		// pointer to bottom DC
	CDC * topDC;

	int w_ext_x, w_ext_y;	// window extents (world units)
	int v_ext_x, v_ext_y;	// viewport extents (pixels)
	double m_wu_per_pixel_x;	// ratio w_ext_x/v_ext_x (world units per pixel)
	double m_wu_per_pixel_y;	// ratio w_ext_y/v_ext_y (world units per pixel)
	double m_pcbu_per_pixel_x;
	double m_pcbu_per_pixel_y;

	// general dragging parameters
	int m_drag_angle;	// angle of rotation of selection rectangle (starts at 0)
	int m_drag_side;	// 0 = no change, 1 = switch to opposite

	// parameters for dragging polyline sides and trace segments
	// that can be modified while dragging
	int m_drag_flag;		// 1 if dragging something
	int m_drag_shape;		// shape 
	int m_drag_start;
	int m_last_drag_shape;	// last shape drawn
	int m_drag_x;			// last cursor position for dragged shape
	int m_drag_y;		  
	int m_drag_xb;			// start of rubberband drag line "before" selected line
	int m_drag_yb;		
	int m_drag_xi;			// start of rubberband drag line
	int m_drag_yi;		
	int m_drag_xf;			// end of rubberband drag line
	int m_drag_yf;		 
	int m_drag_xe;			// start of rubberband drag line at end of trio
	int m_drag_ye;		
	int m_drag_layer_0;		// line layer
	int m_drag_w0;			// line width
	int m_drag_style0;		// line style
	int m_drag_layer_1;		// line layer
	int m_drag_w1;			// line width
	int m_drag_style1;		// line style
	int m_inflection_mode;	// inflection mode
	int m_last_inflection_mode;	// last mode drawn
	// extra parameters when dragging vertex between 2 line segments
	int m_drag_style2;	
	int m_drag_layer_2;	
	int m_drag_w2;
	// parameters used to draw leading via if necessary
	int m_drag_layer_no_via;	
	int m_drag_via_w;		
	int m_drag_via_holew;	
	int m_drag_via_drawn;	

	// arrays of lines and ratlines being dragged
	// these can be rotated and flipped while being dragged
	int m_drag_layer;				// layer
	int m_drag_max_lines;			// max size of array for line segments
	int m_drag_num_lines;			// number of line segments to drag
	CPoint * m_drag_line_pt;		// array of relative coords for line endpoints
	int m_drag_max_ratlines;		// max size of ratline array
	int m_drag_num_ratlines;		// number of ratlines to drag
	CPoint * m_drag_ratline_start_pt;	// absolute coords for ratline start points 
	CPoint * m_drag_ratline_end_pt;		// relative coords for ratline endpoints
	int m_drag_ratline_width;
	int m_highlight_mode;

	// cursor parameters
	int m_cross_hairs;	// 0 = none, 1 = cross-hairs, 2 = diagonals
	CPoint m_cross_left, m_cross_right, m_cross_top, m_cross_bottom; // end-points
	CPoint m_cross_topleft, m_cross_topright, m_cross_botleft, m_cross_botright;

	// grid   
	int m_visual_grid_on;
	double m_visual_grid_spacing;	// in world units	

	// images
	CArray<CTemplate*> m_tmp;
	CArray<CString> m_src;
public:
	int m_show_pictures;
	double m_scale;		// world units per pixel
	int m_org_x;		// world x-coord of left side of screen (world units)
	int m_org_y;		// world y-coord of bottom of screen (world units)
	int m_max_x;		// world x_coord of right side of screen (world units)
	int m_max_y;		// world y_coord of top of screen (world units)
	//int bDrawBegin;		// break if mouse wheel was used	
	int bDrawBreak;		// break if mouse wheel was used
	CDisplayList();
	~CDisplayList();
	BOOL m_vis[MAX_LAYERS];		// layer visibility flags
	BOOL ProcessMessages();
	int GetGridStyle(){ return m_grid_style; }
	void SetGridStyle( int gs ){ m_grid_style = gs; }
	int GetTopLayer(){ return m_top_layer; }
	void SetTopLayer( int tl ){ m_top_layer = tl; }
	void SetHighlightMode( int m ){ m_highlight_mode = m; }
	void SetStatic( int m );
	void SetVisibleGrid( BOOL on, double grid );
	void SetMapping( CRect *client_r, CRect *screen_r, int pane_org_x, int pane_bottom_h, double scale, int org_x, int org_y );
	int  GetMapping( CRect *client_r, int *pane_org_x, int *pane_bottom_h, double *pcbu_per_pixel, double *org_x, double *org_y );
	void SetDCToWorldCoords( CDC * pDC, CDC * bDC, CDC * tDC, int pcbu_org_x, int pcbu_org_y );
	void SetLayerRGB( int layer, int r, int g, int b );
	void SetLayerVisible( int layer, BOOL vis );
	void SetLayerDrawOrder( int layer, int order )
			{ m_layer_in_order[order] = layer; m_order_for_layer[layer] = order; };
	dl_element * GetElById( int layer, int id_i );
	dl_element * Add(	void * ptr, id _id, int layer, int gtype, int visible, 
						RECT * el_rect, int el_w, CPoint * p, int np, BOOL convert=TRUE, dl_element * end_element=NULL );
	dl_element * AddSelector( dl_element * el );	
	dl_element * AddSelector(	void * ptr, id _id, int gtype, int visible, 
								RECT * el_rect, int el_w, CPoint * p, int np, int orig_layer );
	dl_element * HighLight( int gtype, RECT * el_rect, int el_w, CPoint * p, int np, int orig_layer );
	dl_element * CopyToLayer( dl_element * src_el, int layer );
	void HighLight( dl_element * el );
	void RemoveAll();
	id Remove( dl_element * element );
	void HLTClear();
	void Draw( CDC * pDC, int draw_layer );
	int CancelHighLight( BOOL IncludeStatic=FALSE );
	void * TestSelect( int x, int y, id * sel_id, int * sel_layer, 
		id * exclude_id = NULL, void * exclude_ptr = NULL, id * include_id = NULL,
		int n_include_ids=1, int MAX_DIST=0, CArray<CPolyLine> * polyarr=NULL );
	int StartDraggingArray( CDC * pDC, int x, int y, int crosshair = 1 );
	int StartDraggingRatLine( CDC * pDC, int x, int y, int xf, int yf, int layer, 
		int w, int crosshair = 1 );
	int StartDraggingRectangle( CDC * pDC, int x, int y, int xi, int yi,
										int xf, int yf, int layer );
	int StartDraggingLineVertex( CDC * pDC, int x, int y, int xi, int yi,
									int xf, int yf,
									int layer1, int layer2, int w1, int w2,
									int style1, int style2,
									int layer_no_via, int via_w, int via_holew, int dir,
									int crosshair );
	int StartDraggingLineSegment( CDC * pDC, int x, int y,
									int xb, int yb,
									int xi, int yi, 
									int xf, int yf,
									int xe, int ye,
									int layer0, int layer1, int layer2,
									int w0,		int w1,		int w2,
									int style0, int style1, int style2,
									int layer_no_via, int via_w, int via_holew, 
									int crosshair );
	int StartDraggingLine( CDC * pDC, int x, int y, int xi, int yi, int layer1, int w,
									int layer_no_via, int via_w, int via_holew,
									int crosshair, int style, int inflection_mode );
	int StartDraggingArc( CDC * pDC, int style, int x, int y, int xi, int yi, 
									int layer, int w, int crosshair );
	void SetDragArcStyle( int style );
	void Drag( CDC * pDC, int x, int y );
	int StopDragging();
	BOOL Dragging_third_segment() { return m_drag_style2 != DSS_NONE; };
	void IncrementDragAngle( CDC * pDC );
	int MakeDragLineArray( int num_lines );
	int MakeDragRatlineArray( int num_ratlines, int width );
	int AddDragLine( CPoint pi, CPoint pf );
	int AddDragRatline( CPoint pi, CPoint pf );
	int GetDragAngle();
	void FlipDragSide( CDC * pDC );
	int GetDragSide();
	void SetUpCrosshairs( int type, int x, int y );
	void SetInflectionMode( int mode ){ m_inflection_mode = mode; };
	CPoint ScreenToPCB( CPoint point );
	CPoint PCBToScreen( CPoint point );
	CPoint WindowToPCB( CPoint point );
	RECT GetWindowRect();
	RECT GetClientRect( BOOL convert=0 );
	RECT GetHighlightedBounds( int * map_orig_layer, int convert = 1 );
	int HighlightAll();
	dl_element * HighlightRect( RECT * R, int orig_layer, int Transparent=0, int DL_SHAPE=DL_RECT );
	RECT HighlightSelRect( int Transparent=0 );
	int GetHighlightMode(){ return m_highlight_mode; }
	void Move( dl_element * el, int dx, int dy );
	int GetLayer( int order ){ return m_layer_in_order[order]; }
	int GetOrder( int layer ){ return m_order_for_layer[layer]; }

	// get element parameters
	dl_element * Get_Start( int lay );
	dl_element * Get_End( int lay );
	RECT * Get_Rect( dl_element * el, RECT * Get );
	CArray<CPoint> * Get_Points( dl_element * el, CPoint * Get, int * npoints, int dx=0, int dy=0 );
	int Get_el_w( dl_element * el );
	int Get_Selected ();
	void Get_Endpoints(CPoint *cpi, CPoint *cpf);

	// bmp pictures
	void Marker( int m=0, int i=-1 );
	void ShowImages( int vis = 1 );
	int  AddSource( CString * bmp );
	int  FindSource( CString * bmp );
	int  AddTemplate( int index_src, int layer, RECT * r = NULL, BOOL bNU = FALSE );
	void SetTemplate( CString * bmp, RECT * r, int i, int layer );
	CString GetSource( int i );
	int  GetSourceIndex( int tmp_index );
	int  GetTemplate(	int i, 
						CString * bmp, 
						RECT * r=NULL, 
						int * style=NULL, 
						int * vis=NULL, 
						int * format=NULL,
						int * marker=NULL );
	void MoveTemplate( RECT * r, int i, int layer );
	void SetTemplateStyle( int style, int i );
	void SetTemplateVis( int vis, int i );
	int  GetNumTemplates();
	int  GetNumSources();
	CSize GetTemplateSZ( int i );
};

