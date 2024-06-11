// TextList.cpp ... implementation of class CTextList
//
// class representing text elements for PCB (except reference designators for parts)
//
#include "stdafx.h"
#include "utility.h"
#include "textlist.h"
#include "PcbFont.h"
#include "ids.h"
#include "smfontutil.h"
#include "file_io.h"

extern CFreeasyApp theApp;
// CText constructor
// draws strokes into display list if dlist != 0
//
CText::CText( CTextList * tlist,
			int x, int y, int angle, 
			int layer, int font_size, int stroke_width, 
			CString * str_ptr )
{
	m_guid = GUID_NULL;
	HRESULT hr = ::UuidCreate(&m_guid);
	m_x = x;
	m_y = y;
	m_angle = angle;
	m_layer = layer;
	m_stroke_width = stroke_width;
	m_font_size = font_size;
	m_str = *str_ptr;
	m_nchars = str_ptr->GetLength();
	m_tl = tlist;
	m_selected = 0;
	m_merge = -1;
	m_submerge = -1;
	m_polyline_start = -1;
	utility = 0;
	m_dlist = NULL;
	m_smfontutil = NULL;
	isVISIBLE = 0;
	Undraw();
}

// CText destructor
// removes all dlist elements
//
CText::~CText()
{
	Undraw();
}

