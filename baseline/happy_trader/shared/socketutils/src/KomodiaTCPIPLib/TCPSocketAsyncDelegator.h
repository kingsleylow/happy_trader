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

#if !defined(AFX_TCPSOCKETASYNCDELEGATOR_H__E2DEE665_2EE9_4740_B7B9_D598F7E6E981__INCLUDED_)
#define AFX_TCPSOCKETASYNCDELEGATOR_H__E2DEE665_2EE9_4740_B7B9_D598F7E6E981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../KomodiaInfraLib/ErrorHandler.h"

#include "SocketBase.h"

KOMODIA_NAMESPACE_START

class CTCPSocket;
class CTCPSocketAsync;

class SOCKETUTILS_EXP CTCPSocketAsyncDelegator : public CErrorHandler  
{
public:
	//This methods are delegated
	//Socket handlers
	//Socket is connected
	BOOL SocketConnected(int iErrorCode,
						 BOOL bNoEvent=FALSE);

	//When we have data to receive
	BOOL LocalSocketReceive(int iErrorCode,
							BOOL bNoEvent=FALSE);

	//Local socket handlers
	//Event handler for socket connections
	BOOL LocalSocketConnect(int iErrorCode,
							BOOL bNoEvent);

	//Event handler for socket close
	BOOL LocalSocketClose(int iErrorCode,
						  BOOL bNoEvent);

	//Fire the accept event
	BOOL LocalSocketAccept(int iErrorCode,
						   BOOL bNoEvent);

	//Sockets events
	BOOL OnSocketConnect(int iErrorCode);

	//Socket commands
	//Our local connect
	BOOL LocalConnect(unsigned short usSourcePort,
					  IP aDestinationAddress,
					  unsigned short usDestinationPort,
					  BOOL bDisableAsync,
					  BOOL bForceErrorEvent);

	//Our local bind
	BOOL LocalBind(IP aSourceAddress,
				   unsigned short usPort);

	//Get the bind address
	IP LocalGetBindAddress()const;

	//Get the bind port
	unsigned short LocalGetBindPort()const;

	//Our local listen
	BOOL LocalListen(unsigned long ulBackLog);

	//Accept a connection, supply an already made socket
	BOOL LocalAccept(CTCPSocket* pNewSocket);

	//Other methods
	//Set acceptance of a socket
	void SetAcceptance(CTCPSocket* pNewSocket,
					   SOCKET aSocket,
					   sockaddr_in aAddress)const;

	//When we have a system timeout
	void SystemTimeout();

	//Set the connected to
	void SetConnectedTo(const sockaddr_in& rAddress);

	//Switch events type (for linked socket)
	void SwitchEventsType();

	//Go to async mode
	BOOL SetAsync();

	//Clear the events
	BOOL ClearEvents();

	//Detach the socket
	SOCKET Detach();

	//Ctor and Dtor
	CTCPSocketAsyncDelegator(CTCPSocketAsync* pSocket);
	virtual ~CTCPSocketAsyncDelegator();
protected:
	//Get the socket
	CTCPSocketAsync* GetSocket()const;
private:
	//No copy Ctor
	CTCPSocketAsyncDelegator(const CTCPSocketAsyncDelegator& rDelegator);

	//No copy operator
	CTCPSocketAsyncDelegator& operator=(const CTCPSocketAsyncDelegator& rDelegator);

	//Our socket
	CTCPSocketAsync* m_pSocket;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_TCPSOCKETASYNCDELEGATOR_H__E2DEE665_2EE9_4740_B7B9_D598F7E6E981__INCLUDED_)
