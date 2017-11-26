//=============================================================================
//
// FILE NAME:       vdbSetOfServers.cpp
//
// ESSENCE:         Server Information to be kept on disk
//
// DEVELOPER:       Joe Honton
//
// INITIAL DATE:    1/19/96
//
// COPYRIGHT:       Copyright © 1996 by Joe Honton
//
//=============================================================================

#include <vdbWindows.h>
#include <strstream>
#include <fstream>
#include "vdbException.h"

#include "vdbSetOfServers.h"
#include "vdbServerInfo.h"
#include "vdbMessageBox.h"
#include "vdbProfileString.h"
#include "vdbFileDialog.h"
#include "vdbLong.h"

//=============================================================================
// Construction -- Singleton
//=============================================================================

//-------------------------------------------------------
//^^ The vdbSetOfServers constructor
//
vdbSetOfServers::vdbSetOfServers() : vdbSet<vdbServerInfo>()
{
}


//-------------------------------------------------------
//^^ The vdbSetOfServers destructor.
//^ Every application must call delete on the vdbSetOfServers instance
//  created through the GetSetOfServers function.
//
vdbSetOfServers::~vdbSetOfServers()
{
	_instance = 0;
}


//-------------------------------------------------------
// The static member pointer
vdbSetOfServers* vdbSetOfServers::_instance = 0;


//-------------------------------------
//^ The Instance funcion returns a pointer to the vdbSetOfServers
//  instance. All access to the set is through this member function.
//  The first call to the function instantiates the class.  Subsequent
//  calls return a pointer to the same instance.  This mechanism ensures
//  that one and only one instance per application exists.
//
//^ Every application must also call Destroy on the instance created through
//  Instance.
//
vdbSetOfServers* vdbSetOfServers::Instance()
{
	if ( _instance == 0 )
		_instance = new vdbSetOfServers;
	if ( _instance == 0 ) throw vdbMemoryException();

	return _instance;
}


//-------------------------------------
void vdbSetOfServers::Destroy()
{
	if ( _instance != 0 )
	{
		delete _instance; _instance = 0;
	}
}


//=============================================================================
// Read and write from archive
//=============================================================================

//-----------------------------------------
//^ The ReadFromArchive function reads all servers and their datatype maps
//  from disk to memory.
//> openForEdit, if true, will check whether or not the
//  map file is read-only.  The routine will alert the user if it is.
//< returns true if successful
//< returns false on failure
//
bool vdbSetOfServers::ReadFromArchive( bool openForEdit )
{
	bool rc = true;

	// read the vault.ini file to find the location of the mapfile
	char szFilename[80];
	vdbGetPrivateProfileString( "Server Administrator", "Server Map Data", "VSERVER.MAP", szFilename, sizeof(szFilename), "vault.ini" ); 
	_ServerMapFilename = vdbFilename( szFilename );

	// alert user if mapfile cannot be found, request assistance in finding it
	if ( !_ServerMapFilename.Exists() )
	{
		vdbFileDlgData data( NULL, "Unable to locate Vault Server Map", "Select the \"VSERVER.MAP\" file", _ServerMapFilename.Directory(), _ServerMapFilename.FilenameWithExtension(), false );
		data.AddFileFilter( "Map Files (*.map)", "map", true );
		data.AddFileFilter( "All Files (*.*)", "*", false );
		vdbFileDialog( data );
		_ServerMapFilename = data.GetDirectory();
		_ServerMapFilename.Append( data.GetSelectedFilename() );

		vdbWritePrivateProfileString( "Server Administrator", "Server Map Data", _ServerMapFilename, "vault.ini" );
		
		// check again whether or not the mapfile exists
		if ( !_ServerMapFilename.Exists() )
		{
			if ( openForEdit )
				// it's OK for the file to not exist, because the user is running the
				// ServerAdministrator.exe and is creating the file the first time.
				return true;		
			else
				// not OK because the user definately needs this file!
				return false;
		}
	}


	// alert user if we are trying to edit and the file is read-only.
	if ( openForEdit )
	{
		if ( _ServerMapFilename.ReadOnly() )
		{
			std::ostrstream os;
			os << "The Vault server map file " << (const char*) _ServerMapFilename << " is read-only.  Changes will not be saved." << std::ends;
			char* s = os.str();
			vdbMessageBox( NULL, s, "Vault", MB_OK );
			delete[] s;
		}
	}

	try
	{
		// if found, continue
		std::ifstream archive( _ServerMapFilename, std::ios_base::binary );

		// read header
		vdbString header;
		header.Get( archive );
		if ( header != "Vault Server Map" )
		{
			std::ostrstream os;
			os << "The Vault server map file " << (const char *) _ServerMapFilename << " has an unrecognized header.  Continue anyway?." << std::ends;
			char* s = os.str();
			if ( vdbMessageBox( NULL, s, "Vault", MB_OKCANCEL ) == IDCANCEL )
			{
				delete[] s;
				archive.close();
				return false;
			}
			else
				delete[] s;
		}

		// read version
		vdbLong version;
		archive >> version;
		if ( version != 1 )
		{
			std::ostrstream os;
			os << "The Vault server map file " << (const char*) _ServerMapFilename << " has an unrecognized version.  Continue anyway?." << std::ends;
			char* s = os.str();
			if ( vdbMessageBox( NULL, s, "Vault", MB_OKCANCEL ) == IDCANCEL )
			{
				delete[] s;
				archive.close();
				return false;
			}
			else
				delete[] s;
		}

		// loop through all serverInfo instances and read
		this->ClearAll();

		vdbLong countServers;
		archive >> countServers;
		for ( int i = 0; i < countServers; i++ )
		{
			vdbServerInfo* pInfo = new vdbServerInfo;
			if ( pInfo == 0 ) throw vdbMemoryException();
//x			archive >> pInfo->_serverDescription >> pInfo->_serverIdentificationString;
//x			archive >> pInfo->_administratorID >> pInfo->_administratorPassword;
			pInfo->_serverDescription.Get( archive );
			pInfo->_serverIdentificationString.Get( archive );
			pInfo->_administratorID.Get( archive );
			pInfo->_administratorPassword.Get( archive );

			// loop through datatypes and read
			vdbLong countDatatypes;
			archive >> countDatatypes;
			for ( int j = 0; j < countDatatypes; j++ )
			{
				vdbDataTypeMap* pType = new vdbDataTypeMap;
				if ( pType == 0 ) throw vdbMemoryException();
//x				archive >> pType->_serverDataType >> pType->_vaultDataType;
				pType->_serverDataType.Get( archive );
				pType->_vaultDataType.Get( archive );
				pInfo->_datatypes.Add( pType );
			}

			// add to the set of servers
			this->Add( pInfo );
		}

		archive.close();
		return true;
	}

	catch ( ... )
	{
		std::ostrstream os;
		os << "Unable to read the Server Map: " << (const char*) _ServerMapFilename << std::ends;
		char* s = os.str();
		vdbMessageBox( NULL, s, "Vault", MB_OK );
		delete[] s;
		return false;
	}
}