void CText::MakeVisible()
{ 
	isVISIBLE = 1; 
	Draw( m_tl->m_dlist, m_tl->m_smfontutil ); 
}
void CText::InVisible()
{ 
	isVISIBLE = 0; 
	m_polyline_start = -1;
	Undraw(); 
}
CSize CText::GetTextHeight( SMFontUtil * smfontutil )
{
	double coord[64][4];
	double min_x, min_y, max_x, max_y;
	int nLines = smfontutil->GetCharStrokes( 'I', (FONT_TYPE)m_font_number, &min_x, &min_y, &max_x, &max_y, coord, 64 );
	if( nLines )
	{
		CSize H;
		H.cx = min_y;
		H.cy = max_y;
		return H;
	}
	return 0;
}
// Draw text as a series of strokes
// If dlist == NULL, generate strokes but don't draw into display list
void CText::Draw( CDisplayList * dlist, SMFontUtil * smfontutil, int tab, CArray<CPoint> * m_stroke )
{
	if( m_dlist )
		Undraw();
	if( !isVISIBLE )
		return;// == ASSERT(0)
	
	// draw text
	id id( ID_TEXT, ID_STROKE );
	CArray<CPoint> strk;
	if( m_stroke == NULL )
		m_stroke = &strk;
	m_stroke->SetSize( 0 );
	if( smfontutil && m_nchars )
	{
		m_smfontutil = smfontutil;
		// now draw strokes
		CPoint si, sf;
		double tan = 0.0;
		if( getbit( m_pdf_options, ITALIC ) )
			tan = 0.35;
		double x_scale = (double)m_font_size/22.0;
		double y_scale = (double)m_font_size/22.0;
		double y_offset = 9.0*y_scale;
		//
		CSize minmax = GetTextHeight( m_smfontutil );
		double TH = minmax.cy - minmax.cx + 1.0;
		if( TH > 2.0 )
		{
			x_scale = (double)m_font_size/TH;
			y_scale = (double)m_font_size/TH;
			y_offset = -(double)minmax.cx*y_scale;
		}
		double sp_scale = (double)m_font_size/22.0; //2066.0*TH;
		double xc = 0.0;
		float m_w = (float)m_stroke_width/(float)NM_PER_MIL;
		int mxc = 0;
		double yc = 0.0;
		RECT sel;
		sel.left = sel.bottom = INT_MAX;
		sel.right = sel.top = INT_MIN;
		BOOL present = 0;
		int tab_cnt = 0;
		int dot_cnt = 0;
		double letter_spacing = theApp.m_Doc->m_letter_spacing * sp_scale;
		if( getbit( m_pdf_options, COMPRESSED ) )
			letter_spacing *= 2.0;
		else if( getbit( m_pdf_options, UNCLENCHED ) )
			letter_spacing *= 5.0;
		else
			letter_spacing *= 3.5;
		for( int ic=0; ic<m_nchars; ic++ )
		{
			// get stroke info for character
			int xi, yi, xf, yf;
			double coord[64][4];
			double min_x, min_y, max_x, max_y;
			int nstrokes = 0;
			if( m_str[ic] == '|' && present )
				break;
			if( m_str[ic] == '\'' )
			{
				xc = 0;
				mxc = 0;
				float kdoc = theApp.m_Doc->m_alignment;
				yc -= (2.0*(float)m_font_size*kdoc + (float)m_stroke_width/2.0);
				tab_cnt = 0;
				continue;
			}
			if( m_str[ic] == '`' && tab )// for draw BOM lines
			{
				tab_cnt++;
				xc = max( tab*tab_cnt, mxc+(14.0*x_scale) );
				continue;
			}
			if( m_str[ic] == '#' && tab_cnt )
			{
				int sz = m_stroke->GetSize();
				m_stroke->SetSize( sz + 2 );
				CPoint pp1( 0 - m_font_size*3/2, yc + y_offset );
				CPoint pp2( xc - m_font_size*5/2, yc + y_offset );
				// rotate
				if( m_angle )
				{
					RotatePoint( &pp1, -m_angle, zero );
					RotatePoint( &pp2, -m_angle, zero );
				}	
				SwellRect( &sel, pp1 );
				SwellRect( &sel, pp2 );
				pp1.x += m_x;
				pp2.x += m_x;
				pp1.y += m_y;
				pp2.y += m_y;
				m_stroke->SetAt(sz,pp1);
				m_stroke->SetAt(sz+1,pp2);
				continue;
			}
			//if( mxc <= (tab*tab_cnt + tab*colw[?] + tab/2) || tab == 0 )
			{
				dot_cnt = 0;
				nstrokes = m_smfontutil->GetCharStrokes( m_str[ic], (FONT_TYPE)m_font_number, &min_x, &min_y, &max_x, &max_y, coord, 64 );
			}
			//else ///if( dot_cnt < 2 )
			{
			//	dot_cnt++;
			//	nstrokes = m_smfontutil->GetCharStrokes( '.', (FONT_TYPE)m_font_number, &min_x, &min_y, &max_x, &max_y, coord, 64 );
			}
			if( nstrokes > 0 )
			{
				int sz = m_stroke->GetSize();
				m_stroke->SetSize( sz + 2*nstrokes );
				if( getbit( m_pdf_options, COMPRESSED ) ||
					getbit( m_pdf_options, UNCLENCHED ) )
				{
					double kc = 1.2;
					if( getbit( m_pdf_options, COMPRESSED ) )
						kc = 0.8;
					min_x *= kc;
					max_x *= kc;
					for( int is=0; is<nstrokes; is++ )
						for( int ip=0; ip<4; ip+=2 )
							coord[is][ip] *= kc;
				}
				for( int is=0; is<nstrokes; is++ )
				{
					present = 1;
					si.x = (coord[is][0] + (tan*coord[is][1]) - min_x)*x_scale + xc;
					si.y = coord[is][1]*y_scale + y_offset + yc;
					sf.x = (coord[is][2] + (tan*coord[is][3]) - min_x)*x_scale + xc;
					sf.y = coord[is][3]*y_scale + y_offset + yc;
					//
					//
					SwellRect( &sel, si.x, si.y );
					SwellRect( &sel, sf.x, sf.y  );
					// rotate
					if( m_angle )
					{
						RotatePoint( &si, -m_angle, zero );
						RotatePoint( &sf, -m_angle, zero );
					}			
					// add x, y and draw
					CPoint pp1( m_x + si.x, m_y + si.y );
					m_stroke->SetAt(sz+(2*is),pp1);
					CPoint pp2( m_x + sf.x, m_y + sf.y );
					m_stroke->SetAt(sz+(2*is)+1,pp2);
				}
				xc += (max_x - min_x)*x_scale + m_stroke_width + letter_spacing;
				mxc = xc;
			}
			else
				xc += 14.0*x_scale;
		}
		if( dlist )
		{
			m_dlist = dlist;
			// create selection rectangle
			if( sel.left == INT_MAX )
			{
				sel = rect( 0,0,NM_PER_MIL*4,NM_PER_MIL*4 );
			}
			MoveRect( &sel, m_x, m_y );
			SwellRect( &sel, m_stroke_width/2 );
			int width = xc - 8.0*x_scale;
			CPoint pts[4];
			CPoint c;
			pts[0].x = sel.right;
			pts[0].y = sel.bottom;
			pts[1].x = sel.right;
			pts[1].y = sel.top;
			pts[2].x = sel.left;
			pts[2].y = sel.top;
			pts[3].x = sel.left;
			pts[3].y = sel.bottom; 
			c.x = m_x;
			c.y = m_y;
			// rotate to angle
			if( m_angle )
			{
				RotateRect( &sel, m_angle, c );
				RotatePOINTS( &pts[0], 4, -m_angle, c );
			}
			// draw it
			static int index = 0;
			id.st = ID_TXT;
			id.i = 0;
			id.ii = ++index;
			//
			if( m_stroke->GetSize() == 0 )
			{
				m_stroke->Add( pts[0] );
				m_stroke->Add( pts[2] );
				m_stroke->Add( pts[1] );
				m_stroke->Add( pts[3] );
			}
			dl_el = m_dlist->Add( this, id, m_layer, DL_LINES_ARRAY, 1,
										&sel, m_stroke_width, &(*m_stroke)[0], m_stroke->GetSize() );
			dl_sel = m_dlist->AddSelector( this, id, DL_POLYGON, 1, &sel, 0, pts, 4, m_layer );
			//
			if( m_layer == LAY_INFO_TEXT )
				dl_sel->visible = 0;
			else if( (m_layer == LAY_NET_NAME && m_str.Left(3) == FREENET ) || //hide NET0... format of nets
					 (m_tl == theApp.m_Doc->m_tlist && m_str.Find("|") > 0) )  //hide nets of electronic buses
			{
				dl_el->visible = 0;
				dl_sel->visible = 0;
			}	
		}
	}
	else
	{
		m_dlist = NULL;
		m_smfontutil = NULL;
	}
}




