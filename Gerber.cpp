// make Gerber file from partlist, netlist, textlist, etc.
//
#include "stdafx.h"
#include "Gerber.h"
#include <math.h>
#include <afxcoll.h>
#include <afxtempl.h>

#define pi  3.14159265359
int THERMAL_W;
#define CLEARANCE_POLY_STROKE_MILS 1

// constructor
CAperture::CAperture()
{
	m_type = 0;
	m_size1 = 0;
	m_size2 = 0;
}

// constructor
CAperture::CAperture( int type, int size1, int size2, int size3, int size4 ) 
{
	m_type = type;
	m_size1 = size1;
	m_size2 = size2;
	m_size3 = size3;
	m_size4 = size4;
}

// destructor
CAperture::~CAperture()
{
}

// test for equality
//
BOOL CAperture::Equals( CAperture * ap )
{
	if( m_type == ap->m_type )
	{
		if( m_type == AP_CIRCLE && m_size1 == ap->m_size1 )
			return TRUE;
		else if( m_type == AP_SQUARE && m_size1 == ap->m_size1 )
			return TRUE;
		else if( m_type == AP_OCTAGON && m_size1 == ap->m_size1 )
			return TRUE;
		else if( m_type == AP_RECT && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 )
			return TRUE;
		else if( m_type == AP_RRECT && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 
				&& m_size3 == ap->m_size3 )
			return TRUE;
		else if( m_type == AP_RECT_THERMAL && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 
				&& m_size3 == ap->m_size3 && m_size4 == ap->m_size4 )
			return TRUE;
		else if( m_type == AP_RECT_THERMAL_45 && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 
				&& m_size3 == ap->m_size3 && m_size4 == ap->m_size4 )
			return TRUE;
		else if( m_type == AP_THERMAL && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 )
			return TRUE;
		else if( m_type == AP_MOIRE && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 )
			return TRUE;
		else if( m_type == AP_OVAL && m_size1 == ap->m_size1 && m_size2 == ap->m_size2 )
			return TRUE;
	}
	return FALSE;
}

// find aperture in array 
// if not found and ok_to_add == TRUE, add to array
// return posiiton in array, or -1 if not found and not added
//
int CAperture::FindInArray( aperture_array * ap_array, BOOL ok_to_add )
{
	int na = ap_array->GetSize();
	int ifound = -1;
	for( int ia=0; ia<na; ia++ )
	{
		if( this->Equals( &(ap_array->GetAt(ia)) ) )
		{
			ifound = ia;
			break;
		}
	}
	if( ifound != -1 )
		return ifound;
	if( ok_to_add )
	{
		ap_array->Add( *this );
		return ap_array->GetSize()-1;
	}
	ASSERT(0);
	return -1;
}

// things that you can do with a light
enum {
	LIGHT_NONE,
	LIGHT_ON,
	LIGHT_OFF,
	LIGHT_FLASH
};

// generate a "moveto" string for the Gerber file
// enter with:
//	x, y = coords in NM
//
void WriteMoveTo( CStdioFile * f, int x, int y, int light_state )
{
	_int64 x_10 = (_int64)10*x/NM_PER_MIL;	// 2.4
	_int64 y_10 = (_int64)10*y/NM_PER_MIL;	// 2.4
	CString str;
	if( light_state == LIGHT_NONE )
		ASSERT(0);
	else
		str.Format( "G01X%I64dY%I64dD0%d*\n", x_10, y_10, light_state );
	f->WriteString( str );
}

// draw one side of a CPolyLine by writing commands to Gerber file
// the side may be straight or an arc
// arc is aproximated by straight-line segments
// assumes that plotter already at x1, y1
// does not turn the light on or off
// dimensions are in NM
//
void WritePolygonSide( CStdioFile * f, int x1, int y1, int x2, int y2, int style, int nsteps, int light_state )
{
	int n;	// number of steps for arcs
	n = (abs(x2-x1)+abs(y2-y1))/(5*NM_PER_MIL);	// step size approx. 3 to 5 mil
	n = max( n, nsteps );	// or at most 5 degrees of arc
	CString line;
	double xo, yo, a, b, theta1, theta2;
	a = fabs( (double)(x1 - x2) );
	b = fabs( (double)(y1 - y2) );

	if( style == CPolyLine::STRAIGHT || x1 == x2 || y1 == y2 )
	{
		// just draw a straight line with linear interpolation
		WriteMoveTo( f, x2, y2, light_state );
		return;
	}
	else if( style == CPolyLine::ARC_CW )
	{
		// clockwise arc (ie.quadrant of ellipse)
		int i=0, j=0;
		if( x2 > x1 && y2 > y1 )
		{
			// first quadrant, draw second quadrant of ellipse
			xo = x2;	
			yo = y1;
			theta1 = pi;
			theta2 = pi/2.0;
		}
		else if( x2 < x1 && y2 > y1 )
		{
			// second quadrant, draw third quadrant of ellipse
			xo = x1;	
			yo = y2;
			theta1 = 3.0*pi/2.0;
			theta2 = pi;
		}
		else if( x2 < x1 && y2 < y1 )	
		{
			// third quadrant, draw fourth quadrant of ellipse
			xo = x2;	
			yo = y1;
			theta1 = 2.0*pi;
			theta2 = 3.0*pi/2.0;
		}
		else
		{
			xo = x1;	// fourth quadrant, draw first quadrant of ellipse
			yo = y2;
			theta1 = pi/2.0;
			theta2 = 0.0;
		}
	}
	else if( style == CPolyLine::ARC_CCW )
	{
		// counter-clockwise arc
		int i=0, j=0;
		if( x2 > x1 && y2 > y1 )
		{
			xo = x1;	// first quadrant, draw fourth quadrant of ellipse
			yo = y2;
			theta1 = 3.0*pi/2.0;
			theta2 = 2.0*pi;
		}
		else if( x2 < x1 && y2 > y1 )
		{
			xo = x2;	// second quadrant
			yo = y1;
			theta1 = 0.0;
			theta2 = pi/2.0;
		}
		else if( x2 < x1 && y2 < y1 )	
		{
			xo = x1;	// third quadrant
			yo = y2;
			theta1 = pi/2.0;
			theta2 = pi;
		}
		else
		{
			xo = x2;	// fourth quadrant
			yo = y1;
			theta1 = pi;
			theta2 = 3.0*pi/2.0;
		}
	}
	else
		ASSERT(0);
	// now write steps
	for( int is=1; is<=n; is++ )
	{
		double theta = theta1 + ((theta2-theta1)*(double)is)/n;
		double x = xo + a*cos(theta);
		double y = yo + b*sin(theta);
		if( is == n )
		{
			x = x2;
			y = y2;
		}
		WriteMoveTo( f, x, y, light_state );
	}
}



