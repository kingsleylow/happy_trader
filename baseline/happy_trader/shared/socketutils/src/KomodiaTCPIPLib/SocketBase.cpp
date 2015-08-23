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
#include "SocketBase.h"

#include "../KomodiaInfraLib/ErrorHandlerMacros.h"
#include "../KomodiaInfraLib/OSManager.h"
#include "../KomodiaInfraLib/GenericCriticalSection.h"
#include "../KomodiaInfraLib/LibConfig.h"

#include <memory>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

//Are we multithreaded?
BOOL CSocketBase::m_bMultiThreaded=FALSE;

//Are we initialized?
BOOL CSocketBase::m_bInitialized=FALSE;

//Our number of threads
int CSocketBase::m_ulNumberOfThreads=0;

//Our global CS
CGenericCriticalSection* CSocketBase::m_pCSection=NULL;

//Our global shutdown class
CSocketBase* CSocketBase::m_pShutdownClass=NULL;

//Our DNS map
CSocketBase::DNSMap CSocketBase::m_aDNSMap;

//The DNS CS
CGenericCriticalSection* CSocketBase::m_pCSectionDNS=NULL;

//Our wait timeout
#define THREAD_TIMEOUT 10000

#define CSocketBase_Class "CSocketBase"

CSocketBase::CSocketBase() : CErrorHandler()
{
	try
	{
		//Set it
		SetName(CSocketBase_Class);
	}
	ERROR_HANDLER("CSocketBase")
}

CSocketBase::CSocketBase(const CSocketBase& rBase) : CErrorHandler(rBase)
{
	try
	{
		//Set it
		SetName(CSocketBase_Class);
	}
	ERROR_HANDLER("CSocketBase")
}

CSocketBase::~CSocketBase()
{
}

void CSocketBase::SetLastError(const std::string& rMethod) const
{
	try
	{
#ifdef WIN32
		//First set the error
		m_iLastError=WSAGetLastError();
#else
		m_iLastError=errno();
#endif

		//Check if there is an error
		if (m_iLastError)
			ReportError(rMethod,
						"Through SetLastError",
						m_iLastError);
	}
	ERROR_HANDLER("SetLastError")
}

void CSocketBase::SetLastError(const std::string& rMethod,
							  int iErrorCode)const
{
	try
	{
		//First set the error
		m_iLastError=iErrorCode;

		//Check if there is an error
		if (m_iLastError)
			ReportError(rMethod,
						"Through SetLastError",
						m_iLastError);
	}
	ERROR_HANDLER("SetLastError")
}

int CSocketBase::GetLastError()const
{
	return m_iLastError;
}

BOOL CSocketBase::InitializeSockets(BOOL bMultiThreaded,
								   unsigned long ulNumberOfThreads)
{
	//To avoid double initialize
	if (m_bInitialized)
	{
		//Report it
		ReportStaticError(CSocketBase_Class,"InitializeSockets","Already initialized!");

		//Exit
		return TRUE;
	}

	//Check that the number of threads are OK?
	if (ulNumberOfThreads>CLibConfig::GetInstance().GetMaxThreads())
	{
		//Report it
		ReportStaticError(CSocketBase_Class,"InitializeSockets","Too many threads!");

		//Exit
		return FALSE;
	}

	//Do we have threads at all
	if (bMultiThreaded &&
		!ulNumberOfThreads)
	{
		//Report it
		ReportStaticError(CSocketBase_Class,"InitializeSockets","Didn't receive any threads!");

		//Exit
		return FALSE;
	}

	try
	{
		//Initialize the sockets
		WORD wVersionRequested;
		wVersionRequested=MAKEWORD(2,2);
 
		//Try to initialize
		WSADATA wsaData;
		int iErr;
		iErr=WSAStartup(wVersionRequested, 
						&wsaData);

		//Did we succeed?
		if (iErr!=0)
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			return FALSE;
 
		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */
 
		if (LOBYTE(wsaData.wVersion)!=2 || 
			HIBYTE(wsaData.wVersion)!=2)
		{
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			WSACleanup();

			//Exit
			return FALSE;
		}

		//Save the threading information
		m_bMultiThreaded=bMultiThreaded;
		m_ulNumberOfThreads=ulNumberOfThreads;

		//Create the critical section
		m_pCSection=COSManager::CreateCriticalSection();
		m_pCSectionDNS=COSManager::CreateCriticalSection();

		//And we are initialized
		m_bInitialized=TRUE;

		return TRUE;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"InitializeSockets",FALSE)
}