void CText::Undraw()
{
	if( m_dlist )
	{
		if( dl_sel )
			m_dlist->Remove( dl_sel );	
		if( dl_el )
			m_dlist->Remove( dl_el );
		m_dlist = NULL;		// indicated that it is not drawn
	}
	dl_el = NULL;
	dl_sel = NULL;
	m_smfontutil = NULL;	// indicate that strokes have been removed
}

// CTextList constructor/destructors
//

// default constructor
//
CTextList::CTextList()
{
	m_dlist = NULL;
	m_smfontutil = NULL;
}

// normal constructor
//
CTextList::CTextList( CDisplayList * dlist, SMFontUtil * smfontutil )
{
	m_dlist = dlist;
	m_smfontutil = smfontutil;
}

// destructor
//
CTextList::~CTextList()
{
	// destroy all CTexts
	for( int i=0; i<text_ptr.GetSize(); i++ )
	{
		delete text_ptr[i];
	}
}

// AddText ... adds a new entry to TextList, returns pointer to entry
//
CText * CTextList::AddText( int x, int y, int angle, int layer, 
						   int font_size, int stroke_width, int font_num, 
						   CString * str_ptr, BOOL draw_flag, int pdf_opt )
{
	// create new CText and put pointer into text_ptr[]
	CText * text = new CText( this, x, y, angle, layer, font_size, stroke_width, str_ptr );	//ok
	text_ptr.Add( text );
	text->m_merge = -1;
	text->m_submerge = -1; 
	text->m_pdf_options = pdf_opt;
	text->m_font_number = font_num;
	if( draw_flag )
		text->MakeVisible();
	else
		text->InVisible();
	return text;
}

