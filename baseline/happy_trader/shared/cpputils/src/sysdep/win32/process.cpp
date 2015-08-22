

#include "../../process.hpp"

#include "../../except.hpp"
//#include "../../stringtable.hpp"

#include "../../uid.hpp"



namespace CppUtils {


#define PROCESS_STR "PROCESS"


//=============================================================
// Class Process
//=============================================================
Process::Process(
		String const &image,
		String const &commandline,
		String const &currentDir,
		bool redirect,
		bool hidden,
		bool detached
	) :
		processHandle_m( INVALID_HANDLE_VALUE ),
		sout_m( INVALID_HANDLE_VALUE ),
		serr_m( INVALID_HANDLE_VALUE ),
		soutName_m(),
		serrName_m(),
		detached_m(detached)
{
	// Create temp files when redirecting
	//
	if( redirect ) {
		CppUtils::Uid uid;
		serrName_m = CppUtils::getTempPath() + CppUtils::pathDelimiter() + Uid::generateUid().toString();
		soutName_m = CppUtils::getTempPath() + CppUtils::pathDelimiter() + Uid::generateUid().toString();

		SECURITY_ATTRIBUTES secAtts;
		secAtts.nLength = sizeof( SECURITY_ATTRIBUTES );
		secAtts.lpSecurityDescriptor = NULL;
		secAtts.bInheritHandle = true;

		sout_m = CreateFile( soutName_m.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, &secAtts, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL );
		if( sout_m == INVALID_HANDLE_VALUE )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingTempFileForProcess", getOSError().c_str() );

		serr_m = CreateFile( serrName_m.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, &secAtts, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL );
		if( serr_m == INVALID_HANDLE_VALUE )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingTempFileForProcess", getOSError().c_str() );
	}


	// Prepare process creation
	//
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si,sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);

	if( redirect ) {
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = sout_m;
		si.hStdError  = serr_m;

	
	}

	if( hidden ) {
		si.wShowWindow = SW_HIDE;
		si.dwFlags |= STARTF_USESHOWWINDOW;
	}


	// Create
	//
	String fullImage = expandPathName( image );
	String fullCurrentDir = expandPathName( currentDir );

	char *cline = strdup( (String("\"") + fullImage + "\" " + commandline).c_str() );
  //if( !CreateProcess( NULL, cline, NULL, NULL, TRUE, 0, NULL, fullCurrentDir.empty() ? NULL : fullCurrentDir.c_str(), &si, &pi) ) {
	if( !CreateProcess( NULL, cline, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, fullCurrentDir.empty() ? NULL : fullCurrentDir.c_str(), &si, &pi) ) {
		free( cline );
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_CreatingProcess", getOSError().c_str() );
	}
	free( cline );


	// Close thread handle and keep process handle
	//
	CloseHandle( pi.hThread );
	processHandle_m = pi.hProcess;
}
//-------------------------------------------------------------
Process::~Process(
	)
{
	// do nothing if detached
	if (detached_m)
		return;

	if( processHandle_m != NULL ) {
		// Release the handle
		if( CloseHandle( processHandle_m ) == 0 )
			// A problem occured
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_DestroyingProcess", getOSError().c_str() );

		LOG( MSG_DEBUG, PROCESS_STR, "Destroyed process with handle " << processHandle_m );
	}

	if( sout_m != INVALID_HANDLE_VALUE ) {
		CloseHandle( sout_m );
		DeleteFile( soutName_m.c_str() );
	}

	if( serr_m != INVALID_HANDLE_VALUE ) {
		CloseHandle( serr_m );
		DeleteFile( serrName_m.c_str() );
	}
}
//======================================
// Methods
//======================================
ProcessHandle const & Process::getHandle (
	) const
{
	return processHandle_m;
}

int Process::getPid() const
{
	return ::GetProcessId(processHandle_m);
}

//-------------------------------------------------------------
int Process::getResult (
	) const
{
	if( !hasTerminated() )
		// Process is still running
		return -1;
	else {
		// Get the exit code
		DWORD ec;
		if( GetExitCodeProcess( processHandle_m, &ec ) == 0 )
			// A problem occured
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_GettingProcessExitCode", getOSError().c_str() );

		// Return the exit code
		return ec;
	}
}
//-------------------------------------------------------------
bool Process::hasTerminated (
	) const
{
	DWORD res = WaitForSingleObject( processHandle_m, 0 );

	if( res == WAIT_TIMEOUT )
		// Process busy
		return false;
	else if( res == WAIT_FAILED )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForProcess", getOSError().c_str() )
	else
		// Process finished
		return true;
}
//-------------------------------------------------------------
void Process::killProcess (
	)
{
	// Ignore if terminated
	if( hasTerminated() )
		return;

	LOG( MSG_INFO, PROCESS_STR, "Killing process with handle " << processHandle_m );

	if( TerminateProcess( processHandle_m, -1 ) == 0 )
		// A problem occured
		THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_KillingProcess", getOSError().c_str() );
}
//-------------------------------------------------------------
bool Process::waitForProcess (
		double secs
	) const
{
	if( secs < 0 ) {
		if( WaitForSingleObject( processHandle_m, INFINITE ) == WAIT_FAILED )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForProcess", getOSError().c_str() );

		return true;
	}
	else {
		DWORD res = WaitForSingleObject( processHandle_m, (DWORD) (secs*1000) );
		if( res == WAIT_FAILED )
			THROW( ResourceFailure, "util_exc_OsError", "util_ctx_Threading_WaitingForProcess", getOSError().c_str() );

		return !(res == WAIT_TIMEOUT);
	}
}
//-------------------------------------------------------------
void Process::getStdOut (
		String &str
	) const
{
	if( sout_m == INVALID_HANDLE_VALUE )
		str = "";
	else {
		FlushFileBuffers( sout_m );

		FILE *f = fopen( soutName_m.c_str(), "rt" );

		DWORD sz = GetFileSize( sout_m, NULL );
		str.resize( sz + 1 );
		fread( &str[ 0 ], sizeof( Char ), sz, f );

		fclose( f );
	}
}
//-------------------------------------------------------------
void Process::getStdErr (
		String &str
	) const
{
	if( serr_m == INVALID_HANDLE_VALUE )
		str = "";
	else {
		FlushFileBuffers( serr_m );

		FILE *f = fopen( serrName_m.c_str(), "rt" );

		DWORD sz = GetFileSize( serr_m, NULL );
		str.resize( sz + 1 );
		fread( &str[ 0 ], sizeof( Char ), sz, f );

		fclose( f );
	}
}






// End of namespace
}
