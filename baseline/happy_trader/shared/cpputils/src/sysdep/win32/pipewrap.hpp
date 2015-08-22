#ifndef _CPPUTILS_PIPEWRAP_INCLUDED
#define _CPPUTILS_PIPEWRAP_INCLUDED


#include "../../cpputilsdefs.hpp"
#include "../../chunk.hpp"
#include "../../thread.hpp"
#include "../../event.hpp"

// common defines
#define BUFSIZE 4096
#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 


/**
 Helper classes supporting single-thread client - server named pipe communication
*/

namespace CppUtils {
	/**
	 Pipe client
	*/
	class CPPUTILS_EXP PipeClient {
		public:
			// ctor & dtor
			PipeClient();

			~PipeClient();

			// connect to named pipe
			bool connect(CppUtils::String const& pipename, int attempts=3, int const tsecs=5);

			// disconnects from pipe client
			void disconnect();

			// makes transaction against pipe server
			bool makePipeTransaction(MemoryChunk const& request, MemoryChunk& response);

			// writes a number of data to named pipe
			bool writeToPipe(MemoryChunk const& request);

		private:

			
			HANDLE hPipe_m;

	};


	/**
	 Server class 
	 This is pipe server
	*/

	// typdef of pipe overlapper structure
	typedef struct 
	{ 
		OVERLAPPED oOverlap; 
		HANDLE hPipeInst; 
		Byte chRequest[BUFSIZE]; 
		DWORD cbRead;
		Byte chReply[BUFSIZE];
		DWORD cbToWrite; 
		DWORD dwState; 
		BOOL fPendingIO; 

		// buffers for reading and writing
		MemoryChunk reqChunk;
		MemoryChunk replChunk;
	} PIPEINST, *LPPIPEINST; 

	// 
	class CPPUTILS_EXP PipeServer {
		public:
			// ctor & dtor
			PipeServer();

			~PipeServer();

			// whether server is running or not 
			bool start(bool isSynch, int const maxinstances, CppUtils::String const& pipename, int const pipetout=5000);

			void stop();

			// this will ansewer on client request and must be re-implemenetd in derived classes
			void onClientRequestArrivedInternal(int const instId, MemoryChunk const& request, MemoryChunk& response);

			virtual void onClientRequestArrived(int const instId, MemoryChunk const& request, MemoryChunk& response) = 0;

			// wait for a response suspending current thread
			// deadlock can happen!
			// when you have this function returned you must call sendResponseBack
			// subsequently as this will continue working thread
			void waitForRemoteRequest(int& instId, bool& waitresult, CppUtils::MemoryChunk& request, int msec=-1);

			// send back response
			void sendResponseBack(int const instId, CppUtils::MemoryChunk const& response);

			// thread working function
			void workingFunction();

		private:
			

			BOOL connectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);

			void disconnectAndReconnect(DWORD i);

			// add info to read buffer
			void addToReadBuffer(const int pipeIdx, const int sz);

		

			// pipe name
			CppUtils::String pipeName_m;

			// the number of pipe serving instances
			int maxInstances_m;

			// array of pipe structure
			PIPEINST* pipe_m; 

			HANDLE* hEvents_m; 

			Thread* workingThread_m;

			// pipe time out
			int pipeTout_m;

			// events for synchronious work
			CppUtils::Event dataArrived_m;

			CppUtils::Event canContinue_m;

			
			int tmpPipeIdx_m;

			CppUtils::MemoryChunk tmpRequest_m;

			CppUtils::MemoryChunk tmpResponse_m;

			// if true - onClientRequestArrived doesn't work, instead use
			// waitForResponse and sendResponseBack
			// 
			bool isSyncronius_m;

						
			
	};
}

#endif