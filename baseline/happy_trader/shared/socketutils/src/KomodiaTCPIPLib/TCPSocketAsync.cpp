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
#include "TCPSocketAsync.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"

#include "BlockedBuffer.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

class CErrorRegister
{
private:
	CErrorRegister()
	{
		CErrorHandler::RegisterError(teConnectionTimeout,
									 "Connection timeout");
	}

	static CErrorRegister m_sInstance;
};

CErrorRegister CErrorRegister::m_sInstance;

#define CTCPSocketAsync_Class "CTCPSocketAsync"

CTCPSocketAsync::CTCPSocketAsync(BOOL bRawSocket) : CTCPSocket(bRawSocket), 
													CAsyncSocket(),
													m_bDisabledConnect(FALSE),
													m_ulTimeout(0),
													m_pLinkedSocket(NULL),
													m_pOverider(NULL)
{
	try
	{
		//Set our name
		SetName(CTCPSocketAsync_Class);
	}
	ERROR_HANDLER("CTCPSocketAsync")
}

CTCPSocketAsync::~CTCPSocketAsync()
{
	try
	{
		//Is is system shutdown - if so, user had the obligation to clean up, not us!!
		if (!IsDeleting())
			//We need to close it here
			Close();

		//Get rid of linked socket
		//Do we have it
		if (m_pLinkedSocket)
			m_pLinkedSocket->DeleteSocketFromThread();
	}
	ERROR_HANDLER("~CTCPSocketAsync")
}

BOOL CTCPSocketAsync::Connect(IP aDestinationAddress,
							  unsigned short usDestinationPort,
							  BOOL bDisableAsync,
							  BOOL bForceErrorEvent)
{
	try
	{
		//Delegate the call
		return Connect(0,
					   aDestinationAddress,
					   usDestinationPort,
					   bDisableAsync,
					   bForceErrorEvent);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocketAsync::Connect(const std::string& rDestinationAddress,
							  unsigned short usDestinationPort,
							  BOOL bDisableAsync,
							  BOOL bForceErrorEvent)
{
	try
	{
		//Delegate the call
		return Connect(0,
					   rDestinationAddress,
					   usDestinationPort,
					   bDisableAsync,
					   bForceErrorEvent);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocketAsync::LocalConnect(unsigned short usSourcePort,
								   IP aDestinationAddress,
								   unsigned short usDestinationPort,
								   BOOL bDisableAsync,
								   BOOL bForceErrorEvent)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set the async notification
		if (!bDisableAsync)
		{
			int iResult;
			iResult=InternalWSAAsyncSelect(WM_SOCKET_CONNECT,
										   FD_CONNECT);

			if (iResult)
			{
				//Get the error code
				int iErrorCode;
				iErrorCode=GetSystemLastError();

				//Report it
				SetLastError("Connect");

				//Do we need to call event?
				if (bForceErrorEvent)
					SocketConnected(iErrorCode);

				//Exit
				return FALSE;
			}

			//Set our timeout
			if (m_ulTimeout &&
				!IsBlocking())
				if (!SetSystemTimeout(m_ulTimeout))
				{
					//Report it
					ReportError("LocalConnect","Failed to set timer!");

					//Do we need to call event?
					if (bForceErrorEvent)
						SocketConnected(GetErrorCode());

					//Exit
					return FALSE;
				}
		}
		//Set to non blocking!
		else if (!Block())
			return FALSE;

		//Set the flag
		m_bDisabledConnect=bDisableAsync;

		//Call the original connect
		BOOL bResult;
		bResult=CTCPSocket::Connect(usSourcePort,
									aDestinationAddress,
									usDestinationPort);

		//Reset the flag
		m_bDisabledConnect=FALSE;

		if (bResult)
		{
			//Call event, but only if in async
			if (!bDisableAsync &&
				!IsBlocking())
				//Call user, will add socket automatically
				return SocketConnected(0);
			else
				//Set as async
				return SetAsync();
		}
		else if (GetSystemLastError()!=WSAEWOULDBLOCK ||
				 bDisableAsync ||
				 IsBlocking())
		{
			if (m_ulTimeout)
				//Kill the timer
				KillSystemTimer();

			//Get the error code
			int iErrorCode;
			iErrorCode=GetSystemLastError();

			//Report it
			SetLastError("Connect");

			//Do we need to call event?
			if (bForceErrorEvent &&
				!bDisableAsync)
				SocketConnected(iErrorCode);

			//Exit
			return FALSE;
		}
		else
			return TRUE;
	}
	ERROR_HANDLER_RETURN("LocalConnect",FALSE)
}

BOOL CTCPSocketAsync::Connect(unsigned short usSourcePort,
							  IP aDestinationAddress,
							  unsigned short usDestinationPort,
							  BOOL bDisableAsync,
							  BOOL bForceErrorEvent)
{
	try
	{
		//Do we have a linked socket?
		if (m_pLinkedSocket)
			//Delegate to it
			return m_pLinkedSocket->Connect(usSourcePort,
											aDestinationAddress,
											usDestinationPort,
											bDisableAsync,
											bForceErrorEvent);
		else
			//Our local call
			return LocalConnect(usSourcePort,
								aDestinationAddress,
								usDestinationPort,
								bDisableAsync,
								bForceErrorEvent);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocketAsync::Connect(unsigned short usSourcePort,
							  const std::string& rDestinationAddress,
							  unsigned short usDestinationPort,
							  BOOL bDisableAsync,
							  BOOL bForceErrorEvent)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Delegate the call
		return Connect(usSourcePort,
					   StringToLong(rDestinationAddress),
					   usDestinationPort,
					   bDisableAsync,
					   bForceErrorEvent);
	}
	ERROR_HANDLER_RETURN("Connect",FALSE)
}

BOOL CTCPSocketAsync::LocalListen(unsigned long ulBackLog)
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Try to switch the mode
		int iResult;
		iResult=InternalWSAAsyncSelect(WM_SOCKET_ACCEPT,
									   FD_ACCEPT);

		//Is it OK
		if (iResult)
		{
			//Report it
			SetLastError("LocalListen");

			//Exit
			return FALSE;
		}

		//Delegate the call
		return CTCPSocket::Listen(ulBackLog);
	}
	ERROR_HANDLER_RETURN("LocalListen",FALSE)
}

