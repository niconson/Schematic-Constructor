// DlgFindPart.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgFindPart.h"
#include "DlgImportPart.h"
#include "ReadFootprintFolder.h"
#include ".\dlgimportpart.h"

extern CFreeasyApp theApp;
CString LibMan = "FreePcb Footprints Library";

// CDlgImportPart dialog

IMPLEMENT_DYNAMIC(CDlgImportPart, CDialog)
CDlgImportPart::CDlgImportPart(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportPart::IDD, pParent)
{
	bDrawn = 1;
	bOpenTXTPreixes = 0;
	m_prefixes = "";
	m_doc_path[0] = "";
	m_doc_path[1] = "";
	m_doc_path[2] = "";
	m_doc_path[3] = "";
	m_str2 = "";

}

CDlgImportPart::~CDlgImportPart()
{
}

void CDlgImportPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDSETDEF, mb_Ignore);
	DDX_Control(pDX, ID_ONLY, mb_PcbKit1);
	DDX_Control(pDX, ID_PARTS_CURRENT_PR, mb_PcbKit2);
	DDX_Control(pDX, ID_ONCE, mb_PcbKit3);
	DDX_Control(pDX, IDOK, mb_Ok);
	DDX_Control(pDX, ID_QU, m_static[0]);
	DDX_Control(pDX, IDC_TXT_MAX, m_static[1]);
	DDX_Control(pDX, IDC_TXT_MIN, m_static[2]);
	DDX_Control(pDX, IDC_PIN_C, m_static[3]);
	DDX_Control(pDX, IDC_TXT_LINE_OPT, m_static[4]);
	DDX_Control(pDX, IDC_SEARCH_AMG, m_static[5]);
	//
	DDX_Control(pDX, IDC_COMBO1, m_combo_attr);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_LIST, m_part_list);
	DDX_Control(pDX, IDC_LIST2, m_path_list);
	DDX_Control(pDX, IDC_RADIO_STR_BEGIN, m_b[F_BEGIN]);
	DDX_Control(pDX, IDC_RADIO_STR_END, m_b[F_END]);
	DDX_Control(pDX, IDC_RADIO_STR_FIND, m_b[F_FIND]);
	DDX_Control(pDX, IDC_RADIO_WORD, m_b[F_WORD]);
	DDX_Control(pDX, IDC_RADIO_CASE_SENS, m_b[F_CASE]);
	DDX_Control(pDX, IDC_RADIO_PREFIX, m_b[F_PREFIX]);
	DDX_Control(pDX, IDC_RADIO_STR_REF, m_b[F_REF]);
	DDX_Control(pDX, IDC_RADIO_STR_VALUE, m_b[F_VALUE]);
	DDX_Control(pDX, IDC_RADIO_STR_FOOT, m_b[F_FOOT]);
	DDX_Control(pDX, IDC_RADIO_PIN_N, m_b[F_PIN]);
	DDX_Control(pDX, IDC_RADIO_NET_N, m_b[F_NET]);
	DDX_Control(pDX, IDC_RADIO_DESCR, m_b[F_DESC]);
	DDX_Control(pDX, IDC_RADIO_SEARCH_EVERYWHERE, m_b[F_OTHER]);
	DDX_Control(pDX, IDC_RADIO_SEARCH_ALL_PAGES, m_b[F_ALLPAGES]);
	DDX_Control(pDX, IDC_RADIO_LIST, m_b[F_USE_LIST]);
	DDX_Control(pDX, IDC_RADIO_DEF_SZS, m_b[F_DEF_SIZES]);
	DDX_Control(pDX, IDC_EDIT_RNG1, m_range1 );
	DDX_Control(pDX, IDC_EDIT_RNG2, m_range2 );
	//
	DDX_Control(pDX, IDC_PREVIEW, m_preview);
	DDX_Control(pDX, IDC_PREVIEW2, m_preview2);
	DDX_Control(pDX, IDRUNL, mb_runlib);
	DDX_Control(pDX, IDC_DOC1, m_doc1);
	DDX_Control(pDX, IDC_DOC2, m_doc2);
	DDX_Control(pDX, IDC_DOC3, m_doc3);
	DDX_Control(pDX, IDC_DOC4, m_doc4);
	//
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		HICON hIcon;
		hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		HICON prevIcon = SetIcon( hIcon, FALSE );
		if( prevIcon )
			DestroyIcon( prevIcon );
		bDrawn = TRUE;
		CString str;
		str.Format("%d", m_pin_range[0] );
		m_range1.SetWindowTextA( str );
		str.Format("%d", m_pin_range[1] );
		m_range2.SetWindowTextA( str );
		if( m_prefixes.GetLength() == 0 )
			ReadPrefixes();
		UploadPartlist();
		//
		if( m_attr_str && getbit( m_search_flags, F_USE_LIST) )
			if( m_attr_str->GetSize() )
			{
				CString fs = m_attr_str->GetAt(m_attr_str->GetSize()-1);
				int fsL = fs.GetLength();
				for(int k=0; k<m_parts->GetSize(); k++)
				{
					// insert part prefix
					if( m_parts->GetAt(k).Right(fsL+1).Compare(" "+fs) == 0 )
						m_combo_attr.SetWindowText( m_parts->GetAt(k).Left( m_parts->GetAt(k).GetLength() - fsL ) );
				}
			}
		//
		Change(); // (filter by prefix)
		if( m_attr_str )
			if( m_attr_str->GetSize() )
			{
				CString ss;
				///m_combo_attr.GetWindowText( ss );
				///if( ss.GetLength() == 0 )
				{
					// insert last string
					ss = m_attr_str->GetAt(m_attr_str->GetSize()-1);
					if( (ss.Find("@") > 0 && getbit( m_search_flags, F_USE_LIST)) ||
						(ss.Find("@") <= 0 && getbit( m_search_flags, F_USE_LIST) == 0) )
						m_combo_attr.SetWindowText( ss );
				}
			}
		//
		if( m_wt )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, *m_wt );
		}
		for( int f=0; f<NUM_F_FLAGS; f++ )
		{
			if( getbit( m_search_flags, f ) )
				m_b[f].SetCheck(1);
			else
				m_b[f].SetCheck(0);
		}
		if( m_b[F_USE_LIST].GetCheck() && m_part_list.GetCount() )
		{
			CString s, prefix;
			m_combo_attr.GetWindowTextA( s );
			m_part_list.GetText(0,prefix);
			int f = prefix.Find("]");
			if( f > 0 )
				prefix.Truncate(f+2);
			f = m_part_list.FindString(-1,prefix+s);
			if( f != -1 || m_part_list.GetCount() == 0 )
				m_part_list.SetCurSel(f);
			else
				m_part_list.SetCurSel(0);
		}
		UpdatebRunLibText();
		OnClickList();
		OnClickListButton();
		OnCheck();

		// patches
		CMainFrame * wnd = (CMainFrame*)AfxGetMainWnd();
		if( wnd )
			m_description.SetFont(&wnd->m_wndMyToolBar.m_font);
		CArray<CString> * pathes = m_library->GetPathes();
		for( int ip=0; ip<pathes->GetSize(); ip++ )
		{
			CString s = pathes->GetAt(ip).Right( pathes->GetAt(ip).GetLength() - theApp.m_Doc->m_parent_folder.GetLength() );
			m_description.AddString( s );
		}

		//
		CWnd * win = FindWindow(NULL, LibMan);
		if( win )
		{
			CRect flm_r;
			win->GetWindowRect(flm_r);
			CRect getR = rect(0,0,0,0);
			this->GetWindowRect(&getR);
			this->SetWindowPos(NULL,flm_r.right,flm_r.top,abs(getR.right-getR.left),abs(getR.top-getR.bottom),0);
		}
		//
		this->SetTimer(0,250,0);
	}
	else
	{
		// outgoing
		m_combo_attr.GetWindowText( m_str );
		m_search_flags = 0;
		for( int f=0; f<NUM_F_FLAGS; f++ )
		{
			if( m_b[f].GetCheck() )
				setbit( m_search_flags, f );
		}
		CString str;
		m_range1.GetWindowTextA( str );
		m_pin_range[0] = my_atoi(&str);
		m_range2.GetWindowTextA( str );
		m_pin_range[1] = my_atoi(&str);

	}
}


