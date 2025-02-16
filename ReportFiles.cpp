#include "stdafx.h"
#include "FreeSch.h"
#include "ReportFiles.h"
#include "BomInTable.h"

// dialog

IMPLEMENT_DYNAMIC(CDlgRFiles, CDialog)
CDlgRFiles::CDlgRFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRFiles::IDD, pParent)
{

}  

CDlgRFiles::~CDlgRFiles()
{
}

void CDlgRFiles::Ini( CFreePcbDoc * doc )
{
	m_doc = doc;

}

void CDlgRFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_W, m_pic_widths);
	DDX_Control(pDX, IDC_INCL, m_include_reflist);
	DDX_Control(pDX, IDC_EXCL, m_exclude_reflist);
	DDX_Control(pDX, IDC_REF_LISTS, m_ref_list);
	DDX_Control(pDX, IDC_BOM_ENTIRE_FILE, m_entire);
	if( pDX->m_bSaveAndValidate )
	{
		SaveBomOptions();

		float iW = m_pic_widths.GetCurSel() + 5;
		TextW = 1.3;
		BOOL drawTXT = 1;

		// formatting constants
		// set page size
		PageWidth = 210.0/25.4;	
		PageHeight = 297.0/25.4;
		m_pdf_margin = 0.05;
		StringHeight = (PageHeight - (2.0*m_pdf_margin))/30.0;	
		wLink = 20.0/25.4;
		wCount = 20.0/25.4;
		wPicture = iW/5.0;
		wValue = PageWidth - (2*m_pdf_margin) - wLink - wCount - wPicture;
		stPicture = m_pdf_margin;
		stValue = stPicture + wPicture;
		stCount = stValue + wValue;
		stLink = stCount + wCount;
		//
		pagenum = 0;
		CString title;
		title.Format("BOM(%s)", (m_entire.GetCheck()?m_doc->m_name:m_doc->Pages.GetCurrentPCBName()) );
		CPDFdoc * pdf;
		pdf = cpdf_open(0, NULL);
		cpdf_init(pdf);
		///CString utf8 = AnsiToUtf8( &title );
		cpdf_setTitle(pdf,title.GetBuffer());
		TextHeightPts = 15.0;
		LineW = 0.5;
		
		//
		const double clr_pdf = 1.0/255.0;

		BOOL Incl = m_include_reflist.GetCheck();
		BOOL Excl = m_exclude_reflist.GetCheck();
		int index = m_ref_list.GetCurSel();
		index += REF_LIST_INDEX;
		if( index < REF_LIST_INDEX ) 
			index = 0;
		CString V, D, P;

		//modify m_netlist_page_mask
		int cur_pcb = m_doc->Pages.GetCurrentPcbIndex();
		int save_mask[MAX_PAGES];
		save_mask[cur_pcb] = m_doc->Pages.m_netlist_page_mask[cur_pcb];
		for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
		{
			if( ipcb == cur_pcb )
				continue;
			save_mask[ipcb] = m_doc->Pages.m_netlist_page_mask[ipcb];
			if( m_entire.GetCheck() )
			{
				m_doc->Pages.m_netlist_page_mask[cur_pcb] |= m_doc->Pages.m_netlist_page_mask[ipcb];
				m_doc->Pages.m_netlist_page_mask[ipcb] = 0;
			}
		}

		int cnt = m_doc->Pages.FindBOMNextLine(&V, &P, &D, FALSE, TRUE, (Incl?index:0), (Excl?index:0), TRUE );
		if( cnt >= 0 )
		{
			//
			CArray<CString> ** ARRAY = new CArray<CString>*[3];//new020
			CArray<CString> VALUES ;
			CArray<CString> PACKAGES;
			CArray<CString> DETAILS ;
			ARRAY[0] = &VALUES;
			ARRAY[1] = &PACKAGES;
			ARRAY[2] = &DETAILS;
			CArray<int> CNT;
			VALUES.RemoveAll();
			PACKAGES.RemoveAll();
			DETAILS.RemoveAll();
			CNT.RemoveAll();
			do
			{
				if( cnt )
				{
					CString cV=V;
					int sep = cV.Find(CP_LINK);
					if( sep > 0 )
						cV = cV.Left( sep );
					sep = cV.Find("|");
					if( sep > 0 )
						cV.Delete( sep );
					VALUES.Add(cV);
					PACKAGES.Add(P);
					DETAILS.Add(D);
					CNT.Add(cnt);
				}
				D = "";
				cnt = m_doc->Pages.FindBOMNextLine(&V, &P, &D, FALSE, TRUE, (Incl?index:0), (Excl?index:0));
			}while( cnt >= 0 );
			double Cur_Y = 0;
			double y = Cur_Y;
			double x = stPicture + (wPicture/2.0);
			double Pic_Y = INT_MAX;
			CString PrevPac = "";
			while( PACKAGES.GetSize() )
			{
				Cur_Y -= StringHeight;
				if( Cur_Y+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
				{
					Cur_Y = PageInit(pdf);
					Pic_Y = INT_MAX;
				}
				CString GetT = "";
				int item = GetNextRow( ARRAY, &GetT );
				if( item == -1 )
				{
					break;
				}
				BOOL bTITLE = 0;
				if( GetT.GetLength() )
				{
					Cur_Y = min( Cur_Y, Pic_Y-(StringHeight/2.0) );
					bTITLE = 1;
Title:				title = GetT;
					if( PrevPac.GetLength() )
					{
						x = 0;
						y = Cur_Y - (StringHeight/2.0);
						cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)255.0, 
													clr_pdf*(float)255.0, 
													clr_pdf*(float)255.0 );
						cpdf_moveto( pdf, x, max(y,m_pdf_margin+StringHeight) );
						cpdf_lineto( pdf, x, y+StringHeight );
						cpdf_lineto( pdf, PageWidth, y+StringHeight );
						cpdf_lineto( pdf, PageWidth, max(y,m_pdf_margin+StringHeight) );
						cpdf_lineto( pdf, x, max(y,m_pdf_margin+StringHeight) );
						cpdf_fill( pdf );
						x = m_pdf_margin;
						if( Cur_Y+0.01 < PageHeight - m_pdf_margin - (StringHeight/2.0) )
						{
							cpdf_moveto( pdf, x, y+StringHeight );
							cpdf_lineto( pdf, PageWidth-m_pdf_margin, y+StringHeight );
							cpdf_stroke( pdf );
						}
						//
						Cur_Y -= StringHeight;
						if( Cur_Y+0.01 < (m_pdf_margin+(2.0*StringHeight)+(StringHeight/2.0)) )
						{
							if( Cur_Y > m_pdf_margin )
							{
								cpdf_moveto( pdf, 0, Cur_Y+StringHeight );
								cpdf_lineto( pdf, 0, m_pdf_margin+StringHeight );
								cpdf_lineto( pdf, PageWidth, m_pdf_margin+StringHeight );
								cpdf_lineto( pdf, PageWidth, Cur_Y+StringHeight );
								cpdf_lineto( pdf, 0, Cur_Y+StringHeight );
								cpdf_fill( pdf );
							}
							cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin+StringHeight );
							cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin+StringHeight );
							cpdf_stroke( pdf );
							
							Cur_Y = PageInit(pdf);
							Pic_Y = INT_MAX;
						}
					}
					x = m_pdf_margin;
					y = max( m_pdf_margin+StringHeight, Cur_Y - (StringHeight/2.0) );
					cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_doc->m_bom_rgb[eFill][0], 
												clr_pdf*(float)m_doc->m_bom_rgb[eFill][1], 
												clr_pdf*(float)m_doc->m_bom_rgb[eFill][2] );
					cpdf_moveto( pdf, x, y );
					cpdf_lineto( pdf, x, Cur_Y+(StringHeight/2.0) );
					cpdf_lineto( pdf, PageWidth-m_pdf_margin, Cur_Y+(StringHeight/2.0) );
					cpdf_lineto( pdf, PageWidth-m_pdf_margin, y );
					cpdf_lineto( pdf, x, y );
					cpdf_fill( pdf );
					cpdf_moveto( pdf, x, y );
					cpdf_lineto( pdf, x, Cur_Y+(StringHeight/2.0) );
					cpdf_lineto( pdf, PageWidth-m_pdf_margin, Cur_Y+(StringHeight/2.0) );
					cpdf_lineto( pdf, PageWidth-m_pdf_margin, y );
					cpdf_lineto( pdf, x, y );
					//
					cpdf_moveto( pdf, stCount, y );
					cpdf_lineto( pdf, stCount, Cur_Y+(StringHeight/2.0) );
					//
					cpdf_moveto( pdf, stLink, y );
					cpdf_lineto( pdf, stLink, Cur_Y+(StringHeight/2.0) );
					//
					cpdf_stroke( pdf );
					//
					x = 0;
					y = Cur_Y;
					DrawTextBox(pdf, x, y-(StringHeight/3.0), wPicture+wValue, 1, TextW, 
						(float)m_doc->m_bom_rgb[eHeader][0], 
						(float)m_doc->m_bom_rgb[eHeader][1], 
						(float)m_doc->m_bom_rgb[eHeader][2], &title, TRUE );
					//
					title = "pcs";
					x = stCount;
					DrawTextBox(pdf, x, y-(StringHeight/3.0), wCount, 1, TextW, 
						(float)m_doc->m_bom_rgb[eHeader][0], 
						(float)m_doc->m_bom_rgb[eHeader][1], 
						(float)m_doc->m_bom_rgb[eHeader][2], &title, TRUE );
					//
					title = "URL";
					x = stLink;
					DrawTextBox(pdf, x, y-(StringHeight/3.0), wLink, 1, TextW, 
						(float)m_doc->m_bom_rgb[eHeader][0], 
						(float)m_doc->m_bom_rgb[eHeader][1], 
						(float)m_doc->m_bom_rgb[eHeader][2], &title, TRUE );
					//
					Cur_Y -= StringHeight;
					if( Cur_Y+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
					{
						Cur_Y = PageInit(pdf);
					}
				}
				//
				y = Cur_Y;
				//
				CString Vtext = VALUES.GetAt(item);
				CString Ptext = PACKAGES.GetAt(item);

				// checking space
				float n_str1 = DrawTextBox(pdf, x, y-(StringHeight/3.0), wValue, 1, TextW, 0, 0, 0, &Vtext );
				float n_str2 = DrawTextBox(pdf, x, y-(StringHeight/3.0), wPicture, 1, TextW, 0, 0, 0, &Ptext );
				float n_str = max( n_str1, n_str2 );
				if( Cur_Y-((n_str)*StringHeight)+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
				{
					Cur_Y = PageInit(pdf);
					Pic_Y = INT_MAX;
				}
				ExtractComponentName( &Vtext, &Ptext );

				// draw new package
				BOOL isPART = PACKAGES.GetAt(item).Compare( NO_FOOTPRINT );
				if( PACKAGES.GetAt(item).Compare( PrevPac ) || isPART == 0 || bTITLE )
				{
					if( Pic_Y > Cur_Y+(StringHeight/2.0) )
					{
						if( isPART )
						{
							cpdf_moveto( pdf, stValue, Cur_Y+(StringHeight/2.0) );
							cpdf_lineto( pdf, m_pdf_margin, Cur_Y+(StringHeight/2.0) );
						}
						else
						{
							cpdf_moveto( pdf, PageWidth-m_pdf_margin, Cur_Y+(StringHeight/2.0) );
							cpdf_lineto( pdf, m_pdf_margin, Cur_Y+(StringHeight/2.0) );
						}
					}
					else
					{
						cpdf_moveto( pdf, PageWidth-m_pdf_margin, Pic_Y );
						cpdf_lineto( pdf, m_pdf_margin, Pic_Y );
						Cur_Y = Pic_Y - (StringHeight/2.0);
					}
					cpdf_stroke( pdf );
					
					// checking space
					if( Cur_Y-((n_str)*StringHeight)+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
					{
						Cur_Y = PageInit(pdf);
						Pic_Y = INT_MAX;
					}

Package:			// (footprint name)
					x = stPicture;
					y = Cur_Y;
					title = PACKAGES.GetAt(item);
					if( isPART )
						DrawTextBox(pdf, x, y-(StringHeight/3.0), wPicture, 1, TextW, 
									(float)m_doc->m_bom_rgb[eText][0], 
									(float)m_doc->m_bom_rgb[eText][1], 
									(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );

					// reset picY
					Pic_Y = INT_MAX;

					// now try to insert image
					title = m_doc->m_app_dir + main_component + "\\" + Ptext + ".jpg";
					CFile file;
					BOOL ok_pic = 1;
					if( file.Open( title, CFile::modeRead, NULL ) == 0 )
					{
						title = m_doc->m_app_dir + main_component + "\\" + Ptext + ".jpeg";
						if( file.Open( title, CFile::modeRead, NULL ) == 0 )
						{
							title = m_doc->Pages.GetPicture( &Vtext, &Ptext );
							if( title.GetLength() == 0 )
								ok_pic = 0;
							else if( file.Open( title, CFile::modeRead, NULL ) == 0 )
								ok_pic = 0;
						}
					}	
					if( ok_pic )
					{
						CArchive ar( &file, CArchive::load | CArchive::bNoFlushOnDelete );
						CArchiveStream ars(&ar);
						LPPICTURE lp;
						OleLoadPicture( (LPSTREAM)&ars, 0, FALSE, IID_IPicture, (void**)&lp );
						ar.Close();
						file.Close();
						if( lp )
						{
							CSize sz;
							lp->get_Width( &sz.cx );
							lp->get_Height( &sz.cy );
							double k = (double)sz.cx/(double)sz.cy;
							double left = m_pdf_margin + (LineW/25.4);
							double right = stValue - (LineW/25.4);
							float Width = right - left;
							double top = Cur_Y - (n_str2*StringHeight) + (StringHeight/2.0);
							if( isPART == 0 )
								top += StringHeight;
							double bottom = top - (Width/k);
							float Height = (top - bottom)*70.0;
							Width *= 70.0;
							if( bottom < (m_pdf_margin+StringHeight+(StringHeight/10.0)) )
							{
								y = Cur_Y;
								if( bTITLE )
									y += (StringHeight*2.0);
								else
									y += (StringHeight/2.0) - (LineW/100.0);
								cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)255.0, 
															clr_pdf*(float)255.0, 
															clr_pdf*(float)255.0 );
								cpdf_moveto( pdf, 0, y );
								cpdf_lineto( pdf, 0, m_pdf_margin+StringHeight );
								cpdf_lineto( pdf, PageWidth, m_pdf_margin+StringHeight );
								cpdf_lineto( pdf, PageWidth, y );
								cpdf_lineto( pdf, 0, y );
								cpdf_fill( pdf );
								cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin+StringHeight );
								cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin+StringHeight );
								cpdf_stroke( pdf );
								cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_doc->m_bom_rgb[eFill][0], 
															clr_pdf*(float)m_doc->m_bom_rgb[eFill][1], 
															clr_pdf*(float)m_doc->m_bom_rgb[eFill][2] );
								//
								Cur_Y = PageInit(pdf);
								Pic_Y = INT_MAX;

									
								if( bTITLE )
								{
									bTITLE = 0;
									PrevPac = "";
									goto Title;
								}
								else
								{
									goto Package;
								}
							}
							float scx = 1.0;
							float scy = 1.0;
							cpdf_importImage( pdf, title, JPEG_IMG, left, bottom, 0.0, &Width, &Height, &scx, &scy, IM_GSAVE );
							lp->Release();
							Pic_Y = bottom;
						}
					}
					else if( isPART == 0 )
					{	
						title = PACKAGES.GetAt(item);
						DrawTextBox(pdf, x, y-(StringHeight/3.0), wPicture, 1, TextW, 
									(float)m_doc->m_bom_rgb[eText][0], 
									(float)m_doc->m_bom_rgb[eText][1], 
									(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );
					}
				}
				// draw value
				title = VALUES.GetAt(item);

				x = stValue;
				y = Cur_Y;
				DrawTextBox(pdf, x, y-(StringHeight/3.0), wValue, 1, TextW, 
						(float)m_doc->m_bom_rgb[eText][0], 
						(float)m_doc->m_bom_rgb[eText][1], 
						(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );
				
				// draw count
				title.Format("%d", CNT.GetAt(item));
				x = stCount;
				DrawTextBox(pdf, x, y-(StringHeight/3.0), wCount, 1, TextW, 
						(float)m_doc->m_bom_rgb[eText][0], 
						(float)m_doc->m_bom_rgb[eText][1], 
						(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );
				//
				x = stLink;
				int cntLnk = 0;
				int it = -1, ip = -1;
				CString v = VALUES.GetAt(item);
				CString f = PACKAGES.GetAt(item);
				ExtractComponentName( &v, &f );

				BomInTable BOM( m_doc );
				title = "link";
				int curr_page = pagenum;
				double link_Y = Cur_Y;
				while( title.Compare( "link" ) == 0 )
				{
					CString url = BOM.GetURL( &v, &f, &it, &ip );
					if(url.GetLength()==0)
					{
						if( isPART == 0 )
							url = m_doc->m_find_component + VALUES.GetAt(item);
						else
							url = m_doc->m_find_component + VALUES.GetAt(item) + " " + PACKAGES.GetAt(item);
						ReplaceCharsForURL( &url );
						title = "seek";
					}
					else 
					{
						title = "link";
					}
					if( cntLnk )
					{
						link_Y -= StringHeight;
						if( link_Y+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
						{
							link_Y = PageInit(pdf);
							Pic_Y = INT_MAX;
						}
					}
					cntLnk++;
					y = link_Y;
					{
						///CString utf8 = AnsiToUtf8( &url );
						cpdf_setActionURL(pdf, x-0.15+(wLink/2.0), y-0.04, x+0.15+(wLink/2.0), y+0.06, url, NULL );
					}
					DrawTextBox(pdf, x, y-(StringHeight/3.0), wLink, 1, TextW, 
						(float)m_doc->m_bom_rgb[eURL][0], 
						(float)m_doc->m_bom_rgb[eURL][1], 
						(float)m_doc->m_bom_rgb[eURL][2], &title, TRUE );
				}
				if( curr_page != pagenum )
					Cur_Y = link_Y;

				float set1 = max( n_str, cntLnk ) - 1.0;
				if( drawTXT )
				{
					title = m_doc->m_app_dir + main_component + "\\" + Vtext + "@" + Ptext + ".txt";
					CStdioFile File;
					if( File.Open( title, CFile::modeRead, NULL ) )
					{
						File.ReadString( title );
						File.Close();
						if( title.GetLength() )
						{
							// DRAW TEXT
							x = stValue + (StringHeight/2.0);
							CPDFtboxAttr attr;
							attr.alignmode = TBOX_CENTER;
							attr.NLmode = 0;
							attr.noMark = 0;
							attr.paragraphSpacing = 0;
							title += " ";	
							float numStr = DrawTextBox(pdf, x, y, wValue-StringHeight, 0, TextW, 0, 0, 0, &title);
							//
							//---------------------------------------------------
							
							float set2 = n_str1 + numStr - 1.0;
							for( float step = max( set1, set2 ); step>0.5; step-- )
								Cur_Y -= StringHeight;
							if( Cur_Y+0.01 < (m_pdf_margin+StringHeight+(StringHeight/2.0)) )
							{
								Cur_Y = PageInit(pdf);
								Pic_Y = INT_MAX;
								for( float step = numStr; step>1.5; step-- )
									Cur_Y -= StringHeight;
							}
							set1 = 0.0;
							y = Cur_Y - (StringHeight/3.0);
							DrawTextBox(pdf, x, y, wValue-StringHeight, numStr, TextW, 
								(float)m_doc->m_bom_rgb[eName][0], 
								(float)m_doc->m_bom_rgb[eName][1], 
								(float)m_doc->m_bom_rgb[eName][2], &title, TRUE );
							//float h = (numStr-1.0)*StringHeight - (StringHeight/3.0);
							//cpdf_textBox(pdf, x, y, wValue-StringHeight, h, 0.0, 27.0, &attr, title.GetBuffer() );
						}
					}
				}	
				for( float step = set1; step>0.5; step-=1 )
					Cur_Y -= StringHeight;
				//
				if( isPART )
				{
					cpdf_moveto( pdf, stValue, Cur_Y-(StringHeight/2.0) );
					cpdf_lineto( pdf, PageWidth-m_pdf_margin, Cur_Y-(StringHeight/2.0) );
					cpdf_stroke( pdf );
				}
				//
				PrevPac = PACKAGES.GetAt(item);
				VALUES.RemoveAt(item);
				PACKAGES.RemoveAt(item);
				DETAILS.RemoveAt(item);
				CNT.RemoveAt(item);
				if( PACKAGES.GetSize() == 0 )
				{
					if( PrevPac.GetLength() )
					{
						Cur_Y = min( Cur_Y, Pic_Y-(StringHeight/2.0) );

						if( PrevPac.GetLength() )
						{
							x = 0;
							y = Cur_Y - (StringHeight/2.0);
							cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)255.0, 
														clr_pdf*(float)255.0, 
														clr_pdf*(float)255.0 );
							cpdf_moveto( pdf, x, m_pdf_margin+StringHeight );
							cpdf_lineto( pdf, x, y );
							cpdf_lineto( pdf, PageWidth, y );
							cpdf_lineto( pdf, PageWidth, m_pdf_margin+StringHeight );
							cpdf_lineto( pdf, x, m_pdf_margin+StringHeight );
							cpdf_fill( pdf );
							x = m_pdf_margin;
							//if( Cur_Y+0.01 < PageHeight - m_pdf_margin - (StringHeight/2.0) )
							{
								cpdf_moveto( pdf, x, y );
								cpdf_lineto( pdf, PageWidth-m_pdf_margin, y );
								cpdf_stroke( pdf );
							}
							cpdf_moveto( pdf, x, m_pdf_margin+StringHeight );
							cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin+StringHeight );
							cpdf_stroke( pdf );

							cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_doc->m_bom_rgb[eFill][0], 
														clr_pdf*(float)m_doc->m_bom_rgb[eFill][1], 
														clr_pdf*(float)m_doc->m_bom_rgb[eFill][2] );
						}
					}
				}
			}
			delete[] ARRAY;//new020
		}

		// restore m_netlist_page_mask
		if( m_entire.GetCheck() )
			for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
				m_doc->Pages.m_netlist_page_mask[ipcb] = save_mask[ipcb];

		cpdf_finalizeAll(pdf);

		CString s0 = m_doc->m_path_to_folder+"\\related_files\\reports";
		struct _stat buf;
		if( _stat( s0, &buf ) )
		{
			_mkdir( s0 );
			_mkdir( s0+"\\related_files" );
		}
		if( m_entire.GetCheck() )
			title = m_doc->m_name;
		else
			title = m_doc->Pages.GetCurrentPCBName();
		if( title.GetLength() > 4 )
			if( title.Right(4) == ".fpc" || title.Right(4) == ".cds" )
				title.Truncate( title.GetLength() - 4 );

		CString pdf_file = s0 + "\\BOM(" + title + ").pdf";
		int err = cpdf_savePDFmemoryStreamToFile(pdf, pdf_file );
		if( err == -1 )
		{
			if( !m_doc->m_dlg_log )
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Error: Unable to write file\nIt may be read-only or open in another application":
					"Ошибка: Невозможно записать файл\nВозможно, он доступен только для чтения или открыт в другом приложении", MB_OK);
			}
			else
			{
				CString log_message = "*** Error: unable to write file ***\r\n";
				m_doc->m_dlg_log->AddLine( log_message );
			}
		}
		cpdf_close(pdf);
		ShellExecute(	NULL,"open", pdf_file, NULL, m_doc->m_path_to_folder+"\\related_files", SW_SHOWNORMAL );
		// end
	}
	else
	{
		if( m_doc->Pages.IsThePageIncludedInNetlist( m_doc->Pages.GetActiveNumber() ) == 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"The current page is not included in any netlist.\n\nUse the file menu Netlist Settings":
				"Текущая страница не включена ни в один список электрических цепей.\n\nИспользуйте «Настройки списка цепей» из меню «Файл»", MB_ICONERROR);
			pDX->Fail();
		}
		for( int i=0; i<m_doc->m_ref_lists->GetSize(); i++ )
			m_ref_list.AddString( m_doc->m_ref_lists->GetStr(i) );
		for( int i=0; i<=10; i++ )
		{
			CString s;
			s.Format((i<5?"x0%d":"x%d"), i+5);
			m_pic_widths.AddString( s );
		}
		m_pic_widths.SetCurSel(5);
		int bits = 1;
		m_include_reflist.SetCheck( ((bits<<6) & m_doc->m_bom_options)>>6 );
		if( m_include_reflist.GetCheck() == 0 )
			m_exclude_reflist.SetCheck( ((bits<<7) & m_doc->m_bom_options)>>7 );
		m_entire.SetCheck( ((bits<<30) & m_doc->m_bom_options)>>30 );
		
		bits = (1<<5) - 1;
		int cur_sel = (bits & m_doc->m_bom_options);
		if( cur_sel >= 0 && cur_sel < m_ref_list.GetCount() )
			m_ref_list.SetCurSel(cur_sel);
		else
		{
			m_include_reflist.SetCheck( 0 );
			m_exclude_reflist.SetCheck( 0 );
		}
		cur_sel = ( ((bits<<24) & m_doc->m_bom_options)>>24 );
		if( cur_sel >= 0 && cur_sel < m_pic_widths.GetCount() )
			m_pic_widths.SetCurSel(cur_sel);
	}
}


