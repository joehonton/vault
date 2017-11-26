//=============================================================================
//
// CLASS:           vdbFormatStyles
//
// OVERVIEW:        Dialog to format the appearance of reports
//
// FILE NAME:       vdbFormatStyles.cpp
//
// INITIAL DATE:    01/13/00
//
// COPYRIGHT:		Copyright © 2000 by Joe Honton
//
//=============================================================================

#include "stdReportBuilder.h"
#include "vdbFormatStyles.h"
#include "ReportStyleDefinition.h"
#include "ReportFontSpecs.h"
#include <cursors.h>

//warning C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead.
#pragma warning( disable : 4996 )

//=============================================================================
// Essential clas functions
//=============================================================================

//-------------------------------------
vdbFormatStyles::vdbFormatStyles( HDC hdc, ReportStyleDefinition* pDef, OID entryID )
	: vdbDLLDialog(vdbFormatStyles::IDD, NULL),
	  _hdc( hdc ),
	  _pDef( pDef ),
	  _selectedCategory( -2 ),
	  _entryID( entryID ),
	  _bInhibitSelchange( true )
{
	//{{AFX_DATA_INIT(vdbFormatStyles)
	//}}AFX_DATA_INIT
}


//-------------------------------------
void vdbFormatStyles::DoDataExchange(CDataExchange* pDX)
{
	vdbDLLDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(vdbFormatStyles)
	DDX_Control(pDX, IDOK, m_ctlOk);
	DDX_Control(pDX, IDC_VDB_FONT_STYLE_DESC, m_ctlFontStyleDesc);
	DDX_Control(pDX, IDC_VDB_FONT_SIZE_DESC, m_ctlFontSizeDesc);
	DDX_Control(pDX, IDC_VDB_FONT_FACE_DESC, m_ctlFontFaceDesc);
	DDX_Control(pDX, IDC_VDB_FONT_COLOR_DESC, m_ctlFontColorDesc);
	DDX_Control(pDX, IDC_VDB_CATEGORY_DESC, m_ctlCategoryDesc);
	DDX_Control(pDX, IDC_VDB_FONT_STYLE, m_ctlFontStyle);
	DDX_Control(pDX, IDC_VDB_SAMPLE, m_ctlSample);
	DDX_Control(pDX, IDC_VDB_FONT_SIZE, m_ctlFontSize);
	DDX_Control(pDX, IDC_VDB_FONT_FACE, m_ctlFontFace);
	DDX_Control(pDX, IDC_VDB_FONT_COLOR, m_ctlFontColor);
	DDX_Control(pDX, IDC_VDB_CATEGORY, m_ctlCategory);
	//}}AFX_DATA_MAP
}


//-------------------------------------
BEGIN_MESSAGE_MAP(vdbFormatStyles, vdbDLLDialog)
	//{{AFX_MSG_MAP(vdbFormatStyles)
	ON_WM_GETMINMAXINFO()
	ON_LBN_SELCHANGE(IDC_VDB_FONT_FACE, OnSelchangeFontFace)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_VDB_FONT_SIZE, OnSelchangeFontSize)
	ON_LBN_SELCHANGE(IDC_VDB_FONT_COLOR, OnSelchangeFontColor)
	ON_LBN_SELCHANGE(IDC_VDB_FONT_STYLE, OnSelchangeFontStyle)
	ON_LBN_SELCHANGE(IDC_VDB_CATEGORY, OnSelchangeCategory)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//=============================================================================
// Window initialization and sizing
//=============================================================================

//-------------------------------------
int vdbFormatStyles::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (vdbDLLDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ctlFontColor.Subclass( IDC_VDB_FONT_COLOR, this );

	return 0;
}


//-------------------------------------
//^ The OnGetMinMaxInfo function prevents the window from getting smaller
//  than a certain size.
//
void vdbFormatStyles::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// convert resource dialog units to screen coordinates.
	RECT dlgRect;
	dlgRect.left = 0;
	dlgRect.top = 0;
	dlgRect.right = 350;
	dlgRect.bottom = 250;
	MapDialogRect( &dlgRect );

	lpMMI->ptMinTrackSize.x = dlgRect.right;
	lpMMI->ptMinTrackSize.y = dlgRect.bottom;

	vdbDLLDialog::OnGetMinMaxInfo(lpMMI);
}


