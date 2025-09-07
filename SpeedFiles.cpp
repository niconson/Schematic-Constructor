#include "stdafx.h"
#include "SpeedFiles.h"
#define MxPOS 8
#define _USE_MATH_DEFINES
#include <math.h>

extern CFreeasyApp theApp;

void ClearSpeedFiles( CFreePcbDoc * doc )
{
	CMenu* pMenu = &theApp.m_main;
	CMenu* smenu = pMenu->GetSubMenu(0); // file menu
	if( smenu )
	{
		CMenu* ssmenu = smenu->GetSubMenu(2); // open from project folder menu
		if( ssmenu )
		{
			while( ssmenu->GetMenuItemCount() > 3 )
			{
				// delete menu
				int pos = 0;
				CMenu*  sub[MxPOS];
				if( sub[0] = ssmenu->GetSubMenu(0) )
				{
					do
					{
						while( sub[pos+1] = sub[pos]->GetSubMenu(0) )
							pos++;
						pos--;
						if( pos >= 0 )
							sub[pos]->RemoveMenu(0,MF_BYPOSITION);	// DeleteMenu
					} while( pos >= 0 );
					sub[0]->RemoveMenu(0,MF_BYPOSITION);	// DeleteMenu
				}
				ssmenu->RemoveMenu(0,MF_BYPOSITION);	// DeleteMenu		
			}
		}
	}
	CWnd * frm = AfxGetMainWnd();
	if( frm )
	{
		frm->SetMenu( &theApp.m_main );
	}
	doc->m_ProjectFiles.RemoveAll();
}