BEGIN_MESSAGE_MAP(CDlgImportPart, CDialog)
	ON_BN_CLICKED( IDC_RADIO_SEARCH_EVERYWHERE, OnCheck )
	ON_BN_CLICKED( IDSETDEF, OnPinDef )
	ON_BN_CLICKED( ID_ONLY, OnUniqueParts )
	ON_BN_CLICKED( ID_PARTS_CURRENT_PR, OnPartsCurrentProject )
	ON_BN_CLICKED( ID_ONCE, OnOnceAnyProject )
	ON_BN_CLICKED( ID_ONLY, OnUniqueParts )
	//ON_BN_CLICKED( ID_ONLY2, OnAboutUniqueParts )
	ON_BN_CLICKED( IDC_RADIO_LIST, OnClickListButton )
	ON_LBN_SELCHANGE( IDC_LIST, OnClickList )
	ON_LBN_SELCHANGE( IDC_LIST2, OnClickList2 )
	ON_LBN_DBLCLK( IDC_LIST, OnDblClickList )
	ON_LBN_DBLCLK( IDC_LIST2, OnDblClickList2 )
	ON_BN_CLICKED(IDOK, &CDlgImportPart::OnBnClickedOk)
	ON_CBN_DROPDOWN(IDC_COMBO1, &CDlgImportPart::OnCbnDropdownCombo1)
	ON_CBN_EDITCHANGE(IDC_COMBO1, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnChange)
	ON_BN_CLICKED(IDRUNL, &CDlgImportPart::OnBnClickedRunl)
	ON_BN_CLICKED(IDC_DOC1, &CDlgImportPart::OnBnClickedDoc1)
	ON_BN_CLICKED(IDC_DOC2, &CDlgImportPart::OnBnClickedDoc2)
	ON_BN_CLICKED(IDC_DOC3, &CDlgImportPart::OnBnClickedDoc3)
	ON_BN_CLICKED(IDC_DOC4, &CDlgImportPart::OnBnClickedDoc4)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_SEARCH_ALL_PAGES, &CDlgImportPart::OnBnClickedRadioSearchAllPages)
	ON_CBN_EDITCHANGE(IDC_DESCRIPTION, &CDlgImportPart::OnEnChangeDescription)
	ON_CBN_SELCHANGE(IDC_DESCRIPTION, &CDlgImportPart::OnSelChangeDescription)
	ON_BN_CLICKED(ID_Q8, &CDlgImportPart::OnBnClickedQ8)
	ON_BN_CLICKED(IDC_DOC5, &CDlgImportPart::OnBnClickedDoc5)
	ON_BN_CLICKED(ID_QU, &CDlgImportPart::OnBnClickedQu)
END_MESSAGE_MAP()


// CDlgImportPart message handlers

void CDlgImportPart::Initialize( CArray<CString> * str, int flags, int mask, int pin_min, int pin_max, CString * caption, CLibPointer * library, CString * app_dir )
{
	m_app_dir = *app_dir;
	if( m_app_dir.Right(1) != "\\" )
		m_app_dir += "\\";
	m_library = library;
	m_parts = m_library->GetLibrary();
	m_attr_str = str;
	m_search_flags = flags;
	m_search_mask = mask;
	m_wt = caption;
	m_pin_range[0] = pin_min;
	m_pin_range[1] = pin_max;
	m_current_path = -1;
}

void CDlgImportPart::OnCheck()
{
	if( m_b[F_OTHER].GetCheck() )
	{
		m_b[F_ALLPAGES].SetCheck(1);
		m_b[F_ALLPAGES].EnableWindow(0);
	}
	else
		m_b[F_ALLPAGES].EnableWindow(1);
}

void CDlgImportPart::OnPinDef()
{
	m_range1.SetWindowTextA("-1");
	m_range2.SetWindowTextA("-1");
}

void CDlgImportPart::OnClickList()
{
	CString str;
	int i = m_part_list.GetCurSel();
	if( i >= 0 )
	{
		m_part_list.GetText(i,str);
		if( str.Left(1) != "[" )
			RemoveDescription( &str );
		CString str2 = str;
		int scob = str2.Find(" ");
		if( scob > 0 )
		{
			int dog = str2.Find("@");
			if( dog > scob )
				str2 = str2.Right( str2.GetLength()-scob-1 );
			str2 = str2.TrimLeft();
		}
		if( theApp.m_Doc->m_footprint_extention.GetLength() == 0 )
		{
			CWnd * win = FindWindow(NULL, LibMan);
			if( win )
			{
				CString F=str;
				int dog = F.Find("@");
				if( dog > 0 )
					F = F.Right( F.GetLength()  - dog - 1 );
				dog = F.Find("]");
				if( dog > 0 )
					F = F.Right( F.GetLength()-dog-1 );
				F = F.Trim();
				theApp.m_Doc->OnSendMess( win->GetSafeHwnd(), &F );
				win->SetForegroundWindow();
			}
		}
		//
		//
		m_combo_attr.SetWindowTextA(str2);
		for( int i=m_path_list.GetCount()-1; i>=0; i-- )
			m_path_list.DeleteString(i);
		CArray<CString> * Pathes = m_library->GetPathes( &str );
		for( int i=0; i<Pathes->GetSize(); i++ )
		{
			m_path_list.AddString( Pathes->GetAt(i).Right( Pathes->GetAt(i).GetLength() - theApp.m_Doc->m_parent_folder.GetLength() ) );
		}
		m_current_path = 0;
		DrawPattern( &str2 );
		m_path_list.SetCurSel( m_current_path );
		CheckDocuments( &str2 );
	}
}

void CDlgImportPart::OnClickList2()
{
	m_current_path = m_path_list.GetCurSel();
	CString str;
	int i = m_part_list.GetCurSel();
	if( i >= 0 )
	{
		m_part_list.GetText(i,str);
		if( str.Left(1) != "[" )
			RemoveDescription( &str );
		CString str2 = str;
		int scob = str2.Find(" ");
		if( scob > 0 )
		{
			int dog = str2.Find("@");
			if( dog > scob )
				str2 = str2.Right( str2.GetLength()-scob-1 );
			str2 = str2.TrimLeft();
		}
		//
		DrawPattern( &str2 );
		CString file;
		m_path_list.GetText(m_current_path,file);
		file = theApp.m_Doc->m_parent_folder + file;
		CheckDocuments( &str2, &file );
	}
}

