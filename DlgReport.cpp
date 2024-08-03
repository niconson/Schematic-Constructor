
#include "stdafx.h"
#include "FreeSch.h"
#include "DlgReport.h"
#include "BomInTable.h"

extern CFreeasyApp theApp;
// columns for list
enum {
	COL_VALUE = 0,
	COL_FOOTPRINT,
	COL_NAME,
	COL_COUNT,
	COL_URL,
	COL_DETAILS,
	NUM_COLS
};

CArray<CString> bl[NUM_COLS];

// sort types
enum {
	SORT_UP_V = 0,
	SORT_DOWN_V,
	SORT_UP_F,
	SORT_DOWN_F,
	SORT_UP_N,
	SORT_DOWN_N,
	SORT_UP_C,
	SORT_DOWN_C,
	SORT_UP_U,
	SORT_DOWN_U,
	SORT_UP_D,
	SORT_DOWN_D
};

// global callback function for sorting
// lp1, lp2 are indexes to global arrays above
//===============================================================================================	
int CALLBACK CompareBOM( LPARAM lp1, LPARAM lp2, LPARAM type )
{
	int ret = 0;
	switch( type )
	{
		case SORT_UP_V:
		case SORT_DOWN_V:
			ret = (strcmp( bl[COL_VALUE][lp1], bl[COL_VALUE][lp2] ));
			break;

		case SORT_UP_F:
		case SORT_DOWN_F:
			ret = (strcmp( bl[COL_FOOTPRINT][lp1], bl[COL_FOOTPRINT][lp2] ));
			break;

		case SORT_UP_N:
		case SORT_DOWN_N:
			ret = (strcmp( bl[COL_NAME][lp1], bl[COL_NAME][lp2] ));
			break;
			
		case SORT_UP_C:
		case SORT_DOWN_C:
		{
			int c1 = my_atoi( &bl[COL_COUNT][lp1] );
			int c2 = my_atoi( &bl[COL_COUNT][lp2] );
			if( c1 < c2 )
				ret = 1;
			else if( c1 > c2 )
				ret = -1;
			else
				ret = (strcmp( bl[COL_VALUE][lp1], bl[COL_VALUE][lp2] ));
			break;
		}
			
		case SORT_UP_D:
		case SORT_DOWN_D:
			ret = (strcmp( bl[COL_DETAILS][lp1], bl[COL_DETAILS][lp2] ));
			break;

		case SORT_UP_U:
		case SORT_DOWN_U:
			ret = (strcmp( bl[COL_URL][lp1], bl[COL_URL][lp2] ));
			break;
			
	}
	switch( type )
	{
		case SORT_DOWN_V:
		case SORT_DOWN_F:
		case SORT_DOWN_N:
		case SORT_DOWN_C:
		case SORT_DOWN_D:
		case SORT_DOWN_U:
			ret = -ret;
			break;
	}
	return ret;
}

// CDlgPartlistRep dialog
//===============================================================================================
IMPLEMENT_DYNAMIC(CDlgPartlistRep, CDialog)
CDlgPartlistRep::CDlgPartlistRep(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPartlistRep::IDD, pParent)
{
	m_d_len = 0;
	m_csv = 0;
	m_entire = 0;
	main_page = 0;
}

CDlgPartlistRep::~CDlgPartlistRep()
{
}

