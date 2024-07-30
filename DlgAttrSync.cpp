#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAttrSync.h"
#include "DlgAddMerge.h"

// dialog

IMPLEMENT_DYNAMIC(CDlgAttrSync, CDialog)
CDlgAttrSync::CDlgAttrSync(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAttrSync::IDD, pParent)
{
	m_pl = NULL;
	m_box_index = 0;
}  

CDlgAttrSync::~CDlgAttrSync()
{
}

void CDlgAttrSync::Ini( CFreePcbDoc * doc )
{
	m_doc = doc;
	m_pl = &doc->Pages;
}

void CDlgAttrSync::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MP1, m_page[0] );
	DDX_Control(pDX, IDC_MP2, m_page[1] );
	DDX_Control(pDX, IDC_MP3, m_page[2] );
	DDX_Control(pDX, IDC_MP4, m_page[3] );
	DDX_Control(pDX, IDC_MP5, m_page[4] );
	DDX_Control(pDX, IDC_MP6, m_page[5] );
	DDX_Control(pDX, IDC_MP7, m_page[6] );
	DDX_Control(pDX, IDC_MP8, m_page[7] );
	DDX_Control(pDX, IDC_CHECK_MP1, m_mirror[0] );
	DDX_Control(pDX, IDC_CHECK_MP2, m_mirror[1] );
	DDX_Control(pDX, IDC_CHECK_MP3, m_mirror[2] );
	DDX_Control(pDX, IDC_CHECK_MP4, m_mirror[3] );
	DDX_Control(pDX, IDC_CHECK_MP5, m_mirror[4] );
	DDX_Control(pDX, IDC_CHECK_MP6, m_mirror[5] );
	DDX_Control(pDX, IDC_CHECK_MP7, m_mirror[6] );
	DDX_Control(pDX, IDC_CHECK_MP8, m_mirror[7] );
	DDX_Control(pDX, IDC_EXCL_LIST, m_excl );
	if( pDX->m_bSaveAndValidate )
	{
		m_pl->m_page_mirror_mask = 0;
		for( int i=0; i<MAX_PAGES; i++ )
			if( m_page[i].GetCheck() )
				setbit( m_pl->m_page_mirror_mask, i );
		for( int i=0; i<MAX_PAGES; i++ )
			if( m_mirror[i].GetCheck() )
				setbit( m_pl->m_page_mirror_mask, i+MAX_PAGES );
		for( int pg=0; pg<m_pl->GetNumPages(); pg++ )
		{
			CTextList * tl = m_pl->GetTextList(pg,index_part_attr);
			int it = -1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			{
				clrbit( t->m_pdf_options, SYNC_EXCLUDE );
			}
		}
		for( int pg=0; pg<m_pl->GetNumPages(); pg++ )
		{
			if( getbit( m_pl->m_page_mirror_mask, pg ) == 0 )
				continue;
			CTextList * tl = m_pl->GetTextList(pg,index_part_attr);
			int it = -1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			{
				int i = m_excl.FindStringExact(-1,t->m_str);
				if( i >= 0 )
					setbit( t->m_pdf_options, SYNC_EXCLUDE );
			}
			break;
		}
	}
	else
	{
		for( int ip=m_pl->GetNumPages(); ip<MAX_PAGES; ip++ )
		{
			m_page[ip].SetCheck(0);
			m_page[ip].EnableWindow(0);
			m_mirror[ip].SetCheck(0);
			m_mirror[ip].EnableWindow(0);
		}
		int present = 0;
		for( int ip=0; ip<m_pl->GetNumPages(); ip++ )
		{
			CString pN;
			m_pl->GetPageName( ip, &pN );
			m_page[ip].SetWindowTextA( pN );
			m_mirror[ip].SetWindowTextA( pN );
			m_page[ip].EnableWindow(1);
			m_mirror[ip].EnableWindow(1);
			if( getbit( m_pl->m_page_mirror_mask, ip ))
			{
				present = 1;
				m_page[ip].SetCheck(1);
			}
			else 
				m_page[ip].SetCheck(0);
			if( getbit( m_pl->m_page_mirror_mask, ip+MAX_PAGES ))
				m_mirror[ip].SetCheck(1);
			else  
				m_mirror[ip].SetCheck(0);
		}
		if( present == 0 ) 
		{
			PWINDOWINFO pwi;
			for( int i=0; i<MAX_PAGES; i++ )
			{
				if( m_pl->IsThePageIncludedInNetlist( i ) == 0 ) 
					if( i < m_pl->GetNumPages() )
					{
						m_page[i].SetCheck(1);
						break;
					}
			}
		}
		OnCheck();
		for( int pg=0; pg<m_pl->GetNumPages(); pg++ )
		{
			CTextList * tl = m_pl->GetTextList(pg,index_part_attr);
			int it = -1;
			for( CText * t=tl->GetNextText(&it); t; t=tl->GetNextText(&it) )
			{
				if( getbit( t->m_pdf_options, SYNC_EXCLUDE ) )
					if( m_excl.FindStringExact(-1,t->m_str) == -1 )
						m_excl.AddString(t->m_str);
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgAttrSync, CDialog)
	ON_BN_CLICKED( IDC_MP1, OnCheck )
	ON_BN_CLICKED( IDC_MP2, OnCheck )
	ON_BN_CLICKED( IDC_MP3, OnCheck )
	ON_BN_CLICKED( IDC_MP4, OnCheck )
	ON_BN_CLICKED( IDC_MP5, OnCheck )
	ON_BN_CLICKED( IDC_MP6, OnCheck )
	ON_BN_CLICKED( IDC_MP7, OnCheck )
	ON_BN_CLICKED( IDC_MP8, OnCheck )
	ON_BN_CLICKED( IDC_CHECK_MP1, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP2, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP3, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP4, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP5, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP6, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP7, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_MP8, OnCheck2 )
	ON_BN_CLICKED( ID_Q1, OnQ1 )
	ON_BN_CLICKED( ID_Q2, OnQ2 )
	ON_BN_CLICKED( ID_ADD_NEW_REF, OnAddRef )
	ON_BN_CLICKED( ID_REMOVE_REF, OnRemove )
END_MESSAGE_MAP()

void CDlgAttrSync::OnCheck()
{
	for( int i=0; i<m_pl->GetNumPages(); i++ )
		if( m_page[i].GetCheck() )
		{
			m_mirror[i].SetCheck(0);
			m_mirror[i].EnableWindow(0);
		}
		else //if( getbit( m_pl->m_netlist_page_mask, i ) == 0 )
			m_mirror[i].EnableWindow(1);
}
void CDlgAttrSync::OnCheck2()
{
	static int bFirst = 1;
	if( bFirst )
		for( int i=0; i<m_pl->GetNumPages(); i++ )
			if( getbit( m_pl->m_page_mirror_mask, MAX_PAGES+i ) == 0 && m_mirror[i].GetCheck() )
			{
				bFirst = 0;
				AfxMessageBox(G_LANGUAGE == 0 ? 
					("By enabling this option, you set the synchronization of part "\
					"attributes between this page and the main page. This means that the attributes "\
					"\"value\", \"footprint\", as well as the names of the pins will be the same "\
					"for the same reference designators. Before proceeding, "\
					"make sure that the pages are selected correctly."):
					("¬ключа€ эту опцию, вы устанавливаете синхронизацию атрибутов детали "\
					"между этой страницей и главной страницей. Ёто означает, что атрибуты "\
					"\"название\", \"футпринт\", а также имена пинов будут одинаковыми "\
					"дл€ идентичных позиционных обозначений. ѕрежде чем продолжить, "\
					"убедитесь, что страницы выбраны правильно."));
			}
}
void CDlgAttrSync::OnAddRef()
{
	CDlgAddMerge dlg;
	dlg.m_mode = dlg.M_ADD_REF;
	dlg.Doc = m_doc;
	int ret = dlg.DoModal();
	if( ret == IDOK )
	{
		dlg.m_merge_name.Trim();
		if( dlg.m_merge_name.GetLength() )
			m_excl.AddString( dlg.m_merge_name );
	}
}

void CDlgAttrSync::OnRemove()
{
	for( int cs=m_excl.GetCount()-1; cs>=0; cs-- )
		if( m_excl.GetSel(cs) )
			m_excl.DeleteString(cs);
	if( m_excl.GetCount() )
		m_excl.SetCurSel(0);
}

void CDlgAttrSync::OnQ1()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("If on any page you have a modified copy of the layout "\
		"of another page, then you can configure the synchronization "\
		"of the value and footprint attributes for these pages. "\
		"Select the main page on the left and the destination page on the right. "\
		"As soon as you change the part attribute on the main page, "\
		"it will also change on the selected pages, provided "\
		"that the reference designators are the same."):
		("≈сли на какой-либо странице у вас есть видоизмененна€ копи€ схемы "\
		"другой страницы, то вы можете настроить синхронизацию "\
		"атрибутов значени€ и футпринта дл€ этих страниц. "\
		"¬ыберите главную страницу слева и страницу назначени€ справа. "\
		" ак только вы измените атрибут детали на главной странице, "\
		"он также изменитс€ на выбранных страницах, при условии, "\
		"что ссылочные обозначени€ одинаковы."), MB_ICONINFORMATION);
}
void CDlgAttrSync::OnQ2()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("If the page is included in the netlist, then it cannot be selected, "\
		"because interpage duplication of reference designators for these pages is not allowed."):
		("≈сли страница включена в список эл.цепей, то ее нельз€ выбрать, "\
		"поскольку межстраничное дублирование ссылочных обозначений дл€ этих страниц не допускаетс€."), MB_ICONINFORMATION);
}