#include "pipewrap.hpp"
#include "../../except.hpp"

namespace CppUtils {

	
	//

	PipeClient::PipeClient():
		hPipe_m(NULL)
	{
	}

	PipeClient::~PipeClient()
	{
		// disconnect if any
		disconnect();
	}	

	// connect to named pipe
	bool PipeClient::connect(CppUtils::String const& pipename, int attempts, int const tsecs)
	{
		if (hPipe_m==NULL) {
			int cnt = 0;
			while ( cnt <= attempts  ) { 
				cnt++;

				hPipe_m = ::CreateFile( 
         pipename.c_str(),   // pipe name 
         GENERIC_READ |  // read and write access 
         GENERIC_WRITE, 
         0,              // no sharing 
         NULL,           // default security attributes
         OPEN_EXISTING,  // opens existing pipe 
         0,              // default attributes 
         NULL);          // no template file 
 
				// Break if the pipe handle is valid. 
				if (hPipe_m != INVALID_HANDLE_VALUE) {
					// The pipe connected; change to message-read mode. 
					DWORD dwMode = PIPE_READMODE_MESSAGE; 
					BOOL fSuccess = SetNamedPipeHandleState( 
						hPipe_m,    // pipe handle 
						&dwMode,  // new pipe mode 
						NULL,     // don't set maximum bytes 
						NULL);    // don't set maximum time 
					
					if (!fSuccess) {
						LOG(MSG_WARN, "PIPECLIENT", "Pipe cannot change its state: " << CppUtils::getOSError());

						disconnect();
						return false;
					}
					
					return true; 
				}
 
				// Exit if an error other than ERROR_PIPE_BUSY occurs. 
				
				if (::GetLastError() != ERROR_PIPE_BUSY)   {
					LOG(MSG_WARN, "PIPECLIENT", "Error opening pipe: " << CppUtils::getOSError());

					disconnect();
					return false;
				}
 
				// All pipe instances are busy, so wait for 20 seconds. 
				if (! ::WaitNamedPipe(pipename.c_str(), tsecs * 1000) ) {
					LOG(MSG_WARN, "PIPECLIENT", "All pipes are busy: " << CppUtils::getOSError());

					disconnect();
					return false;
				}
			};

		};

		// return true assuming conected
		return true;
		
	};

	// disconnects from pipe client
	void PipeClient::disconnect()
	{
			::CloseHandle(hPipe_m); 
			hPipe_m=NULL;
	}

	// makes transaction against pipe server
	bool PipeClient::makePipeTransaction(MemoryChunk const& request, MemoryChunk& response)
	{
		response.length(0);
		if (hPipe_m != NULL) {
			
			DWORD cbRead;
			Byte chReadBuf[BUFSIZE]; 
			

			DWORD writeSize = request.length();
			BOOL fSuccess = ::TransactNamedPipe( 
				hPipe_m,                  // pipe handle 
				(void*)request.data(),     // message to server
				writeSize,							// message length 
				chReadBuf,              // buffer to receive reply
				BUFSIZE,  // size of read buffer
				&cbRead,                // bytes read
				NULL);                  // not overlapped 

			if (!fSuccess && (::GetLastError() != ERROR_MORE_DATA))  {
				LOG(MSG_WARN, "PIPECLIENT", "Pipe cannot make transaction: " << CppUtils::getOSError());
				return false;
			}

			if (cbRead >0 ) {
				// copy
				response.length(cbRead);
				memcpy(response.data(), chReadBuf, cbRead);
			}

			// continue
			while( true ) { 
      

				// Break if TransactNamedPipe or ReadFile is successful
				if(fSuccess)
					return true;

				// Read from the pipe if there is more data in the message.
				fSuccess = ReadFile( 
					hPipe_m,      // pipe handle 
					chReadBuf,  // buffer to receive reply 
					BUFSIZE,  // size of buffer 
					&cbRead,  // number of bytes read 
					NULL);    // not overlapped 

				if (cbRead >0 ) {
					// copy
					int oldlen = response.length();
					response.length(oldlen + cbRead);
					Byte* ptr= response.data() + oldlen;
					
					memcpy(ptr, chReadBuf, cbRead);
				}

				// Exit if an error other than ERROR_MORE_DATA occurs.
				if( !fSuccess && (GetLastError() != ERROR_MORE_DATA)) 
					return false;
				
			}; // end loop


		}

		return false;
	};

