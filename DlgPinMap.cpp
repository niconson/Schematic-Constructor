
#include "stdafx.h"
#include "FreeSch.h"
#include "DlgPinMap.h"

extern CFreeasyApp theApp;

// DlgDRC dialog

IMPLEMENT_DYNAMIC(DlgPinMap, CDialog)
DlgPinMap::DlgPinMap(CWnd* pParent /*=NULL*/)
	: CDialog(DlgPinMap::IDD, pParent)
{
	m_BGA = 0;
}

DlgPinMap::~DlgPinMap()
{
}

void DlgPinMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PINLIST, m_list);
	DDX_Control(pDX, IDC_PINFILTER, m_edit);
	DDX_Control(pDX, ID_PINBUTT, m_butt);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		m_list_arr.RemoveAll();
		m_BGA = 0;
		int pin_cnt = 0;
		int bga_cnt = 0;
		int max_bga_num = 0;
		int thisPage = m_pages->GetActiveNumber();
		int ipcb = m_pages->GetCurrentPcbIndex();
		int netlist_page_mask = 0;
		if( ipcb >= 0 )
			netlist_page_mask = m_pages->m_netlist_page_mask[ipcb];
		CString RefDes = m_part->m_str;
		int rf = RefDes.FindOneOf(".-");
		if( rf > 0 )
			RefDes = RefDes.Left(rf);
		//
		for( int iP=0; iP<m_pages->GetNumPages(); iP++ )
		{
			if( getbit( netlist_page_mask, iP ) || iP == thisPage )
			{
				int it = -1;
				for( CText * getPart=m_pages->GetTextList(iP,index_part_attr)->GetNextText( &it ); 
							 getPart;
							 getPart=m_pages->GetTextList(iP,index_part_attr)->GetNextText( &it ))
				{
					CString RefDes2 = getPart->m_str;
					rf = RefDes2.FindOneOf(".-");
					if( rf > 0 )
						RefDes2 = RefDes2.Left(rf);
					if( RefDes.Compare( RefDes2 ) )
						continue;
					m_poly = m_pages->GetPolyArray( iP );
					for( int get=getPart->m_polyline_start; get>=0; get=m_poly->GetAt( get ).Next[index_part_attr] )
					{
						if( CText * pin = m_poly->GetAt(get).Check( index_pin_attr ) )
						{
							CArray<CString> arr;
							arr.RemoveAll(); 
							int bRet = 1;
							if( pin->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
								bRet = ParseMultiPin( &pin->m_str, &arr );
							else		
								arr.Add( pin->m_str );
							if( bRet == 0 )
								pDX->Fail(); 
							for( int ip=0; ip<arr.GetSize(); ip++ )
							{
								pin_cnt++;
								CString prefix="";
								int pin_num = ParsePin( &arr.GetAt(ip), &prefix );
								if( prefix.GetLength() && pin_num )
								{
									bga_cnt++;
									max_bga_num = max(max_bga_num,pin_num);
								}
							}
						}
					}
					if( bga_cnt > pin_cnt/2 )
						m_BGA = TRUE;
					for( int get=getPart->m_polyline_start; get>=0; get=m_poly->GetAt( get ).Next[index_part_attr] )
					{
						if( CText * pin = m_poly->GetAt(get).Check( index_pin_attr ) )
						{
							CArray<CString> arr;
							arr.RemoveAll(); 
							if( pin->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
								ParseMultiPin( &pin->m_str, &arr );
							else		
								arr.Add( pin->m_str );
							for( int iarr=0; iarr<arr.GetSize(); iarr++ )
							{
								CString prefix="";
								int pin_num = ParsePin( &arr[iarr], &prefix );
								CString add_str="";
								add_str.Format( "%d", pin_num );
								if( pin_num == 0 )
								{
									if( prefix.GetLength() )
										add_str = prefix;
									else
										add_str = "0";
								}
								else if( m_BGA )
								{	
									for( int i=add_str.GetLength(); i<2; i++ )
										add_str = "0" + add_str;
									add_str = prefix + add_str;
								}
								else
								{
									if( prefix.GetLength() )
										add_str = arr[iarr];
									else for( int i=add_str.GetLength(); i<4; i++ )
										add_str = "0" + add_str;
								}
								add_str += " ";
								{	// check for multiple pin
									int NC = 5;
									if( m_BGA )
										NC = 4;
									int istr = m_list.FindString(-1,add_str.Left(NC));
									if( istr >= 0 )
									{
										CString multi_pin;
										m_list.GetText( istr, multi_pin );
										if( multi_pin.Left(NC) == add_str.Left(NC) )
											add_str += "- multiple pin ";
									}
								}
								if( m_poly->GetAt(get).GetLayer() != LAY_PIN_LINE )
									add_str += "- not used";
								CText * pin_desc = m_poly->GetAt(get).Check( index_desc_attr );
								CText * pin_net =  m_poly->GetAt(get).Check( index_net_attr );
								if( pin_net )
									add_str += (" (" + pin_net->m_str + ") ");
								if( pin_desc )
									add_str += ("- " + pin_desc->m_str);
								
								add_str.Replace("|","");
								add_str.Replace("'"," | ");
								m_list.AddString( add_str );
							}
						}
					}
				}
			}
		}
		for( int i=0; i<m_list.GetCount(); i++ )
		{
			CString str;
			m_list.GetText(i,str);
			m_list_arr.Add( str );
		}
		if( m_BGA )
		{
			CString sort = SHORTCSORT;
			int sz = m_list_arr.GetSize();
			for( int i=1; i<sz; i++ )
			{
				int DELTA = 0;
				CString s1 = m_list_arr.GetAt(i).Left(3);
				CString s2 = m_list_arr.GetAt(i-1).Left(3);
				CString pref1="", pref2="";
				int i1 = ParsePin( &s1, &pref1 );
				int i2 = ParsePin( &s2, &pref2 );
				int ipref1 = sort.Find( pref1 );
				int ipref2 = sort.Find( pref2 );
				if( ipref1 >= 0 && ipref2 >= 0 )
				{
					int delta = ipref1 - ipref2;
					DELTA = delta*max_bga_num;
					if( delta > 0 || (i1-i2) > 1 )
					{
						DELTA += (i1-i2-1);
					}
				}
				CString new_pref = pref2;
				int new_i = ipref2;
				int num = i2;
				for( int ii=0; ii<DELTA; ii++ )
				{
					num++;
					if( num > max_bga_num )
					{
						num = 1;
						new_i++;
						new_pref = sort.GetAt( new_i );
					}
					CString new_pin;
					new_pin.Format( "%d", num );
					for( int i=new_pin.GetLength(); i<2; i++ )
						new_pin = "0" + new_pin;
					new_pin = new_pref + new_pin + " - is missing";
					m_list_arr.Add( new_pin );
				}
			}
			if( sz )
			{
				int isp = m_list_arr.GetAt(sz-1).Find(" ");
				if( isp <= 0 )
					isp = m_list_arr.GetAt(sz-1).GetLength();
				CString s = m_list_arr.GetAt(sz-1).Left( isp );
				CString pref="";
				int ie = ParsePin( &s, &pref );
				for( int ii=ie+1; ii<=max_bga_num; ii++ )
				{
					CString new_pin;
					new_pin.Format( "%d", ii );
					for( int i=new_pin.GetLength(); i<2; i++ )
						new_pin = "0" + new_pin;
					new_pin = pref + new_pin + " - is missing";
					m_list_arr.Add( new_pin );
				}
			}
		}
		else
		{
			int sz = m_list_arr.GetSize();
			for( int i=1; i<sz; i++ )
			{
				CString s1 = m_list_arr.GetAt(i).Left(4);
				CString s2 = m_list_arr.GetAt(i-1).Left(4);
				int i1 = StrToInt( s1 );
				int i2 = StrToInt( s2 );
				if( (i1-i2) < 100 )
				{
					int num = i2;
					for( int ii=(i1-i2); ii>1; ii-- )
					{
						num++;
						CString new_pin;
						new_pin.Format( "%d", num );
						for( int n=new_pin.GetLength(); n<4; n++ )
							new_pin = "0" + new_pin;
						new_pin += " - is missing";
						m_list_arr.Add( new_pin );
					}
				}
			}
		}
		CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
		if( frm )
		{
			CRect R;
			frm->GetWindowRect(&R);
			SetWindowPos(NULL, R.left+125/*(R.left+R.right)/2-110*/, R.top+90, 220, R.Height()-160, 0 );
			m_edit.SetWindowTextA("search");
		}
		Filter();
	}
	else
	{

	}
}