int CTextList::GetSelCount( CArray<CPolyLine> * pArray )
{
	int cnt=0;
	for( int i=text_ptr.GetSize()-1; i>=0; i-- )
	{
		if( text_ptr[i]->m_selected )
			cnt++;
		else if( pArray )
		{
			if( text_ptr[i]->m_polyline_start >= 0 )
				if( text_ptr[i]->m_polyline_start < pArray->GetSize() )
					if( pArray->GetAt( text_ptr[i]->m_polyline_start ).GetSideSel() )
						cnt++;
		}
	}
	return cnt;
}

int CTextList::IsCommand( CText * text )
{
	BOOL isCOMMAND = (	text->m_str.Right(3) == "BLK" || 
						text->m_str.Right(3) == "BOM"	||
						text->m_str.Right(4) == "PATH"	||
						text->m_str.Right(4) == "DATE"	||
						text->m_str.Find( CP_LINK ) > 0 ||
						text->m_str.Right(4) == "ZONE" ||
						text->m_str.Find( "PrintableArea" ) > 0 ||
						text->m_str.Right(7) == "BLK_PTR");
	return isCOMMAND;
}

// RemoveText ... removes an entry and destroys it
//	returns 0 if successful, 1 if unable to find text
//
int CTextList::RemoveText( CText * text )
{
	for( int i=0; i<text_ptr.GetSize(); i++ )
	{
		if( text_ptr[i] == text )
			return RemoveText( i );
	}
	return 1;
}
int CTextList::RemoveText( int it )
{
	text_ptr[it]->Undraw();
	delete text_ptr[it];
	text_ptr.RemoveAt( it );
	return 0;
}

// remove all text entries
//	returns 0 if successful, 1 if unable to find text
//
void CTextList::RemoveAllTexts()
{
	int n = text_ptr.GetSize();
	for( int i=0; i<n; i++ )
	{
		delete text_ptr[i];
	}
	text_ptr.RemoveAll();
	return;
}

// Select text for editing
//
void CTextList::HighlightText( CText * text, int transparent_static )
{
	if( text->dl_el )
	{
		m_dlist->HighLight( text->dl_el );
		if( transparent_static )
		{
			text->dl_el->transparent = transparent_static;
			text->dl_el->el_static = transparent_static;
		}
	}
}

// start dragging a rectangle representing the boundaries of text string
//
void CTextList::StartDraggingText( CDC * pDC, CText * text )
{
	// make text strokes invisible
	text->dl_el->visible = 0;

	// cancel selection 
	m_dlist->SetLayerVisible( LAY_HILITE, FALSE );
	RECT tr;
	GetTextRectOnPCB(text,&tr);
	int tx = text->m_x;
	int ty = text->m_y;
	Rotate_i_Vertex( &tx, &ty, -text->m_angle, text->m_x, text->m_y );
	m_dlist->StartDraggingRectangle(	pDC, 
										0,0,
										tr.left-tx,
										tr.bottom-ty,
										tr.right-tx,
										tr.top-ty,
										LAY_SELECTION );
}

// stop dragging text 
//
void CTextList::CancelDraggingText( CText * text )
{
	m_dlist->StopDragging();
	text->dl_el->visible = 1;
	if( theApp.m_Doc->m_vis[LAY_HILITE] )
		m_dlist->SetLayerVisible( LAY_HILITE, TRUE );
}