//-----------------------------------------
//^ The WriteToArchive function saves all servers and their datatype maps
//  from memory to disk.
//< returns true if successful
//< returns false on failure
//
bool vdbSetOfServers::WriteToArchive()
{
	// do not write if project file is read-only
	if ( _ServerMapFilename.Exists() )
	{
		if ( _ServerMapFilename.ReadOnly() )
		{
			std::ostrstream os;
			os << "The project file " << (const char*) _ServerMapFilename << " is read-only and cannot be saved." << std::ends;
			char* s = os.str();
			vdbMessageBox( NULL, s, "Vault", MB_OK );
			delete[] s;
			return false;
		}
	}

	try
	{
		std::ofstream archive( _ServerMapFilename, std::ios_base::binary );

		// write header and version
		vdbString header( "Vault Server Map" );
//x		archive << header;
		header.Put( archive );
		vdbLong version = 1;
		archive << version;
	
		// loop through all serverInfo instances and write
		vdbLong countServers = this->Count();
		archive << countServers;
		for ( int i = 0; i < countServers; i++ )
		{
			vdbServerInfo* pInfo = &this->Get(i);
//x			archive << pInfo->_serverDescription << pInfo->_serverIdentificationString;
//x			archive << pInfo->_administratorID << pInfo->_administratorPassword;
			pInfo->_serverDescription.Put( archive );
			pInfo->_serverIdentificationString.Put( archive );
			pInfo->_administratorID.Put( archive );
			pInfo->_administratorPassword.Put( archive );

			// loop through datatypes and write
			vdbLong countDatatypes = pInfo->_datatypes.Count();
			archive << countDatatypes;
			for ( int j = 0; j < countDatatypes; j++ )
			{
				vdbDataTypeMap* pType = &(pInfo->_datatypes[j]);
//x				archive << pType->_serverDataType << pType->_vaultDataType;
				pType->_serverDataType.Put( archive );
				pType->_vaultDataType.Put( archive );
			}
		}

		archive.close();
		return true;
	}

	catch ( ... )
	{
		std::ostrstream os;
		os << "Unable to save the Server Map: " << (const char*) _ServerMapFilename << std::ends;
		char* s = os.str();
		vdbMessageBox( NULL, s, "Vault", MB_OK );
		delete[] s;
		return false;
	}
}

	
//-----------------------------------------
//^ The Find function looks through the set of servers and finds the
//  one with the specified driver name.
//> szDriverName is the name of the server to find.
//< Returns a pointer to the server with that name if found, otherwise returns
//  a pointer to the "<default server>".  If there is no "<default server>"
//  throws an exception.
//
vdbServerInfo* vdbSetOfServers::Find( vdbString& szDriverName )
{
	int i;

	// first, search for matching driver name
	for ( i = 0; i < Count(); i++ )
	{
		vdbServerInfo* p = &Get(i);
		if ( p->_serverIdentificationString == szDriverName )
			return p;
	} 	

	// next, search for default
	for ( i = 0; i < Count(); i++ )
	{
		vdbServerInfo* p = &Get(i);
		if ( p->_serverIdentificationString == "<default server>" )
			return p;
	}

	// last ditch effort, alert the user
	{
		std::ostrstream os;
		os << "No matching server with the name \"" << (const char*) szDriverName;
		os << "\" was found in the server map \"" << (const char*) _ServerMapFilename;
		os << "\" and no \"<default server>\" is available.\n\n";
		os << "Press OK to continue anyway or Cancel to quit the application." << std::ends;
		char* s = os.str();
		if ( vdbMessageBox( NULL, s, "Server Error", MB_SETFOREGROUND | MB_OKCANCEL ) == IDCANCEL )
		{
			delete[] s;
			throw vdbException( "Application cancelled" );
		}
		delete[] s;

		// just use the first server in the set
		vdbServerInfo* p = &Get(0);
		return p;
	}
}
