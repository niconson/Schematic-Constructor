// textlist.cpp ... definition of class CTextList
//
#pragma once


#include <afxcoll.h>
#include <afxtempl.h>
#include "DisplayList.h"
#include "PcbFont.h"
#include "smfontutil.h"
#include "UndoList.h"


#define PDF_DEF			0
#define PDF_GRAPHIC		1
#define PDF_USE_FONT	2
#define PDF_ALIGN		3
#define PDF_NO_ALIGN	4
#define SYNC_EXCLUDE	5
#define RVF_FLAG		6
#define COMPRESSED		7
#define UNCLENCHED		8
#define ITALIC			9
#define REF_LIST_INDEX	10

class CTextList;
class CText;
struct stroke;

struct undo_text {
	GUID m_guid;
	int m_x, m_y;
	int m_layer;
	int m_angle;
	int u_merge;
	int u_submerge;
	int u_font_n;
	int u_pdf_opt;
	int m_font_size;
	int m_stroke_width;
	int m_nstrokes;
	int m_Visible;
	int m_p_start;
	CString m_str;
	CTextList * m_tlist;
};

enum{
		index_part_attr=0,
		index_pin_attr,
		index_net_attr,
		index_desc_attr,
		index_foot_attr,
		index_value_attr,
		num_attributes
	};

class CText
{
public:
	// member functions
	//CText();
	CText( CTextList * tlist,
		int x, int y, int angle, 
		int layer, int font_size, 
		int stroke_width, CString * str_ptr );
	~CText();
	void Draw( CDisplayList * dlist, SMFontUtil * smfontutil, int tab=0, CArray<CPoint> * m_stroke=NULL );
	void Undraw();
	CSize GetTextHeight( SMFontUtil * smfontutil );
	// member variables
	GUID m_guid;
	int isVISIBLE;
	int m_x, m_y;
	int m_layer;
	int m_angle;
	BOOL m_selected;
	int m_font_size;
	int m_stroke_width;
	CPcbFont * m_font;
	int m_nchars;
	CString m_str;
	int m_merge;		// merge with other obj
	int m_submerge;		// submerge with other obj
	int m_font_number;
	int m_pdf_options;
	CDisplayList * m_dlist;
	CTextList * m_tl;
	dl_element * dl_el;
	dl_element * dl_sel;
	SMFontUtil * m_smfontutil;
	int utility;
	int m_polyline_start;
	void MakeVisible();
	void InVisible();
};

class CTextList
{
public:
	// member functions
	CTextList();
	CTextList( CDisplayList * dlist, SMFontUtil * smfontutil );
	~CTextList();
	CText * AddText( int x, int y, int angle, int layer, 
					int font_size, int stroke_width, int font_num, 
					CString * str_ptr, BOOL draw_flag=TRUE, int pdf_opt=0 );
	int RemoveText( CText * text );
	int RemoveText( int it );
	int GetSelCount( CArray<CPolyLine> * pArray = NULL );
	int IsCommand( CText * text );
	void  RemoveAllTexts();
	void HighlightText( CText * text, int transparent_static=0 );
	void StartDraggingText( CDC * pDC, CText * text );
	void CancelDraggingText( CText * text );
	void MoveText( CText * text, int x, int y, int angle, int layer );
	void MoveOrigin( int x_off, int y_off );
	void MarkAllTexts( int m );
	CText * GetText( GUID * guid );
	CText * GetText( int x, int y );
	CText * GetText( CString * S, int * it = NULL );
	CText * GetNextText( int * it );  
	CText * GetNextAnyText( int * it );
	CText * GetNextInvisibleText( int * it );
	void CancelSelection();
	void TimeUpdate( CText * text );
	int GetNumTexts(){ return text_ptr.GetSize();};
	int GetNumDifferentTexts( BOOL exclBreak );
	BOOL GetTextBoundaries( RECT * r );
	BOOL GetTextRectOnPCB( CText * t, RECT * r );
	void ReassignALayers( int n_new_layers, int * layer );
	undo_text * CreateUndoRecord( CText * text );
	static void TextUndoCallback( void * ptr, BOOL undo );

	// member variables
	SMFontUtil * m_smfontutil;
	CDisplayList * m_dlist;
	CArray<CText*> text_ptr;
};

struct CAttrib {
	CTextList * m_Reflist;		// text list
	CTextList * m_Pinlist;		// text list
	CTextList * m_pDesclist;	// text list
	CTextList * m_Netlist;		// text list
	CTextList * m_Footlist;		// text list
	CTextList * m_Valuelist;	// text list
};