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
#include "TCPSocket.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "Array_ptr.h"

#include <memory>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CTCPSocket_LOGNAME "CTCPSocket"

CTCPSocket::CTCPSocket(BOOL bRawSocket) : CSocket(bRawSocket)
{
	try
	{
		//Set our name
		SetName(CTCPSocket_LOGNAME);

		//Initialize the TCP
		InitializeTCP();
	}
	ERROR_HANDLER("CTCPSocket")
}

CTCPSocket::CTCPSocket(SOCKET aSocket) : CSocket(aSocket)
{
	try
	{
		//Set our name
		SetName(CTCPSocket_LOGNAME);

		//Initialize all the data
		InitializeTCP();
	}
	ERROR_HANDLER("CTCPSocket")
}

CTCPSocket::~CTCPSocket() 
{
}

BOOL CTCPSocket::Create()
{
	try
	{
		//Initialize our data
		InitializeTCP();

		//Create it
		SetProtocol(IPPROTO_TCP);

		//Are we raw?
		if (IsRaw())
			return CSocket::Create(IPPROTO_IP); //Can't create working raw TCP socket
		else
			return CSocket::Create(IPPROTO_TCP);
	}
	ERROR_HANDLER_RETURN("Create",FALSE)
}

BOOL CTCPSocket::Connect(IP aDestinationAddress,
						 unsigned short usDestinationPort)
{
	try
	{
		return Connect(0,
					   aDestinationAddress,
					   usDestinationPort);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocket::Connect(const std::string& rDestinationAddress,
						 unsigned short usDestinationPort)
{
	try
	{
		return Connect(0,
					   rDestinationAddress,
					   usDestinationPort);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocket::Connect(unsigned short usSourcePort,
						 IP aDestinationAddress,
						 unsigned short usDestinationPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set async notification
		int iResult;
		
		//Create the address
		sockaddr_in aSrc;
	
		//Set to 0
		memset(&aSrc,
			   0,
			   sizeof(aSrc));
		aSrc.sin_family=AF_INET;
		aSrc.sin_addr.s_addr=aDestinationAddress;
		aSrc.sin_port=htons(usDestinationPort);

		//Connect
		iResult=connect(GetHandle(),
						(sockaddr*)&aSrc,
						sizeof(aSrc));

		//Did we have a blocked error
		BOOL bBlocked;
		bBlocked=FALSE;

		//Check the result
		if (iResult==GetErrorCode())
		{
			//Check is it blocking error so we can ignore
			if (WSAGetLastError()!=WSAEWOULDBLOCK)
				SetLastError("Connect");
			else
			{
				//Indicate it's blocked error
				bBlocked=TRUE;
				iResult=!GetErrorCode();
			}
		}
		else
			//Report the error
			SetLastError("Connect");
		
		if (iResult!=GetErrorCode())
		{
			//Check if we are a sync socket
			if (!IsAsyncClass())
				SetConnectionStatus(TRUE);

			//Save where we are connected
			SetConnectedTo(aSrc);
		}

		//Done
		if (!bBlocked)
			return iResult!=GetErrorCode();
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocket::Connect(unsigned short usSourcePort, 
						 const std::string& rDestinationAddress, 
						 unsigned short usDestinationPort)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Delegate the call
		return Connect(usSourcePort,
					   StringToLong(rDestinationAddress),
					   usDestinationPort);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocket::Listen(unsigned long ulBackLog)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Try to listen
		int iResult;
		iResult=listen(GetHandle(),
					   ulBackLog);

		//Is all OK
		if (iResult)
		{
			//Report it
			SetLastError("Listen");

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("Listen",FALSE)
}

CTCPSocket* CTCPSocket::Accept()
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//First accept the socket
		SOCKET aSocket;
		
		//Where we are connected to
		sockaddr_in aConnected;
		
		//Size of the structure
		int iSize;
		iSize=sizeof(aConnected);

		//Accept it
		aSocket=accept(GetHandle(),
					   (sockaddr*)&aConnected,
					   &iSize);

		//Is all OK
		if (aSocket!=INVALID_SOCKET)
		{
			//Create the new tcp socket
			CTCPSocket* pSocket;
			pSocket=new CTCPSocket(aSocket);

			//Set the address
			pSocket->SetConnectedTo(aConnected);
			pSocket->SetConnectionStatus(TRUE);

			//Done
			return pSocket;
		}
		else
		{
			//Error
			SetLastError("Accept");

			//Exit
			return NULL;
		}
	}
	ERROR_HANDLER_RETURN("Accept",NULL)
}

void CTCPSocket::InitializeTCP()
{
	try
	{
		//We are not connected
		m_bConnected=FALSE;
	}
	ERROR_HANDLER("InitializeTCP")
}

BOOL CTCPSocket::Accept(CTCPSocket* pNewSocket)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//First accept the socket
		SOCKET aNewSocket;
		
		//Where we are connected to
		sockaddr_in aAddress;
		
		//Size of the structure
		int iSize;
		iSize=sizeof(aAddress);

		//Accept it
		aNewSocket=accept(GetHandle(),
						  (sockaddr*)&aAddress,
						  &iSize);

		//Is it OK
		if (aNewSocket!=INVALID_SOCKET)
		{
			//Delegate
			SetAcceptance(pNewSocket,
						  aNewSocket,
						  aAddress);

			//Exit
			return TRUE;
		}
		else
		{
			//Error
			SetLastError("Accept");

			//Exit
			return FALSE;
		}
	}
	ERROR_HANDLER_RETURN("Accept",FALSE)
}

void CTCPSocket::SetAcceptance(CTCPSocket* pNewSocket,
							   SOCKET aSocket,
							   sockaddr_in aAddress)const
{
	//Call before accept routing
	pNewSocket->BeforeAccept();

	//Set the socket data
	pNewSocket->SetConnectedTo(aAddress);
	pNewSocket->AssignSocket(aSocket);
	pNewSocket->SetConnectionStatus(TRUE);
	pNewSocket->Accepted();
}

int CTCPSocket::Send(const char *pBuffer,
					 unsigned long ulBufferLength)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Send the data
		int iResult;

		//And send it
		iResult=send(GetHandle(),
					 pBuffer,
					 ulBufferLength,
					 NULL);

		//Did we succeed ?
		if (iResult==GetErrorCode())
			//Set the error code
			SetLastError("Send");

		//Done
		return iResult;
	}
	ERROR_HANDLER_RETURN("Send",FALSE)
}

