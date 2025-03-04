#include "stdafx.h"
#include "FreeSch.h"
#include "TagTable.h"
#include "BomInTable.h"
///#include "afxtempl.h"

//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
void TagTable::MakeReport( CFreePcbDoc * doc )
{
	// ini
	MOVABLE = "|movable";
	PR_NAME = ".project_name";
	PCB_NAME = ".pcb_name";

	m_doc = doc;
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

	// Get Partlist Data
	Partlist.SetSize(0);
	m_doc->ExportPCBNetlist( NULL, EXPORT_PARTS, 0, &Partlist );

	AddPString( NULL,0,0,0 );
	CString pref="A0", V, F;
	int pgi = -1;
	for( int best_i=FindNextPart( &pref, &V, &F, (Xs[PGI]?&pgi:NULL) );
				 best_i>=0 && nPages>=0;
				 best_i=FindNextPart( &pref, &V, &F, (Xs[PGI]?&pgi:NULL) ) )
	{
		CString refText, pref2;
		int Bcount = GetBomRepeats( &pref, &pref2, &V, &F );
		if( Bcount )
		{
			if( Bcount > 1 )
				refText.Format("%s...%s", pref, pref2 );
			else
				refText.Format("%s, %s", pref, pref2 );
			pref = pref2;
		}
		else
			refText = pref;
		CString prefix, sfx;
		ParseRef( &pref, &prefix, &sfx );
		int page = nPages;
		int startPg = 0;
		int startY = 0;
		for( ; (page>=0&&nPages<MAX_PAGES); page-- )
		{
			for( int i=TextData[page].GetSize()-1; i>=0; i-- )
			{
				CText * data = TextData[page].GetAt(i);
				if( abs(data->m_x - Xs[RefDes]) < NM_PER_MM )
				{
					if( data->m_str.Find(".col") == 0 && data->m_str.GetLength() == 5 )
					{
						startY = data->m_y;
						startPg = page;
					}
					if( data->m_str.Compare("."+prefix) == 0 )
					{
						curY = data->m_y;
						nPages = PushRows( curY+(shiftY/2), page );
						if( nPages >= 0 )
							AddObject( page, Bcount+1, &refText, &V, &F, pgi );
						page = -1;// break
						break;
					}
				}	
			}
		}
		if( page == -1 )
		{
			if( !startY )
				ASSERT(0);
			curY = startY;
			page = startPg;
			nPages = PushRows( curY+(shiftY/2), page );
			if( nPages >= 0 )
				AddObject( page, Bcount+1, &refText, &V, &F, pgi );
		}
	}
	if( nPages == -1 )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Big amount of data! The program currently "\
			"supports up to eight pages. Try changing the partlist "\
			"generation parameters in the tag.cds file. "\
			"Open this file with a schematic editor in the "\
			"same way as the project files. (This file is located "\
			"in the program's executable folder)":
			"Большой объем данных! В настоящее время программа "\
			"поддерживает до восьми страниц. Попробуйте изменить параметры генерации "\
			"списка деталей в файле tag.cds. "\
			"Откройте этот файл в редакторе схем тем же способом, что и файлы проекта. (Этот файл находится "\
			"в исполняемой папке программы)", MB_ICONERROR );
	nPages = MAX_PAGES-1;
	RemoveEmptyRows();
	int cnt_pgs = 0;
	for( int i=0; i<MAX_PAGES;i++ )
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
	nPages = min( MAX_PAGES-1, (cnt_pgs-1) );
	RedrawNumbers();


	CString s0 = m_doc->m_path_to_folder+"\\related_files\\reports\\tag";
	struct _stat buf;
	if( _stat( s0, &buf ) )
	{
		ASSERT(0);
	}

	CString title = m_doc->Pages.GetCurrentPCBName();
	if( title.GetLength() > 4 )
		if( title.Right(4) == ".fpc" || title.Right(4) == ".cds" )
			title.Truncate( title.GetLength() - 4 );
	CString s1 = s0 + "\\tag.cds";
	CString s2 = s0 + "\\" + title + " - " + FILE_NAME + ".cds";
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
			G_LANGUAGE == 0 ? ("\""+m_doc->m_app_dir+"\\freecds.exe\""): ("\"" + m_doc->m_app_dir + "\\СхемАтор.exe\""),
			"\""+s0+"\\"+title+" - "+FILE_NAME+".cds\"", 
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
int TagTable::PushRows( int Y, int ip )
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
int TagTable::ReadParamsFromTagFile()
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
	pLayer = defLayer;
	COL3W = 10*NM_PER_MM;
	WARNING = "";
	Sources = dSPACE;

	for(int i=0; i<MaxCols; i++)
	{
		Xs[i] = 0;
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
	int ok = rParam.Open( m_doc->m_path_to_folder+"\\related_files\\reports\\tag\\tag.cds", CFile::modeRead, NULL );
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
							curY = y;
							Xs[col_num-1] = x;
							textH = tH;
							textW = tW;
							if( pLayer == defLayer )
								pLayer = currPLayer;
							
							if( tLayers[col_num-1] == defLayer )
								tLayers[col_num-1] = currTLayer;
						}
						else
							COL3W = my_atoi( &suff.Right( suff.GetLength()-2 ) );
					}
				}
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
		AfxMessageBox(G_LANGUAGE == 0 ? "Unable to open file tag.cds":"Невозможно открыть файл tag.cds", MB_ICONERROR);
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
int TagTable::GetBomRepeats( CString * pref, CString * pref2, CString * V, CString * F )
{
	CString sPref = *pref, prefix, suffix;
	int num = ParseRef( pref, &prefix, &suffix );
	int pnum = num;
	CString sV = *V;
	CString sF = *F;
	// default
	*pref2 = *pref;
	int repeats = 0;
	while( 1 )
	{
		int i = FindNextPart( &sPref, &sV, &sF );
		if( i < 0 )
			break;
		CString sPrefix, sSuffix;
		int snum = ParseRef( &sPref, &sPrefix, &sSuffix );
		if( prefix.Compare(sPrefix) )
			break;
		if( suffix.Compare(sSuffix) == 0 )
		{
			if( (pnum+1) != snum )
				break;
		}
		else
		{
			if( pnum != snum && (pnum+1) != snum )
				break;
			if( suffix.GetLength() == 0 )
				break;
			if( sSuffix.GetLength() == 0 )
				break;
		}
		if( V->Compare(sV) )
			break;
		if( F->Compare(sF) )
			break;

		pnum = snum;
		*pref2 = sPref;
		repeats++;
	}
	return repeats;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int TagTable::LoadPartlistFromString( CString * partlist )
{
	e_reflist = 0;
	i_reflist = 0;
	*partlist = "key: " + *partlist;
	partlist->Replace(","," ");
	partlist->Replace("'"," ");
	CString key;
	int np = ParseKeyString( partlist, &key, &Partlist );
	for( int i=0; i<Partlist.GetSize(); i++ )
	{
		CString str = Partlist.GetAt(i);
		Partlist.SetAt( i, str+dSPACE+"---@---" );
	}
	return np;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int TagTable::GetPartID( CString * ref )
{
	CString sort = CSORT;
	CString pref, suff;
	int p_i = ParseRef( ref, &pref, &suff );
	int RET = sort.Find( pref[0] )*10000000;
	if( pref.GetLength() > 1 )
		RET += sort.Find( pref[1] )*1000000;
	RET += p_i*10000;
	if( suff.GetLength() > 3 )
		RET += sort.Find( suff[3] )*1000;
	if( suff.GetLength() > 2 )
		RET += sort.Find( suff[2] )*100;
	if( suff.GetLength() > 1 )
		RET += sort.Find( suff[1] )*10;
	if( suff.GetLength() )
		RET += sort.Find( suff[0] );
	if( suff.GetLength() > 4 )
		for( int i=4; i<suff.GetLength(); i++ )
			RET += sort.Find( suff[i] );
	return RET;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int TagTable::FindNextPart( CString * ref, CString * V, CString * F, int * printable_area_index )
{
	static int counter = 0;

	int p_id = GetPartID( ref );
	int BEST_I = -1, BEST_ID = INT_MAX;
	for( int i=0; i<Partlist.GetSize(); i++ )
	{
		CString gref = Partlist.GetAt(i);
		int i_sp = gref.Find( dSPACE );
		if( i_sp <= 0 )
			ASSERT(0);
		gref.Truncate(i_sp);
		int g_id = GetPartID( &gref );
		if( g_id <= p_id )
			continue;
		if( i_reflist )
		{
			CText * t = NULL;
			m_doc->Pages.FindPart( &gref, &t, TRUE, TRUE );
			if( t )
				if( getbit( t->m_pdf_options, i_reflist ) == 0 )
					continue;
		}
		if( e_reflist )
		{
			CText * t = NULL;
			m_doc->Pages.FindPart( &gref, &t, TRUE, TRUE );
			if( t )
				if( getbit( t->m_pdf_options, e_reflist ) )
					continue;
		}
		if( g_id < BEST_ID )
		{
			BEST_ID = g_id;
			BEST_I = i;
		}
	}
	if( BEST_I >= 0 )
	{
		CString gref = Partlist.GetAt(BEST_I);
		int i_sp = gref.Find( dSPACE );
		if( i_sp <= 0 )
			ASSERT(0);
		int i_dog = gref.Find( "@" );
		if( i_dog <= 0 )
			ASSERT(0);
		CString old_ref = *ref;
		*ref = gref;
		ref->Truncate(i_sp);
		{
			// test
			CString new_pref, new_suff, old_pref, old_suff;
			int old_num = ParseRef( &old_ref, &old_pref, &old_suff );
			int new_num = ParseRef( ref, &new_pref, &new_suff );
			if( old_pref.Compare( new_pref ) == 0 /*&& old_suff.Compare( new_suff ) == 0*/ )
				if( new_num - old_num > 1 )
					if( WARNING.GetLength() )
					{
						CString s1,s2;
						s1.Format("%s ", old_ref );
						s2.Format(" %s", *ref );
						if( WARNING.Find(s1) < 0 && WARNING.Find(s2) < 0 )
						{
							CString s;
							s.Format("%s then %s", old_ref, *ref );
							if( counter%3 == 0 )
								WARNING += (",\n" + s);
							else
								WARNING += (",\t" + s);
							counter++;
						}
					}
					else
					{
						counter = 1;
						CString s;
						s.Format("%s then %s", old_ref, *ref );
						WARNING.Format("Your circuit design contains gaps in part numbering:\n\n%s", s );
					}
		}
		*V = gref.Mid( i_sp, i_dog-i_sp );
		*V = V->Trim();
		*F = gref.Right( gref.GetLength()-i_dog-1 );
		if( printable_area_index )
		{
			CString getref = *ref;
			*printable_area_index = m_doc->Pages.GetPrintableAreaIndex( &getref );
		}
	}
	return BEST_I;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
/*TagStruct * TagTable::FindNextPart( CString * pref, int * num, CString * suff, CString * V, CString * F )
{
	static int counter = 0;
	static TagStruct RET;

	int MARK = 9;
	if( pref->GetLength() == 0 )
		return NULL;
	CString sort = CSORT;
		
	int i1 = sort.Find( pref->Left(1) );
	int ii1 = -1;
	
	RET.CP_REF = "";
	RET.BEST_T = NULL;
	RET.BEST_IP = -1;
	RET.n_parts = 0;
	

	// iof prefix
	if( pref->GetLength() > 1 )
		ii1 = sort.Find( pref->Mid(1,1) );

	// iof suffix
	int iS1 = -1;
	if( suff->GetLength() )
		iS1 = sort.Find( suff->Right(1) );

	int it=-1, ip=-1;
	for( CText * t=m_doc->Pages.GetNextAttr(index_part_attr, &it, &ip );
				 t;
				 t=m_doc->Pages.GetNextAttr(index_part_attr, &it, &ip ))
	{
		if( t->utility == MARK )
			continue;
		if( m_doc->Pages.IsThePageIncludedInNetlist( ip, 0 ) == 0 &&
			m_doc->Pages.IsThePageIncludedInNetlist( ip, 1 ) )// the page included in different netlist
			continue;
		if( i_reflist )
			if( getbit( t->m_pdf_options, i_reflist ) == 0 )
				continue;
		if( e_reflist )
			if( getbit( t->m_pdf_options, e_reflist ) )
				continue;
		CString pref2, suff2;
		int num2 = ParseRef( &t->m_str, &pref2, &suff2 );
		if( pref2.GetLength() == 0 )
			continue;
		int i2 = sort.Find( pref2.Left(1) );
		if( i2 < i1 )
			continue;

		// iof prefix
		int ii2 = -1;
		if( pref2.GetLength() > 1 )
			ii2 = sort.Find( pref2.Mid(1,1) );

		// iof suffix
		int iS2 = -1;
		if( suff2.GetLength() )
			iS2 = sort.Find( suff2.Right(1) );

		if( i2 == i1 )
		{
			if( ii2 < ii1 )
				continue;
			else if( ii2 == ii1 )
			{
				if( num2 < *num )
					continue;
				else if( num2 == *num )
				{
					if( iS2 < iS1 )
						continue;
				}
			}
		}
		if( RET.BEST_T == NULL )
		{
			RET.BEST_T = t;
			RET.BEST_IP = ip;
			continue;
		}
		CString pref3, suff3;
		int num3 = ParseRef( &RET.BEST_T->m_str, &pref3, &suff3 );
		int i3 = sort.Find( pref3.Left(1) );
		if( i3 < i2 )
			continue;

		// iof prefix
		int ii3 = -1;
		if( pref3.GetLength() > 1 )
			ii3 = sort.Find( pref3.Mid(1,1) );

		if( i3 == i2 )
		{
			if( ii3 < ii2 )
				continue;
			else if( ii3 == ii2 )
			{
				if( num3 < num2 )
					continue;
				else if( num3 == num2 )
				{
					// iof suffix
					int iS3 = -1;
					if( suff3.GetLength() )
						iS3 = sort.Find( suff3.Right(1) );
					if( iS3 < iS2 )
						continue;
				}
			}
		}
		RET.BEST_T = t;
		RET.BEST_IP = ip;
	}
	if( RET.BEST_T )
	{
		RET.BEST_T->utility = MARK;

		CString pref3, suff3;
		int new_num = ParseRef( &RET.BEST_T->m_str, &pref3, &suff3 );
		if( pref->Compare( pref3 ) == 0 && suff->Compare( suff3 ) == 0 )
			if( new_num - (*num) > 1 )
				if( WARNING.GetLength() )
				{
					CString s1,s2;
					s1.Format("%s%d ",*pref, *num );
					s2.Format(" %s%d", pref3, new_num );
					if( WARNING.Find(s1) < 0 && WARNING.Find(s2) < 0 )
					{
						CString s;
						s.Format("%s%d then %s%d",*pref, *num, pref3, new_num );
						if( counter%3 == 0 )
							WARNING += (",\n" + s);
						else
							WARNING += (",\t" + s);
						counter++;
					}
				}
				else
				{
					counter = 1;
					CString s;
					s.Format("%s%d then %s%d",*pref, *num, pref3, new_num );
					WARNING.Format("Your circuit design contains gaps in part numbering:\n\n%s", s );
				}
		*num = new_num;
		*pref = pref3;
		*suff = suff3;
		CArray<CPolyLine> * pP = m_doc->Pages.GetPolyArray( RET.BEST_IP );
		if( RET.BEST_T->m_polyline_start < 0 )
			ASSERT(0);
		if( RET.BEST_T->m_polyline_start >= pP->GetSize() )
			ASSERT(0);
		CText * gV = pP->GetAt(RET.BEST_T->m_polyline_start).Check( index_value_attr );
		if( gV )
			*V = gV->m_str;
		CText * gF = pP->GetAt(RET.BEST_T->m_polyline_start).Check( index_foot_attr );
		if( gF )
			*F = gF->m_str;

		if( m_doc->Pages.IsThePageIncludedInNetlist( RET.BEST_IP, 0 ) == 0 )
		{
			int np =-1;
			RET.CP_REF = m_doc->Pages.CheckBlockPart( &RET.BEST_T->m_str, RET.BEST_IP, V, F, &np );
			if( RET.CP_REF.GetLength() )
				RET.n_parts = np;
		}

		if( gV == NULL || gF == NULL )
			FindNextPart( pref, num, suff, V, F );
	}
	return &RET;
}		*/
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int TagTable::AddObject( int page, int CNT, CString * R, CString * V, CString * F, int pgindex )
{
	static int static_count = 0;
	static CString prevR = "Z";
	int nPushes = 0;
	CString sort = CSORT;
	if( sort.Find(R->Left(1)) < sort.Find(prevR.Left(1)) )
		static_count = 0;
	prevR = *R;

	CText * T = m_doc->GetFreeNet(R, Xs[RefDes], curY, textH, textW );
	TextData[page].Add( T );
	T->m_pdf_options = pdfOptions;
	T->m_font_number = fontNum;
	T->m_layer = tLayers[RefDes];
	T->MakeVisible();
	CString cName, URL;

	if( CNT == -1 )
		ASSERT(0);
	BomInTable BOM( m_doc );
	cName = BOM.GetCName( V, F );
	if( Xs[CName] && cName.GetLength() )
	{
		T = m_doc->GetFreeNet(&cName, Xs[CName], curY, textH, textW );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[CName];
		T->MakeVisible();
		int np = -1;
		while( CText* newT = ParseString( T, COL3W ) )
			np = AddPString( newT, page, np, CName );
		if( np == -1 )
			TextData[page].Add( T );
		else
			T->InVisible();
		nPushes = max( nPushes, np );
	}
	if( Xs[Value] && V->GetLength() )
	{
		T = m_doc->GetFreeNet(V, Xs[Value], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Value];
		T->MakeVisible();
	}
	if( Xs[Footprint] && F->GetLength() )
	{
		T = m_doc->GetFreeNet(F, Xs[Footprint], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Footprint];
		T->MakeVisible();
	}
	int it = -1, ip = -1, step = 0;
	ExtractComponentName( V, F );
	URL = BOM.GetURL( V, F, &it, &ip );
	while( Xs[CURL] && URL.GetLength() )
	{
		CString Link = "^|LINK: " + URL;
		T = m_doc->GetFreeNet(&Link, Xs[CURL]+(step*textH), curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[CURL];
		T->MakeVisible();
		URL = BOM.GetURL( V, F, &it, &ip );
		step++;
	}
	if( Xs[Quantity] )
	{
		CString strCnt;
		strCnt.Format("%d",CNT);
		T = m_doc->GetFreeNet(&strCnt, Xs[Quantity], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[Quantity];
		T->MakeVisible();
	}
	if( Xs[LNumber] )
	{
		CString strCnt;
		strCnt.Format("%d", ++static_count);
		T = m_doc->GetFreeNet(&strCnt, Xs[LNumber], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[LNumber];
		T->MakeVisible();
	}
	if( Xs[PGI] && pgindex >= 0 )
	{
		CString strCnt;
		strCnt.Format("%d", pgindex);
		T = m_doc->GetFreeNet(&strCnt, Xs[PGI], curY, textH, textW );
		TextData[page].Add( T );
		T->m_pdf_options = pdfOptions;
		T->m_font_number = fontNum;
		T->m_layer = tLayers[PGI];
		T->MakeVisible();
	}
	for( int i=0; i<nPushes; i++ )
	{
		curY -= shiftY;
		if( curY < pgBottom[page] )
		{
			page++;
			if( page == MAX_PAGES )
			{
				curY += shiftY;// return back
				break;
			}
			curY = pgTop[page];
		}
	}
	return nPushes;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
void TagTable::RemoveEmptyRows()
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
					/*if (bHDelete == 2 && page > 0)
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
					}*/
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
int TagTable::PushBack( int Y, int ip )
{
	shiftY = -shiftY;
	nPages = PushRows( Y, ip );
	shiftY = -shiftY;
	return nPages;
}
//-------------------------------------------------------------------------
//=========================================================================
//-------------------------------------------------------------------------
int TagTable::RedrawNumbers()
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
CText * TagTable::ParseString( CText * T, int W )
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
int TagTable::AddPString( CText * T, int n_page, int n_pushes, int col )
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
				return n_pushes;
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