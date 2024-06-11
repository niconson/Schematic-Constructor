

#include "stdafx.h"
#include "FreeSch.h"
#include "CCompare.h"
#include "ImportNetlist.h"

int Reverse = 0;

CCompare::CCompare()
{
	
}

CCompare::~CCompare()
{

}

void CCompare::Compare( CFreePcbDoc * doc, BOOL bNetlist )
{
	if( doc->m_view->m_dlg_compare == NULL )
	{
		doc->m_view->m_dlg_compare = new CDlgCompare;
		doc->m_view->m_dlg_compare->m_doc = doc;	
		doc->m_view->m_dlg_compare->CNETLIST = bNetlist;
		doc->m_view->m_dlg_compare->Create( IDD_COMPARE );	
	}
	if( doc->m_view->m_dlg_compare->DLG_CREATED )
	{
		doc->m_view->m_dlg_compare->ShowWindow( SW_SHOW );
		doc->m_view->m_dlg_compare->BringWindowToTop();
	}
	else
	{
		delete doc->m_view->m_dlg_compare;
		doc->m_view->m_dlg_compare = NULL;
	}
}

int CDlgCompare::ComparePartlist( int Pg1, int Pg2 )
{
	//
	//
	while( report1.GetCount() )
		report1.DeleteString(0);
	while( report2.GetCount() )
		report2.DeleteString(0);
	while( report3.GetCount() )
		report3.DeleteString(0);
	while( report4.GetCount() )
		report4.DeleteString(0);
	while( report6.GetCount() )
		report6.DeleteString(0);
	DATA1.RemoveAll();
	DATA2.RemoveAll();
	DATA3.RemoveAll();
	DATA4.RemoveAll();
	DATA6.RemoveAll();
	//
	CArray<CPolyLine> * polyarr_1 = m_doc->Pages.GetPolyArray( Pg1 );
	CArray<CPolyLine> * polyarr_2 = m_doc->Pages.GetPolyArray( Pg2 );
	CTextList * Reflist_1 = m_doc->Pages.GetTextList( Pg1, index_part_attr );
	CTextList * Reflist_2 = m_doc->Pages.GetTextList( Pg2, index_part_attr );
	CTextList * V_list_1 = m_doc->Pages.GetTextList( Pg1, index_value_attr );
	CTextList * V_list_2 = m_doc->Pages.GetTextList( Pg2, index_value_attr );
	CTextList * F_list_1 = m_doc->Pages.GetTextList( Pg1, index_foot_attr );
	CTextList * F_list_2 = m_doc->Pages.GetTextList( Pg2, index_foot_attr );
	//
	CString str;
	int it1 = -1;
	for( CText * t1=Reflist_1->GetNextText(&it1); t1; t1=Reflist_1->GetNextText(&it1) )
	{
		int it2 = -1;
		CText * t2 = NULL;
		if( BY_COORDINATES == 0 )
			t2 = Reflist_2->GetText( &t1->m_str, &it2 );
		else if( t1->m_polyline_start >= 0 )
		{
			if( int nc = polyarr_1->GetAt( t1->m_polyline_start ).GetNumCorners() )
			{
				int x = polyarr_1->GetAt( t1->m_polyline_start ).GetX(0);
				int y = polyarr_1->GetAt( t1->m_polyline_start ).GetY(0);
				t2 = FindPartByCoordinates( polyarr_2, x, y, &t1->m_str, nc );
				if( t2 )
				{
					DATA6.Add( (t1->m_str + t2->m_str) );
				}
			}
		}
		if( t2 )
		{
			CString v1 = "no value";
			CString v2 = "no value";
			CString f1 = "no footprint";
			CString f2 = "no footprint";
			if( t1->m_polyline_start >= 0 )
			{
				if( CText * V = polyarr_1->GetAt( t1->m_polyline_start ).Check( index_value_attr ) )
					v1 = V->m_str;
				if( CText * F = polyarr_1->GetAt( t1->m_polyline_start ).Check( index_foot_attr ) )
					f1 = F->m_str;
			}
			if( t2->m_polyline_start >= 0 )
			{
				if( CText * V = polyarr_2->GetAt( t2->m_polyline_start ).Check( index_value_attr ) )
					v2 = V->m_str;
				if( CText * F = polyarr_2->GetAt( t2->m_polyline_start ).Check( index_foot_attr ) )
					f2 = F->m_str;
			}
			int cmpv = v1.Compare( v2 );
			int cmpf = f1.Compare( f2 );
			if( cmpv )
			{
				str.Format("%s", t1->m_str );
				report2.AddString(str);
				str.Format("%s and %s", v1, v2 );
				DATA2.Add(str);
			}
			if( cmpf )
			{
				str.Format("%s", t1->m_str );
				report3.AddString(str);
				str.Format("%s and %s", f1, f2 );
				DATA3.Add(str);
			}
		}
		else
		{
			str.Format("%s", t1->m_str );
			report1.AddString(str);
			DATA1.Add(str);
		}
	}
	it1 = -1;
	for( CText * t1=Reflist_2->GetNextText(&it1); t1; t1=Reflist_2->GetNextText(&it1) )
	{
		int it2 = -1;
		CText * t2 = NULL;
		if( BY_COORDINATES == 0 )
			t2 = Reflist_1->GetText( &t1->m_str, &it2 );
		else if( t1->m_polyline_start >= 0 )
		{
			if( int nc = polyarr_2->GetAt( t1->m_polyline_start ).GetNumCorners() )
			{
				int x = polyarr_2->GetAt( t1->m_polyline_start ).GetX(0);
				int y = polyarr_2->GetAt( t1->m_polyline_start ).GetY(0);
				t2 = FindPartByCoordinates( polyarr_1, x, y, &t1->m_str, nc );
			}
		}
		if( t2 == NULL )
		{
			str.Format("%s", t1->m_str );
			report4.AddString(str);
			DATA4.Add(str);
		}
	}
	BOOL RET = (DATA1.GetSize() || DATA2.GetSize() || DATA3.GetSize() || DATA4.GetSize());
	return RET;
}