BEGIN_MESSAGE_MAP(DlgPinMap, CDialog)
	ON_CONTROL(EN_CHANGE, IDC_PINFILTER, OnChange)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_PINLIST, &DlgPinMap::OnLbnSelchangePinlist)
	ON_BN_CLICKED(ID_PINBUTT, &DlgPinMap::OnBnClickedPinbutt)
END_MESSAGE_MAP()


// Initialize
// 
void DlgPinMap::Initialize( CText * part, CPageList * PGS )
{
	m_part = part;
	m_pages = PGS;
}

void DlgPinMap::Filter()
{
	CString str;
	m_edit.GetWindowTextA(str);
	if( str.Left(6) == "search" )
		str = "";
	str.MakeLower();
	while( m_list.GetCount() )
		m_list.DeleteString(0);
	for( int i=0; i<m_list_arr.GetSize(); i++ )
	{
		CString get = m_list_arr.GetAt(i);
		if( get.MakeLower().Find( str ) >= 0 )
		{
			m_list.AddString( m_list_arr.GetAt(i) );
			m_list.SelectString( 0, str );
		}
	}
	//int is = -1;
	//while(1)
	//{
	//	is = m_list.FindString(is,"is missing");
	//	if( is >= 0 )
	//		m_list.SetSel(is,1);
	//	else
	//		break;
	//}
}

