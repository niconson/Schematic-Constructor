// FreePcbView.h : interface of the CFreePcbView class
//  
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_FREEPCBVIEW_H__BE1CA173_E2B9_4252_8422_0B9767B01566__INCLUDED_)
#define AFX_FREEPCBVIEW_H__BE1CA173_E2B9_4252_8422_0B9767B01566__INCLUDED_

#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include "DisplayList.h"
#include "Documents.h"   
#include "DlgPartlist.h"
#include "CCompare.h"

#define dSPACE "  "
#define freeasy_netlist "\\related_files\\CDS_netlist\\"
#define rel_f_component "\\related_files\\Component"
#define rel_f_pictures  "\\related_files\\pictures\\"
#define main_component  "\\Component"

class CFreePcbView; 
     
// cursor modes
enum {
	CUR_NONE_SELECTED = 0,		// nothing selected
	CUR_OP_CORNER_SELECTED,		// corner of outline sel.  
	CUR_OP_SIDE_SELECTED,		// edge of outline sel. 
	CUR_TEXT_SELECTED,			// free text selected
	CUR_DRE_SELECTED,			// DRC error selected
	CUR_GROUP_SELECTED,			// multiple parts selected
	CUR_SCALE_FACTOR,
	CUR_NUM_SELECTED_MODES,		// number of SELECTED modes
	// dragging modes
	CUR_DRAG,
	CUR_ADD_OP,		// dragging starting point of outline
	CUR_DRAG_OP_1,	// dragging first corner of outline
	CUR_DRAG_OP,		// dragging next corner of outline
	CUR_DRAG_OP_INSERT,	// dragging corner being inserted
	CUR_DRAG_OP_MOVE,	// dragging corner being moved
	CUR_DRAG_TEXT,		// dragging text box
	CUR_MOVE_ORIGIN,	// dragging new origin
	CUR_DRAG_GROUP,		// dragging a group of parts/segments
	CUR_DRAG_GROUP_ADD,	// dragging a group being added
	CUR_DRAG_MEASURE_1,	// dragging the start of measurement tool
	CUR_DRAG_MEASURE_2,	// dragging the end of measurement tool
	CUR_NUM_MODES		// number of modes
};

// function key options
enum {
	FK_NONE = 0,							
	FK_SIDE,																	
	FK_REDO_RATLINES,					
	FK_MOVE_CORNER,						
	FK_ADD_CORNER,						
	FK_DELETE_CORNER,										
	FK_MOVE_TEXT,						
	FK_ROTATE_TEXT,											
	FK_POLY_STRAIGHT,					
	FK_POLY_ARC_CW,						
	FK_POLY_ARC_CCW,					
	FK_UNDO,							
	FK_SET_SIZE,						
	FK_SET_PARAMS,						
	FK_EDIT_TEXT,						
	FK_SET_POSITION,					
	FK_DELETE_OUTLINE,					
	FK_CHANGE_LAYER,					
	FK_MOVE_GROUP,						
	FK_DELETE_GROUP,					
	FK_ROTATE_GROUP,					
	FK_ROTATE_GROUP_1,					
	FK_ROTATE_GROUP_5,					
	FK_ROTATE_GROUP_45,					
	FK_ROTATE_GROUP_90,					
	FK_ROTATE_GROUP__1,					
	FK_ROTATE_GROUP__5,					
	FK_ROTATE_GROUP__45,				
	FK_ROTATE_GROUP__90,				
	FK_SIDE_STYLE,						
	FK_BACK_WIDTH,						
	FK_NEXT_WIDTH,						
	FK_MERGE_GROUP,						
	FK_TAKE_APART,						
	FK_SELECT_CONTOUR,					
	FK_TEST_ON_CONTACT,					
	FK_DEL_WITH_MERGE,					
	FK_SEL_BETWEEN,						
	FK_RADIUS_DOWN,						
	FK_RADIUS_UP,						
	FK_SET_CLEARANCE,					
	//add 25-08-2016					
	FK_ALIGN_SIDES,						
	FK_ALIGN_X,							
	FK_ALIGN_Y,							
	FK_ALIGN_MIDDLE,        			
	FK_ALIGN_MIDLINE,					
	FK_ALIGN_MIDDLE_X,					
	FK_ALIGN_MIDDLE_Y,					
	FK_CLEARANCE_DOWN,					
	FK_CLEARANCE_UP,					
	FK_OK,								
	FK_DELETE_SIDES,											
	FK_ADD_LINE,
	FK_REPEAT_LINE,
	FK_DELETE_CUTOUT,
	FK_CANCEL_HILITE,					
	FK_GRID_STYLE,						
	FK_SHOW_M,							
	FK_SPLIT_NET,											
	FK_INSIDE_POLYLINE,					
	FK_COPY_CUT,
	FK_SET_ATTRIB,
	FK_SET_WIDTH_HATCH_LAYER,
	FK_DRAG_LINE,
	FK_DRAG_GROUP,
	FK_MAGNETIZE,
	FK_MAGNETIZE_OFF,
	FK_PREV_PAGE,
	FK_NEXT_PAGE,
	FK_SCALE_FACTOR,
	FK_IMPORT_PART,
	FK_SWITCH_TO_PCB,
	FK_OPEN_FOLDER,
	FK_COMPONENT_NOTES,
	FK_DRAW_SQUARE,
	FK_DRAW_RECT,
	FK_DRAW_ROUND,
	FK_DRAW_OVAL,
	FK_DRAW_ELLIPSE,
	FK_DRAW_RNDRECT,
	FK_DRAW_CLOUD,
	FK_CIRC_ALIGN,
	FK_SET_LENGTH,
	FK_ADD_SIZE,
	//									
	FK_NUM_OPTIONS,						
	FK_ARROW							
};																			