BOOL CSocketBase::ShutdownSockets()
{
	//Only if initialized
	if (!m_bInitialized)
		return TRUE;

	try
	{
		//Delete the CS
		delete m_pCSection;
		m_pCSection=NULL;

		//Delete the DNS CS
		delete m_pCSectionDNS;
		m_pCSectionDNS=NULL;

		//Clear the DNS map
		m_aDNSMap.clear();

		//Notify shutdown class
		if (m_pShutdownClass)
		{
			//Notify we are shuting down
			m_pShutdownClass->NotifyShutdown();

			//Delete it
			delete m_pShutdownClass;
			m_pShutdownClass=NULL;
		}

		//Not initialized anymore
		m_bInitialized=FALSE;

		if (WSACleanup()==GetErrorCode())
			return FALSE;

		//Done
		return TRUE;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"ShutdownSockets",FALSE)
}

void CSocketBase::NotifyShutdown()
{
}

void CSocketBase::RegisterShutdown(CSocketBase* pBase)
{
	try
	{
		//Check if we already have a class
		if (m_pShutdownClass)
			delete m_pShutdownClass;

		//Take it
		m_pShutdownClass=pBase;
	}
	ERROR_HANDLER("RegisterShutdown")
}

std::string CSocketBase::LongToStdString(unsigned long ulAddr)
{
	try
	{
		//First create the address
		in_addr addr;

		//Assign it
		addr.S_un.S_addr=ulAddr;

		//Enter the critical section
		CCriticalAutoRelease aRelease(m_pCSection);

		//Return the value
		return inet_ntoa(addr);
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"LongToStdString","0.0.0.0")
}

char FAR * CSocketBase::LongToString(unsigned long ulAddr)
{
	try
	{
		//First create the address
		in_addr addr;

		//Assign it
		addr.S_un.S_addr=ulAddr;

		//Enter the critical section
		CCriticalAutoRelease aRelease(m_pCSection);

		//Return the value
		return inet_ntoa(addr);
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"LongToString",NULL)
}

BOOL CSocketBase::IsMultiThreaded()
{
	return m_bMultiThreaded;
}

unsigned long CSocketBase::GetNumberOfThreads()
{
	return m_ulNumberOfThreads;
}

BOOL CSocketBase::IsInitialized()
{
	return m_bInitialized;
}

unsigned long CSocketBase::StringToLong(const std::string& rAddress)
{
	//Try to convert it
	unsigned long ulAddress;
	ulAddress=inet_addr(rAddress.c_str());

	//Is it valid
	if (ulAddress!=INADDR_NONE)
		return ulAddress;
	else
		//Try to resolve it
		return ResolveDNS(rAddress);
}

int CSocketBase::GetSystemLastError()
{
	return WSAGetLastError();
}

CGenericCriticalSection* CSocketBase::GetGlobalCriticalSection()
{
	return m_pCSection;
}

int CSocketBase::GetErrorCode()
{
	return SOCKET_ERROR;
}

BOOL CSocketBase::ValidAddress(const std::string& rAddress)
{
	try
	{
		return inet_addr(rAddress.c_str())!=INADDR_NONE;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"ValidAddress",FALSE)
}

sockaddr_in CSocketBase::InternalResolveDNS(const std::string& rAddress)
{
	//Convert it to the address
	sockaddr_in aAddr;
	memset(&aAddr,0,sizeof(aAddr));

	try
	{
		//Lock
		CCriticalAutoRelease aRelease(m_pCSectionDNS);

		//Do we have it
		DNSMap::iterator aIterator;
		aIterator=m_aDNSMap.find(rAddress);

		//Do we have it
		if (aIterator!=m_aDNSMap.end())
			//Yes we do
			return aIterator->second;
		else
		{
			//Exit the CS
			aRelease.Exit();

			//Resolve the DNS
			hostent* pData;
			pData=gethostbyname(rAddress.c_str());

			//Check if this address exists
			if (pData)
			{
				//Copy the data
				memcpy(&aAddr.sin_addr,
					   pData->h_addr,
					   pData->h_length);

				//Relock
				aRelease.Enter();

				//Add it
				m_aDNSMap.insert(DNSMap::value_type(rAddress,aAddr));
			}

			return aAddr;
		}
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"InternalResolveDNS",aAddr)
}

unsigned long CSocketBase::ResolveDNS(const std::string& rAddress)
{
	try
	{
		//Resolve the DNS
		sockaddr_in aAddr;
		aAddr=InternalResolveDNS(rAddress.c_str());

		//Check if valid
		if (aAddr.sin_addr.S_un.S_addr==0)
			//Error
			return 0;
		else
			return aAddr.sin_addr.S_un.S_addr;
	}
	ERROR_HANDLER_STATIC_RETURN(CSocketBase_Class,"ResolveDNS",0)
}

KOMODIA_NAMESPACE_END