int CDlgCompare::CompareNetlist( int Pg1, int Pg2 )
{
	while( report5.GetCount() )
		report5.DeleteString(0);
	DATA5.RemoveAll();
	m_doc->m_netlist_created[Pg1] = 0;
	m_doc->m_netlist_created[Pg2] = 0;
	int old_page = m_doc->Pages.GetActiveNumber();
	if( m_doc->m_netlist_created[Pg1] == 0 )
	{
		m_doc->SwitchToPage(Pg1);
		m_doc->CreatePCBNets();
	}
	if( m_doc->m_netlist_created[Pg2] == 0 )
	{
		m_doc->SwitchToPage(Pg2);
		m_doc->CreatePCBNets();
	}
	if( m_doc->Pages.GetActiveNumber() != old_page )
		m_doc->SwitchToPage( old_page );
	m_doc->m_dlg_log->OnShowMe();
	//
	CArray<CPolyLine> * polyarr_1 = m_doc->Pages.GetPolyArray( Pg1 );
	CArray<CPolyLine> * polyarr_2 = m_doc->Pages.GetPolyArray( Pg2 );
	CTextList * Reflist_1 = m_doc->Pages.GetTextList( Pg1, index_part_attr );
	CTextList * Reflist_2 = m_doc->Pages.GetTextList( Pg2, index_part_attr );
	CTextList * Pinlist_1 = m_doc->Pages.GetTextList( Pg1, index_pin_attr );
	CTextList * Pinlist_2 = m_doc->Pages.GetTextList( Pg2, index_pin_attr );
	//
	CString str;//, mem1="", mem2="";
	int ntexts = Reflist_1->GetNumTexts();
	int it1 = -1;
	for( CText * t1=Reflist_1->GetNextText(&it1); t1; t1=Reflist_1->GetNextText(&it1) )
	{
		{
			CMainFrame * pMainFr = (CMainFrame*)AfxGetMainWnd();
			if( pMainFr && (it1 == (ntexts-1) || it1%10 == 9) )
			{
				str.Format( "Text %d from %d", it1+1, ntexts );
				pMainFr->DrawStatus( 3, &str );
			}
		}
		int it2 = -1;
		CText * t2 = NULL;
		if( BY_COORDINATES == 0 )
			t2 = Reflist_2->GetText( &t1->m_str, &it2 );
		else if( t1->m_polyline_start >= 0 )
		{
			if( int nc = polyarr_1->GetAt( t1->m_polyline_start ).GetNumCorners() )
			{
				int x = polyarr_1->GetAt( t1->m_polyline_start ).GetX(0);
				int y = polyarr_1->GetAt( t1->m_polyline_start ).GetY(0);
				t2 = FindPartByCoordinates( polyarr_2, x, y, &t1->m_str, nc );
			}
		}
		int NumPins = m_doc->GetNumAllPins( t1, polyarr_1 );
		int SWAP_WORKED = 0;
		CPolyLine * POLY1 = NULL;
		for( int get=t1->m_polyline_start; get>=0; get=POLY1->Next[index_part_attr] )
		{
			POLY1 = &polyarr_1->GetAt( get );
			if( CText * PIN1=POLY1->Check( index_pin_attr ) )
			{
				CString net1 = "no_net";
				CString pins1="", new_net1="", NControl1="";
				if( CText * NET1=POLY1->Check( index_net_attr ) )
					net1 = NET1->m_str;
				//
				m_doc->Pages.GetNetPins( &net1, &pins1, NL_PADS_PCB, &new_net1, &NControl1, Pg1 );
				CArray<CString> arrPIN1;
				if( PIN1->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
					ParseMultiPin( &PIN1->m_str, &arrPIN1 );
				else
					arrPIN1.Add( PIN1->m_str );
				//
				CString TRY_TO_SWAP = "";
				int LIM1prev = 0;
				int LIM2prev = 0;
				int OKprev = INT_MAX;
				for( int iar=0; iar<arrPIN1.GetSize(); iar++ )
				{
					CString net2 = "no_net";
					CString pins2="", new_net2="", NControl2="";
					CString pinname2 = arrPIN1.GetAt(iar);
					if( t2 )
					{
						if( TRY_TO_SWAP.GetLength() )
						{
							if( pinname2.Compare("1") == 0 )
								pinname2 = "2";
							else if( pinname2.Compare("2") == 0 )
								pinname2 = "1";
						}
						CPolyLine * POLY2 = FindPin( t2, &pinname2, polyarr_2 );
						if( POLY2 )
						{
							if( CText * NET2=POLY2->Check( index_net_attr ) )
								net2 = NET2->m_str;
							m_doc->SwitchToPage( Pg2 );
							m_doc->Pages.GetNetPins( &net2, &pins2, NL_PADS_PCB, &new_net2, &NControl2, Pg2 );
							m_doc->SwitchToPage( old_page );
						}
					}
					else
						continue; // no part
					//
					if( net1.Compare( "no_net" ) == 0 && net2.Compare( "no_net" ) == 0 )
						continue;
					pins1 = "pins1: " + pins1;
					pins2 = "pins2: " + pins2;
					pins1.Replace("\n", " ");
					pins2.Replace("\n", " ");
					CArray<CString> arr1;
					CArray<CString> arr2;
					CString key1, key2;
					ParseKeyString( &pins1, &key1, &arr1 );
					ParseKeyString( &pins2, &key2, &arr2 );
					int OK = 0;
					int LIM1 = arr1.GetSize();
					int LIM2 = arr2.GetSize();
					if( LIM1 <= 1 && LIM2 <= 1 )
						continue;
					for( int i1=0; i1<LIM1; i1++ )
					{
						key1 = arr1.GetAt(i1);
						if( key1.Compare( t1->m_str + "." + arrPIN1.GetAt(iar) ) == 0 )
						{
							if( TRY_TO_SWAP.GetLength() )
								key1 = t1->m_str + "." + pinname2;
						}
						for( int i2=0; i2<LIM2 ; i2++ )
						{
							key2 = arr2.GetAt(i2);
							if( BY_COORDINATES == 0 )
							{
								if( key1.Compare( key2 ) == 0 )
								{
									OK++;
									break;
								}
								else if(( key2.Right(2) == ".1" || key2.Right(2) == ".2" ) && TRY_TO_SWAP.GetLength() )
								{
									CString FindStr = key2.Left( key2.GetLength() - 2 );
									CText * gPart = Reflist_2->GetText( &FindStr );
									if( gPart )
									{
										if( m_doc->GetNumAllPins( gPart, polyarr_2 ) == 2 )
										{
											if( key2.Right(2) == ".1" )
												key2.Replace(".1",".2");
											else if( key2.Right(2) == ".2" )
												key2.Replace(".2",".1");
											if( key1.Compare( key2 ) == 0 )
											{
												OK++;
												break;
											}
										}
									}
								}
							}
							else
							{
								if( Conformity( &key1, &key2 ) )
								{
									OK++;
									break;
								}
							}
						}
						if( OK >= 2 )
							break;
					}
					// check swapping
					BOOL swappingWorked = (OK - LIM1 + LIM1prev >= OKprev);// && LIM1-OK <= LIM1prev-OKprev+1); 
					if( swappingWorked )
					{
						SWAP_WORKED++;
						break;
					}
					// cancel swapping
					if( TRY_TO_SWAP.GetLength() )
					{
						LIM1 = LIM1prev;
						LIM2 = LIM2prev;
						LIM2prev = 0;
						OK = OKprev;
						OKprev = INT_MAX;
						net2 = TRY_TO_SWAP; // return old net name for report
						TRY_TO_SWAP = "";
					}
					else if( NumPins == 2 && TRY_TO_SWAP.GetLength() == 0 && arrPIN1.GetSize() == 1 ) // try to swap
					{
						// Try to swap the contacts 
						// of two-pin parts to fit 
						// the netlist of the circuit 
						// to the netlist of the 
						// printed circuit board
						if( arrPIN1.GetAt(iar).Compare("1") == 0 || arrPIN1.GetAt(iar).Compare("2") == 0 )
						{
							LIM1prev = LIM1;
							LIM2prev = LIM2;
							OKprev = OK;
							TRY_TO_SWAP = net2;  // save this net name for report
							iar--;
							continue;
						}
					}
					// checking number of pins
					if( OK < 2 ) 
					{	
						str.Format("%s.%s", t1->m_str, arrPIN1.GetAt(iar) );
						report5.AddString(str);
						str.Format("%s and %s", net1, net2 );
						DATA5.Add(str);
					}
					else if( net1.Compare( net2 ) )
					{
						int ch1 = report5.FindStringExact( -1, t1->m_str+"."+arrPIN1.GetAt(iar) );
						int ch2 = report5.FindStringExact( -1, t2->m_str+"."+arrPIN1.GetAt(iar) );
						if( ch1 == -1 && ch2 == -1 )
						{
							//if(( LIM1 <= (LIM2 - 2) ) || ( LIM2 <= (LIM1 - 2) ))
							if( LIM1 != LIM2 )
							{
								int fs = report5.FindStringExact(-1,net1);
								if( fs >= 0 )
								{
									CString s2 = DATA5.GetAt( fs );
									s2 = s2.Left( s2.GetLength()-1 );
									str.Format("%s, %s.%s)", s2, t1->m_str, arrPIN1.GetAt(iar) );
									DATA5.SetAt( fs, str );
								}
								else
								{
									report5.AddString(net1);
									str.Format("%s (%s) has been changed (%s.%s)", net1, net2, t1->m_str, arrPIN1.GetAt(iar) );
									DATA5.Add(str);
								}
							}
						}
						BOOL F1=0, F2=0;
						for( int r=0; r<DATA5.GetSize(); r++ )
						{
							CString TxT = DATA5.GetAt(r);
							TxT.Replace("("," ");
							TxT.Replace(")"," ");
							TxT.Replace(","," ");
							if( TxT.Find( " "+t1->m_str+".1 " ) >= 0 )
								F1 = 1;
							else if( TxT.Find( " "+t1->m_str+".2 " ) >= 0 )
								F2 = 1;
							if( F1 && F2 )
								break;
						}
						if( F1 && F2 && NumPins == 2 )
						{
							int fs = report6.FindStringExact(-1,t1->m_str);
							if( fs == -1 )
								report6.AddString( t1->m_str );
						}
					}
				}
			}
		}
		// pin names ?
		if( SWAP_WORKED == 1 )
		{	
			int fs = report5.FindString(-1,t1->m_str+".");
			if( fs == -1 )
			{
				str.Format("%s.?", t1->m_str );
				report5.AddString(str);
				str.Format("The pins may have been renumbered");
				DATA5.Add(str);
			}
		}
		else if( SWAP_WORKED > 1 )
		{
			int fs = report6.FindStringExact(-1,t1->m_str);
			if( fs == -1 )
				report6.AddString( t1->m_str );
		}
	}
	return DATA5.GetSize();
}

BOOL CDlgCompare::DATA5_FindNet( CString * net )
{
	for( int i=0; i<DATA5.GetSize(); i++ )
		if( DATA5.GetAt(i).Find( *net+" " ) >= 0 )
			return 0;
		else if( DATA5.GetAt(i).Find( " "+*net ) >= 0 )
			return 0;
	return 1;
}

int CDlgCompare::Conformity( CString * s1, CString * s2 )
{
	int rf1 = s1->ReverseFind('.');
	int rf2 = s2->ReverseFind('.');
	// 
	if( s1->Left(1) != s2->Left(1) )
		return FALSE;
	if( rf1 > 0 && rf2 > 0 )
		if( s1->Right( s1->GetLength() - rf1 ) != s2->Right( s2->GetLength() - rf2 ) )
			return FALSE;
	// END TEST
	CString str="";
	if( rf1 > 0 )
		str += s1->Left( rf1 );
	else
		str += *s1;
	if( rf2 > 0 )
		str += s2->Left( rf2 );
	else
		str += *s2;
	//
	for( int i=0; i<DATA6.GetSize(); i++ )
	{
		if( str.Compare( DATA6.GetAt(i) ) == 0 )
			return TRUE;
	}
	return FALSE;
}

CPolyLine * CDlgCompare::FindPin( CText * part, CString * pin, CArray<CPolyLine> * po )
{
	CPolyLine * POLY = NULL;
	for( int get=part->m_polyline_start; get>=0; get=POLY->Next[index_part_attr] )
	{
		POLY = &po->GetAt( get );
		if( CText * PIN=POLY->Check( index_pin_attr ) )
		{
			if( pin->Compare( PIN->m_str ) == 0 )
				return POLY;
			//
			if( PIN->m_str.FindOneOf( MULTIPIN_TEST ) > 0 )
			{
				CArray<CString> arrPIN;
				ParseMultiPin( &PIN->m_str, &arrPIN );
				for( int iar=0; iar<arrPIN.GetSize(); iar++ )
					if( pin->Compare( arrPIN.GetAt(iar) ) == 0 )
						return POLY;
			}
		}
	}
	return NULL;
}

CText * CDlgCompare::FindPartByCoordinates( CArray<CPolyLine> * po, int x, int y, CString * compRef, int numCorners )
{
	CText * RET = NULL;
	int d = NM_PER_MM*5;
	int minDis = INT_MAX;
	for( int i=0; i<po->GetSize(); i++ )
	{
		if( po->GetAt(i).m_visible == 0 )
			continue;
		if( po->GetAt(i).GetNumCorners() != numCorners )
			continue;
		if( CText * Ref = po->GetAt(i).Check( index_part_attr ) )
		{
			if( Ref->m_polyline_start == i && po->GetAt(i).GetNumCorners() )
			{
				if( compRef->Left(1).Compare( Ref->m_str.Left(1) ) )
					continue;
				int xx = po->GetAt(i).GetX(0);
				int yy = po->GetAt(i).GetY(0);
				if( InRange( xx, x-d, x+d ) || InRange( x, xx-d, xx+d ) )
					if( InRange( yy, y-d, y+d ) || InRange( y, yy-d, yy+d ) )
					{
						int Dis = Distance( x,y,xx,yy );
						if( Dis < minDis )
						{
							minDis = Dis;
							RET = Ref;
						}
					}
			}
		}
	}
	return RET;
}

//=============================================================================================

IMPLEMENT_DYNAMIC(CDlgCompare, CDialog)
CDlgCompare::CDlgCompare(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCompare::IDD, pParent)
{
	IP1 = 0;
	IP2 = 0;
	CNETLIST = 0;
	BY_COORDINATES = 0;
	DLG_CREATED = FALSE;
	m_netlist_name = "";
	m_netlist_path = "";
}

BEGIN_MESSAGE_MAP(CDlgCompare, CDialog)
	ON_BN_CLICKED(IDCOMPARE, &CDlgCompare::OnBnClickedCompare)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_1, &CDlgCompare::OnLbnSelchangeListDiff1)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_2, &CDlgCompare::OnLbnSelchangeListDiff2)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_3, &CDlgCompare::OnLbnSelchangeListDiff3)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_4, &CDlgCompare::OnLbnSelchangeListDiff4)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_5, &CDlgCompare::OnLbnSelchangeListDiff5)
	ON_LBN_SELCHANGE(IDC_LIST_DIFF_6, &CDlgCompare::OnLbnSelchangeListDiff6)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_1, &CDlgCompare::OnAddedDblClick)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_2, &CDlgCompare::OnValueDblClick)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_3, &CDlgCompare::OnFootprintDblClick)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_4, &CDlgCompare::OnDeletedDblClick)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_5, &CDlgCompare::OnPinsNetsDblClick)
	ON_LBN_DBLCLK(IDC_LIST_DIFF_6, &CDlgCompare::OnSwapPinsDblClick)
	ON_BN_CLICKED(IDC_CH_PARTS, &CDlgCompare::OnBnClickedChParts)
	ON_BN_CLICKED(IDC_CH_NETS, &CDlgCompare::OnBnClickedChNets)
	ON_BN_CLICKED(ID_SELECT1, &CDlgCompare::OnBnClickedSelect1)
	ON_BN_CLICKED(ID_SELECT2, &CDlgCompare::OnBnClickedSelect2)
	ON_BN_CLICKED(ID_SELECT3, &CDlgCompare::OnBnClickedSelect3)
	ON_BN_CLICKED(ID_SELECT4, &CDlgCompare::OnBnClickedSelect4)
	ON_BN_CLICKED(ID_REPAIR_ALL, &CDlgCompare::OnBnClickedRepairAll)
	ON_BN_CLICKED(ID_SEL_NL2, &CDlgCompare::OnBnClickedSelNl2)
