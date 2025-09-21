#include "stdafx.h"
#include "FreeSch.h"
#include "NetlistSettings.h"


// dialog

IMPLEMENT_DYNAMIC(CDlgNetlistSettings, CDialog)
CDlgNetlistSettings::CDlgNetlistSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNetlistSettings::IDD, pParent)
{
	m_pl = NULL;
	m_box_index = 0;
}  

CDlgNetlistSettings::~CDlgNetlistSettings()
{
}

void CDlgNetlistSettings::Ini( CPageList * pl, BOOL rnmb_present, CString * project_folder)
{
	m_pl = pl;
	m_box_index = m_pl->m_netlist_format;
	m_rnmb_present = rnmb_present;
	m_project_folder = *project_folder;
	if( m_rnmb_present )
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"It is highly recommended that you do not change "\
			"the netlist page map after renaming the part reference designations "\
			"in the project. You must first update the netlist in FreePcb-2.":
			"Настоятельно рекомендуется не изменять "\
			"карту страниц списка цепей после переименования обозначений деталей "\
			"в проекте. Сначала необходимо синхронизировать обозначения с печатной платой, обновив список цепей в ПлатФорм.", MB_ICONERROR);
}

void CDlgNetlistSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETLIST_TYPE, m_edit);
	DDX_Control(pDX, ID_ADD_PCB, m_add_pcb);
	DDX_Control(pDX, ID_REMOVE_PCB, m_del_pcb);
	DDX_Control(pDX, IDC_PCB_NAMES, m_edit_pcb_names);
	DDX_Control(pDX, IDC_CHECK_NL1, m_pages[0] );
	DDX_Control(pDX, IDC_CHECK_NL2, m_pages[1] );
	DDX_Control(pDX, IDC_CHECK_NL3, m_pages[2] );
	DDX_Control(pDX, IDC_CHECK_NL4, m_pages[3] );
	DDX_Control(pDX, IDC_CHECK_NL5, m_pages[4] );
	DDX_Control(pDX, IDC_CHECK_NL6, m_pages[5] );
	DDX_Control(pDX, IDC_CHECK_NL7, m_pages[6] );
	DDX_Control(pDX, IDC_CHECK_NL8, m_pages[7] );
	//
	if( pDX->m_bSaveAndValidate )
	{
		m_pl->m_netlist_format = m_edit.GetCurSel();
		if( m_edit_pcb_names.GetCount() )
		{
			m_pl->m_pcb_names.RemoveAll();
			for( int i=0; i<m_edit_pcb_names.GetCount(); i++ )
			{
				CString str;
				m_edit_pcb_names.GetLBText( i, str );
				m_pl->m_pcb_names.Add(str);
			}
		}
	}
	else
	{
		// pcb names
		for( int i=0; i<m_pl->m_pcb_names.GetSize(); i++ )
			m_edit_pcb_names.InsertString( m_edit_pcb_names.GetCount(), m_pl->m_pcb_names.GetAt(i) );
		m_current_i = m_pl->GetCurrentPcbIndex();
		m_edit_pcb_names.SetCurSel( m_current_i );
		//
		for( int ip=0; ip<m_pl->GetNumPages(); ip++ )
		{
			CString pN;
			m_pl->GetPageName( ip, &pN );
			m_pages[ip].SetWindowTextA( pN );
		}
		SetMask();
		m_edit.InsertString( 0, "PADS-PCB with values" );
		m_edit.InsertString( 1, "Tango netlist" );
		m_edit.SetCurSel(m_box_index);
	}
}


BEGIN_MESSAGE_MAP(CDlgNetlistSettings, CDialog)
	ON_BN_CLICKED( IDC_CHECK_NL1, OnCheck1 )
	ON_BN_CLICKED( IDC_CHECK_NL2, OnCheck2 )
	ON_BN_CLICKED( IDC_CHECK_NL3, OnCheck3 )
	ON_BN_CLICKED( IDC_CHECK_NL4, OnCheck4 )
	ON_BN_CLICKED( IDC_CHECK_NL5, OnCheck5 )
	ON_BN_CLICKED( IDC_CHECK_NL6, OnCheck6 )
	ON_BN_CLICKED( IDC_CHECK_NL7, OnCheck7 )
	ON_BN_CLICKED( IDC_CHECK_NL8, OnCheck8 )
	ON_BN_CLICKED( ID_ADD_PCB, OnAddPcb )
	ON_BN_CLICKED( ID_REMOVE_PCB, OnDeletePcb )
	ON_CBN_SELCHANGE( IDC_NETLIST_TYPE, OnCheck9 )
	ON_CBN_SELCHANGE( IDC_PCB_NAMES, OnCheck10 )
	ON_CBN_EDITCHANGE( IDC_PCB_NAMES, OnChange1 )
