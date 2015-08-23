


#include "../config.hpp"

#include "../../log.hpp"
#include "../../except.hpp"
#include "../../assertdef.hpp"
//#include "../../stringtable.hpp"

#include <windows.h>
#include <winreg.h>


//
// This file is included by ../config.cpp
//


#define CONFIGURATION "CONFIGURATION"


namespace CppUtils {




//=============================================================
// Class ConfigurationWin32
//=============================================================
/** Class ConfigurationWin32 is a specialization of Configuration
		to use on Windows32. It utilitizes the registry to store data,
		where the root key is \c XXX/Software/TOP.
		\c XXX is either \c HKEY_USERS or \c HKEY_LOCAL_MACHINE, depending
		on the config type passed in in the ctor. \c TOP is a constant string
		defined in the header file config.hpp and is currently set to "VPI".

		\attention This implementation supports an extenstion to class 
		Configuration in that sections may start with "../" (not with 
		repeated "../"!) in order to access registry keys starting with 
		\c XXX/Software.


		\author Olaf Lenzmann
*/
class ConfigurationWin32 : public Configuration {

public:

	//======================================
	// Creators
	//

	/** Explicit ctor. Raises ExceptionInternal if
			the registry could not be opened or the keys could
			not be retrieved/set. */
	ConfigurationWin32 (
		ConfigType ct
	) throw (
		ExceptionInternal
	);

	/** Dtor. Makes changes permanent. */
	virtual ~ConfigurationWin32 (
	);


	//======================================
	// Methods
	//

	/** Implements function declared in Configuration. */
	void setValue (
		String const &section,
		String const &key,
		String const &value
	);

	/** Implements function declared in Configuration. */
	String getValue (
		String const &section,
		String const &key,
		String const &def = ""
	) const;

	/** Implements function declared in Configuration. */
	StringList getKeys (
		String const &section
	) const;

	/** Implements function declared in Configuration. */
	void getAllEntries (
		String const &section,
		StringMap &entries
	) const;

	/** Implements function declared in Configuration. */
	StringList getSections (
		String const &section
	) const;

	/** Implements function declared in Configuration. */
	void deleteEntry (
		String const &section,
		String const &key
	);

	/** Implements function declared in Configuration. */
	void deleteSection (
		String const &section
	);



private:

	//======================================
	// Variables
	//

	/** Registry key handle to root of configuration data. */
	HKEY startKey_m;

	/** Registry key handle to root of VPI configuration data. */
	HKEY rootKey_m;

	/** Security attributes to use for system keys. */
	SECURITY_ATTRIBUTES securityAttributes_m;

	//======================================
	// Helper functions
	//

	/** Returns the registry key handle to the given section relative to the
			root. If the section does not exist, it returns a NULL handle.

			\attention This function implements the non-standard behavior supporting
			a leading "../" in the section name.
	*/
	HKEY openSection (
		HKEY root,
		String const &section
	) const;

	/** Returns the registry key handle to the given section relative to the
			root. If the section does not exist, it is created. If it could not
			be created, an exception is thrown. */
	HKEY createSection (
		HKEY root,
		String const &section
	) throw (
		ExceptionInternal
	);