	// writes a number of data to named pipe
	bool PipeClient::writeToPipe(MemoryChunk const& request )
	{
		if (hPipe_m == NULL) 
			return false;

		BOOL fSuccess;
		DWORD written;
		
		fSuccess = WriteFile( 
								hPipe_m, 
								request.data(), 
								request.length(), 
								&written, 
								NULL); 

		if (!fSuccess || request.length() != written) {
			LOG(MSG_WARN, "PIPECLIENT", "Pipe cannot write: " << CppUtils::getOSError());
			return false;
		}
		
		return true;
	}


	/////////////////////////////////////////

	int workingThread(void* ctx) {
		PipeServer* pThis = (PipeServer*)ctx;
		pThis->workingFunction();

		return 42;
	}
			
	PipeServer::PipeServer():
	maxInstances_m(-1),
	workingThread_m(NULL),
	pipeTout_m(-1),
	isSyncronius_m(false)
	{
		// here so far there s idle instance	
	}

	bool PipeServer::start(bool isSynch, int const maxinstances, CppUtils::String const& pipename, int const pipetout)
	{
		if (workingThread_m == NULL) {

			// if true call back fun doesnt work
			isSyncronius_m = isSynch;
			pipeName_m = pipename;
			maxInstances_m = maxinstances;
			pipeTout_m = pipetout;

			if (maxInstances_m <0) {
				LOG(MSG_WARN, "PIPESERVER", "Invalid pipe instance count: " << maxInstances_m);
				return false;
			}

			pipe_m = new PIPEINST[maxInstances_m];
			hEvents_m = new HANDLE[maxInstances_m+1];

			// create pipes
			
			// shutdown event
			hEvents_m[maxInstances_m] = CreateEvent( 
					NULL,    // default security attribute 
					TRUE,    // manual-reset event 
					FALSE,    // initial state = non - signaled 
					NULL);

			if (hEvents_m[maxInstances_m] == NULL) {
					LOG(MSG_WARN, "PIPESERVER", "Invalid event created: " << CppUtils::getOSError());
					return false;
			}

			// reset event if signalled
			if (dataArrived_m.lock(0))
				dataArrived_m.clearEvent();

			// reset this event
			if (canContinue_m.lock(0))
				canContinue_m.clearEvent();
			
			// start working thread
			workingThread_m = new Thread(workingThread, this);
			return true;
		}
		return false;
	}

	void PipeServer::stop()
	{
		if (workingThread_m != NULL) {
			// shutdown event
			::SetEvent(hEvents_m[maxInstances_m]);
		
			workingThread_m->waitForThread();
			delete workingThread_m;
			LOG(MSG_INFO, "PIPESERVER", "Pipe processing thread finishined");

			workingThread_m = NULL;

			// clear other stuff
			// clear events
			// clear shutdown event
			::CloseHandle(hEvents_m[ maxInstances_m ]);
		

		
			// clear buffers
			delete [] hEvents_m;
			delete [] pipe_m;
		}
	}

  PipeServer::~PipeServer()
	{
		// stop if any
		stop();
	}
	