// move text
//
void CTextList::MoveText( CText * text, int x, int y, int angle, int layer )
{
	CDisplayList * dl = text->m_dlist;
	SMFontUtil * smf = text->m_smfontutil;
	text->Undraw();
	text->m_x = x;
	text->m_y = y;
	text->m_angle = angle;
	text->m_layer = layer;
	text->Draw( dl, smf );
}

// create new undo_text object for undoing changes
//
undo_text * CTextList::CreateUndoRecord( CText * text )
{
	undo_text * undo = new undo_text;//ok
	undo->m_guid = text->m_guid;
	undo->m_x = text->m_x;
	undo->m_y = text->m_y; 
	undo->m_layer = text->m_layer; 
	undo->m_angle = text->m_angle; 
	undo->u_merge = text->m_merge;
	undo->u_submerge = text->m_submerge;
	undo->u_pdf_opt = text->m_pdf_options;
	undo->u_font_n = text->m_font_number;
	undo->m_font_size = text->m_font_size; 
	undo->m_stroke_width = text->m_stroke_width;
	undo->m_str = text->m_str;
	undo->m_Visible = text->isVISIBLE;
	undo->m_p_start = text->m_polyline_start;
	undo->m_tlist = this;
	return undo;
}

void CTextList::TextUndoCallback( void * ptr, BOOL undo )
{
	undo_text * un_t = (undo_text*)ptr;
	if( undo )
	{
		CTextList * tlist = un_t->m_tlist;
		// find existing CText object
		CText * text = tlist->GetText( &un_t->m_guid );
		if( text )
		{
			// text string found
			// modify text back
			text->Undraw();
			text->m_guid = un_t->m_guid;
			text->m_x = un_t->m_x;
			text->m_y = un_t->m_y;
			text->m_angle = un_t->m_angle;
			text->m_layer = un_t->m_layer;
			text->m_font_size = un_t->m_font_size;
			text->m_stroke_width = un_t->m_stroke_width;
			// renumber part designations
			if( un_t->m_layer == LAY_PART_NAME )
			{
				if( text->m_str.Compare( un_t->m_str ) )
				{
					theApp.m_Doc->m_dlg_log->OnShowMe();
					CString line;
					line.Format( RENUMBERING_PATTERN, un_t->m_str, text->m_str );
					// remove line
					int Removed = 0;
					int ipcb = theApp.m_Doc->Pages.GetCurrentPcbIndex();
					int ActivePage = theApp.m_Doc->Pages.GetActiveNumber();
					for( int ii=theApp.m_Doc->PartRenumber[ipcb].GetSize()-1; ii>=theApp.m_Doc->m_part_renumber_pos; ii-- )
					{
						if( line.Compare( theApp.m_Doc->PartRenumber[ipcb].GetAt(ii) ) == 0 )
						{
							theApp.m_Doc->Renumber__Add( &text->m_str, &un_t->m_str, ActivePage, ii );
							Removed = 1;
							break;
						}
					}
					// add line
					if( Removed == 0 )
					{
						theApp.m_Doc->Renumber__Add( &text->m_str, &un_t->m_str, ActivePage );
					}
				}
			}
			CString old_str = text->m_str;
			text->m_nchars = un_t->m_str.GetLength();
			text->m_str = un_t->m_str;
			text->m_merge = un_t->u_merge;
			text->m_submerge = un_t->u_submerge;
			text->m_pdf_options = un_t->u_pdf_opt;
			text->m_font_number = un_t->u_font_n;
			text->isVISIBLE = un_t->m_Visible;
			text->m_polyline_start = un_t->m_p_start;
			if( text->isVISIBLE )
			{
				text->Draw( tlist->m_dlist, tlist->m_smfontutil );
				if( text->m_layer == LAY_PIN_DESC || text->m_layer >= LAY_ADD_1 )
					theApp.m_Doc->m_view->DrawBOM(text);
			}
			//  INTERPAGE SYNC
			if( text->m_polyline_start >= 0 )
			{
				CPolyLine * p = &theApp.m_Doc->m_outline_poly->GetAt(text->m_polyline_start);
				if( CText * ref = p->Check( index_part_attr ) )
				{
					int i_att = -1;
					if( text->m_tl == theApp.m_Doc->Attr->m_pDesclist )
						i_att = index_desc_attr;
					else if( text->m_tl == theApp.m_Doc->Attr->m_Netlist )
						i_att = index_net_attr;
					else if( text->m_tl == theApp.m_Doc->Attr->m_Pinlist )
						i_att = index_pin_attr;
					else if( text->m_tl == theApp.m_Doc->Attr->m_Footlist )
						i_att = index_foot_attr;
					else if( text->m_tl == theApp.m_Doc->Attr->m_Valuelist )
						i_att = index_value_attr;
					else if( text->m_tl == theApp.m_Doc->Attr->m_Reflist )
						i_att = index_part_attr;
					if( i_att >= 0 )
						if( getbit( ref->m_pdf_options, SYNC_EXCLUDE ) == 0 )
							theApp.m_Doc->AttributeSync( ref, &old_str, text, i_att );
				}
			}
		}
	}
	delete un_t;
}

