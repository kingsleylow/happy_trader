/*
 *  Copyright (c) 2000-2003 Barak Weichselbaum <barak@komodia.com>
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Contact info:
 * -------------
 *
 * Site:					http://www.komodia.com
 * Main contact:			barak@komodia.com
 * For custom projects, 
 * consulting, or other
 * paid services:			sales@komodia.com
 */

#include "stdafx.h"
#include "Socket.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "Array_ptr.h"

#include <memory>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CSocket_LOGNAME "CSocket"

CSocket::CSocket(BOOL bRawSocket) : CSocketBase(),
									m_bRaw(bRawSocket)
{
	try
	{
		//Set our name
		SetName(CSocket_LOGNAME);

		//Initialize the data
		InitializeIP();
	}
	ERROR_HANDLER("CSocket")
}

CSocket::CSocket(SOCKET aSocket) : CSocketBase()
{
	//Check it's a valid socket
	if (aSocket!=INVALID_SOCKET)
		ReportError("CSocket","Received invalid socket!");
	else
		try
		{
			//Set our name
			SetName(CSocket_LOGNAME);

			//Initialize the data
			InitializeIP();

			//Save the handle
			AssignSocket(aSocket);
		}
		ERROR_HANDLER("CSocket")
}

CSocket::~CSocket()
{
	try
	{
		//Close the socket
		Close();
	}
	ERROR_HANDLER("~CSocket")
}

BOOL CSocket::Create(int iProtocol)
{
	//Close the socket if open
	if (ValidSocket())
		Close();

	try
	{
		//Are we overlapped
		if (!m_bOverlapped)
		{
			//Here we create the raw socket
			if (m_bRaw || iProtocol==IPPROTO_ICMP)
				m_aSocket=socket(AF_INET,
									  SOCK_RAW,
									  iProtocol);
			else
				if (iProtocol==IPPROTO_TCP)
					m_aSocket=socket(AF_INET,
										  SOCK_STREAM,
										  iProtocol);
				else if (iProtocol==IPPROTO_UDP)
					m_aSocket=socket(AF_INET,
										  SOCK_DGRAM,
										  iProtocol);
		}
		else
		{
			//Here we create the raw socket
			if (m_bRaw || iProtocol==IPPROTO_ICMP)
				m_aSocket=WSASocket(AF_INET,
										 SOCK_RAW,
										 iProtocol,
										 NULL,
										 NULL,
										 WSA_FLAG_OVERLAPPED);
			else
				if (iProtocol==IPPROTO_TCP)
					m_aSocket=WSASocket(AF_INET,
											 SOCK_STREAM,
											 iProtocol,
											 NULL,
											 NULL,
											 WSA_FLAG_OVERLAPPED);
				else if (iProtocol==IPPROTO_UDP)
					m_aSocket=WSASocket(AF_INET,
											 SOCK_DGRAM,
											 iProtocol,
											 NULL,
											 NULL,
											 WSA_FLAG_OVERLAPPED);
		}

		//Check for socket validity
		if (m_aSocket==INVALID_SOCKET)
		{
			//Error
			SetLastError("Create");

			//Done
			return FALSE;
		}

		if (m_bRaw)
		{
			//Set that the application will send the IP header
			unsigned int iTrue=1;

			if(setsockopt(m_aSocket,
						  IPPROTO_IP,
						  IP_HDRINCL,
						  (char*)&iTrue,
						  sizeof(iTrue))==GetErrorCode())
			{
				//Check for options error
				SetLastError("Create");

				//Exit
				return FALSE;
			}
		}

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Create",FALSE)
}

void CSocket::SetProtocol(unsigned char ucProtocol)
{
	m_ucProtocol=ucProtocol;
}

int CSocket::Send(IP aDestinationAddress,
				  const char* pBuffer,
				  unsigned long ulBufferLength,
				  unsigned short usDestinationPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return GetErrorCode();

		//Is it a valid size
		if (ulBufferLength &&
			!pBuffer)
			return GetErrorCode();

		//Define the target address
		sockaddr_in aTargetAddress;
		memset(&aTargetAddress,
			   0,
			   sizeof(aTargetAddress));

		aTargetAddress.sin_family=AF_INET;
		aTargetAddress.sin_addr.s_addr=aDestinationAddress;
		aTargetAddress.sin_port=htons(usDestinationPort);

		//packet send status ?
		int iResult;

		//Set to no error
		iResult=!GetErrorCode();

		//Check if we had an error
		if (iResult!=GetErrorCode())
			//Use regular send !!!
			iResult=sendto(GetHandle(),
						   (const char*)pBuffer,
						   ulBufferLength,
						   0,
						   (sockaddr*)&aTargetAddress,
						   sizeof(aTargetAddress));
		
		//Is all OK?
		if (iResult==GetErrorCode() &&
			GetSystemLastError()!=WSAEWOULDBLOCK)
			//Set the error
			SetLastError("Send");
		
		//Done
		return iResult;
	}
	ERROR_HANDLER_RETURN("Send",GetErrorCode())
}

