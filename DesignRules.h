//
#pragma once
#include "PolyLine.h"

struct DesignRules
{
	BOOL bCheckUnrouted;
	int part_attr_to_attr; 
	int part_attr_to_poly; 
	int pin_attr_to_attr;
	int pin_attr_to_poly; 
};

class DRError
{
public:
	DRError();
	~DRError();
	int layer;				// layer (if pad error)
	id m_id;				// id, using subtypes above
	CString str;			// descriptive string
	id id1, id2;			// ids of items tested
	int x, y;				// position of error
	int selected;
	dl_element * dl_el;		// DRC graphic
	dl_element * dl_sel;	// DRC graphic selector
	CText * text1; // Textlist for texts
	CText * text2; // Textlist for texts
};

class DRErrorList
{
public:
	DRErrorList();
	~DRErrorList();
	void SetLists( CDisplayList * dl, CArray<CPolyLine>* poly );
	void Clear();
	DRError * Add(	CString * str, 
					id id1, id id2,
					int x1, int y1, 
					int x2, int y2, 
					int w, int layer, CText * t1 = NULL, CText * t2 = NULL );
	void Remove( DRError * dre );
	void HighLight( DRError * dre );
	void MakeSolidCircles();
	void MakeHollowCircles();
	void MoveOrigin( int dx, int dy );

public:
	CDisplayList * m_dlist;
	CArray<CPolyLine> * m_poly;
	CPtrList list;
};