void CTextList::MoveOrigin( int x_off, int y_off )
{
	for( int it=0; it<text_ptr.GetSize(); it++ )
	{
		CText * t = text_ptr[it];
		t->m_x += x_off;
		t->m_y += y_off;
		if( t->isVISIBLE )
			if( t->m_str.Right(3) == "BOM" )
				theApp.m_Doc->m_view->DrawBOM( t );
			else
				t->MakeVisible();
	}
}

void CTextList::MarkAllTexts( int m )
{
	for( int it=0; it<text_ptr.GetSize(); it++ )
	{
		CText * t = text_ptr[it];
		t->utility = m;
	}
}

// return text that matches guid
//
CText * CTextList::GetText( GUID * guid )
{
	for( int it=0; it<text_ptr.GetSize(); it++ )
	{
		CText * text = text_ptr[it];
		if( text->m_guid == *guid )
			return text;
	}
	return NULL;
}

CText * CTextList::GetText( int x, int y )
{
	int it = -1;
	CText * text = GetNextText(&it);
	while( text )
	{
		if( text->m_x == x && text->m_y == y )
			return text;
		text = GetNextText(&it);
	}
	return NULL;
}

CText * CTextList::GetText( CString * S, int * it )
{
	int sL = S->GetLength();
	BOOL inc_Multipart = 0;
	if( S->Right(1) == "&" )
	{
		*S = S->Left( sL-1 );
		inc_Multipart = TRUE;
	}
	int in = -1;
	if( it )
		in = *it;
	for( int i=in+1; i<text_ptr.GetSize(); i++ )
	{
		CText * text = text_ptr[i];
		if( text->isVISIBLE )
		{
			if( text->m_str.Compare(*S) == 0 )
			{
				if( it )
					*it = i;
				return text;
			}
			else if( inc_Multipart && text->m_str.GetLength() > sL )
			{
				CString s = text->m_str.Left(sL);
				if( s.CompareNoCase((*S)+".") == 0 || s.CompareNoCase((*S)+"-") == 0 )
				{
					if( it )
						*it = i;
					return text;
				}
			}
		}
	}
	return NULL;
}

void CTextList::CancelSelection()
{
	for( int it=0; text_ptr.GetSize() > it; it++ )
		text_ptr[it]->m_selected = 0;
}