END_MESSAGE_MAP()

CDlgCompare::~CDlgCompare()
{
}

void CDlgCompare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PAGE_1, mc_page1);
	DDX_Control(pDX, IDC_COMBO_PAGE_2, mc_page2);
	DDX_Control(pDX, IDC_CH_PARTS, mb_by_ref);
	DDX_Control(pDX, IDC_CH_NETS, mb_by_pos);
	DDX_Control(pDX, ID_REPAIR_ALL, mb_swap_all_pins);
	DDX_Control(pDX, IDC_LIST_DIFF_1, report1);
	DDX_Control(pDX, IDC_LIST_DIFF_2, report2);
	DDX_Control(pDX, IDC_LIST_DIFF_3, report3);
	DDX_Control(pDX, IDC_LIST_DIFF_4, report4);
	DDX_Control(pDX, IDC_LIST_DIFF_5, report5);
	DDX_Control(pDX, IDC_LIST_DIFF_6, report6);
	DDX_Control(pDX, IDC_GROUP_BOARD, Group);
	if( pDX->m_bSaveAndValidate )
	{
		m_doc->Pages.DeleteVirtualPages();
	}
	else
	{
		if( m_doc->Pages.GetNumPages() < 2 && CNETLIST == 0 )
		{
			AfxMessageBox("This project has only one page");
			pDX->Fail();
		}
		else
			DLG_CREATED = TRUE;
		if( CNETLIST )
		{
			mb_by_pos.EnableWindow(0);
			SetWindowTextA("Compare Netlist");
			Group.SetWindowTextA("Net list file names");
			mc_page1.EnableWindow(0);
			mc_page2.EnableWindow(0);

			// set page 1
			IP1 = m_doc->Pages.GetActiveNumber();
			int cur_p = IP1;
			while( m_doc->Pages.GetPcbIndex( cur_p ) == m_doc->Pages.GetPcbIndex( IP1 ) )
			{
				cur_p--;
				if( cur_p < 0 )
					break;
			}
			cur_p++;
			IP1 = cur_p;
			if( SelectNetlist() == 0 )
			{
				CNETLIST = FALSE;
				DLG_CREATED = FALSE;
				pDX->Fail();
			}
		}
		mb_by_ref.SetCheck(1);
		mb_by_pos.SetCheck(0);
		CString p,p1,p2;
		int anum = m_doc->Pages.GetActiveNumber();
		//
		int anum2;
		if( anum == 0 )
			anum2 = 1;
		else
			anum2 = anum-1;
		//
		ReloadPages();// after DLG_CREATED = TRUE !
		mc_page1.SetCurSel( anum );
		mc_page2.SetCurSel( anum2 );
		//
		CenterWindow();
	}
}

