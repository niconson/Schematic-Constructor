// DlgVia.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAttr.h"


// CDlgAttr dialog

IMPLEMENT_DYNAMIC(CDlgAttr, CDialog)
CDlgAttr::CDlgAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAttr::IDD, pParent)
{
}

CDlgAttr::~CDlgAttr()
{
}

void CDlgAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_OPEN_LIB,	m_open_lib);
	DDX_Control(pDX, IDC_REFDES,	m_edit_ref);
	DDX_Control(pDX, IDC_PINNAME,	m_edit_pin);
	DDX_Control(pDX, IDC_LIST_DESCRIPTION,	m_edit_pindesc);
	DDX_Control(pDX, IDC_NETNAME,	m_edit_netname);
	DDX_Control(pDX, IDC_FNAME,		m_edit_fname);
	DDX_Control(pDX, IDC_PARTVALUE, m_edit_value);
	DDX_Control(pDX, IDC_ATTR_APPLY_REF, m_check_apply_ref);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_REF, m_edit_height_ref);
	DDX_Control(pDX, IDC_ATTR_WIDTH_REF, m_edit_width_ref);
	DDX_Control(pDX, IDC_ATTR_APPLY_PIN, m_check_apply_pin);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_PIN, m_edit_height_pin);
	DDX_Control(pDX, IDC_ATTR_WIDTH_PIN, m_edit_width_pin);
	DDX_Control(pDX, IDC_ATTR_APPLY_NET, m_check_apply_net);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_NET, m_edit_height_net);
	DDX_Control(pDX, IDC_ATTR_WIDTH_NET, m_edit_width_net);
	DDX_Control(pDX, IDC_ATTR_APPLY_DESC, m_check_apply_pdesc);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_DESC, m_edit_height_pdesc);
	DDX_Control(pDX, IDC_ATTR_WIDTH_DESC, m_edit_width_pdesc);
	DDX_Control(pDX, IDC_ATTR_APPLY_FOOT, m_check_apply_foot);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_FOOT, m_edit_height_foot);
	DDX_Control(pDX, IDC_ATTR_WIDTH_FOOT, m_edit_width_foot);
	DDX_Control(pDX, IDC_ATTR_APPLY_VALUE, m_check_apply_value);
	DDX_Control(pDX, IDC_ATTR_HEIGHT_VALUE, m_edit_height_value);
	DDX_Control(pDX, IDC_ATTR_WIDTH_VALUE, m_edit_width_value);
	for( int ib=0; ib<7; ib++ )
		DDX_Control(pDX, ID_CMD1+ib, Bcmd[ib]);
	//
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		if( getbit( m_mask, index_net_attr ) == 0 )
			Bcmd[4].EnableWindow(0);
		if( getbit( m_mask, index_desc_attr ) == 0 )
		{
			Bcmd[0].EnableWindow(0);
			Bcmd[1].EnableWindow(0);
			Bcmd[2].EnableWindow(0);
			Bcmd[3].EnableWindow(0);
			Bcmd[5].EnableWindow(0);
			Bcmd[6].EnableWindow(0);
		}
		//
		m_edit_ref.SetWindowTextA( m_ref );
		m_edit_pin.SetWindowTextA( m_pin );
		m_edit_pindesc.LimitText( CLR_REF );
		m_edit_pindesc.SetWindowTextA( m_pindesc );
		m_edit_netname.SetWindowTextA( m_net );
		m_edit_fname.SetWindowTextA( m_fname );
		m_edit_value.SetWindowTextA( m_partv );
		if( m_ref.GetLength() == 0 )
			m_check_apply_ref.SetCheck(1);
		else
			m_check_apply_ref.SetCheck(0);
		if( m_pin.GetLength() == 0 )
			m_check_apply_pin.SetCheck(1);
		else
			m_check_apply_pin.SetCheck(0);
		if( m_net.GetLength() == 0 )
			m_check_apply_net.SetCheck(1);
		else
			m_check_apply_net.SetCheck(0);
		if( m_pindesc.GetLength() == 0 )
			m_check_apply_pdesc.SetCheck(1);
		else
			m_check_apply_pdesc.SetCheck(0);
		if( m_fname.GetLength() == 0 )
			m_check_apply_foot.SetCheck(1);
		else
			m_check_apply_foot.SetCheck(0);
		if( m_partv.GetLength() == 0 )
			m_check_apply_value.SetCheck(1);
		else
			m_check_apply_value.SetCheck(0);
		CString str;
		// ref
		str.Format( "%6.2f", m_Asize.H_ref );
		m_edit_height_ref.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_ref );
		m_edit_width_ref.SetWindowTextA( str );
		// pin
		str.Format( "%6.2f", m_Asize.H_pin );
		m_edit_height_pin.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_pin );
		m_edit_width_pin.SetWindowTextA( str );
		// net
		str.Format( "%6.2f", m_Asize.H_net );
		m_edit_height_net.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_net );
		m_edit_width_net.SetWindowTextA( str );
		// dsc
		str.Format( "%6.2f", m_Asize.H_pindesc );
		m_edit_height_pdesc.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_pindesc );
		m_edit_width_pdesc.SetWindowTextA( str );
		// foot
		str.Format( "%6.2f", m_Asize.H_foot );
		m_edit_height_foot.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_foot );
		m_edit_width_foot.SetWindowTextA( str );
		// value
		str.Format( "%6.2f", m_Asize.H_value );
		m_edit_height_value.SetWindowTextA( str );
		str.Format( "%6.2f", m_Asize.W_value );
		m_edit_width_value.SetWindowTextA( str );
		//
		int it = -1;
		if( m_att->m_Reflist->GetText( &m_ref, &it ) )		// duplication
		{
			if( m_att->m_Reflist->GetText( &m_ref, &it ) )	// duplication
			{
				AfxMessageBox(G_LANGUAGE == 0 ? 
					("Group editing of parts is not possible when the circuit contains the same part designations !\n"\
					"You can use the automatic renumbering tool"):
					("Групповое редактирование деталей невозможно, если схема содержит одинаковые обозначения деталей!\n"\
					"Вы можете использовать инструмент автоматической перенумерации"), MB_ICONERROR);
				pDX->Fail();
			}
		}
		
		// loading PREFIXES
		CString pref = dSPACE;
		if( m_prefixes.GetLength() == 0 )
		{
			CString pr = dSPACE;
			CString sf;
			it = -1;
			for( CText * gT=m_att->m_Reflist->GetNextText(&it); gT; gT=m_att->m_Reflist->GetNextText(&it) )
			{
				if( gT->m_str.GetLength() > pr.GetLength() )
					if( gT->m_str.GetAt(pr.GetLength()) >= '0' && gT->m_str.GetAt(pr.GetLength()) <= '9' )
						if( pr.Compare( gT->m_str.Left(pr.GetLength()) ) == 0 )
							continue;
				ParseRef( &gT->m_str, &pr, &sf );
				if( pref.Find( " " + pr + " " ) <= 0 )
				{
					m_edit_ref.AddString( pr );
					pref += pr + " ";
				}
			}
		}
		else
		{
			for( int iof=m_prefixes.Find( "[" ); iof >= 0; iof=m_prefixes.Find( "[", iof+1 ) )
			{
				int iof2 = m_prefixes.Find( "[", iof+1 );
				if( iof2 != -1 )
				{
					m_edit_ref.AddString( m_prefixes.Left(iof2).Right(iof2-iof) );
				}
			}
		}

		// loading VALUES
		it = -1;
		pref = dSPACE;
		for( CText * gT=m_att->m_Valuelist->GetNextText(&it); gT; gT=m_att->m_Valuelist->GetNextText(&it) )
		{
			if( pref.Find( " " + gT->m_str + " " ) <= 0 )
			{
				m_edit_value.AddString( gT->m_str );
				pref += gT->m_str + " ";
			}
		}

		// loading FOOTPRINT NAMES
		for( int fi=0; fi<m_F->GetSize(); fi++ )
			m_edit_fname.AddString( m_F->GetAt(fi) );

		// loading NET NAMES
		it = -1;
		pref = dSPACE;
		for( CText * gT=m_att->m_Netlist->GetNextText(&it); gT; gT=m_att->m_Netlist->GetNextText(&it) )
		{
			if( pref.Find( " " + gT->m_str + " " ) <= 0 )
			{
				m_edit_netname.AddString( gT->m_str );
				pref += gT->m_str + " ";
			}
		}

		// loading PIN NAMES
		it = -1;
		pref = dSPACE;
		for( CText * gT=m_att->m_Pinlist->GetNextText(&it); gT; gT=m_att->m_Pinlist->GetNextText(&it) )
		{
			if( pref.Find( " " + gT->m_str + " " ) <= 0 )
			{
				m_edit_pin.AddString( gT->m_str );
				pref += gT->m_str + " ";
			}
		}
		OnChange();
	}
	else
	{
		// outgoing
		m_edit_ref.GetWindowTextA( m_ref );
		m_edit_pin.GetWindowTextA( m_pin );
		m_edit_pindesc.GetWindowTextA( m_pindesc );
		m_edit_netname.GetWindowTextA( m_net );
		m_edit_fname.GetWindowTextA( m_fname );
		m_edit_value.GetWindowTextA( m_partv );
		m_ref = m_ref.Trim();
		m_pin = m_pin.Trim();
		m_pindesc = m_pindesc.TrimRight();
		m_net = m_net.Trim();
		m_fname = m_fname.Trim();
		m_partv = m_partv.Trim();
		CString str;
		// ref
		m_Asize.app_ref = m_check_apply_ref.GetCheck();
		m_edit_height_ref.GetWindowTextA( str );
		m_Asize.H_ref = my_atof(&str);
		m_edit_width_ref.GetWindowTextA( str );
		m_Asize.W_ref = my_atof(&str);
		// pin
		m_Asize.app_pin = m_check_apply_pin.GetCheck();
		m_edit_height_pin.GetWindowTextA( str );
		m_Asize.H_pin = my_atof(&str);
		m_edit_width_pin.GetWindowTextA( str );
		m_Asize.W_pin = my_atof(&str);
		// net
		m_Asize.app_net = m_check_apply_net.GetCheck();
		m_edit_height_net.GetWindowTextA( str );
		m_Asize.H_net = my_atof(&str);
		m_edit_width_net.GetWindowTextA( str );
		m_Asize.W_net = my_atof(&str);
		// desc
		m_Asize.app_pindesc = m_check_apply_pdesc.GetCheck();
		m_edit_height_pdesc.GetWindowTextA( str );
		m_Asize.H_pindesc = my_atof(&str);
		m_edit_width_pdesc.GetWindowTextA( str );
		m_Asize.W_pindesc = my_atof(&str);
		// foot
		m_Asize.app_foot = m_check_apply_foot.GetCheck();
		m_edit_height_foot.GetWindowTextA( str );
		m_Asize.H_foot = my_atof(&str);
		m_edit_width_foot.GetWindowTextA( str );
		m_Asize.W_foot = my_atof(&str);
		// value
		m_Asize.app_value = m_check_apply_value.GetCheck();
		m_edit_height_value.GetWindowTextA( str );
		m_Asize.H_value = my_atof(&str);
		m_edit_width_value.GetWindowTextA( str );
		m_Asize.W_value = my_atof(&str);
		if( m_Units == MIL )
	 	{
			m_Asize.Multiply( (float)NM_PER_MIL );
		}
		else if( m_Units == MM )
		{
			m_Asize.Multiply( (float)NM_PER_MM );
		}
		//
		if( m_ref.FindOneOf( ILLEGAL_REF ) != -1 )
		{
			CString mess;
			CString illegal = ILLEGAL_REF;
			mess.Format(G_LANGUAGE == 0 ? 
				"Illegal reference designator \"%s\"\n%s - these characters cannot be used":
				"Недопустимое обозначение ссылки \"%s\"\n%s - эти символы не могут быть использованы", m_ref, illegal);
			AfxMessageBox( mess );
			pDX->Fail();
		} 
		if( m_pin.FindOneOf( ILLEGAL_PIN ) != -1 )
		{
			CString mess;
			CString illegal = ILLEGAL_PIN;
			mess.Format(G_LANGUAGE == 0 ? 
				"Illegal pin name \"%s\"\n%s - these characters cannot be used":
				"Недопустимое имя пина \"%s\"\n%s - эти символы не могут быть использованы", m_pin, illegal);
			AfxMessageBox( mess );
			pDX->Fail();
		} 
		if( m_pin.Left(1) < "9" && m_pin.Right(1) > "9" && m_pin.Find(",") == 0 )
		{
			CString mess;
			mess.Format(G_LANGUAGE == 0 ? 
				"Illegal pin name \"%s\"\nHere is the correct pin naming style: A1, A2... B1, B2...":
				"Недопустимое имя пина \"%s\"\nВот правильный стиль именования пинов: A1, A2... B1, B2...", m_pin);
			AfxMessageBox( mess );
			pDX->Fail();
		} 
		if( m_net.FindOneOf( ILLEGAL_NET ) != -1 )
		{
			CString mess;
			CString illegal = ILLEGAL_NET;
			mess.Format(G_LANGUAGE == 0 ? 
				"Illegal net name \"%s\"\n%s - these characters cannot be used":
				"Недопустимое имя эл.цепи \"%s\"\n%s - эти символы не могут быть использованы", m_net, illegal);
			AfxMessageBox( mess );
			pDX->Fail();
		} 
		if( m_fname.FindOneOf( "\"@|'" ) != -1 || m_pindesc.FindOneOf( "\"" ) != -1 || m_partv.FindOneOf( "\"@" ) != -1 )
		{
			CString mess;
			mess.Format(G_LANGUAGE == 0 ? "Illegal characters: ( ) | ' \" @ ":"Недопустимые символы: ( ) | ' \" @ ");
			AfxMessageBox( mess );
			pDX->Fail();
		} 
		if( m_ref.Compare("Multiple") == 0 || m_ref.GetLength() == 0 )
		{
			if( m_ref.GetLength() == 0 ) 
			{
				m_fname = "";
				m_partv = "";
			}
			else
				m_ref = "";
		}
		else
		{
			int it = -1;
			if( m_att->m_Reflist->GetText( &m_ref, &it ) )
				if( m_att->m_Reflist->GetText( &m_ref, &it ) )
				{
					AfxMessageBox(G_LANGUAGE == 0 ? 
						("Group editing of parts is not possible when the circuit contains the same part designations !"\
						"You can use the automatic renumbering tool"):
						("Групповое редактирование деталей невозможно, если схема содержит одинаковые обозначения деталей!"\
						"Вы можете использовать инструмент автоматической перенумерации"), MB_ICONERROR);
					pDX->Fail();
				}
			CText * t = NULL;
			int dot = m_ref.Find(".");
			if( dot <= 0 )
				dot = m_ref.Find("-");
			CString sFind = m_ref;
			if( dot > 0 )
				sFind = m_ref.Left(dot) + "&";// check for multi-parts
			else
				sFind = m_ref + "&";// check for multi-parts
			if( m_pl->IsThePageIncludedInNetlist( m_pl->GetActiveNumber() ) )
			{
				for( m_pl->FindPart( &sFind, &t, TRUE, TRUE ); t; m_pl->FindPart( &sFind, &t, FALSE, TRUE ) )
				{
					if( t->utility == 0 )
					{
						if( m_ref.Compare(t->m_str) == 0 )
							break;

						int dot2 = t->m_str.Find(".");
						if( dot2 <= 0 )
							dot2 = t->m_str.Find("-");

						if( dot != dot2 )
							break;
					}
				}
			}
			else
			{
				int it2 = -1;
				t = m_pl->GetCurrentAttrPtr()->m_Reflist->GetText( &sFind, &it2 );
				if(t)
					if( m_ref.Compare(t->m_str) )
					{
						int dot2 = t->m_str.Find(".");
						if( dot2 <= 0 )
							dot2 = t->m_str.Find("-");
						if( dot == dot2 )
							t = NULL;
					}
			}
			if( t )
				if( t->utility == 0 ) // not in sel group
				{
					int ret = AfxMessageBox(G_LANGUAGE == 0 ? 
						("A part with the same name already exists. "\
						"Determine free index value and rename part automatically?\n\n(If "\
						"you want to attach a polyline to any part, then you need to select "\
						"both the part and the polyline, and then call this dialog box and click OK.)"):
						("Деталь с таким же именем уже существует. "\
						"Хотите определить свободное значение индекса и обозначить деталь автоматически?\n\n"\
						"(Если вы хотите просто прикрепить полилинию к любой детали, то вам нужно выделить "\
						"и деталь, и полилинию, затем вызвать это диалоговое окно и нажать ГОТОВО.)"), MB_YESNO | MB_ICONWARNING);

					if( ret == IDNO )
						pDX->Fail();
					// use next available ref
					CString prefix, new_ref, suff;
					ParseRef( &m_ref, &prefix, &suff );
					for( int x_num=1; t; x_num++ )
					{
						new_ref.Format( "%s%d%s", prefix, x_num, suff );
						int dot = new_ref.Find(".");
						if( dot <= 0 )
							dot = new_ref.Find("-");
						CString sFind = new_ref;
						if( dot > 0 )
							sFind = new_ref.Left(dot) + "&";// check for multi-parts
						else
							sFind = new_ref + "&";// check for multi-parts
						if( m_pl->IsThePageIncludedInNetlist( m_pl->GetActiveNumber() ) )
						{
							for( m_pl->FindPart( &sFind, &t, TRUE, TRUE ); t; m_pl->FindPart( &sFind, &t, FALSE, TRUE ) )
							{
								if( new_ref.Compare(t->m_str) == 0 )
									break;

								int dot2 = t->m_str.Find(".");
								if( dot2 <= 0 )
									dot2 = t->m_str.Find("-");

								if( dot != dot2 )
									break;
							}
						}
						else
						{
							int it2 = -1;
							t = m_pl->GetCurrentAttrPtr()->m_Reflist->GetText( &sFind, &it2 );
							if(t)
								if( new_ref.Compare(t->m_str) )
								{
									int dot2 = t->m_str.Find(".");
									if( dot2 <= 0 )
										dot2 = t->m_str.Find("-");
									if( dot == dot2 )
										t = NULL;
								}
						}
					}
					m_ref = new_ref;
					m_edit_ref.SetWindowTextA( m_ref );
				}
		}
		if( m_ref.GetLength() && m_fname.GetLength() == 0 )
		{
			m_fname = NO_FOOTPRINT;
			m_edit_fname.SetWindowTextA( m_fname );
		}
		if( m_pindesc.GetLength() )
		{
			for( int i=m_pindesc.Find("\n"); i>0; i=m_pindesc.Find("\n") )
			{
				m_pindesc.Delete(i);
				m_pindesc.Insert(i,"\'");
			}
			for( int i=m_pindesc.Find("\r"); i>0; i=m_pindesc.Find("\r") )
			{
				m_pindesc.Delete(i);
			}
		}
	}
}

