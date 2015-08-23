#include "bootmanager.hpp"
#include "mtqueuemanager.hpp"

#define BOOTMGR "BOOTMGR"

namespace MtQueue {
	const int BootManager::foo_scm = BootManager::boot();

	int BootManager::boot()
	{
		// need to set as the base path
		//str = "HT_SERVER_DIR=";

		//	_putenv( str.c_str() );
		CppUtils::String curProcess = CppUtils::getProcessExecutableName();

		CppUtils::String path_s = CppUtils::expandDirName(curProcess);
		CppUtils::String path_log_s = path_s;
		CppUtils::makeDir(path_log_s);

		// set up 
		CppUtils::Log::discardDebugOutput();
		CppUtils::Log::setupLogDetails(path_log_s.c_str());

		LOG(MSG_INFO, BOOTMGR, "Log path is : " << CppUtils::Log::getLogFileName() << ", log level: " <<  CppUtils::Log::getLogLevel() );


		return 42;
	}


};
