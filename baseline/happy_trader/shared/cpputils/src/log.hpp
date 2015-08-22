
/** 

		Header file declaring logging macros and class Debug.

*/


#ifndef _CPPUTILS_LOG_INCLUDED
#define _CPPUTILS_LOG_INCLUDED


#include "cpputilsdefs.hpp"

#include <strstream>




//=============================================================
// Debug macros
//

#define MSG_FATAL 0
#define MSG_ERROR 1
#define MSG_WARN 2
#define MSG_INFO 3
#define MSG_DEBUG 4
#define MSG_DEBUG_DETAIL 5

#define LOG_LEVEL_ENVIROMENT_ENTRY "HT_DEBUG_LEVEL"

#ifndef NO_LOGGING

#undef assert
#define assert( X ) { if( !(X) ) { CppUtils::Log::log( MSG_ERROR, "ASSERTION FAILED", #X, __FILE__, __LINE__ ); } }


#define LOG(L,O,X) { \
		if( L <= CppUtils::Log::getLogLevel() ) { \
			ostrstream sstr; sstr << X << ends; \
			CppUtils::Log::log( L, O, sstr.str(), __FILE__, __LINE__ ); \
			sstr.freeze( false ); \
		} \
	}

#define LOG_FORCE(O,X) { \
		ostrstream sstr; sstr << X << ends; \
		CppUtils::Log::log( -1, O, sstr.str(), __FILE__, __LINE__ ); \
		sstr.freeze( false ); \
	}

#else // NO_LOGGING

#define LOG(L,O,X)

#endif // NO_LOGGING



namespace CppUtils {



//=============================================================
// Class Log
//=============================================================
/** Class Log performs logging. There is a global static 
		instance of this class used to hold this data.

*/
	class CPPUTILS_EXP Log {

public:

	//======================================
	// Functions

	static void log (
		int level,
		const char *caller,
		const char *message,
		const char *file,
		int line
	);

	static int getLogLevel (int const newLevel = -1);

	static void setLogLevel( int const level );

	static void discardDebugOutput ();

	static ostream &debugOutput();

	// this may be called explicitey to setup dir/level
	static bool setupLogDetails(char const* defaultPath = NULL, char const* logPrefix = NULL);

	static void lock ();

	static void unlock ();
	
	static void setErrOut();

	static String getLogFileName();

	static int boot();

private:


	//======================================
	// Variables


	/** The default log level used when no value is
			set in the environment; value is \b 2 */
	static const int logLevelDefault_scm;

	static char* debugStreamName_scm;

	static ostream *debugStream_scm;

	static int foo_scm;

};



// End of namespace
}



#endif // _CPPUTILS_LOG_INCLUDED