void CTextList::TimeUpdate( CText * text )
{
	if( text->m_str.Right(4) == "DATE" )
	{
		CString str = text->m_str;
		str.MakeUpper();
		int sign = str.Find("YY");
		if( sign > 0 )
		{
			int pStart = str.Find("|");
			if( pStart > 0 && sign > pStart )
			{
				CDisplayList * dl = text->m_dlist;
				SMFontUtil * fu = text->m_smfontutil;
				text->Undraw();
				//
				str = str.Right( str.GetLength() - pStart - 1 );
				CString cpy = str;
				for( int ind=str.Find(" "); ind>=0; ind=str.Find(" ") )
					str.Delete(ind);
				//
				CString s = "~";
				if( str.Find(".") > 0 )
					s = ".";
				else if( str.Find("/") > 0 )
					s = "/";
				else if( str.Find("-") > 0 )
					s = "-";
				//
				// get time
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				//
				CString TD1;
				if( str.Left(1) == "D" )
					TD1.Format( "%d%s", timeinfo->tm_mday, s );
				else if( str.Left(1) == "M" )
					TD1.Format( "%d%s", timeinfo->tm_mon+1, s );
				else if( str.Left(1) == "Y" )
					TD1.Format( "%d%s", timeinfo->tm_year+1900, s );
				//
				if( str.Left(2) == "YY" )
				{
					if( str.Left(4) != "YYYY" )
						TD1.Delete(0,2);
				}
				else if( str.Left(2) != str.Left(1)+s )
				{
					//if( TD1.Left(1) == "0" )
					//	TD1.Delete(0);
					if( TD1.GetLength() == 2 )
						TD1 = "0" + TD1;
				}
				//
				int del = str.Find(s);
				if( del > 0 )
					str = str.Right( str.GetLength() - del - 1 );
				//
				CString TD2;
				if( str.Left(1) == "D" )
					TD2.Format( "%d%s", timeinfo->tm_mday, s );
				else if( str.Left(1) == "M" )
					TD2.Format( "%d%s", timeinfo->tm_mon+1, s );
				else if( str.Left(1) == "Y" )
					TD2.Format( "%d%s", timeinfo->tm_year+1900, s );
				//
				if( str.Left(2) == "YY" )
				{
					if( str.Left(4) != "YYYY" )
						TD2.Delete(0,2);
				}
				else if( str.Left(2) != str.Left(1)+s )
				{
					//if( TD2.Left(1) == "0" )
					//	TD2.Delete(0);
					if( TD2.GetLength() == 2 )
						TD2 = "0" + TD2;
				}
				//
				del = str.Find(s);
				if( del > 0 )
					str = str.Right( str.GetLength() - del - 1 );
				//
				CString TD3;
				if( str.Left(1) == "D" )
					TD3.Format( "%d|", timeinfo->tm_mday );
				else if( str.Left(1) == "M" )
					TD3.Format( "%d|", timeinfo->tm_mon+1 );
				else if( str.Left(1) == "Y" )
					TD3.Format( "%d|", timeinfo->tm_year+1900 );
				//
				if( str.Left(2) == "YY" )
				{
					if( str.Left(4) != "YYYY" )
						TD3.Delete(0,2);
				}
				else if( str.Left(2) != str.Left(1)+s )
				{
					//if( TD3.Left(1) == "0" )
					//	TD3.Delete(0);
					if( TD3.GetLength() == 2 )
						TD3 = "0" + TD3;
				}
				text->m_str = TD1 + TD2 + TD3 + cpy;
				text->Draw( dl, fu );
			}
		}
	}
}

// return next text in CTextList, or NULL if at end of list
//
CText * CTextList::GetNextText( int * it )
{
	if( (*it) >= -1 ) for( (*it)++; text_ptr.GetSize() > (*it); (*it)++ )
	{
		if( text_ptr[(*it)]->isVISIBLE )
			return text_ptr[(*it)]; 
		else
			continue;
	}
	return NULL;
}

CText * CTextList::GetNextAnyText( int * it )
{
	if( (*it) >= -1 ) for( (*it)++; text_ptr.GetSize() > (*it); (*it)++ )
		return text_ptr[(*it)]; 
	return NULL;
}

// return next text in CTextList, or NULL if at end of list
//
CText * CTextList::GetNextInvisibleText( int * it )
{
	if( (*it) >= -1 ) for( (*it)++; text_ptr.GetSize() > (*it); (*it)++ )
	{
		if( text_ptr[(*it)]->isVISIBLE == 0 )
			return text_ptr[(*it)]; 
		else
			continue;
	}
	return NULL;
}

