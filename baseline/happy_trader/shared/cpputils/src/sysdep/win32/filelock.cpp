#include "../../filelock.hpp"


namespace CppUtils {

	FileLock::FileLock(CppUtils::FileHandle fhndl)
	{
		memset(&overlapped_m, '\0', sizeof(OVERLAPPED));
		hndl_m = fhndl;
	
		BOOL res = ::LockFileEx(hndl_m, LOCKFILE_EXCLUSIVE_LOCK,0,0,0xffff0000,&overlapped_m);
		if (res == 0) {
			THROW(CppUtils::ResourceFailure, "exc_CannotLockFile", "ctx_FileLock", CppUtils::getOSError());
		}
	}  

	FileLock::~FileLock()
	{
		::UnlockFileEx(hndl_m,0,0,0xffff0000,&overlapped_m);
	}


}; //end of namespace