void CDlgAttr::Initialize(	CArray<CString> * F,
							CFreePcbDoc * doc,
							CString * ref, 
							CString * pin, 
							CString * pindesc, 
							CString * net, 
							CString * fname, 
							CString * partv,
							CString * folder,
							CString * footprint_ext,
							CString * lib_dir,
							CString * prefixes,
							attr_size size,
							int units,
							int mask )
{
	m_prefixes = *prefixes;
	m_lib_dir = *lib_dir;
	m_footprint_ext = *footprint_ext;
	m_folder = *folder;
	m_ref = *ref;
	m_pin = *pin;
	m_pindesc = *pindesc;
	m_net = *net;
	m_fname = *fname;
	m_partv = *partv;
	m_Units = units;
	m_Asize = size;
	m_mask = mask;
	m_doc = doc;
	m_pl = &doc->Pages;
	m_att = m_pl->GetCurrentAttrPtr();
	if( m_Units == MIL )
	{
		m_Asize.Multiply( 1.0/(float)NM_PER_MIL );
	}
	else if( m_Units == MM )
	{
		m_Asize.Multiply( 1.0/(float)NM_PER_MM );
	}
	if( m_pindesc.Right(3) == "BOM" )
	{
		int f = m_pindesc.Find("|");
		if( f > 0 )
		{
			m_pindesc = "Table'" + m_pindesc.Right( m_pindesc.GetLength() - f );
		}
	}
	for( int i=m_pindesc.Find("'"); i>0; i=m_pindesc.Find("'") )
	{
		m_pindesc.Delete(i);
		m_pindesc.Insert(i,"\r\n");
	}
	m_F = F;
}