BEGIN_MESSAGE_MAP(CDlgRFiles, CDialog)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_REF_LISTS, SaveBomOptions)
	ON_BN_CLICKED(IDC_INCL, &CDlgRFiles::OnBnClickedIncl)
	ON_BN_CLICKED(IDC_EXCL, &CDlgRFiles::OnBnClickedExcl)
	ON_BN_CLICKED(IDC_BOM_ENTIRE_FILE, &CDlgRFiles::OnBnClickedBomEntireFile)
	ON_BN_CLICKED(IDC_BUTTON_LAYER1, &CDlgRFiles::OnBnClickedButtonLayer1)
	ON_BN_CLICKED(IDC_BUTTON_LAYER2, &CDlgRFiles::OnBnClickedButtonLayer2)
	ON_BN_CLICKED(IDC_BUTTON_LAYER3, &CDlgRFiles::OnBnClickedButtonLayer3)
	ON_BN_CLICKED(IDC_BUTTON_LAYER4, &CDlgRFiles::OnBnClickedButtonLayer4)
	ON_BN_CLICKED(IDC_BUTTON_LAYER5, &CDlgRFiles::OnBnClickedButtonLayer5)
	ON_BN_CLICKED(IDC_BUTTON_LAYER6, &CDlgRFiles::OnBnClickedButtonLayer6)
	ON_BN_CLICKED(IDC_BUTTON_LAYER7, &CDlgRFiles::OnBnClickedButtonLayer7)
