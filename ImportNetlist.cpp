#include "stdafx.h"
#include "ImportNetlist.h"
#include "math.h"


void Import_PADSPCB_Netlist( CFreePcbDoc * doc, CStdioFile * f )
{
	CString instr;
	f->ReadString( instr );
	instr.Trim();
	if( instr.Compare( "*PADS-PCB*" ) )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "This file is not a PADS-PCB file":"Этот файл не является файлом PADS-PCB", MB_ICONERROR);
		return;
	}
	CText * CurNet = NULL;
	CText * curPart = NULL;
	CText * curFoot = NULL;
	CText * curV = NULL;
	CString keystr;
	CArray<CString> arr;
	int X_NET_SHIFT = -NM_PER_MM*7;
	int Y_NET_SHIFT = 0;
	int X_PART_SHIFT = 0;
	int Y_PART_SHIFT = 0;
	int nParts = 0;
	while( f->ReadString( instr ) )
	{
		instr.Trim();
		int np = ParseKeyString( &instr, &keystr, &arr );
		if( keystr.Left(8) == "*SIGNAL*" && np >= 2 )
		{
			CString mNet = arr.GetAt(0);
			if( np > 2 )
			{
				mNet = instr.Right( instr.GetLength()-8 );
				mNet = mNet.Trim();
			}
			if( mNet.Left(3) == FREENET )
				mNet = "(" + mNet + ")";
			int it = -1;
			CurNet = doc->m_tlist->GetNextInvisibleText( &it );
			if( CurNet == NULL )
				CurNet = doc->m_tlist->AddText( X_NET_SHIFT, Y_NET_SHIFT, 0, LAY_NET_NAME, NM_PER_MM/2, NM_PER_MM/15, doc->m_default_font, &mNet );
			else
			{
				CurNet->m_font_size = NM_PER_MM/2;
				CurNet->m_stroke_width = NM_PER_MM/15;
				CurNet->m_str = arr.GetAt(0);
				CurNet->m_nchars = arr.GetAt(0).GetLength();
				doc->m_tlist->MoveText( CurNet, X_PART_SHIFT, Y_PART_SHIFT, 0, LAY_NET_NAME );
			}
			if( CurNet->m_str.Left(3) != FREENET )
			{
				Y_NET_SHIFT += NM_PER_MM;
				if( Y_NET_SHIFT > (NM_PER_MM*20) )
				{
					Y_NET_SHIFT = 0;
					X_NET_SHIFT -= (NM_PER_MM*7);
				}
			}
			continue;
		}
		if( keystr.Left(5) == "*END*" )
			break;
		if( keystr.Left(6) == "*PART*" )
			continue;
		if( CurNet )
		{
			arr.Add( keystr );
			for( int ip=0; ip<arr.GetSize(); ip++ )
			{
				CString Ps = arr.GetAt(ip);
				int dot = Ps.Find(".");
				if( dot > 0 )
				{
					CString Part = Ps.Left( dot );
					CString Pin = Ps.Right( Ps.GetLength() - dot - 1 );
					int it = -1;
					CText * TPart = doc->Attr->m_Reflist->GetText( &Part, &it );
					if( TPart )
					{
						CText * TPin = GetPartPin( doc->m_outline_poly, TPart, &Pin );
						if( TPin )
						{
							CPolyLine * pin_po = &doc->m_outline_poly->GetAt( TPin->m_polyline_start );
							if( pin_po->pAttr[index_net_attr] == NULL )
								pin_po->pAttr[index_net_attr] = CurNet;
						}
						else
						{
							int sz = doc->m_outline_poly->GetSize();						// First set size!
							doc->m_outline_poly->SetSize( sz+1 );							// First set size!
							doc->m_outline_poly->GetAt(sz).SetDisplayList( doc->m_dlist );	// First set size!
							CPolyLine * outline = &doc->m_outline_poly->GetAt( TPart->m_polyline_start ); // then get outline PTR
							if( outline->GetNumCorners() != 4 )
								ASSERT(0);
							int x0 = outline->GetX(0);
							int y0 = outline->GetY(0);

							// add new pin
							CText * TNet = NULL;
							int npins = doc->GetNumPins( TPart, doc->m_outline_poly );
							int Up = (npins/2)*NM_PER_MM;
							id Id( ID_POLYLINE, ID_GRAPHIC, sz );
							if( npins%2 )
							{
								doc->m_outline_poly->GetAt(sz).Start( LAY_PIN_LINE, NM_PER_MIL*5, NM_PER_MIL, (x0 + NM_PER_MM), (y0 + Up + NM_PER_MM/2), CPolyLine::NO_HATCH, &Id, NULL );
								doc->m_outline_poly->GetAt(sz).AppendCorner( (x0 + NM_PER_MM + NM_PER_MM/5), (y0 + Up + NM_PER_MM/2) );
								TPin = doc->Attr->m_Pinlist->AddText( (x0 + NM_PER_MM + NM_PER_MM/5), (y0 + Up + NM_PER_MM/2 + NM_PER_MM/10), 0, LAY_PIN_NAME, NM_PER_MM/4, NM_PER_MM/30, doc->m_default_font, &Pin );
								//
								if( CurNet )
								{
									TNet = doc->Attr->m_Netlist->AddText(	(x0 + NM_PER_MM + NM_PER_MM/5), 
																			(y0 + Up + NM_PER_MM/10), 
																			0, LAY_NET_NAME, NM_PER_MM/4, NM_PER_MM/30, 
																			doc->m_default_font, &CurNet->m_str );
								}
							}
							else
							{
								doc->m_outline_poly->GetAt(sz).Start( LAY_PIN_LINE, NM_PER_MIL*5, NM_PER_MIL, x0, (y0 + Up + NM_PER_MM/2), CPolyLine::NO_HATCH, &Id, NULL );
								doc->m_outline_poly->GetAt(sz).AppendCorner( (x0 - NM_PER_MM/5), (y0 + Up + NM_PER_MM/2) );
								TPin = doc->Attr->m_Pinlist->AddText( (x0 - NM_PER_MM/5), (y0 + Up + NM_PER_MM/2 + NM_PER_MM/10), 0, LAY_PIN_NAME, NM_PER_MM/4, NM_PER_MM/30, doc->m_default_font, &Pin );
								RECT r;
								if( CurNet )
								{
									TNet = doc->Attr->m_Netlist->AddText(	(x0 - NM_PER_MM/5), 
																			(y0 + Up + NM_PER_MM/10), 
																			0, LAY_NET_NAME, NM_PER_MM/4, NM_PER_MM/30, 
																			doc->m_default_font, &CurNet->m_str );
									// correct pos
									if( doc->Attr->m_Netlist->GetTextRectOnPCB( TNet, &r ) )
									{
										doc->Attr->m_Netlist->MoveText( TNet,	
											TNet->m_x - (r.right-r.left),
											TNet->m_y,
											TNet->m_angle, TNet->m_layer );
									}
								}
								if( doc->Attr->m_Pinlist->GetTextRectOnPCB( TPin, &r ) )
								{
									doc->Attr->m_Pinlist->MoveText( TPin,	
										TPin->m_x - (r.right-r.left),
										TPin->m_y,
										TPin->m_angle, TPin->m_layer );
								}
								outline->SetY( 1, outline->GetY(1) + NM_PER_MM );
								outline->SetY( 2, outline->GetY(2) + NM_PER_MM );
								outline->Draw();
							}		
							doc->m_outline_poly->GetAt(sz).pAttr[index_part_attr] = outline->pAttr[index_part_attr];
							doc->m_outline_poly->GetAt(sz).pAttr[index_foot_attr] = outline->pAttr[index_foot_attr];
							doc->m_outline_poly->GetAt(sz).pAttr[index_value_attr] = outline->pAttr[index_value_attr];
							//doc->m_outline_poly->GetAt(sz).pAttr[index_net_attr] = CurNet;
							doc->m_outline_poly->GetAt(sz).pAttr[index_net_attr] = TNet;
							doc->m_outline_poly->GetAt(sz).pAttr[index_pin_attr] = TPin;
							TPin->m_polyline_start = sz;
							TNet->m_polyline_start = sz;
						}
						doc->AttributeIndexing( TPart, index_part_attr );
					}
				}
			}
		}
		else if( np >= 2 )
		{
			nParts++;
			int sz = doc->m_outline_poly->GetSize();
			doc->m_outline_poly->SetSize( sz+1 );
			doc->m_outline_poly->GetAt(sz).SetDisplayList( doc->m_dlist );
			id Id( ID_POLYLINE, ID_GRAPHIC, sz );
			doc->m_outline_poly->GetAt(sz).Start( LAY_PART_OUTLINE, NM_PER_MIL*5, NM_PER_MIL, X_PART_SHIFT, Y_PART_SHIFT, CPolyLine::NO_HATCH, &Id, NULL );
			doc->m_outline_poly->GetAt(sz).AppendCorner( X_PART_SHIFT, Y_PART_SHIFT, 0, 0 );
			doc->m_outline_poly->GetAt(sz).AppendCorner( X_PART_SHIFT+NM_PER_MM, Y_PART_SHIFT, 0, 0 );
			doc->m_outline_poly->GetAt(sz).AppendCorner( X_PART_SHIFT+NM_PER_MM, Y_PART_SHIFT, 0, 0 );
			doc->m_outline_poly->GetAt(sz).Close();			
			{
				int it = -1;
				curPart = doc->Attr->m_Reflist->GetNextInvisibleText( &it );
				if( curPart == NULL )
					curPart = doc->Attr->m_Reflist->AddText( X_PART_SHIFT, Y_PART_SHIFT-(NM_PER_MM), 0, LAY_PART_NAME, NM_PER_MM/3, NM_PER_MM/15, doc->m_default_font, &keystr );
				else
				{
					curPart->m_font_size = NM_PER_MM/3;
					curPart->m_stroke_width = NM_PER_MM/15;
					curPart->m_str = keystr;
					curPart->m_nchars = keystr.GetLength();
					doc->Attr->m_Reflist->MoveText( curPart, X_PART_SHIFT, Y_PART_SHIFT-(NM_PER_MM), 0, LAY_PART_NAME );
					curPart->MakeVisible();
				}
				curPart->m_polyline_start = sz;
				doc->m_outline_poly->GetAt(sz).pAttr[index_part_attr] = curPart;
			}
			{
				CString foot = NO_FOOTPRINT;
				for( int io=0; io<arr.GetSize(); io++ )
				{
					if( io )
						foot += arr.GetAt(io);
					else
						foot = arr.GetAt(0);
				}
				CString value = "";
				int dog = foot.Find("@");
				if( dog >= 0 )
				{
					value = foot.Left( dog );
					foot = foot.Right( foot.GetLength() - dog - 1 );
				}
				{
					int it = -1;
					curFoot = doc->Attr->m_Footlist->GetNextInvisibleText( &it );
					if( curFoot == NULL )
						curFoot = doc->Attr->m_Footlist->AddText( X_PART_SHIFT, (Y_PART_SHIFT-(NM_PER_MM*2)), 0, LAY_FOOTPRINT, NM_PER_MM/3, NM_PER_MM/15, doc->m_default_font, &foot );
					else
					{
						curFoot->m_font_size = NM_PER_MM/3;
						curFoot->m_stroke_width = NM_PER_MM/15;
						curFoot->m_str = foot;
						curFoot->m_nchars = foot.GetLength();
						doc->Attr->m_Footlist->MoveText( curFoot, X_PART_SHIFT, (Y_PART_SHIFT-(NM_PER_MM*2)), 0, LAY_FOOTPRINT );
						curFoot->MakeVisible();
					}
					curFoot->m_polyline_start = sz;
					doc->m_outline_poly->GetAt(sz).pAttr[index_foot_attr] = curFoot;
				}
				if( value.GetLength() )
				{
					int it = -1;
					curV = doc->Attr->m_Valuelist->GetNextInvisibleText( &it );
					if( curV == NULL )
						curV = doc->Attr->m_Valuelist->AddText( X_PART_SHIFT, (Y_PART_SHIFT-(NM_PER_MM*3)), 0, LAY_PART_VALUE, NM_PER_MM/3, NM_PER_MM/15, doc->m_default_font, &value );
					else
					{
						curV->m_font_size = NM_PER_MM/3;
						curV->m_stroke_width = NM_PER_MM/15;
						curV->m_str = value;
						curV->m_nchars = value.GetLength();
						doc->Attr->m_Valuelist->MoveText( curV, X_PART_SHIFT, (Y_PART_SHIFT-(NM_PER_MM*3)), 0, LAY_PART_VALUE );
						curV->MakeVisible();
					}
					curV->m_polyline_start = sz;
					doc->m_outline_poly->GetAt(sz).pAttr[index_value_attr] = curV;
				}
				else
					curV = NULL;
			}
			doc->AttributeIndexing( curPart, index_part_attr );
		}
	}
	int vpPresent = doc->Pages.VirtualPagesPresent();
	int QnParts = sqrt((float)nParts)*2;
	int counter = 0;
	for( int STEP=0; STEP<4; STEP++ )
	{
		int nPins = 0;
		switch( STEP )
		{
		case 0:
			setbit( nPins, 0 );
			setbit( nPins, 1 );
			break;
		case 1:
			setbit( nPins, 2 );
			break;
		case 2:
			setbit( nPins, 3 );
			setbit( nPins, 4 );
			setbit( nPins, 5 );
			setbit( nPins, 6 );
			setbit( nPins, 7 );
			setbit( nPins, 8 );
			setbit( nPins, 9 );
			setbit( nPins, 10 );
			break;
		case 3:
			break;
		default:
			break;
		}
		if( STEP )
		{
			X_PART_SHIFT = 0;
			Y_PART_SHIFT += (NM_PER_MM*10);
		}
		int it = -1;
		for( CText * t=doc->Attr->m_Reflist->GetNextText(&it); t; t=doc->Attr->m_Reflist->GetNextText(&it) )
		{
			int npins = doc->GetNumPins( t, doc->m_outline_poly );
			if( (getbit( nPins, npins ) && npins <= 10) || (STEP == 3 && npins > 10) )
			{
				doc->m_view->m_fk_button_index = 0;// disable draw bottom pane
				doc->m_view->NewSelectP( t, NULL, !vpPresent );
				doc->SelectGroupAttributes();
				doc->m_view->MoveGroup( X_PART_SHIFT, Y_PART_SHIFT );
				doc->m_view->CancelSelection();
				X_PART_SHIFT += (NM_PER_MM*5);
				if( X_PART_SHIFT > (QnParts*(NM_PER_MM*5)) && STEP != 3 )
				{
					X_PART_SHIFT = 0;
					Y_PART_SHIFT += (NM_PER_MM*5);
				}
				counter++;
				if( counter%5 == 0 )
					if( vpPresent == 0 )
					{
						doc->m_view->OnRangeCmds(NULL);
						doc->m_view->UpdateWindow();
					}
			}
		}
	}
	if( vpPresent == 0 )
		AfxMessageBox(G_LANGUAGE == 0 ? "Netlist successfully imported":"Список эл.цепей успешно импортирован");
	doc->m_view->OnRangeCmds(NULL);
}

void RemoveAllInvisibleElements( CFreePcbDoc * doc )
{
	return;//
	int i=doc->m_outline_poly->GetSize()-1;
	for( ; i>=0; i-- )
	{
		if( doc->m_outline_poly->GetAt(i).m_visible )
			break;
	}
	int start_i = i + 1;
	int count = doc->m_outline_poly->GetSize() - i - 1;
	
	if( count )
	{
		for( i=doc->m_outline_poly->GetSize()-1; i>=start_i; i-- )
		{
			//for( int iatt=0; iatt<num_attributes; iatt++ )
			//{
			//	CText * t = doc->m_outline_poly->GetAt( i ).pAttr[ iatt ];
			//	if( t )
			//		if( t->m_polyline_start == i )
			//		{
			//			if( t->isVISIBLE )
			//				ASSERT(0);
			//			t->m_tl->RemoveText( t );
			//		}
			//}
		}
		doc->m_outline_poly->RemoveAt( start_i, count );
	}
}