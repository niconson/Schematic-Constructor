// DlgSetAreaHatch.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgSetAreaHatch.h"


// CDlgSetAreaHatch dialog

IMPLEMENT_DYNAMIC(CDlgSetAreaHatch, CDialog)
CDlgSetAreaHatch::CDlgSetAreaHatch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetAreaHatch::IDD, pParent)
{
}

CDlgSetAreaHatch::~CDlgSetAreaHatch()
{
}

void CDlgSetAreaHatch::DoDataExchange(CDataExchange* pDX)
{

}


BEGIN_MESSAGE_MAP(CDlgSetAreaHatch, CDialog)
END_MESSAGE_MAP()


// CDlgSetAreaHatch message handlers

void CDlgSetAreaHatch::Init( int hatch )
{
	m_hatch = hatch;
}