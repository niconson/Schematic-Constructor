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
			AfxMessageBox(G_LANGUAGE == 0 ? "Empty string?":"Пустая строка?");
			pDX->Fail();
		}
		if (m_merge_name.Find("\"",0) >= 0)
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Illegal string. The name can not contain the quote character!":
				"Недопустимая строка. Имя не может содержать символ кавычек!");
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
					AfxMessageBox(G_LANGUAGE == 0 ? 
						"This page is already present, enter a different name!":
						"Такая страница уже существует, введите другое имя!", MB_ICONERROR);
					pDX->Fail();
				}
			}
		}
		else if( m_mode == M_READPAGE )
		{
			if( m_box_index == -1 )
			{
				AfxMessageBox(G_LANGUAGE == 0 ? "Illegal string!":"Недопустимая строка!");
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
					AfxMessageBox(G_LANGUAGE == 0 ? "Illegal object index!":"Недопустимый индекс объекта!");
					pDX->Fail();
				}
			}
		}
		else if( m_mode == M_COPY_FOOTPRINT )
		{
			if (m_merge_name.FindOneOf(ILLEGAL_FILENAME) >= 0 || m_merge_name.Find(" ",0) >= 0)
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					"Illegal string. The name can not contain a special characters and space character!":
					"Недопустимая строка. Имя не может содержать специальные символы и пробел!");
				pDX->Fail();
			}
		}
		else if (m_merge_name.Find(" ",0) >= 0)
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"Illegal string. The name can not contain a space character!":
				"Недопустимая строка. Имя не может содержать пробел!");
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
				if (G_LANGUAGE == 0)
				{
					::SetWindowText(wnd, "Select new pattern");
					m_stat.SetWindowTextA("Same as");
				}
				else
				{
					::SetWindowText(wnd, "Выбор образца детали");
					m_stat.SetWindowTextA("Как у...");
				}
			}
			else	
			{
				if (G_LANGUAGE == 0)
				{
					::SetWindowText(wnd, "Do not sync part attributes");
					m_stat.SetWindowTextA("Part");
				}
				else
				{
					::SetWindowText(wnd, "Исключение из синхронизации");
					m_stat.SetWindowTextA("Обозн.");
				}
			}
			int it = -1;
			for( CText * t=Doc->Attr->m_Reflist->GetNextText(&it); t; t=Doc->Attr->m_Reflist->GetNextText(&it) )
			{
				m_edit.AddString( t->m_str );
			}
			if (G_LANGUAGE == 0)
				m_edit.SetWindowText("*** choose ref des ***");
			else
				m_edit.SetWindowText("*** выберите деталь ***");
		}
		else if( m_mode == M_REPLACE_POLY )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			if (G_LANGUAGE == 0)
			{
				::SetWindowText(wnd, "Select new pattern");
				m_stat.SetWindowTextA("Same as");
			}
			else
			{
				::SetWindowText(wnd, "Выбор формы полилинии");
				m_stat.SetWindowTextA("Как у...");
			}
			for( int i=0; i<Doc->m_outline_poly->GetSize(); i++ )
			{
				CString nmb;
				nmb.Format( "%.4d", i+1 );
				m_edit.AddString( nmb );
			}
			if (G_LANGUAGE == 0)
				m_edit.SetWindowText("*** choose polyline ***");
			else
				m_edit.SetWindowText("*** выберите полилинию ***");
		}
		else if( m_mode == M_ADDPAGE || m_mode == M_RENAMEPAGE )
		{
			HWND wnd = CDialog::GetSafeHwnd();
			if( m_mode == M_ADDPAGE )
			{
				if (G_LANGUAGE == 0)
				{
					::SetWindowText(wnd, "Add new page");
					m_stat.SetWindowTextA("Name");
				}
				else
				{
					::SetWindowText(wnd, "Новая страница");
					m_stat.SetWindowTextA("Имя");
				}
			}
			else
			{
				CString str;
				//Doc->Pages.GetPageName( Doc->Pages.GetActiveNumber(), &cName );
				str.Format(G_LANGUAGE == 0?"Rename page":"Переименование страницы");
				::SetWindowText( wnd, str );
				m_stat.SetWindowTextA(G_LANGUAGE == 0 ? "New name":"Новое имя");
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
			::SetWindowText( wnd, G_LANGUAGE == 0 ? "Select page you want to insert":"Выберите страницу для вставки");
			m_stat.SetWindowTextA(G_LANGUAGE == 0 ? "Name":"Имя");
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
				::SetWindowText( wnd, G_LANGUAGE == 0 ? "Remove RefList":"Удаление списка обозначений");
			}
			else
				::SetWindowText( wnd, G_LANGUAGE == 0 ? "Add new RefList":"Создание списка обозначений");
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
			::SetWindowText( wnd, G_LANGUAGE == 0 ? "Remove one of your favorite nets":"Удаление из избранных");
			m_stat.SetWindowTextA(G_LANGUAGE == 0 ? "Select":"Выберите");
		}
		else if( m_mode == M_COPY_FOOTPRINT )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, G_LANGUAGE == 0 ? "Save footprint":"Сохранение футпринта");
		}
		else if( m_mode == M_URL )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, G_LANGUAGE == 0 ? "Component URL":"URL компонента");
			m_stat.SetWindowTextA(G_LANGUAGE == 0 ? "Enter line":"Адрес");
		}
		else if( m_mode == M_SET_POLY_LENGTH )
		{
			m_edit.SetWindowText(m_merge_name);
			HWND wnd = CDialog::GetSafeHwnd();
			::SetWindowText( wnd, G_LANGUAGE == 0 ? "Set segment length":"Длина сегмента");
			m_stat.SetWindowTextA(G_LANGUAGE == 0 ? "Enter value":"Введите");
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgAddMerge, CDialog)
END_MESSAGE_MAP()


// CDlgAddWidth message handlers