END_MESSAGE_MAP()

//
void CDlgNetlistSettings::OnCheck1()
{
	TryConnectPage(0);
}
void CDlgNetlistSettings::OnCheck2()
{
	TryConnectPage(1);
}
void CDlgNetlistSettings::OnCheck3()
{
	TryConnectPage(2);
}
void CDlgNetlistSettings::OnCheck4()
{
	TryConnectPage(3);
}
void CDlgNetlistSettings::OnCheck5()
{
	TryConnectPage(4);
}
void CDlgNetlistSettings::OnCheck6()
{
	TryConnectPage(5);
}
void CDlgNetlistSettings::OnCheck7()
{
	TryConnectPage(6);
}
void CDlgNetlistSettings::OnCheck8()
{
	TryConnectPage(7);
}
void CDlgNetlistSettings::OnCheck9()
{
	m_pl->m_netlist_format = m_edit.GetCurSel();
}
void CDlgNetlistSettings::OnCheck10()
{
	m_current_i = m_edit_pcb_names.GetCurSel();
	SetMask();
}
void CDlgNetlistSettings::OnChange1()
{
	CString str;
	m_edit_pcb_names.GetWindowTextA( str );
	m_current_i = -1;
	for( int i=0; i<m_pl->m_pcb_names.GetSize(); i++ )
		if( str.Compare( m_pl->m_pcb_names.GetAt(i) ) == 0 )
			m_current_i = i;
	SetMask();
}
//
//
void CDlgNetlistSettings::TryConnectPage( int i_page )
{
	if( m_pages[i_page].GetCheck() == 0 )
	{
		if( m_current_i >= 0 )
			clrbit( m_pl->m_netlist_page_mask[m_current_i], i_page );
		return;
	}
	//
	if( m_current_i >= 0 )
	{
		for( int i=0; i<m_pl->GetNumPages(); i++ )
			if( getbit( m_pl->m_netlist_page_mask[m_current_i], i ) )
			{
				m_pl->SwitchToPage( i );
				break;
			}
		setbit( m_pl->m_netlist_page_mask[m_current_i], i_page );
	}
	// after GetCheck
	SetMask();
	//
	CString Conf = dSPACE;
	int it = -1;
	int ip = i_page;
	int Count = 0;
	for( CText * t=m_pl->GetNextAttr( index_part_attr, &it, &ip, FALSE ); t; t=m_pl->GetNextAttr( index_part_attr, &it, &ip, FALSE ) )
	{
		if( ip != i_page )
			break;
		if( Count < 100 )
		{
			int pageOfpart = m_pl->FindPart( &t->m_str, NULL, TRUE, TRUE );
				pageOfpart = m_pl->FindPart( &t->m_str, NULL, FALSE, TRUE );
			if( pageOfpart != -1 )
			{
				Conf += t->m_str + dSPACE;
				Count++;
			}
		}
		else if( Count == 100 )
		{
			Conf += "(.......)";
			Count++;
		}
	}
	if( Conf.GetLength() != 2 )
	{
		CString Str;
		Str.Format(G_LANGUAGE == 0 ? 
			("Duplicate Reference Designators: %s\n\n"\
			"At the moment, it is impossible to include the page "\
			"in the netlist, because There will be a conflict of "\
			"part numbers on this page with other pages. Use the "\
			"\"Clear Part Numbers\" tool, having previously selected "\
			"all the parts on this page, and then try again. It is "\
			"recommended to select pages before starting work, and "\
			"then the renumbering tool will not allow duplication "\
			"of reference designations of parts."):
			("Повторяющиеся позиционные обозначения: %s\n\n"\
			"В данный момент невозможно включить эту страницу "\
			"в список соединений, т.к. возникнет конфликт "\
			"номеров деталей на этой странице с другими страницами. Воспользуйтесь инструментом "\
			"«Сбросить номера обозначений», предварительно выбрав "\
			"все детали на этой странице, а затем повторите попытку. Рекомендуется "\
			"выбирать страницы непосредственно при создании проекта, перед началом работы, и "\
			"тогда инструмент перенумерации не допустит дублирования "\
			"позиционных обозначений деталей"), Conf);
		AfxMessageBox( Str, MB_ICONERROR );
		if( m_current_i >= 0 )
			clrbit( m_pl->m_netlist_page_mask[m_current_i], i_page );
		m_pages[i_page].SetCheck(0);
	}
}