BOOL CTCPSocketAsync::SetAsync()
{
	try
	{
		//Quit if not ok
		if (!CheckSocketValid())
			return FALSE;

		//Set event to read / write / close / oob
		int iResult;
		iResult=InternalWSAAsyncSelect(WM_SOCKET_GENERAL,
									   FD_WRITE | 
									   FD_READ | 
									   FD_CLOSE | 
									   FD_OOB);

		//What happend
		if (iResult)
		{
			//Report it
			SetLastError("SetAsync");

			//Exit
			return FALSE;
		}
		
		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("SetAsync",FALSE)
}

BOOL CTCPSocketAsync::LocalSocketConnect(int iErrorCode,
										 BOOL bNoEvent)
{
	try
	{
		if (m_ulTimeout)
			//Kill the timer
			KillSystemTimer();

		//Did we got an error
		if (!iErrorCode)
			//Set the connection status
			SetConnectionStatus(TRUE);
		
		//Call father
		return CAsyncSocket::SocketConnected(iErrorCode,
											 bNoEvent);
	}
	ERROR_HANDLER_RETURN("LocalSocketConnect",FALSE)
}

BOOL CTCPSocketAsync::SocketConnected(int iErrorCode,
									  BOOL bNoEvent)
{
	try
	{
		//Do we have a linked socket?
		if (m_pLinkedSocket &&
			!m_bCloseEventsOnly)
			//Delegate it
			return m_pLinkedSocket->SocketConnected(iErrorCode,
													bNoEvent);
		else
			//Local call
			return LocalSocketConnect(iErrorCode,
									  bNoEvent);
	}
	ERROR_HANDLER_RETURN("SocketConnected",FALSE)
}

BOOL CTCPSocketAsync::LocalSocketReceive(int iErrorCode,
										 BOOL bNoEvent)
{
	try
	{
		//Delegate call
		return CAsyncSocket::SocketReceive(iErrorCode,
										   bNoEvent);
	}
	ERROR_HANDLER_RETURN("LocalSocketReceivet",FALSE)
}

BOOL CTCPSocketAsync::SocketReceive(int iErrorCode,
									BOOL bNoEvent)
{
	try
	{
		//Do we have a linked socket?
		if (m_pLinkedSocket &&
			!m_bCloseEventsOnly)
			//Delegate it
			return m_pLinkedSocket->SocketReceive(iErrorCode,
													bNoEvent);
		else
			//Local call
			return LocalSocketReceive(iErrorCode,
									  bNoEvent);
	}
	ERROR_HANDLER_RETURN("SocketConnected",FALSE)
}

BOOL CTCPSocketAsync::LocalSocketAccept(int iErrorCode,
								  	    BOOL bNoEvent)
{
	try
	{
		//Delegate call
		return CAsyncSocket::SocketAccept(iErrorCode,
										  bNoEvent);
	}
	ERROR_HANDLER_RETURN("LocalSocketAccept",FALSE)
}

BOOL CTCPSocketAsync::SocketClosed(int iErrorCode,
								   BOOL bNoEvent)
{
	try
	{
		if (m_pLinkedSocket &&
			!m_bCloseEventsOnly)
			//Delegate the call
			return m_pLinkedSocket->SocketClosed(iErrorCode,
												 bNoEvent);
		else
			//Local version
			return LocalSocketClose(iErrorCode,
									bNoEvent);
		
	}
	ERROR_HANDLER_RETURN("SocketClosed",FALSE)
}

BOOL CTCPSocketAsync::LocalSocketClose(int iErrorCode,
									   BOOL bNoEvent)
{
	try
	{
		//We are not connected
		SetConnectionStatus(FALSE);

		//Call father
		return CAsyncSocket::SocketClosed(iErrorCode,
										  bNoEvent);
	}
	ERROR_HANDLER_RETURN("LocalSocketClose",FALSE)
}

void CTCPSocketAsync::Accepted()
{
	try
	{
		//Add to async list
		AddSocketToList();

		//Go to async mode
		SetAsync();
	}
	ERROR_HANDLER("CTCPSocketAsync")
}

SOCKET CTCPSocketAsync::GetAsyncHandle() const
{
	return GetHandle();
}

BOOL CTCPSocketAsync::Create()
{
	try
	{
		if (!CTCPSocket::Create())
			return FALSE;
		else
		{
			//Reset events type
			m_bCloseEventsOnly=FALSE;

			//New async settings
			SocketCreated();

			//Add to list
			AddSocketToList();

			//Done
			return TRUE;
		}
	}
	ERROR_HANDLER_RETURN("Create",FALSE)
}

BOOL CTCPSocketAsync::Close()
{
	try
	{
		//Quit if not ok
		if (!ValidSocket())
			return FALSE;

		//Delegate to remove socket
		if (m_pLinkedSocket &&
			m_bCloseEventsOnly)
			m_pLinkedSocket->Close();

		//Kill the timer
		CAsyncSocket::SocketClosing();

		//Remove from socket list
		RemoveSocketFromList();

		//Done
		return CTCPSocket::Close();
	}
	ERROR_HANDLER_RETURN("Close",FALSE)
}

BOOL CTCPSocketAsync::IsAsyncClass() const
{
	try
	{
		//Check if we are blocking
		if (IsBlocking() || 
			m_bDisabledConnect)
			return FALSE;
		else
			//Not blocking
			return TRUE;
	}
	ERROR_HANDLER_RETURN("IsAsyncClass",FALSE)
}

void CTCPSocketAsync::BeforeAccept()
{
	try
	{
		//Remove ourselves
		RemoveSocketFromList();
	}
	ERROR_HANDLER("BeforeAccept")
}

BOOL CTCPSocketAsync::SendBlockedBuffer(const CBlockedBuffer::CBlockedData& rData)
{
	try
	{
		//Try to send it
		return SendNoAdd(rData.GetData(),
						 rData.GetDataSize())!=GetErrorCode();
	}
	ERROR_HANDLER_RETURN("SendBlockedBuffer",FALSE)
}

int CTCPSocketAsync::Send(const char *pBuffer,
						  unsigned long ulBufferLength)
{
	try
	{
		//Reset the event
		ResetEvent();

		//Send the data
		int iResult;
		iResult=SendNoAdd(pBuffer,
						  ulBufferLength);

		//Did we succeed ?
		if (iResult==GetErrorCode())
			//Is it blocked send ?
			if (WSAGetLastError()==WSAEWOULDBLOCK)
				if (GetBlockedBuffer())
				{
					//Add to the buffer, if we have one
					GetBlockedBuffer()->AddRecord(CBlockedBuffer::CBlockedData(pBuffer,
																			   ulBufferLength));

					//We have not error
					iResult=0;
				}
				else if (IsBlockSend())
					//Are we in the same thread?
					if (!IsInThread())
						if (WaitForBlockEvent())
						{
							//Set the error
							::SetLastError(WSAENOTCONN);

							//Exit
							return GetErrorCode();
						}
						else
							//Retry to send
							return Send(pBuffer,
										ulBufferLength);
					else
					{
						//We'll have to do a loop
						Sleep(10);

						//Try again
						if (SendNoAdd(pBuffer,
									  ulBufferLength)==GetErrorCode())
							//Which error
							if (WSAGetLastError()==WSAEWOULDBLOCK)
								return 0;
							else
								return GetErrorCode();
					}
				//Blocking error (not really an error)
				else
					return 0;
			else
				//Set the error code
				SetLastError("Send");

		//Done
		return iResult;
	}
	ERROR_HANDLER_RETURN("Send",GetErrorCode())
}

int CTCPSocketAsync::SendNoAdd(const char* pBuffer,
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

		//And exit
		return iResult;
	}
	ERROR_HANDLER_RETURN("SendNoAdd",GetErrorCode())
}

