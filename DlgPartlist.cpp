
#include "stdafx.h"
#include "FreeSch.h"
#include "DlgPartlist.h"

//global partlist_info so that sorting callbacks will work
CArray<CString> pl[4];

// columns for list
enum {
	COL_NAME = 0,
	COL_VALUE,
	COL_FOOTPRINT,
	COL_PAGE,
	NUM_COLS
};

// sort types
enum {
	SORT_UP_NAME = 0,
	SORT_DOWN_NAME,
	SORT_UP_FOOTPRINT,
	SORT_DOWN_FOOTPRINT,
	SORT_UP_VALUE,
	SORT_DOWN_VALUE,
	SORT_UP_PAGE,
	SORT_DOWN_PAGE
};

// global callback function for sorting
// lp1, lp2 are indexes to global arrays above
//===============================================================================================	
int CALLBACK ComparePartlist( LPARAM lp1, LPARAM lp2, LPARAM type )
{
	int ret = 0;
	switch( type )
	{
		case SORT_UP_NAME:
		case SORT_DOWN_NAME:
		{
			//
			CString pref1, pref2, suff1, suff2;
			int num1 = ParseRef( &::pl[COL_NAME][lp1], &pref1, &suff1 );
			int num2 = ParseRef( &::pl[COL_NAME][lp2], &pref2, &suff2 );
			int len1 = suff1.GetLength();
			int len2 = suff2.GetLength();
			if( len1 != len2 )
			{
				if( len1 < len2 )
					ret = -1;
				else if( len1 > len2 )
					ret = 1;
				break;
			}
			if( pref1.Compare( pref2 ) == 0 )
			{
				if( len1 && len2 )
				{
					if( suff1 == suff2 )
					{
						if( num1 < num2 )
							ret = -1;
						else 
							ret = 1;
						break;
					}
					ret = (strcmp( suff1, suff2 ));
					break;
				}
				if( num1 != num2 )
				{
					if( num1 < num2 )
						ret = -1;
					else 
						ret = 1;
					break;
				}	
			}
			ret = (strcmp( ::pl[COL_NAME][lp1], ::pl[COL_NAME][lp2] ));
			break;
		}
		case SORT_UP_FOOTPRINT:
		case SORT_DOWN_FOOTPRINT:
			ret = (strcmp( ::pl[COL_FOOTPRINT][lp1], ::pl[COL_FOOTPRINT][lp2] ));
			break;

		case SORT_UP_VALUE:
		case SORT_DOWN_VALUE:
			ret = (strcmp( ::pl[COL_VALUE][lp1], ::pl[COL_VALUE][lp2] ));
			break;

		case SORT_UP_PAGE:
		case SORT_DOWN_PAGE:
			ret = (strcmp( ::pl[COL_PAGE][lp1], ::pl[COL_PAGE][lp2] ));
			break;
	}
	switch( type )
	{
		case SORT_DOWN_NAME:
		case SORT_DOWN_FOOTPRINT:
		case SORT_DOWN_VALUE:
		case SORT_DOWN_PAGE:
			ret = -ret;
			break;
	}
	return ret;
}

// CDlgPartlist dialog
//===============================================================================================
IMPLEMENT_DYNAMIC(CDlgPartlist, CDialog)
CDlgPartlist::CDlgPartlist(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPartlist::IDD, pParent)
{
}

CDlgPartlist::~CDlgPartlist()
{
	
}

