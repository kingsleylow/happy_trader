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

#if !defined(AFX_TCPSOCKET_H__77DA7F21_291E_4C2A_B12B_535ABA1E829C__INCLUDED_)
#define AFX_TCPSOCKET_H__77DA7F21_291E_4C2A_B12B_535ABA1E829C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../socketutilsdefs.hpp"
#include "Socket.h"

KOMODIA_NAMESPACE_START

class SOCKETUTILS_EXP CTCPSocket : public CSocket
{
public:
	//Set nagle status (default is on)
	BOOL SetNagle(BOOL bNagle);

	//Close the socket
	virtual BOOL Close();

	//Get our connection status
	BOOL IsConnected()const;

	//Send data over the sockets
	//Return value:
	//Positive - The number of bytes received.
	//Negative - Error
	virtual int Send(const char* pBuffer,
					 unsigned long ulBufferLength);

	//Accept a connection, supply an already made socket
	virtual BOOL Accept(CTCPSocket* pNewSocket);

	//Accept a connection, create the socket class
	CTCPSocket* Accept();

	//Listen to incoming connections
	virtual BOOL Listen(unsigned long ulBackLog=5);

	//Connect to a remote system
	//Choose the source address explicitly
	//usSourcePort - OS will allocate source port
	virtual BOOL Connect(unsigned short usSourcePort,
						 IP aDestinationAddress,
						 unsigned short usDestinationPort);
	virtual BOOL Connect(unsigned short usSourcePort,
						 const std::string& rDestinationAddress,
						 unsigned short usDestinationPort);

	//Connect to a remote system
	//Using OS allocated source port
	virtual BOOL Connect(IP aDestinationAddress,
						 unsigned short usDestinationPort);
	virtual BOOL Connect(const std::string& rDestinationAddress,
						 unsigned short usDestinationPort);

	//Create the socket
	virtual BOOL Create();

	//Ctor and Dtor
	CTCPSocket(BOOL bRawSocket=false);
	virtual ~CTCPSocket();
protected:
	//Called before we accept the socket (user overide)
	virtual void BeforeAccept();

	//Indicate if we are a blocked socket or an async one
	virtual BOOL IsAsyncClass()const;

	//Set our connection status
	void SetConnectionStatus(BOOL bConnected);

	//When the socket is accepted, what to do (user overide)
	virtual void Accepted();

	//Set acceptance of a socket
	void SetAcceptance(CTCPSocket* pNewSocket,
					   SOCKET aSocket,
					   sockaddr_in aAddress)const;
private:
	//Attach to a socket
	CTCPSocket(SOCKET aSocket);

	//No copy Ctor
	CTCPSocket(const CTCPSocket& rSocket);

	//No assignment operator
	CTCPSocket& operator=(const CTCPSocket& rSocket); 

	//Initialize the class
	void InitializeTCP();
	
	//Are we connected
	BOOL m_bConnected;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_TCPSOCKET_H__77DA7F21_291E_4C2A_B12B_535ABA1E829C__INCLUDED_)
