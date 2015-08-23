#ifndef _MTQUEUE_MTPROXYMANAGER_INCLUDED
#define _MTQUEUE_MTPROXYMANAGER_INCLUDED

#include "mtqueuedefs.hpp"
#include "helperstruct.hpp"
#include "transport.hpp"
#include <queue>
#include "handlers.hpp"



namespace MtQueue {
	// base class for iner-pipe communications to make MT4 communication
	

	// -----------------------------

	class ResponseHandler: public HandlerBase
	{
	public:
		ResponseHandler();

		// make shallow copy!
		ResponseHandler(ResponseHandler const& src);

		virtual ~ResponseHandler();

		virtual bool isValid() const;

		// clear resources and invalidate
		virtual void clear();

		void init(Result const& result);

		Result* getResultPtr();
	
	private:
		
		Result* result_m;

	};

	// --------------------------------
	// 
	class EnviromentHandler: public HandlerBase
	{
	public:
		EnviromentHandler();

		// shallow copy only
		EnviromentHandler(EnviromentHandler const& src);

		virtual ~EnviromentHandler();

		virtual bool isValid() const;

		// clear resources and invalidate
		virtual void clear();

		void init();

		TransportLayer* getTransport();

		HandlerStorage<ResponseHandler>* getResponseHandlers();
		
	private:
		TransportLayer* transport_m;
		
		HandlerStorage<ResponseHandler>* responseHandlers_m;
	};

	// --------------------------


	
	// --------------------------

	class MtQueueManager:  public CppUtils::Mutex {
	public:
		

		// ctor & dtor
		MtQueueManager();

		~MtQueueManager();

		int initEnv();

		void makeScreenShot();

		void setEnvParams(int const envHndl, char const* key, char const* secret, char const* apiUrl = NULL);

		// returs response handler
		void btc_getInfo(int const envHndl, int& newId);

		// general call
		void btc_generalCall(int const envHndl, char const* nonce, char const* method, char const* parameters, int& newId);

		void freeEnv(int const envHndlr);

		void freeResponse(int const envHndlr, int const respHndlr);

		void respGetError(int const envHndlr, int const respHndlr, int& statusCode, CppUtils::String& statusMessage);

		void respStartMessageRead(int const envHndlr, int const respHndlr, int& messageLength);

		// reads up to bufferlength chars to buffer
		// returns actual bytes reead
		void respReadMessage(int const envHndlr, int const respHndlr, CppUtils::Byte* buffer, int const bufferlength, int& readBytes);
		
	private:

		ResponseHandler getResponseHandlerHelper(int const envHndlr, int const respHndlr);

		HandlerStorage<EnviromentHandler> envHandlers_m;


	};

};

#endif