void CDlgPartlistRep::SaveBomOptions()
{
	CString str;
	m_det_max.GetWindowTextA( str );
	m_d_len = my_atoi(&str);
	if( m_d_len )
	{
		if( m_d_len < 8 )
			m_d_len = 8;
		if( m_d_len > 60 )
			m_d_len = 60;
	}
	int bits = (1<<8) - 1;
	int save_bits = ((bits<<24) & theApp.m_Doc->m_bom_options);// save options for extended bom
	//
	theApp.m_Doc->m_bom_options = m_ref_list.GetCurSel();
	theApp.m_Doc->m_bom_options = max( theApp.m_Doc->m_bom_options, 0 );
	if( m_ignore.GetCheck() )
		setbit( theApp.m_Doc->m_bom_options, 5 );
	if( m_incl.GetCheck() )
		setbit( theApp.m_Doc->m_bom_options, 6 );
	if( m_excl.GetCheck() )
		setbit( theApp.m_Doc->m_bom_options, 7 );
	theApp.m_Doc->m_bom_options |= (m_d_len<<16);
	theApp.m_Doc->m_bom_options |= save_bits;// restore options for extended bom
}
//===============================================================================================
void CDlgPartlistRep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ENTIRE_PR, m_entire);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
	DDX_Control(pDX, IDC_INCL, m_incl);
	DDX_Control(pDX, IDC_EXCL, m_excl);
	DDX_Control(pDX, IDC_IGNORE, m_ignore);
	DDX_Control(pDX, IDC_REF_LISTS, m_ref_list);
	DDX_Control(pDX, IDC_DT_LEN, m_det_max);
	DDX_Text(pDX, IDC_DT_LEN, m_d_len );
	DDV_MinMaxInt(pDX, m_d_len, 0, 999 );
	if( pDX->m_bSaveAndValidate )
	{
		theApp.m_Doc->SwitchToPage( main_page );
	}
	else
	{
		for( int i=0; i<theApp.m_Doc->m_ref_lists->GetSize(); i++ )
			m_ref_list.AddString( theApp.m_Doc->m_ref_lists->GetStr(i) );
		
		//
		int bits = 1;
		m_ignore.SetCheck( ((bits<<5) & theApp.m_Doc->m_bom_options)>>5 );
		m_incl.SetCheck( ((bits<<6) & theApp.m_Doc->m_bom_options)>>6 );
		if( m_incl.GetCheck() == 0 )
			m_excl.SetCheck( ((bits<<7) & theApp.m_Doc->m_bom_options)>>7 );

		//
		bits = (1<<5) - 1;
		int cur_sel = (bits & theApp.m_Doc->m_bom_options);
		if( cur_sel >= 0 && cur_sel < m_ref_list.GetCount() )
			m_ref_list.SetCurSel(cur_sel);
		else
		{
			m_incl.SetCheck(0);
			m_excl.SetCheck(0);
		}
		bits = (1<<7) - 1;
		int det_max = ( ((bits<<16) & theApp.m_Doc->m_bom_options)>>16 );
		CString s;
		s.Format("%d", det_max);
		m_det_max.SetWindowTextA(s);
		
		//
		ReloadList();
		//
		m_list_ctrl.InsertColumn( COL_VALUE, "Value", LVCFMT_LEFT, 80 );
		m_list_ctrl.InsertColumn( COL_FOOTPRINT, "Footprint", LVCFMT_LEFT, 80 );
		m_list_ctrl.InsertColumn( COL_NAME, "Name", LVCFMT_LEFT, 180 );
		m_list_ctrl.InsertColumn( COL_COUNT, "Count", LVCFMT_LEFT, 25 );
		m_list_ctrl.InsertColumn( COL_DETAILS, "Details", LVCFMT_LEFT, 65 );
		m_list_ctrl.InsertColumn( COL_URL, "URL", LVCFMT_LEFT, 80 );
		//
		DrawListCtrl();
	}
	SaveBomOptions();
}