// selection masks
enum {
	SEL_MASK_PART = 0,
	SEL_MASK_OP_SIDES,
	SEL_MASK_OP_CORNERS,
	SEL_MASK_TEXT,
	SEL_MASK_DRC,
	SEL_COMPLEX_PARTS,
	SEL_MASK_MERGES,
	NUM_SEL_MASKS
};

#if G_LANGUAGE==0
// function key menu strings
const char fk_str[FK_NUM_OPTIONS*2+2][32] =     
{ 
"",			"",				
" Flip",	" Group",
" Recalc.",	" Nets",
" Move",	" Corner",
" Add",		" Corner",
" Delete",	" Corner",
" Move",	" Text",
" Rotate",	" Text",
" Straight"," Line",
" Arc",		" (CW)",
" Arc",		" (CCW)",
" Undo",	"",
" Set",		" Size",
" Set",		" Params",
" Edit",	" Text",
" Set",		" Position",
" Delete",	" Outline",
" Change",	" Layer",
" Move",	" Group",
" Delete",	" Group",
" Rotation"," Mode",
" Rotate",	"+1 Deg..",
" Rotate",	"+5 Deg..",
" Rotate",	"+45 Deg..",
" Rotate",	"+90 Deg..",
" Rotate",	"-1 Deg..",
" Rotate",	"-5 Deg..",
" Rotate",	"-45 Deg..",
" Rotate",	"-90 Deg..",
" Set Side"," Style",
" Back",    " Width",
" Next",    " Width",
" Merge",   " Group",
" Delete",  " Merge",
" Select",  " Outline",
" Test On", " Contact",
" Delete",  " Objects",
" Select",  " Between",
" Decrease",  " Radius",
" Increase",  " Radius",
" Set ",    "Clearance", 
//---add 25-08-2016-----*/ 
" Align",   " Sides",
" Align by"," Prev X",
" Align by"," Prev Y",
" Align",   " Between",
" Middle",  " Of Line", 
" Between", " Prev X's",
" Between", " Prev Y's",
" Less",    "",
" More",    "",
" Return",  "",
" Delete",  " Sides",
" Add",     " Line",
" Repeat",  " Line",
" Delete",  " Cutout",
" Cancel",  " Highlight",
" Grid",    " Style",
" Show",    " Merges",
" Split",   " Net",
" Inside",  " Polyline",
" Copy",    " Cutout",
" Set",     " Attributes",
" Edit",    "Properties",
" Drag",    " Item",
" Drag",    " Group",
" Magnetic"," Vertices", 
" Turn off"," magnet",
" Prev",    " Page",
" Next",    " Page",
" Scale",   " Factor",
" Import ", " Part",
" Switch",  " To PCB",
" Open",    " Folder",
" Open",    " Notes",
//
" Add",		" Square",
" Add",		" Rect",
" Add",		" Round",
" Add",		" Oval",
" Add",		" Ellipse",
" Add",		" RndRect",
" Add",		" Cloud",
" Rotary",	"Alignment",
" Set",		" Length",
" Measure", " Length",
//
"",	        ""	
};

