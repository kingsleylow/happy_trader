#ifndef _BACKEND_INQUEUE_TRANSPORTTCPIP_INCLUDED
#define _BACKEND_INQUEUE_TRANSPORTTCPIP_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "../brklib/brklib.hpp"
#include "transportbase.hpp"

namespace Inqueue {

	/**
	*	Implementation
	*/
	class HtServerSocket_TCPIP: public HtServerSocket{
	public:
		HtServerSocket_TCPIP();

		virtual ~HtServerSocket_TCPIP();
		virtual BOOL Shutdown(CSocket::SocketShutdown eHow);
		virtual BOOL Close();
		virtual BOOL Create();
		virtual BOOL Bind(const std::string& rSourceAddress,	unsigned short usPort);
		virtual int GetLastError() const;
		virtual BOOL Listen(unsigned long ulBackLog=5);
		virtual BOOL Accept(HtClientSocket* clientSocket);

		static HtServerSocket* createTCPIPSocket();

	private:
		CTCPSocket socket_m;
	};

	class HtClientSocket_TCPIP: public HtClientSocket {
		friend class HtServerSocket_TCPIP;
	public:
		HtClientSocket_TCPIP();

		virtual ~HtClientSocket_TCPIP();

		virtual int Send(const char *pBuffer, unsigned long ulBufferLength);
		virtual BOOL Shutdown(CSocket::SocketShutdown eHow);
		virtual BOOL Close();
		virtual long GetPeerAddress()const;
		virtual int Receive(char *pBuffer, unsigned long ulBufferLength);
		virtual BOOL SetNagle(BOOL bNagle);
		virtual int GetLastError()const;

		static HtClientSocket* createTCPIPSocket();

	private:
		CTCPSocket* getClientSocket();
		

	private:
		CTCPSocket clientSocket_m;

	};
};

#endif