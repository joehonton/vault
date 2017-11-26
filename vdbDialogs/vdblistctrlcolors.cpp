//=============================================================================
//
// CLASS:           vdbListCtrlColors
//
// ESSENCE:         Set pphListCtrl background and highlist colors
//
// FILE NAME:       vdbListCtrlColors.cpp
//
// INITIAL DATE:    1/6/00
//
// COPYRIGHT:       Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "vdbWindows.h"
#include "vdbListCtrlColors.h"
#include <vdbRegistrationKey.h>
#include "VaultResourceInstance.h"


//=============================================================================
// Construction and initialization
//=============================================================================

//-------------------------------------
vdbListCtrlColors::vdbListCtrlColors( const char* szRegistrationKey, CWnd* pParent )
	: vdbDLLDialog(vdbListCtrlColors::IDD, pParent),
	  _sRegistrationKey( szRegistrationKey )
{
	_sColumn1Name = "Name";
	_sColumn2Name = "Description";

	//{{AFX_DATA_INIT(vdbListCtrlColors)
	//}}AFX_DATA_INIT
}


//-------------------------------------
void vdbListCtrlColors::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbListCtrlColors)
	DDX_Control(pDX, IDC_VDB_SAMPLE_LIST, m_ctlList);
	DDX_Control(pDX, IDC_VDB_HIGHLIGHT, m_ctlHighlight);
	DDX_Control(pDX, IDC_VDB_BACKGROUND, m_ctlBackground);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbListCtrlColors, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbListCtrlColors)
	ON_CBN_SELCHANGE(IDC_VDB_BACKGROUND, OnSelchangeBackground)
	ON_CBN_SELCHANGE(IDC_VDB_HIGHLIGHT, OnSelchangeHighlight)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//-------------------------------------
int vdbListCtrlColors::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (vdbDLLDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ctlList.Subclass( IDC_VDB_SAMPLE_LIST, this );
	
	return 0;
}


//-------------------------------------
BOOL vdbListCtrlColors::OnInitDialog() 
{
	vdbDLLDialog::OnInitDialog();

	m_ctlList.PostCreate();
	m_ctlList.InsertColumn( 0, "0", LVCFMT_LEFT, 0, 0 );
	m_ctlList.InsertColumn( 1, _sColumn1Name, LVCFMT_LEFT, 100, 1 );
	m_ctlList.InsertColumn( 2, _sColumn2Name, LVCFMT_LEFT, 100, 2 );

	if ( _column1Samples.Count() < 6 || _column2Samples.Count() < 6 )
	{
		// if no sample data was supplied, fill first few rows with something
		for ( int i = 0; i < 6; i++ )
		{
			m_ctlList.InsertItem( i, "" );
			m_ctlList.SetItemText( i, 1, "Name" );
			m_ctlList.SetItemText( i, 2, "Description" );
		}
	}
	else
	{
		// show sample data in the list
		for ( int i = 0; i < _column1Samples.Count() && i < _column2Samples.Count(); i++ )
		{
			m_ctlList.InsertItem( i, "" );
			m_ctlList.SetItemText( i, 1, _column1Samples[i] );
			m_ctlList.SetItemText( i, 2, _column2Samples[i] );
		}
	}

	int index;
	index = m_ctlBackground.AddString( "White" );
	m_ctlBackground.SetItemData( index, RGB(255,255,255) );
	index = m_ctlBackground.AddString( "Light Gray" );
	m_ctlBackground.SetItemData( index, RGB(224,224,224) );
	index = m_ctlBackground.AddString( "Dark gray" );
	m_ctlBackground.SetItemData( index, RGB(192,192,192) );
	index = m_ctlBackground.AddString( "Yellow" );
	m_ctlBackground.SetItemData( index, RGB(255,255,224) );
	index = m_ctlBackground.AddString( "Green" );
	m_ctlBackground.SetItemData( index, RGB(224,255,224) );
	index = m_ctlBackground.AddString( "Blue" );
	m_ctlBackground.SetItemData( index, RGB(224,224,255) );
	
	index = m_ctlHighlight.AddString( "White" );
	m_ctlHighlight.SetItemData( index, RGB(255,255,255) );
	index = m_ctlHighlight.AddString( "Light Gray" );
	m_ctlHighlight.SetItemData( index, RGB(224,224,224) );
	index = m_ctlHighlight.AddString( "Dark gray" );
	m_ctlHighlight.SetItemData( index, RGB(192,192,192) );
	index = m_ctlHighlight.AddString( "Yellow" );
	m_ctlHighlight.SetItemData( index, RGB(255,255,224) );
	index = m_ctlHighlight.AddString( "Green" );
	m_ctlHighlight.SetItemData( index, RGB(224,255,224) );
	index = m_ctlHighlight.AddString( "Blue" );
	m_ctlHighlight.SetItemData( index, RGB(224,224,255) );

	COLORREF color;
	vdbRegistrationKey key( HKEY_CURRENT_USER, _sRegistrationKey, "Highlight" );
	key.GetValue( color );
	
	int count = m_ctlHighlight.GetCount();
	for ( int i = 0; i < count; i++ )
	{
		if ( m_ctlHighlight.GetItemData( i ) == color )
		{
			m_ctlHighlight.SetCurSel( i );
			m_ctlList.SetHighlightColor( color );
			break;
		}
	}

	key.SetLeaf( "Background" );
	key.GetValue( color );
	
	count = m_ctlBackground.GetCount();
	for ( int i = 0; i < count; i++ )
	{
		if ( m_ctlBackground.GetItemData( i ) == color )
		{
			m_ctlBackground.SetCurSel( i );
			m_ctlList.SetBackgroundColor( color );
			break;
		}
	}

	return TRUE;
}