// Change aperture if necessary
// If PASS0, just make sure that the aperture is in ap_array
// Otherwise, write aperture change to file if necessary
//
void ChangeAperture( CAperture * new_ap,			// new aperture
					CAperture * current_ap,			// current aperture
					aperture_array * ap_array,		// array of apertures
					BOOL PASS0,						// flag for PASS0
					CStdioFile * f )				// file to write to
{
	int current_iap;
	CString line;
	new_ap->m_size1 = max(new_ap->m_size1, _2540);
	if( !(*current_ap).Equals( new_ap ) )
	{
		// change aperture
		current_iap = new_ap->FindInArray( ap_array, PASS0 );
		if( !PASS0 )
		{
			*current_ap = *new_ap;
			line.Format( "G54D%2d*\n", current_iap+10 );
			f->WriteString( line );	 // select new aperture
		}
	}
}

void WriteTiltPolygon( int x, int y, int t, int s1, int s2, int s3, int s4, int r, int ang, CStdioFile * f )
{
#define cnt_pts 20
static CPoint rnd[cnt_pts];
	if( t == CAperture::AP_RECT_THERMAL_45 )
	{
		if( s1/2 < (s3+s4)*0.707 || s2/2 < (s3+s4)*0.707 )
			t = CAperture::AP_RECT_THERMAL;
	}
	else if( t == CAperture::AP_THERMAL )
	{
		t = CAperture::AP_RECT_THERMAL;
		s4 = s1-s2;
		s2 = s1;
		s3 = THERMAL_W;
	}
	// correct radius
	if( t == CAperture::AP_SQUARE )
	{
		r = 0;
		s2 = s1;
	}
	else if( t == CAperture::AP_CIRCLE )
		ASSERT(0);
	else if( t == CAperture::AP_RECT )
		r = 0;
	else if( t == CAperture::AP_OVAL )
		r = min(s1,s2)/2;
	else if( t == CAperture::AP_RECT_THERMAL )
	{
		s3 = min(s3, s1-2*s4);
		s3 = min(s3, s2-2*s4);
	}
	else if( t == CAperture::AP_RECT_THERMAL_45 )
	{
		r = s4;
	}
	else if( t == CAperture::AP_OCTAGON )
	{
		double alpha = 22.5;
		s1 = s1*cos(alpha*pi/180);
		r = s1/2 - s1/2*sin(alpha*pi/180);
		s2 = s1;
	}
	CPoint porg;
		porg.x = x;
		porg.y = y;
	if( t == CAperture::AP_RECT_THERMAL_45 )
	{
		double k = ((double)s2-(double)s3)/((double)s1-(double)s3);
		f->WriteString( "G36*\n" );
		CPoint p[6];
		if( s1 > (s2+((float)s3*1.5)) )
		{
			p[0].x = x + s1/2;
			p[0].y = y + s2/2 - (float)s3*0.707;
			p[1].x = x + s1/2;
			p[1].y = y - s2/2 + (float)s3*0.707;
			p[2].x = x + s1/2 - s2/2 + (float)s3*0.707;
			p[2].y = y;
	
			RotatePOINTS(p,3,-ang,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			RotatePOINTS(p,3,180,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			p[0].x = x + s1/2 - (float)s3*0.707;
			p[0].y = y + s2/2;
			p[1].x = x - s1/2 + (float)s3*0.707;
			p[1].y = y + s2/2;
			p[2].x = x - s1/2 + s2/2 + (float)s3*0.707;
			p[2].y = y;
			p[3].x = x - s1/2 + (float)s3*0.707;
			p[3].y = y - s2/2;
			p[4].x = x + s1/2 - (float)s3*0.707;
			p[4].y = y - s2/2;
			p[5].x = x + s1/2 - s2/2 - (float)s3*0.707;
			p[5].y = y;

			RotatePOINTS(p,6,-ang,porg);
			WriteMoveTo( f, p[5].x, p[5].y, LIGHT_OFF );
			for(int g=0;g<6;g++)
				WriteMoveTo( f, p[g].x, p[g].y, LIGHT_ON );
		}
		else if( s2 > (s1+(float)s3*1.5) )
		{
			p[0].y = y + s2/2;
			p[0].x = x + s1/2 - (float)s3*0.707;
			p[1].y = y + s2/2;
			p[1].x = x - s1/2 + (float)s3*0.707;
			p[2].y = y + s2/2 - s1/2 + (float)s3*0.707;
			p[2].x = x;
	
			RotatePOINTS(p,3,-ang,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			RotatePOINTS(p,3,180,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			p[0].y = y + s2/2 - (float)s3*0.707;
			p[0].x = x + s1/2;
			p[1].y = y - s2/2 + (float)s3*0.707;
			p[1].x = x + s1/2;
			p[2].y = y - s2/2 + s1/2 + (float)s3*0.707;
			p[2].x = x;
			p[3].y = y - s2/2 + (float)s3*0.707;
			p[3].x = x - s1/2;
			p[4].y = y + s2/2 - (float)s3*0.707;
			p[4].x = x - s1/2;
			p[5].y = y + s2/2 - s1/2 - (float)s3*0.707;
			p[5].x = x;

			RotatePOINTS(p,6,-ang,porg);
			WriteMoveTo( f, p[5].x, p[5].y, LIGHT_OFF );
			for(int g=0;g<6;g++)
				WriteMoveTo( f, p[g].x, p[g].y, LIGHT_ON );
		}
		else
		{
			p[0].x = x + s1/2;
			p[0].y = y + s2/2 - (float)s3*0.707;
			p[1].x = x + s1/2;
			p[1].y = y - s2/2 + (float)s3*0.707;
			p[2].x = x + s1/2 - s2/2 + (float)s3*0.707;
			p[2].y = y;
		
			RotatePOINTS(p,3,-ang,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			RotatePOINTS(p,3,180,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			p[0].y = y + s2/2;
			p[0].x = x + s1/2 - (float)s3*0.707;
			p[1].y = y + s2/2;
			p[1].x = x - s1/2 + (float)s3*0.707;
			p[2].y = y + s2/2 - s1/2 + (float)s3*0.707;
			p[2].x = x;

			RotatePOINTS(p,3,-ang,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );

			RotatePOINTS(p,3,180,porg);
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_OFF );
			WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
			WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
			WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );
		}
		f->WriteString( "G37*\n" );
	}
	else
	{
		int npo;
		if( t == CAperture::AP_OCTAGON || r < NM_PER_MIL )
			npo = Gen_RndRectPoly( x,y,s1,s2,r,-ang, rnd, 8 );
		else
			npo = Gen_RndRectPoly( x,y,s1,s2,r,-ang, rnd, cnt_pts );
		if( npo )
		{
			if( t == CAperture::AP_SQUARE ||
				t == CAperture::AP_RECT ||
				t == CAperture::AP_RRECT ||
				t == CAperture::AP_OVAL ||
				t == CAperture::AP_OCTAGON )
			{
				f->WriteString( "G36*\n" );
				WriteMoveTo( f, rnd[0].x, rnd[0].y, LIGHT_OFF );
				for( int i=1; i<npo; i++ )
					WriteMoveTo( f, rnd[i].x, rnd[i].y, LIGHT_ON );
				f->WriteString( "G37*\n" );
			}
			else if( t == CAperture::AP_RECT_THERMAL )
			{
				if( npo%4 )
					ASSERT(0);
				//
				f->WriteString( "G36*\n" );
				WriteMoveTo( f, rnd[0].x, rnd[0].y, LIGHT_OFF );
				for( int i=1; i<npo/4; i++ )
					WriteMoveTo( f, rnd[i].x, rnd[i].y, LIGHT_ON );
	
				CPoint p[3];
				p[0].x = x+s3/2;
				p[0].y = y+s2/2;
				p[1].x = x+s3/2;
				p[1].y = y+s3/2;
				p[2].x = x+s1/2;
				p[2].y = y+s3/2;
				
				RotatePOINTS(p,3,-ang,porg);
				WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
				WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
				WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );
				
				f->WriteString( "G37*\n" );
				//
				f->WriteString( "G36*\n" );
				WriteMoveTo( f, rnd[npo/4].x, rnd[npo/4].y, LIGHT_OFF );
				for( int i=npo/4+1; i<npo/2; i++ )
					WriteMoveTo( f, rnd[i].x, rnd[i].y, LIGHT_ON );
	
				p[0].x = x-s1/2;
				p[0].y = y+s3/2;
				p[1].x = x-s3/2;
				p[1].y = y+s3/2;
				p[2].x = x-s3/2;
				p[2].y = y+s2/2;
				RotatePOINTS(p,3,-ang,porg);
				WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
				WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
				WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );
				
				f->WriteString( "G37*\n" );
				//
				f->WriteString( "G36*\n" );
				WriteMoveTo( f, rnd[npo/2].x, rnd[npo/2].y, LIGHT_OFF );
				for( int i=npo/2+1; i<3*npo/4; i++ )
					WriteMoveTo( f, rnd[i].x, rnd[i].y, LIGHT_ON );
				
				p[0].x = x-s3/2;
				p[0].y = y-s2/2;
				p[1].x = x-s3/2;
				p[1].y = y-s3/2;
				p[2].x = x-s1/2;
				p[2].y = y-s3/2;
				RotatePOINTS(p,3,-ang,porg);
				WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
				WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
				WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );
				
				f->WriteString( "G37*\n" );
				//
				f->WriteString( "G36*\n" );
				WriteMoveTo( f, rnd[3*npo/4].x, rnd[3*npo/4].y, LIGHT_OFF );
				for( int i=3*npo/4+1; i<npo; i++ )
					WriteMoveTo( f, rnd[i].x, rnd[i].y, LIGHT_ON );
	
				p[0].x = x+s1/2;
				p[0].y = y-s3/2;
				p[1].x = x+s3/2;
				p[1].y = y-s3/2;
				p[2].x = x+s3/2;
				p[2].y = y-s2/2;
				RotatePOINTS(p,3,-ang,porg);
				WriteMoveTo( f, p[0].x, p[0].y, LIGHT_ON );
				WriteMoveTo( f, p[1].x, p[1].y, LIGHT_ON );
				WriteMoveTo( f, p[2].x, p[2].y, LIGHT_ON );
				
				f->WriteString( "G37*\n" );
			}
		}
	}
}


// write the Gerber file for a layer
// assumes that the file is already open for writing
// returns 0 if successful
//
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
					StrList * ml )
{
int LPD = 0;
#define SET_LPD	if(LPD != 1) {f->WriteString( "%LPD*%\n" ); LPD = 1;} else {;}
#define SET_LPC	if(LPD != 2) {f->WriteString( "%LPC*%\n" ); LPD = 2;} else {;}
#define LAYER_TEXT_HEIGHT			100*NM_PER_MIL	// for layer ID sring
#define	LAYER_TEXT_STROKE_WIDTH		10*NM_PER_MIL
#define PASS0 (ipass==0)	
#define PASS1 (ipass==1)

	aperture_array ap_array;
	int current_iap = -1;
	CAperture current_ap;
	RECT op_rect;
	op_rect.left = op_rect.bottom = INT_MAX;
	op_rect.right = op_rect.top = INT_MIN;
	const double cos_oct = cos( pi/8.0 ); 
	CString str;

	// get boundaries of outline (in nm)
	if( int s = op->GetSize() )
		for( int ib=0; ib<s; ib++ ) 
		{
			id gid = (*op)[ib].GetId();
			RECT gr = (*op)[ib].GetBounds();
			SwellRect( &op_rect, gr );
		}
	if( op_rect.left == INT_MAX )
	{
		op_rect = rect( 0,0,0,0 );
	}
	
	// perform two passes through data, first just get apertures, then write file
	for( int ipass=0; ipass<2; ipass++ )
	{
		CString line;
		if( PASS1 )
		{	//=============================================================================
			// ******************** apertures created, now write them *********************
			//=============================================================================
			CString thermal_angle_str = "45.0";
			CString layer_name_str;
			if( layer == LAY_PASTE_TOP )
				layer_name_str = "top paste mask";
			else if( layer == LAY_PASTE_BOTTOM )
				layer_name_str = "bottom paste mask";
			else
				layer_name_str.Format( "%s", &layer_str[layer][0] );
			line.Format( "G04 %s layer *\n", layer_name_str );
			f->WriteString( line );
			f->WriteString( "G04 Scale: 100 percent, Rotated: No, Reflected: No *\n" );
			f->WriteString( "%FSLAX24Y24*%\n" );	// 2.4
			f->WriteString( "%MOIN*%\n" );
			f->WriteString( "%LN " + layer_name_str + " *%\n" );
			// macros
			f->WriteString( "G04 Rounded Rectangle Macro, params: W/2, H/2, R *\n" );
			f->WriteString( "%AMRNDREC*\n" );
			f->WriteString( "21,1,$1+$1,$2+$2-$3-$3,0,0,0*\n" );
			f->WriteString( "21,1,$1+$1-$3-$3,$2+$2,0,0,0*\n" );
			f->WriteString( "1,1,$3+$3,$1-$3,$2-$3*\n" );
			f->WriteString( "1,1,$3+$3,$3-$1,$2-$3*\n" );
			f->WriteString( "1,1,$3+$3,$1-$3,$3-$2*\n" );
			f->WriteString( "1,1,$3+$3,$3-$1,$3-$2*%\n" );
			f->WriteString( "G04 Rectangular Thermal Macro, params: W/4, H/4, TW/4, TC/4 *\n" );
			f->WriteString( "%AMRECTHERM*\n" );  
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4,$2+$2-$3-$3-$4-$4-$4-$4,0-$1-$3-$4,0-$2-$3,0*\n" ); 
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4-$4-$4,$2+$2-$3-$3-$4-$4,0-$1-$3,0-$2-$3-$4,0*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,0-$2-$2+$4+$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,0-$3-$3-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$3-$3-$4-$4,0-$2-$2+$4+$4*\n" );
			//
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4,$2+$2-$3-$3-$4-$4-$4-$4,0-$1-$3-$4,$2+$3,0*\n" ); 
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4-$4-$4,$2+$2-$3-$3-$4-$4,0-$1-$3,$2+$3+$4,0*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,$2+$2-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,$3+$3+$4+$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$3-$3-$4-$4,$2+$2-$4-$4*\n" );
			//
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4,$2+$2-$3-$3-$4-$4-$4-$4,$1+$3+$4,0-$2-$3,0*\n" ); 
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4-$4-$4,$2+$2-$3-$3-$4-$4,$1+$3,0-$2-$3-$4,0*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,0-$2-$2+$4+$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,0-$3-$3-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$3+$3+$4+$4,0-$2-$2+$4+$4*\n" );
			//
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4,$2+$2-$3-$3-$4-$4-$4-$4,$1+$3+$4,$2+$3,0*\n" ); 
			f->WriteString( "21,1,$1+$1-$3-$3-$4-$4-$4-$4,$2+$2-$3-$3-$4-$4,$1+$3,$2+$3+$4,0*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,$2+$2-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,$3+$3+$4+$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$3+$3+$4+$4,$2+$2-$4-$4*%\n" );
			//
			f->WriteString( "%AMRECTHERM_45*\n" );  
			/*f->WriteString( "21,1,$4+$4+$4+$4,$2+$2+$2+$2-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,$1+$1-$4-$4,0,0*\n" ); 
			f->WriteString( "21,1,$4+$4+$4+$4,$2+$2+$2+$2-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,0-$1-$1+$4+$4,0,0*\n" );
			f->WriteString( "21,1,$1+$1+$1+$1-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,$4+$4+$4+$4,0,$2+$2-$4-$4,0*\n" ); 
			f->WriteString( "21,1,$1+$1+$1+$1-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,$4+$4+$4+$4,0,0-$2-$2+$4+$4,0*\n" );*/
			f->WriteString( "21,1,$1+$1+$1+$1,$2+$2+$2+$2-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,0,0,0*\n" );
			f->WriteString( "21,1,$1+$1+$1+$1-$4-$4-$4-$4-$4-$4-$4-$4-$4-$4-$3-$3,$2+$2+$2+$2,0,0,0*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,$2+$2-$4-$4-$4-$4-$4-$3*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4,0-$2-$2+$4+$4+$4+$4+$4+$3*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4-$4-$4-$4-$3,$2+$2-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4+$4+$4+$4+$3,$2+$2-$4-$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,$2+$2-$4-$4-$4-$4-$4-$3*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4,0-$2-$2+$4+$4+$4+$4+$4+$3*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,$1+$1-$4-$4-$4-$4-$4-$3,0-$2-$2+$4+$4*\n" );
			f->WriteString( "1,1,$4+$4+$4+$4,0-$1-$1+$4+$4+$4+$4+$4+$3,0-$2-$2+$4+$4*%\n" );
			//
			// define all of the apertures
			for( int ia=0; ia<ap_array.GetSize(); ia++ )
			{
				if( ap_array[ia].m_type == CAperture::AP_CIRCLE )
				{
					line.Format( "ADD%dC,%.6f*", ia+10, (double)ap_array[ia].m_size1/25400000.0 );
					f->WriteString( "%" + line + "%\n" );
				}
				else if( ap_array[ia].m_type == CAperture::AP_SQUARE )
				{
					line.Format( "ADD%dR,%.6fX%.6f*", ia+10, 
						(double)ap_array[ia].m_size1/25400000.0, (double)ap_array[ia].m_size1/25400000.0 );
					f->WriteString( "%" + line + "%\n" );
				}
				else if( ap_array[ia].m_type == CAperture::AP_OCTAGON )
				{
					line.Format( "ADD%dP,%.6fX8X22.5*", ia+10, 
						((double)ap_array[ia].m_size1/25400000.0 )/cos_oct );
					f->WriteString( "%" + line + "%\n" );
				}
				else if( ap_array[ia].m_type == CAperture::AP_OVAL ) 
				{
					line.Format( "ADD%dO,%.6fX%.6f*", ia+10, 
						(double)ap_array[ia].m_size1/25400000.0, (double)ap_array[ia].m_size2/25400000.0 );
					f->WriteString( "%" + line + "%\n" );
				}
				else if( ap_array[ia].m_type == CAperture::AP_RECT ) 
				{
					line.Format( "ADD%dR,%.6fX%.6f*", ia+10, 
						(double)ap_array[ia].m_size1/25400000.0, (double)ap_array[ia].m_size2/25400000.0 );
					f->WriteString( "%" + line + "%\n" );
				}
				else if( ap_array[ia].m_type == CAperture::AP_RRECT ) 
				{
					line.Format( "ADD%dRNDREC,%.6fX%.6fX%.6f*", ia+10, 
						(double)ap_array[ia].m_size1/(2*25400000.0), (double)ap_array[ia].m_size2/(2*25400000.0),
						(double)ap_array[ia].m_size3/25400000.0 );
					f->WriteString( "%" + line + "%\n" ); 
				}
				else
					ASSERT(0);
			}
			f->WriteString( "G90*\n" );			// absolute format
			f->WriteString( "G70D02*\n" );		// use inches
			f->WriteString( "G04 begin color: clBlack*\n" );
			SET_LPD								// start color clBlack
		}

		// draw outline poly
		if( PASS1 )
		{
			f->WriteString( "\nG04 ----------------------- Draw outline (positive)*\n" );
			SET_LPD
			current_ap.m_type = CAperture::AP_NONE;	// force selection of aperture
		}
		// if hilite layer
		// scan all elements and continues
		if  ( layer == LAY_HILITE )
		{
			dl_element * el = dl->Get_Start(LAY_HILITE);
			while( el->next->next )
			{
				el = el->next;
				if ( !el->visible )
					continue;
				BOOL fl = FALSE;
				int n_l = 16;//nl->GetNumCopperLayers();
				for( int sc=LAY_FREE_LINE; sc<LAY_ADD_1+n_l; sc++ )
					if( el->layer == sc && dl->m_vis[sc] )
					{
						fl = TRUE;
						break;
					}
				if( !fl )
					continue;
				RECT Get;
				dl->Get_Rect( el, &Get );
				int xi = (Get.right + Get.left)/2;
				int yi = (Get.top + Get.bottom)/2;
				int w = max(highlight_width,abs(el->el_w)*m_pcbu_per_wu);
				if( el->gtype == DL_CIRC || el->gtype == DL_HOLLOW_CIRC )
				{
					if( el->gtype == DL_CIRC )
						w = (Get.right - Get.left);
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
						::WriteMoveTo( f, xi, yi, LIGHT_FLASH );
					if( el->gtype == DL_HOLLOW_CIRC && w > highlight_width*2 )
					{
						CAperture ap( CAperture::AP_CIRCLE, w-(highlight_width*2), 0 );
						ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
						if PASS1
						{
							SET_LPC
							::WriteMoveTo( f, xi, yi, LIGHT_FLASH );
							SET_LPD
						}
					}
				}
				else if( el->gtype == DL_RECT )
				{
					CAperture ap( CAperture::AP_RECT, Get.right-Get.left, Get.top-Get.bottom );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
						::WriteMoveTo( f, xi, yi, LIGHT_FLASH );
				}
				else if( el->gtype == DL_RRECT )
				{
					CAperture ap( CAperture::AP_RRECT, Get.right-Get.left, Get.top-Get.bottom );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
						::WriteMoveTo( f, xi, yi, LIGHT_FLASH );
				}
				else if( el->gtype == DL_HOLLOW_RECT  )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						::WriteMoveTo( f, Get.left,  Get.bottom, LIGHT_OFF );
						::WriteMoveTo( f, Get.left,  Get.top, LIGHT_ON );
						::WriteMoveTo( f, Get.right, Get.top, LIGHT_ON );
						::WriteMoveTo( f, Get.right, Get.bottom, LIGHT_ON );
						::WriteMoveTo( f, Get.left,  Get.bottom, LIGHT_ON );
					}
				}
				if( el->gtype == DL_RECT_X )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CPoint P[4];
						int np = 4;
						dl->Get_Points( el, P, &np ); 
						if( np == 4 )
						{
							::WriteMoveTo( f, P[0].x, P[0].y, LIGHT_OFF );
							for( int ii=1; ii<np; ii++ )
								::WriteMoveTo( f, P[ii].x, P[ii].y, LIGHT_ON );
							::WriteMoveTo( f, P[0].x, P[0].y, LIGHT_ON );
							::WriteMoveTo( f, P[2].x, P[2].y, LIGHT_ON );
							::WriteMoveTo( f, P[1].x, P[1].y, LIGHT_OFF );
							::WriteMoveTo( f, P[3].x, P[3].y, LIGHT_ON );
						}
					}
				}
				if( el->gtype == DL_LINES_ARRAY )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CArray<CPoint> * PPTS;
						PPTS = dl->Get_Points( el, NULL, 0 ); 
						int np = PPTS->GetSize();
						CPoint * PTS = new CPoint[np];//new007
						dl->Get_Points( el, PTS, &np ); 
						if( np )
						{
							for( int ii=0; ii<np-1; ii+=2 )
							{
								::WriteMoveTo( f, PTS[ii].x, PTS[ii].y, LIGHT_OFF );
								::WriteMoveTo( f, PTS[ii+1].x, PTS[ii+1].y, LIGHT_ON );
							}
						}
						delete PTS;//new007
					}
				}
				if( el->gtype == DL_POLYGON || el->gtype == DL_POLYLINE )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CArray<CPoint> * PPTS;
						PPTS = dl->Get_Points( el, NULL, 0 ); 
						int np = PPTS->GetSize();
						CPoint * PTS = new CPoint[np];//new008
						dl->Get_Points( el, PTS, &np ); 
						if( np )
						{
							::WriteMoveTo( f, PTS[0].x, PTS[0].y, LIGHT_OFF );
							for( int ii=1; ii<np; ii++ )
								::WriteMoveTo( f, PTS[ii].x, PTS[ii].y, LIGHT_ON );
							::WriteMoveTo( f, PTS[0].x, PTS[0].y, LIGHT_ON );
							if( el->gtype == DL_POLYGON )
							{
								f->WriteString( "G36*\n" );
								::WriteMoveTo( f, PTS[0].x, PTS[0].y, LIGHT_OFF );
								for( int ii=1; ii<np; ii++ )
									::WriteMoveTo( f, PTS[ii].x, PTS[ii].y, LIGHT_ON );
								::WriteMoveTo( f, PTS[0].x, PTS[0].y, LIGHT_ON );
								f->WriteString( "G37*\n" );
							}
						}
						delete PTS;//new008
					}
				}
				else if( el->gtype == DL_LINE )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CPoint P[2];
						int np = 2;
						dl->Get_Points( el, P, &np ); 
						if( np == 2 )
						{
							::WriteMoveTo( f, P[0].x, P[0].y, LIGHT_OFF );
							::WriteMoveTo( f, P[1].x, P[1].y, LIGHT_ON );
						}
					}
				}
				else if( el->gtype == DL_HOLLOW_LINE )
				{
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CArray<CPoint> * PTS;
						PTS = dl->Get_Points( el, NULL, 0 ); 
						int np = PTS->GetSize();
						CPoint * P = new CPoint[np];//new009
						dl->Get_Points( el, P, &np );
						if( np > 4 )
						{
							::WriteMoveTo( f, P[2].x, P[2].y, LIGHT_OFF );
							for( int ii=3; ii<np; ii++ )
								::WriteMoveTo( f, P[ii].x, P[ii].y, LIGHT_ON );
							::WriteMoveTo( f, P[2].x, P[2].y, LIGHT_ON );
						}
						delete P;//new009
					}
				}
				else if( el->gtype == DL_ARC_CW || el->gtype == DL_ARC_CCW )
				{
					int tp;
					if (el->gtype == DL_ARC_CW)
						tp = CPolyLine::ARC_CW;
					else
						tp = CPolyLine::ARC_CCW;
					CAperture ap( CAperture::AP_CIRCLE, w, 0 );
					ChangeAperture( &ap, &current_ap, &ap_array, PASS0, f );
					if PASS1
					{
						CArray<CPoint> * PTS;
						PTS = dl->Get_Points( el, NULL, 0 ); 
						int np = PTS->GetSize();
						CPoint * P = new CPoint[np];//new010
						dl->Get_Points( el, P, &np );
						if( np >= 2 )
						{
							::WriteMoveTo( f, P[0].x, P[0].y, LIGHT_OFF );
							::WritePolygonSide( f, P[0].x, P[0].y, P[1].x, P[1].y, tp, N_SIDES_APPROX_ARC, LIGHT_ON );
						}
						delete P;//new010
					}
				}
			}
			// end of file
			if( PASS0 )
				continue;
			f->WriteString( "M00*\n" );
			return 0;
		}
		
		// draw text
		if( Attr )
		{
			if( PASS1 )
			{
				f->WriteString( "\nG04 Draw Text*\n" );
			}
			for( int lay=LAY_HILITE; lay<LAY_ADD_1; lay++ )
			{
				CTextList * tl = NULL;
				if( lay == LAY_PART_NAME )
					tl = Attr->m_Reflist;
				else if( lay == LAY_PIN_NAME ) 
					tl = Attr->m_Pinlist;
				else if( lay == LAY_PIN_DESC || lay >= LAY_ADD_1 )
					tl = Attr->m_pDesclist;
				else if( lay == LAY_NET_NAME ) 
					tl = Attr->m_Netlist;
				else if( lay == LAY_FOOTPRINT )
					tl = Attr->m_Footlist;
				else if( lay == LAY_PART_VALUE ) 
					tl = Attr->m_Valuelist;
				if( tl )
				{
					if( layer == lay || (getbit(flags, GERBER_RENDER_ALL) && getbit(flags, lay)) )
					{
						int it=-1;
						for( CText * t = tl->GetNextText(&it); t; t = tl->GetNextText(&it)  )
						{
							if( t->m_font_size > min_txt_stroke_h )
							{
								// draw text
								int w = t->m_stroke_width;
								w = max( t->m_stroke_width, min_txt_stroke_w );
								CAperture text_ap( CAperture::AP_CIRCLE, w, 0 );
								ChangeAperture( &text_ap, &current_ap, &ap_array, PASS0, f );
								if( PASS1 )
								{
									if( t->dl_el )
									{
										CArray<CPoint> * tpt = dl->Get_Points( t->dl_el, NULL, 0 );
										int np = tpt->GetSize();
										CPoint * m_stroke = new CPoint[np];//new011
										dl->Get_Points( t->dl_el, m_stroke, &np );
										for( int istroke=0; istroke+1<np; istroke+=2 )
										{
											::WriteMoveTo( f, m_stroke[istroke].x, m_stroke[istroke].y, LIGHT_OFF );
											::WriteMoveTo( f, m_stroke[istroke+1].x, m_stroke[istroke+1].y, LIGHT_ON );
										}
										delete m_stroke;//new011
									}
								}
							}
						}
					}
				}
			}
		}

		// draw outlines
		if( op )
		{
			SET_LPD
			for( int i=0; i<op->GetSize(); i++ )
			{
				CPolyLine * poly = &op->GetAt(i);
				if( poly->m_visible == 0 )
					continue;
				int pL = poly->GetLayer();
				if ( layer == pL || (getbit(flags, GERBER_RENDER_ALL) && getbit(flags, pL)) ) 
				{
					int pW =  abs(poly->GetW());
					pW = max( pW, polyline_wid );
					int pCl = poly->GetClosed();
					int Htch = poly->GetHatch();
					if( poly->Node[0] > 1 )
					{
						CAperture node_ap( CAperture::AP_CIRCLE, poly->Node[0], 0 );
						ChangeAperture( &node_ap, &current_ap, &ap_array, PASS0, f );
						if( PASS1 )
							::WriteMoveTo( f, poly->GetX(0), poly->GetY(0), LIGHT_FLASH );
					}
					if( poly->Node[poly->GetNumCorners()-1] > 1 )
					{
						CAperture node_ap( CAperture::AP_CIRCLE, poly->Node[poly->GetNumCorners()-1], 0 );
						ChangeAperture( &node_ap, &current_ap, &ap_array, PASS0, f );
						if( PASS1 )
							::WriteMoveTo( f, poly->GetX(poly->GetNumCorners()-1), poly->GetY(poly->GetNumCorners()-1), LIGHT_FLASH );
					}
					// stroke outline with aperture to create clearance
					if( Htch >= CPolyLine::HALF_TONE )
					{
						CAperture h_ap( CAperture::AP_CIRCLE, NM_PER_MIL, 0 );
						ChangeAperture( &h_ap, &current_ap, &ap_array, PASS0, f );
					}
					else
					{
						CAperture sm_ap( CAperture::AP_CIRCLE, pW, 0 );
						ChangeAperture( &sm_ap, &current_ap, &ap_array, PASS0, f );
					}
					if( PASS1 )
					{
						int nc = poly->GetNumCorners();
						if( pCl == 0 )
							nc--;
						for( int ico=0; ico<poly->GetNumContours(); ico++ )
						{
							int cst = poly->GetContourStart(ico);
							int cend = poly->GetContourEnd(ico);
							cend = min(cend,nc-1);
							int last_x = poly->GetX(cst);
							int last_y = poly->GetY(cst);
							::WriteMoveTo( f, last_x, last_y, LIGHT_OFF );
							for( int ic=cst; ic<=cend; ic++ )
							{
								last_x = poly->GetX(ic);
								last_y = poly->GetY(ic);
								int in = poly->GetNextCornerIndex(ic);
								int x = poly->GetX(in);
								int y = poly->GetY(in);
								int style = poly->GetSideStyle(ic);
								::WritePolygonSide( f, last_x, last_y, x, y, style, N_SIDES_APPROX_ARC, LIGHT_ON );	
							}
						}
						if( pCl && Htch )
						{
							CArray <dl_element*> * GetH = poly->GetHatchPos();
							int nh = GetH->GetSize();
							for( int ic=0; ic<nh; ic++ )
							{
								CArray<CPoint> * pA = dl->Get_Points( GetH->GetAt(ic), NULL, 0 );
								int np = pA->GetSize();
								CPoint * P = new CPoint[np];//new012
								dl->Get_Points( GetH->GetAt(ic), P, &np );
								for( int ip=0; ip+1<np; ip+=2 )
								{
									if( P[ip].x-P[ip+1].x )
									{
										double sqrt2 = 0.70710678;
										double k = fabs((double)P[ip].y-(double)P[ip+1].y)/fabs((double)P[ip].x-(double)P[ip+1].x);
										double ky = (double)pW*sqrt2*k;
										double iy = P[ip].y;
										int invX = 1;
										int invY = 1;
										if( P[ip].x > P[ip+1].x )
											invX = -1;
										if( P[ip].y > P[ip+1].y )
											invY = -1;
										for( double ix=P[ip].x; ix*invX<(double)P[ip+1].x*invX; ix+=(double)pW*sqrt2*(double)invX )
										{
											::WriteMoveTo( f, (int)ix, (int)iy, LIGHT_FLASH );
											iy += ky*(double)invY;
										}
									}
									else
										ASSERT(0);
								}
								delete P;//new012
							}
						}
					}
				}
			}
		}

		// end of file
		if( PASS1 )
			f->WriteString( "M00*\n" );

	}	// end of pass
	return 0;
}

// find value in CArray<int> and return position in array
// if not found, add to array if add_ok = TRUE, otherwise return -1
//
int AddToArray( int value, CArray<int,int> * array )
{
	for( int i=0; i<array->GetSize(); i++ )
		if( value == array->GetAt(i) )
			return i;
	array->Add( value );
	return array->GetSize()-1;
}