//-------------------------------------
void vdbFormatStyles::OnSize(UINT nType, int cx, int cy) 
{
	vdbDLLDialog::OnSize(nType, cx, cy);
	
	if ( !IsWindow( m_ctlSample.m_hWnd ) )
		return;

	int spacer = 5;
	int widthSample = cx - (spacer*2);
	int heightListboxes = cy * 3 / 5;
	int heightSample = cy - ( (spacer*3) + heightListboxes );
	int topStatic = (spacer*2) + heightSample;
	int heightStatic = 18;
	int widthWideListbox = (cx - (spacer*5)) * 2 / 6;
	int widthNarrowListbox = (cx - (spacer*5)) / 6;
	int leftColumn1 = spacer;
	int leftColumn2 = leftColumn1 + widthWideListbox + spacer;
	int leftColumn3 = leftColumn2 + widthWideListbox + spacer;
	int leftColumn4 = leftColumn3 + widthNarrowListbox + spacer;
	int topListbox = topStatic + heightStatic;	
	int heightListbox = cy - ( (3*spacer) + heightSample + heightStatic );
	int heightOneThirdListbox = heightListbox / 3;
	int heightTwoThirdsListbox = (heightListbox * 2 / 3) - (spacer + heightStatic);

	int topSizeStatic = topListbox + heightOneThirdListbox + spacer;
	int topSizeListbox = topSizeStatic + heightStatic;
	
	int widthOk = 80;
	int heightOk = 24;
	int leftOk = spacer + ( (widthWideListbox - widthOk) / 2 );
	int topOk = cy - (heightOk + spacer);

	m_ctlCategory.MoveWindow( leftColumn1, topListbox, widthWideListbox, heightTwoThirdsListbox, TRUE );
	m_ctlFontFace.MoveWindow( leftColumn2, topListbox, widthWideListbox, heightListbox, TRUE );
	m_ctlFontStyle.MoveWindow( leftColumn3, topListbox, widthNarrowListbox, heightOneThirdListbox, TRUE );
	m_ctlFontSize.MoveWindow( leftColumn3, topSizeListbox, widthNarrowListbox, heightTwoThirdsListbox, TRUE );
	m_ctlFontColor.MoveWindow( leftColumn4, topListbox, widthNarrowListbox, heightListbox, TRUE );

	m_ctlSample.MoveWindow( spacer, spacer, widthSample, heightSample, TRUE );
	m_ctlCategoryDesc.MoveWindow( leftColumn1, topStatic, widthWideListbox, heightStatic, TRUE );
	m_ctlFontFaceDesc.MoveWindow( leftColumn2, topStatic, widthWideListbox, heightStatic, TRUE );
	m_ctlFontStyleDesc.MoveWindow( leftColumn3, topStatic, widthNarrowListbox, heightStatic, TRUE );
	m_ctlFontSizeDesc.MoveWindow( leftColumn3, topSizeStatic, widthNarrowListbox, heightStatic, TRUE );
	m_ctlFontColorDesc.MoveWindow( leftColumn4, topStatic, widthNarrowListbox, heightStatic, TRUE );

	m_ctlOk.MoveWindow( leftOk, topOk, widthOk, heightOk, TRUE );
}