void LoadSpeedFiles( CFreePcbDoc * doc )
{
	if( doc->m_ProjectFiles.GetSize() )
		return;
	struct _stat buf;
	if( doc->m_parent_folder.Right(1) == "\\" )
		doc->m_parent_folder = doc->m_parent_folder.Left(doc->m_parent_folder.GetLength() - 1);
	int err = _stat( doc->m_parent_folder, &buf );//ok
	if( err && doc->m_parent_folder.GetLength() > 3 )
	{
		int isep = doc->m_app_dir.ReverseFind( '\\' );
		if( isep == -1 )
			isep = doc->m_app_dir.ReverseFind( ':' );
		if( isep == -1 )
			ASSERT(0);		// unable to parse path
		CString app = doc->m_app_dir.Left(isep+1);
		doc->m_parent_folder = app + "Projects";
	}
	//
	CMenu* pMenu = &theApp.m_main;
	CMenu* smenu = pMenu->GetSubMenu(0); // file menu
	int pos = 0;
	int n_menu = 0;
	int n_item = 0;
	BOOL bWorking[MxPOS];
	CFileFind finder[MxPOS];
	CString search_str[MxPOS];
	CString Path = doc->m_parent_folder;
	CString out_str = "";
	int CDSFileFound = 0;
	if( smenu )
	{
		CMenu* ssmenu[MxPOS];
		for( int im=0; im<MxPOS; im++ )
			ssmenu[im] = NULL;
		ssmenu[pos] = smenu->GetSubMenu(2); // open from project folder menu
		if( ssmenu )
		{
			int win = Path.Find("Windows");
			int mcnt = ssmenu[pos]->GetMenuItemCount();
			if( mcnt == 3 && win == -1 )
			{
				search_str[pos] = Path + "\\*";
				bWorking[pos] = finder[pos].FindFile( search_str[pos] );
				while ( bWorking[pos] )
				{
					bWorking[pos] = finder[pos].FindNextFile();
					CString name = finder[pos].GetFileName();
					win = max( win, name.Find("Windows") );
					if( finder[pos].IsDots() ||
						finder[pos].IsHidden() ||
						finder[pos].IsSystem() ||
						win >= 0 )
					{
					}
					else
					{
						CString fn = finder[pos].GetFilePath();
						if( !finder[pos].IsDirectory() )
						{
							if( name.Right(4) == ".cds" &&
								name.Compare("bom.cds") && // skip reserved names
								name.Compare("tag.cds") )
							{
								n_item++;
								CDSFileFound = n_item;
								n_menu++;
								ssmenu[pos]->InsertMenu(0, MF_BYPOSITION, ID_FILE_OPEN_FROM_START+n_menu, name );
								doc->m_ProjectFiles.Add( fn );

								// test for changes
								{
									CStdioFile f;
									int ret = f.Open( fn, CFile::modeRead, NULL );
									if( ret )
									{
										CString str, key_str;
										for( int ir=0; ir<5; ir++ )
										{
											if( f.ReadString( str ) == 0 )
												break;
											CArray<CString> arr;
											int np = ParseKeyString( &str, &key_str, &arr );
											if( np == 2 && key_str == "app_folder" )
											{
												if( arr.GetAt(0).Compare( doc->m_app_dir ) )// comp w m_get_app_folder
												{
													CString path = doc->m_app_dir + "\\Backups";
													struct _stat buf;
													int err = _stat( path+"\\"+name, &buf );
													if( err == 0 )
													{
														fn.Replace("version","v");
														if( fn.GetLength() > 50 )
															out_str += "..." + fn.Right(50) + "\n";
														else
															out_str += fn + "\n";
													}
												}
												break;
											}
										}
										f.Close();
									}
								}
							}
						}
						else if( pos >= 0 && pos < (MxPOS-1) &&
								name.Compare("bom") && // skip reserved names
								name.Compare("tag") )
						{
							n_item++;
							MENUITEMINFO MenuItem; 
							MenuItem.cbSize=sizeof(MENUITEMINFO);
							MenuItem.fMask=MIIM_STATE|MIIM_TYPE|MIIM_SUBMENU|MIIM_ID;
							MenuItem.fType=MFT_STRING;
							MenuItem.fState=MFS_ENABLED;		
							MenuItem.dwTypeData = name.GetBuffer();
							MenuItem.cch = name.GetLength()+1;
							MenuItem.wID = ID_FILE_OPEN_FROM_START;//ПОПРАВИТЬ НА НУЖНЫЙ ID
							MenuItem.hSubMenu=CreatePopupMenu();
							InsertMenuItem( ssmenu[pos]->GetSafeHmenu(), 0, 1, &MenuItem );
							ssmenu[pos+1] = ssmenu[pos]->GetSubMenu(0);
							pos++;
							search_str[pos] = fn + "\\*";
							Path += ("\\" + name);	
							bWorking[pos] = finder[pos].FindFile( search_str[pos] );
						}
					}
					while( !bWorking[pos] || !ssmenu[pos] )
					{
						pos--;
						if( pos < 0 )
							break;
						if( n_item > CDSFileFound )
						{
							// delete menu
							ssmenu[pos]->RemoveMenu( 0, MF_BYPOSITION );
							n_item--;
						}
						int rf = Path.ReverseFind('\\');
						if( rf > 0 )
							Path = Path.Left(rf);
						if( Path.Right(1) == "\\" )
							Path.Truncate( Path.GetLength()-1 );
					}
					if( pos < 0 || n_menu >= (ID_FILE_OPEN_FROM_END - ID_FILE_OPEN_FROM_START) )
						break;
				}
			}
			if( win >= 0 )
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Project folder and file names cannot contain the reserved word <Windows>":
					"Имена папок и файлов проекта не могут содержать зарезервированное слово «Windows»");
		}
	}
	out_str.MakeLower();
	if( out_str.GetLength() )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			("The following files have been modified by a program "\
			"version with a different ID:\n\n"+out_str+"\nIt is possible that "\
			"changes were made to the component datasheet library, which is "\
			"stored in the root directory with the program. In this regard, it is "\
			"recommended to open each of these files and resave in the current version of the program."):
			("Следующие файлы были изменены программой СхемАтор "\
			"с другим идентификатором:\n\n" + out_str + "\nВозможно, что "\
			"были внесены изменения в библиотеку спецификаций компонентов, которая "\
			"хранится в корневом каталоге с программой. В связи с этим рекомендуется "\
			"открыть каждый из этих файлов и пересохранить в текущей версии программы."));
	CWnd * frm = AfxGetMainWnd();
	if( frm )
	{
		frm->SetMenu( &theApp.m_main );
	}
}


