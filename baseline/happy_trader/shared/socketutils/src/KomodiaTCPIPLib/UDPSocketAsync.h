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

#if !defined(AFX_UDPSOCKETASYNC_H__0E8998D8_F193_43E6_9E69_2DECD38C049C__INCLUDED_)
#define AFX_UDPSOCKETASYNC_H__0E8998D8_F193_43E6_9E69_2DECD38C049C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UDPSocket.h"
#include "AsyncSocket.h"

KOMODIA_NAMESPACE_START

class SOCKETUTILS_EXP CUDPSocketAsync :	public CUDPSocket, 
						public CAsyncSocket  
{
public:
	//Send UDP datagrams (with default source port as 0)
	//Return value:
	//Positive - The number of bytes sent.
	//0 - Send couldn't finish because it would block
	//Negative - Error
	virtual BOOL Send(IP aDestinationAddress, 
					  unsigned short usDestinationPort,
					  const char* pBuffer,
					  unsigned short usBufferLength);
	virtual BOOL Send(const std::string& rDestinationAddress, 
					  unsigned short usDestinationPort,
					  const char* pBuffer,
					  unsigned short usBufferLength);

	//Close the socket
	virtual BOOL Close();

	//Listen to incoming connections
	virtual BOOL Listen();

	//Create the socket
	virtual BOOL Create();

	//Ctor and Dtor
	CUDPSocketAsync(BOOL bRawSocket=false);
	virtual ~CUDPSocketAsync();
protected:
	//Go to async mode
	virtual BOOL SetAsync();

	//Set the socket to async mode
	virtual BOOL OnSocketConnect(int iErrorCode);

	//Get the socket handle
	virtual SOCKET GetAsyncHandle()const;
private:
	//No copy Ctor
	CUDPSocketAsync(const CUDPSocketAsync& rSocket);

	//No assigment operator
	CUDPSocketAsync& operator=(const CUDPSocketAsync& rSocket); 

	//When the socket is accepted, what to do
	virtual void Accepted();
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_UDPSOCKETASYNC_H__0E8998D8_F193_43E6_9E69_2DECD38C049C__INCLUDED_)