void DlgPinMap::OnChange()
{
	Filter();
}

void DlgPinMap::OnSize(UINT nType, int cx, int cy)
{
	if( m_list_arr.GetSize() )
	{
		CRect R;
		this->GetWindowRect(&R);
		m_list.SetWindowPos(NULL,2,2,R.Width()-25,R.Height()-70,0);
		m_butt.SetWindowPos(NULL,cx-77,cy-22,70,18,0);
		m_edit.SetWindowPos(NULL,2,cy-22,cx-84,18,0);
	}
}

void DlgPinMap::OnLbnSelchangePinlist()
{
	// TODO: добавьте свой код обработчика уведомлений
	static CString mem_str = "";
	CString str;
	int sel = m_list.GetCurSel();
	if( sel == -1 )
		return;
	m_list.GetText( sel, str );
	if( str.GetLength() == 0 )
		return;

	CText * f_pin = NULL;
	if( m_BGA )
	{
		int isp = str.Find(" ");
		if( isp > 0 )
			str = str.Left(isp);
		CString pref="";
		int num = ParsePin( &str, &pref );
		if( num )
			str.Format( "%s%d", pref, num );
		else
			str = pref;
	}
	else
	{
		while( str.Left(1) == "0" && str.GetLength() )
			str = str.Right(str.GetLength()-1);
		int pr = str.Find(" ");
		if( pr >= 0 )
			str = str.Left(pr);
	}
	BOOL SAME_NAME = 0;
	if( str.Compare( mem_str ) == 0 )
		SAME_NAME = TRUE;
	mem_str  = str;
	int thisPage = m_pages->GetActiveNumber();
	int ipcb = m_pages->GetCurrentPcbIndex();
	int netlist_page_mask = 0;
	if( ipcb >= 0 )
		netlist_page_mask = m_pages->m_netlist_page_mask[ipcb];
	CString RefDes = m_part->m_str;
	int rf = RefDes.FindOneOf(".-");
	if( rf > 0 )
		RefDes = RefDes.Left(rf);
	BOOL Out = 0;
	BOOL CancelSel[MAX_PAGES];
	for( int i=0; i<MAX_PAGES; i++ )
		CancelSel[i] = 0;
	for( int iP=0; iP<m_pages->GetNumPages(); iP++ )
	{
		BOOL sw = (iP != thisPage || netlist_page_mask == 0 || SAME_NAME == 0);
		if( getbit( netlist_page_mask, iP ) || iP == thisPage )
		{
			int it = -1;
			for( CText * getPart=m_pages->GetTextList(iP,index_part_attr)->GetNextText( &it ); 
						 getPart;
						 getPart=m_pages->GetTextList(iP,index_part_attr)->GetNextText( &it ))
			{
				CString RefDes2 = getPart->m_str;
				rf = RefDes2.FindOneOf(".-");
				if( rf > 0 )
					RefDes2 = RefDes2.Left(rf);
				if( RefDes.Compare( RefDes2 ) )
					continue;
				m_poly = m_pages->GetPolyArray( iP );
				for( int get=getPart->m_polyline_start; get>=0; get=m_poly->GetAt(get).Next[index_part_attr] )
				{
					if( CText * Pin = m_poly->GetAt(get).Check( index_pin_attr ) )
					{
						if( Pin->m_str.Compare( str ) == 0 )
						{	
							f_pin = Pin;
							if( sw )
							{
								if( CancelSel[iP] == 0 )
									theApp.m_Doc->m_view->CancelSelection();
								CancelSel[iP] = TRUE;
								theApp.m_Doc->SwitchToPage( iP, TRUE, FALSE );
								RECT pR = theApp.m_Doc->GetPartBounds( getPart, NULL );
								SwellRect( &pR, (pR.top-pR.bottom)/3 + (10*NM_PER_MM) );
								theApp.m_Doc->m_view->ViewAllElements( &pR );
								theApp.m_Doc->m_view->NewSelectP( getPart, Pin, 0, 15 );
							}
							else if( thisPage == 0 )
								continue;
							else
							{
								Out = TRUE;
								break;
							}
						}
						else if( Pin->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
						{
							CArray<CString> arr;
							ParseMultiPin( &Pin->m_str, &arr );
							for( int iarr=0; iarr<arr.GetSize(); iarr++ )
							{
								if( arr[iarr].Compare( str ) == 0 )
								{
									if( f_pin == NULL )
										theApp.m_Doc->m_view->CancelSelection();
									f_pin = Pin;
									if( sw )
									{
										if( CancelSel[iP] == 0 )
											theApp.m_Doc->m_view->CancelSelection();
										CancelSel[iP] = TRUE;
										theApp.m_Doc->SwitchToPage( iP, TRUE, FALSE );
										RECT pR = theApp.m_Doc->GetPartBounds( getPart, NULL );
										SwellRect( &pR, (pR.top-pR.bottom)/3 + (10*NM_PER_MM) );
										theApp.m_Doc->m_view->ViewAllElements( &pR );
										theApp.m_Doc->m_view->NewSelectP( getPart, Pin, 0, 15 );
									}
									break;
								}
							}
							if( f_pin == Pin && sw == 0 )
							{
								if( thisPage == 0 )
									continue;
								else
								{
									Out = TRUE;
									break;
								}
							}
						}
					}
				}
				if( Out )
					break;
			}
			if( Out )
				break;
		}
	}
	if( f_pin )
	{
		theApp.m_Doc->m_view->HighlightGroup();
	}
	else
	{
		theApp.m_Doc->m_dlist->CancelHighLight();
		theApp.m_Doc->OnRangeCmds( NULL );
	}
}


void DlgPinMap::OnBnClickedPinbutt()
{
	// TODO: добавьте свой код обработчика уведомлений
	CString path = theApp.m_Doc->m_path_to_folder + "\\related_files\\" + theApp.m_Doc->m_name + ".txt";
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
		for( int ii=0; ii<m_list.GetCount(); ii++ )
		{
			CString gS = "";
			m_list.GetText(ii,gS);
			File.WriteString( gS );
			File.WriteString( "\n" );			
		}
		File.Close();
		ShellExecute(	NULL,"open", path, NULL, NULL, SW_SHOWNORMAL );
	}
}