//-------------------------------------
BOOL vdbFormatStyles::OnInitDialog() 
{
	ShowHourglass();
	vdbDLLDialog::OnInitDialog();
	
	// force an immediate resize
	CRect rect;
	GetClientRect( rect );
	SendMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM( rect.right+1, rect.bottom+1) );

	m_ctlSample.Setup( _pDef, _entryID );

	int index = m_ctlCategory.AddString( "<All categories>" );
	m_ctlCategory.SetItemData( index, -1 );
	
	int countFontSpecs = _pDef->_fontSpecs.Count();
	for ( int i = 0; i < countFontSpecs; i++ )
	{
		index = m_ctlCategory.AddString( _pDef->_fontSpecs[i]._sFontSpecDescription );
		m_ctlCategory.SetItemData( index, i );
	}

	AddFontsToList();
	m_ctlFontSize.AddString( "6" );
	m_ctlFontSize.AddString( "7" );
	m_ctlFontSize.AddString( "8" );
	m_ctlFontSize.AddString( "9" );
	m_ctlFontSize.AddString( "10" );
	m_ctlFontSize.AddString( "11" );
	m_ctlFontSize.AddString( "12" );
	m_ctlFontSize.AddString( "14" );
	m_ctlFontSize.AddString( "16" );
	m_ctlFontSize.AddString( "18" );
	m_ctlFontSize.AddString( "20" );
	m_ctlFontSize.AddString( "24" );
	m_ctlFontSize.AddString( "28" );
	m_ctlFontSize.AddString( "32" );
	m_ctlFontSize.AddString( "36" );
	m_ctlFontSize.AddString( "48" );
	m_ctlFontSize.AddString( "60" );
	m_ctlFontSize.AddString( "72" );

	// light red to dark red
	AddColorToList( RGB(255,192,192) );
	AddColorToList( RGB(255,128,128) );
	AddColorToList( RGB(255,0,0) );
	AddColorToList( RGB(192,0,0) );

	// light orange to dark orange
	AddColorToList( RGB(255,192,128) );
	AddColorToList( RGB(255,128,0) );
	AddColorToList( RGB(192,128,64) );
	AddColorToList( RGB(192,96,0) );

	// light yellow to dark yellow
	AddColorToList( RGB(255,255,192) );
	AddColorToList( RGB(255,255,128) );
	AddColorToList( RGB(255,255,0) );
	AddColorToList( RGB(192,192,0) );

	// light green to dark green
	AddColorToList( RGB(192,255,192) );
	AddColorToList( RGB(128,255,128) );
	AddColorToList( RGB(96,192,0) );
	AddColorToList( RGB(64,128,0) );

	// light blue to dark blue	
	AddColorToList( RGB(192,192,255) );
	AddColorToList( RGB(128,128,255) );
	AddColorToList( RGB(0,128,255) );
	AddColorToList( RGB(0,0,255) );

	// light violet to dark violet
	AddColorToList( RGB(255,192,255) );
	AddColorToList( RGB(255,128,255) );
	AddColorToList( RGB(255,64,255) );
	AddColorToList( RGB(192,0,192) );

	// white, light gray, dark gray, black
	AddColorToList( RGB(255,255,255) );
	AddColorToList( RGB(227,227,227) );
	AddColorToList( RGB(192,192,192) );
	AddColorToList( RGB(128,128,128) );
	AddColorToList( RGB(96,96,96) );
	AddColorToList( RGB(64,64,64) );
	AddColorToList( RGB(0,0,0) );

	// start by showing the first category
	m_ctlCategory.SetCurSel( 0 );
	_bInhibitSelchange = false;
	OnSelchangeCategory();
	
	ShowPointer();
	return TRUE;
}


//=============================================================================
// Helper functions
//=============================================================================

//-------------------------------------
void vdbFormatStyles::AddFontsToList()
{
	LOGFONT lf;
	lf.lfCharSet = ANSI_CHARSET;
	strcpy( lf.lfFaceName, "" );

	FONTENUMPROC pFunc = &EnumFontFaces;
	EnumFontFamiliesEx( _hdc, &lf, pFunc, (LPARAM) this, 0 ); 
}


//-------------------------------------
void vdbFormatStyles::AddColorToList( COLORREF color )
{
	int index = m_ctlFontColor.AddString( "" );
	m_ctlFontColor.SetItemData( index, color );
}


//-------------------------------------
int CALLBACK EnumFontFaces( const LOGFONT *plf, const TEXTMETRIC *ptm, unsigned long FontType, LPARAM lParam )
{
	if ( plf->lfFaceName[0] == '@' )
		return 1;

	// check for duplicate font names
	vdbFormatStyles* pThis = reinterpret_cast<vdbFormatStyles*>( lParam );
	int index = pThis->m_ctlFontFace.FindStringExact( -1, plf->lfFaceName );
	if ( index != -1 )
		return 1;

	pThis->m_ctlFontFace.AddString( plf->lfFaceName );

	return 1;
}


//-------------------------------------
int CALLBACK EnumFontStyles( const LOGFONT *plf, const TEXTMETRIC *ptm, unsigned long FontType, LPARAM lParam )
{
	vdbFormatStyles* pThis = reinterpret_cast<vdbFormatStyles*>( lParam );

	const ENUMLOGFONTEX* plfe = reinterpret_cast<const ENUMLOGFONTEX*>(plf);
	
	char sz[32];
	vdbUtility::SafeCopy( sz, (const char*) plfe->elfStyle, sizeof(sz) );

	pThis->m_ctlFontStyle.AddString( sz );

	return 1;
}