END_MESSAGE_MAP()

int CDlgRFiles::GetNextRow( CArray<CString> ** ARRAY, CString * title  )
{
	static CString PrevPac = OLD_NO_FP;
	static CString PrevVal = OLD_NO_FP;
	static CString PrevPref = OLD_NO_FP;
	CArray<CString> * VALUES = ARRAY[0];
	CArray<CString> * PACKAGES = ARRAY[1];
	CArray<CString> * DETAILS = ARRAY[2];

	int BEST_I = -1;
	CString BEST_PREF = "ZZ";
	CString sort = SHORTCSORT;
	for(int i=0; i<DETAILS->GetSize(); i++)
	{
		CString s = DETAILS->GetAt(i);
		CString cur_pac = PACKAGES->GetAt(i);
		CString cur_val = VALUES->GetAt(i);

		ASSERT( cur_val.GetLength() && cur_pac.GetLength() );
		int iof = s.FindOneOf(" ,'");
		if( iof > 0 )
			s.Truncate(iof);
		CString pref, suff;
		ParseRef( &s, &pref, &suff );
		if( pref.GetLength() == 0 )
			continue;
		int f1 = sort.Find( pref.Left(1) );
		int f2 = sort.Find( BEST_PREF.Left(1) );
		if( f1 < f2 )
		{
			BEST_I = i;
			BEST_PREF = pref;
		}
		else if( f1 == f2 )
		{
			BOOL CompPac = 0;
			if( pref.GetLength() == 1 )
			{
				if( BEST_PREF.GetLength() > 1 )
				{
					BEST_I = i;
					BEST_PREF = pref;
				}
				else
					CompPac = 1;
			}
			else if( BEST_PREF.GetLength() > 1 )
			{
				f1 = sort.Find( pref.Left(2).Right(1) );
				f2 = sort.Find( BEST_PREF.Left(2).Right(1) );
				if( f1 < f2 )
				{
					BEST_I = i;
					BEST_PREF = pref;
				}
				else if( f1 == f2 )
					CompPac = 1;
			}
			if( CompPac && BEST_I >= 0 )
			{
				BOOL CompVal = 0;
				if( cur_pac.Compare( PrevPac ) == 0 )
				{
					if( PACKAGES->GetAt(BEST_I).Compare( PrevPac ) )
					{
						BEST_I = i;
						BEST_PREF = pref;
					}
					else
					{
						CompVal = 1;
					}
				}
				else if( PACKAGES->GetAt(BEST_I).Compare( PrevPac ) )
				{
					CompVal = 1;
				}
				if( CompVal )
				{
					BOOL CompL = 0;
					if( cur_val.Right(1) == PrevVal.Right(1) )
					{
						if( VALUES->GetAt(BEST_I).Right(1) != PrevVal.Right(1) )
						{
							BEST_I = i;
							BEST_PREF = pref;
						}
						else
							CompL = 1;
					}
					else if( VALUES->GetAt(BEST_I).Right(1) != PrevVal.Right(1) )
						CompL = 1;
					if( CompL ) 
					{
						f1 = sort.Find( cur_val.Left(1) );
						f2 = sort.Find( VALUES->GetAt(BEST_I).Left(1) );
						if( f1 < f2 )
						{
							BEST_I = i;
							BEST_PREF = pref;
						}
						else if( f1 == f2 )
						{
							if( cur_val.GetLength() < VALUES->GetAt(BEST_I).GetLength() )
							{
								BEST_I = i;
								BEST_PREF = pref;
							}
						}
					}
				}
			}
		}
	}
	if( PrevPref.Compare( BEST_PREF ) )
	{
		CStdioFile rPrefix;
		int rf = rPrefix.Open( m_doc->m_app_dir+"\\prefixes.txt", CFile::modeRead );
		if( rf )
		{
			while( rPrefix.ReadString( *title ) )
				if( title->Find( "["+BEST_PREF+"]" ) >= 0 )
				{
					CString title2;
					rPrefix.ReadString( title2 );
					*title += (" - " + title2);
					break;
				}
			rPrefix.Close();
		}
	}
	if( BEST_I >= 0 )
	{
		PrevPac = PACKAGES->GetAt(BEST_I);
		PrevVal = VALUES->GetAt(BEST_I);
		PrevPref = BEST_PREF;
	}
	return BEST_I;
}