// selection mask menu strings
const char sel_mask_str[NUM_SEL_MASKS][32] =
{
	"parts, pins",
	"line sides",
	"line vertices",
	"attributes",
	"DRC errors",
	"complex parts",
	"merges"
};
#else
// function key menu strings
const char fk_str[FK_NUM_OPTIONS * 2 + 2][32] =
{
"",			"",
" Зеркало",	" группы",
" Создать",	" эл.цепи",
" Двигать",	" узел",
" Новый",	" узел",
" Удалить",	" узел",
" Двигать",	" текст",
" Вращать",	" текст",
" Прямая"," линия",
" Дуга",		" (CW)",
" Дуга",		" (CCW)",
" Отменить",	"",
" Задать",		" размер",
" Задать",		" парам.",
" Опции",	" текста",
" Задать",		" позицию",
" Удалить",	" контур",
" Сменить",	" слой",
" Двигать",	" группу",
" Удалить",	" группу",
" Режим"," углов",
" Вращать",	" на +1°",
" Вращать",	" на +5°",
" Вращать",	" на +45°",
" Вращать",	" на +90°",
" Вращать",	" на -1°",
" Вращать",	" на -5°",
" Вращать",	" на -45°",
" Вращать",	" на -90°",
" Задать"," стиль",
" Пред.",    " ширина",
" След.",    " ширина",
" Слияние",   " группы",
" Удалить",  " слияние",
" Контур",  " целиком",
" Показ", " контакта",
" Удалить",  " объекты",
" Между",  " ними...",
" Радиус",  " меньше",
" Радиус",  " больше",
" Задать ",    "зазор",
//---add 25-08-2016-----*/ 
" Ровнять",   " стороны",
" Ровнять"," по X",
" Ровнять"," по Y",
" Ровнять",   " между...",
" Центр",  " линии",
" Между", " пред. X",
" Между", " пред. Y",
" Меньше",    "",
" Больше",    "",
" Назад",  "",
" Удалить",  " стороны",
" Новая",     " линия",
" Скорая",  " линия",
" Удалить",  " вырез",
" Отмена",  " подсв.",
" Стиль",    " сетки",
" Показ",    " слияний",
" Резать",   " цепь",
" Внутри",  " контура",
" Дубль",    " выреза",
" Задать",     " атрибут",
" Опции",    " линий",
" Двигать",    " узел",
" Двигать",    " группу",
" Магнит"," узлов",
" Убрать"," магнит",
" Пред.",    " стр.",
" След.",    " стр.",
" Менять",   " масштаб",
" Импорт ", " детали",
" Показ",  " на плате",
" Папка",    " проекта",
" Откр.",    " заметки",
//
" Квадрат",		" ",
" Прямо-",		" угольник",
" Круг",		" ",
" Овал",		" ",
" Элипс",		" ",
" Скругл.",		" прямоуг.",
" Облако",		" ",
" Ровнять",	"повернув",
" Задать",		" длину",
" Замер", " длины",
//
"",	        ""
};

// selection mask menu strings
const char sel_mask_str[NUM_SEL_MASKS][32] =
{
	"детали, пины",
	"сегменты",
	"вершины",
	"атрибуты",
	"метки",
	"составные",
	"слияния"
};
#endif

// snap modes
enum {	SM_GRID_POINTS,	// snap to grid points
		SM_GRID_LINES	// snap to grid lines
};

// descriptor for undo/redo
struct undo_descriptor {
	CFreePcbView * view;	// the view class
	CUndoList * list;		// undo or redo list
	int type;				// type of operation
	CString name1, name2;	// can be used for parts, nets, etc.
	int int1, int2;			// parameter
	CString str1;			// parameter
	void * ptr;				// careful with this
};

// group descriptor
struct undo_group_descriptor {
	CFreePcbView * view;	// the view class
	CUndoList * list;		// undo or redo list
	int type;				// type of operation
	CArray<CString> str;	// array strings with names of items in group
	CArray<id> m_id;		// array of item ids
};

