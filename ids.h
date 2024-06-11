// definition of ID structure used by FreePCB
//
#pragma once

// struct id : this structure is used to identify PCB design elements
// such as instances of parts or nets, and their subelements
// Each element will have its own id.
// An id is attached to each item of the Display List so that it can
// be linked back to the PCB design element which drew it.
// These are mainly used to identify items selected by clicking the mouse
//
// In general: 
//		id.type	= type of PCB element (e.g. part, net, text)
//		id.st	= subelement type (e.g. part pad, net connection)
//		id.i	= subelement index (zero-based)
//		id.sst	= subelement of subelement (e.g. net connection segment)
//		id.ii	= subsubelement index (zero-based)
//
// For example, the id for segment 0 of connection 4 of net 12 would be
//	id = { ID_NET, 12, ID_CONNECT, 4, ID_SEG, 0 };
//

// these are constants used in ids
// root types   
enum {
	ID_NONE = 0,	// an undefined type or st (or an error)
	ID_TEXT,		// free-standing text
	ID_DRC,			// DRC error
	ID_POLYLINE,	// polylines
	ID_MULTI		// if multiple selections
};
//
class id {
public:
	// constructor
	enum{ def=-1 };
	id( int qt=ID_NONE, int qst=def, int qis=def, int qsst=def, int qiis=def ) 
	{ type=qt; st=qst; i=qis; sst=qsst; ii=qiis; } 
	// operators
	friend int operator ==(id id1, id id2)
	{ return (id1.type==id2.type 
			&& id1.st==id2.st 
			&& id1.sst==id2.sst 
			&& id1.i==id2.i 
			&& id1.ii==id2.ii ); 
	}
	friend int operator !=(id id1, id id2)
	{ return (id1.type!=id2.type 
			|| id1.st!=id2.st 
			|| id1.sst!=id2.sst 
			|| id1.i!=id2.i 
			|| id1.ii!=id2.ii ); 
	}
	// member functions
	void Clear() 
	{ type=ID_NONE; st=def; i=def; sst=def; ii=def; } 
	void Set( int qt, int qst=def, int qis=def, int qsst=def, int qiis=def ) 
	{ type=qt; st=qst; i=qis; sst=qsst; ii=qiis; } 
	// member variables
	int type;	// type of element
	int st;	// type of subelement
	int i;		// index of subelement
	int sst;	// type of subsubelement
	int ii;	// index of subsubelement
};


// subtypes
enum {

	// subtypes of ID_TEXT  
	ID_TXT = 1,				

	// subtypes of polylines
	ID_GRAPHIC,		// graphic lines
	ID_PART,		// part
	ID_PIN,			// part ref, value
	ID_NET,			// net
	// subtypes of ID_DRC
	// for subsubtypes, use types in DesignRules.h
	ID_DRE
};


// subsubtypes 
enum {
	// subsubtypes of ID_POLYLINE
	ID_SIDE = 1,
	ID_CORNER,  
	ID_HATCH,
	ID_STROKE			// stroke for text or outlines
};




