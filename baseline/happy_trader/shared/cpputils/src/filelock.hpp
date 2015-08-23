#ifndef _CPPUTILS_FILELOCK_INCLUDED
#define _CPPUTILS_FILELOCK_INCLUDED


#include "cpputilsdefs.hpp"
#include "except.hpp"

#include "sysdep/sysdep.hpp"





namespace CppUtils {

	class CPPUTILS_EXP FileLock
	{
		public:
			FileLock(CppUtils::FileHandle fhndl);

			~FileLock();
		
		private:

			// overlapped structure
			OVERLAPPED overlapped_m;

			HANDLE hndl_m;

	};

	
}; //end of namespace

#define FILE_LOCK_FOR_SCOPE(HNDL)	CppUtils::FileLock _hndl_scoped_lock(HNDL);

#endif


