#ifndef _BACKEND_INQUEUE_TRANSPORTBASE_INCLUDED
#define _BACKEND_INQUEUE_TRANSPORTBASE_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "../brklib/brklib.hpp"

namespace Inqueue {
	
	// server socket class
	// abstract must be derived
	class HtClientSocket;

	class HtServerSocket {
	public:

		HtServerSocket()
		{
		}

		virtual ~HtServerSocket()
		{
		}

		virtual BOOL Shutdown(CSocket::SocketShutdown eHow) = 0;
		virtual BOOL Close() = 0;
		virtual BOOL Create() = 0;
		virtual BOOL Bind(const std::string& rSourceAddress,	unsigned short usPort) = 0;
		virtual int GetLastError() const = 0;
		virtual BOOL Listen(unsigned long ulBackLog=5) = 0;
		virtual BOOL Accept(HtClientSocket* clientSocket) = 0;
	};

	class HtClientSocket {

	public:
		HtClientSocket()
		{
		}

		virtual ~HtClientSocket()
		{
		}

		virtual int Send(const char *pBuffer, unsigned long ulBufferLength) = 0;
		virtual BOOL Shutdown(CSocket::SocketShutdown eHow) = 0;
		virtual BOOL Close() = 0;
		virtual long GetPeerAddress()const = 0;
		virtual int Receive(char *pBuffer, unsigned long ulBufferLength)= 0;
		virtual BOOL SetNagle(BOOL bNagle) = 0;
		virtual int GetLastError()const	= 0;


	};

};

#endif