class attr_size {
	public:
	int app_ref;
	int app_pin;
	int app_net;
	int app_pindesc;
	int app_foot;
	int app_value;
	float H_ref;
	float H_pin; 
	float H_net;
	float H_pindesc;
	float H_foot;
	float H_value;
	float W_ref;
	float W_pin; 
	float W_net;
	float W_pindesc;
	float W_foot;
	float W_value;
	attr_size()
	{
		int def_H = 800000;
		int def_W = 150000;
		app_ref=0;
		app_pin=0;
		app_net=0;
		app_pindesc=0;
		app_foot=0;
		app_value=0;
		H_ref=		def_H;
		H_pin=		def_H; 
		H_net=		def_H;
		H_pindesc=	def_H;
		H_foot=		def_H;
		H_value=	def_H;
		W_ref=		def_W;
		W_pin=		def_W; 
		W_net=		def_W;
		W_pindesc=	def_W;
		W_foot=		def_W;
		W_value=	def_W;
	};
	void Multiply( float value )
	{
		H_ref		*= value;
		H_pin		*= value; 
		H_net		*= value;
		H_pindesc	*= value;
		H_foot		*= value;
		H_value		*= value;
		W_ref		*= value;
		W_pin		*= value; 
		W_net		*= value;
		W_pindesc	*= value;
		W_foot		*= value;
		W_value		*= value;
		W_ref		= min( H_ref/2.0, W_ref );
		W_pin		= min( H_pin/2.0, W_pin );
		W_net		= min( H_net/2.0, W_net );
		W_pindesc	= min( H_pindesc/2.0, W_pindesc );
		W_foot		= min( H_foot/2.0, W_foot );
		W_value		= min( H_value/2.0, W_value );
	};
};

class CFreePcbView : public CView
{
public:
	enum {		
		// undo types
		UNDO_MOVE_ORIGIN = 1,		// redo
		UNDO_OP,
		UNDO_TEXT,					// redo
		UNDO_GROUP
	};

public: // create from serialization only
	CFreePcbView();
	DECLARE_DYNCREATE(CFreePcbView)

// Attributes
public:
	CFreePcbDoc* GetDocument();

// member variables
public:
	CFreePcbDoc * m_Doc;	// the document
	CDlgPartlist * m_dlg_partlist;
	CDlgCompare * m_dlg_compare;

	// Windows fonts
	CFont m_small_font;

	// attr_size 
	attr_size m_attr_size;

	// Favorites
	int m_favorite_poly[MAX_FAVORITES];
	int m_favorite_page[MAX_FAVORITES];

	// protection
	int m_protect;

	// path to package image
	CString m_draw_pic;

	// cursor mode
	int m_cursor_mode;		// see enum above

	// scale and axis
	double m_user_scale;
	double m_ang_axis;

	// current object
	int m_object;

	// magnetization to vertices
	int m_magnetize;

	// page number of obj
	int m_page_copy;

    // user clearance between segments 
	int m_seg_clearance;

	// distance of measurement
	int m_measure_dist;
	int m_measure_scale;
	int m_arrow_scale;

	//
	int m_dragging_mode;

	// previous
	int prevx;
	int ppx;
	int prev_middle_x;
	//
	float prev_m_ang;

	// previous
	int prevy;
	int ppy;
	int prev_middle_y;

	// page of fk_text
	int m_page;
	   
	// F1 - F9
	int m_fk_button_index;

	// if merge selected 
	CString m_sel_merge_name;

	// flag to indicate that a newly-created item is being dragged,
	// as opposed to an existing item
	// if so, right-clicking or ESC will delete item not restore it
	BOOL m_dragging_new_item;

	// parameters for dragging selection rectangle
	BOOL m_bLButtonDown;
	BOOL m_bRButtonDown;
	BOOL m_bDraggingRect;
	CPoint m_start_pt;
	CRect m_drag_rect, m_last_drag_rect;
	RECT m_sel_rect;		// rectangle used for selection

	// mode for drawing new polyline segments
	int m_polyline_style;	// STRAIGHT, ARC_CW or ARC_CCW
	int m_polyline_hatch;	// NONE, DIAGONAL_FULL, DOTTED or HALF_TONE
	int m_polyline_layer;	// layer being drawn
	int m_polyline_width;	//    
	int m_polyline_closed;  //

