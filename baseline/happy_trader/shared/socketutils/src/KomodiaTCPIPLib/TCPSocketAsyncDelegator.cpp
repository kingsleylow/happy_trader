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
#include "TCPSocketAsyncDelegator.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"

#include "TCPSocketAsync.h"

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

#define CTCPSocketAsyncDelegator_Class "CTCPSocketAsyncDelegator"

CTCPSocketAsyncDelegator::CTCPSocketAsyncDelegator(CTCPSocketAsync* pSocket) : CErrorHandler(),
																			   m_pSocket(pSocket)
{
	try
	{
		//Set our name
		SetName(CTCPSocketAsyncDelegator_Class);
	}
	ERROR_HANDLER("CTCPSocketAsyncDelegator")
}

CTCPSocketAsyncDelegator::~CTCPSocketAsyncDelegator()
{
}

CTCPSocketAsync* CTCPSocketAsyncDelegator::GetSocket()const
{
	return m_pSocket;
}

BOOL CTCPSocketAsyncDelegator::SocketConnected(int iErrorCode,
											   BOOL bNoEvent)
{
	return m_pSocket->SocketConnected(iErrorCode,
									  bNoEvent);
}

BOOL CTCPSocketAsyncDelegator::LocalSocketConnect(int iErrorCode,
												  BOOL bNoEvent)
{
	return m_pSocket->LocalSocketConnect(iErrorCode,
										 bNoEvent);
}

BOOL CTCPSocketAsyncDelegator::OnSocketConnect(int iErrorCode)
{
	return m_pSocket->OnSocketConnect(iErrorCode);
}

BOOL CTCPSocketAsyncDelegator::LocalSocketReceive(int iErrorCode,
												  BOOL bNoEvent)
{
	return m_pSocket->LocalSocketReceive(iErrorCode,
										 bNoEvent);
}

BOOL CTCPSocketAsyncDelegator::LocalConnect(unsigned short usSourcePort,
										    IP aDestinationAddress,
										    unsigned short usDestinationPort,
										    BOOL bDisableAsync,
										    BOOL bForceErrorEvent)
{
	return m_pSocket->LocalConnect(usSourcePort,
								   aDestinationAddress,
								   usDestinationPort,
								   bDisableAsync,
								   bForceErrorEvent);
}

BOOL CTCPSocketAsyncDelegator::LocalBind(IP aSourceAddress,
								  	     unsigned short usPort)
{
	return m_pSocket->LocalBind(aSourceAddress,
								usPort);
}

BOOL CTCPSocketAsyncDelegator::LocalSocketClose(int iErrorCode,
												BOOL bNoEvent)
{
	return m_pSocket->LocalSocketClose(iErrorCode,
									   bNoEvent);
}

void CTCPSocketAsyncDelegator::SetConnectedTo(const sockaddr_in& rAddress)
{
	//Delegate the call
	m_pSocket->SetConnectedTo(rAddress);
}

void CTCPSocketAsyncDelegator::SwitchEventsType()
{
	//Delegate call
	m_pSocket->SwitchEventsType();
}

BOOL CTCPSocketAsyncDelegator::SetAsync()
{
	//First clear the events
	m_pSocket->ClearEvents();

	//Reset it
	return m_pSocket->SetAsync();
}

IP CTCPSocketAsyncDelegator::LocalGetBindAddress()const
{
	return m_pSocket->LocalGetBindAddress();
}

unsigned short CTCPSocketAsyncDelegator::LocalGetBindPort()const
{
	return m_pSocket->LocalGetBindPort();
}

BOOL CTCPSocketAsyncDelegator::LocalSocketAccept(int iErrorCode,
												 BOOL bNoEvent)
{
	return m_pSocket->LocalSocketAccept(iErrorCode,
										bNoEvent);
}

BOOL CTCPSocketAsyncDelegator::LocalAccept(CTCPSocket* pNewSocket)
{
	return m_pSocket->LocalAccept(pNewSocket);
}

void CTCPSocketAsyncDelegator::SetAcceptance(CTCPSocket* pNewSocket,
										     SOCKET aSocket,
											 sockaddr_in aAddress)const
{
	m_pSocket->SetAcceptance(pNewSocket,
							 aSocket,
							 aAddress);
}

BOOL CTCPSocketAsyncDelegator::ClearEvents()
{
	return m_pSocket->ClearEvents();
}

SOCKET CTCPSocketAsyncDelegator::Detach()
{
	return m_pSocket->Detach();
}

BOOL CTCPSocketAsyncDelegator::LocalListen(unsigned long ulBackLog)
{
	return m_pSocket->LocalListen(ulBackLog);
}

KOMODIA_NAMESPACE_END