	void PipeServer::workingFunction()
	{
		try {
				for (int i = 0; i < maxInstances_m; i++) { 
	 
				// Create an event object for this instance. 
	 
				hEvents_m[i] = CreateEvent( 
					NULL,    // default security attribute 
					TRUE,    // manual-reset event 
					TRUE,    // initial state = signaled 
					NULL);   // unnamed event object 

				if (hEvents_m[i] == NULL) {
					THROW(CppUtils::OperationFailed, "exc_InvalidEventCreated", "ctx_PipeWorkingThread", CppUtils::getOSError());
				}
	 
				// initialize overlap structure
				memset(&pipe_m[i].oOverlap, 0, sizeof(OVERLAPPED));
				// assign event here
				pipe_m[i].oOverlap.hEvent = hEvents_m[i]; 
	 
				pipe_m[i].hPipeInst = CreateNamedPipe( 
					pipeName_m.c_str(),            // pipe name 
					PIPE_ACCESS_DUPLEX |     // read/write access 
					FILE_FLAG_OVERLAPPED,    // overlapped mode 
					PIPE_TYPE_MESSAGE |      // message-type pipe 
					PIPE_READMODE_MESSAGE |  // message-read mode 
					PIPE_WAIT,               // blocking mode 
					maxInstances_m,               // number of instances 
					BUFSIZE,   // output buffer size 
					BUFSIZE,   // input buffer size 
					pipeTout_m,            // client time-out 
					NULL);                   // default security attributes 

				if (pipe_m[i].hPipeInst == INVALID_HANDLE_VALUE)  {
					THROW(CppUtils::OperationFailed, "exc_InvalidPipeCreated", "ctx_PipeWorkingThread", CppUtils::getOSError());
				}

				// tries clients to connect
				pipe_m[i].fPendingIO = connectToNewClient( 
					pipe_m[i].hPipeInst, 
					&pipe_m[i].oOverlap); 
	 
				pipe_m[i].dwState = pipe_m[i].fPendingIO ? 
					CONNECTING_STATE : // still connecting 
					READING_STATE;     // ready to read 
				
				

			}; // end loop



			BOOL fSuccess;
			DWORD cbRet, dwErr;
			
			while (true) {
				//try {
					// Wait for the event object to be signaled, indicating 
					// completion of an overlapped read, write, or 
					// connect operation. 
	 
					DWORD dwWait = WaitForMultipleObjects( 
						maxInstances_m+1,    // number of event objects 
						hEvents_m,      // array of event objects 
						FALSE,        // does not wait for all 
						INFINITE);    // waits indefinitely 
	 
					// dwWait shows which pipe completed the operation. 
	 
					DWORD i = dwWait - WAIT_OBJECT_0;  // determines which pipe 
					if (i < 0 || i > maxInstances_m)  {
						THROW(CppUtils::OperationFailed, "exc_IndexOutOfRange", "ctx_PipeWorkingThread", (long)i);
					}
					if (i==maxInstances_m) {
						// shutdown event
						break;
					}
	 
					// Get the result if the operation was pending. 
	 
					if (pipe_m[i].fPendingIO) { 
						
						fSuccess = ::GetOverlappedResult( 
							pipe_m[i].hPipeInst, // handle to pipe 
							&pipe_m[i].oOverlap, // OVERLAPPED structure 
							&cbRet,            // bytes transferred 
							FALSE);            // do not wait 
	 
						switch (pipe_m[i].dwState) { 
							// Pending connect operation 
							case CONNECTING_STATE: 
								if (!fSuccess)  {
									THROW(CppUtils::OperationFailed, "exc_ExceptionOnConnecting", "ctx_PipeWorkingThread", CppUtils::getOSError());
								}
								// reset read buffer
								pipe_m[i].reqChunk.length(0);
								 

								pipe_m[i].dwState = READING_STATE; 
								break; 
	 
							// Pending read operation 
							case READING_STATE: 
														
								dwErr = GetLastError();
	
								if (dwErr != ERROR_MORE_DATA) {
									if (!fSuccess || cbRet == 0)  { 
										disconnectAndReconnect(i); 
										continue; 
									}	 

									addToReadBuffer(i, cbRet );

								}
								else {

									addToReadBuffer(i, cbRet );

									while (true) {
										
										if (fSuccess)
											break;

										fSuccess = ReadFile( 
											pipe_m[i].hPipeInst, 
											pipe_m[i].chRequest, 
											BUFSIZE, 
											&pipe_m[i].cbRead, 
											&pipe_m[i].oOverlap); 

										// Exit if an error other than ERROR_MORE_DATA occurs.
										if( !fSuccess && (GetLastError() != ERROR_MORE_DATA)) 
											break;

										fSuccess = ::GetOverlappedResult( 
											pipe_m[i].hPipeInst, // handle to pipe 
											&pipe_m[i].oOverlap, // OVERLAPPED structure 
											&cbRet,            // bytes transferred 
										FALSE);      

										//if (!fSuccess || cbRet == 0)  
										//	break;

										addToReadBuffer(i, cbRet );

										
										
									}
								}

								

								pipe_m[i].dwState = WRITING_STATE; 
								break; 
	 
							// Pending write operation 
							case WRITING_STATE: 
								if (! fSuccess || cbRet != pipe_m[i].cbToWrite) { 
										disconnectAndReconnect(i); 
										continue; 
								} 

								// reset read buffer
								pipe_m[i].reqChunk.length(0);

								pipe_m[i].dwState = READING_STATE; 
								break; 
	 
							default: 
							{
								THROW(CppUtils::OperationFailed, "exc_InvalidPipeState", "ctx_PipeWorkingThread", "");
							}
					}; // end switch  
				}; // end if pending
	 
				// The pipe state determines which operation to do next. 
	 
				switch (pipe_m[i].dwState)   { 
						// READING_STATE: 
						// The pipe instance is connected to the client 
						// and is ready to read a request from the client. 
	 
					case READING_STATE: 
							pipe_m[i].cbRead = BUFSIZE;
							fSuccess = ReadFile( 
								pipe_m[i].hPipeInst, 
								pipe_m[i].chRequest, 
								BUFSIZE, 
								&pipe_m[i].cbRead, 
								&pipe_m[i].oOverlap); 

							

	 
							// The read operation completed successfully. 
							if (fSuccess && pipe_m[i].cbRead != 0) { 
								pipe_m[i].fPendingIO = FALSE; 

								addToReadBuffer(i, pipe_m[i].cbRead );
								pipe_m[i].dwState = WRITING_STATE; 
								continue; 
							} 
	 
					// The read operation is still pending. 
	 
							dwErr = GetLastError(); 
							if (! fSuccess && (dwErr == ERROR_IO_PENDING)) { 
								pipe_m[i].fPendingIO = TRUE; 
								continue; 
							} 
	 
							// An error occurred; disconnect from the client. 
	 
							disconnectAndReconnect(i); 
							break; 
	 
							// WRITING_STATE: 
							// The request was successfully read from the client. 
							// Get the reply data and write it to the client. 
	 
					case WRITING_STATE: 
						pipe_m[i].replChunk.length(0);
						
						// call internal processing handler
						onClientRequestArrivedInternal(i, pipe_m[i].reqChunk, pipe_m[i].replChunk);
									
						pipe_m[i].cbToWrite = pipe_m[i].replChunk.length();
						fSuccess = WriteFile( 
								pipe_m[i].hPipeInst, 
								pipe_m[i].replChunk.data(), 
								pipe_m[i].cbToWrite, 
								&cbRet, 
								&pipe_m[i].oOverlap); 
	 
							// The write operation completed successfully. 
	 
							if (fSuccess && cbRet == pipe_m[i].cbToWrite)  { 
								pipe_m[i].fPendingIO = FALSE; 
								pipe_m[i].dwState = READING_STATE;

								// reset read buffer
								pipe_m[i].reqChunk.length(0);
								continue; 
							} 
	 
							// The write operation is still pending. 
	 
							dwErr = GetLastError(); 
							if (! fSuccess && (dwErr == ERROR_IO_PENDING))  { 
								pipe_m[i].fPendingIO = TRUE; 
								continue; 
							} 
	 
							// An error occurred; disconnect from the client. 
	 
							disconnectAndReconnect(i); 
							break; 
	 
					default: 
					{            
							THROW(CppUtils::OperationFailed, "exc_InvalidPipeState", "ctx_PipeWorkingThread", "");
					}
					}; // end  switch pipe state
			
				

				//}
				//catch(CppUtils::Exception& e2) {
				//	LOG( MSG_WARN, "SERVER", "Exception on pipe server work inside loop: " << e2.message() );
				//}
				//catch(...) {
				//	LOG( MSG_WARN, "SERVER", "Unknown exception on pipe server work inside loop");
				//}

			}; // end main loop 
  
		}
		catch(CppUtils::Exception& e) {
			LOG( MSG_WARN, "PIPESERVER", "Exception on pipe server work outside loop: " << e.message() );
		}
		catch(...) {
			LOG( MSG_WARN, "PIPESERVER", "Unknown exception on pipe server work outside loop");
		}

		// clear events
		for (int i = 0; i < maxInstances_m; i++) {
			::CloseHandle(hEvents_m[ i ]);
		}

		// deinitialize pipes
		for (int i = 0; i < maxInstances_m; i++) {
			::FlushFileBuffers(pipe_m[ i ].hPipeInst);
			::DisconnectNamedPipe(pipe_m[ i ].hPipeInst);
			::CloseHandle(pipe_m[ i ].hPipeInst);
		}

		LOG(MSG_INFO, "PIPESERVER", "Pipe processing thread finishing...");

	}
	
	