BEGIN_MESSAGE_MAP(CDlgAttr, CDialog)
	ON_CBN_EDITCHANGE(IDC_REFDES, OnChange)
	ON_BN_CLICKED(ID_OPEN_LIB, OnOpenLib)
	ON_BN_CLICKED(ID_Q1, OnQ1)
	ON_BN_CLICKED(ID_Q2, OnQ2)
	ON_BN_CLICKED(ID_Q3, OnQ3)
	ON_BN_CLICKED(ID_Q4, OnQ4)
	ON_BN_CLICKED(ID_Q5, OnQ5)
	ON_BN_CLICKED(ID_Q6, OnQ6)
	ON_BN_CLICKED(ID_Q7, OnQ7)
	ON_BN_CLICKED(ID_Q8, OnQ8)
	ON_BN_CLICKED(ID_Q9, OnQ9)
	ON_BN_CLICKED(ID_Q10, OnQ10)
	ON_BN_CLICKED(ID_Q13, OnQ11)
	ON_BN_CLICKED(ID_Q14, OnQ12)
	//
	ON_BN_CLICKED(ID_CMD1, OnCmd1)
	ON_BN_CLICKED(ID_CMD2, OnCmd2)
	ON_BN_CLICKED(ID_CMD3, OnCmd3)
	ON_BN_CLICKED(ID_CMD4, OnCmd4)
	ON_BN_CLICKED(ID_CMD5, OnCmd5)
	ON_BN_CLICKED(ID_CMD6, OnCmd6)
	ON_BN_CLICKED(ID_CMD7, OnCmd7)
	ON_BN_CLICKED(ID_CMD8, OnCmd8)
	ON_BN_CLICKED(ID_CMD9, OnCmd9)
	//
	ON_WM_COPYDATA()
	ON_CBN_EDITCHANGE(IDC_FNAME, &CDlgAttr::OnCbnSelchangeFname)
	ON_CBN_EDITCHANGE(IDC_PARTVALUE, &CDlgAttr::OnCbnSelchangePartvalue)
	ON_CBN_EDITCHANGE(IDC_PINNAME, &CDlgAttr::OnCbnSelchangePinname)
	ON_CBN_EDITCHANGE(IDC_NETNAME, &CDlgAttr::OnCbnSelchangeNetname)
	ON_CBN_SELCHANGE(IDC_REFDES, &CDlgAttr::OnCbnSelchangeRefdes)
	ON_CBN_SELENDOK(IDC_REFDES, &CDlgAttr::OnCbnSelendokRefdes)
	ON_CBN_CLOSEUP(IDC_REFDES, &CDlgAttr::OnCbnCloseupRefdes)
	ON_CBN_EDITUPDATE(IDC_REFDES, &CDlgAttr::OnCbnEditupdateRefdes)
