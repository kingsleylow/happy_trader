
#include "platform/server/backend/src/brklibmetatrader/brklibmetatrader.hpp"

#define ENGINE_CREATOR_BROKER_FUNCTION_NAME "createBroker" 
#define ENGINE_DESTROYER_BROKER_FUNCTION_NAME "destroyBroker" 


// =======================================
// This is more complete test test involving broker functions

TESTCASE_BEGIN( test2 )

// =======================================================
	// Setup
	CppUtils::PipeClient pipeclient;
	
	class TestServer: public CppUtils::PipeServer {
	public:
	
	};

	TestServer pipeserver;

	TESTCASE_SETUP
	{
		bool res = pipeserver.start(true, 1, "\\\\.\\pipe\\mt4_client_pipe");
		ASSERT(res);
	}

	// =======================================================
	// Wrapup
	TESTCASE_WRAPUP
	{
		pipeserver.stop();
	}


DEFINE_TEST( T1 ) 
{
	

}

TESTCASE_INDEX( test2 )
{
		REGISTER_TEST( test2, T1 );
}


TESTCASE_END;