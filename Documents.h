// Documents.h : interface of the CFreePcbDoc class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_FREEPCBDOC_H__A00395C2_2CF4_4902_9C7B_CBB16DB58836__INCLUDED_)
#define AFX_FREEPCBDOC_H__A00395C2_2CF4_4902_9C7B_CBB16DB58836__INCLUDED_

#include "stdafx.h"
  
#pragma once
#include "Pages.h"
#include "TextList.h"
#include "PcbFont.h"
#include "SMfontutil.h"
#include "SMcharacter.h"
#include "UndoBuffer.h"
#include "UndoList.h"
#include "LibPointer.h"
#include "DlgDRC.h"
#include "DesignRules.h"
#include "Merge.h"
#include "LinkList.h"
#include "Cuid.h"

#define MAX_REF_LISTS	20
#define NL_PADS_PCB		0
#define NL_TANGO		1
#define NL_SPICE		2
#define NUM_SHORTCUTS   3
#define NUM_CAM_FOLDERS 7
#define setbit(reg,bit) reg|=(1<<bit)              //установить в регистре бит в 1       
#define clrbit(reg,bit) reg&=~(1<<bit)             //установить в регистре бит в 0       
#define invbit(reg,bit) reg^=(1<<bit)              //инвертировать в регистре бит       
#define getbit(reg,bit) ((reg & (1<<bit)) ? 1 : 0) //проверить состо€ние бита в регистре
///#define getbit(reg,bit) (__asm{ BT reg, bit })

#define RENUMBERING_PATTERN "%s --> %s"
#if G_LANGUAGE==0
#define PROGRAM_NAME "Schematic Constructor"
#define NETLIST_UPDATED "The netlist was updated but was not loaded into the PCB editor." /*this phrase must be the same in both PCB and CDS*/
#define NETLIST_WARNING "The netlist was loaded into FreePcb-2, but after that the file was not saved!" /*this phrase must be the same in both PCB and CDS*/
#else
#define PROGRAM_NAME "—хемјтор"
#define NETLIST_UPDATED "Ќетлист был обновлЄн через редактор схем, но пока не был загружен в редактор плат." /*this phrase must be the same in both PCB and CDS*/
#define NETLIST_WARNING "ќбновлЄнный нетлист загружали в редактор плат, но изменени€ не сохранили!" /*this phrase must be the same in both PCB and CDS*/
#endif
#define FILE_PROTECTED "Protected"
#define CLR_REF 50000

class CFreePcbDoc;
class CFreePcbView;

struct poly_ptr_array
{
	RECT Rect[4];
	CArray <CPolyLine*> Array[4];
};

static char attribute_str[num_attributes][64] = 
{ 
	"part name",
	"pin name",
	"net name",
	"description",
	"footprint",
	"part value"
};
struct undo_outline_poly {
	int layer;
	int wid;
	int m_bmp;
	int hatch_style;
	int closed;
	int ncorners;
	int merge_op;		// merge
	int submerge_op;	// submerge
	int num;
	int bShown;
	int next[num_attributes];
	CText * u_Attr[num_attributes];
	RECT grr;
	// array of undo_corners starts here
};

struct undo_corner {
	int x, y, num_contour, style, node;	// style is for following side
};

struct undo_move_origin {
	int x_off;
	int y_off;
};