void CTCPSocket::Accepted()
{
}

void CTCPSocket::SetConnectionStatus(BOOL bConnected)
{
	m_bConnected=bConnected;
}

BOOL CTCPSocket::IsConnected()const
{
	return m_bConnected;
}

BOOL CTCPSocket::IsAsyncClass()const
{
	return FALSE;
}

BOOL CTCPSocket::SetNagle(BOOL bNagle)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Invert the flag (convert to true bool)
		if (bNagle)
			bNagle=FALSE;
		else
			bNagle=TRUE;

		//Try to set the option
		if (setsockopt(GetHandle(),
					   IPPROTO_TCP,
					   TCP_NODELAY,
					   (const char*)&bNagle,
					   sizeof(bNagle))==GetErrorCode())
		{
			//Report it
			ReportError("SetNagle","Failed to set nagle");

			//Set it
			SetLastError("SetNagle");
		
			//Exit
			return FALSE;
		}

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("SetNagle",FALSE)
}

BOOL CTCPSocket::Close()
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Check if the father can close
		if (CSocket::Close())
		{
			//Set the connection status
			SetConnectionStatus(FALSE);

			//And exit
			return TRUE;
		}
		else
			return FALSE;
	}
	ERROR_HANDLER_RETURN("Close",FALSE)
}

void CTCPSocket::BeforeAccept()
{
}

KOMODIA_NAMESPACE_END