float CDlgRFiles::PageInit( CPDFdoc * pdf )
{
	const double clr_pdf = 1.0/255.0;
	int mode = PORTRAIT;
	pagenum++;
	cpdf_pageInit(pdf, pagenum, mode, A4, A4);		/* page orientation */
	cpdf_setlinecap( pdf, 1 );	// round end-caps
	cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_doc->m_bom_rgb[eSource][0], 
								clr_pdf*(float)m_doc->m_bom_rgb[eSource][1], 
								clr_pdf*(float)m_doc->m_bom_rgb[eSource][2] );
	cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_doc->m_bom_rgb[eStroke][0], 
								clr_pdf*(float)m_doc->m_bom_rgb[eStroke][1], 
								clr_pdf*(float)m_doc->m_bom_rgb[eStroke][2] );
	cpdf_setlinewidth( pdf, LineW );
	cpdf_setTextRenderingMode(pdf, TEXT_FILL);
	cpdf_setFont(pdf, cpdf_fontnamelist[24], "WinAnsiEncoding", TextHeightPts*2.0/3.0);
	//
	cpdf_newpath( pdf );
	//
	cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin );
	cpdf_lineto( pdf, m_pdf_margin, StringHeight+m_pdf_margin );
	cpdf_lineto( pdf, PageWidth-m_pdf_margin, StringHeight+m_pdf_margin );
	cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin );
	cpdf_lineto( pdf, m_pdf_margin, m_pdf_margin );
	cpdf_fill( pdf );
	//
	int color_summ = m_doc->m_bom_rgb[eSource][0] + m_doc->m_bom_rgb[eSource][1] + m_doc->m_bom_rgb[eSource][2];
	if (color_summ < 127 * 3)
		cpdf_setrgbcolorFill(pdf, clr_pdf * 255.0,
			clr_pdf * 255.0,
			clr_pdf * 255.0);
	else
		cpdf_setrgbcolorFill(pdf, clr_pdf * 0.0,
			clr_pdf * 0.0,
			clr_pdf * 0.0);
	cpdf_text(pdf, 0.1, 0.08, 0, "niconson.com");
	cpdf_setActionURL(pdf, 0.1, 0.08, 1.0, 0.2, "https://niconson.com/freepcb2", NULL);
	cpdf_setrgbcolorFill( pdf,	clr_pdf*(float)m_doc->m_bom_rgb[eFill][0], 
								clr_pdf*(float)m_doc->m_bom_rgb[eFill][1], 
								clr_pdf*(float)m_doc->m_bom_rgb[eFill][2] );
	//
	cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin );
	cpdf_lineto( pdf, m_pdf_margin, PageHeight-m_pdf_margin );
	cpdf_lineto( pdf, PageWidth-m_pdf_margin, PageHeight-m_pdf_margin );
	cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin );
	cpdf_lineto( pdf, m_pdf_margin, m_pdf_margin );
	//
	cpdf_moveto( pdf, m_pdf_margin, m_pdf_margin+StringHeight );
	cpdf_lineto( pdf, PageWidth-m_pdf_margin, m_pdf_margin+StringHeight );
	//
	cpdf_moveto( pdf, stLink, PageHeight-m_pdf_margin );
	cpdf_lineto( pdf, stLink, m_pdf_margin );
	//
	cpdf_moveto( pdf, stCount, PageHeight-m_pdf_margin );
	cpdf_lineto( pdf, stCount, m_pdf_margin+StringHeight );
	//
	cpdf_moveto( pdf, stValue, PageHeight-m_pdf_margin );
	cpdf_lineto( pdf, stValue, m_pdf_margin+StringHeight );
	//
	cpdf_stroke( pdf );	
	//
	CString title;
	title.Format("Page%d", pagenum);
	float x = stLink;
	float y = m_pdf_margin + (StringHeight/2.0);
	DrawTextBox(pdf, x, y-(StringHeight/3.0), wLink, 1, TextW, 
								(float)m_doc->m_bom_rgb[eText][0], 
								(float)m_doc->m_bom_rgb[eText][1], 
								(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );
	//
	title = m_doc->m_pcb_full_path.Right( m_doc->m_pcb_full_path.GetLength() - m_doc->m_parent_folder.GetLength() );
	x = stPicture;
	DrawTextBox(pdf, x, y-(StringHeight/3.0), (wPicture+wValue+wCount), 1, TextW, 
								(float)m_doc->m_bom_rgb[eText][0], 
								(float)m_doc->m_bom_rgb[eText][1], 
								(float)m_doc->m_bom_rgb[eText][2], &title, TRUE );
	//
	//
	cpdf_closepath( pdf );
	return (PageHeight - m_pdf_margin - (StringHeight/2.0));
}


