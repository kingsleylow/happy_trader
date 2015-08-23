#include "bootmanager.hpp"
#include "mtproxymanager.hpp"

#define BOOTMGR "BOOTMGR"

namespace MtQueue {
	const int BootManager::foo_scm = BootManager::boot();

	int BootManager::boot()
	{

		CppUtils::String curProcess = CppUtils::getProcessExecutableName();

		CppUtils::String path_s = CppUtils::expandDirName(curProcess);
		CppUtils::String path_log_s = path_s;
		CppUtils::makeDir(path_log_s);

		// set up 
		CppUtils::Log::discardDebugOutput();
		CppUtils::Log::setupLogDetails(path_log_s.c_str());


		//LOG(MSG_INFO, BOOTMGR, "Successfully booted");

		return 42;
	}


};