//===============================================================================================
void CDlgPartlist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
	DDX_Control(pDX, IDOK, Butt[0]);
	DDX_Control(pDX, IDHPCB, Butt[1]);
	DDX_Control(pDX, IDJUMP, Butt[2]);
	DDX_Control(pDX, IDSALL, Butt[3]);
	DDX_Control(pDX, IDBOM, Butt[4]);
	DDX_Control(pDX, IDPARTLIST, Butt[5]);
	DDX_Control(pDX, IDC_CHECK_CURRENT_PCB, Butt[6]);
	DDX_Control(pDX, IDC_FILTER, m_flt);
	if( pDX->m_bSaveAndValidate )
	{
		
	}
	else
	{	
		m_list_ctrl.InsertColumn( COL_NAME, "Reference", LVCFMT_LEFT, m_colw[COL_NAME] );
		m_list_ctrl.InsertColumn( COL_VALUE, "Value", LVCFMT_LEFT, m_colw[COL_VALUE] );
		m_list_ctrl.InsertColumn( COL_FOOTPRINT, "Footprint", LVCFMT_LEFT, m_colw[COL_FOOTPRINT] );
		m_list_ctrl.InsertColumn( COL_PAGE, "Page", LVCFMT_LEFT, m_colw[COL_PAGE] );
		//
		ImportPartlist();
		m_flt.SetWindowTextA("search");
		SetWindowPos( this->GetWindow(1), 
			m_view->m_Doc->m_partlist_pos.cx, 
			m_view->m_Doc->m_partlist_pos.cy, 
			m_view->m_Doc->m_partlist_size.cx, 
			m_view->m_Doc->m_partlist_size.cy, 0 );
		CRect CR;
		this->GetClientRect(CR);
		Size( CR.Width(), abs(CR.Height()), 0 );	 
	}
}

//===============================================================================================
BEGIN_MESSAGE_MAP(CDlgPartlist, CDialog)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnLvnColumnClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnJumpTo)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_BN_CLICKED(IDJUMP, JumpTo)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDOK, OnHighlight)
	ON_BN_CLICKED(IDBOM, InvertSelection)
	ON_BN_CLICKED(IDPARTLIST, OnTXT)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_CONTROL(EN_CHANGE, IDC_FILTER, OnChange)
	ON_BN_CLICKED(IDC_CHECK_CURRENT_PCB, OnPCBCheck)
	ON_BN_CLICKED(IDSALL, &CDlgPartlist::OnBnClickedSall)
	ON_BN_CLICKED(IDHPCB, &CDlgPartlist::OnBnClickedHpcb)
	ON_EN_CHANGE(IDC_FILTER, &CDlgPartlist::OnEnChangeFilter)
END_MESSAGE_MAP()

//===============================================================================================
BOOL CDlgPartlist::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sort_type = SORT_UP_NAME;
	return TRUE;
}

