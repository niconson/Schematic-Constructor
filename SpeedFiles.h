#pragma once
#include "stdafx.h"

void ClearSpeedFiles( CFreePcbDoc * doc );
void LoadSpeedFiles( CFreePcbDoc * doc );
void OnGroupGridMagnetize( CFreePcbDoc * doc );
void OnGroupVtxMagnetize( CFreePcbDoc * doc );
void AddGraphicSize( CFreePcbDoc* doc );
int GetStandartNode(CFreePcbDoc* doc, int ipoly);
int OnPolylineUpdatePcbView(CFreePcbDoc* doc, int m_sel_i, CString * old_board, BOOL bCheckMergeOnly = 0);