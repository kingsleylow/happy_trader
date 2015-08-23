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

#if !defined(AFX_TCPSOCKETASYNC_H__1D78325D_C154_4B21_97E5_320394FD47EF__INCLUDED_)
#define AFX_TCPSOCKETASYNC_H__1D78325D_C154_4B21_97E5_320394FD47EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TCPSocket.h"
#include "AsyncSocket.h"

#include <vector>

KOMODIA_NAMESPACE_START

typedef enum _TCPSocketAsyncErrors
{
	teConnectionTimeout=-100
} TCPSocketAsyncErrors;

class SOCKETUTILS_EXP CTCPSocketAsync :	public CTCPSocket, 
						public CAsyncSocket
{
	friend class CTCPSocketAsyncDelegator;
public:
	using CTCPSocket::Bind;
public:
	//Detach, remove the socket out of this class
	virtual SOCKET Detach();

	//Accept a connection, supply an already made socket
	virtual BOOL Accept(CTCPSocket* pNewSocket);

	//Listen to incoming connections
	virtual BOOL Listen(unsigned long ulBackLog=5);

	//Get the bind address
	virtual IP GetBindAddress()const;

	//Get the bind port
	virtual unsigned short GetBindPort()const;

	//Bind to a specific address
	virtual BOOL Bind(IP aSourceAddress,
					  unsigned short usPort);

	//Set/get a connection timeout
	BOOL SetConnectionTimeout(unsigned long ulMS);
	unsigned long GetConnectionTimeout()const;

	//Send data over the sockets
	//Send may fail because the system isn't ready to 
	//send the data, check out the value of the send
	//SOCKET_ERROR - Failure
	//0 - Send couldn't finish because it would block
	//    if there was a blocking buffer, it was queued
	//	  (check out AllowBlockedBuffer in CAsyncSocket)
	//Value - Bytes sent 
	virtual int Send(const char* pBuffer,
					 unsigned long ulBufferLength);

	//Close the socket
	virtual BOOL Close();

	//Recieve data from remote socket, can be used with all
	//sub sockets (protocols)
	//Return value:
	//Positive - The number of bytes received.
	//Zero - No data to receive.
	//Negative - Error
	virtual int Receive(char* pBuffer,
						unsigned long ulBufferLength);

	//Get data from remote socket with out deleting data from buffer
	//Return value:
	//Positive - The number of bytes received.
	//Zero - No data to receive.
	//Negative - Error
	virtual int Peek(char* pBuffer,
					 unsigned long ulBufferLength);

	//Connect with default source port as zero
	//bDisableAsync - Connect blockingly to destination
	//				  (user event will not be called)
	//bForceErrorEvent - Will call OnSocketConnect event if an
	//					 immediate error occured (only if bDisableAsync==FALSE)
	virtual BOOL Connect(IP aDestinationAddress,
						 unsigned short usDestinationPort,
						 BOOL bDisableAsync=FALSE,
						 BOOL bForceErrorEvent=FALSE);
	virtual BOOL Connect(const std::string& rDestinationAddress,
						 unsigned short usDestinationPort,
						 BOOL bDisableAsync=FALSE,
						 BOOL bForceErrorEvent=FALSE);

	//Our async connection
	virtual BOOL Connect(unsigned short usSourcePort,
						 IP aDestinationAddress,
						 unsigned short usDestinationPort,
						 BOOL bDisableAsync=FALSE,
						 BOOL bForceErrorEvent=FALSE);
	virtual BOOL Connect(unsigned short usSourcePort,
						 const std::string& rDestinationAddress,
						 unsigned short usDestinationPort,
						 BOOL bDisableAsync=FALSE,
						 BOOL bForceErrorEvent=FALSE);

	//Create as a raw socket
	virtual BOOL Create();

	//Ctor and Dtor
	CTCPSocketAsync(BOOL bRawSocket=false);
	virtual ~CTCPSocketAsync();
public:
	//Class of overider
	class CTCPSocketOverider
	{
	public:
		//The vector for selective overide
		typedef std::vector<CTCPSocketAsync*> SelectiveVector;
	public:
		//Actual overider
		virtual BOOL OverideSocket(CTCPSocketAsync* pSocket)=0;

		//Does support selective overiding
		virtual BOOL DoesSupportSelectiveOveriding()const
		{
			return FALSE;
		}

		//Selective overide
		virtual CTCPSocketOverider* GetSelectiveOveride(const SelectiveVector& rVector)const
		{
			return NULL;
		}

		//Ctor and Dtor
		CTCPSocketOverider()
		{
		}
		virtual ~CTCPSocketOverider()
		{
		}
	};
public:
	//Class of overiable
	class CTCPSocketOveridable
	{
	public:
		//Can overide thru this
		virtual BOOL OverideSocket(CTCPSocketOverider* pOverdier)=0;

		//Ctor and Dtor
		CTCPSocketOveridable()
		{
		}
		virtual ~CTCPSocketOveridable()
		{
		}
	};
public:
	//Set a linked socket
	//For proxy implemetation
	void SetLinkedSocket(CTCPSocketAsync* pSocket,
						 CTCPSocketOverider* pOverider);

	//Get the overider
	CTCPSocketOverider* GetOverider()const;
	operator const CTCPSocketOverider*()const;
protected:
	//Send but don't add to buffer
	virtual int SendNoAdd(const char* pBuffer,
						  unsigned long ulBufferLength);

	//Indicate if we are a blocked socket or an async one
	virtual BOOL IsAsyncClass()const;

	//Go to async mode
	virtual BOOL SetAsync();

	//Get the socket handle
	virtual SOCKET GetAsyncHandle()const;

	//Send data from the send buffer
	virtual BOOL SendBlockedBuffer(const CBlockedBuffer::CBlockedData& rData);

	//Socket is connected
	virtual BOOL SocketConnected(int iErrorCode,
								 BOOL bNoEvent=FALSE);

	//When we have data to receive
	virtual BOOL SocketReceive(int iErrorCode,
							   BOOL bNoEvent=FALSE);
protected:
	//Actual commands of the socket
	//Our local listen
	BOOL LocalListen(unsigned long ulBackLog);

	//Our local connect
	BOOL LocalConnect(unsigned short usSourcePort,
					  IP aDestinationAddress,
					  unsigned short usDestinationPort,
					  BOOL bDisableAsync,
					  BOOL bForceErrorEvent);

	//Event handler for socket connections
	BOOL LocalSocketConnect(int iErrorCode,
							BOOL bNoEvent);

	//Event handler for socket receive
	BOOL LocalSocketReceive(int iErrorCode,
							BOOL bNoEvent);

	//Event handler for socket accept
	BOOL LocalSocketAccept(int iErrorCode,
						   BOOL bNoEvent);

	//Event handler for socket close
	BOOL LocalSocketClose(int iErrorCode,
						  BOOL bNoEvent);

	//Our local bind
	BOOL LocalBind(IP aSourceAddress,
				   unsigned short usPort);

	//Get the bind address
	IP LocalGetBindAddress()const;

	//Get the bind port
	unsigned short LocalGetBindPort()const;

	//Accept a connection, supply an already made socket
	BOOL LocalAccept(CTCPSocket* pNewSocket);
private:
	//No copy Ctor
	CTCPSocketAsync(const CTCPSocketAsync& rSocket);

	//Socket it closed
	virtual BOOL SocketClosed(int iErrorCode,
							  BOOL bNoEvent);

	//When we have data to write
	virtual BOOL SocketWrite(int iErrorCode);

	//When the socket is accepted, what to do
	virtual void Accepted();

	//When we have a system timeout
	virtual void SystemTimeout();

	//Called before we accept the socket
	virtual void BeforeAccept();

	//Switch events type (for linked socket)
	void SwitchEventsType();

	//Is this a disabled connect
	BOOL m_bDisabledConnect;

	//The timeout
	unsigned long m_ulTimeout;

	//Our linked socket for proxy implementation
	CTCPSocketAsync* m_pLinkedSocket;

	//Close events only to linked socket
	BOOL m_bCloseEventsOnly;

	//Our overider socket
	CTCPSocketOverider* m_pOverider;

	//Our error string
	static const std::string m_sError;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_TCPSOCKETASYNC_H__1D78325D_C154_4B21_97E5_320394FD47EF__INCLUDED_)
