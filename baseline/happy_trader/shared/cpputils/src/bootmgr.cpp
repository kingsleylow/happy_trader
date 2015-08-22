
/** \file    

		File implementing BootManager specialization for package
		CppUtils.

		
*/

 
#include "bootmgr.hpp" 

#include "log.hpp"
#include "config.hpp"
#include "servicethread.hpp"
#include "except.hpp"





namespace CppUtils {



// =======================================================
// BootManager
//
const int BootManager::foo_scm = BootManager::boot( false );
//-------------------------------------------------------------
int BootManager::boot (
		bool startServiceThread
	)
{
	static Mutex mutex_s;
	static bool initialized_s = false;

	LOCK_FOR_SCOPE( mutex_s );

	if( !initialized_s ) {

		
		// Now check if $HT_SERVER_DIR is set
		//
		String str;
		if( !getEnvironmentVar( "%HT_SERVER_DIR%", str ) || str.empty() || !directoryExists( str ) ) {
			// Set it 
			//

			// REVISIT: Factor Windows dependencies out into sysdep
			//

			// Get process dir
			char buffer[1024];
			int len = GetModuleFileName( NULL, buffer, 1023 );

			// Append \0
			buffer[ len ] = 0;

			// Cut off filename
			char *p = strrchr( buffer, '\\' );
			if( p == 0 ) {
				LOG( MSG_FATAL, "HT Application", "The environment variable HT_SERVER_DIR is not set or points to an invalid path. The application failed to retrieve the executable's path." ); 
			}

			// Set env; go up one dir!
			*p = 0;
			str = "HT_SERVER_DIR=";
			str += buffer;
			str += "\\..\\";

			_putenv( str.c_str() );

			// Log
			LOG( MSG_WARN, "HT Application", "The environment variable HT_SERVER_DIR is not set or points to an invalid path. It has been set to \"" << str << "\"." ); 
		}


		// Now check if $TMP is set
		//
		if( !getEnvironmentVar( "%TMP%", str ) || str.empty() || !directoryExists( str ) ) {
			// Set
			//

			// REVISIT: Factor Windows dependencies out into sysdep
			//

			// Let OS decide
			char buffer[ 1024 ];
			GetTempPath( 1023, buffer );

			// Set env
			str = "TMP=";
			str += buffer;
			_putenv( str.c_str() );

			// Log
			LOG( MSG_WARN, "Happy Trader Application", "The environment variable TMP is not set or points to an invalid path. It has been set to \"" << str << "\"." ); 
		}



		// Get the root object mgr
		//
		RootObjectManager &rootObjMgr = RootObjectManager::singleton();

		
		// no config objects
		/*
		// Initialize the configuration objects
		Configuration *userConfig = getConfiguration( Configuration::CT_User );
		Configuration *systemConfig = getConfiguration( Configuration::CT_System );

		
		// Register the configuration objects
		//rootObjMgr.registerRootObject( Configuration::getRootObjectName_User(), userConfig );
		//rootObjMgr.registerRootObject( Configuration::getRootObjectName_System(), systemConfig );
		*/

		// Done
		initialized_s = true;
	}


	// Make sure the service thread is started when needed
	//
	if( startServiceThread && getRootObject<ServiceThread>() == NULL )
		// Register the service thread
		RootObjectManager::singleton().registerRootObject( ServiceThread::getRootObjectName(), new ServiceThread() );
	
	// Done
	return 42;
}




// End of namespace
};

