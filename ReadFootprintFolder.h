#pragma once
#include "stdafx.h"

void ReadFootprintFolder( CFreePcbDoc * doc, CString * FindFootprintName = NULL, CArray<CString> * Data = NULL );
void ReadFootprintNames( CFreePcbDoc * doc, CString * FPL, CString * FindFootprintName, CArray<CString> * mFootprintData );
void ClearFootprintNames( CFreePcbDoc * doc );
BOOL FindFootprint( CFreePcbDoc * doc, CString * foot );
BOOL UsedAlienLibrary( CFreePcbDoc * doc );
int  FindFootprintInFolder( CFreePcbDoc * doc, CString * fname, CString * old_fname, CString * CAT=NULL );