void CDlgImportPart::OnDblClickList()
{
	CString s = theApp.m_Doc->m_pcb_full_path;
	int L = theApp.m_Doc->m_parent_folder.GetLength();
	s = s.Right( s.GetLength() - L );
	m_current_path = m_path_list.FindStringExact(-1,s);
	if( m_current_path >= 0 )
	{
		m_path_list.SetCurSel( m_current_path );
		OnDblClickList2();
	}
	else
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"This part is not used in the current project":
			"Эта деталь не используется в текущем проекте.");
}

void CDlgImportPart::OnDblClickList2()
{
	CString ref,path;
	m_combo_attr.GetWindowTextA( ref );
	m_path_list.GetText( m_current_path, path );
	path = theApp.m_Doc->m_parent_folder + path;
	theApp.m_Doc->SwitchToCDS( &path, &ref );
}

void CDlgImportPart::OnClickListButton()
{
	static CRect WR = rect(0,0,0,0);
	if( WR.Width() == 0 || WR.left == 0 )
		this->GetWindowRect(&WR);
	if( m_b[F_USE_LIST].GetCheck() )
	{
		mb_Ok.SetWindowTextA("IMPORT");
		m_part_list.ShowWindow(1);
		m_path_list.ShowWindow(1);
		for( int i=0; i<F_USE_LIST; i++ )
			m_b[i].ShowWindow(0);
		m_range1.ShowWindow(0);
		m_range2.ShowWindow(0);
		mb_Ignore.ShowWindow(0);
		mb_PcbKit1.ShowWindow(1);
		mb_PcbKit2.ShowWindow(1);
		mb_PcbKit3.ShowWindow(1);
		m_static[0].ShowWindow(1);
		for( int st=1; st<6; st++ )
			m_static[st].ShowWindow(0);
		CRect getR = rect(0,0,0,0);
		this->GetWindowRect(&getR);
		this->SetWindowPos(NULL,getR.left,getR.top,734,615,0);
		this->UpdateWindow();
		if( m_part_list.GetCount() )
		{
			if( m_part_list.GetCurSel() == -1 )
				m_part_list.SetCurSel(0);
			m_preview.UpdateWindow();
			OnClickList ();
			OnClickList2();
		}	
	}
	else
	{
		mb_Ok.SetWindowTextA("FIND");
		m_part_list.ShowWindow(0);
		m_path_list.ShowWindow(0);
		for( int i=0; i<F_USE_LIST; i++ )
		{
			m_b[i].ShowWindow(1);
			if( getbit( m_search_mask, i ) )
				m_b[i].EnableWindow(1);
			else
				m_b[i].EnableWindow(0);
		}
		m_range1.ShowWindow(1);
		m_range2.ShowWindow(1);
		mb_Ignore.ShowWindow(1);
		mb_PcbKit1.ShowWindow(0);
		mb_PcbKit2.ShowWindow(0);
		mb_PcbKit3.ShowWindow(0);
		m_static[0].ShowWindow(0);
		for( int st=1; st<6; st++ )
			m_static[st].ShowWindow(1);
		m_doc1.ShowWindow(0);
		m_doc2.ShowWindow(0);
		m_doc3.ShowWindow(0);
		m_doc4.ShowWindow(0);
		this->GetWindowRect(&WR);
		this->SetWindowPos(NULL,WR.left,WR.top,(309),(439),0);
	}
}

void CDlgImportPart::OnChange()
{
	int isel = m_combo_attr.GetCurSel();
	if( isel == m_combo_attr.GetCount()-1 )
	{
		m_combo_attr.SetCurSel(-1);
		OpenTXTPreixes();
		bOpenTXTPreixes = 0;
		m_prefixes = "";
		return;
	}
	if( m_b[F_USE_LIST].GetCheck() )
	{
		//DWORD i1 = m_combo_attr.GetEditSel();
		CString gs;
		if( isel != -1 )
		{ 
			m_combo_attr.GetLBText( isel, gs );
			m_combo_attr.SetCurSel(-1);
			int f = gs.Find("] ");
			if( f > 0 )
				gs.Truncate( f+1 );
			m_combo_attr.SetWindowTextA( gs );
			if( gs.Left(1) != "[" )
				return;
		}
		else
			m_combo_attr.GetWindowTextA( gs );
		if( gs.Left(1) == "[" )
			m_combo_attr.SetEditSel( gs.GetLength(), gs.GetLength() );
		Change();
		m_combo_attr.Invalidate(0);
	}
}

void CDlgImportPart::Change()
{
	if( m_parts )
	{
		if( m_parts->GetSize() )
		{
			CString s;
			m_combo_attr.GetWindowTextA(s);
			s.MakeLower();

			m_part_list.EnableWindow(0);
			m_part_list.ShowWindow(SW_HIDE);
			for( int i=m_part_list.GetCount()-1; i>=0; i-- )
				m_part_list.DeleteString(i);
			for( int i=m_parts->GetSize()-1; i>=0; i-- )
			{
				CString gs = m_parts->GetAt(i);
				gs.MakeLower();
				if( gs.Find(s) >= 0 )
					m_part_list.AddString( m_parts->GetAt(i) );
			}
			m_part_list.ShowWindow(SW_SHOW);
			m_part_list.EnableWindow(1);
		}
	}
}

void CDlgImportPart::OnUniqueParts()
{
	if( PcbAssemblyKit(0) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "No unique parts found":"Уникальных деталей не найдено");
		OnChange();
	}
}

void CDlgImportPart::OnPartsCurrentProject()
{
	if( PcbAssemblyKit(1) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "No parts used in the current project":"Детали не используются в текущем проекте.");
		OnChange();
	}
}

void CDlgImportPart::OnOnceAnyProject()
{
	if( PcbAssemblyKit(2) == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"No part found that was used only once":
			"Не обнаружено ни одной детали, которая использовалась бы только один раз.");
		OnChange();
	}
}

int CDlgImportPart::PcbAssemblyKit( int MODE )
{
	enum{ M_ONLY=0, M_CURRENT, M_ONCE };
	if( m_parts )
	{
		if( m_parts->GetSize() )
		{
			CString s = theApp.m_Doc->m_pcb_full_path;
			m_part_list.EnableWindow(0);
			m_part_list.ShowWindow(SW_HIDE);
			for( int i=m_part_list.GetCount()-1; i>=0; i-- )
				m_part_list.DeleteString(i);
			for( int i=m_parts->GetSize()-1; i>=0; i-- )
			{
				CString gs = m_parts->GetAt(i);
				CArray<CString> * pathes = m_library->GetPathes(&gs);
				if( MODE == M_ONCE )
				{
					if( pathes->GetSize() == 1 )
						m_part_list.AddString( gs );
				}
				else for( int ii=0; ii<pathes->GetSize(); ii++ )
				{
					if( s.CompareNoCase( pathes->GetAt(ii) ) == 0 )
					{
						if( MODE != M_ONLY || pathes->GetSize() == 1 )
						{
							m_part_list.AddString( gs );
							break;
						}
					}
					if( MODE == M_ONLY )
						break;
				}
			}
			m_part_list.ShowWindow(SW_SHOW);
			m_part_list.EnableWindow(1);
		}
	}
	return m_part_list.GetCount();
}

void CDlgImportPart::OnAboutUniqueParts()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		"Filters to help optimize pcb assembly kit.\nDouble-clicking on the file path (bottom listbox) will open this file and the program will jump to the selected part.":
		"Фильтры для оптимизации комплекта для сборки печатной платы.\nДвойной щелчок по пути к файлу (нижний список) откроет этот файл, и программа перейдет к выбранной детали.", MB_ICONINFORMATION);
}

