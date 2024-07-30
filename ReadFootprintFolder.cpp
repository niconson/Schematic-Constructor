#include "stdafx.h"
#include "DlgAddMerge.h"
#include "ReadFootprintFolder.h"



int FindFootprintInFolder( CFreePcbDoc * doc, CString * fname, CString * old_fname, CString * CAT )
{
	CArray<CString> FootprintData;
	CString LibManSys = doc->m_app_dir+"\\FPC_EXE\\FreePCB_Libraries\\options.txt";
	CString catalog = "";
	if ( CAT )
		catalog = *CAT;
	else
	{
		CStdioFile ROpt;
		int OK = ROpt.Open( LibManSys, CFile::modeRead );
		if( OK )
		{
			ROpt.ReadString( catalog );
			ROpt.Close();
			if( catalog.GetLength() > 4 )
				catalog = catalog.Right( catalog.GetLength()-4 );
			catalog = catalog.Trim();
		}
	}
	if( catalog.GetLength() == 0 )
		return 0;
	if( catalog.Compare( doc->m_full_lib_dir ) == 0 )
		return 0;
	struct _stat buf;
	if( _stat( doc->m_full_lib_dir, &buf ) && _stat( catalog, &buf ) == 0 )
	{
		int answ;
		if( doc->m_full_lib_dir.GetLength() )
		{
			CString qstr;
			qstr.Format("The current footprint library folder does not exist:\n\n%s\n\n"\
			"Want to set the following folder as your footprint library folder?\n\n%s", doc->m_full_lib_dir, catalog );
			answ = AfxMessageBox(qstr, MB_ICONQUESTION | MB_YESNO);
		}
		else
		{
			answ = IDYES;
		}
		if( answ == IDYES )
		{
			doc->m_full_lib_dir = catalog;	
			ReadFootprintFolder( doc );
			if( FindFootprint( doc, fname ) )
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}

	// save lib dir
	CString mem_lib_dir = doc->m_full_lib_dir;
	doc->m_full_lib_dir = catalog; 
	ReadFootprintFolder( doc, fname, &FootprintData );

	// restore lib dir
	doc->m_full_lib_dir = mem_lib_dir;

	// checking
	if( FootprintData.GetSize() )
	{
		CString str;
		str.Format(				"Warning! Footprint %s was not found "\
								"in the current footprint library folder, which "\
								"is specified in the project settings (Project >> Options):\n\n%s"\
								"\n\nFootprint %s found in another directory:\n\n%s\n\n"\
								"Want the "PROGRAM_NAME" to save this footprint in the current library? (is recommended)", *fname, mem_lib_dir, *fname, catalog );
		int retQ = AfxMessageBox( str, MB_ICONQUESTION | MB_YESNO );
		if( retQ == IDYES )
		{
			CDlgAddMerge dlg;
			dlg.Doc = doc;
			dlg.m_mode = dlg.M_COPY_FOOTPRINT;
			dlg.m_combo_box.RemoveAll();
			dlg.m_combo_box.Add( *old_fname );
			dlg.m_merge_name = *fname;
			int ret;
			CString newname="";
			BOOL FileAlreadyExists;
			struct _stat buf;
			do
			{
				FileAlreadyExists = 0;
				ret = dlg.DoModal();
				if( ret != IDOK )
					break;
				newname = (mem_lib_dir + "\\" + dlg.m_merge_name + ".fpl");
				FileAlreadyExists |= FindFootprint( doc, &dlg.m_merge_name );
				FileAlreadyExists |= !(_stat( newname, &buf ));
				if( FileAlreadyExists )
				{
					AfxMessageBox("This name already used. Please enter different name", MB_ICONERROR );
				}
			} while( FileAlreadyExists );
			//write footprint data
			if( ret == IDOK )
			{
				CStdioFile wr;
				int ok = wr.Open( newname, CFile::modeCreate | CFile::modeWrite );
				if( !ok )
				{
					CString log_message;
					log_message.Format( "ERROR: Unable to open file \r\n\"%s\"\r\n", newname );
				}
				else
				{
					wr.WriteString( "name: \"" + dlg.m_merge_name + "\"\n" );
					for( int iter=1; iter<FootprintData.GetSize(); iter++ )
					{
						wr.WriteString( FootprintData.GetAt(iter) + "\n" );
					}
					wr.Close();
					doc->m_FootprintNames.Add( dlg.m_merge_name );
					return FootprintData.GetSize();
				}
			}
		}
		return 1;
	}
	return 0;
}

BOOL UsedAlienLibrary( CFreePcbDoc * doc )
{
	return (BOOL)doc->m_footprint_extention.GetLength();
}

BOOL FindFootprint( CFreePcbDoc * doc, CString * foot )
{
	for( int ii=0; ii<doc->m_FootprintNames.GetSize(); ii++ )
		if( foot->CompareNoCase( doc->m_FootprintNames[ii] ) == 0 )
		{
			return TRUE;
		}
	return FALSE;
}

void ClearFootprintNames( CFreePcbDoc * doc )
{
	doc->m_FootprintNames.RemoveAll();
	doc->m_footprint_extention = "";
}

void ReadFootprintFolder( CFreePcbDoc * doc, CString * FindFootprintName, CArray<CString> * Data )
{
	if( Data == NULL )
		ClearFootprintNames( doc );

	CFileFind finder;
	CString first_f, search_str;
	//
	first_f = "\\*.fpl";
	search_str = doc->m_full_lib_dir + first_f;
	BOOL bWorking = finder.FindFile( search_str );
	if( bWorking )
	{
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			CString fn = finder.GetFilePath();
			if( !finder.IsDots() )
				if( !finder.IsDirectory() )
				{
					ReadFootprintNames(doc,&fn,FindFootprintName,Data);
					if( Data )
						if( Data->GetSize() )
							return;
				}
		}
	}
	else 
	{
		first_f = "\\*.fpc";
		search_str = doc->m_full_lib_dir + first_f;
		bWorking = finder.FindFile( search_str );
		if( bWorking )
		{
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				CString fn = finder.GetFilePath();
				if( !finder.IsDots() )
					if( !finder.IsDirectory() )
					{
						ReadFootprintNames(doc,&fn,FindFootprintName,Data);
						if( Data )
							if( Data->GetSize() )
								return;
					}
			}
		}
		else if( Data == NULL )
		{
			first_f = "\\*.*";
			search_str = doc->m_full_lib_dir + first_f;
			BOOL bWorking = finder.FindFile( search_str );
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				CString fn = finder.GetFilePath();
				if( !finder.IsDots() )
					if( !finder.IsDirectory() )
					{
						CString s = finder.GetFileName();
						int rf = s.ReverseFind('.');
						if( rf > 0 )
						{
							doc->m_FootprintNames.Add( s.Left(rf) );
							if( doc->m_footprint_extention.GetLength() == 0 )
								doc->m_footprint_extention = s.Right( s.GetLength() - rf );
						}
						else
							doc->m_FootprintNames.Add( s );
					}
			}
		}
	}
}

