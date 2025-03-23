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
		int dx=0, dy=0, ok=0;
		for(int i=0; i<doc->m_outline_poly->GetSize(); i++)
		{
			CPolyLine * p = &doc->m_outline_poly->GetAt(i);
			if( p->m_visible == 0 )
				continue;
			if( p->GetSideSel() == 0 )
			{
				for(int i1=0; i1<p->GetNumCorners(); i1++)
				{
					CPoint P( p->GetX(i1), p->GetY(i1) );
					for(int i2=0; i2<pts.GetSize(); i2++)
					{
						dx = P.x-pts.GetAt(i2).x;
						dy = P.y-pts.GetAt(i2).y;
						if( abs( dx ) < doc->m_part_grid_spacing/doc->m_view->m_user_scale &&
							abs( dy ) < doc->m_part_grid_spacing/doc->m_view->m_user_scale )
						{
							ok = 1;
							break;
						}
						if (abs(dx) < p->GetW() &&
							abs(dy) < p->GetW())
						{
							ok = 1;
							break;
						}
					}
					if(ok)
						break;
				}
				if(ok)
					break;
			}
		}
		if (ok == 0)
		{
			dx = 0, dy = 0;
			int grid_sp = doc->m_part_grid_spacing / doc->m_view->m_user_scale;
			for (int i = 0; i < doc->m_outline_poly->GetSize(); i++)
			{
				CPolyLine* p = &doc->m_outline_poly->GetAt(i);
				if (p->m_visible == 0)
					continue;
				if (p->GetSideSel() == 0)
				{
					for (int i1 = 0; i1 < p->GetNumCorners(); i1++)
					{
						int in = p->GetNextCornerIndex(i1);
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
						}
						if (ok)
							break;
					}
					if (ok)
						break;
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