END_MESSAGE_MAP()


// CDlgAttr message handlers
void CDlgAttr::OnChange()
{
	CString str;
	m_edit_ref.GetWindowTextA(str);
	if( str.GetLength() == 0 )
	{
		m_edit_fname.EnableWindow(0);
		m_edit_value.EnableWindow(0);
	}
	else
	{
		m_edit_fname.EnableWindow(1);
		m_edit_value.EnableWindow(1);
	}
	if( getbit( m_mask, index_part_attr ) == 0 )
	{
		m_edit_ref.EnableWindow(0);
		m_check_apply_ref.EnableWindow(0);
		m_edit_height_ref.EnableWindow(0);
		m_edit_width_ref.EnableWindow(0);
	}
	if( getbit( m_mask, index_pin_attr ) == 0 )
	{
		m_edit_pin.EnableWindow(0);
		m_check_apply_pin.EnableWindow(0);
		m_edit_height_pin.EnableWindow(0);
		m_edit_width_pin.EnableWindow(0);
	}
	if( getbit( m_mask, index_net_attr ) == 0 )
	{
		m_edit_netname.EnableWindow(0);
		m_check_apply_net.EnableWindow(0);
		m_edit_height_net.EnableWindow(0);
		m_edit_width_net.EnableWindow(0);	
	}
	if( getbit( m_mask, index_desc_attr ) == 0 )
	{
		m_edit_pindesc.EnableWindow(0);
		m_check_apply_pdesc.EnableWindow(0);
		m_edit_height_pdesc.EnableWindow(0);
		m_edit_width_pdesc.EnableWindow(0);
	}
	if( getbit( m_mask, index_foot_attr ) == 0 )
	{
		m_edit_fname.EnableWindow(0);
		m_check_apply_foot.EnableWindow(0);
		m_edit_height_foot.EnableWindow(0);
		m_edit_width_foot.EnableWindow(0);
	}
	if( getbit( m_mask, index_value_attr ) == 0 )
	{
		m_edit_value.EnableWindow(0);
		m_check_apply_value.EnableWindow(0);
		m_edit_height_value.EnableWindow(0);
		m_edit_width_value.EnableWindow(0);
	}

	int cur = m_edit_ref.GetCurSel();
	if( cur >= 0 )
	{
		CString s;
		m_edit_ref.GetLBText( cur, s );
		int iof = s.Find("]");
		if( iof > 0 )
		{
			s = s.Left(iof);
			s = s.Right( s.GetLength()-1 );
			m_edit_ref.SetWindowText( s );
			m_edit_ref.Invalidate(0);
		}
	}
}