void OnGroupGridMagnetize( CFreePcbDoc * doc )
{
	CArray<CPoint> pts;
	CPoint P1(doc->m_view->m_last_cursor_point.x, doc->m_view->m_last_cursor_point.y);
	pts.Add(P1);
	/*for (int i = 0; i<doc->m_outline_poly->GetSize(); i++)
	{
		CPolyLine * p = &doc->m_outline_poly->GetAt(i);
		if( p->m_visible == 0 )
			continue;
		if( p->GetSideSel() && p->GetNumCorners() )
		{
			///if( p->Check( index_pin_attr ) )
			{
				CPoint P1( p->GetX(0), p->GetY(0) );
				pts.Add(P1);
				CPoint P2( p->GetX(p->GetNumCorners()-1), p->GetY(p->GetNumCorners()-1) );
				pts.Add(P2);
			}
		}
	}*/
	if( pts.GetSize() )
	{
		int dx=0, dy=0, ok=0, min_d=INT_MAX;
		for(int i=0; i<doc->m_outline_poly->GetSize(); i++)
		{
			CPolyLine * p = &doc->m_outline_poly->GetAt(i);
			if( p->m_visible == 0 )
				continue;
			if( p->GetSideSel() == 0 )
			{
				for(int i1=0; i1<p->GetNumCorners(); i1++)
				{
					if (p->GetSel(i1))
						continue;
					CPoint P( p->GetX(i1), p->GetY(i1) );
					int dx2 = 0;
					int dy2 = 0;
					for(int i2=0; i2<pts.GetSize(); i2++)
					{
						dx2 = P.x-pts.GetAt(i2).x;
						dy2 = P.y-pts.GetAt(i2).y;
						if( abs( dx2 ) < doc->m_part_grid_spacing/doc->m_view->m_user_scale &&
							abs( dy2 ) < doc->m_part_grid_spacing/doc->m_view->m_user_scale )
						{
							int d = Distance(P.x, P.y, pts.GetAt(i2).x, pts.GetAt(i2).y);
							if (d < min_d)
							{
								min_d = d;
								dx = dx2;
								dy = dy2;
								ok = 1;
								break;
							}
						}
					}
				}
			}
		}
		if (ok == 0)
		{
			int grid_sp = doc->m_part_grid_spacing / doc->m_view->m_user_scale;
			for (int i = 0; i < doc->m_outline_poly->GetSize(); i++)
			{
				CPolyLine* p = &doc->m_outline_poly->GetAt(i);
				if (p->m_visible == 0)
					continue;
				if (p->GetSideSel() == 0)
				{
					for (int i1 = 0; i1 < p->GetNumSides(); i1++)
					{
						if (p->GetSel(i1))
							continue;
						int in = p->GetNextCornerIndex(i1);
						if (p->GetSel(in))
							continue;
						CPoint P1(p->GetX(i1), p->GetY(i1));
						CPoint P2(p->GetX(in), p->GetY(in));
						int style = p->GetSideStyle(i1);
						for (int i2 = 0; i2 < pts.GetSize(); i2++)
						{
							double X0 = 0, Y0 = 0;
							int dst = GetClearanceBetweenSegments(P1.x, P1.y, P2.x, P2.y, style, 0,
								pts.GetAt(i2).x, pts.GetAt(i2).y,
								pts.GetAt(i2).x, pts.GetAt(i2).y, 0, 0, grid_sp * 2, &dx, &dy);

							if (dst < grid_sp && style == 0)
							{
								float a = Angle(P1.x, P1.y, P2.x, P2.y);
								CPoint P3(pts.GetAt(i2).x, pts.GetAt(i2).y);
								if (a < 1.0 || a > 359.0)
									P3.y += 100;
								else if (a < 181.0 && a > 179.0)
									P3.y += 100;
								else if (a < 91.0 && a > 89.0)
									P3.x += 100;
								else if (a < 271.0 && a > 269.0)
									P3.x += 100;
								else
								{
									P3.x += 100 * cos((a - 90.0) * M_PI / 180.0);
									P3.y += 100 * sin((a - 90.0) * M_PI / 180.0);
								}
								FindLineIntersection((double)P1.x, (double)P1.y, (double)P2.x, (double)P2.y,
									(double)pts.GetAt(i2).x, (double)pts.GetAt(i2).y,
									(double)P3.x, (double)P3.y, &X0, &Y0);
								dx = X0 - pts.GetAt(i2).x;
								dy = Y0 - pts.GetAt(i2).y;
								ok = 1;
								break;
							}
							else
							{
								dx = 0;
								dy = 0;
							}
						}
						if (ok)
							break;
					}
					if (ok)
						break;
				}
			}
		}
		if (!dx || !dy)
		{
			int grid_sp = doc->m_part_grid_spacing / doc->m_view->m_user_scale;
			for (int i = 0; i < doc->m_outline_poly->GetSize(); i++)
			{
				CPolyLine* p = &doc->m_outline_poly->GetAt(i);
				if (p->m_visible == 0)
					continue;
				if (p->GetSel())
				{
					for (int i1 = 0; i1 < p->GetNumSides(); i1++)
					{
						if (p->GetSideSel(i1))
							continue;
						int in = p->GetNextCornerIndex(i1);
						int ip = p->GetPrevCornerIndex(i1);
						int next_side_sel = 0;
						int prev_side_sel = 0;
						if (in < p->GetNumSides())
							next_side_sel = p->GetSideSel(in);
						if (ip < p->GetNumSides())
							prev_side_sel = p->GetSideSel(ip);
						BOOL ok1 = ((p->GetSel(i1) || prev_side_sel) && p->GetSel(in) == 0);
						BOOL ok2 = (p->GetSel(i1) == 0 && (p->GetSel(in) || next_side_sel));
						if (ok1 || ok2)
						{
							int ch_x = p->GetX(i1) - p->GetX(in);
							int ch_y = p->GetY(i1) - p->GetY(in);
							if ((abs(ch_x) < grid_sp) && !dx )
								if (ok1)
									dx = -ch_x;
								else
									dx = ch_x;
							if ((abs(ch_y) < grid_sp) && !dy )
								if (ok1)
									dy = -ch_y;
								else
									dy = ch_y;
							ok = 1;
						}
					}
				}
			}
		}
		if(ok)
		{
			doc->m_view->MoveGroup( dx, dy );
		}
	}
}

