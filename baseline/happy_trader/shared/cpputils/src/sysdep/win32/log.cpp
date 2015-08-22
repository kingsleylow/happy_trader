#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T 1
#endif

#include "../../log.hpp"

#include "../../thread.hpp"
//#include "../../stringtable.hpp"
#include "msgboxtout.hpp"
#include "console.hpp"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN


#include <time.h>



namespace CppUtils {

	//char* STRLEVEL[] = {"FATAL     ", "ERROR     ", "WARN      ", "INFO      ", "DEBUG     ", "DBG DETAIL"};


int Log::foo_scm  = Log::boot();

//-------------------------------------------------------------
// Mutex at file scope for locking
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




//-------------------------------------------------------------
/** Function to abort the program after 30 secs. */
int dieIn30Secs (void *)
{
	sleep( 30.0 );
	abort();

	return 0;
}




//=============================================================
// Class Log
//=============================================================


//-------------------------------------------------------------
#ifdef _DEBUG 
const int Log::logLevelDefault_scm = MSG_DEBUG;
#else
const int Log::logLevelDefault_scm = MSG_INFO;
#endif
//-------------------------------------------------------------
char* Log::debugStreamName_scm = NULL;
//-------------------------------------------------------------
ostream *Log::debugStream_scm = NULL;
//-------------------------------------------------------------
bool errOut_g = false;
// ------------------------------------------------------------


//======================================
// Methods
//======================================
void Log::log (
		int level,
		const char *caller,
		const char *message,
		const char *file,
		int line
	)
{  
	// Acquire lock
	lock();

	try {
		// Act depending on level
		//
		if( level <= getLogLevel() ) {  
			// Log to stream
			//
						
			debugOutput() << level << " - " << getAscTime() << " - " << getTID() <<
				" - " << caller << " - " << (message!=NULL?message:"") << " @ line " << line << 
				" in file " << file << endl;
			
			
			// duplicate
			if (errOut_g) {
				
				
				cout  << level << " - " << getAscTime() << " - " << getTID() <<
					" - " << caller << " - " << (message!=NULL?message:"") << " @ line " << line << 
					" in file " << file << endl;
				
				
			}

			
		}


		// Special handling for certain log events
		//
		if( level == MSG_FATAL ) {
			// Localize
			//
			//String msg = GET_LANG_STRING( message );
			String msg =  (message!=NULL?message:"");


			// Write to Windows application event log
			//
			HANDLE hLog = OpenEventLog( NULL, "Application" );
			if( hLog != NULL ) {
				HANDLE hSource = RegisterEventSource( NULL, "HappyTraderServer" );

				if( hSource != NULL ) {
				  char buf[4096]; 
					ostrstream sstr( buf, 4095 ); 
					sstr << endl << caller << " : " << msg << " @ " << line << " in file " << file << endl << ends;

					char const **strs = new char const *[1];
					strs[ 0 ] = sstr.str();

					ReportEvent( 
						hSource,
						EVENTLOG_ERROR_TYPE,
						0,
						0,
						NULL,
						1,
						0,
						strs,
						NULL
					);

					delete strs;
					sstr.freeze( false );

					DeregisterEventSource( hSource );
				}

				CloseEventLog( hLog );				
			}


			// Start self-destruction
			//
			new Thread( &dieIn30Secs, NULL );

			
			// Display error
			//
			//MessageBox( NULL, msg.c_str(), "Fatal Error - Aborting in 30 seconds", MB_ICONERROR | MB_SERVICE_NOTIFICATION | MB_APPLMODAL | MB_OK );

			// going to be auto close window error
			HMODULE hUser32 = LoadLibrary("user32.dll");

			if (hUser32)
			{
					int iRet = 0;
					UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_SYSTEMMODAL|MB_ICONINFORMATION;
					
					// window for 30 seconds
					iRet = MessageBoxTimeout(NULL, msg.c_str(), "Fatal Error - Aborting in 30 seconds", uiFlags, 0, 30000);
					
					FreeLibrary(hUser32);
			}

		}
	}
	catch( ... ) {
		unlock();
		throw;
	}

	// Release lock
	unlock();
}
//-------------------------------------------------------------
void Log::lock (
	)
{
	// Acquire lock
	EnterCriticalSection(getMutex());
}
//-------------------------------------------------------------
void Log::unlock (
	)
{
	// Acquire lock
	LeaveCriticalSection( getMutex() );
}
//-------------------------------------------------------------
int Log::getLogLevel (int const newLevel)
{
	// The default level
  // static cause neeed fast access 
	static int logLevel_s = logLevelDefault_scm;

	if (newLevel >= 0) {
		
		if (newLevel >= MSG_FATAL && newLevel <= MSG_DEBUG_DETAIL) {
			 lock();
			 logLevel_s = newLevel;
			 unlock();  
		}
	}

	

	return logLevel_s;
}
//-------------------------------------------------------------
int Log::boot()
{
	// assign default log level

	int logLevel_s = logLevelDefault_scm;

	String logLevelStr;
	if (getEnvironmentVar( String("%") + String(LOG_LEVEL_ENVIROMENT_ENTRY) + "%", logLevelStr )) {
		if (logLevelStr.length() > 0) {
			sscanf( logLevelStr.c_str(), "%d", &logLevel_s );
		}
	}


	setLogLevel(logLevel_s);

	return 42;
}

// ------------------------------------------------------------

void Log::setLogLevel(int const level )
{
	
	int foo = getLogLevel( level );

}


//-------------------------------------------------------------
void Log::discardDebugOutput (
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
ostream & Log::debugOutput ()
{ 
	// Need to create stream?
	if( debugStream_scm == NULL ) {
		// Already closed?
		if( debugStreamName_scm != NULL && strcmp(debugStreamName_scm,"XXX")==0 )  
			// Yes; return cerr
			return cerr;



		// Not opened yet -> open log file

		// Modify TMP for the call of _tempnam to point to 
		char* tmp = getenv( "TMP" );
		String oldTmp = tmp ? tmp : "";
		tmp = getenv( "HT_SERVER_DIR" );
		String tmpP = String( tmp );
		
		if (tmpP[tmpP.length()-1] != PATH_SEPARATOR_CHAR)
			 tmpP += PATH_SEPARATOR_CHAR;

		String logPath = tmp ? (tmpP + "logs") : "";
		String newTmp( "TMP=" );
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
			
#ifndef _SINGLE_LOG_FILE
			// as usual

			sprintf( buf, "%s-%4d%02d%02d-%02d%02d%02d_", getProcessName().c_str(), now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
#else
			// single file creating
			sprintf( buf, "%s-%4d%02d%02d_", getProcessName().c_str(), now->tm_year+1900, now->tm_mon+1, now->tm_mday);
			#pragma message("SINGLE LOG FILE USED IN THE PROJECT")
#endif			
			
#ifndef _SINGLE_LOG_FILE

			char *tmpFileName = _tempnam( logPath.c_str(), buf );
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
#else
			String fullnewPathLog;
			fullnewPathLog.append(logPath).append("\\").append(buf).append(".log");

			logFileName = new char[ fullnewPathLog.size() + 1 ];
			strcpy( logFileName, fullnewPathLog.c_str() );

			break;
#endif

		} while( true );

		// Revert to old TMP
		String newTmp2( "TMP=" );
		newTmp2 += ( oldTmp.empty() == false ) ? oldTmp : "";
		putenv( newTmp2.c_str() );


		// Open stream
#ifndef _SINGLE_LOG_FILE
		debugStream_scm = new ofstream( logFileName, ios::out );
#else
		debugStream_scm = new ofstream( logFileName, ios::out | ios::app );
#endif	

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
			
			debugStreamName_scm = new char[ strlen( logFileName ) + 1 ];
			strcpy(debugStreamName_scm,logFileName);
			
		} 

		delete [] logFileName;
		
		
	}

	// Return stream
	return *debugStream_scm;
}

bool Log::setupLogDetails(char const* defaultPath, char const* logPrefix )
{
	if (defaultPath == NULL) 
		return false;
	

	// initialize if necessary
	if( debugStream_scm == NULL ) {
		
		if( debugStreamName_scm != NULL )  {
			
			free( debugStreamName_scm );
			debugStreamName_scm  = NULL;
		}


		// Not opened yet -> open log file

		// Modify TMP for the call of _tempnam to point to 
		char* tmp = getenv( "TMP" );
		String oldTmp = tmp ? tmp : "";

		// pass default path here
		String tmpP = defaultPath;
		
		if (tmpP[tmpP.length()-1] != PATH_SEPARATOR_CHAR)
			 tmpP += PATH_SEPARATOR_CHAR;

		String logPath = tmpP + "logs";
		
		String newTmp( "TMP=" );
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
#ifndef _SINGLE_LOG_FILE
			// as usual

			sprintf( buf, "%s%s-%4d%02d%02d-%02d%02d%02d_", logPrefix ? logPrefix: "", getProcessName().c_str(), now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
#else
			// single file creating
			sprintf( buf, "%s%s-%4d%02d%02d_", logPrefix ? logPrefix: "", getProcessName().c_str(), now->tm_year+1900, now->tm_mon+1, now->tm_mday);
			#pragma message("SINGLE LOG FILE USED IN THE PROJECT")
#endif
		 

#ifndef _SINGLE_LOG_FILE

			char *tmpFileName = _tempnam( logPath.c_str(), buf );
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
#else
			String fullnewPathLog;
			fullnewPathLog.append(logPath).append("\\").append(buf).append(".log");

			logFileName = new char[ fullnewPathLog.size() + 1 ];
			strcpy( logFileName, fullnewPathLog.c_str() );

			break;
#endif

		} while( true );


		// Revert to old TMP
		String newTmp2( "TMP=" );
		newTmp2 += ( oldTmp.empty() == false ) ? oldTmp : "";
		putenv( newTmp2.c_str() );


		// Open stream
#ifndef _SINGLE_LOG_FILE
		debugStream_scm = new ofstream( logFileName, ios::out );
#else
		debugStream_scm = new ofstream( logFileName, ios::out | ios::app );
#endif

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
		
		return true;
		
	}

	return false;
}

void Log::setErrOut(
	)
{
	lock();
	errOut_g = true;
	//Console& console = Console::instance();
	//console.init();
	unlock();
}

String Log::getLogFileName(
)
{
	return (debugStreamName_scm ? debugStreamName_scm : "dummy" ) ;
}

// End of namespace
}