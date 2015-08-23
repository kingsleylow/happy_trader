
#ifndef _CPPUTILSS_PROCESS_INCLUDED
#define _CPPUTILSS_PROCESS_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


//=============================================================
// Class Process
//=============================================================
/** Class Process is an abstraction from OS-specific process
		implementations. It spawns a new process and offers functionality
		to control execution	of that process.

*/
class CPPUTILS_EXP Process {

public:

	//======================================
	// Con- & Destructor

	/** Ctor. Creates a new process. */
	Process (
		String const &image,
		String const &commandline,
		String const &currentDir,
		bool redirect,
		bool hidden,
		bool detached = false
	);

	/** Dtor. Waits for the process to terminate.

			\attention This call blocks until the process terminates.
	*/
	~Process(
	);


	//======================================
	// Methods

	/** Returns the OS-specific process handle. */
	ProcessHandle const & getHandle (
	) const;

	int getPid() const;
	

	/** Returns the result code that the process has returned. 
			If the process has not completed yet, this function returns -1. */
	int getResult (
	) const;

	/** Returns true if the spawned process has terminated. */
	bool hasTerminated (
	) const;

	/** Kills the spawned process from another process.
	
			\warning Extremely dangerous function, as all
			sorts of chaos can happen.
	*/
	void killProcess (
	);

	/** Blocks until the spawned process has finished or until the specified
			time has elapsed; block infinitly if the argument is < 0. Raises
			ExceptionInternal if the process cannot be accessed. Returns true
			if the process has stopped.
	*/
	bool waitForProcess (
		double secs = -1
	) const;

	/** Return the stdout of the process; returns
			the empty string if redirection is not used. */
	void getStdOut (
		String &str
	) const;

	/** Return the stderr of the process; returns
			the empty string if redirection is not used. */
	void getStdErr (
		String &str
	) const;

	

private:

	//======================================
	// Creators

	/** Private, unimplemented ctor, */
	Process (
		Process const &
	);



	//======================================
	// Variables

	/** Native handle of the process. */
	ProcessHandle processHandle_m;

	/** Files used to redirect stdout. */
	FileHandle sout_m;

	/** Files used to redirect stderr. */
	FileHandle serr_m;

	/** Name of stdout file. */
	String soutName_m;

	/** Name of stderr file. */
	String serrName_m;

	// delete on dtor
	bool detached_m;
};





// End of namespace
}


#endif // _CPPUTILSS_PROCESS_INCLUDED
