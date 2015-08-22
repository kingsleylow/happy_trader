
#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T 1
#endif


#include "../sysdep.hpp"
#include "../../chunk.hpp"
#include "../../misc_ht.hpp"
#include "../../mutex.hpp"
#include "../../except.hpp"
#include "../../assertdef.hpp"
#include "Shellapi.h"



// Pull in special Windows stuff
//
//#include <rpc.h>
#include <process.h>
#include <sys/timeb.h>
#include <io.h>

#include <nb30.h>
#include <wincon.h>
#include <time.h>

#include <assert.h>
#include <eh.h>

#include <direct.h>

#include <mapi.h>

#include <winsock2.h>

#include "errno.h"

#include <Oleauto.h>
#include "Windows.h"
#include "psapi.h"




namespace CppUtils {



	//=============================================================
	// Error handling
	//=============================================================
	String getOSError (
		)
	{
		char *msgBuf;
		String errorMsg;

		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msgBuf, 0, NULL );

		// Get rid of trailing \n
		if( *msgBuf )
			msgBuf[ strlen(msgBuf)-1 ] = 0;

		errorMsg = msgBuf;

		// Free the buffer.
		LocalFree( msgBuf );

		// Done
		return errorMsg;
	}
	//-------------------------------------------------------------
	void displayError (
		String const &msg
		)
	{
		MessageBox( NULL, msg.c_str(), "Happy Trader - Fatal Error", MB_OK | MB_ICONERROR );
	}



	//=============================================================
	// Core functions
	//=============================================================
	//-------------------------------------------------------------
	void abort (
		)
	{
		_exit( -1 );
	}
	//-------------------------------------------------------------
	unsigned long int getPID (
		)
	{
		return _getpid();
	}
	//-------------------------------------------------------------
	String getProcessName (
		)
	{
		const char *p = GetCommandLine();

		// Find closing " if the string start with a quote
		const char *e = p;
		if( e && *e == '\"' )
			e = strchr( e + 1, '\"' );
		else {
			// Not enclosed by quotes; get first white space
			while( e && *e && !isspace( *e ) )
				++e;
		}

		// Find last path delim backward
		const char *d = e;
		while( d && d > p && *d != pathDelimiter() )
			--d;

		if( d && *d == pathDelimiter() )
			++d;

		// Find last dot
		const char *t = e;
		while( t && t > d && *t != '.' )
			--t;

		// Dot found?
		if( t == d )
			t = e;

		// Copy process name
		char name[256];
		memset( name, 0, 256 );
		if( d && t )
			strncpy( name, d, min( t - d, 256 ) );
		else
			strcpy( name, "<unknown>" );

		return name;
	}
	//-------------------------------------------------------------
	unsigned long int getTID (
		)
	{
		return GetCurrentThreadId();
	}
	//-------------------------------------------------------------
	String getAscTime (
		double t 
		)
	{
		struct tm *newtime;
		int milli;

		if( t <= 0 ) {

			time_t aclock;
			time( &aclock );
			newtime = localtime( &aclock );
			struct _timeb now; _ftime( &now ); 
			milli = now.millitm;

		}
		else {
			time_t aclock = (int) t;
			newtime = localtime( &aclock );
			milli = (int) (( t - (int) t ) * 1000.0);
		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min, newtime->tm_sec, milli );

		return buffer;
	}

	//-------------------------------------------------------------
	String getAscTime2 (
		double const& t
		)
	{
		// 2007.07.10 12:00
		struct tm *newtime;
		// int milli;

		if( t <= 0 ) {
			/*
			time_t aclock;
			time( &aclock );
			newtime = localtime( &aclock );
			struct _timeb now; _ftime( &now ); milli = now.millitm;
			*/
			return "";
		}
		else {
			time_t aclock = (int) t;
			newtime = localtime( &aclock );
			//milli = (int) (( t - (int) t ) * 1000.0);
		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d.%02d.%02d %02d:%02d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min);

		return buffer;
	}
	// ------------------------------------------------------------

	CPPUTILS_EXP String getAscTimeAsFileName (
		double const t
		)
	{
		struct tm *newtime;

		if( t <= 0 ) {
			time_t aclock;
			time( &aclock );
			newtime = localtime( &aclock );
			struct _timeb now; _ftime( &now ); 


		}
		else {
			time_t aclock = (int) t;
			newtime = localtime( &aclock );

		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d-%02d-%02d %02d_%02d_%02d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

		return buffer;
	}


	// ------------------------------------------------------------
	String getGmtTime (
		double const& t
		)
	{
		// 2007.07.10 12:00
		struct tm *newtime;
		int milli;

		if( t <= 0 ) {
			/*
			time_t aclock;
			time( &aclock );
			newtime = gmtime( &aclock );
			struct _timeb now; _ftime( &now ); milli = now.millitm;
			*/
			return "";
		}
		else {
			time_t aclock = (int) t;
			newtime = gmtime( &aclock );
			milli = (int) (( t - (int) t ) * 1000.0);
		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min, newtime->tm_sec, milli );

		return buffer;	

	}


	//-------------------------------------------------------------
	String getGmtTime2 (
		double const& t
		)
	{
		// 2007.07.10 12:00
		struct tm *newtime;
		int milli;

		if( t <= 0 ) {
			/*
			time_t aclock;
			time( &aclock );
			newtime = gmtime( &aclock );
			struct _timeb now; _ftime( &now ); milli = now.millitm;
			*/
			return "";
		}
		else {
			time_t aclock = (int) t;
			newtime = gmtime( &aclock );
			milli = (int) (( t - (int) t ) * 1000.0);
		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d.%02d.%02d %02d:%02d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min);

		return buffer;	
	}

	//-------------------------------------------------------------
	String getGmtTime3 (
		double const& t
		)
	{
		// 2007.07.10 12:00
		struct tm *newtime;
		int milli;

		if( t <= 0 ) {
			/*
			time_t aclock;
			time( &aclock );
			newtime = gmtime( &aclock );
			struct _timeb now; _ftime( &now ); milli = now.millitm;
			*/
			return "";
		}
		else {
			time_t aclock = (int) t;
			newtime = gmtime( &aclock );
			milli = (int) (( t - (int) t ) * 1000.0);
		}


		// Print ISO 8601 string
		char buffer[256];
		sprintf( buffer, "%04d-%02d-%02d %02d:%02d",
			newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
			newtime->tm_hour, newtime->tm_min);

		return buffer;	
	}

	//-------------------------------------------------------------

	void crackGmtTime(
		double const& t,
		tm& newtime
		)
	{
		memset(&newtime, '\0', sizeof(tm));

		if( t > 0 ) {
			time_t aclock = (int) t;
			tm* gmtval = gmtime( &aclock );
			memcpy(&newtime, gmtval, sizeof(tm));

		}
	}

	//-------------------------------------------------------------

	double roundToBeginningOfTheDay(double const t)
	{
		tm curtime;
		crackGmtTime(t, curtime);

		curtime.tm_hour = 0;
		curtime.tm_min = 0;
		curtime.tm_sec = 0; 

		// no day light saving time
		curtime.tm_isdst = 0;
		time_t result = mktime( &curtime ) - _timezone;
		return (double) result;


	}

	//-------------------------------------------------------------

	CPPUTILS_EXP double roundToMinutes(double const t)
	{
		tm curtime;
		crackGmtTime(t, curtime);

		curtime.tm_sec = 0; 
		curtime.tm_isdst = 0;
		time_t result = mktime( &curtime ) - _timezone;
		return (double) result;

	}



	//-------------------------------------------------------------


	String getSimpleAscTime (
		double t
		)
	{
		struct tm *newtime;

		HTASSERT( t > 0 );

		time_t aclock = (int) t;
		newtime = localtime( &aclock );


		// Print string
		char buffer[256];
		strftime( buffer, 256, "%d. %B %Y at %H:%M", newtime );

		return buffer;
	}
	//-------------------------------------------------------------

	double parseAscTime (
		String const &t
		)
	{
		struct tm time;
		unsigned int milli = 0;

		// Parse ISO 8601 string
		sscanf( t.c_str(), "%d-%d-%dT%d:%d:%d.%d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec, &milli );

		time.tm_year -= 1900;
		time.tm_mon -= 1;
		time.tm_isdst = -1;

		// Convert
		return ((double) mktime( &time )) + ((double) milli) / 1000.0;
	}

	//-------------------------------------------------------------

	double parseFormattedDateTime (
		String const &format, 
		String const &t,
		int tm_isdst
		)
	{
		if (format.size() <= 0)
			return -1;

		struct tm gtime;
		unsigned int milli = 0;

		memset(&gtime, '\0', sizeof(tm));


		/*
		Will use a simplified scheme
		M - month
		d - day
		Y - year as four digits
		y - year as 2 digits
		h - hour
		m - minute
		s - second
		l - millisecond
		\,/, , - observed as delimiters, actually all is delimiters

		for example 
		M/d/Y h:m:s
		*/

		static const char* MONTH_TOKEN	=					"M";
		static const char* DAY_TOKEN		=					"d";
		static const char* YEAR_TOKEN_LONG	=			"Y";
		static const char* YEAR_TOKEN_SHORT	=			"y";
		static const char* HOUR_TOKEN	=						"h";
		static const char* MINUTE_TOKEN	=					"m";
		static const char* SECOND_TOKEN	=					"s";
		static const char* MILLISECOND_TOKEN	=		"l";

		/*
		static const char* TOKENS[] = 
		{
		MONTH_TOKEN,
		DAY_TOKEN,
		YEAR_TOKEN_LONG,
		YEAR_TOKEN_SHORT,
		HOUR_TOKEN,
		MINUTE_TOKEN,
		SECOND_TOKEN,
		MILLISECOND_TOKEN,
		0
		};
		*/

		CppUtils::StringList format_tokens, data_tokens;

		//static CppUtils::String delimiters_format;
		/*
		static Mutex mutex;
		if (delimiters_format.size() ==0) {

		// all is delimiters excepting our special symbols
		LOCK_FOR_SCOPE(mutex);
		for(int i = 32; i <= 255; i++) {

		for(char** c = (char**)TOKENS; *c!=NULL; c++) {

		// check the first symbol of a token
		if (i == **c)
		// found
		break;

		}


		if (*c==NULL)
		// not found
		delimiters_format.append(1,i);
		}
		}
		*/

		// actual data tokens
		//static CppUtils::String delimiters_data;

		/*
		static Mutex mutex_data;

		if (delimiters_data.size()==0) {
		LOCK_FOR_SCOPE(mutex_data);
		for(int i = 32; i <= 255; i++) {
		if (i <= '9' && i >= '0') {
		continue;
		}

		delimiters_data.append(1,i);
		}

		}
		*/

		static CppUtils::String delimiters_global("\\/:, -.;");
		// this is format
		CppUtils::tokenize(format, format_tokens, delimiters_global );

		// this is data
		CppUtils::tokenize(t, data_tokens, delimiters_global );


		// iterating through format_tokens assuming the respective entries in data_tokens
		if (format_tokens.size() != data_tokens.size())
			THROW(CppUtils::OperationFailed, "exc_DateTimeMaskDoesNotMatchInputString", "ctx_ParseDateTime", t + " - " + format);


		for(int i = 0; i < format_tokens.size(); i++) {
			CppUtils::String const& token_i = format_tokens[ i ];

			int val_i = atoi(data_tokens[i].c_str());
			if (token_i.compare(MONTH_TOKEN) == 0) {
				gtime.tm_mon = val_i;
				gtime.tm_mon -= 1;
			}
			else if (token_i.compare(DAY_TOKEN) == 0) {
				gtime.tm_mday = val_i;
			}
			else if (token_i.compare(YEAR_TOKEN_LONG) == 0) {
				gtime.tm_year = val_i;
				gtime.tm_year -= 1900;
			}
			else if (token_i.compare(YEAR_TOKEN_SHORT) == 0) {
				// get current centure
				time_t aclock;
				time( &aclock );

				struct tm* newtime = gmtime( &aclock );
				int gyear = newtime->tm_year + 1900;

				gtime.tm_year = (int(gyear / 100))*100 + val_i;
				gtime.tm_year -= 1900;
			}
			else if (token_i.compare(HOUR_TOKEN) == 0) {
				gtime.tm_hour = val_i;
			}
			else if (token_i.compare(MINUTE_TOKEN) == 0) {
				gtime.tm_min = val_i;
			}
			else if (token_i.compare(SECOND_TOKEN) == 0) {
				gtime.tm_sec = val_i;
			}
			else if (token_i.compare(MILLISECOND_TOKEN) == 0) {
				milli = val_i;
			}
			else
				// unregistered token found
				THROW(CppUtils::OperationFailed, "exc_InvalidTokenInDateTimeFormat", "ctx_ParseDateTime", token_i );


		}




		gtime.tm_isdst = tm_isdst;

		// Convert
		double result = (double) mktime( &gtime ) - (double)_timezone;
		return result + ((double) milli) / 1000.0;
	}


	//-------------------------------------------------------------
	double getTime (
		)
	{
		struct _timeb now;
		_ftime( &now );

		return time(NULL) + now.millitm / 1000.0;
	}
	//-------------------------------------------------------------
	void sleep (
		double sec
		)
	{
		Sleep( int( sec * 1000 ) );
	}
	//-------------------------------------------------------------
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS adapt;
		NAME_BUFFER    NameBuff [30];

	} ASTAT, *PASTAT;


	bool getMAC (
		Byte *mac
		)
	{
		// This code ripped from MSDN KB Q118623

		ASTAT Adapter;

		NCB Ncb;
		UCHAR uRetCode;
		LANA_ENUM lenum;
		int i;

		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_buffer = (UCHAR *)&lenum;
		Ncb.ncb_length = sizeof(lenum);
		uRetCode = Netbios( &Ncb );

		for(i=0; i < lenum.length ;i++)
		{
			memset( &Ncb, 0, sizeof(Ncb) );
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];

			uRetCode = Netbios( &Ncb );

			memset( &Ncb, 0, sizeof (Ncb) );
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];

			strcpy( (char *) Ncb.ncb_callname,  "*               " );
			Ncb.ncb_buffer = (unsigned char *) &Adapter;
			Ncb.ncb_length = sizeof(Adapter);

			uRetCode = Netbios( &Ncb );
			if ( uRetCode == 0 )
			{
				for(int i=0; i<6; ++i)
					mac[i] = Adapter.adapt.adapter_address[i];

				return true;
			}
		}

		return false;
	}
	//-------------------------------------------------------------
	Char pathDelimiter (
		)
	{
		return '\\';
	}
	//-------------------------------------------------------------
	bool fileExists (
		String const &filename
		)
	{
		HANDLE hFile = CreateFile( filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if( hFile == INVALID_HANDLE_VALUE )
			return false;

		CloseHandle( hFile );
		return true;
	}
	//-------------------------------------------------------------
	bool directoryExists (
		String const &path
		)
	{
		Char buffer[ 1024 ];
		GetCurrentDirectory( 1023, buffer );

		bool result = !!SetCurrentDirectory( path.c_str() );
		SetCurrentDirectory( buffer );

		return result;
	}
	//-------------------------------------------------------------

	String getCurrentDirectory (
		)
	{
		Char buffer[ 1024 ];
		GetCurrentDirectory( 1023, buffer );

		return buffer;
	}



	//-------------------------------------------------------------
	bool compareFiles (
		String const &filename1,
		String const &filename2
		)
	{
		// Open files
		FILE *f1 = fopen( filename1.c_str(), "rb" );
		FILE *f2 = fopen( filename2.c_str(), "rb" );

		// Either one failed?
		if( !f1 && f2 ) {
			fclose( f2 );
			return false;
		}
		if( f1 && !f2 ) {
			fclose( f2 );
			return false;
		}
		if( !f1 && !f2 )
			return false;

		// Both files are valid


		// Compare length first
		//
		fseek( f1, 0, SEEK_END );
		long l1 = ftell( f1 );

		fseek( f2, 0, SEEK_END );
		long l2 = ftell( f2 );


		// Do we have to compare content?
		//
		bool equal = (l1 == l2);
		if( equal ) {
			// Yes
			fseek( f1, 0, SEEK_SET );
			fseek( f2, 0, SEEK_SET );

			char buf1[4096], buf2[4096];
			l1 = 0;
			while( l1 < l2 ) {
				int l = min(l2 - l1, 4096);
				fread( buf1, l, 1, f1 );
				fread( buf2, l, 1, f2 );

				if( memcmp( buf1, buf2, l ) ) {
					equal = false;
					break;
				}

				l1 += l;
			}
		}

		// Done
		fclose( f1 );
		fclose( f2 );

		return equal;
	}
	//-------------------------------------------------------------
	void *lockFile (String const &filename)
	{
		HANDLE hFile = ::CreateFile( filename.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		HTASSERT( hFile != 0 );

		if( hFile == INVALID_HANDLE_VALUE )
			return 0;

		if( !LockFile( hFile, 0, 0, 1, 0 ) ) {
			// Locking failed
			CloseHandle( hFile );
			return 0;
		}
		else
			// Return handle
			return hFile;
	}
	//-------------------------------------------------------------
	void unlockFile (
		void *handle
		)
	{
		if( handle != 0 ) {
			// Unlock & close
			UnlockFile( handle, 0, 0, 1, 0 );
			CloseHandle( handle );
		}
	}
	//-------------------------------------------------------------
	bool removeFile (
		String const &filename
		)
	{
		return ( 0 == _unlink( filename.c_str() ) );
	}
	//-------------------------------------------------------------
	bool renameFile (
		String const &src,
		String const &dest
		)
	{
		return ( 0 == rename( src.c_str(), dest.c_str() ) );
	}
	//-------------------------------------------------------------
	bool copyFile (
		String const &src,
		String const &dest,
		bool overwrite
		)
	{
		return ( 0 != CopyFile( src.c_str(), dest.c_str(), !overwrite ) );
	}

	//-------------------------------------------------------------


	CPPUTILS_EXP bool copyDir (
		String const &src_path,
		String const &dest_path,
		bool overwrite
		)
	{
		WIN32_FIND_DATA info ;
		HANDLE hwnd ;
		
		String src_path_s = expandDirName(src_path + pathDelimiter());
		String dst_path_s = expandDirName(dest_path + pathDelimiter());

		if (!directoryExists(src_path_s))
			return false;

		String temp_1 = src_path_s + "*.*";
		

		makeDir(dest_path);
	
		hwnd = FindFirstFile(temp_1.c_str(), &info) ;
		do{
			if (!strcmp(info.cFileName , ".")) 
				continue ;
			if (!strcmp(info.cFileName , "..")) 
				continue ;

			String temp_src = src_path_s + info.cFileName;
		
			if (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){

				String temp_dst = dst_path_s + info.cFileName;
				
				//CreateDirectory(temp_dest, NULL) ;
				copyDir(temp_src, temp_dst, overwrite) ;

			}else{

				String dst_file = dst_path_s +  info.cFileName;
			


				if (!copyFile(temp_src, dst_file, overwrite)) {
					FindClose(hwnd);
					return false;
				}

			}



		} while (FindNextFile(hwnd, &info)) ;
		
		FindClose(hwnd);

		return true;
	}

	bool copyDirFilter (
		String const &src_path,
		String const &dest_path,
		bool overwrite,
		function< bool(char const*) > & filterFun // functor to return true if copy taht file otherwise call false
	)
	{
		WIN32_FIND_DATA info ;
		HANDLE hwnd ;
		
		String src_path_s = expandDirName(src_path + pathDelimiter());
		String dst_path_s = expandDirName(dest_path + pathDelimiter());

		if (!directoryExists(src_path_s))
			return false;

		String temp_1 = src_path_s + "*.*";
		

		makeDir(dest_path);
	
		hwnd = FindFirstFile(temp_1.c_str(), &info) ;
		do{
			if (!strcmp(info.cFileName , ".")) 
				continue ;
			
			if (!strcmp(info.cFileName , "..")) 
				continue ;

			if (!filterFun(info.cFileName))
				continue;

			String temp_src = src_path_s + info.cFileName;
		
			if (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){

				String temp_dst = dst_path_s + info.cFileName;
				
				//CreateDirectory(temp_dest, NULL) ;
				copyDirFilter(temp_src, temp_dst, overwrite, filterFun) ;

			}else{

				String dst_file = dst_path_s +  info.cFileName;
			


				if (!copyFile(temp_src, dst_file, overwrite)) {
					FindClose(hwnd);
					return false;
				}

			}



		} while (FindNextFile(hwnd, &info)) ;
		
		FindClose(hwnd);

		return true;
	}


	//-------------------------------------------------------------
	bool makeDir (
		String const &path
		)
	{
		String spath = path;
		int pos = 0;
		do
		{
			int pos2 = spath.find_first_of("\\", pos);
			if(pos2 == std::string::npos)
				pos = spath.find_first_of("/", pos);
			else
				pos = pos2;

			if(pos != std::string::npos)
				//			CreateDirectory (spath.substr(0, pos++ ).c_str(), NULL);
					// May not return on failure, because of Win drive letters
						_mkdir( spath.substr(0, pos++ ).c_str() );
		}
		while(pos != std::string::npos);


		if( 0 == _mkdir( spath.c_str() ) || errno == EEXIST )
			//	if( 0 != CreateDirectory( spath.c_str() , NULL ) )
				// Directory has been created or existed already
					return true;
		else
			return false;
	}
	//-------------------------------------------------------------
	bool removeDir (
		String const &path,
		bool force
		)
	{
		if(force)
		{
			String workPath = path;
			if(workPath.find("*.*") == String::npos)
				workPath += "\\*.*";

			WIN32_FIND_DATA find;
			HANDLE fileHandle = FindFirstFile(workPath.c_str(), &find);
			int next = 1;

			while((fileHandle != INVALID_HANDLE_VALUE) && (next != 0))
			{
				if((strcmp(find.cFileName, "..") != 0) && (strcmp(find.cFileName, ".") != 0))
				{
					if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						//
						// remove the content of the directory
						//
						// build the file path
						String tempName = workPath.substr(0, workPath.find("\\*.*"));
						tempName += "\\";
						tempName += find.cFileName;
						removeDir(tempName.c_str(), true);
					}
					else
					{
						//
						// this is a file. remove it.
						//
						String tempName = workPath.substr(0, workPath.find("\\*.*"));;
						tempName += "\\";
						tempName += find.cFileName;

						if(find.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
						{
							find.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
							SetFileAttributes(tempName.c_str(), find.dwFileAttributes);
						}

						removeFile(tempName.c_str());
					}
				}
				next = FindNextFile(fileHandle, &find);
			}


			FindClose(fileHandle);
			//
			// directory should be removeable
			//
			return removeDir(path.c_str(), false);
		}
		else
			return ( 0 == _rmdir( path.c_str() ) );
	}
	//-------------------------------------------------------------
	String getTempPath (
		)
	{
		return expandPathName( "%TMP%" );
	}
	//-------------------------------------------------------------
	String getCanonicalFileName (
		String const &filename
		)
	{
		char buf[ MAX_PATH ];

		GetShortPathName( filename.c_str(), buf, MAX_PATH );

		return buf;
	}
	//-------------------------------------------------------------
	String expandPathName (
		String const &path
		)
	{
		// Use a do(..) loop to make sure that we have allocated
		// enough memory in the output buffer

		String spath( path );
		int len, nlen = 2 * spath.length();
		char *buf = NULL;

		if( nlen == 0 )
			return spath;

		do {
			len = nlen;
			if( buf ) delete buf;
			buf = new char[ len ];
			nlen = ExpandEnvironmentStrings( spath.c_str(), buf, len );	
		} while( nlen > len );

		String result( buf );
		delete buf;
		return result;
	}
	//-------------------------------------------------------------
	void getAllFileMatches(
		String const &pattern,
		StringList &l_fileName,
		StringList &l_dirName
		)
	{
		// Clear result
		l_fileName.clear();
		l_dirName.clear();

		// Use Windows fun
		WIN32_FIND_DATA findFileData;
		HANDLE hFind;

		// Get first match
		hFind = FindFirstFile( pattern.c_str(), &findFileData );

		if( hFind == INVALID_HANDLE_VALUE )
			// No match
				return;

		// Get all matches
		do {
			// Read match
			if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				// Skip "." and ".."
				if( strcmp( findFileData.cFileName, "." ) && strcmp( findFileData.cFileName, ".." ) )
					l_dirName.push_back( findFileData.cFileName );
			}
			else
				l_fileName.push_back( findFileData.cFileName );
		} while( FindNextFile( hFind, &findFileData ) != 0 );

		// Close search handle
		FindClose(hFind);
	}

	//-------------------------------------------------------------
	
	void getAllFileMatchesRecursive(
		String const &basePath,
		StringList &l_fileNameFull)
	{
		StringList  l_dirName, l_fileName;
		static const char* TEMPLATE = "*.*";

		// make sure we  finish with dir
		String basePath_s = CppUtils::expandDirName(basePath + CppUtils::pathDelimiter());

		String pattern = basePath_s + TEMPLATE;
		getAllFileMatches(pattern, l_fileName, l_dirName);

		// full paths
		for(int i = 0; i < l_fileName.size(); i++) {
			l_fileNameFull.push_back( basePath_s + l_fileName[ i ] );
		}

		for(int i = 0; i < l_dirName.size(); i++) {
			CppUtils::String new_pattern = basePath_s + l_dirName[i];
			getAllFileMatchesRecursive(new_pattern, l_fileNameFull );
		}
	}
	

	//-------------------------------------------------------------
	bool getEnvironmentVar (
		String const &var,
		String &value
		)
	{
		// Use a do(..) loop to make sure that we have allocated
		// enough memory in the output buffer

		String svar( var );
		int len, nlen = 2 * svar.length();
		char *buf = NULL;

		if( nlen == 0 )
			return false;

		do {
			len = nlen;
			if( buf ) delete [] buf;
			buf = new char[ len ];
			nlen = ExpandEnvironmentStrings( svar.c_str(), buf, len );
		} while( nlen > len );

		value = buf;
		delete buf;

		return nlen > 0;
	}
	//-------------------------------------------------------------
	int setEnvironmentVar (
		String const &var,
		String const &value
		)
	{
		return SetEnvironmentVariable( var.c_str(), value.c_str() );
	}

	void getCurrentProcessEnviromentVars(CppUtils::StringMap& vars)
	{
		vars.clear();
		char* l_EnvStr = GetEnvironmentStrings();

		if (l_EnvStr==NULL)
			return;

		LPTSTR l_str = l_EnvStr;

		int count = 0;
		while (true)
		{
			if (*l_str == 0) 
				break;
			while (*l_str != 0) l_str++;
			l_str++;
			count++;
		}

		String entry;
		for (int i = 0; i < count; i++)
		{

			entry = (l_EnvStr != NULL ? l_EnvStr:"");
			int idx = entry.find('=');
			if (idx != String::npos) {
				String k = entry.substr(0, idx);
				String v = entry.substr(idx+1);

				if (k.size() >0) {
					vars[k] = v;
				}
			}

			while(*l_EnvStr != '\0')
				l_EnvStr++;

			l_EnvStr++;
		}

		FreeEnvironmentStrings(l_EnvStr);
	}

	//-------------------------------------------------------------
	String getCwd (
		bool shortName
		)
	{
		char buf[1024], buf2[1024];
		_getcwd( buf, 1024 );

		if( shortName ) {
			GetShortPathName( buf, buf2, 1024 );
			return buf2;
		}
		else
			return buf;
	}
	//-------------------------------------------------------------
	void setCwd (
		String const &dir
		)
	{
		_chdir( dir.c_str() );
	}
	//-------------------------------------------------------------
	String getHostName (
		)
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 1, 0 );

		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 )
			return "";

		char buffer[1024];
		gethostname( buffer, 1023 );

		WSACleanup();

		return buffer;
	}
	//-------------------------------------------------------------
	String getOsInfo (
		)
	{
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (!GetVersionEx ((OSVERSIONINFO *)&osvi))
			return "system info not available";

		String info;

		if( osvi.dwMajorVersion == 3 && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
			info = "Windows 3.51";
		}
		else if( osvi.dwMajorVersion == 4 ) {
			if( osvi.dwMinorVersion == 0 && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
				info = "Windows NT 4.0";
			else if( osvi.dwMinorVersion == 0 && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
				info = "Windows 95";
			else if( osvi.dwMinorVersion == 10 && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
				info = "Windows 98";
			else if( osvi.dwMinorVersion == 90 && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
				info = "Windows Me";
			else
				info = "Windows 4.x; unknown minor version number";
		}
		else if( osvi.dwMajorVersion == 5 ) {
			if( osvi.dwMinorVersion == 0 && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
				info = "Windows 2000";
			else if( osvi.dwMinorVersion == 1 && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
				info = "Windows XP";
			else
				info = "Windows 5.x; unknown minor version number";
		}
		else if( osvi.dwMajorVersion == 6 ) {
			if( osvi.dwMinorVersion == 0   ) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					info = "Windows Vista";
				else
					info = "Windows Server 2008";
			}
			else if( osvi.dwMinorVersion == 1  ) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					info = "Windows 7";
				else
					info = "Windows Server 2008 R2";
			}
			else
				info = "Windows 6.x; unknown minor version number";

		}
		else
			info = "Windows: unknown major version number";

		if( strlen( osvi.szCSDVersion ) != 0 ) {
			info += " ";
			info += osvi.szCSDVersion;
		}

		return info;
	}

	//-------------------------------------------------------------


	String expandFileName(String const &path)
	{
		if(!path.size())
			return "";

		char* lastp = (char*)strrchr(path.c_str(), pathDelimiter() );
		// one more attempt with '/'
		if (!lastp)
			lastp = (char*)strrchr(path.c_str(), '/' );

		// we use these functions to process URLs
		char filename[MAX_PATH];

		if (! lastp)
			return "";

		strcpy(filename, lastp+1 );
		return String(filename);
	};

	//-------------------------------------------------------------

	String expandDirName(String const &path)
	{
		if(!path.size())
			return "";

		// exclude quotation symbols
		String pathproc = path;
		if (path.at(0) == '"') 
			pathproc = pathproc.substr(1, pathproc.size() );

		if (path.at(path.size()-1) == '"')
			pathproc.resize(pathproc.size() - 1);


		char pdelim =  pathDelimiter();
		char* lastp = (char*)strrchr(pathproc.c_str(), pdelim );

		if (!lastp) {
			pdelim = '/';
			lastp = (char*)strrchr(pathproc.c_str(), pdelim );
		}

		if (!lastp)
			return "";

		size_t num = (size_t)(lastp-pathproc.c_str());

		// we use these functions to process URLs
		HTASSERT(num < MAX_PATH);
		char dirname[MAX_PATH+1];

		strncpy(dirname, pathproc.c_str(), num);

		dirname[num] = '\0';
		if (dirname[ num - 1 ] != pdelim) {
			dirname[ num ] = pdelim;
			dirname[ num + 1] = '\0';
		}



		return String(dirname);
	}

	//-------------------------------------------------------------

	String getStartupDir()
	{
		return expandDirName(getProcessExecutableName());
	}

	//-------------------------------------------------------------


	long getFileSize(FileHandle filehandle)
	{
		if (!filehandle || filehandle==INVALID_HANDLE_VALUE)
			return -2;

		int size = ::GetFileSize(filehandle, NULL);

		if (size != INVALID_FILE_SIZE)
			return size;
		else
			return -1;
	}

	//-------------------------------------------------------------

	FileHandle openFile(String const &filename, DWORD access, DWORD share, DWORD disp)
	{
		HANDLE fh = ::CreateFile(filename.c_str(), access, share, 0, disp, 0, 0);

		return fh;
	}

	//-------------------------------------------------------------

	bool closeFile(FileHandle filehandle)
	{
		return (::CloseHandle( filehandle ) != 0);

	}

	//-------------------------------------------------------------


	bool exLockFile(FileHandle filehandle)
	{
		if (!filehandle || filehandle==INVALID_HANDLE_VALUE)
			return false;

		return LockFile( filehandle, 0, 0, 1, 0 ) > 0;

	}

	//-------------------------------------------------------------


	void exUnlockFile(FileHandle filehandle)
	{
		if (!filehandle || filehandle==INVALID_HANDLE_VALUE)
			return;

		UnlockFile( filehandle, 0, 0, 1, 0 );

	}


	

	//-------------------------------------------------------------

	bool positionFromEnd(FileHandle filehandle, LongLong offset, bool rewritecontent)
	{
#if(_WIN32_WINNT >= 0x0403)
		if (!filehandle || filehandle==INVALID_HANDLE_VALUE)
			return false;


		LARGE_INTEGER newFilePointer;
		LARGE_INTEGER liDistanceToMove;

		liDistanceToMove.QuadPart = offset;

		int res = ::SetFilePointerEx(filehandle,	liDistanceToMove,	&newFilePointer, FILE_END	);

		if (res==0)
			return false;

		if (rewritecontent) {
			return ::SetEndOfFile( filehandle ) > 0;
		}
		else
			return true;

#else

#error"Not implemented for this verison of Windows";

#endif

	}
	//-------------------------------------------------------------

	bool positionFromBegin(FileHandle filehandle, LongLong offset, bool rewritecontent)
	{
#if(_WIN32_WINNT >= 0x0403)
		if (!filehandle || filehandle==INVALID_HANDLE_VALUE)
			return false;


		LARGE_INTEGER newFilePointer;
		LARGE_INTEGER liDistanceToMove;

		liDistanceToMove.QuadPart = offset;

		int res = SetFilePointerEx(filehandle,	liDistanceToMove,	&newFilePointer, FILE_BEGIN	);

		if (res==0)
			return false;

		if (rewritecontent) {
			return ::SetEndOfFile( filehandle ) > 0;
		}
		else
			return true;

#else

#error "Not implemented for this verison of Windows";

#endif

	}


	//-------------------------------------------------------------

	bool saveContentInFile2(String const& file, String const& content)
	{
		if (file.size() <=0)
			return false;


		FILE* f = fopen(file.c_str(), "wt" );

		if (f == NULL) {
			fclose( f );
			return false;
		}


		size_t totalwrite = 0;
		char* ptr = (char*)content.c_str();
		static const int blocksz = 4096;

		while (true) {


			size_t to_write = content.size() - totalwrite;

			if (to_write >= blocksz)
				to_write = blocksz;
			size_t actually_wrote = fwrite(ptr, sizeof(char), to_write, f );

			// error
			if (actually_wrote != to_write) {
				fclose(f);	
				return false;
			}


			totalwrite += actually_wrote;

			if (totalwrite >= content.size())
				break;

			ptr += actually_wrote;

		}

		fclose(f);	
		return true;
	}

	//-------------------------------------------------------------

	bool readContentFromFile2(String const& file, String & content)
	{
		if (file.size() <=0)
			return false;

		unsigned char buf[4096];

		content.clear();
		FILE* f = fopen(file.c_str(), "rt" );

		if (f == NULL) {
			//fclose( f );
			return false;
		}


		while (!feof(f)) {
			// read from file
			size_t read = fread(buf, sizeof(char), 4096, f);
			if (read <=0)
				break;

			content.insert( content.size(), (char*)buf, read );

		}

		// close
		fclose(f);
		return true;
	}

	//-------------------------------------------------------------

	bool saveContentInFile(FileHandle handle, String const& content)
	{

		if (!handle) 
			return false;

		// rewrite and destroy current content
		int res = ::SetFilePointer( handle, 0, 0, FILE_BEGIN );
		// fails if new file pointer not 0
		if (res != 0)
			return false;

		// just cut the file
		res = ::SetEndOfFile(handle);

		if (res == 0)
			return false;


		size_t totalwrite = 0;
		char* ptr = (char*)content.c_str();
		static const int blocksz = 4096;



		res = 1;
		while (true) {

			size_t to_write = content.size() - totalwrite;

			if (to_write >= blocksz)
				to_write = blocksz;

			DWORD actually_wrote;
			res = ::WriteFile(handle, ptr, to_write, &actually_wrote, NULL); 
			if (actually_wrote != to_write || res == 0)
				return false;

			totalwrite += actually_wrote;

			if (totalwrite >= content.size())
				break;

			ptr += actually_wrote;


		}


		return true;

	}

	//-------------------------------------------------------------

	bool readContentFromFile(FileHandle handle, String & content)
	{

		unsigned char buf[4096];
		content.clear();

		if (!handle) {
			return false;
		}

		DWORD read = 0;
		while (1) {
			// read from file
			read = 4096;
			int res = ::ReadFile(handle, buf, 4096, &read, NULL);

			if (!read || res <=0 )
				break;

			content.insert( content.size(), (char*)buf, read );

		}

		return true;

	}
	//-------------------------------------------------------------

	bool saveContentInFile2(String const& file, MemoryChunk const& content)
	{
		if (file.size() <=0)
			return false;


		FILE* f = fopen(file.c_str(), "wt" );

		if (f == NULL) {
			fclose( f );
			return false;
		}


		size_t totalwrite = 0;
		CppUtils::Byte* ptr = (CppUtils::Byte*)content.data();
		static const int blocksz = 4096;

		while (true) {


			size_t to_write = content.length() - totalwrite;

			if (to_write >= blocksz)
				to_write = blocksz;
			size_t actually_wrote = fwrite(ptr, sizeof(CppUtils::Byte), to_write, f );

			// error
			if (actually_wrote != to_write) {
				fclose(f);	
				return false;
			}


			totalwrite += actually_wrote;

			if (totalwrite >= content.length())
				break;

			ptr += actually_wrote;

		}

		fclose(f);	
		return true;
	}

	//-------------------------------------------------------------

	bool readContentFromFile2(String const& file, MemoryChunk & content)
	{
		char buf[4096];


		FILE* f = fopen(file.c_str(), "rt" );

		if (f == NULL) {
			//fclose( f );
			return false;
		}

		content.length(0);		
		while (!feof(f)) {
			// read from file
			size_t read = fread(buf, sizeof(char), 4096, f);
			if (read <=0)
				break;

			content.length(content.length() + read );
			memcpy(content.data(), buf, read);

		}

		// close
		fclose(f);
		return true;
	}

	//-------------------------------------------------------------

	long writeToFile(FileHandle handle, CppUtils::Byte const* buffer, long toWrite)
	{
		DWORD written = 0;
		int res = WriteFile(handle, (LPCVOID)buffer, (DWORD)toWrite, &written, NULL);

		if (res)
			return (long)written;
		else
			return res;
	}

	//-------------------------------------------------------------


	long writeToFile(void* handle, MemoryChunk const & content)
	{
		DWORD written = 0;
		int res = WriteFile(handle, (LPCVOID)content.data(), (DWORD)content.length(), &written, NULL);

		if (res)
			return (long)written;
		else
			return res;
	}

	//-------------------------------------------------------------

	UnsignedLongLong getFileSizeEx(FileHandle handle)
	{

#if(_WIN32_WINNT >= 0x0403)

		LARGE_INTEGER result;
		int res = ::GetFileSizeEx(handle, &result);
		if (res)
			return result.QuadPart;
		else
			return -1;
#else

#error "Not implemented for this verison of Windows";

#endif
	}


	//-------------------------------------------------------------

	long readFromFile(FileHandle handle, CppUtils::Byte* buffer, long toRead)
	{

		DWORD wasRead;
		int result =  ReadFile( handle, (void*)buffer, toRead, &wasRead, NULL);

		if (result)  {
			// success

			// then number of bytes can be 0
			return wasRead;
		}
		else
			// failure
			return -1;
	}


	//-------------------------------------------------------------


	long readFromFile(FileHandle handle, MemoryChunk& content)
	{
		DWORD wasRead;
		int result =  ReadFile( handle, (void*)content.data(), content.length(), &wasRead, NULL);

		if (result)  {
			// success

			// then number of bytes can be 0
			return wasRead;
		}
		else
			// failure
			return -1;
	}

	//-------------------------------------------------------------

	CPPUTILS_EXP ThreadExecutionState setThreadExecutionState(ThreadExecutionState const newstate)
	{
		return ::SetThreadExecutionState(newstate);
	}

	CPPUTILS_EXP int getProcessOpenHandles( )
	{
		DWORD handles;
		BOOL res  = ::GetProcessHandleCount(::GetCurrentProcess(),  &handles);

		if (!res)
			return -1;

		return handles;

	}



	//=============================================================
	// UUID creation
	//=============================================================

	/* Assume that _ftime() has millisecond granularity */
#define MAX_ADJUSTMENT 100

	static void getClock(
		long *clock_high,
		long *clock_low,
		short *ret_clock_seq
		)
	{
		static int adjustment = 0;
		static _timeb	last = {0, 0};
		static short clock_seq;

		_timeb tv;
		__int64 clock_reg;

try_again:
		_ftime(&tv);

		if ((tv.time < last.time) || ((tv.time == last.time) && (tv.millitm < last.millitm))) {
			clock_seq = (clock_seq+1) & 0x1FFF;
			adjustment = 0;
		}
		else
			if ((tv.time == last.time) && (tv.millitm == last.millitm)) {
				if (adjustment >= MAX_ADJUSTMENT)
					goto try_again;
				adjustment++;
			}
			else
				adjustment = 0;

		clock_reg = tv.millitm*100 + adjustment;
		clock_reg += ((__int64) tv.time)*10000000;
		clock_reg += (((__int64) 0x01B21DD2) << 32) + 0x13814000;

		*clock_high = clock_reg >> 32;
		*clock_low = clock_reg;
		*ret_clock_seq = clock_seq;
		last = tv;
	}

	struct uuid {
		long time_low;
		short	time_mid;
		short	time_hi_and_version;
		short	clock_seq;
		char node[6];
	};

	void generateGuid (
		char *out
		)
	{
		static Byte node_id[6];
		static int has_init = 0;
		uuid uu;
		long clock_mid;

		if (!has_init) {
			// Sleep 10 msec to make sure we do not have
			// multiple runs of the same program
			Sleep( 10 );

			if (!getMAC(node_id)) {
				for (int i=0; i < 6; i++)
					node_id[i] = rand() & 0xFF;

				/*
				* Set multicast bit, to prevent conflicts
				* with IEEE 802 addresses obtained from
				* network cards
				*/
				node_id[0] |= 0x80;
			}
			has_init = 1;
		}

		getClock(&clock_mid, &uu.time_low, &uu.clock_seq);

		uu.clock_seq |= 0x8000;
		uu.time_mid = (short) clock_mid;
		uu.time_hi_and_version = (clock_mid >> 16) | 0x1000;
		memcpy(uu.node, node_id, 6);

		// Pack result
		long	tmp;

		tmp = uu.time_low;
		out[3] = (unsigned char) tmp;
		tmp >>= 8;
		out[2] = (unsigned char) tmp;
		tmp >>= 8;
		out[1] = (unsigned char) tmp;
		tmp >>= 8;
		out[0] = (unsigned char) tmp;

		tmp = uu.time_mid;
		out[5] = (unsigned char) tmp;
		tmp >>= 8;
		out[4] = (unsigned char) tmp;

		tmp = uu.time_hi_and_version;
		out[7] = (unsigned char) tmp;
		tmp >>= 8;
		out[6] = (unsigned char) tmp;

		tmp = uu.clock_seq;
		out[9] = (unsigned char) tmp;
		tmp >>= 8;
		out[8] = (unsigned char) tmp;

		memcpy(out+10, uu.node, 6);
	}






	//=============================================================
	// Dynamic linking
	//=============================================================
	void *dynlibOpen (
		String const &libName
		)
	{
		return  LoadLibrary( libName.c_str() );
	}

	void * dynlibOpenEx (
		String const &libName,
		int dwFlags
		)
	{
		return LoadLibraryEx(libName.c_str(), NULL, (DWORD)dwFlags);
	}

	//--------------------------------------------------------------
	void dynlibClose (
		void *library
		)
	{
		FreeLibrary( (HMODULE) library );
	}
	//--------------------------------------------------------------
	void *dynlibGetSym (
		void *library,
		String const &symName
		)
	{
		return GetProcAddress( (HMODULE) library, symName.c_str() );
	}




	double convertOleTimeToUnix(double const& oledt)
	{
		double result = -1;
		SYSTEMTIME systemtime;
		struct tm newtime;
		memset(&newtime, '\0', sizeof(tm));


		if (::VariantTimeToSystemTime( oledt, &systemtime ) == TRUE) {
			newtime.tm_year = systemtime.wYear - 1900;
			newtime.tm_mon = systemtime.wMonth - 1;

			newtime.tm_mday = systemtime.wDay;
			newtime.tm_hour = systemtime.wHour;

			newtime.tm_sec = systemtime.wSecond;
			newtime.tm_min = systemtime.wMinute;

			newtime.tm_isdst = 0;
			newtime.tm_wday = 0;
			newtime.tm_yday = 0;

			result = (double) mktime( &newtime ) - (double)_timezone;

		}

		return result;

	}

	void convertOleTimeToUnix(double const& oledt, double& utime, tm& tm_struct)
	{
		utime = -1;
		SYSTEMTIME systemtime;

		memset(&tm_struct, '\0', sizeof(tm));


		if (::VariantTimeToSystemTime( oledt, &systemtime ) == TRUE) {
			tm_struct.tm_year = systemtime.wYear - 1900;
			tm_struct.tm_mon = systemtime.wMonth - 1;

			tm_struct.tm_mday = systemtime.wDay;
			tm_struct.tm_hour = systemtime.wHour;

			tm_struct.tm_sec = systemtime.wSecond;
			tm_struct.tm_min = systemtime.wMinute;

			tm_struct.tm_isdst = 0;
			tm_struct.tm_wday = 0;
			tm_struct.tm_yday = 0;

			utime = (double) mktime( &tm_struct ) - (double)_timezone;

		}

	}

	double convertUnixTimeToOleTime(double const& utime)
	{

		SYSTEMTIME systemtime;


		time_t atime = (int) utime;
		struct tm *newtime = gmtime(&atime);

		// get milliseconds
		double millisec = (utime - (int) utime) * 1000.0;


		systemtime.wYear = newtime->tm_year + 1900;
		systemtime.wMonth = newtime->tm_mon + 1;

		systemtime.wDay = newtime->tm_mday;
		systemtime.wHour = newtime->tm_hour;
		systemtime.wSecond = newtime->tm_sec;
		systemtime.wMilliseconds = (int)millisec;
		systemtime.wMinute = newtime->tm_min;
		systemtime.wDayOfWeek = 0;

		DATE result = -1;
		if ( ::SystemTimeToVariantTime(&systemtime, &result) )
			return result;
		else
			return -1;

	}

	double returnGmtFromTm(tm const& candleTime)
	{
		tm tmp_tm;
		memcpy(&tmp_tm, &candleTime, sizeof(tm));
		return (double)mktime(&tmp_tm) - (double)_timezone;
	}

	CppUtils::String getDefaultBrowser()
	{
		CppUtils::String strBrowser;
		// Have we already got the browser?

		// Get the default browser from HKCR\http\shell\open\command
		HKEY hKey = NULL;
		// Open the registry
		if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, "http\\shell\\open\\command", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			// Data size
			DWORD cbData = 0;
			// Get the default value
			if (::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL, &cbData) == ERROR_SUCCESS && cbData > 0)
			{
				// Allocate a suitable buffer
				char* psz = new char [cbData];
				// Success?
				if (psz != NULL)
				{
					if (::RegQueryValueEx(hKey, NULL, NULL,	NULL, (LPBYTE)psz, &cbData) ==	ERROR_SUCCESS)
					{
						// Success!
						strBrowser = psz;
					}
					delete [] psz;
				}
			}
			::RegCloseKey(hKey);
		}
		// Do we have the browser?
		if (strBrowser.size() > 0)
		{
			// Strip the full path from the string
			int nStart = strBrowser.find('"');
			int nEnd = strBrowser.find_last_of('"');
			// Do we have either quote?
			// If so, then the path contains spaces
			if (nStart >= 0 && nEnd >= 0)
			{
				// Are they the same?
				if (nStart != nEnd)
				{   
					// Get the full path
					strBrowser = strBrowser.substr(nStart + 1, nEnd - nStart - 1);
				}
			}
			else
			{
				// We may have a pathname with spaces but
				// no quotes (Netscape), e.g.:
				//   C:\PROGRAM FILES\NETSCAPE\COMMUNICATOR\PROGRAM\NETSCAPE.EXE -h "%1"
				// Look for the last backslash
				int nIndex = strBrowser.find_last_of('\\');
				// Success?
				if (nIndex > 0)
				{
					// Look for the next space after the final
					// backslash
					int nSpace = strBrowser.find(' ', nIndex);
					// Do we have a space?
					if (nSpace > 0)
						strBrowser = strBrowser.substr(0,nSpace);    
				}
			}
		}

		// Done
		return strBrowser;

	}

	bool compareTwoPaths(CppUtils::String const& path1, CppUtils::String const& path2)
	{



		//Get file handles
		// FILE_FLAG_BACKUP_SEMANTICS - to open directories
		HANDLE handle1 = ::CreateFile(path1.c_str(), 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL); 
		HANDLE handle2 = ::CreateFile(path2.c_str(), 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL); 

		bool bResult = false;

		//if we could open both paths...
		if (handle1 != INVALID_HANDLE_VALUE && handle2 != INVALID_HANDLE_VALUE)
		{
			BY_HANDLE_FILE_INFORMATION fileInfo1;
			BY_HANDLE_FILE_INFORMATION fileInfo2;
			if (::GetFileInformationByHandle(handle1, &fileInfo1) && ::GetFileInformationByHandle(handle2, &fileInfo2))
			{
				//the paths are the same if they refer to the same file (fileindex) on the same volume (volume serial number)
				bResult = fileInfo1.dwVolumeSerialNumber == fileInfo2.dwVolumeSerialNumber &&
					fileInfo1.nFileIndexHigh == fileInfo2.nFileIndexHigh &&
					fileInfo1.nFileIndexLow == fileInfo2.nFileIndexLow;
			}
		}

		//free the handles
		if (handle1 != INVALID_HANDLE_VALUE )
		{
			::CloseHandle(handle1);
		}

		if (handle2 != INVALID_HANDLE_VALUE )
		{
			::CloseHandle(handle2);
		}

		//return the result
		return bResult;

	}


	CppUtils::String getProcessExecutableName()
	{
		CppUtils::String result;

		char buf[MAX_PATH+1];
		DWORD r= ::GetModuleFileName(NULL, buf, MAX_PATH );

		if (r >0 && r <= MAX_PATH) {
			result = buf;
		}

		return result;


	}

	CppUtils::String getProcessNameByHandle(HANDLE pid)
	{
		CppUtils::String pname;
		char szProcessName[MAX_PATH+1];
		//memset(szProcessName, '\0', MAX_PATH+1);

		int r;
		if (pid == NULL) {
			// current

			r = ::GetModuleFileName(NULL, szProcessName, MAX_PATH);
			if (r == 0 ) {
				pname = "GetModuleFileName(...) failed because of: " + CppUtils::getOSError();
				return pname;
			}

			szProcessName[r] = '\0';
		} 
		else {
			r = ::GetModuleFileNameEx(pid, NULL, szProcessName, MAX_PATH );
			if (r == 0 ) {
				pname = "GetModuleFileNameEx(...) failed because of: " + CppUtils::getOSError();
				return pname;
			}

			szProcessName[r] = '\0';
		}



		pname = szProcessName;
		return pname;
	}

	CppUtils::String reportProcessMemoryUsage(HANDLE pid)
	{
		CppUtils::String report = "Memory Report:\n";

		HANDLE pid_i = pid ? pid : GetCurrentProcess();

		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		BOOL r = GlobalMemoryStatusEx(&memInfo);
		if (!r) {
			report = "GlobalMemoryStatusEx(...) failed because of: " + CppUtils::getOSError();
			return report;
		}




		PROCESS_MEMORY_COUNTERS_EX pmc;
		r = ::GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
		if (! r ) {
			report = "GetProcessMemoryInfo(...) failed because of: " + CppUtils::getOSError();
			return report;
		}




		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;

		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

		DWORD pctTotalMemoryUsed= memInfo.dwMemoryLoad;



		report. append("Process name: ").append(getProcessNameByHandle(pid_i)).append("\n").
			append("Total virtual memory: ").append(CppUtils::long64ToString(totalVirtualMem)).append( "\n" ).
			append("Total virtual memory used: ").append(CppUtils::long64ToString(virtualMemUsed)).append( "\n").
			append("Total virtual memory by the process: ").append(CppUtils::long2String(virtualMemUsedByMe)).append("\n").
			append("Total physical memory: ").append(CppUtils::long64ToString(totalPhysMem)).append( "\n").
			append("Total physical memory used: ").append(CppUtils::long64ToString(physMemUsed)).append( "\n").
			append("Total physical memory used by the process: ").append(CppUtils::long2String(physMemUsedByMe)).append("\n").
			append("Pct total physical memory used: ").append(CppUtils::long2String(pctTotalMemoryUsed)).append( "\n" );


		return report;

	}

	ULONGLONG subtractTimes(const FILETIME& ftA, const FILETIME& ftB)
	{
		LARGE_INTEGER a, b;
		a.LowPart = ftA.dwLowDateTime;
		a.HighPart = ftA.dwHighDateTime;

		b.LowPart = ftB.dwLowDateTime;
		b.HighPart = ftB.dwHighDateTime;

		return a.QuadPart - b.QuadPart;
	}


	ResourecUsageStruct createFullSystemUsageReport32( int const delayBetweenSnapshotsSec )
	{

		ResourecUsageStruct result;




		if (delayBetweenSnapshotsSec > 0) {

			createFullSystemUsageReport32Helper(result.cpuUsages, NULL, NULL);
			CppUtils::sleep(delayBetweenSnapshotsSec);
			createFullSystemUsageReport32Helper(result.cpuUsages, &result.globPhysMemUsageKB, &result.globVirtMemUsageKB);
		}
		return result;

	}

	void createFullSystemUsageReport32Helper(map<int, CPUUsageStruct> &result, int* globPhysMemUsageKB, int* globVirtMemUsageKB)
	{
		// static because we can incremet only
		static map<int, CPUUsageStruct> cpuUsages;
		static CppUtils::Mutex mutex;

		result.clear();


		//CppUtils::String r;

		DWORD aProcesses[2048], cbNeeded, cProcesses;


		if ( !::EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )  {
			return;
		}

		cProcesses = cbNeeded / sizeof(DWORD);

		DWORD curId = ::GetCurrentProcessId();
		PROCESS_MEMORY_COUNTERS_EX pmc;

		for ( int i = 0; i < cProcesses; i++ )
		{
			if( aProcesses[i] != 0 )   {
				char szProcessName[MAX_PATH] = TEXT("<unknown>");



				// Get a handle to the process.

				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
					PROCESS_VM_READ,
					FALSE, aProcesses[i] );

				// Get the process name.

				if (NULL != hProcess )	{
					HMODULE hMod;
					DWORD cbNeeded;

					if ( ::EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
					{
						GetModuleBaseName( hProcess, hMod, szProcessName, MAX_PATH );
						CppUtils::String szProcessName_s = (szProcessName ? szProcessName : "");
						if (szProcessName_s.size() <=0)
							continue;

						// process memory info


						bool first_time = false;

						// can calculate
						{
							LOCK_FOR_SCOPE(mutex)

								CPUUsageStruct &processUsage = cpuUsages[aProcesses[i]];
							processUsage.processName  = szProcessName_s;
							processUsage.pid = aProcesses[i];

							if (curId == aProcesses[i]) {
								processUsage.curProcess = true;
							}

							// CPU usage
							if (GetSystemTimes(
								&processUsage.current.ftSysIdle, 
								&processUsage.current.ftSysKernel, 
								&processUsage.current.ftSysUser
								) &&
								GetProcessTimes(
								hProcess, 
								&processUsage.current.ftProcCreation, 
								&processUsage.current.ftProcExit, 
								&processUsage.current.ftProcKernel, 
								&processUsage.current.ftProcUser)
								)
							{


								if (!processUsage.firstTime) {

									// not the firts time

									ULONGLONG ftSysKernelDiff = subtractTimes(processUsage.current.ftSysKernel, processUsage.previous.ftSysKernel);
									ULONGLONG ftSysUserDiff = subtractTimes(processUsage.current.ftSysUser, processUsage.previous.ftSysUser);

									ULONGLONG ftProcKernelDiff = subtractTimes(processUsage.current.ftProcKernel, processUsage.previous.ftProcKernel);
									ULONGLONG ftProcUserDiff = subtractTimes(processUsage.current.ftProcUser, processUsage.previous.ftProcUser);

									ULONGLONG nTotalSys =  ftSysKernelDiff + ftSysUserDiff;
									ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;

									if (nTotalSys > 0)	{
										processUsage.nCpuCopy = (short)((100.0 * nTotalProc) / nTotalSys);
									}


								}
								else {
									first_time = true;
									processUsage.firstTime = false;
								}



								// copy prev
								processUsage.previous = processUsage.current;




							}

							if (!first_time) {
								// get this info if not the first time
								if (::GetProcessMemoryInfo(hProcess, (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc))){
									double virtualMemUsedByMeKb = pmc.PrivateUsage / 1024.0;
									double physMemUsedByMeKb = pmc.WorkingSetSize / 1024.0;

									processUsage.virtMemUsageKB = virtualMemUsedByMeKb;
								}
							}

							/*
							r.append(szProcessName).append(",").append(CppUtils::long2String(aProcesses[i])).append(", ");
							r.append(CppUtils::float2String(virtualMemUsedByMeKb, -1, 1)).append(" KB, ");	
							r.append(!first_time ? CppUtils::float2String(processUsage.nCpuCopy, -1, 1) : "N/A").append(" %");
							r.append("\r\n");
							*/


							// pass
							result[aProcesses[i]] = processUsage;
						} // end mutex

					}
				}

				// Print the process name and identifier.


				// Release the handle to the process.

				CloseHandle( hProcess );

			}


		}


		// global memory
		if (globPhysMemUsageKB != NULL && globVirtMemUsageKB != NULL) {
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			if (::GlobalMemoryStatusEx(&memInfo)) {
				//DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
				DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;

				DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
				DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

				*globPhysMemUsageKB = (double)physMemUsed / 1024.0;
				*globVirtMemUsageKB = (double)virtualMemUsed / 1024.0;

				/*
				r.append("\r\n");
				r.append("Total physical memory installed:  ").append(CppUtils::float2String((double)totalPhysMem / 1024.0, -1, 1)).append(" KB\r\n");
				r.append("Total virtual memory used:  ").append(CppUtils::float2String((double)virtualMemUsed / 1024.0, -1, 1)).append(" KB\r\n");
				r.append("Total physical memory used:  ").append(CppUtils::float2String((double)physMemUsed / 1024.0, -1, 1)).append(" KB\r\n");
				*/

			}
		}


	}


	bool killProcess(int const pid, int exitCode)
	{
		DWORD dwDesiredAccess = PROCESS_TERMINATE;
		BOOL  bInheritHandle  = FALSE;
		HANDLE hProcess = ::OpenProcess(dwDesiredAccess, bInheritHandle, pid);
		if (hProcess == NULL)
			return FALSE;

		BOOL result = ::TerminateProcess(hProcess, exitCode);

		::CloseHandle(hProcess);

		return result;
	}

	bool isProcessRunning(int const pid)
	{
		HANDLE process = ::OpenProcess(SYNCHRONIZE, FALSE, pid);
		DWORD ret = ::WaitForSingleObject(process, 0);
		::CloseHandle(process);
		return ret == WAIT_TIMEOUT;
	}

	bool shellExecute(HWND hwnd, CppUtils::String const& image, CppUtils::String const& cmdLine, CppUtils::String const& startupDir, int const showCmd)
	{
		int ret_open = (int)::ShellExecute(hwnd, "open", image.c_str(), cmdLine.c_str(), startupDir.c_str(),showCmd );

		if (ret_open <= 32) {
			return false;
		}
		return true;
	}


	//=============================================================
	// Helper to set unexpected exception handler to produce a message.
	//=============================================================
	void unexpectedHandler (
		)
	{
		displayError( "Unexpected exception." );

		// Abort
		terminate();
	}
	//--------------------------------------------------------------
	const int SetUnexpectedHandler::foo_sm = SetUnexpectedHandler::setHandler();
	//--------------------------------------------------------------
	int SetUnexpectedHandler::setHandler (
		)
	{
		set_unexpected( unexpectedHandler );

		return 42;
	}


	// ------------------------------

	FileLockHelper::FileLockHelper():
			f_m(INVALID_HANDLE_VALUE),
			fsize_m(INVALID_FILE_SIZE)
	{
	}


	FileLockHelper::~FileLockHelper()
	{
		unlock();
	}

	bool FileLockHelper::initialize(char const* fileName, int const desiredAccess, int const sharedMode, int const disposition)
	{
		lastError_m.clear();
		
		if (f_m != INVALID_HANDLE_VALUE)
			return false;

		memset(&overlapped_m, 0, sizeof(overlapped_m));
		
		if (nvl(fileName)) {
			lastError_m = "Empty file name";
			return false;
		}

		f_m = ::CreateFileA(fileName,  desiredAccess, sharedMode, 0, disposition, 0, 0);

		if (f_m == INVALID_HANDLE_VALUE) {
			lastError_m = getOSError();
			return false;
		}

		long f_size = ::GetFileSize(f_m, NULL);
		if (f_size == INVALID_FILE_SIZE) 
		{
			lastError_m = getOSError();

			::CloseHandle(f_m);
			f_m = INVALID_HANDLE_VALUE;
			
			return false;
		}

		bool res = ::LockFileEx(f_m, LOCKFILE_EXCLUSIVE_LOCK, 0, f_size, 0, &overlapped_m);
		if (!res) {
			// lock cannot be obtained
			lastError_m = getOSError();
		}

		return res;
		
	}

	void FileLockHelper::unlock()
	{

		if (f_m != INVALID_HANDLE_VALUE) {

		
			if (fsize_m != INVALID_FILE_SIZE)
				::UnlockFileEx(f_m, 0, fsize_m, 0, &overlapped_m);

			::CloseHandle(f_m);

			f_m = INVALID_HANDLE_VALUE;
			fsize_m = INVALID_FILE_SIZE;
		}

		
	}



	// End of namespace
}