//=============================================================================
// User interface callbacks
//=============================================================================

//-------------------------------------
void vdbFormatStyles::OnSelchangeFontFace() 
{
	if ( _selectedCategory == -2 )
		return;

	char szFaceName[81];
	m_ctlFontFace.GetText( m_ctlFontFace.GetCurSel(), szFaceName );

	// if the face name is not blank look up all font styles
	if ( strcmp( szFaceName, "" ) )
	{
		// get current font style and save
		char szSaveStyle[32];
		int index = m_ctlFontStyle.GetCurSel();
		m_ctlFontStyle.GetText( index, szSaveStyle );
		m_ctlFontStyle.ResetContent();

		// lookup all possible font styles for the newly selected face
		LOGFONT lf;
		lf.lfCharSet = ANSI_CHARSET;
		vdbUtility::SafeCopy( lf.lfFaceName, szFaceName, sizeof(lf.lfFaceName) );
		FONTENUMPROC pFunc = &EnumFontStyles;
		EnumFontFamiliesEx( _hdc, &lf, pFunc, (LPARAM) this, 0 ); 

		// reselect previous font style
		index = m_ctlFontStyle.FindStringExact( -1, szSaveStyle );
		m_ctlFontStyle.SetCurSel( index );
	}

	if ( _bInhibitSelchange == true )
		return;
	_bInhibitSelchange = true;

	// A single category is selected
	if ( _selectedCategory != -1 )
	{
		assert( _selectedCategory >= 0 );
		_pDef->_fontSpecs[_selectedCategory]._sFontFaceName = szFaceName;
	}
	// The "<All categories>" item is selected
	else
	{
		int countFontSpecs = _pDef->_fontSpecs.Count();
		for ( int i = 0; i < countFontSpecs; i++ )
			_pDef->_fontSpecs[i]._sFontFaceName = szFaceName;
	}

	ShowSample();
	_bInhibitSelchange = false;
}


//-------------------------------------
void vdbFormatStyles::OnSelchangeFontStyle() 
{
	if ( _selectedCategory == -2 )
		return;

	if ( _bInhibitSelchange == true )
		return;
	_bInhibitSelchange = true;
	
	char szFontStyle[81];
	m_ctlFontStyle.GetText( m_ctlFontStyle.GetCurSel(), szFontStyle );

	// A single category is selected
	if ( _selectedCategory != -1 )
	{
		assert( _selectedCategory >= 0 );
		_pDef->_fontSpecs[_selectedCategory]._sFontStyle = szFontStyle;
	}
	// The "<All categories>" item is selected
	else
	{
		int countFontSpecs = _pDef->_fontSpecs.Count();
		for ( int i = 0; i < countFontSpecs; i++ )
			_pDef->_fontSpecs[i]._sFontStyle = szFontStyle;
	}

	ShowSample();
	_bInhibitSelchange = false;
}


//-------------------------------------
void vdbFormatStyles::OnSelchangeFontSize() 
{
	if ( _selectedCategory == -2 )
		return;

	if ( _bInhibitSelchange == true )
		return;
	_bInhibitSelchange = true;
	
	char szFontSize[10];
	m_ctlFontSize.GetText( m_ctlFontSize.GetCurSel(), szFontSize );

	// A single category is selected
	if ( _selectedCategory != -1 )
	{
		assert( _selectedCategory >= 0 );
		_pDef->_fontSpecs[_selectedCategory]._pointSize = atoi( szFontSize );
	}
	// The "<All categories>" item is selected
	else
	{
		int countFontSpecs = _pDef->_fontSpecs.Count();
		for ( int i = 0; i < countFontSpecs; i++ )
			_pDef->_fontSpecs[i]._pointSize = atoi( szFontSize );
	}

	ShowSample();
	_bInhibitSelchange = false;
}


