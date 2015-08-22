#include "brklogger.hpp"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN


#include <time.h>

namespace BrkLib {
	
	CRITICAL_SECTION * getMutex (
	)
	{	
		static CRITICAL_SECTION mutex_s;

#if(_WIN32_WINNT >= 0x0403)
	static DWORD scount = 4000 | 0x80000000;
	static bool initialized_s = ( InitializeCriticalSectionAndSpinCount( &mutex_s, scount ), true );
#else
	static bool initialized_s = ( InitializeCriticalSection( &mutex_s ), true );
#endif

		return &mutex_s;
	}

#ifdef _DEBUG 
const int BrkLog::logLevelDefault_scm = MSG_DEBUG;
#else
const int BrkLog::logLevelDefault_scm = MSG_INFO;
#endif
//-------------------------------------------------------------
char* BrkLog::debugStreamName_scm = NULL;
//-------------------------------------------------------------
ostream *BrkLog::debugStream_scm = NULL;
//-------------------------------------------------------------


//======================================
// Methods
//======================================
void BrkLog::log (
		char const* module,
		int level,
		const char *caller,
		const char *message,
		const char *file,
		int line
	)
{  
	// Acquire lock
	EnterCriticalSection( getMutex() );

	try {
		// Act depending on level
		//
		if( level <= getLogLevel() ) {  
			// Log to stream
			//
						
			debugOutput() << "[ " << module << " ] [" << level << "] - " << CppUtils::getAscTime() << " - " << CppUtils::getTID() <<
				" - " << caller << " - " << (message!=NULL?message:"") << " @ line " << line << 
				" in file " << file << endl;
			
		}
		
	}
	catch( ... ) {
		LeaveCriticalSection( getMutex() );
		throw;
	}

	// Release lock
	LeaveCriticalSection( getMutex() );
}

//-------------------------------------------------------------
int BrkLog::getLogLevel (	 int const newLevel )
{
	// The default level
	static int logLevel_s = logLevelDefault_scm;

	// enforce change level
	if (newLevel >= 0) {
		if (newLevel <= BRK_MSG_DEBUG_DETAIL && newLevel >= BRK_MSG_FATAL) {
			 EnterCriticalSection( getMutex() );;

			 logLevel_s = newLevel;

			 LeaveCriticalSection( getMutex() );
		}
	}

	return logLevel_s;
}

void BrkLog::setLogLevel(int const level )
{
	

	int foo = getLogLevel( level );

	
}
//-------------------------------------------------------------
void BrkLog::discardDebugOutput (
	)
{
	if( debugStream_scm && debugStream_scm != &cerr ) {
		// Get the stream
		ofstream *fstream = dynamic_cast<ofstream *>( debugStream_scm );

		// Is it a file stream?
		if( fstream ) {
			// Yes; close and discard
			fstream->close();
			delete debugStream_scm;

			remove( debugStreamName_scm );

			// free buffer
			delete debugStreamName_scm;

			// Remember no stream
			debugStreamName_scm = _strdup("XXX");
			debugStream_scm = NULL;
		}
	}
}
//-------------------------------------------------------------
ostream & BrkLog::debugOutput (
	)
{ 
	// Need to create stream?
	if( debugStream_scm == NULL ) {
		// Already closed?
		if( debugStreamName_scm != NULL && strcmp(debugStreamName_scm,"XXX")==0 )
			// Yes; return cerr
			return cerr;


		// Not opened yet -> open log file

		// Modify TMP for the call of _tempnam to point to $VPI_INSTALL
		char* tmp = getenv( "TMP" );
		CppUtils::String oldTmp = tmp ? tmp : "";
		tmp = getenv( "HT_SERVER_DIR" );
		CppUtils::String tmpP = CppUtils::String( tmp );
		
		if (tmpP[tmpP.length()-1] != PATH_SEPARATOR_CHAR)
			 tmpP += PATH_SEPARATOR_CHAR;

		CppUtils::String logPath = tmp ? (tmpP + "logs") : "";
		CppUtils::String newTmp( "TMP=" );
		newTmp += ( logPath.empty() ) == false ? logPath : ( ( oldTmp.empty() == false) ? oldTmp : "" );
		putenv( newTmp.c_str() );


		// Construct file name; we must do this in a loop as
		// the OS (at least bloody Windoze) sometimes does not
		// properly generate the temp file name when another process
		// attempts to generate the same name concurrently. It can
		// happen that both processes end up with the same name.
		char *logFileName = NULL;

		do {
			
			// Construct temp name
			struct tm *now; long lt;
			time( &lt );
			now = localtime( &lt );
			static char buf[1024];
			sprintf( buf, "%s_broker_log-%4d%02d%02d-%02d%02d%02d_", CppUtils::getProcessName().c_str(), now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
			char *tmpFileName = _tempnam( NULL, buf );
			logFileName = new char[ strlen( tmpFileName ) + 5 ];
			strcpy( logFileName, tmpFileName );
			strcat( logFileName, ".log" );
			free( tmpFileName );

			// Check if the file really does not exist
			FILE *f = fopen( logFileName, "r" );
			if( f )
				// File exists! Close handle and repeat loop
				fclose( f );
			else
				// File does not exist; can break
				break;
		} while( true );

		// Revert to old TMP
		CppUtils::String newTmp2( "TMP=" );
		newTmp2 += ( oldTmp.empty() == false ) ? oldTmp : "";
		putenv( newTmp2.c_str() );


		// Open stream
		debugStream_scm = new ofstream( logFileName, ios::out );
		

		// Check for success
		if( !debugStream_scm->good() ) {
			// Delete debug stream and redirect to cerr (which is /dev/null on Windoze)
			delete debugStream_scm;
			debugStream_scm = &cerr;

			// Remember name
			debugStreamName_scm = _strdup("<stderr>");
		}
		else {
			// Could open file
			//

			// Remember name
			
			debugStreamName_scm = _strdup(logFileName);
		
			
		} 

		delete logFileName;
		
		
	}

	// Return stream
	return *debugStream_scm;
}

CppUtils::String BrkLog::getLogFileName(
)
{
	return (debugStreamName_scm ? debugStreamName_scm : "dummy" ) ;
}

}; // end of ns