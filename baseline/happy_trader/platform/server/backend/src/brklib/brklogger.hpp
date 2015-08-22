#ifndef _BRKLIB_BRKLOGGER_INCLUDED
#define _BRKLIB_BRKLOGGER_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brokersession.hpp"
#include "brkconnect.hpp"

#include <strstream>


namespace BrkLib {

#define BRK_MSG_FATAL 0
#define BRK_MSG_ERROR 1
#define BRK_MSG_WARN 2
#define BRK_MSG_INFO 3
#define BRK_MSG_DEBUG 4
#define BRK_MSG_DEBUG_DETAIL 5


#define BRK_LOG(M,L,O,X) { \
		if( L <= BrkLib::BrkLog::getLogLevel() ) { \
			ostrstream sstr; sstr << X << ends; \
			BrkLib::BrkLog::log( M,L, O, sstr.str(), __FILE__, __LINE__ ); \
			sstr.freeze( false ); \
		} \
	}


class BRKLIB_EXP BrkLog {
		
public:

	static void log (
		char const* module,
		int level,
		const char *caller,
		const char *message,
		const char *file,
		int line
	);

	static int getLogLevel (		int const newLevel = -1	);

	static void setLogLevel( 		int const level	);

	static void discardDebugOutput (	);

	static ostream &debugOutput(	);

	
	static CppUtils::String getLogFileName(
	);

private:

	static const int logLevelDefault_scm;

	static char* debugStreamName_scm;

	static ostream *debugStream_scm;

};

}; // end of ns

#endif