void CDlgAttr::OnOpenLib()
{
	//m_open_lib.EnableWindow(0);
	CString Mess, foot;
	m_edit_fname.GetWindowTextA(foot);
	CWnd * win = FindWindow(NULL, "FreePcb Footprints Library");
	if( win )
	{
		m_doc->OnSendMess( win->GetSafeHwnd(), &foot );
		win->SetForegroundWindow();
		return;
	}
	if( m_footprint_ext.GetLength() == 0 )
	{
		struct _stat buf;
		if( _stat( m_lib_dir, &buf ) )
			m_lib_dir = "";
		HANDLE cmd = GlobalAlloc(GMEM_FIXED,2048);
		Mess.Format("SchConstr: \"%s\"", foot );
		memcpy( cmd, Mess, Mess.GetLength()+1 );
		
		do Sleep(100);  
			while(!OpenClipboard());
		EmptyClipboard();
		SetClipboardData(CF_TEXT,cmd);
		CloseClipboard();
		CString folder = m_folder+"\\FPC_EXE\\FreePCB_Libraries";
		ShellExecute(	NULL,"open",folder+"\\ABC_library.exe",
										NULL,folder,SW_SHOWNORMAL);
	}
	else
	{
		CString m_lib = m_folder + "\\" + foot + m_footprint_ext;
		ShellExecute(	NULL, "open", m_lib, NULL, m_folder, SW_SHOWNORMAL );
	}
}