	// flag to disable context menu on right-click,
	// if right-click handled some other way
	int m_disable_context_menu;

	// selection mask
	int m_sel_mask;
	id m_mask_id[NUM_SEL_MASKS];

	// selected items
	id m_sel_id;			// id of selected item
	
	CText * m_sel_text;		// pointer to text, if selected
#define		m_sel_op		m_Doc->m_outline_poly->GetAt(m_sel_id.i)
	DRError * m_sel_dre;	// pointer to DRC error, if selected
	int m_prev_sel_merge;
	int m_sel_layer;		// layer of selected item
	int m_sel_count;		// number of selected items
	int m_sel_flags;		// mask obj
	int m_paste_flags;

	// mask selected obj
	enum{ 
		FLAG_SEL_PART = 0,		
		FLAG_SEL_TEXT,		
		FLAG_SEL_OP,		
		FLAG_SEL_DRE,
		FLAG_SEL_VTX,
		FLAG_SEL_PIN,
		FLAG_SEL_SIDE
	};
#define PART_SEL		1
#define TEXT_SEL		2
#define OP_SEL  		4
#define DRE_SEL 		8
#define VTX_SEL 		16 
#define PIN_SEL	    	32 
#define SIDE_SEL	    64 

#define TEXT_ONLY		(TEXT_SEL)
#define OP_SIDE_ONLY	(OP_SEL+SIDE_SEL)
#define VTX_ONLY		(OP_SEL+VTX_SEL)
#define PART_ONLY		(PART_SEL+OP_SIDE_ONLY)
#define PINS_ONLY		(PIN_SEL+OP_SIDE_ONLY)
#define PARTS_PINS		(PART_SEL+PIN_SEL+OP_SIDE_ONLY)
#define DRE_ONLY		(DRE_SEL)
	  
#define ID_TEXT_DEF	    ID_TEXT,ID_TXT,-1,-1,-1
	     
#define m_sel_ic m_sel_id.i							// index of selected connection
#define m_sel_ia m_sel_id.i							// index of selected area
#define m_sel_is m_sel_id.ii						// index of selected side, segment, or corner
#define m_sel_iv m_sel_id.ii						// index of selected vertex

#define OBJ_RECT		0
#define OBJ_SQUARE		1
#define OBJ_OVAL		2
#define OBJ_ROUND		3
#define OBJ_ELLIPSE		4
#define OBJ_RNDRECT		5
#define OBJ_CLOUD		6


	// display coordinate mapping
	double m_pcbu_per_pixel;	// pcb units per pixel
	double m_org_x;				// x-coord of left side of screen in pcb units
	double m_org_y;				// y-coord of bottom of screen in pcb units

	// grid stuff
	CPoint m_snap_angle_ref;	// reference point for snap angle
	int m_snap_mode;			// snap mode
	int m_inflection_mode;		// inflection mode for routing

	// window parameters
	CPoint m_client_origin;	// coordinates of (0,0) in screen coords
	CRect m_client_r;		// in device coords
	int m_left_pane_w;		// width of pane at left of screen for layer selection, etc.
	int m_bottom_pane_h;	// height of pane at bottom of screen for key assignments, etc.
	CRgn m_pcb_rgn;			// region for the pcb
	int m_left_pane_invalid;	// flag to erase and redraw left pane

	// active layer for routing and placement
	int m_active_layer;
	int m_draw_layer;
	
	// mouse
	CPoint m_last_mouse_point;	// last mouse position
	CPoint m_last_cursor_point;	// last cursor position (may be different from mouse)
	CPoint m_from_pt;			// for dragging rect, origin

	// function key shortcuts
	int m_fkey_option[12];
	int m_fkey_command[12];
	char m_fkey_str[24][32];

	// memory DC and bitmap
	BOOL m_botDC_created;
	BOOL m_topDC_created;
	CDC m_botDC;
	CDC m_topDC;
	CBitmap m_bitmap1;
	CBitmap m_bitmap2;
	HBITMAP m_old_bitmap1;
	HBITMAP m_old_bitmap2;
	CRect m_bitmap_rect1;
	CRect m_bitmap_rect2;