int CSocket::Send(const std::string& rDestinationAddress,
					   const char* pBuffer,
					   unsigned long ulBufferLength,
					   unsigned short usDestinationPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return GetErrorCode();

		return Send(inet_addr(rDestinationAddress.c_str()),
					pBuffer,
					ulBufferLength,
					usDestinationPort);
	}
	ERROR_HANDLER_RETURN("Send",GetErrorCode())
}

BOOL CSocket::ValidSocket() const
{
	return m_aSocket!=INVALID_SOCKET;
}

BOOL CSocket::Bind(IP aSourceAddress,
				   unsigned short usPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Create the local address
		sockaddr_in soSrc;

		//Set to 0
		memset(&soSrc,0,sizeof(soSrc));
		soSrc.sin_family=AF_INET;

		//Populate the connection data
		if (aSourceAddress)
			soSrc.sin_addr.s_addr=aSourceAddress;
		else
			soSrc.sin_addr.s_addr=ADDR_ANY ;

		soSrc.sin_port=htons(usPort);

		//Now we need to bind it
		if (bind(GetHandle(),
				 (sockaddr*)&soSrc,
				 sizeof(soSrc)))
		{
			//Error
			SetLastError("Bind");

			//Exit
			return FALSE;
		}
		else
			//Save the address
			m_aConnectedTo=soSrc;

		//Save new addresses
		if (aSourceAddress &&
			!m_ulSourceAddress)
			m_ulSourceAddress=aSourceAddress;

		return TRUE;
	}
	ERROR_HANDLER_RETURN("Bind",FALSE)
}

BOOL CSocket::Bind(const std::string& rSourceAddress,
						unsigned short usPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		if (rSourceAddress.empty())
			return Bind((IP)0,
						usPort);
		else
			return Bind(inet_addr(rSourceAddress.c_str()),
						usPort);
	}
	ERROR_HANDLER_RETURN("Bind",FALSE)
}

unsigned short CSocket::GetBindPort()const
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Create the local address
		sockaddr_in soSrc;

		//Buffer size
		int iSize;
		iSize=sizeof(soSrc);

		//Try to get it
		if (getsockname(GetHandle(),
						(sockaddr*)&soSrc,
						&iSize))
		{
			//Error
			SetLastError("Bind");

			//Exit
			return 0;
		}
		else
			return htons(soSrc.sin_port);
	}
	ERROR_HANDLER_RETURN("GetBindPort",0)
}

SOCKET CSocket::GetHandle()const
{
	return m_aSocket;
}

SOCKET CSocket::Detach()
{
	//Save the socket
	SOCKET aTmp;
	aTmp=m_aSocket;

	//Erase it
	m_aSocket=INVALID_SOCKET;

	//Return the original
	return aTmp;
}

BOOL CSocket::CheckSocketValid()const
{
	try
	{
		//Check if socket is invalid
		if (!ValidSocket())
		{
			//Report it
			ReportError("CheckSocketValid","Operation made on non existant socket!");

			//Exit
			return FALSE;
		}	

		//OK
		return TRUE;
	}
	ERROR_HANDLER_RETURN("CheckSocketValid",FALSE)
}

BOOL CSocket::Close()
{
	try
	{
		//Close the socket
		//Quit if not ok
		if (!ValidSocket())
			return FALSE;

		//Close it
		if (closesocket(GetHandle())==GetErrorCode())
		{
			//Error in closing ?
			SetLastError("Close");

			//Exit
			return FALSE;
		}

		//Set the socket to invalid
		m_aSocket=INVALID_SOCKET;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Close",FALSE)
}

void CSocket::SetSourceAddress(IP aSourceAddress)
{
	m_ulSourceAddress=aSourceAddress;
}

void CSocket::SetSourceAddress(const std::string& rSourceAddress)
{
	try
	{
		//Set the source address, in case we want to spoof it
		if (rSourceAddress.empty())
			m_ulSourceAddress=0;
		else
			m_ulSourceAddress=inet_addr(rSourceAddress.c_str());
	}
	ERROR_HANDLER("SetSourceAddress")
}

void CSocket::SetTTL(unsigned char ucTTL)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return;

		if(setsockopt(GetHandle(),
				      IPPROTO_IP,
				      IP_TTL,
				      (const char*)&ucTTL,
				      sizeof(ucTTL)))
			SetLastError("SetTTL");
	}
	ERROR_HANDLER("SetTTL")
}

