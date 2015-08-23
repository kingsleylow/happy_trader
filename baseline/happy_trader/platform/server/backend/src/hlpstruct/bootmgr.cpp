#include "bootmgr.hpp"
#include "xmlwrap.hpp"
#include "stringtable.hpp"

namespace HlpStruct {




// =======================================================
// BootManager
//
const int BootManager::foo_scm = BootManager::boot( );
//-------------------------------------------------------------
int BootManager::boot ()
{
	static CppUtils::Mutex mutex_s;
	static bool initialized_s = false;

	LOCK_FOR_SCOPE( mutex_s );

	if( !initialized_s ) {
		// init XML here
		initXML();

		// Register a string table instance as the first thing
		// since we need it to get localized error messages.
		CppUtils::RootObjectManager::singleton().registerRootObject( HlpStruct::StringTable::getRootObjectName(), new HlpStruct::StringTable() );

			
	}
	// Done
	return 42;
};



// End of namespace
};
