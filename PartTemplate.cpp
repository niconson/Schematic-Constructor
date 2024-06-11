#include "stdafx.h"
#include "PartTemplate.h"

void CreateMultiPinPartTemplate( CFreePcbDoc * doc, UINT CMD, int n_pins, int polyline_w, int text_height, int grid_step )
{
	text_height = min( text_height, NM_PER_MM*100/n_pins );
	int old_s = doc->m_outline_poly->GetSize();
	int new_s = old_s + n_pins + 1;
	doc->m_outline_poly->SetSize( new_s );
	int pin_step = text_height - text_height%grid_step + grid_step;
	pin_step *= 2;
	pin_step = max( pin_step, NM_PER_MM*2 );
	RECT outline = rect(0,0,0,0);
	outline.right = max( NM_PER_MM*10, abs(polyline_w)*n_pins*2 );
	outline.right = min( outline.right, NM_PER_MM*30 );
	outline.top = pin_step*(n_pins/2);
	text_height = max( text_height, NM_PER_MM );
	float text_w = (float)doc->m_view->m_attr_size.H_pin/(float)text_height;
	text_w = (float)doc->m_view->m_attr_size.W_pin/text_w;
	id p_id( ID_POLYLINE, ID_GRAPHIC, old_s );
	CPolyLine * p = &doc->m_outline_poly->GetAt( old_s );
	p->SetDisplayList( doc->m_dlist );
	p->Start( LAY_FREE_LINE, abs(polyline_w), 0, 0, 0, 0, &p_id, NULL );
	p->AppendCorner( 0, outline.top );
	p->AppendCorner( outline.right, outline.top );
	p->AppendCorner( outline.right, 0 );
	p->Close();
	for( int i=1; i<(n_pins+1); i++ )
	{
		p_id.i = old_s+i;
		p = &doc->m_outline_poly->GetAt( old_s+i );
		p->SetDisplayList( doc->m_dlist );
		int X = (i<=(n_pins/2)?0:outline.right+(outline.right/10));
		int Y = outline.top - (pin_step*i) + (pin_step/2);
		if( i > (n_pins/2) )
			Y += pin_step*(2*i-n_pins-1);
		p->Start( LAY_FREE_LINE, abs(polyline_w), 0, X, Y, 0, &p_id, NULL );
		p->AppendCorner( X-(outline.right/10), Y );
		// pin attrs
		CText * tPin = NULL;
		CString pin_s;
		pin_s.Format( "%d", i );
		if( i > (n_pins/2) )
		{
			int sh = max( text_height/2, NM_PER_MM );
			tPin = doc->Attr->m_Pinlist->AddText( X-(outline.right/10)+sh, Y+((pin_step-text_height)/2), 0, LAY_PIN_NAME, text_height, text_w, doc->m_default_font, &pin_s );
		}
		else
		{
			int sh = max( text_height/2, NM_PER_MM );
			sh = -sh;
			tPin = doc->Attr->m_Pinlist->AddText( X, Y+((pin_step-text_height)/2), 0, LAY_PIN_NAME, text_height, text_w, doc->m_default_font, &pin_s );
			RECT TR;
			if( doc->Attr->m_Pinlist->GetTextRectOnPCB( tPin, &TR ) )
			{
				sh = TR.right - sh;
				doc->Attr->m_Pinlist->MoveText( tPin, tPin->m_x - sh, tPin->m_y, 0, tPin->m_layer );
			}
		}
		tPin->m_polyline_start = old_s+i;
		p->pAttr[index_pin_attr] = tPin;
	}
	if( doc->m_outline_poly->GetSize() >= (CMD-ID_0_PINS+1) )
	{
		id ID( ID_POLYLINE, ID_GRAPHIC, -1, ID_SIDE, -1 );
		int lim = (doc->m_outline_poly->GetSize()-CMD+ID_0_PINS-2);
		for( int i=doc->m_outline_poly->GetSize()-1; i>lim && i>=0; i-- )
		{
			ID.i = i;
			for( int ii=0; ii<doc->m_outline_poly->GetAt(i).GetNumSides(); ii++ )
			{
				ID.ii = ii;
				doc->m_view->NewSelect( NULL, &ID, 0, 0 );
				if( doc->m_outline_poly->GetAt(i).pAttr[index_pin_attr] )
				{
					id IDT( ID_TEXT_DEF );
					doc->m_view->NewSelect( doc->m_outline_poly->GetAt(i).pAttr[index_pin_attr], &IDT, 0, 0 );
					doc->m_outline_poly->GetAt(i).pAttr[index_pin_attr]->isVISIBLE = 0;
				}
			}
			doc->m_outline_poly->GetAt(i).m_visible = 0;
		}
		doc->m_view->SaveUndoInfoForGroup( doc->m_undo_list );
		for( int i=doc->m_outline_poly->GetSize()-1; i>lim && i>=0; i-- )
		{
			doc->m_outline_poly->GetAt(i).m_visible = 1;
			if( doc->m_outline_poly->GetAt(i).pAttr[index_pin_attr] )
				doc->m_outline_poly->GetAt(i).pAttr[index_pin_attr]->isVISIBLE = 1;
		}
	}
}