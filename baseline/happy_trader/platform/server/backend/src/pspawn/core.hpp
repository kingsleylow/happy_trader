#ifndef CORE_HEADER_INCLUDED_PSPAWN
#define CORE_HEADER_INCLUDED_PSPAWN

#include "platform/server/backend/src/hlpstruct/hlpstruct.hpp"
#include "platform/server/backend/src/inqueue/inqueue.hpp"
#include "shared/cpputils/src/cpputils.hpp"


class CoreSpawner :
	public CppUtils::RefCounted, 
	private CppUtils::Mutex,
	public CppUtils::ProcessManager::Bootable 
{
public:

	static CppUtils::String const &getRootObjectName();

	CoreSpawner(int const port);
	
	~CoreSpawner();

	void threadRun();

	virtual void boot();

	
private:

	void startExecutable(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result);

	void destroyProcess(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result);

	void waitForProcessFinish(HlpStruct::XmlParameter const& command, HlpStruct::XmlParameter& result);

	void setupResult(HlpStruct::XmlParameter& result, int const resultcode, CppUtils::String const& resultString);

	void cleanup();


	// shutdown event
	CppUtils::Event toShutdown_m;

	// started event
	CppUtils::Event started_m;


	// thread object to run in background
	CppUtils::Thread* thread_m;

	// server socket
	CTCPSocket ssrvsock_m;



	

	int port_m;

	HlpStruct::ParametersHandler paramsProc_m;

	map<CppUtils::String, CppUtils::Process*> processes_m;
};

#endif