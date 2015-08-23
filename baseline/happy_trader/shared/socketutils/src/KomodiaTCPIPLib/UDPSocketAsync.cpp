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
#include "UDPSocketAsync.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CUDPSocketAsync_Class "CUDPSocketAsync"

CUDPSocketAsync::CUDPSocketAsync(BOOL bRawSocket) : CUDPSocket(bRawSocket), 
													CAsyncSocket()
{
	try
	{
		//Set our name
		SetName(CUDPSocketAsync_Class);
	}
	ERROR_HANDLER("CUDPSocketAsync")
}

CUDPSocketAsync::~CUDPSocketAsync()
{
	try
	{
		//We need to close it here
		Close();
	}
	ERROR_HANDLER("~CUDPSocketAsync")
}

BOOL CUDPSocketAsync::Listen()
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set async mode
		int iResult;
		iResult=InternalWSAAsyncSelect(WM_SOCKET_GENERAL,
									   FD_READ);

		//Check result
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

BOOL CUDPSocketAsync::SetAsync()
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set event to read / write 
		int iResult;
		iResult=InternalWSAAsyncSelect(WM_SOCKET_GENERAL,
									   FD_WRITE | FD_READ);

		//Check result
		if (iResult)
		{
			//Report it
			SetLastError("SetAsync");

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("SetAsync",FALSE)
}

BOOL CUDPSocketAsync::OnSocketConnect(int iErrorCode)
{
	//First set async again
	return SetAsync();
}

void CUDPSocketAsync::Accepted()
{
	try
	{
		//Add the socket to a list
		AddSocketToList();

		//Go to async mode
		SetAsync();
	}
	ERROR_HANDLER("CTCPSocketAsync")
}

SOCKET CUDPSocketAsync::GetAsyncHandle()const
{
	return GetHandle();
}

BOOL CUDPSocketAsync::Create()
{
	try
	{
		if (!CUDPSocket::Create())
			return FALSE;
		else
		{
			//Add the socket to a list
			AddSocketToList();

			//Exit
			return TRUE;
		}
	}
	ERROR_HANDLER_RETURN("Create",FALSE)
}

BOOL CUDPSocketAsync::Close()
{
	try
	{
		//Quit if not ok
		if (!ValidSocket())
			return FALSE;

		//Kill the timer
		CAsyncSocket::SocketClosing();

		//Remove from socket list
		RemoveSocketFromList();

		//Delegate call
		return CUDPSocket::Close();
	}
	ERROR_HANDLER_RETURN("Close",FALSE)
}

BOOL CUDPSocketAsync::Send(IP aDestinationAddress, 
						   unsigned short usDestinationPort,
						   const char* pBuffer,
						   unsigned short usBufferLength)
{
	try
	{
		//Delegate call
		return CUDPSocket::Send(aDestinationAddress,
								usDestinationPort,
								pBuffer,
								usBufferLength);
	}
	ERROR_HANDLER_RETURN("Send",GetErrorCode())
}

BOOL CUDPSocketAsync::Send(const std::string& rDestinationAddress, 
						   unsigned short usDestinationPort,
						   const char* pBuffer,
						   unsigned short usBufferLength)
{
	try
	{
		//Delegate call
		return CUDPSocket::Send(rDestinationAddress,
								usDestinationPort,
								pBuffer,
								usBufferLength);
	}
	ERROR_HANDLER_RETURN("Send",GetErrorCode())
}

KOMODIA_NAMESPACE_END