void OnGroupVtxMagnetize( CFreePcbDoc * doc )
{
	for(int i=0; i<doc->m_outline_poly->GetSize(); i++)
	{
		CPolyLine * p = &doc->m_outline_poly->GetAt(i);
		if( p->m_visible == 0 )
			continue;
		if( p->GetSel() && p->GetNumCorners() )
		{
			for( int ii=0; ii<p->GetNumCorners(); ii++ )
			{
				if( p->GetSel(ii)  )
				{
					CPoint P( p->GetX(ii), p->GetY(ii) );
					int BEST_I = -1;
					int BEST_II = -1;
					int MIN_D = INT_MAX;
					for(int n=0; n<doc->m_outline_poly->GetSize(); n++)
					{
						CPolyLine * pp = &doc->m_outline_poly->GetAt(n);
						if( pp->m_visible == 0 )
							continue;
						if( pp->GetSel() && pp->GetNumCorners() )
						{
							for( int nn=0; nn<pp->GetNumCorners(); nn++ )
							{
								if( pp == p && nn == ii )
									continue;
								if( pp->GetSel(nn)  )
								{
									CPoint PP( pp->GetX(nn), pp->GetY(nn) );
									int dist = Distance( P.x, P.y, PP.x, PP.y );
									if( BEST_I == -1 )
									{
										BEST_I = n; 
										BEST_II = nn;
										MIN_D = dist;
									}
									else if( MIN_D > dist )
									{		
										BEST_I = n; 
										BEST_II = nn;
										MIN_D = dist;
									}
								}
							}
						}
					}
					if( BEST_I != -1 )
					{
						CPolyLine * pp = &doc->m_outline_poly->GetAt(BEST_I);
						int isNetMark = 0;
						if( CText * net = p->Check( index_net_attr ) )
							if( net->m_tl == doc->Attr->m_Netlist )
								if( pp->Check( index_pin_attr ) == NULL )
									isNetMark = 1;
						if( p->Check( index_pin_attr ) || isNetMark )
						{
							int ok = 0;
							if( pp->Check( index_part_attr ) )
								ok = 1;
							else if( CText * net = pp->Check( index_net_attr ) )
								if( net->m_tl == doc->Attr->m_Netlist )
									ok = 1;
							if( ok )
							{
								int dx = P.x - pp->GetX( BEST_II );
								int dy = P.y - pp->GetY( BEST_II );
								if( CText * net = pp->Check( index_net_attr ) )
								{
									net->m_x += dx;
									net->m_y += dy;
									net->MakeVisible();
								}
								for( int nn=0; nn<pp->GetNumCorners(); nn++ )
									if( nn != BEST_II )
									{
										pp->SetX( nn, pp->GetX(nn)+dx );
										pp->SetY( nn, pp->GetY(nn)+dy );
									}
							}
							pp->SetX( BEST_II, P.x );
							pp->SetY( BEST_II, P.y );
							pp->Show();
						}
						else
						{
							int ok = 0;
							if( p->Check( index_part_attr ) )
								ok = 1;
							else if( CText * net = p->Check( index_net_attr ) )
								if( net->m_tl == doc->Attr->m_Netlist )
									ok = 1;
							if( ok )
							{
								int dx = P.x - pp->GetX( BEST_II );
								int dy = P.y - pp->GetY( BEST_II );
								if( CText * net = p->Check( index_net_attr ) )
								{
									net->m_x -= dx;
									net->m_y -= dy;
									net->MakeVisible();
								}
								for( int nn=0; nn<p->GetNumCorners(); nn++ )
									if( nn != ii )
									{
										p->SetX( nn, p->GetX(nn)-dx );
										p->SetY( nn, p->GetY(nn)-dy );
									}
							}
							p->SetX( ii, pp->GetX( BEST_II ) );
							p->SetY( ii, pp->GetY( BEST_II ) );
							p->Show();
						}
					}
				}
			}
		}
	}
}