void CDlgCompare::OnBnClickedCompare()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( mb_by_ref.GetCheck() )
		BY_COORDINATES = 0;
	else
		BY_COORDINATES = 1;
	int ret = 0;
	if( CNETLIST )
	{
		if( Reverse )
		{
			m_doc->Pages.SwitchToVP1();
			IP2 = m_doc->Pages.GetActiveNumber(); // virtual page 1
			m_doc->Pages.SwitchToVP2();
			ret |= ComparePartlist( m_doc->Pages.GetActiveNumber(), IP2 );
			ret |= CompareNetlist( m_doc->Pages.GetActiveNumber(), IP2 );
			m_doc->SwitchToPage( IP1, TRUE ); // restore number
		}
		else
		{	
			// normal cmp
			m_doc->Pages.SwitchToVP2();
			IP2 = m_doc->Pages.GetActiveNumber(); // virtual page 2
			m_doc->Pages.SwitchToVP1();
			ret |= ComparePartlist( m_doc->Pages.GetActiveNumber(), IP2 );
			ret |= CompareNetlist( m_doc->Pages.GetActiveNumber(), IP2 );
			m_doc->SwitchToPage( IP1, TRUE ); // restore number
		}
		Reverse = !Reverse;
		CWnd * C = GetDlgItem( IDCOMPARE );
		if( C )
			if( Reverse )
				C->SetWindowTextA("Swap...");
			else
				C->SetWindowTextA("Compare");
	}
	else
	{
		IP1 = mc_page1.GetCurSel();
		IP2 = mc_page2.GetCurSel();
		m_doc->SwitchToPage( IP1, TRUE );
		ret |= ComparePartlist( IP1, IP2 );
		ret |= CompareNetlist( IP1, IP2 );
	}
	int it = -1;
	if( ret == 0 && m_doc->Attr->m_Reflist->GetNextText( &it ) )
	{
		AfxMessageBox("These pages are completely identical.");
	}
}

