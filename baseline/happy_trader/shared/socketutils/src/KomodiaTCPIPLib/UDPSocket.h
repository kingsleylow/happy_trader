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

#if !defined(AFX_UDPSOCKET_H__B6FDDD18_19E2_4E61_A04E_CC6DD8583966__INCLUDED_)
#define AFX_UDPSOCKET_H__B6FDDD18_19E2_4E61_A04E_CC6DD8583966__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Socket.h"

class SOCKETUTILS_EXP CUDPSocket :	public CSocket
{
public:
	//Create the socket
	virtual BOOL Create();

	//Recieve data from remote socket
	//Return value:
	//Positive - The number of bytes received.
	//Zero - Socket has been closed.
	//Negative - Error
	virtual int Receive(char* pBuffer,
						unsigned long ulBufferLength);

	//Recieve data from remote socket, and get it's data
	//Return value:
	//Positive - The number of bytes received.
	//Zero - Socket has been closed.
	//Negative - Error
	virtual int Receive(char* pBuffer,
						unsigned long ulBufferLength,
						IP& rIP,
						unsigned short& rSourcePort);

	//Send UDP datagrams (with default source port as 0)
	//Return value:
	//Positive - The number of bytes sent.
	//Negative - Error
	virtual BOOL Send(IP aDestinationAddress, 
					  unsigned short usDestinationPort,
					  const char* pBuffer,
					  unsigned short usBufferLength);
	virtual BOOL Send(const std::string& rDestinationAddress, 
					  unsigned short usDestinationPort,
					  const char* pBuffer,
					  unsigned short usBufferLength);

	//Ctor and Dtor
	CUDPSocket(BOOL bRawSocket=false);
	virtual ~CUDPSocket();
private:
	//No copy Ctor
	CUDPSocket(const CUDPSocket& rSocket);

	//No assigment operator
	CUDPSocket& operator=(const CUDPSocket& rSocket); 
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_UDPSOCKET_H__B6FDDD18_19E2_4E61_A04E_CC6DD8583966__INCLUDED_)
