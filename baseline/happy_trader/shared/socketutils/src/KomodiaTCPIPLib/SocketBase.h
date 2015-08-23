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

#if !defined(AFX_SPOOFBASE_H__3760FA30_2B52_4F62_9EB6_46640C36E4F1__INCLUDED_)
#define AFX_SPOOFBASE_H__3760FA30_2B52_4F62_9EB6_46640C36E4F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//windows sockets2
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../socketutilsdefs.hpp"
#include "../KomodiaInfraLib/ErrorHandler.h"

KOMODIA_NAMESPACE_START

class CGenericCriticalSection;

typedef unsigned long IP;

//Handles basic errors
class SOCKETUTILS_EXP CSocketBase : public CErrorHandler
{
public:
	//Get the error code for compatability
	//returns SOCKET_ERROR for Win32
	static int GetErrorCode();

	//Get the system last error (needed for threading)
	static int GetSystemLastError();

	//Are we initialized
	static BOOL IsInitialized();

	//Converts a string to long
	static unsigned long StringToLong(const std::string& rAddress);

	//Convert long to string (Not thread safe)
	static char FAR * LongToString(unsigned long ulAddr);

	//Convert long to string but as a standard string (thread safe)
	static std::string LongToStdString(unsigned long ulAddr);

	//Initialize the sockets
	//bMultiThreaded - Do we want multithreaded support 
	//ulNumberOfThreads - Number of threads to allocate
	static BOOL InitializeSockets(BOOL bMultiThreaded=TRUE,
								  unsigned long ulNumberOfThreads=10);

	//Shutdown the sockets
	static BOOL ShutdownSockets();

	//Get the last error
	int GetLastError()const;
	
	//Get the number of threads
	static unsigned long GetNumberOfThreads();

	//Are we multithreaded
	static BOOL IsMultiThreaded();

	//Get the critical section
	static CGenericCriticalSection* GetGlobalCriticalSection();

	//Resolve a DNS entry (using winsock2 routines)
	static unsigned long ResolveDNS(const std::string& rAddress);
	
	//Check if an address is valid
	static BOOL ValidAddress(const std::string& rAddress);

	//Ctor and Dtor
	CSocketBase();
	CSocketBase(const CSocketBase& rBase);
	virtual ~CSocketBase();
protected:
	//Shutdown notifier
	virtual void NotifyShutdown();

	//Register shutdown class
	void RegisterShutdown(CSocketBase* pBase);

	//Set the socket last error
	void SetLastError(const std::string& rMethod)const;
	void SetLastError(const std::string&,
					  int iErrorCode)const;

	//Our critical section
	static CGenericCriticalSection* m_pCSection;
private:
	//Our DNS cache
	typedef std::map<std::string,sockaddr_in> DNSMap;
private:
	//Reseolve DNS
	static sockaddr_in InternalResolveDNS(const std::string& rAddress);

	//Last error we had
	mutable int m_iLastError;

	//Are we initialized
	static BOOL m_bInitialized;

	//Class to notify
	static CSocketBase* m_pShutdownClass;

	//Are we multithreaded
	static BOOL m_bMultiThreaded;

	//Number of threaded
	static int m_ulNumberOfThreads;

	//Our DNS map
	static DNSMap m_aDNSMap;

	//CS for the MAP
	static CGenericCriticalSection* m_pCSectionDNS;
};

KOMODIA_NAMESPACE_END

#endif // !defined(AFX_SPOOFBASE_H__3760FA30_2B52_4F62_9EB6_46640C36E4F1__INCLUDED_)
