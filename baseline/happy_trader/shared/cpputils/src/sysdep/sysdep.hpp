

#ifndef _CPPUTILS_SYSDEP_INCLUDED
#define _CPPUTILS_SYSDEP_INCLUDED



#include "../cpputilsdefs.hpp"
#include <functional>




namespace CppUtils {

struct CPUTimeStruct
{
	
	FILETIME ftSysIdle;
	FILETIME ftSysKernel;
	FILETIME ftSysUser;

	FILETIME ftProcCreation; 
	FILETIME ftProcExit; 
	FILETIME ftProcKernel; 
	FILETIME ftProcUser;

	
};

struct CPUUsageStruct
{
	CPUUsageStruct(): 
		nCpuCopy(0), 
		firstTime(true), 
		pid(-1),
		virtMemUsageKB(-1),
		curProcess(false)
	{
	};
	CPUTimeStruct current;
	CPUTimeStruct previous;
	short nCpuCopy;
	bool firstTime;
	int pid;
	CppUtils::String processName;
	float virtMemUsageKB;
	bool curProcess;
};

struct ResourecUsageStruct
{
	map<int, CPUUsageStruct> cpuUsages;
	int globPhysMemUsageKB;
	int globVirtMemUsageKB;

};




class MemoryChunk;

//=============================================================
// Basic functions
//=============================================================

/** Function returning the name of the exectuble of the current process. */
CPPUTILS_EXP String getProcessName (
);

/** Function to abort the program without calling functions installed
		with atexit(). Exits the program with return code -1. */
CPPUTILS_EXP void abort (
);

/** Function returning the id of the current process. */
CPPUTILS_EXP unsigned long int getPID (
);

/** Function returning the id of the current thread. */
CPPUTILS_EXP unsigned long int getTID (
);

/** Function to return the seconds since 1.1.1970 00:00.
		The returned time value has a millisecond resolution. */
CPPUTILS_EXP double getTime (
);

/** Get argument time as human-readable string. If \a t is
		zero, the current time is returned. */
CPPUTILS_EXP String getAscTime (
	double t = 0.0
);

/** Get argument time as human-readable sring in more simple format. If \a t is
		zero, the current time is returned. */
CPPUTILS_EXP String getAscTime2 (
	double const& t 
);


CPPUTILS_EXP String getAscTimeAsFileName (
	double const t = 0.0
);



/** Get argument time as human-readable sring in more simple format. If \a t is
		zero, the current time is returned. Returns locale-independent GMT time */
CPPUTILS_EXP String getGmtTime (
	double const& t 
);

/** Get argument time as human-readable sring in more simple format. If \a t is
		zero, the current time is returned. Returns locale-independent GMT time 
		yyyy.mm.dd hh:mm
*/
CPPUTILS_EXP String getGmtTime2 (
	double const& t 
);

/** Get argument time as human-readable sring in more simple format. If \a t is
		zero, the current time is returned. Returns locale-independent GMT time 
		yyyy-mm-dd hh:mm
*/
CPPUTILS_EXP String getGmtTime3 (
	double const& t 
);


/** Returns tm compounds */
CPPUTILS_EXP void crackGmtTime(
	double const& t,
	tm& newtime
);

/** round to the beginning of the day */
CPPUTILS_EXP double roundToBeginningOfTheDay(double const t);		



/** round minutes */
CPPUTILS_EXP double roundToMinutes(double const t);		


/** Convert the ISO time string into unix time. */
CPPUTILS_EXP double parseAscTime (
	String const &t
);

/** Convert an arbitrary time string into unix time. */
CPPUTILS_EXP double parseFormattedDateTime (
	String const &format, 
	String const &t,
	int tm_isdst
);


/** Get argument time as simple human-readable string. */
CPPUTILS_EXP String getSimpleAscTime (
	double t
);


/** Sleep for the given time in seconds. */
CPPUTILS_EXP void sleep (
	double sec
);

/** Function to obtain the MAC address of the network card or
		an equivalently unique id. Fills the argument \a mac with the
		MAC address; \a mac must point to an allocated memory portion
		of length 6 bytes. Returns true if the MAC address has been
		retrieved successfully. */
CPPUTILS_EXP bool getMAC (
	Byte *mac
);

/** Generate a UUID according to IETF draft, filling the argument
		\a out that must point to 16 bytes allocated memory. */
CPPUTILS_EXP void generateGuid(
	Byte *out
);

/** Return the path delimiting character. */
CPPUTILS_EXP Char pathDelimiter (
);

/** Check if the given file exists. */
CPPUTILS_EXP bool fileExists (
	String const &filename
);

/** Check if the given path exists. */
CPPUTILS_EXP bool directoryExists (
	String const &path
);

/** Get current directory. */
CPPUTILS_EXP String getCurrentDirectory (
);


/** Compare the content of two files and return true if they
		are equal. Non-existing files are treated as empty files. */
CPPUTILS_EXP bool compareFiles (
	String const &filename1,
	String const &filename2
);

/** On Windows, this converts to short path name. */
CPPUTILS_EXP String getCanonicalFileName (
	String const &filename
);

/** Lock a file for exclusive access by the calling process and return
		a handle to which the lock is bound. If the file could not be locked,
		the handle is 0.

		\attention unlockFile() must be called.
*/
CPPUTILS_EXP void *lockFile (
	String const &filename
);

/** Unlock a file for exclusive access by the calling process by the
		handle supplied as obtained by lockFile(). */
CPPUTILS_EXP void unlockFile (
	void *handle
);

/** Remove the file with given name and return true on success. */
CPPUTILS_EXP bool removeFile (
	String const &filename
);

/** Rename the source file to the destination and return true on success. */
CPPUTILS_EXP bool renameFile (
	String const &src,
	String const &dest
);

/** Copy the source file to the destination and return true on success;
		the parameter \a overwrite defines whether to overwrite any existing
		destination file. */
CPPUTILS_EXP bool copyFile (
	String const &src,
	String const &dest,
	bool overwrite
);

/** 
	Copy entire content from src_path t dst_path. It creates directories if necessary
	
 */
CPPUTILS_EXP bool copyDir (
	String const &src_path,
	String const &dest_path,
	bool overwrite
);

/**
The same as above but accept which files to be copied

*/
CPPUTILS_EXP bool copyDirFilter (
	String const &src_path,
	String const &dest_path,
	bool overwrite,
	function< bool(char const*) > & filterFun // functor to return true if copy taht file otherwise call false
);

/** Create directory \a path if it does not exist; return false if the
		directory does not exist and could not be created. */
CPPUTILS_EXP bool makeDir (
	String const &path
);

/** Remove the directory \a path. If \a force is true,
		remove even if the directory is not empty. Return false
		if the directory exists but could not been removed. */
CPPUTILS_EXP bool removeDir (
	String const &path,
	bool force = false
);

/** Get the temp directory. */
CPPUTILS_EXP String getTempPath (
);

/** Expand environment variables in the path name and
		return the expanded path. */
CPPUTILS_EXP String expandPathName (
	String const &path
);

/** Get the value of the environment variable \a var and return
		true if the value has been retrieved successfully. If false
		is returned, the content of \a value is undefined. */
CPPUTILS_EXP bool getEnvironmentVar (
	String const &var,
	String &value
);

/** Set the value of the environment variable \a var. */
CPPUTILS_EXP int setEnvironmentVar (
	String const &var,
	String const &value
);

/** Get all enviroment variables for a current process*/
CPPUTILS_EXP void getCurrentProcessEnviromentVars(CppUtils::StringMap& vars);


/** Return the current directory. */
CPPUTILS_EXP String getCwd (
	bool shortName
);

/** Set the current directory. */
CPPUTILS_EXP void setCwd (
	String const &dir
);


CPPUTILS_EXP void getAllFileMatches(
	String const &pattern,
	StringList &l_fileName,
	StringList &l_dirName
);


CPPUTILS_EXP void getAllFileMatchesRecursive(
	String const &basePath,
	StringList &l_fileName
);


/** Get the name of this host. */
CPPUTILS_EXP String getHostName (
);

/** Get string identifying the OS & version. */
CPPUTILS_EXP String getOsInfo (
);


/** Expands file name from the complete path */
CPPUTILS_EXP String expandFileName(String const &path);

/** Expands directory from complete path (/ is added to the end)*/
CPPUTILS_EXP String expandDirName(String const &path);

/** Returns executable starup directory*/
CPPUTILS_EXP String getStartupDir();

/** Get file size */
CPPUTILS_EXP long getFileSize(FileHandle filehandle);

/** Open file in different modes */
CPPUTILS_EXP FileHandle openFile(String const &filename, DWORD access, DWORD share, DWORD disp);

/** Close file */
CPPUTILS_EXP bool closeFile(FileHandle filehandle);

/** Lock file */
CPPUTILS_EXP bool exLockFile(FileHandle filehandle);

/** Unlock file */
CPPUTILS_EXP void exUnlockFile(FileHandle filehandle);

/** Set the file position from begin of file*/
CPPUTILS_EXP bool positionFromBegin(FileHandle filehandle, LongLong offset, bool rewritecontent);

/** Set the file position from end of file */
CPPUTILS_EXP bool positionFromEnd(FileHandle filehandle, LongLong offset, bool rewritecontent);

/** Save string content to file */
CPPUTILS_EXP bool saveContentInFile2(String const& file, String const& content);

/** Read string content from file */
CPPUTILS_EXP bool readContentFromFile2(String const& file, String & content);

/** Save string content to file */
CPPUTILS_EXP bool saveContentInFile(FileHandle handle, String const& content);

/** Read string content from file */
CPPUTILS_EXP bool readContentFromFile(FileHandle handle, String & content);

/** Save binary content to file */
CPPUTILS_EXP bool saveContentInFile2(String const& file, MemoryChunk const& content);

/** Read binary content from file */
CPPUTILS_EXP bool readContentFromFile2(String const& file, MemoryChunk & content);

/** Write to file */
CPPUTILS_EXP long writeToFile(FileHandle handle, CppUtils::Byte const* buffer, long toWrite); 

/** Write to file */
CPPUTILS_EXP long writeToFile(FileHandle handle, MemoryChunk const & content); 

/** Get current file size extended */
CPPUTILS_EXP UnsignedLongLong getFileSizeEx(FileHandle handle); 

/** Read from file from current position toRead bytes, returning number of bytes actually read */
CPPUTILS_EXP long readFromFile(FileHandle handle, CppUtils::Byte* buffer, long toRead); 

/** Read from file from current position content.length() bytes returning the actual number that was read */
CPPUTILS_EXP long readFromFile(FileHandle handle, MemoryChunk & content); 

/** Set thread execution state*/
CPPUTILS_EXP ThreadExecutionState setThreadExecutionState(ThreadExecutionState const newstate);

/** Get process open handles */
CPPUTILS_EXP int getProcessOpenHandles( );


//=============================================================
// Dynamic linking
//=============================================================

/** Open a dynamic library and return a handle. Returns NULL handle on failure. */
CPPUTILS_EXP void * dynlibOpen (
	String const &libName
);

/** Open a dynamic library and return a handle. Returns NULL handle on failure. */
CPPUTILS_EXP void * dynlibOpenEx (
	String const &libName,
	int dwFlags
);

/** Close the library with given handle. Ignored if the handle is unknown. */
CPPUTILS_EXP void dynlibClose (
	void *library
);

/** Resolve a symbol in the library with given handle. Return NULL if not found. */
CPPUTILS_EXP void * dynlibGetSym (
	void *library,
	String const &symName
);




CPPUTILS_EXP double convertOleTimeToUnix(double const& oledt);

CPPUTILS_EXP void convertOleTimeToUnix(double const& oledt, double& utime, tm& tm_struct);

CPPUTILS_EXP double convertUnixTimeToOleTime(double const& utime);

/** just returns gmt from tm */
CPPUTILS_EXP double returnGmtFromTm(tm const& candleTime);


/** Read default web browser from registry*/
CPPUTILS_EXP CppUtils::String getDefaultBrowser();

// returns true if 2 paths (dirs or files point to same objects)
CPPUTILS_EXP bool compareTwoPaths(CppUtils::String const& path1, CppUtils::String const& path2);

/** get current process executavle */
CPPUTILS_EXP CppUtils::String getProcessExecutableName();

CPPUTILS_EXP CppUtils::String getProcessNameByHandle(HANDLE pid = NULL);

/** Reports memory usage. pid = NULL means current process*/
CPPUTILS_EXP CppUtils::String reportProcessMemoryUsage(HANDLE pid = NULL);

CPPUTILS_EXP ULONGLONG subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

/** Full system CPU/memory usage report*/

CPPUTILS_EXP ResourecUsageStruct createFullSystemUsageReport32( int const delayBetweenSnapshotsSec = 5.0 );

CPPUTILS_EXP void createFullSystemUsageReport32Helper(map<int, CPUUsageStruct>& result, int* globPhysMemUsageKB, int* globVirtMemUsageKB );

CPPUTILS_EXP bool killProcess(int const pid, int exitCode);

CPPUTILS_EXP bool isProcessRunning(int const pid);

CPPUTILS_EXP bool shellExecute(HWND hwnd, CppUtils::String const& image, CppUtils::String const& cmdLine, CppUtils::String const& startupDir, int const showCmd);


//=============================================================
// OS-specific minor functions
//=============================================================
/** Return a string describing the last operating system error that occurred. */
CPPUTILS_EXP String getOSError (
);

/** Display an error message in a message box.

		\attention This call might block the program. */
CPPUTILS_EXP void displayError (
	String const &msg
);






//=============================================================
/** Helper class to set unexpected exception handler
		to display a message box and to terminate.

*/
class SetUnexpectedHandler {

private:

	/** Function to the unexpected exception handler to produce a message box. */
	static int setHandler (void);

	/** Helper var for initialization. */
	static const int foo_sm;
};

class CPPUTILS_EXP FileLockHelper 
{
public:
	FileLockHelper();
	~FileLockHelper();

	bool initialize(char const* fileName, int const desiredAccess, int const sharedMode, int const disposition);
	void unlock();

	inline unsigned long getFileSize() const
	{
			return fsize_m;
	}

	inline HANDLE getFileHandler()
	{
		return f_m;
	}

	inline String const& errorMsg() const
	{
		return lastError_m;
	}
private:
	
	HANDLE f_m;
	unsigned long fsize_m;
	OVERLAPPED overlapped_m;
	String lastError_m;
	
};

// End of namespace
};






#endif // _CPPUTILS_SYSDEP_INCLUDED
