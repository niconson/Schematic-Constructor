#include "stdafx.h"
#include "Pages.h"
#include "TagTable.h"
extern CFreeasyApp theApp;



//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CPageList::CPageList()
{
	m_num_pages = 0;
	max_num_pages = 0;
	pgs.SetSize( max_num_pages );
	ThisPage = NULL;
	ThisNumber = 0;
	PolyLines = NULL;
	Attributes = NULL;
	TextList = NULL;
	DisplayList = NULL;
	DREList = NULL;
	UndoList = NULL;
	RedoList = NULL;
	p_sel_id = NULL;
	p_sel_text = NULL;
	p_cursor_mode = NULL;
	//
	if( MAX_PAGES != 8 )
		ASSERT(0);
	//
	// reset netlist page mask
	for( int i=0; i<MAX_PAGES; i++ )
		m_netlist_page_mask[i] = ~0;
	//
	m_netlist_format = 0;
	m_page_mirror_mask = 1;
	m_get_suffixes = 0;
	m_get_prefixes = "";
	///for( int i=0; i<MAX_PAGES; i++ )
	///	m_block_suffixes[i].RemoveAll();
	InvalidatePcbIndexes();
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CPageList::~CPageList()
{
	ThisNumber = 0;
	m_num_pages = 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::Assignment(	CArray<CPolyLine> ** polylines,
							CAttrib ** attr,
							CTextList ** t_list,
							CDisplayList ** d_List,
							DRErrorList ** dre_List,
							CUndoList ** UList,
							CUndoList ** RList,
							CText ** sel_text,
							id * sel_id,
							int * cur_mode,
							int max_n_pages,
							SMFontUtil * smfont )
{
	max_num_pages = max_n_pages;
	p_cursor_mode = cur_mode;
	p_sel_id = sel_id;
	p_sel_text = sel_text;
	RedoList = RList;
	UndoList = UList;
	DREList = dre_List;
	DisplayList = d_List;
	TextList = t_list;
	Attributes = attr;
	PolyLines = polylines;
	m_smfontutil = smfont;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::Next()
{
	return SwitchToPage( ThisNumber+1 );
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::Prev()
{
	return SwitchToPage( ThisNumber-1 );
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::ClearAll()
{
	for( int i=pgs.GetSize()-1; i>=0; i-- )
		Clear(i);
	InvalidatePcbIndexes();
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::RemoveAll()
{
	for( int i=pgs.GetSize()-1; i>=0; i-- )
		Remove(i);
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::AddNew( CString * Name, BOOL insert )
{
	int num = pgs.GetSize();
	if( num < max_num_pages )
	{	
		CFreeasyPage newpgs;
		newpgs.DisplayList = new CDisplayList();
		newpgs.Attributes = new CAttrib;
		newpgs.Attributes->m_Reflist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.Attributes->m_Footlist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.Attributes->m_Valuelist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.Attributes->m_Pinlist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.Attributes->m_Netlist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.Attributes->m_pDesclist =	new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.TextList = new CTextList( newpgs.DisplayList, m_smfontutil );
		newpgs.PolyLines = new CArray<CPolyLine>;
		newpgs.PolyLines->RemoveAll();
		newpgs.DREList = new DRErrorList;
		newpgs.DREList->SetLists( newpgs.DisplayList, newpgs.PolyLines );
		newpgs.UndoList = new CUndoList( 9999, 35 );
		newpgs.RedoList = new CUndoList( 9999, 35 );
		newpgs.p_sel_text = NULL;
		newpgs.p_cursor_mode = CUR_NONE_SELECTED;
		newpgs.name = *Name;
		if( insert == 0 || ThisNumber < 0 || ThisNumber >= num )
			pgs.Add( newpgs );
		else
			pgs.InsertAt( ThisNumber, newpgs );
		m_num_pages++;
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::VirtualPagesPresent()
{
	if( m_num_pages != pgs.GetSize() )
		return 1; // already present
	else
		return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::CreateVirtualPages()
{
	if( VirtualPagesPresent() )
		return 0; // already present
	max_num_pages += 2;
	CString VirtualName = "*VP1#";
	m_num_pages -= AddNew( &VirtualName );
	VirtualName = "*VP2#";
	m_num_pages -= AddNew( &VirtualName );

	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::DeleteVirtualPages()
{
	if( VirtualPagesPresent() == 0 )
		return 0; // not found
	Clear( pgs.GetSize()-1 );
	Clear( pgs.GetSize()-2 );
	Remove( pgs.GetSize()-1 );
	Remove( pgs.GetSize()-1 );
	max_num_pages -= 2;
	m_num_pages += 2;

	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::SwitchToVP1()
{
	if( VirtualPagesPresent() == 0 )
		return 0; // not found
	return SwitchToPage( pgs.GetSize()-2 );
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::SwitchToVP2()
{
	if( VirtualPagesPresent() == 0 )
		return 0; // not found
	return SwitchToPage( pgs.GetSize()-1 );
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::Clear( int index )
{
	pgs[index].Attributes->m_Reflist->RemoveAllTexts();
	pgs[index].Attributes->m_Footlist->RemoveAllTexts();
	pgs[index].Attributes->m_Valuelist->RemoveAllTexts();
	pgs[index].Attributes->m_Pinlist->RemoveAllTexts();
	pgs[index].Attributes->m_Netlist->RemoveAllTexts();
	pgs[index].Attributes->m_pDesclist->RemoveAllTexts();
	pgs[index].TextList->RemoveAllTexts();
	pgs[index].DREList->Clear();
	pgs[index].PolyLines->RemoveAll();
	pgs[index].UndoList->Clear();
	pgs[index].RedoList->Clear();
	pgs[index].DisplayList->RemoveAll();
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::Remove( int index )
{
	delete pgs[index].Attributes->m_Reflist;
	delete pgs[index].Attributes->m_Footlist;
	delete pgs[index].Attributes->m_Valuelist;
	delete pgs[index].Attributes->m_Pinlist;
	delete pgs[index].Attributes->m_Netlist;
	delete pgs[index].Attributes->m_pDesclist;
	delete pgs[index].Attributes;
	delete pgs[index].TextList;
	delete pgs[index].DREList;
	delete pgs[index].PolyLines;
	delete pgs[index].UndoList;
	delete pgs[index].RedoList;
	delete pgs[index].DisplayList;
	pgs.RemoveAt( index );
	if( ThisNumber == index || ThisNumber >= pgs.GetSize() )
	{
		ThisNumber = 0;
		if( pgs.GetSize() )
			SwitchToPage(0);
	}
	m_num_pages--;
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::Rename( int index, CString * Name )
{
	pgs[index].name = *Name;
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::SwitchToPage( int page_index )
{
	pgs[ThisNumber].p_sel_id = *p_sel_id;
	pgs[ThisNumber].p_sel_text = *p_sel_text;
	pgs[ThisNumber].p_cursor_mode = *p_cursor_mode;
	if( page_index >= 0 && page_index < pgs.GetSize() )
	{
		ThisNumber = page_index;
		ThisPage = &pgs[page_index];
		*PolyLines = pgs[page_index].PolyLines;
		*Attributes = pgs[page_index].Attributes;
		*TextList = pgs[page_index].TextList;
		*DisplayList = pgs[page_index].DisplayList;
		*DREList = pgs[page_index].DREList;
		*UndoList = pgs[page_index].UndoList;
		*RedoList = pgs[page_index].RedoList;
		*p_sel_id = pgs[page_index].p_sel_id;
		*p_sel_text = pgs[page_index].p_sel_text;
		*p_cursor_mode = pgs[page_index].p_cursor_mode;
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::ReturnPageByPCB( CString * Name )
{
	int pcb = -1;
	for( int i=0; i<m_pcb_names.GetSize(); i++ )
	{
		if( Name->Compare( m_pcb_names.GetAt(i) ) == 0 )
		{
			pcb = i;
			break;
		}
	}
	if( pcb >= 0 )
	{
		for( int i=0; i<MAX_PAGES; i++ )
		{
			if( getbit( m_netlist_page_mask[pcb], i ) )
				return i;
		}
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::IsThePageIncludesCP( int cppg )
{
	static BOOL WarShown = 0;
	if( cppg == -1 )
		cppg = ThisNumber;
	int it = -1; 
	int ip = -1;
	while( CText * t=FindBLK_PTR( &it, &ip, TRUE ) )
	{
		int iBlkPage = theApp.m_Doc->ExtractBLK_PTR( &t->m_str );
		if( iBlkPage == cppg )
			if( ip < cppg )
				return ip;
			else if( WarShown == 0 )
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"A complex part cannot link to a page with a lower index! Place complex parts on the last pages":
					"Составная деталь не может ссылаться на страницу с более низким индексом! Размещайте иерархические части на последних страницах", MB_ICONERROR);
				WarShown = 1;
			}
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindMerge( int index )
{
	for( int np=0; np<pgs.GetSize(); np++ )
	{
		for( int io=0; io<pgs[np].PolyLines->GetSize(); io++ )
		{
			if( pgs[np].PolyLines->GetAt(io).GetMerge() == index )
				return np;
		}
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::ShiftPrintableAreaIndexes( id * pID, int di )
{
	CArray<int> ids;
	id farea = *pID;
	do
	{
		farea = FindPrintableArea( &farea, !ids.GetSize() );
		if( farea.i >= 0 )
			ids.Add( farea.i );
	}while( farea.i != -1 );
	for( int i=0; i<ids.GetSize(); i++ )
	{
		CPolyLine * p = &pgs[ThisNumber].PolyLines->GetAt(ids.GetAt(i));
		if( CText * cmd = p->Check( index_desc_attr ) )
		{
			int fa = cmd->m_str.Find("PrintableArea");
			if( fa == cmd->m_str.GetLength()-13 && fa != -1 )
			{
				CString pref, suff;
				int gi = ParseRef( &cmd->m_str, &pref, &suff );
				cmd->m_str.Format("%s%d%s", pref, gi+di, suff );
				cmd->m_nchars = cmd->m_str.GetLength();
				cmd->MakeVisible();
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetPrintableAreaIndex( CString * Part )
{
	CText * Found = NULL;
	int ip = FindPart( Part, &Found, TRUE, TRUE );
	if( ip == -1 )
	{
		ip = ThisNumber;
		int id_el = -1;
		Found = FindCPartElement( Part, &ip, &id_el );
	}
	if( Found )
	{
		int mThisNumber = ThisNumber;
		ThisNumber = ip;
		id print_area;
		do
		{
			print_area = FindPrintableArea( &print_area );
			if( print_area.i >= 0 )
			{
				CPolyLine * p = &pgs[ThisNumber].PolyLines->GetAt(print_area.i);
				RECT R = p->GetBounds();
				if( InRange( Found->m_x, R.left, R.right ) &&
					InRange( Found->m_y, R.bottom, R.top ) )
				{
					ThisNumber = mThisNumber;
					return print_area.sst;
				}
			}
		} while( print_area.i >= 0 );
		ThisNumber = mThisNumber;
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
id CPageList::FindPrintableArea( id * prevID, BOOL bIncludeSameIndex )
{
	id out = *prevID;
	out.i = -1;
	int INDEX = INT_MAX;
	int mINDEX = 0;
	int MAX_INDEX = 0;
	if( prevID->i >= 0 )
	{
		CPolyLine * p = &pgs[ThisNumber].PolyLines->GetAt(prevID->i);
		if( CText * cmd = p->Check( index_desc_attr ) )
		{
			int fa = cmd->m_str.Find("PrintableArea");
			if( fa == cmd->m_str.GetLength()-13 && fa != -1 )
			{
				CString pref, suff;
				mINDEX = ParseRef( &cmd->m_str, &pref, &suff );
			}
		}
	}
	int cnt_same = 0; // same index
	for(int ipp=0; ipp<pgs[ThisNumber].PolyLines->GetSize(); ipp++ )
	{
		if( ipp == prevID->i )
			continue;
		CPolyLine * p = &pgs[ThisNumber].PolyLines->GetAt(ipp);
		if( p->GetNumCorners() < 3 )
			continue;
		if( CText * cmd = p->Check( index_desc_attr ) )
		{
			int fArea = cmd->m_str.Find("PrintableArea");
			if( fArea >= 0 && fArea == cmd->m_str.GetLength()-13 ) // !!!
			{
				CString pref, suff;
				int I = ParseRef( &cmd->m_str, &pref, &suff );
				MAX_INDEX = max( I, MAX_INDEX );
				
				if( I <= mINDEX )
				{
					if( I == mINDEX )
					{
						cnt_same++;
						if( bIncludeSameIndex == 0 )
							continue;
					}
					else
						continue;
				}
				if( I > INDEX )
					continue;
				else
				{
					INDEX = I;
					out.i = ipp;
				}
			}
		}
	}
	if( out.type == ID_NONE )
	{
		out.ii = MAX_INDEX;
		out.sst = INDEX;
		out.st = cnt_same;
		out.type = mINDEX;
	}
	return out;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindBlkPtrs( int polylineIndex, BOOL bSelect )
{
	// функция которая создает суффиксы пинов
	// комплексной детали из обычных пинов
	// у контурной линии и выделяет эти пины
	CArray<CPolyLine> * pa = GetPolyArray( ThisNumber );
	RECT R = pa->GetAt( polylineIndex ).GetBounds();
	CText * dt = pa->GetAt( polylineIndex ).Check( index_desc_attr );
	if( !dt )
		return polylineIndex;
	CString SUFF, REF;
	int IP = theApp.m_Doc->ExtractBLK( &dt->m_str, &SUFF, &REF );
	if( IP < 0 )
		IP = 0;
	if( REF.GetLength() == 0 )
		REF = "xx";
	for( int i=0; i<pa->GetSize(); i++ )
	{
		if( i == polylineIndex )
			continue;
		if( pa->GetAt(i).Check( index_desc_attr ) )
			continue;
		if( pa->GetAt(i).Check( index_part_attr ) == NULL )
			if( pa->GetAt(i).Check( index_pin_attr ) )
			{
				RECT pinR = pa->GetAt(i).GetBounds();
				if( RectsIntersection( pinR, R ) >= 0 )
				{
					if( bSelect )
					{
						id sid( ID_POLYLINE, ID_GRAPHIC, i, ID_SIDE, 0 );
						id tsid( ID_TEXT_DEF );
						theApp.m_Doc->m_view->NewSelect( NULL, &sid, 0, 0 );
						CText * pinname = pa->GetAt(i).pAttr[ index_pin_attr ];
						CString CMDSTR;
						CMDSTR.Format("%s'"CP_SUFFIX" %s'"CP_LINK" %s'|pin_name: %s'"CP_PAGE_NUM" %d'"COMMAND" BLK_PTR", pinname->m_str, SUFF, REF, pinname->m_str, IP+1 );
						CText * descA = pgs[ThisNumber].Attributes->m_pDesclist->AddText( pinname->m_x, 
						pinname->m_y, pinname->m_angle, LAY_PIN_DESC, pinname->m_font_size, 
						pinname->m_stroke_width, pinname->m_font_number, &CMDSTR, 0 );
						pa->GetAt(i).pAttr[ index_desc_attr ] = descA;
						descA->m_polyline_start = i;
						theApp.m_Doc->m_view->NewSelect( descA, &tsid, 0, 0 );
					}
					else
					{
						pa->GetAt(i).pAttr[ index_pin_attr ]->InVisible();
						pa->GetAt(i).pAttr[ index_desc_attr ]->MakeVisible();
					}
				}
			}
	}
	return polylineIndex;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::GetText( CString * str )
{
	for( int ia=0; ia<num_attributes; ia++ )
	{
		int it = -1;
		int ip = ThisNumber;
		for( CText * t=GetNextAttr( ia, &it, &ip ); t; t=GetNextAttr( ia, &it, &ip ) )
		{
			if( ip != ThisNumber )
				break;
			if( str->Compare( t->m_str ) == 0 )
				return t;
		}
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CString CPageList::GetPicture( CString * V, CString * P )
{
	CText * Part = NULL;
	CString PicFile = "";
	int pg = 0;
	if( FindPart( V, P, TRUE, NULL, &Part, &pg ) )
	{
		CPolyLine * p = NULL;
		for( int index=Part->m_polyline_start; index>=0; index=p->Next[index_part_attr] )
		{
			p = &pgs[pg].PolyLines->GetAt( index );
			int pic = p->GetPicture();
			if( pic >= 0 )
			{
				int vis=0, f=0;
				pgs[pg].DisplayList->GetTemplate( pic, NULL, NULL, NULL, &vis, &f );
				if( vis == 0 )
					continue;
				if( f == BMP )
					continue;
				int isrc = pgs[pg].DisplayList->GetSourceIndex( pic );
				if( isrc >= 0 )
					PicFile = pgs[pg].DisplayList->GetSource( isrc );
			}
		}
	}
	return PicFile;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindPart( CString * V, CString * P, BOOL bRemoveIllegalSymbols, CArray<CText*> * ExText, CText ** FoundPart, int * ipage )
{
	int it = -1; 
	int ip = -1;
	if( ipage )
		ip = *ipage;
	while( CText * tV = GetNextAttr( index_value_attr, &it, &ip ) )
	{
		BOOL bEx = 0;
		if( ExText ) 
			for( int i=0; i<ExText->GetSize(); i++ )
				if( tV == ExText->GetAt(i) )
				{
					bEx = 1;
					break;
				}
		if( bEx )
			continue;
		CString v = tV->m_str;
		if( bRemoveIllegalSymbols )
			ExtractComponentName( &v, NULL );
		if( V->Left(1) != v.Left(1) )
			continue;
		if( V->Compare( v ) )
			continue;
		int start = tV->m_polyline_start;
		if( start < 0 )
			continue;
		CArray<CPolyLine> * po = pgs[ip].PolyLines;
		CPolyLine * poly = &po->GetAt( start );
		CText * tF = poly->pAttr[ index_foot_attr ];
		if( tF )
		{
			CString f = tF->m_str;
			if( bRemoveIllegalSymbols )
				ExtractComponentName( &f, NULL );
			if( P->Left(1) != f.Left(1) )
				continue;
			if( P->Compare( f ) )
				continue;
			if( FoundPart )
				*FoundPart = poly->Check( index_part_attr );
			if( ipage )
				*ipage = ip;
			return 1;
		}
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindPart( CString * Ref, CText ** found, BOOL StartOver, BOOL use_netlist_page_mask, int it, int ip )
{
	static int current__page = 0;
	static int current__it = -1;
	if( StartOver )
	{
		current__page = max(ip,0);
		current__it = max(it,-1);
	}
	int sL = Ref->GetLength();
	BOOL inc_Multipart = 0;
	CString ref = *Ref;
	if( Ref->Right(1) == "&" )
	{
		ref = Ref->Left( sL-1 );
		inc_Multipart = TRUE;
	}
	for( ; current__page<pgs.GetSize(); current__page++ )
	{
		if( use_netlist_page_mask )
			if( IsThePageIncludedInNetlist( current__page ) == 0 )
				continue;
		for( CText * nt=pgs[current__page].Attributes->m_Reflist->GetNextText(&current__it); nt; nt=pgs[current__page].Attributes->m_Reflist->GetNextText(&current__it) )
		{
			if( nt->m_str.CompareNoCase( ref ) == 0 )
			{
				if( found )
					*found = nt;
				return current__page;
			}
			else if( inc_Multipart && nt->m_str.GetLength() > sL )
			{
				CString s = nt->m_str.Left(sL);
				if( s.CompareNoCase(ref+".") == 0 || s.CompareNoCase(ref+"-") == 0 )
				{
					if( found )
						*found = nt;
					return current__page;
				}
			}
		}
		current__it = -1;
	}
	current__page = MAX_PAGES;
	if( found )
		*found = NULL;
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::FindCPartElement( CString * FullRefDes, int * ipage, int * start_i, BOOL bFoundBlkPtr )
{
	if( ipage == NULL || FullRefDes == NULL )
		return NULL;
	if( *ipage < 0 )
		return NULL;

	CString SFX, PRF;
	ParseRef( FullRefDes, &PRF, &SFX );
	int it = -1;
	int ip = -1;
	for( CText * t=FindBLK( &it, &ip, TRUE );
				 t;
				 t=FindBLK( &it, &ip, TRUE ) )
	{
		if( GetPcbIndex(ip,0) != GetPcbIndex(*ipage,1) )
			continue;
		
		CString sfx;
		theApp.m_Doc->ExtractBLK( &t->m_str, &sfx );
		if( SFX.GetLength() < sfx.GetLength() )
			continue;
		if( SFX.Find( sfx ) >= 0 )
		{
			int blkNum = 0;
			CString Destination, ASuff, FullRef = *FullRefDes;
			int cpp = GetBlockParts( &Destination, t, NL_PADS_PCB, &blkNum, NULL, &ASuff, &FullRef );
			if( cpp >= 0 )
			{
				CString refDes = FullRef;
				int it2 = *start_i;
				CText * fPart = pgs[cpp].Attributes->m_Reflist->GetText( &refDes, &it2 );
				if( fPart == NULL )
				{
					refDes += "&";
					it2 = *start_i;
					fPart = pgs[cpp].Attributes->m_Reflist->GetText( &refDes, &it2 );
					if( fPart )
						*start_i = it2;
					else
						continue;
				}
				else 
					*start_i = it2;
				
				// return complex part ptr
				if( bFoundBlkPtr )
				{
					*ipage = ip;
					return t;
				}

				// return part inside CP
				*ipage = cpp;
				return fPart;
			}
		}
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetLastFreeNetIndex( BOOL use_netlist_page_mask )
{
	int max_num = 0;
	for( int ipg=0; ipg<pgs.GetSize(); ipg++ )
	{
		if( use_netlist_page_mask )
			if( IsThePageIncludedInNetlist( ipg ) == 0 )
				continue;
		int it = -1;
		for( CText * nt=pgs[ipg].TextList->GetNextText(&it); nt; nt=pgs[ipg].TextList->GetNextText(&it) )
		{
			if( nt->m_str.Left(3) == FREENET )
			{
				CString pref;
				int num = ParsePin( &nt->m_str, &pref );
				if( num > max_num )
					max_num = num;
			}
		}
	}
	return max_num;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetSelCount( int page_index, int * flags, BOOL TEST )
{
	if( flags && TEST == 0 )
		*flags = 0;
	int cnt = 0;
	for( int i=pgs[page_index].PolyLines->GetSize()-1; i>=0; i-- )
	{
		CPolyLine * p = &pgs[page_index].PolyLines->GetAt(i);
		//if( p->m_visible == 0 )
		//	continue; !
		int gns = p->GetNumCorners()-1;
		int cl = p->GetClosed();
		for( int ii=gns; ii>=0; ii-- )
		{
			if( p->GetSel(ii) )
			{
				if(TEST)
					return 1;
				cnt++;
				if( flags )
				{
					setbit( *flags, CFreePcbView::FLAG_SEL_OP );
					setbit( *flags, CFreePcbView::FLAG_SEL_VTX );
					if( p->Check( index_part_attr ) )
						setbit( *flags, CFreePcbView::FLAG_SEL_PART );
					if( p->Check( index_pin_attr ) )
						setbit( *flags, CFreePcbView::FLAG_SEL_PIN );
				}
			}
			if( cl == 0 && ii == gns )
				continue;
			if( p->GetSideSel(ii) )
			{
				if(TEST)
					return 1;
				cnt++;
				if( flags )
				{
					setbit( *flags, CFreePcbView::FLAG_SEL_OP );
					setbit( *flags, CFreePcbView::FLAG_SEL_SIDE );
					if( p->Check( index_part_attr ) )
						setbit( *flags, CFreePcbView::FLAG_SEL_PART );
					if( p->Check( index_pin_attr ) )
						setbit( *flags, CFreePcbView::FLAG_SEL_PIN );
				}
			}
		}
	}
	int mem = cnt;
	cnt += pgs[page_index].Attributes->m_Reflist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].Attributes->m_Footlist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].Attributes->m_Valuelist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].Attributes->m_Netlist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].Attributes->m_Pinlist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].Attributes->m_pDesclist->GetSelCount();
	if(TEST&&cnt)
		return 1;
	cnt += pgs[page_index].TextList->GetSelCount();
	if( flags )
		if( mem != cnt )
			setbit( *flags, CFreePcbView::FLAG_SEL_TEXT );
	return cnt;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::IsAttr( CText * t )
{
	for( int ipg=0; ipg<pgs.GetSize(); ipg++ )
	{
		if( t->m_tl == pgs[ipg].Attributes->m_Reflist )
			return index_part_attr;
		if( t->m_tl == pgs[ipg].Attributes->m_Valuelist )
			return index_value_attr;
		if( t->m_tl == pgs[ipg].Attributes->m_Footlist )
			return index_foot_attr;
		if( t->m_tl == pgs[ipg].Attributes->m_Pinlist )
			return index_pin_attr;
		if( t->m_tl == pgs[ipg].Attributes->m_Netlist )
			return index_net_attr;
		if( t->m_tl == pgs[ipg].Attributes->m_pDesclist )
			return index_desc_attr;
	}
	return -1;
}
int CPageList::CheckTemplate( CString * src )
{
	for( int ipg=0; ipg<pgs.GetSize(); ipg++ )
	{
		for( int t=0; t<pgs[ipg].DisplayList->GetNumTemplates(); t++ )
		{
			CString s="";
			pgs[ipg].DisplayList->GetTemplate( t, &s );
			int fi = s.ReverseFind('\\');
			s = s.Right( s.GetLength() - fi - 1 );
			if( s.CompareNoCase(*src) == 0 )
				return ipg;
		}
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetNetPages( CText * net, int * pageMask, int CurrentPage )
{
	int ipcb = GetPcbIndex( CurrentPage );
	*pageMask = 0;
	for( int ipg=0; ipg<pgs.GetSize(); ipg++ )
	{
		if( GetPcbIndex(ipg) != ipcb )
			continue;
		int it = -1;
		if( pgs[ipg].Attributes->m_Netlist->GetText( &net->m_str, &it ) )
			setbit( *pageMask, ipg );
	}
	int Mask = *pageMask;
	clrbit( Mask, CurrentPage );
	if( Mask == 0 )
		return 0;
	else 
		return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetBOMCount( CString * Value, CString * Package, CString * Details, int detail_column_width, int * DLen, int inc_list, int ex_list, CString * LINK, CString * SUFF, int ip )
{
	int CNT = 0;
	int prev_page = ThisNumber;
	RECT Rpart = rect(0,0,0,0);
	if( LINK && ip >= 0 )
	{
		int it = -1;
		CText * ptr = pgs[ip].Attributes->m_Reflist->GetText( LINK, &it );
		if( ptr )
		{
			if( ThisNumber != ip )
				theApp.m_Doc->SwitchToPage(ip);
			Rpart = theApp.m_Doc->GetPartBounds( ptr, NULL );
		}
		else return 0;
	}
	for( int ipg=max(0,ip); ipg<pgs.GetSize(); ipg++ )
	{
		if( IsThePageIncludedInNetlist( ipg ) == 0 && ip == -1 )
			continue;
		//
		int it = -1;
		for( CText * P=pgs[ipg].Attributes->m_Footlist->GetText(Package,&it); P; P=pgs[ipg].Attributes->m_Footlist->GetText(Package,&it) )
		{
			if( P->m_polyline_start >= 0 )
			{
				CPolyLine * p = &pgs[ipg].PolyLines->GetAt(P->m_polyline_start);
				if( CText * part = p->Check( index_part_attr ) )
				{
					if( inc_list > 0 )
						if( getbit( part->m_pdf_options, inc_list ) == 0 )
							continue;
					if( ex_list > 0 )
						if( getbit( part->m_pdf_options, ex_list ) )
							continue;
					CString Vstr = "no value";
					CText * V = p->Check( index_value_attr );
					if( V )
						Vstr = V->m_str;
					if( Vstr.Compare(*Value) == 0 )
					{
						// if LINK
						if( LINK )
						{
							RECT getR = theApp.m_Doc->GetPartBounds( part, NULL );
							if( RectsIntersection( Rpart, getR ) < 0 || getR.right - getR.left > Rpart.right - Rpart.left )
								continue;
						}
						//
						BOOL add_det = 1;
						CString Ref = part->m_str;
						int dot = Ref.Find(".");
						if( dot <= 0 )
							dot = Ref.Find("-");
						if( dot > 0 )
						{
							Ref = Ref.Left(dot);
							if( SUFF )
								Ref += *SUFF;
							if( Details->Compare(Ref) == 0 )
								Ref = "";
							else if( Details->Left(Ref.GetLength()+1).Compare(Ref+",") == 0 )
								Ref = "";
							else if( Details->Right(Ref.GetLength()+1).Compare(" "+Ref) == 0 )
								Ref = "";
							else if( Details->Find(" "+Ref+",") > 0 )
								Ref = "";
						}
						else if( SUFF )
							if( Ref.GetLength() )
								Ref += *SUFF;
						//
						if( Ref.GetLength() )
						{
							CNT++;
							if( Details->GetLength() )
							{
								*Details += ", ";
								*DLen += 2;
							}
							*DLen += part->m_str.GetLength();
							if( SUFF )
								*DLen += SUFF->GetLength();
							int added_cols = ((detail_column_width-10)/7) + 2;
							if( *DLen > (added_cols*8) && Details->GetLength() )
							{
								*Details += "'";
								*DLen = 0;
							}
							*Details += Ref;	
							///if( SUFF ) already applied
							///	*Details += *SUFF;
						}
					}
				}
			}
		}
		if( ip >= 0 )
			break;
	}
	if( ThisNumber != prev_page )
		theApp.m_Doc->SwitchToPage(prev_page);
	return CNT;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindBOMNextLine( CString * Value, 
								CString * Package, 
								CString * Details, 
								int detail_column_width, 
								BOOL ignore_without_value, 
								int inc_list, 
								int ex_list, 
								BOOL StartOver, 
								int * CPPresent )
{
	static int cur_page = 0;
	static int cur_it = -1;
	static CArray<CString> B_SUFF;
	static CArray<CString> B_LINK;
	static CArray<int> B_PAGENUMB;
	static CString CASH;
	if( StartOver )
	{
		cur_page = 0;
		cur_it = -1;
		CASH = "    ";
		CString B_CASH = dSPACE;
		// find blocks
		B_SUFF.SetSize(0);
		B_LINK.SetSize(0);
		B_PAGENUMB.SetSize(0);

		int ipg[MAX_PAGES];
		int it[MAX_PAGES];
		CString ASUFF[MAX_PAGES];
		for( int iL=0; iL<MAX_PAGES; iL++ )
		{
			ipg[iL] = 0;
			it[iL] = -1;
			ASUFF[iL] = "";
		}
		int cur_level = 0;
		for( ipg[cur_level]=0; ipg[cur_level]<pgs.GetSize(); ipg[cur_level]++ )
		{
			if( IsThePageIncludedInNetlist( ipg[cur_level] ) == 0 )
				continue;
			
			BOOL Flag;
			do
			{
				Flag = FALSE;
				for( CText* D=pgs[ipg[cur_level]].Attributes->m_pDesclist->GetNextText(&it[cur_level]); 
							D; 
							D=pgs[ipg[cur_level]].Attributes->m_pDesclist->GetNextText(&it[cur_level]) )
				{
					if( D->m_str.Right(3) == "BLK" )
					{
						CString NAME, REF;
						int np = theApp.m_Doc->ExtractBLK( &D->m_str, &NAME, &REF );
						if( IsThePageIncludedInNetlist( np ) )
						{
							// деталь ссылается на страницу вкл в нетлист
							AfxMessageBox(G_LANGUAGE == 0 ? 
								"Error! The complex part links to a page included in the netlist":
								"Ошибка! Составная деталь ссылается на страницу, включенную в какой-либо список эл.цепей, что недопустимо", MB_ICONERROR);
							continue;
						}
						CString pgName;
						pgName.Format("(%d)", np );
						CString nmstr = (REF + NAME + ASUFF[cur_level] + pgName + dSPACE);
						if( B_CASH.Find( dSPACE + nmstr ) == -1 )
						{
							B_CASH += nmstr;
							B_SUFF.Add(NAME+ASUFF[cur_level]);
							B_LINK.Add(REF);
							B_PAGENUMB.Add(np);
							cur_level++;
							ipg[cur_level] = np;
							ASUFF[cur_level] = NAME + ASUFF[cur_level-1];
							Flag = TRUE;
							break;
						}
					}
				}
				if( Flag == 0 )
				{
					it[cur_level] = -1;
					cur_level--;
				}
			}while( cur_level >= 0 );
			cur_level = 0;
		}
		if( CPPresent )
			if( B_LINK.GetSize() )
				*CPPresent = 1;
	}
	for( int ipg=cur_page; ipg<pgs.GetSize(); ipg++ )
	{
		if( IsThePageIncludedInNetlist( ipg ) == 0 )
			continue;
		//
		int it = cur_it;
		for( CText * P=pgs[ipg].Attributes->m_Footlist->GetNextText(&it); P; P=pgs[ipg].Attributes->m_Footlist->GetNextText(&it) )
		{		
			if( P->m_str.GetLength() && P->m_polyline_start >= 0 )
			{
				///if( P->m_str.Compare(NO_FOOTPRINT) == 0 )
				///	continue;
				CPolyLine * p = &pgs[ipg].PolyLines->GetAt(P->m_polyline_start);
				if( CText * part = p->Check( index_part_attr ) )
				{
					if( inc_list > 0 )
						if( getbit( part->m_pdf_options, inc_list ) == 0 )
							continue;
					if( ex_list > 0 )
						if( getbit( part->m_pdf_options, ex_list ) )
							continue;
					CString Vstr = "no value";
					CText * V = p->Check( index_value_attr );
					if( V )
						Vstr = V->m_str;
					else if( ignore_without_value )
						continue;
					// check
					if( CASH.Find( dSPACE + Vstr + "@" + P->m_str + dSPACE ) < 0 )
					{					
						CASH += ( Vstr + "@" + P->m_str + dSPACE );
						cur_it = it;
						cur_page = ipg;
						*Value = Vstr;
						*Package = P->m_str;
						int CNT = 0;
						int DLen = 0;
						for( int i=0; i<B_LINK.GetSize(); i++ )
							CNT += GetBOMCount( Value, Package, Details, detail_column_width, &DLen, inc_list, ex_list, &B_LINK.GetAt(i), &B_SUFF.GetAt(i), B_PAGENUMB.GetAt(i) );
						CNT += GetBOMCount( Value, Package, Details, detail_column_width, &DLen, inc_list, ex_list );
						SortDetails( Details, detail_column_width );
						return CNT;
					}
				}
			}
		}
		// reset cur_it
		cur_it = -1;
	}
	cur_page = MAX_PAGES;
	int CNT = -1;
	int sv_page = ThisNumber;
	for( int i=B_LINK.GetSize()-1; i>=0; i-- )
	{
		CString LINK = B_LINK.GetAt(i);
		int it = -1;
		int ipg = B_PAGENUMB.GetAt(i);
		RECT Rpart = rect(0,0,0,0);
		CText * ptr = pgs[ipg].Attributes->m_Reflist->GetText( &LINK, &it );
		if( ptr )
		{
			if( ThisNumber != ipg )
				theApp.m_Doc->SwitchToPage( ipg );
			Rpart = theApp.m_Doc->GetPartBounds( ptr, NULL );
		}
		else continue;
		it = -1;
		for( CText * P=pgs[ipg].Attributes->m_Footlist->GetNextText(&it); P; P=pgs[ipg].Attributes->m_Footlist->GetNextText(&it) )
		{			
			if( P->m_str.GetLength() && P->m_polyline_start >= 0 )
			{
				///if( P->m_str.Compare(NO_FOOTPRINT) == 0 )
				///	continue;
				CPolyLine * p = &pgs[ipg].PolyLines->GetAt(P->m_polyline_start);
				if( CText * part = p->Check( index_part_attr ) )
				{
					RECT gr = theApp.m_Doc->GetPartBounds( part, NULL );
					if( RectsIntersection( Rpart, gr ) < 0 || gr.right - gr.left > Rpart.right - Rpart.left )
						continue;
					CString Vstr = "no value";
					CText * V = p->Check( index_value_attr );
					if( V )
						Vstr = V->m_str;
					else if( ignore_without_value )
						continue;
					// check
					if( CASH.Find( dSPACE + Vstr + "@" + P->m_str + dSPACE ) < 0 )
					{	
						CASH += ( Vstr + "@" + P->m_str + dSPACE );
						CNT = max(CNT,0);
						*Value = Vstr;
						*Package = P->m_str;
						int DLen = 0;
						for( int i2=0; i2<B_LINK.GetSize(); i2++ )
							CNT += GetBOMCount( Value, Package, Details, detail_column_width, &DLen, inc_list, ex_list, &B_LINK.GetAt(i2), &B_SUFF.GetAt(i2), B_PAGENUMB.GetAt(i2) );
						break;
					}
				}
			}
		}
		if( CNT >= 0 )
			break;
	}
	if( ThisNumber != sv_page )
		theApp.m_Doc->SwitchToPage( sv_page );
	SortDetails( Details, detail_column_width );
	return CNT;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::SortDetails( CString * Details, int column_width )
{
	if( Details == NULL )
		return 0;

	TagTable T;
	T.LoadPartlistFromString( Details );
	*Details = "";
	int prev_w=0;
	CString ref = "A0", v="---", f="---";
	for( int i=T.FindNextPart( &ref, &v, &f, -1 ); i>=0; i=T.FindNextPart( &ref, &v, &f, -1 ) )
	{
		if( Details->GetLength() )
			*Details += ", ";
		int width = Details->GetLength() / (column_width?column_width:10);
		if( width != prev_w && width > 0 )
			*Details += "'";
		prev_w = width;
		CString ref2;
		int bCount = T.GetBomRepeats( &ref, &ref2, &v, &f, -1 );
		if( bCount > 1 )
		{
			*Details += (ref+"..."+ref2);
			ref = ref2;
		}
		else
			*Details += ref;
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetActiveNumber()
{
	return ThisNumber;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetNumPages()
{
	return m_num_pages;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::GetPageName( int num, CString * name )
{
	if( name )
		*name = pgs[num].name;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetNetPins(	CString * NetName, 
							CString * Pins, 
							int i_format, 
							CString * NewNameOrSuffix, 
							CString * NetControl, 
							int i_page,
							CString * ASUFF,
							RECT * inRect )
{	
	static int cp_counter = 0;
	BOOL CP_inside_CP = (GetPcbIndex(ThisNumber,0)==-1);
	const int ERR = TRUE;
	CString SFX = "";
	if( i_page == -1 )
		*NewNameOrSuffix = *NetName; // (set default)
	if( ASUFF )
		SFX = *ASUFF;
	for( int ip=max(0,i_page); ip<pgs.GetSize(); ip++ )
	{
		if( i_page == -1 )
		{
			if( CP_inside_CP )
				ip = ThisNumber;
			else if( IsThePageIncludedInNetlist( ip ) == 0 )
				continue;
		}
		for( int ipo=0; ipo<pgs[ip].PolyLines->GetSize(); ipo++ )
		{
			CPolyLine * p = &pgs[ip].PolyLines->GetAt(ipo);
			RECT pr = p->GetCornerBounds();
			if( inRect )
			{
				if( inRect->left > pr.right ||
					inRect->right < pr.left ||
					inRect->top < pr.bottom ||
					inRect->bottom > pr.top )
					continue;
			}
			if( CText * gNet = p->Check( index_net_attr ) )
			{
				if( gNet->m_str.Compare(*NetName) == 0 )
				{
					if( CText * gDesc = p->Check( index_desc_attr ) )
					{
						if( gDesc->m_str.Right(7) == "BLK_PTR" && NetControl )//&& i_page == -1 )
						{
							CString  SUFF, REF, PIN;
							int BlockPage = theApp.m_Doc->ExtractBLK_PTR( &gDesc->m_str, &SUFF, &REF, &PIN );
							CTextList * p_list = pgs[BlockPage].Attributes->m_Reflist;
							int it = -1;
							CText * Cpart = p_list->GetText( &REF, &it );
							if( Cpart == NULL )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"Fatal error of \"Get Net Pins\" function (Cpart == NULL). See Pages.cpp":
									"Фатальная ошибка функции \"GetNetPins\" (Cpart == NULL). См. Pages.cpp", MB_ICONERROR);
								return ERR;
							}
							if( Cpart->m_polyline_start < 0 )
								ASSERT(0);
							CText * fPin = NULL;
							for( int get=Cpart->m_polyline_start; get>=0; get=pgs[BlockPage].PolyLines->GetAt(get).Next[index_part_attr] )
							{
								if( CText * gP = pgs[BlockPage].PolyLines->GetAt(get).Check(index_pin_attr) )
									if( gP->m_str.Compare( PIN ) == 0 )
									{
										fPin = gP;
										break;
									}
							}
							if( fPin == NULL )
							{
								AfxMessageBox(G_LANGUAGE == 0 ? 
									"Fatal error of \"Get Net Pins\" function (fPin == NULL). See Pages.cpp":
									"Фатальная ошибка функции \"GetNetPins\" (fPin == NULL). См. Pages.cpp", MB_ICONERROR);
								return ERR;
							}
							if( CText * BlockNet = pgs[BlockPage].PolyLines->GetAt(fPin->m_polyline_start).Check(index_net_attr) )
							{
								CString fStr = (dSPACE + BlockNet->m_str + REF + SUFF + dSPACE);

								if( NetControl->Find( fStr ) >= 0 )
								{
									// fatal error
									// conflict of nets
									// different nets connect
									CString s;
									s.Format(G_LANGUAGE == 0 ? 
										"Fatal error! Different nets are connected inside a complex part:  %s%s. Fix the error and try again\n\n%s":
										"Фатальная ошибка! Цепи с разными именами накоротко соединены внутри иерархической детали: %s%s. Исправьте ошибку и попробуйте снова\n\n%s", REF, SUFF, fStr);
									AfxMessageBox(s,MB_ICONERROR);
									return ERR;
								}
								else
								{

									// get CP rectangle
									RECT partR = theApp.m_Doc->GetPolylineBounds();
									int old_pg = theApp.m_Doc->SwitchToPage( BlockPage );
									CText * Part = pgs[ThisNumber].Attributes->m_Reflist->GetText( &REF );
									if( Part )
										partR = theApp.m_Doc->GetPartBounds( Part, NULL );
			
									CString POSTSUFF;
									if( ASUFF )
									{
										if( i_page >= 0 )
											POSTSUFF = *NewNameOrSuffix + *ASUFF;
										else
											POSTSUFF = *ASUFF;
									}
									///CString Control = "";
									int err = GetNetPins( &BlockNet->m_str, Pins, i_format, &SUFF, NetControl, BlockPage, ASUFF?&POSTSUFF:NULL, &partR );
									if( err )
										return ERR;
									//if( i_page >= 0 )
									//	return 0;

									theApp.m_Doc->SwitchToPage( old_pg );

									if( CP_inside_CP == 0 )
										*NetControl += (BlockNet->m_str + REF + SUFF + dSPACE);

									if( BlockNet->m_str.Left(3) != FREENET )
									{
										if( NetName->Left(3) != FREENET && NetName->Compare( BlockNet->m_str ) )
										{
											// fatal error
											// conflict of nets
											// different nets connect
											CString s;
											s.Format(G_LANGUAGE == 0 ? 
												"Fatal error! Different nets connect (%s and %s):\n\n\tName of complex part: %s%s\n\nOne net label is in a schematic diagram and the other is in a complex part. Fix the error and try again":
												"Фатальная ошибка! Соединяются цепи с разными именами (%s и %s):\n\n\tИмя иерархической детали: %s%s\n\nОдна метка эл.цепи находится на принципиальной схеме, а другая — на странице иерархической детали. Исправьте ошибку и попробуйте снова", BlockNet->m_str, *NetName, REF, SUFF);
											AfxMessageBox(s,MB_ICONERROR);
											return ERR;
										}
										else
										{
											// rename net
											*NewNameOrSuffix = BlockNet->m_str;
										}
									}
								}
							}	
						}
					}
					if( CText * tPart = p->Check( index_part_attr ) )
					{
						if( CText * tPin = p->Check( index_pin_attr ) )		
						{
							if( CText * tFoot = p->Check( index_foot_attr ) )
							{
								if( tFoot->m_str.Compare( NO_FOOTPRINT ) == 0 )
									continue;
							}
							// suffix for complex parts
							CString SUFFIX = "";
							if( i_page >= 0 )
								SUFFIX = *NewNameOrSuffix;
							//
							CString REF = tPart->m_str;
							int dot = REF.Find(".");
							if( dot <= 0 )
								dot = REF.Find("-");
							if( dot > 0 )
								REF = REF.Left(dot);
							CString P = tPin->m_str;
							if( P.FindOneOf( MULTIPIN_TEST ) > 0 )
							{
								CArray<CString> arr;
								int ok = ParseMultiPin( &P, &arr );
								if( !ok )
									return ERR;
								for( int a=0; a<arr.GetSize(); a++ )
								{
									CString add="";
									if( i_format == NL_PADS_PCB )
										add = " " + REF + SUFFIX + SFX + "." + arr[a] + "\n";
									else if( i_format == NL_TANGO )
										add = " " + REF + SUFFIX + SFX + "-" + arr[a] + "\n";
									if( Pins->Find(add) < 0 )
										*Pins += add;
								}
							}
							else
							{
								CString add="";
								if( i_format == NL_PADS_PCB )
									add = " " + REF + SUFFIX + SFX + "." + P + "\n";
								else if( i_format == NL_TANGO )
									add = " " + REF + SUFFIX + SFX + "-" + P + "\n";
								if( Pins->Find(add) < 0 )
									*Pins += add;
							}
						}
					}
				}
			}
		}
		if( CP_inside_CP )
			break;
		if( i_page >= 0 )
			break;
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::IncorrectElectricalCirquit( CString * NetName, int * X, int * Y, int page )
{
	enum{ OK=0, INCORR, SHORT_C };
	CString pin_string, NetName2="", Key, NControl;
	GetNetPins( NetName, &pin_string, NL_PADS_PCB, &NetName2, &NControl );
	pin_string = "Key: " + pin_string;
	// reset netname2
	NetName2 = "";
	CArray<CString> Array;
	ParseKeyString( &pin_string, &Key, &Array );
	if( Array.GetSize() == 0 )
		return OK;
	//
	BOOL incorrect = 1;
	*X = 0;
	*Y = 0;
	for( int i1=0; i1<Array.GetSize(); i1++ )
	{
		CString part = Array.GetAt(i1);
		int dot = part.Find(".");
		if( dot <= 0 )
			ASSERT(0);
		part = part.Left(dot);
		part += "&";
		int it = -1;
		CText * part_ptr = pgs[page].Attributes->m_Reflist->GetText( &part, &it );
		if( part_ptr )
		{
			if( *X == 0 && *Y == 0 )
			{
				*X = part_ptr->m_x;
				*Y = part_ptr->m_y;
			}
			int num_pins = theApp.m_Doc->GetNumAllPins( part_ptr, pgs[page].PolyLines );
			if( num_pins == 2 )
			{
				if( Array.GetSize() == 1 )
					return INCORR;
				BOOL bDIFF = 0;
				for( int scan_part=part_ptr->m_polyline_start; scan_part>=0; scan_part=pgs[page].PolyLines->GetAt( scan_part ).Next[index_part_attr] )
				{
					CPolyLine * pp = &pgs[page].PolyLines->GetAt( scan_part );
					if( pp->pAttr[index_pin_attr] )
					{
						*X = pp->pAttr[index_pin_attr]->m_x;
						*Y = pp->pAttr[index_pin_attr]->m_y;
						if( pp->GetLayer() != LAY_PIN_LINE )
							return INCORR;
						else if( pp->pAttr[index_net_attr] )
						{
							if( NetName->Compare( pp->pAttr[index_net_attr]->m_str ) )
							{
								bDIFF = 1;
								if( NetName2.GetLength() == 0 )
									NetName2 = pp->pAttr[index_net_attr]->m_str;
								else 
								{
									if( NetName2.Compare( pp->pAttr[index_net_attr]->m_str ) )
										incorrect = 0;
								}
							}
						}
					}
				}
				if( bDIFF == 0 )
					return SHORT_C;
			}
			else
				incorrect = 0;
		}
		else
			incorrect = 0;
	}
	if( incorrect )
		return INCORR;
	else 
		return OK;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::GetNextAttr( int i_attr, int * i_text, int * i_page, BOOL use_netlist_page_mask )
{
	*i_page = max( 0, *i_page );
	for( ; (*i_page)<pgs.GetSize(); (*i_page)++ )
	{
		if( use_netlist_page_mask )
			if( GetPcbIndex(*i_page,0) != GetCurrentPcbIndex() )//if( IsThePageIncludedInNetlist( *i_page ) == 0 )
				continue;
		CTextList * tl = pgs[*i_page].TextList;
		if( i_attr == index_part_attr )
			tl = pgs[*i_page].Attributes->m_Reflist;
		else if( i_attr == index_foot_attr )
			tl = pgs[*i_page].Attributes->m_Footlist;
		else if( i_attr == index_value_attr )
			tl = pgs[*i_page].Attributes->m_Valuelist;
		else if( i_attr == index_pin_attr )
			tl = pgs[*i_page].Attributes->m_Pinlist;
		else if( i_attr == index_net_attr )
			tl = pgs[*i_page].Attributes->m_Netlist;
		else if( i_attr == index_desc_attr )
			tl = pgs[*i_page].Attributes->m_pDesclist;
		CText * t=tl->GetNextText(i_text);
		if( t )
			return t;
		else
			(*i_text) = -1;
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CDisplayList * CPageList::GetDisplayList( int ipage )
{
	return pgs[ipage].DisplayList;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CTextList * CPageList::GetTextList( int ipage, int attr )
{
	if( ipage < 0 || ipage >= pgs.GetSize() )
		return NULL;
	if( attr == index_part_attr )
		return pgs[ipage].Attributes->m_Reflist;
	else if( attr == index_value_attr )
		return pgs[ipage].Attributes->m_Valuelist;
	else if( attr == index_foot_attr )
		return pgs[ipage].Attributes->m_Footlist;
	else if( attr == index_pin_attr )
		return pgs[ipage].Attributes->m_Pinlist;
	else if( attr == index_net_attr )
		return pgs[ipage].Attributes->m_Netlist;
	else if( attr == index_desc_attr )
		return pgs[ipage].Attributes->m_pDesclist;
	else
		return pgs[ipage].TextList;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CArray<CPolyLine> * CPageList::GetPolyArray( int ipage )
{
	return pgs[ipage].PolyLines;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CAttrib * CPageList::GetCurrentAttrPtr()
{
	return *Attributes;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
DRErrorList * CPageList::GetDRErrorList( int ipage )
{
	return pgs[ipage].DREList;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::RemoveEmptyMergers( StrList * m_ml )
{
	m_ml->mark0();
	// Go
	for( int i=0; i<m_ml->GetSize(); i++ )
	{
		for( int ipage=0; ipage<pgs.GetSize(); ipage++ )
		{
			CArray<CPolyLine> * po = GetPolyArray(ipage);
			for( int ip=0; ip<po->GetSize(); ip++ )
			{
				if( po->GetAt(ip).m_visible == 0 )
					continue;

				// merge
				int mer = po->GetAt(ip).GetMerge();
				if( mer >= 0 )
				{
					m_ml->mark1(mer);

					// submerge
					mer = po->GetAt(ip).GetMerge(TRUE); 
					if( mer >= 0 )
						m_ml->mark1(mer);
				}
				// scan texts
				CText * att = NULL;
				for( int i_att=0; i_att<num_attributes; i_att++ )
				{
					if( att=po->GetAt(ip).Check(i_att) )
					{
						if( att->m_merge >= 0 )
						{
							m_ml->mark1( att->m_merge );
							if( att->m_submerge >= 0 )
								m_ml->mark1( att->m_submerge );
						}
					}
				}
			}
		}
	}
	int last_i = m_ml->GetSize();
	for( int i=m_ml->GetSize()-1; i>=0; i-- )
	{
		if( m_ml->GetMark(i) == 0 )
			continue;
		if( last_i-i > 1 )
		{
			int count = last_i-i-1;

			// remove mergers
			for( int del=last_i-1; del>i; del-- )
				m_ml->RemoveAt(del);

			// correct indexes
			for( int ipage=0; ipage<pgs.GetSize(); ipage++ )
			{
				CArray<CPolyLine> * po = GetPolyArray(ipage);
				for( int ip=0; ip<po->GetSize(); ip++ )
				{
					// merge
					int mer = po->GetAt(ip).GetMerge();
					if( mer > i )
						po->GetAt(ip).SetMerge( mer-count );
					
					// submerge
					mer = po->GetAt(ip).GetMerge(TRUE); 
					if( mer > i )
						po->GetAt(ip).SetMerge( mer-count, TRUE );
					
					// for texts
					CText * att = NULL;
					for( int i_att=0; i_att<num_attributes; i_att++ )
					{
						if( att=po->GetAt(ip).Check(i_att) )
						{
							if( att->m_merge > i )
								att->m_merge -= count;
							if( att->m_submerge > i )
								att->m_submerge -= count;
						}
					}
				}
			}
		}
		last_i = i;
	}
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::FindPartInRect( RECT * pR, CString * Ref, CString * V, CString * P, int ip )
{
	int it = -1;
	for( CText * t = GetNextAttr( index_part_attr, &it, &ip );
				 t;
				 t = GetNextAttr( index_part_attr, &it, &ip ) )
	{
		if( t->m_str.Compare( *Ref ) )
			continue;
		if( InRange( t->m_x, pR->left, pR->right ) &&
			InRange( t->m_y, pR->bottom, pR->top ) )
		{
			CArray<CPolyLine> * po = GetPolyArray(ip);
			if( t->m_polyline_start >= 0 && t->m_polyline_start < po->GetSize() )
			{
				if( CText * vT = po->GetAt( t->m_polyline_start ).Check( index_value_attr ) )
					*V = vT->m_str;
				if( CText * fT = po->GetAt( t->m_polyline_start ).Check( index_foot_attr ) )
					*P = fT->m_str;
				return 1;
			}
		}
	}
	return 0;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetBlockParts(	CString * destination, 
								CText * description, 
								int iFormat, 
								int * BlkNum, 
								CDlgLog * report, 
								CString * ASUFF,
								CString * FindPart,
								int fromSelection,
								CArray<CString> * Partlist )
{
	static CString MEM_STR = dSPACE;
	static CString REF_MEM = dSPACE; // ref duplicat protection

	CString SUFF, REF;
	if( *BlkNum == 0 )
	{
		MEM_STR = dSPACE;
		REF_MEM = dSPACE;
	}

	int iBlkPage = -1;
	if( description->m_str.Right(3) == "BLK" )
		iBlkPage = theApp.m_Doc->ExtractBLK( &description->m_str, &SUFF, &REF );
	else
		iBlkPage = theApp.m_Doc->ExtractBLK_PTR( &description->m_str, &SUFF, &REF, NULL );
	if( ASUFF )
		SUFF += *ASUFF;
	if( MEM_STR.Find( dSPACE + REF + SUFF + dSPACE ) >= 0 )
		return -1; //this complex part are already written
	
	// flag this complex part are already written
	MEM_STR += (REF + SUFF + dSPACE);

	// report
	if( report )
		report->AddLine("\r\nParts inside a complex part "+REF+SUFF+"\r\n");
	// 
	CTextList * pl = pgs[iBlkPage].Attributes->m_Reflist;
	int it = -1;
	CText * ref = pl->GetText( &REF, &it );
	if( ref == NULL )
		ASSERT(0);
	if( description->m_polyline_start < 0 )
		ASSERT(0);
	int sv_page = ThisNumber;
	if( ThisNumber != iBlkPage )
		theApp.m_Doc->SwitchToPage(iBlkPage);
	RECT BlkRect = theApp.m_Doc->GetPartBounds(ref,NULL);
	it = -1;
	for( CText * t=pl->GetNextText(&it); t; t=pl->GetNextText(&it) )
	{
		RECT r = theApp.m_Doc->GetPartBounds(t,NULL);
		if( RectsIntersection( BlkRect, r ) >= 0 &&
			(BlkRect.right-BlkRect.left) >= (r.right-r.left) )
		{
			CString RD=t->m_str, V="", F="";
			if( CText * vT = pgs[iBlkPage].PolyLines->GetAt( t->m_polyline_start ).pAttr[index_value_attr] )
				V = vT->m_str;
			if( CText * fT = pgs[iBlkPage].PolyLines->GetAt( t->m_polyline_start ).pAttr[index_foot_attr] )
				F = fT->m_str;
			if( F.Compare(NO_FOOTPRINT) == 0 )
				continue;
			int i1 = t->m_str.Find(".");
			if( i1 <= 0 )
				i1 = t->m_str.Find("-");
			if( i1 > 0 )
			{
				RD = t->m_str.Left(i1);
				if( iFormat == NL_PADS_PCB )
				{
					if( destination->Find( "\n" + RD + SUFF + dSPACE ) >= 0 )
						continue;
				}
				else if( iFormat == NL_TANGO )
				{
					if( destination->Find( "[\n" + RD + SUFF + "\n" ) >= 0 )
						continue;
				}
			}

			// special option for parts
			if( FindPart )
				if( FindPart->Compare( RD + SUFF ) == 0 )
				{
					*FindPart = RD;
					if( ThisNumber != sv_page )
						theApp.m_Doc->SwitchToPage(sv_page);
					return iBlkPage;
				}

			// write part name
			if( fromSelection == -1 )
				if( iFormat == NL_PADS_PCB )
				{
					if( destination->GetLength() == 0 )
						*destination = "\n";
					if( V.GetLength() )
						*destination += (RD + SUFF + dSPACE + V + "@" + F + "\n");
					else
						*destination += (RD + SUFF + dSPACE + F + "\n");
				}
				else if( iFormat == NL_TANGO )
				{
					*destination += "[\n" + RD + SUFF + "\n" + F + "\n" + F + "\n" + V + "\n\n\n]\n";
				}

			// fill partlist data
			if( Partlist )
			{
				CString nV = V.GetLength()?V:"---";
				Partlist->Add(RD + SUFF + dSPACE + nV + "@" + F);
			}

			// test on err
			if( REF_MEM.Find(dSPACE + RD + SUFF + dSPACE) >= 0 )
			{
				if( t->m_str.FindOneOf(".-") == -1 )
				{
					// FATAL
					CString s;
					s.Format(G_LANGUAGE == 0 ? 
						"Fatal Error! Designation conflict within complex parts %s%s. The %s%s part previously encountered in the process is duplicated":
						"Фатальная ошибка! Конфликт обозначений в сотавных деталях %s%s. Деталь %s%s, ранее встречавшаяся в процессе, дублируется. Прервано...", REF, SUFF, RD, SUFF);
					AfxMessageBox(s, MB_ICONERROR);
					if( ThisNumber != sv_page )
						theApp.m_Doc->SwitchToPage(sv_page);
					return -1;
				}
			}
			else
			{
				REF_MEM += (RD + SUFF + dSPACE);
			}

			// report
			if( report )
				report->AddLine("\t"+RD+SUFF+"\r\n");
		}
	}
	(*BlkNum)++;
	it=-1;
	int ip = iBlkPage; 
	for( CText * ptr = FindBLK_PTR(&it, &ip, TRUE); ptr; ptr=FindBLK_PTR(&it, &ip, TRUE) )
	{
		if( ip != iBlkPage )
			break;

		// from selection
		if( fromSelection >= 0 )
		{
			CPolyLine * p = &pgs[ThisNumber].PolyLines->GetAt( ptr->m_polyline_start );	 
			if( p->GetSideSel() == 0 ) 
				continue; // disable write
		}

		int part_ipg = GetBlockParts( destination, ptr, iFormat, BlkNum, report, &SUFF, FindPart, -1, Partlist );
		if( part_ipg >= 0 )
		{
			if( ThisNumber != sv_page )
				theApp.m_Doc->SwitchToPage(sv_page);
			return part_ipg;
		}
	}
	if( ThisNumber != sv_page )
		theApp.m_Doc->SwitchToPage(sv_page);
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetBlockNets(	CString * destination, 
								CText * description, 
								int iFormat, 
								int * BlkNum, 
								CString * NetNameFormat, 
								CString * NetControl,
								CDlgLog * report,
								CString * ASUFF )
{
	const int NetIncludedBlkPtr = 2;
	const int NOT = 3;
	static CString MEM_STR = dSPACE;

	CString SUFF, REF, PIN;

	// сброс массива с именами записанных CP
	if( *BlkNum == 0 )
	{
		MEM_STR = dSPACE;
	}

	// тест на дублирование
	if( NetNameFormat->GetLength() && 
		NetControl->Find( dSPACE+(*NetNameFormat)+dSPACE ) >= 0 )
		return 0; //this net are already written

	int iBlkPage = theApp.m_Doc->ExtractBLK_PTR( &description->m_str, &SUFF, &REF, &PIN );

	// суммируем текущий суффикс 
	// с суффиксом родительской детали
	if( ASUFF )
		SUFF += *ASUFF;

	if( MEM_STR.Find( dSPACE + REF + SUFF + dSPACE ) >= 0 )
		return 0; //this complex part are already written

	// report
	if( report && NetNameFormat->GetLength() == 0 )
		report->AddLine("\r\nNets inside a complex part "+REF+" ("+SUFF+")\r\n");
	//
	CTextList * pl = pgs[iBlkPage].Attributes->m_Reflist;
	int it = -1;
	CText * ref = pl->GetText( &REF, &it );
	if( ref == NULL )
		ASSERT(0);
	if( description->m_polyline_start < 0 )
		ASSERT(0);

	BOOL CP_inside_CP = (GetPcbIndex(ThisNumber,0)==-1);
	{
		// mark nets that are already written
		// маркируем сети страницы с комплексной деталью
		// каждый раз при входе в функцию !!!
		// это нужно для того чтобы далее
		// пометить сети контачащие с пинами комплексной детали
		// они уже считаны функцией GetNetPins
		pgs[iBlkPage].Attributes->m_Netlist->MarkAllTexts(0);
		pgs[iBlkPage].TextList->MarkAllTexts(0);
	}
	it=-1;
	int ip = (CP_inside_CP?ThisNumber:-1);
	for( CText * ptr = FindBLK_PTR(&it,&ip,CP_inside_CP); ptr; ptr=FindBLK_PTR(&it,&ip,CP_inside_CP) )
	{
		// если мы в режиме считывания субдетали то
		// сканируем только текущую страницу
		// иначе сканируем все страницы текущего нетлиста
		// 
		// а сети маркируем на странице комплексной детали 
		// чтобы далее игнорировать их при считывании пинов
		// они уже считаны функцией GetNetPins
		if( CP_inside_CP )
			if( ip != ThisNumber )
				break;
		CString getSUFF, getREF, getPIN;
		int getPage = theApp.m_Doc->ExtractBLK_PTR( &ptr->m_str, &getSUFF, &getREF, &getPIN );
		if( ASUFF )
			getSUFF += *ASUFF;
		if( getPage == iBlkPage )
			if( getSUFF.Compare( SUFF ) == 0 )
				if( getREF.Compare( REF ) == 0 )
				{
					// mark net
					for( int get = ref->m_polyline_start; get >= 0; get = pgs[iBlkPage].PolyLines->GetAt(get).Next[index_part_attr] )
					{
						if( CText * pnet = pgs[iBlkPage].PolyLines->GetAt(get).Check( index_net_attr ) )
							if( CText * ppin = pgs[iBlkPage].PolyLines->GetAt(get).Check( index_pin_attr ) )
								if( ppin->m_str.Compare( getPIN ) == 0 )
									pnet->utility = TRUE;
					}
				}
	}
	{
		// Complex part inside a Complex part
		// тестируем нет ли внутри комплексной 
		// детали другой комплексной детали 
		// и также маркируем сети которые контачат 
		// с пинами субдетали чтобы считать
		// отдельно в конце ( рекурсивным вызовом )
		it=-1;
		ip=iBlkPage;
		for( CText * ptr = FindBLK_PTR(&it, &ip, TRUE); ptr; ptr=FindBLK_PTR(&it, &ip, TRUE) )
		{
			if( ip != iBlkPage )
				break;
			
			if( CText * net_ptr = pgs[ip].PolyLines->GetAt( ptr->m_polyline_start ).Check( index_net_attr ) )
			{
				net_ptr->utility = NetIncludedBlkPtr;
				CPolyLine * ref_p = NULL;
				for (int i = ref->m_polyline_start; i >= 0; i = ref_p->Next[index_part_attr])
				{
					ref_p = &pgs[iBlkPage].PolyLines->GetAt( i );
					if( CText * n = ref_p->Check( index_net_attr ) )
						if( n->m_str.Compare( net_ptr->m_str ) == 0 )
							net_ptr->utility = NOT;
				}
			}
		}
	}

	// индекс начала текущей сети
	int start_of_this_net = -1;

	int sv_page = ThisNumber;
	int pcb_i = GetPcbIndex( sv_page ); // с учетом  CP
	if( pcb_i < 0 )
		ASSERT(0);

	// переключаемся на страницу комплексной детали
	// и начинаем опрос пинов внутри 
	// прямоугольника комплексной детали
	if( ThisNumber != iBlkPage )
		theApp.m_Doc->SwitchToPage(iBlkPage);
	RECT BlkRect = theApp.m_Doc->GetPartBounds(ref,NULL);
	CTextList * pin_list = pgs[iBlkPage].Attributes->m_Pinlist;
	it = -1;
	for( CText * pin=pin_list->GetNextText(&it); pin; pin=pin_list->GetNextText(&it) )
	{
		CPolyLine * p = &pgs[iBlkPage].PolyLines->GetAt( pin->m_polyline_start );
		if( CText * pin_net = p->Check( index_net_attr ) )
		{
			if( pin_net->utility == NetIncludedBlkPtr )
				continue;

			RECT r = p->GetBounds();
			if( RectsIntersection( BlkRect, r ) >= 0 )
			{
				if( NetNameFormat->GetLength() && NetNameFormat->Compare(pin_net->m_str) ) 
					continue; // режим считывания userNet
				else //if( NetNameFormat->GetLength() == 0 ) // !!! тестируем в любом случае
				{
					//if( pin_net->m_str.Left(3) != FREENET ) // !!! тестируем в любом случае
					{
						// улучшенный вариант теста ниже !
						/*for( int scan_page=0; scan_page<pgs.GetSize(); scan_page++ )
						{
							if( getbit( m_netlist_page_mask[pcb_i], scan_page ) == 0 )
								continue;
							CTextList * gtl = pgs[scan_page].TextList;
							int it3 = -1;
							if( CText * found = gtl->GetText( &pin_net->m_str, &it3 ) )
							{
								Found = 1;
								break;
							}
						}*/
						// тестируем не была ли сеть уже ранее считана
						CString fStr = (dSPACE + pin_net->m_str + dSPACE);
						if( NetControl->Find( fStr ) >= 0 )
							continue;
						
						// тест на случайное совпадение имен
						// например GND и GNDA добавленным суффиксом
						// но эта ситуация похоже невозможна
						BOOL Found = 0;
						CString FullNetName = pin_net->m_str + SUFF;
						for( int scan_page=0; scan_page<pgs.GetSize(); scan_page++ )
						{
							if( getbit( m_netlist_page_mask[pcb_i], scan_page ) == 0 )
								continue;
							CTextList * gtl = pgs[scan_page].TextList;
							int it3 = -1;	
							if( CText * found = gtl->GetText( &FullNetName, &it3 ) )
							{
								Found = 1;
								break;
							}
						}
						if( Found )
						{
							CString s;
							s.Format(G_LANGUAGE == 0 ? 
								"Fatal Error! Invalid net name in complex part %s%s. There was a duplication of the net name %s in a complex part when generating nets.":
								"Фатальная ошибка! Недопустимое имя эл.цепи в иерархической детали %s%s. При генерации цепей произошло дублирование имени цепи %s в иерархической части.", REF, SUFF, FullNetName);
							AfxMessageBox(s);
							continue;
						}
					}
				}

				CString net_rep = "";

				// add net
				start_of_this_net = destination->GetLength();
				// add head string for nets belonging to this block
				if( NetNameFormat->GetLength() == 0 )
				{
					// добавляем заголовок только в режиме
					// считывания всех сетей, а не одной сети комплексной детали
					if( iFormat == NL_PADS_PCB )
						*destination += "*SIGNAL* " + pin_net->m_str + REF + SUFF + "\n";
					else if( iFormat == NL_TANGO )
						*destination += "(\n" + pin_net->m_str + REF + SUFF + "\n";
					// report
					if( report )
					{
						net_rep += ("\t" + pin_net->m_str + REF + SUFF + ":\r\n");
					}
				}

				// scan pins
				int fail = 0;
				int it2 = -1;
				for( CText * add_pin=pin_list->GetNextText(&it2); add_pin; add_pin=pin_list->GetNextText(&it2) )
				{
					if( add_pin->m_polyline_start < 0 )
						continue;
					CPolyLine * pp = &pgs[iBlkPage].PolyLines->GetAt( add_pin->m_polyline_start );
					if( CText * add_net = pp->Check( index_net_attr ) )
					{
						RECT ppr = pp->GetBounds();
						if( RectsIntersection( BlkRect, ppr ) == -1 )
							continue;
						BOOL bADD = 0;
						if( add_net == pin_net )
							bADD = 1;
						else if( add_net->m_str.Compare( pin_net->m_str ) == 0 )
							bADD = 1;
						if( bADD )
						{
							CText * add_ref = pp->Check( index_part_attr );
							if( add_ref )
							{
								if( add_net->utility )
								{
									// fail
									// user named net on pin
									// net already written
									// remove net
									*destination = destination->Left( start_of_this_net );
									fail = 1;
									break;
								}
								CString refDes = add_ref->m_str;
								int i1 = refDes.Find(".");
								if( i1 <= 0 )
									i1 = refDes.Find("-");
								if( i1 > 0 )
								{
									refDes = refDes.Left(i1);
								}
								if( add_pin->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
								{
									CArray<CString> arr;
									int ok = ParseMultiPin( &add_pin->m_str, &arr );
									//if( !ok )
									//	return ERR; no use
									for( int a=0; a<arr.GetSize(); a++ )
									{
										CString add="";
										if( iFormat == NL_PADS_PCB )
											add = (" " + refDes + SUFF + "." + arr[a] + "\n");
										else if( iFormat == NL_TANGO )
											add = (" " + refDes + SUFF + "-" + arr[a] + "\n");
										if( destination->Find(add) < 0 )
											*destination += add;
									}
								}
								else
								{
									if( iFormat == NL_PADS_PCB )
										*destination += (" " + refDes + SUFF + "." + add_pin->m_str + "\n");
									else if( iFormat == NL_TANGO )
										*destination += (" " + refDes + SUFF + "-" + add_pin->m_str + "\n");
								}

								// report
								if( report )
									net_rep += ("\t\t" +refDes + SUFF + "." + add_pin->m_str +"\r\n");
							}
						}
					}
				}
				if( !fail )
				{
					if( NetNameFormat->GetLength() == 0 )
						if( iFormat == NL_TANGO )
							*destination += ")\n";
					// report
					if( report )
					{
						if( NetNameFormat->GetLength() )
							report->AddLine("\tThe same net in a complex part: "+REF+" ("+SUFF+")\r\n");
						report->AddLine(net_rep);
					}
				}
				// mark net
				pin_net->utility = TRUE;
			}
		}
	}
	
	if( ThisNumber != iBlkPage )
		ASSERT(0);
	
	// чтение сетей связанных с субдеталями
	// и далее рекурсивный вызов
	{
		it=-1;
		ip = iBlkPage;
		for( CText* net=GetNextAttr( -1, &it, &ip, 0 ); 
					net; 
					net=GetNextAttr( -1, &it, &ip, 0 ) )
		{
			if( ip != iBlkPage )
				break;
			if( net->utility == NetIncludedBlkPtr )
			{
				// Get Net Pins
				CString gPins, Rename="";
				// CString Control = "";
				int err = GetNetPins( &net->m_str, &gPins, iFormat, &Rename, NetControl, -1, &SUFF, &BlkRect );
				if( err )
				{
					// замыкание сетей или 
					// какая то другая ошибка
					*destination += "ERROR";
					if( report )
						report->AddLine("\r\nFAIL\r\n");
					return 0;
				}
				if( NetNameFormat->GetLength() && NetNameFormat->Compare( Rename ) )
					continue;
				if( NetControl->Find( dSPACE+Rename+dSPACE ) >= 0 )
					continue;

				if( NetNameFormat->GetLength() == 0 )
				{
					// добавляем заголовок только в режиме
					// считывания всех сетей, а не одной сети комплексной детали
					if( iFormat == NL_PADS_PCB )
						*destination += "*SIGNAL* " + Rename + REF + SUFF + "\n";
					else if( iFormat == NL_TANGO )
						*destination += "(\n" + Rename + REF + SUFF + "\n";
				}
				
				*destination += gPins;

				// report
				if( report )
				{
					gPins.Replace("-",".");
					gPins.Replace("\n","\r\n\t\t");
					gPins.Replace(" ","");
					if( Rename.Left(3) != FREENET )
					{
						report->AddLine("\tNet " + Rename + " on complex part pin:\r\n");
						report->AddLine("\t\t" + gPins + "\r\n");
					}
					else
					{
						report->AddLine("\t" + Rename + REF + SUFF + ":\r\n");
						report->AddLine("\t\t" + gPins + "\r\n");
					}
				}

				// close tango-netlist
				// закрываем заголовок если в режиме
				// считывания всех сетей, а не одной сети
				if( NetNameFormat->GetLength() == 0 )
					if( iFormat == NL_TANGO )
						*destination += (")\n");

			}
		}
		
		// считываем остальные сети внутри комплексной детали ( режим Nets="" )
		// кроме пинов соединенных с пинами комплексной детали
		// и кроме сетей добавленных в список NetControl (уже считаных)
		//int CNT = 0;
		it=-1;
		ip = iBlkPage;
		for( CText * ptr = FindBLK_PTR(&it, &ip, TRUE); ptr; ptr=FindBLK_PTR(&it, &ip, TRUE) )
		{
			if( ip != iBlkPage )
				break;
			CString Nets = *NetNameFormat;
			GetBlockNets( destination, ptr, iFormat, BlkNum, &Nets, NetControl, report, &SUFF );
		}
	}

	if( ThisNumber != sv_page )
		theApp.m_Doc->SwitchToPage(sv_page);

	// flag this complex part are already written
	// добавляем имя комплексной детали в массив уже записанных имен 
	// в любом режиме GetBlockNets - и в режиме считывания всех сетей 
	// внутри комплексной детали ( NetNameFormat пустой ), 
	// и в режиме считывания конкретной сети ( NetNameFormat=userNet )
	///if( NetNameFormat->GetLength() == 0 )
	MEM_STR += (REF + SUFF + dSPACE);

	// инкрементируем индекс количества вызовов 
	(*BlkNum)++;

	if( destination->GetLength() == 0 )
	{
		if( report && NetNameFormat->GetLength() == 0 )
			report->AddLine("\tis missing\r\n");
		return 0;
	}
	return 1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::FindBLK( int * it, int * ip, BOOL any_page )
{
	for( int ipg=max(0,*ip); ipg<pgs.GetSize(); ipg++ )
	{
		BOOL yes = 0;
		if( any_page )
			yes = 1;
		else if( IsThePageIncludedInNetlist( ipg ) )
			yes = 1;
		if( yes )
		{
			if( *ip != ipg )
				*it = -1;
			*ip = ipg;
			for( CText * t=pgs[ipg].Attributes->m_pDesclist->GetNextText(it); t; t=pgs[ipg].Attributes->m_pDesclist->GetNextText(it) )
			{
				if( t->m_str.Right(3) == "BLK" )
				{
					return t;
				}
			}
		}
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::FindBLK_PTR( CString * description, int page )
{
	CString SFX, LINK, SFX2, LINK2;
	int pg = theApp.m_Doc->ExtractBLK( description, &SFX, &LINK );
	int ip = page;
	int it = -1;
	for( CText * t=FindBLK_PTR( &it, &ip, TRUE ); t; t=FindBLK_PTR( &it, &ip, TRUE ) )
	{
		int pg2 = theApp.m_Doc->ExtractBLK_PTR( &t->m_str, &SFX2, &LINK2 );
		if( pg == pg2 && SFX.Compare(SFX2) == 0 && LINK.Compare(LINK2) == 0 )
		{
			return t;
		}
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
CText * CPageList::FindBLK_PTR( int * it, int * ip, BOOL any_page )
{
	for( int ipg=max(0,*ip); ipg<pgs.GetSize(); ipg++ )
	{
		BOOL yes = 0;
		if( any_page )
			yes = 1;
		else if( IsThePageIncludedInNetlist( ipg ) )
			yes = 1;
		if( yes )
		{
			if( *ip != ipg )
				*it = -1;
			*ip = ipg;
			for( CText * t=pgs[ipg].Attributes->m_pDesclist->GetNextText(it); t; t=pgs[ipg].Attributes->m_pDesclist->GetNextText(it) )
			{
				if( t->m_str.Right(7) == "BLK_PTR" )
				{
					return t;
				}
			}
		}
	}
	return NULL;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetPageIndex( CString * name )
{
	for( int ipg=0; ipg<pgs.GetSize(); ipg++ )
	{
		if( pgs[ipg].name.Compare( *name ) == 0 )
			return ipg;
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetPcbIndex( int ipage, BOOL incCP )
{
	for( int i=0; i<m_pcb_names.GetSize(); i++ )
	{
		if( getbit( m_netlist_page_mask[i], ipage ) )
			return i;
	}
	if( incCP )
	{
		if( m_pcb_index[ipage] < MAX_PAGES )
			return m_pcb_index[ipage];
		
		int cpp = IsThePageIncludesCP( ipage );
		if( cpp >= 0 )
			cpp = GetPcbIndex( cpp, TRUE );
		if( cpp < m_pcb_names.GetSize() )
		{
			m_pcb_index[ipage] = cpp;
			return cpp;
		}
	}
	return -1;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::InvalidatePcbIndexes()
{
	for( int i=0; i<MAX_PAGES+2; i++ )
		m_pcb_index[i] = MAX_PAGES; // default
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::CheckPCBNames( CString * folder )
{
	if( folder->Right(1) == "\\" )
		folder->Truncate( folder->GetLength()-1 );
	for( int i=0; i<m_pcb_names.GetSize(); i++ )
	{
		struct _stat buf;
		CString old_file_name = *folder + "\\" + m_pcb_names.GetAt( i );
		int err = _stat( old_file_name, &buf );//ok
		if( err )
		{	
			CFileFind finder;
			CString first_f = "\\*.fpc";
			CString search_str = *folder + first_f;
			BOOL bWorking = finder.FindFile( search_str );
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				CString fp = finder.GetFilePath();
				CString fn = finder.GetFileName();
				if( !finder.IsDirectory() && !finder.IsDots() )
				{
					// found a file
					CStdioFile F;
					int ok = F.Open( fp, CFile::modeRead, NULL );
					if( ok )
					{
						CString instr, key;
						while( F.ReadString( instr ) )
						{
							if( instr.Left(13) == "project_name:" )
							{
								CArray<CString> arr;
								int np = ParseKeyString( &instr, &key, &arr );
								if( np >= 2 )
								{
									if( m_pcb_names.GetAt( i ).Compare( arr.GetAt(0) ) == 0 ||
										m_pcb_names.GetAt( i ).Compare( arr.GetAt(0)+".fpc" ) == 0 )
									{
										// first replace pcb name
										CString old = m_pcb_names.GetAt( i );
										m_pcb_names.SetAt( i, fn );

										// now correction desc texts
										for( int pg=0; pg<m_num_pages; pg++ )
										{
											int it = -1;
											for( CText * tDesc=pgs[pg].Attributes->m_pDesclist->GetNextText(&it); 
														 tDesc;
														 tDesc=pgs[pg].Attributes->m_pDesclist->GetNextText(&it) )
											{
												if( tDesc->m_str.GetLength() > 10 )
												{
													if( tDesc->m_str.Find( old ) > 0 )
													{
														tDesc->m_str.Replace( old, fn );
														tDesc->m_nchars = tDesc->m_str.GetLength();
														theApp.m_Doc->m_view->DrawBOM( tDesc, pgs[pg].PolyLines );
													}
												}
											}
										}
									}
								}
								break;
							}
							else if( instr.Left(2) == "[f" )
								break;
						}
						F.Close();
					}
				}
			} 
		}
	}
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::GetCurrentPcbIndex()
{
	return GetPcbIndex( ThisNumber ); 
}

CString CPageList::GetCurrentPCBName( int ip )
{
	int i = GetPcbIndex( ip<0?ThisNumber:ip );
	if( i >= 0 )
		return m_pcb_names.GetAt(i);
	else 
	{
		CString LEFT = "";
		int cpp = IsThePageIncludesCP( ip );
		if( cpp >= 0 )
			LEFT = GetCurrentPCBName( cpp );
		return LEFT;
	}
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
BOOL CPageList::IsThePageIncludedInNetlist( int ipage, BOOL bAny )
{
	int ipcb = GetPcbIndex( ipage, FALSE );// ! without CP
	if( ipcb < 0 )
		return FALSE;
	else if( bAny )
		return TRUE;
	else if( ipcb == GetCurrentPcbIndex() )
		return TRUE;
	else
		return FALSE;
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
void CPageList::Protection( int pcb_num )
{
	BOOL bUNPROTECT = 0;
	if( pcb_num == -1 )
		bUNPROTECT = 1;
	if( pcb_num == -1 || pcb_num == MAX_PAGES ) // default
		pcb_num = GetPcbIndex( ThisNumber, TRUE );
	if( pcb_num < 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This page is not included in any of the netlists.":
			"Эта страница не включена ни в один из списков электрических цепей");
		return;
	}
	CString in_str="";
	CString messg = theApp.m_Doc->m_netlist_full_path + ".txt";
	if( theApp.m_Doc->Check_Txt_Msg( &messg, &in_str ) )
	{
		if( in_str.GetLength() == 0 && bUNPROTECT == 0 ) // PROTECT
		{
			// update message
			CStdioFile m_file;
			int err = m_file.Open( LPCSTR(messg), CFile::modeCreate | CFile::modeWrite, NULL );
			if( err )
			{
				m_file.WriteString( FILE_PROTECTED );
				setbit( theApp.m_Doc->m_protection, pcb_num );
				m_file.Close();
				if( theApp.m_Doc->m_view )
				{
					theApp.m_Doc->m_view->m_protect = 1; // upd m_view
					theApp.m_Doc->m_view->SetFKText( theApp.m_Doc->m_view->m_cursor_mode );
				}
				theApp.m_Doc->ProjectModified(); // upd menu
			}
		}
		else if(( in_str.Compare( FILE_PROTECTED ) == 0 || in_str.GetLength() == 0 ) && bUNPROTECT ) // UNPROTECT
		{
			// update message
			CStdioFile m_file;
			int err = m_file.Open( LPCSTR(messg), CFile::modeCreate | CFile::modeWrite, NULL );
			if( err )
			{
				m_file.WriteString( "" );
				clrbit( theApp.m_Doc->m_protection, pcb_num );
				m_file.Close();
				if( theApp.m_Doc->m_view )
				{
					theApp.m_Doc->m_view->m_protect = 0; // upd m_view
					theApp.m_Doc->m_view->SetFKText( theApp.m_Doc->m_view->m_cursor_mode );
				}
				theApp.m_Doc->ProjectModified(); // upd menu
			}
		}
		else if( in_str.Compare( FILE_PROTECTED ) && in_str.GetLength() ) // fail
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				("It is not possible to protect the project when schematic and PCB netlists are mismatched.\n("+in_str+")"):
				("Невозможно защитить проект, если списки эл.цепей схемы и печатной платы не совпадают.\n(" + in_str + ")"), MB_ICONERROR);
		}
	}
	else 
	{
		if( theApp.m_Doc->m_view )
		{
			if( theApp.m_Doc->m_view->m_protect == 0 && bUNPROTECT == 0 )
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"The page could not be protected. "\
					"The new project must first be synchronized "\
					"with the board, and then you can turn on the "\
					"protection.\n\nClick the \"File -> Save with netlist file\" "\
					"menu to export the netlist to the PCB editor.":
					"Страница не может быть защищена. "\
					"Новый проект сначала должен быть синхронизирован "\
					"с платой, а затем можно будет включить "\
					"защиту.\n\nНажмите меню \"Файл -> Сохранить со списком цепей\" "\
					"для экспорта списка соединений в файл, зетем импортируйте его из редактора печатных плат ПлатФорм.", MB_ICONERROR );
			else
			{
				clrbit( theApp.m_Doc->m_protection, pcb_num );
				theApp.m_Doc->m_view->m_protect = 0;
			}
		}
	}
}
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
/*CArray<CString> * CPageList::GetBlockSuffixes()
{
	if( getbit( m_get_suffixes, ThisNumber ) == 0 )
	{
		m_block_suffixes[ThisNumber].RemoveAll();
		for( int ip=0; ip<pgs.GetSize(); ip++ )
		{
			if( IsThePageIncludedInNetlist( ip ) == 0 )
				continue;
			CTextList * dl = pgs[ip].Attributes->m_pDesclist;
			if( dl == NULL )
				continue;
			CArray<CPolyLine> * poly_arr = pgs[ip].PolyLines;
			int it = -1;
			for( CText * desc=dl->GetNextText(&it); desc; desc=dl->GetNextText(&it) )
			{
				if( desc->m_str.Right(3) == "BLK" )
				{
					CString suff = "";
					if( theApp.m_Doc->ExtractBLK( &desc->m_str, &suff ) )
						m_block_suffixes[ThisNumber].Add( suff );
				}
			}
		}
		setbit( m_get_suffixes, ThisNumber );
	}
	if( m_block_suffixes[ThisNumber].GetSize() )
		return &m_block_suffixes[ThisNumber];
	else
		return NULL;
}*/
//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
int CPageList::InvalidateMerges( int mergeIndex )
{
	if( mergeIndex == -1 )
		return 0;
	int RET = 0;
	for( int i=0; i<GetNumPages(); i++ )
	{
		if( i == ThisNumber )
			continue;
		CArray<CPolyLine> * po = GetPolyArray(i);
		for( int ii=0; ii<po->GetSize(); ii++ )
		{
			if( po->GetAt(ii).GetMerge() == mergeIndex )
			{
				RET++;
				po->GetAt(ii).SetMerge(-1);
			}
			if( po->GetAt(ii).GetMerge(TRUE) == mergeIndex )
			{
				RET++;
				po->GetAt(ii).SetMerge(-1,TRUE);
			}
		}
		int it = -1;
		CTextList * tl = pgs[i].Attributes->m_Reflist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
		it = -1;
		tl = pgs[i].Attributes->m_Valuelist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
		it = -1;
		tl = pgs[i].Attributes->m_Footlist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
		it = -1;
		tl = pgs[i].Attributes->m_Pinlist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
		it = -1;
		tl = pgs[i].Attributes->m_Netlist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
		it = -1;
		tl = pgs[i].Attributes->m_pDesclist;
		for( CText * t=tl->GetNextText( &it ); t; t=tl->GetNextText( &it ) )
			if( t->m_merge == mergeIndex )
			{
				RET++;
				t->m_merge = -1;
			}
	}
	return RET;
}