void CDlgCompare::OnLbnSelchangeListDiff1()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report1.GetCurSel();
	SetWindowText( DATA1.GetAt(isel) );
}

void CDlgCompare::OnLbnSelchangeListDiff2()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report2.GetCurSel();
	SetWindowText( DATA2.GetAt(isel) );
}


void CDlgCompare::OnLbnSelchangeListDiff3()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report3.GetCurSel();
	SetWindowText( DATA3.GetAt(isel) );
}


void CDlgCompare::OnLbnSelchangeListDiff4()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report4.GetCurSel();
	SetWindowText( DATA4.GetAt(isel) );
}

void CDlgCompare::OnLbnSelchangeListDiff5()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report5.GetCurSel();
	SetWindowText( DATA5.GetAt(isel) );
}

void CDlgCompare::OnLbnSelchangeListDiff6()
{
	// TODO: добавьте свой код обработчика уведомлений
	int isel = report6.GetCurSel();
	SetWindowText( "Try swapping pins" );
}

void CDlgCompare::OnAddedDblClick() // (dbl click)
{
	CString str;
	int isel = report1.GetCurSel();
	report1.GetText( isel, str );
	if( CNETLIST )
		str += "&";
	m_doc->SwitchToPage( IP1, TRUE );
	m_doc->m_view->CancelSelection();
	int it = -1;
	CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
	if( t == NULL )
	{
		it = -1;
		int Page = IP1;
		int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,Page);
		if( t && pg != Page )
			m_doc->SwitchToPage( pg );
	}
	if( t == NULL )
	{
		int Page = IP1;
		int start_i = -1;
		t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
		if( t && Page != IP1 )
			m_doc->SwitchToPage( Page );
	}
	if( t )
	{
		m_doc->m_view->NewSelectP( t, NULL );
		m_doc->m_view->JumpToPart( t );
	}
}

void CDlgCompare::OnDeletedDblClick() // (dbl click)
{
	if( CNETLIST )
		m_doc->SwitchToPage( IP1, TRUE );
	else
		m_doc->SwitchToPage( IP2, TRUE );
	CString str;
	int isel = report4.GetCurSel();
	report4.GetText( isel, str );
	if( CNETLIST )
		str += "&";
	m_doc->m_view->CancelSelection();
	int it = -1;
	CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
	if( t == NULL )
	{
		it = -1;
		int Page = m_doc->Pages.GetActiveNumber();
		int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,Page);
		if( t && pg != Page )
			m_doc->SwitchToPage( pg );
	}
	if( t == NULL )
	{
		int Page = m_doc->Pages.GetActiveNumber();
		int start_i = -1;
		t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
		if( t && Page != m_doc->Pages.GetActiveNumber() )
			m_doc->SwitchToPage( Page );
	}
	if( t )
	{
		m_doc->m_view->NewSelectP( t, NULL );
		m_doc->m_view->JumpToPart( t );
	}
}
void CDlgCompare::OnValueDblClick() // (value dbl click)
{
	CString str;
	int cur_p = m_doc->Pages.GetActiveNumber();
	int isel = report2.GetCurSel();
	report2.GetText( isel, str );
	if( CNETLIST )
	{
		str += "&";
		cur_p = IP1;
	}
	for( int STEP=0; STEP<2; STEP++ )
	{
		int Page;
		if( CNETLIST )
			Page = IP1;
		else if( STEP == 0 )
			Page = (cur_p==IP1?IP2:IP1);
		else
			Page = (cur_p==IP1?IP1:IP2);
		m_doc->SwitchToPage( Page );
		m_doc->m_view->CancelSelection();
		int it = -1;
		CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
		if( t == NULL )
		{
			it = -1;
			int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,Page);
			if( t && pg != Page )
				m_doc->SwitchToPage( pg );
		}
		if( t == NULL )
		{
			int start_i = -1;
			t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
			if( t && Page != m_doc->Pages.GetActiveNumber() )
				m_doc->SwitchToPage( Page );
		}
		if( t )
		{
			m_doc->m_view->NewSelectP( t, NULL );
			m_doc->m_view->JumpToPart( t );
		}
		if( CNETLIST )
			break;
	}
}

