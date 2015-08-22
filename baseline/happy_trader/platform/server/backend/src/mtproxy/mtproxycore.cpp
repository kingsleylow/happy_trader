#include "mtproxycore.hpp"
#include "mtproxymanager.hpp"



#define MTPROXYCORE "MTPROXYCORE"

#define TRY \
	try {

#define CATCH_ALL(FUNNAME)	\
	return MTST_NO_ERROR;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception in: [" << #FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception in: [" << #FUNNAME << "] " );\
	}\
	return MTST_GENERAL_ERROR;

#define CATCH_ALL_NR(FUNNAME)	\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception in: [" << #FUNNAME << "] " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception in: [" << #FUNNAME << "] " );\
	}\
	


#define CATCH_ALL_1()	\
	return MTST_NO_ERROR;\
	}\
	catch(CppUtils::Exception& e)	\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Exception: " << e.message() );\
	}\
	catch(...)\
	{\
		LOG(MSG_ERROR, MTPROXYCORE, "Unknown exception" );\
	}\
	return MTST_GENERAL_ERROR;


int dieInPeriodSecs (void* dieSecP)
{
	if (dieSecP) {
		DWORD dieSecI = (DWORD) dieSecP;
		LOG(MSG_DEBUG, MTPROXYCORE, "Killer thread is going to abort in: "<<  dieSecI << " seconds");
		CppUtils::sleep( dieSecI );

		
		abort();
	}

	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                                         )
{
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
					
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
                break;
        }
        return TRUE;
}


namespace MtProxy {

};

extern "C" {

	// ------------------
int PROCCALL getVersion()
{
	return MTPROXY_VERSION;
}


int PROCCALL initialize(int dieSec)
{
	TRY
		DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
		SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);

		if (dieSec > 0) {
			// launch dying thread
			LOG(MSG_INFO, MTPROXYCORE, "Will launch killer thread in: " << dieSec << " seconds");

			new CppUtils::Thread(dieInPeriodSecs, (void*)(DWORD(dieSec)) );
		}
	CATCH_ALL("init")
}


int PROCCALL	getTerminalUid(wchar_t* terminal_uid)
{
	TRY
		CppUtils::Uid terminalUid;
		terminalUid.generate();
		
		
		if (!terminalUid.isValid())
			return MTST_GENERAL_ERROR;

		//if (!MtProxy::MqlStrHelper::copy(terminal_uid, terminalUid.toString().c_str()))
		//	THROW(CppUtils::OperationFailed, "exc_NonSufficientStringBuffer", "ctx_getTerminalUid", "");
		
		AlgLib::MqlStrHelper::copyStringToWchar(terminal_uid, terminalUid.toString().c_str());
		
		
	CATCH_ALL("getTerminalUid")
}



MTPROXY_EXP int PROCCALL saveFileContent(wchar_t const* file, wchar_t const* content)
{
	TRY
		
		// must exclusevely lock this file for write accesss 
		CppUtils::String file_s = AlgLib::MqlStrHelper::wcharToString(file);
		CppUtils::String content_s = AlgLib::MqlStrHelper::wcharToString(content);

		LOG(MSG_DEBUG, MTPROXYCORE, "Saving file: " << file_s << " and content length: " << content_s.size() );

		// write access but expect concurrent reads
		HANDLE hFile = ::CreateFileA(file_s.c_str(),  GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
		if (hFile == INVALID_HANDLE_VALUE)
				THROW(CppUtils::OperationFailed, "exc_CannotOpenFile", "ctx_saveFileContent", file_s << " - " << CppUtils::getOSError());

		if (!CppUtils::saveContentInFile(hFile, content_s)) {
			::CloseHandle( hFile );
			THROW(CppUtils::OperationFailed, "exc_CannotSaveDataToFile", "ctx_saveFileContent", file_s << " - " << CppUtils::getOSError());
		}

		::CloseHandle( hFile );
		
	CATCH_ALL("saveFileContent")

}

MTPROXY_EXP void PROCCALL internallog(int const level, wchar_t const* content)
{
	CppUtils::String content_s = AlgLib::MqlStrHelper::wcharToString(content);
	LOG(level, MTPROXYCORE, "Internal: [ " << content_s << " ] " );
}


void PROCCALL setloglevel(int const level)
{
	if (level >=MSG_FATAL && level <= MSG_DEBUG_DETAIL) {
		CppUtils::Log::setLogLevel(level);

		LOG_FORCE(MTPROXYCORE, "Set up new log level: " << level);
	}
}



	
} // end of namespace
