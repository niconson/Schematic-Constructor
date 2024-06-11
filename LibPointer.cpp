// CopyRight by Duxah's LTD
//
#include "stdafx.h"
#include "FreeSch.h"
#include "LibPointer.h"


CLibPointer::CLibPointer()
{
	Alpha = SHORTCSORT;
	if( AlphaLen != Alpha.GetLength() )
		ASSERT(0);
	Clear();
	ClearTestString();
}

CLibPointer::~CLibPointer()
{

}

void CLibPointer::ClearTestString()
{
	m_test_string = "";
}

void CLibPointer::ShowTestString()
{
	if( m_test_string.GetLength() )
	{
		AfxMessageBox(m_test_string, MB_ICONERROR);
	}
}

int CLibPointer::iof( CString * Str, CArray<CString> *S )
{
	int strGL = Str->GetLength();
	int io = Str->Find("] ",1);
	CString Str2="";
	if( io > 0 )
		Str2 = Str->Right( strGL-io-2 );


	for( int i=S->GetSize()-1; i>=0; i-- )
	{
		if( Str->Compare( S->GetAt(i) ) == 0 )
			return i;
		else if( S->GetAt(i).Left(1) == "[" )
		{
			CString * tmps = &S->GetAt(i);
			io = tmps->Find("] ",1);
			if( io > 0 )
			{
				int sGL = tmps->GetLength();
				if( Str->Compare( tmps->Right( sGL-io-2 ) ) == 0 )
					return i;
				if( Str2.GetLength() )
					if( Str2.CompareNoCase( tmps->Right( sGL-io-2 ) ) == 0 )
					{
						if( m_test_string.GetLength() == 0 )
							m_test_string = "The reused part has a different designation in the second case, the prefixes or case does not match!\n\n";
						if( m_test_string.Find( *Str ) <= 0 )
							m_test_string += *Str + "\n";
						if( m_test_string.Find( *tmps ) <= 0 )
							m_test_string += *tmps + "\n";
					}
			}
		}
	}
	return -1;
}

void CLibPointer::SetPath( CString * current_path )
{
	m_current_path_index = iof( current_path, &Pathes );
	if( m_current_path_index == -1 )
	{
		Pathes.Add( *current_path );
		m_current_path_index = Pathes.GetSize() - 1;
	}
}

int CLibPointer::IsEmpty()
{
	for( int i=0; i<=AlphaLen; i++ )
		if( Items[i].GetSize() )
			return 0;
	return 1;
}

CArray<CString> * CLibPointer::GetLibrary()
{
	TempLibArray.RemoveAll();
	for( int i=0; i<=AlphaLen; i++ )
		for( int ii=0; ii<Items[i].GetSize(); ii++ )
			TempLibArray.Add( Items[i].GetAt(ii) );
	return &TempLibArray;
}

int CLibPointer::AddItem( CString * ValuePackage, int merge_index )
{
	static int pI = -1;
	static CString pStr = "";
	if( ValuePackage->GetLength() == 0 )
	{
		pStr = ""; // reset
		return 0;
	}
	if( pI == m_current_path_index )
		if( ValuePackage->Compare( pStr ) == 0 )
			return 0;
	CString F = ValuePackage->GetAt( ValuePackage->GetLength()-1 );
	F.MakeUpper();
	int index = Alpha.Find( F );
	if( index < 0 )
		index = AlphaLen;
	if( m_current_path_index >= 0 )
	{
		pI = m_current_path_index;
		pStr = *ValuePackage;
		int item = iof( ValuePackage, &Items[index] );
		if( item == -1 )
		{
			INDEX * idx = new INDEX;
			idx->path_index = m_current_path_index;
			idx->merge_index = merge_index;
			idx->next = NULL;
			Items[index].Add( *ValuePackage );	
			m_start[index].Add( idx );
			m_end[index].Add( idx );
			return 1;
		}
		else 
		{
			if( m_end[index].GetAt(item)->path_index == m_current_path_index )
				return 0;
			//
			INDEX * idx = new INDEX;
			idx->path_index = m_current_path_index;
			idx->merge_index = merge_index;
			idx->next = NULL;
			m_end[index].GetAt(item)->next = idx;
			m_end[index].SetAt( item, idx );
			if( m_start[index].GetAt(item)->next == NULL )
				ASSERT(0);
			return 2;
		}
	}
	return 0;
}

