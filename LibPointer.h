// CopyRight by Duxah's LTD
//
#pragma once
#include "stdafx.h"

struct INDEX
{
	int path_index;
	int merge_index;
	INDEX * next;
};

class CLibPointer
{
public:
	CLibPointer();
	~CLibPointer();
	int IsEmpty();
	CArray<CString> * GetLibrary();
	int AddItem( CString * ValuePackage, int merge_index=-1 );
	void SetPath( CString * current_path );
	CString GetPath( CString * ValuePackage, int index=0, int * mergeIndex=NULL );
	CArray<CString> * GetPathes( CString * ValuePackage );
	CArray<CString> * GetPathes();
	CArray<CString> * GetPartlistByPath( CString * path );
	int isPresent( CString * ValuePackage );
	void Clear();
	void ClearTestString();
	void ShowTestString();
private:
	CString Alpha;
	CString m_test_string;
	enum{ AlphaLen = 36 };
	CArray<CString> Items[AlphaLen+1];
	CArray<INDEX*> m_start[AlphaLen+1];
	CArray<INDEX*> m_end[AlphaLen+1];
	CArray<CString> Pathes;
	CArray<CString> TempLibArray;
	CArray<CString> TempPathArray;
	int iof( CString * Str, CArray<CString> *S );
	int m_current_path_index;
};