	// logos
	CBitmap m_bitmap_txt;
	CBitmap m_bitmap_word;
	CBitmap m_bitmap_pdf;
	CBitmap m_bitmap_url;

// Operations
public:
	void InitInstance();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFreePcbView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFreePcbView();
	void InitializeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	void SetMainMenu( BOOL bAll );									//will remain
	int SetDCToWorldCoords( CDC * pDC );							//will remain
	void SetCursorMode( int mode );									//will remain	
	void SetFKText( int mode );										//will remain
	void DrawBottomPane();											//will remain
	int ShowCursor();												//will remain
	int ShowSelectStatus();											//will remain
	void ShowRelativeDistance( int dx, int dy );					//will remain
	void ShowRelativeDistance( int x, int y, int dx, int dy );		//will remain
	int ShowActiveLayer(int n_layers, BOOL swCASE=0);				//will remain				
	void CancelSelection( BOOL ConsiderFlags=TRUE );						//will remain
	void SetSelMaskArray( int mask );								//will remain
	void SnapCursorPoint( CPoint wp, UINT nFlags );					//will remain
	void InvalidateLeftPane()
	{ 
		m_left_pane_invalid = TRUE; 
	}		//will remain
	void HandleKeyPress(UINT nChar, UINT nRepCnt, UINT nFlags);		//will remain
	void RedrawLayers();
	void DrawBOM( CText * t, CArray<CPolyLine> * Arr=NULL );
	BOOL DrawBOM(	CPolyLine * p, 
					int iStart, 
					int iEnd, 
					int sortStyle, 
					int Extended, 
					BOOL ignore_without_value, 
					int inc_list, 
					int ex_list, 
					CString * order,
					CString * pcb);
	void DrawPATH( CText * t, int ipage );
	void SetMagnet( int mag );

	// for group
	BOOL GetSelectedItem();											//
	void ScaleFactor( double scale, BOOL bTOTAL=FALSE );
	void MarkAllOutlinePoly( int utility, int layer );				//
	void MoveGroup( int dx, int dy, CText * BOM_TXT=NULL );			//
	void RotateGroup( int angle, int cx, int cy, double accurate=0.0 );					//
	void RotateGroup( int angle );									//
	void MergeGroup();												//
	void MergeGroup(int merge0);									//
	void ExplodeGroup();											//
	void RemoveOrphanMergeNames();
	int  NewSelect( void * ptr, id * sid, BOOL bSet_cursor_mode, BOOL bInvert );	// Set sel element 
	int  UnSelect( void * ptr, id * sid, BOOL bSET_CURSOR_MODE=0, BOOL GSI=TRUE );	// Unselect element
	CString SelectBlock( CPolyLine * poly, int npage, BOOL bSelect=0 );
	CString NewSelectM( int number );								// M-elements selection
	void NewSelectP( CText * part, CText * pin, BOOL hAttrs=TRUE, int Swell=0 );
	void NewSelectB( CText * descriptor );
	void SelectItemsInRect( CRect r, UINT nFlags );
	void TurnGroup ();												//
	void MoveOrigin( int x_off, int y_off );						//
	void DeleteGroup( BOOL wMerge );								//
	void FindGroupCenter( BOOL bFLOOR=TRUE );											//
	void HighlightGroup(BOOL bPins=TRUE);							//
	void StartDraggingGroup( BOOL bAdd=FALSE, int x=0, int y=0 );	//
	void CancelDraggingGroup();										//
	void OnInsidePolyline();										//
	BOOL TestSelElements(int mode);									//
	void SelectTextLayer( int Layer, BOOL SEL=1 );
	void SelectPolylineLayer( int Layer, BOOL SEL=1 );
	//
	enum	{
		FOR_FK_SET_CLEARANCE = 0,
		FOR_FK_APPROXIM_ARC,
	    FOR_FK_RADIUS_UP_DOWN,
		FOR_FK_INSERT_SEGMENT,
		FOR_FK_CONVERT_TO_LINE,
		FOR_FK_SELECT_BETWEEN,
		FOR_FK_ALIGN_SEGMENTS
	};
	void SelectContour();																				//will remain
	int DeleteWithRecalculationPoint (CPolyLine * p, BOOL WITH_MERGE);									//will remain	
	void FindNEXTSegmentOfGroup ( float angle, id * _id, id * f_id );									//will remain
	BOOL find_side_for_set_clearance( CPolyLine * pp, id * _id, int Y, float angle, int min_len );		//will remain
	void OnSetClearance();																				//will remain
	void ApproximArc();																					//will remain
	int  ArcApp( CPolyLine * po, int forArcOnly, int m_i );												//will remain
	void TracesRadiusUpDown(BOOL UP);																	//will remain							
	void AlignSides( int type, int ic, int iv);															//will remain
	void OnGroupPaste( BOOL bwDialog, BOOL bSaveMerges=0, int m_apply_def_w=0 );						//will remain
	void CheckMessages();
	void OnMess( int command, int n_str, CArray<CString> *str, BOOL IronScale );						//will remain
	CWnd * OnInfoBoxSendMess( CString mess );	
	void SelPolylineTest( CText ** part_found, CString * P, CString * V, CString * F, 
							int * nparts, int * nvalues, int * nfootprints, 
							int * without_attr, int max_attr=INT_MAX );
	void MultipleAttributes( int MODE=0 );
	void JumpToPart( CText * ref, CText * pin=NULL, int IronScale=0 );
	void UnSelVertices();
	void EditPolylineProperties();
	CText * FindSimilarPart( CText * RefText, int start_i = -1 );