//===============================================================================================
void CDlgPartlist::DrawListCtrl()
{
	// now set up listview control
	int nItem;
	LVITEM lvitem;
	CString str;
	DWORD old_style = m_list_ctrl.GetExtendedStyle();
	m_list_ctrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | old_style );
	m_list_ctrl.DeleteAllItems();
	//
	m_list_ctrl.ShowWindow(SW_HIDE);
	int item = 0;
	for( int i=0; i<::pl[COL_NAME].GetSize(); i++ )
	{
		if( i < bVisibled.GetSize() )
			if( bVisibled[i] == 0 )
				continue;
		lvitem.mask = LVIF_TEXT | LVIF_PARAM;
		lvitem.pszText = "";
		lvitem.lParam = i;
		nItem = m_list_ctrl.InsertItem( item, "" );
		m_list_ctrl.SetItemData( item, (LPARAM)i );
		m_list_ctrl.SetItem( item, COL_NAME, LVIF_TEXT, ::pl[COL_NAME][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( item, COL_VALUE, LVIF_TEXT, ::pl[COL_VALUE][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( item, COL_FOOTPRINT, LVIF_TEXT, ::pl[COL_FOOTPRINT][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( item, COL_PAGE, LVIF_TEXT, ::pl[COL_PAGE][i], 0, 0, 0, 0 );
		item++;
	}
	m_list_ctrl.SortItems( ::ComparePartlist, m_sort_type );	// resort 
	m_list_ctrl.ShowWindow(SW_SHOW);
}
//===============================================================================================
void CDlgPartlist::Initialize( CFreePcbView * view )
{
	m_view = view;
	m_pl = &m_view->m_Doc->Pages;
	m_sort_type = SORT_UP_NAME;
	pl[COL_NAME].RemoveAll();
	pl[COL_VALUE].RemoveAll();
	pl[COL_FOOTPRINT].RemoveAll();
	pl[COL_PAGE].RemoveAll();
	int colW = m_view->m_Doc->m_partlist_col_w;
	int bits = 255;
	m_colw[0] = bits&colW;
	m_colw[1] = ((bits<<8)&colW)>>8;
	m_colw[2] = ((bits<<16)&colW)>>16;
	m_colw[3] = ((bits<<24)&colW)>>24;
}


// set m_sort_type based on column clicked and last sort, 
// then sort the list, then save m_last_sort_type = m_sort_type
//===============================================================================================
void CDlgPartlist::OnLvnColumnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int column = pNMLV->iSubItem;
	if( column == COL_NAME )
	{
		if( m_sort_type == SORT_UP_NAME )
			m_sort_type = SORT_DOWN_NAME;
		else
			m_sort_type = SORT_UP_NAME;
		m_list_ctrl.SortItems( ::ComparePartlist, m_sort_type );
	}
	else if( column == COL_FOOTPRINT )
	{
		if( m_sort_type == SORT_UP_FOOTPRINT )
			m_sort_type = SORT_DOWN_FOOTPRINT;
		else
			m_sort_type = SORT_UP_FOOTPRINT;
		m_list_ctrl.SortItems( ::ComparePartlist, m_sort_type );
	}
	else if( column == COL_VALUE )
	{
		if( m_sort_type == SORT_UP_VALUE )
			m_sort_type = SORT_DOWN_VALUE;
		else
			m_sort_type = SORT_UP_VALUE;
		m_list_ctrl.SortItems( ::ComparePartlist, m_sort_type );
	}
	else if( column == COL_PAGE )
	{
		if( m_sort_type == SORT_UP_PAGE )
			m_sort_type = SORT_DOWN_PAGE;
		else
			m_sort_type = SORT_UP_PAGE;
		m_list_ctrl.SortItems( ::ComparePartlist, m_sort_type );
	}
	*pResult = 0;
}

//===============================================================================================
void CDlgPartlist::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*static int prev = -1;
	if( m_list_ctrl.GetSelectedCount() == 1 )
	{
		POSITION pos = m_list_ctrl.GetFirstSelectedItemPosition();
		int isel = m_list_ctrl.GetNextSelectedItem( pos );
		if( isel == prev && isel != -1 )
		{
			m_list_ctrl.SetItemState( isel, 0, LVIS_SELECTED );
			prev = -1;
		}
		else
			prev = isel;
	}*/
	SaveSelections();
	*pResult = 0;
}
//===============================================================================================
void CDlgPartlist::SaveSelections()
{
	int nItems = m_list_ctrl.GetItemCount();
	bSelected.SetSize( ::pl[COL_NAME].GetSize() );
	for( int i=0; i<bSelected.GetSize(); i++ )
		bSelected[i] = FALSE;
	for( int iItem=0; iItem<nItems; iItem++ )
	{
		int ip = m_list_ctrl.GetItemData( iItem );
		if( m_list_ctrl.GetItemState( iItem, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			bSelected[ip] = TRUE;
		}
	}
}
//===============================================================================================
void CDlgPartlist::RestoreSelections()
{
	int nItems = m_list_ctrl.GetItemCount();
	for( int iItem=0; iItem<nItems; iItem++ )
	{
		m_list_ctrl.SetItemState( iItem, 0, LVIS_SELECTED );
		int ip = m_list_ctrl.GetItemData( iItem );
		if( ip < bSelected.GetSize() )
			if( bSelected[ip] == TRUE )
			{
				m_list_ctrl.SetItemState( iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
			}
	}
}

void CDlgPartlist::OnJumpTo(NMHDR *pNMHDR, LRESULT *pResult)
{
	JumpTo();
}

void CDlgPartlist::JumpTo()
{
	int sel=0;
	for( ; sel<bSelected.GetSize(); sel++ )
		if( bSelected[sel] )
			break;
	if( sel < pl[COL_NAME].GetSize() )
	{
		CString ref = pl->GetAt(sel);
		if( ref.Right(1) == "." )
			if( ref.GetLength() > 2 )
				ref.Truncate( ref.GetLength()-2 );

		int it = -1;
		int curr_p = m_pl->GetActiveNumber();
		CText* Ref = NULL;
		int sw_to_page = m_pl->GetPageIndex(&(pl[COL_PAGE][sel]));
		sw_to_page = m_pl->FindPart( &ref, &Ref, TRUE, FALSE, -1, sw_to_page );
		if( Ref && sw_to_page >= 0 )
		{
			if( sw_to_page != curr_p )
				m_view->m_Doc->SwitchToPage( sw_to_page, TRUE, FALSE );
			m_view->JumpToPart( Ref );
		}
	}
}

void CDlgPartlist::Size( int cx, int cy, int save )
{
	RECT R;
	this->GetWindowRect(&R);
	if( save )
	{	
		m_view->m_Doc->m_partlist_pos.cx = R.left;
		m_view->m_Doc->m_partlist_pos.cy = R.top;
		m_view->m_Doc->m_partlist_size.cx = R.right-R.left;
		m_view->m_Doc->m_partlist_size.cy = R.bottom-R.top;
	}
	m_list_ctrl.SetWindowPos(NULL,0,0,cx-100,cy-20,0);
	m_flt.SetWindowPos(NULL,0,cy-21,cx-100,18,0);
	Butt[0].GetWindowRect(&R);
	for( int i=0; i<7; i++ )
		Butt[i].SetWindowPos(NULL,(i==6?(cx-92):(cx-90)),(i*25)+10,(i==6?(R.right-R.left+10):(R.right-R.left)),R.bottom-R.top,0);
	GetColWidths();
}

void CDlgPartlist::OnSize(UINT nType, int cx, int cy)
{
	if( pl[COL_NAME].GetSize() )//!!!
		Size(cx,cy,TRUE);	
}

void CDlgPartlist::OnMove(int x, int y)
{
	if( pl[COL_NAME].GetSize() )
	{
		RECT R;
		this->GetWindowRect(&R);
		m_view->m_Doc->m_partlist_pos.cx = R.left;
		m_view->m_Doc->m_partlist_pos.cy = R.top;
		m_view->m_Doc->m_partlist_size.cx = R.right-R.left;
		m_view->m_Doc->m_partlist_size.cy = R.bottom-R.top;
		GetColWidths();
	}
}

void CDlgPartlist::OnFilter()
{
	const int CURRENT_PCB = 6;
	int cur_pg = m_pl->GetActiveNumber();
	if( m_view->m_Doc->Pages.IsThePageIncludedInNetlist( cur_pg, TRUE ) == 0 )
	{
		int pg = m_view->m_Doc->Pages.IsThePageIncludesCP( cur_pg );
		if( pg >= 0 )
			cur_pg = pg;
	}
	//	
	CString str, key;
	m_flt.GetWindowTextA(str);
	if( str.Left(6) == "search" )
		str = "";
	
	BOOL bWhole = 0;
	CArray<CString> arr;
	int len = str.GetLength();
	if( str.Find( " " ) > 0 )
	{
		str = str.Trim();
		len = str.GetLength();
		str = "key: " + str;
		ParseKeyString( &str, &key, &arr );
		bWhole = 1;
	}
	
	bVisibled.SetSize( ::pl[COL_NAME].GetSize() );
	for( int i=pl[COL_NAME].GetSize()-1; i>=0; i-- )
	{
		bVisibled[i] = FALSE;
		if( Butt[CURRENT_PCB].GetCheck() )
		{
			int curr_ip = m_view->m_Doc->Pages.GetPageIndex( &pl[COL_PAGE][i] );
			if( m_view->m_Doc->Pages.GetPcbIndex(cur_pg) == -1 )
			{
				if( curr_ip != cur_pg )
					continue;
			}
			else
			{
				if( m_view->m_Doc->Pages.GetPcbIndex(cur_pg) != m_view->m_Doc->Pages.GetPcbIndex(curr_ip) )
					continue;
			}
		}
		//-----------
		if( len == 0 )
			bVisibled[i] = TRUE;
		else if( arr.GetSize() )
			for( int istr=0; istr<arr.GetSize(); istr++ )
			{
				str = arr.GetAt(istr);
				if( bWhole )
				{
					if( pl[COL_NAME][i].Compare(str) == 0 ||
						pl[COL_VALUE][i].Compare(str) == 0 ||
						pl[COL_FOOTPRINT][i].Compare(str) == 0 )
					{
						bVisibled[i] = TRUE;
						break;
					}
				}
				else if( ( pl[COL_NAME][i].Find(str) >= 0 ) ||
						 ( pl[COL_VALUE][i].Find(str) >= 0 ) ||
						 ( pl[COL_FOOTPRINT][i].Find(str) >= 0 ) )
				{
					bVisibled[i] = TRUE;
					break;
				}
			}
		else if( bWhole )
		{
			if( pl[COL_NAME][i].Compare(str) == 0 ||
				pl[COL_VALUE][i].Compare(str) == 0 ||
				pl[COL_FOOTPRINT][i].Compare(str) == 0 )
			{
				bVisibled[i] = TRUE;
			}
		}
		else if( ( pl[COL_NAME][i].Find(str) >= 0 ) ||
				 ( pl[COL_VALUE][i].Find(str) >= 0 ) ||
				 ( pl[COL_FOOTPRINT][i].Find(str) >= 0 ) )
		{
			bVisibled[i] = TRUE;
		}
	}
	if( Butt[CURRENT_PCB].GetCheck() )
	{
		for( int ipg=0; ipg<m_pl->GetNumPages(); ipg++ )
		{
			if( m_view->m_Doc->Pages.GetPcbIndex(cur_pg) != m_view->m_Doc->Pages.GetPcbIndex(ipg) )
				continue;
			int old_p = m_pl->GetActiveNumber();
			CTextList * dtl = m_view->m_Doc->Pages.GetTextList( ipg, index_desc_attr );
			int it = -1;
			for( CText * desc = dtl->GetNextText(&it); desc; desc = dtl->GetNextText(&it) )
			{
				if( desc->m_str.Right(3) == "BLK" )
				{
					CString Suffix="", Link="";
					int page_num = m_view->m_Doc->ExtractBLK( &desc->m_str, &Suffix, &Link );
					if( page_num >= 0 )
					{
						m_view->m_Doc->SwitchToPage( page_num );
						CString cur_page_name="";
						m_view->m_Doc->Pages.GetPageName( page_num, &cur_page_name );
						int it2 = -1;
						CTextList * rtl = m_view->m_Doc->Pages.GetTextList( page_num, index_part_attr );
						if( rtl == NULL )
							continue;
						CText * LinkT = rtl->GetText( &Link, &it2 );
						if( LinkT )
						{
							RECT partRect;
							partRect = m_view->m_Doc->GetPartBounds( LinkT, NULL );
							it2 = -1;
							for( CText * CPpart=rtl->GetNextText(&it2); CPpart; CPpart=rtl->GetNextText(&it2) )
							{
								if( InRange( CPpart->m_x, partRect.left, partRect.right ) )
									if( InRange( CPpart->m_y, partRect.bottom, partRect.top ) )
									{
										int ind = Find( &CPpart->m_str, &cur_page_name );
										if( ind >= 0 )
											bVisibled[ind] = TRUE;
									}
							}
						}
					}
				}
			}
			m_view->m_Doc->SwitchToPage( old_p );
		}
	}
}

int CDlgPartlist::Find( CString * s, CString * page )
{
	for( int i=0; i<pl[COL_NAME].GetSize(); i++ )
	{
		if( s->Compare( pl[COL_NAME].GetAt(i) ) == 0 )
			if( page->Compare( pl[COL_PAGE].GetAt(i) ) == 0 )
				return i;
	}
	return -1;
}

void CDlgPartlist::ReDraw()
{
	OnFilter();
	DrawListCtrl();
	RestoreSelections();
}

void CDlgPartlist::OnCancel()
{
	GetColWidths();
	OnOK();
}


void CDlgPartlist::OnHighlight()
{
	SaveSelections();
	int prev_np = m_view->m_Doc->Pages.GetActiveNumber();
	int save_np = prev_np;
	int n_sel = 0;
	for( int i=0; i<bSelected.GetSize(); i++ )
	{
		if( bVisibled[i] && bSelected[i] )
			n_sel++;
	}
	m_view->CancelSelection();
	for( int i=0; i<bSelected.GetSize(); i++ )
	{
		if( bVisibled[i] )
		if( bSelected[i] || n_sel == 0 )
		{
			int np = m_pl->GetPageIndex( &pl[COL_PAGE][i] );
			if( np >= 0 )
			{
				if( np != prev_np )
					m_view->m_Doc->SwitchToPage(np);
				CTextList * tl = m_pl->GetTextList(np,index_part_attr);
				if( tl )
				{
					int it = -1;
					CString refstr = pl[COL_NAME][i];
					if( refstr.Right(1) == "." )
						if( refstr.GetLength() > 2 )
							refstr.Truncate( refstr.GetLength()-2 );
					CText * part = tl->GetText( &refstr, &it );
					if( part )
					{
						m_view->m_Doc->SelectPart( part, NULL, 0 );
					}
				}
				prev_np = np;
				

			}
		}
	}

	if( save_np != prev_np )
		m_view->m_Doc->SwitchToPage(save_np, TRUE, 0);
	if( m_view->m_sel_count )
	{
		m_view->SetCursorMode( CUR_GROUP_SELECTED );
		m_view->HighlightGroup();
		m_view->JumpToPart(NULL);
	}
	else if( save_np != prev_np )
	{
		m_view->m_Doc->SwitchToPage(prev_np, TRUE, 0);
		if( m_view->m_sel_count )
		{
			m_view->SetCursorMode( CUR_GROUP_SELECTED );
			m_view->HighlightGroup();
			m_view->JumpToPart(NULL);
		}
	}
	CWnd * MWnd = AfxGetMainWnd();
	if( MWnd )
		MWnd->SetFocus();
	m_view->m_draw_layer = 0;
	m_view->Invalidate(FALSE);
}

void CDlgPartlist::OnChange()
{
	SaveSelections();
	ReDraw();
}

void CDlgPartlist::OnPCBCheck()
{
	SaveSelections();
	ReDraw();
}

void CDlgPartlist::GetColWidths()
{
	int w = m_list_ctrl.GetColumnWidth(COL_NAME);
	w = min(w, 255);
	m_view->m_Doc->m_partlist_col_w = w;
	w = m_list_ctrl.GetColumnWidth(COL_VALUE);
	w = min(w, 255);
	m_view->m_Doc->m_partlist_col_w |= (w<<8);
	w = m_list_ctrl.GetColumnWidth(COL_FOOTPRINT);
	w = min(w, 255);
	m_view->m_Doc->m_partlist_col_w |= (w<<16);
	w = m_list_ctrl.GetColumnWidth(COL_PAGE);
	w = min(w, 255);
	m_view->m_Doc->m_partlist_col_w |= (w<<24);
}

void CDlgPartlist::OnTXT()
{
	CString path = m_view->m_Doc->m_path_to_folder + "\\related_files\\" + m_view->m_Doc->m_name + ".txt";
	CStdioFile File;
	int err = File.Open( LPCSTR(path), CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening file
		CString mess;
		mess.Format( "Unable to open file \"%s\"", path ); 
		AfxMessageBox( mess );
		return;
	}
	else
	{
		int nsel = m_list_ctrl.GetSelectedCount();
		int col[4];
		for( int i=0; i<NUM_COLS; i++ )
			col[i] = 0;
		for( int i=0; i<NUM_COLS; i++ )
			for( int ii=0; ii<m_list_ctrl.GetItemCount(); ii++ )
			{
				if( m_list_ctrl.GetItemState( ii, LVIS_SELECTED ) == LVIS_SELECTED || nsel < 2 )
				{
					CString gS = m_list_ctrl.GetItemText(ii,i);
					col[i] = max( col[i], gS.GetLength() );
				}
			}
		for( int ii=0; ii<m_list_ctrl.GetItemCount(); ii++ )
		{
			if( m_list_ctrl.GetItemState( ii, LVIS_SELECTED ) == LVIS_SELECTED || nsel < 2 )
			{
				for( int i=0; i<NUM_COLS; i++ )
				{
					CString gS = m_list_ctrl.GetItemText(ii,i);
					File.WriteString( gS );
					for( int sp=col[i]+2; sp>gS.GetLength(); sp-- )
						File.WriteString( " " );
				}
				File.WriteString( "\n" );
			}			
		}
		File.Close();
		ShellExecute(	NULL,"open", path, NULL, NULL, SW_SHOWNORMAL );
	}
}

void CDlgPartlist::ImportPartlist()
{
	BOOL EnSel = 0;
	CString str;
	m_flt.GetWindowTextA(str);
	if( str.GetLength()  == 0 || str.Left(6) == "search" )
		EnSel = 1;
	else
		SaveSelections();

	pl[COL_NAME].RemoveAll();
	pl[COL_VALUE].RemoveAll();
	pl[COL_FOOTPRINT].RemoveAll();
	pl[COL_PAGE].RemoveAll();
	if( EnSel )
		bSelected.RemoveAll();
	bVisibled.RemoveAll();
	CString p_name;
	int it = -1;
	int ip = -1;
	int m_ip = -1;
	int ThisNPage = m_pl->GetActiveNumber();
	CArray<CPolyLine> * po = m_pl->GetPolyArray( ThisNPage );
	CString SFX = "";
	if( m_view->m_Doc->Pages.IsThePageIncludesCP( 0 ) >= 0 )
		SFX = "..";
	for( CText * t=m_pl->GetNextAttr(index_part_attr,&it,&ip,0); t; t=m_pl->GetNextAttr(index_part_attr,&it,&ip,0) )
	{
		if( m_ip != ip )
		{
			m_pl->GetPageName(ip,&p_name);
			po = m_pl->GetPolyArray( ip );
			SFX = "";
			if( m_view->m_Doc->Pages.IsThePageIncludesCP( ip ) >= 0 )
				SFX = "..";
		}
		m_ip = ip;
		pl[COL_NAME].Add( t->m_str+SFX );
		pl[COL_PAGE].Add( p_name );
		int st = t->m_polyline_start;
		if( st >= 0 )
		{
			if( EnSel )
				bSelected.Add( t->m_selected || po->GetAt(st).GetSideSel() );
			if( po->GetAt(st).Check( index_value_attr ) )
				pl[COL_VALUE].Add( po->GetAt(st).pAttr[index_value_attr]->m_str );
			else
				pl[COL_VALUE].Add("");
			if( po->GetAt(st).Check( index_foot_attr ) )
				pl[COL_FOOTPRINT].Add( po->GetAt(st).pAttr[index_foot_attr]->m_str );
			else
				pl[COL_FOOTPRINT].Add("");
		}
		else
			ASSERT(0);
	}
	ReDraw();
}

void CDlgPartlist::InvertSelection()
{
	int nItems = m_list_ctrl.GetItemCount();
	for( int iItem=0; iItem<nItems; iItem++ )
		if( m_list_ctrl.GetItemState( iItem, LVIS_SELECTED ) == LVIS_SELECTED )
			m_list_ctrl.SetItemState( iItem, 0, LVIS_SELECTED );
		else
			m_list_ctrl.SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
}

int CDlgPartlist::GetPCBButtonCheck()
{
	return Butt[6].GetCheck();
}

void CDlgPartlist::OnBnClickedSall()
{
	int nItems = m_list_ctrl.GetItemCount();
	for( int iItem=0; iItem<nItems; iItem++ )
		m_list_ctrl.SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
}




void CDlgPartlist::OnBnClickedHpcb()
{
	OnHighlight();
	m_view->m_Doc->SwitchToPCB();
}


void CDlgPartlist::OnEnChangeFilter()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления
}