BOOL CTCPSocketAsync::SocketWrite(int iErrorCode)
{
	try
	{
		//Only if we have a buffer
		if (GetBlockedBuffer())
			GetBlockedBuffer()->SendData();

		//Call father
		return CAsyncSocket::SocketWrite(iErrorCode);
	}
	ERROR_HANDLER_RETURN("OnSocketWrite",TRUE)
}

BOOL CTCPSocketAsync::SetConnectionTimeout(unsigned long ulMS)
{
	try
	{
		//Do we have a timeout ?
		if (HasSystemTimer())
			if (!KillSystemTimer())
			{
				//Report it
				ReportError("SetConnectionTimeout","Failed to kill previous timer!");

				//Exit
				return FALSE;
			}

		//Create the timer
		m_ulTimeout=ulMS;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_RETURN("SetConnectionTimeout",FALSE)
}

unsigned long CTCPSocketAsync::GetConnectionTimeout()const
{
	return m_ulTimeout;
}

void CTCPSocketAsync::SystemTimeout()
{
	//Call socket timeout
	SocketConnected(teConnectionTimeout);
}

int CTCPSocketAsync::Receive(char* pBuffer,
							 unsigned long ulBufferLength)
{
	try
	{
		//Try to receive
		int iResult;
		if ((iResult=CTCPSocket::Receive(pBuffer,
										 ulBufferLength))>=0)
			return iResult;
		//Check is it an error
		else if (GetSystemLastError()==WSAEWOULDBLOCK)
			return 0;
		else
			return GetErrorCode();
	}
	ERROR_HANDLER_RETURN("Receive",GetErrorCode())
}

int CTCPSocketAsync::Peek(char* pBuffer,
						  unsigned long ulBufferLength)
{
	try
	{
		//Try to receive
		int iResult;
		if ((iResult=CTCPSocket::Peek(pBuffer,
										 ulBufferLength))>=0)
			return iResult;
		//Check is it an error
		else if (GetSystemLastError()==WSAEWOULDBLOCK)
			return 0;
		else
			return GetErrorCode();
	}
	ERROR_HANDLER_RETURN("Peek",GetErrorCode())
}

void CTCPSocketAsync::SetLinkedSocket(CTCPSocketAsync* pSocket,
									  CTCPSocketOverider* pOverider)
{
	try
	{
		//Do we have an old socket?
		if (m_pLinkedSocket)
			m_pLinkedSocket->DeleteSocketFromThread();

		//Save it
		m_pLinkedSocket=pSocket;
		m_pOverider=pOverider;

		//Set the events type
		m_bCloseEventsOnly=FALSE;
	}
	ERROR_HANDLER("SetLinkedSocket")
}

BOOL CTCPSocketAsync::Listen(unsigned long ulBackLog)
{
	try
	{
		//Do we have a linked socket?
		if (m_pLinkedSocket)
			//Delegate the call
			return m_pLinkedSocket->Listen(ulBackLog);
		else
			return LocalListen(ulBackLog);
	}
	ERROR_HANDLER_RETURN("Listen",FALSE)
}

BOOL CTCPSocketAsync::Bind(IP aSourceAddress,
						   unsigned short usPort)
{
	try
	{
		//Do we have a linked socket?
		if (m_pLinkedSocket)
			//Delegate the call
			return m_pLinkedSocket->Bind(aSourceAddress,
										 usPort);
		else
			return LocalBind(aSourceAddress,
							 usPort);
	}
	ERROR_HANDLER_RETURN("Listen",FALSE)
}

BOOL CTCPSocketAsync::LocalBind(IP aSourceAddress,
							    unsigned short usPort)
{
	try
	{
		return CTCPSocket::Bind(aSourceAddress,
								usPort);
	}
	ERROR_HANDLER_RETURN("LocalBind",FALSE)
}

void CTCPSocketAsync::SwitchEventsType()
{
	m_bCloseEventsOnly=TRUE;
}

CTCPSocketAsync::CTCPSocketOverider* CTCPSocketAsync::GetOverider()const
{
	return m_pOverider;
}

CTCPSocketAsync::operator const CTCPSocketAsync::CTCPSocketOverider*()const
{
	return m_pOverider;
}

IP CTCPSocketAsync::GetBindAddress()const
{
	if (m_pLinkedSocket)
		return m_pLinkedSocket->GetBindAddress();
	else
		return LocalGetBindAddress();
}

unsigned short CTCPSocketAsync::GetBindPort()const
{
	if (m_pLinkedSocket)
		return m_pLinkedSocket->GetBindPort();
	else
		return LocalGetBindPort();
}

IP CTCPSocketAsync::LocalGetBindAddress()const
{
	try
	{
		return CTCPSocket::GetBindAddress();
	}
	ERROR_HANDLER_RETURN("LocalGetBindAddress",0)
}

unsigned short CTCPSocketAsync::LocalGetBindPort()const
{
	try
	{
		return CTCPSocket::GetBindPort();
	}
	ERROR_HANDLER_RETURN("LocalGetBindPort",0)
}

BOOL CTCPSocketAsync::Accept(CTCPSocket* pNewSocket)
{
	try
	{
		if (m_pLinkedSocket)
			return m_pLinkedSocket->Accept(pNewSocket);
		else
			return LocalAccept(pNewSocket);
	}
	ERROR_HANDLER_RETURN("Accept",FALSE)
}

BOOL CTCPSocketAsync::LocalAccept(CTCPSocket* pNewSocket)
{
	try
	{
		return CTCPSocket::Accept(pNewSocket);
	}
	ERROR_HANDLER_RETURN("LocalAccept",FALSE)
}

SOCKET CTCPSocketAsync::Detach()
{
	//Remove it from the async socket
	RemoveSocketFromList();

	//Delegate
	return CTCPSocket::Detach();
}

KOMODIA_NAMESPACE_END