int OnPolylineUpdatePcbView(CFreePcbDoc* doc, int m_sel_i, CString* old_board, BOOL bCheckMergeOnly)
{
	CText* desc = doc->m_outline_poly->GetAt(m_sel_i).Check(index_desc_attr);
	if (desc == NULL)
		return 0;
	RECT oprect = doc->m_outline_poly->GetAt(m_sel_i).GetCornerBounds();
	int centX = (oprect.left + oprect.right) / 2;
	int centY = (oprect.top + oprect.bottom) / 2;
	CString cmd = "";
	int iof = desc->m_str.Find("|pcb:");
	if (iof > 0)
	{
		cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 5);
		iof = cmd.Find("'");
		if (iof >= 0)
			cmd = cmd.Left(iof);
	}
	cmd = cmd.Trim();
	CString name = cmd + PCBV;
	CString pcb_view = doc->m_path_to_folder + freeasy_netlist + name;
	if (PathFileExists(pcb_view) == 0)
	{
		cmd.Format(G_LANGUAGE?"Указанный путь к файлу печатной платы не существует:\n\n%s":"PCB file not found:\n\n%s", pcb_view);
		AfxMessageBox(cmd);
		return 0;
	}
	if (old_board)
		if(old_board->GetLength())
		{
			iof = old_board->Find("|pcb:");
			if (iof > 0)
			{
				cmd = old_board->Right(old_board->GetLength() - iof - 5);
				iof = cmd.Find("'");
				if (iof >= 0)
					cmd = cmd.Left(iof);
			}
			cmd = cmd.Trim();
			CString old_name = cmd + PCBV;
			int old_flipped = 0;
			iof = old_board->Find("|flipped:");
			if (iof > 0)
			{
				cmd = old_board->Right(old_board->GetLength() - iof - 9);
				iof = cmd.Find("'");
				if (iof >= 0)
					cmd = cmd.Left(iof);
				cmd = cmd.Trim();
				old_flipped = my_atoi(&cmd);
			}
			CString old_mergeName;
			old_mergeName.Format("%s%d", old_name, old_flipped + 1);
			int idm = doc->m_mlist->GetIndex(old_mergeName);
			if (idm >= 0)
			{
				doc->m_view->NewSelectM(idm);
				if (bCheckMergeOnly && doc->m_view->m_sel_count > 0)
					return 0;
				doc->m_view->DeleteGroup(1);
			}
		}
	//======================  flipped  ========================================
	CString mergeName="";
	{
		int flipped = 0;
		iof = desc->m_str.Find("|flipped:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 9);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			flipped = my_atoi(&cmd);
		}
		mergeName.Format("%s%d", name, flipped + 1);
		//------------------------------------------
		doc->PasteFromFile(pcb_view, FALSE, flipped);
		//------------------------------------------
	}
	//===========  rotation  =============  scale_factor  ===============
	{
		int rotation = 0;
		iof = desc->m_str.Find("|rotation:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 10);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			rotation = my_atof(&cmd);
		}
		if (rotation)
			doc->m_view->RotateGroup(rotation);
	}
	int id_m = doc->m_mlist->AddNew(mergeName);
	doc->m_view->MergeGroup(id_m);
	RECT hiliteRect = doc->m_dlist->GetHighlightedBounds(NULL);
	int AverageX = (hiliteRect.right + hiliteRect.left) / 2;
	int AverageY = (hiliteRect.top + hiliteRect.bottom) / 2;
	doc->m_view->MoveGroup(centX - AverageX, centY - AverageY);
	double scale = (double)(oprect.right - oprect.left) / (double)(hiliteRect.right - hiliteRect.left);
	double scale2 = (double)(oprect.top - oprect.bottom) / (double)(hiliteRect.top - hiliteRect.bottom);
	scale = min(scale, scale2);
	double scale_factor = 1.0;
	iof = desc->m_str.Find("|scale_factor:");
	if (iof > 0)
	{
		cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 14);
		iof = cmd.Find("'");
		if (iof >= 0)
			cmd = cmd.Left(iof);
		cmd = cmd.Trim();
		scale_factor = my_atof(&cmd);
		if (abs(scale_factor) > 0.09 && abs(scale_factor) < 11)
			scale = scale_factor;
		else
			scale_factor = scale;
	}
	doc->m_view->ScaleFactor(scale, 1);
	//========================  draw_size  ==========================
	CString DrawSize = "";
	{
		iof = desc->m_str.Find("|draw_size:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 11);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			DrawSize = cmd;
			if(DrawSize.FindOneOf("LRTB") >= 0)
				doc->m_view->OnAddGroupRect();
		}
	}
	//==========  text_height  ============  font_width  ============  UID  ==========
	{
		int TH = 0;
		iof = desc->m_str.Find("|text_height:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 13);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			TH = my_atof(&cmd) * scale_factor;// / doc->m_view->m_user_scale;
		}
		int TW = 0;
		iof = desc->m_str.Find("|font_width:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 12);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			TW = my_atof(&cmd) * scale_factor;// / doc->m_view->m_user_scale;
		}
		int UID = 0;
		iof = desc->m_str.Find("|UID:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 5);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			UID = my_atoi(&cmd);
		}
		int it = -1;
		for (CText* gt = doc->Attr->m_pDesclist->GetNextText(&it); gt; gt = doc->Attr->m_pDesclist->GetNextText(&it))
		{
			if (gt->m_selected == 0)
				continue;
			if (gt->m_str.Find("VIA") == 0 ||
				gt->m_str.Find("BOARD") == 0)
			{
				gt->m_font_size = 0;
				gt->m_stroke_width = 0;
			}
			else
			{   //-------------------
				if (gt->m_angle%180 == 0)
					gt->m_angle = 0;
				else if (gt->m_angle%90 == 0)
					gt->m_angle = -90;
				//-------------------
				gt->m_font_size = TH;
				gt->m_stroke_width = TW;
				if (UID)
				{
					gt->Undraw();
					iof = gt->m_str.Find("|");
					gt->m_str = gt->m_str.Right(gt->m_str.GetLength() - iof - 1);
					gt->m_nchars = gt->m_str.GetLength();
				}
			}
			gt->MakeVisible();
		}
	}
	//===========  fill_board  ==========  fill_mask  ===========  line_width  =============
	{
		int fill_board = 0;
		iof = desc->m_str.Find("|fill_board:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 12);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			fill_board = my_atof(&cmd);
		}
		int fill_mask = 0;
		iof = desc->m_str.Find("|fill_mask:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 11);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			fill_mask = my_atof(&cmd);
		}
		int line_width = 0;
		iof = desc->m_str.Find("|line_width:");
		if (iof > 0)
		{
			cmd = desc->m_str.Right(desc->m_str.GetLength() - iof - 12);
			iof = cmd.Find("'");
			if (iof >= 0)
				cmd = cmd.Left(iof);
			cmd = cmd.Trim();
			line_width = my_atof(&cmd) * scale_factor;// / doc->m_view->m_user_scale;
		}
		for (int i = 0; i < doc->m_outline_poly->GetSize(); i++)
		{
			CPolyLine* poly = &doc->m_outline_poly->GetAt(i);
			if (poly->m_visible == 0)
				continue;
			if (poly->GetSel() == 0)
				continue;
			if (poly->GetLayer() == LAY_PCB_BOARD)
				poly->SetHatch(fill_board);
			else if (poly->GetLayer() == LAY_PCB_MASK)
				poly->SetHatch(fill_mask);
			else if (line_width)
			{
				if (poly->GetLayer() == LAY_PCB_SILK || poly->GetLayer() == LAY_PCB_NOTES)
				{
					poly->SetW(line_width);
					poly->Draw();
				}	
			}
			
		}
	}
	//==============================================================
	desc->m_font_size = 0;
	desc->m_stroke_width = 0;
	desc->MakeVisible();
	if (DrawSize.FindOneOf("LRTB") >= 0)
	{
		int sz = doc->m_outline_poly->GetSize();
		doc->m_outline_poly->GetAt(sz - 1).SetMerge(id_m);
		for (int item = 0; item < 4; item++)
		{
			switch (item)
			{
			case 0:
				if (DrawSize.Find("L") == -1)
					continue;
				break;
			case 1:
				if (DrawSize.Find("T") == -1)
					continue;
				break;
			case 2:
				if (DrawSize.Find("R") == -1)
					continue;
				break;
			case 3:
				if (DrawSize.Find("B") == -1)
					continue;
				break;
			default:
				break;
			}
			id ID(ID_POLYLINE, ID_GRAPHIC, sz - 1, ID_SIDE, item);
			doc->m_view->CancelSelection(0);
			doc->m_view->NewSelect(NULL, &ID, 0, 0);
			AddGraphicSize(doc);
			doc->m_outline_poly->GetAt(doc->m_outline_poly->GetSize() - 1).SetMerge(id_m);
		}
	}
	doc->ProjectModified(TRUE);
	doc->m_view->m_draw_layer = 0;
	doc->OnRangeCmds(NULL);
	return TRUE;
}

