#include "stdafx.h"
#include "FreeSch.h"
#include "BomInTable.h"
//#include <cmath>

BomInTable::BomInTable( CFreePcbDoc * doc )
{
	m_doc = doc;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
void BomInTable::MakeReport()
{
	// ini
	MOVABLE = "|movable";
	PR_NAME = ".project_name";
	PCB_NAME = ".pcb_name";


	if( m_doc->Pages.IsThePageIncludedInNetlist( m_doc->Pages.GetActiveNumber() ) == 0 )
		return;
	if( ReadParamsFromTagFile() )
		return;

	CString PcbName = m_doc->Pages.GetCurrentPCBName();
	if( PcbName.Right(3) == "fpc" )
		PcbName.Truncate( PcbName.GetLength()-4 );
	CString ProjName = m_doc->m_name;
	if( ProjName.Right(3) == "cds" )
		ProjName.Truncate( ProjName.GetLength()-4 );

	// mark all ref texts
	int it = -1; int ip = -1;
	for( CText * t=m_doc->Pages.GetNextAttr(index_part_attr, &it, &ip, TRUE );
				 t;
				 t=m_doc->Pages.GetNextAttr(index_part_attr, &it, &ip, TRUE ))
				 t->utility = 0;

	
	//modify m_netlist_page_mask
	int cur_pcb = m_doc->Pages.GetCurrentPcbIndex();
	int save_mask[MAX_PAGES];
	save_mask[cur_pcb] = m_doc->Pages.m_netlist_page_mask[cur_pcb];
	for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
	{
		if( ipcb == cur_pcb )
			continue;
		save_mask[ipcb] = m_doc->Pages.m_netlist_page_mask[ipcb];
		if( m_entire )
		{
			m_doc->Pages.m_netlist_page_mask[cur_pcb] |= m_doc->Pages.m_netlist_page_mask[ipcb];
			m_doc->Pages.m_netlist_page_mask[ipcb] = 0;
		}
	}
	
	CString R,V,F;
	arrV.SetSize( 0 );
	arrF.SetSize( 0 );
	arrC.SetSize( 0 );
	arrR.SetSize( 0 );
	int count = m_doc->Pages.FindBOMNextLine(&V, &F, &R, 9999, TRUE, i_reflist, e_reflist, TRUE );
	if( count >= 0 )
	{
		do
		{
			if( count )
			{
				CString cV=V;
				int sep = cV.Find(CP_LINK);
				if( sep > 0 )
					cV = cV.Left( sep );
				sep = cV.Find("|");
				if( sep > 0 )
					cV.Delete( sep );
				arrV.Add(cV);
				arrF.Add(F);
				CString CNT;
				CNT.Format("%d",count);
				arrC.Add(CNT);
				arrR.Add(R);
			}
			R = "";
			count = m_doc->Pages.FindBOMNextLine(&V, &F, &R, 9999, TRUE, i_reflist, e_reflist );
		}while( count >= 0 );
	}

	// restore m_netlist_page_mask
	if( m_entire )
		for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
			m_doc->Pages.m_netlist_page_mask[ipcb] = save_mask[ipcb];

	R = "AA0";
	AddPString( NULL,0,0,0 );
	for( int index=FindNextRow( &R ); index>=0; index=FindNextRow( &R ) )
	{
		R = arrR.GetAt(index);
		V = arrV.GetAt(index);
		F = arrF.GetAt(index);
		for( int page = nPages; (page>=0&&nPages<MAX_PAGES); page-- )
		{
			for( int i=TextData[page].GetSize()-1; i>=0; i-- )
			{
				CText * data = TextData[page].GetAt(i);
				if( data->m_str.Left(4) == ".col" && data->m_str.GetLength() == 5 )
				{
					curY = data->m_y;
					nPages = PushRows( curY+(shiftY/2), page );
					if( nPages >= 0 )
						AddObject( page, &arrC.GetAt(index), &R, &V, &F );
					page = 0;// break
					break;
				}	
			}
		}
		arrR.RemoveAt(index);
		arrV.RemoveAt(index);
		arrF.RemoveAt(index);
		arrC.RemoveAt(index);
	}
	nPages = MAX_PAGES-1;
	RemoveEmptyRows();
	int cnt_pgs = 0;
	for (int i = 0; i < MAX_PAGES; i++)
	{
		int TextDataSize = TextData[i].GetSize();
		if (TextDataSize)
		{
			int dot_cnt = 0;
			for (int ii = 0; ii < TextDataSize; ii++)
			{
				CText* st = TextData[i].GetAt(ii);
				if (st->m_str.Left(1) == ".")
					dot_cnt++;
			}
			if (dot_cnt != TextDataSize)
				cnt_pgs++;
		}
	}
	nPages = min( nPages, (cnt_pgs-1) );
	RedrawNumbers();


	CString s0 = m_doc->m_path_to_folder+"\\related_files\\reports\\bom";
	struct _stat buf;
	if( _stat( s0, &buf ) )
	{
		ASSERT(0);
	}

	CString title;
	if( m_entire )
		title = m_doc->m_name;
	else
		title = m_doc->Pages.GetCurrentPCBName();
	if( title.GetLength() > 4 )
		if( title.Right(4) == ".fpc" || title.Right(4) == ".cds" )
			title.Truncate( title.GetLength() - 4 );
	CString s1 = s0 + "\\bom.cds";
	CString s2 = s0+"\\" + title + " - " + FILE_NAME + ".cds";
	CStdioFile r, w;
	BOOL ok1 = r.Open( s1, CFile::modeRead, NULL );
	BOOL ok2 = w.Open( s2, CFile::modeCreate | CFile::modeWrite, NULL );
	if( ok1 && ok2 )
	{
		CArray<CString> MemStr;
		BOOL bFlagWriteToMem = FALSE;
		int nPage = 0;
		CString instr;
		while( r.ReadString( instr ) )
		{
			instr = instr.Trim();
			if( instr.Find("project_name:") == 0 )
				instr = "project_name: \"" + title + " - " + FILE_NAME + ".cds\"";
			else if( instr.Find("rename_page:") == 0 )
				instr = "rename_page: \"" + title + " - " + FILE_NAME + "\"";
			else if( instr.Find("page_number:") == 0 )
				instr = "page_number: \"0\"";

			if( instr.Find(PR_NAME) > 0 )
				instr.Replace(PR_NAME,ProjName);
			else if( instr.Find(PCB_NAME) > 0 )
				instr.Replace(PCB_NAME,PcbName);
			else if( instr.Find(".n_pages") > 0 )
			{
				CString npages;
				npages.Format("%d", nPages+1);
				instr.Replace(".n_pages",npages);
			}
			else if( instr.Find(".n_page") > 0 )
			{
				CString npage;
				npage.Format("%d|.n_page", nPage+1);
				instr.Replace(".n_page",npage);
			}
			else if( instr.Find("\".") > 0 || 
				instr.Find( MOVABLE ) > 0 ||
				instr.Find("\"(col") > 0 )//skip command lines
				continue;
			
			if( instr.Left(12) == "add_new_page" ||
				instr.Left(5) == "[end]" )
			{
				bFlagWriteToMem = TRUE;
				CString line;
				for( int i=0; i<TextData[nPage].GetSize(); i++)
				{
					CText * t = TextData[nPage].GetAt(i);
					if( t->m_str.Left(1) == "." ) //skip command line
						continue;
					line.Format( "polyline: %d %d %d %d %d %d %d %d\n", 2, 0, pLayer, NM_PER_MIL, -1, -1, -1, 0);
					w.WriteString( line );
					line.Format( "  corner: %d %d %d %d %d\n", 1, 0, 0, 0, 0 );
					w.WriteString( line );
					line.Format( "  corner: %d %d %d %d %d\n", 2, NM_PER_MM, 0, 0, 0 );
					w.WriteString( line );
					line.Format( "description: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", 
						t->m_str,
						t->m_x, 
						t->m_y, 
						t->m_layer, 
						t->m_angle, 
						t->m_font_size, 
						t->m_stroke_width,
						t->m_merge, 
						t->m_submerge,
						t->m_font_number,
						t->m_pdf_options );
					w.WriteString( line );
					w.WriteString("\n");
				}
				nPage++;
				if( instr.Left(5) == "[end]" )
				{
					for( ; nPage<=nPages; nPage++ )
					{
						if( TextData[nPage].GetSize() == 0 )
							continue;
						line.Format("add_new_page: \"Page %d\"", nPage+1 );
						w.WriteString( line );
						w.WriteString("\n");
						for( int imem=1; imem<MemStr.GetSize(); imem++ )
						{
							line = MemStr.GetAt(imem);
							if( line.Find("2|.n_page") > 0 )
							{
								CString npage;
								npage.Format("%d", nPage+1);
								line.Replace("2|.n_page",npage);
							}
							w.WriteString( line );
							w.WriteString("\n");
						}
						for( int i=0; i<TextData[nPage].GetSize(); i++)
						{
							CText * t = TextData[nPage].GetAt(i);
							if( t->m_str.Left(1) == "." ) //skip command line
								continue;
							line.Format( "polyline: %d %d %d %d %d %d %d %d\n", 2, 0, pLayer, NM_PER_MIL, -1, -1, -1, 0);
							w.WriteString( line );
							line.Format( "  corner: %d %d %d %d %d\n", 1, 0, 0, 0, 0 );
							w.WriteString( line );
							line.Format( "  corner: %d %d %d %d %d\n", 2, NM_PER_MM, 0, 0, 0 );
							w.WriteString( line );
							line.Format( "description: \"%s\" %d %d %d %d %d %d %d %d %d %d\n", 
								t->m_str,
								t->m_x, 
								t->m_y, 
								t->m_layer, 
								t->m_angle, 
								t->m_font_size, 
								t->m_stroke_width,
								t->m_merge, 
								t->m_submerge,
								t->m_font_number,
								t->m_pdf_options );
							w.WriteString( line );
							w.WriteString("\n");
						}
					}
				}
				else if( TextData[nPage].GetSize() == 0 )
					instr = "";
			}
			w.WriteString( instr );
			w.WriteString("\n");
			if( bFlagWriteToMem )
				MemStr.Add(instr);
		}
		r.Close();
		w.Close();
		if( (UINT)ShellExecute(	NULL,"open", 
			"\""+m_doc->m_app_dir+"\\freecds.exe\"", 
			"\""+s0+"\\"+title + " - " + FILE_NAME + ".cds\"", 
			m_doc->m_app_dir, SW_SHOWNORMAL ) <= 32 )
		{
			ShellExecute(	NULL,"open", s0, NULL, s0, SW_SHOWNORMAL );
		}
	}
	else if( ok1 )
		r.Close();
	else if( ok2 )
		w.Close();
	// end
	it = -1;
	for( CText *t=m_doc->m_tlist->GetNextText(&it); t; t=m_doc->m_tlist->GetNextText(&it) )
	{
		t->InVisible();
	}
	if( WARNING.GetLength() )
		AfxMessageBox( WARNING, MB_ICONINFORMATION );
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
CString BomInTable::GetURL( CString * v, CString * f, int * it, int * ip )
{
	static CString mem="";
	if( *it == -1 && *ip == -1 )
	{
		mem = dSPACE;
		*ip = 0;
		CString path = m_doc->m_app_dir + main_component + "\\" + *v + "@" + *f +".url";
		CStdioFile ff;
		if( ff.Open( path, CFile::modeRead, NULL ) )
		{
			CString str;
			ff.ReadString(str);
			ff.ReadString(str);
			ff.Close();
			if( str.GetLength() > 4 )
				str = str.Right( str.GetLength()-4 );
			str = str.Trim();
			if( str.GetLength() )
				if( mem.Find( dSPACE+str+dSPACE ) == -1 )
				{
					mem += (str+dSPACE);
					return str;
				}
		}
	}
	for(CText*t=m_doc->Pages.GetNextAttr( index_desc_attr, it, ip, 0 );
		t;
		t=m_doc->Pages.GetNextAttr( index_desc_attr, it, ip, 0 ) )
	{
		if( m_doc->Pages.IsThePageIncludedInNetlist( *ip, TRUE ) == 0 &&
			m_doc->Pages.IsThePageIncludedInNetlist( *ip, FALSE ) )
			continue;
		if( t->m_str.GetLength() > 20 )
		{
			int cpf = t->m_str.Find(CP_LINK);
			if( cpf > 0 )
				if( t->m_str.Find("www") > 0 || t->m_str.Find("//") > 0 )
				{
					CArray<CPolyLine> * po = m_doc->Pages.GetPolyArray( *ip );
					if( t->m_polyline_start >= 0 && t->m_polyline_start < po->GetSize() )
					{
						CPolyLine * p = &po->GetAt( t->m_polyline_start );
						if( CText * pv = p->Check( index_value_attr ) )
							if( CText * pf = p->Check( index_foot_attr ) )
							{
								CString gv = pv->m_str;
								CString gf = pf->m_str;
								ExtractComponentName( &gv, &gf );
								if( v->Compare( gv ) == 0 )
									if( f->Compare( gf ) == 0 )
									{
										CString str = t->m_str.Right( t->m_str.GetLength() - cpf - 6 );
										cpf = str.Find(CP_PAGE_NUM);
										if( cpf > 0 )
											str = str.Left( cpf );
										str.Replace("'"," ");
										str = str.Trim();
										if( mem.Find( dSPACE+str+dSPACE ) == -1 )
										{
											mem += (str+dSPACE);
											return str;
										}
									}
							}
					}
				}
		}
	}
	return "";
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::PushRows( int Y, int ip )
{
	for( int index=(shiftY<0?ip:nPages); (index>=ip && index<=nPages); shiftY<0?index++:index-- )
	{
		//int it = -1;
		for( int i=TextData[index].GetSize()-1; i>=0; i-- )
		{
			CText * t=TextData[index].GetAt(i);
			if( t->m_y <= pgTop[index] )
			if( t->m_y < Y || index > ip )
			{
				if( t->m_y >= pgBottom[index] )
				{
					t->m_y -= shiftY;
					if( t->m_y < pgBottom[index] && shiftY > 0 )
					{
						if( index == nPages )
							nPages++;
						if( nPages >= MAX_PAGES )
						{
							t->m_y += shiftY;
							return -1; // Fail
						}
						TextData[index].RemoveAt(i);
						t->m_y = pgTop[index+1];
						TextData[index+1].Add(t);	
					}
					else if( t->m_y > pgTop[index] && index > 0 && shiftY < 0 )
					{
						TextData[index].RemoveAt(i);
						t->m_y = pgBottom[index-1] + ((pgTop[index-1]-pgBottom[index-1])%shiftY);
						TextData[index-1].Add(t);	
					}
				}
			}
		}
	}
	return nPages;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::ReadParamsFromTagFile()
{
	const int defLayer = LAY_ADD_1+3;
	curY = 0;
	shiftY = 0;
	textH = 0;
	textW = 0;
	fontNum = 0;
	pdfOptions = 0;
	nPages = 0;
	e_reflist = 0;
	i_reflist = 0;
	m_entire = 0;
	pLayer = defLayer;
	WARNING = "";
	Sources = dSPACE;

	for(int i=0; i<MaxCols; i++)
	{
		Xs[i] = 0;
		Ws[i] = 30*NM_PER_MM;
		tLayers[i] = defLayer;
	}
	for(int i=0; i<MAX_PAGES; i++)
	{
		pgTop[i] = 0;
		pgBottom[i] = 0;
		TextData[i].RemoveAll();
	}
	int it = -1;
	for(CText * t=m_doc->m_tlist->GetNextText(&it); t; t=m_doc->m_tlist->GetNextText(&it))
		t->InVisible();
	CStdioFile rParam;
	int ok = rParam.Open( m_doc->m_path_to_folder+"\\related_files\\reports\\bom\\bom.cds", CFile::modeRead, NULL );
	if( ok )
	{
		CString instr, key;
		CArray<CString> arr;
		int row2Y = 0;
		int cur = 0; // current page number
		int currPLayer = 0;
		int currTLayer = 0;
		while( rParam.ReadString( instr ) )
		{
			instr = instr.Trim();
			int np = ParseKeyString( &instr, &key, &arr );
			if( key == "description" && np >= 2 )
			{
				int x = my_atoi( &arr[1] );
				int y = my_atoi( &arr[2] );
				currTLayer = my_atoi( &arr[3] );
				int tH = my_atoi( &arr[5] );
				int tW = my_atoi( &arr[6] );
				
				if( arr[0].Left(4) == ".col" )
				{
					nPages = cur;
					pdfOptions = 1;
					if ( np >= 12 )
						pdfOptions = my_atoi( &arr[10] );
					CString ref = arr[0].Right( arr[0].GetLength() - 1 );
					CString pref, suff;
					int col_num = ParseRef(&ref, &pref, &suff);
					if(col_num>0 && col_num<=MaxCols)
					{
						if( suff.GetLength() == 0 )
						{
							Xs[col_num-1] = x;
							curY = y;
							textH = tH;
							textW = tW;
							if( pLayer == defLayer )
								pLayer = currPLayer;
							if( tLayers[col_num-1] == defLayer )
								tLayers[col_num-1] = currTLayer;
						}
						else
							Ws[col_num-1] = my_atoi( &suff.Right( suff.GetLength()-2 ) );
					}
				}

				//
				if( arr[0].Left(5) == ".row2" )
				{
					row2Y = y;
				}
				else if( arr[0].Left(4) == ".top" )
				{
					pgTop[cur] = y;
				}
				else if( arr[0].Left(7) == ".bottom" )
				{
					pgBottom[cur] = y;
				}
				else if( arr[0].Left(8) == ".exclude" )
				{
					CString s = arr[0].Right( arr[0].GetLength()-8 );
					s = s.Trim();
					e_reflist = m_doc->m_ref_lists->GetIndex(s);
					if( e_reflist >= 0 )
						e_reflist += REF_LIST_INDEX;
					else
						e_reflist = 0;
				}
				else if( arr[0].Left(8) == ".include" )
				{
					CString s = arr[0].Right( arr[0].GetLength()-8 );
					s = s.Trim();
					i_reflist = m_doc->m_ref_lists->GetIndex(s);
					if( i_reflist >= 0 )
						i_reflist += REF_LIST_INDEX;
					else
						i_reflist = 0;
				}	
				else if( arr[0].Find(".entire") == 0 )
				{
					m_entire = TRUE;
				}
				else if( arr[0].Left(10) == ".file_name" )
				{
					FILE_NAME = arr[0].Right( arr[0].GetLength()-10 );
					ExtractComponentName( &FILE_NAME, NULL );
				}
				else if( arr[0].Left(1) == "." || arr[0].Find( MOVABLE ) > 0 )
				{
					CText * T = m_doc->GetFreeNet(&arr[0], x, y, tH, tW );
					T->m_layer = currTLayer;
					TextData[cur].Add( T );
					T->MakeVisible();
				}
			}
			else if( key == "default_font" && np >= 2 )
			{
				fontNum = my_atoi( &arr[0] );
			}
			else if( key == "add_new_page" && np >= 2 )
			{
				cur++;
			}
			else if( key == "polyline" && np >= 4 )
			{
				currPLayer = my_atoi( &arr[2] );
			}
			else if( key == "bmp_src" && np >= 2 )
			{
				if( Sources.Find(arr[0]+dSPACE) == -1 )
					Sources += (arr[0]+dSPACE);
			}
		}
		rParam.Close();
		if( row2Y == 0 )
			row2Y = curY;
		shiftY = curY - row2Y;
	}
	else 
	{
		AfxMessageBox("Unable to open file bom.cds", MB_ICONERROR);
		return 1;
	}
	for(int i=2; i<MAX_PAGES; i++)
	{
		pgTop[i] = pgTop[i-1];
		pgBottom[i] = pgBottom[i-1];
	}
	return 0;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
CString BomInTable::GetCName( CString * V, CString * F )
{
	CString gv = *V;
	CString gf = *F;
	ExtractComponentName( &gv, &gf );
	CString fName = gv + "@" + gf + ".txt";
	CString sfile = m_doc->m_app_dir + main_component + "\\" + fName;
	CStdioFile f;
	int ok = f.Open( sfile, CFile::modeRead, NULL );
	if( ok )
	{
		CString instr;
		if( f.ReadString(instr) )
		{
			instr = instr.Trim();
			instr.Replace( "\"", "*" );
		}
		f.Close();
		return instr;
	}
	return "";
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
void BomInTable::AddObject( int page, CString * CNT, CString * R, CString * V, CString * F )
{
	CText * T;
	if( Xs[RefDes] )
	{
		T = m_doc->GetFreeNet(R, Xs[RefDes], curY, textH, textW );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[RefDes];
		T->MakeVisible();
		int np = -1;
		while( CText* newT = ParseString( T, Ws[RefDes] ) )
			np = AddPString( newT, page, np, RefDes );
		if( np == -1 )
			TextData[page].Add( T );
		else
			T->InVisible();
	}
	CString cName;
	cName = GetCName( V, F );
	if( Xs[CName] && cName.GetLength() )
	{
		T = m_doc->GetFreeNet(&cName, Xs[CName], curY, textH, textW );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[CName];
		T->MakeVisible();
		int np = -1;
		while( CText* newT = ParseString( T, Ws[CName] ) )
			np = AddPString( newT, page, np, CName );
		if( np == -1 )
			TextData[page].Add( T );
		else
			T->InVisible();
	}
	if( Xs[Value] && V->GetLength() )
	{
		T = m_doc->GetFreeNet(V, Xs[Value], curY, textH, textW );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Value];
		T->MakeVisible();
		int np = -1;
		while( CText* newT = ParseString( T, Ws[Value] ) )
			np = AddPString( newT, page, np, Value );
		if( np == -1 )
			TextData[page].Add( T );
		else
			T->InVisible();
	}
	if( Xs[Footprint] && F->GetLength() )
	{
		T = m_doc->GetFreeNet(F, Xs[Footprint], curY, textH, textW );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Footprint];
		T->MakeVisible();
		int np = -1;
		while( CText* newT = ParseString( T, Ws[Footprint] ) )
			np = AddPString( newT, page, np, Footprint );
		if( np == -1 )
			TextData[page].Add( T );
		else
			T->InVisible();
	}
	int it = -1, ip = -1, step = 0;
	ExtractComponentName( V, F );
	CString URL = GetURL( V, F, &it, &ip );
	while( Xs[CURL] && URL.GetLength() )
	{
		CString Link = "^|LINK: " + URL;
		T = m_doc->GetFreeNet(&Link, Xs[CURL]+(step*textH*2), curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[CURL];
		T->MakeVisible();
		URL = GetURL( V, F, &it, &ip );
		step++;
	}
	if( Xs[Quantity] )
	{
		T = m_doc->GetFreeNet(CNT, Xs[Quantity], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Quantity];
		T->MakeVisible();
	}
	if( Xs[LNumber] )
	{
		CString strCnt;
		strCnt.Format("%d", 0);
		T = m_doc->GetFreeNet(&strCnt, Xs[LNumber], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[LNumber];
		T->MakeVisible();
	}
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
void BomInTable::RemoveEmptyRows()
{
	for( int page=0; page<=nPages; page++ )
	{
		for( int i=TextData[page].GetSize()-1; i>=0; i-- )
		{
			CText * data = TextData[page].GetAt(i);
			if( abs(data->m_x - Xs[RefDes]) < NM_PER_MM )
			{
				if( data->m_str.Left(1) == "." && 
					data->m_str.Find(PR_NAME) < 0 &&
					data->m_str.Find(PCB_NAME) < 0 )
				{
					int bHDelete = 0;
					TextData[page].RemoveAt(i);
					PushBack( data->m_y+(shiftY/2), page );
					int topT = -1;
					for( int i2=TextData[page].GetSize()-1; i2>=0; i2-- )
					{
						CText * t = TextData[page].GetAt(i2);
						if( (t->m_y - data->m_y) < (shiftY*3/2) &&
							(t->m_y - data->m_y) > (shiftY/2) )
						{
							if( t->m_str.Find( MOVABLE ) > 0 )
							{
								topT = i2;
								setbit( bHDelete, 0 );
							}
						}
						if( abs(t->m_y - data->m_y) < (shiftY/2) )
						{
							if( t->m_str.Find( MOVABLE ) > 0 )
							{
								setbit( bHDelete, 1 );
							}
						}
					}	
					if( bHDelete == 2 && page > 0 )
					{
						for( int i2=TextData[page-1].GetSize()-1; i2>=0; i2-- )
						{
							CText * t = TextData[page-1].GetAt(i2);
							if( abs(t->m_y - pgBottom[page-1]) < (shiftY/2) )
							{
								if( t->m_str.Find( MOVABLE ) > 0 )
								{
									page--;
									topT = i2;
									setbit( bHDelete, 0 );
									break;
								}
							}
						}
					}
					if( bHDelete == 3 )
					{
						PushBack( TextData[page].GetAt(topT)->m_y-(shiftY/2), page );
						TextData[page].RemoveAt(topT);
					}
					i=TextData[page].GetSize();
				}
			}
		}
	}
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::PushBack( int Y, int ip )
{
	shiftY = -shiftY;
	nPages = PushRows( Y, ip );
	shiftY = -shiftY;
	return nPages;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::RedrawNumbers()
{
	int nC = 0;
	for( int page=0; page<=nPages; page++ )
	{
		int BEST_HIT;
		int BEST_Y;
		int CurrentY = INT_MAX;
		do
		{
			BEST_HIT = -1;
			BEST_Y = INT_MIN;
			for( int i=TextData[page].GetSize()-1; i>=0; i-- )
			{
				CText * data = TextData[page].GetAt(i);
				if( data->m_str.Find(".col") == 0 )
					continue;
				if( abs(data->m_x - Xs[LNumber]) < NM_PER_MIL )
					if( data->m_y < CurrentY )
						if( data->m_y <= pgTop[page] && data->m_y >= pgBottom[page] )
						{
							if( BEST_Y < data->m_y )
							{
								BEST_HIT = i;
								BEST_Y = data->m_y;
							}
						}
			}
			if( BEST_HIT >= 0 )
			{
				CurrentY = BEST_Y;
				CText * t = TextData[page].GetAt( BEST_HIT );
				t->m_str.Format("%d", ++nC );
				TextData[page].GetAt( BEST_HIT )->m_nchars = t->m_str.GetLength();
			}
		}while( BEST_HIT >= 0 );
	}
	return nC;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::FindNextRow( CString * R )
{
	CString sort = CSORT;
	int iof = R->Find(",");
	if( iof > 0 )
		R->Truncate( iof );
	CString pref, suff;
	int num = ParseRef( R, &pref, &suff );
	int i1 = sort.Find( pref.Left(1) );
	int ii1 = -1;
	if( pref.GetLength() > 1 )
		ii1 = sort.Find( pref.Mid(1,1) );
	int BEST_HIT = -1;
	for( int idx=0; idx<arrR.GetSize(); idx++ )
	{
		CString str = arrR.GetAt(idx);
		iof = str.Find(",");
		if( iof > 0 )
			str.Truncate( iof );
		CString pref2, suff2;
		int num2 = ParseRef( &str, &pref2, &suff2 );
		if( pref2.GetLength() == 0 )
			continue;
		int i2 = sort.Find( pref2.Left(1) );
		if( i2 < i1 )
			continue;
		int ii2 = -1;
		if( pref2.GetLength() > 1 )
			ii2 = sort.Find( pref2.Mid(1,1) );
		if( i2 == i1 )
		{
			if( ii2 < ii1 )
				continue;
			else if( ii2 == ii1 && num2 < num )
				continue;
		}
		if( BEST_HIT == -1 )
		{
			BEST_HIT = idx;
			continue;
		}

		CString str3 = arrR.GetAt(BEST_HIT);
		iof = str3.Find(",");
		if( iof > 0 )
			str3.Truncate( iof );
		CString pref3, suff3;
		int num3 = ParseRef( &str3, &pref3, &suff3 );
		int i3 = sort.Find( pref3.Left(1) );
		if( i3 < i2 )
			continue;
		int ii3 = -1;
		if( pref3.GetLength() > 1 )
			ii3 = sort.Find( pref3.Mid(1,1) );
		if( i3 == i2 )
		{
			if( ii3 < ii2 )
				continue;
			else if( ii3 == ii2 && num3 < num2 )
				continue;
		}
		BEST_HIT = idx;
	}
	return BEST_HIT;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
CText * BomInTable::ParseString( CText * T, int W )
{
	static int cur = 0;
	if( T->m_dlist == NULL )
		T->MakeVisible();
	RECT r;
	T->m_tl->GetTextRectOnPCB(T,&r);
	int tw = (r.right-r.left);
	if( tw > W )
	{
		float nrows = (float)tw/(float)W;
		int gLen = T->m_str.GetLength();
		int num_chars = (float)gLen/nrows;
		num_chars = max( num_chars, 5 );
		if( cur < gLen )
		{
			CString str = T->m_str.Right( gLen-cur );
			int iadd = min( str.GetLength(), num_chars );
			CString str_out = str.Left( iadd );
			if( cur+iadd < gLen )
			{
				int mem_iadd = iadd;
				while( iadd )
				{
					if( str_out.Right(1) != " " )
						iadd--;
					else
						break;
					str_out = str.Left( iadd );
				}
				if( str_out.Trim().GetLength() == 0 )
				{
					str_out = str.Left( mem_iadd );
					iadd = mem_iadd;
				}
			}
			cur += iadd;
			return m_doc->GetFreeNet(&str_out, 0, 0, textH, textW );
		}
	}
	cur = 0;
	return NULL;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int BomInTable::AddPString( CText * T, int n_page, int n_pushes, int col )
{
	static int NPushes = 0;
	static int prev_y = 0;
	if( T == NULL )
	{
		NPushes = 0;
		return n_pushes;
	}
	if( prev_y != curY )
		NPushes = 0;
	prev_y = curY;
	n_pushes++;

	T->m_x = Xs[col];
	int new_y = curY;
	int new_page = n_page;
	for( int i=0; i<n_pushes; i++ )
	{
		new_y -= shiftY;
		if( new_y < pgBottom[new_page] )
		{
			new_page++;
			if( new_page == MAX_PAGES )
				return n_pushes-1;
			new_y = pgTop[new_page];
		}
	}
	if( n_pushes > NPushes )
	{
		nPages = PushRows( new_y+(shiftY/2), new_page );
	}
	T->m_y = new_y;
	TextData[new_page].Add( T );
	T->m_pdf_options = pdfOptions;
	T->m_font_number = fontNum;
	T->m_layer = tLayers[col];
	T->MakeVisible();
	NPushes = max( NPushes, n_pushes );
	return n_pushes;
}