void CDlgPartlistRep::ReloadList()
{
	theApp.m_Doc->SwitchToPage( main_page );

	BOOL Incl = m_incl.GetCheck();
	BOOL Excl = m_excl.GetCheck();
	BOOL IgnoreWV = m_ignore.GetCheck();
	int index = m_ref_list.GetCurSel();
	index += REF_LIST_INDEX;
	if( index < REF_LIST_INDEX ) 
		index = 0;

	bl[COL_VALUE].SetSize( 0 );
	bl[COL_FOOTPRINT].SetSize( 0 );
	bl[COL_NAME].SetSize( 0 );
	bl[COL_COUNT].SetSize( 0 );
	bl[COL_DETAILS].SetSize( 0 );
	bl[COL_URL].SetSize( 0 );

	//modify m_netlist_page_mask
	int cur_pcb = m_plist->GetCurrentPcbIndex();
	int save_mask[MAX_PAGES];
	save_mask[cur_pcb] = m_plist->m_netlist_page_mask[cur_pcb];
	for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
	{
		if( ipcb == cur_pcb )
			continue;
		save_mask[ipcb] = m_plist->m_netlist_page_mask[ipcb];
		if( m_entire )
		{
			m_plist->m_netlist_page_mask[cur_pcb] |= m_plist->m_netlist_page_mask[ipcb];
			m_plist->m_netlist_page_mask[ipcb] = 0;
		}
	}

	CString V="", P="", D="";
	int cnt = m_plist->FindBOMNextLine(&V, &P, &D, 9999, IgnoreWV, (Incl?index:0), (Excl?index:0), TRUE );
	if( cnt >= 0 )
	{
		do
		{
			if( cnt )
			{
				CString cV=V, cP=P;

				// value
				int sep = cV.Find(CP_LINK);
				if( sep > 0 )
					cV = cV.Left( sep );
				sep = cV.Find("|");
				if( sep > 0 )
					cV.Delete( sep );
				if( cV.GetLength() == 0 )
					cV = "---";
				bl[COL_VALUE].Add(cV);

				// footprint
				bl[COL_FOOTPRINT].Add(cP);

				// count
				CString CNT;
				CNT.Format("%d",cnt);
				bl[COL_COUNT].Add(CNT);

				// details
				bl[COL_DETAILS].Add(D);

				// name
				BomInTable BOM( theApp.m_Doc ) ;
				CString CN = BOM.GetCName( &V, &P );
				if( CN.GetLength() == 0 )
					CN = "---";
				bl[COL_NAME].Add(CN);

				// url
				ExtractComponentName( &cV, &cP );
				CString URL = "";
				int it = -1, ip = -1;
				CN = BOM.GetURL( &cV, &cP, &it, &ip );
				if( CN.GetLength() ) 
				{
					while( CN.GetLength() )
					{
						URL += CN + dSPACE;
						CN = BOM.GetURL( &cV, &cP, &it, &ip );	
					}
				}
				else 
					URL = "---";
				URL = URL.TrimRight();
				bl[COL_URL].Add(URL);
			}
			D = "";
			cnt = m_plist->FindBOMNextLine(&V, &P, &D, 9999, IgnoreWV, (Incl?index:0), (Excl?index:0) );
		}while( cnt >= 0 );
	}
	// restore m_netlist_page_mask
	if( m_entire )
		for( int ipcb=0; ipcb<MAX_PAGES; ipcb++ )
			m_plist->m_netlist_page_mask[ipcb] = save_mask[ipcb];
}

//===============================================================================================
BEGIN_MESSAGE_MAP(CDlgPartlistRep, CDialog)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnLvnColumnClickList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_BN_CLICKED(IDBOM, OnTXT)
	ON_CONTROL(BN_CLICKED, IDC_IGNORE, OnChange)
	///ON_CONTROL(BN_CLICKED, IDC_INCL, OnClick1)
	///ON_CONTROL(BN_CLICKED, IDC_EXCL, OnClick2)
	ON_CBN_SELCHANGE(IDC_REF_LISTS, OnChange)
	ON_CONTROL(EN_CHANGE, IDC_DT_LEN, OnChange)
	ON_BN_CLICKED(IDBOM2, &CDlgPartlistRep::OnBnClickedBom2)
	ON_BN_CLICKED(IDC_ENTIRE_PR, &CDlgPartlistRep::OnBnClickedEntirePr)
	ON_BN_CLICKED(IDC_EXCL, &CDlgPartlistRep::OnClick2)
	ON_BN_CLICKED(IDC_INCL, &CDlgPartlistRep::OnClick1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDlgPartlistRep::OnNMRClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDlgPartlistRep::OnNMDblclkList1)
	ON_CBN_DROPDOWN(IDC_REF_LISTS, &CDlgPartlistRep::OnCbnDropdownRefLists)
	ON_BN_CLICKED(IDCANCEL, &CDlgPartlistRep::OnBnClickedCancel)
