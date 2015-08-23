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

#if !defined(AFX_SPOOFSOCKET_H__5BAEA068_961A_4652_8BBD_90B78F6FBB09__INCLUDED_)
#define AFX_SPOOFSOCKET_H__5BAEA068_961A_4652_8BBD_90B78F6FBB09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SocketBase.h"

KOMODIA_NAMESPACE_START

class SOCKETUTILS_EXP CSocket : virtual public CSocketBase
{
public:
	//Shutdown enum
	typedef enum _SocketShutdown
	{
		ssReceive,
		ssSend,
		ssBoth
	} SocketShutdown;

	//Friend class the crafter
	friend class CIPCrafter;
public:
	//Get the protocol
	unsigned char GetProtocol()const;

	//Set the protocol we are working on
	void SetProtocol(unsigned char iProtocol);

	//Indication if we are a raw socket
	BOOL IsRaw()const;

	//Set receive timeout (for blocked only)
	BOOL SetReceiveTimeout(unsigned long ulMS);

	//Get the socket handle
	SOCKET GetHandle()const;

	//Detach, remove the socket out of this class
	virtual SOCKET Detach();

	//Set to be a broadcast
	BOOL SetBroadcast(BOOL bBroadcast);

	//Is this socket created ?
	BOOL IsCreated()const;

	//Check if socket if ready for writing
	BOOL CanWrite()const;

	//Check if socket if ready for reading
	BOOL CanRead()const;

	//Get the port if the remote connected system
	unsigned short GetPeerPort()const;

	//Close one way of the socket (receive,send,both)
	BOOL Shutdown(SocketShutdown eHow);

	//Get the address of the remote connected system
	long GetPeerAddress()const;

	//Recieve data from remote socket, can be used with all
	//sub sockets (protocols)
	//Return value:
	//Positive - The number of bytes received.
	//Zero - Socket has been closed.
	//Negative - Error
	virtual int Receive(char* pBuffer,
						unsigned long ulBufferLength);
	
	//Get data from remote socket with out deleting data from buffer, can be used with all
	//sub sockets (protocols)
	//Return value:
	//Positive - The number of bytes received.
	//Zero - Socket has been closed.
	//Negative - Error
	virtual int Peek(char* pBuffer,
					 unsigned long ulBufferLength);

	//Set the packet Time to live
	void SetTTL(unsigned char ucTTL);

	//Set source address for spoofing
	virtual void SetSourceAddress(IP aSourceAddress);
	virtual void SetSourceAddress(const std::string& rSourceAddress);

	//Close the socket
	virtual BOOL Close();

	//Get the bind address
	virtual IP GetBindAddress()const;

	//Get the port address, if OS chose the port (0 is error)
	virtual unsigned short GetBindPort()const;

	//Bind to a specific address (if port is zero then OS will choose port)
	virtual BOOL Bind(const std::string& rSourceAddress,
					  unsigned short usPort);
	virtual BOOL Bind(IP aSourceAddress,
					  unsigned short usPort);

	//Send data to a socket , can be used with all
	//sub sockets (protocols)
	//Return value:
	//Positive - The number of bytes sent.
	//Negative - Error
	virtual int Send(IP aDestinationAddress,
					 const char* pBuffer,
					 unsigned long ulBufferLength,
					 unsigned short usDestinationPort=0);
	virtual int Send(const std::string& rDestinationAddress,
					 const char* pBuffer,
					 unsigned long ulBufferLength,
					 unsigned short usDestinationPort=0);

	//Create a socket
	BOOL Create(int iProtocol);

	//Operators, for compare
	bool operator==(const CSocket& rSocket)const;
	bool operator<(const CSocket& rSocket)const;

	//Ctor and Dtor
	CSocket(BOOL bRawSocket=false);
	virtual ~CSocket();
protected:
	//Attach to a socket by constructor
	CSocket(SOCKET aSocket);

	//Set the connected to
	void SetConnectedTo(const sockaddr_in& rAddress);

	//Get the source address
	IP GetSourceAddress()const;

	//Get the TTL
	unsigned char GetTTL()const;

	//Check this socket is valid
	BOOL CheckSocketValid()const;

	//Attach to a socket
	void AssignSocket(SOCKET aNewSocket,
					  unsigned char ucProtocol=IPPROTO_TCP);

	//Is our socket valid ?
	BOOL ValidSocket()const;

	//initialize all the private memebers
	virtual void InitializeIP();

	//Recieve data from remote socket, saving the address
	virtual int ReceiveFrom(char* pBuffer,
							unsigned long ulBufferLength,
							IP& rIP,
							unsigned short& rSourcePort);
private:
	//No copy Ctor
	CSocket(const CSocket& rSocket);

	//No assigment operator
	CSocket& operator=(const CSocket& rSocket);

	//Receive data
	int LocalReceive(char* pBuffer,
					 unsigned long ulBufferLength,
					 BOOL bPeek);

	//Are we raw ?
	BOOL m_bRaw;

	//Our protocol
	unsigned char m_ucProtocol;

	//Time to live
	unsigned char m_ucTTL;

	//Our source address
	IP m_ulSourceAddress;
	
	//The actual socket handle
	SOCKET m_aSocket;

	//Are we an ovelapped socket
	BOOL m_bOverlapped;

	//Remote address we are conencted to
	sockaddr_in m_aConnectedTo;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SPOOFSOCKET_H__5BAEA068_961A_4652_8BBD_90B78F6FBB09__INCLUDED_)