void CDlgImportPart::DrawPattern( CString * VdP )
{
	if( bDrawn )
		return;
	CString SpeedFile = theApp.m_Doc->m_library.GetPath( VdP, m_current_path );
	int insertMode = 1;
	int singleMode = 1;
	int f_flags = 0;
	setbit( f_flags, F_WORD );
	setbit( f_flags, F_VALUE );
	setbit( f_flags, F_FOOT );
	setbit( f_flags, NO_SRC );

	//
	int idog = VdP->Find("@");
	CString str_value="", str_footprint="";
	if( idog > 0 )
	{
		str_value = VdP->Left(idog);
		str_footprint = VdP->Right(VdP->GetLength()-idog-1);
		str_value.MakeLower();
		str_footprint.MakeLower();
	}
	int cX=0, cY=0;
	CArray<CPolyLine> * p_arr = &theApp.m_Doc->clip_outline_poly;
	p_arr->RemoveAll();
	CString SRC="";
	theApp.m_Doc->m_view->FindStrInFile( &SpeedFile, (&str_value), (&str_footprint), f_flags, insertMode, singleMode, &cX, &cY, -1, -1, -1, &SRC );
	//
	CRect wr;
	m_preview.GetClientRect(&wr);
	int win_W = wr.Width();
	int win_H = wr.Height();
	RECT TotalR;
	TotalR.left = TotalR.bottom = INT_MAX;
	TotalR.top = TotalR.right = INT_MIN;
	if( p_arr->GetSize() == 0 )
		TotalR = rect(0,0,NM_PER_MM,NM_PER_MM);
	else for( int i=0; i<p_arr->GetSize(); i++ )
	{
		CPolyLine * p = &p_arr->GetAt(i);
		RECT r = p->GetBounds();
		SwellRect( &TotalR, r );
		if( CText * pin=p->Check( index_pin_attr ) )
		{
			int ch = pin->m_tl->GetTextRectOnPCB( pin, &r );
			if( ch && pin->m_font_size > 0 )
				SwellRect( &TotalR, r );
		}
		if( CText * desc=p->Check( index_desc_attr ) )
		{
			int ch = desc->m_tl->GetTextRectOnPCB( desc, &r );
			if( ch && desc->m_font_size > 0 )
				SwellRect( &TotalR, r );
		}
	}
	m_doc_icon_dir = FALSE;
	if( TotalR.right-TotalR.left > TotalR.top-TotalR.bottom )
		m_doc_icon_dir = TRUE;
	int sw_value = max((TotalR.right-TotalR.left),(TotalR.top-TotalR.bottom))/25;
	SwellRect( &TotalR, sw_value );
	//
	int k1 = (TotalR.right-TotalR.left)/win_W;
	int k2 = (TotalR.top-TotalR.bottom)/win_H;
	k1 = max( k1, k2 );
	k2 = TotalR.left/k1;
	int k3 = TotalR.bottom/k1;
	CDC * gDC = m_preview.GetDC();
	COLORREF txt = RGB(80,70,70);
	COLORREF blk = RGB(190,220,190);
	COLORREF rgb = RGB(240,240,255);
	COLORREF dsc = RGB(0,128,255);
	CPen pentxt( PS_SOLID, 2, txt );
	CPen penblk( PS_SOLID, 3, blk );
	CPen penrgb( PS_SOLID, 1, rgb );
	CPen pendsc( PS_SOLID, 0, dsc );

	COLORREF gnd = RGB(255,255,255);
	CPen pengnd( PS_SOLID, 1, gnd );
	CBrush brush( gnd );
	gDC->SelectObject( &brush );
	gDC->SelectObject( &pengnd );
	gDC->Rectangle( 0, 0, win_W, win_H );
	
	CPen * Pens[4];
	Pens[0] = &pentxt;
	Pens[1] = &penblk;
	Pens[2] = &penrgb;
	Pens[3] = &pendsc;

	m_doc1.ShowWindow(0);
	m_doc2.ShowWindow(0);
	m_doc3.ShowWindow(0);
	m_doc4.ShowWindow(0);

	int w = abs(TotalR.right-TotalR.left)/k1;
	int h = abs(TotalR.top-TotalR.bottom)/k1;
	int kx = (win_W - w)/2;
	int ky = (win_H - h)/2;
	if( p_arr->GetSize() )for( int STEP=0; STEP<3; STEP++ )
	{
		gDC->SelectObject( Pens[STEP] );
		int SP = (STEP?-2:0);
		int ST = 2*(2-STEP);
		for( int i=0; i<p_arr->GetSize(); i++ )
		{
			CRect poly_rect;
			poly_rect.left = poly_rect.bottom = INT_MAX;
			poly_rect.right = poly_rect.top = INT_MIN;
			CPolyLine * p = &p_arr->GetAt(i);
			for( int ii=0; ii<p->GetNumSides(); ii++ )
			{
				if( p->GetContourStart( p->GetNumContour(ii) ) == ii )
				{
					int x = p->GetX(ii)/k1 - k2 + kx;
					int y = p->GetY(ii)/k1 - k3 + ky;
					y = win_H - y;
					gDC->MoveTo(x+SP,y+SP);
					SwellRect( &poly_rect, x+SP,y+SP );
				}
				int inx = p->GetNextCornerIndex(ii);
				int x = p->GetX(inx)/k1 - k2 + kx;
				int y = p->GetY(inx)/k1 - k3 + ky;
				y = win_H - y;
				if( p->GetSideStyle(ii) )
				{
					int xp = p->GetX(ii)/k1 - k2 + kx;
					int yp = p->GetY(ii)/k1 - k3 + ky;
					yp = win_H - yp;
					CPoint P[10];
					int np = Generate_Arc( xp, yp, x, y, 3-p->GetSideStyle(ii), P, 9 );
					for(int ip=0; ip<np; ip++ )
					{
						gDC->LineTo( P[ip].x+SP, P[ip].y+SP );
					}
				}
				else
					gDC->LineTo(x+SP,y+SP);
				SwellRect( &poly_rect, x+SP,y+SP );
			}
			if( STEP == 0 )
			{
				// draw picture
				if( p->GetPicture() == -2 && SRC.GetLength() )
				{
					CFile file;
					int err = file.Open( SRC, CFile::modeRead, NULL );
					if( err )
					{
						SwellRect( &poly_rect, -2 );
						MoveRect( &poly_rect, -1,-1 );
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
							poly_rect.NormalizeRect();
							lp->Render( gDC->GetSafeHdc(), 
								 poly_rect.left, 
								 poly_rect.bottom, 
								 poly_rect.right - poly_rect.left, 
								 poly_rect.top - poly_rect.bottom, 
								 0,0,sz.cx,sz.cy, NULL );
							lp->Release();
						}
					}
				}
				continue;
			}
			//
			if( CText * pin=p->Check( index_pin_attr ) )
			{
				if( pin->m_font_size > 0 )
				{
					CArray<CPoint> * pt_arr = pin->dl_el->dlist->Get_Points( pin->dl_el, NULL, 0 );
					int np = pt_arr->GetSize();
					CPoint * GET = new CPoint[np];//new004
					pin->dl_el->dlist->Get_Points( pin->dl_el, GET, &np ); 
					for( int iv=0; iv<np; iv++ )
					{
						int x = GET[iv].x/k1 - k2 + kx;
						int y = GET[iv].y/k1 - k3 + ky;
						y = win_H - y;
						if( x > 1 && x < win_W-1 && y > 1 && y < win_H-1 )
						{
							if( iv%2 == 0 )
								gDC->MoveTo(x+ST,y+ST);
							else
								gDC->LineTo(x+ST,y+ST);
						}
						else if( iv%2 == 0 )
							iv++;
					}
					delete GET;//new004
				}
			}
			if( STEP == 1 )
				continue;
			gDC->SelectObject( Pens[3] );
			if( CText * desc=p->Check( index_desc_attr ) )
			{
				if( desc->m_font_size > 0 )
				{
					CArray<CPoint> * pt_arr = desc->dl_el->dlist->Get_Points( desc->dl_el, NULL, 0 );
					int np = pt_arr->GetSize();
					CPoint * GET = new CPoint[np];//new005
					desc->dl_el->dlist->Get_Points( desc->dl_el, GET, &np ); 
					for( int iv=0; iv<np; iv++ )
					{
						int x = GET[iv].x/k1 - k2 + kx;
						int y = GET[iv].y/k1 - k3 + ky;
						y = win_H - y;
						if( x > 1 && x < win_W-1 && y > 1 && y < win_H-1 )
						{
							if( iv%2 == 0 )
								gDC->MoveTo(x+ST,y+ST);
							else
								gDC->LineTo(x+ST,y+ST);
						}
						else if( iv%2 == 0 )
							iv++;
					}
					delete GET;//new005
				}
			}
			gDC->SelectObject( Pens[STEP] );
		}
	}
	for( int i=0; i<p_arr->GetSize(); i++ )
	{
		CPolyLine * p = &p_arr->GetAt(i);
		int iatt = -1;
		while( CText * t=theApp.m_Doc->ScanAttr( p, &iatt ) )
		{
			t->InVisible();
			p->pAttr[iatt] = NULL;
		}
		int src = p->GetPicture();
		if( src >= 0 )
			theApp.m_Doc->m_dlist->SetTemplateVis( 0, src );
	}
	p_arr->RemoveAll();
    mb_runlib.EnableWindow(1);

	// draw picture
	ReleaseDC( gDC );
	gDC = m_preview2.GetDC();
	CRect rect;
	m_preview2.GetClientRect( &rect );
	win_W = rect.Width();
	win_H = rect.Height();
	COLORREF gray = RGB(222,222,222);
	CPen pgray( PS_SOLID, 1, gray );
	CBrush br( gray );
	gDC->SelectObject( &br );
	gDC->SelectObject( &pgray );
	gDC->Rectangle( 0,0,win_W,win_H );
	if( str_footprint.GetLength() == 0 )
	{
		ReleaseDC( gDC );
		return;
	}
	CString picFile = str_footprint;
	ExtractComponentName( &picFile, NULL );
	picFile = theApp.m_Doc->m_app_dir + main_component + "\\" + picFile;
	CFile file;
	if( file.Open( picFile + ".jpeg", CFile::modeRead, NULL ) == 0 )
		if( file.Open( picFile + ".jpg", CFile::modeRead, NULL ) == 0 )
			if( file.Open( picFile + ".bmp", CFile::modeRead, NULL ) == 0 )
			{
				ReleaseDC( gDC );
				return;
			}
	{
		CArchive ar( &file, CArchive::load | CArchive::bNoFlushOnDelete );
		CArchiveStream ars(&ar);
		LPPICTURE lp;
		OleLoadPicture( (LPSTREAM)&ars, 0, FALSE, IID_IPicture, (void**)&lp );
		ar.Close();
		file.Close();
		if( lp )
		{
			gDC->SelectObject( &brush );
			gDC->Rectangle( 0,0,win_W,win_H );
			CSize sz;
			lp->get_Width( &sz.cx );
			lp->get_Height( &sz.cy );
			double k = (double)sz.cx/(double)sz.cy;
			if( k > 1.0 )
				lp->Render( gDC->GetSafeHdc(), 
				 0,
				 (double)rect.Height()/2.0/k*(k+1.0),
				 rect.Width(), 
				 -(double)rect.Height()/k, 
				 0,0,sz.cx,sz.cy, NULL );
			else
				lp->Render( gDC->GetSafeHdc(), 
				 (double)rect.Width()/2.0*(1.0-k),
				 rect.Height(),
				 (double)rect.Width()*k, 
				 -rect.Height(), 
				 0,0,sz.cx,sz.cy, NULL );
			lp->Release();
		}
	}
	ReleaseDC( gDC );
}