void CDlgAttr::OnQ1()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("The part designation must begin with a letter, followed by a part number. "\
		"Use a dash or period to create a multi-graphic character. "\
		"For example, name one part DA1-1, and another name DA1-2. "\
		"In netlist, this will appear as the same part DA1"):
		("Обозначение детали должно начинаться с буквы, за которой следует номер детали. "\
		"Используйте тире или точку для создания многографического символа. "\
		"Например, назовите одну деталь DA1-1, а другую — DA1-2. "\
		"В списке цепей это будет выглядеть как та же деталь DA1"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ2()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("Indicate the path to the FreePcb-2 footprint library via the Project->Options menu"\
		"so that you can select them in this combo box, and also "\
		"so that the "PROGRAM_NAME" can check for the presence "\
		"of the specified footprint in the library"):
		("Укажите путь к библиотеке футпринтов ПлатФорм через меню Проект->Настройки, "\
		"чтобы можно было выбрать их в этом выпадающем списке, а также "\
		"чтобы "PROGRAM_NAME" мог проверить наличие "\
		"указанного футпринта в библиотеке"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ3()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("You can turn any open polyline into a pin."\
		"In order to connect several pins to one polyline, you can list "\
		"the pin names separated by commas, or you can use a dash to "\
		"indicate the range of pins. For example, 24,36,50-52 or A2,B1-B3. "\
		"A space character is not allowed.\n\n"):
		("В СхемАторе можно превратить любую полилинию в пин. "\
		"Чтобы назначить несколько пинов одной полилиний, вы можете перечислить "\
		"имена пинов, разделенные запятыми, или использовать тире, чтобы "\
		"указать диапазон пинов. Например, 24,36,50-52 или A2,B1-B3. "\
		"Символ пробела не допускается.\n\n"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ4()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("You can create a net label on any polyline. "\
		"At least one end of the net label polyline must "\
		"located on another polyline to connect to it. "\
		"For net naming, do not use lines starting with NET..., "\
		"because this is reserved for automatic generation of net names "\
		"when creating netlist"):
		("Вы можете создать метку эл.цепи на любой полилинии. "\
		"Хотя бы один конец метки эл.цепи должен "\
		"располагаться на другой полилинии для соединения с ней. "\
		"Для имени цепи не используйте строки, начинающиеся с NET..., "\
		"потому что это зарезервировано для автоматической генерации имен эл.цепей "\
		"при создании списка"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ5()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("Each time the file is saved, this text will change "\
		"to reflect the current date. You can change the pattern "\
		"by swapping DD and MM, and you can also use the \"/\" character "\
		"instead of a period as a separator"):
		("Каждый раз при сохранении изменённого файла этот текст будет меняться "\
		"чтобы отразить текущую дату. Вы можете изменить шаблон "\
		"поменяв местами ДД и ММ, а также можете использовать символ \"/\" "\
		"вместо точки в качестве разделителя"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ6()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("In the first line you can enter any link text. "\
		"After the text: "CP_LINK", separated by a space, indicate which part you want to jump to. "\
		"For the link to work, you need to click on it "\
		"while holding down the CTRL key"):
		("В первой строке можно ввести любой текст ссылки. "\
		"После текста: "CP_LINK", через пробел укажите, к какой детали вы хотите перейти. "\
		"Чтобы ссылка сработала, нужно нажать на нее "\
		"удерживая клавишу CTRL"), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ7()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("For multi-channel circuits, it is convenient to use a complex part that contains "\
		"a small schematic diagram inside, consisting of other parts.. "\
		"It is best to create a circuit of a complex part on a separate page. "\
		"Create a new page via the View-> Add New Page menu and then "\
		"exclude it from the netlist via the File-> Netlist_Settings menu. "\
		"Draw the outline of a complex part then add pins, just like you usually "\
		"draw any other part, and enter its designation for example \"Channel\". "\
		"Place a schematic diagram of this complex part inside it, "\
		"and connect the leads to the desired nodes of the circuits. The complex part template is ready."\
		"\n\nNow switch to the page "\
		"where you want to add this complex part. This page must be included "\
		"in the netlist via the File-> Netlist_Settings menu. "\
		"To add any complex part to the diagram and, accordingly, to the netlist, "\
		"you must first draw a contour line of this complex part and then "\
		"in the \"description\" attribute of the polyline, enter "\
		"for example the following text \n\n\tChannel_1\n\t"CP_SUFFIX" _1\n\t"COMMAND" BLK\n\n "\
		"The program will create a parts block named [ref des of complex part]_1 (those. Channel_1) and the suffix _1 "\
		"will be added to all part numbers in that block. See next item to continue ..."):
		("Создание сложной детали: Для многоканальных схем удобно использовать иерархическую деталь, содержащую внутри "\
		"небольшую принципиальную схему, состоящую из других деталей.. "\
		"Схему иерархической детали лучше всего создавать на отдельной странице. "\
		"Создайте новую страницу через меню «Вид -> Добавить новую страницу», а затем "\
		"исключите ее из списка соединений через меню «Файл-> Настройки списка эл.цепей». "\
		"Нарисуйте контур иерархической детали, затем добавьте пины, как будто "\
		"вы создаёте обычную деталь, и введите ее обозначение, например \"Канал\". "\
		"Поместите внутрь нее принципиальную схему этой иерархической детали, "\
		"и подключите пины к нужным узлам схем. Шаблон иерархической детали готов."\
		"\n\nТеперь перейдите на страницу с основной схемой, "\
		"куда вы хотите добавить эту иерархическую деталь. Эту страницу необходимо включить "\
		"в список эл.цепей через меню «Файл -> Настройки списка эл.цепей». "\
		"Чтобы добавить любую иерархическую деталь к схеме и соответственно к списку эл.цепей, "\
		"необходимо сначала нарисовать контурную полилинию этой иерархической детали, а затем "\
		"в атрибуте \"описание\" полилинии ввести "\
		"например следующий текст \n\n\tChannel_1\n\t"CP_SUFFIX" _1\n\t"COMMAND" BLK\n\n "\
		"Программа создаст блок деталей с именем [обозначение]_1 (т.е. в нашем примере Channel_1) и суффикс _1 "\
		"будет добавлен ко всем номерам деталей в этом блоке. Продолжение см. в следующем пункте..."), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ8()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("To get started, see the previous question\n\n"\
		"Draw a pin of block, that should touch the outline of this block. "\
		"In the \"description\" attribute of the pin polyline, "\
		"enter the following for example: \n\n\t8\n\t"CP_SUFFIX" _1"\
		"\n\t"CP_LINK" Channel\n\t|pin_name: 8\n\t"CP_PAGE_NUM" 2\n\t"COMMAND" BLK_PTR\n\nThe program "\
		"will add a suffix _1 to the ref \"Channel\" (and also to all parts inside this complex part) and connect "\
		"pin 8 of part Channel_1 to this polyline. \n"\
		"A suffix will be added to the nets with an automatically generated name "\
		"(these include nets starting with \"NET0 ....\").Nets named by the user inside a "\
		"complex part will remain unchanged, which means that no suffix will be added to their names.\n"\
		"It should be borne in mind that in fact the Channel_1 part and "\
		"its connections inside it are not displayed anywhere, but are inside the program."):
		("Для начала см. предыдущий вопрос\n\n"\
		"Нарисуйте пин блока, который должен касаться контура этого блока. "\
		"В атрибуте \"Описание\" полилинии пина, "\
		"введите следующее, например: \n\n\t8\n\t"CP_SUFFIX" _1"\
		"\n\t"CP_LINK" Channel\n\t|pin_name: 8\n\t"CP_PAGE_NUM" 2\n\t"COMMAND" BLK_PTR\n\nПрограмма "\
		"добавит указанный суффикс _1 к ссылке \"Channel\" (а также ко всем деталям внутри этой иерархической детали) и подключит "\
		"к пину 8 детали Channel_1. \n"\
		"Суффикс _1 также будет добавлен к эл.цепям (к ним относятся цепи, формата "\
		"NETxxxxx). Глобальные (межстраничные) эл.цепи, названные пользователем "\
		"останутся без изменений, то есть к их именам не будет добавлен суффикс.\n"\
		"Следует иметь в виду, что на самом деле деталь Channel_1 и ее "\
		"соединения внутри нее нигде не отображаются, а находятся внутри программы."), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ9()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("If you add the pipe character \"|\" to the end of the net name "\
		"then the text following it will be interpreted by the program "\
		"as the name of the electronic bus. For example 1|bus1. "\
		"For the electronic bus to work, you will need to draw a thick line "\
		"and enter the same bus1 bus name in the \"description\" "\
		"attribute of this polyline. This polyline will be the graphic "\
		"representation of the electronic bus."):
		("Если вы добавите символ вертикальной черты \"|\" в конец имени эл.цепи "\
		"то текст, следующий за ним, будет интерпретироваться программой "\
		"как имя электронной шины. Например, 1|bus1. "\
		"Чтобы электронная шина работала, вам нужно будет нарисовать толстую линию "\
		"и также ввести имя шины bus1 в атрибут \"description\" "\
		"этой полилинии. Эта полилиния будет графическим "\
		"представлением электронной шины."), MB_ICONINFORMATION);
}

void CDlgAttr::OnQ10()
{
	AfxMessageBox(G_LANGUAGE == 0 ? 
		("You can add a BOM table snippet to the page. "\
		"Draw a rectangle of the desired size "\
		"by moving the mouse while holding down the button from the "\
		"upper left corner to the lower right corner in line drawing mode. In the "\
		"\"description\" attribute of this polyline enter "\
		"the following for example: \n\n\tTable\n\t|start_number: 1\n\t|"\
		"end_number: 100\n\t|sorting_column: 1\n\t|detail_column_width: 20\n\t"\
		"|ignore_those_without_value: 1\n\t|include_ref_list: not_used\n\t"\
		"|exclude_ref_list: not_used\n\t|column_order: 12345\n\t|pcb_name: example.fpc\n\t"COMMAND" BOM\n\n"\
		"The program will insert into the outline of this "\
		"rectangle from 1 to 100 lines of the BOM list. Auto-update table when saving file."):
		("Вы можете добавить фрагмент таблицы спецификации на страницу. "\
		"Нарисуйте прямоугольник нужного размера "\
		"перемещая мышь, удерживая кнопку, из "\
		"верхнего левого угла в нижний правый угол в режиме рисования линий. "\
		"Далее в атрибуте \"Описание\" этой полилинии (в этом окне) введите следующее, например: "\
		"\n\n\tTable\n\t|start_number: 1\n\t|"\
		"end_number: 100\n\t|sorting_column: 1\n\t|detail_column_width: 20\n\t"\
		"|ignore_those_without_value: 1\n\t|include_ref_list: not_used\n\t"\
		"|exclude_ref_list: not_used\n\t|column_order: 12345\n\t|pcb_name: example.fpc\n\t"COMMAND" BOM\n\n"\
		"Программа вставит в контур этого прямоугольника от 1 до 100 строк списка спецификации. "\
		"Авто-обновление таблицы происходит при сохранении файла"), MB_ICONINFORMATION);
}

void CDlgAttr::OnCmd1()
{
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "00.00.00";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The description text already contains the command":
			"Текст описания уже содержит код команды");
		return;
	}
	m_edit_pindesc.SetWindowTextA(str+"|DD.MM.YYYY DATE");
}

void CDlgAttr::OnCmd2()
{
	CString str, str2;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
	{
		str = "^ ";
		str2 = "Enter_RefDes_or_URL";
	}
	else
		str2 = str;
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	m_edit_pindesc.SetWindowTextA(str + "\r\n"CP_LINK" "+str2+"\r\n"CP_PAGE_NUM" 1");
}

void CDlgAttr::OnCmd3()
{
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "VisibleText ";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	m_edit_pindesc.SetWindowTextA(str+"\r\n"CP_SUFFIX" xx\r\n"CP_LINK" xx\r\n"CP_PAGE_NUM" xx\r\n"COMMAND" BLK");
}

void CDlgAttr::OnCmd4()
{
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "VisibleText ";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	m_edit_pindesc.SetWindowTextA(str+"\r\n"CP_SUFFIX" xx\r\n"CP_LINK" xx\r\n|pin_name: xx\r\n"CP_PAGE_NUM" xx\r\n"COMMAND" BLK_PTR");
}

void CDlgAttr::OnCmd5()
{
	CString str;
	m_edit_netname.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "EnterNetName";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"The net name already contains the command":
			"Имя сети уже содержит код команды");
		return;
	}
	m_edit_netname.SetWindowTextA(str+"|EnterBusName");
}