class CFreePcbDoc : public CDocument
{
public:

protected: // create from serialization only
	CFreePcbDoc();
	DECLARE_DYNCREATE(CFreePcbDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFreePcbDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFreePcbDoc();
	void OnTimer(); 
	CString RunFileDialog( BOOL bOPEN, CString format );
	BOOL FileOpen( LPCTSTR fn );
	int FileClose();
	void CheckBOM();
	void ProjectModified( int flag=-1, BOOL b_clear_redo=TRUE );
	void InitializeNewProject(); 
	void SendInitialUpdate();
	int  CombineBoardOutlines( int ib1, int ib2 );
	void OPRefreshID();
	void ClipOP ( int i );
	CString ReadGraphics( CStdioFile * pcb_file, CArray<CPolyLine> * sm=NULL, int dx=0, int dy=0, RECT * gRECT=NULL, int i_page=-1, long long LinkToPart=0, int mINDEX=-1, BOOL ADD_SRC=TRUE );
	void WriteOutlinesPoly( CStdioFile * pcb_file, BOOL SEL_ONLY );
	int ReadOptions( CStdioFile * pcb_file, BOOL rColors );
	void WriteOptions( CStdioFile * file, BOOL bConfig, CString * pr_name = NULL );
//
	int ExportPCBNetlist( CStdioFile * file, UINT flags, UINT format, CArray<CString> * Partlist=NULL );
	undo_move_origin * CreateMoveOriginUndoRecord( int x_off, int y_off );
	static void MoveOriginUndoCallback( void * ptr, BOOL undo );
	undo_outline_poly * CreateOutlinePolyUndoRecord( CPolyLine * poly, int num );    
	static void OutlinePolyUndoCallback( void * ptr, BOOL undo );
	void OnFileAutoOpen( LPCTSTR fn );
	int FileSave( CString * folder, CString * filename, 
		CString * old_folder, CString * old_filename,    
		BOOL bBackup=TRUE );
	BOOL AutoSave();
	int GetGerberPathes(CString Path);
	void ResetUndoState();
	void PasteFromFile( CString pathname, BOOL bwDialog, int i_page=-1 );
	int DRC();
	int CreatePCBNets( int ITERATOR = 0 );
	int FindNodeLine(	int ic, 
						CText * net_ptr = NULL, 
						CText * net_label_ptr = NULL, 
						BOOL StartOver = 1, 
						BOOL foundAndReturned = 0, 
						CArray<CPolyLine*>*Q = NULL,
						RECT * PolyR = NULL,
						RECT * ArrayR = NULL);
	int FindNodeLines( CText * net_ptr, CText * net_label_ptr, int * Nodes, int lay_mask, poly_ptr_array * FQ, RECT * polyR );
	int FindFreePolyline( int lay_mask );
	int CheckFree( CPolyLine * p, int lay_mask );
	CText * GetFreeNet( CString * new_net, int x, int y, int H, int W );
	CTextList * GetTextListbyLayer( int L );
	CText * ScanAttr( CPolyLine * p, int * i_attr = NULL );  
	void AttributeIndexing( CText * t, int n_a );
	void AttributeIndexing( int layer );
	void HighlightLineAttributes( CPolyLine * p );
	void HighlightPartAttributes( CText * part, CText * pin );
	void SelectPart( CText * part, CText * pin = NULL, BOOL SCM=TRUE, int Swell=0 ); 
	void SelectLineAttributes( CPolyLine * p, BOOL incInvisible=0 );
	void SelectPartAttributes( CText * part, BOOL incInvisible=0 );
	void SelectGroupAttributes();
	void ReplacePartPattern( CText * newPattern );
	void UpdatePartPattern( CText * oldPattern, CText * newPattern );
	void CheckComponentInLibrary( CText * attr=NULL, CString * old_value=NULL, CString * old_footprint=NULL );
	void HighLightNet( CText * Net );
	int  SwitchToPage( int number, BOOL Invalidate=FALSE, BOOL UpdPartlist=TRUE );
	void RenamePage( int number, CString * NewName = NULL );
	void DeletePage( int number );
	void AddNewPage( CString * Name, BOOL insert=0 );
	void Renumber__Add( CString * old_ref, CString * new_ref, int current_page, int RemoveAt = -1, BOOL CP_RENUMBERING = FALSE );
	int RenumberComplexPart( CText * description, CString * old_suff, CString * new_suff, CString * ASUFF=NULL );
	int AttributeSync( CText * REF, CString * old_att, CText * new_att, int i_att );
	int GetNumPins( CText * part, CArray<CPolyLine> * arr = NULL );
	int GetNumAllPins( CText * part, CArray<CPolyLine> * arr = NULL );
	RECT GetPartBounds( CText * part, CText * pin );
	RECT HighlightSelRect();
	RECT GetPolylineBounds( RECT * maxPolyRect = NULL );
	int AddSource( CString * new_name );
	int ExtractBLK( CString * desc, CString * out, CString * link=NULL );
	int ExtractBLK_PTR( CString * desc, CString * name=NULL, CString * ref=NULL, CString * pin=NULL );
	int PinTextAlignment( CText * txt );
	void EnableMenuItems();
	void DisableMenuItems();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	  
public:
	enum{ AUTOPROTECT=MAX_PAGES }; 
	enum{ PROTECT_ARCHIVED=1 };
	double m_version;			// version number, such as "1.105"
	double m_file_version;		// the oldest version of FreePCB that can read
								// files created with this version
	double m_read_version;		// the version from the project file
	int m_protection;			// flags
	BOOL bNoFilesOpened;		// TRUE if no files have been opened
	BOOL m_edit_footprint;		// TRUE to edit footprint of selected part
	BOOL m_project_open;		// FALSE if no project open
	BOOL m_pcb_modified[MAX_PAGES];
	BOOL m_project_modified;	// FALSE if project not modified since loaded
	BOOL m_project_modified_since_autosave;	// FALSE if project not modified since loaded
	BOOL m_footprint_modified;	// FALSE if the footprint being edited has not changed
	BOOL m_footprint_name_changed;	// TRUE if the footprint being edited has had its name changed
	BOOL m_netlist_completed;		// netlist_completed		//
	//CString m_part_search;		// string for search footprint in DlgAddPart
	CString m_find_component;	// 
	CString m_window_title;		// window title for PCB editor
	CString m_fp_window_title;	// window title for footprint editor
	CString m_name;				// project name
	CString m_get_app_folder;	// 
	CString m_app_dir;			// application directory (full path) 
	CString m_lib_dir;			// path to default library folder (may be relative)   
	CString m_full_lib_dir;		// full path to default library folder
	CString m_parent_folder;	// path to parent of project folders (may be relative)
	CString m_path_to_folder;	// path to project folder
	CString m_pcb_filename;		// name of project file
	CString m_pcb_full_path;	// full path to project file
	CString m_cam_full_path;	// full path to CAM file folder
	CString m_cam_shortcut[NUM_SHORTCUTS];
	CString m_netlist_full_path;	// last netlist loaded
	CString CAM[NUM_CAM_FOLDERS];				//cam folders
	//   
	CPageList Pages;
	CArray<CPolyLine> * m_outline_poly;	// outline
	CDisplayList * m_dlist;		// display list
	dl_element * last_el;		//for static hilite with timer
	StrList * m_mlist;			// merge list
	StrList * m_ref_lists;		// ref lists
	SMFontUtil * m_smfontutil;	// Hershey font utility
	CAttrib * Attr;
	CTextList * m_tlist;		// text list 
	CFreePcbView * m_view;		// pointer to CFreePcbView 
	CDlgLog * m_dlg_log;
	CSize m_partlist_size;
	CSize m_partlist_pos;
	int m_partlist_col_w;
	DRErrorList * m_drelist;
	int m_file_close_ret;		// return value from OnFileClose() dialog

	// undo and redo stacks and state
	//BOOL m_bLastPopRedo;		// flag that last stack op was pop redo
	CUndoList * m_undo_list;	// undo stack
	CUndoList * m_redo_list;	// redo stack

	// pseudo-clipboard
	StrList * clip_mlist;
	CArray<CPolyLine> clip_outline_poly;
	CArray<CString> m_FootprintNames;
	CArray<CString> m_ProjectFiles;
	CLibPointer m_library;
	CLibPointer m_merge_library;
	CString m_footprint_extention;

	// grids and units for pcb editor
	int m_units;					// MM or MIL
	double m_visual_grid_spacing;	// grid spacing
	double m_part_grid_spacing;		// grid spacing
	double m_routing_grid_spacing;	// grid spacing
	int m_snap_angle;				// 0, 45 or 90
	CArray<double> m_visible_grid;	// array of choices for visible grid
	CArray<double> m_part_grid;		// array of choices for placement grid
	CArray<double> m_routing_grid;	// array of choices for routing grid
	            
	// layers  
	int m_num_layers;			// number of drawing layers (note: different than copper layers)
	int m_num_additional_layers;	// number of copper layers
	int m_rgb[MAX_LAYERS][3];	// array of RGB values for each drawing layer
	int m_vis[MAX_LAYERS];		// array of visible flags
	int m_system_colors;
	CString m_color_data;		// memory of system colors

	// pdf
	int m_pdf_rgb[MAX_LAYERS][3];	// array of RGB values for each pdf drawing layer
	int m_pdf_vis[MAX_LAYERS];		// array of visible flags
	int m_pdf_font_i;
	int m_pdf_checking;
	float m_pdf_margin;

	// bom
	int m_bom_rgb[7][3];
	int m_bom_options;	// 5bit - ref list index, 
						// 1bit - ignore without value 
						// 1bit - include list 
						// 1bit - exclude list 
						// 16bit - detail string len from 16th bit 

	// default 
	double m_letter_spacing;
	int m_magnetize_value;
	int m_seg_clearance;
	int m_polyline_w;		// default trace width
	int m_node_w;
	int m_default_font;			
	int m_via_hole_w;		// default via hole diameter
	CArray<CString> PartRenumber[MAX_PAGES];
	CString m_special_chars;

	// CAM flags and parameters
	int m_cam_flags;
	int m_cam_units;
	int m_min_polyline_width;
	int m_min_text_stroke_width;
	int m_highlight_wid;
	int m_min_text_height;
	int m_cam_layers;

	// report file options
	int m_report_flags;
	//
	int m_part_renumber_pos;
	int m_renumber_left_to_right;
	//
	// autosave times
	int m_auto_interval;	// interval (sec)
	int m_auto_elapsed;		// time since last save (sec)

	//DRC limits
	DesignRules m_dr;
	int m_clearance_checked[MAX_PAGES+2]; // (+2 virtual pages)
	int m_footprint_checked[MAX_PAGES+2]; // (+2 virtual pages)
	int m_netlist_created[MAX_PAGES+2];   // (+2 virtual pages)
	int m_parts_checked[MAX_PAGES+2];	  // (+2 virtual pages)

	// Netlist was uploaded to freepcb-2
	BOOL Check_Txt_Msg( CString * TXT=NULL, CString * MSG=NULL );

	// page index
	int m_current_page;

	// search index
	int m_parent_index;

	// alignment clearance
	float m_alignment;

// Generated message map functions
public:
	//{{AFX_MSG(CFreePcbDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnFileClose();
	afx_msg void OnViewLayers();
	void SaveOptions();
	afx_msg void OnAppExit();
	afx_msg void OnEditUndo();
	afx_msg void OnFileGenerateCadFiles();
	afx_msg void OnProjectOptions(); 
	afx_msg void OnProjectPartList();
	afx_msg void OnProjectGenBOM();
	afx_msg void OnProjectGenBOM2();
	afx_msg void OnProjectGenBOM3();
	afx_msg void OnProjectGenTag();
	afx_msg void OnProjectAdaptGrid();
	afx_msg void OnFileExport1();
	afx_msg void OnFileSetNetlist();

	void FileExport( CString str, int type );
	afx_msg void OnToolsDrc();
	afx_msg void OnToolsClearDrc();
	afx_msg void OnToolsClearAllDrc();
	afx_msg void OnToolsShowDRCErrorlist();
	afx_msg void OnViewLog();
	afx_msg void OnEditPasteFromFile();
	afx_msg void OnEditSelectAll();
	afx_msg void OnFilePrint();
	afx_msg void SelectAdj();
	afx_msg void OnSimilarPoly();
	afx_msg void OnReplacePoly();
	afx_msg BOOL OnCompare( UINT CMD );
	afx_msg BOOL OnOpenTemplate( UINT CMD );
	afx_msg void OnComponentNote();
	afx_msg void OnComponentURL();
	afx_msg void OnComponentImage();
	void ComponentImage( CString * fp );
	void ComponentNote( int MODE=0 );
	afx_msg void OnOpenComponentDatasheets();
	afx_msg void OnOpenComponentDatasheetsM();
	afx_msg void OnTXTNote();
	afx_msg void OnDOCNote();
	afx_msg void OnPDFNote();
	afx_msg void OnURLNote();
	afx_msg void OnGetPinMap();
	afx_msg void OnAddMarks();
	afx_msg void OnAddFavorite();
	afx_msg void OnProtection();
	afx_msg void OnCreateBranch();
	afx_msg void OnImportNetlist();
	afx_msg void OnSelectProjectFolder();
	afx_msg void OnReloadMenu();
	afx_msg void OnRunLibManager();
	afx_msg void OnRunFontManager();
	afx_msg void OnRunImpDxf();
	afx_msg void OnRunImpPcad();
	afx_msg BOOL OnRangeCmds( UINT CMD );
	afx_msg BOOL OnSpeedFile( UINT CMD );
	//
	void OnFileGerbV(CString G, CString app);
	afx_msg void OnFileCam1(){OnFileGerbV(CAM[0],m_cam_shortcut[0]);}
	afx_msg void OnFileCam2(){OnFileGerbV(CAM[1],m_cam_shortcut[0]);}
	afx_msg void OnFileCam3(){OnFileGerbV(CAM[2],m_cam_shortcut[0]);}
	afx_msg void OnFileCam4(){OnFileGerbV(CAM[3],m_cam_shortcut[0]);}
	afx_msg void OnFileCam5(){OnFileGerbV(CAM[4],m_cam_shortcut[0]);}
	afx_msg void OnFileCam6(){OnFileGerbV(CAM[5],m_cam_shortcut[0]);}
	afx_msg void OnFileCam7(){OnFileGerbV(CAM[6],m_cam_shortcut[0]);}
	afx_msg void OnFileCam8(){OnFileGerbV(CAM[0],m_cam_shortcut[1]);}
	afx_msg void OnFileCam9(){OnFileGerbV(CAM[1],m_cam_shortcut[1]);}
	afx_msg void OnFileCam10(){OnFileGerbV(CAM[2],m_cam_shortcut[1]);}
	afx_msg void OnFileCam11(){OnFileGerbV(CAM[3],m_cam_shortcut[1]);}
	afx_msg void OnFileCam12(){OnFileGerbV(CAM[4],m_cam_shortcut[1]);}
	afx_msg void OnFileCam13(){OnFileGerbV(CAM[5],m_cam_shortcut[1]);}
	afx_msg void OnFileCam14(){OnFileGerbV(CAM[6],m_cam_shortcut[1]);}
	afx_msg void OnFileCam15(){OnFileGerbV(CAM[0],m_cam_shortcut[2]);}
	afx_msg void OnFileCam16(){OnFileGerbV(CAM[1],m_cam_shortcut[2]);}
	afx_msg void OnFileCam17(){OnFileGerbV(CAM[2],m_cam_shortcut[2]);}
	afx_msg void OnFileCam18(){OnFileGerbV(CAM[3],m_cam_shortcut[2]);}
	afx_msg void OnFileCam19(){OnFileGerbV(CAM[4],m_cam_shortcut[2]);}
	afx_msg void OnFileCam20(){OnFileGerbV(CAM[5],m_cam_shortcut[2]);}
	afx_msg void OnFileCam21(){OnFileGerbV(CAM[6],m_cam_shortcut[2]);}
	//
	afx_msg void RenumberPartDesignations();
	afx_msg void ClearPartDesignations();
	afx_msg void RaisePartNumbers();
	void PartRenumbering( CString * prefix, CString * suffix, int start, BOOL bClear, BOOL LR, BOOL bGroups);
	afx_msg void TryToRemoveSuffix();
	//
	afx_msg void OnEditRedo();
	afx_msg void OnRepeatDrc();
	afx_msg void OnProjectCombineBoard();
	afx_msg void OnCreatePolyline();
	afx_msg void AddPage();
	afx_msg void SwitchToPage1();
	afx_msg void SwitchToPage2();
	afx_msg void SwitchToPage3();
	afx_msg void SwitchToPage4();
	afx_msg void SwitchToPage5();
	afx_msg void SwitchToPage6();
	afx_msg void SwitchToPage7();
	afx_msg void SwitchToPage8();
	afx_msg void RenamePage1();
	afx_msg void RenamePage2();
	afx_msg void RenamePage3();
	afx_msg void RenamePage4();
	afx_msg void RenamePage5();
	afx_msg void RenamePage6();
	afx_msg void RenamePage7();
	afx_msg void RenamePage8();
	afx_msg void DeletePage1();
	afx_msg void DeletePage2();
	afx_msg void DeletePage3();
	afx_msg void DeletePage4();
	afx_msg void DeletePage5();
	afx_msg void DeletePage6();
	afx_msg void DeletePage7();
	afx_msg void DeletePage8();
	afx_msg void OnAttrSync();
	afx_msg void OnDetachBMP();
	afx_msg void OnAttachBMP1();
	afx_msg void OnAttachBMP2();
	void AttributeSync();  
	void AttachBMP( int Style, CString * fn = NULL );
	afx_msg void AddRefList();
	int AddNewRefList( CString * Name );
	afx_msg void DelRefList();
	int RefList( BOOL clear );
	void OnReflistRemoveItem( int number );
	afx_msg void PlayRefList1();
	afx_msg void PlayRefList2();
	afx_msg void PlayRefList3();
	afx_msg void PlayRefList4();
	afx_msg void PlayRefList5();
	afx_msg void PlayRefList6();
	afx_msg void PlayRefList7();
	afx_msg void PlayRefList8();
	afx_msg void PlayRefList9();
	afx_msg void PlayRefList10();
	afx_msg void PlayRefList11();
	afx_msg void PlayRefList12();
	afx_msg void PlayRefList13();
	afx_msg void PlayRefList14();
	afx_msg void PlayRefList15();
	afx_msg void PlayRefList16();
	afx_msg void PlayRefList17();
	afx_msg void PlayRefList18();
	afx_msg void PlayRefList19();
	afx_msg void PlayRefList20();
	//
	afx_msg void SwitchToPCB();
	void SwitchToPCB( BOOL duty, BOOL bGRAB, BOOL bIronScale = 0 );
	void SwitchToCDS( CString * CdsFile=NULL, CString * PartRef=NULL, BOOL bMERGE=FALSE );
	void EnsureNotMinimized( HWND hWnd, BOOL bGRAB, BOOL bIronScale );
	void PlayRefList( int i );
	HWND OnSendMess( HWND hWnd, CString * mess );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FREEPCBDOC_H__A00395C2_2CF4_4902_9C7B_CBB16DB58836__INCLUDED_)
