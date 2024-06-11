// make Gerber file
#include "stdafx.h"

enum {
	GERBER_RENDER_ALL = 0,
	FL_VIEWMATE,
	FL_GERBV,
	FL_GERBERLOGIX,
	FL_LATER
};

class CAperture;

typedef CArray<CAperture,CAperture> aperture_array;

class CAperture
{
public:
	enum {
		AP_NONE = 0,
		AP_CIRCLE,
		AP_SQUARE,
		AP_RECT,
		AP_RRECT,
		AP_MOIRE,
		AP_THERMAL,
		AP_RECT_THERMAL,
		AP_RECT_THERMAL_45,
		AP_OCTAGON,
		AP_OVAL
	};
	int m_type;						// type of aperture, see enum above
	int m_size1, m_size2, m_size3, m_size4;	// in NM
	//	Width	 Height	  ThermW   ThermClearance
	//	Width	 Height	  Radius
	CAperture();
	CAperture( int type, int size1, int size2, int size3=0, int size4=0 );
	~CAperture();
	BOOL Equals( CAperture * ap );
	int FindInArray( aperture_array * ap_array, BOOL ok_to_add=FALSE );
};

int WriteGerberFile( CStdioFile * f, 
					int flags, 
					int layer, 
					CDlgLog * log, 
					int min_txt_stroke_w, 
					int highlight_width, 
					int polyline_wid, 
					int min_txt_stroke_h,
					CArray<CPolyLine> * op, 
					CAttrib * Attr, 
					CDisplayList * dl,
					StrList * ml ); 