END_MESSAGE_MAP()

//===============================================================================================
BOOL CDlgPartlistRep::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sort_type = SORT_UP_V;
	return TRUE;
}

//===============================================================================================
void CDlgPartlistRep::DrawListCtrl()
{
	// now set up listview control
	int nItem;
	LVITEM lvitem;
	CString str;
	DWORD old_style = m_list_ctrl.GetExtendedStyle();
	m_list_ctrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | old_style );
	m_list_ctrl.DeleteAllItems();
	m_n_sel = 0;
	for( int i=0; i<::bl[COL_VALUE].GetSize(); i++ )
	{
		lvitem.mask = LVIF_TEXT | LVIF_PARAM;
		lvitem.pszText = "";
		lvitem.lParam = i;
		nItem = m_list_ctrl.InsertItem( i, "" );
		m_list_ctrl.SetItemData( i, (LPARAM)i );
		///ListView_SetCheckState( m_list_ctrl, nItem, 1 );
		m_list_ctrl.SetItem( i, COL_VALUE, LVIF_TEXT, ::bl[COL_VALUE][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( i, COL_FOOTPRINT, LVIF_TEXT, ::bl[COL_FOOTPRINT][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( i, COL_COUNT, LVIF_TEXT, ::bl[COL_COUNT][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( i, COL_DETAILS, LVIF_TEXT, ::bl[COL_DETAILS][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( i, COL_NAME, LVIF_TEXT, ::bl[COL_NAME][i], 0, 0, 0, 0 );
		m_list_ctrl.SetItem( i, COL_URL, LVIF_TEXT, ::bl[COL_URL][i], 0, 0, 0, 0 );
		if( i < bSelected.GetSize() )
			if( bSelected[i] )
			{
				m_list_ctrl.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				m_n_sel++;
			}
	}
	///m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );	// resort 
	///RestoreSelections();
}
//===============================================================================================
void CDlgPartlistRep::Initialize( CPageList * plist )
{
	m_n_sel = 0;
	m_plist = plist;
	m_sort_type = SORT_UP_V;
	main_page = m_plist->GetActiveNumber();
}


// set m_sort_type based on column clicked and last sort, 
// then sort the list, then save m_last_sort_type = m_sort_type
//===============================================================================================
void CDlgPartlistRep::OnLvnColumnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int column = pNMLV->iSubItem;
	if( column == COL_VALUE )
	{
		if( m_sort_type == SORT_UP_V )
			m_sort_type = SORT_DOWN_V;
		else
			m_sort_type = SORT_UP_V;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	else if( column == COL_FOOTPRINT )
	{
		if( m_sort_type == SORT_UP_F )
			m_sort_type = SORT_DOWN_F;
		else
			m_sort_type = SORT_UP_F;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	else if( column == COL_NAME )
	{
		if( m_sort_type == SORT_UP_N )
			m_sort_type = SORT_DOWN_N;
		else
			m_sort_type = SORT_UP_N;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	else if( column == COL_COUNT )
	{
		if( m_sort_type == SORT_UP_C )
			m_sort_type = SORT_DOWN_C;
		else
			m_sort_type = SORT_UP_C;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	else if( column == COL_DETAILS )
	{
		if( m_sort_type == SORT_UP_D )
			m_sort_type = SORT_DOWN_D;
		else
			m_sort_type = SORT_UP_D;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	else if( column == COL_URL )
	{
		if( m_sort_type == SORT_UP_U )
			m_sort_type = SORT_DOWN_U;
		else
			m_sort_type = SORT_UP_U;
		m_list_ctrl.SortItems( ::CompareBOM, m_sort_type );
	}
	*pResult = 0;
}

//===============================================================================================
void CDlgPartlistRep::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	SaveSelections();
	static int last_n_sel = 0;
	m_n_sel = m_list_ctrl.GetSelectedCount();
	last_n_sel = m_n_sel;
	CString V = m_list_ctrl.GetItemText( pNMItemActivate->iItem, COL_VALUE );
	CString F = m_list_ctrl.GetItemText( pNMItemActivate->iItem, COL_FOOTPRINT );
	CString U = m_list_ctrl.GetItemText( pNMItemActivate->iItem, COL_URL );
	if( V.GetLength() && F.GetLength() )
	{
		BomInTable BOM( theApp.m_Doc ) ;
		CString CN = BOM.GetCName( &V, &F );
		int it = -1, ip = -1; 
		CString URL = BOM.GetURL( &V, &F, &it, &ip );
		if( CN.GetLength() )
		{
			m_list_ctrl.SetItemText( pNMItemActivate->iItem, COL_NAME, CN );
		}
		if( U.GetLength() == 3 && URL.GetLength() )
		{
			m_list_ctrl.SetItemText( pNMItemActivate->iItem, COL_URL, URL );
		}
	}
	*pResult = 0;
}
//===============================================================================================
void CDlgPartlistRep::SaveSelections()
{
	int nItems = m_list_ctrl.GetItemCount();
	bSelected.SetSize( ::bl[COL_VALUE].GetSize() );
	for( int iItem=0; iItem<nItems; iItem++ )
	{
		int ip = m_list_ctrl.GetItemData( iItem );
		if( m_list_ctrl.GetItemState( iItem, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			bSelected[ip] = TRUE;
		}
		else
		{
			bSelected[ip] = FALSE;
		}
	}
}
//===============================================================================================
void CDlgPartlistRep::RestoreSelections()
{

	int nItems = m_list_ctrl.GetItemCount();
	for( int iItem=0; iItem<nItems; iItem++ )
	{
		int ip = m_list_ctrl.GetItemData( iItem );
		if( ip < bSelected.GetSize() )
			if( bSelected[ip] == TRUE )
				m_list_ctrl.SetItemState( iItem, LVIS_SELECTED, LVIS_SELECTED );
	}
	bSelected.SetSize(0);
}

void CDlgPartlistRep::OnTXT()
{
	CString s0 = theApp.m_Doc->m_path_to_folder+"\\related_files\\reports";
	struct _stat buf;
	if( _stat( s0, &buf ) )
	{
		_mkdir( s0 );
		_mkdir( s0+"\\related_files" );
	}
	CString path = s0;

	theApp.m_Doc->SwitchToPage( main_page );
	CString nm = theApp.m_Doc->Pages.GetCurrentPCBName();
	if( m_entire )
		nm = theApp.m_Doc->m_name;
	if( nm.GetLength() > 4 )
		if( nm.Right(4) == ".fpc" || nm.Right(4) == ".cds" )
			nm.Truncate( nm.GetLength() - 4 );

	if( m_csv )
		path += ("\\BOM(" + nm + ").csv");
	else
		path += ("\\BOM(" + nm + ").txt");
	CStdioFile File;
	int err = File.Open( LPCSTR(path), CFile::modeCreate | CFile::modeWrite, NULL );
	if( !err )
	{
		// error opening file
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? 
			"Unable to open file \"%s\"":
			"Невозможно открыть файл \"%s\"", path);
		AfxMessageBox( mess );
		return;
	}
	else
	{
		if( m_list_ctrl.GetSelectedCount() < 2 )
			for( int ii=0; ii<m_list_ctrl.GetItemCount(); ii++ )
				m_list_ctrl.SetItemState( ii, LVIS_SELECTED, LVIS_SELECTED );
		int col[NUM_COLS];
		col[COL_VALUE] = 7;
		col[COL_FOOTPRINT] = 11;
		col[COL_NAME] = 6;
		col[COL_COUNT] = 7;
		col[COL_DETAILS] = 9;
		col[COL_URL] = 5;
		for( int i=0; i<NUM_COLS; i++ )
			for( int ii=0; ii<m_list_ctrl.GetItemCount(); ii++ )
			{
				if( m_list_ctrl.GetItemState( ii, LVIS_SELECTED ) == LVIS_SELECTED )
				{
					CString gS = m_list_ctrl.GetItemText(ii,i);
					col[i] = max( col[i], gS.GetLength() );
				}
			}

		if( !m_csv ) 
		{
			File.WriteString( path );
			File.WriteString( "\n" );
			for( int i=0; i<path.GetLength(); i++ )
				File.WriteString( "-" );
			File.WriteString( "\n" );
		}
		//
		int detailsstart = 0;
		CString Head;
		if( m_csv )
			Head = "Number;";
		else
			Head = "Number  ";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		Head = "Value";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_VALUE]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		Head = "Footprint";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_FOOTPRINT]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		Head = "Name";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_NAME]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		Head = "Count";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_COUNT]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		Head = "URL";
		detailsstart += Head.GetLength();
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_URL]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		Head = "Details";
		File.WriteString(Head);
		if( m_csv )
			File.WriteString(";");
		else for( int sp=col[COL_DETAILS]+2; sp>Head.GetLength(); sp-- )
		{
			detailsstart++;
			File.WriteString( " " );
		}
		File.WriteString("\n");
		//
		for( int i=0; i<=(m_csv?NUM_COLS:detailsstart); i++ )
			if( m_csv )
				File.WriteString(";");
			else
				File.WriteString( "-" );
		File.WriteString( "\n" );
		int num = 0;
		for( int ii=0; ii<m_list_ctrl.GetItemCount(); ii++ )
		{
			if( m_list_ctrl.GetItemState( ii, LVIS_SELECTED ) == LVIS_SELECTED )
			{
				CString s;
				s.Format("%d",++num);
				int L = s.GetLength();
				if( !m_csv ) for( int i=0; i<(4-L); i++ )
					File.WriteString( " " );
				File.WriteString( s );
				if( m_csv )
					File.WriteString(";");
				else 
					File.WriteString( "    " );
				for( int i=0; i<NUM_COLS; i++ )
				{
					CString gS = m_list_ctrl.GetItemText(ii,i);
					/*if( i == COL_DETAILS && gS.GetLength() > m_d_len )
					{
						if( m_d_len == 0 )
							continue;
						for(int r=m_d_len; r<gS.GetLength(); r+=m_d_len )
							for( int a=r; a<gS.GetLength(); a++ )
								if( gS.GetAt(a) == ' ' )
								{
									gS.Delete(a);
									for( int w=0; w<detailsstart; w++ )
										gS.Insert(a," ");
									gS.Insert(a,"\n");
									r += detailsstart;
									break;
								}
					}*/
					File.WriteString( gS );
					if( m_csv )
						File.WriteString(";");
					else for( int sp=(col[i]+2); sp>gS.GetLength(); sp-- )
						File.WriteString( " " );
				}
				File.WriteString( "\n" );
			}			
		}
		File.Close();
		if( m_csv && _stat( theApp.m_Doc->m_app_dir+"\\CSVFileView.exe", &buf ) == 0 )
			ShellExecute(	NULL,"open", "\""+theApp.m_Doc->m_app_dir+"\\CSVFileView.exe\"", "\""+path+"\"", NULL, SW_SHOWNORMAL );
		else
			ShellExecute(	NULL,"open", "\""+path+"\"", NULL, NULL, SW_SHOWNORMAL );
	}
	m_csv = 0;
}

void CDlgPartlistRep::OnChange()
{
	ReloadList();
	DrawListCtrl();
	SaveBomOptions();
}

void CDlgPartlistRep::OnClick1()
{
	m_excl.SetCheck(0);
	OnChange();
}

void CDlgPartlistRep::OnClick2()
{
	m_incl.SetCheck(0);
	OnChange();
}

void CDlgPartlistRep::OnBnClickedBom2()
{
	m_csv = 1;
	OnTXT();
}


void CDlgPartlistRep::OnBnClickedEntirePr()
{
	m_entire = !m_entire;
	OnChange();
}


void CDlgPartlistRep::OnBnClickedExcl()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDlgPartlistRep::OnBnClickedIncl()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CDlgPartlistRep::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: добавьте свой код обработчика уведомлений
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CONTEXT));
	CMenu* pPopup;
	pPopup = menu.GetSubMenu(7);
	ASSERT(pPopup != NULL);
	CString V = m_list_ctrl.GetItemText( pNMItemActivate->iItem, COL_VALUE );
	CString F = m_list_ctrl.GetItemText( pNMItemActivate->iItem, COL_FOOTPRINT );
	{
		theApp.m_Doc->m_view->OnGroupCancelSel();
		CText * FOUND = NULL;
		int it=-1, ip=0; 
		for( CText *txt = m_plist->GetNextAttr( index_foot_attr, &it, &ip );
			 txt;
			 txt = m_plist->GetNextAttr( index_foot_attr, &it, &ip ) )
		{
			if( m_entire == 0 )
				if( m_plist->GetPcbIndex(ip) !=  m_plist->GetPcbIndex(main_page) )
					continue;
			CArray<CPolyLine> * po = m_plist->GetPolyArray( ip );
			CText *  gV = po->GetAt( txt->m_polyline_start ).Check( index_value_attr );
			CText *  gF = po->GetAt( txt->m_polyline_start ).Check( index_foot_attr );
			CText *  gR = po->GetAt( txt->m_polyline_start ).Check( index_part_attr );
			if( gF && gR )
			{
				CString gvT = "no value";
				if( gV )
					gvT = gV->m_str;
				gvT.Replace("|","");
				if( V.CompareNoCase( gvT ) == 0 &&
					F.CompareNoCase( gF->m_str ) == 0 )
				{
					FOUND = gR;
					if( ip != m_plist->GetActiveNumber() )
						theApp.m_Doc->SwitchToPage( ip );
					theApp.m_Doc->m_view->NewSelectP( FOUND, NULL );
				}
			}
		}
		if( FOUND && m_list_ctrl.GetSelectedCount() == 1 )
		{
			if( V.CompareNoCase("no value") == 0 )
			{
				pPopup->EnableMenuItem( ID_COMPONENT_NOTE, MF_GRAYED );
				pPopup->EnableMenuItem( ID_COMPONENT_URL, MF_GRAYED );
				pPopup->EnableMenuItem( ID_COMPONENT_IMAGE, MF_GRAYED );
			}

			POINT point;
			GetCursorPos( &point );
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x+10, point.y, this );
		}
	}
	*pResult = 0;
}


void CDlgPartlistRep::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: добавьте свой код обработчика уведомлений
	CString str = m_list_ctrl.GetItemText( pNMItemActivate->iItem, pNMItemActivate->iSubItem );
	HANDLE cmd = GlobalAlloc(GMEM_FIXED,2048);
	memcpy( cmd, str, str.GetLength()+1 );
	do Sleep(100);  
		while(!OpenClipboard());
	EmptyClipboard();
	SetClipboardData(CF_TEXT,cmd);
	CloseClipboard();
	CString mess;
	mess.Format(G_LANGUAGE == 0 ? 
		"The text %s has been copied to the clipboard":
		"Текст %s был скопирован в буфер обмена.", str);
	AfxMessageBox( mess );
	*pResult = 0;
}


void CDlgPartlistRep::OnCbnDropdownRefLists()
{
	// TODO: добавьте свой код обработчика уведомлений
	while( m_ref_list.GetCount() )
		m_ref_list.DeleteString(0);
	for( int i=0; i<theApp.m_Doc->m_ref_lists->GetSize(); i++ )
		m_ref_list.AddString( theApp.m_Doc->m_ref_lists->GetStr(i) );
}


void CDlgPartlistRep::OnBnClickedCancel()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialog::OnCancel();
	theApp.m_Doc->SwitchToPage( main_page );
}