	/** Delete an entire section as qualified by key handle/subkey from the registry. */
	void killSection (
		HKEY key,
		String const &subkey
	);
};




//=============================================================
// Factory for class Configuration
//=============================================================
Configuration * getConfiguration (
		Configuration::ConfigType ct
	) throw (
		ExceptionInternal
	)
{
	return new ConfigurationWin32( ct );
}




//=============================================================
// Class ConfigurationWin32 on Windows 32
//=============================================================
ConfigurationWin32::ConfigurationWin32 (
		ConfigType ct
	) :
		startKey_m( 0 ),
		rootKey_m( 0 )
{
	LOG( MSG_DEBUG, CONFIGURATION, "Opening configuration for " << (ct == CT_User ? "user" : "machine") << "." );


	// Create a security descriptor to create HKEY_LOCAL_MACHINE keys
	// that can also be accessed by a non-admin
	//
	securityAttributes_m.nLength = sizeof( SECURITY_ATTRIBUTES );
	securityAttributes_m.bInheritHandle = false;
	securityAttributes_m.lpSecurityDescriptor = new SECURITY_DESCRIPTOR;

	if( !InitializeSecurityDescriptor( securityAttributes_m.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION ) )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Configuration_Init", getOSError().c_str() );

	if( !SetSecurityDescriptorDacl( securityAttributes_m.lpSecurityDescriptor, false, NULL, false ) )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Configuration_Init", getOSError().c_str() );


	// First open the start key with read-only
	if( RegOpenKeyEx( ( ct == CT_User ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE ),
		"Software", 0, KEY_READ, &startKey_m ) != ERROR_SUCCESS )
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Configuration_Init", getOSError().c_str() );

	// Now get the company key as read-write
	if( RegOpenKeyEx( startKey_m, companyName().c_str(), 0, (KEY_READ | KEY_WRITE), &rootKey_m ) != ERROR_SUCCESS ) {
		// Failed to open -> either the key does not exists or we do
		// not have the required rights; assume the former and
		// create the entire key
		//

		// Re-open the start key with write-access
		RegCloseKey( startKey_m );
		if( RegOpenKeyEx( ( ct == CT_User ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE ),
			"Software", 0, (KEY_READ | KEY_WRITE), &startKey_m ) != ERROR_SUCCESS ) {
			// We are not allowed; give up
			//LOG( MSG_FATAL, CONFIGURATION, 
			//	GET_LANG_STRING( "util_exc_Configuration_FailedToCreateRootKey" ) << "\n( " <<
			//	GET_LANG_STRING( "util_ctx_Configuration_Init" ) << " )"
			//);

			LOG( MSG_FATAL, CONFIGURATION, 
				 "util_exc_Configuration_FailedToCreateRootKey"  << "\n( " <<
				 "util_ctx_Configuration_Init"  << " )"
			);
		}

		// Create the root key
		LOG( MSG_DEBUG, CONFIGURATION, "Creating configuration root key \"" << companyName() << "\"." );

		if( RegCreateKeyEx( startKey_m, companyName().c_str(), 0, NULL, REG_OPTION_NON_VOLATILE,
			(KEY_READ | KEY_WRITE), &securityAttributes_m, &rootKey_m, NULL ) != ERROR_SUCCESS ) {
			// Strange... give up
			//LOG( MSG_FATAL, CONFIGURATION, 
			//	GET_LANG_STRING( "util_exc_Configuration_FailedToCreateRootKey" ) << "\n( " <<
			//	GET_LANG_STRING( "util_ctx_Configuration_Init" ) << " )"
			//);

			LOG( MSG_FATAL, CONFIGURATION, 
				 "util_exc_Configuration_FailedToCreateRootKey"  << "\n( " <<
				 "util_ctx_Configuration_Init"  << " )"
			);
		}
	}
	else {
		// Re-set the security attributes on the root key to make sure
		// that also non-admins may write it
		LOG( MSG_DEBUG, CONFIGURATION, "Setting ACL on configuration root key." );

		RegSetKeySecurity( rootKey_m, DACL_SECURITY_INFORMATION, securityAttributes_m.lpSecurityDescriptor );
	}
}
//-------------------------------------------------------------
ConfigurationWin32::~ConfigurationWin32 (
	)
{
	// Flush and close our start key if we got one
	if( startKey_m != 0 ) {
		RegFlushKey( startKey_m );
		RegCloseKey( startKey_m );
	}

	// Flush and close our root key if we got one
	if( rootKey_m != 0 ) {
		RegFlushKey( rootKey_m );
		RegCloseKey( rootKey_m );
	}

	// Delete the security descriptor
	delete securityAttributes_m.lpSecurityDescriptor;
	securityAttributes_m.lpSecurityDescriptor = NULL;
}
//======================================
// Methods
//======================================
void ConfigurationWin32::setValue (
		String const &section,
		String const &key,
		String const &value
	)
{
	// Lock the registry for the scope
	WRITE_LOCK_FOR_SCOPE( *this );

	// Nothing to go into the top-level section
	HTASSERT( section.find_first_not_of( sectionDelimiter_scm ) != String::npos );

	LOG( MSG_DEBUG, CONFIGURATION, "Setting key \"" << key << "\" in section \"" << section << "\" to \"" << value << "\"" );

	// Get the HKEY for the section
	HKEY k = createSection( rootKey_m, section );

	// Set the value in the section
	if( RegSetValueEx( k, key.c_str(), 0, REG_SZ, (const unsigned char *) value.c_str(), value.length()+1 ) != ERROR_SUCCESS ) {
		// Some error occurred; clean up and throw
		if( k != rootKey_m && k != startKey_m )
			RegCloseKey( k );
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Configuration_SetKey", getOSError().c_str() );
	}

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );
}
//-------------------------------------------------------------
String ConfigurationWin32::getValue (
		String const &section,
		String const &key,
		String const &def
	) const
{
	// Lock the registry for the scope
	READ_LOCK_FOR_SCOPE( *this );

	// Nothing to go into the top-level section
	HTASSERT( section.find_first_not_of( sectionDelimiter_scm ) != String::npos );

	// Get the HKEY for the section
	HKEY k = openSection( rootKey_m, section );
	if( k == HKEY(0) )
		// Section not found; return the default
		return def;

	// Get the value; first get the size
	DWORD length;
	if( RegQueryValueEx( k, key.c_str(), 0, NULL, NULL, &length ) != ERROR_SUCCESS ) {
		// Value not found; return the default
		if( k != rootKey_m && k != startKey_m )
			RegCloseKey( k );
		return def;
	}

	// Allocate memory and get the actual data
	unsigned char *buf = new unsigned char[length];
	RegQueryValueEx( k, key.c_str(), 0, NULL, buf, &length );
	String result((char *) buf);
	delete buf;

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );

	// Log
	LOG( MSG_DEBUG_DETAIL, CONFIGURATION, "Retrieving key \"" << key << "\" in section \"" << section << "\" yielded \"" << result << "\"" );

	return result;
}
//-------------------------------------------------------------
StringList ConfigurationWin32::getSections (
		String const &section
	) const
{
	// Lock the registry for the scope
	READ_LOCK_FOR_SCOPE( *this );

	int sub = 0;
	DWORD length = 1024;
	char buffer[1024];
	StringList sections;

	// Get the HKEY for the section
	HKEY k = openSection( rootKey_m, section );
	if( k == HKEY(0) )
		// Section not found; return an empty set
		return sections;

	// Iterate through the key
	while( RegEnumKeyEx( k, sub, buffer, &length, NULL, NULL, NULL, NULL ) != ERROR_NO_MORE_ITEMS ) {
		// push_back section
		sections.push_back( buffer );

		// Go on
		length = 1024;
		++sub;
	}

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );

	// Log
	LOG( MSG_DEBUG_DETAIL, CONFIGURATION, "Retrieving subsections of section \"" << section << "\" yielded " << sections.size() << " hits." );

	return sections;
}
//-------------------------------------------------------------
StringList ConfigurationWin32::getKeys (
		String const &section
	) const
{
	// Lock the registry for the scope
	READ_LOCK_FOR_SCOPE( *this );

	int sub = 0;
	DWORD length = 1024;
	char buffer[1024];
	StringList keys;

	// Get the HKEY for the section
	HKEY k = openSection( rootKey_m, section );
	if( k == HKEY(0) )
		// Section not found; return an empty set
		return keys;

	// Iterate through the key
	while( RegEnumValue( k, sub, buffer, &length, NULL, NULL, NULL, NULL ) != ERROR_NO_MORE_ITEMS ) {
		// push_back key
		keys.push_back( buffer );

		// Go on
		length = 1024;
		++sub;
	}

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );

	// Log
	LOG( MSG_DEBUG_DETAIL, CONFIGURATION, "Retrieving keys of section \"" << section << "\" yielded " << keys.size() << " hits." );

	return keys;
}
//-------------------------------------------------------------
void ConfigurationWin32::getAllEntries (
		String const &section,
		StringMap &entries
	) const
{
	entries.clear();

	// Lock the registry for the scope
	READ_LOCK_FOR_SCOPE( *this );

	int sub = 0;
	DWORD length = 1024;
	char buffer[1024];
	StringList keys;

	// Get the HKEY for the section
	HKEY k = openSection( rootKey_m, section );
	if( k == HKEY(0) )
		// Section not found; return an empty map
		return;

	// Iterate through the key
	while( RegEnumValue( k, sub, buffer, &length, NULL, NULL, NULL, NULL ) != ERROR_NO_MORE_ITEMS ) {
		// Get the value; first get the size
		DWORD valueLength;
		if( RegQueryValueEx( k, buffer, 0, NULL, NULL, &valueLength ) != ERROR_SUCCESS )
			// Value not found; return empty map
			return;

		// Allocate memory and get the actual data
		unsigned char *buf = new unsigned char[valueLength];
		RegQueryValueEx( k, buffer, 0, NULL, buf, &valueLength );
		entries[ buffer ] = (char *) buf;
		delete buf;

		// Go on
		length = 1024;
		++sub;
	}

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );

	// Log
	LOG( MSG_DEBUG_DETAIL, CONFIGURATION, "Retrieving entries of section \"" << section << "\" yielded " << entries.size() << " hits." );
}
//-------------------------------------------------------------
void ConfigurationWin32::deleteEntry (
		String const &section,
		String const &key
	)
{
	// Lock the registry for the scope
	WRITE_LOCK_FOR_SCOPE( *this );

	// Nothing to go into the top-level section
	HTASSERT( section.find_first_not_of( sectionDelimiter_scm ) != String::npos );

	LOG( MSG_DEBUG, CONFIGURATION, "Deleting key \"" << key << "\" in section \"" << section << "\"." );

	// Get the HKEY for the section
	HKEY k = openSection( rootKey_m, section );
	if( k == HKEY(0) )
		// Section not found; nothing to delete
		return;

	// Try to delete the entry
	RegDeleteValue( k, key.c_str() );

	// Done
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );
}
//-------------------------------------------------------------
void ConfigurationWin32::deleteSection (
		String const &section
	)
{
	// Lock the registry for the scope
	WRITE_LOCK_FOR_SCOPE( *this );

	// Can't delete the root key
	HTASSERT( section.find_first_not_of( sectionDelimiter_scm ) != String::npos );

	LOG( MSG_DEBUG, CONFIGURATION, "Deleting section \"" << section << "\"." );

	// Get the HKEY for the section; we need to stop one key before the one to delete
	HKEY k = rootKey_m;
	// Is the section to be deleted not at the top-level?
	if( section.find_first_of( sectionDelimiter_scm ) != String::npos )
		k = openSection( k, section.substr( 0, section.find_last_of( sectionDelimiter_scm ) ) );

	if( k == HKEY(0) )
		// Section not found; nothing to delete
		return;

	// Call recursive deletion fun
	killSection( k, section.substr( section.find_last_of( sectionDelimiter_scm ) + 1 ) );

	// Close the key if not root
	if( k != rootKey_m && k != startKey_m )
		RegCloseKey( k );
}
//======================================
// Helper functions
//======================================
HKEY ConfigurationWin32::openSection (
		HKEY root,
		String const &section
	) const
{
	// No need to lock, as this is an internal helper fun

	HKEY k1 = root, k2;
	int index = 0;

	do {
		// Get the next section element
		int nextIndex = section.find_first_of( sectionDelimiter_scm, index );
		String thisSection = section.substr( index, nextIndex == String::npos ? String::npos : nextIndex - index );
		if( nextIndex != String::npos )
			index = nextIndex + 1;
		else
			index = String::npos;

		// Windows-only barf hack to support leading "../" in section string
		if( k1 == rootKey_m && thisSection == ".." ) {
			k1 = root = startKey_m;
			thisSection = "";
		}

		// Open the section
		if( !thisSection.empty() ) {
			if( RegOpenKeyEx( k1,	thisSection.c_str(), 0, (KEY_READ | KEY_WRITE), &k2 ) != ERROR_SUCCESS ) {
				// Key does not exist; clean up and exit
				if( k1 != rootKey_m && k1 != startKey_m )
					RegCloseKey( k1 );
				return HKEY(0);
			}

			// Go further down the path
			if( k1 != rootKey_m && k1 != startKey_m )
				RegCloseKey( k1 );

			k1 = k2;
		}
	} while( index != String::npos );

	// Done
	return k1;
}
//-------------------------------------------------------------
HKEY ConfigurationWin32::createSection (
		HKEY root,
		String const &section
	)
{
	LOG( MSG_DEBUG, CONFIGURATION, "Creating subsection \"" << section << "\"." );

	// No need to lock, as this is an internal helper fun
	HKEY k1 = root, k2;
	DWORD x;
	int index = 0;

	do {
		// Get the next section element
		int nextIndex = section.find_first_of( sectionDelimiter_scm, index );
		String thisSection = section.substr( index, nextIndex == String::npos ? String::npos : nextIndex - index );
		if( nextIndex != String::npos )
			index = nextIndex + 1;
		else
			index = String::npos;

		// Create the section
		if( !thisSection.empty() ) {
			if( RegCreateKeyEx( k1,	thisSection.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, (KEY_READ | KEY_WRITE), &securityAttributes_m, &k2, &x ) != ERROR_SUCCESS ) {
				// Key does not exist; clean up and exit
				if( k1 != rootKey_m && k1 != startKey_m )
					RegCloseKey( k1 );
		
				THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Configuration_CreateKey", getOSError().c_str() );
			}

			// Go further down the path
			if( k1 != rootKey_m && k1 != startKey_m )
				RegCloseKey( k1 );

			k1 = k2;
		}
	} while( index != String::npos );

	// Done
	return k1;
}
//-------------------------------------------------------------
void ConfigurationWin32::killSection (
		HKEY key,
		String const &subkey
	)
{
	// No need to lock, as this is an internal helper fun

	HKEY sk;
	DWORD length = 1024;
	char buffer[1024];

	// Get the subkey handle to peek into it
	if( RegOpenKeyEx( key, subkey.c_str(), 0, (KEY_READ | KEY_WRITE), &sk ) != ERROR_SUCCESS ) {
		// Key does not exist; exit
		return;
	}

	// Recursively delete all subkeys of the subkey
	//
	while( RegEnumKeyEx( sk, 0, buffer, &length, NULL, NULL, NULL, NULL ) != ERROR_NO_MORE_ITEMS ) {
		// Delete the subkey
		killSection( sk, buffer );
		length = 1024;
	}
	RegCloseKey( sk );

	// Now remove the key itself
	//
	RegDeleteKey( key, subkey.c_str() );
}





// End of namespace
}