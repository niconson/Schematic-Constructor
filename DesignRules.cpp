// Implementation of class DRErrorList
//
#include "stdafx.h"
//******************************************************
//
// class DRError
//
// constructor
//
DRError::DRError()
{
	dl_el = NULL;
	dl_sel = NULL;
}

DRError::~DRError()
{
	if( dl_el )
	{
		dl_el->dlist->Remove( dl_el );
	}
	if( dl_sel )
	{
		dl_sel->dlist->Remove( dl_sel );
	}
}



//******************************************************
//
// class DRErrorList
//
// constructor
//
DRErrorList::DRErrorList()
{
	m_dlist = NULL;
}

DRErrorList::~DRErrorList()
{
	Clear();
}

// Set the DisplayList for error symbols
//
void DRErrorList::SetLists( CDisplayList * dl, CArray<CPolyLine>* poly )
{
	m_dlist = dl;
	m_poly = poly;
}

// clear all entries from DRE list
//
void DRErrorList::Clear()
{
	while( !list.IsEmpty() )
	{
		void * ptr = list.GetHead();
		DRError * dre = (DRError*)ptr;
		delete dre;
		list.RemoveHead();
	}
}

// Remove entry from list
//
void DRErrorList::Remove( DRError * dre )
{
	POSITION pos = list.Find( dre );
	if( pos )
	{
		list.RemoveAt( pos );
		delete dre;
	}
}

// Add error to list
//
DRError * DRErrorList::Add(CString * str, 
						   id id1, id id2,
						   int x1, int y1, 
						   int x2, int y2, 
						   int w, int layer, CText * t1, CText * t2 )
{ 
	POSITION pos;
	void * ptr;

	// find center of coords
	int x = x1;
	int y = y1;
	
#define DRE_MIN_SIZE 50*NM_PER_MIL 
	DRError * dre = new DRError;// add
	
	// make id
	dre->m_id.type = ID_DRC;
	dre->m_id.st = ID_DRE;
	dre->m_id.i = (unsigned int)dre;
	dre->m_id.sst = 0;
	dre->m_id.ii = list.GetSize()+1;
	// set rest of params
	dre->str = *str;
	dre->str.Trim();
	dre->id1 = id1;
	dre->id2 = id2;
	dre->x = x;
	dre->y = y;
	dre->layer = layer;
	dre->text1 = t1;
	dre->text2 = t2;
	//
	if( m_dlist )
	{
		int d = DRE_MIN_SIZE/2;
		d = max( DRE_MIN_SIZE, Distance( x1, y1, x2, y2 ) )/2;
		if( w )
			d = max( d, w/2 );
		RECT rdre;
		rdre.left =   x-d;
		rdre.right =  x+d;
		rdre.bottom = y-d;
		rdre.top =    y+d;
		dre->dl_el = m_dlist->Add(	(void*)dre, dre->m_id, LAY_DRC_ERROR, DL_HOLLOW_CIRC, 1, 
									&rdre, d,  NULL, 0 ); 
		dre->m_id.st = ID_DRE;
		dre->dl_sel = m_dlist->AddSelector( dre->dl_el ); 
	}
	list.AddTail( dre );
#undef DRE_MIN_SIZE
	return dre;
}


// Highlight the error in the layout window
//
void DRErrorList::HighLight( DRError * dre )
{
	m_dlist->HighLight( dre->dl_el );
	if( m_dlist )
	{
	 	if( dre->id1.type == ID_TEXT )
		{
			if( dre->text1 )
				if( dre->text1->dl_el )
					m_dlist->HighLight( dre->text1->dl_el );
		}
		if( dre->id2.type == ID_TEXT )
		{
			if( dre->text2 )
				if( dre->text2->dl_el )
					m_dlist->HighLight( dre->text2->dl_el );
		}
		if( dre->id1.type == ID_POLYLINE )
		{
			if( dre->id1.i < m_poly->GetSize() )
			{
				CPolyLine * p = &(m_poly->GetAt(dre->id1.i));
				if( p->bDrawn )
				{
					for( int ii=0; ii<p->GetNumSides(); ii++ )
						if( p->dl_side[ii] )
							m_dlist->HighLight( p->dl_side[ii] );
				}
			}
		}
		if( dre->id2.type == ID_POLYLINE )
		{
			if( dre->id2.i < m_poly->GetSize() )
			{
				CPolyLine * p = &(m_poly->GetAt(dre->id2.i));
				if( p->bDrawn )
				{
					for( int ii=0; ii<p->GetNumSides(); ii++ )
						if( p->dl_side[ii] )
							m_dlist->HighLight( p->dl_side[ii] );
				}
			}
		}
	}
}

// Make symbol for the error a solid circle with diameter = 250 mils
//
void DRErrorList::MakeSolidCircles(int sw)
{
	POSITION pos;
	void * ptr;

	if( !list.IsEmpty() )
	{
		for( pos = list.GetHeadPosition(); pos != NULL; )
		{
			ptr = list.GetNext( pos );
			DRError * dre = (DRError*)ptr;
			if( dre->dl_el )
			{
				if( dre->dl_el->gtype == DL_HOLLOW_CIRC )
				{
					dre->dl_el->gtype = DL_CIRC;
					RECT Get;
					RECT * r = m_dlist->Get_Rect( dre->dl_el, &Get );
					dre->dl_el->el_w = r->right - r->left;
					//dre->dl_el->transparent = TRANSPARENT_HILITE;
					SwellRect( r, sw);
				}
			}
		}
	}
}

// Make symbol for error a ring 
//
void DRErrorList::MakeHollowCircles()
{
	POSITION pos;
	void * ptr;

	if( !list.IsEmpty() )
	{
		for( pos = list.GetHeadPosition(); pos != NULL; )
		{
			ptr = list.GetNext( pos );
			DRError * dre = (DRError*)ptr;
			if( dre->dl_el )
			{
				if( dre->dl_el->gtype == DL_CIRC )
				{
					dre->dl_el->gtype = DL_HOLLOW_CIRC;
					RECT Get;
					RECT * r = m_dlist->Get_Rect( dre->dl_el, &Get );
					int x = (r->right + r->left)/2;
					int y = (r->top + r->bottom)/2;
					r->left =	x - dre->dl_el->el_w/2;
					r->right =  x + dre->dl_el->el_w/2;
					r->bottom =	y - dre->dl_el->el_w/2;
					r->top =		y + dre->dl_el->el_w/2;
				}
			}
		}
	}
}

void DRErrorList::MoveOrigin( int dx, int dy )
{
	POSITION pos;
	void * ptr;

	if( !list.IsEmpty() )
	{
		for( pos = list.GetHeadPosition(); pos != NULL; )
		{
			ptr = list.GetNext( pos );
			DRError * dre = (DRError*)ptr;
			dre->x += dx;
			dre->y += dy;
			if( dre->dl_el )
			{
				m_dlist->Move( dre->dl_el, dx/m_pcbu_per_wu, dy/m_pcbu_per_wu );
			}
			if( dre->dl_sel )
			{
				m_dlist->Move( dre->dl_sel, dx/m_pcbu_per_wu, dy/m_pcbu_per_wu );
			}
		}
	}
}