//=============================================================================
// Callbacks
//=============================================================================

//-------------------------------------
void vdbListCtrlColors::OnOK() 
{
	// save to SOFTWARE\\CrescentBloom\\CompleatBotanica\\1.0
	vdbRegistrationKey key1( HKEY_CURRENT_USER, _sRegistrationKey, "" );
	
	// save to SOFTWARE\\ProgrammersHeaven\\Vault\\1.0 for use by the Report Builder	
	vdbRegistrationKey key2( HKEY_CURRENT_USER, "SOFTWARE", "" );
	key2.CreateBranch( "ProgrammersHeaven" );
	key2.SetSubkey( "SOFTWARE\\ProgrammersHeaven" );
	key2.CreateBranch( "Vault" );
	key2.SetSubkey( "SOFTWARE\\ProgrammersHeaven\\Vault" );
	key2.CreateBranch( "1.0" );
	key2.SetSubkey( "SOFTWARE\\ProgrammersHeaven\\Vault\\1.0" );
	key2.CreateBranch( "Colors" );
	key2.SetSubkey( "SOFTWARE\\ProgrammersHeaven\\Vault\\1.0\\Colors" );

	int sel = m_ctlHighlight.GetCurSel();
	key1.SetLeaf( "Highlight" );
	key1.SetValue( m_ctlHighlight.GetItemData( sel ) );
	key2.SetLeaf( "Highlight" );
	key2.SetValue( m_ctlHighlight.GetItemData( sel ) );

	sel = m_ctlBackground.GetCurSel();
	key1.SetLeaf( "Background" );
	key1.SetValue( m_ctlBackground.GetItemData( sel ) );
	key2.SetLeaf( "Background" );
	key2.SetValue( m_ctlBackground.GetItemData( sel ) );

	vdbDLLDialog::OnOK();
}

						
//-------------------------------------
void vdbListCtrlColors::OnSelchangeBackground() 
{
	int index = m_ctlBackground.GetCurSel();
	COLORREF color = m_ctlBackground.GetItemData( index );

	m_ctlList.SetBackgroundColor( color );
}


//-------------------------------------
void vdbListCtrlColors::OnSelchangeHighlight() 
{
	int index = m_ctlHighlight.GetCurSel();
	COLORREF color = m_ctlHighlight.GetItemData( index );

	m_ctlList.SetHighlightColor( color );
}



