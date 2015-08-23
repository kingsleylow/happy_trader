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
#include "Win32Mutex.h"

#include <string>

#ifdef _MEMORY_DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
#endif

KOMODIA_NAMESPACE_START

CWin32Mutex::CWin32Mutex(const char* pMutexName) : CGenericMutex(pMutexName),
												   m_hMutex(NULL)
{
	//Try to create the mutex
	m_hMutex=CreateMutex(NULL,
						 FALSE,
						 pMutexName);

	//Did we create the mutex?
	if (!m_hMutex)
		throw std::string("Failed to create mutex!");

	//Check if we are the owners
	if (pMutexName && 
		GetLastError()!=ERROR_ALREADY_EXISTS)
		SetFirst();
}

CWin32Mutex::~CWin32Mutex()
{
	//Delete the mutex
	if (m_hMutex)
		CloseHandle(m_hMutex);
}

BOOL CWin32Mutex::Aquire(unsigned long ulMSTimeout)
{
	DWORD dwWaitResult;

	//Wait for the mutex
	dwWaitResult=WaitForSingleObject(m_hMutex,
									 ulMSTimeout);

	//Check why did we quit
	return dwWaitResult==WAIT_TIMEOUT;
}

BOOL CWin32Mutex::Release()
{
	//Release the mutex
	return ReleaseMutex(m_hMutex)!=0;
}

HANDLE CWin32Mutex::GetHandle()const
{
	return m_hMutex;
}

void CWin32Mutex::ReleaseObject()
{
	Release();
}

KOMODIA_NAMESPACE_END
