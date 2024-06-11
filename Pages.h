#pragma once
#include "stdafx.h"
#include "PolyLine.h"
#include "DesignRules.h"
#include "DlgLog.h"
#include "Merge.h"
#define MAX_PAGES		8
#define MAX_FAVORITES	8
struct CFreeasyPage
{
	CString name;
	CArray<CPolyLine> * PolyLines;
	CAttrib * Attributes;
	CTextList * TextList;
	CDisplayList * DisplayList;
	DRErrorList * DREList;
	CUndoList * UndoList;
	CUndoList * RedoList;
	CText * p_sel_text;
	id p_sel_id;
	int p_cursor_mode;
};

class CPageList
{
private:
	int m_pcb_index[MAX_PAGES+2]; // (+2 virtual pages)
	int max_num_pages;
	int m_num_pages;
	int ThisNumber;
	CFreeasyPage * ThisPage;
	CArray<CFreeasyPage> pgs;
	CArray<CPolyLine> ** PolyLines;
	CAttrib ** Attributes;
	CTextList ** TextList;
	CDisplayList ** DisplayList;
	DRErrorList ** DREList;
	CUndoList ** UndoList;
	CUndoList ** RedoList;
	CText ** p_sel_text;
	id * p_sel_id;
	int * p_cursor_mode;
	SMFontUtil * m_smfontutil;
	///CArray<CString> m_block_suffixes[MAX_PAGES];

public:
	
	int m_netlist_page_mask[MAX_PAGES];
	int m_netlist_format;
	int m_page_mirror_mask;
	int m_get_suffixes;		// complex part suffixes
	CString m_get_prefixes; // part prefixes
	CArray<CString> m_pcb_names;
	CPageList();
	~CPageList();
	void Assignment(		CArray<CPolyLine> ** polylines,
							CAttrib ** attr,
							CTextList ** t_list,
							CDisplayList ** d_List,
							DRErrorList ** dre_List,
							CUndoList ** UList,
							CUndoList ** RList,
							CText ** sel_text,
							id * sel_id,
							int * cur_mode,
							int max_n_pages,
							SMFontUtil * smfont );
	int Next();
	int Prev();
	int Clear(int index);
	int Remove(int index);
	int ClearAll();
	int RemoveAll();
	int GetNumPages();
	int GetActiveNumber();
	int AddNew( CString * Name, BOOL insert=0 );
	int CreateVirtualPages();
	int DeleteVirtualPages();
	int VirtualPagesPresent();
	int SwitchToVP1();
	int SwitchToVP2();
	int Rename( int index, CString * Name );
	int SwitchToPage( int page_index );
	//
	// related w PCB
	int GetPcbIndex( int ipage, BOOL incCP = TRUE );
	int GetCurrentPcbIndex();
	int ReturnPageByPCB( CString * Name );
	int IsThePageIncludesCP( int cppg=-1 );
	CString GetCurrentPCBName( int ip=-1 );
	BOOL IsThePageIncludedInNetlist( int ipage, BOOL bAny=FALSE );
	void Protection( int pcb_num = MAX_PAGES );
	void InvalidatePcbIndexes();
	void CheckPCBNames( CString * folder );
	//
	CString GetPicture( CString * V, CString * P );
	int FindPart( CString * Part, CText ** found,  BOOL StartOver, BOOL use_netlist_page_mask = 0, int it = -1, int ip = 0 );
	int FindPart( CString * V, CString * P, BOOL bRemoveIllegalSymbols, CArray<CText*> * ExPart=NULL, CText ** FoundPart=NULL, int * ipage=NULL );
	int FindPartInRect( RECT * pR, CString * Ref, CString * V, CString * P, int ip );
	int FindMerge( int index );
	id  FindPrintableArea( id * prevID, BOOL bIncludeSameIndex=0 );
	int GetPrintableAreaIndex( CString * Part );
	int ShiftPrintableAreaIndexes( id * pID, int di );
	int GetLastFreeNetIndex( BOOL use_netlist_page_mask = 0 );
	int GetSelCount( int page_index, int * flags, BOOL TEST=FALSE );
	int IsAttr( CText * t );
	int CheckTemplate( CString * src ); 
	int GetNetPages( CText * net, int * pageMask, int CurrentPage );
	int GetBOMCount( CString * Value, CString * Package, CString * Details, int detail_column_width, int * DLen, int inc_list, int ex_list, CString * LINK=NULL, CString * SUFF=NULL, int ip=-1 );
	int FindBOMNextLine(	CString * Value, 
							CString * Package, 
							CString * Details, 
							int detail_column_width, 
							BOOL ignore_without_value, 
							int inc_list=0, 
							int ex_list=0, 
							BOOL StartOver = FALSE, int * CPPresent=NULL );
	int SortDetails( CString * Details, int column_width );
	int GetNetPins( CString * NetName, 
					CString * Pins, 
					int i_format, 
					CString * NewNameOrSuffix, 
					CString * NetControl, 
					int i_page = -1,
					CString * ASUFF=NULL,
					RECT * inRect=NULL );
	int IncorrectElectricalCirquit( CString * NetName, int * X, int * Y, int page );
	int GetPageIndex( CString * name );
	void GetPageName( int num, CString * name );
	//CString CheckBlockPart( CString * R, int orig_page, CString * V, CString * P, int * nparts );
	int GetBlockParts( CString * destination, 
						CText * description, 
						int iFormat, 
						int * BlkNum, 
						CDlgLog * report, 
						CString * ASUFF=NULL,
						CString * FindPart=NULL,
						int fromSelection=-1,
						CArray<CString> * Partlist=NULL );
	int GetBlockNets(	CString * destination, 
						CText * description, 
						int iFormat, 
						int * BlkNum, 
						CString * NetNameFormat, 
						CString * NetControl, 
						CDlgLog * report,
						CString * ASUFF=NULL );
	///CArray<CString> * GetBlockSuffixes();
	void RemoveEmptyMergers( StrList * m_ml );
	CText * GetText( CString * str );
	CText * GetNextAttr( int i_attr, int * i_text, int * i_page, BOOL use_netlist_page_mask = 0 );	
	CText * FindBLK( int * it, int * ip, BOOL any_page=0 );
	CText * FindBLK_PTR( int * it, int * ip, BOOL any_page=0 );
	CText * FindBLK_PTR( CString * description, int page );
	CText * FindCPartElement( CString * FullRefDes, int * ipage, int * start_i, BOOL bFoundBlkPtr=FALSE );
	int FindBlkPtrs( int polylineIndex, BOOL bSelect=0 );
	CTextList * GetTextList( int ipage, int attr );
	CDisplayList * GetDisplayList( int ipage );
	CArray<CPolyLine> * GetPolyArray( int ipage );
	CAttrib * GetCurrentAttrPtr();
	DRErrorList * GetDRErrorList( int ipage );
	int InvalidateMerges( int mergeIndex );
};