void AddGraphicSize(CFreePcbDoc* doc)
{
	if (doc->m_view->m_sel_count == 0)
		return;

	// test
	if (doc->m_view->m_sel_id.i < 0 || 
		doc->m_view->m_sel_id.i >= doc->m_outline_poly->GetSize())
		return;
	if (doc->m_view->m_sel_id.ii < 0 ||
		doc->m_view->m_sel_id.ii >= doc->m_outline_poly->GetAt(doc->m_view->m_sel_id.i).GetNumSides())
		return;
#define sel_op doc->m_outline_poly->GetAt(doc->m_view->m_sel_id.i)
	//
	int n = sel_op.GetNextCornerIndex(doc->m_view->m_sel_id.ii);
	double x1 = sel_op.GetX(doc->m_view->m_sel_id.ii);
	double y1 = sel_op.GetY(doc->m_view->m_sel_id.ii);
	double x2 = sel_op.GetX(n);
	double y2 = sel_op.GetY(n);
	double dx = x2 - x1;
	double dy = y2 - y1;
	int len = Distance(x1, y1, x2, y2);
	RECT R;
	doc->GetPolylineBounds(&R);
	double shift = (R.right - R.left) / doc->m_view->m_measure_scale;
	double arrow = (R.right - R.left) / doc->m_view->m_arrow_scale;
	shift = max(shift, 3 * NM_PER_MM);
	double an = Angle(x2, y2, x1, y1);
	double acc = an - (int)an;
	double dir = 90.0;
	doc->m_view->SelectContour();
	CPoint Center = doc->m_view->FindGroupCenter(0);
	doc->m_view->SaveUndoInfoForGroup(doc->m_undo_list);
	doc->m_view->RotateGroup(-(int)an, Center.x, Center.y, -acc);
	doc->ProjectModified(TRUE);
	double y1m = sel_op.GetY(doc->m_view->m_sel_id.ii);
	R = sel_op.GetCornerBounds();
	if (y1m < (R.top + R.bottom) / 2)
		dir = -90.0;
	doc->OnEditUndo();
	double cur_x = x1, cur_y = y1;
	cur_x = x1 + shift * cos((an + dir) * M_PI / 180.0);
	cur_y = y1 + shift * sin((an + dir) * M_PI / 180.0);
	int sz = doc->m_outline_poly->GetSize();
	doc->m_outline_poly->SetSize(sz + 1);
	doc->m_outline_poly->GetAt(sz).SetDisplayList(doc->m_dlist);
	id sel_id(ID_POLYLINE, ID_GRAPHIC, sz);
	doc->m_outline_poly->GetAt(sz).Start(LAY_ADD_1, NM_PER_MIL, NM_PER_MIL, x1, y1, CPolyLine::DIAGONAL_FULL, &sel_id, NULL);
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	cur_x = x1 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
	cur_y = y1 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	if (len > arrow / 2)
	{
		double cur_x2 = cur_x + 0.17 * arrow * cos((an + 0.1 * dir) * M_PI / 180.0);
		double cur_y2 = cur_y + 0.17 * arrow * sin((an + 0.1 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.14 * arrow * cos((an)*M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an)*M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x = x2 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
		cur_y = y2 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
		cur_x2 = cur_x + 0.14 * arrow * cos((an + 2.0 * dir) * M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an + 2.0 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.17 * arrow * cos((an + 1.9 * dir) * M_PI / 180.0);
		cur_y2 = cur_y + 0.17 * arrow * sin((an + 1.9 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
	}
	else
	{
		double cur_x2 = cur_x + 0.17 * arrow * cos((an + 1.9 * dir) * M_PI / 180.0);
		double cur_y2 = cur_y + 0.17 * arrow * sin((an + 1.9 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.14 * arrow * cos((an + 2.0 * dir) * M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an + 2.0 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		double cur_x3 = cur_x + 0.25 * arrow * cos((an + 2.0 * dir) * M_PI / 180.0);
		double cur_y3 = cur_y + 0.25 * arrow * sin((an + 2.0 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x3, cur_y3);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.17 * arrow * cos((an + 0.1 * dir + 180.0) * M_PI / 180.0);
		cur_y2 = cur_y + 0.17 * arrow * sin((an + 0.1 * dir + 180.0) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
		cur_x = x2 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
		cur_y = y2 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
	}
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	cur_x = x2 + shift * cos((an + dir) * M_PI / 180.0);
	cur_y = y2 + shift * sin((an + dir) * M_PI / 180.0);
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	doc->m_outline_poly->GetAt(sz).AppendCorner(x2, y2);
	cur_x = x2 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
	cur_y = y2 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	if (len > arrow / 2)
	{
		double cur_x2 = cur_x + 0.17 * arrow * cos((an + 0.1 * dir + 180.0) * M_PI / 180.0);
		double cur_y2 = cur_y + 0.17 * arrow * sin((an + 0.1 * dir + 180.0) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.14 * arrow * cos((an + 180.0) * M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an + 180.0) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x = x1 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
		cur_y = y1 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
		cur_x2 = cur_x + 0.14 * arrow * cos((an)*M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an)*M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.17 * arrow * cos((an + 1.9 * dir + 180.0) * M_PI / 180.0);
		cur_y2 = cur_y + 0.17 * arrow * sin((an + 1.9 * dir + 180.0) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
	}
	else
	{
		double cur_x2 = cur_x + 0.17 * arrow * cos((an + 0.1 * dir) * M_PI / 180.0);
		double cur_y2 = cur_y + 0.17 * arrow * sin((an + 0.1 * dir) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.14 * arrow * cos((an)*M_PI / 180.0);
		cur_y2 = cur_y + 0.14 * arrow * sin((an)*M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		double cur_x3 = cur_x + 0.25 * arrow * cos((an)*M_PI / 180.0);
		double cur_y3 = cur_y + 0.25 * arrow * sin((an)*M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x3, cur_y3);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		cur_x2 = cur_x + 0.17 * arrow * cos((an + 1.9 * dir + 180.0) * M_PI / 180.0);
		cur_y2 = cur_y + 0.17 * arrow * sin((an + 1.9 * dir + 180.0) * M_PI / 180.0);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x2, cur_y2);
		doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	}
	cur_x = x1 + 0.9 * shift * cos((an + dir) * M_PI / 180.0);
	cur_y = y1 + 0.9 * shift * sin((an + dir) * M_PI / 180.0);
	doc->m_outline_poly->GetAt(sz).AppendCorner(cur_x, cur_y);
	doc->m_outline_poly->GetAt(sz).Close();

	// info text
	double xc = (x1 + x2) / 2;
	double yc = (y1 + y2) / 2;
	if (len < arrow / 2)
		shift *= 1.1;
	cur_x = xc + (0.92 * shift + doc->m_view->m_attr_size.H_pindesc) * cos((an + dir) * M_PI / 180.0);
	cur_y = yc + (0.92 * shift + doc->m_view->m_attr_size.H_pindesc) * sin((an + dir) * M_PI / 180.0);
	CString info;
	MakeCStringFromDimension(doc->m_view->m_user_scale, &info, len, doc->m_units, 1, 1, 1, doc->m_units == MIL ? 1 : 2);
	int t_an = -an;
	if (t_an < -270)
		t_an += 360;
	else if (t_an < -90)
		t_an += 180;
	CText* newTxt = doc->Attr->m_pDesclist->AddText(cur_x,
		cur_y,
		t_an,
		LAY_PIN_DESC,
		doc->m_view->m_attr_size.H_pindesc,
		doc->m_view->m_attr_size.W_pindesc,
		doc->m_default_font,
		&info);

	doc->m_outline_poly->GetAt(sz).pAttr[index_desc_attr] = newTxt;
	newTxt->m_polyline_start = sz;
	doc->Attr->m_pDesclist->GetTextRectOnPCB(newTxt, &R);
	xc = (R.right + R.left) / 2;
	yc = (R.top + R.bottom) / 2;
	doc->Attr->m_pDesclist->MoveText(newTxt,
		2 * cur_x - xc,
		2 * cur_y - yc,
		t_an, LAY_PIN_DESC);
	doc->m_outline_poly->GetAt(sz).Hide();
	newTxt->isVISIBLE = 0;
	doc->m_view->SaveUndoInfoForOutlinePoly(sz, TRUE, doc->m_undo_list);
	doc->m_view->SaveUndoInfoForText(newTxt, 1, doc->m_undo_list);
	doc->m_outline_poly->GetAt(sz).Show();
	newTxt->MakeVisible();
	doc->ProjectModified(TRUE);

	// set selection
	id idt(ID_TEXT_DEF);
	doc->m_view->NewSelect(newTxt, &idt, 0, 0);
	for (int i = 1; i < doc->m_outline_poly->GetAt(sz).GetNumCorners(); i++)
	{
		if (len > arrow / 2)
		{
			if (i == 9)
				continue;
		}
		else if (i == 11)
			continue;
		id ID(ID_POLYLINE, ID_GRAPHIC, sz, ID_CORNER, i);
		doc->m_view->NewSelect(NULL, &ID, 0, 0);
	}
#undef sel_op
}

int GetStandartNode(CFreePcbDoc* doc, int ipoly)
{
	float dw = doc->m_outline_poly->GetAt(ipoly).GetW() - abs(doc->m_polyline_w);
	return abs(doc->m_node_w) + (dw * 1.5);
}