void CDlgRFiles::SaveBomOptions()
{
	int bits = (1<<8) - 1;
	int m_d_len = ( (bits<<16) & m_doc->m_bom_options );
	int m_ignore = getbit( m_doc->m_bom_options, 5 );
	m_doc->m_bom_options = m_ref_list.GetCurSel();
	m_doc->m_bom_options = max( m_doc->m_bom_options, 0 );
	if( m_ignore )
		setbit( m_doc->m_bom_options, 5 );
	if( m_include_reflist.GetCheck() )
		setbit( m_doc->m_bom_options, 6 );
	if( m_exclude_reflist.GetCheck() )
		setbit( m_doc->m_bom_options, 7 );
	if( m_entire.GetCheck() )
		setbit( m_doc->m_bom_options, 30 );
	m_doc->m_bom_options |= m_d_len;
	int cur = m_pic_widths.GetCurSel();
	m_doc->m_bom_options |= (cur<<24);

	m_doc->SaveOptions();
}



void CDlgRFiles::OnBnClickedIncl()
{
	m_exclude_reflist.SetCheck(0);
	SaveBomOptions();
}


void CDlgRFiles::OnBnClickedExcl()
{
	m_include_reflist.SetCheck(0);
	SaveBomOptions();
}

float CDlgRFiles::DrawTextBox(	CPDFdoc * pdf, 
								float x, 
								float y, 
								float w,  
								float str_cnt,
								float textW,
								float R,
								float G, 
								float B,
								CString * text,
								BOOL bDRAW )
{
	if( text->GetLength() == 0 )
		return 0.0;

	const double clr_pdf = 1.0/255.0;
	float k = textW*4.0;
	CText * n = m_doc->GetFreeNet( text, 0, 0, NM_PER_MM, NM_PER_MIL*k );
	n->m_font_number = m_doc->m_default_font;
	n->MakeVisible();
	cpdf_setrgbcolorStroke( pdf,clr_pdf*R, 
								clr_pdf*G, 
								clr_pdf*B );
	cpdf_setlinewidth( pdf, textW );
	float dy= StringHeight*str_cnt;
	float nstr = 0.0;
	while( CText * nnew = ParseString( n, w ) )
	{
		dy -= 0.38;
		nstr += 1.0;
		if( bDRAW )
		{
			nnew->m_stroke_width = NM_PER_MIL*k;
			nnew->m_font_number = m_doc->m_default_font;
			nnew->MakeVisible();
			RECT r;
			m_doc->m_tlist->GetTextRectOnPCB( nnew, &r );
			float dx = r.right - r.left;
			dx = dx/6.5/(float)NM_PER_MM;
			CArray<CPoint> * pts = nnew->m_dlist->Get_Points( nnew->dl_el, NULL, NULL );
			int np = pts->GetSize();
			CPoint * get = new CPoint[np];//new001
			nnew->m_dlist->Get_Points( nnew->dl_el, get, &np );
			for( int i=0; i<np; i++ )
			{
				float gx = (float)get[i].x/(float)NM_PER_MIL;
				float gy = (float)get[i].y/(float)NM_PER_MIL;
				cpdf_moveto( pdf, x+w/2.0+gx/254.0-dx/2.0, y+0.07+gy/254.0+dy );
				i++;
				gx = (float)get[i].x/(float)NM_PER_MIL;
				gy = (float)get[i].y/(float)NM_PER_MIL;
				cpdf_lineto( pdf, x+w/2.0+gx/254.0-dx/2.0, y+0.07+gy/254.0+dy );
			}
			delete get;//new001
		}
		nnew->InVisible();
	}
	n->InVisible();
	if( bDRAW )
		cpdf_stroke( pdf );
	cpdf_setlinewidth( pdf, LineW );
	cpdf_setrgbcolorStroke(pdf, clr_pdf*(float)m_doc->m_bom_rgb[eStroke][0], 
								clr_pdf*(float)m_doc->m_bom_rgb[eStroke][1], 
								clr_pdf*(float)m_doc->m_bom_rgb[eStroke][2] );
	return nstr;
}