//-------------------------------------
void vdbFormatStyles::OnSelchangeFontColor() 
{
	if ( _selectedCategory == -2 )
		return;

	if ( _bInhibitSelchange == true )
		return;
	_bInhibitSelchange = true;
	
	COLORREF color = m_ctlFontColor.GetItemData( m_ctlFontColor.GetCurSel() );

	// A single category is selected
	if ( _selectedCategory != -1 )
	{
		assert( _selectedCategory >= 0 );
		_pDef->_fontSpecs[_selectedCategory]._textColor = color;
	}
	// The "<All categories>" item is selected
	else
	{
		int countFontSpecs = _pDef->_fontSpecs.Count();
		for ( int i = 0; i < countFontSpecs; i++ )
			_pDef->_fontSpecs[i]._textColor = color;
	}

 	ShowSample();
	_bInhibitSelchange = false;
}


//-------------------------------------
void vdbFormatStyles::ShowSample()
{
	m_ctlSample.Refresh();
}


//-------------------------------------
//^ The OnSelchangeCategory displays the selected category's font, size, and color
//  in the user controls.
void vdbFormatStyles::OnSelchangeCategory() 
{
	if ( _bInhibitSelchange == true )
		return;
	_bInhibitSelchange = true;

	int index = m_ctlCategory.GetCurSel();
	if ( index == -1 )
		return;
	
	_selectedCategory = m_ctlCategory.GetItemData( index );


	// The "<All categories>" item is selected
	if ( _selectedCategory == -1 )
	{
		int countFontSpecs = _pDef->_fontSpecs.Count();
		if ( countFontSpecs < 1 )
		{
			_bInhibitSelchange = false;
			return;
		}

		ReportFontSpecs& c = _pDef->_fontSpecs[0];
		vdbString sCommonFaceName = c._sFontFaceName;
		vdbString sCommonStyle = c._sFontStyle;
		int commonPointSize = c._pointSize;
		COLORREF commonColor = c._textColor;

		for ( int i = 1; i < countFontSpecs; i++ )
		{
			ReportFontSpecs& rc = _pDef->_fontSpecs[i];

			if ( sCommonFaceName != rc._sFontFaceName )
				sCommonFaceName = "";

			if ( sCommonStyle != rc._sFontStyle )
				sCommonStyle = "";

			if ( commonPointSize != rc._pointSize )
				commonPointSize = -1;

			if ( commonColor != rc._textColor )
				commonColor = 12345;
		}
		
		index = m_ctlFontFace.FindStringExact( -1, sCommonFaceName );
		m_ctlFontFace.SetCurSel( index );
		OnSelchangeFontFace();

		index = m_ctlFontStyle.FindStringExact( -1, sCommonStyle );
		m_ctlFontStyle.SetCurSel( index );

		char sz[10];
		vdbUtility::SafeFormat( sz, sizeof(sz), "%d", commonPointSize );
		index = m_ctlFontSize.FindStringExact( -1, sz );
		m_ctlFontSize.SetCurSel( index );

		if ( commonColor == 12345 )
			m_ctlFontColor.SetCurSel( -1 );
		else
		{
			int countColors = m_ctlFontColor.GetCount();
			for ( int j = 0; j < countColors; j++ )
			{
				COLORREF color = m_ctlFontColor.GetItemData( j );
				if ( color == commonColor )
				{
					m_ctlFontColor.SetCurSel( j );
					break;
				}
			}
		}
	}
	
	// A single category is selected
	else
	{
		ReportFontSpecs& c = _pDef->_fontSpecs[_selectedCategory];

		index = m_ctlFontFace.FindStringExact( -1, c._sFontFaceName );
		m_ctlFontFace.SetCurSel( index );
		OnSelchangeFontFace();

		index = m_ctlFontStyle.FindStringExact( -1, c._sFontStyle );
		m_ctlFontStyle.SetCurSel( index );

		char sz[10];
		vdbUtility::SafeFormat( sz, sizeof(sz), "%d", c._pointSize );
		index = m_ctlFontSize.FindStringExact( -1, sz );
		m_ctlFontSize.SetCurSel( index );

		int countColors = m_ctlFontColor.GetCount();
		for ( int i = 0; i < countColors; i++ )
		{
			COLORREF color = m_ctlFontColor.GetItemData( i );
			if ( color == c._textColor )
			{
				m_ctlFontColor.SetCurSel( i );
				break;
			}
		}
	}

	ShowSample();
	_bInhibitSelchange = false;
}


