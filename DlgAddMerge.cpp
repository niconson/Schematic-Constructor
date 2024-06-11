// DlgAddMerge.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAddMerge.h"
#define MAX_NAME_SIZE 59

// CDlgAddMergeName dialog

IMPLEMENT_DYNAMIC(CDlgAddMerge, CDialog)
CDlgAddMerge::CDlgAddMerge(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddMerge::IDD, pParent)
{
	Doc = NULL;
	m_merge_name = "";
	m_mode = M_MERGES;
}  

CDlgAddMerge::~CDlgAddMerge()
{
}

void CDlgAddMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MERGE_NAME, m_edit);
	DDX_Control(pDX, IDC_STATIC_M_TEXT, m_stat);
	if( pDX->m_bSaveAndValidate )
	{
		m_edit.GetWindowText(m_merge_name);
		m_merge_name = m_merge_name.Trim();
		m_box_index = m_edit.GetCurSel();
		if( m_merge_name.GetLength() == 0 && m_mode != M_URL )
		{
			AfxMessageBox( "Empty string?" );
			pDX->Fail();
		}
		if (m_merge_name.Find("\"",0) >= 0)
		{
			AfxMessageBox( "Illegal string. The name can not contain the quote character!" );
			pDX->Fail();
		}
		if( m_mode == M_ADDPAGE || m_mode == M_RENAMEPAGE )
		{
			CString str;
			for( int i=0; i<m_edit.GetCount(); i++ )
			{
				m_edit.SetCurSel(i);
				m_edit.GetWindowTextA(str);
				str = str.Trim();
				if( str.CompareNoCase( m_merge_name ) == 0 )
				{
					AfxMessageBox( "This page is already present, enter a different name!", MB_ICONERROR );
					pDX->Fail();
				}
			}
		}
		else if( m_mode == M_READPAGE )
		{
			if( m_box_index == -1 )
			{
				AfxMessageBox( "Illegal string!" );
				pDX->Fail();
			}
		}
		else if( m_mode == M_REPLACE_PATT || m_mode == M_REPLACE_POLY || m_mode == M_REPLACE_FAV )
		{
			int F = 1;
			if( m_box_index >= 0 )
			{
				CString gstr;
				m_edit.GetLBText( m_box_index, gstr );
				if( m_merge_name.Compare( gstr ) == 0 )
				{
					F = 0;
				}
			}
			if( F )
			{
				if( m_mode == M_REPLACE_POLY )
					m_box_index = my_atoi(&m_merge_name)-1;
				else
					m_box_index = m_edit.FindStringExact( -1, m_merge_name );
				if( m_box_index < 0 || m_box_index >= m_edit.GetCount() )
				{
					AfxMessageBox( "Illegal object index!" );
					pDX->Fail();
				}
			}
		}
		else if( m_mode == M_COPY_FOOTPRINT )
		{
			if (m_merge_name.FindOneOf(ILLEGAL_FILENAME) >= 0 || m_merge_name.Find(" ",0) >= 0)
			{
				AfxMessageBox( "Illegal string. The name can not contain a special characters and space character!" );
				pDX->Fail();
			}
		}
		else if (m_merge_name.Find(" ",0) >= 0)
		{
			AfxMessageBox( "Illegal string. The name can not contain a space character!" );
			pDX->Fail();
		}
		else if( m_mode != M_URL )
			m_merge_name = m_merge_name.Left(MAX_NAME_SIZE);
	}
	else
	{
		for( int i=0; i<m_combo_box.GetSize(); i++ )
			m_edit.AddString( m_combo_box.GetAt(i) );

		if( m_mode == M_MERGES )
		{
			CString s;
			int num=0;	
			if( m_merge_name.GetLength() == 0 )
			{
				m_merge_name = "Merge-1";
				while( Doc->m_mlist->GetIndex(m_merge_name) >= 0 )
				{
					num++;
					::MakeCStringFromDimension( 1.0, &s, num, NM, FALSE, FALSE, FALSE, 0 );
					m_merge_name.Format( "Merge-%s", s );
				}
			}
			m_edit.AddString("None");
			for( int im=0; im<Doc->m_mlist->GetSize(); im++ )
			{
				m_edit.AddString( Doc->m_mlist->GetStr(im) );
			}
			m_edit.SetWindowText(m_merge_name);
		}
		else if( m_mode == M_REPLACE_PATT || m_mode == M_ADD_REF )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			if( m_mode == M_REPLACE_PATT )
			{
				::SetWindowText( wnd, "Select new pattern" );
				m_stat.SetWindowTextA( "Same as" );
			}
			else	
			{
				::SetWindowText( wnd, "Do not sync part attributes" );
				m_stat.SetWindowTextA( "Part" );
			}
			int it = -1;
			for( CText * t=Doc->Attr->m_Reflist->GetNextText(&it); t; t=Doc->Attr->m_Reflist->GetNextText(&it) )
			{
				m_edit.AddString( t->m_str );
			}
			m_edit.SetWindowText("*** choose ref des ***");
		}
		else if( m_mode == M_REPLACE_POLY )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Select new pattern" );
			m_stat.SetWindowTextA( "Same as" );
			for( int i=0; i<Doc->m_outline_poly->GetSize(); i++ )
			{
				CString nmb;
				nmb.Format( "%.4d", i+1 );
				m_edit.AddString( nmb );
			}
			m_edit.SetWindowText("*** choose polyline ***");
		}
		else if( m_mode == M_ADDPAGE || m_mode == M_RENAMEPAGE )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			if( m_mode == M_ADDPAGE )
			{
				::SetWindowText( wnd, "Add new page" );
				m_stat.SetWindowTextA( "Name" );
			}
			else
			{
				CString str;
				//Doc->Pages.GetPageName( Doc->Pages.GetActiveNumber(), &cName );
				str.Format("Rename page");
				::SetWindowText( wnd, str );
				m_stat.SetWindowTextA( "New name" );
			}
			//
			int it = -1;
			for( int p=0; p<Doc->Pages.GetNumPages(); p++ )
			{
				CString pN;
				Doc->Pages.GetPageName(p, &pN);
				m_edit.InsertString( p, pN );
			}
			m_edit.SetWindowText("");
		}
		else if( m_mode == M_READPAGE )
		{
			CString key;
			CArray<CString> pks;
			ParseKeyString( &m_merge_name, &key, &pks );
			for( int p=0; p<pks.GetSize(); p++ )
				m_edit.InsertString( p, pks[p] );
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Select page you want to insert" );
			m_stat.SetWindowTextA( "Name" );
		}
		else if( m_mode == M_REF_LIST || m_mode == M_REM_LIST )
		{
			CString s;
			int num=0;	
			if( m_merge_name.GetLength() == 0 )
			{
				m_merge_name = "RefList-1";
				while( Doc->m_ref_lists->GetIndex(m_merge_name) >= 0 )
				{
					num++;
					::MakeCStringFromDimension( 1.0, &s, num, NM, FALSE, FALSE, FALSE, 0 );
					m_merge_name.Format( "RefList-%s", s );
				}
			}
			for( int im=0; im<Doc->m_ref_lists->GetSize(); im++ )
			{
				m_edit.AddString( Doc->m_ref_lists->GetStr(im) );
			}
			HWND wnd = CDialog::GetSafeHwnd();
			//
			if( m_mode == M_REM_LIST )
			{
				m_merge_name = "";
				::SetWindowText( wnd, "Remove RefList" );
			}
			else
				::SetWindowText( wnd, "Add new RefList" );
			m_edit.SetWindowText(m_merge_name);
		}
		else if( m_mode == M_REPLACE_FAV )
		{
			for( int p=0; p<MAX_FAVORITES; p++ )
			{
				CArray<CPolyLine> * p_arr = Doc->Pages.GetPolyArray( Doc->m_view->m_favorite_page[p] );
				if( Doc->m_view->m_favorite_poly[p] >= 0 && Doc->m_view->m_favorite_poly[p] < p_arr->GetSize() )
				{
					CText * net = p_arr->GetAt( Doc->m_view->m_favorite_poly[p] ).Check( index_net_attr );
					if( net )
						m_edit.InsertString( p, net->m_str );
					else
						m_edit.InsertString( p, "no net" );
				}
				else 
				{
					m_edit.InsertString( p, "index error" );
					Doc->m_view->m_favorite_poly[p] = -1;
				}
			}
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Remove one of your favorite nets" );
			m_stat.SetWindowTextA( "Select" );
		}
		else if( m_mode == M_COPY_FOOTPRINT )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Save footprint" );
		}
		else if( m_mode == M_URL )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Component URL" );
			m_stat.SetWindowTextA( "Enter line" );
		}
		else if( m_mode == M_SET_POLY_LENGTH )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, "Set segment length" );
			m_stat.SetWindowTextA( "Enter value" );
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgAddMerge, CDialog)
END_MESSAGE_MAP()


// CDlgAddWidth message handlers