void CDlgNetlistSettings::SetMask()
{
	for( int ip=0; ip<MAX_PAGES; ip++ )
	{
		m_pages[ip].SetCheck(0);
		m_pages[ip].EnableWindow(0);
	}
	if( m_current_i >= 0 ) 
	{
		for( int ip=0; ip<m_pl->GetNumPages(); ip++ )
		{
			m_pages[ip].EnableWindow(1);
			if( m_current_i >= 0 )
				if( getbit( m_pl->m_netlist_page_mask[m_current_i], ip ) )
					m_pages[ip].SetCheck(1);
		}
		for( int ipcb=0; ipcb<m_pl->m_pcb_names.GetSize(); ipcb++ )
		{
			if( m_current_i != ipcb )
			{
				for( int ip=0; ip<m_pl->GetNumPages(); ip++ )
				{
					if( getbit( m_pl->m_netlist_page_mask[ipcb], ip ) )
					{
						if( m_pages[ip].GetCheck() )
							clrbit( m_pl->m_netlist_page_mask[m_current_i], ip );
						m_pages[ip].SetCheck(0);
						m_pages[ip].EnableWindow(0);
					}
				}
			}
		}
	}
}

void CDlgNetlistSettings::OnAddPcb()
{
	CString str;
	m_edit_pcb_names.GetWindowTextA( str );
	str.Trim();
	if( str.GetLength() == 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? "First enter new pcb name":"Сначала введите новое имя печатной платы.", MB_ICONWARNING);
		return;
	}
	if( str.Right(4) != ".fpc" && str.Right(4) != ".FPC" )
		str += ".fpc";
	for( int f=0; f<m_pl->m_pcb_names.GetSize(); f++ )
	{
		if( m_pl->m_pcb_names.GetAt(f).Compare( str ) == 0 )
		{
			AfxMessageBox(G_LANGUAGE == 0 ? 
				"This name is already in use. Please enter some other name":
				"Это имя уже используется. Введите другое имя", MB_ICONWARNING);
			return;
		}
	}
	AfxMessageBox(G_LANGUAGE == 0 ? 
		"PCB successfully connected to the project\n\n(If this file does not already exist in the project folder, then click the View >> Switch to PCB menu and the program will create it.)":
		"Печатная плата успешно подключена к проекту\n\n(Если этот файл еще не существует в папке проекта, нажмите меню «Вид >> Переключиться на редактор печатных плат», и программа создаст его)");
	m_pl->m_pcb_names.Add( str );
	m_edit_pcb_names.InsertString( m_edit_pcb_names.GetCount(), str );
	m_current_i = m_edit_pcb_names.GetCount() - 1;
	SetMask();
}

void CDlgNetlistSettings::OnDeletePcb()
{
	CString str;
	m_edit_pcb_names.GetWindowTextA( str );
	if( str.GetLength() == 0 )
	{
		return;
	}
	int ind = -1;
	for( int f=0; f<m_pl->m_pcb_names.GetSize(); f++ )
	{
		if( m_pl->m_pcb_names.GetAt(f).Compare( str ) == 0 )
		{
			ind = f;
			break;
		}
	}
	if( ind != -1 )
	{
		CString mess;
		mess.Format(G_LANGUAGE == 0 ? 
			"Are you sure you want to remove the %s from this list? (this will not affect the file in the folder)":
			"Вы уверены, что хотите удалить из этого списка %s? (это не затронет сам файл в папке)", str);
		int ret = AfxMessageBox( mess, MB_ICONQUESTION | MB_OKCANCEL );
		if( ret == IDOK )
		{
			remove(m_project_folder + freeasy_netlist + str + ".txt");
			m_pl->m_pcb_names.RemoveAt( ind );
			m_edit_pcb_names.DeleteString( ind );
			if( m_pl->m_pcb_names.GetSize() != m_edit_pcb_names.GetCount() )
				ASSERT(0);
		
			// shift netlist page mask
			for( int ii=ind; ii<MAX_PAGES; ii++ )
			{
				if( (ii+1) == MAX_PAGES )
					m_pl->m_netlist_page_mask[ii] = 0;
				else
					m_pl->m_netlist_page_mask[ii] = m_pl->m_netlist_page_mask[ii+1];
			}
			// reset m_current_i
			m_current_i = m_edit_pcb_names.GetCount()-1;
			m_edit_pcb_names.SetCurSel(m_current_i);
			SetMask();
		}
	}
}