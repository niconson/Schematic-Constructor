#pragma once
#include "stdafx.h"

struct TagStruct 
{
	CText * BEST_T;
	CString CP_REF;
	int n_parts;
	int BEST_IP;
};

class TagTable
{
public:
	void MakeReport( CFreePcbDoc * doc );
	int LoadPartlistFromString( CString * partlist );
	int FindNextPart( CString * ref, CString * V, CString * F, int * printable_area_index=NULL );
	int GetBomRepeats( CString * pref, CString * pref2, CString * V, CString * F );

private:
	enum{LNumber=0,RefDes,CName,Value,Footprint,Quantity,CURL,PGI,MaxCols};
	CString MOVABLE;
	CString WARNING;
	CString PR_NAME;
	CString PCB_NAME;
	CString FILE_NAME;
	CString Sources;

	CFreePcbDoc * m_doc;
	CArray<CText*> TextData[MAX_PAGES];
	CArray<CString> Partlist;
	int Xs[MaxCols];
	int curY;
	int shiftY;
	int textH;
	int textW;
	int fontNum;
	int pdfOptions;
	int nPages;
	int pgBottom[MAX_PAGES];
	int pgTop[MAX_PAGES];
	int e_reflist;
	int i_reflist;
	int pLayer;
	int tLayers[MaxCols];
	int COL3W;

	
	void RemoveEmptyRows();
	int ReadParamsFromTagFile();
	int PushRows( int Y, int ip );
	int PushBack( int Y, int ip );
	int RedrawNumbers();
	int AddObject( int page, int CNT, CString * R, CString * V, CString * F, int pgindex );
	int AddPString( CText * T, int n_page, int n_pushes, int col );
	//TagStruct * FindNextPart( CString * pref, int * num, CString * suff, CString * V, CString * F );
	int GetPartID( CString * ref );
	CText * ParseString( CText * T, int W );
};



