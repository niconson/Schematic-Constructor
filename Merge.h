#pragma once
#include "stdafx.h"
class StrList
{
#define dERROR -1
public:
	StrList(void)
	{
		m_size = 0;
		if( m_strings.GetSize() )
			Clear();
	}

	virtual ~StrList(void){}

	int AddNew ( CString mName )
	{
		if( !mName.GetLength() )
			return -1;
		for( int i=m_size-1; i>=0; i-- )
		{
			if( m_strings[i].CompareNoCase(mName) == 0 )
			{
				return i;
			}
		}
		m_strings.Add( mName );
		utility.Add(0);
		m_size++;
		return m_size-1;
	}

	void mark0 ()
	{
		for( int i=m_size-1; i>=0; i-- )
			utility[i] = 0;
	}

	void mark1 (int i, int value=1)
	{
		if( i >= 0 && i < m_size )
			utility[i] = value;
	}

	int GetMark ( int i )
	{
		if( i >= 0 && i < m_size )
			return utility[i];
		return 0;
	}

	int GetIndex ( CString mName )
	{
		for( int i=m_size-1; i>=0; i-- )
		{
			if( m_strings[i].Compare(mName) == 0 )
				return i;
		}
		return -1;
	}

	CString GetStr (int index)
	{
		if( index >= 0 && index < m_size )
			return m_strings[index];
		else
			return "";
	}

	void SetStr (int index, CString Name)
	{
		if( index >= 0 && index < m_size )
			m_strings[index] = Name;
	}

	int GetSize ()
	{
		return m_size;
	}

	void Clear()
	{
		m_strings.RemoveAll();
		utility.RemoveAll();
		m_size = 0;
	}

	void RemoveAt(int i)
	{
		m_strings.RemoveAt(i);
		utility.RemoveAt(i);
		m_size--;
	}

	BOOL GetSelected()
	{
		for( int i=m_size-1; i>=0; i-- )
			if( GetMark(i) )
				return true;
		return false;
	}

	void CopyFrom( StrList * from )
	{
		int new_size = from->GetSize();
		m_strings.SetSize(new_size);
		utility.SetSize(new_size);
		m_size = new_size;
		mark0();
		for( int i=new_size-1; i>=0; i-- )
		{
			m_strings[i] = from->GetStr(i); 
			utility[i] = from->GetMark(i);
		}
	}

private:
	CArray<CString> m_strings;
	CArray<int> utility;
	int m_size;
};

