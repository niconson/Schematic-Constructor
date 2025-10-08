// DlgProtection.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgCreateBranch.h"


// DlgProtection dialog

IMPLEMENT_DYNAMIC(DlgCreateBranch, CDialog)
DlgCreateBranch::DlgCreateBranch(CWnd* pParent /*=NULL*/)
	: CDialog(DlgCreateBranch::IDD, pParent)
{
}

DlgCreateBranch::~DlgCreateBranch()
{
}



void DlgCreateBranch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_4, mb_increment);
	DDX_Control(pDX, IDC_RADIO_5, mb_suffix);
	DDX_Control(pDX, IDC_EDIT_SUFF, m_edit);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		mb_increment.SetCheck(1);
		mb_suffix.SetCheck(0);
		OnBnClickedCheckCopy();
	}
	else
	{
		// outgoing
		// copy project
		m_edit.GetWindowTextA( m_suffix );
		if( m_suffix.GetLength() == 0 )
			if( mb_suffix.GetCheck() )
				pDX->Fail();
		int pos = 0;
		int mem_num = -1;
		int mem_len = -1;
		BOOL bWorking[MAX_POS];
		CFileFind finder[MAX_POS];
		CString search_str[MAX_POS];
		CString Path = m_doc->m_pcb_full_path;
		CString Path2 = m_doc->m_parent_folder;
		int rf = Path.ReverseFind('\\');
		int add = 0;
		struct _stat buf;
		if( rf > 0 )
		{
			Path = Path.Left(rf);
			rf = Path.ReverseFind('\\');
			if( rf > 0 )
			{

				Path2 = Path.Left(rf);
				CString str;
				CString prefix="", postfix="", prefSumm="";
				CString folder_name = Path.Right(Path.GetLength() - rf - 1);
				if( mb_increment.GetCheck() )
				{
					int LEN = 0;
					int num = GetRightNumber( &folder_name, &prefix, &postfix, &LEN );
					mem_num = num;
					mem_len = LEN;
					add++;
					str.Format("%d",num+add);
					while( str.GetLength() < LEN )
						str = "0" + str;
					if( Path2.Right(1) == "\\" )
						Path2 += (prefix + str + postfix);
					else
						Path2 += ("\\" + prefix + str + postfix);
				}
				else
				{
					Path2 += ("\\" + folder_name + m_suffix);
				}
				if( mb_increment.GetCheck() ) while( _stat( Path2, &buf ) == 0 )
				{
					add++;
					Path2 = Path.Left(rf);
					CString str;
					str.Format("%d",mem_num+add);
					while( str.GetLength() < mem_len )
						str = "0" + str;
					if( Path2.Right(1) == "\\" )
						Path2 += (prefix + str + postfix);
					else
						Path2 += ("\\" + prefix + str + postfix);
				}
			}
		}
		if( _stat( Path2, &buf ) )
		{
			CString str;
			int err = _mkdir( Path2 );
			if( err )
			{
				str.Format(G_LANGUAGE == 0 ? 
					"Unable to create folder \"%s\"":
					"Невозможно создать папку \"%s\"", Path2);
				AfxMessageBox( str, MB_OK );
				pDX->Fail();
			}
		}
		else
		{
			CString str;
			str.Format(G_LANGUAGE == 0 ? "Error! %s folder already exists":"Ошибка! Папка %s уже существует", Path2);
			AfxMessageBox( str, MB_ICONERROR );
			pDX->Fail();
		}
		CArray<CString> PCB_OLD;
		CArray<CString> PCB_NEW;
		PCB_OLD.SetSize(0);
		PCB_NEW.SetSize(0);
		search_str[pos] = Path + "\\*.fpc";
		bWorking[pos] = finder[pos].FindFile( search_str[pos] );
		do
		{
			if( bWorking[pos] )
			{
				bWorking[pos] = finder[pos].FindNextFile();
				if( !finder[pos].IsDots() )
				{
					CString fn = finder[pos].GetFilePath();
					CString name = finder[pos].GetFileName();
					if( !finder[pos].IsDirectory() )
					{
						if( search_str[pos].Right(1) == "*" && fn.Right(4) == ".fpc" )
							continue;
						if( fn.Right(4) == ".txt" || 
							fn.Right(4) == ".net" ||
							fn.Right(2) == ".b" ||
							fn.Right(5) == ".rnmb" ||
							fn.Right(4) == ".pdf" ||
							fn.Right(4) == ".lib" ||
							name.Left(4) == "CAM(" ) 
						{
							if( fn.Find( "openscad" ) > 0 && fn.Right(4) != ".lib" )
								CopyFile( fn, Path2+"\\"+name, 1 );
						}
						else
						{
							if( fn.Right(4) == ".cds" ||
								fn.Right(4) == ".fpc" ||
								fn.Right(5) == ".scad" ||
								fn.Right(8) == ".PCBVIEW")
							{
								{
									CString str = name.Left( name.GetLength() - 4 );
									PCB_OLD.Add( str );
									if( mb_increment.GetCheck() )
									{		
										CString prefix="", postfix="", prefSumm="";
										int LEN = 0;
										int num = GetRightNumber( &str, &prefix, &postfix, &LEN );
										
										if( LEN )
										{
											CString s;
											s.Format(G_LANGUAGE == 0 ? 
												"The number in the file name %s is %d. Increment this file number?":
												"Номер в имени файла %s равен %d. Увеличить этот номер файла?", name, num);
											int Q = AfxMessageBox(s, MB_YESNOCANCEL);
											if( Q == IDYES )
											{
												if( num == mem_num && LEN == mem_len )
													num += add;
												else
													num++;
											}
											else if( Q == IDCANCEL )
												num = mem_num+add;
											//
											str.Format("%d",num);
										}	
										else
											str = "";
										
										while( str.GetLength() < LEN )
											str = "0" + str;
										name = prefix + str + postfix;// + fn.Right(4);
									}
									else if( name.Left(4) != "tag." && name.Left(4) != "bom." )
									{
										name = str + m_suffix;// + fn.Right(4);
									}
								}
								PCB_NEW.Add( name );
								name += fn.Right(4);
								CString TMP = Path2+"\\"+name+".tmp";
								CopyFile( fn, TMP, 1 );
								CStdioFile File;
								int ok = File.Open( LPCSTR(TMP), CFile::modeRead, NULL );
								if( ok )
								{
									CStdioFile WrFile;
									int ok2 = WrFile.Open( LPCSTR(Path2+"\\"+name), CFile::modeCreate | CFile::modeWrite, NULL );
									if( ok2 )
									{
										CString instr;
										while( File.ReadString( instr ) )
										{
											if( instr.Left(18) == "netlist_file_path:" || instr.Left(11) == "CAM_folder:" )
											{
												int idx = instr.Find(":");
												if( idx > 0 )
													WrFile.WriteString( instr.Left(idx)+": \"\"\n" );
											}
											else if( instr.Left(12) == "description:" ||
													 instr.Left(12) == "add_new_page" ||
													 instr.Left(12) == "rename_page:")
											{							
												for( int iname=0; iname<PCB_OLD.GetSize(); iname++ )
												{
													if( instr.Replace( PCB_OLD.GetAt( iname ), PCB_NEW.GetAt( iname ) ) )
														break;
												}
												WrFile.WriteString( instr+"\n" );
											}
											else if( instr.Left(5) == "text:" ||
													 instr.Left(9) == "pcb_name:" || 
													 instr.Left(13) == "project_name:" )
											{
												for( int iname=0; iname<PCB_OLD.GetSize(); iname++ )
												{
													if( instr.Replace( "\""+PCB_OLD.GetAt( iname )+".", "\""+PCB_NEW.GetAt( iname )+"." ) )
														break;
													else if( instr.Replace( "\""+PCB_OLD.GetAt( iname )+"\"", "\""+PCB_NEW.GetAt( iname )+"\"" ) )
														break;
													else if( instr.Left(5) == "text:" )
													{
														CString instr_low = instr;
														instr_low.MakeLower();
														CString PCB_OLD_LOW = PCB_OLD.GetAt( iname );
														PCB_OLD_LOW.MakeLower();
														if( instr_low.Replace( "\""+PCB_OLD_LOW+"\"", "\""+PCB_NEW.GetAt( iname )+"\"" ) )
															instr = instr_low;
														else if( instr_low.Replace( "\""+PCB_OLD_LOW+".", "\""+PCB_NEW.GetAt( iname )+"." ) )
															instr = instr_low;
													}
												}
												WrFile.WriteString( instr+"\n" );
											}
											else
												WrFile.WriteString( instr+"\n" );
										}
										WrFile.Close();
									}
									File.Close();
									DeleteFile( TMP );
								}
							}
							else
								CopyFile( fn, Path2+"\\"+name, 1 );
						}
					}
					//else if( name.CompareNoCase( "reports" ) == 0 )
					//{
					//}
					else if( name.Compare( "related_files" ) == 0 || pos > 0 )
					{
						pos++;
						if( pos >= MAX_POS )
							break;
						search_str[pos] = fn + "\\*.fpc";
						Path2 += ("\\" + name);	
						if( _stat( Path2, &buf ) )
						{
							CString str;
							int err = _mkdir( Path2 );
							if( err )
							{
								str.Format(G_LANGUAGE == 0 ? 
									"Unable to create folder \"%s\"":
									"Невозможно создать папку \"%s\"", Path2);
								AfxMessageBox( str, MB_OK );
								pDX->Fail();
							}
						}
						bWorking[pos] = finder[pos].FindFile( search_str[pos] );
					}
				}
			}
			if( !bWorking[pos] )
			{
				if( search_str[pos].Right(1) != "*" )
				{
					search_str[pos].Truncate( search_str[pos].GetLength()-4 );
					if( search_str[pos].Right(1) != "*" )
						ASSERT(0);
					bWorking[pos] = finder[pos].FindFile( search_str[pos] );
				}
			}
			while( !bWorking[pos] )
			{
				pos--;
				if( pos < 0 )
					break;
				rf = Path2.ReverseFind('\\');
				if( rf > 0 )
					Path2 = Path2.Left(rf);
			}
			if( pos < 0 )
				break;
		} while ( bWorking[pos] );
		ShellExecute(	NULL,"open", Path2, NULL, NULL, SW_SHOWNORMAL );
	}
}

BEGIN_MESSAGE_MAP(DlgCreateBranch, CDialog)
	ON_BN_CLICKED(IDOK, &DlgCreateBranch::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_COPY, &DlgCreateBranch::OnBnClickedCheckCopy)
	ON_BN_CLICKED(IDC_RADIO_4, &DlgCreateBranch::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO_5, &DlgCreateBranch::OnBnClickedRadio5)
END_MESSAGE_MAP()


void DlgCreateBranch::Initialize( CFreePcbDoc * doc )
{
	m_doc = doc;
}



void DlgCreateBranch::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialog::OnOK();
}


void DlgCreateBranch::OnBnClickedCheckCopy()
{
	// TODO: добавьте свой код обработчика уведомлений
	if( mb_increment.GetCheck() )
	{
		m_edit.EnableWindow(0);
	}
	else
	{
		m_edit.EnableWindow(1);
	}
}


void DlgCreateBranch::OnBnClickedRadio4()
{
	// TODO: добавьте свой код обработчика уведомлений
	OnBnClickedCheckCopy();
}


void DlgCreateBranch::OnBnClickedRadio5()
{
	// TODO: добавьте свой код обработчика уведомлений
	OnBnClickedCheckCopy();
}