BOOL CSocket::IsRaw() const
{
	return m_bRaw;
}

void CSocket::InitializeIP()
{
	try
	{
		//Invalid the socket
		m_aSocket=INVALID_SOCKET;

		//More invalids
		m_ulSourceAddress=0;

		//Some defaults
		m_ucTTL=64;

		//We are not overlapped
		m_bOverlapped=FALSE;

		//Not connected
		memset(&m_aConnectedTo,
			   0,
			   sizeof(m_aConnectedTo));
	}
	ERROR_HANDLER("InitializeIP")
}

void CSocket::AssignSocket(SOCKET aNewSocket,
						   unsigned char ucProtocol)
{
	try
	{
		//Sets the protocol
		m_ucProtocol=ucProtocol;

		//Binds to a socket
		m_aSocket=aNewSocket;
	}
	ERROR_HANDLER("AssignSocket")
}

int CSocket::ReceiveFrom(char* pBuffer,
						 unsigned long ulBufferLength,
						 IP& rIP,
						 unsigned short& rSourcePort)
{
	try
	{
		if (!ValidSocket() ||
			!pBuffer ||
			!ulBufferLength)
			return GetErrorCode();

		//Receive data
		int iResult;

		//Receive
		if (m_ucProtocol!=IPPROTO_TCP)
		{
			//Get the remote address
			sockaddr saConnected;

			int iTmp;
			iTmp=sizeof(saConnected);

			//Accept it
			iResult=recvfrom(GetHandle(),
							 pBuffer,
							 ulBufferLength,
							 NULL,
							 &saConnected,
							 &iTmp);

			//If OK set it
			if (iResult!=GetErrorCode())
			{
				//Address
				rIP=((sockaddr_in*)&saConnected)->sin_addr.S_un.S_addr;

				//Port
				rSourcePort=htons(((sockaddr_in*)&saConnected)->sin_port);

				//Done
				return iResult;
			}
			else
			{
				//Error
				SetLastError("Receive");

				//Reset the data
				rIP=0;
				rSourcePort=0;

				//Done
				return iResult;
			}
		}
		else
		{
			//Report it
			ReportError("Receive","Can't run on TCP socket!");

			//Exit
			return GetErrorCode();
		}
	}
	ERROR_HANDLER_RETURN("Receive",GetErrorCode())
}

int CSocket::LocalReceive(char* pBuffer,
							   unsigned long ulBufferLength,
							   BOOL bPeek)
{
	try
	{
		if (!ValidSocket() ||
			!pBuffer ||
			!ulBufferLength)
			return GetErrorCode();

		//Are we peek?
		int iFlag;
		if (bPeek)
			iFlag=MSG_PEEK;
		else
			iFlag=NULL;

		//Receive data
		int iResult;

		//Receive
		if (m_ucProtocol!=IPPROTO_TCP)
		{
			//Get the remote address
			sockaddr saConnected;

			int iTmp;
			iTmp=sizeof(saConnected);

			//Accept it
			iResult=recvfrom(GetHandle(),
							 pBuffer,
							 ulBufferLength,
							 iFlag,
							 &saConnected,
							 &iTmp);

			//If OK set it
			if (iResult!=GetErrorCode())
				//Take the connected to data
				memcpy(&m_aConnectedTo,
					   &saConnected,
					   sizeof(saConnected));
		}
		else

			//Do a regular receive
			iResult=recv(GetHandle(),
						 pBuffer,
						 ulBufferLength,
						 iFlag);

		//Check if error
		if (iResult==GetErrorCode() &&
			GetSystemLastError()!=WSAEWOULDBLOCK)
			//Which error are we?
			if (!bPeek)
				//Receive
				SetLastError("Receive");
			else
				//Peek
				SetLastError("Peek");

		//Number of bytes received
		return iResult;
	}
	ERROR_HANDLER_RETURN("LocalReceive",GetErrorCode())
}

int CSocket::Receive(char *pBuffer, 
						  unsigned long ulBufferLength)
{
	try
	{
		return LocalReceive(pBuffer,
							ulBufferLength,
							FALSE);
	}
	ERROR_HANDLER_RETURN("Receive",GetErrorCode())
}