void CDlgCompare::OnFootprintDblClick() // (dbl click)
{
	int cur_p = m_doc->Pages.GetActiveNumber();
	int isel = report3.GetCurSel();
	CString str;
	report3.GetText( isel, str );
	if( CNETLIST )
	{
		str += "&";
		cur_p = IP1;
	}
	for( int STEP=0; STEP<2; STEP++ )
	{
		int Page;
		if( CNETLIST )
			Page = IP1;
		else if( STEP == 0 )
			Page = (cur_p==IP1?IP2:IP1);
		else
			Page = (cur_p==IP1?IP1:IP2);
		m_doc->SwitchToPage( Page );
		m_doc->m_view->CancelSelection();
		int it = -1;
		CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
		if( t == NULL )
		{
			it = -1;
			int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,Page);
			if( t && pg != Page )
				m_doc->SwitchToPage( pg );
		}
		if( t == NULL )
		{
			int start_i = -1;
			t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
			if( t && Page != m_doc->Pages.GetActiveNumber() )
				m_doc->SwitchToPage( Page );
		}
		if( t )
		{
			m_doc->m_view->NewSelectP( t, NULL );
			m_doc->m_view->JumpToPart( t );
		}
		if( CNETLIST )
			break;
	}
}

void CDlgCompare::OnPinsNetsDblClick() // (pins & nest)
{
	int cur_p = m_doc->Pages.GetActiveNumber();
	int isel = report5.GetCurSel();
	CString str;
	report5.GetText( isel, str );
	if( CNETLIST )	
	{
		cur_p = IP1;
	}
	int rf = str.ReverseFind('.');
	if( rf > 0 )
	{
		CString Part = str.Left( rf );
		if( CNETLIST )	
			Part += "&";
		CString Pin = str.Right( str.GetLength() - rf - 1 );
		for( int STEP=0; STEP<2; STEP++ )
		{
			int Page;
			if( CNETLIST )
				Page = IP1;
			else if( STEP == 0 )
				Page = (cur_p==IP1?IP2:IP1);
			else
				Page = (cur_p==IP1?IP1:IP2);
			m_doc->SwitchToPage( Page );
			m_doc->m_view->CancelSelection();
			CText * t = NULL;		
			int it = -1;
			CPolyLine * P = NULL;
			if( t == NULL )
			{
				it = -1;
				int pg = Page;
				CText * ff = NULL;
				for( pg = m_doc->Pages.FindPart(&Part,&t,1,1,it,pg);
					 (P==NULL && t && pg>=0);
					 pg = m_doc->Pages.FindPart(&Part,&t,0,1) )
				{
					if( t && pg != Page )
						m_doc->SwitchToPage( pg );
					if(t)
					{
						ff = t;
						P = FindPin( t, &Pin, m_doc->m_outline_poly );
					}
				}
				t = ff;
			}
			if( t == NULL )
			{
				it = -1;
				int mpg = Page;
				CText * ff = NULL;
				do
				{
					Page = mpg;
					t = m_doc->Pages.FindCPartElement( &str, &Page, &it );
					if( t && Page != mpg )
						m_doc->SwitchToPage( Page );
					if(t)
					{
						ff = t;
						P = FindPin( t, &Pin, m_doc->m_outline_poly );
					}
				}while( P == NULL && t );
				t = ff;
			}
			if( t )
			{
				if( P == NULL )
					P = FindPin( t, &Pin, m_doc->m_outline_poly );
				m_doc->m_view->NewSelectP( t, P?P->Check( index_pin_attr ):NULL, 0, P?12:0 );
				m_doc->m_view->JumpToPart( t, P?P->Check( index_pin_attr ):NULL );
			}
			if( CNETLIST )
				break;
		}
	}
	else
	{
		str = DATA5.GetAt(isel);
		str.Replace("("," ");
		str.Replace(")"," ");
		str.Replace(","," ");
		CString key;
		CArray<CString> arr;
		ParseKeyString( &str, &key, &arr );
		for( int STEP=0; STEP<2; STEP++ )
		{
			int Page;
			if( CNETLIST )
				Page = IP1;
			else if( STEP == 0 )
				Page = (cur_p==IP1?IP2:IP1);
			else
				Page = (cur_p==IP1?IP1:IP2);
			m_doc->SwitchToPage( Page );
			m_doc->m_view->CancelSelection();
			for( int item=0; item<arr.GetSize(); item++ )
			{
				int idx = arr.GetAt(item).Find(".");
				if( idx > 0 )
				{
					str = arr.GetAt(item).Left(idx);
					CString pin = arr.GetAt(item).Right( arr.GetAt(item).GetLength() - idx - 1 );
					int it = -1;
					CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
					if( t == NULL )
					{
						it = -1;
						int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,IP1);
						if( t && pg != IP1 )
							m_doc->SwitchToPage( pg );
					}
					if( t == NULL )
					{
						int start_i = -1;
						t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
						if( t && Page != IP1 )
							m_doc->SwitchToPage( Page );
					}
					if(t)
					{
						CPolyLine * PIN = FindPin( t, &pin, m_doc->m_outline_poly );
						m_doc->m_view->NewSelectP( t, PIN?PIN->Check( index_pin_attr ):NULL, 0, 15 );
					}
				}
			}
			m_doc->m_view->JumpToPart( NULL );
			if( CNETLIST )
				break;
		}
	}
	m_doc->m_view->OnRangeCmds( NULL );
}

