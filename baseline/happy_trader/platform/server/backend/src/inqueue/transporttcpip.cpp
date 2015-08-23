#include "transporttcpip.hpp"

namespace Inqueue {
	HtServerSocket* HtServerSocket_TCPIP::createTCPIPSocket()
	{
		return new HtServerSocket_TCPIP();
	}

	HtServerSocket_TCPIP::HtServerSocket_TCPIP()
	{
	}

	HtServerSocket_TCPIP::~HtServerSocket_TCPIP()
	{
	}

	BOOL HtServerSocket_TCPIP::Shutdown(CSocket::SocketShutdown eHow)
	{
		return socket_m.Shutdown(eHow);
	}

	BOOL HtServerSocket_TCPIP::Close()
	{
		return socket_m.Close();
	}

	BOOL HtServerSocket_TCPIP::Create()
	{
		return socket_m.Create();
	}

	BOOL HtServerSocket_TCPIP::Bind(const std::string& rSourceAddress,	unsigned short usPort)
	{
		return socket_m.Bind(rSourceAddress, usPort);
	}

	int HtServerSocket_TCPIP::GetLastError() const
	{
		return socket_m.GetLastError();
	}

	BOOL HtServerSocket_TCPIP::Listen(unsigned long ulBackLog)
	{
		return	socket_m.Listen(ulBackLog);
	}

	BOOL HtServerSocket_TCPIP::Accept(HtClientSocket* clientSocket)
	{
		HtClientSocket_TCPIP* p = (HtClientSocket_TCPIP*)clientSocket;
		return socket_m.Accept(p->getClientSocket());
	}

	// ---------------


	HtClientSocket_TCPIP::HtClientSocket_TCPIP()
	{
	}

	HtClientSocket_TCPIP::~HtClientSocket_TCPIP()
	{
	}

	int HtClientSocket_TCPIP::Send(const char *pBuffer, unsigned long ulBufferLength)
	{
		return clientSocket_m.Send(pBuffer, ulBufferLength);
	}

	BOOL HtClientSocket_TCPIP::Shutdown(CSocket::SocketShutdown eHow)
	{
		return	clientSocket_m.Shutdown(eHow);
	}

	BOOL HtClientSocket_TCPIP::Close()
	{
		return	clientSocket_m.Close();
	}

	long HtClientSocket_TCPIP::GetPeerAddress() const
	{
		return clientSocket_m.GetPeerAddress();
	}

	int HtClientSocket_TCPIP::Receive(char *pBuffer, unsigned long ulBufferLength)
	{
		return clientSocket_m.Receive(pBuffer, ulBufferLength);
	}

	CTCPSocket* HtClientSocket_TCPIP::getClientSocket()
	{
			return &clientSocket_m;
	}

	BOOL HtClientSocket_TCPIP::SetNagle(BOOL bNagle)
	{
		return clientSocket_m.SetNagle(bNagle);
	}

	int HtClientSocket_TCPIP::GetLastError() const
	{
		return clientSocket_m.GetLastError();
	}
		
	HtClientSocket* HtClientSocket_TCPIP::createTCPIPSocket()
	{
		return new	HtClientSocket_TCPIP();
	}


}