void CDlgAttr::OnCmd6()
{
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "Table ";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	if( m_pl->m_pcb_names.GetSize() )
	{
		CString pname = m_pl->GetCurrentPCBName( m_pl->GetActiveNumber() );
		if( pname.GetLength() == 0 )
			pname = m_pl->m_pcb_names.GetAt( 0 );
		CString str2;
		str2.Format("\r\n|start_number: 1\r\n|end_number: 100\r\n|sorting_column: 5\r\n|detail_column_width: 15\r\n|ignore_those_without_value: 1\r\n|include_ref_list: not_used\r\n|exclude_ref_list: not_used\r\n|column_order: 12345\r\n|pcb_name: %s\r\n"COMMAND" BOM", pname );
		m_edit_pindesc.SetWindowTextA(str+str2);
	}
	else
	{
		AfxMessageBox(G_LANGUAGE == 0 ? 
			"PCB name is missing. First you need to attach the PCB to this project. Go to File >> NetlistSettings and set the name of the circuit board.":
			"Отсутствует имя печатной платы. Сначала необходимо прикрепить печатную плату к этой странице. Перейдите в «Файл >> Опции списка эл.цепей» и задайте имя печатной платы.");
		return;
	}
}

void CDlgAttr::OnCmd7()
{
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if( str.GetLength() == 0 )
		str = "Path ";
	int f = str.Find("|");
	if( f > 0 )
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	m_edit_pindesc.SetWindowTextA(str+"\r\n|path: 0\r\n|name: 1\r\n|pcb_flag: 1\r\n|extention: 1\r\n|length: 99\r\n|make_lower: 0\r\n"COMMAND" PATH");
}