	// check mode   
	BOOL CurNone();				//will remain
	BOOL CurSelected();			//will remain
	BOOL CurDragging();			//will remain
	BOOL CurDraggingPlacement();//will remain
	
	// undo
	void SaveUndoInfoForMoveOrigin( int x_off, int y_off, CUndoList * list );					//will remain
	void SaveUndoInfoForOutlinePoly( int index, BOOL new_event, CUndoList * list );				//will remain
	void SaveUndoInfoForText( CText * text, BOOL new_event, CUndoList * list );		//will remain
	int SaveUndoInfoForGroup( CUndoList * list, BOOL bDELETE=0, BOOL wMerge=0 );	//will remain
	void *  CreateUndoDescriptor( CUndoList * list, int type,									//will remain
		CString * name1, CString * name2, int int1, int int2, CString * str1, void * ptr );		//will remain
	static void UndoCallback( void * ptr, BOOL undo );								//will remain
	static void UndoGroupCallback( void * ptr, BOOL undo );							//will remain

	
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFreePcbView)
	afx_msg void OnSize(UINT nType, int cx, int cy);					//will remain
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);				//will remain
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);				//will remain
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);		//will remain
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);				//will remain
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);			//will remain
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	//will remain
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);		//will remain
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);	//will remain
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point );	//will remain
	afx_msg void OnTextMove();								//will remain
	afx_msg void OnPartRemoveMerge();						//will remain
	afx_msg void OnTextEdit();								//will remain
	afx_msg void OnTextCSV();
	void OnTextAlign( char edge );
	afx_msg void OnTextAlignL();
	afx_msg void OnTextAlignR();
	afx_msg void OnTextAlignT();
	afx_msg void OnTextAlignB();
	afx_msg void OnTextAlignRV();
	afx_msg void OnTextAlignRF();
	void TextAlignRVF( BOOL RFV=0, BOOL RV=0, BOOL RF=0 );
	afx_msg void OnTextAlignRVF(); 	
	afx_msg void OnTextAlignRFV();
	afx_msg void OnAddOutlinePoly();						//will remain
	afx_msg void OnAddCutout();
	afx_msg void OnSetPrintArea();
	void AddOutlinePoly( BOOL bREPEAT_OR_ADD_CUTOUT );		//will remain
	afx_msg void OnOPCornerMove();							//will remain
	afx_msg void OnOPCornerEdit();							//will remain
	afx_msg void OnOPCornerDelete();						//will remain
	afx_msg void OnOPSideAddCorner();						//will remain
	afx_msg void OnOPSetAttributes();
	CString OPSetAttributes( CString * bDialog=NULL );
	afx_msg void OnOPSideConvertToStraightLine();			//will remain
	afx_msg void OnOPSideConvertToArcCw();					//will remain
	afx_msg void OnOPSideConvertToArcCcw();					//will remain
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);					//will remain
	afx_msg void OnViewAllElements();						//will remain
	RECT ViewAllElements( RECT * r );						//will remain
	afx_msg void OnViewMerges();
	afx_msg void OnViewSelTextsByLabels();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);//will remain
	afx_msg void OnViewFindAttr();							//will remain
	int FindStrInFile(	CString * file, 
						CString * str, 
						CString * str2, 
						int f_flags, 
						int insertMode, 
						int singleMode, 
						int * curX, 
						int * curY, 
						int npinmin, 
						int npinmax, 
						int mergeIndex=-1,
						CString * jPEG=NULL );
	void FootprintNameTest( CString * FP, CString * OLD_FP );
	afx_msg void OnReplacePartPattern();
	afx_msg void OnUpdatePartPattern();
	afx_msg void OnViewFindAmoungHid();
	afx_msg void OnViewFindAmoungSel();
	afx_msg void OnDrc();									//will remain
	afx_msg void OnClearDRC();								//will remain
	afx_msg void OnViewAll();								//will remain							
	afx_msg void OnToolsMoveOrigin();						//will remain
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	//will remain
	afx_msg void OnGroupMove();								//will remain
	afx_msg void OnPasteSinglePart();
	afx_msg void OnPastePartRow();
	afx_msg void OnTextChangeF();
	afx_msg void OnTextChangeV();
	afx_msg void OnTextReplace();
	void OnTextChangeAttr( int iatt );
	void FindAttr( int insertMode, int singleMode, CString * autoRun=NULL );
	LONG OnChangeVisibleGrid( UINT wp, LONG lp );			//will remain
	LONG OnChangePlacementGrid( UINT wp, LONG lp );			//will remain
	LONG OnChangeTextGrid( UINT wp, LONG lp );			//will remain
	LONG OnChangeSnapAngle( UINT wp, LONG lp );				//will remain
	LONG OnChangeUnits( UINT wp, LONG lp );					//will remain
	afx_msg void OnGroupSaveToFile();						//will remain
	void SaveToFile( CString * fileS );
	afx_msg void OnGroupSaveToOpenscadFile();
	afx_msg void OnGroupSaveToDXFFile();
	afx_msg void OnAddGroupOrigin();
	afx_msg void OnAddGroupRect();
	afx_msg void OnGroupStaticHighlight();					//will remain
	afx_msg void OnGroupCancelHighlight();					//will remain
	afx_msg void OnGroupCancelSel();
	void GroupCancelHighlight();							//will remain
	afx_msg void OnApproximationArc();						//will remain
	afx_msg void OnSelectPartsByAttr();
	afx_msg void OnSelectPolyByAttr();
	afx_msg void OnSelectObjectByAttr();
	void SelectByAttr( BOOL bPOLY = FALSE );
	afx_msg void OnGroupCopy();								//will remain
	void GroupCopy( int clearBuf );
	afx_msg void OnGroupCut();								//will remain
	afx_msg void OnGroupDelete();							//will remain
	afx_msg void OnGroupLineUp();
	afx_msg void OnEditCopy();								//will remain
	afx_msg void OnEditPaste();								//will remain
	afx_msg void OnEditCut();								//will remain
	afx_msg void OnGroupRotate();							//will remain
	afx_msg void OnTextSimilar();
	afx_msg void OnTextGrabPart();
	afx_msg void OnProjectRunInfoBox();						//will remain
	afx_msg void OnSetOriginToSelectedItem();				//will remain
	void SetOriginToSelectedItem();							//will remain
	void ProjectRunInfoBox();								//will remain
	afx_msg void OnImportMerges();
	//
	afx_msg BOOL OnRangeCmds( UINT CMD );
	afx_msg BOOL OnEditSelectLayer( UINT CMD );
	afx_msg BOOL OnHidePictures( UINT CMD );
	afx_msg BOOL OnCreateMultiPinPartTemplate( UINT CMD );
	afx_msg BOOL OnApplyScale( UINT CMD );
	afx_msg BOOL OnSetSlidingAngle( UINT CMD );
};

#ifndef _DEBUG  // debug version in FreePcbView.cpp
inline CFreePcbDoc* CFreePcbView::GetDocument()
   { return (CFreePcbDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FREEPCBVIEW_H__BE1CA173_E2B9_4252_8422_0B9767B01566__INCLUDED_)