void CDlgCompare::OnSwapPinsDblClick() //(swap pins)
{
	CString str;
	int cur_p = m_doc->Pages.GetActiveNumber();
	int isel = report6.GetCurSel();
	report6.GetText( isel, str );
	if( CNETLIST )
	{
		str += "&";
		cur_p = IP1;
	}
	for( int STEP=0; STEP<2; STEP++ )
	{
		int Page;
		if( CNETLIST )
			Page = IP1;
		else if( STEP == 0 )
			Page = (cur_p==IP1?IP2:IP1);
		else
			Page = (cur_p==IP1?IP1:IP2);
		m_doc->SwitchToPage( Page );
		m_doc->m_view->CancelSelection();
		int it = -1;
		CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
		if( t == NULL )
		{
			it = -1;
			int pg = m_doc->Pages.FindPart(&str,&t,1,1,it,IP1);
			if( t && pg != IP1 )
				m_doc->SwitchToPage( pg );
		}
		if( t == NULL )
		{
			int start_i = -1;
			t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
			if( t && Page != IP1 )
				m_doc->SwitchToPage( Page );
		}
		if( t )
		{
			m_doc->m_view->NewSelectP( t, NULL );
			m_doc->m_view->JumpToPart( t );
		}
		if( CNETLIST )
			break;
	}
}
//====================================================================================
void CDlgCompare::OnBnClickedChParts()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( mb_by_ref.GetCheck() )
	{
		mb_by_pos.SetCheck(0);
	}
	else
	{
		mb_by_pos.SetCheck(1);
	}
}
//====================================================================================
void CDlgCompare::OnBnClickedChNets()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( mb_by_pos.GetCheck() )
	{
		mb_by_ref.SetCheck(0);
	}
	else
	{
		mb_by_ref.SetCheck(1);
	}
}
//====================================================================================
void CDlgCompare::ReloadPages()
{
	if( DLG_CREATED == 0 )
		m_doc->m_view->m_dlg_compare->Create( IDD_COMPARE );	
	if( DLG_CREATED )
	{
		while( mc_page1.GetCount() )
			mc_page1.DeleteString(0);
		while( mc_page2.GetCount() )
			mc_page2.DeleteString(0);
		if( CNETLIST )
		{
			mc_page1.AddString( m_doc->Pages.GetCurrentPCBName(IP1)+".net" );
			mc_page2.AddString( m_netlist_name );
			mc_page1.SetCurSel( 0 );
			mc_page2.SetCurSel( 0 );
		}
		else
		{
			for( int i=0; i<m_doc->Pages.GetNumPages(); i++ )
			{
				CString p;
				m_doc->Pages.GetPageName( i, &p );
				mc_page1.AddString( p );
				mc_page2.AddString( p );
			}
			mc_page1.SetCurSel( 0 );
			mc_page2.SetCurSel( 1 );
		}
	}
}
//====================================================================================
void CDlgCompare::OnBnClickedSelect1() //(added)
{
	// TODO: добавьте свой код обработчика уведомлений
	int cur_p = m_doc->Pages.GetActiveNumber();
	if( CNETLIST )
		if( m_doc->Pages.GetPcbIndex( cur_p ) != m_doc->Pages.GetPcbIndex( IP1 ) )
			cur_p = IP1;
	m_doc->SwitchToPage( cur_p, TRUE );
	m_doc->m_view->CancelSelection();
	for( int i=0; i<report1.GetCount(); i++ )
	{
		CString str;
		report1.GetText( i, str );
		if( CNETLIST )
			str += "&";
		int it = -1;
		CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
		if( t == NULL )
		{
			int start_i = -1;
			int Page = IP1;
			t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
			if( Page != cur_p )
				continue;
		}
		if( t )
			m_doc->m_view->NewSelectP( t, NULL );
	}
	m_doc->m_view->JumpToPart( NULL );
	m_doc->m_view->OnRangeCmds(NULL);
}
//====================================================================================
void CDlgCompare::OnBnClickedSelect4() //(deleted)
{
	// TODO: добавьте свой код обработчика уведомлений
	if( CNETLIST )
		m_doc->SwitchToPage( IP1, TRUE );
	else
		m_doc->SwitchToPage( IP2, TRUE );
	m_doc->m_view->CancelSelection();
	for( int i=0; i<report4.GetCount(); i++ )
	{
		CString str;
		report4.GetText( i, str );
		int it = -1;
		CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
		if( t )
			m_doc->m_view->NewSelectP( t, NULL );
	}
	m_doc->m_view->JumpToPart( NULL );
	m_doc->m_view->OnRangeCmds(NULL);
}
//====================================================================================
void CDlgCompare::OnBnClickedSelect2() //(value)
{
	// TODO: добавьте свой код обработчика уведомлений
	int cur_p = m_doc->Pages.GetActiveNumber();
	if( CNETLIST )
		if( m_doc->Pages.GetPcbIndex( cur_p ) != m_doc->Pages.GetPcbIndex( IP1 ) )
			cur_p = IP1;
	m_doc->SwitchToPage( cur_p, TRUE );
	for( int STEP=0; STEP<2; STEP++ )
	{
		int Page;
		if( CNETLIST )
			Page = IP1;
		else if( STEP == 0 )
			Page = (cur_p==IP1?IP2:IP1);
		else
			Page = (cur_p==IP1?IP1:IP2);
		m_doc->SwitchToPage( Page );
		m_doc->m_view->CancelSelection();
		for( int i=0; i<report2.GetCount(); i++ )
		{
			CString str;
			report2.GetText( i, str );
			if( CNETLIST )
				str += "&";
			int it = -1;
			CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
			if( t == NULL )
			{
				int start_i = -1;
				int Page = IP1;
				t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
				if( Page != cur_p )
					continue;
			}
			if( t )
				m_doc->m_view->NewSelectP( t, NULL );
		}
		m_doc->m_view->JumpToPart( NULL );
		m_doc->m_view->OnRangeCmds(NULL);
		if( CNETLIST )
			break;
	}
}
//====================================================================================
void CDlgCompare::OnBnClickedSelect3() //(footprint)
{
	// TODO: добавьте свой код обработчика уведомлений
	int cur_p = m_doc->Pages.GetActiveNumber();
	if( CNETLIST )
		if( m_doc->Pages.GetPcbIndex( cur_p ) != m_doc->Pages.GetPcbIndex( IP1 ) )
			cur_p = IP1;
	m_doc->SwitchToPage( cur_p, TRUE );
	for( int STEP=0; STEP<2; STEP++ )
	{
		int Page;
		if( CNETLIST )
			Page = IP1;
		else if( STEP == 0 )
			Page = (cur_p==IP1?IP2:IP1);
		else
			Page = (cur_p==IP1?IP1:IP2);
		m_doc->SwitchToPage( Page );
		m_doc->m_view->CancelSelection();
		for( int i=0; i<report3.GetCount(); i++ )
		{
			CString str;
			report3.GetText( i, str );
			if( CNETLIST )
				str += "&";
			int it = -1;
			CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
			if( t == NULL )
			{
				int start_i = -1;
				int Page = IP1;
				t = m_doc->Pages.FindCPartElement( &str, &Page, &start_i );
				if( Page != cur_p )
					continue;
			}
			if( t )
				m_doc->m_view->NewSelectP( t, NULL );
		}
		m_doc->m_view->JumpToPart( NULL );
		m_doc->m_view->OnRangeCmds(NULL);
		if( CNETLIST )
			break;
	}
}
//====================================================================================
void CDlgCompare::OnBnClickedRepairAll()
{
	// TODO: добавьте свой код обработчика уведомлений
	int CUR = m_doc->Pages.GetActiveNumber();
	if( m_doc->Pages.GetPcbIndex( CUR ) != m_doc->Pages.GetPcbIndex(IP1) )
	{
		AfxMessageBox( "This page does not belong to the testable netlist", MB_ICONERROR );
		return;
	}
	if( m_doc->m_view->m_protect )
	{
		AfxMessageBox( "This PCB is protected.", MB_ICONERROR );
		return;
	}
	int ok = AfxMessageBox( "Do you want the program to swap pins with names 1 and 2 for this listbox parts?", MB_YESNO );
	if( ok == IDYES )
	{
		// cancel sel on all pcb pages
		m_doc->m_view->CancelSelection();
		for( int i=0; i<report6.GetCount(); i++ )
		{
			CString str;
			report6.GetText( i, str );
			int it = -1;
			CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
			if( t )
				m_doc->m_view->NewSelectP( t, NULL );
		}
		m_doc->m_view->SaveUndoInfoForGroup( m_doc->m_undo_list );
		for( int index=report6.GetCount()-1; index>=0; index-- )
		{
			CString str;
			report6.GetText( index, str );
			int it = -1;
			CText * t = m_doc->Attr->m_Reflist->GetText( &str, &it );
			if( t )
				if( m_doc->GetNumAllPins( t ) == 2 )
				{
					CText * Tpin1 = NULL;
					CText * Tpin2 = NULL;
					CText * Tdesc1 = NULL;
					CText * Tdesc2 = NULL;
					CPolyLine * p = NULL;
					for( int get=t->m_polyline_start; get>=0; get=p->Next[index_part_attr] )
					{
						p = &m_doc->m_outline_poly->GetAt( get );
						if( CText * pin = p->Check( index_pin_attr ) )
						{
							if( pin->m_str.Compare( "1" ) == 0 )
							{
								Tpin1 = pin;
								if( CText * desc = p->Check( index_desc_attr ) )
									Tdesc1 = desc;
							}
							else if( pin->m_str.Compare( "2" ) == 0 )
							{
								Tpin2 = pin;
								if( CText * desc = p->Check( index_desc_attr ) )
									Tdesc2 = desc;
							}
						}
					}
					if( Tpin1 && Tpin2 )
					{
						CString d1 = "";
						if( Tdesc1 )
							d1 = Tdesc1->m_str;
						CString d2 = "";
						if( Tdesc2 )
							d2 = Tdesc2->m_str;
						if( (d1.GetLength() == 0 && d2.GetLength() == 0) || (d1.GetLength() && d2.GetLength()) )
						{
							Tpin1->Undraw();
							Tpin2->Undraw();
							if( Tpin1->m_str.Compare( "1" ) == 0 )
							{
								Tpin1->m_str = "2";				
								Tpin2->m_str = "1";	
							}
							else
							{
								Tpin1->m_str = "1";
								Tpin2->m_str = "2";
							}
							Tpin1->m_nchars = Tpin2->m_nchars = 1;
							Tpin1->MakeVisible();
							Tpin2->MakeVisible();
							if( d1.GetLength() )
							{
								Tdesc1->Undraw();
								Tdesc1->m_str = d2;
								Tdesc1->m_nchars = d2.GetLength();
								Tdesc1->MakeVisible();
								Tdesc2->Undraw();
								Tdesc2->m_str = d1;
								Tdesc2->m_nchars = d1.GetLength();
								Tdesc2->MakeVisible();
							}
							report6.DeleteString( index );
						}
						else
						{
							AfxMessageBox( "It is not possible to swap the pins of the "+t->m_str+" part. One pin has a description and the other does not.");
						}
					}
				}
		}
		m_doc->m_view->JumpToPart( NULL );
		m_doc->m_view->OnRangeCmds(NULL);
		m_doc->ProjectModified(TRUE);
	}
}
//====================================================================================
BOOL CDlgCompare::SelectNetlist()
{
	m_doc->Pages.DeleteVirtualPages();
	m_doc->Pages.CreateVirtualPages();

	// clear data
	while( report1.GetCount() )
		report1.DeleteString(0);
	while( report2.GetCount() )
		report2.DeleteString(0);
	while( report3.GetCount() )
		report3.DeleteString(0);
	while( report4.GetCount() )
		report4.DeleteString(0);
	while( report5.GetCount() )
		report5.DeleteString(0);
	while( report6.GetCount() )
		report6.DeleteString(0);
	DATA1.RemoveAll();
	DATA2.RemoveAll();
	DATA3.RemoveAll();
	DATA4.RemoveAll();
	DATA5.RemoveAll();
	DATA6.RemoveAll();

	int old_pg = m_doc->Pages.GetActiveNumber();

	// load netlist 1
	CString nList1 = m_doc->m_netlist_full_path + ".net";
	CStdioFile File;
	int ok = File.Open( nList1, CFile::modeRead, NULL );
	if( ok )
	{
		m_doc->Pages.SwitchToVP1();
		Import_PADSPCB_Netlist( m_doc, &File ); // On VP1
		File.Close();
		m_doc->SwitchToPage( old_pg, TRUE );
	}
	else 
	{
		if( m_doc->m_netlist_full_path.GetLength() )
			AfxMessageBox("Unable to open file: \n" + nList1, MB_ICONERROR);
		AfxMessageBox("First, generate a PADS-PCB netlist via 'File -> Save with Netlist' menu item. Before that, make sure that the PADS-PCB format is selected in the 'File -> Netlist Settings' menu. ");
		return 0;
	}
	// load netlist 2
	m_netlist_path = m_doc->RunFileDialog( TRUE, "net" );
	int rf = m_netlist_path.ReverseFind('\\');
	if( rf > 0 )
	{
		m_netlist_name = m_netlist_path.Right( m_netlist_path.GetLength() - rf - 1 );
		ok = File.Open( m_netlist_path, CFile::modeRead, NULL );
		if( ok )
		{
			m_doc->Pages.SwitchToVP2();
			Import_PADSPCB_Netlist( m_doc, &File );
			File.Close();
			m_doc->SwitchToPage( old_pg, TRUE );
		}
		else
			AfxMessageBox("Unable to open file: \n" + m_netlist_path, MB_ICONERROR);
	}
	m_doc->m_view->m_fk_button_index = -1;
	m_doc->m_view->DrawBottomPane();
	m_doc->m_view->OnRangeCmds(NULL);
	return 1;
}

void CDlgCompare::OnBnClickedSelNl2()
{
	// TODO: добавьте свой код обработчика уведомлений
	{
		OnCancel();
		m_doc->Pages.DeleteVirtualPages();
		if( m_doc->m_view->m_dlg_compare->DLG_CREATED )
			m_doc->m_view->m_dlg_compare->DestroyWindow();

		Reverse = 0;
		m_doc->m_view->m_dlg_compare = NULL;
		delete this;
	}
}