void CDlgImportPart::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialog::OnOK();
	CString F="";
	int cur = m_part_list.GetCurSel();
	if( cur >= 0 )
	{
		m_part_list.GetText( cur, F );
		int dog = F.Find("@");
		if( dog > 0 )
			F = F.Right( F.GetLength()  - dog - 1 );
		dog = F.Find("]");
		if( dog > 0 )
			F = F.Right( F.GetLength()-dog-1 );
		F = F.Trim();
	}
	else 
		return;
	if( FindFootprint( theApp.m_Doc, &F ) ==  0 )
	{		
		CString fpath="";
		int ip = m_path_list.GetCurSel();
		if( ip >= 0 )
		{
			m_path_list.GetText( ip, fpath );
			fpath = theApp.m_Doc->m_parent_folder + fpath;
			ip = fpath.ReverseFind('\\');
			if( ip > 0 )
				fpath.Truncate( ip );

			BOOL OK = FindFootprintInFolder( theApp.m_Doc, &F, &F, &fpath );
			if( OK == 0 && F.Compare( NO_FOOTPRINT ) )
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"WARNING: The footprint of this part was not found in the library!":
					"ВНИМАНИЕ: футпринт этой детали не найден в библиотеке!");
			else if( OK > 1 )
				m_str2 = theApp.m_Doc->m_FootprintNames.GetAt( theApp.m_Doc->m_FootprintNames.GetSize()-1 );
		}
		ReadFootprintFolder( theApp.m_Doc );
	}
}

void CDlgImportPart::OpenTXTPreixes()
{
	ShellExecute( NULL,"open", m_app_dir+"prefixes.txt", NULL, NULL, SW_SHOWNORMAL);
}

void CDlgImportPart::OnCbnDropdownCombo1()
{
	if( m_prefixes.GetLength() == 0 )
	{
		ReadPrefixes();

		// update prefixes
		UploadPartlist();
	}

	
	if( bOpenTXTPreixes || m_prefixes.Find( OLD_NO_FP ) != -1 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"Fill in the missing part class names in the text file. Enter text instead of question marks and when finished save this text file":
			"Заполните недостающие имена классов деталей в текстовом файле. Введите текст вместо вопросительных знаков и по завершению, сохраните этот текстовый файл");
		bOpenTXTPreixes = 0;
		m_prefixes = "";
		OpenTXTPreixes();
	}
}


