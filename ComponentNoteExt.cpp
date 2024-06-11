// CComponentNoteExt.cpp : implementation file
//

#include "stdafx.h"
//#include "FreeSch.h"
#include "ComponentNoteExt.h"


// CComponentNoteExt dialog

IMPLEMENT_DYNAMIC(CComponentNoteExt, CDialog)
CComponentNoteExt::CComponentNoteExt(CWnd* pParent /*=NULL*/)
	: CDialog(CComponentNoteExt::IDD, pParent)
{
	ext_num = 0;
	m_extern = 0;
}

CComponentNoteExt::~CComponentNoteExt()
{
}

void CComponentNoteExt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_1, m_1);
	DDX_Control(pDX, IDC_RADIO_2, m_2);
	DDX_Control(pDX, IDC_RADIO_3, m_3);
	DDX_Control(pDX, IDEXFILE, m_external_file);

	if( !pDX->m_bSaveAndValidate )
	{	
		// input
		m_1.SetCheck(0);
		m_2.SetCheck(0);
		m_3.SetCheck(0);
		if( ext_num == 0 )
			m_1.SetCheck(1);
		else if( ext_num == 1 )
			m_2.SetCheck(1);
		else if( ext_num == 2 )
			m_3.SetCheck(1);
	}
	else
	{
		// outgoing
		if( m_1.GetCheck() )
		{
			ext = ".docx";
			ext_num = 0;
		}
		else if( m_2.GetCheck() )
		{
			ext = ".txt";
			ext_num = 1;
		}
		else 
		{
			ext = ".pdf";
			ext_num = 2;
		}
	}
}

// CComponentNoteExt message handlers
BEGIN_MESSAGE_MAP(CComponentNoteExt, CDialog)
	ON_BN_CLICKED(IDEXFILE, &CComponentNoteExt::OnBnClickedExfile)
END_MESSAGE_MAP()

void CComponentNoteExt::Initialize( int num )
{
	ext_num = num;
}


void CComponentNoteExt::OnBnClickedExfile()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_extern = 1;
	OnOK();
}

void CComponentNoteExt::Sync( CFreePcbDoc * doc )
{
	if( doc->m_pcb_full_path.GetLength() == 0 )
		return;
	int rf = doc->m_pcb_full_path.ReverseFind('\\');
	if( rf <= 0 )
		return;
	CString Path2 = doc->m_pcb_full_path.Left(rf) + rel_f_component;
	struct _stat buf;
	if( _stat( Path2, &buf ) )
	{
		// test 
		struct _stat buf;
		CString rel_f = (doc->m_pcb_full_path.Left(rf)+"\\related_files");
		if( _stat( rel_f, &buf ) )
			_mkdir( rel_f );

		int err = _mkdir( Path2 );
		if( err )
		{
			CString str;
			str.Format( "Unable to create folder \"%s\"", Path2 );
			AfxMessageBox( str, MB_OK );
			return;
		}
	}
	CString CDir = doc->m_app_dir + main_component;
	CFileFind finder;
	CString first_f = "\\*.*";
	//
	for( int STEP=0; STEP<2; STEP++ )
	{
		CString search_str = CDir + first_f;
		BOOL bWorking = finder.FindFile( search_str );
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if( !finder.IsDirectory() && !finder.IsDots() )
			{
				CString fp = finder.GetFilePath();
				CString fn = finder.GetFileName();
				CString Name2 = Path2 + "\\" + fn;
				//
				struct _stat buf1;
				if( _stat( fp, &buf1 ) )
					continue;
				struct _stat buf2;
				if( _stat( Name2, &buf2 ) == 0 )
				{
					if( buf1.st_mtime <= buf2.st_mtime )
						continue;
				}
				//
				int iof = fn.ReverseFind('.');
				if( iof > 0 )
				{
					BOOL Found = FALSE;
					fn = fn.Left(iof);
					iof = fn.Find("@");
					CString package = fn;
					if( iof > 0 )
					{
						CString value = fn.Left( iof );
						package = fn.Right( fn.GetLength() - iof - 1 );
						Found = doc->Pages.FindPart( &value, &package, 1 );
					}
					else
					{
						int it = -1;
						int ip = -1;
						for( CText * xt=doc->Pages.GetNextAttr( index_foot_attr, &it, &ip );
							 xt;
							 xt = doc->Pages.GetNextAttr( index_foot_attr, &it, &ip ) )
						{
							if( xt->m_str.Left(1) != package.Left(1) )
								continue;
							CString st = xt->m_str;
							ExtractComponentName( &st, NULL );
							if( st.Compare( package ) == 0 )
							{
								Found = TRUE;
								break;
							}
						}
					}
					if( Found )
					{
						SetFileAttributes(fp, FILE_ATTRIBUTE_NORMAL);
						SetFileAttributes(Name2, FILE_ATTRIBUTE_NORMAL);
						CopyFile( fp, Name2, 0 );
						if( STEP == 0 )
							SetFileAttributes(Name2, FILE_ATTRIBUTE_READONLY);
						else
							SetFileAttributes(fp, FILE_ATTRIBUTE_READONLY);
					}
				}
			}
		}
		CString bufStr = CDir;
		CDir = Path2;
		Path2 = bufStr;
	}
}