void ReadFootprintNames( CFreePcbDoc * doc, CString * FPL, CString * FindFootprintName, CArray<CString> * mFootprintData )
{
	CStdioFile file;
	int OK = file.Open( *FPL, CFile::modeRead );
	if( !OK )     
	{
		CString mess = "Unable to open library file\n\n \"" + *FPL + "\"";
		AfxMessageBox( mess, MB_ICONERROR );
	}

	CString instr, bPacFound="", shape_name="";
	int pos = 0;
	int last_ih = 0;
	int last_if = -1;
	if( OK ) 
	{
		if( mFootprintData )
			mFootprintData->RemoveAll();
		BOOL bFound = FALSE;
		while( file.ReadString( instr ) )
		{
			instr.Trim();
			if( instr.Left(6) == "[board" )
			{
				file.Close();
				if( FindFootprintName && bFound == 0 && bPacFound.GetLength() )
				{
					ReadFootprintNames( doc, FPL, &bPacFound, mFootprintData );
				}
				return;
			}
			else if( instr.Left(5) == "name:" )
			{
				if( bFound )
				{
					file.Close();
					return;
				}
				bFound = FALSE;
				shape_name = instr.Right( instr.GetLength()-5 );
				shape_name = shape_name.Trim();
				if( shape_name.Right(1) == "\"" )
					shape_name = shape_name.Left( shape_name.GetLength() -1 );
				if( shape_name.Left(1) == "\"" )
					shape_name = shape_name.Right( shape_name.GetLength() -1 );
				shape_name = shape_name.Trim();

				if( mFootprintData == NULL )
					doc->m_FootprintNames.Add( shape_name );
				else if( FindFootprintName )
				{
					if( FindFootprintName->Compare( shape_name ) == 0 )
					{
						bFound = TRUE;
					}
				}
			}
			else if( instr.Left(8) == "package:" && FindFootprintName && bPacFound.GetLength() == 0 )
			{
				CString pac_name = instr.Right( instr.GetLength()-8 );
				pac_name = pac_name.Trim();
				if( pac_name.Right(1) == "\"" )
					pac_name = pac_name.Left( pac_name.GetLength() -1 );
				if( pac_name.Left(1) == "\"" )
					pac_name = pac_name.Right( pac_name.GetLength() -1 );
				pac_name = pac_name.Trim();

				if( FindFootprintName->Compare( pac_name ) == 0 )
					bPacFound = shape_name;
			}
			//
			if( bFound )
				mFootprintData->Add(instr);
		}
		file.Close();
	}
}