int CSocket::Peek(char* pBuffer,
					   unsigned long ulBufferLength)
{
	try
	{
		return LocalReceive(pBuffer,
							ulBufferLength,
							TRUE);
	}
	ERROR_HANDLER_RETURN("Peek",GetErrorCode())
}

long CSocket::GetPeerAddress() const
{
	//Get the address we are connected to
	return m_aConnectedTo.sin_addr.S_un.S_addr;
}

BOOL CSocket::Shutdown(SocketShutdown eHow)
{
	if (!CheckSocketValid())
		return FALSE;

	try
	{
		int iHow;

		//Convert the how to a real flag
		if (eHow==ssReceive)
			iHow=SD_RECEIVE;
		else if (eHow==ssSend)
			iHow=SD_SEND;
		else
			iHow=SD_BOTH;

		//Do it
		if (shutdown(GetHandle(),iHow))
		{
			//Report it
			SetLastError("Shutdown");

			//Exit
			return FALSE;
		}

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("Shutdown",FALSE)
}

unsigned short CSocket::GetPeerPort() const
{
	return htons(m_aConnectedTo.sin_port);
}

BOOL CSocket::CanRead()const
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Create our structure
		fd_set aDescriptor;
		FD_ZERO(&aDescriptor);

		//Add our socket
		FD_SET(GetHandle(),
			   &aDescriptor);

		//And create the timeval
		timeval aTime;
		aTime.tv_sec=0;
		aTime.tv_usec=0;

		//And run the select
		int iRetVal;
		iRetVal=select(NULL,
					   &aDescriptor,
					   NULL,
					   NULL,
					   &aTime);

		//Check if we had an error
		if (iRetVal==GetErrorCode())
		{
			//Report it
			SetLastError("CanRead");

			//Exit
			return FALSE;
		}
		else
			//Check is our socket set
			return FD_ISSET(GetHandle(),
							&aDescriptor);
	}
	ERROR_HANDLER_RETURN("CanRead",FALSE)
}

BOOL CSocket::CanWrite()const
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Create our structure
		fd_set aDescriptor;
		FD_ZERO(&aDescriptor);

		//Add our socket
		FD_SET(GetHandle(),
			   &aDescriptor);

		//And create the timeval
		timeval aTime;
		aTime.tv_sec=0;
		aTime.tv_usec=0;

		//And run the select
		int iRetVal;
		iRetVal=select(NULL,
					   NULL,
					   &aDescriptor,
					   NULL,
					   &aTime);

		//Check if we had an error
		if (iRetVal==GetErrorCode())
		{
			//Report it
			SetLastError("CanWrite");

			//Exit
			return FALSE;
		}
		else
			//Check is our socket set
			return FD_ISSET(GetHandle(),
							&aDescriptor);
	}
	ERROR_HANDLER_RETURN("CanWrite",FALSE)
}

BOOL CSocket::IsCreated()const
{
	return ValidSocket();
}

BOOL CSocket::SetBroadcast(BOOL bBroadcast)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set broadcast option
		if(setsockopt(GetHandle(),
					  SOL_SOCKET,
					  SO_BROADCAST,
					  (char*)&bBroadcast,
					  sizeof(bBroadcast))==GetErrorCode())
		{
			//Check for options error
			SetLastError("SetBroadcast");

			//Exit
			return FALSE;
		}	

		return TRUE;
	}
	ERROR_HANDLER_RETURN("SetBroadcast",FALSE)
}

unsigned char CSocket::GetTTL()const
{
	return m_ucTTL;
}

IP CSocket::GetSourceAddress()const
{
	return m_ulSourceAddress;
}

unsigned char CSocket::GetProtocol()const
{
	return m_ucProtocol;
}

BOOL CSocket::SetReceiveTimeout(unsigned long ulMS)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set it
		if(setsockopt(GetHandle(),
					  SOL_SOCKET,
					  SO_RCVTIMEO,
					  (char*)&ulMS,
					  sizeof(ulMS))==GetErrorCode())
		{
			//Check for options error
			SetLastError("SetReceiveTimeout");

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("SetReceiveTimeout",FALSE)
}

bool CSocket::operator==(const CSocket& rSocket)const
{
	return m_aSocket==rSocket.m_aSocket;
}

bool CSocket::operator<(const CSocket& rSocket)const
{
	return m_aSocket<rSocket.m_aSocket;
}

void CSocket::SetConnectedTo(const sockaddr_in& rAddress)
{
	m_aConnectedTo=rAddress;
}

IP CSocket::GetBindAddress()const
{
	return m_ulSourceAddress;
}

KOMODIA_NAMESPACE_END