int CLibPointer::isPresent( CString * ValuePackage )
{
	if( ValuePackage->GetLength() == 0 )
		return 0;
	CString F = ValuePackage->GetAt( ValuePackage->GetLength()-1 );
	F.MakeUpper();
	int index = Alpha.Find( F );
	if( index < 0 )
		index = AlphaLen;
	if( m_current_path_index >= 0 )
	{
		int item = iof( ValuePackage, &Items[index] );
		if( item >= 0 )
		{
			for( INDEX*check=m_start[index].GetAt(item); check; check=check->next )  
				if( check->path_index == m_current_path_index )
					return TRUE;
		}
	}
	return FALSE;
}

CString CLibPointer::GetPath( CString * ValuePackage, int get_by_index, int * mergeIndex )
{
	if( ValuePackage->GetLength() == 0 )
		return "Error! String is empty";
	
	CString F = ValuePackage->GetAt( ValuePackage->GetLength()-1 );
	F.MakeUpper();
	int index = Alpha.Find( F );
	if( index < 0 )
		index = AlphaLen;
	int item = iof( ValuePackage, &Items[index] );
	if( item == -1 )
		return "Error! String not found";
	int ip = m_start[index].GetAt(item)->path_index;
	if( mergeIndex )
		*mergeIndex = m_start[index].GetAt(item)->merge_index;
	if( get_by_index > 0 )
	{
		int count = 0;
		for( INDEX*idx=m_start[index].GetAt(item)->next; idx; idx=idx->next )
		{
			count++;
			if( count == get_by_index )
				if( idx->path_index >= 0 )
				{
					if( mergeIndex )
						*mergeIndex = idx->merge_index;
					return Pathes.GetAt( idx->path_index );
				}
		}
	}
	if( ip >= 0 )
		return Pathes.GetAt(ip);
	else
		return "Error! Path not found";
}

CArray<CString> * CLibPointer::GetPathes( CString * ValuePackage )
{
	if( ValuePackage->GetLength() == 0 )
		return NULL;

	CString F = ValuePackage->GetAt( ValuePackage->GetLength()-1 );
	F.MakeUpper();
	int index = Alpha.Find( F );
	if( index < 0 )
		index = AlphaLen;
	int item = iof( ValuePackage, &Items[index] );
	if( item == -1 )
		return NULL;
	int ip = m_start[index].GetAt(item)->path_index;
	if( ip >= 0 )
	{
		TempPathArray.RemoveAll();
		TempPathArray.Add( Pathes.GetAt(ip) );
		for( INDEX*idx=m_start[index].GetAt(item)->next; idx; idx=idx->next )
		{
			if( idx->path_index >= 0 )
				TempPathArray.Add( Pathes.GetAt( idx->path_index ) );
		}
		return &TempPathArray;
	}
	else
		return NULL;
}

CArray<CString> * CLibPointer::GetPathes()
{
	return &Pathes;
}

void CLibPointer::Clear()
{
	for( int i=0; i<=AlphaLen; i++ )
	{
		Items[i].SetSize(0);
		for( int ii=0; ii<m_start[i].GetSize(); ii++ )
			for( INDEX*idx=m_start[i].GetAt(ii); idx; )
			{
				INDEX*next=idx->next;
				delete idx;
				idx = next;
			}
		m_start[i].SetSize(0);
		m_end[i].SetSize(0);
	}
	Pathes.SetSize(0);
	m_current_path_index = -1;
	CString T = "";
	AddItem( &T ); //reset
}

CArray<CString> * CLibPointer::GetPartlistByPath( CString * path )
{
	TempPathArray.RemoveAll();
	for( int i=0; i<=AlphaLen; i++ )
	{
		for( int ii=0; ii<m_start[i].GetSize(); ii++ )
			for( INDEX*idx=m_start[i].GetAt(ii); idx; idx=idx->next )
			{
				if( path->CompareNoCase( Pathes.GetAt( idx->path_index ) ) == 0 )
					TempPathArray.Add( Items[i].GetAt(ii) ); 
			}
	}
	return &TempPathArray;
}