int CTextList::GetNumDifferentTexts( BOOL exclBreak )
{
	int count = 0;
	if( text_ptr.GetSize() )
	{
		int * U = new int[text_ptr.GetSize()];//new021
		for( int i=0; i<text_ptr.GetSize();i++ )
			U[i] = 0;		
		for( int i=0; i<text_ptr.GetSize(); i++ )
		{
			if( U[i] )
				continue;
			if( text_ptr[i]->isVISIBLE )
			{
				if( !exclBreak || text_ptr[i]->m_str.Find("|") <= 0 )
					count++;
				for( int i2=i+1; i2<text_ptr.GetSize(); i2++ )
				{
					if( text_ptr[i2]->isVISIBLE )
					{
						if( text_ptr[i]->m_str.Compare( text_ptr[i2]->m_str ) == 0 )
							U[i2] = 1;
					}
				}
			}
		}
		delete U;//new021
	}
	return count;
}

// get bounding rectangle for all text strings
// return FALSE if no text strings
//
BOOL CTextList::GetTextBoundaries( RECT * r )
{
	BOOL bValid = FALSE;
	RECT br;
	(&br)->bottom = INT_MAX;
	(&br)->left = INT_MAX;
	(&br)->top = INT_MIN;
	(&br)->right = INT_MIN;
	if( m_smfontutil )
	{
		int it = -1;
		CText * t = GetNextText(&it);
		while( t )
		{
			int w = t->m_stroke_width;
			CArray<CPoint> gP;
			if( t->dl_el == NULL )
				t->Draw( 0, m_smfontutil, 0, &gP );
			for( int is=0; is<gP.GetSize(); is+=2 )
			{
				br.bottom = min( br.bottom, gP[is].y - w );
				br.bottom = min( br.bottom, gP[is+1].y - w );
				br.top = max( br.top, gP[is].y + w );
				br.top = max( br.top, gP[is+1].y + w );
				br.left = min( br.left, gP[is].x - w );
				br.left = min( br.left, gP[is+1].x - w );
				br.right = max( br.right, gP[is].x + w );
				br.right = max( br.right, gP[is+1].x + w );	
			}
			if( t->dl_el == NULL )
				t->Undraw();
			bValid = TRUE;
			t = GetNextText(&it);
		}
	}
	*r = br;
	return bValid;
}

// get bounding rectangle for text string
// return FALSE if no text strings
//
BOOL CTextList::GetTextRectOnPCB( CText * t, RECT * r )
{
	BOOL bValid = FALSE;
	if( t->dl_sel )
	{		
		RECT Get;
		m_dlist->Get_Rect( t->dl_sel, &Get );
		(*r).left =		Get.left;
		(*r).right =	Get.right;
		(*r).bottom =	Get.bottom;
		(*r).top =		Get.top;
		bValid = 1;
	}
	return bValid;
}

// reassign copper text to new layers
// enter with layer[] = table of new copper layers for each old copper layer
//
void CTextList::ReassignALayers( int n_new_layers, int * layer )
{
	int it = -1;
	CText * t = GetNextText(&it);
	while( t )
	{
		int old_layer = t->m_layer;
		if( old_layer >= LAY_ADD_1 )
		{
					int index = old_layer - LAY_ADD_1;
					int new_layer = layer[index];
					if( new_layer == old_layer )
					{
						// do nothing
					}
					else if( new_layer == -1 )
					{
						// delete this text
						t->Undraw();
						RemoveText( t );
					}
					else
					{
						// move to new layer
						t->Undraw();
						t->m_layer = new_layer + LAY_ADD_1;
						t->Draw( m_dlist, m_smfontutil ); 
					}
		}
		t = GetNextText(&it);
	}
}