void CDlgImportPart::UploadPartlist()
{
	while( m_combo_attr.GetCount() )
		m_combo_attr.DeleteString(0);
	
	bOpenTXTPreixes = 0;
	CString Sprev = "";
	for(int k=0; k<m_parts->GetSize(); k++)
	{
		int iofi = m_parts->GetAt(k).Find("]");
		if( iofi == -1 )
			continue;
		CString found = m_parts->GetAt(k).Left( iofi+1 );
		if( Sprev.Compare( found ) == 0 )
			continue;
		//
		if( m_combo_attr.FindString( -1, found ) == -1 )
		{
			int i1 = m_prefixes.Find( found );
			int i2 = m_prefixes.Find( "[", i1+1 );
			if( i1 != -1 && i2 != -1 )
			{
				m_combo_attr.AddString( m_prefixes.Left(i2).Right(i2-i1) );
			}
			else
			{
				CString str2="";
				str2 += dSPACE;
				str2 += OLD_NO_FP;
				m_combo_attr.AddString( found+str2 );
				CStdioFile addWr;
				int ret = addWr.Open( m_app_dir+"prefixes.txt", CFile::modeWrite, NULL );
				if( ret )
				{
					addWr.SeekToEnd();
					if( bOpenTXTPreixes == 0 )
						addWr.WriteString( "\n" ); // needed
					addWr.WriteString( found );
					addWr.WriteString( "\n" );
					addWr.WriteString( OLD_NO_FP );
					addWr.WriteString( "\n" );
					addWr.Close();
					bOpenTXTPreixes = 1;

					m_prefixes += found.Right( found.GetLength()-1 );
					m_prefixes += dSPACE;
					m_prefixes += OLD_NO_FP;
					m_prefixes += "[";
				}
			}
		}
		Sprev = found;
	}
	for( int i=m_combo_attr.GetCount()-1; i>=0; i-- )
	{
		CString found;
		m_combo_attr.GetLBText( i, found );
		if( Sprev.Left(2) != found.Left(2) )
		{
			//CString Ins="*";
			//for( int ie=0; ie<15; ie++ )
			//	Ins += found.Left(2).Right(1) + "*";
			m_combo_attr.InsertString( i+1, "----------------------------------------------------------");
		}
		Sprev = found;
	}
	m_combo_attr.InsertString( 0, "----------------------------------------------------------");
	m_combo_attr.InsertString( m_combo_attr.GetCount(), "--> Edit class names ..." );
}

void CDlgImportPart::ReadPrefixes()
{
	if( m_app_dir.Right(1) != "\\" )
		m_app_dir += "\\";

	// now read prefixes
	struct _stat buf;
	CStdioFile rPrefix;
	int rf = rPrefix.Open( m_app_dir+"prefixes.txt", CFile::modeRead );
	if( rf )
	{
		m_prefixes = "";
		CString instr;
		while( rPrefix.ReadString( instr ) )
		{
			instr = instr.Trim();
			m_prefixes += instr;
			if( instr.Left(1) == "[" )
				m_prefixes += dSPACE; // NOT CHANGE IT
		}
		m_prefixes += "[";
		rPrefix.Close();
	}
	else if( _stat( m_app_dir+"prefixes.txt", &buf ) )
	{
		rf = rPrefix.Open( m_app_dir+"prefixes.txt", CFile::modeCreate | CFile::modeWrite, NULL );
		if( rf )
		{
			rPrefix.WriteString("[C]");
			rPrefix.WriteString("\n");
			rPrefix.WriteString("Capasitors");
			rPrefix.WriteString("\n");
			rPrefix.WriteString("[R]");
			rPrefix.WriteString("\n");
			rPrefix.WriteString("Resistors");
			rPrefix.WriteString("\n");
			rPrefix.Close();

			m_prefixes = "[C]  Capacitors[R]  Resistors[";
		}
		else
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Unable to open file prefixes.txt (Error code 9532)":
				"Невозможно открыть файл prefixes.txt (код ошибки 9532)", MB_ICONERROR);
	}
	else
		AfxMessageBox(G_LANGUAGE == 0 ?
			"Unable to open file prefixes.txt (Error code 4756)" :
			"Невозможно открыть файл prefixes.txt (код ошибки 4756)", MB_ICONERROR );
}


void CDlgImportPart::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ???????? ???? ??? ??????????? ????????? ??? ????? ????????????
	bDrawn = 0;
	this->SetTimer( 0, INT_MAX, 0 );
	OnClickList2();
	CDialog::OnTimer(nIDEvent);
}


void CDlgImportPart::OnBnClickedRunl()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	CString F="";
	int cur = m_part_list.GetCurSel();
	if( cur >= 0 )
	{
		m_part_list.GetText( cur, F );
		if( F.Left(1) != "[" )
			RemoveDescription( &F );
		int dog = F.Find("@");
		if( dog > 0 )
			F = F.Right( F.GetLength()  - dog - 1 );
		dog = F.Find("]");
		if( dog > 0 )
			F = F.Right( F.GetLength()-dog-1 );
		F = F.Trim();
	}
	else 
		return;


	if( theApp.m_Doc->m_footprint_extention.GetLength() == 0 )
	{
		CWnd * win = FindWindow(NULL, LibMan);
		if( win )
		{
			theApp.m_Doc->OnSendMess( win->GetSafeHwnd(), &F );
			win->SetForegroundWindow();
			return;
		}
		mb_runlib.EnableWindow(0);

		CString Mess;
		CString mem_str = theApp.m_Doc->m_full_lib_dir;
		if( FindFootprint( theApp.m_Doc, &F ) ==  0 )
		{		
			theApp.m_Doc->m_full_lib_dir = "";
			FindFootprintInFolder( theApp.m_Doc, &F, &F ); // Replace m_full_lib_dir folder to "LibMan" folder
		}
		if( FindFootprint( theApp.m_Doc, &F ) ==  0 ) // now m_full_lib_dir = "LibMan" folder
		{
			theApp.m_Doc->m_full_lib_dir = mem_str; // cancel replacement
			AfxMessageBox(G_LANGUAGE == 0 ? 
				(F+" not found in the current footprint library:\n\n"+theApp.m_Doc->m_full_lib_dir):
				(F + " не найден в текущей библиотеке футпринтов:\n\n" + theApp.m_Doc->m_full_lib_dir), MB_ICONERROR);
		}
		else
		{
			if( mem_str.Compare( theApp.m_Doc->m_full_lib_dir ) )
			{
				CString str;
				str.Format(G_LANGUAGE == 0 ? 
					("Warning! Footprint %s was not found "\
					"in the current footprint library folder, which "\
					"is specified in the project settings (Project >> Options):\n\n%s"\
					"\n\nFootprint %s found in another folder:\n\n%s\n\n"\
					"Want to set this folder as your footprint library folder?"):
					("Внимание! Футпринт %s не найден "\
					"в текущей папке библиотеки футпринтов, которая "\
					"указана в настройках проекта (Проект >> Настройки):\n\n%s"\
					"\n\nФутпринт %s найден в другой папке:\n\n%s\n\n"\
					"Хотите установить эту папку в качестве папки библиотеки футпринтов?"),
					F, mem_str, F, theApp.m_Doc->m_full_lib_dir ); 
				int retQ = AfxMessageBox( str, MB_ICONQUESTION|MB_YESNO );
				if( retQ == IDNO )
					theApp.m_Doc->m_full_lib_dir = mem_str;
			}
		}
		Mess.Format("SchConstr: \"%s\" \"%s\"", F, theApp.m_Doc->m_full_lib_dir );
		UpdatebRunLibText();
		HANDLE cmd = GlobalAlloc(GMEM_FIXED,2048);
		memcpy( cmd, Mess, Mess.GetLength()+1 );

		do Sleep(100);  
			while(!OpenClipboard());
		EmptyClipboard();
		SetClipboardData(CF_TEXT,cmd);
		CloseClipboard();
		CString folder = theApp.m_Doc->m_app_dir+"\\FPC_EXE\\FreePCB_Libraries";
		ShellExecute(	NULL,"open",folder+"\\ABC_library.exe",
										NULL,folder,SW_SHOWNORMAL);
	}
	else
	{
		CString m_lib = theApp.m_Doc->m_full_lib_dir + "\\" + F + theApp.m_Doc->m_footprint_extention;
		ShellExecute(	NULL, "open", m_lib, NULL, theApp.m_Doc->m_full_lib_dir, SW_SHOWNORMAL );
	}
}

