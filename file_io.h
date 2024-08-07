// file name : file_io.h
//
// header file for file_io.cpp
//
// file i/o routines for PROGRAM_NAME
//
#pragma once
#include "Documents.h"

int ParseStringFields( CString * str, CArray<CString> * field );
int ParseKeyString( CString * str, CString * key_str, CArray<CString> * param_str );
int my_atoi( CString * str );
double my_atof( CString * str );