	/**
	 Helpers
	*/

	void PipeServer::disconnectAndReconnect(DWORD i)
	{
		LOG(MSG_WARN, "PIPESERVER", "Error occured working with pipe: " << CppUtils::getOSError());

		// Disconnect the pipe instance. 
 		if (!::DisconnectNamedPipe(pipe_m[i].hPipeInst) )  {
			THROW(CppUtils::OperationFailed, "exc_disconnectPipeFailed", "ctx_disconnectAndReconnect", CppUtils::getOSError());
		}
 
		// Call a subroutine to connect to the new client. 
 
		pipe_m[i].fPendingIO = connectToNewClient( 
      pipe_m[i].hPipeInst, 
      &pipe_m[i].oOverlap); 
 
		pipe_m[i].dwState = pipe_m[i].fPendingIO ? 
      CONNECTING_STATE : // still connecting 
      READING_STATE;     // ready to read 

	}

	BOOL PipeServer::connectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
	{
		BOOL fConnected, fPendingIO = FALSE; 
 
		// Start an overlapped connection for this pipe instance. 
		fConnected = ::ConnectNamedPipe(hPipe, lpo); 
 
		// Overlapped ConnectNamedPipe should return zero. 
		if (fConnected) {
			THROW(CppUtils::OperationFailed, "exc_ConnectNamedPipeFailed", "ctx_connectToNewClient", CppUtils::getOSError());
		}
 
		switch (GetLastError())   { 
			// The overlapped connection in progress. 
      case ERROR_IO_PENDING: 
         fPendingIO = TRUE; 
         break; 
 
			// Client is already connected, so signal an event. 
      case ERROR_PIPE_CONNECTED: 
         if (SetEvent(lpo->hEvent)) 
            break; 
 
			// If an error occurs during the connect operation... 
      default:  {
         THROW(CppUtils::OperationFailed, "exc_ConnectNamedPipeFailed", "ctx_connectToNewClient", CppUtils::getOSError());
         return 0;
      }
		}; 
 
		return fPendingIO; 

	}