//-------------------------------------------------------------------------
CText * CDlgRFiles::ParseString( CText * T, float W )
{
	static int cur = 0;
	if( T->m_dlist == NULL )
		T->MakeVisible();
	RECT r;
	T->m_tl->GetTextRectOnPCB(T,&r);
	int tw = (r.right-r.left);
	{
		float nrows = tw/W/6000000.0;
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
			return m_doc->GetFreeNet(&str_out, 0, 0, NM_PER_MM, NM_PER_MIL );
		}
	}
	cur = 0;
	return NULL;
}
//

void CDlgRFiles::OnBnClickedBomEntireFile()
{
	
}

// set color boxes
//
HBRUSH CDlgRFiles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  Change any attributes of the DC here
	// TODO:  Return a different brush if the default is not desired
	if ( pWnd->GetDlgCtrlID() >= IDC_STATIC_LAYER1 && pWnd->GetDlgCtrlID() <= IDC_STATIC_LAYER7 )
	{
		int i = pWnd->GetDlgCtrlID() - IDC_STATIC_LAYER1;
		// Set layer color
		pDC->SetTextColor(RGB(m_doc->m_bom_rgb[i][0], m_doc->m_bom_rgb[i][1], m_doc->m_bom_rgb[i][2]));
		pDC->SetBkMode(TRANSPARENT);
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush( RGB(m_doc->m_bom_rgb[i][0], m_doc->m_bom_rgb[i][1], m_doc->m_bom_rgb[i][2]) );
		hbr = m_brush;
	}
	return hbr;
}