int CDlgAttr::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* Msg) 
{
	CString Mess;
	Mess = (char*)Msg->lpData;
	if( Mess.GetLength() )
		m_edit_fname.SetWindowTextA( Mess );
	return 0;
}

void CDlgAttr::OnCbnSelchangeFname()
{
	// TODO: добавьте свой код обработчика уведомлений
	ShowHint( index_foot_attr );
	m_open_lib.EnableWindow(1);
}

void CDlgAttr::OnCbnSelchangePartvalue()
{
	// TODO: добавьте свой код обработчика уведомлений
	ShowHint( index_value_attr );
}

void CDlgAttr::OnCbnSelchangePinname()
{
	// TODO: добавьте свой код обработчика уведомлений
	//ShowHint( index_pin_attr ); мешает
}


void CDlgAttr::OnCbnSelchangeNetname()
{
	// TODO: добавьте свой код обработчика уведомлений
	ShowHint( index_net_attr );
}

void CDlgAttr::ShowHint( int item )
{
	CString str="";
	CComboBox * ComboBox;
	if( item == index_part_attr )
		ComboBox = &m_edit_ref;
	else if( item == index_foot_attr )
		ComboBox = &m_edit_fname;
	else if( item == index_value_attr )
		ComboBox = &m_edit_value;
	else if( item == index_pin_attr )
		ComboBox = &m_edit_pin;
	else if( item == index_net_attr )
		ComboBox = &m_edit_netname;
	else 
		ASSERT(0);
	ComboBox->GetWindowTextA( str );
	CString mstr = str;
	str.MakeLower();
	CString s;
	int count = 0;
	//ComboBox->ShowWindow( SW_HIDE );
	if( str.GetLength() ) for( int i=0; i<ComboBox->GetCount(); i++ )
	{
		ComboBox->GetLBText( i, s );
		s.MakeLower();
		int p = s.Find( str );
		if( p >= 0 )
		{
			ComboBox->GetLBText( i, s );
			if( p == 0 )
				ComboBox->InsertString( 0, s );
			else
				ComboBox->InsertString( count, s );
			ComboBox->DeleteString( i+1 );
			count++;
		}
	}
	//ComboBox->ShowDropDown(count);
	//ComboBox->SetWindowTextA( mstr );
	//ComboBox->SetEditSel( mstr.GetLength(), mstr.GetLength() );
	//ComboBox->ShowWindow( SW_SHOW );
	ShowCursor(1);
}

void CDlgAttr::OnCbnSelchangeRefdes()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
}


void CDlgAttr::OnCbnSelendokRefdes()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
}


void CDlgAttr::OnCbnCloseupRefdes()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
}


void CDlgAttr::OnCbnEditupdateRefdes()
{
	// TODO: ???????? ???? ??? ??????????? ???????????
}


void CDlgAttr::OnCmd8()
{
	// TODO: добавьте свой код обработчика уведомлений
	id iD;
	int num_pg = m_doc->Pages.FindPrintableArea(&iD).ii + 1;
	CString name;
	m_doc->Pages.GetPageName(m_doc->Pages.GetActiveNumber(), &name);
	ExtractComponentName(&name, NULL);
	CString cmd;
	cmd.Format("Page%d\r\n|name: %s.%d.pdf\r\n%s PrintableArea", num_pg, name, num_pg, COMMAND);
	m_edit_pindesc.SetWindowTextA(cmd);
}


void CDlgAttr::OnCmd9()
{
	// TODO: добавьте свой код обработчика уведомлений
	CString str;
	m_edit_pindesc.GetWindowTextA(str);
	if (str.GetLength() == 0)
		str = "PCB ";
	int f = str.Find("|");
	if (f > 0)
	{
		AfxMessageBox(G_LANGUAGE == 0 ?
			"The description text already contains the command" :
			"Текст описания уже содержит код команды");
		return;
	}
	CString pname = "PCB.fpc";
	if (m_pl->m_pcb_names.GetSize())
	{
		pname = m_pl->GetCurrentPCBName(m_pl->GetActiveNumber());
		if (pname.GetLength() == 0)
			pname = m_pl->m_pcb_names.GetAt(0);
	}
	CString str2;
	str2.Format("\r\n|UID: 0\r\n|text_height: 1mm\r\n|font_width: 0.1mm\r\n|line_width: 0\r\n|fill_board: 20\r\n|fill_mask: 1\r\n|scale_factor: 0\r\n|rotation: 0\r\n|flipped: 0\r\n|pcb: %s\r\n"COMMAND" PCBVIEW", pname);
	m_edit_pindesc.SetWindowTextA(str + str2);
}


void CDlgAttr::OnQ11()
{
	// TODO: добавьте свой код обработчика уведомлений
	AfxMessageBox(G_LANGUAGE ? "В Схематоре можно делать областью печати любую полилинию. "\
		"Каждая новая область печати будет напечатана либо на отдельной "\
		"странице PDF, либо в отдельном файле PDF, если вы выберете в "
		"настройках PDF опцию сепарированной печати." :
		"In the Schemator you can make any polyline a print area. "\
		"Each new print area will be printed either on a separate PDF "\
		"page or in a separate PDF file if you select the separated print option in the PDF settings.", MB_ICONINFORMATION);
}


void CDlgAttr::OnQ12()
{
	// TODO: добавьте свой код обработчика уведомлений
	AfxMessageBox(G_LANGUAGE ? "В Схематоре можно отобразить вид печатной "\
		"платы с верхней и нижней стороны.Введите название печатной платы, "\
		"файл которой находится в том же проекте что и файл схемы.Таким образом "\
		"можно создавать интерактивные монтажные схемы прямо в Схематоре " :
		"In the Schemator, you can display the view of printed circuit board "\
		"(top and bottom side).Enter the name of the printed circuit board, "\
		"the file of which is in the same project as the scheme file.Thus, "\
		"you can create interactive assembly schemes directly in the schemator", MB_ICONINFORMATION);
}