void CDlgImportPart::UpdatebRunLibText()
{
	CString lib_str = theApp.m_Doc->m_full_lib_dir;
	if( lib_str.GetLength() > 40 )
	{
		lib_str = lib_str.Right(40);
		int rf = lib_str.Find("\\");
		if( rf > 0 )
			lib_str = lib_str.Right( lib_str.GetLength() - rf );
		lib_str = "  %" + lib_str;
	}
	if( lib_str.GetLength() == 0 )
		lib_str = "Path to footprint library not set !";
	//lib_str = "Check for footprint: "+ lib_str;
	mb_runlib.SetWindowTextA( lib_str );
}


void CDlgImportPart::CheckDocuments( CString * name, CString * file )
{
	COLORREF REF = RGB(255,255,255);

	int shiftX = 0, shiftY = 0;
	CButton * Cdoc;
	CString nm = *name;
	ExtractComponentName( &nm, NULL );
	CString path, defPath="";
	if( file == NULL )
		defPath = theApp.m_Doc->m_app_dir + main_component;
	else
	{
		int rf = file->ReverseFind('\\');
		if( rf > 0 )
			defPath = file->Left(rf) + rel_f_component;
	}
	DWORD i1 = m_description.GetEditSel();
	if( i1 < 5 )
		m_description.SetWindowTextA( "" );
	struct _stat buf;
	int pstep = -1;
	for( int step=0; step<MAX_ICONS; step++ )
	{
		m_doc_path[step] = "";
		if( step == 0 )
		{
			Cdoc = &m_doc1;
			path = defPath + "\\" + nm + ".pdf";
		}
		else if( step == 1 )
		{
			Cdoc = &m_doc2;
			path = defPath + "\\" + nm + ".doc";
		}
		else if( step == 2 )
		{
			Cdoc = &m_doc2;
			path = defPath + "\\" + nm + ".docx";
		}
		else if( step == 3 )
		{
			Cdoc = &m_doc3;
			path = defPath + "\\" + nm + ".txt";
		}
		else if( step == 4 )
		{
			Cdoc = &m_doc4;
			path = defPath + "\\" + nm + ".url";
		}
		if( _stat( path, &buf ) == 0 ) // file exists
		{
		    WORD bt1=1, bt2=2;
			CString DLL = path; 
			LPSTR lpStr = new char[MAX_PATH];//new015
			strcpy( lpStr, DLL.GetBufferSetLength(MAX_PATH-1) );
			HICON icon = ExtractAssociatedIcon( GetModuleHandle(NULL), lpStr, &bt1 );
			delete lpStr;//new015
			if( icon )
			{
				if( pstep == 1 && step == 2 )
					shiftX += 40;// cancel shifting
				Cdoc->SetWindowPos( NULL, (260+shiftX), (361+shiftY), 38, 44, 0 );
				DestroyIcon( Cdoc->SetIcon( icon ) );
				Cdoc->ShowWindow(1);
				Cdoc->BringWindowToTop();
				m_doc_path[step] = path;
				shiftX -= 40;
				pstep = step;
			}
			if( step == 3 && i1 < 5 )
			{
				CStdioFile txtf;
				int ok = txtf.Open( path, CFile::modeRead );
				if( ok )
				{
					CString instr;
					txtf.ReadString( instr );
					txtf.Close();
					instr = instr.Trim();
					//CT2A ascii(instr, 1251);
					//CStringA strA(instr);
					//m_description.SetFont( m_part_list.GetFont() );
					//m_description.SetLocale( MAKELCID( MAKELANGID( LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA ), SORT_DEFAULT ) );
					//CString newstr;
					//newstr.Format("%s",instr);
					m_description.SetWindowText( instr );
				}
			}
		}	
	}
}



void CDlgImportPart::OnBnClickedDoc1()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	if( m_doc_path[0].GetLength() )
	{
		CString P = "";
		int t = m_doc_path[0].ReverseFind('\\');
		if( t > 0 )
			P = m_doc_path[0].Left(t);
		if( P.Find( rel_f_component ) > 0 )
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Warning! This file is read-only":
				"Внимание! Этот файл доступен только для чтения", MB_ICONINFORMATION);
		ShellExecute(	NULL, "open", m_doc_path[0], NULL, P, SW_SHOWNORMAL );
	}
}


void CDlgImportPart::OnBnClickedDoc2()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	if( m_doc_path[1].GetLength() )
	{
		CString P = "";
		int t = m_doc_path[1].ReverseFind('\\');
		if( t > 0 )
			P = m_doc_path[1].Left(t);
		if( P.Find( rel_f_component ) > 0 )
			AfxMessageBox(G_LANGUAGE == 0 ?
				"Warning! This file is read-only" :
				"Внимание! Этот файл доступен только для чтения",MB_ICONINFORMATION);
		ShellExecute(	NULL, "open", m_doc_path[1], NULL, P, SW_SHOWNORMAL );
	}
	if( m_doc_path[2].GetLength() )
	{
		CString P = "";
		int t = m_doc_path[2].ReverseFind('\\');
		if( t > 0 )
			P = m_doc_path[2].Left(t);
		if( P.Find( rel_f_component ) > 0 )
			AfxMessageBox(G_LANGUAGE == 0 ?
				"Warning! This file is read-only" :
				"Внимание! Этот файл доступен только для чтения",MB_ICONINFORMATION);
		ShellExecute(	NULL, "open", m_doc_path[2], NULL, P, SW_SHOWNORMAL );
	}
}


void CDlgImportPart::OnBnClickedDoc3()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	if( m_doc_path[3].GetLength() )
	{
		CString P = "";
		int t = m_doc_path[3].ReverseFind('\\');
		if( t > 0 )
			P = m_doc_path[3].Left(t);
		if( P.Find( rel_f_component ) > 0 )
			AfxMessageBox(G_LANGUAGE == 0 ?
				"Warning! This file is read-only" :
				"Внимание! Этот файл доступен только для чтения",MB_ICONINFORMATION);
		ShellExecute(	NULL, "open", m_doc_path[3], NULL, P, SW_SHOWNORMAL );
	}
}

void CDlgImportPart::OnBnClickedDoc4()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	if( m_doc_path[4].GetLength() )
	{
		CString P = "";
		int t = m_doc_path[4].ReverseFind('\\');
		if( t > 0 )
			P = m_doc_path[4].Left(t);
		ShellExecute(	NULL, "open", m_doc_path[4], NULL, P, SW_SHOWNORMAL );
	}
}

void CDlgImportPart::OnBnClickedRadioSearchAllPages()
{
	
}