void CDlgRFiles::EditColor( int layer )
{
	CColorDialog dlg( RGB(m_doc->m_bom_rgb[layer][0], m_doc->m_bom_rgb[layer][1], m_doc->m_bom_rgb[layer][2]), 
		CC_FULLOPEN | CC_ANYCOLOR );
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		COLORREF color = dlg.GetColor();
		m_doc->m_bom_rgb[layer][0] = GetRValue( color );
		m_doc->m_bom_rgb[layer][1] = GetGValue( color );
		m_doc->m_bom_rgb[layer][2] = GetBValue( color );
		Invalidate( FALSE );
	}
}


void CDlgRFiles::OnBnClickedButtonLayer1()
{
	EditColor( 0 );
}


void CDlgRFiles::OnBnClickedButtonLayer2()
{
	EditColor( 1 );
}


void CDlgRFiles::OnBnClickedButtonLayer3()
{
	EditColor( 2 );
}


void CDlgRFiles::OnBnClickedButtonLayer4()
{
	EditColor( 3 );
}


void CDlgRFiles::OnBnClickedButtonLayer5()
{
	EditColor( 4 );
}


void CDlgRFiles::OnBnClickedButtonLayer6()
{
	EditColor( 5 );
}


void CDlgRFiles::OnBnClickedButtonLayer7()
{
	EditColor( 6 );
}
