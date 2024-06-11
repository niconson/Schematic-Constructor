#pragma once
#include "stdafx.h"

class BomInTable
{
public:
	BomInTable( CFreePcbDoc * doc );
	void MakeReport();
	CString GetURL( CString * v, CString * f, int * it, int * ip );
	CString GetCName( CString * V, CString * F );

private:
	enum{LNumber=0,RefDes,CName,Value,Footprint,Quantity,CURL,MaxCols};
	CString MOVABLE;
	CString WARNING;
	CString PR_NAME;
	CString PCB_NAME;
	CString FILE_NAME;
	CString Sources;

	CFreePcbDoc * m_doc;
	CArray<CText*> TextData[MAX_PAGES];
	CArray<CString> arrV;
	CArray<CString> arrF;
	CArray<CString> arrC;
	CArray<CString> arrR;
	int Xs[MaxCols];
	int Ws[MaxCols];
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
	int m_entire;

	void RemoveEmptyRows();
	int ReadParamsFromTagFile();
	void AddObject( int page, CString * CNT, CString * R, CString * V, CString * F );
	int PushRows( int Y, int ip );
	int PushBack( int Y, int ip );
	int FindNextRow( CString * R );
	int RedrawNumbers();
	int AddPString( CText * T, int n_page, int n_pushes, int col );
	CText * ParseString( CText * T, int W );
};

