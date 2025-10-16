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
	int FindNextPart( CString * ref, CString * V, CString * F, int iof_ref_list, int * printable_area_index=NULL );
	int GetBomRepeats( CString * pref, CString * pref2, CString * V, CString * F, int iof_ref_list);

private:
	enum{LNumber=0,RefDes,CName,Value,Footprint,Quantity,CURL,PGI,Comment,MaxCols};
	enum{NUM_PAGES= MAX_PAGES*2};
	CString MOVABLE;
	CString WARNING;
	CString PR_NAME;
	CString PCB_NAME;
	CString FILE_NAME;
	CString Sources;

	CFreePcbDoc * m_doc;
	CArray<CText*> TextData[NUM_PAGES];
	CArray<CString> Partlist;
	CArray<CString> Headers;
	int Xs[MaxCols];
	int curY;
	int shiftY;
	int RangeY;
	int textH;
	int textW;
	int fontNum;
	int pdfOptions;
	int nPages;
	int pgBottom[NUM_PAGES];
	int pgTop[NUM_PAGES];
	int e_reflist;
	CArray <int> i_reflist;
	int pLayer;
	int tLayers[MaxCols];
	CString Suffix[MaxCols];
	int COL3W, COL9W;
	POINT globalStartData; // globalStartData.x ==> start_Page  globalStartData.y ==> start_Y

	void CopyTagPointers(int h_index, BOOL ERASE=FALSE);
	int LineTest(int page, int Y);
	void RemoveEmptyRows();
	int ReadParamsFromTagFile();
	int PushRows( int Y, int ip, int copyingMode=0 );
	int PushBack( int Y, int ip );
	int RedrawNumbers();
	int AddObject( int page, int CNT, CString * R, CString * V, CString * F, int pgindex );
	int AddPString( CText * T, int n_page, int n_pushes, int col );
	//TagStruct * FindNextPart( CString * pref, int * num, CString * suff, CString * V, CString * F );
	int GetPartID( CString * ref );
	CText * ParseString( CText * T, int W );
};



