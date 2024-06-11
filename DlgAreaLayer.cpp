// DlgAreaLayer.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgAreaLayer.h"


// DlgAreaLayer dialog

IMPLEMENT_DYNAMIC(DlgAreaLayer, CDialog)
DlgAreaLayer::DlgAreaLayer(CWnd* pParent /*=NULL*/)
	: CDialog(DlgAreaLayer::IDD, pParent)
{
}

DlgAreaLayer::~DlgAreaLayer()
{
}

void DlgAreaLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAYER, m_list_layer);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		for( int il=0; il<m_num_layers; il++ )
		{
			m_list_layer.InsertString( il, &layer_str[il+LAY_ADD_1][0] );
		}
		m_list_layer.SetCurSel( m_layer - LAY_ADD_1 );
	}
	else
	{
		// outgoing
		m_layer = LAY_ADD_1 + m_list_layer.GetCurSel();
	}
}


BEGIN_MESSAGE_MAP(DlgAreaLayer, CDialog)
END_MESSAGE_MAP()

void DlgAreaLayer::Initialize( int num_layers )
{
	m_num_layers = num_layers - LAY_ADD_1;
}

// DlgAreaLayer message handlers