	void PipeServer::addToReadBuffer(const int pipeIdx, const int sz)
	{
		if (sz > 0) {
			int oldlen = pipe_m[pipeIdx].reqChunk.length();
			pipe_m[pipeIdx].reqChunk.length(oldlen + sz);
			memcpy(pipe_m[pipeIdx].reqChunk.data() + oldlen, pipe_m[pipeIdx].chRequest, sz);
		}
	}
	
	void PipeServer::onClientRequestArrivedInternal(int const instId, MemoryChunk const& request, MemoryChunk& response)
	{
		// synch mode allowed we are using synch processing
		if (isSyncronius_m) {

			tmpPipeIdx_m  = instId;
			tmpRequest_m = request;

			
			// release data available lock
			dataArrived_m.signalEvent();

			// freese whether we can continue
			canContinue_m.lock();

			// send this back
			response = tmpResponse_m;

		} else {
			// call derived
			onClientRequestArrived(instId, request, response);
		}
	}

	
	void PipeServer::waitForRemoteRequest(int& instId, bool& waitresult, CppUtils::MemoryChunk& request, int msec)
	{
		if (!isSyncronius_m) {
			waitresult = false;
			return;
		}

		waitresult = dataArrived_m.lock(msec);
		instId = tmpPipeIdx_m;
		request = tmpRequest_m;

		dataArrived_m.clearEvent();
	}

	
	void PipeServer::sendResponseBack(int const instId,CppUtils::MemoryChunk const& response)
	{	
		if (!isSyncronius_m) 
			return;
		

		tmpResponse_m = response;
		tmpPipeIdx_m = instId;
		
		// signal to continue processing
		canContinue_m.signalEvent();

	}



}; // end of namespace