void CDlgImportPart::OnEnChangeDescription()
{
	// TODO:  ???? ??? ??????? ?????????? RICHEDIT, ?? ??????? ?????????? ?? ?????
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  ???????? ??? ???????? ??????????
	//DWORD i1 = m_description.GetEditSel();
	//if( i1 > 5 )
	{
		if( m_parts == NULL )
			return;
		if( m_parts->GetSize() == 0 )
			return;
		m_part_list.EnableWindow(0);
		m_part_list.ShowWindow(SW_HIDE);
		for( int i=m_part_list.GetCount()-1; i>=0; i-- )
			m_part_list.DeleteString(i);
		//
		CString text, defPath;
		m_description.GetWindowTextA( text );
		text.MakeLower();
		CArray<CString> texts;
		texts.RemoveAll();
		if( text.Find(" ") > 0 )
		{
			text = "key: " + text;
			CString key;
			ParseKeyString( &text, &key, &texts );
			if( texts.GetSize() >= 32 )
				texts.RemoveAll();
		}
		else
			texts.Add(text);
		defPath = theApp.m_Doc->m_app_dir + main_component;
		CFileFind finder;
		CString first_f = "\\*.txt";
		CString search_str = defPath + first_f;
		BOOL bWorking = finder.FindFile( search_str );
		//BOOL bWARN = 0;
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if( !finder.IsDirectory() && !finder.IsDots() )
			{
				CString instr;
				CString fp = finder.GetFilePath();
				CStdioFile txtf;
				int ok = txtf.Open( fp, CFile::modeRead, NULL );
				if( ok )
				{
					int found = 0;
					for( int itxt=0; itxt<texts.GetSize(); itxt++ )
						setbit( found, itxt );
					while( txtf.ReadString( instr ) )
					{
						if( instr.GetLength() == 0 )
							continue;
						instr.MakeLower();
						for( int itxt=0; found && itxt<texts.GetSize(); itxt++ )
							if( texts.GetAt(itxt).GetLength() )
								if( instr.Find( texts.GetAt(itxt) ) >= 0 )
									clrbit( found, itxt );
						if( found == 0 )
							break;
					}
					txtf.Close();
					if( found == 0 )
					{
						CString fn = finder.GetFileName();
						CString PartName = fn.Left(fn.GetLength()-4);
						//
						PartName.MakeLower();
						for( int i=m_parts->GetSize()-1; i>=0; i-- )
						{
							if( m_parts->GetAt(i).GetLength() < PartName.GetLength() )
								continue;
							CString gs = m_parts->GetAt(i);
							gs.MakeLower();
							ExtractComponentName( &gs, NULL );
							gs = " " + gs;
							if( gs.Find(" "+PartName) >= 0 )//==0!!!
							{
								if(instr.GetLength() > 30)
								{
									instr.Truncate(30);
									instr += "...";
								}
								instr += " ";
								//if( instr.FindOneOf("[]") >= 0 )
								//	bWARN = 1;
								m_part_list.AddString( instr + m_parts->GetAt(i) );
							}
						}
					}
				}
			}
		}
		//if( bWARN )
		//	AfxMessageBox("Text file attached to the component must not contain characters [ ]", MB_ICONINFORMATION);
		m_part_list.ShowWindow(SW_SHOW);
		m_part_list.EnableWindow(1);
	}
}
void CDlgImportPart::OnSelChangeDescription()
{
	int i2 = m_description.GetCurSel();
	if( i2 >= 0 )
	{
		CString file;
		m_description.GetLBText( i2, file );
		file = theApp.m_Doc->m_parent_folder + file;
		CArray<CString> * parts = m_library->GetPartlistByPath( &file );
		m_part_list.EnableWindow(0);
		m_part_list.ShowWindow(SW_HIDE);
		for( int i=m_part_list.GetCount()-1; i>=0; i-- )
			m_part_list.DeleteString(i);
		for( int i=0; i<parts->GetSize(); i++ )
			m_part_list.AddString( parts->GetAt(i) );
		m_part_list.ShowWindow(SW_SHOW);
		m_part_list.EnableWindow(1);
	}
}

void CDlgImportPart::OnBnClickedQ8()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("This combo box displays the first line of the text "\
		"file attached to the part. In a text file, you can store "\
		"any information related to this part. To attach a text file "\
		"to a part, close this window, select a part on the workspace, "\
		"and in the right-click menu, select \"Component Attachments\", then "\
		"the \"attach file\" submenu, and then \"create text file\"."\
		"\n\nIn this combo box, you can also search for parts "\
		"by the text fragment of the description. Enter a text fragment "\
		"of the file content and you will see that the "\
		"partlist will display those parts in the description of which "\
		"there is this text fragment. When you completely erase the "\
		"entered text fragment using the Backspace key, then all the "\
		"parts that have an attached text file will remain in the filter. "\
		"\nFor multiple filtration, enter text fragments of the file content through the space character"\
		"\n\nThe drop-down list of this combo box stores all schematic "\
		"projects in the working folder. Thus, you can see the  partlist "\
		"used in any of the projects. Select the file you are interested "\
		"in from the drop-down list and the project partlist will be "\
		"displayed in the filter."):
		("В этом поле со списком отображается первая строка текстового "\
		"файла, прикрепленного к детали. В текстовом файле можно хранить "\
		"любую информацию, связанную с этой деталью. Чтобы прикрепить текстовый файл "\
		"к детали, закройте это окно, выберите деталь на рабочем пространстве, "\
		"и в контекстном меню выберите \"Вложения компонента\", затем "\
		"подменю \"Прикрепить файл\", а затем \"Создать текстовый файл\"."\
		"\n\nВ этом поле со списком также можно выполнять поиск деталей "\
		"по текстовому фрагменту описания. Введите текстовый фрагмент "\
		"содержимого файла, и вы увидите, что в "\
		"списке деталей будут отображаться те детали, в описании которых "\
		"есть этот текстовый фрагмент.Когда вы полностью стираете "\
		"введенный текстовый фрагмент с помощью клавиши Backspace, то все "\
		"детали, к которым прикреплен текстовый файл, останутся в фильтре. "\
		"\nДля множественной фильтрации введите текстовые фрагменты содержимого файла через символ пробела"\
		"\n\nРаскрывающийся список этого поля со списком сохраняет все проекты схем "\
		"в рабочей папке.Таким образом, вы можете увидеть список деталей, "\
		"используемых в любом из проектов. Выберите интересующий вас файл "\
		"из раскрывающегося списка, и список деталей проекта будет "\
		"отображен в фильтре"), MB_ICONINFORMATION);
}


void CDlgImportPart::OnBnClickedDoc5()
{
	CString footprint_name;
	m_combo_attr.GetWindowTextA( footprint_name );
	int dog = footprint_name.Find("@");
	if( dog > 0 )
	{
		footprint_name = footprint_name.Right( footprint_name.GetLength()-dog-1 );
		ExtractComponentName( &footprint_name, NULL );
		theApp.m_Doc->ComponentImage( &footprint_name );
		OnClickList();
	}	
}

void CDlgImportPart::RemoveDescription( CString * str )
{
	for( int np=0; np<m_parts->GetSize(); np++ )
	{
		CString * S = &m_parts->GetAt(np);
		if( S->GetLength() < str->GetLength() )
			if( S->Compare( str->Right( S->GetLength() ) ) == 0 )
			{
				*str = str->Right( S->GetLength() );
				break;
			}
	}
}

void CDlgImportPart::OnBnClickedQu()
{
	// TODO: добавьте свой код обработчика уведомлений
	OnAboutUniqueParts();
}
