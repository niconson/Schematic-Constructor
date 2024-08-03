// DlgMyMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "FreeSch.h"
#include "DlgMyMessageBox.h"


// CDlgMyMessageBox dialog

IMPLEMENT_DYNAMIC(CDlgMyMessageBox, CDialog)
CDlgMyMessageBox::CDlgMyMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMyMessageBox::IDD, pParent)
{
}

CDlgMyMessageBox::~CDlgMyMessageBox()
{
}



void CDlgMyMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FULL_PATH, m_message);
	DDX_Control(pDX, IDC_STATIC_MYMESSAGE, m_q );
	DDX_Control(pDX, IDC_BMP, m_picture);
	if( !pDX->m_bSaveAndValidate )
	{
		// incoming
		m_message.SetWindowText( m_mess );
		m_q.SetWindowText( G_LANGUAGE==0?"Do you want to continue working in FreePcb-2?":"’отите продолжить работу с печатной платой в ѕлат‘орм?");

		// show cursor
		::ShowCursor( TRUE );
		// picture
		if( m_bitmap_ptr )
		{
			m_bmp.DeleteObject();
			m_bmp.LoadBitmap( m_bitmap_ptr );
			m_picture.SetBitmap( m_bmp );	
			if( m_bitmap_ptr == IDB_BITMAP_INTERSECT )
			{
				//CWnd * W = AfxGetMainWnd();
				//if( W )
				{
					CRect R;
					GetWindowRect(R);
					//W->SetWindowPos( NULL, R.left, R.top, R.Width(), R.Height()-50, 0 );
					//SetWindowPos( W, R.left, R.top, R.Width(), R.Height()-4, 0 );
					MoveWindow(R.left, R.top, R.Width(), R.Height()-50 );
				}
			}
		}
	}
	else
	{
		// outgoing
		::ShowCursor( FALSE );
	}
}

BEGIN_MESSAGE_MAP(CDlgMyMessageBox, CDialog)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CDlgMyMessageBox::Initialize( LPCTSTR mess, long long bitmap_ptr  )
{
	m_mess = mess;
	m_bitmap_ptr = bitmap_ptr;
}

